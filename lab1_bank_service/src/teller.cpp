#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <semaphore>
#include <atomic>
#include <future>
#include <chrono>
#include <stdexcept>

#include "../include/teller.hpp"
#include "../include/customer.hpp"
#include "../include/globals.hpp"
#include "../include/utils.hpp"

ServiceRecord::ServiceRecord(int customer_name, int customer_number,
                             std::chrono::steady_clock::time_point open_time,
                             std::chrono::steady_clock::time_point service_start_time,
                             std::chrono::steady_clock::time_point service_end_time)
    : customer_name(customer_name), customer_number(customer_number),
      open_time(open_time), service_start_time(service_start_time),
      service_end_time(service_end_time) {}

Teller::Teller(int name) : _name(name), _serving_for(nullptr) {}

void Teller::start()
{
    this->_thread = std::thread(&Teller::_serve, this);
}

void Teller::join()
{
    if (this->_thread.joinable())
    {
        this->_thread.join();
    }
}

int Teller::get_name() const
{
    return this->_name;
}

const std::vector<ServiceRecord> &Teller::get_service_records() const
{
    return this->_service_records;
}

void Teller::_serve()
{
    try
    {
        while (globals::served_customers_number.load() < globals::customers_number)
        {
            // Wait for a customer in the waiting queue.
            if (!globals::customer_ready->try_acquire_for(std::chrono::milliseconds(1)))
            {
                continue;
            }

            // Get a customer from the waiting queue.
            {
                std::unique_lock<std::mutex> lock(globals::waiting_queue_mutex);
                if (!globals::waiting_queue.empty())
                {
                    this->_serving_for = std::move(globals::waiting_queue.front());
                    globals::waiting_queue.pop();
                }
                else
                {
                    // If no customer in the queue, continue to wait.
                    continue;
                }
            }

            if (this->_serving_for == nullptr)
            {
                // If no customer in the queue, continue to wait.
                continue;
            }

            // Notify the customer that he is being called.
            this->_serving_for->notify_called(this->_name);

            // Be ready to serve.
            globals::teller_ready->release();

            // Serve the customer.
            utils::safe_print("[Teller ", this->_name,
                              "] serving [Customer ", this->_serving_for->get_name(),
                              "], number ", this->_serving_for->get_number(),
                              ", for ", this->_serving_for->get_service_time(), " ms.");
            std::this_thread::sleep_for(std::chrono::milliseconds(this->_serving_for->get_service_time()));

            // Add a record of the service process
            this->_service_records.emplace_back(
                this->_serving_for->get_name(),
                this->_serving_for->get_number(),
                this->_serving_for->get_start_time_point(),
                this->_serving_for->get_serve_time_point(),
                std::chrono::steady_clock::now()
                // Here do not use "get_leave_time_point()" because it may not be set yet.
            );

            // Add a served customer.
            globals::served_customers_number.fetch_add(1);

            utils::safe_print("[Teller ", this->_name,
                              "] finished serving [Customer ", this->_serving_for->get_name(), "].");

            this->_serving_for.reset();
        }
    }
    catch (const std::exception &e)
    {
        utils::safe_print("[ERROR] Teller ", this->_name, " thread caught exception: ", e.what());
    }
}
