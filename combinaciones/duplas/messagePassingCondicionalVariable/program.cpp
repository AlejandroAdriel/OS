#include <iostream>
#include <unistd.h>
#include <sys/msg.h>
#include <mutex>
#include <condition_variable>
#include <thread>

struct my_msgbuf {
    long mtype;
    char mtext[32];
};

std::mutex mtx;
std::condition_variable cv;
bool msgReady = false;

int main() {
    // [TECNICA: MESSAGE PASSING] Creacion de cola
    int msqid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

    std::thread productor([&]() {
        struct my_msgbuf msg = {1, "cola_y_condvar"};
        // [TECNICA: MESSAGE PASSING] Envio
        msgsnd(msqid, &msg, sizeof(msg.mtext), 0);
        {
            // [TECNICA: CONDICIONAL VARIABLE] Notificacion de mensaje disponible
            std::lock_guard<std::mutex> lock(mtx);
            msgReady = true;
        }
        cv.notify_one();
    });

    std::thread consumidor([&]() {
        std::unique_lock<std::mutex> lock(mtx);
        // [TECNICA: CONDICIONAL VARIABLE] Espera de notificacion antes de leer
        cv.wait(lock, [] { return msgReady; });

        struct my_msgbuf rec;
        // [TECNICA: MESSAGE PASSING] Recepcion
        msgrcv(msqid, &rec, sizeof(rec.mtext), 1, 0);
        std::cout << "[MESSAGE PASSING + COND VAR] Extraido de cola tras notificacion: " << rec.mtext << "\n";
    });

    productor.join();
    consumidor.join();
    msgctl(msqid, IPC_RMID, NULL);
    return 0;
}
