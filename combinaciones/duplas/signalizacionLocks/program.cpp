#include <iostream>
#include <csignal>
#include <mutex>
#include <thread>

std::mutex mtx;

// [TECNICA: SIGNALIZACION] Manejador de senal
void handleSignal(int sig) {
    // [TECNICA: LOCKS] Exclusion mutua dentro del manejador
    if (mtx.try_lock()) {
        std::cout << "[SIGNALIZACION + LOCKS] Senal " << sig << " capturada con lock adquirido\n";
        mtx.unlock();
    }
}

int main() {
    // [TECNICA: SIGNALIZACION] Registro de la senal SIGUSR1
    std::signal(SIGUSR1, handleSignal);

    // [TECNICA: LOCKS] Bloqueo de seccion critica
    {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "[LOCKS] Seccion protegida antes de senal\n";
    }

    // [TECNICA: SIGNALIZACION] Emision de senal
    std::raise(SIGUSR1);
    return 0;
}
