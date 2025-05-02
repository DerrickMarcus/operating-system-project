#include <vector>
#include <memory>
#include <semaphore>
#include <atomic>
#include <barrier>
#include <chrono>
#include <stdexcept>
#include "../include/globals.hpp"
#include "../include/customer.hpp"
#include "../include/teller.hpp"
#include "../include/utils.hpp"

int main(int argc, const char *argv[])
{

    try
    {
        // Load configuration from JSON file.
        utils::load_config_from_json(globals::CONFIG_PATH);

        // Parse the information of customers from text file.
        std::vector<CustomerInfo> customers_info = utils::parse_customer_info(globals::CUSTOMER_INFO_PATH);

        // Initialize some global variables.
        globals::customer_ready = std::make_unique<std::counting_semaphore<globals::MAX_CUSTOMERS_NUMBER>>(0);
        globals::teller_ready = std::make_unique<std::counting_semaphore<globals::TELLERS_NUMBER>>(globals::TELLERS_NUMBER);
        globals::customers_number = customers_info.size();
        globals::served_customers_number.store(0);
        globals::getting_number = 1;
        globals::customers_barrier = std::make_unique<std::barrier<>>(globals::customers_number + 1);

        utils::safe_print("Total ", globals::customers_number, " customers will arrive.");
        utils::safe_print("Total ", globals::TELLERS_NUMBER, " tellers will serve.");

        // Create and start tellers thread.
        std::vector<std::unique_ptr<Teller>> tellers;
        for (int i = 1; i <= globals::TELLERS_NUMBER; ++i)
        {
            tellers.emplace_back(std::make_unique<Teller>(i));
        }
        for (const auto &teller : tellers)
        {
            teller->start();
        }
        utils::safe_print("All tellers are ready.");

        // Create and start customers thread.
        std::vector<std::shared_ptr<Customer>> customers;
        for (const auto &info : customers_info)
        {
            customers.emplace_back(std::make_shared<Customer>(info.name, info.arrive_time, info.service_time));
        }
        for (const auto &customer : customers)
        {
            customer->start();
        }
        utils::safe_print("All customers are ready.");

        // All customers start at the same time.
        globals::open_time_point = std::chrono::steady_clock::now();
        globals::customers_barrier->arrive_and_wait();

        // Wait for all customers to finish.
        for (const auto &customer : customers)
        {
            customer->join();
        }

        // Wait for all tellers to finish.
        for (const auto &teller : tellers)
        {
            teller->join();
        }

        utils::safe_print("Total ", globals::served_customers_number.load(),
                          " / ", globals::customers_number, " customers have been served.");

        // Output the information of thread to text file.
        utils::output_customer_thread_info(globals::CUSTOMER_THREAD_INFO_PATH, customers);
        utils::output_teller_thread_info(globals::TELLER_THREAD_INFO_PATH, tellers);

        return EXIT_SUCCESS;
    }
    catch (const std::exception &e)
    {
        utils::safe_print("[ERROR] Main thread caught exception: ", e.what());
        return EXIT_FAILURE;
    }
}
