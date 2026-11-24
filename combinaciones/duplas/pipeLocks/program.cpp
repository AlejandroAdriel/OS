#include <iostream>
#include <unistd.h>
#include <mutex>
#include <thread>

std::mutex mtx;

int main() {
    int pfd[2];
    // [TECNICA: PIPE] Creacion de pipe anonimo
    pipe(pfd);

    std::thread t([&]() {
        // [TECNICA: LOCKS] Proteccion de seccion critica al escribir en el pipe
        std::lock_guard<std::mutex> lock(mtx);
        write(pfd[1], "msg_hilo_seguro", 15);
        close(pfd[1]);
    });

    t.join();

    char buf[32];
    // [TECNICA: PIPE] Lectura de pipe
    int n = read(pfd[0], buf, sizeof(buf)-1);
    buf[n] = '\0';
    close(pfd[0]);

    std::cout << "[PIPE + LOCKS] Leido del pipe protegido por lock: " << buf << "\n";
    return 0;
}
