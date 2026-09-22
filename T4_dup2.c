#include <stdio.h>
#include <unistd.h>
#include <string.h>

// TÉCNICA 4: DUP2 (Redirección de Descriptores de Archivo)
int main(void) {
    int fd[2];
    pipe(fd); // Crear la tubería anónima (fd[0]: lectura, fd[1]: escritura)

    if (fork() == 0) {
        // PROCESO HIJO
        close(fd[1]); // Cierra extremo de escritura

        // dup2(oldfd, newfd): Clona oldfd sobre newfd.
        // dup2(fd[0], STDIN_FILENO): Hace que la Entrada Estándar (descriptor 0) sea el Pipe.
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]); // Cierra el descriptor original ya que fue clonado en STDIN_FILENO

        // Reemplaza la imagen por ./T4_hijo, el cual heredará la entrada estándar modificada
        char *args[] = {"./T4_hijo", NULL};
        execv("./T4_hijo", args);
    } else {
        // PROCESO PADRE
        close(fd[0]); // Cierra extremo de lectura

        char msg[] = "Mensaje enviado al Pipe y leído vía STDIN";
        write(fd[1], msg, strlen(msg) + 1); // Escribe el mensaje en el pipe

        close(fd[1]); // Cierra escritura
    }

    return 0;
}
