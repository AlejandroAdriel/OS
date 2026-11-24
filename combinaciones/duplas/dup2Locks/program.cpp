#include <iostream>
#include <unistd.h>
#include <mutex>
#include <thread>

std::mutex mtx;

int main() {
    std::thread t([]() {
        // [TECNICA: LOCKS] Proteccion de descriptor y tabla de archivos
        std::lock_guard<std::mutex> lock(mtx);
        int clon = dup(STDOUT_FILENO);
        // [TECNICA: DUP2] Reasignacion bajo exclusion mutua
        dup2(clon, STDOUT_FILENO);
        close(clon);
        std::cout << "[DUP2 + LOCKS] Redireccion protegida por lock exitosa\n";
    });

    t.join();
    return 0;
}
