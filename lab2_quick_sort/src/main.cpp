#include <stddef.h>
#include <sys/mman.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>

#include "../include/shared_memory.hpp"
#include "../include/sorter.hpp"

int main(int argc, char *argv[])
{
    const std::string INPUT_PATH = "../data/integers.txt";
    const std::string OUTPUT_PATH = "../data/sorted_integers.txt";
    std::cout << "Reading data from: " << INPUT_PATH << std::endl;

    // Read the input file and store the integers in a vector.
    std::ifstream fin(INPUT_PATH);
    if (!fin.is_open())
    {
        std::cerr << "Error: Failed to open the input file:" << INPUT_PATH << std::endl;
        return 1;
    }
    std::vector<int> buffer;
    int num;
    while (fin >> num)
    {
        buffer.push_back(num);
    }
    fin.close();
    if (buffer.empty())
    {
        std::cerr << "Error: The input file is empty." << std::endl;
        return 1;
    }
    size_t data_size = buffer.size();
    std::cout << "The number of integers: " << data_size << std::endl;

    // Create a shared memory.
    const std::string shm_name = "/lab2_quick_sort";
    const int max_procs_num = 20;
    const int threshold = 1000;

    SharedMemory shm(shm_name, data_size, true);
    int *data_ptr = shm.get_data_ptr();
    if (data_ptr == nullptr)
    {
        std::cerr << "Error: Failed to get the data pointer." << std::endl;
        return 1;
    }
    std::copy(buffer.begin(), buffer.end(), data_ptr);

    // Sort the data in the shared memory.
    Sorter sorter(shm_name, data_size, false, max_procs_num, threshold);
    sorter.sort_all();

    if (!sorter.verify_sorted())
    {
        std::cerr << "Error: The data is not sorted correctly." << std::endl;
        return 1;
    }
    std::cout << "The data is sorted correctly." << std::endl;

    // Unlink the shared memory, otherwise it will still exist in the system.
    shm.remove();

    // Write the sorted data to the output file.
    std::ofstream fout(OUTPUT_PATH);
    if (!fout.is_open())
    {
        std::cerr << "Error: Failed to open the output file:" << OUTPUT_PATH << std::endl;
        return 1;
    }
    for (size_t i = 0; i < data_size; i++)
    {
        fout << data_ptr[i] << std::endl;
    }
    fout.close();
    std::cout << "The sorted data is written to: " << OUTPUT_PATH << std::endl;

    return 0;
}