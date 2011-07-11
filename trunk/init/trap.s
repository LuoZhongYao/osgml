	.file	"trap.c"
.globl idt
	.bss
	.align 4
	.type	idt, @object
	.size	idt, 4
idt:
	.zero	4
	.section	.rodata
.LC0:
	.string	"This is default interrupt!\n"
	.text
.globl default_int
	.type	default_int, @function
default_int:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$.LC0, (%esp)
	call	printk
	leave
	ret
	.size	default_int, .-default_int
.globl clock
	.type	clock, @function
clock:
	pushl	%ebp
	movl	%esp, %ebp
	movl	ken_struct, %eax
	movl	8(%eax), %edx
	addl	$1, %edx
	movl	%edx, 8(%eax)
	movl	$32, %eax
	movl	$32, %edx
#APP
# 23 "trap.c" 1
	out    %al,%dx
# 0 "" 2
#NO_APP
	movl	$32, %eax
	movl	$160, %edx
#APP
# 24 "trap.c" 1
	out    %al,%dx
# 0 "" 2
#NO_APP
	popl	%ebp
	ret
	.size	clock, .-clock
.globl install_int
	.type	install_int, @function
install_int:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	movl	20(%ebp), %eax
	movw	%ax, -4(%ebp)
	movl	idt, %eax
	movl	8(%ebp), %edx
	sall	$3, %edx
	leal	(%eax,%edx), %edx
	movl	12(%ebp), %eax
	movw	%ax, (%edx)
	movl	idt, %eax
	movl	8(%ebp), %edx
	sall	$3, %edx
	leal	(%eax,%edx), %edx
	movl	12(%ebp), %eax
	shrl	$16, %eax
	movw	%ax, 6(%edx)
	movl	idt, %eax
	movl	8(%ebp), %edx
	sall	$3, %edx
	leal	(%eax,%edx), %edx
	movl	16(%ebp), %eax
	movw	%ax, 2(%edx)
	movl	idt, %eax
	movl	8(%ebp), %edx
	sall	$3, %edx
	leal	(%eax,%edx), %edx
	movzwl	-4(%ebp), %eax
	movw	%ax, 4(%edx)
	leave
	ret
	.size	install_int, .-install_int
	.ident	"GCC: (Ubuntu/Linaro 4.5.2-8ubuntu4) 4.5.2"
	.section	.note.GNU-stack,"",@progbits
