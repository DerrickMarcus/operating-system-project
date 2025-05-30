#pragma once

#include <string>
#include <queue>
#include <memory>
#include <mutex>
#include <semaphore>
#include <atomic>
#include <barrier>
#include <chrono>

class Customer;

namespace globals
{

    // The number of customers is determined by the data file, but the template
    // parameter of std::counting_semaphore must be determined at compile time,
    // so we have to set a large number parameter for customer_ready.
    inline constexpr int MAX_CUSTOMERS_NUMBER = 1000; // can be changed manually
    inline constexpr int TELLERS_NUMBER = 5;          // can be changed manually

    inline std::unique_ptr<std::counting_semaphore<MAX_CUSTOMERS_NUMBER>> customer_ready;
    inline std::unique_ptr<std::counting_semaphore<TELLERS_NUMBER>> teller_ready;

    inline int customers_number = 0; // determined after the main thread reads data file
    inline std::atomic<int> served_customers_number{0};

    inline int getting_number = 1;
    inline std::mutex getting_number_mutex;

    inline std::queue<std::shared_ptr<Customer>> waiting_queue;
    inline std::mutex waiting_queue_mutex;

    inline std::unique_ptr<std::barrier<>> customers_barrier;

    inline std::chrono::steady_clock::time_point open_time_point;

    // can be changed manually
    inline const std::string CONFIG_PATH = "../config/config.json";
    inline std::string CUSTOMER_INFO_PATH = "../data/customer_info.txt";
    inline std::string CUSTOMER_THREAD_INFO_PATH = "../data/customer_log.txt";
    inline std::string TELLER_THREAD_INFO_PATH = "../data/teller_log.txt";

} // namespace globals