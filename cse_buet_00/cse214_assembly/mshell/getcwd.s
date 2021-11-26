# getcwd.s
# gets the current directory name
#
# Ayub <mrayub@gmail.com>
#
.data
.equ MAX_PROMPT, 256
.equ GETCWD, 183
prompt:
	.rept MAX_PROMPT
	.asciz ""
	.endr
append_str:	# this string will be appended to the command prompt
	.asciz " $ "

.text
.type getcwd, @function
.globl getcwd
getcwd:
	pushl %ebp
	movl %esp, %ebp

	subl $8, %esp
	movl %ebx, -4(%ebp)
	movl %ecx, -8(%ebp)

	movl $GETCWD, %eax
	movl $prompt, %ebx
	movl $MAX_PROMPT, %ecx
	int  $0x80

	pushl $append_str
	pushl $prompt
	call strcat

	pushl $prompt
	call print_str

getcwd_end:
	movl %ebp, %esp		# reset the stack pointer
	popl %ebp
	ret

