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

// DishPvrHdd.h: interface for the DishPvrHdd class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DISHPVRHDD__
#define __DISHPVRHDD__

//----------------------------- headers ------------------------
#include "Platform.h"
#include "GlobalDefs.h"
#include "HDDPVR.h"

//----------------------------- definitions --------------------


//------------------------------- classes -----------------------
class DishPvrHdd  
{
public:
	unsigned long	GetPvrFileTime( char *FileName );
	void			AdjustAudioFileSize( HANDLE FileHandle );
	int64			GetPvrFileSize( HANDLE FileHandle );
	void			ClosePvrFile( HANDLE FileHandle );
	unsigned long	ReadPvrFile( HANDLE fileHandle, unsigned char *Data, unsigned long Length);
	HANDLE			OpenPvrFile( char *FileName );
	int				SetPvrHdd( char *Path, int Type);
	DishPvrHdd();
	virtual ~DishPvrHdd();

	int m_PvrType;
	char m_FileRootPath[256];
private:
	HDDPVR			*m_PvrHdd;
};

#endif // __DISHPVRHDD__
