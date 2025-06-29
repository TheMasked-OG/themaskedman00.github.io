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

// WriteCache.h: interface for the WriteCache class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __WRITECACHE__
#define __WRITECACHE__

//----------------------------- headers ------------------------
#include "Platform.h"
#include "GlobalDefs.h"
#include "WriteCache.h"	


//----------------------------- definitions --------------------
#define  CACHE_BUFFER_SIZE	(2000 * 2048)


//------------------------------- classes -----------------------

class WriteCache  
{
public:
	void		FlushCache();
#ifdef __MS__
	void		SetFileHandle(HANDLE FileHandle);
#endif
#ifdef __MAC__
	void		SetFileHandle(int FileHandle);
#endif
	void		WriteCachedFile( unsigned char *Buffer, unsigned long Length);
	WriteCache();
	virtual ~WriteCache();

private:
#ifdef __MS__
	HANDLE			m_Handle;
#endif
#ifdef __MAC__
	int				m_Handle;
#endif
	unsigned long	m_OutBufferPos;
	unsigned char	*m_OutBuffer;
};

#endif // __WRITECACHE__
