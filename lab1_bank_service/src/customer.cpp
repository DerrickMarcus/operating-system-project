#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <semaphore>
#include <barrier>
#include <future>
#include <chrono>
#include <stdexcept>
#include "../include/customer.hpp"
#include "../include/globals.hpp"
#include "../include/utils.hpp"

CustomerInfo::CustomerInfo(int name, int arrive_time, int service_time)
    : name(name), arrive_time(arrive_time), service_time(service_time) {}

Customer::Customer(int name, int arrive_time, int service_time)
    : _name(name), _arrive_time(arrive_time), _service_time(service_time),
      _number(-1), _served_by(-1), _called_future(_called_promise.get_future()) {}

void Customer::start()
{
    this->_thread = std::thread(&Customer::_arrive, this);
}

void Customer::join()
{
    if (this->_thread.joinable())
    {
        this->_thread.join();
    }
}

int Customer::get_name() const
{
    return this->_name;
}

int Customer::get_arrive_time() const
{
    return this->_arrive_time;
}

int Customer::get_service_time() const
{
    return this->_service_time;
}

int Customer::get_number() const
{
    return this->_number;
}

int Customer::get_served_by() const
{
    return this->_served_by;
}

const std::chrono::steady_clock::time_point &Customer::get_start_time_point() const
{
    return this->_start_time_point;
}

const std::chrono::steady_clock::time_point &Customer::get_arrive_time_point() const
{
    return this->_arrive_time_point;
}

const std::chrono::steady_clock::time_point &Customer::get_serve_time_point() const
{
    return this->_serve_time_point;
}

const std::chrono::steady_clock::time_point &Customer::get_leave_time_point() const
{
    return this->_leave_time_point;
}

void Customer::notify_called(int teller_name)
{
    this->_called_promise.set_value(teller_name);
}

void Customer::_arrive()
{
    try
    {
        // wait for all customers be prepared
        globals::customers_barrier->arrive_and_wait();
        this->_start_time_point = globals::open_time_point;

        // sleep until arrival
        std::this_thread::sleep_for(std::chrono::milliseconds(this->_arrive_time));
        this->_arrive_time_point = std::chrono::steady_clock::now();

        // arrive and get a number
        {
            std::unique_lock<std::mutex> lock(globals::getting_number_mutex);
            this->_number = globals::getting_number;
            globals::getting_number++;
            utils::safe_print("[Customer ", this->_name,
                              "] arrived, getting number ", this->_number, ".");
        }

        // join the waiting queue
        {
            std::unique_lock<std::mutex> lock(globals::waiting_queue_mutex);
            globals::waiting_queue.emplace(shared_from_this());
        }

        // notify that a customer is ready
        globals::customer_ready->release();

        // wait for being called
        this->_served_by = this->_called_future.get();

        // wait for a teller
        globals::teller_ready->acquire();

        // be served and leave
        this->_serve_time_point = std::chrono::steady_clock::now();
        utils::safe_print("[Customer ", this->_name,
                          "] being served by [Teller ", this->_served_by,
                          "] for ", this->_service_time, " ms.");
        std::this_thread::sleep_for(std::chrono::milliseconds(this->_service_time));

        this->_leave_time_point = std::chrono::steady_clock::now();
        utils::safe_print("[Customer ", this->_name, "] finished.");
    }
    catch (const std::exception &e)
    {
        utils::safe_print("[ERROR] Customer ", this->_name, " thread caught exception: ", e.what());
    }
}
