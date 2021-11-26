# the rm function
# rm <file>
#
# Ayub <mrayub@gmail.com>
#
.data
rm_usage:
	.asciz "Usage: rm <file>\n"

.text
.type rm, @function
.globl rm
.equ UNLINK, 10
rm:
	pushl %ebp
	movl %esp, %ebp

# check if filename supplied
	pushl 8(%ebp)
	call strlen
	cmpl $0, %eax
	je rm_print_usage
#	addl $4, %esp

# save the general registers
#	subl $4, %esp
	movl %ebx, -4(%ebp)

	movl $UNLINK, %eax	# unlink() syscall
	movl 8(%ebp), %ebx
	int  $0x80

# restore the general registers
	movl -4(%ebp), %ebx
	jmp rm_end

rm_print_usage:
	pushl $rm_usage
	call print_str

rm_end:
	movl %ebp, %esp		# reset the stack pointer
	popl %ebp
	ret

