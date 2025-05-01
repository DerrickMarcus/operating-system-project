#include <fstream>
#include <string>
#include <iomanip>
#include <vector>
#include <memory>
#include <chrono>
#include <stdexcept>
#include "../include/utils.hpp"
#include "../include/customer.hpp"
#include "../include/teller.hpp"
#include "../include/globals.hpp"
#include "../include/json.hpp"

namespace utils
{

    void load_config_from_json(const std::string &json_path)
    {
        std::ifstream config_file(json_path);
        if (!config_file.is_open())
        {
            utils::safe_print("[WARNING] Failed to open: " + json_path + ". Using default values instead.");
            utils::safe_print("[WARNING] CUSTOMER_INFO_FILE_PATH: ", globals::CUSTOMER_INFO_FILE_PATH);
            utils::safe_print("[WARNING] CUSTOMER_THREAD_INFO_FILE_PATH: ", globals::CUSTOMER_THREAD_INFO_FILE_PATH);
            utils::safe_print("[WARNING] TELLER_THREAD_INFO_FILE_PATH: ", globals::TELLER_THREAD_INFO_FILE_PATH);
            return;
        }

        nlohmann::json j;
        config_file >> j;

        if (j.contains("CUSTOMER_INFO_FILE_PATH") && j["CUSTOMER_INFO_FILE_PATH"].is_string())
        {
            globals::CUSTOMER_INFO_FILE_PATH = j["CUSTOMER_INFO_FILE_PATH"];
        }

        if (j.contains("CUSTOMER_THREAD_INFO_FILE_PATH") && j["CUSTOMER_THREAD_INFO_FILE_PATH"].is_string())
        {
            globals::CUSTOMER_THREAD_INFO_FILE_PATH = j["CUSTOMER_THREAD_INFO_FILE_PATH"];
        }

        if (j.contains("TELLER_THREAD_INFO_FILE_PATH") && j["TELLER_THREAD_INFO_FILE_PATH"].is_string())
        {
            globals::TELLER_THREAD_INFO_FILE_PATH = j["TELLER_THREAD_INFO_FILE_PATH"];
        }

        utils::safe_print("Loaded configurations from file: ", json_path);
    }

    const std::vector<CustomerInfo> parse_customer_info(const std::string &file_path)
    {
        std::vector<CustomerInfo> customers;
        std::ifstream file(file_path);

        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open: " + file_path + ". Please check the file path.\n");
        }

        int name, arrive_time, service_time;
        while (file >> name >> arrive_time >> service_time)
        {
            customers.emplace_back(name, arrive_time, service_time);
        }

        file.close();
        utils::safe_print("Parsed customers information from file: ", file_path);
        return customers;
    }

    void output_customer_thread_info(const std::vector<std::shared_ptr<Customer>> &customers,
                                     const std::string &file_path)
    {
        std::ofstream file(file_path);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open: " + file_path + ". Please check the file path.\n");
            return;
        }

        file << std::left
             << std::setw(10) << "customer"
             << std::setw(8) << "number"
             << std::setw(11) << "arrive at"
             << std::setw(10) << "serve at"
             << std::setw(10) << "leave at"
             << std::setw(11) << "wait time"
             << std::setw(12) << "serve time"
             << std::setw(6) << "teller" << std::endl;

        file << "------------------------------------------------------------------------------" << std::endl;

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

            file << std::left
                 << std::setw(10) << customer->get_name()
                 << std::setw(8) << customer->get_number()
                 << std::setw(11) << arrive_at
                 << std::setw(10) << serve_at
                 << std::setw(10) << leave_at
                 << std::setw(11) << serve_at - arrive_at
                 << std::setw(12) << leave_at - serve_at
                 << std::setw(6) << customer->get_served_by() << std::endl;
        }

        file.close();
        utils::safe_print("Output customers thread log to file: ", file_path);
    }

    void output_teller_thread_info(const std::vector<std::unique_ptr<Teller>> &tellers,
                                   const std::string &file_path)
    {
        std::ofstream file(file_path);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open: " + file_path + ". Please check the file path.\n");
            return;
        }

        file << std::left
             << std::setw(10) << "customer"
             << std::setw(8) << "number"
             << std::setw(10) << "begin at"
             << std::setw(8) << "end at"
             << std::setw(9) << "serve time" << std::endl;

        file << "----------------------------------------------" << std::endl;

        for (const auto &teller : tellers)
        {
            file << "teller " << teller->get_name() << std::endl;

            for (const auto &record : teller->get_service_records())
            {
                auto begin_at = std::chrono::duration_cast<std::chrono::milliseconds>(
                                    record.service_start_time - record.open_time)
                                    .count();
                auto end_at = std::chrono::duration_cast<std::chrono::milliseconds>(
                                  record.service_end_time - record.open_time)
                                  .count();

                file << std::left
                     << std::setw(10) << record.customer_name
                     << std::setw(8) << record.customer_number
                     << std::setw(10) << begin_at
                     << std::setw(8) << end_at
                     << std::setw(9) << end_at - begin_at << std::endl;
            }

            file << std::endl;
        }

        file.close();
        utils::safe_print("Output tellers thread log to file: ", file_path);
    }

} // namespace utils
