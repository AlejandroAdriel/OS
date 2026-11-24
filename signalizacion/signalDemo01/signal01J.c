/*

 1 HUP      2 INT      3 QUIT     4 ILL      5 TRAP     6 ABRT     6 IOT      7 BUS
 8 FPE      9 KILL    10 USR1    11 SEGV    12 USR2    13 PIPE    14 ALRM    15 TERM
16 STKFLT  17 CHLD    17 CLD     18 CONT    19 STOP    20 TSTP    21 TTIN    22 TTOU
23 URG     24 XCPU    25 XFSZ    26 VTALRM  27 PROF    28 WINCH   29 IO      29 POLL
30 PWR     31 SYS     34 RTMIN   64 RTMAX

*/

#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>

static sigjmp_buf punto_salto;

void manejador(int signo) {
  printf("\nSe recibió la señal %d\n", signo);

  siglongjmp(punto_salto, signo);
}

int main(void) {

  signal(SIGINT, manejador);  // 2  - Exit Interrupt
  signal(SIGTRAP, manejador); // 5  - Core Trace/Breakpoint Trap
  signal(SIGXFSZ, manejador); // 25 - Core File size limit exceeded

  int signal = sigsetjmp(punto_salto, 1);

  if (signal == 0) {

    printf("Programa iniciado.\n");
    printf("Pulsa Ctrl+C para enviar SIGINT.\n");
    printf("Desde terminal kill -5 <PID> SIGTRAP\n");
    printf("Desde terminal kill -25 <PID> SIGXFSZ\n");

    while (1) {
      printf("Ejecutando código normal...\n");
      sleep(2);
    }
  } else {

    printf("¡Se realizó el salto de código!\n");

    switch (signal) {
      case SIGINT:
      printf("Señal SIGINT (%d) - Exit Interrupt\n", signal);
      break;
      case SIGTRAP:
      printf("Señal SIGTRAP (%d) - Core Trace Breakpoint Trap\n", signal);
      break;
      case SIGXFSZ:
      printf("Señal SIGXFSZ (%d) - Core File Size Limit Exceeded\n", signal);
      break;
      default:
      printf("Señal no clasificada (%d)\n", signal);
      break;
    }

    printf("Continuando desde el punto de recuperación...\n");
  }

  printf("Fin del programa.\n");

  return 0;
}
