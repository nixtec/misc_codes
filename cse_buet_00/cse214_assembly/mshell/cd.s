# cd.s
# the directory changing function definition
# cd <dir>
# changes current directory to <dir>
# returns 0 on success, -1 on failure
# dependency: print_str.s
#
# Ayub <mrayub@gmail.com>
#

.data
cd_usage:
	.asciz "Usage: cd <dir>\n"

.text
.type cd, @function
.globl cd
cd:
	pushl %ebp
	movl %esp, %ebp

# check if directory name supplied
	pushl 8(%ebp)
	call strlen
	cmpl $0, %eax
	je cd_print_usage
#	addl $4, %esp		# reset the stack pointer

# save the general register
#	subl $4, %esp
	movl %ebx, -4(%ebp)	# save %ebx, because we'll modify this

	movl $12, %eax		# chdir() syscall
	movl 8(%ebp), %ebx	# directory name
	int  $0x80

# restore the general register
	movl -4(%ebp), %ebx
	jmp cd_end

cd_print_usage:
	pushl $cd_usage
	call print_str

cd_end:
	movl %ebp, %esp		# reset the stack pointer
	popl %ebp
	ret

