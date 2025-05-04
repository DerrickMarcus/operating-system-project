#pragma once

#include <stddef.h>
#include <sys/types.h>

#include <string>
#include <vector>
#include <atomic>

#include "../include/shared_memory.hpp"

class Sorter
{
public:
    Sorter(const std::string &shm_name, size_t data_size, bool create,
           int max_procs_num, int threshold);

    ~Sorter();

    void sort_all();

    bool verify_sorted() const;

private:
    SharedMemory _shm;
    std::atomic<int> _active_procs_num{1}; // The number of active processes
    const int _max_procs_num;
    const int _threshold;
    std::vector<pid_t> _child_pids; // The child process IDs

    // All the range includes the left and right index: [left, right]
    // The total range is [0, data_size - 1]
    void std_sort(int left, int right);
    int partition(int left, int right);
    pid_t fork_and_sort(int left, int right);
    void quick_sort(int left, int right);
    void wait_for_children();
};