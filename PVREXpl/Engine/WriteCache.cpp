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

// WriteCache.cpp: implementation of the WriteCache class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WriteCache.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WriteCache::WriteCache()
{
#ifdef __MS__
	m_Handle = INVALID_HANDLE_VALUE;
#endif
#ifdef __MAC__
	m_Handle = -1;
#endif
	m_OutBuffer = NULL;
	m_OutBufferPos = 0;
}

WriteCache::~WriteCache()
{
	if( m_OutBuffer != NULL )
		delete []m_OutBuffer;
}

void 
WriteCache::WriteCachedFile(unsigned char *Buffer, unsigned long Length)
{
	if( Length > CACHE_BUFFER_SIZE)
		return; //this is bad
	if( (m_OutBufferPos + Length) > CACHE_BUFFER_SIZE)
		FlushCache();
	memcpy((void *)&m_OutBuffer[m_OutBufferPos],(void *)Buffer,Length);
	m_OutBufferPos += Length;
}

void 
#ifdef __MS__
WriteCache::SetFileHandle(HANDLE FileHandle)
{
#endif
#ifdef __MAC__
WriteCache::SetFileHandle(int FileHandle)
{
#endif
	if(m_OutBuffer != NULL )
		delete []m_OutBuffer;
		
	m_OutBuffer = (unsigned char *)new unsigned char[CACHE_BUFFER_SIZE];
	m_OutBufferPos = 0;
	m_Handle = FileHandle;
}

void 
WriteCache::FlushCache()
{
	unsigned long	x;

	if( m_OutBufferPos > 0 ){
#ifdef __MS__
	if( m_Handle != INVALID_HANDLE_VALUE)
		WriteFile(m_Handle,m_OutBuffer,m_OutBufferPos,&x,NULL);
#endif
#ifdef __MAC__
	if( m_Handle > 0)
		x = write(m_Handle,(void *)m_OutBuffer,m_OutBufferPos);
#endif
	}
	m_OutBufferPos = 0;
}
