#include <fstream>
#include <memory>
#include <thread>
#include <semaphore>
#include <barrier>
#include "globals.hpp"
#include "customer.hpp"
#include "teller.hpp"
#include "utils.hpp"

int main(int argc, const char *argv[])
{

    // std::vector<std::shared_ptr<Customer>> customers;
    // customers.emplace_back(std::make_shared<Customer>(...));

    // customers_barrier.arrive_and_wait();
    // global_open_time_point = std::chrono::steady_clock::now();
    // std::barrier customers_barrier(total_customers + 1);

    // parse information of customers from text file
    auto customer_info = utils::parse_customer_info(globals::CUSTOMER_INFO_FILE_PATH);

    // initialize global variables
    globals::customer_ready = std::make_unique<std::counting_semaphore<globals::MAX_CUSTOMERS_NUMBER>>(0);
    globals::teller_ready = std::make_unique<std::counting_semaphore<globals::TELLERS_NUMBER>>(globals::TELLERS_NUMBER);
    globals::customers_number = customer_info.size();
    globals::served_customers_number.store(0);
    globals::getting_number = 1;
    globals::customers_barrier = std::make_unique<std::barrier<>>(globals::customers_number + 1);

    // create tellers thread
    std::vector<std::unique_ptr<Teller>> tellers;
    for (int i = 1; i <= globals::TELLERS_NUMBER; ++i)
    {
        tellers.emplace_back(std::make_unique<Teller>(i));
    }
    for (const auto &teller : tellers)
    {
        teller->start();
    }

    // create customers thread
    std::vector<std::shared_ptr<Customer>> customers;
    for (const auto &info : customer_info)
    {
        customers.emplace_back(std::make_shared<Customer>(info.name, info.arrive_time, info.service_time));
    }
    for (const auto &customer : customers)
    {
        customer->start();
    }

    // wait for all customers to arrive
    globals::customers_barrier->arrive_and_wait();
    globals::open_time_point = std::chrono::steady_clock::now();

    // wait for all customers to finish
    for (const auto &customer : customers)
    {
        customer->join();
    }

    // wait for all tellers to finish
    for (const auto &teller : tellers)
    {
        teller->join();
    }

    // output thread information to text file
    utils::safe_print("Total ", globals::served_customers_number.load(), " customers have been served.");
    utils::output_customer_thread_info(customers, globals::CUSTOMER_THREAD_INFO_FILE_PATH);
    utils::output_teller_thread_info(tellers, globals::TELLER_THREAD_INFO_FILE_PATH);

    return 0;
}
