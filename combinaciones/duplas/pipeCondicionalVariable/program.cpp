#include <iostream>
#include <unistd.h>
#include <mutex>
#include <condition_variable>
#include <thread>

std::mutex mtx;
std::condition_variable cv;
bool pipeReady = false;

int main() {
    int pfd[2];
    // [TECNICA: PIPE] Creacion de pipe anonimo
    pipe(pfd);

    std::thread productor([&]() {
        // [TECNICA: PIPE] Escritura
        write(pfd[1], "listo_en_pipe", 13);
        close(pfd[1]);
        {
            // [TECNICA: CONDICIONAL VARIABLE] Notificacion del evento
            std::lock_guard<std::mutex> lock(mtx);
            pipeReady = true;
        }
        cv.notify_one();
    });

    std::thread consumidor([&]() {
        std::unique_lock<std::mutex> lock(mtx);
        // [TECNICA: CONDICIONAL VARIABLE] Espera pasiva de disponibilidad
        cv.wait(lock, [] { return pipeReady; });

        // [TECNICA: PIPE] Lectura efectiva
        char buf[32];
        int n = read(pfd[0], buf, sizeof(buf)-1);
        buf[n] = '\0';
        close(pfd[0]);
        std::cout << "[PIPE + COND VAR] Leido tras wait: " << buf << "\n";
    });

    productor.join();
    consumidor.join();
    return 0;
}
