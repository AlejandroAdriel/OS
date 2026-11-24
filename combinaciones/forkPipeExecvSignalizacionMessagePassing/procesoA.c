#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

// Estructura para la Cola de Mensajes System V
struct msg_buffer {
    long msg_type;
    char msg_text[128];
};

// Variables globales para ser accedidas desde las funciones manejadoras de señales
int pipe_fd_write = -1;
int msg_queue_id = -1;

// Manejador para la Señal 2 (SIGINT) -> Transmite al Proceso B vía Cola de Mensajes
void manejador_sigint(int sig) {
    struct msg_buffer msg;
    msg.msg_type = 1;
    snprintf(msg.msg_text, sizeof(msg.msg_text), 
             "Hola Proceso B! Transmitido mediante Cola de Mensajes (Señal %d - SIGINT recibida en A)", sig);

    if (msgsnd(msg_queue_id, &msg, sizeof(msg.msg_text), 0) == -1) {
        perror("[PROCESO A] Error enviando mensaje a la Cola");
    } else {
        printf("\n[PROCESO A] Capturada Señal %d (SIGINT). Mensaje enviado a la Cola (Proceso B).\n", sig);
    }
}

// Manejador para la Señal 5 (SIGTRAP) -> Transmite al Proceso C vía Tubería (Pipe)
void manejador_sigtrap(int sig) {
    char mensaje[128];
    snprintf(mensaje, sizeof(mensaje), 
             "Hola Proceso C! Transmitido mediante Tubería (Señal %d - SIGTRAP recibida en A)\n", sig);

    if (write(pipe_fd_write, mensaje, strlen(mensaje)) == -1) {
        perror("[PROCESO A] Error escribiendo en la Tubería");
    } else {
        printf("\n[PROCESO A] Capturada Señal %d (SIGTRAP). Mensaje enviado por Pipe (Proceso C).\n", sig);
    }
}

int main(void) {
    int fd[2];

    // 1. Inicializar Tubería (Pipe)
    if (pipe(fd) == -1) {
        perror("Error creando pipe");
        exit(1);
    }

    // 2. Inicializar Cola de Mensajes (System V)
    msg_queue_id = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    if (msg_queue_id == -1) {
        perror("Error creando Cola de Mensajes");
        exit(1);
    }

    // 3. Registrar los manejadores para las señales SIGINT (2) y SIGTRAP (5)
    signal(SIGINT, manejador_sigint);
    signal(SIGTRAP, manejador_sigtrap);

    // 4. Crear Proceso B mediante fork()
    pid_t pid_b = fork();

    if (pid_b < 0) {
        perror("Error en fork de Proceso B");
        exit(1);
    }

    if (pid_b == 0) {
        // PROCESO B (Hijo de A)
        
        // Clonar inmediatamente creando al "Clon de B"
        pid_t pid_clon = fork();

        if (pid_clon < 0) {
            perror("[PROCESO B] Error en fork de Clon de B");
            exit(1);
        }

        if (pid_clon == 0) {
            // CLON DE B (Hijo de B) -> Se transformará en PROCESO C
            close(fd[1]); // Cierra el extremo de escritura en el clon

            // Convertir descriptor de lectura del pipe a texto
            char descriptor_str[20];
            snprintf(descriptor_str, sizeof(descriptor_str), "%d", fd[0]);

            // Reemplazar imagen de memoria con ./proceso_c
            char *args[] = {"./proceso_c", descriptor_str, NULL};
            execv("./proceso_c", args);

            perror("[CLON DE B] Error al ejecutar execv");
            exit(1);
        }

        // PROCESO B ORIGINAL -> Escucha la Cola de Mensajes
        close(fd[0]); // Proceso B no usa el pipe
        close(fd[1]);

        printf("[PROCESO B] PID: %d | Escuchando mensajes en la Cola IPC (ID: %d)...\n", 
               getpid(), msg_queue_id);

        struct msg_buffer msg;
        while (1) {
            // Espera bloqueante a recibir un mensaje del tipo 1
            if (msgrcv(msg_queue_id, &msg, sizeof(msg.msg_text), 1, 0) != -1) {
                printf("[PROCESO B] (PID %d) Mensaje recibido vía COLA DE MENSAJES:\n    -> %s\n", 
                       getpid(), msg.msg_text);
            }
        }

        exit(0);
    }

    // PROCESO A (Padre Principal)
    close(fd[0]); // Cierra lectura del pipe en el padre
    pipe_fd_write = fd[1]; // Conserva escritura para el manejador de SIGTRAP


    printf("  PROCESO A (PADRE PRINCIPAL)\n");

    printf("  PID Proceso A: %d\n", getpid());
    printf("  PID Proceso B: %d\n", pid_b);

    printf(" Instrucciones para enviar señales desde otra terminal:\n");
    printf("   kill -2 %d    --> Envía SIGINT (2)   -> Mensaje vía COLA -> Proceso B\n", getpid());
    printf("   kill -5 %d    --> Envía SIGTRAP (5)  -> Mensaje vía PIPE -> Proceso C\n", getpid());


    // Mantener al Proceso A en espera pasiva de señales
    while (1) {
        pause();
    }

    // Limpieza de recursos (si llegara a salir)
    msgctl(msg_queue_id, IPC_RMID, NULL);
    return 0;
}
