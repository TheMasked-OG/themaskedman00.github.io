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
#include "HDD50X.h"
#include "LogFile.h"

//////////////////////////////////////////////////////////////////////
//                  Globals 
//////////////////////////////////////////////////////////////////////
extern LogFile		dbgLog;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HDD50X::HDD50X()
{
	m_NumberOfPartitions = 0;
}

HDD50X::~HDD50X()
{

}

//////////////////////////////////////////////////////////////////////
// Public Methods
//////////////////////////////////////////////////////////////////////

//--------------------------------
// this function will open the file on a 50X pVR HDD
HANDLE 
HDD50X::OpenPvrFile(char *Path)
{
	char			*Partition;
	char			*FileName;
	FILE_ENTRY		FileEntry;	
	pFILE_ENTRY		rc;
	HANDLE			FileHandle;
	int				j;
	char			PathFileName[128];
	bool			FolderScan;
	pDISH_FILE_HANDLE	fh;
	pMYDIR_FILE_ENTRY	fEntry;

	strcpy(PathFileName,Path);
	if( m_NumberOfPartitions == 0 )
		GetNumberOfPartitions();
	//split the name
	j = 0;
	if( PathFileName[j] == '/' )
		j++;
	Partition = &PathFileName[j];
	while( PathFileName[j] != 0x00 && PathFileName[j] != '/' && j < (int)strlen(PathFileName))
		j++;
	if( PathFileName[j] != '/' ){
		dbgLog.Sprintf("Invalid PVR File Path : %s",PathFileName);
		return(NULL);  //something wrong in the path
	}
	PathFileName[j] = 0x00;
	FileName = &PathFileName[j + 1]; 
	rc = FindFirst( Partition,&FileEntry); 
	FolderScan = false;
	if( !stricmp(FileName,"FOLDER_DIR.DIR")){
		//we are opening this to create a ram file that lists the directory entry
		FolderScan = true;
		fh = (pDISH_FILE_HANDLE)new unsigned char[sizeof(DISH_FILE_HANDLE)];
		fh->ClusterSize = 0;
		fh->NumberOfCluster = 1;
		fh->FilePonter = 0;
		fh->FileSize = 0;
		fh->FatStart = 0;
		fh->CurrentClusterNumber = 0;
		fh->FileData = (unsigned char *)new char[sizeof(MYDIR_FILE_ENTRY) * 1024];  //size for 1024 entries
		fEntry = (pMYDIR_FILE_ENTRY)fh->FileData;
	}
	while( rc != NULL )
	{
		if( FolderScan ){
			j = fh->CurrentClusterNumber;
			strcpy(fEntry[j].FileName,FileEntry.FileName);
			fEntry[j].FileSize = (int64)Cvt(FileEntry.FileSizeH);
			fEntry[j].FileSize <<= 32;
			fEntry[j].FileSize |= (int64)Cvt(FileEntry.FileSize);
			fh->CurrentClusterNumber++;
			fh->FileSize += sizeof(MYDIR_FILE_ENTRY);
			fh->ClusterSize += sizeof(MYDIR_FILE_ENTRY);
		} else {
			if( !stricmp(FileEntry.FileName,FileName)) 
			{
				break;
			}
		}
		rc = FindNext(&FileEntry);
	}
	if( rc != NULL )
	{
		FileHandle = DishOpenFile(&FileEntry);
		return(FileHandle);
	}
	if( FolderScan )
		return((HANDLE)fh);
	dbgLog.Sprintf("PVR File not found : %s",PathFileName);
	return(NULL);
}

//----------------------------------
// This function will read from a pvr file
unsigned long 
HDD50X::ReadPvrFile(HANDLE FileHandle, unsigned char *Data, unsigned long Length)
{
	pDISH_FILE_HANDLE	Handle;
	unsigned long		ClusterIndex;
	unsigned long		Begin,ThisLength;
	unsigned long		Amount;
	unsigned long		AmountRead = 0;
	
	Handle = (pDISH_FILE_HANDLE)FileHandle;
	memset(Data,0xCD,Length);
	Amount = Length; //assume this in most cases
	if( Handle->FilePonter + Length > Handle->FileSize){
		Amount = (unsigned long)(Handle->FileSize - Handle->FilePonter);
		Length = Amount;
	}
	//now check for special file
	if( Handle->FatStart == 0 && Handle->NumberOfCluster == 1 ){
		memcpy((void *)Data,(void *)&Handle->FileData[Handle->FilePonter],Amount);
		Handle->FilePonter += Amount;
		return(AmountRead);
	}
	if( Handle->FilePonter >= Handle->FileSize)
		return(0);  //EOF
	ClusterIndex = (unsigned long)(Handle->FilePonter / ((int64)Handle->ClusterSize  * (int64)512));
	if( Handle->CurrentClusterNumber != Handle->Cluster[ClusterIndex]){
		if( !ReadBlock(Handle->Cluster[ClusterIndex],Handle->FileData,Handle->ClusterSize))
			return(0);
		Handle->CurrentClusterNumber = Handle->Cluster[ClusterIndex];
	}
	Begin = (unsigned long)(Handle->FilePonter % (int64)(Handle->ClusterSize  * 512));
	ThisLength = (Handle->ClusterSize  * 512) - Begin;
	if( Length < ThisLength ){
		ThisLength = Length;
	}
	if( (Handle->FilePonter + ThisLength) >= Handle->FileSize){
    		ThisLength = (unsigned long)(Handle->FileSize - Handle->FilePonter);
	}
	memcpy((void *)Data,(void *)&Handle->FileData[Begin],ThisLength);
	Handle->FilePonter += ThisLength;
	Length -= ThisLength;
	if( Length ){
		return(ThisLength + ReadPvrFile(FileHandle,&Data[ThisLength],Length));
	}
	AmountRead = Amount;
	return(AmountRead);
}

//----------------------------------
// this function will close and clean up from a file
void 
HDD50X::ClosePvrFile(HANDLE FileHandle)
{
	pDISH_FILE_HANDLE	Handle;
	unsigned char		*cPtr;
	
	if( NULL != FileHandle ) {
		Handle = (pDISH_FILE_HANDLE)FileHandle;
		if( NULL != Handle->FileData )
			delete []Handle->FileData;
		cPtr = (unsigned char *)FileHandle;
		delete []cPtr;
	}
}

//-----------------------------------
//   this function will determin if the drive is really a pvr drive of this type
bool 
HDD50X::IsPvrDrive()
{
	int		NumPartitions;

	m_NumberOfPartitions = 0;
	NumPartitions = GetNumberOfPartitions();
	if( NumPartitions == 0 )
		return(false);
	return(true);
}

//------------------------------------------
//  this function will return the file size of the opend file
int64 
HDD50X::GetPvrFileSize( HANDLE FileHandle )
{
	pDISH_FILE_HANDLE	Handle;
	
	if( NULL == FileHandle )
		return(0);
	Handle = (pDISH_FILE_HANDLE)FileHandle;
	return(Handle->FileSize);

}

//------------------------------------------
//  this function will round the audio file size up to the neares cluster.
void 
HDD50X::AdjustAudioFileSize(HANDLE FileHandle)
{
	
	pDISH_FILE_HANDLE  Handle;
	
	Handle = (pDISH_FILE_HANDLE)FileHandle;
	//now round up to the next largest cluster size.
	//this could leave garbadge at the end of the audio file!
	
	
	Handle->FileSize += (Handle->FileSize % (Handle->ClusterSize * 512));


}

//------------------------------------------
//  this function will no thothing here we do not need this info at this level
unsigned long 
HDD50X::GetPvrFileTime( char *FileName )
{
	return(0);
}

//////////////////////////////////////////////////////////////////////
// Private Methods
//////////////////////////////////////////////////////////////////////

char			*ptSign="DaveSystemDisk";
char			*pSign="Long Live Dave!";
//-----------------------------------------
// this function will return the number of valid 50x partitions found
int 
HDD50X::GetNumberOfPartitions()
{
	int				rc = 0;
	unsigned char	mbrBuffer[512];
	unsigned char	Buffer[512];
	pDISH_MBR		mbr;
	pPARTITION_SEC	pPtr;
	LBAPTR			partSector;
	char			tstring[120];
	unsigned char	totalParts;
	
	if( m_NumberOfPartitions > 0 )     //if we have already done this then do not do this again
		return(m_NumberOfPartitions);
	//first scan for 50x/510
	memset(mbrBuffer,0,512);
	ReadBlock(0,mbrBuffer,1);
	mbr = (pDISH_MBR)mbrBuffer;
	totalParts = mbr->bNumberPartitions;
	partSector = Cvt(mbr->dwPart[0][0]);
	tstring[0]=0;
	if( !strcmp(mbr->szSignature,ptSign) ) //check signature first
	{
		while( totalParts  )
		{
			if( !ReadBlock(partSector,Buffer,1))// read next partition
				break;
			pPtr = (pPARTITION_SEC)Buffer;
			if( !strcmp(pPtr->szSignature,pSign)) //check signature first
			{
				m_PartArray[rc][0] = Cvt(mbr->dwPart[rc][0]);
				strcpy((char *)&m_PartArray[rc][1], pPtr->szPartitionName);
				rc++; // next partition
				partSector = Cvt(mbr->dwPart[rc][0]); // next partition's  sector
				totalParts--; // left processing
			} else 	{
//				never should be here !
				dbgLog.Sprintf("Problem with the PT content!\nSignature: %s",pPtr->szSignature);
				dbgLog.Sprintf("(sector: %lu) Invalid Partition Signature!",partSector);
				dbgLog.PrintData((unsigned char *)pPtr,512);
				return(0);
			}
		}
		m_NumberOfPartitions = rc;
	}
	return(rc);
}

//-----------------------------------------
// this function will Execute the find first function
pFILE_ENTRY 
HDD50X::FindFirst(char *PartitionName, pFILE_ENTRY Entry)
{
	pFILE_ENTRY		rc;
	pPARTITION_SEC	pPtr;
	pROOT_ENTRY		Root;
	unsigned char	Buffer[512];
	int				i=0;

	while (m_PartArray[i][0])
	{
		if( !strcmp((char *)&m_PartArray[i][1],PartitionName))
			break;
		i++;
	}
	if (m_PartArray[i][0] == 0)
		return(NULL);
	ReadBlock(m_PartArray[i][0],Buffer,1);
	pPtr = (pPARTITION_SEC)Buffer;
	if( strcmp(pPtr->szSignature,pSign)) {//check signature first
		dbgLog.PrintLnLog("Invalid Partition Signature!");
		return(NULL);
	}
	ReadBlock(Cvt(pPtr->Root),Buffer,1);
	Root = (pROOT_ENTRY)Buffer;
	ReadBlock(Cvt(Root->FirstEntry),Buffer,1);
	rc = (pFILE_ENTRY)Buffer;
	while( rc->FreeFlag != 0 && rc->NextEntry != 0xFFFFFFFF ){
		ReadBlock(Cvt(rc->NextEntry),Buffer,1);
	}
	if( rc->FreeFlag == 0 ){
		memcpy((void *)Entry,(void *)Buffer,sizeof(FILE_ENTRY));
		rc = Entry;
	} else {
		rc = NULL;
	}
	return(rc);
}

//--------------------------------------------------------------------//
//  this function will find the next in a search sequence
pFILE_ENTRY 
HDD50X::FindNext(pFILE_ENTRY CurrentEntry)
{
	pFILE_ENTRY		rc;
	unsigned char	Buffer[512];

	rc = (pFILE_ENTRY)Buffer;
	ReadBlock(Cvt(CurrentEntry->NextEntry),Buffer,1);
	while( rc->FreeFlag != 0 && rc->NextEntry != 0xFFFFFFFF ){
		if( !ReadBlock(Cvt(rc->NextEntry),Buffer,1))
			return(NULL);
	}
	if( rc->FreeFlag == 0 ){
		memcpy((void *)CurrentEntry,(void *)Buffer,sizeof(FILE_ENTRY));
		rc = CurrentEntry;
	} else {
		rc = NULL;
	}
	return(rc);
}

//-------------------------------------------------
//  this function will open the 50x file found for reading
HANDLE 
HDD50X::DishOpenFile(pFILE_ENTRY FileEntry)
{
	pDISH_FILE_HANDLE	FileHandle;
	pFAT_NODE			Fat;
	unsigned char		Buffer[512];
	int					NumClusters;
	int					j;
	int					AllocSize;

	ReadBlock(Cvt(FileEntry->FatStart),Buffer,1);
	Fat = (pFAT_NODE)Buffer;
	NumClusters = 0;
	while(!Fat->FreeFlag) 
	{
		for( j = 0; j < 100; j++ )
		{
			if( Cvt(Fat->Cluster[j]) != 0x000000 && Cvt(Fat->Cluster[j]) != 0xFFFFFFFF )
				NumClusters++;
		}
		if( Cvt(Fat->NextFatNode) == 0x00000000 || Cvt(Fat->NextFatNode) == 0xFFFFFFFF )
			break;
		ReadBlock(Cvt(Fat->NextFatNode),Buffer,1);
	}
	AllocSize = sizeof(DISH_FILE_HANDLE) + (sizeof(unsigned long) * NumClusters) + 255;
	FileHandle = (pDISH_FILE_HANDLE)new unsigned char[AllocSize];
	if( FileHandle == NULL ){
		dbgLog.PrintLnLog("Unable to allocate File Handle !");
		return(FileHandle);
	}
	ReadBlock(Cvt(FileEntry->FatStart),Buffer,1);
	Fat = (pFAT_NODE)Buffer;
	FileHandle->NumberOfCluster = NumClusters;
	FileHandle->ClusterSize = Cvt(Fat->ClusterSize);
	FileHandle->FilePonter = 0;
	
	FileHandle->FileSize = (int64)Cvt(FileEntry->FileSizeH);
	FileHandle->FileSize <<= 32;
	FileHandle->FileSize |= (int64)Cvt(FileEntry->FileSize);
	
	FileHandle->FatStart = Cvt(FileEntry->FatStart);
	FileHandle->CurrentClusterNumber = 0xFFFFFFFF;
	AllocSize = FileHandle->ClusterSize * 512;
	FileHandle->FileData = (unsigned char *)new unsigned char[AllocSize];
	if( FileHandle->FileData == NULL ){
		dbgLog.PrintLnLog("Unable to allocate File Handle buffer!");
		free(FileHandle);
		return(NULL);
	}
	NumClusters = 0;
	while(!Fat->FreeFlag) 
	{
		for( j = 0; j < 100; j++ )
		{
			if( Cvt(Fat->Cluster[j]) != 0x000000 && Cvt(Fat->Cluster[j]) != 0xFFFFFFFF )
				FileHandle->Cluster[NumClusters++] = Cvt(Fat->Cluster[j]);
		}
		if( Cvt(Fat->NextFatNode) == 0x00000000 || Cvt(Fat->NextFatNode) == 0xFFFFFFFF )
			break;
		ReadBlock(Cvt(Fat->NextFatNode),Buffer,1);
	}
	return(FileHandle);
}

//---------------------------------------------------
// endien conversion
unsigned long	
HDD50X::Cvt( unsigned long x)
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

