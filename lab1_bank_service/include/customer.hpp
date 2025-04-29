#pragma once

#include <thread>
#include <future>
#include <chrono>

// simple customer information structure
struct CustomerInfo
{
    int name;
    int arrive_time;
    int service_time;

    CustomerInfo(int name, int arrive_time, int service_time);
};

class Customer : public std::enable_shared_from_this<Customer>
{
public:
    Customer(int name, int arrive_time, int service_time);
    void start(); // start the customer thread
    void join();  // wait for the customer thread to finish

    int get_name() const;
    int get_arrive_time() const;
    int get_service_time() const;
    int get_number() const;
    int get_served_by() const;
    const std::chrono::steady_clock::time_point &get_start_time_point() const;
    const std::chrono::steady_clock::time_point &get_arrive_time_point() const;
    const std::chrono::steady_clock::time_point &get_serve_time_point() const;
    const std::chrono::steady_clock::time_point &get_leave_time_point() const;

    // notify the customer that he is being called by a teller
    void notify_called(int teller_name);

private:
    void arrive();

    int name_;
    int arrive_time_;
    int service_time_;

    int number_;    // getting number
    int served_by_; // served by which teller
    std::thread thread_;

    std::chrono::steady_clock::time_point start_time_point_;
    std::chrono::steady_clock::time_point arrive_time_point_;
    std::chrono::steady_clock::time_point serve_time_point_;
    std::chrono::steady_clock::time_point leave_time_point_;

    std::promise<int> called_promise_;
    std::future<int> called_future_;
};
