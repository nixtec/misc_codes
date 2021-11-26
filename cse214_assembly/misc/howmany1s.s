# howmany1s.s
# count how many one's in a binary version of a decimal number

.data
n:
	.long 255		# how many bits are 'on'? in this number

.text
.globl _start
_start:
	movl n, %eax		# load the number in %eax

	movl $0, %ebx		# this will store the counter
	movl $32, %ecx		# continue loop 31 times (as %ecx > 0? testing)

count_loop:
	shl $1, %eax
	jnc skip_inc		# skip incrementing if carry is not 1
	incl %ebx
skip_inc:
	loop count_loop		# decrement %ecx and loop until %ecx becomes 0

	movl $1, %eax		# the exit() syscall
	int $0x80		# invoke the kernel

