; Equivalent C code
; /* printf1.c  print a long int, 64-bit, and an expression */
; #include <stdio.h>
; int main()
; {
;   long int a=5;
;   printf("a=%ld, rax=%ld\n", a, a+2);
;   return 0;
; }

; Declare external function
extern	printf		; the C function, to be called

SECTION .data		; Data section, initialized variables

	a:	dq	5	; long int a=5;
    fmt:    db "a=%ld, rax=%ld", 10, 0	; The printf format, "\n",'0'


SECTION .text           ; Code section.

    global main		; the standard gcc entry point
main:				; the program label for the entry point
    push    rbp		; set up stack frame
	
	mov	rax,[a]		; put "a" from store into register
	add	rax,2		; a+2  add constant 2
	mov	rdi,fmt		; format for printf
	mov	rsi,[a]         ; first parameter for printf
	mov	rdx,rax         ; second parameter for printf
	mov	rax,0		; no xmm registers
        call    printf		; Call C function

	pop	rbp		; restore stack

	mov	rax,0		; normal, no error, return value
	ret			; return