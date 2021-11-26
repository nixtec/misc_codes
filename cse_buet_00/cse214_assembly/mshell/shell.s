# shell.s
# a simple shell implementation using assembly
#
# Ayub <mrayub@gmail.com>
#
.data
err_msg:
	.asciz "Error performing the request\n"

delim:		# this is the parsing delimiter
	.asciz " "

.bss
.equ MAX_CMD_LEN, 200
.lcomm cmd, MAX_CMD_LEN
.equ EXIT, 1
.text
.globl _start
_start:
loop_forever:
	call getcwd
	movl $3, %eax
	movl $1, %ebx
	movl $cmd, %ecx
	movl $100, %edx
	int  $0x80
	cmpl $0, %eax
	je do_exit		# Ctrl-d pressed (terminate application)
	cmpl $1, %eax		# Only ENTER pressed
	je loop_forever		# prompt again for next command
	decl %eax
	movb $0, cmd(, %eax, 1)		# set the terminating null

	pushl $delim
	pushl $cmd
	call parser
	addl $8, %esp

	cmpl $0, %eax		# check status
	jl  print_err_msg
	jmp loop_forever

print_err_msg:
	pushl $err_msg
	call print_str
	addl $4, %esp
	jmp loop_forever

do_exit:
	call print_nl
	movl $EXIT, %eax
	xorl %ebx, %ebx		# %ebx = 0
	int  $0x80

