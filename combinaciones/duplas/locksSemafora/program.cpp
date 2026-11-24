#include <iostream>
#include <sys/sem.h>
#include <mutex>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

std::mutex mtx;

int main() {
    // [TECNICA: SEMAFORA] Creacion de semaforo System V
    int semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    union semun arg;
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);

    struct sembuf p = {0, -1, 0};
    struct sembuf v = {0, 1, 0};

    // [TECNICA: LOCKS] Lock a nivel de hilo
    {
        std::lock_guard<std::mutex> lock(mtx);
        // [TECNICA: SEMAFORA] Semaforo a nivel de proceso
        semop(semid, &p, 1);
        std::cout << "[LOCKS + SEMAFORA] Sincronizacion de dos capas (Mutex C++ + Semafaro POSIX)\n";
        semop(semid, &v, 1);
    }

    semctl(semid, 0, IPC_RMID, arg);
    return 0;
}
