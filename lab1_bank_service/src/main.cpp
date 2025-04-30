#include <vector>
#include <memory>
#include <semaphore>
#include <atomic>
#include <barrier>
#include <chrono>
#include <stdexcept>
#include "globals.hpp"
#include "customer.hpp"
#include "teller.hpp"
#include "utils.hpp"

int main(int argc, const char *argv[])
{

    try
    {
        // parse information of customers from text file
        std::vector<CustomerInfo> customer_info = utils::parse_customer_info(globals::CUSTOMER_INFO_FILE_PATH);
        utils::safe_print("Parsed customers from file: ", globals::CUSTOMER_INFO_FILE_PATH);

        // initialize global variables
        globals::customer_ready = std::make_unique<std::counting_semaphore<globals::MAX_CUSTOMERS_NUMBER>>(0);
        globals::teller_ready = std::make_unique<std::counting_semaphore<globals::TELLERS_NUMBER>>(globals::TELLERS_NUMBER);
        globals::customers_number = customer_info.size();
        globals::served_customers_number.store(0);
        globals::getting_number = 1;
        globals::customers_barrier = std::make_unique<std::barrier<>>(globals::customers_number + 1);

        utils::safe_print("Total ", globals::customers_number, " customers will arrive.");
        utils::safe_print("Total ", globals::TELLERS_NUMBER, " tellers will serve.");

        // create and start tellers thread
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

        // create and start customers thread
        std::vector<std::shared_ptr<Customer>> customers;
        for (const auto &info : customer_info)
        {
            customers.emplace_back(std::make_shared<Customer>(info.name, info.arrive_time, info.service_time));
        }
        for (const auto &customer : customers)
        {
            customer->start();
        }
        utils::safe_print("All customers are ready.");

        std::this_thread::sleep_for(std::chrono::seconds(5));

        // wait for all customers to arrive
        globals::open_time_point = std::chrono::steady_clock::now();
        globals::customers_barrier->arrive_and_wait();
        // globals::open_time_point = std::chrono::steady_clock::now();

        // wait for all customers to finish
        for (const auto &customer : customers)
        {
            customer->join();
            utils::safe_print("Customer ", customer->get_name(), " left.");
        }

        // wait for all tellers to finish
        for (const auto &teller : tellers)
        {
            teller->join();
            utils::safe_print("Teller ", teller->get_name(), " off work.");
        }

        // output thread information to text file
        utils::safe_print("Total ", globals::served_customers_number.load(), " customers have been served.");

        utils::output_customer_thread_info(customers, globals::CUSTOMER_THREAD_INFO_FILE_PATH);
        utils::safe_print("Output customer thread log to file: ", globals::CUSTOMER_THREAD_INFO_FILE_PATH);

        utils::output_teller_thread_info(tellers, globals::TELLER_THREAD_INFO_FILE_PATH);
        utils::safe_print("Output teller thread log to file: ", globals::TELLER_THREAD_INFO_FILE_PATH);

        return EXIT_SUCCESS;
    }
    catch (const std::exception &e)
    {
        utils::safe_print("[ERROR] Main thread caught exception: ", e.what());
        return EXIT_FAILURE;
    }
}
