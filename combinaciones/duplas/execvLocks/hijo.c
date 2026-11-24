#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("[PROGRAMA HIJO EXECV] Ejecutando nuevo binario con PID: %d\n", getpid());
    return 0;
}
