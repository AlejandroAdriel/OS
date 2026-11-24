#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <descriptor_lectura_pipe>\n", argv[0]);
        return 1;
    }

    // Convertir el argumento string al número entero del descriptor
    int pipe_read_fd = atoi(argv[1]);

    printf("[PROCESO C] Ejecutable independiente iniciado via execv.\n");
    printf("[PROCESO C] PID: %d | Canal de recepción (Descriptor Pipe): %d\n", getpid(), pipe_read_fd);
    printf("[PROCESO C] En escucha de mensajes transmitidos por Proceso A...\n\n");

    char buffer[256];

    // Escucha continua de la tubería (Pipe)
    while (1) {
        ssize_t n = read(pipe_read_fd, buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0';
            printf("[PROCESO C] (PID %d) Mensaje recibido vía PIPE (fd %d):\n    -> %s", 
                   getpid(), pipe_read_fd, buffer);
        } else if (n == 0) {
            printf("[PROCESO C] Tubería cerrada por Proceso A. Finalizando.\n");
            break;
        } else {
            perror("[PROCESO C] Error al leer de la tubería");
            break;
        }
    }

    close(pipe_read_fd);
    return 0;
}
