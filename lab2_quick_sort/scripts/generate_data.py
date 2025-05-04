import random
from pathlib import Path


def generate_random_integers(file_path, count, max_value=10_000_000):
    """Generate random integers and save them to a text file.
    The integers are generated in the range [0, max_value].

    Args:
        file_path (pathlib.Path): The path to the file where the random integers will be saved.
        count (int): The number of random integers to generate.
        max_value (int, optional): The maximum value of the random integers. Defaults to 10_000_000.
    """
    file_path.parent.mkdir(parents=True, exist_ok=True)

    with file_path.open("w") as f:
        for _ in range(count):
            f.write(f"{random.randint(0, max_value)}\n")
    print(f"Generated {count} random integers in {file_path}.")


if __name__ == "__main__":
    script_dir = Path(__file__).resolve().parent
    output_path = script_dir / "../data/integers.txt"
    count = 1_000_000
    max_value = 10_000_000

    generate_random_integers(output_path, count, max_value)
