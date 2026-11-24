#include <signal.h>
#include <unistd.h>

void handler(int sig) {
    (void)sig;
}

int main(void) {
    signal(SIGUSR1, handler);
    raise(SIGUSR1);
    return 0;
}
