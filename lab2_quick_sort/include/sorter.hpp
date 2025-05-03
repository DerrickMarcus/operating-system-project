#pragma once

#include <atomic>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <algorithm>
#include <stdexcept>

#include "../include/shared_memory.hpp"

class Sorter
{
public:
private:
    SharedMemory _shm;
    std::atomic<int> _active_procs_num{1}; // The number of active processes
};