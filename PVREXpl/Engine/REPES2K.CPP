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

// RePES2K.cpp: implementation of the RePES2K class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RePES2K.h"


unsigned char	VHeaderBase[22] = {0x00,0x00,0x01,0xE0,0x07,0xEC,0x80,0x01,0x03,0x1E,0,0,0,0,0,0,0,0,0,0,0,0};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RePES2K::RePES2K()
{
	int		j;

	m_CurrentBucketNumber = 0;
	m_BucketSequence = 0;
	m_NewPTS = 0x551B;
	m_PartialFilled = 0;
	m_PartialPTS = 0;
	m_LastFrameType = 0;
	m_FrameCount = 8;
	m_PTSOffset = 0;
	m_Canceled = false;
	for(j = 0; j < MAX_BUCKETS; j++){
		m_BucketState[j] = BUCKET_FREE;
	}
#ifdef __MS__
	m_Mutex = CreateMutex(NULL,false,NULL);
	m_Semaphore = CreateSemaphore(NULL,0,MAX_BUCKETS,NULL);
#endif
#ifdef __MAC__
	m_Mutex = sem_open("RePesVMutex",O_CREAT,0,0);
	if( m_Mutex != (sem_t *)SEM_FAILED )
		sem_post(m_Mutex);  //set the mutex for aquire
	m_Semaphore = sem_open("RePesV",O_CREAT,0,0);
#endif
}

RePES2K::~RePES2K()
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
// Public methods
//////////////////////////////////////////////////////////////////////

//-------------------------------------------
// this function will take in a frame of video data and repack it to the right size
// it will block the caller if there is no free buckets available and wait until there is
void 
RePES2K::RePESData(unsigned char *Data, unsigned long Length, int64 PTS, char Type)
{
	unsigned long	BucketLeft;
	int				j,x;
	unsigned char	shft;
	int				InputPos;
	int				BucketNum;
	unsigned char	Buf[32];
	int64			DTS;

	
	if( Type != 'B' && Type != 'P' && Type != 'I')
		return;
	InputPos = 0;
	while( Length > 0 ){  //while we have work to do...
		//TODO *** we should exit of canceled

		if( Length < (unsigned long)((BUCKET_SIZE - 12) - m_PartialFilled) ){ //now see if we are done with a partial
		//****if( Length < (ULONG)((BUCKET_SIZE - 9) - m_PartialFilled) ){ //now see if we are done with a partial
			memcpy((void *)&m_PartialBucket[m_PartialFilled],(void *)&Data[InputPos],Length);
			m_PartialFilled += Length;
			if( PTS > 0 && m_PartialPTS == 0){
				m_PartialPTS = PTS;
				m_PartialType = Type;
			}
			Length = 0;
			continue;
		} 
		BucketNum = GetNextFreeBucket();  //get the next free or partial bucket number
		if( BucketNum == BUCKET_FREE ){ // then there are none free
#ifdef __MS__
			WaitForSingleObject(m_Semaphore,1000);	//wait for them to be precessed	
#endif
#ifdef __MAC__
			sem_wait(m_Semaphore);
#endif
			if( m_Canceled )
				return;
			continue;
		} else {
			BucketLeft = BUCKET_SIZE;
			memcpy(Buf,VHeaderBase,22);
			j = 9;
			if( PTS != 0 || m_PartialPTS != 0){
				if( m_PartialPTS > 0 ){
					PTS = m_PartialPTS;
					m_PartialPTS = 0;
					Type = m_PartialType;
				} 
				Buf[7] = 0x81;
				//****Buf[7] = 0x80;
				Buf[8] = 0x08;
				//****Buf[8] = 0x05;
				shft = (unsigned char)((PTS >> 30) & 0x07);
				shft <<= 1;
				Buf[j++] = 0x21 | shft;
				Buf[j++] = (unsigned char)((PTS >> 22) & 0xFF);
				shft = (unsigned char)((PTS >> 15) & 0x7F);
				shft <<= 1;
				Buf[j++] = 0x01 | shft;
				Buf[j++] = (unsigned char)((PTS >> 7) & 0xFF);
				shft = (unsigned char)((PTS) & 0x7F);
				shft <<= 1;
				Buf[j++] = 0x01 | shft;
				if( Type == 'I' || Type == 'P'){
					if( PTS > 0 )
						DTS = PTS - 0x1776;
					else 
						DTS = PTS + 0x1776;
					Buf[7] |= 0x40;
					Buf[8] = 0x0D;
					//*****Buf[8] = 0x0A;
					Buf[9] |= 0x10;
					shft = (unsigned char)((DTS >> 30) & 0x07);
					shft <<= 1;
					Buf[j++] = 0x11 | shft;
					Buf[j++] = (unsigned char)((DTS >> 22) & 0xFF);
					shft = (unsigned char)((DTS >> 15) & 0x7F);
					shft <<= 1;
					Buf[j++] = 0x01 | shft;
					Buf[j++] = (unsigned char)((DTS >> 7) & 0xFF);
					shft = (unsigned char)((DTS) & 0x7F);
					shft <<= 1;
					Buf[j++] = 0x01 | shft;
				}
				PTS = 0;
			} else { //no PTS/DTS
				Buf[8] = 0x03;
			}
			Buf[j++] = 0x1E;
			Buf[j++] = 0x61;
			Buf[j++] = 0x02;  //j now has the size of the header
			BucketLeft -= j;
			memcpy((void *)&m_Bucket[BucketNum][0],(void *)Buf,j);
			
			if( m_PartialFilled > 0 ){  //fill in the partial
				x = m_PartialFilled;
				if( m_PartialFilled > (int)BucketLeft ){
					x = BucketLeft;
				}
				memcpy((void *)&m_Bucket[BucketNum][j],(void *)m_PartialBucket,x);
				m_PartialFilled -= x;
				BucketLeft -= x;
				if( m_PartialFilled > 0 ){
					memcpy((void *)&m_PartialBucket[0],(void *)&m_PartialBucket[x],m_PartialFilled);
				}
			}
			if( BucketLeft > 0 ){
				memcpy((void *)&m_Bucket[BucketNum][BUCKET_SIZE - BucketLeft],(void *)&Data[InputPos],BucketLeft);
				InputPos += BucketLeft;
				Length -= BucketLeft;
			}
			SendBucket(BucketNum);
		}
	}
}

//-------------------------------------------------------
// this function will return a repacked pes bucket, it will block until on is available
unsigned char * 
RePES2K::GetNextPES()
{
	//this will get the next one ready then allocate a buffer
	//copy the data and return the pointer
	//the caller is responsible to delete the buffer
	//return NULL if none ready or memory error
	int		j;
	unsigned char	*rc;

	AquireMutex();
	for(j = 0; j < MAX_BUCKETS; j++){
		if( m_BucketState[j] == m_CurrentBucketNumber ){ //we found a bucket!
			rc = (unsigned char *)new unsigned char[BUCKET_SIZE];	
			if( rc != NULL ){
				memcpy((void *)rc,(void *)&m_Bucket[j][0],BUCKET_SIZE);
				m_BucketState[j] = BUCKET_FREE;
				m_CurrentBucketNumber++;
				if( m_CurrentBucketNumber >= MAX_BUCKETS )
					m_CurrentBucketNumber = 0;
				FreeMutex();
				return(rc);
			}
		}
	}
	FreeMutex();
#ifdef __MS__
	ReleaseSemaphore(m_Semaphore,1,NULL);
#endif
#ifdef __MAC__
	sem_post(m_Semaphore);
#endif
	return(NULL);
}

//-----------------------------------------------------
// this function will flush any partially filled buckets
void 
RePES2K::Flush()
{
	unsigned long	BucketLeft;
	int				j,x;
	unsigned char	shft;
	int				BucketNum;
	unsigned char	Buf[32];
	int64			PTS;

	if( m_PartialFilled > 0 ){ //then we need to flush the data out
		BucketNum = GetNextFreeBucket();  //get the next free or partial bucket number
		if( BucketNum == BUCKET_FREE ){ // then there are none free
#ifdef __MS__
			WaitForSingleObject(m_Semaphore,1000);	//wait for them to be precessed	
#endif
#ifdef __MAC__
			sem_wait(m_Semaphore);
#endif
			if( m_Canceled )
				return;
			while( BucketNum == BUCKET_FREE ){
				BucketNum = GetNextFreeBucket();  //see if any were processed
#ifdef __MS__
				WaitForSingleObject(m_Semaphore,1000);	//wait for them to be precessed	
#endif
#ifdef __MAC__
				sem_wait(m_Semaphore);
#endif
				if( m_Canceled )
					return;
				}
			Flush();  //possible hang if something is wrong!
			return;
		} else {
			BucketLeft = BUCKET_SIZE;
			memcpy(Buf,VHeaderBase,22);
			j = 9;
			PTS = m_PartialPTS;
			if( PTS != 0 ){
				Buf[7] = 0x81;
				Buf[8] = 0x08;
				shft = (unsigned char)((PTS >> 30) & 0x07);
				shft <<= 1;
				Buf[j++] = 0x21 | shft;
				Buf[j++] = (unsigned char)((PTS >> 22) & 0xFF);
				shft = (unsigned char)((PTS >> 15) & 0x7F);
				shft <<= 1;
				Buf[j++] = 0x01 | shft;
				Buf[j++] = (unsigned char)((PTS >> 7) & 0xFF);
				shft = (unsigned char)((PTS) & 0x7F);
				shft <<= 1;
				Buf[j++] = 0x01 | shft;
				PTS = 0;
			}
		}
		Buf[j++] = 0x1E;
		Buf[j++] = 0x61;
		Buf[j++] = 0x02;  //j now has the size of the header
		BucketLeft -= j;
		memcpy((void *)&m_Bucket[BucketNum][0],(void *)Buf,j);
		x = m_PartialFilled;
		memcpy((void *)&m_Bucket[BucketNum][j],(void *)m_PartialBucket,x);
		m_PartialFilled -= x;
		BucketLeft -= x;
		SendBucket(BucketNum);
	}
}

//---------------------------------------------------
// this will indicate if all the buckest are flushed
bool 
RePES2K::IsFlushed()
{
	int		j;

	for(j = 0; j < MAX_BUCKETS; j++){
		if( m_BucketState[j] != BUCKET_FREE ){ //we found a bucket!
			return(false);
		}
	}
	return(true);
}


void RePES2K::Cancel()
{
	m_Canceled = true;
#ifdef __MS__
	ReleaseSemaphore(m_Semaphore,1,NULL);
#endif
#ifdef __MAC__
	sem_post(m_Semaphore);
#endif
}
//////////////////////////////////////////////////////////////////////
// Private methods
//////////////////////////////////////////////////////////////////////

int 
RePES2K::GetNextFreeBucket()
{
	int		j,rc;

	rc = BUCKET_FREE;
	AquireMutex();
	for(j = 0; j < MAX_BUCKETS; j++){
		if( m_BucketState[j] == BUCKET_FREE ){ //we found a bucket!
			rc = j;
			break;
		}
	}
	FreeMutex();
	return(rc);
}


void 
RePES2K::SendBucket(int number)
{
	
	AquireMutex();
	m_BucketState[number] = m_BucketSequence++;
	if( m_BucketSequence >= MAX_BUCKETS )
		m_BucketSequence = 0;
	FreeMutex();
}

void 
RePES2K::AquireMutex()
{
#ifdef __MS__
	WaitForSingleObject(m_Mutex,2000);	
#endif
#ifdef __MAC__
	sem_wait(m_Mutex);
#endif
}

void RePES2K::FreeMutex()
{
#ifdef __MS__
	ReleaseMutex(m_Mutex);
#endif
#ifdef __MAC__
	sem_post(m_Mutex);
#endif
}


int RePES2K::GetBucketStatus(int *TotalBuckets)
{
	int		j,rc;
	
	*TotalBuckets = MAX_BUCKETS;
	rc = 0;
	for( j = 0; j < MAX_BUCKETS; j++){
		if( m_BucketState[j] != BUCKET_FREE )
			rc++;
	}
	return(rc);
}