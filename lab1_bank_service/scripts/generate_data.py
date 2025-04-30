#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import random


def generate_customer_data(customers_num, filename, increasing_arrival=True):
    os.makedirs(os.path.dirname(filename), exist_ok=True)

    with open(filename, "w") as file:
        arrival_times = random.choices(range(1, 1000), k=customers_num)

        if increasing_arrival:
            arrival_times.sort()

        for i in range(0, customers_num):
            arrive_time = arrival_times[i]
            service_time = random.randint(50, 150)
            file.write(f"{i + 1} {arrive_time} {service_time}\n")


if __name__ == "__main__":
    customers_num = 100
    script_dir = os.path.dirname(os.path.realpath(__file__))
    filename = os.path.join(script_dir, "../data/example_2.txt")

    generate_customer_data(customers_num, filename, True)
    print(f"Generated {customers_num} customer records in {filename}.")
