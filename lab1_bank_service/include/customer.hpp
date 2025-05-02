#pragma once

#include <thread>
#include <future>
#include <chrono>

// a simple customer information structure
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
    void start(); // Start the customer thread.
    void join();  // Wait for the customer thread to finish.

    int get_name() const;
    int get_arrive_time() const;
    int get_service_time() const;
    int get_number() const;
    int get_served_by() const;
    const std::chrono::steady_clock::time_point &get_start_time_point() const;
    const std::chrono::steady_clock::time_point &get_arrive_time_point() const;
    const std::chrono::steady_clock::time_point &get_serve_time_point() const;
    const std::chrono::steady_clock::time_point &get_leave_time_point() const;

    // Notify the customer that he is being called by a teller.
    void notify_called(int teller_name);

private:
    void _arrive();

    int _name;
    int _arrive_time;
    int _service_time;

    int _number;    // getting number
    int _served_by; // served by which teller
    std::thread _thread;

    std::chrono::steady_clock::time_point _start_time_point;
    std::chrono::steady_clock::time_point _arrive_time_point;
    std::chrono::steady_clock::time_point _serve_time_point;
    std::chrono::steady_clock::time_point _leave_time_point;

    std::promise<int> _called_promise;
    std::future<int> _called_future;
};
