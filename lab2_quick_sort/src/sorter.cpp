#include <stddef.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <string>
#include <algorithm>
#include <atomic>
#include <stdexcept>

#include "../include/sorter.hpp"
#include "../include/shared_memory.hpp"

Sorter::Sorter(const std::string &shm_name, size_t data_size, bool create,
               int max_procs_num, int threshold)
    : _shm(shm_name, data_size, create),
      _max_procs_num(max_procs_num),
      _threshold(threshold),
      _child_pids(std::vector<int>()) {}

Sorter::~Sorter()
{
    // if (this->_shm.get_is_owner())
    // {
    //     this->_shm.remove();
    // }
}

void Sorter::sort_all()
{
    this->quick_sort(0, static_cast<int>(this->_shm.get_data_size()) - 1);
    this->wait_for_children();
}

bool Sorter::verify_sorted() const
{
    auto *data_ptr = this->_shm.get_data_ptr();
    for (size_t i = 0; i < this->_shm.get_data_size() - 1; i++)
    {
        if (data_ptr[i] > data_ptr[i + 1])
        {
            return false;
        }
    }
    return true;
}

void Sorter::std_sort(int left, int right)
{
    int *data_ptr = this->_shm.get_data_ptr();
    std::sort(data_ptr + left, data_ptr + right + 1);
}

int Sorter::partition(int left, int right)
{
    int *data_ptr = this->_shm.get_data_ptr();
    int pivot_val = data_ptr[left];
    int i = left;
    for (int j = left + 1; j <= right; j++)
    {
        if (data_ptr[j] < pivot_val)
        {
            i++;
            std::swap(data_ptr[i], data_ptr[j]);
        }
    }
    std::swap(data_ptr[left], data_ptr[i]);
    return i;
}

pid_t Sorter::fork_and_sort(int left, int right)
{
    pid_t pid = ::fork();
    if (pid < 0)
    {
        throw std::runtime_error("Fork failed.");
    }
    else if (pid == 0)
    {
        // child process
        this->quick_sort(left, right);
        this->wait_for_children();
        ::_exit(0);
    }
    else
    {
        // parent process
        this->_child_pids.push_back(pid);
        this->_active_procs_num.fetch_add(1, std::memory_order_relaxed);
    }
    return pid;
}

void Sorter::quick_sort(int left, int right)
{
    if (left >= right)
        return;

    if ((right - left + 1) <= this->_threshold)
    {
        this->std_sort(left, right);
        return;
    }

    int pivot = this->partition(left, right);
    if (this->_active_procs_num.load(std::memory_order_relaxed) <= this->_max_procs_num)
    {
        // The child process sorts the left part.
        pid_t pid = this->fork_and_sort(left, pivot - 1);
        if (pid > 0)
        {
            // The parent process sorts the right part.
            this->quick_sort(pivot + 1, right);
        }
    }
    else
    {
        this->quick_sort(left, pivot - 1);
        this->quick_sort(pivot + 1, right);
    }
}

void Sorter::wait_for_children()
{
    for (pid_t c : this->_child_pids)
    {
        int status;
        ::waitpid(c, &status, 0);
        this->_active_procs_num.fetch_sub(1, std::memory_order_relaxed);
    }
    this->_child_pids.clear();
}