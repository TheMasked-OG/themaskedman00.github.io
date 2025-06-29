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

// HDD50X.cpp: implementation of the HDD50X class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HDD522.h"
#include "Partition.h"
#include "LogFile.h"

//////////////////////////////////////////////////////////////////////
//                  Globals 
//////////////////////////////////////////////////////////////////////
extern LogFile		dbgLog;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HDD522::HDD522()
{
	m_Root = NULL;
	m_Mounted = -1;
}

HDD522::~HDD522()
{
	if( m_Root != NULL )
		ClosePvrFile(m_Root);
	m_Root = NULL;
}

//////////////////////////////////////////////////////////////////////
// Public Methods
//////////////////////////////////////////////////////////////////////

//--------------------------------
// this function will open the file on a 50X pVR HDD
HANDLE 
HDD522::OpenPvrFile(char *FileName)
{
	char	*Name;
	int		PartNum;
	HANDLE	retHandle;

	//ok the the partition number should be the first in the file name ie "0/Catalog.cat", or "2/in8001.tsp"
	PartNum = FileName[0] - '0';
	Name = &FileName[2];
	if( m_Mounted != PartNum ){
		GetNumberOfPartitions();
		if( PartNum != 0 && PartNum != 2 ){
			dbgLog.PrintLnLog("Invalid Partition number!");
			return(NULL);  //we only do these partitions
		}
		MountExt3(PartNum);
	}
	if( m_Root == NULL ){
		m_Root = OpenExt2File( NULL,"root");
		if( m_Root == NULL ){
			dbgLog.PrintLnLog("Unable to open Root Directory!");
			return(NULL);   //if we cannot get the root then fail
		}
	}
	dbgLog.Sprintf("Opening Ext3 file: %s",FileName);
	retHandle = OpenExt2File(m_Root,Name);
	
	return(retHandle);
}

//----------------------------------
// This function will read from a pvr file
unsigned long 
HDD522::ReadPvrFile(HANDLE FileHandle, unsigned char *Buffer, unsigned long Length)
{
	pEXTFILEPOINTER fPtr;
	unsigned long	fpStartBlock,fpStartOffset,sgLeft;
	unsigned char	tBuffer[4096];
	pSGLIST			sPtr;
	unsigned long	x,AmountRead,j;

	AmountRead = 0;
	fPtr = (pEXTFILEPOINTER)FileHandle;
	if( Length == 0 )
		return(0); // not an error
	// which block is the filepointer in
	fpStartBlock = (unsigned long)(fPtr->FilePointer / m_Super.s_log_block_size);
	fpStartOffset = (unsigned long)(fPtr->FilePointer % m_Super.s_log_block_size);
	if( fPtr->FilePointer >= fPtr->FileLength )
		return(0);  //EOF
	sPtr = fPtr->Data.SGPointer;
	if( fpStartOffset > 0 ){
		for(j = 0; j < sPtr->NumSgElements; j++){
			if( fpStartBlock >= sPtr->SgElement[j].sgLength ){
				fpStartBlock -= sPtr->SgElement[j].sgLength;
				continue;
			} else {
				break;
			}
		}
		if( j >= sPtr->NumSgElements )
			return(0);  //none left!
		//now we have the sg element for start of read
		fpStartBlock += sPtr->SgElement[j].sgStartLba;
		ReadBlock(m_PartitionOffset + (fpStartBlock * m_Super.s_Sectors_Per_Block),(unsigned char *)tBuffer,m_Super.s_Sectors_Per_Block);
		x = m_Super.s_log_block_size - fpStartOffset;
		if( x > Length )
			x = Length;
		memcpy((void *)Buffer,(void *)&tBuffer[fpStartOffset],x);
		fPtr->FilePointer += x;
		Length -= x;
		Buffer += x;
		AmountRead += x;
	}
	x = 1;
	while(x > 0){
		// now lets see ho much regular blocks to do;
		fpStartBlock = (unsigned long)(fPtr->FilePointer / m_Super.s_log_block_size);
		fpStartOffset = (unsigned long)(fPtr->FilePointer % m_Super.s_log_block_size);
		// at this point fpStartOffset had better be 0
		if( fpStartOffset != 0 )
			return(AmountRead);
		sPtr = fPtr->Data.SGPointer;
		for(j = 0; j < sPtr->NumSgElements; j++){
			if( fpStartBlock >= sPtr->SgElement[j].sgLength ){
				fpStartBlock -= sPtr->SgElement[j].sgLength;
				continue;
			} else {
				break;
			}
		}
		if( j >= sPtr->NumSgElements )
			return(AmountRead);  //none left!
		//now we have the sg element for start of read
		sgLeft = sPtr->SgElement[j].sgLength - fpStartBlock;
		fpStartBlock += sPtr->SgElement[j].sgStartLba;
		x = Length / m_Super.s_log_block_size;
		if( x > sgLeft )
			x = sgLeft;
		// x now has the number of blocks to read
		if( x > 0 ){
			ReadBlock(m_PartitionOffset + (fpStartBlock * m_Super.s_Sectors_Per_Block),Buffer,x * m_Super.s_Sectors_Per_Block);
			x = (x * m_Super.s_log_block_size);
			fPtr->FilePointer += x;
			Length -= x;
			Buffer += x;
			AmountRead += x;
			x = Length / m_Super.s_log_block_size;
		}
	}


	// now see how much is left
	if( Length > 0 ){
		fpStartBlock = (unsigned long)(fPtr->FilePointer / m_Super.s_log_block_size);
		fpStartOffset = (unsigned long)(fPtr->FilePointer % m_Super.s_log_block_size);
		// at this point fpStartOffset had better be 0
		if( fpStartOffset != 0 )
			return(0);
		sPtr = fPtr->Data.SGPointer;
		for(j = 0; j < sPtr->NumSgElements; j++){
			if( fpStartBlock >= sPtr->SgElement[j].sgLength ){
				fpStartBlock -= sPtr->SgElement[j].sgLength;
				continue;
			} else {
				break;
			}
		}
		if( j >= sPtr->NumSgElements )
			return(0);  //none left!
		//now we have the sg element for start of read
		fpStartBlock += sPtr->SgElement[j].sgStartLba;
		ReadBlock(m_PartitionOffset + (fpStartBlock * m_Super.s_Sectors_Per_Block),(unsigned char *)tBuffer,m_Super.s_Sectors_Per_Block);
		x = Length;
		memcpy((void *)Buffer,(void *)&tBuffer[0],x);
		fPtr->FilePointer += x;
		Length -= x;
		Buffer += x;
		AmountRead += x;
	}
	return(AmountRead);
}

//----------------------------------
// this function will close and clean up from a file
void 
HDD522::ClosePvrFile(HANDLE FileHandle)
{
	pEXTFILEPOINTER fPtr;

	fPtr = (pEXTFILEPOINTER)FileHandle;
	if( fPtr->DataPointerType == POINTER_TYPE_DIR )
		delete fPtr->Data.DirData;
	else 
		delete fPtr->Data.SGPointer;
	delete fPtr;

}

//-----------------------------------
//   this function will determin if the drive is really a pvr drive of this type
bool 
HDD522::IsPvrDrive()
{
	int		NumParts;
	HANDLE	FileHandle;
	char	FileName[MAX_PATH];


	NumParts = GetNumberOfPartitions();
	if( NumParts == 0 )
		return(false);  //then no point in looking further
	dbgLog.Sprintf("Number of Linux Partitions Found: %d",NumParts);
	if( NumParts != 3 ) //we only know about 3
		return(false);
	//now we still do not know if it is a PVR disk...
	MountExt3(0);   //ok mount the first partition
	strcpy(FileName,"0/catalog.cat");  //now open up the catalog.cat file on partition 0 this should give us what we need
	FileHandle = OpenPvrFile(FileName);
	if( FileHandle != NULL ){
		ClosePvrFile(FileHandle);
		return(true);
	}
	dbgLog.PrintLnLog("IsDrive() : Unable to locate Catalog.cat file. Not 522/625 drive");
	return(false);
}

//------------------------------------------
//  this function will return the file size of the opend file
int64 
HDD522::GetPvrFileSize( HANDLE FileHandle )
{
	return(GetExt2FileSize(FileHandle));

}

//------------------------------------------
//  this function will round the audio file size up to the neares cluster.
void 
HDD522::AdjustAudioFileSize(HANDLE FileHandle)
{
	
	//do nothing
}
//------------------------------------------
//  this function will return the creation time of the file
unsigned long 
HDD522::GetPvrFileTime( char *FileName )
{
	char	*Name;
	int		PartNum;
	unsigned long InodeNumber,j,rc;
	pEXTFILEPOINTER	File;
	pEXT2_INODE		iPtr;


	//ok the the partition number should be the first in the file name ie "0/Catalog.cat", or "2/in8001.tsp"
	PartNum = FileName[0] - '0';
	Name = &FileName[2];
	if( m_Mounted != PartNum ){
		GetNumberOfPartitions();
		if( PartNum != 0 && PartNum != 2 ){
			dbgLog.PrintLnLog("Invalid Partition number!");
			return(0);  //we only do these partitions
		}
		MountExt3(PartNum);
	}
	if( m_Root == NULL ){
		m_Root = OpenExt2File( NULL,"root");
		if( m_Root == NULL ){
			dbgLog.PrintLnLog("Unable to open Root Directory!");
			return(0);   //if we cannot get the root then fail
		}
	}
	dbgLog.Sprintf("Getting File time Ext3 file: %s",FileName);
	File = (pEXTFILEPOINTER)m_Root;
	InodeNumber = 0;
	for( j = 0; j < File->Data.DirData->NumEntries; j++){
		if( !strcmp((char *)File->Data.DirData->DirEntry[j].Name,Name)){
			InodeNumber = File->Data.DirData->DirEntry[j].InodeNumber;
			break;
		}
	}
	if( InodeNumber == 0 ){
		return(0);  //bad
	}
	// now get the Inode
	iPtr = GetInode( InodeNumber );   //make sure we delete this...
	if( iPtr == NULL ){
		dbgLog.PrintLnLog("Unable to locate INODE!");
		return(0);
	}
	rc = iPtr->i_atime;
	delete iPtr;
	return(rc);
}
//////////////////////////////////////////////////////////////////////
// Private Methods
//////////////////////////////////////////////////////////////////////

//-----------------------------------------
// this function will return the number of valid Ext3 partitions found
int 
HDD522::GetNumberOfPartitions()
{
	PARTITIONENTRYPTR	pPtr,npPtr;
	unsigned char		Buffer[512];
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
				m_PartStart[m_NumberOfPartitions] = Cvt(npPtr->peStartSector) + ExtStart;
				m_PartSize[m_NumberOfPartitions] = Cvt(npPtr->peLength);
				m_NumberOfPartitions++;
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

//-------------------------------------------------
//  this function will point the partition at PartOffset
bool 
HDD522::MountExt3(int PartNum)
{
	unsigned char		Buffer[4096];
	pEXT2_SUPER_BLOCK	super;
	unsigned long		NumGroups,Start,j,NumDescBlocks,DescPerBlock;
	pGROUPDESCRIPTOR	gPtr;
	
	
	// load in the super block
	ReadBlock(m_PartStart[PartNum] + 2,Buffer,2);
	m_PartitionOffset = m_PartStart[PartNum];
	super = (pEXT2_SUPER_BLOCK)&Buffer[0];
	//check for ext3 signature
	memcpy((void *)&m_Super,(void *)super,sizeof(EXT2_SUPER_BLOCK));
	
	m_Super.s_inodes_count = Cvt(m_Super.s_inodes_count);
	m_Super.s_blocks_count = Cvt(m_Super.s_blocks_count);
	m_Super.s_r_blocks_count = Cvt(m_Super.s_r_blocks_count);
	m_Super.s_free_blocks_count = Cvt(m_Super.s_free_blocks_count);
	
	m_Super.s_free_inodes_count = Cvt(m_Super.s_free_inodes_count);
	m_Super.s_first_data_block = Cvt(m_Super.s_first_data_block);
	m_Super.s_log_block_size = Cvt(m_Super.s_log_block_size);
	m_Super.s_Sectors_Per_Block = Cvt(m_Super.s_Sectors_Per_Block);
	m_Super.s_blocks_per_group = Cvt(m_Super.s_blocks_per_group);
	m_Super.s_frags_per_group = Cvt(m_Super.s_frags_per_group);
	m_Super.s_inodes_per_group = Cvt(m_Super.s_inodes_per_group);
	m_Super.s_mtime = Cvt(m_Super.s_mtime);
	m_Super.s_wtime = Cvt(m_Super.s_wtime);

	m_Super.s_mnt_count = Cvts(m_Super.s_mnt_count);
	m_Super.s_max_mnt_count = Cvts(m_Super.s_max_mnt_count);
	m_Super.s_magic = Cvts(m_Super.s_magic);
	m_Super.s_state = Cvts(m_Super.s_state);
	m_Super.s_errors = Cvts(m_Super.s_errors);
	m_Super.s_minor_rev_level = Cvts(m_Super.s_minor_rev_level);

	m_Super.s_lastcheck = Cvt(m_Super.s_lastcheck);
	m_Super.s_checkinterval = Cvt(m_Super.s_checkinterval);
	m_Super.s_creator_os = Cvt(m_Super.s_creator_os);
	m_Super.s_rev_level = Cvt(m_Super.s_rev_level);

	m_Super.s_def_resuid = Cvts(m_Super.s_def_resuid);
	m_Super.s_def_resgid = Cvts(m_Super.s_def_resgid);
	m_Super.s_first_ino = Cvt(m_Super.s_first_ino);

	m_Super.s_inode_size = Cvts(m_Super.s_inode_size);
	m_Super.s_block_group_nr = Cvts(m_Super.s_block_group_nr);
	m_Super.s_feature_compat = Cvt(m_Super.s_feature_compat);
	m_Super.s_feature_incompat = Cvt(m_Super.s_feature_incompat);
	m_Super.s_feature_ro_compat = Cvt(m_Super.s_feature_ro_compat);
	m_Super.s_algorithm_usage_bitmap = Cvt(m_Super.s_algorithm_usage_bitmap);
	m_Super.s_reserved_gdt_blocks = Cvts(m_Super.s_reserved_gdt_blocks);
	m_Super.s_journal_inum = Cvt(m_Super.s_journal_inum);
	m_Super.s_journal_dev = Cvt(m_Super.s_journal_dev);
	m_Super.s_last_orphan = Cvt(m_Super.s_last_orphan);
	m_Super.s_last_orphan = Cvt(m_Super.s_last_orphan);
	m_Super.s_first_meta_bg = Cvt(m_Super.s_first_meta_bg);
	m_Super.s_reserved_word_pad = Cvts(m_Super.s_reserved_word_pad);

	if( m_Super.s_log_block_size == 0 )
		m_Super.s_log_block_size = 1024;
	if( m_Super.s_log_block_size == 1 )
		m_Super.s_log_block_size = 2048;
	if( m_Super.s_log_block_size == 2 )
		m_Super.s_log_block_size = 4096;
	m_Super.s_Sectors_Per_Block = m_Super.s_log_block_size / 512;
	//now read in the groups and get thier desriptors
	NumGroups = (m_Super.s_blocks_count - m_Super.s_first_data_block + m_Super.s_blocks_per_group - 1) / m_Super.s_blocks_per_group;
	DescPerBlock = m_Super.s_log_block_size / sizeof(GROUPDESCRIPTOR);
	
	NumDescBlocks = (NumGroups + DescPerBlock - 1) / DescPerBlock;
	for( j = 0; j < NumDescBlocks; j++ ){
		Start = Descriptor_loc(j);
		Start *= m_Super.s_Sectors_Per_Block;
		Start += m_PartStart[PartNum];
		ReadBlock(Start,Buffer,m_Super.s_Sectors_Per_Block);
		gPtr = (pGROUPDESCRIPTOR)Buffer;
		memcpy((void *)&m_Grp_desc[j * DescPerBlock],(void *)Buffer,DescPerBlock * sizeof(GROUPDESCRIPTOR));
	}
	m_Mounted = PartNum;
	if( m_Root != NULL )
		ClosePvrFile(m_Root);
	m_Root = NULL;
	return(true);

}

//--------------------------------------------------------
// this function will open the file handle with reference to a dir
HANDLE 
HDD522::OpenExt2File(HANDLE DirHandle, char *FileName)
{
	unsigned long	InodeNumber;
	unsigned long	j;
	pEXTFILEPOINTER	File;
	pEXT2_INODE		iPtr;
	pEXTFILEPOINTER fPtr;
	int64			fSize;
	pSGLIST			sgPtr;
	unsigned char	*dPtr;
	
	File = (pEXTFILEPOINTER)DirHandle;
	dbgLog.Sprintf("Opening Ext3 File: %s",FileName);
	if( DirHandle == NULL ){ // then re are opening the root
		InodeNumber = 2;
	} else {
		InodeNumber = 0;
		for( j = 0; j < File->Data.DirData->NumEntries; j++){
			if( !strcmp((char *)File->Data.DirData->DirEntry[j].Name,FileName)){
				InodeNumber = File->Data.DirData->DirEntry[j].InodeNumber;
				break;
			}
		}
	}
	dbgLog.Sprintf("Inode Number = %d",InodeNumber);
	if( InodeNumber == 0 ){
		return(NULL);  //bad
	}
	// now get the Inode
	iPtr = GetInode( InodeNumber );   //make sure we delete this...
	if( iPtr == NULL ){
		dbgLog.PrintLnLog("Unable to locate INODE!");
		return(NULL);
	}
	dbgLog.Sprintf("File Mode = 0x%X",iPtr->i_mode);
	fPtr = (pEXTFILEPOINTER)new EXTFILEPOINTER;
	if( fPtr == NULL ){
		dbgLog.PrintLnLog("Unable to allocate File Handle!");
		delete iPtr;
		return(NULL);
	}
	fPtr->FileInodeNumber = InodeNumber;
	fPtr->FilePointer = 0;
	fPtr->FileCTime = iPtr->i_ctime;
	//Fill in the scatter gather elements
	sgPtr = GetSGFromInode( iPtr );
	if( sgPtr == NULL ){
		dbgLog.PrintLnLog("Unable to Get Scatter gather list!");
		delete fPtr;
		delete iPtr;
		return(NULL);
	}
	
//debug
	dbgLog.Sprintf(" Number of SG ELements = %d",sgPtr->NumSgElements);
	for(j = 0; j < sgPtr->NumSgElements; j++){
		dbgLog.Sprintf(" %8d: %8X - %8d",j,sgPtr->SgElement[j].sgStartLba,sgPtr->SgElement[j].sgLength);
	}
	
	fPtr->FileLength = GetFileLength(iPtr,sgPtr);
	fPtr->Data.SGPointer = sgPtr;
	fPtr->DataPointerType = POINTER_TYPE_FILE;
	// now see if this is a directory then fill in the entries.
	if( S_ISDIR(iPtr->i_mode)){ // then a directory
		fSize = GetExt2FileSize((HANDLE)fPtr);
		dPtr = (unsigned char *)new unsigned char[(unsigned long)fSize];
		if( dPtr == NULL ){
			dbgLog.PrintLnLog("Unable to allocate Directory buffer!");
			delete sgPtr;
			delete fPtr;
			delete iPtr;
			return(NULL);
		}
		j = ReadPvrFile((HANDLE)fPtr,dPtr,(unsigned long)fSize);
		if( j != (unsigned long)fSize){
			// then an error reading
			dbgLog.PrintLnLog("Error Reading directory!");
			delete []dPtr;
			delete sgPtr;
			delete fPtr;
			delete iPtr;
			return(NULL);
		}
		fPtr->Data.DirData = GetDirListing(dPtr,(unsigned long)fSize);
		fPtr->DataPointerType = POINTER_TYPE_DIR;
		delete sgPtr;
		delete []dPtr;
	} else {
		if( !S_ISREG(iPtr->i_mode) ){ // then other than a regular file
			//then I do not know what it is
			dbgLog.PrintLnLog("Not a normal file!");
			delete sgPtr;
			delete fPtr;
			delete iPtr;
			return(NULL);
		}
	}
	delete iPtr;
	return((HANDLE)fPtr);
}

//--------------------------------------------------
//  this function will get an Inode from disk given a Inode number
pEXT2_INODE 
HDD522::GetInode(unsigned long InodeNumber)
{
	unsigned long				Group, Start;
	unsigned long				InodeBlockOffset,InodeBlock;
	unsigned char				Buffer[4096];
	pEXT2_INODE			iPtr;

	InodeNumber--;  //is this thing 1 based...

	Group = InodeNumber / (m_Super.s_inodes_per_group);
	InodeBlock = InodeNumber % m_Super.s_inodes_per_group;
	InodeBlockOffset = InodeBlock % ( m_Super.s_log_block_size / m_Super.s_inode_size );
	InodeBlock = InodeBlock / ( m_Super.s_log_block_size / m_Super.s_inode_size );
	
	Start = Cvt(m_Grp_desc[Group].bg_inode_table) + InodeBlock;
	Start = Start * m_Super.s_Sectors_Per_Block;
	Start += m_PartitionOffset;
	ReadBlock( Start,Buffer,m_Super.s_Sectors_Per_Block);
	iPtr = (pEXT2_INODE) new EXT2_INODE;
	memcpy((void *)iPtr,(void *)&Buffer[InodeBlockOffset * sizeof(EXT2_INODE)],sizeof(EXT2_INODE));

	iPtr->i_mode = Cvts(iPtr->i_mode);
	iPtr->i_uid = Cvts(iPtr->i_uid);
	iPtr->i_gid = Cvts(iPtr->i_gid);
	iPtr->i_links_count = Cvts(iPtr->i_links_count);

	iPtr->i_size = Cvt(iPtr->i_size);
	iPtr->i_atime = Cvt(iPtr->i_atime);
	iPtr->i_ctime = Cvt(iPtr->i_ctime);
	iPtr->i_mtime = Cvt(iPtr->i_mtime);
	iPtr->i_dtime = Cvt(iPtr->i_dtime);
	iPtr->i_blocks = Cvt(iPtr->i_blocks);
	iPtr->i_flags = Cvt(iPtr->i_flags);

	iPtr->i_version = Cvt(iPtr->i_version);
	iPtr->i_file_acl = Cvt(iPtr->i_file_acl);
	iPtr->i_dir_acl = Cvt(iPtr->i_dir_acl);
	iPtr->i_faddr = Cvt(iPtr->i_faddr);
	return(iPtr);
}

//-------------------------------------------------------
// this function will get the Size of the opened file
int64 
HDD522::GetExt2FileSize(HANDLE FileHandle)
{
	pEXTFILEPOINTER fPtr;

	fPtr = (pEXTFILEPOINTER)FileHandle;
	return(fPtr->FileLength);
}

//-------------------------------------------------------
// this function will get the Scatter Gather list from the inode
pSGLIST 
HDD522::GetSGFromInode(pEXT2_INODE iPtr)
{

	pSGLIST		sgPtr;
	

	// first of allocate an array of sg elements lets say up to 10,000 fragments
	sgPtr = (pSGLIST)new SGLIST;
	if( sgPtr == NULL ){
		dbgLog.PrintLnLog("Unable to allocate scatter gather list!");
		return(NULL);
	}
	sgPtr->NumSgElements = 0;  
	sgPtr->SgElement[0].sgStartLba = 0;
	sgPtr->SgElement[0].sgLength = 0;
	//ok get the direct blocks
	
	ProcessBlockList(sgPtr,&iPtr->i_block[0],12);
	if( iPtr->i_block[12] > 0 ){ //then we have a first degree indirect
		ProcessFistIndirect( sgPtr, Cvt(iPtr->i_block[12]));
	}
	if( iPtr->i_block[13] > 0 ){ //then we have a second degree indirect
		ProcessSecondIndirect(sgPtr, Cvt(iPtr->i_block[13]));
	}
	if( iPtr->i_block[14] > 0 ){ //then we have a third degree indirect
		ProcessThirdIndirect(sgPtr, Cvt(iPtr->i_block[14]));
	}
	if( sgPtr->SgElement[sgPtr->NumSgElements].sgStartLba != 0 )
		sgPtr->NumSgElements++;  //account for the last element
	return(sgPtr);
}

//-------------------------------------------------------
// this function will process the blocks in the Inode
bool 
HDD522::ProcessBlockList(pSGLIST spPtr, unsigned long *dPtr, unsigned long Length)
{
	unsigned long	count;
	
	dbgLog.PrintLnLog("<< Process Block List >>");
	dbgLog.PrintData(dPtr,Length);
	count = 0;
	while( count < Length ){
		if( Cvt(*dPtr) != 0 ){
			if( (spPtr->SgElement[spPtr->NumSgElements].sgStartLba + spPtr->SgElement[spPtr->NumSgElements].sgLength) == Cvt(*dPtr) ){ // then a continious block
				spPtr->SgElement[spPtr->NumSgElements].sgLength++;
			} else {
				if( spPtr->SgElement[spPtr->NumSgElements].sgStartLba != 0 ) //then the first one
					spPtr->NumSgElements++;
				if( spPtr->NumSgElements >= MAX_SG_LIST_SIZE){
					dbgLog.PrintLnLog("...File Exceeds Fragmentation limits! truncating...");
					break;// we are done no more! this file is too fragmented
				}
				spPtr->SgElement[spPtr->NumSgElements].sgStartLba = Cvt(*dPtr);
				spPtr->SgElement[spPtr->NumSgElements].sgLength = 1;
			}
		}
		count++;
		dPtr++;
	}
				
	return(true);
}

//-------------------------------------------------------
// this function will process the first indirect block from the Inode
bool 
HDD522::ProcessFistIndirect(pSGLIST sPtr, unsigned long Block)
{
	unsigned char	Buffer[4096];

	
	ReadBlock(	m_PartitionOffset + (Block * m_Super.s_Sectors_Per_Block),(unsigned char *)Buffer,m_Super.s_Sectors_Per_Block);
	dbgLog.PrintLnLog("<< Process First Indirect >>");
	return( ProcessBlockList( sPtr, (unsigned long *)&Buffer[0], m_Super.s_log_block_size / sizeof(unsigned long)));
}

//-------------------------------------------------------
// this function will process the second indirect block from the Inode
bool 
HDD522::ProcessSecondIndirect(pSGLIST sPtr, unsigned long Block)
{
	unsigned char	Buffer[4096];
	unsigned long	*dPtr;
	unsigned long	j;

	ReadBlock(	m_PartitionOffset + (Block * m_Super.s_Sectors_Per_Block),(unsigned char *)Buffer,m_Super.s_Sectors_Per_Block);
	dPtr = (unsigned long *)&Buffer[0];
	dbgLog.PrintLnLog("<< Process Second Indirect >>");
	dbgLog.PrintData(dPtr,m_Super.s_log_block_size / sizeof(unsigned long));
	for( j = 0; j < m_Super.s_log_block_size / sizeof(unsigned long); j++ ){
		if( Cvt(*dPtr) != 0 ){
			ProcessFistIndirect(sPtr,Cvt(*dPtr));
		}
		dPtr++;
	}
	return(true);
}

//-------------------------------------------------------
// this function will process the third indirect block from the Inode
bool 
HDD522::ProcessThirdIndirect(pSGLIST sPtr, unsigned long Block)
{
	unsigned char	Buffer[4096];
	unsigned long	*dPtr;
	unsigned long	j;

	ReadBlock(	m_PartitionOffset + (Block * m_Super.s_Sectors_Per_Block),(unsigned char *)Buffer,m_Super.s_Sectors_Per_Block);
	dPtr = (unsigned long *)&Buffer[0];
	dbgLog.PrintLnLog("<< Process Third Indirect >>");
	dbgLog.PrintData(dPtr,m_Super.s_log_block_size / sizeof(unsigned long));
	for( j = 0; j < m_Super.s_log_block_size / sizeof(unsigned long); j++ ){
		if( Cvt(*dPtr) != 0 ){
			ProcessSecondIndirect(sPtr,Cvt(*dPtr));
		}
		dPtr++;
	}
	return(true);

}

//--------------------------------------------------------------
// this function processes the dir file data
pEXTDIR 
HDD522::GetDirListing(unsigned char *Buffer, unsigned long Length)
{
	pEXTDIR			dPtr;
	pEXT2_DIR_ENTRY dir;
	unsigned char	*cPtr;
	pEXT2_INODE		iPtr;
	unsigned long	cnt;
	
	dPtr = (pEXTDIR)new EXTDIR;
	dPtr->NumEntries = 0;
	cPtr = Buffer;
	cnt = 0;
	while( cnt < Length ){
		dir = (pEXT2_DIR_ENTRY)cPtr;
		if(dir->name_len > 0){
			dPtr->DirEntry[dPtr->NumEntries].InodeNumber = Cvt(dir->inode);
			memcpy((void *)dPtr->DirEntry[dPtr->NumEntries].Name,(void *)dir->name,dir->name_len);
			dPtr->DirEntry[dPtr->NumEntries].Name[dir->name_len] = 0x00;
			//strcpy( (char *)dPtr->DirEntry[dPtr->NumEntries].Name,dir->name);
			dbgLog.Sprintf("> %s",(char *)dPtr->DirEntry[dPtr->NumEntries].Name);
			
			iPtr = GetInode(Cvt(dir->inode));
			dPtr->DirEntry[dPtr->NumEntries].EntryType = POINTER_TYPE_FILE;
			if( S_ISDIR(iPtr->i_mode) )
				dPtr->DirEntry[dPtr->NumEntries].EntryType = POINTER_TYPE_DIR;
			dPtr->DirEntry[dPtr->NumEntries].Size = iPtr->i_dir_acl;	
			dPtr->DirEntry[dPtr->NumEntries].Size <<= 32;
			dPtr->DirEntry[dPtr->NumEntries].Size |= iPtr->i_size;
			delete iPtr;
			dPtr->NumEntries++;
		}
		cPtr += Cvts(dir->rec_len);
		cnt += Cvts(dir->rec_len);
		if( Cvts(dir->rec_len) == 0 ) 
			break; // then we have a problem
	}
	return(dPtr);
}

//-------------------------------------------------------------
//	this function is called to get the right location of the Descriptors
unsigned long 
HDD522::Descriptor_loc(unsigned long DescBlock)
{
	unsigned long	bg,first_data_block,first_meta_bg,DescPerBlock,has_super,Logical_sb_Block;

	first_data_block = m_Super.s_first_data_block;
	first_meta_bg = m_Super.s_first_meta_bg;
	Logical_sb_Block = 1;
	if( m_Super.s_log_block_size != 1024 )
		Logical_sb_Block = 0;

	if( !(m_Super.s_feature_incompat & EXT3_FEATURE_INCOMPAT_META_BG) || DescBlock < first_meta_bg){
		return( DescBlock + Logical_sb_Block + 1 ); // or + 1 do not know
	}
	DescPerBlock = m_Super.s_log_block_size / sizeof(GROUPDESCRIPTOR);
	bg = DescPerBlock * DescBlock;
	has_super = 0;
	if( ext3_bg_has_super( bg ) )
		has_super = 1;
	return( first_data_block + has_super + (bg * m_Super.s_blocks_per_group));
}

//-------------------------------------------------------------
//	this function is called to determine if this group has a super block copy
bool 
HDD522::ext3_bg_has_super(unsigned long BlockGroup)
{
	if( (m_Super.s_feature_ro_compat & 0x0001) && !ext3_group_sparse( BlockGroup ))
		return(false);
	return(true);
}

//-------------------------------------------------------------
//	this function is called to determine if sparse mode is used in this ext3 format
bool 
HDD522::ext3_group_sparse(unsigned long Group)
{
	return( test_group( Group , 3) || test_group( Group , 5) || test_group( Group , 7));
}

//-------------------------------------------------------------
//	this function is called to determine test grouping
bool 
HDD522::test_group(unsigned long a, unsigned long b)
{
	if( a == 0 )
		return(true);
	while(1){
		if( a == 1 )
			return(true);
		if( a % b )
			return(false);
		a = a / b;
	}
	return(false); //should never get here
}
//---------------------------------------------------
// endien conversion
unsigned long	
HDD522::Cvt( unsigned long x)
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
HDD522::Cvts( unsigned short x)
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

//----------------------------------------
// this is to check the file size against the allocated blocks and to never be greater
int64 HDD522::GetFileLength(pEXT2_INODE iPtr, pSGLIST sPtr)
{
	int64		rc,blk;
	int			j;

	rc = iPtr->i_dir_acl;  //the file size high
	rc <<= 32;
	rc |= iPtr->i_size;
	//ok now get the size of the sglist
	blk = 0;
	for( j = 0; j < (int)sPtr->NumSgElements; j++){
		blk += sPtr->SgElement[j].sgLength;
	}
	blk = blk * m_Super.s_log_block_size;
	if( rc > blk )
		rc = blk;
	return(rc);

}
