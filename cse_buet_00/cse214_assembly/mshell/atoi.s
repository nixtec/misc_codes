# atoi.s
# convert ascii string to integer/long value and return it via %eax
#
# Ayub <mrayub@gmail.com>
#

.data
atoi_mulby:
	.long 10

.text
.type atoi, @function
.globl atoi
atoi:
	pushl %ebp
	movl %esp, %ebp

	pushl 8(%ebp)
	call strlen
	cmpl $0, %eax
	je atoi_end		# string not supplied
#	addl $4, %esp

# save the general purpose registers
#	subl $20, %esp
	subl $16, %esp		# $4 is already subtracted
	movl %ebx, -4(%ebp)
	movl %ecx, -8(%ebp)
	movl %edx, -12(%ebp)
	movl %esi, -16(%ebp)
	movl %edi, -20(%ebp)

	movl %eax, %esi		# save the string length
	decl %esi		# for indexing (starts from 0)
	movl 8(%ebp), %ebx	# the ascii value pointer
#	pushl %ebx
#	call strlen
#	movl %eax, %esi		# index of ascii string

	xorl %edi, %edi		# %edi = sum = 0 (initialization)
	movl $1, %ecx		# the multiplier
atoi_mul_loop:
	xorl %edx, %edx
	movzbl (%ebx, %esi, 1), %eax
	subl $48, %eax		# convert from char to int
	mull %ecx
	addl %eax, %edi		# add the result with the previous value
	decl %esi		# increment the index
	cmpl $0, %esi		# multiplication complete?
	jl atoi_mul_loop_end
	movl %ecx, %eax
	mull atoi_mulby		# multiply by 10
	movl %eax, %ecx		# save the value again in %ecx
	jmp atoi_mul_loop

atoi_mul_loop_end:
	movl %edi, %eax		# return value goes in %eax

# restore the general registers
	movl -4(%ebp), %ebx
	movl -8(%ebp), %ecx
	movl -12(%ebp), %edx
	movl -16(%ebp), %esi
	movl -20(%ebp), %edi

atoi_end:
	movl %ebp, %esp
	popl %ebp
	ret

