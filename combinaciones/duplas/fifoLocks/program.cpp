#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <mutex>
#include <thread>

#define FIFO_PATH "/tmp/dupla_fifo_locks"
std::mutex mtx;

int main() {
    unlink(FIFO_PATH);
    // [TECNICA: FIFO] Creacion de FIFO
    mkfifo(FIFO_PATH, 0666);

    std::thread t([]() {
        // [TECNICA: LOCKS] Proteccion de acceso concurrente a la FIFO
        std::lock_guard<std::mutex> lock(mtx);
        int fd = open(FIFO_PATH, O_WRONLY);
        write(fd, "lock_protege_fifo", 17);
        close(fd);
    });

    int fd = open(FIFO_PATH, O_RDONLY);
    char buf[32];
    int n = read(fd, buf, sizeof(buf)-1);
    buf[n] = '\0';
    close(fd);

    t.join();
    unlink(FIFO_PATH);
    std::cout << "[FIFO + LOCKS] Leido de FIFO protegida: " << buf << "\n";
    return 0;
}
