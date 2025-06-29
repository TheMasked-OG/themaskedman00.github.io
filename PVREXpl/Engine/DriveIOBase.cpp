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

// DriveIOBase.cpp: implementation of the DriveIOBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Platform.h"
#include "DriveIOBase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DriveIOBase::DriveIOBase()
{
#ifdef __MS__
	m_DiskFileHandle = INVALID_HANDLE_VALUE;
#else
	m_DiskFileHandle = -1;
#endif
	m_HddOffset = 0;
}

DriveIOBase::~DriveIOBase()
{
	ClosePvrDrive();
}

//////////////////////////////////////////////////////////////////////
// Public Methods
//////////////////////////////////////////////////////////////////////

//--------------------------------
// this function will opent the HDD for reading
bool 
DriveIOBase::OpenPVRDrive(char *Path)
{
	bool	rc = false;	

#ifdef __MS__
	m_DiskFileHandle = CreateFile(Path,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if( m_DiskFileHandle != INVALID_HANDLE_VALUE ){
		rc = true;
#endif

#ifdef __MAC__
	m_DiskFileHandle = open(Path,O_RDONLY,0x777);
	if( m_DiskFileHandle > 0 ){
		rc = true;
#endif

#ifdef __LINUX__
	
#endif
	} else {
		//OutPutDebugString("Unable to open PVR File");
		//OutPutDebugString(Path);
	}
	return(rc);
}

//--------------------------------
// this function will close the hdd
void 
DriveIOBase::ClosePvrDrive()
{

#ifdef __MS__
	if( m_DiskFileHandle != INVALID_HANDLE_VALUE){
		CloseHandle(m_DiskFileHandle);
		m_DiskFileHandle = INVALID_HANDLE_VALUE;
	}
#endif

#ifdef __MAC__
	if( m_DiskFileHandle > 0 ){
		close(m_DiskFileHandle);
		m_DiskFileHandle = -1;
	}
#endif

#ifdef __LINUX__
	
#endif
}

//-----------------------------------
// this function will read a block from the drive assuming 512 bytes per sector
bool 
DriveIOBase::ReadBlock(unsigned long Lba, unsigned char *Data, unsigned long NumBlocks)
{
	int64			pos;
	unsigned long	length;
	bool			rtc = false;
#ifdef __MS__
	DWORD			PosLow;
	DWORD			PosHigh;
	DWORD			AmountRead;
	BOOL			rc;
#endif
#ifdef __MAC__
	int				AmountRead;	
#endif

	
	pos = (int64)Lba * HDD_BLOCK_SIZE;    //set the position of the LBA
	if( m_HddOffset > 0 )
		pos += (int64)m_HddOffset * HDD_BLOCK_SIZE;
	length = NumBlocks * HDD_BLOCK_SIZE;  //now the amount to read

#ifdef __MS__
	PosLow = (DWORD)(pos & 0xFFFFFFFF);
	PosHigh = (DWORD)(pos >> 32);
	SetFilePointer(m_DiskFileHandle,PosLow,(long*)&PosHigh,FILE_BEGIN);
	rc = ReadFile(m_DiskFileHandle,(void *)Data,length,&AmountRead,NULL);
	if( rc != 0 && AmountRead == length ){
		rtc = true;  //then the read is ok
#endif

#ifdef __MAC__
	AmountRead = lseek(m_DiskFileHandle,pos,SEEK_SET);
	AmountRead = read(m_DiskFileHandle,(void *)Data,length);
	if( AmountRead == length ){
		rtc = true;
#endif
	} else {
		OutPutDebugString("Error Reading PVR File");
	}
	return(rtc);

}

void DriveIOBase::SetHDDOffset(unsigned long Offset)
{
	m_HddOffset = Offset;
}
