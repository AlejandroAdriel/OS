#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <mutex>
#include <condition_variable>
#include <thread>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;

int main() {
    // [TECNICA: FORK] Creacion de proceso hijo
    pid_t pid = fork();

    if (pid == 0) {
        // En el hijo se gestiona un hilo y su condition variable
        std::thread t([]() {
            {
                std::lock_guard<std::mutex> lock(mtx);
                ready = true;
            }
            // [TECNICA: CONDICIONAL VARIABLE] Notificacion interna
            cv.notify_one();
        });

        std::unique_lock<std::mutex> lock(mtx);
        // [TECNICA: CONDICIONAL VARIABLE] Espera de hilo en el hijo
        cv.wait(lock, [] { return ready; });
        t.join();
        std::cout << "[COND VAR + FORK] Hijo completo sincronizacion de hilos exitosamente\n";
        _exit(0);
    } else {
        // [TECNICA: FORK] Padre espera la finalizacion del hijo
        wait(NULL);
        std::cout << "[COND VAR + FORK] Padre recibio confirmacion de hijo\n";
    }
    return 0;
}
