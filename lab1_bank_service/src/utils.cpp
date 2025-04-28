#include "utils.hpp"
#include <fstream>
#include <stdexcept>

namespace utils
{

    std::vector<CustomerInfo> parse_customer_info(const std::string &file_path)
    {
        std::vector<CustomerInfo> customers;
        std::ifstream file(file_path);

        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open: " + file_path + ", please check the file path.\n");
        }

        int name, arrive_time, service_time;
        while (file >> name >> arrive_time >> service_time)
        {
            customers.emplace_back(name, arrive_time, service_time);
        }
        file.close();
        return customers;
    }

} // namespace utils
