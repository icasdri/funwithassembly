# Fun With Assembly

A collection of interesting assembly programs and C programs that call into assembly for `Linux x86_64`.

## Assembling/Building

Build and link `*.asm` files with

    nasm -f elf64 example.nasm -o example.o
    ld example.o -o example

Build and link `*.c` file as usual

    gcc example.c -o example
