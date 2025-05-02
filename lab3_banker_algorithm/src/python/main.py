import json
import logging
from pathlib import Path

from banker import Banker


def load_json(json_path):
    with open(json_path, "r") as f:
        return json.load(f)


def main():
    # Set input and output file paths.
    BASE_DIR = Path(__file__).resolve().parent
    DATA_DIR = BASE_DIR.parent.parent / "data"
    JSON_FILE = DATA_DIR / "example_1.json"
    LOG_FILE = DATA_DIR / "output.log"

    # Configure the logging format.
    logging.basicConfig(
        level=logging.INFO,
        # format="[%(asctime)s] %(message)s",
        format="%(message)s",
        handlers=[
            logging.FileHandler(LOG_FILE, mode="w", encoding="utf-8"),
            logging.StreamHandler(),
        ],
    )

    logging.info(f"Read data from file {JSON_FILE}.")
    data = load_json(JSON_FILE)

    banker = Banker(data["total"], data["allocation"], data["max_demand"])
    banker.print_state()  # Print the initial state.

    for req in data.get("requests", []):
        pid = req["pid"]
        request = req["request"]
        banker.process_request(pid, request)
        banker.print_state()


if __name__ == "__main__":
    main()
