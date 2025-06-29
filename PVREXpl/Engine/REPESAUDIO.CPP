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

// RePESAudio.cpp: implementation of the RePESAudio class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RePESAudio.h"

unsigned char	AHeaderBase[22] = {0x00,0x00,0x01,0xC0,0x07,0xEC,0x80,0x01,0x03,0x1E,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned char	AC3HeaderBase[22] = {0x00,0x00,0x01,0xBD,0x07,0xEC,0x80,0x81,0x08,0x27,0x69,0xFF,0xD0,0xCD,0,0,0,0,0,0,0,0};

/////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RePESAudio::RePESAudio()
{
	int		j;

	m_CurrentBucketNumber = 0;
	m_BucketSequence = 0;
	m_NewPTS = 0x551B;
	m_PartialFilled = 0;
	m_FrameCount = 0;
	m_PtsOffset = 0;
	m_CompData1 = 0;
	m_CompData2 = 0;
	m_IsAC3 = false;
	m_AC3count = 0;
	m_Canceled = false;
	for(j = 0; j < MAX_BUCKETS; j++){
		m_BucketState[j] = BUCKET_FREE;
	}
	m_AudioPTSFrameRate = 0x870;  //bugbug this is assuming 48K samples per second
								  //44.1 = 0x92F, 32 = 0xCA8 for Mpeg, 
								   //44.1 = 0xC3E, 32 = 0x10E0 for AC3

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

RePESAudio::~RePESAudio()
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

/////////////////////////////////////////////////////////////////////
// public methods
//////////////////////////////////////////////////////////////////////

//-------------------------------------------------
//  this function will return the number of frames in the sample
int 
RePESAudio::GetNumberOfFrames(unsigned char *Data, unsigned long Length)
{
	int		j,rc;

	rc = 0;			//scan the data for Frame header in the Mpeg Audio data or AC3
	if( !m_IsAC3 )
	{
		for(j = 0; j < (int)Length - 2; j++)
		{
			if( Data[j] == 0xFF && (Data[j+1] & 0xF0) == 0xF0 )
			{
				if( m_CompData1 == 0 )
				{
					m_CompData1 = (Data[j+1] & 0x0F);
					m_CompData2 = (Data[j+2]);
					//now verify the if correct
					if( (m_CompData1 & 0x08) == 0x00 )
					{ //check ID bit then bad
						m_CompData1 = 0;
						continue;
					}
					if( (m_CompData1 & 0x06) == 0x00 )
					{ //Layer bits then bad
						m_CompData1 = 0;
						continue;
					}
					if( (m_CompData2 & 0xF0) == 0xF0 || (m_CompData2 & 0xF0) < 0x40)
					{ //Bit rate index then bad
						m_CompData1 = 0;
						continue;
					}
					if( (m_CompData2 & 0x0C) == 0x0C )
					{ //Sampeling frequency then bad
						m_CompData1 = 0;
						continue;
					}
				}
				if( (Data[j+1] & 0x0F) == m_CompData1 && Data[j+2] == m_CompData2)
				{ //ID = 1, Layer = 2, protection = 1
					rc++;
				}
			}
		}
	} else { //AC3 
		for(j = 0; j < (int)Length - 5; j++)
		{
			if( Data[j] == 0x0B && Data[j+1] == 0x77 )
			{
				if( m_CompData1 == 0 )
				{
					m_CompData1 = Data[j+4];
					//now verify the if correct
					if( (m_CompData1 & 0xC0) == 0xC0 )
					{ //check the rate code for reserved
						m_CompData1 = 0;
						continue;
					}
					if( ((m_CompData1 & 0x3F) / 2) >= 19 )
					{ //Check the Bit Rate
						m_CompData1 = 0;
						continue;
					}
				}
				if( Data[j+4] == m_CompData1 )
				{ //check the rates
					rc++;
				}
			}
		}
	}
	return(rc);
}

//--------------------------------------
// this function repacks the data into new chunks
void 
RePESAudio::RePESData(unsigned char *Data, unsigned long Length, int64 PTS)
{
	int				BucketNum;
	unsigned char	Buf[32];
	int				BucketLeft;
	int				j,x;
	unsigned char	shft;
	int				InputPos;
	int				FrameNumber;
	int				PESHeaderSize;

	
	PESHeaderSize = (m_IsAC3)? 21:17;
	//****PESHeaderSize = 9;
	//check for a droped frame first
	InputPos = 0;
	while( Length > 0 )
	{  //while we have work to do...
		//*** TODO we should exit if canceled
		if( (m_PartialFilled + Length) < (unsigned long)(BUCKET_SIZE - PESHeaderSize) )
		{  //then this is a partial fill
			memcpy((void *)&m_PartialBucket[m_PartialFilled],(void *)Data,Length);
			m_PartialFilled += Length;
			return;
		}
		BucketNum = GetNextFreeBucket();  //get the next free or partial bucket number
		if( BucketNum == BUCKET_FREE )
		{ // then there are none free
#ifdef __MS__
			WaitForSingleObject(m_Semaphore,1000);	//wait for them to be precessed	
#endif
#ifdef __MAC__
			sem_wait(m_Semaphore);
#endif
			if( m_Canceled )
				return;
			continue;
		} 
		else 
		{
			BucketLeft = BUCKET_SIZE;

			if( !m_IsAC3 )
				memcpy(Buf,AHeaderBase,PESHeaderSize);
			else 
				memcpy(Buf,AC3HeaderBase,PESHeaderSize);

			j = 0;
			if( m_PartialFilled > 0 )
			{
				j = GetNumberOfFrames(m_PartialBucket,m_PartialFilled);
			}

			PTS -= ((int64)j * m_AudioPTSFrameRate);
			j = 9;
			Buf[7] = 0x81;
			Buf[8] = 0x08;
			//***Buf[7] = 0x80;
			//***Buf[8] = 0x05;
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
			if( !m_IsAC3 )
			{
				Buf[j++] = 0x1E;
				Buf[j++] = 0x40;
				Buf[j++] = 0x20;  //j now has the size of the header
			} 
			else 
			{
				Buf[j++] = 0x1E;
				Buf[j++] = 0x60;
				Buf[j++] = 0x04;  
				Buf[j++] = 0x80;
				m_AC3count = j;
				Buf[j++] = 0x02;
				Buf[j++] = 0x00;
				Buf[j++] = 0x01;
			}
			BucketLeft -= j;  //j now has the size of the header
			memcpy((void *)&m_Bucket[BucketNum][0],(void *)Buf,j);
			if( m_PartialFilled > 0 )
			{  //fill in the partial
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
				
			if( m_IsAC3 )
			{ //now we need to find the Next Sync Frame
				j = FindAC3FrameOffset(	&m_Bucket[BucketNum][PESHeaderSize],(BUCKET_SIZE - PESHeaderSize));
				if( j > 0xFF )
					m_Bucket[BucketNum][m_AC3count] = 0x01;
				m_Bucket[BucketNum][m_AC3count + 2] = (unsigned char)(j & 0xFF);
				m_Bucket[BucketNum][m_AC3count + 1] = (unsigned char)((j >> 8) & 0xFF);

			}
			FrameNumber = GetNumberOfFrames(&m_Bucket[BucketNum][PESHeaderSize],(BUCKET_SIZE - PESHeaderSize));
			SendBucket(BucketNum,FrameNumber);
			m_FrameCount += FrameNumber;
			PTS += FrameNumber * m_AudioPTSFrameRate;
			if( !m_IsAC3 )
			{
				//now check to see if a boundry crossing
				if( m_Bucket[BucketNum][BUCKET_SIZE - 1] == 0xFF && Length > 1)
				{ //possible crossing
					if( (Data[InputPos] & 0x0F) == m_CompData1 && Data[InputPos+1] == m_CompData2)
						PTS += m_AudioPTSFrameRate;
				}
				if( m_Bucket[BucketNum][BUCKET_SIZE - 2] == 0xFF && (m_Bucket[BucketNum][BUCKET_SIZE - 1] & 0x0F) == m_CompData1 && Length > 0)
				{ //possible crossing
					if( Data[InputPos] == m_CompData2)
						PTS += m_AudioPTSFrameRate;
				}
			} 
			else 
			{
				//now check to see if a boundry crossing
				if( m_Bucket[BucketNum][BUCKET_SIZE - 1] == 0x0B && Length > 0)
				{ //possible crossing
					if( (Data[InputPos] ) == 0x77 )
						PTS += m_AudioPTSFrameRate;
				}
			}
			if( Length < (unsigned long)(BUCKET_SIZE - PESHeaderSize))
			{ //now see if we are done with a partial
				memcpy((void *)m_PartialBucket,(void *)&Data[InputPos],Length);
				m_PartialFilled = Length;
				Length = 0;
				m_NewPTS = PTS;
			} 
		}
	}
}

//-----------------------------------------------------
// this function will get the next repacked bucket
unsigned char *
RePESAudio::GetNextPES( )
{
	//this will get the next one ready then allocate a buffer
	//copy the data and return the pointer
	//the caller is responsible to delete the buffer
	//return NULL if none ready or memory error
	int				j;
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
RePESAudio::Flush()
{
	int64			PTS;
	int				BucketNum;
	unsigned char	Buf[32];
	int				BucketLeft;
	int				j;
	unsigned char	shft;
	int				FrameNumber;

	
	if( m_PartialFilled > 0 ){
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
			Flush();	//possible hang if something is wrong!
		} else {
			BucketLeft = BUCKET_SIZE;
			memcpy(Buf,AHeaderBase,17);
			PTS = m_NewPTS;
			j = 9;
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
			Buf[j++] = 0x1E;
			Buf[j++] = 0x40;
			Buf[j++] = 0x20;  //j now has the size of the header
			BucketLeft -= j;
			memcpy((void *)&m_Bucket[BucketNum][0],(void *)Buf,j);
			memcpy((void *)&m_Bucket[BucketNum][j],(void *)m_PartialBucket,m_PartialFilled);
			BucketLeft -= m_PartialFilled;
			FrameNumber = GetNumberOfFrames(&m_Bucket[BucketNum][17],m_PartialFilled );
			m_PartialFilled = 0;
			SendBucket(BucketNum,FrameNumber);
		}
	}
}

//---------------------------------------------------
// this will indicate if all the buckest are flushed
bool 
RePESAudio::IsFlushed()
{
	int		j;

	for(j = 0; j < MAX_BUCKETS; j++){
		if( m_BucketState[j] != BUCKET_FREE ){ //we found a bucket!
			return(false);
		}
	}
	return(true);
}

//---------------------------------------------
// this function will set the AC3 flag
void 
RePESAudio::SetAC3Mode(bool Mode)
{
	m_IsAC3 = Mode;
	if( Mode )
		m_AudioPTSFrameRate = 0xB40;		//this is based on 48K Samples per sec
	else
		m_AudioPTSFrameRate = 0x870;		//this is based on 48K Samples per sec
}

void
 RePESAudio::Cancel()
{
	m_Canceled = true;
#ifdef __MS__
	ReleaseSemaphore(m_Semaphore,1,NULL);
#endif
#ifdef __MAC__
	sem_post(m_Semaphore);
#endif
}
/////////////////////////////////////////////////////////////////////
// private methods
//////////////////////////////////////////////////////////////////////


void 
RePESAudio::AquireMutex()
{
#ifdef __MS__
	WaitForSingleObject(m_Mutex,2000);	
#endif
#ifdef __MAC__
	sem_wait(m_Mutex);
#endif
}

void 
RePESAudio::FreeMutex()
{
#ifdef __MS__
	ReleaseMutex(m_Mutex);
#endif
#ifdef __MAC__
	sem_post(m_Mutex);
#endif
}

int 
RePESAudio::GetNextFreeBucket()
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
RePESAudio::SendBucket(int number, int numFrames)
{
	AquireMutex();
	m_BucketState[number] = m_BucketSequence++;
	m_NumFramesInBucket[number] = numFrames;
	if( m_BucketSequence >= MAX_BUCKETS )
		m_BucketSequence = 0;
	FreeMutex();
}


int 
RePESAudio::FindAC3FrameOffset(unsigned char *Data, int Length)
{
	int		j;

	j = 0;
	while( j < Length - 1 ){
		if( Data[j] == 0x0B && Data[j+1] == 0x77 ){
			return(j+1);
		}
		j++;
	}
	return(0);
}

int RePESAudio::GetBucketStatus(int *TotalBuckets)
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