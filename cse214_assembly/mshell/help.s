# help.s
#
# Ayub <mrayub@gmail.com>
#

.data
infile:
	.asciz "help"

.bss
.equ BUFFER_SIZE, 500		# for cp function
.lcomm buffer, BUFFER_SIZE	# for cp function

.text
.type help, @function
.globl help
.equ O_RDONLY, 0			# read-only
.equ O_CREAT_WRONLY_TRUNC, 03101	# create, writeonly, truncate
.equ END_OF_FILE, 0
help:
	pushl %ebp
	movl %esp, %ebp


# save the general registers
	subl $16, %esp				# make room for local use
	movl %ebx, -4(%ebp)
	movl %ecx, -8(%ebp)
	movl %edx, -12(%ebp)

# open the input file
	movl $5, %eax				# open() syscall
	movl $infile, %ebx			# input filename into %ebx
	movl $O_RDONLY, %ecx			# read-only flag
	movl $0666, %edx			# no matter for reading
	int  $0x80				# call linux kernel

	cmpl $-1, %eax				# check for error condition
	je help_end
	movl %eax, -16(%ebp)			# save the input fd

help_loop_begin:
	movl -16(%ebp), %ebx			# get the input file desc.
	movl $buffer, %ecx			# location to read into
	movl $BUFFER_SIZE, %edx			# size of the buffer
	movl $3, %eax				# read it
	int  $0x80				# size of buffer read is
						# returned in %eax
# check for EOF condition
	cmpl $END_OF_FILE, %eax			# check for EOF or error
	jle  help_loop_end			# if found, go to the end

# write to output file
	movl $1, %ebx				# stdout will be used
	movl $buffer, %ecx			# location of buffer
	movl %eax, %edx				# size of buffer
	movl $4, %eax				# will be written
	int  $0x80				# write it

	jmp help_loop_begin			# continue again

help_loop_end:
	movl -16(%ebp), %ebx		# input fd
	movl $6, %eax			# close the input file
	int  $0x80

# restore the general registers
	movl -4(%ebp), %ebx
	movl -8(%ebp), %ecx
	movl -12(%ebp), %edx
#	jmp help_end

help_end:
# return to caller function

	movl %ebp, %esp
	popl %ebp
	ret

