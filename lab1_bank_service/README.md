# Lab1: Bank Service

## How to Run the Program

To run the program, follow these steps in your terminal:

```bash
cd lab1_bank_service

rm -rf build # # only if "build/" directory exists
mkdir build
cd build

cmake ..
make # or: cmake --build .

cd ../bin
./bank_service
```

Alternatively, you can also run the program simply using the provided scripts:

```bash
cd lab1_bank_service

chmod +x ./run.sh
./run.bash

# or:
python3 ./scripts/run.py
```

## How to Modify Parameters

The number of customers and tellers `TELLERS_NUMBER, MAX_CUSTOMERS_NUMBER` determines the type of `std::counting_semaphore`, and must be determined when compiling before running. The number of tellers `TELLERS_NUMBER` is also the maximum value of semaphore `teller_ready` . The number of customers is unknown when compiling. It is known until the program read the data file when running, so you can just set a large enough value to `MAX_CUSTOMERS_NUMBER` . If you need to modify these **template parameters** , you can edit the following constants in the `./include/globals.hpp` file:

```cpp
// ./include/globals.hpp
// ...
inline constexpr int MAX_CUSTOMERS_NUMBER = 1000; // the maximum number of customers
inline constexpr int TELLERS_NUMBER = 10;         // the number of tellers
// ...
```

After making any changes, run the command `./run.bash` again to recompile and generate a new executable file at `./bin/bank_service`.

If you want to change the **data or log file paths**, such as `CUSTOMER_INFO_PATH`, you do not need to recompile the program. These paths are defined in the configuration file `./config/config.json`, and the program will automatically load them when running. Here is the default configurations:

```json
{
  "CUSTOMER_INFO_PATH": "../data/example_1.txt",
  "CUSTOMER_THREAD_INFO_PATH": "../data/customer_log.txt",
  "TELLER_THREAD_INFO_PATH": "../data/teller_log.txt"
}
```

Simply edit this file to specify new paths for your data and log files.

## How to Generate Data of Customers

You can generate example customer data for testing by running the `./scripts/generate_data.py` .

```bash
cd lab1_bank_service/scripts
python3 generate_data.py
```

You can customize the number of customers (`customers_num`), the output file path, and the range of random integers used in the data generation process.

The flag variable `increasing_arrival` controls whether the arrival times of customers increase based on their customer numbers. More specifically:

1. If `increasing_arrival = True`, the arrival times of customers are assigned in increasing order, with each subsequent customer arriving later than the previous one.
2. If `increasing_arrival = False`, the arrival times of all customers are completely unordered, and each customer’s arrival time is randomly selected without any specific order.

When generating data, make sure to adjust the relationship between the number of customers and the range of random numbers. When there are many customers, increase the arrival time range and decrease the service time range appropriately. Otherwise, the bank will be very crowded, and customers arriving later will experience very long waiting times.
