from pathlib import Path

import numpy as np


def generate_random_integers(file_path, count, max_value=10_000_000):
    """Generate random integers and save them to a binary file.
    The integers are in the range of [0, max_value] and of type int32.

    Args:
        file_path (pathlib.Path): The path to the file where the random integers will be saved.
        count (int): The number of random integers to generate.
        max_value (int, optional): The maximum value of the random integers. Defaults to 10_000_000.
    """
    file_path.parent.mkdir(parents=True, exist_ok=True)

    data = np.random.randint(0, max_value + 1, size=count, dtype=np.int32)
    data.tofile(file_path)
    print(f"Generated {count} random integers in {file_path}.")


if __name__ == "__main__":
    script_dir = Path(__file__).resolve().parent
    output_path = script_dir / "../data/integers.bin"
    count = 1_000_000
    max_value = 10_000_000

    generate_random_integers(output_path, count, max_value)
