#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

// Ruta del archivo especial FIFO en el sistema de archivos
#define FIFO_PATH "/tmp/demo_fifo"

// TÉCNICA 7: FIFO (Tubería Nombrada en Disco)
int main(void) {
    unlink(FIFO_PATH); // Limpia por si existía de una ejecución previa

    // mkfifo(ruta, permisos): Crea un archivo especial de tubería nombrada en disco.
    mkfifo(FIFO_PATH, 0666);

    if (fork() == 0) {
        // PROCESO HIJO (LECTOR)
        // open() en O_RDONLY bloquea hasta que otro proceso abra la FIFO en modo escritura
        int fd = open(FIFO_PATH, O_RDONLY);

        char buffer[100];
        read(fd, buffer, sizeof(buffer)); // Lee los datos de la FIFO
        printf("[HIJO]  Leído de la FIFO nombrada: %s\n", buffer);

        close(fd); // Cierra el archivo FIFO
    } else {
        // PROCESO PADRE (ESCRITOR)
        // open() en O_WRONLY abre la FIFO para enviar datos
        int fd = open(FIFO_PATH, O_WRONLY);

        char msg[] = "Hola desde Tubería Nombrada FIFO en disco";
        write(fd, msg, strlen(msg) + 1); // Escribe en el archivo FIFO

        close(fd);
        unlink(FIFO_PATH); // Elimina el archivo FIFO del sistema de archivos
    }

    return 0;
}
