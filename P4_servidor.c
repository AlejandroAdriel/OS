#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#define FIFO_PATH "/tmp/p4_fifo"

// Estructura para la Cola de Mensajes System V (Message Passing)
struct msg_buffer {
    long msg_type;
    char msg_text[512];
};

int msg_queue_id = -1;

// Buffers de salto para las señales (Patrón Lab 2)
static sigjmp_buf salto_sigint;
static sigjmp_buf salto_sigusr1;

// Manejadores de señal que realizan el siglongjmp (Patrón Lab 2)
void handler_sigint(int signo) {
    (void)signo;
    siglongjmp(salto_sigint, 1);
}

void handler_sigusr1(int signo) {
    (void)signo;
    siglongjmp(salto_sigusr1, 1);
}

// =========================================================================
// FUNCIÓN QUE EJECUTA LA CADENA DE 7 TÉCNICAS TRAS EL SALTO DE SEÑAL
// =========================================================================
void disparar_cadena_7_tecnicas(const char *nombre_evento, int num_senal) {
    printf("-------------------------------------------------------------------\n");
    printf("[CADENA DOMINÓ] Disparando 7 técnicas POSIX para %s (Señal %d)\n", 
           nombre_evento, num_senal);
    printf("-------------------------------------------------------------------\n");

    // ---------------------------------------------------------------------
    // PASO 2: FORK -> El Servidor se duplica para procesar la petición
    // ---------------------------------------------------------------------
    pid_t pid = fork();

    if (pid < 0) {
        perror("Error al ejecutar fork");
        return;
    }

    if (pid > 0) {
        // PROCESO PADRE (SERVIDOR)
        // -----------------------------------------------------------------
        // PASO 3: MESSAGE PASSING -> Padre deposita el trabajo en la Cola IPC
        // -----------------------------------------------------------------
        struct msg_buffer msg;
        msg.msg_type = (num_senal == 2) ? 1 : 2;
        snprintf(msg.msg_text, sizeof(msg.msg_text), 
                 "[1. SIGNAL (%s - %d)] -> [2. FORK (Hijo PID %d)] -> [3. MESSAGE PASSING]", 
                 nombre_evento, num_senal, pid);

        printf("[PADRE SERVIDOR] (Paso 3: Message Passing) Enviando evento a la Cola IPC...\n");
        msgsnd(msg_queue_id, &msg, sizeof(msg.msg_text), 0);

        // El Padre espera que finalice el Hijo para no dejar procesos zombi y seguir listo
        wait(NULL);
        printf("[PADRE SERVIDOR] Petición procesada. Regresando al bucle while(1) del servidor...\n\n");
    } else {
        // PROCESO HIJO WORKER
        // -----------------------------------------------------------------
        // PASO 3: MESSAGE PASSING -> Hijo recibe el evento de la Cola
        // -----------------------------------------------------------------
        struct msg_buffer msg_recibido;
        long tipo_buscado = (num_senal == 2) ? 1 : 2;
        msgrcv(msg_queue_id, &msg_recibido, sizeof(msg_recibido.msg_text), tipo_buscado, 0);
        printf("[HIJO WORKER] (Paso 3: Message Passing) Recibido de la Cola: '%s'\n", msg_recibido.msg_text);

        // -----------------------------------------------------------------
        // PASO 4: FIFO / NAMED PIPE -> Escribe datos acumulados en la FIFO en disco
        // -----------------------------------------------------------------
        int fifo_fd_out = open(FIFO_PATH, O_WRONLY);
        char texto_fifo[1024];
        snprintf(texto_fifo, sizeof(texto_fifo), "%s -> [4. FIFO (%s)]", 
                 msg_recibido.msg_text, FIFO_PATH);
        write(fifo_fd_out, texto_fifo, strlen(texto_fifo));
        close(fifo_fd_out);
        printf("[HIJO WORKER] (Paso 4: FIFO) Escribió datos en la tubería nombrada.\n");

        // -----------------------------------------------------------------
        // PASO 5: PIPE ANÓNIMO -> Lee de FIFO y los pasa a la Tubería Anónima
        // -----------------------------------------------------------------
        int pipe_fd[2];
        if (pipe(pipe_fd) == -1) {
            perror("Error creando Pipe");
            exit(1);
        }

        // Leer datos acumulados desde la FIFO
        int fifo_fd_in = open(FIFO_PATH, O_RDONLY);
        char texto_desde_fifo[1024];
        ssize_t n_fifo = read(fifo_fd_in, texto_desde_fifo, sizeof(texto_desde_fifo) - 1);
        close(fifo_fd_in);

        if (n_fifo > 0) texto_desde_fifo[n_fifo] = '\0';

        // Escribir en la Tubería Anónima (Pipe)
        char texto_para_pipe[2048];
        snprintf(texto_para_pipe, sizeof(texto_para_pipe), "%s -> [5. PIPE ANÓNIMO]", texto_desde_fifo);
        write(pipe_fd[1], texto_para_pipe, strlen(texto_para_pipe));
        close(pipe_fd[1]); // Cerrar escritura para marcar el EOF
        printf("[HIJO WORKER] (Paso 5: Pipe) Pasó contenido de FIFO al Pipe Anónimo.\n");

        // -----------------------------------------------------------------
        // PASO 6: DUP2 -> Redirigir el extremo de lectura del Pipe a STDIN_FILENO
        // -----------------------------------------------------------------
        dup2(pipe_fd[0], STDIN_FILENO);
        close(pipe_fd[0]);
        printf("[HIJO WORKER] (Paso 6: Dup2) Redirigió Pipe a STDIN_FILENO.\n");

        // -----------------------------------------------------------------
        // PASO 7: EXECV -> Cargar el binario ejecutable independiente ./P4_final
        // -----------------------------------------------------------------
        printf("[HIJO WORKER] (Paso 7: Execv) Ejecutando ./P4_final...\n\n");
        char *args[] = {"./P4_final", NULL};
        execv("./P4_final", args);

        perror("Error al ejecutar execv");
        exit(1);
    }
}

int main(void) {
    // 1. Inicializar FIFO en disco
    unlink(FIFO_PATH);
    if (mkfifo(FIFO_PATH, 0666) == -1) {
        perror("Error creando FIFO");
        exit(1);
    }

    // 2. Inicializar Cola de Mensajes System V
    msg_queue_id = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    if (msg_queue_id == -1) {
        perror("Error creando Cola de Mensajes");
        exit(1);
    }

    // 3. Registrar los manejadores para las señales (Patrón Lab 2)
    signal(SIGINT, handler_sigint);   // Señal 2 (SIGINT / Ctrl+C)
    signal(SIGUSR1, handler_sigusr1); // Señal 10 (SIGUSR1)

    printf("===================================================================\n");
    printf("   SERVIDOR P4 (PATRÓN LAB 2): while(1) + sigsetjmp / siglongjmp\n");
    printf("===================================================================\n");
    printf("  PID Servidor Principal: %d\n", getpid());
    printf("-------------------------------------------------------------------\n");
    printf(" Envía cualquiera de las 2 señales desde otra terminal:\n");
    printf("   kill -2  %d   --> (Señal 2  - SIGINT  -> Salto sigsetjmp Tipo 1)\n", getpid());
    printf("   kill -10 %d   --> (Señal 10 - SIGUSR1 -> Salto sigsetjmp Tipo 2)\n", getpid());
    printf("===================================================================\n\n");
    printf("Esperando señales en bucle servidor...\n\n");

    // =====================================================================
    // PATRÓN LAB 2: BUCLE while(1) CON sigsetjmp Y pause() ENGLOBANDO SEÑALES
    // =====================================================================
    while (1) {
        // Captura de salto para SIGINT (Señal 2)
        if (sigsetjmp(salto_sigint, 1) != 0) {
            printf("¡Se realizó el salto de código (SIGINT)! Procesando evento...\n");
            disparar_cadena_7_tecnicas("SIGINT - PETICIÓN ESTÁNDAR", 2);
        }

        // Captura de salto para SIGUSR1 (Señal 10)
        if (sigsetjmp(salto_sigusr1, 1) != 0) {
            printf("¡Se realizó el salto de código (SIGUSR1)! Procesando evento...\n");
            disparar_cadena_7_tecnicas("SIGUSR1 - DIAGNÓSTICO SERVIDOR", 10);
        }

        pause(); // Mantiene en suspensión pasiva hasta la siguiente señal
    }

    // Limpieza de recursos
    unlink(FIFO_PATH);
    msgctl(msg_queue_id, IPC_RMID, NULL);
    return 0;
}
