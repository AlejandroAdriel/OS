# P2: Demostración Integrada de las 7 Técnicas POSIX en C

Este proyecto **P2** es un módulo didáctico completo que reúne en un solo sistema funcional las **7 técnicas fundamentales de Sistemas Operativos y Comunicación Interprocesos (IPC)** en Linux.

---

## Mapeo y Explicación de las 7 Técnicas

### 1. **Fork (`fork()`)**
- **Dónde se usa**: En `P2_principal.c`.
- **Qué hace**: Duplica el proceso principal para dar nacimiento a dos procesos independientes:
  - `pid_worker`: Encargado del procesamiento de la cola de mensajes y escritura en FIFO.
  - `pid_exec`: Encargado de transformar su ejecución al programa independiente `P2_relector`.

### 2. **Signalización (`signal` / `kill`)**
- **Dónde se usa**: En `P2_principal.c`.
- **Qué hace**: Captura señales enviadas mediante la terminal (`kill`):
  - **`SIGINT` (Señal 2)**: Al recibir Ctrl+C o `kill -2`, activa el envío por Cola de Mensajes.
  - **`SIGUSR1` (Señal 10)**: Al recibir `kill -10`, activa el envío por la Tubería Anónima (Pipe).

### 3. **Message Passing (Cola de Mensajes System V)**
- **Dónde se usa**: En `P2_principal.c` (`msgget`, `msgsnd`, `msgrcv`).
- **Qué hace**: `P2_principal` deposita mensajes estructurados en la cola mediante `msgsnd()`. `Hijo Worker` los escucha mediante `msgrcv()`.

### 4. **FIFO (Named Pipe / Tubería con Nombre)**
- **Dónde se usa**: En `P2_principal.c` con `mkfifo("/tmp/p2_fifo", 0666)`.
- **Qué hace**: Una vez que `Hijo Worker` recibe una orden desde la Cola de Mensajes, abre la FIFO en disco (`open`), escribe un reporte de vuelta al Proceso Padre y cierra la FIFO.

### 5. **Pipe (Tubería Anónima)**
- **Dónde se usa**: En `P2_principal.c` con `pipe(pipe_fd)`.
- **Qué hace**: Conecta la salida de mensajes del Proceso Padre hacia el `Hijo Exec`.

### 6. **Dup2 (`dup2()`)**
- **Dónde se usa**: En `P2_principal.c` dentro del `pid_exec`.
- **Qué hace**: Redirige la entrada estándar con `dup2(pipe_fd[0], STDIN_FILENO)`. De esta forma, el programa que se ejecute después leerá automáticamente del Pipe cuando consulte su entrada estándar.

### 7. **Execv (`execv()`)**
- **Dónde se usa**: En `P2_principal.c` invocando `./P2_relector`.
- **Qué hace**: Reemplaza el código del hijo por el binario ejecutable independiente `P2_relector.c`, el cual lee directamente de `STDIN_FILENO` la información enviada por el Pipe.

---

## Diagrama de la Arquitectura P2

```text
               +----------------------------------+
               |  LÍNEA DE COMANDOS (kill / CLI) |
               +----------------------------------+
                                |
               +----------------+----------------+
               | (kill -2)                       | (kill -10)
               v                                 v
   +-----------------------+         +-----------------------+
   |  Señal 2 (SIGINT)     |         |  Señal 10 (SIGUSR1)   |
   +-----------------------+         +-----------------------+
               |                                 |
               v                                 v
+-------------------------------------------------------------------+
|                     PROCESO PRINCIPAL (PADRE)                     |
+-------------------------------------------------------------------+
       |                                           |
       | Message Passing                           | Pipe (Tubería Anónima)
       v                                           v
+-----------------------+                 +-----------------------+
|      HIJO WORKER      |                 |       HIJO EXEC       |
+-----------------------+                 | (dup2 + execv)        |
       |                                  +-----------------------+
       | Escribe Reporte                              |
       v                                              v
+-----------------------+                 +-----------------------+
|  FIFO (/tmp/p2_fifo)  |                 |      P2_RELECTOR      |
+-----------------------+                 |  (Lee de STDIN)       |
       |                                  +-----------------------+
       | Lee Notificación
       v
  [Proceso Padre]
```

---

## Guía de Ejecución y Pruebas

### Compilación:
```bash
gcc -Wall P2_relector.c -o P2_relector
gcc -Wall P2_principal.c -o P2_principal
```

### Ejecución:
En una terminal corre:
```bash
./P2_principal
```

### Pruebas desde otra terminal:
Reemplaza `<PID_PADRE>` por el PID indicado en pantalla.

1. **Probar Señal 2 (SIGINT) -> Cola -> Worker -> FIFO -> Padre**:
   ```bash
   kill -2 <PID_PADRE>
   ```

2. **Probar Señal 10 (SIGUSR1) -> Pipe -> Dup2 -> Execv (P2_relector)**:
   ```bash
   kill -10 <PID_PADRE>
   ```
