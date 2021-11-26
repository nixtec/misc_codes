# exit.s
# exits to shell
# always succeeds (returns 0 to shell)

#
# Ayub <mrayub@gmail.com>
#

.text
.type exit, @function
.globl exit
.equ EXIT, 1
exit:
	movl $EXIT, %eax
	xorl %ebx, %ebx
	int  $0x80

