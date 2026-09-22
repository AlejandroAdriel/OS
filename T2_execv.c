#include <stdio.h>
#include <unistd.h>

// TÉCNICA 2: EXECV (Reemplazo de la Imagen de Memoria)
int main(void) {
    printf("[PROCESO INICIAL] Reemplazando imagen de memoria por ./T2_hijo...\n");

    // Arreglo de argumentos que se pasarán al nuevo programa (debe terminar en NULL)
    char *args[] = {"./T2_hijo", NULL};

    // execv(ruta_ejecutable, matriz_argumentos)
    // Borra el código actual de la memoria y carga el binario './T2_hijo'
    execv("./T2_hijo", args);

    // Si execv() tiene éxito, las líneas de abajo NUNCA se ejecutan porque el código cambió
    perror("Error al ejecutar execv");
    return 1;
}
