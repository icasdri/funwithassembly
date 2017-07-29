; Simple program that prints out the number of arguments that was passed to it
; Assemble with: nasm -f elf64 argcount.asm
; Link with: ld argcount.o -o argcount
; and Run! ./argcount

section .data
    newline: db `\n`

section .bss
    buf: resb 4

section .text
    global _start

_start:
    pop rax ; argc
    add rax, -1 ; first "argument" is program name
    xor r15,r15 ; r15 is our counter
    mov rbx,10 ; the constant 10
    jmp get_digit

get_digit:
    xor rdx,rdx ; on input, rdx must be 0, or else div coughs up craziness
    div rbx ; rax / 10 -> rax; rax % 10 -> rdx
    ; here: rax is quotient; rdx is remainder

    push rdx ; push a digit onto stack
    inc r15 ; increment our counter
    cmp rax,0 ; check if we're done (if the quotient is now 0 -- aka. we have all digits)
    je print_digit
    jmp get_digit

print_digit:
    pop r14 ; r14 is our digit now
    add r14,48 ; convert raw int to ASCII value ('0' == ASCII 48)
    mov [buf],r14 ; store in buffer for use with syscall

    mov rax,1 ; syscall: sys_write
    mov rdi,1 ; fd: stdout
    mov rsi,buf ; an ADDRESS is needed here for this syscall (not a value)
    mov rdx,1 ; len: 1 (we're just printing one char)
    syscall

    ; check if there are any digits left for us on the stack
    dec r15
    cmp r15,0
    je end
    jmp print_digit

end:
    ; print a newline
    mov rax,1 ; syscall: sys_write
    mov rdi,1 ; fd: stdout
    mov rsi,newline ; an ADDRESS is needed here for this syscall (not a value)
    mov rdx,1 ; len: 1 (we're just printing one char)
    syscall

    ; exit
    mov rax,60 ; syscall: sys_exit
    mov rdi,0 ; exit with success code 0
    syscall
