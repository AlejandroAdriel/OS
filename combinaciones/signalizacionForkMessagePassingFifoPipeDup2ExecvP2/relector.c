#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Programa independiente que es invocado mediante execv.
// Gracias al dup2() realizado en el hijo antes del execv, su STDIN_FILENO (entrada estándar)
// está conectada directamente al extremo de lectura de la Tubería Anónima (Pipe).
int main(void) {
    printf("[P2_RELECTOR] Ejecutable independiente cargado vía execv.\n");
    printf("[P2_RELECTOR] PID: %d | Escuchando en STDIN_FILENO (conectado al Pipe vía dup2)...\n\n", getpid());

    char buffer[256];
    ssize_t n;

    while (1) {
        n = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0';
            printf("[P2_RELECTOR] (PID %d) Recibido desde Pipe a través de STDIN:\n    -> %s", 
                   getpid(), buffer);
        } else if (n == 0) {
            printf("[P2_RELECTOR] Tubería cerrada. Finalizando programa relector.\n");
            break;
        } else {
            perror("[P2_RELECTOR] Error al leer de STDIN");
            break;
        }
    }

    return 0;
}
