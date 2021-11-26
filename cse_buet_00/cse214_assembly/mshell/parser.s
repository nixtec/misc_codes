# parser.s
# parses a command and calls appropriate function
#
# Ayub <mrayub@gmail.com>
#

# data section
.data
counter:	# counts how many tokens are available
	.long 0

cmd_cd:
	.asciz "cd"
cmd_chmod:
	.asciz "chmod"
cmd_cp:
	.asciz "cp"
cmd_create:
	.asciz "create"
cmd_exit:
	.asciz "exit"
cmd_find:
	.asciz "find"
cmd_ls:
	.asciz "ls"
cmd_mkdir:
	.asciz "mkdir"
cmd_mv:
	.asciz "mv"
cmd_rmdir:
	.asciz "rmdir"
cmd_rm:
	.asciz "rm"
cmd_touch:
	.asciz "touch"
cmd_help:
	.asciz "help"
unknown_cmd:
	.asciz "Unrecognized command\n"

.equ CMD, -32			# command's position onto the stack
.equ ARG1, -36			# 1st argument's position onto the stack
.equ ARG2, -40			# 2nd argument's position onto the stack

# bss section
.bss            # uninitialized data goes here
.equ STR_LEN, 50
.lcomm cmd, STR_LEN
.lcomm arg1, STR_LEN
.lcomm arg2, STR_LEN

# code section
.text
.type parser, @function
# parser <cmd> <delim>
.globl parser
parser:
# push them onto stack and change the following as necessary
	pushl %ebp
	movl %esp, %ebp

	subl $28, %esp		# space for 5 general registers and two local variables
	movl %ebx, -4(%ebp)
	movl %ecx, -8(%ebp)
	movl %edx, -12(%ebp)
	movl %esi, -16(%ebp)
	movl %edi, -20(%ebp)

	movl 8(%ebp), %ebx	# points to cmd (%ebx, %esi, 1)
	movl 12(%ebp), %ecx	# points to delim (%ecx, %edi, 1)

	pushl %ebx
	call strlen
	movl %eax, %edx		# %edx=len_cmd
	pushl %ecx
	call strlen
	addl $8, %esp		# reset the stack pointer
	cmpl %eax, %edx		# cmd must be greater than delim in length
	jl parser_exit
	subl %eax, %edx		# %edx=len_cmd-len_delim, %eax=len_delim

	movl %edx, -24(%ebp)	# save it onto stack as local variable
	movl %eax, -28(%ebp)	# save it onto the stack as local variable

	pushl %ebx		# the first token (command)
	movl $1, counter

	movl $-1, %esi		# will be incremented
parser_loop_i:
	incl %esi		# increment i
	cmpl %esi, -24(%ebp)	# i<=(len_cmd-len_delim)?
	jl parser_print		# the whole cmd scan complete
	movl %esi, %edx
	xorl %edi, %edi		# the delim index, j=0
parser_loop_j:
	cmpl %edi, -28(%ebp)
	je parser_found		# if %edi equals len_delim, we got NULL of delim
	movb (%ebx, %edx, 1), %ah	# cmd[i]
	movb (%ecx, %edi, 1), %al	# delim[j]
	cmpb %ah, %al
	jne  parser_loop_i		# break inner loop (loop of j)
	incl %edi
	incl %edx
	jmp parser_loop_j	# continue loop

parser_found:
	incl counter		# increment the argument counter
	movl %ebx, %eax
	addl %esi, %eax
	movb $0, (%eax)		# set a null to separate tokens
	addl %edi, %eax		# %eax now points to next token
	addl %edi, %esi
	pushl %eax		# save it onto stack
	jmp parser_loop_i	# find more tokens
parser_print:
	pushl CMD(%ebp)
	pushl $cmd
	call strcpy
	addl $8, %esp

	cmpl $2, counter
	jl parser_no_arg1	# argument1 not given
	pushl ARG1(%ebp)
	pushl $arg1
	call strcpy
	addl $8, %esp

	cmpl $3, counter
	jl parser_no_arg2	# argument2 not given
	pushl ARG2(%ebp)
	pushl $arg2
	call strcpy
	addl $8, %esp

	jmp parser_arg_ok
parser_no_arg1:
	movb $0, arg1		# sets starting null to argument1
parser_no_arg2:
	movb $0, arg2		# sets starting null to argument2
parser_arg_ok:			# all arguments are prepared for use
	pushl $cmd
	pushl $cmd_cd
	call strcmp
	addl $8, %esp
	cmpl $0, %eax
	jne parser_next_cmd1
	pushl $arg2
	pushl $arg1
	call cd
	addl $8, %esp
	jmp parser_exit			# return to caller for next command
parser_next_cmd1:
	pushl $cmd
	pushl $cmd_chmod
	call strcmp
	addl $8, %esp
	cmpl $0, %eax
	jne parser_next_cmd2
	pushl $arg2
	pushl $arg1
	call chmod
	addl $8, %esp
	jmp parser_exit			# return to caller for next command
parser_next_cmd2:
	pushl $cmd
	pushl $cmd_cp
	call strcmp
	addl $8, %esp
	cmpl $0, %eax
	jne parser_next_cmd3
	pushl $arg2
	pushl $arg1
	call cp
	addl $8, %esp
	jmp parser_exit			# return to caller for next command
parser_next_cmd3:
	pushl $cmd
	pushl $cmd_create
	call strcmp
	addl $8, %esp
	cmpl $0, %eax
	jne parser_next_cmd4
	pushl $arg1
	call create
	addl $4, %esp
	jmp parser_exit			# return to caller for next command
parser_next_cmd4:
	pushl $cmd
	pushl $cmd_exit
	call strcmp
	addl $8, %esp
	cmpl $0, %eax
	jne parser_next_cmd5
	call exit
	jmp parser_exit			# return to caller for next command
parser_next_cmd5:
	pushl $cmd
	pushl $cmd_find
	call strcmp
	addl $8, %esp
	cmpl $0, %eax
	jne parser_next_cmd6
	pushl $arg1
	call find
	addl $4, %esp
	jmp parser_exit			# return to caller for next command
parser_next_cmd6:
	pushl $cmd
	pushl $cmd_ls
	call strcmp
	addl $8, %esp
	cmpl $0, %eax
	jne parser_next_cmd7
	pushl $arg1
	call ls
	addl $4, %esp
	jmp parser_exit			# return to caller for next command
parser_next_cmd7:
	pushl $cmd
	pushl $cmd_mkdir
	call strcmp
	addl $8, %esp
	cmpl $0, %eax
	jne parser_next_cmd8
	pushl $arg1
	call mkdir
	addl $4, %esp
	jmp parser_exit			# return to caller for next command
parser_next_cmd8:
	pushl $cmd
	pushl $cmd_mv
	call strcmp
	addl $8, %esp
	cmpl $0, %eax
	jne parser_next_cmd9
	pushl $arg2
	pushl $arg1
	call mv
	addl $8, %esp
	jmp parser_exit			# return to caller for next command
parser_next_cmd9:
	pushl $cmd
	pushl $cmd_rmdir
	call strcmp
	addl $8, %esp
	cmpl $0, %eax
	jne parser_next_cmd10
	pushl $arg1
	call rmdir
	addl $4, %esp
	jmp parser_exit			# return to caller for next command
parser_next_cmd10:
	pushl $cmd
	pushl $cmd_rm
	call strcmp
	addl $8, %esp
	cmpl $0, %eax
	jne parser_next_cmd11
	pushl $arg1
	call rm
	addl $4, %esp
	jmp parser_exit			# return to caller for next command
parser_next_cmd11:
	pushl $cmd
	pushl $cmd_touch
	call strcmp
	addl $8, %esp
	cmpl $0, %eax
	jne parser_next_cmd12
	pushl $arg1
	call touch
	addl $4, %esp
	jmp parser_exit			# return to caller for next command
parser_next_cmd12:
	pushl $cmd
	pushl $cmd_help
	call strcmp
	addl $8, %esp
	cmpl $0, %eax
	jne parser_next_cmd13
	call help
	jmp parser_exit			# return to caller for next command
parser_next_cmd13:
	pushl $unknown_cmd
	call print_str
	jmp parser_exit

parser_exit:
	movl -4(%ebp), %ebx
	movl -8(%ebp), %ecx
	movl -12(%ebp), %edx
	movl -16(%ebp), %esi
	movl -20(%ebp), %edi

	movl %ebp, %esp
	popl %ebp
	ret

