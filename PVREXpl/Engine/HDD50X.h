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

// HDD50X.h: interface for the HDD50X class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __HDD50X__
#define __HDD50X__

//----------------------------- headers ------------------------
#include "Platform.h"
#include "GlobalDefs.h"
#include "HDDPVR.h"
#include "LogFile.h"

//----------------------------- definitions --------------------

typedef struct {  
        char			szCorpName[32]; 
        char			szSignature[32]; 
        unsigned char   bZero; 
        unsigned char   bNumberPartitions; 
		unsigned short	wUnused;
        unsigned long	dwPart[5][2];  //first/last sector of 5 partition s
        unsigned long   dwUnkn0;    // for now = 768
        unsigned long   dwUnkn1;    // for now = 0
        unsigned long   dwUnused[1]; 
        char			cStatus1[4]; // "003D" - factory; "004D" - re-init; "005D" - working
        char			cStatus2[4]; // "000C" - factory; "001C" - working
        unsigned long   dwUnused2[94]; 
} DISH_MBR, *pDISH_MBR;

typedef struct {
	char			szSignature[16];
	char			szPartitionName[16];
	unsigned long	res1[3];
	unsigned long	LinkListNumber; // MAU - number of sectors in allocated unit ( cluster size )
	unsigned long	PartitionType;
	LBAPTR			Root;
	LBAPTR			This;
	LBAPTR			NextPartition;
	unsigned long	res;
	LBAPTR			InfoSector;
	LBAPTR			FatNodeStart;
} PARTITION_SEC, *pPARTITION_SEC;

typedef struct {
	char			FileName[32];
	unsigned long	SomeFlag;
	unsigned long	Unknown;
	LBAPTR			Root[2];
	LBAPTR			This;
	LBAPTR			Root2[2];
	LBAPTR			NextEntry;
	LBAPTR			Root3[2];
	LBAPTR			PreviousEntry;
	unsigned long	FreeFlag;
	unsigned long	null[32];
	unsigned long	BytesLeftInCluster;
	unsigned long	SpaceOnDiskH;
	unsigned long	SpaceOnDisk;
	unsigned long	FileSizeH;
	unsigned long	FileSize;
	LBAPTR			FatStart;
} FILE_ENTRY, *pFILE_ENTRY;

typedef struct {
	LBAPTR			FatNodeStart;
	unsigned long	NumberOfClusters;
	unsigned long	unknown[2];
} INFO_SECTOR, *pINFO_SECTOR;

typedef struct {
	LBAPTR			NextFatNode;
	LBAPTR			PreviousFatNode;
	unsigned long	ClusterSize;
	unsigned long	FreeFlag;
	unsigned long	res[24];
	LBAPTR			Cluster[100];
} FAT_NODE, *pFAT_NODE;

typedef struct {
	unsigned long	res[64];
	unsigned long	Unknown;
	LBAPTR			This;
	unsigned long	null[3];
	LBAPTR			ThisNode[2];
	LBAPTR			FirstFreeEntry;
	LBAPTR			ThisNode2[2];
	LBAPTR			FirstEntry;
	LBAPTR			ThisNode3[2];
	LBAPTR			LastUsedEntry;
	LBAPTR			ThisNode4[2];
	LBAPTR			LastEntry;
} ROOT_ENTRY, *pROOT_ENTRY;

typedef struct {
	unsigned long	NumberOfCluster;			//1 if DIR file
	unsigned long	ClusterSize;				//file size if DIR file
	int64			FilePonter;
	int64			FileSize;					//file size if DIR file
	unsigned long	FatStart;					//0 if Dir file
	unsigned long	CurrentClusterNumber;		//number of direcotry entries if DIR file
	unsigned char	*FileData;					//file data
	unsigned long	Cluster[1];
} DISH_FILE_HANDLE, *pDISH_FILE_HANDLE;


typedef struct {
	char			FileName[32];
	__int64			FileSize;

} MYDIR_FILE_ENTRY, *pMYDIR_FILE_ENTRY;

//------------------------------- classes -----------------------
class HDD50X : public HDDPVR  
{
public:
	unsigned long	Cvt( unsigned long x);
	unsigned long GetPvrFileTime( char *FileName );
	void  AdjustAudioFileSize( HANDLE FileHandle );
	int64 GetPvrFileSize( HANDLE FileHandle );
	bool IsPvrDrive();
	void ClosePvrFile( HANDLE FileHandle );
	unsigned long ReadPvrFile( HANDLE fileHandle, unsigned char *Data, unsigned long Length);
	HANDLE OpenPvrFile( char *FileName );
	HDD50X();
	virtual ~HDD50X();

private:
	pFILE_ENTRY		FindFirst(char *PartitionName, pFILE_ENTRY Entry);
	pFILE_ENTRY		FindNext(pFILE_ENTRY CurrentEntry);
	HANDLE			DishOpenFile(pFILE_ENTRY FileEntry);

	//Vars	
	int				m_NumberOfPartitions;
	int				GetNumberOfPartitions();
	unsigned long	m_PartArray[7][6];
};

#endif // __HDD50X__
