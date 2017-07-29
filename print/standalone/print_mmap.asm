section .text
    global _start

_start:
    call theprintfunc

    mov rax,60  ; exit
    mov rdi,0   ; exit code
    syscall

theprintfunc:
    mov rax,9   ; mmap
    mov rdi,0   ; NULL addr
    mov rsi,13  ; len
    mov rdx,3   ; PROT_READ | PROT_WRITE
    mov r10,34  ; MAP_ANONYMOUS | MAP_PRIVATE
    mov r8,-1   ; fd of -1 (we're not mapping from file)
    mov r9,0    ; offset of 0
    syscall

    mov r14,rax  ; addr of our mapped memory
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
    mov rsi,r14  ; pointer to our buffer
    mov rdx,13   ; len
    syscall

    ret
