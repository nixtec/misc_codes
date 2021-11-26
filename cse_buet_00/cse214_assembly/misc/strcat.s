# strcat.s
# catenates two strings
# the destination string must be larger enough

# data section
.data
msg:
        .ascii "Enter String : "
        len = . - msg
nl:	# the newline LF character
	.ascii "\n"

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
        call strcat		# request for catenation
        addl $8, %esp		# reset the stack pointer

# print out the catenated string
# first get the length of the string

# get the length of the catenated string
	pushl $dest
	call strlen		# calculate the length of 'dest'
	addl $4, %esp		# reset the stack pointer
	movl %eax, %edx		# save the length returned by strlen

# print it
	movl $4, %eax
	movl $1, %ebx
	movl $dest, %ecx
	int  $0x80

# print a newline
	movl $4, %eax
	movl $1, %ebx
	movl $nl, %ecx
	movl $1, %edx
	int  $0x80

# exit to shell
        xorl %ebx, %ebx		# exit status
        movl $1, %eax		# exit () syscall
        int  $0x80

# here goes the strcat function definition
.type strcat, @function
strcat:
# first push the useful registers
	pushl %ebp		# save the base pointer
	movl %esp, %ebp		# get the control of stack

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
#	movl 12(%ebp), %eax	# %eax will return the address of 'dest'
	movl %ebp, %esp
	popl %ebp		# restore the base pointer
        ret


# here goes the strlen function definition
.type strlen, @function
strlen:
        pushl %edi              # because we'll edit this
        movl 8(%esp), %edi      # load the pointer to %edi

        xorl %ecx, %ecx         # %ecx=0
        xorb %al, %al		# this is the terminating character
        notl %ecx               # %ecx=-1
        cld
        repne scasb             # repeat while '\0' not found
        notl %ecx               # not(-n) = |n|-1
        decl %ecx
        movl %ecx, %eax         # %eax contains the return value

        popl %edi
	ret

