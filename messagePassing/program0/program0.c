#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// Estructura requerida por System V Message Passing:
// Debe tener un 'long mtype' al inicio, seguido por los datos del mensaje.
struct msgbuf {
    long mtype;
    char mtext[100];
};

// TÉCNICA 6: MESSAGE PASSING (Cola de Mensajes System V)
int main(void) {
    // msgget(llave, permisos_y_flags): Crea o accede a una cola de mensajes IPC.
    // IPC_PRIVATE genera un ID de cola privado de uso exclusivo entre procesos emparentados.
    int msqid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

    if (fork() == 0) {
        // PROCESO HIJO (RECEPTOR)
        struct msgbuf buf;

        // msgrcv(id_cola, &buffer, tamaño_max_datos, tipo_mensaje, flags):
        // Lee bloqueando hasta que aparezca un mensaje de tipo 1 en la cola IPC.
        msgrcv(msqid, &buf, sizeof(buf.mtext), 1, 0);
        printf("[HIJO]  Recibido de Cola IPC: %s\n", buf.mtext);
    } else {
        // PROCESO PADRE (EMISOR)
        struct msgbuf buf = {1, "Hola desde la Cola de Mensajes IPC"};

        // msgsnd(id_cola, &buffer, tamaño_datos, flags):
        // Coloca el mensaje struct en la cola compartida IPC.
        msgsnd(msqid, &buf, strlen(buf.mtext) + 1, 0);

        sleep(1); // Espera a que el hijo procese el mensaje
        msgctl(msqid, IPC_RMID, NULL); // Destruye la cola de mensajes del kernel
    }

    return 0;
}
