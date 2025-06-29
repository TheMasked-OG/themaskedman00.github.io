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

// DriveIOBase.h: interface for the DriveIOBase class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DRIVEIOBASE__
#define __DRIVEIOBASE__

//----------------------------- headers ------------------------
#include "Platform.h"
#include "GlobalDefs.h"
#include "LogFile.h"

//----------------------------- definitions --------------------

#define  HDD_BLOCK_SIZE	 512

//------------------------------- classes -----------------------
class DriveIOBase  
{
public:
	void SetHDDOffset( unsigned long Offset);
	bool ReadBlock( unsigned long Lba, unsigned char *Data, unsigned long NumBlocks);
	void ClosePvrDrive();
	bool OpenPVRDrive( char *Path );
	DriveIOBase();
	virtual ~DriveIOBase();

private:
	unsigned long m_HddOffset;
#ifdef __MS__
	HANDLE	m_DiskFileHandle;
#else
	int		m_DiskFileHandle;  //MAC and Linux us int
#endif

};

#endif // __DRIVEIOBASE__
