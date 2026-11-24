#include <mutex>
#include <condition_variable>
#include <thread>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;

void worker() {
    std::unique_lock<std::mutex> lck(mtx);
    while (!ready) cv.wait(lck);
}

int main() {
    std::thread t(worker);
    {
        std::lock_guard<std::mutex> lck(mtx);
        ready = true;
    }
    cv.notify_one();
    t.join();
    return 0;
}
