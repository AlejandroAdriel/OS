#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <mutex>
#include <condition_variable>
#include <thread>

#define FIFO_PATH "/tmp/dupla_fifo_cv"
std::mutex mtx;
std::condition_variable cv;
bool fifoReady = false;

int main() {
    unlink(FIFO_PATH);
    // [TECNICA: FIFO] Creacion de FIFO
    mkfifo(FIFO_PATH, 0666);

    std::thread escritor([]() {
        int fd = open(FIFO_PATH, O_WRONLY);
        write(fd, "datos_notificados", 17);
        close(fd);
        {
            // [TECNICA: CONDICIONAL VARIABLE] Notificacion de disponibilidad
            std::lock_guard<std::mutex> lock(mtx);
            fifoReady = true;
        }
        cv.notify_one();
    });

    std::thread lector([]() {
        std::unique_lock<std::mutex> lock(mtx);
        // [TECNICA: CONDICIONAL VARIABLE] Espera activa a que se termine la escritura
        cv.wait(lock, [] { return fifoReady; });

        // [TECNICA: FIFO] Lectura segura
        int fd = open(FIFO_PATH, O_RDONLY);
        char buf[32];
        int n = read(fd, buf, sizeof(buf)-1);
        buf[n] = '\0';
        close(fd);
        std::cout << "[FIFO + COND VAR] Leido tras notify: " << buf << "\n";
    });

    escritor.join();
    lector.join();
    unlink(FIFO_PATH);
    return 0;
}
