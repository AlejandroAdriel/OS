#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;

int main() {
    std::thread trabajador([]() {
        // [TECNICA: LOCKS] Cerrojo con unique_lock requerido por variable condicional
        std::unique_lock<std::mutex> lock(mtx);
        // [TECNICA: CONDICIONAL VARIABLE] Espera de evento
        cv.wait(lock, [] { return ready; });
        std::cout << "[LOCKS + COND VAR] Hilo continuo tras notificacion con lock adquirido\n";
    });

    {
        // [TECNICA: LOCKS] Modificacion segura de estado
        std::lock_guard<std::mutex> lock(mtx);
        ready = true;
    }
    // [TECNICA: CONDICIONAL VARIABLE] Despertar al hilo
    cv.notify_one();

    trabajador.join();
    return 0;
}
