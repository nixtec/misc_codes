# strcpy.s
# copy one string into another
#
# Ayub <mrayub@gmail.com>
#

# here goes the strcpy function definition
# strcpy <dest> <src>
# copy <src> to <dest>
.type strcpy, @function
.globl strcpy
strcpy:
# if you need %esi, %edi intact,
# push them onto stack and change the following as necessary
	pushl %ebp
	movl %esp, %ebp

	subl $16, %esp		# make room for temporary push
	movl %ebx, -4(%ebp)
	movl %ecx, -8(%ebp)
	movl %esi, -12(%ebp)
	movl %edi, -16(%ebp)

        movl 8(%ebp), %eax	# points to dest (%eax, %esi, 1)
	movl 12(%ebp), %ebx	# points to src (%ebx, %edi, 1)
	xorl %esi, %esi		# the dest index
	xorl %edi, %edi		# the src index

# now do the assignment of second array to first array
strcpy_loop:
	movb (%ebx, %edi, 1), %cl	# for temporary assignment
	movb %cl, (%eax, %esi, 1)
	cmpb $0, %cl		# check if null character of 'src' reached
	je   strcpy_end
	incl %esi
	incl %edi
	jmp strcpy_loop		# continue loop

strcpy_end:
	movl -4(%ebp), %ebx
	movl -8(%ebp), %ecx
	movl -12(%ebp), %esi
	movl -16(%ebp), %edi

	movl %ebp, %esp		# reset the stack pointer
	popl %ebp
        ret

