#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <mutex>

std::mutex mtx;

int main() {
    // [TECNICA: LOCKS] Cerrojo antes de bifurcar
    mtx.lock();

    // [TECNICA: FORK] Clonacion de proceso
    pid_t pid = fork();

    if (pid == 0) {
        std::cout << "[LOCKS + FORK] Proceso hijo en ejecucion\n";
        _exit(0);
    } else {
        mtx.unlock(); // [TECNICA: LOCKS] Padre desbloquea
        // [TECNICA: FORK] Padre espera al hijo
        wait(NULL);
        std::cout << "[LOCKS + FORK] Padre finalizo correctamente tras wait\n";
    }
    return 0;
}
