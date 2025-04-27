#include <iostream>
#include <semaphore>
#include <thread>

const int TELLER_NUMBERS = 10;

std::counting_semaphore<> customer_ready(0);
std::counting_semaphore<> teller_ready(0);

int main(int argc, const char *argv[])
{

    return 0;
}