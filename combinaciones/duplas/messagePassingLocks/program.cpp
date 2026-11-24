#include <iostream>
#include <unistd.h>
#include <sys/msg.h>
#include <mutex>
#include <thread>

struct my_msgbuf {
    long mtype;
    char mtext[32];
};

std::mutex mtx;

int main() {
    // [TECNICA: MESSAGE PASSING] Creacion de cola
    int msqid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

    std::thread emisor([&]() {
        // [TECNICA: LOCKS] Proteccion del acceso a la cola IPC
        std::lock_guard<std::mutex> lock(mtx);
        struct my_msgbuf msg = {1, "msg_con_lock"};
        // [TECNICA: MESSAGE PASSING] Envio
        msgsnd(msqid, &msg, sizeof(msg.mtext), 0);
    });

    emisor.join();

    struct my_msgbuf rec;
    // [TECNICA: MESSAGE PASSING] Recepcion
    msgrcv(msqid, &rec, sizeof(rec.mtext), 1, 0);
    std::cout << "[MESSAGE PASSING + LOCKS] Recibido: " << rec.mtext << "\n";

    msgctl(msqid, IPC_RMID, NULL);
    return 0;
}
