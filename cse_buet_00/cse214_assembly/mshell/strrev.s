# strrev.s

# reverse a string
# dependency: strlen.s
#
# Ayub <mrayub@gmail.com>
#

.type strrev, @function
.globl strrev			# you can call it from a C program
strrev:
	pushl %ebp
	movl %esp, %ebp

	subl $20, %esp		# make room for general registers
	movl %ebx, -4(%ebp)
	movl %ecx, -8(%ebp)
	movl %edx, -12(%ebp)
	movl %esi, -16(%ebp)
	movl %edi, -20(%ebp)

	movl 8(%ebp), %ebx	# pointer to string
	pushl %ebx
	call strlen
	cmpl $1, %eax		# check for null or one-character string
	jle strrev_end
	movl %eax, %edx		# %edx = strlen (str)
	movb $2, %cl
	idivb %cl		# divide %ax by 2; %al=len/2, %ah=len%2
	movzbl %al, %ecx	# %ecx = strlen (str)/2

	xorl %esi, %esi
	decl %edx		# strlen (str) - 1
	movl %edx, %edi

strrev_loop:
	cmpl %esi, %ecx		# while (i<half)
	je strrev_end
	movb (%ebx, %esi, 1), %al	# %al is for temporary assignment
	movb (%ebx, %edi, 1), %ah	# %ah is for temporary assignment
	movb %al, (%ebx, %edi, 1)
	movb %ah, (%ebx, %esi, 1)
	incl %esi
	decl %edi
	jmp strrev_loop

strrev_end:
	movl -4(%ebp), %ebx
	movl -8(%ebp), %ecx
	movl -12(%ebp), %edx
	movl -16(%ebp), %esi
	movl -20(%ebp), %edi

	movl %ebp, %esp
	popl %ebp
	ret

