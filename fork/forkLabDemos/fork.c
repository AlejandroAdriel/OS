#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include<stdio.h>

int main() {
   int pid;
   pid = fork();

   // Child process
   if (pid == 0) {
      //corre otra terminal
      system("echo 'hijo'");
      //cual es tu process id del echo osea la llamada a la terminal del echo en system
      system("echo $$");
      sleep(100);
   } else {
      system("echo 'padre'");
      system("echo $$");
      sleep(100);
   }
   return 0;
}
