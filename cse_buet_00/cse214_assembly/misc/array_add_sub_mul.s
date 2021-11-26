# test.s
# a simple calculator implementation in assembly
# only 'add', 'sub' and 'mul' are possible right now
#
# copyright (c) Ayub <grub@programmer.net>
#
# written in the Vi editor

/*
  prototype of the functions used in this program:

  Array lengths are set global
  So, it doesn't need to be pushed onto the stack

  add_array (array1, array2)
  sub_array (array1, array2)
  mul_array (array1, array2)

  parameters must be pushed onto the stack in backward direction
*/

.data
/*
menu_msg:
	.ascii "Which operation you want to perform:\n"
	.ascii "<1> Addition\n"
	.ascii "<2> Subtraction\n"
	.ascii "<3> Multiplication\n"
	.ascii "Enter your choice: "
	len_menu_msg = . - menu_msg

choice:		# the last byte is for 'newline' which we will discard later
	.byte 0, 0
*/

msg1:
	.ascii "Enter first integer  : "
	len_msg1 = . - msg1

msg2:
	.ascii "Enter second integer : "
	len_msg2 = . - msg2

msg_add:
	.ascii "Addition of the numbers : "
	len_msg_add = . - msg_add

msg_sub:
	.ascii "Subtraction of the numbers : "
	len_msg_sub = . - msg_sub

msg_mul:
	.ascii "Multiplication of the numbers : "
	len_msg_mul = . - msg_mul


nl:
	.ascii "\n"

array1:
	.rept 100
	.byte 0		# initialize the array with NUL (0)
	.endr

arraylen1:		# first array length, byte for saving memory
	.byte 100

array2:
	.rept 100
	.byte 0		# initialize the array with NUL (0)
	.endr

arraylen2:
	.byte 100	# second array length

divby:
	.byte 0		# divide by (10)

temp:
	.byte 0		# temporary variable

array3:
	.rept 201
	.byte 0
	.endr

arraylen3:
	.byte 201

isneg:			# will the resultant number be nagative?
	.byte 0


.text
.globl _start
_start:

/*
# display the menu
	movl $4, %eax		# write () syscall number
	movl $1, %ebx		# stdout file descriptor value
	movl $menu_msg, %ecx
	movl $len_menu_msg, %edx
	int  $0x80

# now get the choice from the user
	movl $3, %eax		# the read() syscall number
	movl $0, %ebx		# the stdin file descriptor
	movl $array1, %ecx	# the buffer to get input into
	movl $1, %edx		# maximum size of input
	int  $0x80		# invoke the kernel
	decl %eax		# discard the newline
	movb %al, arraylen1	# save the array length (returned in %eax)
*/

# display the first input message
	movl $4, %eax		# write() syscall number
	movl $1, %ebx		# stdout file descriptor value
	movl $msg1, %ecx	# the message pointer
	movl $len_msg1, %edx	# message length
	int  $0x80		# invoke the kernel


# now get the input from the user
	movl $3, %eax		# the read() syscall number
	movl $0, %ebx		# the stdin file descriptor
	movl $array1, %ecx	# the buffer to get input into
	movl arraylen1, %edx	# maximum size of input
	int  $0x80		# invoke the kernel
	decl %eax		# discard newline
	movb %al, arraylen1	# save the array length (returned in %eax)

# display the second input message
	movl $4, %eax		# write() syscall number
	movl $1, %ebx		# stdout file descriptor value
	movl $msg2, %ecx	# the message pointer
	movl $len_msg2, %edx	# message length
	int  $0x80		# invoke the kernel

# now get the input from the user
	movl $3, %eax		# the read() syscall number
	movl $0, %ebx		# the stdin file descriptor
	movl $array2, %ecx      # the buffer to get input into
	movl arraylen2, %edx	# maximum size of input
	int  $0x80		# invoke the kernel
	decl %eax		# discard newline
	movb %al, arraylen2	# save the array length (returned in %eax)

	movb arraylen1, %al
	movb arraylen2, %bl

/*
# take action according to choice
	movb choice, %cl
	cmpb '1', %cl
	je   calc_array3_len_add
	cmpb '2', %cl
	je   calc_array3_len_sub
	cmpb '3', %cl
	je   calc_array3_len_mul
# if invalid choice input supplied
	jmp  exit_to_shell
*/


/*
# array length calculation for resultant array for addition
calc_array3_len_add:
	cmpb %bl, %al
	jg   assign_len1_add
	incb %bl
	movb %bl, arraylen3
	jmp  endif
assign_len1_add:
	incb %al
        movb %al, arraylen3
	jmp  endif
*/



# array length calculation for resultant array for subtraction
calc_array3_len_sub:
	cmpb %bl, %al
	jg   assign_len1_sub
	movb %bl, arraylen3
	jmp  endif
assign_len1_sub:
        movb %al, arraylen3
	jmp  endif


/*
# array3 length calculation for resultant multiplication array
calc_array3_len_mul:
	addb %bl, %al
	movb %al, arraylen3
*/



endif:
	pushl $array2
	pushl $array1

#	call add_array
	call sub_array
#	call mul_array

	addl $8, %esp		# reset the stack pointer

/*
# print the addition message
        movl $4, %eax		# write() syscall number
        movl $1, %ebx		# stdout file descriptor value
        movl $msg_add, %ecx	# the message pointer
        movl $len_msg_add, %edx	# message length
        int  $0x80		# invoke the kernel
*/


# print the subtruction message
        movl $4, %eax		# write() syscall number
        movl $1, %ebx		# stdout file descriptor value
        movl $msg_sub, %ecx	# the message pointer
        movl $len_msg_sub, %edx	# message length
        int  $0x80		# invoke the kernel

/*
# print the multiplication message
        movl $4, %eax		# write() syscall number
        movl $1, %ebx		# stdout file descriptor value
        movl $msg_mul, %ecx	# the message pointer
        movl $len_msg_mul, %edx	# message length
        int  $0x80		# invoke the kernel
*/

# display the resultant array
        movl $4, %eax		# write() syscall number
        movl $1, %ebx		# stdout file descriptor value
        movl $array3, %ecx	# the message pointer
        movl arraylen3, %edx	# array length
        int  $0x80		# invoke the kernel

# print a newline
	movl $4, %eax		# write() syscall number
	movl $1, %ebx		# stdout file descriptor value
	movl $nl, %ecx		# the message pointer
	movl $1, %edx		# message length
	int  $0x80		# invoke the kernel

exit_to_shell:			# just a label so that anybody can jump to here
	movl $1, %eax		# the exit() system call
	movl $0, %ebx		# exit status
	int  $0x80

/*
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
	jge  add_if
	movl %ebx, %edi		# k = i
	incl %edi
	jmp  add_loop_start
add_if:			# k = j
	movl %ecx, %edi
	incl %edi		# resultant array may be one byte big

# now start the real work
add_loop_start:
	cmpl $0, %edi
	jle  add_loop_end
# check for underflow condition
add_check_array1_index:
	cmpl $0, %ebx			# if array1 reaches negative index
	jl   add_array1_idx_neg
	movb array1(, %ebx, 1), %ah
	subb $48, %ah			# convert to real value
add_check_array2_index:
	cmpl $0, %ecx
	jl   add_array2_idx_neg
	movb array2(, %ecx, 1), %al
	subb $48, %al			# convert to real value
	jmp  add_proceed
add_array1_idx_neg:
	xorb %ah, %ah			# %ah=0
	jmp  add_check_array2_index
add_array2_idx_neg:
	xorb %al, %al			# %al=0

add_proceed:
	addb %ah, %al
	addb temp, %al
	xorb %ah, %ah			# makes %ax = %al (upper half 0)
	movb $10, divby			# divide by what?
	divb divby
	movb %al, temp			# quotient
	addb $48, %ah
	movb %ah, array3(, %edi, 1)	# remainder
	decl %ebx
	decl %ecx
	decl %edi
	jmp  add_loop_start

add_loop_end:
# test if carry really exists
	cmpb $0, temp
	je   add_array_end
	movb temp, %al	# prepare for placing the carry at the first position
	addb $48, %al		# convert it to character
	movb %al, array3(, %edi, 1)	# place it now

add_array_end:
# do the ending tasks
	movl %ebp, %esp
	popl %ebp
	ret
*/


# sub_array: array subtraction function
# here digit by digit subtraction will never be with two digits
# so we need not have to divide the result for carry here
# so we can use the %edx register here referencing the first array
# and we can also use %esi register here for referencing the second array
# both array will be pushed onto the stack
# %edi is being used for indexing the resultant array
.type sub_array, @function
sub_array:
# do the initialization tasks
	pushl %ebp		# save the base pointer
	movl %esp, %ebp		# so that we don't accidentally change stack

	movl 8(%ebp), %edx	# get the reference of the first array
	movl 12(%ebp), %esi	# get the reference of the second array

	movzbl arraylen1, %ebx		# i=len1
	decl %ebx			# for indexing
	movzbl arraylen2, %ecx		# j=len2
	decl %ecx			# for indexing

	movzbl arraylen3, %edi
	decl %edi
/*
	cmpl %ebx, %ecx
	jge  sub_if
	movl %ebx, %edi		# k = i
	jmp  sub_loop_start
sub_if:			# k = j
	movl %ecx, %edi
*/

sub_loop_start:
	cmpl $0, %edi		# check underflow condition of the resultant array
	jl   sub_loop_end
# check for underflow condition
sub_check_array1_index:
	cmpl $0, %ebx			# if array1 reaches negative index
	jl   sub_array1_idx_neg
	movb (%edx, %ebx, 1), %al
	subb $48, %al			# convert to numeric value
sub_check_array2_index:
	cmpl $0, %ecx
	jl   sub_array2_idx_neg
	movb (%esi, %ecx, 1), %ah
	subb $48, %ah			# convert to numeric value
	jmp  sub_proceed
sub_array1_idx_neg:
	xorb %al, %al
	jmp  sub_check_array2_index
sub_array2_idx_neg:
	xorb %ah, %ah

# everything is tested, now do the real work
sub_proceed:
	addb temp, %ah			# add the carry
	movb $0, temp			# so that it doesn't create any problem next time
	cmpb %al, %ah
	jg   sub_do_something
sub_marker:		# 'sub_do_something' will jump again here
	subb %ah, %al			# a[i] = a[i] - b[j]; %al = %al - %ah
	xorb %ah, %ah			# makes %ax = %al (upper half 0)
	jmp  sub_next
sub_do_something:
	addb $10, %al			# so that we can subtract %al from it
	movb $1, temp			# update the carry
	jmp sub_marker

sub_next:
	addb $48, %al			# convert it to char
	movb %al, array3(, %edi, 1)	# save it to resultant array
	decl %ebx
	decl %ecx
	decl %edi
	jmp  sub_loop_start

sub_loop_end:

sub_array_end:
# do the ending tasks
	movl %ebp, %esp
	popl %ebp
	ret


/*
# mul_array: array multiplication function
.type mul_array, @function
mul_array:
# do the initialization tasks
	pushl %ebp		# save the base pointer
	movl %esp, %ebp		# so that we don't accidentally change stack

	movzbl arraylen1, %ebx		# i=len1
	decl %ebx			# for indexing
	movzbl arraylen2, %ecx		# j=len2
	decl %ecx			# for indexing

	cmpl %ebx, %ecx
	jge  mul_if
	movl %ebx, %edi		# k = i
	incl %edi
	jmp  mul_loop_start
mul_if:			# k = j
	movl %ecx, %edi
	incl %edi		# resultant array may be one byte big

mul_loop_start:
	cmpl $0, %edi
	jle  mul_loop_end
# check for underflow condition
mul_check_array1_index:
	cmpl $0, %ebx			# if array1 reaches negative index
	jl   mul_array1_idx_neg
	movb array1(, %ebx, 1), %ah
	subb $48, %ah			# convert to real value
mul_check_array2_index:
	cmpl $0, %ecx
	jl   mul_array2_idx_neg
	movb array2(, %ecx, 1), %al
	subb $48, %al			# convert to real value
	jmp  mul_proceed
mul_array1_idx_neg:
	xorb %ah, %ah
	jmp  mul_check_array2_index
mul_array2_idx_neg:
	xorb %al, %al

mul_proceed:
	addb %ah, %al
	addb temp, %al
	xorb %ah, %ah			# makes %ax = %al (upper half 0)
	movb $10, divby			# divide by what?
	divb divby
	movb %al, temp			# quotient
	addb $48, %ah
	movb %ah, array3(, %edi, 1)	# remainder
	decl %ebx
	decl %ecx
	decl %edi
	jmp  mul_loop_start

mul_loop_end:
# test if carry really exists
	cmpb $0, temp
	je mul_array_end
	movb temp, %al	# prepare for placing the carry at the first position
	addb $48, %al		# convert it to character
	movb %al, array3(, %edi, 1)	# place it now

mul_array_end:
# do the ending tasks
	movl %ebp, %esp
	popl %ebp
	ret
*/

