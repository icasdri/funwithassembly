section .text
    global _start

_start:
    call theprintfunc

    mov rax,60  ; exit
    mov rdi,0   ; exit code
    syscall

theprintfunc:
    sub rsp,13   ; get more stack space
    mov r14,rsp  ; address to our stack buffer

    mov BYTE [rsp],'H'
    mov BYTE [rsp + 1],'e'
    mov BYTE [rsp + 2],'l'
    mov BYTE [rsp + 3],'l'
    mov BYTE [rsp + 4],'o'
    mov BYTE [rsp + 5],' '
    mov BYTE [rsp + 6],'W'
    mov BYTE [rsp + 7],'o'
    mov BYTE [rsp + 8],'r'
    mov BYTE [rsp + 9],'l'
    mov BYTE [rsp + 10],'d'
    mov BYTE [rsp + 11],'!'
    mov BYTE [rsp + 12],10  ; newline

    mov rax,1    ; write
    mov rdi,1    ; fd 1 (stdout)
    mov rsi,r14  ; address to our stack buffer
    mov rdx,13   ; len
    syscall

    add rsp,13  ; put back stack space

    ret
