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

// PackOut.h: interface for the PackOut class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __PACKOUT__
#define __PACKOUT__

//----------------------------- headers ------------------------
#include "Platform.h"
#include "GlobalDefs.h"
#include "WriteCache.h"	


//----------------------------- definitions --------------------
#define PACK_HEADER_SIZE	14


//------------------------------- classes -----------------------
class PackOut : public  WriteCache
{
public:
	void		SystemHeaderOut();
	void		OutPutPackHeader(int64 Clock);
	int64		GetMuxRate();
	PackOut(bool AC3);
	virtual ~PackOut();

private:
	bool	m_IsAC3;
	int64	m_PackCount;
	int64	m_TimeBase;
};

#endif // __PACKOUT__
