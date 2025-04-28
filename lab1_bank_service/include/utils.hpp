#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <mutex>

namespace utils
{
    // thread-safe std::cout
    inline std::mutex cout_mutex;

    template <typename... Args>
    void safe_print(Args &&...args)
    {
        std::unique_lock<std::mutex> lock(cout_mutex);

        std::ostringstream oss;
        (oss << ... << std::forward<Args>(args));

        std::cout << oss.str() << std::endl;
    }

    // parse customer information from text file
    struct CustomerInfo
    {
        int name;
        int arrive_time;
        int service_time;

        CustomerInfo(int name, int arrive_time, int service_time)
            : name(name), arrive_time(arrive_time), service_time(service_time) {}
    };

    std::vector<CustomerInfo> parse_customer_info(const std::string &file_path);

} // namespace utils
