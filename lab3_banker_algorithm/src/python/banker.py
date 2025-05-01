import logging

import numpy as np


class Banker:
    def __init__(self, total, allocation, max_demand):
        self.total = np.array(total)
        self.allocation = np.array(allocation)
        self.max_demand = np.array(max_demand)
        self.num_processes, self.num_resources = self.allocation.shape

        self.need = self.max_demand - self.allocation
        self.available = self.total - np.sum(self.allocation, axis=0)

    def print_state(self):
        logging.info("Currnet state of the system:")
        logging.info(f"Total resources: {self.total}")
        logging.info(f"Allocated resources:\n{self.allocation}")
        logging.info(f"Maximum demand:\n{self.max_demand}")
        logging.info(f"Available resources: {self.available}")
        logging.info("-" * 60)

    def is_request_valid(self, pid: int, request: list) -> bool:
        """Check if the request form a process is valid or not.

        Args:
            pid (int): The process id
            request (list): The request list for resources.

        Returns:
            bool: True if the request is valid, False otherwise.
        """
        request = np.array(request)
        if pid < 0 or pid >= self.num_processes:
            logging.warning(f"Invalid process ID: {pid}")
            return False
        elif np.any(request > self.need[pid]):
            logging.warning(
                f"Process {pid} is requesting more than its maximum demand:",
            )
            logging.warning(f"Request: {request}")
            logging.warning(f"Need:    {self.need[pid]}")
            return False
        elif np.any(request > self.available):
            logging.warning(
                f"Process {pid} is requesting more than available resources:",
            )
            logging.warning(f"Request:   {request}")
            logging.warning(f"Available: {self.available}")
            return False
        else:
            return True

    def is_system_safe(self) -> bool:
        """Check if the system is in a safe state or not.

        Returns:
            bool: True if the system is safe, False otherwise.
        """
        work = self.available.copy()
        finish = np.zeros(self.num_processes, dtype=bool)
        safe_sequence = []

        while True:
            found = False
            for i in range(self.num_processes):
                if not finish[i] and np.all(self.need[i] <= work):
                    work += self.allocation[i]
                    finish[i] = True
                    safe_sequence.append(i)
                    found = True
                    break
            if not found:
                break

        is_safe = all(finish)
        if is_safe:
            logging.info(
                f"The system will be in a safe state. Safe sequence: {safe_sequence}"
            )
        else:
            logging.warning("The system will be in an unsafe state.")
        return is_safe

    def process_request(self, pid: int, request: list) -> bool:
        """Solve the request from a process.

        Args:
            pid (int): The process id.
            request (list): The request list for resources.

        Returns:
            bool: True if the request is accepted, False otherwise.
        """
        request = np.array(request)
        logging.info(f"Process {pid} is requesting resources: {request}")

        if not self.is_request_valid(pid, request):
            logging.info("Invalid request. Allocation rejected.")
            return False

        self.available -= request
        self.allocation[pid] += request
        self.need[pid] -= request

        if self.is_system_safe():
            logging.info("Request accepted. Allocation successful.")
            return True
        else:
            self.available += request
            self.allocation[pid] -= request
            self.need[pid] += request
            logging.info("Request denied. Rollback to the previous state.")
            return False

    def release_resources(self, pid: int, release: list) -> bool:
        """Release resources from a process.

        Args:
            pid (int): The process id.
            release (list): The release list of resources.

        Returns:
            bool: True if the release is successful, False otherwise.
        """
        release = np.array(release)
        if pid < 0 or pid >= self.num_processes:
            logging.warning(f"Invalid process ID: {pid}")
            return False
        elif np.any(release > self.allocation[pid]):
            logging.warning(
                f"Process {pid} is trying to release more than allocated resources:"
            )
            logging.warning(f"Release: {release}")
            logging.warning(f"Allocated: {self.allocation[pid]}")
            return False
        else:
            self.allocation[pid] -= release
            self.need[pid] += release
            self.available += release
            logging.info(f"Process {pid} released resources: {release}")
            return True
