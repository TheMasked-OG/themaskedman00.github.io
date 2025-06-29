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

// RePES2K.h: interface for the RePES2K class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __REPES2K__
#define __REPES2K__

//----------------------------- headers ------------------------
#include "Platform.h"
#include "GlobalDefs.h"


//----------------------------- definitions --------------------

#define		BUCKET_FREE		0xFFFFFFFF
#define		BUCKET_PARTIAL	0x8F

#define		MAX_BUCKETS		1000
#define		BUCKET_SIZE		2034


//------------------------------- classes -----------------------
class RePES2K  
{
public:
	int GetBucketStatus( int *TotalBuckets);
	void Cancel();
	bool			IsFlushed();
	void			Flush();
	unsigned char	*GetNextPES( );
	void			RePESData( unsigned char *Data, unsigned long Length, int64 PTS, char Type);
	RePES2K();
	virtual ~RePES2K();

private:
	bool			m_Canceled;
	char			m_PartialType;
	int64			m_PTSOffset;
	double			m_FrameCount;
	char			m_LastFrameType;
	int64			m_PartialPTS;
	void			FreeMutex();
	void			AquireMutex();
	unsigned char	m_PartialBucket[BUCKET_SIZE];
	void			SendBucket(int number);
	int64			m_NewPTS;
	int				m_BucketState[MAX_BUCKETS];
	int				m_BucketSequence;
	int				GetNextFreeBucket();
	int				m_PartialFilled;
	unsigned char	m_Bucket[MAX_BUCKETS][BUCKET_SIZE];
	int				m_CurrentBucketNumber;
#ifdef __MS__
	HANDLE			m_Mutex;
	HANDLE			m_Semaphore;
#endif
#ifdef __MAC__
	sem_t			*m_Mutex;
	sem_t			*m_Semaphore;
#endif
};

#endif // __REPES2K__
