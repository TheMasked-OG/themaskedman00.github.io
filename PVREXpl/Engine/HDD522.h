//===========================================================================
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//============================================================================

// HDD522.h: interface for the HDD50X class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __HDD522__
#define __HDD522__

//----------------------------- headers ------------------------
#include "Platform.h"
#include "GlobalDefs.h"
#include "HDDPVR.h"
#include "partition.h"
#include "LogFile.h"
//----------------------------- definitions --------------------

typedef unsigned char		__u8;
typedef unsigned short		__u16;
typedef unsigned long		__u32;

typedef char	__s8;
typedef short	__s16;
typedef long	__s32;

#ifdef __MS__
#pragma pack( push )
#pragma pack(1)
#endif
#ifdef __MAC__
#pragma pack(1)
#endif

typedef struct {
	__u32	bg_block_bitmap;
	__u32	bg_inode_bitmap;
	__u32   bg_inode_table;
	__u16	bg_free_blocks_count;
	__u16	bg_free_inodes_count;
	__u16	bg_used_dirs_count;
	__u16	bg_pad;
	__u32	bg_reserved[3];
} GROUPDESCRIPTOR, *pGROUPDESCRIPTOR;

typedef struct  {
/*00*/	__u32	s_inodes_count;		/* Inodes count */
	__u32	s_blocks_count;		/* Blocks count */
	__u32	s_r_blocks_count;	/* Reserved blocks count */
	__u32	s_free_blocks_count;	/* Free blocks count */
/*10*/	__u32	s_free_inodes_count;	/* Free inodes count */
	__u32	s_first_data_block;	/* First Data Block */
	__u32	s_log_block_size;	/* Block size */
	__u32	s_Sectors_Per_Block;	/* Fragment size */
/*20*/	__u32	s_blocks_per_group;	/* # Blocks per group */
	__u32	s_frags_per_group;	/* # Fragments per group */
	__u32	s_inodes_per_group;	/* # Inodes per group */
	__u32	s_mtime;		/* Mount time */
/*30*/	__u32	s_wtime;		/* Write time */
	__u16	s_mnt_count;		/* Mount count */
	__u16	s_max_mnt_count;	/* Maximal mount count */
	__u16	s_magic;		/* Magic signature */
	__u16	s_state;		/* File system state */
	__u16	s_errors;		/* Behaviour when detecting errors */
	__u16	s_minor_rev_level;	/* minor revision level */
/*40*/	__u32	s_lastcheck;		/* time of last check */
	__u32	s_checkinterval;	/* max. time between checks */
	__u32	s_creator_os;		/* OS */
	__u32	s_rev_level;		/* Revision level */
/*50*/	__u16	s_def_resuid;		/* Default uid for reserved blocks */
	__u16	s_def_resgid;		/* Default gid for reserved blocks */
	/*
	 * These fields are for EXT3_DYNAMIC_REV superblocks only.
	 *
	 * Note: the difference between the compatible feature set and
	 * the incompatible feature set is that if there is a bit set
	 * in the incompatible feature set that the kernel doesn't
	 * know about, it should refuse to mount the filesystem.
	 *
	 * e2fsck's requirements are more strict; if it doesn't know
	 * about a feature in either the compatible or incompatible
	 * feature set, it must abort and not try to meddle with
	 * things it doesn't understand...
	 */
	__u32	s_first_ino;		/* First non-reserved inode */
	__u16   s_inode_size;		/* size of inode structure */
	__u16	s_block_group_nr;	/* block group # of this superblock */
	__u32	s_feature_compat;	/* compatible feature set */
/*60*/	__u32	s_feature_incompat;	/* incompatible feature set */
	__u32	s_feature_ro_compat;	/* readonly-compatible feature set */
/*68*/	__u8	s_uuid[16];		/* 128-bit uuid for volume */
/*78*/	char	s_volume_name[16];	/* volume name */
/*88*/	char	s_last_mounted[64];	/* directory where last mounted */
/*C8*/	__u32	s_algorithm_usage_bitmap; /* For compression */
	/*
	 * Performance hints.  Directory preallocation should only
	 * happen if the EXT3_FEATURE_COMPAT_DIR_PREALLOC flag is on.
	 */
	__u8	s_prealloc_blocks;	/* Nr of blocks to try to preallocate*/
	__u8	s_prealloc_dir_blocks;	/* Nr to preallocate for dirs */
	__u16	s_reserved_gdt_blocks;	/* Per group desc for online growth */
	/*
	 * Journaling support valid if EXT3_FEATURE_COMPAT_HAS_JOURNAL set.
	 */
/*D0*/	__u8	s_journal_uuid[16];	/* uuid of journal superblock */
/*E0*/	__u32	s_journal_inum;		/* inode number of journal file */
	__u32	s_journal_dev;		/* device number of journal file */
	__u32	s_last_orphan;		/* start of list of inodes to delete */
	__u32	s_hash_seed[4];		/* HTREE hash seed */
	__u8	s_def_hash_version;	/* Default hash version to use */
	__u8	s_reserved_char_pad;
	__u16	s_reserved_word_pad;
	__u32	s_default_mount_opts;
	__u32	s_first_meta_bg; 	/* First metablock block group */
	__u32	s_reserved[190];	/* Padding to the end of the block */

} EXT2_SUPER_BLOCK, *pEXT2_SUPER_BLOCK;



#define EXT3_FEATURE_INCOMPAT_META_BG		0x0010



#define	EXT2_N_BLOCKS			15

typedef struct  {
	__u16	i_mode;		/* File mode */
	__u16	i_uid;		/* Owner Uid */
	__u32	i_size;		/* Size in bytes */
	__u32	i_atime;	/* Access time */
	__u32	i_ctime;	/* Creation time */
	__u32	i_mtime;	/* Modification time */
	__u32	i_dtime;	/* Deletion Time */
	__u16	i_gid;		/* Group Id */
	__u16	i_links_count;	/* Links count */
	__u32	i_blocks;	/* Blocks count */
	__u32	i_flags;	/* File flags */
	union {
		struct {
			__u32  l_i_reserved1;
		} linux1;
		struct {
			__u32  h_i_translator;
		} hurd1;
		struct {
			__u32  m_i_reserved1;
		} masix1;
	} osd1;				/* OS dependent 1 */
	__u32	i_block[EXT2_N_BLOCKS];/* Pointers to blocks */
	__u32	i_version;	/* File version (for NFS) */
	__u32	i_file_acl;	/* File ACL */
	__u32	i_dir_acl;	/* Directory ACL */
	__u32	i_faddr;	/* Fragment address */
	union {
		struct {
			__u8	l_i_frag;	/* Fragment number */
			__u8	l_i_fsize;	/* Fragment size */
			__u16	i_pad1;
			__u32	l_i_reserved2[2];
		} linux2;
		struct {
			__u8	h_i_frag;	/* Fragment number */
			__u8	h_i_fsize;	/* Fragment size */
			__u16	h_i_mode_high;
			__u16	h_i_uid_high;
			__u16	h_i_gid_high;
			__u32	h_i_author;
		} hurd2;
		struct {
			__u8	m_i_frag;	/* Fragment number */
			__u8	m_i_fsize;	/* Fragment size */
			__u16	m_pad1;
			__u32	m_i_reserved2[2];
		} masix2;
	} osd2;				/* OS dependent 2 */
} EXT2_INODE, *pEXT2_INODE;



typedef struct  {
	__u32	inode;			/* Inode number */
	__u16	rec_len;		/* Directory entry length */
	__u8	name_len;		/* Name length */
	__u8	file_type;
	char	name[255];	/* File name */
} EXT2_DIR_ENTRY, *pEXT2_DIR_ENTRY;

#ifdef __MS__
#pragma pack( pop )
#endif
#ifdef __MAC__
#pragma pack()
#endif



#define		EXT_FILE_TYPE_FILE	1
#define		EXT_FILE_TYPE_DIR	2

#define		MAX_DIR_ENTRIES2		4096

#define		ERR_NO_MORE_XFS_ENTRIES	3

typedef struct {
	unsigned long	InodeNumber;
	unsigned long		EntryType;
	int64		Size;
	unsigned char		Name[256];
} EXTDIRENTRY, *pEXTDIRENTRY;

typedef struct {
	unsigned long		NumEntries;
	EXTDIRENTRY DirEntry[MAX_DIR_ENTRIES2];
} EXTDIR, *pEXTDIR;

typedef struct {
	unsigned long		FileInodeNumber;
	unsigned long		FileCTime;
	int64		FileLength;
	int64		FilePointer;
	int			DataPointerType;
	union {
		unsigned char	*InternalFile;
		pSGLIST			SGPointer;
		pEXTDIR			DirData;
	} Data;
} EXTFILEPOINTER, *pEXTFILEPOINTER;

#define		POINTER_TYPE_DIR	1
#define		POINTER_TYPE_IFILE	2
#define		POINTER_TYPE_FILE	3

typedef struct {
	pEXTFILEPOINTER		fPtr;
	unsigned long				CurrentEntry;
} EXTFIND, *pEXTFIND;

#define S_XIFMT  00170000
#define S_IFSOCK 0140000
#define S_IFLNK	 0120000
#define S_IFREG  0100000
#define S_IFBLK  0060000
#define S_IFDIR  0040000
#define S_IFCHR  0020000
#define S_IFIFO  0010000
#define S_ISUID  0004000
#define S_ISGID  0002000
#define S_ISVTX  0001000

#define S_ISLNK(m)	(((m) & S_XIFMT) == S_IFLNK)
#define S_ISREG(m)	(((m) & S_XIFMT) == S_IFREG)
#define S_ISDIR(m)	(((m) & S_XIFMT) == S_IFDIR)
#define S_ISCHR(m)	(((m) & S_XIFMT) == S_IFCHR)
#define S_ISBLK(m)	(((m) & S_XIFMT) == S_IFBLK)
#define S_ISFIFO(m)	(((m) & S_XIFMT) == S_IFIFO)
#define S_ISSOCK(m)	(((m) & S_XIFMT) == S_IFSOCK)

#define  MAX_PARTITIONS	10

//------------------------------- classes -----------------------
class HDD522 : public HDDPVR  
{
public:
	unsigned long GetPvrFileTime( char *FileName );
	void  AdjustAudioFileSize( HANDLE FileHandle );
	int64 GetPvrFileSize( HANDLE FileHandle );
	bool IsPvrDrive();
	void ClosePvrFile( HANDLE FileHandle );
	unsigned long ReadPvrFile( HANDLE fileHandle, unsigned char *Data, unsigned long Length);
	HANDLE OpenPvrFile( char *FileName );
	HDD522();
	virtual ~HDD522();

private:
	int m_CurSGElementCount;
	int64			GetFileLength( pEXT2_INODE iptr, pSGLIST sPtr);
	unsigned short	Cvts( unsigned short x);
	unsigned long	Cvt( unsigned long x);
	int				GetNumberOfPartitions();
	bool			MountExt3(int PartNum);
	HANDLE			OpenExt2File(HANDLE DirHandle, char *FileName);
	pEXT2_INODE		GetInode(unsigned long InodeNumber);
	int64			GetExt2FileSize(HANDLE FileHandle);
	pSGLIST			GetSGFromInode(pEXT2_INODE iPtr);
	bool			ProcessBlockList(pSGLIST spPtr, unsigned long *dPtr, unsigned long Length);
	bool			ProcessFistIndirect(pSGLIST sPtr, unsigned long Block);
	bool			ProcessSecondIndirect(pSGLIST sPtr, unsigned long Block);
	bool			ProcessThirdIndirect(pSGLIST sPtr, unsigned long Block);
	pEXTDIR			GetDirListing(unsigned char *Buffer, unsigned long Length);
	unsigned long	Descriptor_loc(unsigned long DescBlock);
	bool			ext3_bg_has_super(unsigned long BlockGroup);
	bool			ext3_group_sparse(unsigned long Group);
	bool			test_group(unsigned long a, unsigned long b);

	
	unsigned long	m_PartStart[MAX_PARTITIONS];
	unsigned long	m_PartSize[MAX_PARTITIONS];
	int				m_NumberOfPartitions;
	unsigned long	m_PartitionOffset;
	GROUPDESCRIPTOR	m_Grp_desc[4096];
	EXT2_SUPER_BLOCK m_Super;
	int				m_Mounted;
	HANDLE			m_Root;
};

#endif // __HDD522__
