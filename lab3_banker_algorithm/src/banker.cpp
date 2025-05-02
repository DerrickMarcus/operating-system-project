#include <iostream>
#include <string>
#include <vector>
#include "../include/banker.hpp"
#include "../include/utils.hpp"

Request::Request(int pid, const std::vector<int> &req)
    : pid(pid), request(req) {}

Banker::Banker(
    const std::vector<int> &total,
    const std::vector<std::vector<int>> &allocation,
    const std::vector<std::vector<int>> &max_demand)
    : _total(total),
      _allocation(allocation),
      _max_demand(max_demand),
      _num_processes(allocation.size()),
      _num_resources(total.size())
{
    this->calculate_need();
    this->calculate_available();
}

void Banker::calculate_need()
{
    this->_need.resize(this->_num_processes, std::vector<int>(this->_num_resources));
    for (size_t i = 0; i < this->_num_processes; i++)
    {
        for (size_t j = 0; j < this->_num_resources; j++)
        {
            this->_need[i][j] = this->_max_demand[i][j] - this->_allocation[i][j];
        }
    }
}

void Banker::calculate_available()
{
    this->_available = this->_total;
    for (const auto &alloc : this->_allocation)
    {
        for (size_t i = 0; i < this->_num_resources; i++)
        {
            this->_available[i] -= alloc[i];
        }
    }
}

void Banker::print_state() const
{
    std::cout << std::string(60, '-') << std::endl;
    std::cout << "Currnet state of the system resources:" << std::endl;
    std::cout << "Total: " << utils::format_vector(this->_total) << std::endl;
    std::cout << "Allocated:\n"
              << utils::format_matrix(this->_allocation) << std::endl;
    std::cout << "Max demand:\n"
              << utils::format_matrix(this->_max_demand) << std::endl;
    std::cout << "Need:\n"
              << utils::format_matrix(this->_need) << std::endl;
    std::cout << "Available: " << utils::format_vector(this->_available) << std::endl;
    std::cout << std::string(60, '-') << std::endl;
}

bool Banker::is_request_valid(int pid, const std::vector<int> &request) const
{
    if (pid < 0 || pid >= this->_num_processes)
    {
        std::cerr << "Invalid process ID: " << pid << std::endl;
        return false;
    }

    for (size_t i = 0; i < this->_num_resources; i++)
    {
        if (request[i] > this->_need[pid][i])
        {
            std::cerr << "Process " << pid << " is requesting more than its maximum demand:" << std::endl;
            std::cerr << "Request: " << utils::format_vector(request) << std::endl;
            std::cerr << "Need:    " << utils::format_vector(this->_need[pid]) << std::endl;
            return false;
        }
        if (request[i] > this->_available[i])
        {
            std::cerr << "Process " << pid << " is requesting more than available resources:" << std::endl;
            std::cerr << "Request:   " << utils::format_vector(request) << std::endl;
            std::cerr << "Available: " << utils::format_vector(this->_available) << std::endl;
            return false;
        }
    }

    return true;
}

std::vector<int> Banker::is_system_safe() const
{
    std::vector<int> work = _available;
    std::vector<bool> finish(_num_processes, false);
    std::vector<int> safe_sequence;

    while (safe_sequence.size() < _num_processes)
    {
        bool found = false;

        for (size_t i = 0; i < _num_processes; i++)
        {
            if (!finish[i])
            {
                bool can_finish = true;
                for (size_t j = 0; j < _num_resources; j++)
                {
                    if (_need[i][j] > work[j])
                    {
                        can_finish = false;
                        break;
                    }
                }

                if (can_finish)
                {
                    // Simulate releasing resource.
                    for (size_t j = 0; j < _num_resources; j++)
                    {
                        work[j] += _allocation[i][j];
                    }

                    finish[i] = true;
                    safe_sequence.push_back(i);
                    found = true;
                }
            }
        }

        if (!found)
        {
            return std::vector<int>();
        }
    }

    return safe_sequence;
}

bool Banker::process_request(int pid, const std::vector<int> &request)
{
    std::cout << "Process " << pid << " is requesting resources: " << utils::format_vector(request) << std::endl;

    if (!this->is_request_valid(pid, request))
    {
        std::cerr << "Invalid request. Allocation rejected." << std::endl;
        return false;
    }

    // Try to allocate resources.
    for (size_t i = 0; i < this->_num_resources; i++)
    {
        this->_allocation[pid][i] += request[i];
        this->_need[pid][i] -= request[i];
        this->_available[i] -= request[i];
    }

    auto safe_seq = this->is_system_safe();
    if (safe_seq.empty())
    {
        std::cerr << "Request would lead to an unsafe state." << std::endl;
        std::cerr << "Request denied. Back to the previous state." << std::endl;
        for (size_t i = 0; i < this->_num_resources; i++)
        {
            this->_allocation[pid][i] -= request[i];
            this->_need[pid][i] += request[i];
            this->_available[i] += request[i];
        }
        return false;
    }
    else
    {
        std::cout << "Request is safe. Safe sequence:" << utils::format_vector(safe_seq) << std::endl;
        std::cout << "Request accepted. Allocation successful." << std::endl;
        return true;
    }
}

bool Banker::release_resources(int pid, const std::vector<int> &release)
{
    if (pid < 0 || pid >= this->_num_processes)
    {
        std::cerr << "Invalid process ID: " << pid << std::endl;
        return false;
    }

    for (size_t i = 0; i < this->_num_resources; i++)
    {
        if (release[i] > this->_allocation[pid][i])
        {
            std::cerr << "Process " << pid << " is trying to release more than allocated resources:" << std::endl;
            std::cerr << "Release:   " << utils::format_vector(release) << std::endl;
            std::cerr << "Allocated: " << utils::format_vector(this->_allocation[pid]) << std::endl;
            return false;
        }
    }

    // release resources
    for (size_t i = 0; i < this->_num_resources; i++)
    {
        this->_allocation[pid][i] -= release[i];
        this->_available[i] += release[i];
        this->_need[pid][i] += release[i];
    }

    std::cout << "Process " << pid << " has released resources: " << utils::format_vector(release) << std::endl;
    return true;
}

const std::vector<int> &Banker::get_total() const
{
    return this->_total;
}

const std::vector<int> &Banker::get_available() const
{
    return this->_available;
}

const std::vector<std::vector<int>> &Banker::get_allocation() const
{
    return this->_allocation;
}

const std::vector<std::vector<int>> &Banker::get_max_demand() const
{
    return this->_max_demand;
}

const std::vector<std::vector<int>> &Banker::get_need() const
{
    return this->_need;
}