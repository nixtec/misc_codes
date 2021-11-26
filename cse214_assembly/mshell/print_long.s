# print_long.s
# takes long value and print it as string
# dependency: print_str.s, strrev.s
#
# Ayub <mrayub@gmail.com>
#

.data
ln_char:	# this will hold the temporary ascii string
	.rept 20
	.asciz ""
	.endr
.text

.type print_long, @function
.globl print_long
print_long:
	pushl %ebp
	movl %esp, %ebp

	subl $16, %esp
	movl %ebx, -4(%ebp)
	movl %ecx, -8(%ebp)
	movl %edx, -12(%ebp)
	movl %esi, -16(%ebp)

	movl $ln_char, %ecx

	movl 8(%ebp), %eax	# the long value
	xorl %esi, %esi		# %esi=0; index
	movl $10, %ebx		# the divisor

print_long_div_loop:
	xorl %edx, %edx
	divl %ebx
	addb $48, %dl		# convert remainder to char
	movb %dl, (%ecx, %esi, 1)	# save it
	incl %esi		# increment the index
	cmpl $0, %eax		# division complete?
	je print_long_div_loop_end
	jmp print_long_div_loop

print_long_div_loop_end:
	movb $0, (%ecx, %esi, 1)	# set the terminating null
	pushl %ecx
	call strrev
	call print_str

	movl -4(%ebp), %ebx
	movl -8(%ebp), %ecx
	movl -12(%ebp), %edx
	movl -16(%ebp), %esi

	movl %ebp, %esp
	popl %ebp
	ret

