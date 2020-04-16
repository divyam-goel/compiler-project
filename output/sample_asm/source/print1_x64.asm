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

	a:	dd	4	; int a=5;
	b:	dd	10
	f1:	dq	2.0
	f2: dq	2.333
	tmp1:dd	0.0
	tmp2:dd 0.0
	inta1:	dd	12345678

    fmt:    db "a=%ld, rax=%ld", 10, 0	; The printf format, "\n",'0'
	print_fmt_int:	db "Output: %d" , 10, 0 
	print_fmt_float:	db "Output: %e" , 10, 0 
	print_fmt_bool_false:	db "Output: false" , 10, 0 
	print_fmt_bool_true:	db "Output: true" , 10, 0 

SECTION .text           ; Code section.

    global main		; the standard gcc entry point
main:				; the program label for the entry point
    push    rbp		; set up stack frame
	
	; do some addition
	mov ax, [a]
	add ax, [b]
	mov [a],ax

	; print integer out
	mov	rdi,print_fmt_int		; format for printf
	mov	rsi,[a]         ; first parameter for printf
	mov	rax,0		; no xmm registers
    call printf		; Call C function

	; addition time
	mov dword[tmp1] , __float32__(0.030000)
	; mov dword[tmp2] , __float32__(23.0)
	fld dword[tmp1]
	fmul qword[f1]
	fstp qword[f1]
	; print float
	mov rdi,print_fmt_float
	movq xmm0,qword[f1]
	mov	rax,1		; 1 xmm register
	call printf

	pop	rbp		; restore stack

	mov	rax,0		; normal, no error, return value
	ret			; return