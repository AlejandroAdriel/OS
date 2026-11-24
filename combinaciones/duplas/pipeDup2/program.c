#include <stdio.h>
#include <unistd.h>

int main(void) {
    int pfd[2];
    // [TECNICA: PIPE] Creacion de tuberia anonima
    pipe(pfd);

    // [TECNICA: DUP2] Clonacion del extremo de lectura del pipe sobre STDIN
    dup2(pfd[0], STDIN_FILENO);
    close(pfd[0]);

    // [TECNICA: PIPE] Escritura al pipe
    write(pfd[1], "pipe_a_stdin", 12);
    close(pfd[1]);

    char buf[32];
    // Lectura normal desde STDIN consume los datos del Pipe gracias a dup2
    int n = read(STDIN_FILENO, buf, sizeof(buf)-1);
    buf[n] = '\0';
    printf("[PIPE + DUP2] Leido desde STDIN redireccionado al Pipe: %s\n", buf);
    return 0;
}
