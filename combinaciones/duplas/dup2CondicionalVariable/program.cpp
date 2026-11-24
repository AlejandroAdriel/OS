#include <iostream>
#include <unistd.h>
#include <mutex>
#include <condition_variable>
#include <thread>

std::mutex mtx;
std::condition_variable cv;
bool redirected = false;

int main() {
    std::thread t1([]() {
        {
            std::lock_guard<std::mutex> lock(mtx);
            // [TECNICA: DUP2] Operacion de redireccion
            int clon = dup(STDOUT_FILENO);
            dup2(clon, STDOUT_FILENO);
            close(clon);
            // [TECNICA: CONDICIONAL VARIABLE] Notificacion del estado
            redirected = true;
        }
        cv.notify_one();
    });

    std::thread t2([]() {
        std::unique_lock<std::mutex> lock(mtx);
        // [TECNICA: CONDICIONAL VARIABLE] Espera pasiva de finalizacion de redireccion
        cv.wait(lock, [] { return redirected; });
        std::cout << "[DUP2 + COND VAR] Salida comprobada post-redireccion\n";
    });

    t1.join();
    t2.join();
    return 0;
}
