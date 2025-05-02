#pragma once

#include <string>
#include <stdexcept>

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

struct SharedData
{
    // Meta data describes the information of the shared memory.
    struct MetaData
    {
        size_t elements_num; // the number of elements
        int processes_num;   // the number of active processes
        bool is_sorted;      // whether the data is sorted
    } meta;

    // Variable-length array. The size is determined by the total size.
    int data[];
};

class SharedMemory
{
public:
    SharedMemory(const std::string &shm_name,
                 const std::string &sem_meta_name,
                 const std::string &sem_data_name,
                 size_t data_size,
                 bool create);

    ~SharedMemory();

    // Disable the copy constructor and assignment operator.
    SharedMemory(const SharedMemory &) = delete;
    SharedMemory &operator=(const SharedMemory &) = delete;

    bool get_is_owner() const;

    bool get_is_valid() const;

    size_t get_elements_num() const;

    int *get_data_ptr() const;

    const SharedData::MetaData &get_meta() const;

    void lock_meta();
    void unlock_meta();

    void lock_data();
    void unlock_data();

    void add_processes_num(int delta);

    void set_is_sorted(bool is_sorted);
    bool get_is_sorted() const;

    // Only the owner can use to unlink the shared memory and semaphore.
    void remove();

private:
    int _shm_fd;          // the file descriptor for the shared memory
    SharedData *_shm_ptr; // the pointer to the shared memory
    sem_t *_sem_meta;     // the semaphore for the meta data
    sem_t *_sem_data;     // the semaphore for the data
    size_t _shm_size;     // the size of the shared memory
    bool _is_owner;       // whether the current process is the owner of the shared memory
    bool _is_valid;       // whether the process links the shared memory  successfully
};