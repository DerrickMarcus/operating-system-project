#include <semaphore>
#include "utils.hpp"

const int TELLER_NUMBER = 10;

std::counting_semaphore<> customer_ready(0);
std::counting_semaphore<TELLER_NUMBER> teller_ready(TELLER_NUMBER);

int main(int argc, const char *argv[])
{
    // for (auto &customer : customers)
    // {
    //     threads.emplace_back(&Customer::arrive, &customer);
    // }
    // std::vector<utils::CustomerInfo> customers;

    return 0;
}