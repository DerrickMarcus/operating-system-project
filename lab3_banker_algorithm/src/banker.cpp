#include <iostream>
#include <vector>
#include <string>
#include "banker.hpp"

Banker::Banker(
    const std::vector<int> &total,
    const std::vector<std::vector<int>> &allocation,
    const std::vector<std::vector<int>> &max_demand)
    : total(total),
      allocation(allocation),
      max_demand(max_demand),
      num_processes(allocation.size()),
      num_resources(total.size())
{
    this->calculate_need();
    this->calculate_available();
}

void Banker::calculate_need()
{
    this->need.resize(this->num_processes, std::vector<int>(this->num_resources));
    for (int i = 0; i < this->num_processes; i++)
    {
        for (int j = 0; j < this->num_resources; j++)
        {
            this->need[i][j] = this->max_demand[i][j] - this->allocation[i][j];
        }
    }
}

void Banker::calculate_available()
{
    this->available = this->total;
    for (const auto &alloc : this->allocation)
    {
        for (int i = 0; i < this->num_resources; i++)
        {
            this->available[i] -= alloc[i];
        }
    }
}

void Banker::print_state() const
{
}

bool Banker::is_request_valid(int pid, const std::vector<int> &request) const
{
    if (pid < 0 || pid >= this->num_processes)
    {
        std::cerr << "Invalid process ID: " << pid << std::endl;
        return false;
    }

    for (int i = 0; i < this->num_resources; i++)
    {
        if (request[i] > this->need[pid][i])
        {
            std::cerr << "Process " << pid << " is requesting more than its maximum demand." << std::endl;
            return false;
        }
        if (request[i] > this->available[i])
        {
            std::cerr << "Process " << pid << " is requesting more than available resources" << std::endl;
            return false;
        }
    }

    return true;
}

bool Banker::is_system_safe() const
{
}

bool Banker::process_request(int pid, const std::vector<int> &request)
{
}

bool Banker::release_resources(int pid, const std::vector<int> &release)
{
}

const std::vector<int> &Banker::get_total() const
{
    return this->total;
}

const std::vector<int> &Banker::get_available() const
{
    return this->available;
}

const std::vector<std::vector<int>> &Banker::get_allocation() const
{
    return this->allocation;
}

const std::vector<std::vector<int>> &Banker::get_max_demand() const
{
    return this->max_demand;
}

const std::vector<std::vector<int>> &Banker::get_need() const
{
    return this->need;
}