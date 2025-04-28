#pragma once

#include <queue>
#include <thread>
#include <semaphore>
#include <mutex>
#include <barrier>
#include <future>
#include <chrono>

extern std::counting_semaphore<> customer_ready;
extern std::counting_semaphore<> teller_ready;

extern int getting_number;

extern std::mutex getting_number_mutex;
extern std::mutex queue_mutex;

extern std::queue<Customer *> waiting_queue;

extern std::barrier<> customers_barrier;

extern std::chrono::steady_clock::time_point open_time_point;

class Customer
{
public:
    Customer(int name, int arrive_time, int service_time);
    void start(); // start the customer thread
    void join();  // wait for the customer thread to finish

    int get_number() const;
    int get_service_time() const;

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

    std::chrono::steady_clock::time_point arrive_time_point_;
    std::chrono::steady_clock::time_point service_time_point_;
    std::chrono::steady_clock::time_point leave_time_point_;

    std::promise<int> called_promise_;
    std::future<int> called_future_;
};
