#include <iostream>
#include <unistd.h>
#include <mutex>

std::mutex mtx;

int main() {
    {
        // [TECNICA: LOCKS] Proteccion de operaciones previas a la transicion
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "[EXECV + LOCKS] Lock liberado antes de execv...\n";
    }

    char *args[] = {"./hijo", NULL};
    // [TECNICA: EXECV] Reemplazo de memoria
    execv("./hijo", args);
    return 0;
}
