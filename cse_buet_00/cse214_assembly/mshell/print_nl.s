# print_nl.s
# prints a newline
#
# Ayub <mrayub@gmail.com>
#

.data		# optional (assembler is smart enough to handle this)
nl:
	.ascii "\n"

.text		# (optional) assembler is smart enough to handle this)
.type print_nl, @function
.globl print_nl			# set it global for all
print_nl:
# print a newline
# first save the general registers
	pushl %ebx
	pushl %ecx
	pushl %edx

	movl $4, %eax
	movl $1, %ebx
	movl $nl, %ecx
	movl $1, %edx
	int  $0x80

	popl %edx
	popl %ecx
	popl %ebx
	ret

