# cp.s
# the cp function definition
# cp <src> <dest>
#
# Ayub <mrayub@gmail.com>
#

.data
cp_usage:
	.asciz "Usage: cp <src> <dest>\n"

.bss
.equ BUFFER_SIZE, 500		# for cp function
.lcomm buffer, BUFFER_SIZE	# for cp function

.text
.type cp, @function
.globl cp
.equ O_RDONLY, 0			# read-only
.equ O_CREAT_WRONLY_TRUNC, 03101	# create, writeonly, truncate
.equ END_OF_FILE, 0
cp:
	pushl %ebp
	movl %esp, %ebp


# check for parameters
	pushl 8(%ebp)
	call strlen
	cmpl $0, %eax				# check if filename is supplied
	je cp_print_usage
	pushl 12(%ebp)
	call strlen
	cmpl $0, %eax
	je cp_print_usage
#	addl $8, %esp				# reset the stack pointer

# save the general registers
#	subl $20, %esp				# make room for local use
	subl $12, %esp				# $8 already subtracted
	movl %ebx, -4(%ebp)
	movl %ecx, -8(%ebp)
	movl %edx, -12(%ebp)

# open the input file
	movl $5, %eax				# open() syscall
	movl 8(%ebp), %ebx			# input filename into %ebx
	movl $O_RDONLY, %ecx			# read-only flag
	movl $0666, %edx			# no matter for reading
	int  $0x80				# call linux kernel

	cmpl $-1, %eax				# check for error condition
	je cp_end
	movl %eax, -16(%ebp)			# save the input fd

# open the output file (create)
	movl $5, %eax				# open file
	movl 12(%ebp), %ebx			# output filename into %ebx
	movl $O_CREAT_WRONLY_TRUNC, %ecx	# flags for writing to file
	movl $0666, %edx			# mode for new file (created)
	int  $0x80				# call linux

	cmpl $-1, %eax				# check for error condition
	je cp_close_in
	movl %eax, -20(%ebp)			# store the file descriptor

cp_loop_begin:
	movl -16(%ebp), %ebx			# get the input file desc.
	movl $buffer, %ecx			# location to read into
	movl $BUFFER_SIZE, %edx			# size of the buffer
	movl $3, %eax				# read it
	int  $0x80				# size of buffer read is
						# returned in %eax
# check for EOF condition
	cmpl $END_OF_FILE, %eax			# check for EOF or error
	jle  cp_loop_end			# if found, go to the end

# write to output file
	movl -20(%ebp), %ebx			# file to use
	movl $buffer, %ecx			# location of buffer
	movl %eax, %edx				# size of buffer
	movl $4, %eax				# will be written
	int  $0x80				# write it

	jmp cp_loop_begin			# continue again

cp_loop_end:
	movl -20(%ebp), %ebx			# output fd
	movl $6, %eax
	int  $0x80

cp_close_in:				# close input file
	movl -16(%ebp), %ebx		# input fd
	movl $6, %eax
	int  $0x80

# restore the general registers
	movl -4(%ebp), %ebx
	movl -8(%ebp), %ecx
	movl -12(%ebp), %edx
	jmp cp_end

cp_print_usage:
	pushl $cp_usage
	call print_str

cp_end:
# return to caller function

	movl %ebp, %esp
	popl %ebp
	ret

