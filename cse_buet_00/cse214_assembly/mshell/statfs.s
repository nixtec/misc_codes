# statfs.s
# displays file system statistics
#
# Ayub <mrayub@gmail.com>
#

.data
struct_statfs:
	f_type:		.long 0		# type of file system
	f_bsize:	.long 0		# optimal transfer block size
	f_blocks:	.long 0		# total data blocks in the file system
	f_bfree:	.long 0		# free blocks in the file system
	f_bavail:	.long 0		# free blocks available to non-superuser
	f_files:	.long 0		# total file nodes in file system (linux)
	f_ffre:		.long 0		# free file nodes in file system
	f_fsid:				# file system id
		.rept 2
			.long 0
		.endr
	f_namelen:	.long 0		# maximum length of file names
	f_spare:			# spare for later
		.rept 6
			.long 0
		.endr
statfs_msg:
	.asciz "File System Information :\n"
statfs_type_msg:
	.asciz "Type:\t\t"
statfs_affs_msg:
	.asciz "AFFS\n"
statfs_efs_msg:
	.asciz "EFS\n"
statfs_ext_msg:
	.asciz "EXT\n"
statfs_ext2_old_msg:
	.asciz "EXT2_OLD\n"
statfs_ext2_msg:
	.asciz "EXT2\n"
statfs_hpfs_msg:
	.asciz "HPFS\n"
statfs_iso_msg:
	.asciz "ISO\n"
statfs_minix_msg:
	.asciz "MINIX\n"
statfs_minix_30_msg:
	.asciz "MINIX (30 char names)\n"
statfs_minix2_msg:
	.asciz "MINIX2\n"
statfs_minix2_30_msg:
	.asciz "MINIX2 (30 char names)\n"
statfs_msdos_msg:
	.asciz "MSDOS\n"
statfs_ncp_msg:
	.asciz "NCP\n"
statfs_nfs_msg:
	.asciz "NFS\n"
statfs_proc_msg:
	.asciz "PROC\n"
statfs_smb_msg:
	.asciz "SMB\n"
statfs_xenix_msg:
	.asciz "XENIX\n"
statfs_sysv4_msg:
	.asciz "SYSV4\n"
statfs_sysv2_msg:
	.asciz "SYSV2\n"
statfs_coh_msg:
	.asciz "COH\n"
statfs_ufs_msg:
	.asciz "UFS\n"
statfs_xfs_msg:
	.asciz "XFS\n"
statfs_xia_msg:
	.asciz "XIAFS\n"
statfs_unknown_msg:
	.asciz "UNKNOWN\n"

statfs_bsize_msg:
	.asciz "Block size:\t"
statfs_block_msg:
	.asciz "Total blocks:\t"
statfs_bfree_msg:
	.asciz "Free blocks:\t"
statfs_files_msg:
	.asciz "Total nodes:\t"
statfs_namelen_msg:
	.asciz "Max. namelen:\t"


.equ STATFS, 99

# file system type definition
# 23 types of file systems are verified right now
.equ AFFS, 0xADFF		# affs_fs
.equ EFS, 0x00414A53		# efs_fs
.equ EXT, 0x137D		# ext_fs
.equ EXT2_OLD, 0xEF51		# ext2_fs
.equ EXT2, 0xEF53		#  do
.equ HPFS, 0xF995E849		# hpfs_fs
.equ ISOFS, 0x9660		# iso_fs
.equ MINIX, 0x137F		# minix_fs
.equ MINIX_30, 0x138F		#  do
.equ MINIX2, 0x2468		#  do
.equ MINIX2_30, 0x2478		#  do
.equ MSDOS, 0x4D44		# msdos_fs
.equ NCP, 0x564C		# ncp_fs
.equ NFS, 0x6969		# nfs_fs
.equ PROC, 0x9FA0		# proc_fs
.equ SMB, 0x517B		# smb_fs
.equ XENIX, 0x012FF7B4		# sysv_fs
.equ SYSV4, 0x012FF7B5		#  do
.equ SYSV2, 0x012FF7B6		#  do
.equ COH, 0x012FF7B7		#  do
.equ UFS, 0x00011954		# ufs_fs
.equ XFS, 0x58465342		# xfs_fs
.equ XIAFS, 0x012FD16D		# xia_fs

.text
.type statfs, @function
.globl statfs
statfs:
	pushl %ebp
	movl %esp, %ebp

# check if filename (full path) is supplied
	pushl 8(%ebp)
	call strlen
	cmpl $0, %eax
	je statfs_end
#	addl $4, %esp		# reset the stack pointer

# save general purpose registers
#	subl $8, %esp
	subl $4, %esp		# $4 is already subtracted
	movl %ebx, -4(%ebp)
	movl %ecx, -8(%ebp)

	movl $STATFS, %eax		# statfs() syscall
	movl 8(%ebp), %ebx
	movl $struct_statfs, %ecx
	int  $0x80

	cmpl $0, %eax			# check for success
	jne statfs_end

	call print_nl
	pushl $statfs_msg
	call print_str
	pushl $statfs_type_msg
	call print_str
	addl $8, %esp		# free some of the stack

	movl f_type, %ebx

	cmpl $AFFS, %ebx
	je statfs_print_affs
	cmpl $EFS, %ebx
	je statfs_print_efs
	cmpl $EXT, %ebx
	je statfs_print_ext
	cmpl $EXT2_OLD, %ebx
	je statfs_print_ext2_old
	cmpl $EXT2, %ebx
	je statfs_print_ext2
	cmpl $HPFS, %ebx
	je statfs_print_hpfs
	cmpl $ISOFS, %ebx
	je statfs_print_iso
	cmpl $MINIX, %ebx
	je statfs_print_minix
	cmpl $MINIX_30, %ebx
	je statfs_print_minix_30
	cmpl $MINIX2, %ebx
	je statfs_print_minix2
	cmpl $MINIX2_30, %ebx
	je statfs_print_minix2_30
	cmpl $MSDOS, %ebx
	je statfs_print_msdos
	cmpl $NCP, %ebx
	je statfs_print_ncp
	cmpl $NFS, %ebx
	je statfs_print_nfs
	cmpl $PROC, %ebx
	je statfs_print_proc
	cmpl $SMB, %ebx
	je statfs_print_smb
	cmpl $XENIX, %ebx
	je statfs_print_xenix
	cmpl $SYSV4, %ebx
	je statfs_print_sysv4
	cmpl $SYSV2, %ebx
	je statfs_print_sysv2
	cmpl $COH, %ebx
	je statfs_print_coh
	cmpl $UFS, %ebx
	je statfs_print_ufs
	cmpl $XFS, %ebx
	je statfs_print_xfs
	cmpl $XIAFS, %ebx
	je statfs_print_xia
	jmp statfs_print_unknown

statfs_print_affs:
	pushl $statfs_affs_msg
	call print_str
	jmp statfs_print_type_ok

statfs_print_efs:
	pushl $statfs_efs_msg
	call print_str
	jmp statfs_print_type_ok

statfs_print_ext:
	pushl $statfs_ext_msg
	call print_str
	jmp statfs_print_type_ok

statfs_print_ext2_old:
	pushl $statfs_ext2_old_msg
	call print_str
	jmp statfs_print_type_ok

statfs_print_ext2:
	pushl $statfs_ext2_msg
	call print_str
	jmp statfs_print_type_ok

statfs_print_hpfs:
	pushl $statfs_hpfs_msg
	call print_str
	jmp statfs_print_type_ok

statfs_print_iso:
	pushl $statfs_iso_msg
	call print_str
	jmp statfs_print_type_ok

statfs_print_minix:
	pushl $statfs_minix_msg
	call print_str
	jmp statfs_print_type_ok

statfs_print_minix_30:
	pushl $statfs_minix_30_msg
	call print_str
	jmp statfs_print_type_ok

statfs_print_minix2:
	pushl $statfs_minix2_msg
	call print_str
	jmp statfs_print_type_ok

statfs_print_minix2_30:
	pushl $statfs_minix2_30_msg
	call print_str
	jmp statfs_print_type_ok

statfs_print_msdos:
	pushl $statfs_msdos_msg
	call print_str
	jmp statfs_print_type_ok

statfs_print_ncp:
	pushl $statfs_ncp_msg
	call print_str
	jmp statfs_print_type_ok

statfs_print_nfs:
	pushl $statfs_nfs_msg
	call print_str
	jmp statfs_print_type_ok

statfs_print_proc:
	pushl $statfs_proc_msg
	call print_str
	jmp statfs_print_type_ok

statfs_print_smb:
	pushl $statfs_smb_msg
	call print_str
	jmp statfs_print_type_ok

statfs_print_xenix:
	pushl $statfs_xenix_msg
	call print_str
	jmp statfs_print_type_ok

statfs_print_sysv4:
	pushl $statfs_sysv4_msg
	call print_str
	jmp statfs_print_type_ok

statfs_print_sysv2:
	pushl $statfs_sysv2_msg
	call print_str
	jmp statfs_print_type_ok

statfs_print_coh:
	pushl $statfs_coh_msg
	call print_str
	jmp statfs_print_type_ok

statfs_print_ufs:
	pushl $statfs_ufs_msg
	call print_str
	jmp statfs_print_type_ok

statfs_print_xfs:
	pushl $statfs_xfs_msg
	call print_str
	jmp statfs_print_type_ok

statfs_print_xia:
	pushl $statfs_xia_msg
	call print_str
	jmp statfs_print_type_ok

statfs_print_unknown:
	pushl $statfs_unknown_msg
	call print_str
#	jmp statfs_print_type_ok

statfs_print_type_ok:

statfs_print_bsize_msg:
	pushl $statfs_bsize_msg
	call print_str
	pushl f_bsize
	call print_long
	call print_nl
statfs_print_block_msg:
	pushl $statfs_block_msg
	call print_str
	pushl f_blocks
	call print_long
	call print_nl
statfs_print_bfree_msg:
	pushl $statfs_bfree_msg
	call print_str
	pushl f_bfree
	call print_long
	call print_nl
statfs_print_files_msg:
	pushl $statfs_files_msg
	call print_str
	pushl f_files
	call print_long
	call print_nl
statfs_print_namelen_msg:
	pushl $statfs_namelen_msg
	call print_str
	pushl f_namelen
	call print_long
	call print_nl

# restore the general registers
	movl -4(%ebp), %ebx
	movl -8(%ebp), %ecx

statfs_end:
	movl %ebp, %esp
	popl %ebp
	ret

