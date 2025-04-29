#pragma once

#include <vector>
#include <memory>
#include <thread>
#include <chrono>

class Customer;

struct ServiceRecord
{
    int customer_name;
    int customer_number;
    std::chrono::steady_clock::time_point open_time;
    std::chrono::steady_clock::time_point service_start_time;
    std::chrono::steady_clock::time_point service_end_time;

    ServiceRecord(int customer_name, int customer_number,
                  std::chrono::steady_clock::time_point open_time,
                  std::chrono::steady_clock::time_point service_start_time,
                  std::chrono::steady_clock::time_point service_end_time);
};

class Teller
{
public:
    Teller(int name);
    void start(); // start the teller thread
    void join();  // wait for the teller thread to finish

    int get_name() const;
    const std::vector<ServiceRecord> &get_service_records() const;

private:
    void serve();
    int name_;
    std::thread thread_;

    std::shared_ptr<Customer> serving_for_;
    std::vector<ServiceRecord> service_records_;
};