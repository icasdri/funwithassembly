section .text
    global print_int

print_int:
    ; save non-volatile registers
    push rdi
    push rsi

    mov r10,10  ; the constant 10

    mov rsi,rsp  ; pointer to current position in buffer
    dec rsi  ; adjust to last element

    movsx rax,edi  ; consume first argument
    cmp rax,0
    jge .loop
    ; if negative, make non-negative
    ; (safe as this is 64-bit storing 32-bit, no signed overflow possible)
    imul rax,-1

    .loop:  ; main loop: encodes non-negative number digit-by-digit to ASCII
    xor rdx,rdx  ; div requires 0 in rdx
    div r10  ; rax = rax / 10; rdx = rax % 10;
    add rdx,'0'  ; convert to ASCII
    mov byte [rsi],dl  ; dl is lowest byte for rdx
    dec rsi
    cmp rax,0
    jne .loop

    ; if original argument (esi) was negative, add negative sign and alter length
    cmp edi,0
    jge .final
    mov byte [rsi],'-'
    dec rsi

    .final:
    ; correct to address of first element and compute length
    inc rsi
    mov rdx,rsp
    sub rdx,rsi

    mov rax,1
    mov rdi,1
    ; rsi already set as pointer to first element
    ; rdx already set to length
    syscall

    ; restore non-volatile registers
    pop rsi
    pop rdi

    ret
