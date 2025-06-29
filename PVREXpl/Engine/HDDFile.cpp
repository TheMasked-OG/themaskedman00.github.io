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

// HDDFile.cpp: implementation of the HDDFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HDDFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HDDFile::HDDFile()
{

}

HDDFile::~HDDFile()
{

}

//////////////////////////////////////////////////////////////////////
// Public Methods
//////////////////////////////////////////////////////////////////////
void  
HDDFile::AdjustAudioFileSize( HANDLE FileHandle )
{
	//this does nothing here...
}
	
int64 
HDDFile::GetPvrFileSize( HANDLE FileHandle )
{
	int64	rc;
#ifdef __MS__
	unsigned long x,y;

	x = GetFileSize(FileHandle,&y);
	rc = y;
	rc <<= 32;
	rc |= x;
#endif
#ifdef __MAC__
	//todo... NEED TO LOOK THIS UP...
#endif
	return(rc);
}

bool 
HDDFile::IsPvrDrive()
{
	//this should never be called so just return true;
	return(true);
}
	
void 
HDDFile::ClosePvrFile( HANDLE FileHandle )
{
#ifdef __MS__
	CloseHandle(FileHandle);
#endif
#ifdef __MAC__
	close((int)FileHandle);
#endif
}

unsigned long 
HDDFile::ReadPvrFile( HANDLE fileHandle, unsigned char *Data, unsigned long Length)
{
	unsigned long rc;

#ifdef __MS__
	ReadFile(fileHandle,(void *)Data,Length,&rc,NULL);
#endif
#ifdef __MAC__
	rc = read((int)fileHandle,(void *)Data,Length);
#endif
	return(rc);
}


HANDLE 
HDDFile::OpenPvrFile( char *FileName )
{
	HANDLE	rc;

#ifdef __MS__
	rc = CreateFile(FileName,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
#endif
#ifdef __MAC__
	rc = (HANDLE)open(FileName,O_RDONLY,0x777);
#endif
	return(rc);
}
//  this function will return the creation time of the file
unsigned long 
HDDFile::GetPvrFileTime( char *FileName )
{
	return(0);
}
//////////////////////////////////////////////////////////////////////
// Private Methods
//////////////////////////////////////////////////////////////////////
