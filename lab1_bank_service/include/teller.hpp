#pragma once

#include <semaphore>
#include <thread>

extern std::counting_semaphore<> customer_ready;
extern std::counting_semaphore<> teller_ready;

extern std::mutex queue_mutex;

class Teller
{
public:
    Teller(int name);
    void start(); // start the teller thread
    void join();  // wait for the teller thread to finish

private:
    void serve();
    int name_;
    std::thread thread_;
};