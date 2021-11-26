# rmdir.s
# the rmdir function definition
# the directory must be empty
# rmdir <dir>
#
# Ayub <mrayub@gmail.com>
#

.data
rmdir_usage:
	.asciz "Usage: rmdir <dir>\n"

.text
.type rmdir, @function
.globl rmdir
.equ RMDIR, 40
rmdir:
	pushl %ebp
	movl %esp, %ebp

# check if direname supplied
	pushl 8(%ebp)
	call strlen
	cmpl $0, %eax
	je rmdir_print_usage
#	addl $4, %esp		# reset the stack pointer

# save the general registers
#	subl $4, %esp
	movl %ebx, -4(%ebp)

	movl $RMDIR, %eax		# rmdir() syscall
	movl 8(%ebp), %ebx		# the directory name
	int  $0x80

# restore the general registers
	movl -4(%ebp), %ebx
	jmp rmdir_end

rmdir_print_usage:
	pushl $rmdir_usage
	call print_str

rmdir_end:
	movl %ebp, %esp		# reset the stack pointer
	popl %ebp
	ret

