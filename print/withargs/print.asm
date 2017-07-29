section .data
    hello: db 'Hello World',10

section .text
    global _start

_start:
    mov rdi,hello
    call theprintfunc

    mov rax,60
    mov rdi,0
    syscall

theprintfunc:
    push   rbp
    mov    rbp,rsp

    mov rax,1
    mov rsi,rdi
    mov rdi,1
    mov rdx,12
    syscall

    pop    rbp
    ret
