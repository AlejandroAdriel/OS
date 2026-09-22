#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// =========================================================================
// PASO 7: EXECV (Ejecutable Final)
// Recibe los datos leídos desde STDIN_FILENO (redireccionado desde el Pipe por dup2)
// e imprime el reporte final procesado por la cadena de 7 técnicas.
// =========================================================================
int main(void) {
    printf("[P4_FINAL] Ejecutable independiente cargado vía execv (PID: %d).\n", getpid());
    printf("[P4_FINAL] Leyendo datos acumulados desde STDIN_FILENO...\n\n");

    char buffer[1024];
    ssize_t n = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);

    if (n > 0) {
        buffer[n] = '\0';
        printf("===================================================================\n");
        printf("   REPORTE SERVIDOR P4: PROCESADO POR CADENA DOMINÓ (7 TÉCNICAS)\n");
        printf("===================================================================\n");
        printf("%s\n", buffer);
        printf("===================================================================\n\n");
    } else {
        perror("[P4_FINAL] Error leyendo de STDIN");
    }

    return 0;
}
