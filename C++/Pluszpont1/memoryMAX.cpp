#include <iostream>
#include <vector>
#include <cstring> // For memset
#include <thread>
#include <chrono>

int main() {
    // Vector to hold our pointers so the OS doesn't reclaim the memory
    std::vector<char*> memory_blocks;
    
    // Chunk size: 100 MB
    size_t chunk_size = 100 * 1024 * 1024; 
    size_t total_allocated = 0;

    std::cout << "Starting RAM allocation... Press Ctrl+C to stop." << std::endl;

    try {
        while (true) {
            // 1. Allocate the memory
            char* block = new char[chunk_size];

            // 2. "Touch" the memory. 
            // Without memset, the OS might not actually commit physical RAM.
            std::memset(block, 1, chunk_size);

            memory_blocks.push_back(block);
            total_allocated += chunk_size;

            std::cout << "Allocated: " << total_allocated / (1024 * 1024) << " MB" << std::endl;

            // Optional: Small sleep so you can watch the climb in Task Manager
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } catch (const std::bad_alloc& e) {
        std::cout << "Reached memory limit (or OS stopped us): " << e.what() << std::endl;
    }

    // Keep the program alive so the memory stays allocated
    std::cout << "Target reached. Keeping memory locked. Press Enter to exit and free RAM." << std::endl;
    std::cin.get();

    // Cleanup
    for (char* block : memory_blocks) {
        delete[] block;
    }

    return 0;
}