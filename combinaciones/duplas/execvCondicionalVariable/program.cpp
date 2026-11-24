#include <iostream>
#include <unistd.h>
#include <mutex>
#include <condition_variable>
#include <thread>

std::mutex mtx;
std::condition_variable cv;
bool readyForExec = false;

int main() {
    std::thread t([]() {
        {
            std::lock_guard<std::mutex> lock(mtx);
            // [TECNICA: CONDICIONAL VARIABLE] Disparo de condicion
            readyForExec = true;
        }
        cv.notify_one();
    });

    {
        std::unique_lock<std::mutex> lock(mtx);
        // [TECNICA: CONDICIONAL VARIABLE] Espera de autorizacion para ejecutar execv
        cv.wait(lock, [] { return readyForExec; });
    }
    t.join();

    std::cout << "[EXECV + COND VAR] Condicion cumplida. Lanzando execv...\n";
    char *args[] = {"./hijo", NULL};
    // [TECNICA: EXECV] Reemplazo de memoria
    execv("./hijo", args);
    return 0;
}
