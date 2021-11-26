# strlen.s
# finds the length of the given string

# data section
.data
msg:
	.ascii "Enter String : "
	len = . - msg

# bss section
.bss		# uninitialized data goes here
.equ MAX_LEN, 50
.lcomm s, MAX_LEN

# code section
.text
.globl _start
_start:
	movl $4, %eax		# write() syscall
	movl $1, %ebx		# stdout
	movl $msg, %ecx		# string to be printed
	movl $len, %edx		# length of the string
	int  $0x80		# call kernel

	movl $3, %eax
	movl $0, %ebx
	movl $s, %ecx		# input buffer
	movl $MAX_LEN, %edx	# maximum length of input
	int  $0x80
	decl %eax
	movb $0, s(, %eax, 1)	# set the terminating null (replacing '\n')

# the string is assumed to be NUL '\0' terminated
	pushl $s
	call strlen
	addl $4, %esp

	movl %eax, %ebx		# save the string length
	movl $1, %eax
	int  $0x80

# here goes the strlen function definition
.type strlen, @function
strlen:
	pushl %edi		# because we'll edit this
	movl 8(%esp), %edi	# load the string pointer to %edi

	xorl %ecx, %ecx		# %ecx=0
	xorb %al, %al		# terminating character (NUL)
	notl %ecx		# %ecx=-1
	cld
	repne scasb		# repeat while '\0' not found
	notl %ecx		# not(-n) = |n|-1
	decl %ecx
	movl %ecx, %eax		# %eax contains the return value

	popl %edi
	ret

