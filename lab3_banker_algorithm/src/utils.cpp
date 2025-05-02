#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <vector>
#include <stdexcept>
#include "../include/utils.hpp"
#include "../include/banker.hpp"
#include "../include/json.hpp"

namespace utils
{

    bool load_data_from_json(const std::string &path,
                             std::vector<int> &total,
                             std::vector<std::vector<int>> &allocation,
                             std::vector<std::vector<int>> &max_demand,
                             std::vector<Request> &requests)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            std::cerr << "Failed to open JSON file: " << path << std::endl;
            return false;
        }

        nlohmann::json j;
        try
        {
            file >> j;

            total = j.at("total").get<std::vector<int>>();
            allocation = j.at("allocation").get<std::vector<std::vector<int>>>();
            max_demand = j.at("max_demand").get<std::vector<std::vector<int>>>();

            requests.clear();
            if (j.contains("requests"))
            {
                for (const auto &item : j.at("requests"))
                {
                    auto pid = item.at("pid").get<int>();
                    auto request = item.at("request").get<std::vector<int>>();
                    requests.emplace_back(pid, request);
                }
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            return false;
        }

        file.close();
        std::cout << "Loaded data from " << path << " successfully.\n";
        return true;
    }

    std::string format_vector(const std::vector<int> &vec)
    {
        if (vec.empty())
            return "[]";

        size_t max_width = 0;
        for (auto val : vec)
        {
            max_width = std::max(max_width, static_cast<size_t>(std::to_string(val).length()));
        }

        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < vec.size(); i++)
        {
            oss << std::setw(max_width) << vec[i];
            if (i < vec.size() - 1)
            {
                oss << " ";
            }
        }
        oss << "]";

        return oss.str();
    }

    std::string format_matrix(const std::vector<std::vector<int>> &mat)
    {
        if (mat.empty())
            return "[]";

        size_t max_width = 0;
        for (const auto &row : mat)
        {
            for (auto num : row)
            {
                max_width = std::max(max_width, static_cast<size_t>(std::to_string(num).length()));
            }
        }

        std::ostringstream oss;
        oss << "[";

        for (size_t i = 0; i < mat.size(); i++)
        {
            if (i > 0)
            {
                oss << " ";
            }
            oss << "[";
            for (size_t j = 0; j < mat[i].size(); j++)
            {
                oss << std::setw(max_width) << mat[i][j];
                if (j < mat[i].size() - 1)
                {
                    oss << " ";
                }
            }
            oss << "]";
            if (i < mat.size() - 1)
            {
                oss << std::endl;
            }
        }

        oss << "]";
        return oss.str();
    }

} // namespace utils