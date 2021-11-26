# find.s
# find file
# find <file>	[wildcard allowed]
# if you use wildcard, you must supply it as <*pattern>
# this means that you must use the wildcard character at the starting
#
# Ayub <mrayub@gmail.com>
#

.data
.equ NAME_MAX, 256	# maximum directory name length
struct_dirent:
	.long 0		# inode number (no need for now)
	.long 0		# offset to this dirent (no need for now)
d_reclen:
	.short 0	# length of the d_name
d_name:		# name of the directory
	.rept 100	# because it is not absolute filename
	.asciz ""
	.endr

abs_filename:
	.rept NAME_MAX
	.asciz ""
	.endr

struct_stat:
	st_dev:		.short 0	# device
	st_ino:		.short 0	# inode
	st_mode:	.short 0	# protection
	st_nlink:	.short 0	# #of hard links
	st_uid:		.short 0	# user id
	st_gid:		.short 0	# group id
	st_rdev:	.short 0	# device type (if inode device)
	st_dummy:	.short 0	# dummy value, used for alignment (2^n)
	st_size:	.long 0		# total size (in bytes)
	st_atime:	.long 0		# time last accessed
	st_mtime:	.long 0		# time last modified
	st_ctime:	.long 0		# time last changed (permission)

current_dir:
	.asciz "."

find_usage:
	.asciz "Usage: find <file>\n"

.text
# find <file>		[wildcard allowed]
.type find, @function
.globl find
.equ O_DIRECTORY, 020000	# is it a directory? syscall will fail if not
.equ READDIR, 89	# old_readdir() syscall number
.equ S_IRWXU, 00700
find:
	pushl %ebp
	movl %esp, %ebp

	pushl 8(%ebp)
	call strlen
	cmpl $0, %eax
	je find_print_usage
#	addl $4, %esp

# save the general registers
#	subl $16, %esp		# make room for registers
	subl $12, %esp		# $4 is already subtracted
	movl %ebx, -4(%ebp)
	movl %ecx, -8(%ebp)
	movl %edx, -12(%ebp)
	movl %esi, -16(%ebp)

# check if wildcard symbol is given
	xorl %ebx, %ebx
	movl 8(%ebp), %esi
	cmpb $'*', (%esi, %ebx, 1)
	je find_wildcard_given
	jmp find_wildcard_test_ok
find_wildcard_given:
	incl %esi		# we now discard wildcard
find_wildcard_test_ok:

	movl $current_dir, %ebx
find_assigned:
	movl $5, %eax		# open() syscall
	movl $O_DIRECTORY, %ecx	# descriptor must be a directory
	movl $S_IRWXU, %edx
	int  $0x80

	cmpl $-1, %eax		# check for success
	je   find_restore
	movl %eax, %ebx		# save directory file descriptor

find_loop_start:
	movl $READDIR, %eax
	movl $struct_dirent, %ecx
	movl $1, %edx		# count (ignored)
	int  $0x80

	cmpl $0, %eax		# check for error
	jl   find_restore	# error
	cmpl $0, %eax		# check for end of directory
	je   find_loop_end
# success
	movl $d_name, %edi
	movzwl d_reclen, %eax
	addl %eax, %edi		# %edi now points to the NUL of d_name
	pushl %esi
	call strlen
	subl %eax, %edi
	addl $4, %esp

	pushl %esi
	pushl %edi
	call strcmp
	addl $8, %esp		# reset the stack pointer
	cmpl $0, %eax
	jne  find_loop_start	# continue search; file not found
	pushl $d_name
	call print_str		# search matched; print the file name
	call print_nl
	addl $4, %esp		# reset the stack pointer
	jmp find_loop_start

find_loop_end:
# close the directory
# %ebx already contains the descriptor
	movl $6, %eax
	int  $0x80

find_restore:
	movl -4(%ebp), %ebx
	movl -8(%ebp), %ecx
	movl -12(%ebp), %edx
	movl -16(%ebp), %esi
	jmp find_end

find_print_usage:
	pushl $find_usage
	call print_str

find_end:
	movl %ebp, %esp
	popl %ebp
	ret

