#include <iostream>
#include <sys/sem.h>
#include <mutex>
#include <condition_variable>
#include <thread>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

std::mutex mtx;
std::condition_variable cv;
bool ready = false;

int main() {
    // [TECNICA: SEMAFORA] Creacion de semaforo System V
    int semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    union semun arg;
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);

    std::thread t([&]() {
        struct sembuf p = {0, -1, 0};
        // [TECNICA: SEMAFORA] Operacion P
        semop(semid, &p, 1);
        {
            // [TECNICA: CONDICIONAL VARIABLE] Notificacion al hilo principal
            std::lock_guard<std::mutex> lock(mtx);
            ready = true;
        }
        cv.notify_one();
        struct sembuf v = {0, 1, 0};
        // [TECNICA: SEMAFORA] Operacion V
        semop(semid, &v, 1);
    });

    {
        std::unique_lock<std::mutex> lock(mtx);
        // [TECNICA: CONDICIONAL VARIABLE] Espera pasiva de que el hilo complete la operacion
        cv.wait(lock, [] { return ready; });
    }
    t.join();

    std::cout << "[COND VAR + SEMAFORA] Coordinacion combinada completada\n";
    semctl(semid, 0, IPC_RMID, arg);
    return 0;
}
