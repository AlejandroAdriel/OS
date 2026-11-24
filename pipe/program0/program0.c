#include <stdio.h>
#include <unistd.h>
#include <string.h>

// TÉCNICA 3: PIPE (Tubería Anónima en Memoria)
int main(void) {
    int fd[2]; // Arreglo de 2 descriptores de archivo

    // pipe(fd) crea un canal de comunicación unidireccional en RAM:
    //   - fd[0]: Extremo de LECTURA
    //   - fd[1]: Extremo de ESCRITURA
    pipe(fd);

    if (fork() == 0) {
        // PROCESO HIJO (LECTOR)
        close(fd[1]); // Cierra el extremo de escritura porque no lo va a usar

        char buffer[100];
        // read(descriptor, bufer_destino, tamaño) bloquea hasta que haya datos en el pipe
        read(fd[0], buffer, sizeof(buffer));
        printf("[HIJO]  Leído del Pipe: %s\n", buffer);

        close(fd[0]); // Cierra el extremo de lectura al finalizar
    } else {
        // PROCESO PADRE (EMISOR)
        close(fd[0]); // Cierra el extremo de lectura porque no lo va a usar

        char msg[] = "Hola a través del Pipe Anónimo";
        // write(descriptor, datos_a_enviar, cantidad_bytes) manda los datos por el pipe
        write(fd[1], msg, strlen(msg) + 1);

        close(fd[1]); // Cierra escritura (esto manda la señal de Fin de Archivo / EOF al lector)
    }

    return 0;
}
