#pragma once

#include <string>
#include <vector>

struct Request;

namespace utils
{
    bool load_data_from_json(const std::string &path,
                             std::vector<int> &total,
                             std::vector<std::vector<int>> &allocation,
                             std::vector<std::vector<int>> &max_demand,
                             std::vector<Request> &requests);

    std::string format_vector(const std::vector<int> &vec);

    std::string format_matrix(const std::vector<std::vector<int>> &mat);
};