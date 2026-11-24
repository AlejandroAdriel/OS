#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {
    int dev_null = open("/dev/null", O_WRONLY);

    // [TECNICA: DUP2] Redireccion de stdout antes de llamar a execv
    dup2(dev_null, STDOUT_FILENO);
    close(dev_null);

    char *args[] = {"./hijo", NULL};
    // [TECNICA: EXECV] Reemplazo de memoria heredando la redireccion
    execv("./hijo", args);
    return 0;
}
