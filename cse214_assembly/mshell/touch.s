# touch.s
# updates the modification time of inode to current time
# touch <file/directory>
#
# Ayub <mrayub@gmail.com>
#
.data
touch_usage:
	.asciz "Usage: touch <file>\n"

.text
.type touch, @function
.globl touch
.equ UTIME, 30
touch:
	pushl %ebp
	movl %esp, %ebp

# check if filename is supplied
	pushl 8(%ebp)
	call strlen
	cmpl $0, %eax
	je touch_print_usage
#	addl $4, %esp

# save the general registers
#	subl $8, %esp
	subl $4, %esp		# $4 is already subtracted
	movl %ebx, -4(%ebp)
	movl %ecx, -8(%ebp)

	movl $UTIME, %eax	# utime() [update time] syscall
	movl 8(%ebp), %ebx	# filename
	xorl %ecx, %ecx		# (struct timebuf *) NULL
	int  $0x80

	movl -4(%ebp), %ebx
	movl -8(%ebp), %ecx
	jmp touch_end

touch_print_usage:
	pushl $touch_usage
	call print_str

touch_end:
	movl %ebp, %esp		# reset the stack pointer
	popl %ebp
	ret

