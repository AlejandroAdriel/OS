#include <stdio.h>
#include <unistd.h>

// TÉCNICA 1: FORK (Duplicación de Procesos)
int main(void) {
    // fork() clona el proceso actual creando un proceso hijo idéntico.
    // Retorna:
    //   - 0 al proceso HIJO.
    //   - El PID del hijo creado (valor > 0) al proceso PADRE.
    //   - Un número negativo (< 0) si ocurrió un error al clonar.
    pid_t pid = fork();

    if (pid == 0) {
        // Este bloque solo es ejecutado por el PROCESO HIJO
        // getpid(): Obtiene el PID propio del hijo
        // getppid(): Obtiene el PID del Padre (Parent PID)
        printf("[HIJO]  Mi PID: %d | PID de mi Padre: %d\n", getpid(), getppid());
    } else {
        // Este bloque solo es ejecutado por el PROCESO PADRE
        // La variable 'pid' contiene el ID del hijo que acaba de crear
        printf("[PADRE] Mi PID: %d | PID de mi Hijo: %d\n", getpid(), pid);
    }

    return 0;
}
