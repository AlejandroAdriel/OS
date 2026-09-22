#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#define FIFO_PATH "/tmp/p3_fifo"

// Estructura para la Cola de Mensajes System V (Message Passing)
struct msg_buffer {
    long msg_type;
    char msg_text[512];
};

int msg_queue_id = -1;

// =========================================================================
// PASO 1: SIGNALIZACIÓN (Detonador de la Cadena)
// =========================================================================
void manejador_sigint(int sig) {
    printf("\n===================================================================\n");
    printf("[PASO 1: SIGNAL] Capturada Señal %d (SIGINT). Iniciando la Seguidilla...\n", sig);
    printf("===================================================================\n");

    // =====================================================================
    // PASO 2: FORK (Creación del proceso para procesar la cadena)
    // =====================================================================
    pid_t pid = fork();

    if (pid < 0) {
        perror("Error al ejecutar fork");
        return;
    }

    if (pid > 0) {
        // -----------------------------------------------------------------
        // PROCESO PADRE
        // -----------------------------------------------------------------
        // PASO 3: MESSAGE PASSING (Padre envía el mensaje inicial a la Cola)
        struct msg_buffer msg;
        msg.msg_type = 1;
        snprintf(msg.msg_text, sizeof(msg.msg_text), 
                 "[1. SIGNAL] -> [2. FORK (PID %d)] -> [3. MESSAGE PASSING]", pid);

        printf("[PADRE] (Paso 3: Message Passing) Depositando datos en la Cola IPC...\n");
        msgsnd(msg_queue_id, &msg, sizeof(msg.msg_text), 0);

        // Espera a que el proceso hijo complete la cadena y finalice
        wait(NULL);
        printf("[PADRE] Cadena dominó completada exitosamente.\n\n");
    } else {
        // -----------------------------------------------------------------
        // PROCESO HIJO
        // -----------------------------------------------------------------
        // PASO 3: MESSAGE PASSING (Hijo recibe de la Cola de Mensajes)
        struct msg_buffer msg_recibido;
        msgrcv(msg_queue_id, &msg_recibido, sizeof(msg_recibido.msg_text), 1, 0);
        printf("[HIJO]  (Paso 3: Message Passing) Recibido de la Cola: '%s'\n", msg_recibido.msg_text);

        // -----------------------------------------------------------------
        // PASO 4: FIFO / NAMED PIPE (Hijo escribe el acumulado en la FIFO)
        // -----------------------------------------------------------------
        int fifo_fd_out = open(FIFO_PATH, O_WRONLY);
        char texto_fifo[1024];
        snprintf(texto_fifo, sizeof(texto_fifo), "%s -> [4. FIFO (%s)]", 
                 msg_recibido.msg_text, FIFO_PATH);
        write(fifo_fd_out, texto_fifo, strlen(texto_fifo));
        close(fifo_fd_out);
        printf("[HIJO]  (Paso 4: FIFO) Datos escritos en la tubería nombrada.\n");

        // -----------------------------------------------------------------
        // PASO 5: PIPE ANÓNIMO (Leer de FIFO y transferir al Pipe Anónimo)
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
        close(pipe_fd[1]); // Cerrar extremo de escritura para emitir EOF
        printf("[HIJO]  (Paso 5: Pipe) Transfirió contenido de la FIFO al Pipe Anónimo.\n");

        // -----------------------------------------------------------------
        // PASO 6: DUP2 (Redirigir la lectura del Pipe a STDIN_FILENO)
        // -----------------------------------------------------------------
        dup2(pipe_fd[0], STDIN_FILENO);
        close(pipe_fd[0]); // Cierra descriptor duplicado
        printf("[HIJO]  (Paso 6: Dup2) Conectó la entrada del Pipe a STDIN_FILENO.\n");

        // -----------------------------------------------------------------
        // PASO 7: EXECV (Reemplazar imagen con ./P3_final)
        // -----------------------------------------------------------------
        printf("[HIJO]  (Paso 7: Execv) Ejecutando ./P3_final...\n\n");
        char *args[] = {"./P3_final", NULL};
        execv("./P3_final", args);

        perror("Error al ejecutar execv");
        exit(1);
    }
}

int main(void) {
    // Inicializar FIFO
    unlink(FIFO_PATH);
    if (mkfifo(FIFO_PATH, 0666) == -1) {
        perror("Error creando FIFO");
        exit(1);
    }

    // Inicializar Cola de Mensajes
    msg_queue_id = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    if (msg_queue_id == -1) {
        perror("Error creando Cola de Mensajes");
        exit(1);
    }

    // Registrar manejador de señal SIGINT (Señal 2)
    signal(SIGINT, manejador_sigint);

    printf("===================================================================\n");
    printf("   CADENA SECUENCIAL P3: SEGUIDILLA DOMINÓ DE 7 TÉCNICAS POSIX\n");
    printf("===================================================================\n");
    printf("  PID Proceso Principal: %d\n", getpid());
    printf("-------------------------------------------------------------------\n");
    printf(" Para iniciar la seguidilla de 7 pasos, ejecuta en otra terminal:\n");
    printf("   kill -2 %d    (Señal 2 - SIGINT -> Inicia la Cadena Dominó)\n", getpid());
    printf("===================================================================\n\n");
    printf("Esperando la señal SIGINT para activar la cadena...\n");

    // Mantener proceso en espera
    while (1) {
        pause();
    }

    // Limpieza
    unlink(FIFO_PATH);
    msgctl(msg_queue_id, IPC_RMID, NULL);
    return 0;
}
