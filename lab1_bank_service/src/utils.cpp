#include <fstream>
#include <string>
#include <iomanip>
#include <vector>
#include <memory>
#include <chrono>
#include <stdexcept>
#include "utils.hpp"
#include "customer.hpp"
#include "teller.hpp"

namespace utils
{

    const std::vector<CustomerInfo> parse_customer_info(const std::string &file_path)
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

    void output_customer_thread_info(const std::vector<std::shared_ptr<Customer>> &customers,
                                     const std::string &file_path)
    {
        std::ofstream file(file_path);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open: " + file_path + ", please check the file path.\n");
        }

        int width = 12;

        file << std::left << std::setw(width) << "customer"
             << std::setw(width) << "number"
             << std::setw(width) << "arrive at"
             << std::setw(width) << "serve at"
             << std::setw(width) << "leave at"
             << std::setw(width) << "wait time"
             << std::setw(width) << "serve time"
             << std::setw(width) << "teller" << std::endl;

        for (const auto &customer : customers)
        {
            auto arrive_at = std::chrono::duration_cast<std::chrono::milliseconds>(
                                 customer->get_arrive_time_point() - customer->get_start_time_point())
                                 .count();
            auto serve_at = std::chrono::duration_cast<std::chrono::milliseconds>(
                                customer->get_serve_time_point() - customer->get_start_time_point())
                                .count();
            auto leave_at = std::chrono::duration_cast<std::chrono::milliseconds>(
                                customer->get_leave_time_point() - customer->get_start_time_point())
                                .count();

            file << std::left << std::setw(width) << customer->get_name()
                 << std::setw(width) << customer->get_number()
                 << std::setw(width) << arrive_at
                 << std::setw(width) << serve_at
                 << std::setw(width) << leave_at
                 << std::setw(width) << serve_at - arrive_at
                 << std::setw(width) << leave_at - serve_at
                 << std::setw(width) << customer->get_served_by() << std::endl;
        }

        file.close();
    }

    void output_teller_thread_info(const std::vector<std::unique_ptr<Teller>> &tellers,
                                   const std::string &file_path)
    {
        std::ofstream file(file_path);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open: " + file_path + ", please check the file path.\n");
        }

        int width = 10;

        file << std::left << std::setw(width) << "customer"
             << std::setw(width) << "number"
             << std::setw(width) << "begin at"
             << std::setw(width) << "end at"
             << std::setw(width) << "serve time" << std::endl;

        for (const auto &teller : tellers)
        {
            file << "teller " << teller->get_name() << " :" << std::endl;

            for (const auto &record : teller->get_service_records())
            {
                auto begin_at = std::chrono::duration_cast<std::chrono::milliseconds>(
                                    record.service_start_time - record.open_time)
                                    .count();
                auto end_at = std::chrono::duration_cast<std::chrono::milliseconds>(
                                  record.service_end_time - record.open_time)
                                  .count();

                file << std::left << std::setw(width) << record.customer_name
                     << std::setw(width) << record.customer_number
                     << std::setw(width) << begin_at
                     << std::setw(width) << end_at
                     << std::setw(width) << end_at - begin_at << std::endl;
            }

            file << std::endl;
        }

        file.close();
    }

} // namespace utils
