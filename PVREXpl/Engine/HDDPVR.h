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

// HDDPVR.h: interface for the HDDPVR class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __HDDPVR__
#define __HDDPVR__

//----------------------------- headers ------------------------
#include "Platform.h"
#include "GlobalDefs.h"
#include "DriveIOBase.h"

//----------------------------- definitions --------------------


//------------------------------- classes -----------------------
class HDDPVR : public DriveIOBase  
{
public:
	virtual unsigned long GetPvrFileTime( char *FileName ) = 0;
	virtual void  AdjustAudioFileSize( HANDLE FileHandle ) = 0;
	virtual int64 GetPvrFileSize( HANDLE FileHandle ) = 0;
	virtual bool IsPvrDrive() = 0;
	virtual void ClosePvrFile( HANDLE FileHandle ) = 0;
	virtual unsigned long ReadPvrFile( HANDLE fileHandle, unsigned char *Data, unsigned long Length) = 0;
	virtual HANDLE OpenPvrFile( char *FileName ) = 0;
	//HDDPVR();
	//virtual ~HDDPVR();

};

#endif // __HDDPVR__
