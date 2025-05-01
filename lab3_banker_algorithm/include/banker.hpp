#pragma once

#include <vector>
#include <string>

class Banker
{
public:
    Banker(const std::vector<int> &total,
           const std::vector<std::vector<int>> &allocation,
           const std::vector<std::vector<int>> &max_demand);

    void print_state() const;

    bool is_request_valid(int pid, const std::vector<int> &request) const;

    bool is_system_safe() const;

    bool process_request(int pid, const std::vector<int> &request);

    bool release_resources(int pid, const std::vector<int> &release);

    const std::vector<int> &get_total() const;
    const std::vector<int> &get_available() const;
    const std::vector<std::vector<int>> &get_allocation() const;
    const std::vector<std::vector<int>> &get_max_demand() const;
    const std::vector<std::vector<int>> &get_need() const;

private:
    std::vector<int> total;
    std::vector<int> available;

    std::vector<std::vector<int>> allocation;
    std::vector<std::vector<int>> max_demand;
    std::vector<std::vector<int>> need;

    int num_processes;
    int num_resources;

    void calculate_need();
    void calculate_available();
};