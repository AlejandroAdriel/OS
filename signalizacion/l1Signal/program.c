#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>

static sigjmp_buf salto_sigint;
static sigjmp_buf salto_sigusr1;
static sigjmp_buf salto_sigusr2;

void handler_sigint(int signo)
{
    printf("\nSe recibió la señal %d SIGINT\n", signo);
    siglongjmp(salto_sigint, 1);
}

void handler_sigusr1(int signo)
{
    printf("\nSe recibió la señal %d SIGUSR1\n", signo);
    siglongjmp(salto_sigusr1, 1);
}

void handler_sigusr2(int signo)
{
    printf("\nSe recibió la señal %d SIGUSR2\n", signo);
    siglongjmp(salto_sigusr2, 1);
}

int main(void)
{
    signal(SIGINT, handler_sigint);
    signal(SIGUSR1, handler_sigusr1);
    signal(SIGUSR2, handler_sigusr2);

    if (sigsetjmp(salto_sigint, 1) != 0) {
        printf("¡Se realizó el salto de código SIGINT!\n");
        printf("Continuando desde el punto de recuperación...\n");
    }

    if (sigsetjmp(salto_sigusr1, 1) != 0) {
        printf("¡Se realizó el salto de código SIGUSR1!\n");
        printf("Continuando desde el punto de recuperación...\n");
    }

    if (sigsetjmp(salto_sigusr2, 1) != 0) {
        printf("¡Se realizó el salto de código SIGUSR2!\n");
        printf("Continuando desde el punto de recuperación...\n");
    }

    printf("PID: %d\n", getpid());
    printf("Programa iniciado.\n");
    printf("Pulsa Ctrl+C para enviar SIGINT.\n");
    printf("Desde terminal: kill -10 %d para enviar SIGUSR1\n", getpid());
    printf("Desde terminal: kill -12 %d para enviar SIGUSR2\n", getpid());


    while (1) {
        printf("Ejecutando código normal...\n");
        sleep(2);
    }

    printf("Fin del programa.\n");

    return 0;
}
