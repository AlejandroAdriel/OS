#include <stdio.h>
#include <unistd.h>
#include <signal.h>

// Función manejadora que se ejecutará cuando el sistema operativo capture la señal
void mi_manejador(int sig) {
    printf("\n[MANEJADOR] ¡Señal %d (SIGINT / Ctrl+C) capturada exitosamente!\n", sig);
}

// TÉCNICA 5: SIGNAL (Manejo de Señales del Sistema Operativo)
int main(void) {
    // signal(NUM_SEÑAL, FUNCION_MANEJADORA): Registra la captura de la señal.
    // SIGINT es la señal número 2 (enviada con Ctrl+C o kill -2 PID).
    signal(SIGINT, mi_manejador);

    printf("[PROCESO] PID: %d. En otra terminal ejecuta: kill -2 %d\n", getpid(), getpid());
    printf("[PROCESO] Esperando señal...\n");

    // pause() suspende la ejecución del proceso de forma pasiva (0% CPU) hasta recibir una señal
    pause();

    printf("[PROCESO] Proceso finalizado tras procesar la señal.\n");
    return 0;
}
