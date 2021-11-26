# print_tab.s
# prints a tab
#
# Ayub <mrayub@gmail.com>
#

.data		# optional (assembler is smart enough to handle this)
tab:
	.ascii "\t"

.text		# (optional) assembler is smart enough to handle this)
.type print_tab, @function
.globl print_tab			# set it global for all
print_tab:
# print a tab
# first save the general registers
	pushl %ebx
	pushl %ecx
	pushl %edx

	movl $4, %eax
	movl $1, %ebx
	movl $tab, %ecx
	movl $1, %edx
	int  $0x80

	popl %edx
	popl %ecx
	popl %ebx
	ret

