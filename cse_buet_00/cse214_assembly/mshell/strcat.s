# strcat.s
# catenates two strings
# the destination string must be larger enough
#
# Ayub <mrayub@gmail.com>
#

# the strcat function definition

.type strcat, @function
.globl strcat
strcat:
# first push the useful registers
	pushl %ebp		# save the base pointer
	movl %esp, %ebp		# get the control of stack

	subl $16, %esp
	movl %ebx, -4(%ebp)
	movl %ecx, -8(%ebp)
	movl %esi, -12(%ebp)
	movl %edi, -16(%ebp)

# if you need %esi, %edi intact,
# push them onto stack and change the following as necessary
        movl 8(%ebp), %eax	# points to dest (%eax, %esi, 1)
	movl 12(%ebp), %ebx	# points to src (%ebx, %edi, 1)
	xorl %esi, %esi		# the dest index
	xorl %edi, %edi		# the src index

strcat_dest_loop:		# loop to the end of 'dest' first
	cmpb $0, (%eax, %esi, 1)	# NUL reached?
	je   strcat_loop
	incl %esi
	jmp strcat_dest_loop

# now do the assignment of second array to first array
strcat_loop:
	movb (%ebx, %edi, 1), %cl	# for temporary assignment
	movb %cl, (%eax, %esi, 1)
	cmpb $0, %cl		# check if null character of 'src' reached
	je   strcat_end
	incl %esi
	incl %edi
	jmp strcat_loop		# continue loop

strcat_end:
	movl -4(%ebp), %ebx
	movl -8(%ebp), %ecx
	movl -12(%ebp), %esi
	movl -16(%ebp), %edi

	movl %ebp, %esp
	popl %ebp		# restore the base pointer
        ret

