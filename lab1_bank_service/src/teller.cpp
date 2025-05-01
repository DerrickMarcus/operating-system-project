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

Teller::Teller(int name) : name_(name), serving_for_(nullptr) {}

void Teller::start()
{
    this->thread_ = std::thread(&Teller::serve, this);
}

void Teller::join()
{
    if (this->thread_.joinable())
    {
        this->thread_.join();
    }
}

int Teller::get_name() const
{
    return this->name_;
}

const std::vector<ServiceRecord> &Teller::get_service_records() const
{
    return this->service_records_;
}

void Teller::serve()
{
    try
    {
        while (globals::served_customers_number.load() < globals::customers_number)
        {
            // wait for a customer in the queue
            // globals::customer_ready->acquire();
            if (!globals::customer_ready->try_acquire_for(std::chrono::milliseconds(1)))
            {
                continue;
            }

            // get a customer from the queue
            {
                std::unique_lock<std::mutex> lock(globals::waiting_queue_mutex);
                if (!globals::waiting_queue.empty())
                {
                    this->serving_for_ = std::move(globals::waiting_queue.front());
                    globals::waiting_queue.pop();
                }
                else
                {
                    // no customer in the queue, continue to wait
                    continue;
                }
            }

            if (this->serving_for_ == nullptr)
            {
                // no customer in the queue, continue to wait
                continue;
            }

            // notify the customer that he is being called
            this->serving_for_->notify_called(this->name_);

            // be ready to serve
            globals::teller_ready->release();

            // serve the customer
            utils::safe_print("[Teller ", this->name_,
                              "] serving [Customer ", this->serving_for_->get_name(),
                              "], number ", this->serving_for_->get_number(),
                              ", for ", this->serving_for_->get_service_time(), " ms.");
            std::this_thread::sleep_for(std::chrono::milliseconds(this->serving_for_->get_service_time()));

            // record the service process
            this->service_records_.emplace_back(
                this->serving_for_->get_name(),
                this->serving_for_->get_number(),
                this->serving_for_->get_start_time_point(),
                this->serving_for_->get_serve_time_point(),
                std::chrono::steady_clock::now()
                // here do not use "get_leave_time_point()" because it may not be set yet
            );

            // add a served customer
            globals::served_customers_number.fetch_add(1);

            utils::safe_print("[Teller ", this->name_,
                              "] finished serving [Customer ", this->serving_for_->get_name(), "].");

            this->serving_for_.reset();
        }
    }
    catch (const std::exception &e)
    {
        utils::safe_print("[ERROR] Teller ", this->name_, " thread caught exception: ", e.what());
    }
}
