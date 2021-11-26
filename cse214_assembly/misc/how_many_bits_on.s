# howmany1s1.s
# counts how many bits are on in a number

.data
n:
	.long 255			# place a value between -2^31 -> 2^31-1

.text
.globl _start
_start:
	xorl %ebx, %ebx			# %ebx=0, initialize the counter
	movl n, %eax			# %eax=data

start_loop:
	cmpl $0, %eax
	je   end_loop
	movl %eax, %ecx			# copy the data
	decl %ecx			# %ecx = data - 1
	andl %ecx, %eax			# %eax = data & (data - 1)
	incl %ebx			# increment the counter
	jmp  start_loop

end_loop:
# %ebx already contains the number of bits on
	movl $1, %eax			# exit() syscall
	int  $0x80			# call kernel

