#include <stdio.h>
#include <unistd.h>

// Programa secundario ejecutable que será cargado en memoria por T2_execv
int main(void) {
    // getpid() demuestra que conserva el mismo PID del proceso que lo invocó
    printf("[NUEVO PROGRAMA] ¡Imagen reemplazada exitosamente vía execv! PID: %d\n", getpid());
    return 0;
}
