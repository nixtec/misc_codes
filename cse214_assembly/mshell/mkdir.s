# mkdir.s
# the mkdir function definition
# mkdir <dir>
#
# Ayub <mrayub@gmail.com>
#

.data
mkdir_usage:
	.asciz "Usage: mkdir <dir>\n"

.text
.type mkdir, @function
.globl mkdir
.equ MKDIR, 39
.equ MKDIR_PERM, 0775
mkdir:
	pushl %ebp
	movl %esp, %ebp

# check if dirname supplied
	pushl 8(%ebp)
	call strlen
	cmpl $0, %eax
	je mkdir_print_usage
#	addl $4, %esp		# reset the stack pointer

# save the general registers
#	subl $8, %esp
	subl $4, %esp		# $4 already subtracted
	movl %ebx, -4(%ebp)
	movl %ecx, -8(%ebp)

	movl $MKDIR, %eax		# mkdir() syscall
	movl 8(%ebp), %ebx		# directory name
	movl $MKDIR_PERM, %ecx
	int  $0x80

# reset the registers
	movl -4(%ebp), %ebx
	movl -8(%ebp), %ecx
	jmp mkdir_end

mkdir_print_usage:
	pushl $mkdir_usage
	call print_str

mkdir_end:
	movl %ebp, %esp
	popl %ebp
	ret

