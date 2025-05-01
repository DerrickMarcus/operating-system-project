import random
from pathlib import Path


def generate_customer_data(customers_num, file_path, increasing_arrival=True):
    file_path.parent.mkdir(parents=True, exist_ok=True)

    with file_path.open("w") as file:
        arrival_times = random.choices(range(1, 1000), k=customers_num)

        if increasing_arrival:
            arrival_times.sort()

        for i in range(customers_num):
            arrive_time = arrival_times[i]
            service_time = random.randint(50, 150)
            file.write(f"{i + 1} {arrive_time} {service_time}\n")

    print(f"Generated {customers_num} customer records in {file_path}.")


if __name__ == "__main__":
    customers_num = 100
    script_dir = Path(__file__).resolve().parent
    file_path = script_dir.parent / "data" / "example_2.txt"

    generate_customer_data(customers_num, file_path, True)
