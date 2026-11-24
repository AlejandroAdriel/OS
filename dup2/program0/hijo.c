#include <stdio.h>
#include <unistd.h>

// Programa receptor secundario para la prueba de dup2
int main(void) {
    char buffer[100];

    // STDIN_FILENO representa la entrada estándar (descriptor 0, normalmente el teclado).
    // Gracias a dup2() realizado antes de execv, al leer de STDIN_FILENO en realidad lee del Pipe.
    read(STDIN_FILENO, buffer, sizeof(buffer));
    printf("[NUEVO PROGRAMA] Leído desde STDIN_FILENO (redireccionado por dup2): %s\n", buffer);

    return 0;
}
