#include <iostream>
#include <thread>
#include <chrono>

void pauseThread(int n) {
    std::this_thread::sleep_for(std::chrono::seconds(n));
    std::cout << "Pause of " << n << " seconds ended\n";
}

int main() {
    std::cout << "Spawning and detaching 3 threads...\n";
    std::thread(pauseThread, 1).detach();
    std::thread(pauseThread, 2).detach();
    std::thread(pauseThread, 3).detach();
    std::cout << "Done spawning threads.\n";
    pauseThread(4);
    return 0;
}
