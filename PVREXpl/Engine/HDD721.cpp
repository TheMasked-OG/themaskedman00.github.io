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

// HDD721.cpp: implementation of the HDD50X class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HDD721.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HDD721::HDD721()
{
	m_IsMounted = false;
	m_Root = NULL;
}

HDD721::~HDD721()
{

	if( m_Root != NULL )
		ClosePvrFile(m_Root);
	m_Root = NULL;
}

//////////////////////////////////////////////////////////////////////
// Public Methods
//////////////////////////////////////////////////////////////////////

//--------------------------------
// this function will open the file on a 721 pVR HDD
HANDLE 
HDD721::OpenPvrFile(char *FileName)
{
	HANDLE	DirHandle;
	HANDLE	FileHandle;
	int		j;
	char	*Name;
	char	*dir;

	if( !m_IsMounted ){
		GetNumberOfPartitions();
		MountXFS( m_NumberOfPartitions - 1);
		if( !m_IsMounted ){
			OutPutDebugString("Error Mounting XFS File System!");
			return(NULL);  //the we could not mount
		}
	}
	// ok now check the root.
	if( m_Root == NULL ){
		m_Root = OpenXFSFile( NULL, "root");
		if( m_Root == NULL ){
			OutPutDebugString("Unable to open root directory!");
			return(NULL);  //something is wrong
		}
	}
	// now parse the input file name and get the name and any first layer directories...
	j = strlen(FileName) - 1;
	while( j > 0 && FileName[j] != '/')
		j--;
	if( j > 0 ){
		FileName[j] = 0x00;
		Name = &FileName[j+1];
		dir = FileName;
	} else {
		Name = FileName;
		dir = NULL;
	}
	if( dir != NULL ){	
		DirHandle = OpenXFSFile( m_Root, dir);
		if( DirHandle == NULL ){
			OutPutDebugString("Unable to open sub directory!");
			return(NULL);
		}
		FileHandle = OpenXFSFile( DirHandle, Name);
		ClosePvrFile( DirHandle );
	} else {
		FileHandle = OpenXFSFile( m_Root, Name);
	}
	return(FileHandle);
}

//----------------------------------
// This function will read from a pvr file
unsigned long 
HDD721::ReadPvrFile(HANDLE FileHandle, unsigned char *Buffer, unsigned long Length)
{
	unsigned long		rc;
	pXFSFILEPOINTER		fPtr;
	int64				FileRemaining;
	pSGELEMENT			sg;
	unsigned long		segLength,segOffset,SecOffset,SecLength;
	unsigned char		*sgBuffer;
	
	fPtr = (pXFSFILEPOINTER)FileHandle;
	if( fPtr->DataPointerType == POINTER_TYPE_DIR ){
		OutPutDebugString("Invalid File Type for read operation!");
		return(0);  //can only do findfirst, findnext on dirs
	}
	rc = 0;
	FileRemaining = fPtr->FileLength - fPtr->FilePosition;
	if( (int64)Length > FileRemaining)
		Length = (unsigned long)FileRemaining;
	while( Length > 0 && fPtr->FilePosition < fPtr->FileLength){
		if( fPtr->DataPointerType == POINTER_TYPE_IFILE ){  //this case the file is in memeory already
			memcpy((void *)Buffer,(void *)&fPtr->Data.InternalFile[fPtr->FilePosition],Length);
			fPtr->FilePosition += Length;
			rc += Length;
			Length -= Length;
		} else {
			while( Length > 0 && fPtr->FilePosition < fPtr->FileLength){
				//Find the SG element that fits the current pointer
 				sg = fPtr->Data.SGPointer;
				FileRemaining = 0;
				while( !(fPtr->FilePosition >= FileRemaining && fPtr->FilePosition < (FileRemaining + ((int64)sg->sgLength * (int64)m_Super->sb_sectsize)))){
					FileRemaining += ((int64)sg->sgLength * (int64)m_Super->sb_sectsize);
					sg++;
				}
				//we sould now have the sg 
				segOffset = (unsigned long)( fPtr->FilePosition - FileRemaining );
				SecOffset = segOffset / 512;
				segOffset = segOffset - (SecOffset * 512);

				segLength = (sg->sgLength * m_Super->sb_sectsize) - segOffset;
				if( segLength > Length )
					segLength = Length;
				SecLength = segLength / 512;
				if( segLength % 512 )
					SecLength++;

				sgBuffer = (unsigned char *)new unsigned char[SecLength * m_Super->sb_sectsize];
				if( sgBuffer == NULL )
					return(0);
				ReadBlock(sg->sgStartLba + SecOffset,sgBuffer,SecLength);
				memcpy((void *)Buffer,(void *)&sgBuffer[segOffset],segLength);
				Buffer += segLength;
				rc += segLength;
				Length -= segLength;
				fPtr->FilePosition += segLength;
				delete []sgBuffer;
			}

		}
	}
	return(rc);
}

//----------------------------------
// this function will close and clean up from a file
void 
HDD721::ClosePvrFile(HANDLE FileHandle)
{
	pXFSFILEPOINTER		fPtr;
	
	fPtr = (pXFSFILEPOINTER)FileHandle;
	if( fPtr->DataPointerType == POINTER_TYPE_DIR )
		delete fPtr->Data.DirData;
	if( fPtr->DataPointerType == POINTER_TYPE_IFILE )
		delete []fPtr->Data.InternalFile;
	if( fPtr->DataPointerType == POINTER_TYPE_FILE )
		delete fPtr->Data.SGPointer;
	delete fPtr;
}

//-----------------------------------
//   this function will determin if the drive is really a pvr drive of this type
bool 
HDD721::IsPvrDrive()
{
	int		NumParts;

	NumParts = GetNumberOfPartitions();
	if( NumParts == 4 ) //if we find an XFS drive with 4 partitions then assume pvr.
		return(true);
	return(false);
}

//------------------------------------------
//  this function will return the file size of the opend file
int64 
HDD721::GetPvrFileSize( HANDLE FileHandle )
{
	pXFSFILEPOINTER		fPtr;
	
	if( FileHandle == NULL )
		return(0);
	fPtr = (pXFSFILEPOINTER)FileHandle;
	return(fPtr->FileLength);
}

//------------------------------------------
//  this function will round the audio file size up to the neares cluster.
void 
HDD721::AdjustAudioFileSize(HANDLE FileHandle)
{
	
	//do nothing
}
//------------------------------------------
//  this function will return do nothing
unsigned long 
HDD721::GetPvrFileTime( char *FileName )
{
	return(0);
}
//////////////////////////////////////////////////////////////////////
// Private Methods
//////////////////////////////////////////////////////////////////////

//-----------------------------------------
// this function will return the number of valid Ext3 partitions found
int 
HDD721::GetNumberOfPartitions()
{
	PARTITIONENTRYPTR	pPtr,npPtr;
	unsigned char		Buffer[512];
	unsigned char		Buffer2[512];
	unsigned short		*uPtr;
	unsigned long		ExtStart = 0;
	unsigned long		ExtBase = 0;
	unsigned long		j;
	bool				Scanning;
	unsigned short		PartSig;

	m_NumberOfPartitions = 0;
	ReadBlock(0,Buffer,1);
	uPtr = (unsigned short *)&Buffer[510];
#ifdef __PwPC__
	PartSig = 0x55AA;
#else
	PartSig = 0xAA55;
#endif
	if( *uPtr != PartSig )
		return(0);
	Scanning = true;
	while( Scanning )
	{
		pPtr = (PARTITIONENTRYPTR)&Buffer[510 - (4 * sizeof(PARTITIONENTRY))];
		npPtr = pPtr;
		for( j = 0; j < 4; j++ )
		{
			if( npPtr->FileSystem == 131 || npPtr->FileSystem == 130)
			{
				ReadBlock(Cvt(npPtr->peStartSector) + ExtStart,Buffer2,1);
				if( !strncmp((char *)Buffer2,"XFSB",4)){
					m_PartStart[m_NumberOfPartitions] = Cvt(npPtr->peStartSector) + ExtStart;
					m_PartSize[m_NumberOfPartitions] = Cvt(npPtr->peLength);
					m_NumberOfPartitions++;
				}
			}
			npPtr++;
		}
		//now look for an extended partition
		npPtr = pPtr;
		for( j = 0; j < 4; j++ )
		{
			if( npPtr->FileSystem == 0x0F || npPtr->FileSystem == 0x05 )
			{
				if( ExtStart == 0 ) {
					ExtStart = ExtStart + Cvt(npPtr->peStartSector);
					ExtBase = ExtStart;
				} else {
					ExtStart = ExtBase + Cvt(npPtr->peStartSector);
				}
				ReadBlock( ExtStart,Buffer,1);
				uPtr = (unsigned short *)&Buffer[510];
				if( *uPtr != PartSig ){
					Scanning = false;
				}
				break;
			}
			npPtr++;
		}
		if( j == 4 )
			Scanning = false;
	}
	return(m_NumberOfPartitions);
}

//---------------------------------------------------------------
//  this function is calles to mount the file system on partition number
void 
HDD721::MountXFS( int PartNum )
{

	m_IsMounted = false;
	m_PartitionStart = m_PartStart[PartNum];
	//find the super block
	//m_PartitionStart = 0x3F;
	ReadBlock(m_PartitionStart,m_SuperData,1);
	m_Super = (xfs_sb_t *)m_SuperData;
	
	ConvertEndian( (void *)&m_Super->sb_blocksize,sizeof(unsigned long));
	ConvertEndian( (void *)&m_Super->sb_dblocks,sizeof(int64));
	ConvertEndian( (void *)&m_Super->sb_rblocks,sizeof(int64));
	ConvertEndian( (void *)&m_Super->sb_rextents,sizeof(int64));
	ConvertEndian( (void *)&m_Super->sb_logstart,sizeof(int64));
	ConvertEndian( (void *)&m_Super->sb_rootino,sizeof(int64));
	ConvertEndian( (void *)&m_Super->sb_rbmino,sizeof(int64));
	ConvertEndian( (void *)&m_Super->sb_rsumino,sizeof(int64));
	ConvertEndian( (void *)&m_Super->sb_rextsize,sizeof(unsigned long));
	ConvertEndian( (void *)&m_Super->sb_agblocks,sizeof(unsigned long));
	ConvertEndian( (void *)&m_Super->sb_agcount,sizeof(unsigned long));
	ConvertEndian( (void *)&m_Super->sb_rbmblocks,sizeof(unsigned long));
	ConvertEndian( (void *)&m_Super->sb_logblocks,sizeof(unsigned long));
	ConvertEndian( (void *)&m_Super->sb_versionnum,sizeof(unsigned short));
	ConvertEndian( (void *)&m_Super->sb_sectsize,sizeof(unsigned short));
	ConvertEndian( (void *)&m_Super->sb_inodesize,sizeof(unsigned short));
	ConvertEndian( (void *)&m_Super->sb_inopblock,sizeof(unsigned short));

	ConvertEndian( (void *)&m_Super->sb_icount,sizeof(int64));
	ConvertEndian( (void *)&m_Super->sb_ifree,sizeof(int64));
	ConvertEndian( (void *)&m_Super->sb_fdblocks,sizeof(int64));
	ConvertEndian( (void *)&m_Super->sb_frextents,sizeof(int64));
	
	ConvertEndian( (void *)&m_Super->sb_uquotino,sizeof(int64));
	ConvertEndian( (void *)&m_Super->sb_gquotino,sizeof(int64));

	ConvertEndian( (void *)&m_Super->sb_qflags,sizeof(unsigned short));
	ConvertEndian( (void *)&m_Super->sb_inoalignmt,sizeof(unsigned long));
	ConvertEndian( (void *)&m_Super->sb_unit,sizeof(unsigned long));
	ConvertEndian( (void *)&m_Super->sb_width,sizeof(unsigned long));

	if( m_Root != NULL )
		ClosePvrFile(m_Root);
	m_Root = NULL;
	//set the mounted flag
	if( !strncmp((char *)&m_Super->sb_magicnum,"XFSB",4))
		m_IsMounted = true;
}

//----------------------------------------------------------------
// convert endien order
void 
HDD721::ConvertEndian(void *Ptr, int Len)
{
#ifdef __x86__
	unsigned char	tmp[32];
	unsigned char	*cPtr;
	int		j;

	cPtr = (unsigned char *)Ptr;
	for( j = 1; j <= Len; j++){
		tmp[j - 1] = cPtr[Len - j];
	}
	cPtr = (unsigned char *)Ptr;
	for( j = 0; j < Len; j++){
		cPtr[j] = tmp[j];
	}
#endif
}

//----------------------------------------------------------
//  this function will do the actual opening of the file given a dir.
HANDLE 
HDD721::OpenXFSFile(HANDLE DirHandle, char *Name)
{
	unsigned long		InodeNumber;
	unsigned long				j;
	pXFSFILEPOINTER		File;
	xfs_dinode_t		*iPtr;
	pXFSFILEPOINTER		fPtr;

	File = (pXFSFILEPOINTER)DirHandle;
	if( DirHandle == NULL ){  // then assume the name = ROOT
		InodeNumber = (unsigned long)m_Super->sb_rootino;
	} else {
		InodeNumber = 0;
		for(j = 0; j < File->Data.DirData->NumEntries; j++){
			if( !strcmp((char *)File->Data.DirData->DirEntry[j].Name,Name)){
				InodeNumber = File->Data.DirData->DirEntry[j].InodeNumber;
				break;
			}
		}
		if( InodeNumber == 0 ) {//did not fine it...
			OutPutDebugString("Invalid INODE number!");
			return(NULL);
		}
	}
	iPtr = GetInode( InodeNumber );  //make sure you free this when done with it
	if( iPtr == NULL ){
		OutPutDebugString("Unable to locate INODE number!");
		return(NULL);
	}
	fPtr = (pXFSFILEPOINTER)new XFSFILEPOINTER;
	if( fPtr == NULL ){
		OutPutDebugString("Unable to allocate file handle!");
		delete iPtr;
		return(NULL);
	}
	fPtr->FileInodeNumber = InodeNumber;
	fPtr->FileLength = iPtr->di_core.di_size;
	fPtr->FilePosition = 0;
	if( (iPtr->di_core.di_mode & IFMT) == IFDIR ){  //then this is a directory so...
		fPtr->DataPointerType = POINTER_TYPE_DIR;
		fPtr->Data.DirData = GetDirectoryListing(iPtr);
	} else {
		if( iPtr->di_core.di_format == XFS_DINODE_FMT_LOCAL ){
			fPtr->Data.InternalFile = (unsigned char *)new unsigned char[(unsigned long)iPtr->di_core.di_size];		//allocate for the space
			if( fPtr->Data.InternalFile == NULL){
				delete iPtr;
				return(NULL);
			}
			memcpy((void *)fPtr->Data.InternalFile,(void *)&iPtr->di_u.di_c[0],(unsigned long)iPtr->di_core.di_size);
			fPtr->DataPointerType = POINTER_TYPE_IFILE;
		} else {
			fPtr->Data.SGPointer = BuildSGList( iPtr );
			fPtr->DataPointerType = POINTER_TYPE_FILE;
		}
	}
	delete iPtr;
	return(fPtr);
}

//-------------------------------------------------------------
//  this function will get the idone for the inode number
xfs_dinode_t  *
HDD721::GetInode(unsigned long InodeNumber)
{
	unsigned long	offset;
	unsigned long	AllocationGroupNumber;
	unsigned long	AllocationGroup;
	unsigned long	Sector;
	unsigned char	Buffer[4096];				//assume 4K blocks
	xfs_dinode_t	*iPtr;

	offset = (InodeNumber & 0x0000000F) * m_Super->sb_inodesize;	//0-3 == offset
	AllocationGroupNumber = ((InodeNumber >> 4) & 0x000FFFFF);		//4 - 23 == AGN
	AllocationGroup = ((InodeNumber >> 24) & 0x000000FF);			//24 - 31 == AG
	Sector = ((AllocationGroup * m_Super->sb_agblocks) + AllocationGroupNumber) * (m_Super->sb_blocksize / m_Super->sb_sectsize) + m_PartitionStart;
	ReadBlock(Sector,Buffer,(m_Super->sb_blocksize / m_Super->sb_sectsize)); //read in the logical block
	//allocate the pointer
	iPtr = (xfs_dinode_t *)new INODEBYTES;
	if( iPtr == NULL )
		return(NULL);
	memcpy((void *)iPtr,(void *)&Buffer[offset],sizeof(INODEBYTES));
 	ConvertEndian( (void *)&iPtr->di_core.di_mode,sizeof(unsigned short));
 	ConvertEndian( (void *)&iPtr->di_core.di_onlink,sizeof(unsigned short));
 	ConvertEndian( (void *)&iPtr->di_core.di_uid,sizeof(unsigned long));
 	ConvertEndian( (void *)&iPtr->di_core.di_gid,sizeof(unsigned long));
 	ConvertEndian( (void *)&iPtr->di_core.di_nlink,sizeof(unsigned long));
 	ConvertEndian( (void *)&iPtr->di_core.di_projid,sizeof(unsigned short));
 	ConvertEndian( (void *)&iPtr->di_core.di_atime.t_sec,sizeof(unsigned long));
 	ConvertEndian( (void *)&iPtr->di_core.di_atime.t_nsec,sizeof(unsigned long));
 	ConvertEndian( (void *)&iPtr->di_core.di_mtime.t_sec,sizeof(unsigned long));
 	ConvertEndian( (void *)&iPtr->di_core.di_mtime.t_nsec,sizeof(unsigned long));
 	ConvertEndian( (void *)&iPtr->di_core.di_ctime.t_sec,sizeof(unsigned long));
 	ConvertEndian( (void *)&iPtr->di_core.di_ctime.t_nsec,sizeof(unsigned long));
 
 	ConvertEndian( (void *)&iPtr->di_core.di_size,sizeof(int64));
 	ConvertEndian( (void *)&iPtr->di_core.di_nblocks,sizeof(int64));
 	ConvertEndian( (void *)&iPtr->di_core.di_extsize,sizeof(unsigned long));
 	ConvertEndian( (void *)&iPtr->di_core.di_nextents,sizeof(unsigned long));
 	ConvertEndian( (void *)&iPtr->di_core.di_anextents,sizeof(unsigned short));
 	ConvertEndian( (void *)&iPtr->di_core.di_dmevmask,sizeof(unsigned long));
 	ConvertEndian( (void *)&iPtr->di_core.di_dmstate,sizeof(unsigned short));
 	ConvertEndian( (void *)&iPtr->di_core.di_flags,sizeof(unsigned short));
 	ConvertEndian( (void *)&iPtr->di_core.di_gen,sizeof(unsigned long));
	return(iPtr);
}

//----------------------------------------------------------
// this function will parse a directory file and get the elements
pXFSDIR 
HDD721::GetDirectoryListing(xfs_dinode_t *iPtr)
{
	unsigned char	*Buffer;
	pXFSDIR	rc;
	unsigned char	*cPtr;
	unsigned long	j,DirRead;
	xfs_dir2_sf_t		*d2Ptr;
	xfs_dir2_sf_entry_t *dePtr;
	xfs_dinode_t		*tmpIPtr;
	unsigned long		Lba,Sectors;
	unsigned long		scan;
	xfs_dir2_data_hdr_t *dHdrPtr;
	xfs_dir2_data_union_t	*duPtr;
	void				*vPtr;


	rc =(pXFSDIR)new XFSDIR;
	if( rc == NULL)
		return(NULL);
	rc->NumEntries = 0;
	Buffer = (unsigned char *)new unsigned char[(unsigned long)iPtr->di_core.di_size];		//allocate for the space
	if( Buffer == NULL)
		return(rc);
	if( iPtr->di_core.di_format == XFS_DINODE_FMT_LOCAL ){
		memcpy((void *)Buffer,(void *)&iPtr->di_u.di_c[0],(unsigned long)iPtr->di_core.di_size);
		//strip contents
		cPtr = Buffer;
		d2Ptr = (xfs_dir2_sf_t *)cPtr;

		ConvertEndian( (void *)&d2Ptr->hdr.parent,sizeof(unsigned long));
		rc->DirEntry[rc->NumEntries].InodeNumber = d2Ptr->hdr.parent.i4; //need to check this out!
		strcpy((char *)rc->DirEntry[rc->NumEntries].Name,"..");
		tmpIPtr = GetInode(rc->DirEntry[rc->NumEntries].InodeNumber);
		rc->DirEntry[rc->NumEntries].Size = tmpIPtr->di_core.di_size;
		rc->DirEntry[rc->NumEntries].EntryType = ((tmpIPtr->di_core.di_mode & IFMT) == IFDIR)? POINTER_TYPE_DIR:POINTER_TYPE_FILE;
		rc->NumEntries++;
		delete tmpIPtr;
		
		cPtr += sizeof(xfs_dir2_sf_hdr_t);
		for( j = 0; j < d2Ptr->hdr.count; j++){
			dePtr = (xfs_dir2_sf_entry_t *)cPtr;
			vPtr = (void *)(cPtr + dePtr->namelen + 3);
			
			ConvertEndian( vPtr,sizeof(unsigned long));
			rc->DirEntry[rc->NumEntries].InodeNumber = *(unsigned long *)vPtr; //need to check this out!
			memcpy((void *)&rc->DirEntry[rc->NumEntries].Name[0],(void *)&dePtr->name[0],dePtr->namelen);
			rc->DirEntry[rc->NumEntries].Name[dePtr->namelen] = 0x00;
			tmpIPtr = GetInode(rc->DirEntry[rc->NumEntries].InodeNumber);
			rc->DirEntry[rc->NumEntries].Size = tmpIPtr->di_core.di_size;
			rc->DirEntry[rc->NumEntries].EntryType = ((tmpIPtr->di_core.di_mode & IFMT) == IFDIR)? POINTER_TYPE_DIR:POINTER_TYPE_FILE;
			cPtr += dePtr->namelen + 7; //or 11 depends in the inode size
			rc->NumEntries++;
			delete tmpIPtr;
		}
	} else {
		if( iPtr->di_core.di_format == XFS_DINODE_FMT_EXTENTS ){
			cPtr = Buffer;
			DirRead = 0;
			for( j = 0; j < iPtr->di_core.di_nextents && DirRead < (unsigned long)iPtr->di_core.di_size ; j++){
				Lba = ConvertExtent(&iPtr->di_u.di_bmx[j],&Sectors);
				ReadBlock(Lba,cPtr,Sectors);
				cPtr += Sectors * m_Super->sb_sectsize;
				DirRead += Sectors * m_Super->sb_sectsize;
			}
			//now the buffer is full of block dir data
			cPtr = Buffer;
			scan = 0;
			dHdrPtr = (xfs_dir2_data_hdr_t *)cPtr;
			ConvertEndian( (void *)&dHdrPtr->bestfree[0].offset,sizeof(xfs_dir2_data_off_t));
			ConvertEndian( (void *)&dHdrPtr->bestfree[0].length,sizeof(xfs_dir2_data_off_t));
			ConvertEndian( (void *)&dHdrPtr->bestfree[1].offset,sizeof(xfs_dir2_data_off_t));
			ConvertEndian( (void *)&dHdrPtr->bestfree[1].length,sizeof(xfs_dir2_data_off_t));
			ConvertEndian( (void *)&dHdrPtr->bestfree[2].offset,sizeof(xfs_dir2_data_off_t));
			ConvertEndian( (void *)&dHdrPtr->bestfree[2].length,sizeof(xfs_dir2_data_off_t));
			cPtr += sizeof(xfs_dir2_data_hdr_t);
			scan += sizeof(xfs_dir2_data_hdr_t);
			while( scan < iPtr->di_core.di_size ){
				duPtr = (xfs_dir2_data_union_t *)cPtr;
				if( duPtr->unused.freetag == 0xFFFF ){
					ConvertEndian( (void *)&duPtr->unused.length,sizeof(unsigned short));
					scan += duPtr->unused.length;
					if( scan % 8 )
						scan += 8 - (scan % 8);
				} else { // then a dir entry
					ConvertEndian( (void *)&duPtr->entry.inumber,sizeof(int64));
					if( duPtr->entry.namelen == 0 )
						break;
					rc->DirEntry[rc->NumEntries].InodeNumber = (unsigned long)duPtr->entry.inumber; 
					memcpy((void *)&rc->DirEntry[rc->NumEntries].Name[0],(void *)&duPtr->entry.name[0],duPtr->entry.namelen);
					rc->DirEntry[rc->NumEntries].Name[duPtr->entry.namelen] = 0x00;
					tmpIPtr = GetInode(rc->DirEntry[rc->NumEntries].InodeNumber);
					rc->DirEntry[rc->NumEntries].Size = tmpIPtr->di_core.di_size;
					rc->DirEntry[rc->NumEntries].EntryType = ((tmpIPtr->di_core.di_mode & IFMT) == IFDIR)? POINTER_TYPE_DIR:POINTER_TYPE_FILE;
					rc->NumEntries++;
					scan += sizeof(xfs_ino_t) + duPtr->entry.namelen + 1;
					delete tmpIPtr;
					if( scan % 8 )
						scan += 8 - (scan % 8);
				}
				cPtr = &Buffer[scan];
			}
		} else {
			//big Freak dir should never get here
		}
	}
	delete []Buffer;
	return(rc);
}

//----------------------------------------------------------
//  this converts the extents to a usable structure
unsigned long 
HDD721::ConvertExtent(xfs_bmbt_rec_32_t *ext, unsigned long *SectorCount)
{
	unsigned long	count;
	unsigned long	LBA;
	
	ConvertEndian( (void *)&ext->l0,sizeof(unsigned long));
	ConvertEndian( (void *)&ext->l1,sizeof(unsigned long));
	ConvertEndian( (void *)&ext->l2,sizeof(unsigned long));
	ConvertEndian( (void *)&ext->l3,sizeof(unsigned long));
	count = ext->l3 & 0x1FFFFF;
	LBA = ext->l2;
	LBA <<= 11;
	LBA |= (ext->l3 >> 21) & 0x7FF;
	LBA *= (m_Super->sb_blocksize / m_Super->sb_sectsize);
	LBA += m_PartitionStart;
	count *= (m_Super->sb_blocksize / m_Super->sb_sectsize);
	*SectorCount = count;
	return(LBA);
}

//-------------------------------------------------------------
//	this function will build the sg list for a given inode
pSGELEMENT 
HDD721::BuildSGList(xfs_dinode_t *iPtr)
{
	pSGELEMENT	base,sPtr;
	unsigned long		j,k;
	unsigned long		Lba,count;
	unsigned char		Leaf[4096];
	xfs_btree_parent_t	*bPtr;
	xfs_btree_block_t	*tPtr;
	//xfs_btree_block_t	*btreeBlock;

	//get the number of extents used
	base = (pSGELEMENT)new SGELEMENT[ iPtr->di_core.di_nextents ];
	if( base == NULL )
		return(NULL);
	sPtr = base;
	if( iPtr->di_core.di_format == XFS_DINODE_FMT_EXTENTS ){ //then in extent format
		for( j = 0; j < iPtr->di_core.di_nextents; j++){
			Lba = ConvertExtent(&iPtr->di_u.di_bmx[j],&count);
			sPtr->sgStartLba = Lba;
			sPtr->sgLength = count;
			sPtr++;
		}
	} else { //in btree format
		//from the root node get to the left most leaf
		ConvertEndian( (void *)&iPtr->di_u.di_bmbt.bb_level,sizeof(unsigned short));
		ConvertEndian( (void *)&iPtr->di_u.di_bmbt.bb_numrecs,sizeof(unsigned short));
		ConvertEndian( (void *)&iPtr->di_u.di_bmbt.bTree_Ptr[0],sizeof(int64));
		Lba = (unsigned long)iPtr->di_u.di_bmbt.bTree_Ptr[0] * 8;
		do{
			ReadBlock(Lba + m_PartitionStart,Leaf,8);
			bPtr = (xfs_btree_parent_t *)Leaf;
			ConvertEndian( (void *)&bPtr->bb_h.bb_level,sizeof(unsigned short));
			ConvertEndian( (void *)&bPtr->bb_h.bb_numrecs,sizeof(unsigned short));
			if( bPtr->bb_h.bb_level > 0 ){
#ifdef __MS__
				while( bPtr->bb_leftsib != (int64)0xFFFFFFFFFFFFFFFF){
#endif
#ifdef __MAC__
				while( bPtr->bb_leftsib != (int64)0xFFFFFFFFFFFFFFFFLL){
#endif
					ConvertEndian( (void *)&bPtr->bb_leftsib,sizeof(int64));
					Lba = (unsigned long)bPtr->bb_Ptr[0] * 8;
					ReadBlock(Lba + m_PartitionStart,Leaf,8);
				}
				ConvertEndian( (void *)&bPtr->bb_Ptr[0],sizeof(int64));
				Lba = (unsigned long)bPtr->bb_Ptr[0] * 8;
			}
		} while( bPtr->bb_h.bb_level > 0 );
		tPtr = (xfs_btree_block_t *)Leaf;
		k = 0;
		while( k <  iPtr->di_core.di_nextents ){  //do all the extents in all the nodes
			for( j = 0; j < tPtr->bb_h.bb_numrecs; j++){
				Lba = ConvertExtent(&tPtr->rec[j],&count);
				sPtr->sgStartLba = Lba;
				sPtr->sgLength = count;
				sPtr++;
				k++;
			}
#ifdef __MS__
			if( tPtr->bb_rightsib != (int64)0xFFFFFFFFFFFFFFFF && k < iPtr->di_core.di_nextents){
#endif
#ifdef __MAC__
			if( tPtr->bb_rightsib != (int64)0xFFFFFFFFFFFFFFFFLL && k < iPtr->di_core.di_nextents){
#endif
				ConvertEndian( (void *)&tPtr->bb_rightsib,sizeof(int64));
				Lba = (unsigned long)bPtr->bb_rightsib * 8;
				ReadBlock(Lba + m_PartitionStart,Leaf,8);
				tPtr = (xfs_btree_block_t *)Leaf;
				ConvertEndian( (void *)&tPtr->bb_h.bb_level,sizeof(unsigned short));
				ConvertEndian( (void *)&tPtr->bb_h.bb_numrecs,sizeof(unsigned short));
			}

			
		}
	}
	return(base);
}

//---------------------------------------------------
// endien conversion
unsigned long	
HDD721::Cvt( unsigned long x)
{
#ifdef __x86__
	return(x);
#endif
#ifdef __PwPC__
	unsigned long rc;
	unsigned char cPtr[5];
	unsigned char t;
	
	*(unsigned long *)&cPtr[0] = x;
	t = cPtr[0];
	cPtr[0] = cPtr[3];
	cPtr[3] = t;
	t = cPtr[1];
	cPtr[1] = cPtr[2];
	cPtr[2] = t;
	rc = *(unsigned long *)&cPtr[0];
	return(rc);
#endif
}
//---------------------------------------------------
// endien conversion
unsigned short	
HDD721::Cvts( unsigned short x)
{
#ifdef __x86__
	return(x);
#endif
#ifdef __PwPC__
	unsigned short rc;
	unsigned char cPtr[5];
	unsigned char t;
	
	*(unsigned short *)&cPtr[0] = x;
	t = cPtr[0];
	cPtr[0] = cPtr[1];
	cPtr[1] = t;
	rc = *(unsigned short *)&cPtr[0];
	return(rc);
#endif
}

