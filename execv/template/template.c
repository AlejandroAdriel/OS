#include <unistd.h>

int main(void) {
    char *args[] = {"./hijo", NULL};
    execv("./hijo", args);
    return 0;
}
