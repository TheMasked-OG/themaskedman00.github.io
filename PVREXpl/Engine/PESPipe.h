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

// PESPipe.h: interface for the PESPipe class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __PESPIPE__
#define __PESPIPE__

//----------------------------- headers ------------------------
#include "Platform.h"
#include "GlobalDefs.h"


//----------------------------- definitions --------------------
#define  MAX_QUEUE_SIZE		512


//------------------------------- classes -----------------------
class PESPipe  
{
public:
	void GetPipeStatus( int *Head, int *Size, int *Waiting);
	void Cancel();
	void *PeekPESFrame(int offset);
	bool IsFull();
	void PutPESFrame( void *Ptr );
	void * GetPESFrame();
	PESPipe();
	virtual ~PESPipe();

private:
	void	AquireMutex();
	void	FreeMutex();

	void	*m_Buffers[MAX_QUEUE_SIZE];
	int		m_Head;
	int		m_Tail;
	bool	m_Waiting;
	int		m_Size;
	bool	m_Cancel;

#ifdef __MS__
	HANDLE			m_Mutex;
	HANDLE			m_Semaphore;
#endif
#ifdef __MAC__
	sem_t			*m_Mutex;
	sem_t			*m_Semaphore;
#endif
};

#endif // __PESPIPE__
