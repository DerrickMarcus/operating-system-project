import random


def generate_customer_data(num_customers, filename):
    with open(filename, "w") as file:
        for i in range(1, num_customers + 1):
            arrive_time = random.randint(1, 100)
            service_time = random.randint(1, 20)
            file.write(f"{i} {arrive_time} {service_time}\n")


if __name__ == "__main__":
    num_customers = 10
    filename = "../data/example_2.txt"
    generate_customer_data(num_customers, filename)
    print(f"Generated {num_customers} customer records in {filename}.")
