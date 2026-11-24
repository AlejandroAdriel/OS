#include <iostream>
#include <sys/shm.h>
#include <cstring>
#include <mutex>
#include <thread>

std::mutex mtx;

int main() {
    // [TECNICA: SHARED MEMORY] Creacion de memoria compartida
    int shmid = shmget(IPC_PRIVATE, 64, 0666 | IPC_CREAT);
    char *shm = (char *)shmat(shmid, NULL, 0);

    std::thread t([&]() {
        // [TECNICA: LOCKS] Proteccion entre hilos al manipular memoria compartida
        std::lock_guard<std::mutex> lock(mtx);
        std::strcpy(shm, "shm_actualizada_con_lock");
    });

    t.join();
    std::cout << "[LOCKS + SHARED MEMORY] Lectura de shm: " << shm << "\n";

    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
