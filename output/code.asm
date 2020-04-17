global main
extern printf

SECTION .data
	float_temp1:	dq	0.000000
	float_temp2:	dq	0.000000
	print_fmt_int:	db	"Output: %d", 10, 0
	print_fmt_float:	db	"Output: %e", 10, 0
	print_fmt_false:	db	"Output: false", 10, 0
	print_fmt_true:	db	"Output: true", 10, 0

SECTION .text

main:
LAB:

	mov	ax , 10
	mov	bx , 20
	add	ax , bx
	mov	[@00000] , ax

	mov	ax , [@00000]
	mov	[x] , ax

	mov	ax , [x]
	mov	bx , [y]
	add	ax , bx
	mov	[@00001] , ax

	mov	ax , [@00001]
	mov	[z] , ax

	mov	ax , [x]
	mov	bx , [y]
	sub	ax , bx
	mov	[@00002] , ax

	mov	ax , [@00002]
	mov	[z] , ax

	mov	ax , [x]
	mov	cx , [y]
	imul	ax , cx
	mov	[@00003] , ax

	mov	ax , [@00003]
	mov	[z] , ax

	mov	ax , [x]
	mov	cx , [y]
	idiv	cx
	mov	[@00004] , ax

	mov	ax , [@00004]
	mov	[z] , ax

	fld	qword[b]
	fadd	qword[c]
	FSTP	qword[@00005]

	mov	ax , [@00005]
	mov	[a] , ax

	fld	qword[b]
	fsub	qword[c]
	FSTP	qword[@00006]

	mov	ax , [@00006]
	mov	[a] , ax

	fld	qword[b]
	fmul	qword[c]
	FSTP	qword[@00007]

	mov	ax , [@00007]
	mov	[a] , ax

	fld	qword[b]
	fdiv	qword[c]
	FSTP	qword[@00008]

	mov	ax , [@00008]
	mov	[a] , ax

	mov	dword[float_temp2] , __float32__(0.030000)
	fld	qword[b]
	fdiv	dword[float_temp2]
	FSTP	qword[@00009]

	mov	ax , [@00009]
	mov	[a] , ax

