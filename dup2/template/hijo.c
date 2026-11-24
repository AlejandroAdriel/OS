#include <unistd.h>

int main(void) {
    char buf;
    read(STDIN_FILENO, &buf, 1);
    return 0;
}
