#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

static const char mmap_template[] = {
    0xb8, 0x09, 0x00, 0x00, 0x00,              // mov rax,9  ; mmap
    0xbf, 0x00, 0x00, 0x00, 0x00,              // mov rdi,0  ; NULL addr
    0xbe, 0x0d, 0x00, 0x00, 0x00,              // mov rsi,13 ; len
    0xba, 0x03, 0x00, 0x00, 0x00,              // mov rdx,3  ; PROT_READ | PROT_WRITE
    0x41, 0xba, 0x22, 0x00, 0x00, 0x00,        // mov r10,34 ; MAP_ANON. | MAP_PRIV.
    0x49, 0xc7, 0xc0, 0xff, 0xff, 0xff, 0xff,  // mov r8,01  ; fd -1
    0x41, 0xb9, 0x00, 0x00, 0x00, 0x00,        // mov r9,0   ; offset 0
    0x0f, 0x05                                 // syscall
};
static const off_t mmap_n_offset = 11;

static const char write_template[] = {
    0xb8, 0x01, 0x00, 0x00, 0x00,  // mov rax,1    ; write
    0xbf, 0x01, 0x00, 0x00, 0x00,  // mov rdi,1    ; fd 1 (stdout)
    0x4c, 0x89, 0xf6,              // mov rsi,r14  ; addr of buffer (stored in r14)
    0xba, 0x0d, 0x00, 0x00, 0x00,  // mov rsi,13   ; len
    0x0f, 0x05                     // syscall
};
static const off_t write_n_offset = 14;

static inline void write_n(unsigned char *buf, size_t n) {
    unsigned char hold[sizeof(size_t)];
    *((size_t *) hold) = n;
    memcpy(buf, hold, 4);
}

int print_jit(char *s) {
    size_t n = strlen(s);
    if (n <= 0) return 0;
    if (n >= 128) return 1;  // we only support string lengths < 128

    size_t len = 0 \
       + sizeof(mmap_template)  // mmap syscall
       + 3   // intermediate save mov
       + 4   // base dereference set (index 0)
       + (n - 1) * 5  // dereference sets (index 1 througn n - 1)
       + sizeof(write_template)  // write syscall
       + 1   // return
       ;

    unsigned char *root_buf = mmap(NULL, len,
        PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
        -1, 0);
    unsigned char *buf = root_buf;

    if (buf == NULL) return 2;

    /* mmap part */
    memcpy(buf, mmap_template, sizeof(mmap_template));
    write_n(buf + mmap_n_offset, n);

    buf += sizeof(mmap_template);


    /* intermediate save part */
    // mov r14,rax  ; save addr return of mmap to r14
    buf[0] = 0x49;
    buf[1] = 0x89;
    buf[2] = 0xc6;

    buf += 3;


    /* base/first dereference set part */
    // mov BYTE [r14],...
    buf[0] = 0x41;
    buf[1] = 0xc6;
    buf[2] = 0x06;
    buf[3] = s[0];

    buf += 4;


    /* remaining dereference sets part */
    for (unsigned char i = 1; i < n; i++) {
        // mov BYTE [r14 + i],...
        buf[0] = 0x41;
        buf[1] = 0xc6;
        buf[2] = 0x46;
        buf[3] = i;
        buf[4] = s[i];
        buf += 5;
    }

    /* write part */
    memcpy(buf, write_template, sizeof(write_template));
    write_n(buf + write_n_offset, n);

    buf += sizeof(write_template);

    /* ret part */
    buf[0] = 0xc3;

    buf += 1;

    /* done writing to buf */
    buf = NULL;


    // mark our buffer as executable
    if (mprotect(root_buf, len, PROT_READ | PROT_EXEC)) return 3;

    // execute it!
    void (*func)(void) = (void (*)(void)) root_buf;
    (*func)();

    if (munmap(root_buf, len)) return 4;

    return 0;
}

int main(size_t argc, char *argv[]) {
    char *msg = "Hello World!";
    if (argc == 2) msg = argv[1];

    int r = print_jit(msg);
    if (!r) printf("\n");

    return r;
}
