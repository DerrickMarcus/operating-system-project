#include "teller.hpp"
#include "customer.hpp"

Teller::Teller(int name) : name_(name) {}

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

void Teller::serve()
{
    while (true)
    {
        // wait for a customer
        customer_ready.acquire();

        // get a customer from the queue
        Customer *customer = nullptr;

        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (!waiting_queue.empty())
            {
                customer = waiting_queue.front();
                waiting_queue.pop();
            }
        }

        // notify the customer that he is being called
        customer->notify_called(this->name_);

        // serve the customer
        std::this_thread::sleep_for(std::chrono::milliseconds(customer->get_service_time()));
        teller_ready.release();
    }
}
