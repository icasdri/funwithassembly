#include <stdio.h>
#include <unistd.h>

void print_int(int);

int main(void) {
    FILE *r = fopen("/dev/urandom", "r");
    if (r == NULL) {
        fprintf(stderr, "Failed to open /dev/urandom.\n");
        return 2;
    }

    int i;
    for (;;) {
        size_t n = fread(&i, sizeof(int), 1, r);
        if (n != 1) {
            fprintf(stderr, "Failed to read from /dev/urandom.\n");
            return 3;
        }

        print_int(i);
        printf(" %d\n", i);

        usleep(500000);
    }
}
