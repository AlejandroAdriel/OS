#include <iostream>
#include <csignal>
#include <mutex>
#include <condition_variable>
#include <thread>

std::mutex mtx;
std::condition_variable cv;
bool signalReceived = false;

// [TECNICA: SIGNALIZACION] Manejador de senal
void handleSignal(int sig) {
    {
        // [TECNICA: CONDICIONAL VARIABLE] Notificacion bajo mutex
        std::lock_guard<std::mutex> lock(mtx);
        signalReceived = true;
    }
    cv.notify_one();
}

int main() {
    // [TECNICA: SIGNALIZACION] Registro de la senal
    std::signal(SIGUSR1, handleSignal);

    std::thread t([]() {
        std::unique_lock<std::mutex> lock(mtx);
        // [TECNICA: CONDICIONAL VARIABLE] Espera de condicion disparada por senal
        cv.wait(lock, [] { return signalReceived; });
        std::cout << "[SIGNALIZACION + COND VAR] Hilo reactivado tras senal\n";
    });

    // [TECNICA: SIGNALIZACION] Envio de senal
    std::raise(SIGUSR1);
    t.join();
    return 0;
}
