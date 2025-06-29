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

//---------------------- XFS Linux structures ------------------------
#if !defined(XFS_LINUX__INCLUDED_)
#define XFS_LINUX__INCLUDED_

//************************* Dish Linux Stuff **********************************
//typedef __uint64_t	xfs_drfsbno_t;	/* blockno in filesystem (raw) */
//typedef	__uint64_t	xfs_drtbno_t;	/* extent (block) in realtime area */
//typedef __uint64_t	xfs_dfsbno_t;	/* blockno in filesystem (agno|agbno) */
//typedef __u64	xfs_ino_t;	/* <inode> type */
//typedef __s64	xfs_daddr_t;	/* <disk address> type */
//typedef __uint32_t	xfs_agblock_t;	/* blockno in alloc. group */
//typedef	__uint32_t	xfs_extlen_t;	/* extent length in blocks */
//typedef	__uint32_t	xfs_agnumber_t;	/* allocation group number */
//typedef __int32_t	xfs_extnum_t;	/* # of extents in a file */
//typedef __int16_t	xfs_aextnum_t;	/* # extents in an attribute fork */
//typedef	__int64_t	xfs_fsize_t;	/* bytes in a file */
//typedef __uint64_t	xfs_ufsize_t;	/* unsigned bytes in a file */

//typedef char *	xfs_caddr_t;	/* <core address> type */
//typedef struct {
//        unsigned char   __u_bits[16];
//} uuid_t;
#ifdef __MS__
#pragma pack( push )
#pragma pack(1)
#endif
#ifdef __MAC__
#pragma pack(1)
#endif
typedef struct xfs_sb
{
	unsigned long	sb_magicnum;		/* magic number == XFS_SB_MAGIC */
	unsigned long	sb_blocksize;		/* logical block size, bytes */
	int64			sb_dblocks;			/* number of data blocks */
	int64			sb_rblocks;			/* number of realtime blocks */
	int64			sb_rextents;		/* number of realtime extents */
	unsigned char	sb_uuid[16];		/* file system unique id */
	int64			sb_logstart;		/* starting block of log if internal */
	int64			sb_rootino;			/* root inode number */
	int64			sb_rbmino;			/* bitmap inode for realtime extents */
	int64			sb_rsumino;			/* summary inode for rt bitmap */
	unsigned long	sb_rextsize;		/* realtime extent size, blocks */
	unsigned long	sb_agblocks;		/* size of an allocation group */
	unsigned long	sb_agcount;			/* number of allocation groups */
	unsigned long	sb_rbmblocks;		/* number of rt bitmap blocks */
	unsigned long	sb_logblocks;		/* number of log blocks */
	unsigned short	sb_versionnum;		/* header version == XFS_SB_VERSION */
	unsigned short	sb_sectsize;		/* volume sector size, bytes */
	unsigned short	sb_inodesize;		/* inode size, bytes */
	unsigned short	sb_inopblock;		/* inodes per block */
	char			sb_fname[12];		/* file system name */
	unsigned char	sb_blocklog;		/* log2 of sb_blocksize */
	unsigned char	sb_sectlog;			/* log2 of sb_sectsize */
	unsigned char	sb_inodelog;		/* log2 of sb_inodesize */
	unsigned char	sb_inopblog;		/* log2 of sb_inopblock */
	unsigned char	sb_agblklog;		/* log2 of sb_agblocks (rounded up) */
	unsigned char	sb_rextslog;		/* log2 of sb_rextents */
	unsigned char	sb_inprogress;		/* mkfs is in progress, don't mount */
	unsigned char	sb_imax_pct;		/* max % of fs for inode space */
					/* statistics */
	/*
	 * These fields must remain contiguous.  If you really
	 * want to change their layout, make sure you fix the
	 * code in xfs_trans_apply_sb_deltas().
	 */
	int64			sb_icount;			/* allocated inodes */
	int64			sb_ifree;			/* free inodes */
	int64			sb_fdblocks;		/* free data blocks */
	int64			sb_frextents;		/* free realtime extents */
	/*
	 * End contiguous fields.
	 */
	int64			sb_uquotino;		/* user quota inode */
	int64			sb_gquotino;		/* group quota inode */
	unsigned short	sb_qflags;			/* quota flags */
	unsigned char	sb_flags;			/* misc. flags */
	unsigned char	sb_shared_vn;		/* shared version number */
	unsigned long	sb_inoalignmt;		/* inode chunk alignment, fsblocks */
	unsigned long	sb_unit;			/* stripe or raid unit */
	unsigned long	sb_width;			/* stripe or raid width */	
	unsigned char	sb_dirblklog;		/* log2 of dir block size (fsbs) */
    unsigned char	sb_dummy[7];		/* padding */
} xfs_sb_t;

typedef struct xfs_timestamp {
	unsigned long	t_sec;			/* timestamp seconds */
	unsigned long	t_nsec;			/* timestamp nanoseconds */
} xfs_timestamp_t;

typedef struct xfs_dinode_core
{
	unsigned short	di_magic;		/* inode magic # = XFS_DINODE_MAGIC */
	unsigned short	di_mode;		/* mode and type of file */
	unsigned char	di_version;		/* inode version */
	unsigned char	di_format;		/* format of di_c data */
	unsigned short	di_onlink;		/* old number of links to file */
	unsigned long	di_uid;			/* owner's user id */
	unsigned long	di_gid;			/* owner's group id */
	unsigned long	di_nlink;		/* number of links to file */
	unsigned short	di_projid;		/* owner's project id */
	unsigned char	di_pad[10];		/* unused, zeroed space */
	xfs_timestamp_t	di_atime;		/* time last accessed */
	xfs_timestamp_t	di_mtime;		/* time last modified */
	xfs_timestamp_t	di_ctime;		/* time created/inode modified */
	int64			di_size;		/* number of bytes in file */
	int64			di_nblocks;		/* # of direct & btree blocks used */
	unsigned long	di_extsize;		/* basic/minimum extent size for file */
	unsigned long	di_nextents;	/* number of extents in data fork */
	unsigned short	di_anextents;	/* number of extents in attribute fork*/
	unsigned char	di_forkoff;		/* attr fork offs, <<3 for 64b align */
	unsigned char	di_aformat;		/* format of attr fork's data */
	unsigned long	di_dmevmask;	/* DMIG event mask */
	unsigned short	di_dmstate;		/* DMIG state info */
	unsigned short	di_flags;		/* random flags, XFS_DIFLAG_... */
	unsigned long	di_gen;			/* generation number */
} xfs_dinode_core_t;

typedef	unsigned short		xfs_dir2_data_off_t;
typedef unsigned short		xfs_dir2_sf_off_t;
typedef	int64		xfs_ino_t;

typedef struct xfs_bmdr_block
{
	unsigned short	bb_level;		/* 0 is a leaf */
	unsigned short	bb_numrecs;		/* current # of data records */
	int64			bTree_Key[9];	/* key for File offset */
	int64			bTree_Ptr[9];	/* Block number of the leaf */
} xfs_bmdr_block_t;
typedef struct xfs_bmbt_rec_32
{
	unsigned long	l0, l1, l2, l3;
} xfs_bmbt_rec_32_t;

typedef struct { unsigned char i[sizeof(int64)]; } xfs_dir_ino_t;
/*
 * The parent directory has a dedicated field, and the self-pointer must
 * be calculated on the fly.
 *
 * Entries are packed toward the top as tight as possible.  The header
 * and the elements much be bcopy()'d out into a work area to get correct
 * alignment for the inode number fields.
 */
typedef struct xfs_dir_shortform {
	struct xfs_dir_sf_hdr {			/* constant-structure header block */
		xfs_dir_ino_t	parent;		/* parent dir inode number */
		unsigned char	count;		/* count of active entries */
	} hdr;
	struct xfs_dir_sf_entry {
		xfs_dir_ino_t	inumber;	/* referenced inode number */
		unsigned char	namelen;	/* actual length of name (no NULL) */
		unsigned char	name[1];	/* name */
	} list[1];			/* variable sized array */
} xfs_dir_shortform_t;

typedef	int64 xfs_dir2_ino8_t;
typedef unsigned long	xfs_dir2_ino4_t;

typedef union {
//	xfs_dir2_ino8_t	i8;
	xfs_dir2_ino4_t	i4;
} xfs_dir2_inou_t;

typedef struct xfs_dir2_sf_hdr {
	unsigned char		count;		/* count of entries */
	unsigned char		i8count;	/* count of 8-byte inode #s */
	xfs_dir2_inou_t		parent;		/* parent dir inode number */
} xfs_dir2_sf_hdr_t;

typedef struct xfs_dir2_sf_entry {
	unsigned char		namelen;	/* actual name length */
	xfs_dir2_sf_off_t	offset;		/* saved offset */
	unsigned char		name[1];	/* name, variable size */
	xfs_dir2_inou_t		inumber;	/* inode number, var. offset */
} xfs_dir2_sf_entry_t;

typedef struct xfs_dir2_sf {
	xfs_dir2_sf_hdr_t	hdr;		/* shortform header */
	xfs_dir2_sf_entry_t	list[1];	/* shortform entries */
} xfs_dir2_sf_t;

typedef struct xfs_attr_shortform {
	struct xfs_attr_sf_hdr {		/* constant-structure header block */
		unsigned short	totsize;	/* total bytes in shortform list */
		unsigned char	count;		/* count of active entries */
	} hdr;
	struct xfs_attr_sf_entry {
		unsigned char	namelen;	/* actual length of name (no NULL) */
		unsigned char	valuelen;	/* actual length of value (no NULL) */
		unsigned char	flags;		/* flags bits (see xfs_attr_leaf.h) */
		unsigned char	nameval[1];	/* name & value bytes concatenated */
	} list[1];			/* variable sized array */
} xfs_attr_shortform_t;


typedef struct xfs_dinode
{
	xfs_dinode_core_t	di_core;
	/*
	 * In adding anything between the core and the union, be
	 * sure to update the macros like XFS_LITINO below and
	 * XFS_BMAP_RBLOCK_DSIZE in xfs_bmap_btree.h.
	 */
	unsigned long			di_next_unlinked;/* agi unlinked list ptr */
	union {
		xfs_bmdr_block_t	di_bmbt;	/* btree root block */
		xfs_bmbt_rec_32_t	di_bmx[1];	/* extent list */
		xfs_dir_shortform_t di_dirsf;	/* shortform directory */
		xfs_dir2_sf_t		di_dir2sf;	/* shortform directory v2 */
		char				di_c[1];	/* local contents */
		unsigned long		di_dev;		/* device for IFCHR/IFBLK */
		unsigned char		di_muuid[16];	/* mount point value */
		char				di_symlink[1];	/* local symbolic link */
	}di_u;
	union {
		xfs_bmdr_block_t di_abmbt;	/* btree root block */
		xfs_bmbt_rec_32_t di_abmx[1];	/* extent list */
		xfs_attr_shortform_t di_attrsf;	/* shortform attribute list */
	}di_a;
} xfs_dinode_t;

typedef struct {
	unsigned char	InodeBytes[256];
} INODEBYTES;

typedef struct xfs_agf
{
	/*
	 * Common allocation group header information
	 */
	unsigned long	agf_magicnum;		/* magic number == XFS_AGF_MAGIC */
	unsigned long	agf_versionnum;		/* header version == XFS_AGF_VERSION */
	unsigned long	agf_seqno;			/* sequence # starting from 0 */
	unsigned long	agf_length;			/* size in blocks of a.g. */
	/*
	 * Freespace information
	 */
	unsigned long	agf_roots[2];	/* root blocks */
	unsigned long	agf_spare0;	/* spare field */
	unsigned long	agf_levels[2];	/* btree levels */
	unsigned long	agf_spare1;	/* spare field */
	unsigned long	agf_flfirst;	/* first freelist block's index */
	unsigned long	agf_fllast;	/* last freelist block's index */
	unsigned long	agf_flcount;	/* count of blocks in freelist */
	unsigned long	agf_freeblks;	/* total free blocks */
	unsigned long	agf_longest;	/* longest free space */
} xfs_agf_t;

#define	XFS_AGI_UNLINKED_BUCKETS	64

typedef unsigned long	xfs_agnumber_t;
typedef unsigned long	xfs_agblock_t;
typedef unsigned long	xfs_agino_t;

typedef struct xfs_agi
{
	/*
	 * Common allocation group header information
	 */
	unsigned long	agi_magicnum;	/* magic number == XFS_AGI_MAGIC */
	unsigned long	agi_versionnum;	/* header version == XFS_AGI_VERSION */
	xfs_agnumber_t	agi_seqno;	/* sequence # starting from 0 */
	xfs_agblock_t	agi_length;	/* size in blocks of a.g. */
	/*
	 * Inode information
	 * Inodes are mapped by interpreting the inode number, so no
	 * mapping data is needed here.
	 */
	xfs_agino_t		agi_count;	/* count of allocated inodes */
	xfs_agblock_t	agi_root;	/* root of inode btree */
	unsigned long	agi_level;	/* levels in inode btree */
	xfs_agino_t		agi_freecount;	/* number of free inodes */
	xfs_agino_t		agi_newino;	/* new inode just allocated */
	xfs_agino_t		agi_dirino;	/* last directory inode chunk */
	/*
	 * Hash table of inodes which have been unlinked but are
	 * still being referenced.
	 */
	xfs_agino_t	agi_unlinked[XFS_AGI_UNLINKED_BUCKETS];
} xfs_agi_t;


//The file mode is a set of flags that specify the type of file and the access permissions
//identifier value comment 
#define		IFMT		0xF000	//format mask 
#define		IFSOCK		0xA000	//socket 
#define		IFLNK		0xC000	//symbolic link 
#define		IFREG		0x8000	//regular file 
#define		IFBLK		0x6000	//block device 
#define		IFDIR		0x4000	//directory 
#define		IFCHR		0x2000	//character device 
#define		IFIFO		0x1000	//fifo 
      
#define		ISUID		0x0800	//SUID 
#define		ISGID		0x0400	//SGID 
#define		ISVTX		0x0200	//sticky bit 
      
#define		IRWXU		0x01C0	//user mask 
#define		IRUSR		0x0100	//read 
#define		IWUSR		0x0080	//write 
#define		IXUSR		0x0040	//execute 
      
#define		IRWXG		0x0038	//group mask 
#define		IRGRP		0x0020	//read 
#define		IWGRP		0x0010	//write 
#define		IXGRP		0x0008	//execute 
      
#define		IRWXO		0x0007	//other mask 
#define		IROTH		0x0004	//read 
#define		IWOTH		0x0002	//write 
#define		IXOTH		0x0001	//execute 

/* Values for di_format */
typedef enum xfs_dinode_fmt
{
	XFS_DINODE_FMT_DEV,			/* CHR, BLK: di_dev */
	XFS_DINODE_FMT_LOCAL,		/* DIR, REG: di_c */
								/* LNK: di_symlink */
	XFS_DINODE_FMT_EXTENTS,		/* DIR, REG, LNK: di_bmx */
	XFS_DINODE_FMT_BTREE,		/* DIR, REG, LNK: di_bmbt */
	XFS_DINODE_FMT_UUID 		/* MNT: di_uuid */
} xfs_dinode_fmt_t;

/*
 * Entries are packed toward the top as tightly as possible.  The header
 * and the elements must be bcopy()'d out into a work area to get correct
 * alignment for the inode number fields.
 */
typedef unsigned short	xfs_dir2_sf_off_t;


//---------------------------------------------------------------
#define	XFS_DIR2_BLOCK_MAGIC	0x58443242	/* XD2B: for one block dirs */

typedef struct xfs_dir2_block_tail {
	unsigned long		count;			/* count of leaf entries */
	unsigned long		stale;			/* count of stale lf entries */
} xfs_dir2_block_tail_t;

/*
 * Describe a free area in the data block.
 * The freespace will be formatted as a xfs_dir2_data_unused_t.
 */
typedef struct xfs_dir2_data_free {
	xfs_dir2_data_off_t	offset;		/* start of freespace */
	xfs_dir2_data_off_t	length;		/* length of freespace */
} xfs_dir2_data_free_t;

#define		XFS_DIR2_DATA_FD_COUNT 3
/*
 * Header for the data blocks.
 * Always at the beginning of a directory-sized block.
 * The code knows that XFS_DIR2_DATA_FD_COUNT is 3.
 */
typedef struct xfs_dir2_data_hdr {
	unsigned long	magic;		/* XFS_DIR2_DATA_MAGIC */
										/* or XFS_DIR2_BLOCK_MAGIC */
	xfs_dir2_data_free_t	bestfree[XFS_DIR2_DATA_FD_COUNT];
} xfs_dir2_data_hdr_t;

/*
 * Active entry in a data block.  Aligned to 8 bytes.
 * Tag appears as the last 2 bytes.
 */
typedef struct xfs_dir2_data_entry {
	xfs_ino_t		inumber;	/* inode number */
	unsigned char	namelen;	/* name length */
	unsigned char	name[1];	/* name bytes, no null */
						/* variable offset */
	xfs_dir2_data_off_t	tag;		/* starting offset of us */
} xfs_dir2_data_entry_t;

/*
 * Unused entry in a data block.  Aligned to 8 bytes.
 * Tag appears as the last 2 bytes.
 */
typedef struct xfs_dir2_data_unused {
	unsigned short				freetag;	/* XFS_DIR2_DATA_FREE_TAG */
	xfs_dir2_data_off_t	length;		/* total free length */
						/* variable offset */
	xfs_dir2_data_off_t	tag;		/* starting offset of us */
} xfs_dir2_data_unused_t;

typedef union {
	xfs_dir2_data_entry_t	entry;
	xfs_dir2_data_unused_t	unused;
} xfs_dir2_data_union_t;

/*
 * Generic data block structure, for xfs_db.
 */
typedef struct xfs_dir2_data {
	//xfs_dir2_data_hdr_t	hdr;		/* magic XFS_DIR2_DATA_MAGIC */
	xfs_dir2_data_union_t	u[1];
} xfs_dir2_data_t;

//------------------------------------------------------------
/*
 * Combined header and structure, used by common code.
 */
typedef struct xfs_btree_hdr
{
	unsigned long		bb_magic;	/* magic number for block type */
	unsigned short		bb_level;	/* 0 is a leaf */
	unsigned short		bb_numrecs;	/* current # of data records */
} xfs_btree_hdr_t;

typedef struct xfs_btree_block
{
	xfs_btree_hdr_t		bb_h;		/* header */
	int64				bb_leftsib;
	int64				bb_rightsib;
	xfs_bmbt_rec_32_t	rec[255];	// this will fill up one 4096 block
} xfs_btree_block_t;

typedef struct xfs_btree_parent
{
	xfs_btree_hdr_t		bb_h;		/* header */
	int64				bb_leftsib;
	int64				bb_rightsib;
	int64				bb_Key[255];	// this will fill up one 4096 block
	int64				bb_Ptr[255];	// this will fill up one 4096 block
} xfs_btree_parent_t;

#ifdef __MS__
#pragma pack( pop )
#endif
#ifdef __MAC__
#pragma pack()
#endif

#endif
