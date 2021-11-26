# binsearch.s
# this program has been written in AT&T syntax which is widely used in UNIX
# copyright (c) Ayub <grub@programmer.net>
# a simple binary search program that finds out an integer value
# from an integer array.
# it returns the position of the number (index + 1) if found
# otherwise, it returns zero (which means no such value found)
# function call: binsearch x v n [parameters will be pushed onto the stack]

.data

v:	# the array 'v'
	.long 23, 45, 57, 58, 60, 65, 67, 89, 95, 99
x:	# this value should be searched in the array
	.long 65		# change this value to test the program
n:	# size of the array
	.long 10

.text

.globl _start		# entry point of the program (ELF)
_start:
	pushl n			# size of the array
	pushl $v		# pointer to the array
	pushl x			# this value should be searched
	call binsearch		# search the value and return something
	addl $12, %esp		# move the stack pointer back

	movl %eax, %ebx		# return value goes in %ebx from %eax
	movl $1, %eax		# exit () syscall
	int $0x80		# invoke the kernel


.type binsearch, @function
binsearch:
	binsearch_init:
		pushl %ebp		# save the old base pointer
		movl %esp, %ebp		# take control of the stack
		subl $12, %esp		# reserve space for variables

	binsearch_start:
		movl $0, -4(%ebp)	# low = 0
		movl 16(%ebp), %edx	# temporary move of n
		movl %edx, -8(%ebp)	# high = n
		decl -8(%ebp)		# high-1
		movl $0, %edi		# initialize index for array

	binsearch_loop_start:
		movl -4(%ebp), %ebx	# assign temporarily the 'low'
					# as two addresses in the same
					# stack can't be
					# accessed directly
		cmpl -8(%ebp), %ebx	# compare 'high' and 'low'
		jg binsearch_loop_end_not_found		# if low > high
		movl %ebx, %eax		# %eax = low
		addl -8(%ebp), %eax	# %eax = high + low
		movl $0, %edx		# it must be set to 0
					# so that the remainder can be
					# saved here.
					# DON'T FORGET IT
					# if you don't do it, you'll
					# get a SIGFPE signal which will
					# terminate the application
					# I'V GOT IT AFTER DEBUGGING

		movl $2, %ecx		# %eax will be divided with this value
		idiv %ecx		# (high + low) / 2; (high+low) in %eax
		movl %eax, -12(%ebp)	# get the quotient (mid)
		movl %eax, %edi		# also save it as index
		movl 8(%ebp), %ebx	# assign x temporarily
		cmpl v(, %edi, 4), %ebx		# compare v[mid] and x
		jl h_mid_m1			# calculate high = mid -1
		cmpl v(, %edi, 4), %ebx		# compare v[mid] and x
		jg l_mid_p1			# calculate low = mid + 1
		jmp binsearch_loop_end_found	# match found

	h_mid_m1:			# assigns high = mid - 1
		movl -8(%ebp), %ebx
		movl -12(%ebp), %ebx	# high = mid
		decl -8(%ebp)			# high = high - 1
		jmp binsearch_loop_start	# loop from the beginning

	l_mid_p1:			# assigns low = mid + 1
		movl -12(%ebp), %ebx
		movl %ebx, -4(%ebp)	# low = mid
		incl -4(%ebp)			# low = low + 1
		jmp binsearch_loop_start	# loop from the beginning

	binsearch_loop_end_found:
		movl %edi, %eax		# return value goes in %eax
		incl %eax
		jmp binsearch_end	# do shutdown tasks

	binsearch_loop_end_not_found:
		movl $0, %eax		# value not found
		jmp binsearch_end	# do shutdown tasks

	binsearch_end:
		# the return value is already saved in %eax
		movl %ebp, %esp		# restore the stack pointer
		popl %ebp		# restore the base pointer
		ret

