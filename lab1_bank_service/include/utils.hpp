#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <mutex>

struct CustomerInfo;
class Customer;
class Teller;

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
    const std::vector<CustomerInfo> parse_customer_info(const std::string &file_path);

    // output customer thread information to text file
    void output_customer_thread_info(const std::vector<std::shared_ptr<Customer>> &customers,
                                     const std::string &file_path);

    // output teller thread information to text file
    void output_teller_thread_info(const std::vector<std::unique_ptr<Teller>> &tellers,
                                   const std::string &file_path);

} // namespace utils
