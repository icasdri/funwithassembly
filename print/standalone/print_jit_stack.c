#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

const char write_template[] = {
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
    size_t slen = strlen(s);
    if (slen <= 0) return 0;
    if (slen >= 128) return 1;  // we only support string lengths < 128

    unsigned char n = slen;

    /* ALLOCATE THE BUFFER */

    size_t len = 0 \
       + 4   // stack increase
       + 3   // save stack pointer
       + 4   // first char put (index 0)
       + (n - 1) * 5  // remaining char puts (index 1 througn n - 1)
       + sizeof(write_template)  // write syscall
       + 4   // stack restore
       + 1   // return
       ;

    unsigned char *root_buf = mmap(NULL, len,
        PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
        -1, 0);
    unsigned char *buf = root_buf;

    if (buf == NULL) return 2;

    /* FILL THE BUFFER */

    /* stack increase */
    // sub rsp,n
    buf[0] = 0x48;
    buf[1] = 0x83;
    buf[2] = 0xec;
    buf[3] = n;

    buf += 4;


    /* save stack pointer */
    // mov r14,rsp
    buf[0] = 0x49;
    buf[1] = 0x89;
    buf[2] = 0xe6;

    buf += 3;


    /* first char put */
    // mov BYTE [rsp],...
    buf[0] = 0x41;
    buf[1] = 0xc6;
    buf[2] = 0x06;
    buf[3] = s[0];

    buf += 4;


    /* remaining char puts */
    for (unsigned char i = 1; i < n; i++) {
        // mov BYTE [r14 + i],...
        buf[0] = 0x41;
        buf[1] = 0xc6;
        buf[2] = 0x46;
        buf[3] = i;
        buf[4] = s[i];
        buf += 5;
    }

    /* write syscall */
    memcpy(buf, write_template, sizeof(write_template));
    write_n(buf + write_n_offset, n);

    buf += sizeof(write_template);

    /* stack restore */
    // add rsp,n
    buf[0] = 0x48;
    buf[1] = 0x83;
    buf[2] = 0xc4;
    buf[3] = n;

    buf += 4;

    /* return */
    buf[0] = 0xc3;

    buf += 1;

    buf = NULL;

    /* EXECUTE THE BUFFER */

    // mark as executable
    if (mprotect(root_buf, len, PROT_READ | PROT_EXEC)) return 3;

    // execute!
    void (*func)(void) = (void (*)(void)) root_buf;
    (*func)();

    /* FREE THE BUFFER */
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
