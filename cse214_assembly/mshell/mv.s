# mv.s
# mv (rename) function definition
# mv <old> <new>
#
# Ayub <mrayub@gmail.com>
#
.data
mv_usage:
	.asciz "Usage: mv <old> <new>\n"

.text
.type mv, @function
.globl mv
.equ RENAME, 38
mv:
	pushl %ebp
	movl %esp, %ebp

# check if filenames supplied
	pushl 8(%ebp)
	call strlen
	cmpl $0, %eax
	je mv_print_usage
	pushl 12(%ebp)
	call strlen
	cmpl $0, %eax
	je mv_print_usage
#	addl $8, %esp

# save the general registers
#	subl $8, %esp		# $8 already subtracted
	movl %ebx, -4(%ebp)
	movl %ecx, -8(%ebp)

	movl 8(%ebp), %ebx		# old
	movl 12(%ebp), %ecx		# new
	movl $RENAME, %eax		# rename() syscall
	int  $0x80

	movl -4(%ebp), %ebx
	movl -8(%ebp), %ecx
	jmp mv_end

mv_print_usage:
	pushl $mv_usage
	call print_str

mv_end:
	movl %ebp, %esp			# reset the stack pointer
	popl %ebp
	ret

