# strcmp.s
# compare two strings
#
# Ayub <mrayub@gmail.com>
#

.text
.type strcmp, @function
# strcmp <s1> <s2>
.globl strcmp
strcmp:
	pushl %ebp
	movl %esp, %ebp

	subl $16, %esp		# make room for local use
	movl %ebx, -4(%ebp)
	movl %ecx, -8(%ebp)
	movl %esi, -12(%ebp)
	movl %edi, -16(%ebp)

	pushl 12(%ebp)		# s2
	call strlen
	movl %eax, %edi		# %edi=strlen (s2); src=s2
	pushl 8(%ebp)		# s1
	call strlen
	addl $8, %esp		# reset the stack pointer
	cmpl %eax, %edi		# compare the string lengths (%eax=strlen(s1))
	jne strcmp_not_equal	# not equal length (no testing required)
# if we reach here, we must compare the strings byte by byte
	xorl %esi, %esi		# the dest index
	xorl %edi, %edi		# the src index

	movl 8(%ebp), %ebx	# points to s1 (%ebx, %esi, 1)
	movl 12(%ebp), %ecx	# points to s2 (%ecx, %edi, 1)

# now do the assignment of second array to first array
strcmp_loop:
	movb (%ebx, %esi, 1), %ah	# s1[i]
	movb (%ecx, %edi, 1), %al	# s2[i]
	cmpb $0, %ah			# check for end of string
	je   strcmp_end
	cmpb $0, %al			# check for end of string
	je   strcmp_end
	cmpb %ah, %al
	jne   strcmp_end
	incl %esi
	incl %edi
	jmp strcmp_loop		# continue loop
strcmp_not_equal:
	subl %edi, %eax		# %eax=strlen(s1)-strlen(s2)
	jmp strcmp_exit
strcmp_end:
	subb %al, %ah		# s1[i] = s1[i] - s2[i]
	movsbl %ah, %eax	# copy byte to long (sign extension)
strcmp_exit:
	movl -4(%ebp), %ebx
	movl -8(%ebp), %ecx
	movl -12(%ebp), %esi
	movl -16(%ebp), %edi

	movl %ebp, %esp
	popl %ebp
	ret

