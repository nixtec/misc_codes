# ls.s
# the file statistics function
# stat <filename>
# dependency: print_str.s
#
# Ayub <mrayub@gmail.com>
#

.data
.equ NAME_MAX, 256	# maximum directory name length
struct_dirent:
	.long 0		# inode number (no need for now)
	.long 0		# offset to this dirent (no need for now)
	.short 0	# length of the d_name (no need for now)
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

PERMISSION:
	.asciz "----------"

.text
.type stat, @function
.globl stat
.equ STAT, 18		# sys_stat () syscall number
stat:
# the following flags are defined for the st_mode filed:
.equ S_IFMT,   0170000		# bitmask for file type bitfields
.equ S_IFSOCK, 0140000
.equ S_IFLNK,  0120000
.equ S_IFREG,  0100000
.equ S_IFBLK,  0060000
.equ S_IFDIR,  0040000
.equ S_IFCHR,  0020000
.equ S_IFIFO,  0010000

.equ S_IRWXU,  00700		# mask for file owner permission
.equ S_IRUSR,  00400
.equ S_IWUSR,  00200
.equ S_IXUSR,  00100

.equ S_IRWXG,  00070		# mask for file group permission
.equ S_IRGRP,  00040
.equ S_IWGRP,  00020
.equ S_IXGRP,  00010

.equ S_IRWXO,  00007		# mask for others permission (not in group)
.equ S_IROTH,  00004
.equ S_IWOTH,  00002
.equ S_IXOTH,  00001

	pushl %ebp
	movl %esp, %ebp

	subl $8, %esp
	movl %ebx, -4(%ebp)
	movl %ecx, -8(%ebp)

	movl $STAT, %eax	# stat() syscall
	movl 8(%ebp), %ebx	# filename
	movl $struct_stat, %ecx	# struct_stat pointer
	int  $0x80

	cmpl $0, %eax
	jne stat_end

stat_verify_type:
	movzwl st_mode, %ebx	# zero extend from short to long
	andl $S_IFMT, %ebx	# mask for file type

	cmpl $S_IFSOCK, %ebx	# is it a socket file
	je stat_assign_sock
	cmpl $S_IFLNK, %ebx	# is it a link file
	je stat_assign_lnk
	cmpl $S_IFREG, %ebx	# is it a regular file
	je stat_assign_reg
	cmpl $S_IFBLK, %ebx	# is it a block device file
	je stat_assign_blk
	cmpl $S_IFDIR, %ebx	# is it a directory file
	je stat_assign_dir
	cmpl $S_IFCHR, %ebx	# is it a character device file
	je stat_assign_chr
	cmpl $S_IFIFO, %ebx	# is it a FIFO (pipe) file
	je stat_assign_fifo
	jmp stat_assign_unknown	# unknown type

stat_assign_sock:	# socket (s)
	movl $0, %eax
	movb $'s', PERMISSION(, %eax, 1)
	jmp stat_verify_type_ok

stat_assign_lnk:	# symbolic link (l)
	movl $0, %eax
	movb $'l', PERMISSION(, %eax, 1)
	jmp stat_verify_type_ok

stat_assign_reg:	# regular file (-)
	movl $0, %eax
	movb $'-', PERMISSION(, %eax, 1)
	jmp stat_verify_type_ok

stat_assign_blk:	# block device (b)
	movl $0, %eax
	movb $'b', PERMISSION(, %eax, 1)
	jmp stat_verify_type_ok

stat_assign_dir:	# dir file (d)
	movl $0, %esi
	movb $'d', PERMISSION(, %esi, 1)
	jmp stat_verify_type_ok

stat_assign_fifo:	# fifo file (p)
	movl $0, %eax
	movb $'p', PERMISSION(, %eax, 1)
	jmp stat_verify_type_ok

stat_assign_chr:
	movl $0, %eax
	movb $'c', PERMISSION(, %eax, 1)
	jmp stat_verify_type_ok

stat_assign_unknown:

	movl $0, %eax
	movb $'?', PERMISSION(, %eax, 1)
#	jmp stat_verify_type_ok

stat_verify_type_ok:

stat_verify_owner_perm:		# now check for owner permission
	movzwl st_mode, %ebx	# zero extend from short to long
	andl $S_IRWXU, %ebx	# mask for file owner permission

	movl %ebx, %ecx		# save for later use

stat_verify_rusr:
	andl $S_IRUSR, %ebx
	cmpl $0, %ebx
	jne stat_assign_rusr	# use has read permission
	movl $1, %eax
	movb $'-', PERMISSION(, %eax, 1)

stat_verify_wusr:
	movl %ecx, %ebx
	andl $S_IWUSR, %ebx
	cmpl $0, %ebx
	jne stat_assign_wusr
	movl $2, %eax
	movb $'-', PERMISSION(, %eax, 1)

stat_verify_xusr:
	movl %ecx, %ebx
	andl $S_IXUSR, %ebx
	cmpl $0, %ebx
	jne stat_assign_xusr
	movl $3, %eax
	movb $'-', PERMISSION(, %eax, 1)
	jmp stat_verify_owner_perm_ok

stat_assign_rusr:
	movl $1, %eax
	movb $'r', PERMISSION(, %eax, 1)
	jmp stat_verify_wusr

stat_assign_wusr:
	movl $2, %eax
	movb $'w', PERMISSION(, %eax, 1)
	jmp stat_verify_xusr

stat_assign_xusr:
	movl $3, %eax
	movb $'x', PERMISSION(, %eax, 1)
#	jmp stat_verify_owner_perm_ok

stat_verify_owner_perm_ok:	# owner permission verification ok

stat_verify_group_perm:		# now check for group permission
	movzwl st_mode, %ebx	# zero extend from short to long
	andl $S_IRWXG, %ebx	# mask for file group permission

	movl %ebx, %ecx		# save for later use

stat_verify_rgrp:
	andl $S_IRGRP, %ebx
	cmpl $0, %ebx
	jne stat_assign_rgrp	# group has read permission
	movl $4, %eax
	movb $'-', PERMISSION(, %eax, 1)
	
stat_verify_wgrp:
	movl %ecx, %ebx
	andl $S_IWGRP, %ebx
	cmpl $0, %ebx
	jne stat_assign_wgrp
	movl $5, %eax
	movb $'-', PERMISSION(, %eax, 1)

stat_verify_xgrp:
	movl %ecx, %ebx
	andl $S_IXGRP, %ebx
	cmpl $0, %ebx
	jne stat_assign_xgrp
	movl $6, %eax
	movb $'-', PERMISSION(, %eax, 1)
	jmp stat_verify_group_perm_ok

stat_assign_rgrp:
	movl $4, %eax
	movb $'r', PERMISSION(, %eax, 1)
	jmp stat_verify_wgrp

stat_assign_wgrp:
	movl $5, %eax
	movb $'w', PERMISSION(, %eax, 1)
	jmp stat_verify_xgrp

stat_assign_xgrp:
	movl $6, %eax
	movb $'x', PERMISSION(, %eax, 1)
#	jmp stat_verify_group_perm_ok

stat_verify_group_perm_ok:	# group permission verification ok

stat_verify_other_perm:		# now check for group permission
	movzwl st_mode, %ebx	# zero extend from short to long
	andl $S_IRWXO, %ebx	# mask for file group permission

	movl %ebx, %ecx		# save for later use

stat_verify_roth:
	andl $S_IROTH, %ebx
	cmpl $0, %ebx
	jne stat_assign_roth	# group has read permission
	movl $7, %eax
	movb $'-', PERMISSION(, %eax, 1)
	
stat_verify_woth:
	movl %ecx, %ebx
	andl $S_IWOTH, %ebx
	cmpl $0, %ebx
	jne stat_assign_woth
	movl $8, %eax
	movb $'-', PERMISSION(, %eax, 1)
stat_verify_xoth:
	movl %ecx, %ebx
	andl $S_IXOTH, %ebx
	cmpl $0, %ebx
	jne stat_assign_xoth
	movl $9, %eax
	movb $'-', PERMISSION(, %eax, 1)
	jmp stat_verify_other_perm_ok

stat_assign_roth:
	movl $7, %eax
	movb $'r', PERMISSION(, %eax, 1)
	jmp stat_verify_woth

stat_assign_woth:
	movl $8, %eax
	movb $'w', PERMISSION(, %eax, 1)
	jmp stat_verify_xoth

stat_assign_xoth:
	movl $9, %eax
	movb $'x', PERMISSION(, %eax, 1)
#	jmp stat_verify_other_perm_ok

stat_verify_other_perm_ok:

	pushl $PERMISSION		# permission and type of file
	call print_str
	call print_tab
#	addl $4, %esp
	movzwl st_uid, %ebx		# uid of the file
	pushl %ebx
	call print_long
	call print_tab
	movzwl st_gid, %ebx		# gid of the file
	pushl %ebx
	call print_long
	call print_tab
	pushl st_size			# size of the file
	call print_long
#	call print_tab

/*
	call print_tab
	pushl st_mtime			# modification time
	call print_long
*/
#	jmp stat_end

stat_end:
	movl -4(%ebp), %ebx
	movl -8(%ebp), %ecx

	movl %ebp, %esp
	popl %ebp
	ret


# the directory listing function
# dir [<dir>]
# dependency: strcpy.s, strcat.s, print_str.s, print_tab.s, print_nl.s

.type ls, @function
.globl ls
.equ O_DIRECTORY, 020000	# is it a directory? syscall will fail if not
.equ S_IRWXU, 700
.equ READDIR, 89	# old_readdir() syscall number
ls:
	pushl %ebp
	movl %esp, %ebp

	subl $16, %esp		# 4-bytes for directory descriptor
	movl %ebx, -4(%ebp)
	movl %ecx, -8(%ebp)
	movl %edx, -12(%ebp)

	movl 8(%ebp), %ebx

	pushl %ebx
	call strlen
	addl $4, %esp		# reset the stack pointer
	cmpl $0, %eax		# check if a directory name supplied
	je ls_assign_current
	jmp ls_assigned
ls_assign_current:		# assigns the current directory
	movl $current_dir, %ebx
ls_assigned:
	movl $5, %eax		# open() syscall
	movl $O_DIRECTORY, %ecx	# descriptor must be a directory
	movl $S_IRWXU, %edx
	int  $0x80

	cmpl $-1, %eax		# check for success
	je   ls_end
	movl %eax, %ebx		# save directory file descriptor

ls_loop_start:
	movl $READDIR, %eax
	movl $struct_dirent, %ecx
	movl $1, %edx		# count (ignored)
	int  $0x80

	cmpl $0, %eax		# check for error
	jl   ls_end		# error
	cmpl $0, %eax		# check for end of directory
	je   ls_loop_end
# success
	pushl 8(%ebp)		# the directory name
	pushl $abs_filename
	call strcpy
	addl $8, %esp		# reset the stack pointer

	pushl $d_name
	pushl $abs_filename
	call strcat
	addl $8, %esp		# pop the abs_filename

	pushl $abs_filename
	call stat
	call print_tab
	addl $4, %esp

	pushl $d_name
	call print_str		# print the directory name
	call print_nl
	addl $4, %esp		# reset the stack pointer
	jmp ls_loop_start

ls_loop_end:
# now print file system information
	pushl $abs_filename
	call statfs
	call print_nl

# close the directory
# %ebx already contains the descriptor
	movl $6, %eax
	int  $0x80

ls_end:
	movl -4(%ebp), %ebx
	movl -8(%ebp), %ecx
	movl -12(%ebp), %edx

	movl %ebp, %esp
	popl %ebp
	ret

