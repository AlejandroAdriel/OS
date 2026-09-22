#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#define FIFO_PATH "/tmp/p2_fifo"

// Estructura para la Cola de Mensajes System V (Message Passing)
struct msg_buffer {
    long msg_type;
    char msg_text[128];
};

// Variables globales para ser accedidas desde las funciones de señal
int pipe_fd_write = -1;
int msg_queue_id = -1;

// [TÉCNICA 2: SIGNALIZACIÓN] - Manejador para SIGINT (Señal 2)
void manejador_sigint(int sig) {
    struct msg_buffer msg;
    msg.msg_type = 1;
    snprintf(msg.msg_text, sizeof(msg.msg_text), 
             "Mensaje por COLA (SIGINT recibida en Proceso Principal)");

    // [TÉCNICA 3: MESSAGE PASSING]
    if (msgsnd(msg_queue_id, &msg, sizeof(msg.msg_text), 0) == -1) {
        perror("[PADRE] Error enviando mensaje a la Cola");
    } else {
        printf("\n[PADRE] Señal 2 (SIGINT) enviada a la Cola de Mensajes (para Hijo Worker).\n");
    }
}

// [TÉCNICA 2: SIGNALIZACIÓN] - Manejador para SIGUSR1 (Señal 10)
void manejador_sigusr1(int sig) {
    char mensaje[128];
    snprintf(mensaje, sizeof(mensaje), 
             "Mensaje por PIPE (SIGUSR1 recibida en Proceso Principal)\n");

    // [TÉCNICA 5: PIPE - ESCRITURA]
    if (write(pipe_fd_write, mensaje, strlen(mensaje)) == -1) {
        perror("[PADRE] Error escribiendo en Pipe");
    } else {
        printf("\n[PADRE] Señal 10 (SIGUSR1) enviada por Pipe (para P2_relector).\n");
    }
}

int main(void) {
    int pipe_fd[2];

    // -------------------------------------------------------------------------
    // [TÉCNICA 4: FIFO / NAMED PIPE] - Crear tubería con nombre en disco
    // -------------------------------------------------------------------------
    unlink(FIFO_PATH); // Limpiar si existía previamente
    if (mkfifo(FIFO_PATH, 0666) == -1) {
        perror("Error creando FIFO");
        exit(1);
    }

    // -------------------------------------------------------------------------
    // [TÉCNICA 5: PIPE ANÓNIMO] - Crear tubería anónima
    // -------------------------------------------------------------------------
    if (pipe(pipe_fd) == -1) {
        perror("Error creando Pipe");
        exit(1);
    }

    // -------------------------------------------------------------------------
    // [TÉCNICA 3: MESSAGE PASSING] - Crear Cola de Mensajes System V
    // -------------------------------------------------------------------------
    msg_queue_id = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    if (msg_queue_id == -1) {
        perror("Error creando Cola de Mensajes");
        exit(1);
    }

    // -------------------------------------------------------------------------
    // [TÉCNICA 2: SIGNALIZACIÓN] - Registrar manejadores de señales
    // -------------------------------------------------------------------------
    signal(SIGINT, manejador_sigint);
    signal(SIGUSR1, manejador_sigusr1);

    // =========================================================================
    // [TÉCNICA 1: FORK #1] - Crear Hijo Worker (escucha Cola y escribe en FIFO)
    // =========================================================================
    pid_t pid_worker = fork();

    if (pid_worker == 0) {
        // HIJO WORKER
        close(pipe_fd[0]);
        close(pipe_fd[1]);

        printf("[HIJO WORKER] PID: %d | Escuchando en Cola de Mensajes...\n", getpid());

        struct msg_buffer msg;
        while (1) {
            // Recibir de la Cola de Mensajes
            if (msgrcv(msg_queue_id, &msg, sizeof(msg.msg_text), 1, 0) != -1) {
                printf("[HIJO WORKER] (PID %d) Recibió de Cola: '%s'\n", getpid(), msg.msg_text);

                // Responder abriendo y escribiendo en la FIFO (Named Pipe)
                int fifo_fd = open(FIFO_PATH, O_WRONLY);
                if (fifo_fd != -1) {
                    char respuesta_fifo[256];
                    snprintf(respuesta_fifo, sizeof(respuesta_fifo), 
                             "Reporte de Worker (PID %d) enviado por FIFO tras recibir orden.\n", getpid());
                    write(fifo_fd, respuesta_fifo, strlen(respuesta_fifo));
                    close(fifo_fd);
                    printf("[HIJO WORKER] Reporte enviado exitosamente a la FIFO (%s).\n", FIFO_PATH);
                }
            }
        }
        exit(0);
    }

    // =========================================================================
    // [TÉCNICA 1: FORK #2] - Crear Hijo Exec (aplica DUP2 y EXECV)
    // =========================================================================
    pid_t pid_exec = fork();

    if (pid_exec == 0) {
        // HIJO EXEC
        close(pipe_fd[1]); // Cierra extremo de escritura

        // ---------------------------------------------------------------------
        // [TÉCNICA 6: DUP2] - Redirigir la entrada estándar (STDIN_FILENO) al Pipe
        // ---------------------------------------------------------------------
        dup2(pipe_fd[0], STDIN_FILENO);
        close(pipe_fd[0]); // Cierra el descriptor original ya duplicado

        // ---------------------------------------------------------------------
        // [TÉCNICA 7: EXECV] - Reemplazar la imagen del proceso por P2_relector
        // ---------------------------------------------------------------------
        char *args[] = {"./P2_relector", NULL};
        execv("./P2_relector", args);

        perror("[HIJO EXEC] Error en execv");
        exit(1);
    }

    // =========================================================================
    // PROCESO PRINCIPAL (PADRE)
    // =========================================================================
    close(pipe_fd[0]); // Cierra lectura del Pipe
    pipe_fd_write = pipe_fd[1]; // Almacena descriptor de escritura para el manejador SIGUSR1

    printf("===================================================================\n");
    printf("     DEMOSTRACIÓN P2: SISTEMA POSIX COMPLETO (7 TÉCNICAS)\n");
    printf("===================================================================\n");
    printf("  PID Proceso Principal (Padre): %d\n", getpid());
    printf("  PID Hijo Worker:                %d\n", pid_worker);
    printf("  PID Hijo Exec (P2_relector):    %d\n", pid_exec);
    printf("-------------------------------------------------------------------\n");
    printf(" Para enviar señales desde otra terminal utiliza:\n");
    printf("   kill -2  %d   --> (SIGINT)  -> Cola -> Worker -> FIFO -> Padre\n", getpid());
    printf("   kill -10 %d   --> (SIGUSR1) -> Pipe -> Dup2 -> Execv (P2_relector)\n", getpid());
    printf("===================================================================\n\n");

    // Abrir FIFO en modo lectura no bloqueante o bloqueante en un hilo/bucle paralelo
    // Para no bloquear pause(), abrimos la FIFO en modo lectura/escritura para mantener el FD abierto
    int fifo_rd = open(FIFO_PATH, O_RDWR);

    char buf_fifo[256];
    while (1) {
        // En cada iteración lee de la FIFO si hay datos disponibles
        ssize_t n = read(fifo_rd, buf_fifo, sizeof(buf_fifo) - 1);
        if (n > 0) {
            buf_fifo[n] = '\0';
            printf("[PADRE] Notificación recibida desde la FIFO (%s):\n    -> %s", 
                   FIFO_PATH, buf_fifo);
        }
        sleep(1);
    }

    // Limpieza
    close(fifo_rd);
    unlink(FIFO_PATH);
    msgctl(msg_queue_id, IPC_RMID, NULL);
    return 0;
}
