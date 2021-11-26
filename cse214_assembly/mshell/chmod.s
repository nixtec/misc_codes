# chmod.s
# change file permissions
# mode must be entered as numeric value
# chmod <mod> <file>

/*
  note:
	Mode must be supplied as decimal value (converted to string)
*/

#
# Ayub <mrayub@gmail.com>
#

.data
chmod_usage:
	.asciz "Usage: chmod <mod> <file>\n"

chmod_mod_usage:
	.asciz "000 <= mode <= 511\n"

.text
.type chmod, @function
.globl chmod
.equ CHMOD, 15		# chmod() syscall number
chmod:
	pushl %ebp
	movl %esp, %ebp

# save general registers
	subl $8, %esp		# make room for registers
	movl %ebx, -4(%ebp)
	movl %ecx, -8(%ebp)

# check if filename and mode is supplied
	movl 8(%ebp), %ecx		# mode as string
	pushl %ecx
	call atoi
/*
	xorl %ecx, %ecx
	movw %ax, %cx
*/
	movl %eax, %ecx			# mode as numeric value
	cmpl $0, %ecx			# mode
	jl chmod_print_mod_usage
	cmpl $511, %ecx
	jg chmod_print_mod_usage
	movl 12(%ebp), %ebx		# filename
	pushl %ebx
	call strlen
	cmpl $0, %eax
	je chmod_print_usage

	movl $CHMOD, %eax
	int $0x80

# restore the general registers
	movl -4(%ebp), %ebx
	movl -8(%ebp), %ecx
	jmp chmod_end

chmod_print_mod_usage:
	pushl $chmod_mod_usage
	call print_str
	jmp chmod_end

chmod_print_usage:
	pushl $chmod_usage
	call print_str

chmod_end:
	movl %ebp, %esp			# reset the stack pointer
	popl %ebp
	ret

