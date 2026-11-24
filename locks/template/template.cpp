#include <mutex>
#include <thread>

std::mutex mtx;

void task() {
    mtx.lock();
    mtx.unlock();
}

int main() {
    std::thread t1(task);
    std::thread t2(task);
    t1.join();
    t2.join();
    return 0;
}
