# print_str.s
# string printing function definition
# string must be a null terminated string
# only one string may be printed on each call
# dependency: strlen.s
#
# Ayub <mrayub@gmail.com>
#

.text		# optional (assembler is smart enough to handle this)
.type print_str, @function
.globl print_str
print_str:
	pushl %ebp
	movl %esp, %ebp

	subl $12, %esp		# make room for general registers
	movl %ebx, -4(%ebp)
	movl %ecx, -8(%ebp)
	movl %edx, -12(%ebp)

	movl 8(%ebp), %ecx
	pushl %ecx		# push the address of string
	call strlen		# get the length of the string

	movl %eax, %edx		# the length of the string
	movl $4, %eax		# write () syscall
	movl $1, %ebx		# stdout
	int  $0x80		# call kernel

	movl -4(%ebp), %ebx
	movl -8(%ebp), %ecx
	movl -12(%ebp), %edx

	movl %ebp, %esp		# reset the stack pointer
	popl %ebp
	ret

