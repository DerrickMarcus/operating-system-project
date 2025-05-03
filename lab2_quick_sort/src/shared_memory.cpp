#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#include <iostream>
#include <string>
#include <cstring>
#include <cstddef>
#include <cerrno>
#include <stdexcept>

#include "../include/shared_memory.hpp"

void SharedMemory::init_mutex()
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);

    // Set the mutex to be shared between processes.
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);

    // Set the mutex type to error-checking.
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

    pthread_mutex_init(&this->_shm_ptr->meta.meta_mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}

size_t SharedMemory::get_shm_size(size_t data_size)
{
    return sizeof(SharedData) + data_size * sizeof(int);
}

SharedMemory::SharedMemory(const std::string &shm_name,
                           size_t data_size,
                           bool create)
    : _shm_fd(-1),
      _shm_name(shm_name),
      _shm_ptr(nullptr),
      _shm_size(this->get_shm_size(data_size)),
      _is_owner(create),
      _is_valid(false)
{
    // Create or open the shared memory.
    int shm_flag_ = create ? (O_CREAT | O_EXCL | O_RDWR) : O_RDWR;
    this->_shm_fd = shm_open(shm_name.c_str(), shm_flag_, 0666);
    if (this->_shm_fd < 0)
    {
        throw std::runtime_error("Failed to shm_open: " + std::string(strerror(errno)));
    }

    // Set the size of the shared memory.
    if (create && ftruncate(this->_shm_fd, this->_shm_size) < 0)
    {
        shm_unlink(shm_name.c_str());
        ::close(this->_shm_fd);
        throw std::runtime_error("Failed to ftruncate: " + std::string(strerror(errno)));
    }

    // Map the shared memory to the process's address space.
    void *addr_ = mmap(nullptr, this->_shm_size, PROT_READ | PROT_WRITE,
                       MAP_SHARED, this->_shm_fd, 0);
    if (addr_ == MAP_FAILED)
    {
        ::close(this->_shm_fd);
        throw std::runtime_error("Failed to mmap: " + std::string(strerror(errno)));
    }
    this->_shm_ptr = static_cast<SharedData *>(addr_);

    // If creating the shared memory, initialize the meta data.
    if (create)
    {
        this->init_mutex();
        this->lock_meta();
        this->_shm_ptr->meta.data_size = data_size;
        this->_shm_ptr->meta.is_sorted = false;
        this->unlock_meta();
    }
    else if (this->_shm_ptr->meta.data_size != data_size)
    {
        munmap(this->_shm_ptr, _shm_size);
        ::close(this->_shm_fd);
        throw std::runtime_error("Shared memory size mismatch.");
    }

    this->_is_valid = true;
}

SharedMemory::~SharedMemory()
{
    if (!this->_is_valid)
    {
        return;
    }

    // if (this->_is_owner)
    // {
    //     if (pthread_mutex_destroy(&this->_shm_ptr->meta.meta_mutex) != 0)
    //     {
    //         std::cerr << "Failed to destroy meta_mutex: " << std::strerror(errno) << std::endl;
    //     }
    //     if (pthread_mutex_destroy(&this->_shm_ptr->meta.data_mutex) != 0)
    //     {
    //         std::cerr << "Failed to destroy data_mutex: " << std::strerror(errno) << std::endl;
    //     }
    // }

    if (_is_owner)
    {
        while (true)
        {
            int r = pthread_mutex_trylock(&this->_shm_ptr->meta.meta_mutex);
            if (r == 0)
            {
                pthread_mutex_unlock(&this->_shm_ptr->meta.meta_mutex);
                if (pthread_mutex_destroy(&this->_shm_ptr->meta.meta_mutex) != 0)
                {
                    std::cerr << "Failed to destroy meta_mutex: " << std::strerror(errno) << std::endl;
                }
                break;
            }
            if (r != EBUSY)
            {
                std::cerr << "meta_mutex trylock error: " << std::strerror(r) << std::endl;
                break;
            }
            usleep(10000); // 10ms
        }
    }

    if (munmap(this->_shm_ptr, this->_shm_size) != 0)
    {
        std::cerr << "Failed to munmap:" << std::strerror(errno) << std::endl;
    }
    this->_shm_ptr = nullptr;

    if (::close(this->_shm_fd) != 0)
    {
        std::cerr << "Failed to close file descriptor: " << std::strerror(errno) << std::endl;
    }
    this->_shm_fd = -1;
}

void SharedMemory::remove()
{
    if (!this->_is_owner)
    {
        return;
    }

    if (shm_unlink(this->_shm_name.c_str()) != 0)
    {
        std::cerr << "Failed to shm_unlink: " << this->_shm_name << std::strerror(errno) << std::endl;
    }
}

bool SharedMemory::get_is_owner() const
{
    return this->_is_owner;
}

bool SharedMemory::get_is_valid() const
{
    return this->_is_valid;
}

size_t SharedMemory::get_data_size() const
{

    return this->_shm_ptr->meta.data_size;
}

int *SharedMemory::get_data_ptr() const
{
    return this->_shm_ptr->data;
}

const SharedData::MetaData &SharedMemory::get_meta() const
{
    return this->_shm_ptr->meta;
}

void SharedMemory::lock_meta()
{
    pthread_mutex_lock(&this->_shm_ptr->meta.meta_mutex);
}

void SharedMemory::unlock_meta()
{
    pthread_mutex_unlock(&this->_shm_ptr->meta.meta_mutex);
}

void SharedMemory::set_is_sorted(bool is_sorted)
{
    lock_meta();
    this->_shm_ptr->meta.is_sorted = is_sorted;
    unlock_meta();
}

bool SharedMemory::get_is_sorted() const
{
    auto self_ = const_cast<SharedMemory *>(this);
    self_->lock_meta();
    bool sorted_ = this->_shm_ptr->meta.is_sorted;
    self_->unlock_meta();
    return sorted_;
}