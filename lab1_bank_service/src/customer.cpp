#include "customer.hpp"
#include "utils.hpp"

Customer::Customer(int name, int arrive_time, int service_time)
    : name_(name), arrive_time_(arrive_time), service_time_(service_time), number_(-1), served_by_(-1), called_future_(called_promise_.get_future()) {}

void Customer::start()
{
    this->thread_ = std::thread(&Customer::arrive, this);
}

void Customer::join()
{
    if (this->thread_.joinable())
    {
        this->thread_.join();
    }
}

int Customer::get_number() const
{
    return this->number_;
}

int Customer::get_service_time() const
{
    return this->service_time_;
}

void Customer::notify_called(int teller_name)
{
    this->called_promise_.set_value(teller_name);
}

void Customer::arrive()
{
    // wait for all customers be prepared
    customers_barrier.arrive_and_wait();

    // sleep until arrival
    std::this_thread::sleep_for(std::chrono::milliseconds(this->arrive_time_));
    this->arrive_time_point_ = std::chrono::steady_clock::now();

    // arrive and get a number
    {
        std::unique_lock<std::mutex> lock(getting_number_mutex);
        this->number_ = getting_number;
        getting_number++;
        utils::safe_print("Customer ", this->name_, " arrived, getting number ", this->number_, ".");
    }

    // join the waiting queue
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        waiting_queue.push(this);
    }

    // notify that a customer is ready
    customer_ready.release();

    // wait for being called
    this->served_by_ = called_future_.get();

    // wait for a teller
    teller_ready.acquire();

    // be served and leave
    this->service_time_point_ = std::chrono::steady_clock::now();
    utils::safe_print("Customer ", this->name_, " being served by teller ", this->served_by_, ", for ", this->service_time_, " ms.");
    std::this_thread::sleep_for(std::chrono::milliseconds(this->service_time_));

    this->leave_time_point_ = std::chrono::steady_clock::now();
    utils::safe_print("Customer ", this->name_, " finished service.");
}
