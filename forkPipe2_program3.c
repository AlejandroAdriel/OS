#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{

    if (argc != 2) {
        fprintf(stderr, "Uso: %s descriptor\n", argv[0]); // argc es el numero de argumentos, argv es el array de argumentos argv[0] es el nombre del programa
        return 1;
    }

    int fd = atoi(argv[1]); // atoi convierte la cadena a entero
    printf("descriptor en el hijo (ARG): %d\n", fd); 

    //////////////////////
    int flags = fcntl(fd, F_GETFD); // fcntl obtiene el descriptor de archivo

    if (flags == -1) {  // si fcntl falla
        perror("fcntl");
    }
    else if (flags & FD_CLOEXEC) { // FD_CLOEXEC es una bandera que se le puede dar a un descriptor de archivo
        printf("FD_CLOEXEC está ACTIVADO\n");
    }
    else { 
        printf("FD_CLOEXEC está DESACTIVADO\n");
    }
    //////////////////////////////////


    char buffer[100];
    ssize_t n;

    n = read(fd, buffer, sizeof(buffer) - 1); // lee el descriptor de archivo
    printf("n:%d\n",(int)n); //imprime la cantidad de bytes leidos
    if (n > 0) { // si se leyo algo
        buffer[n] = '\0'; // agrega el caracter nulo
        printf("Recibido: %s", buffer); // imprime lo recibido
    }

    close(fd);

    return 0;
}