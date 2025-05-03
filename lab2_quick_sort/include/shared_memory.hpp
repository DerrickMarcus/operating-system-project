#pragma once

#include <pthread.h>

#include <string>
#include <cstddef>

struct SharedData
{
    // Meta data describes the information of the shared memory.
    struct MetaData
    {
        size_t data_size;           // the number of elements
        bool is_sorted;             // whether the data is sorted
        pthread_mutex_t meta_mutex; // mutex for the meta data
        pthread_mutex_t data_mutex; // mutex for the data
    } meta;

    // Variable-length array. The size is determined by the total size.
    int data[];
};

class SharedMemory
{
public:
    SharedMemory(const std::string &shm_name,
                 size_t data_size,
                 bool create);

    ~SharedMemory();

    // Disable the copy constructor and assignment operator.
    SharedMemory(const SharedMemory &) = delete;
    SharedMemory &operator=(const SharedMemory &) = delete;

    // Only the owner can use to unlink the shared memory and destroy the mutex.
    void remove();

    bool get_is_owner() const;
    bool get_is_valid() const;
    size_t get_data_size() const;
    int *get_data_ptr() const;
    const SharedData::MetaData &get_meta() const;

    void lock_meta();
    void unlock_meta();

    void lock_data();
    void unlock_data();

    void set_is_sorted(bool is_sorted);
    bool get_is_sorted() const;

private:
    int _shm_fd;           // the file descriptor for the shared memory
    std::string _shm_name; // the name of the shared memory
    SharedData *_shm_ptr;  // the pointer to the shared memory
    size_t _shm_size;      // the size of the shared memory
    bool _is_owner;        // whether the current process is the owner of the shared memory
    bool _is_valid;        // whether the process links the shared memory successfully

    void init_mutex();
    static size_t get_shm_size(size_t data_size);
};