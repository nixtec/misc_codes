# strlen.s
# the strlen function definition
#
# Ayub <mrayub@gmail.com>
#

.type strlen, @function
.globl strlen
strlen:
	pushl %ebp
	movl %esp, %ebp

	subl $8, %esp		# make room for necessary registers
	movl %edi, -4(%ebp)
	movl %ecx, -8(%ebp)

	movl 8(%ebp), %edi      # load the string pointer to %edi

	xorl %ecx, %ecx         # %ecx=0
	xorb %al, %al		# this is the terminating character
	notl %ecx               # %ecx=-1
	cld
	repne scasb             # repeat while '\0' not found
	notl %ecx               # not(-n) = |n|-1
	decl %ecx
	movl %ecx, %eax         # %eax contains the return value

	movl -4(%ebp), %edi
	movl -8(%ebp), %ecx

	movl %ebp, %esp		# reset the stack pointer
	popl %ebp
	ret

