#include <iostream>
#include <thread>
#include <vector>
#include <atomic>

// This function runs an infinite loop to keep the CPU core busy
void spin() {
    while (true) {
        // We use a volatile variable or a no-op to prevent 
        // the compiler from optimizing the loop away.
        __asm__(""); 
    }
}

int main() {
    // Determine how many hardware threads (cores) are available
    unsigned int num_threads = std::thread::hardware_concurrency();
    
    std::cout << "Detected " << num_threads << " logical cores." << std::endl;
    std::cout << "Starting threads to max out CPU... Press Ctrl+C to stop." << std::endl;

    std::vector<std::thread> workers;

    // Launch a thread for every core
    for (unsigned int i = 0; i < num_threads; ++i) {
        workers.emplace_back(spin);
    }

    // Join threads (this keeps the main thread alive)
    for (auto& t : workers) {
        if (t.joinable()) {
            t.join();
        }
    }

    return 0;
}