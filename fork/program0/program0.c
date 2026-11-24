#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
   int pid;
   pid = fork();

   // proceso hijo clon del padre
   if (pid == 0) { 
      //corre otra terminal
      system("echo 'hijo'");
      //cual es tu process id del echo osea la llamada a la terminal del echo en system
      system("echo $$");
      printf("PID del hijo: %d, PID de su padre: %d\n", getpid(), getppid());
      sleep(100);
   } else { //proceso padre
      system("echo 'padre'");
      system("echo $$");
      printf("PID del padre: %d, PID del hijo creado: %d\n", getpid(), pid); //pid ya almacena el id del hijo creado
      sleep(100);
   }
   return 0;
}
