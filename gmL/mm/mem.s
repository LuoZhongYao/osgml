	.file	"mem.c"
.globl HMEM
	.bss
	.align 4
	.type	HMEM, @object
	.size	HMEM, 4
HMEM:
	.zero	4
.globl LMEM
	.align 4
	.type	LMEM, @object
	.size	LMEM, 4
LMEM:
	.zero	4
	.text
.globl do_no_page
	.type	do_no_page, @function
do_no_page:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	LMEM, %eax
	cmpl	12(%ebp), %eax
	jae	.L1
	movl	HMEM, %eax
	cmpl	12(%ebp), %eax
	jbe	.L1
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	put_page
.L1:
	leave
	ret
	.size	do_no_page, .-do_no_page
.globl mem_init
	.type	mem_init, @function
mem_init:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	$1050628, %eax
	movl	(%eax), %eax
	movl	%eax, ker_data+4
	movl	$3145728, ker_data+12
	movl	$0, -12(%ebp)
	jmp	.L4
.L5:
	movl	ker_data+12, %edx
	movl	-12(%ebp), %eax
	leal	(%edx,%eax), %eax
	movb	$-1, (%eax)
	addl	$1, -12(%ebp)
.L4:
	cmpl	$1048575, -12(%ebp)
	jle	.L5
	movl	ker_data+4, %eax
	shrl	$2, %eax
	movl	%eax, -20(%ebp)
	movl	$1024, -16(%ebp)
	jmp	.L6
.L7:
	movl	ker_data+12, %eax
	addl	-16(%ebp), %eax
	movb	$0, (%eax)
	addl	$1, -16(%ebp)
.L6:
	movl	-16(%ebp), %eax
	cmpl	-20(%ebp), %eax
	jb	.L7
	movl	ker_data+4, %eax
	movl	%eax, HMEM
	movl	$1048576, LMEM
	movl	$36352, 12(%esp)
	movl	$16, 8(%esp)
	movl	$page_fault, 4(%esp)
	movl	$14, (%esp)
	call	install_int
	leave
	ret
	.size	mem_init, .-mem_init
.globl get_free_page
	.type	get_free_page, @function
get_free_page:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	ker_data+4, %eax
	shrl	$2, %eax
	movl	%eax, -8(%ebp)
	movl	$0, -4(%ebp)
	jmp	.L9
.L12:
	movl	ker_data+12, %edx
	movl	-4(%ebp), %eax
	leal	(%edx,%eax), %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	jne	.L10
	movl	ker_data+12, %edx
	movl	-4(%ebp), %eax
	leal	(%edx,%eax), %eax
	movb	$1, (%eax)
	movl	-4(%ebp), %eax
	jmp	.L11
.L10:
	addl	$1, -4(%ebp)
.L9:
	movl	-4(%ebp), %eax
	cmpl	-8(%ebp), %eax
	jl	.L12
	movl	$-1, %eax
.L11:
	leave
	ret
	.size	get_free_page, .-get_free_page
.globl free_page
	.type	free_page, @function
free_page:
	pushl	%ebp
	movl	%esp, %ebp
	movl	HMEM, %eax
	sall	$12, %eax
	cmpl	8(%ebp), %eax
	jg	.L14
	movl	$-1, %eax
	jmp	.L15
.L14:
	movl	ker_data+12, %edx
	movl	8(%ebp), %eax
	leal	(%edx,%eax), %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	jne	.L16
	movl	$-1, %eax
	jmp	.L15
.L16:
	movl	ker_data+12, %edx
	movl	8(%ebp), %eax
	leal	(%edx,%eax), %eax
	movzbl	(%eax), %edx
	subl	$1, %edx
	movb	%dl, (%eax)
	movl	$0, %eax
.L15:
	popl	%ebp
	ret
	.size	free_page, .-free_page
.globl put_page
	.type	put_page, @function
put_page:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	call	get_free_page
	movl	%eax, -4(%ebp)
	cmpl	$-1, -4(%ebp)
	jne	.L18
	movl	$-1, %eax
	jmp	.L19
.L18:
	movl	HMEM, %eax
	sall	$12, %eax
	cmpl	-4(%ebp), %eax
	jle	.L20
	movl	LMEM, %eax
	sall	$12, %eax
	cmpl	-4(%ebp), %eax
	jl	.L21
.L20:
	movl	$-1, %eax
	jmp	.L19
.L21:
	movl	ker_data+16, %eax
	movl	8(%ebp), %edx
	shrl	$22, %edx
	sall	$2, %edx
	addl	%edx, %eax
	movl	(%eax), %eax
	testl	%eax, %eax
	jne	.L22
	call	get_free_page
	movl	%eax, -8(%ebp)
	cmpl	$-1, -8(%ebp)
	jne	.L23
	movl	$-1, %eax
	jmp	.L19
.L23:
	movl	ker_data+16, %eax
	movl	8(%ebp), %edx
	shrl	$22, %edx
	sall	$2, %edx
	addl	%edx, %eax
	movl	-8(%ebp), %edx
	sall	$12, %edx
	orl	$7, %edx
	movl	%edx, (%eax)
.L22:
	movl	ker_data+16, %eax
	movl	8(%ebp), %edx
	shrl	$22, %edx
	sall	$2, %edx
	addl	%edx, %eax
	movl	(%eax), %eax
	andl	$-4096, %eax
	movl	%eax, -12(%ebp)
	movl	8(%ebp), %eax
	shrl	$12, %eax
	andl	$1023, %eax
	sall	$2, %eax
	addl	-12(%ebp), %eax
	movl	-4(%ebp), %edx
	sall	$12, %edx
	orl	$7, %edx
	movl	%edx, (%eax)
	movl	$0, %eax
.L19:
	leave
	ret
	.size	put_page, .-put_page
.globl free_mm
	.type	free_mm, @function
free_mm:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$20, %esp
	movl	$0, -4(%ebp)
	jmp	.L25
.L30:
	movl	-4(%ebp), %eax
	sall	$2, %eax
	addl	8(%ebp), %eax
	movl	(%eax), %eax
	andl	$1, %eax
	testb	%al, %al
	je	.L26
	movl	-4(%ebp), %eax
	sall	$2, %eax
	addl	8(%ebp), %eax
	movl	(%eax), %eax
	andl	$-4096, %eax
	movl	%eax, -12(%ebp)
	movl	$0, -8(%ebp)
	jmp	.L27
.L29:
	movl	-8(%ebp), %eax
	sall	$2, %eax
	addl	-12(%ebp), %eax
	movl	(%eax), %eax
	andl	$1, %eax
	testb	%al, %al
	je	.L28
	movl	-8(%ebp), %eax
	sall	$2, %eax
	addl	-12(%ebp), %eax
	movl	(%eax), %eax
	shrl	$12, %eax
	movl	%eax, (%esp)
	call	free_page
.L28:
	addl	$1, -8(%ebp)
.L27:
	cmpl	$1023, -8(%ebp)
	jle	.L29
	movl	-4(%ebp), %eax
	sall	$2, %eax
	addl	8(%ebp), %eax
	movl	(%eax), %eax
	shrl	$12, %eax
	movl	%eax, (%esp)
	call	free_page
.L26:
	addl	$1, -4(%ebp)
.L25:
	cmpl	$1023, -4(%ebp)
	jle	.L30
	movl	8(%ebp), %eax
	sarl	$12, %eax
	movl	%eax, (%esp)
	call	free_page
	leave
	ret
	.size	free_mm, .-free_mm
	.ident	"GCC: (Ubuntu/Linaro 4.5.2-8ubuntu4) 4.5.2"
	.section	.note.GNU-stack,"",@progbits
