#pragma once

#include <vector>

struct Request
{
    int pid;
    std::vector<int> request;

    Request(int pid, const std::vector<int> &req);
};

class Banker
{
public:
    Banker(const std::vector<int> &total,
           const std::vector<std::vector<int>> &allocation,
           const std::vector<std::vector<int>> &max_demand);

    void print_state() const;

    bool is_request_valid(int pid, const std::vector<int> &request) const;

    std::vector<int> is_system_safe() const;

    bool process_request(int pid, const std::vector<int> &request);

    bool release_resources(int pid, const std::vector<int> &release);

    const std::vector<int> &get_total() const;
    const std::vector<int> &get_available() const;
    const std::vector<std::vector<int>> &get_allocation() const;
    const std::vector<std::vector<int>> &get_max_demand() const;
    const std::vector<std::vector<int>> &get_need() const;

private:
    std::vector<int> _total;
    std::vector<int> _available;

    std::vector<std::vector<int>> _allocation;
    std::vector<std::vector<int>> _max_demand;
    std::vector<std::vector<int>> _need;

    int _num_processes;
    int _num_resources;

    void calculate_need();
    void calculate_available();
};