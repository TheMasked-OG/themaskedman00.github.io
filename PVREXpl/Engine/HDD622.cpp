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

// HDD622.cpp: implementation of the HDD50X class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HDD622.h"
#include "LogFile.h"

//////////////////////////////////////////////////////////////////////
//                  Globals 
//////////////////////////////////////////////////////////////////////
extern LogFile		dbgLog;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HDD622::HDD622()
{
	m_NumberOfPartitions = 0;
}

HDD622::~HDD622()
{

}

// this function will open the file on a 50X pVR HDD
HANDLE 
HDD622::OpenPvrFile(char *Path)
{

	if( Path[1] == '/'){  //then ext3 path
		m_Ext_Handle = m_Ext3.OpenPvrFile(Path);
		return(m_Ext_Handle);
	} else { //else dave path
		m_Ext_Handle = NULL;
		return(this->HDD50X::OpenPvrFile(Path));
	}
}

//  this function will return the file size of the opend file
int64 
HDD622::GetPvrFileSize( HANDLE FileHandle )
{
	if( FileHandle == m_Ext_Handle )
		return(m_Ext3.GetPvrFileSize(FileHandle));
	else
		return(this->HDD50X::GetPvrFileSize(FileHandle));

}

unsigned long 
HDD622::ReadPvrFile(HANDLE FileHandle, unsigned char *Data, unsigned long Length)
{
	if( FileHandle == m_Ext_Handle )
		return(m_Ext3.ReadPvrFile(FileHandle,Data,Length));
	else
		return(this->HDD50X::ReadPvrFile(FileHandle,Data,Length));
}

//----------------------------------
// this function will close and clean up from a file
void 
HDD622::ClosePvrFile(HANDLE FileHandle)
{
	
	if( FileHandle == m_Ext_Handle )
		m_Ext3.ClosePvrFile(FileHandle);
	else
		this->HDD50X::ClosePvrFile(FileHandle);
}

//--------------------------------------
bool 
HDD622::OpenAltPVRDrive(char *Path)
{
	return(m_Ext3.OpenPVRDrive(Path));
}
//////////////////////////////////////////////////////////////////////
// Public Methods
//////////////////////////////////////////////////////////////////////


//-----------------------------------
//   this function will determin if the drive is really a pvr drive of this type
bool 
HDD622::IsPvrDrive()
{
	m_NumberOfPartitions = GetNumberOfExt3Partitions();
	if( m_NumberOfPartitions == 4 ){
		this->SetHDDOffset(m_PartStart[m_NumberOfPartitions-1]);
		return(this->HDD50X::IsPvrDrive());
	}
	return(false);
}

//////////////////////////////////////////////////////////////////////
// Private Methods
//////////////////////////////////////////////////////////////////////

//-----------------------------------------
// this function will return the number of valid Ext3 partitions found
int 
HDD622::GetNumberOfExt3Partitions()
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
