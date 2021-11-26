# strstr.s
# finds needle in haystack

# data section
.data
msg:
        .ascii "Enter String : "
        len = . - msg
nl:	# the newline LF character
	.ascii "\n"
len_dest:	# length of the haystack
	.long 0
len_src:	# length of the needle
	.long 0


# bss section
.bss            # uninitialized data goes here
.equ SRC_LEN, 50
.equ DEST_LEN, 100
.lcomm dest, DEST_LEN
.lcomm src, SRC_LEN

# code section
.text
.globl _start
_start:
        movl $4, %eax           # write() syscall
        movl $1, %ebx           # stdout
        movl $msg, %ecx         # string to be printed
        movl $len, %edx         # length of the string
        int  $0x80              # call kernel

        movl $3, %eax
        movl $0, %ebx
        movl $dest, %ecx	# input buffer
        movl $DEST_LEN, %edx	# maximum length of input
        int  $0x80
	decl %eax		# for indexing to the last character
	movb $0, dest(, %eax, 1) # set the terminating null (overwrite \n)

        movl $4, %eax           # write() syscall
        movl $1, %ebx           # stdout
        movl $msg, %ecx         # string to be printed
        movl $len, %edx         # length of the string
        int  $0x80              # call kernel

        movl $3, %eax
        movl $0, %ebx
        movl $src, %ecx		# input buffer
        movl $SRC_LEN, %edx	# maximum length of input
        int  $0x80
	decl %eax		# for indexing to the last character
	movb $0, src(, %eax, 1)	# set the terminating null (overwrite \n)

        pushl $src
	pushl $dest
        call strcmp		# request for comparison
        addl $8, %esp		# reset the stack pointer

	movl %eax, %ecx

	movl $4, %eax
	movl $1, %ebx
	int  $0x80

# exit to shell
        xorl %ebx, %ebx		# exit status
        movl $1, %eax		# exit () syscall
        int  $0x80

# here goes the strstr function definition
.type strstr, @function
strstr:
	movl 4(%esp), %esi	# the haystack pointer
	movl 8(%esp), %edi	# the needle pointer
	pushl %esi
	call strlen
	addl $4, %esp		# reset the stack pointer
	movl %eax, len_dest
	pushl %edi
	call strlen
	addl $4, %esp		# reset the stack pointer
	movl %eax, len_src

strstr_loop:
# this loop will call strcmp function many times

# here goes the strcmp function definition
.type strcmp, @function
strcmp:
# if you need %esi, %edi intact,
# push them onto stack and change the following as necessary
        movl 4(%esp), %eax	# points to dest (%eax, %esi, 1)
	movl 8(%esp), %ebx	# points to src (%ebx, %edi, 1)
	xorl %esi, %esi		# the dest index
	xorl %edi, %edi		# the src index

# now do the assignment of second array to first array
strcmp_loop:
	movb (%eax, %esi, 1), %ch	# s1[i]
	movb (%ebx, %edi, 1), %cl	# s2[i]
	cmpb $0, %ch			# check for end of string
	je   strcmp_end
	cmpb $0, %cl			# check for end of string
	je   strcmp_end
	cmpb %ch, %cl
	jne   strcmp_end
	incl %esi
	incl %edi
	jmp strcmp_loop		# continue loop

strcmp_end:
	subb %cl, %ch		# s1[i] = s1[i] - s2[i]
	movsbl %ch, %eax	# copy byte to long (sign extension)
        ret

