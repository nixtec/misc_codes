# create.s
# create function definition
# create <file>
#
# Ayub <mrayub@gmail.com>
#

.data
create_usage:
	.asciz "Usage: create <file>\n"

.text
.type create, @function
.globl create
.equ CREATE, 8
.equ CREATE_PERM, 0664
create:
	pushl %ebp
	movl %esp, %ebp

# check if filename is given
	pushl 8(%ebp)
	call strlen
	cmpl $0, %eax
	je create_print_usage
#	addl $4, %esp		# reset the stack pointer

# save the general registers
#	subl $8, %esp
	subl $4, %esp		# $4 already subtracted
	movl %ebx, -4(%ebp)
	movl %ecx, -8(%ebp)

	movl $CREATE, %eax	# create() syscall
	movl 8(%ebp), %ebx
	movl $CREATE_PERM, %ecx	# permission mode
	int  $0x80

# restore the general registers
	movl -4(%ebp), %ebx
	movl -8(%ebp), %ecx
	jmp create_end

create_print_usage:
	pushl $create_usage
	call print_str

create_end:
	movl %ebp, %esp
	popl %ebp
	ret

