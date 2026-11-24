#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    int fd[2];
    pipe(fd);

    pid_t pid = fork();

    if (pid == 0) {
        // HIJO: ejecutará programa2

        close(fd[1]); //cierra el descriptor de escritura en el hijo
        dup2(fd[0], STDIN_FILENO); //duplica el descriptor de lectura en la entrada estandar
        close(fd[0]);

        char *args[] = {"./programa2.exe", NULL};

        execv("./programa2.exe", args);

        perror("execv");
        exit(1);
    }

    // PADRE
    close(fd[0]);

    char mensaje[] = "Hola desde el proceso padre\n";
    write(fd[1], mensaje, sizeof(mensaje) - 1);

    close(fd[1]);

    wait(NULL);

    return 0;
}