; section .data
;  hello:     db 'Hello world!',10    ; 'Hello world!' plus a linefeed character
;  helloLen:  equ $-hello             ; Length of the 'Hello world!' string


; section .text
;  global _start

; _start:
;  mov eax,4            ; The system call for write (sys_write)
;  mov ebx,1            ; File descriptor 1 - standard output
;  mov ecx,hello        ; Put the offset of hello in ecx
;  mov edx,helloLen     ; helloLen is a constant, so we don't need to say
;                       ; mov edx,[helloLen] to get it's actual value
;  int 80h              ; Call the kernel

;  mov eax,1            ; The system call for exit (sys_exit)
;  mov ebx,0            ; Exit with return code of 0 (no error)
;  int 80h

default rel            ; make [rel format] the default, you always want this.
extern printf, exit    ; NASM requires declarations of external symbols, unlike GAS
section .rodata
    format db "%#x", 10, 0   ; C 0-terminated string: "%#x\n" 
section .text
global main
main:
    sub   rsp, 8             ; re-align the stack to 16 before calling another function

    ; Call printf.
    mov   esi, 0x12345678    ; "%x" takes a 32-bit unsigned int
    lea   rdi, [rel format]
    xor   eax, eax           ; AL=0  no FP args in XMM regs
    call  printf

    ; Return from main.
    xor   eax, eax
    add   rsp, 8
    ret