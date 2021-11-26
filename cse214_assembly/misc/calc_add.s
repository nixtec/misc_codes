# calc_add.s
# a simple calculator implementation in assembly
# this program calculates only addition of two numbers
.data
msg1:
	.ascii "Enter first integer     : "
	lenmsg1 = . - msg1
msg2:
	.ascii "Enter second integer    : "
	lenmsg2 = . - msg2
msg3:
	.ascii "Addition of the numbers : "
	lenmsg3 = . - msg3
nl:
	.ascii "\n"
array1:
	.rept 100
	.byte 0		# initialize the array with NUL (0)
	.endr
arraylen1:		# first arrar length, byte for saving memory
	.byte 0
array2:
	.rept 100
	.byte 0		# initialize the array with NUL (0)
	.endr
arraylen2: .byte 0	# second array length
divby: .byte 0		# divide by (10)
temp: .byte 0		# temporary variable
array3:
	.rept 101
	.byte 0
	.endr
arraylen3:
	.byte 0

.text
.globl _start
_start:
# display the first input message
	movl $4, %eax		# write() syscall number
	movl $1, %ebx		# stdout file descriptor value
	movl $msg1, %ecx	# the message pointer
	movl $lenmsg1, %edx	# message length
	int $0x80		# invoke the kernel

# now get the input from the user
	movl $3, %eax		# the read() syscall number
	movl $0, %ebx		# the stdin file descriptor
	movl $array1, %ecx	# the buffer to get input into
	movl $100, %edx		# maximum size of input
	int $0x80		# invoke the kernel
	movb %al, arraylen1	# save the array length (returned in %eax)
	decb arraylen1		# discard newline

# display the second input message
        movl $4, %eax           # write() syscall number
        movl $1, %ebx           # stdout file descriptor value
        movl $msg2, %ecx        # the message pointer
        movl $lenmsg2, %edx     # message length
        int $0x80               # invoke the kernel

# now get the input from the user
        movl $3, %eax           # the read() syscall number
        movl $0, %ebx           # the stdin file descriptor
        movl $array2, %ecx      # the buffer to get input into
        movl $100, %edx          # maximum size of input
        int $0x80               # invoke the kernel
        movb %al, arraylen2	# save the array length (returned in %eax)
	decb arraylen2		# discard newline

calc_array3_len:
	movb arraylen1, %al
	movb arraylen2, %bl
        cmpb %bl, %al
        jge assign_len1
	movb %bl, arraylen3
	incb arraylen3
	jmp next
assign_len1:
        movb %al, arraylen3
        incb arraylen3
	jmp endif
next:
        movl %ecx, %edi
        inc %edi                # resultant array may be one byte big

endif:
	call add_array
#	call sub_array
#	call mul_array
#	call div_array

# print the addition message
        movl $4, %eax           # write() syscall number
        movl $1, %ebx           # stdout file descriptor value
        movl $msg3, %ecx        # the message pointer
        movl $lenmsg3, %edx     # message length
        int $0x80               # invoke the kernel

# display the resultant array
        movl $4, %eax           # write() syscall number
        movl $1, %ebx           # stdout file descriptor value
        movl $array3, %ecx	# the message pointer
        movl arraylen3, %edx	# array length
        int $0x80		# invoke the kernel

# print a newline
        movl $4, %eax		# write() syscall number
        movl $1, %ebx		# stdout file descriptor value
        movl $nl, %ecx		# the message pointer
        movl $1, %edx		# message length
        int $0x80		# invoke the kernel

# now exit to shell
	movl $1, %eax		# the exit() system call
	movl $0, %ebx		# exit status
	int $0x80

# add_array: array addition function
.type add_array, @function
add_array:
# do the initialization tasks
	pushl %ebp		# save the base pointer
	movl %esp, %ebp		# so that we don't accidentally change stack

	movzbl arraylen1, %ebx		# i=len1
	decl %ebx			# for indexing
	movzbl arraylen2, %ecx		# j=len2
	decl %ecx			# for indexing

	cmpl %ebx, %ecx
	jge if
	movl %ebx, %edi		# k = i
	incl %edi
	jmp loop_start
if:			# k = j
	movl %ecx, %edi
	incl %edi		# resultant array may be one byte big

loop_start:
	cmpl $0, %edi
	jle loop_end
# check for underflow condition
check_array1_index:
	cmpl $0, %ebx			# if array1 reaches negative index
	jl array1_idx_neg
	movb array1(, %ebx, 1), %ah
	subb $48, %ah			# convert to real value
check_array2_index:
	cmpl $0, %ecx
	jl array2_idx_neg
	movb array2(, %ecx, 1), %al
	subb $48, %al			# convert to real value
	jmp proceed
array1_idx_neg:
	movb $0, %ah
	jmp check_array2_index
array2_idx_neg:
	movb $0, %al

proceed:
	addb %ah, %al
	addb temp, %al
	movb $0, %ah			# makes %ax = %al (upper half 0)
	movb $10, divby			# divide by what?
	divb divby
	movb %al, temp			# quotient
	addb $48, %ah
	movb %ah, array3(, %edi, 1)	# remainder
	decl %ebx
	decl %ecx
	decl %edi
	jmp loop_start

loop_end:
# test if carry really exists
	cmpb $0, temp
	je add_array_end
	movb temp, %al	# prepare for placing the carry at the first position
	addb $48, %al		# convert it to character
	movb %al, array3(, %edi, 1)	# place it now

add_array_end:
# do the ending tasks
	movl %ebp, %esp
	popl %ebp
	ret

