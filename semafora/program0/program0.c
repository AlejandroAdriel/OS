/*

ucsp@ucsp-b302:~/Downloads/lab$ ./sc.exe c "chau"
sem:2
ucsp@ucsp-b302:~/Downloads/lab$ ./sc.exe g "caooau"




ipcs


borrar todas las semaforas

ipcrm -a


variable condicional entre procesos
si se cumple la condicion continuas sino sueltas el lock hasta que toque

*/



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define KEY 0x1111

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

union semun u;

//para hacer las opercaciones .p() y .v()
struct sembuf p = {0, -1, 0};
struct sembuf v = {0, +1, 0};

void semP(int id)
{
    if (semop(id, &p, 1) < 0) //semop para configurar una operacion, el 1 es de cuantas operaciones se van a utilizar 
    {
        perror("semop p");
        exit(13);
    }
}

void semV(int id)
{
    if (semop(id, &v, 1) < 0)  
    {
        perror("semop p");
        exit(14);
    }
}

//crear la semafora
int getSemCreate()
{
        int i = semget(KEY, 1, 0666 | IPC_CREAT); //  semget crea un array de semaforas , poniendo 1 lo que haces es hacer que ese array solo cree una semafora , si pones 0 no se crearia ninguna 
        if (i < 0)
        {
            perror("semget");
            exit(11);
        }
    return i;
}

//obtener la semafora
int getSem()
{
    int i = semget(KEY, 1, 0 );
    if (i < 0)
    {
            perror("semget");
            exit(11);
    }
    return i;
}

//controlar la semafora
void controlSem(int id, int numRecursos)
{
    //union semun u;
    u.val = numRecursos;
    if (semctl(id, 0, SETVAL, u) < 0)
    {
        perror("semctl");
        exit(12);
    }
}

int main(int argc, char *argv[]) 
{
    char c;
    int id ;
    
    
    
        // argv es argumentos 
        if ( argv[1][0]=='c'){
            id = getSemCreate();
            controlSem(id, 1);
        }
        if ( argv[1][0]=='g'){
            id = getSem();
        }
        
        printf("sem:%d\n",id);

        char *s = argv[2];
        int l = strlen(s);
        for (int i = 0; i < l; ++i)
        {
            semP(id);   //wait for semaphora to be positive reduce by one
            putchar(s[i]); fflush(stdout);
            
            sleep(rand() % 2);
            putchar(s[i]); fflush(stdout);
            
            //en la seccion critica no se puede tener informacion que dependa del usuario
            //nunca se puede pedir informacion al usuario en la seccion critica, error de programador
                if (argv[2][0] == 'a')   //si en el segundo argumento en la segunda posicion hay una letra a se queda esperando por el usuario
                    scanf(" %c", &c);
            semV(id);
            sleep(rand() % 2);
        }

}