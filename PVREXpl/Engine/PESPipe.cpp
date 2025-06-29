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

// PESPipe.cpp: implementation of the PESPipe class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PESPipe.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PESPipe::PESPipe()
{
	int		j;

	for( j = 0; j < MAX_QUEUE_SIZE; j++)
		m_Buffers[j] = NULL;
	m_Head = 0;
	m_Tail = 0;
	m_Waiting = false;
	m_Size = 0;
	m_Cancel = false;

#ifdef __MS__
	m_Mutex = CreateMutex(NULL,false,NULL);
	m_Semaphore = CreateSemaphore(NULL,0,255,NULL);
#endif
#ifdef __MAC__
	m_Mutex = sem_open("RePesVMutex",O_CREAT,0,0);
	if( m_Mutex != (sem_t *)SEM_FAILED )
		sem_post(m_Mutex);  //set the mutex for aquire
	m_Semaphore = sem_open("RePesV",O_CREAT,0,0);
#endif

}

PESPipe::~PESPipe()
{

#ifdef __MAC__
	if( m_Mutex != (sem_t *)SEM_FAILED ){
		sem_close(m_Mutex);
		sem_unlink("RePesVMutex");
	}
	if( m_Semaphore != (sem_t *)SEM_FAILED ){
		sem_close(m_Semaphore);
		sem_unlink("RePesV");
	}
#endif
#ifdef __MS__
	if( m_Mutex != NULL )
		CloseHandle(m_Mutex);
	if( m_Semaphore != NULL )
		CloseHandle(m_Semaphore);
#endif

}

//////////////////////////////////////////////////////////////////////
// public methods
//////////////////////////////////////////////////////////////////////
void * 
PESPipe::PeekPESFrame(int offset)
{
	void	*rc;
	int		Index;

	AquireMutex();
	if( m_Cancel ){
		FreeMutex();
		return(NULL);
	}
	while( m_Size < (offset + 1) ){
		m_Waiting = true;
		FreeMutex();
#ifdef __MS__
		WaitForSingleObject(m_Semaphore,INFINITE);	//wait for them to be precessed	
#endif
#ifdef __MAC__
		sem_wait(m_Semaphore);
#endif
		AquireMutex();		
		if( m_Cancel ){
			FreeMutex();
			return(NULL);
		}
	}
	Index = m_Head + offset;
	if( Index >= MAX_QUEUE_SIZE)
		Index = Index - MAX_QUEUE_SIZE;
	rc = m_Buffers[Index];
	FreeMutex();
	return(rc);
}

void * 
PESPipe::GetPESFrame()
{
	void	*rc;

	AquireMutex();
	if( m_Cancel ){
		FreeMutex();
		return(NULL);
	}
	while( m_Size == 0 ){
		m_Waiting = true;
		FreeMutex();
#ifdef __MS__
		WaitForSingleObject(m_Semaphore,INFINITE);	//wait for them to be precessed	
#endif
#ifdef __MAC__
		sem_wait(m_Semaphore);
#endif
		AquireMutex();		
		if( m_Cancel ){
			FreeMutex();
			return(NULL);
		}
	}
	rc = m_Buffers[m_Head];
	m_Buffers[m_Head++] = NULL;
	m_Size--;
	if( m_Head >= MAX_QUEUE_SIZE )
		m_Head = 0;
	if( rc == NULL ){ //then this pipe has reached EOF
		m_Cancel = true;
	}
	FreeMutex();
	return(rc);
}

void 
PESPipe::PutPESFrame(void *Ptr)
{
	
	AquireMutex();
	if( m_Cancel ){
		FreeMutex();
		return;
	}
	
	if( m_Size >= MAX_QUEUE_SIZE - 1){
		//this should never be! if it is then sleep then recurse
		FreeMutex();
#ifdef __MS__
		Sleep(1000);	
#endif
#ifdef __MAC__
		sleep(1);
#endif
		PutPESFrame(Ptr);
		return;
	}
	m_Buffers[m_Tail++] = Ptr;
	m_Size++;
	if( m_Tail >= MAX_QUEUE_SIZE )
		m_Tail = 0;
	if( m_Waiting ){
#ifdef __MS__
		ReleaseSemaphore(m_Semaphore,1,NULL);	
#endif
#ifdef __MAC__
		sem_post(m_Semaphore);
#endif
		m_Waiting = false;
	}
	FreeMutex();
}

bool 
PESPipe::IsFull()
{
	bool	rc;

	rc = false;
	AquireMutex();
	if( m_Size >= MAX_QUEUE_SIZE - 1)
		rc = true;
	FreeMutex();
	return(rc);
}

void PESPipe::Cancel()
{
	AquireMutex();
	m_Cancel = true;
	if( m_Waiting ){
#ifdef __MS__
		ReleaseSemaphore(m_Semaphore,1,NULL);	
#endif
#ifdef __MAC__
		sem_post(m_Semaphore);
#endif
		m_Waiting = false;
	}
	FreeMutex();
}

//--------------------------------------------------------
// this is for debuging get the status of these pipes
void 
PESPipe::GetPipeStatus(int *Head, int *Size, int *Waiting)
{
	*Head = m_Head;
	*Size = m_Size;
	*Waiting = (m_Waiting)? 1:0;
}

//////////////////////////////////////////////////////////////////////
// private methods
//////////////////////////////////////////////////////////////////////

void 
PESPipe::AquireMutex()
{
#ifdef __MS__
	DWORD	rc;

	rc = WaitForSingleObject(m_Mutex,INFINITE);	
	if( rc != WAIT_OBJECT_0 ){
		rc = GetLastError();
	}
#endif
#ifdef __MAC__
	sem_wait(m_Mutex);
#endif
}

void 
PESPipe::FreeMutex()
{
#ifdef __MS__
	BOOL	rc;
	DWORD	x;

	rc = ReleaseMutex(m_Mutex);
	if( !rc ){
		x = GetLastError();
	}

#endif
#ifdef __MAC__
	sem_post(m_Mutex);
#endif
}


