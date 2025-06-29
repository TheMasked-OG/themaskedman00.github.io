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

// HDD622.h: interface for the HDD50X class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __HDD622__
#define __HDD622__

//----------------------------- headers ------------------------
#include "Platform.h"
#include "GlobalDefs.h"
#include "HDDPVR.h"
#include "HDD50X.h"
#include "HDD522.h"
#include "LogFile.h"
#include "partition.h"

//----------------------------- definitions --------------------


//------------------------------- classes -----------------------
class HDD622 : public HDD50X  
{
public:
	int64 GetPvrFileSize( HANDLE FileHandle );
	unsigned long ReadPvrFile(HANDLE FileHandle, unsigned char *Data, unsigned long Length);
	void ClosePvrFile(HANDLE FileHandle);
	bool OpenAltPVRDrive(char *Path);
	bool IsPvrDrive();
	HANDLE OpenPvrFile(char *Path);
	HDD622();
	virtual ~HDD622();

private:
	HANDLE m_Ext_Handle;
	//Vars	
	int				m_NumberOfPartitions;
	int				GetNumberOfExt3Partitions();
	unsigned long	m_PartStart[9];
	HDD522			m_Ext3;
};

#endif // __HDD622__
