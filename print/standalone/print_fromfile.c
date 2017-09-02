#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

int main() {
    char *filename = "print";
    size_t len = 129;
    off_t offset = 145;

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open %s.\n", filename);
        return 2;
    }

    if (fseek(file, offset, SEEK_SET)) {
        fprintf(stderr, "Failed to seek to %zu in %s.\n", offset, filename);
        return 3;
    }

    unsigned char *buf = mmap(NULL, len,
        PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
        -1, 0);

    if (buf == MAP_FAILED) {
        fprintf(stderr, "Failed to map %zu bytes of memory.\n", len);
        return 4;
    }

    if (fread(buf, 1, len, file) != len) {
        fprintf(stderr, "Failed to read %zu bytes (offset %zu) from %s.\n",
                len, offset, filename);
        return 5;
    }

    if (mprotect(buf, len, PROT_READ | PROT_EXEC)) {
        fprintf(stderr, "Failed to mark buffer as executable.\n");
        return 6;
    }

    // execute it!
    void (*func)(void) = (void (*)(void)) buf;
    (*func)();

    if (munmap(buf, len)) {
        fprintf(stderr, "Failed to unmap buffer.\n");
        return 7;
    }
}
