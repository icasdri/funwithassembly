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

    mov BYTE [r14],'H'
    mov BYTE [r14 + 1],'e'
    mov BYTE [r14 + 2],'l'
    mov BYTE [r14 + 3],'l'
    mov BYTE [r14 + 4],'o'
    mov BYTE [r14 + 5],' '
    mov BYTE [r14 + 6],'W'
    mov BYTE [r14 + 7],'o'
    mov BYTE [r14 + 8],'r'
    mov BYTE [r14 + 9],'l'
    mov BYTE [r14 + 10],'d'
    mov BYTE [r14 + 11],'!'
    mov BYTE [r14 + 12],10  ; newline

    mov rax,1    ; write
    mov rdi,1    ; fd 1 (stdout)
    mov rsi,r14  ; address to our stack buffer
    mov rdx,13   ; len
    syscall

    add rsp,13  ; put back stack space

    ret
