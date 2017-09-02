#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

unsigned char initial[] = {0x55, 0x48, 0x89, 0xe5, 0xb8, 0x01, 0x00, 0x00, 0x00, 0x48, 0x89, 0xfe, 0xbf, 0x01, 0x00, 0x00, 0x00, 0xba, 0x0c, 0x00, 0x00, 0x00, 0x0f, 0x05, 0x5d, 0xc3};

int main() {
    unsigned char *buf = mmap(NULL, sizeof(initial),
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS,
            -1, 0);

    if (buf == MAP_FAILED) {
        return 2;
    }

    memcpy(buf, initial, sizeof(initial));
    mprotect(buf, sizeof(initial), PROT_READ | PROT_EXEC);

    void (*func)(char *) = (void (*)(char *)) buf;
    char hello[] = "Hello World\n";
    (*func)(hello);

    if (munmap(buf, sizeof(initial))) {
        return 3;
    }
}
