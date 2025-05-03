#include <iostream>
#include <string>
#include <vector>

#include "../include/banker.hpp"
#include "../include/utils.hpp"

int main(int argc, char *argv[])
{
    std::string json_path = (argc > 1) ? argv[1] : "../data/example_1.json";

    std::vector<int> total;
    std::vector<std::vector<int>> allocation;
    std::vector<std::vector<int>> max_demand;
    std::vector<Request> requests;

    if (!utils::load_data_from_json(json_path, total, allocation, max_demand, requests))
    {
        return 1;
    }

    Banker banker(total, allocation, max_demand);
    banker.print_state();

    for (const auto &req : requests)
    {
        banker.process_request(req.pid, req.request);
        banker.print_state();
    }

    return 0;
}