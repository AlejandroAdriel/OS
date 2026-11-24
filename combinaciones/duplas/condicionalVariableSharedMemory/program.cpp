#include <iostream>
#include <sys/shm.h>
#include <cstring>
#include <mutex>
#include <condition_variable>
#include <thread>

std::mutex mtx;
std::condition_variable cv;
bool shmWritten = false;

int main() {
    // [TECNICA: SHARED MEMORY] Creacion de segmento compartido
    int shmid = shmget(IPC_PRIVATE, 64, 0666 | IPC_CREAT);
    char *shm = (char *)shmat(shmid, NULL, 0);

    std::thread escritor([&]() {
        // [TECNICA: SHARED MEMORY] Escritura en memoria compartida
        std::strcpy(shm, "shm_lista_para_lectura");
        {
            // [TECNICA: CONDICIONAL VARIABLE] Notificacion de que el dato en shm esta listo
            std::lock_guard<std::mutex> lock(mtx);
            shmWritten = true;
        }
        cv.notify_one();
    });

    std::thread lector([&]() {
        std::unique_lock<std::mutex> lock(mtx);
        // [TECNICA: CONDICIONAL VARIABLE] Espera de notificacion antes de leer la shm
        cv.wait(lock, [] { return shmWritten; });
        // [TECNICA: SHARED MEMORY] Lectura segura
        std::cout << "[COND VAR + SHARED MEMORY] Leido de shm sincronizada: " << shm << "\n";
    });

    escritor.join();
    lector.join();
    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
