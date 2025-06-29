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

// ExtractProgram.cpp: implementation of the ExtractProgram class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ExtractProgram.h"
#include "PACKOUT.H"
#include "LogFile.h"



//////////////////////////////////////////////////////////////////////
//                  Globals 
//////////////////////////////////////////////////////////////////////
extern LogFile		dbgLog;

#ifdef __MS__
DWORD WINAPI DePESVThreadProc(void *Param);
DWORD WINAPI DePESAThreadProc(void *Param);
DWORD WINAPI PackOutThreadProc(void *Param);
DWORD WINAPI StubPackOutThreadProc(void *Param);
DWORD WINAPI InputWorkerThreadProc(void *Param);
#endif

#ifdef __MAC__
void * DePESVThreadProc(void *Param);
void * DePESAThreadProc(void *Param);
void * PackOutThreadProc(void *Param);
void * StubPackOutThreadProc(void *Param);
void * InputWorkerThreadProc(void *Param);
#endif


int		Bitrates[3][16] = { {0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,0},
							{0,32,48,56,64,80,96,112,128,160,192,224,256,320,384,0},
							{0,32,40,48,56,64,80,96,112,128,160,192,224,256,320,0} };

int		AC3Rates[19] = { 32,40,48,56,64,80,96,112,128,160,192,224,256,320,384,448,512,576,640 };
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ExtractProgram::ExtractProgram()
{
	m_IsCanceled = false;
	m_Status = EXTRACT_STATUS_IDLE;
	m_AudioFileHandle = m_VideoFileHandle = NULL;
	m_pvr = NULL;
	m_VideoPipe = NULL;
	m_AudioPipe = NULL;
	m_VideoRepack = NULL;
	m_AudioRepack = NULL;
	m_OutputThreadHandle = NULL;
	m_VideoThreadHandle = NULL;
	m_AudioThreadHandle = NULL;
	m_InputWorkerThread = NULL;
	m_VideoThreadDone = false;
	m_AudioThreadDone = false;
	m_OutputThreadDone = false;
	m_InputThreadDone = false;
	m_VideoPID = -1;
	m_AudioPID = -1;
	m_VideoX = -1;
	m_VideoY = -1;
	m_FrameRate = -1;
	m_CompData1 = 0;
	m_CompData2 = 0;
	m_CurFrameCount = 0;
}

ExtractProgram::~ExtractProgram()
{
	if( m_pvr != NULL ){
		if( m_VideoFileHandle != NULL )
			m_pvr->ClosePvrFile(m_VideoFileHandle);
		if( m_AudioFileHandle != NULL )
			m_pvr->ClosePvrFile(m_AudioFileHandle);
	}
	if( m_VideoPipe != NULL )
		delete m_VideoPipe;
	if( m_AudioPipe != NULL )
		delete m_AudioPipe;
	if( m_VideoRepack != NULL )
		delete m_VideoRepack;
	if( m_AudioRepack != NULL )
		delete m_AudioRepack;
#ifdef __MS__
	if( m_OutputThreadHandle != NULL )
		CloseHandle(m_OutputThreadHandle);
	if( m_VideoThreadHandle != NULL )
		CloseHandle(m_VideoThreadHandle);
	if( m_AudioThreadHandle != NULL )
		CloseHandle(m_AudioThreadHandle);
	if( m_InputWorkerThread != NULL )
		CloseHandle(m_InputWorkerThread);
#endif
#ifdef __MAC__

#endif

}

//------------------------------------------------------------
//   PUBLIC Methods
//--------------------------------------------------------------

//-----------------------------------------------
// this function will start the extraction process
void 
ExtractProgram::StartExtractProcess(DishPvr *pvr, int ProgramNumber, WhichStreams *streams, char *FileNameBase)
{
	int				AudioOptions;
	char			LogFileName[MAX_PATH];
	char			Name[128];
	char			DateTime[64];
	char			Length[32];
	char			ProgramChName[64];
	int				ProgramCh;
	char			ProgramDesc[512];
	char			Line[256];

#ifdef __MS__
	unsigned long	ThreadID;
#endif
	
	m_pvr = pvr;
	m_Streams = streams;
	strcpy((char *)m_FileNameBase,FileNameBase);
	if( m_Streams->m_LogChk ){
		strcpy(LogFileName,(char *)m_Streams->m_DefaultPath);
		strcat(LogFileName,FileNameBase);
		strcat(LogFileName,".log");
		m_Log->OpenLogFile(LogFileName,LOG_FILE_NEW);

		m_Log->PrintLnLog("<< Extract Log File >>");
		m_Log->PrintLnLog("<< Program Information >>");
		m_Log->PrintLnLog(" ");
		Name[0] = DateTime[0] = Length[0] = ProgramChName[0] = ProgramDesc[0] = 0;
		m_pvr->GetProgramInfo(Name,DateTime,Length,ProgramChName,&ProgramCh,ProgramDesc,ProgramNumber);
		sprintf(Line,"%d ( %s )",ProgramCh,ProgramChName);
		m_Log->PrintLnLog(Line);
		m_Log->PrintLnLog(" ");
		sprintf(Line,"%s ( %s -- %s )",Name,DateTime,Length);
		m_Log->PrintLnLog(Line);
		m_Log->PrintLnLog(" ");
		m_Log->PrintLnLog("<< Program Description >>");
		m_Log->PrintLnLog(" ");
		m_Log->PrintLnLog(ProgramDesc);
	}
	
	//open the input files
	m_VideoFileHandle = m_pvr->OpenVideoFile( ProgramNumber );
	if( m_VideoFileHandle == NULL ){
		//then fail the process
		dbgLog.PrintLnLog("Unable to Video File!");
		m_Status = EXTRACT_STATUS_ERROR;
		return;
	}
	m_TotalByteToProcess = m_pvr->GetPvrFileSize(m_VideoFileHandle);
	if( !m_pvr->IsSingleTransport() ){
		AudioOptions = 0;
		if( m_Streams->m_AudioIsAC3){
			AudioOptions = HAS_AUDIO_AC3;
			if( m_Streams->m_AlterAudioChk )
				AudioOptions = HAS_AUDIO_ENG;
		} else {
			if( m_Streams->m_AlterAudioChk ){
				AudioOptions = HAS_AUDIO_ALT;
			} else {
				AudioOptions |= HAS_AUDIO_ENG;
			}
		}
		m_AudioFileHandle = m_pvr->OpenAudioFile( ProgramNumber, AudioOptions);
		if( m_AudioFileHandle == NULL ){
			dbgLog.PrintLnLog("Unable to Audio File!");
			m_Status = EXTRACT_STATUS_ERROR;
			return;
		}
		m_TotalByteToProcess += m_pvr->GetPvrFileSize(m_AudioFileHandle);
	}
	dbgLog.Sprintf("Total Bytes to Process: %d",(unsigned long)m_TotalByteToProcess);
	m_BytesProcessed = 0;
	m_CurFrameCount = 0;
	// create the output pipes
	m_VideoPipe = (PESPipe *)new PESPipe;
	m_AudioPipe = (PESPipe *)new PESPipe;
	if( m_VideoPipe == NULL || m_AudioPipe == NULL ){
		dbgLog.PrintLnLog("Unable to create Audio or Video input pipes!");
		m_Status = EXTRACT_STATUS_ERROR;
		return;
	}
	// create the repacking pipes
	m_VideoRepack = (RePES2K *)new RePES2K;
	m_AudioRepack = (RePESAudio *)new RePESAudio;
	if( m_VideoRepack == NULL || m_AudioRepack == NULL ){
		dbgLog.PrintLnLog("Unable to create Audio or Video repack pipes!");
		m_Status = EXTRACT_STATUS_ERROR;
		return;
	}
	m_AudioRepack->SetAC3Mode(m_Streams->m_AudioIsAC3);
#ifdef __MS__
	m_AudioSyncSem = CreateSemaphore(NULL,0,255,NULL);
#endif
#ifdef __MAC__
	m_AudioSyncSem = sem_open("AudSync",O_CREAT,0,0);
#endif
#ifdef __MS__
	//create the output thread
	if( m_Streams->m_VideoChk || m_Streams->m_RenderChk ){
		m_OutputThreadHandle = CreateThread(NULL,0,PackOutThreadProc,(void *)this,0,&ThreadID);
		//create the Video muxing thread
		m_VideoThreadHandle = CreateThread(NULL,0,DePESVThreadProc,(void *)this,0,&ThreadID);
	} else {
		m_VideoThreadDone = true;
		m_OutputThreadHandle = CreateThread(NULL,0,StubPackOutThreadProc,(void *)this,0,&ThreadID);
	}
	//create the audio muxing thread
	m_AudioThreadHandle = CreateThread(NULL,0,DePESAThreadProc,(void *)this,0,&ThreadID);
	//create the input worker thread
	m_InputWorkerThread = CreateThread(NULL,0,InputWorkerThreadProc,(void *)this,0,&ThreadID);
#endif
#ifdef __MAC__
	//create the output thread
	if( m_Streams->m_VideoChk || m_Streams->m_RenderChk ){
		pthread_create(&m_OutputThreadHandle,NULL,PackOutThreadProc,(void *)this);
		//create the Video muxing thread
		pthread_create(&m_VideoThreadHandle,NULL,DePESVThreadProc,(void *)this);
	} else {
		m_VideoThreadDone = true;
		pthread_create(&m_OutputThreadHandle,NULL,StubPackOutThreadProc,(void *)this);
	}
	//create the audio muxing thread
	pthread_create(&m_AudioThreadHandle,NULL,DePESAThreadProc,(void *)this);
	//create the input worker thread
	pthread_create(&m_InputWorkerThread,NULL,InputWorkerThreadProc,(void *)this);
#endif
	m_Status = EXTRACT_STATUS_RUNNING;

}

void	
ExtractProgram::GetProgressInfo( pSTREAM_INFO info )
{
	int	Layer;
	int Rate;

	info->AudioOffset = m_AudioOffset;
	if( !m_Streams->m_AudioIsAC3 ){
		Layer = 0;
		if( (m_CompData1 & 0x06) == 0x04 )
			Layer = 1;
		if( (m_CompData1 & 0x06) == 0x02 )
			Layer = 2;;
		Rate = (m_CompData2 & 0xF0) >> 4;
		info->AudioRate = Bitrates[Layer][Rate];
	} else {
		Rate = (m_CompData1 & 0x3F) / 2;
		if( Rate < 19 )
			info->AudioRate = AC3Rates[Rate];
		else
			info->AudioRate = 0;
	}
	info->AudioType[0] = 0x00;
	if( !m_Streams->m_AudioIsAC3 ){
		if( (m_CompData1 & 0x06) == 0x06 )
			strcat(info->AudioType,"L=I");
		if( (m_CompData1 & 0x06) == 0x04 )
			strcat(info->AudioType,"L=II");
		if( (m_CompData1 & 0x06) == 0x02 )
			strcat(info->AudioType,"L=III");
		if( (m_CompData2 & 0x0C) == 0x00 )
			strcat(info->AudioType," 44.1kHz");
		if( (m_CompData2 & 0x0C) == 0x04 )
			strcat(info->AudioType," 48kHz");
		if( (m_CompData2 & 0x0C) == 0x08 )
			strcat(info->AudioType," 32kHz");
	} else {
		strcat(info->AudioType,"AC3");
		if( (m_CompData1 & 0xC0) == 0x00 )
			strcat(info->AudioType," 48kHz");
		if( (m_CompData1 & 0xC0) == 0x40 )
			strcat(info->AudioType," 44.1kHz");
		if( (m_CompData1 & 0xC0) == 0x80 )
			strcat(info->AudioType," 32kHz");
	}
	info->VideoX = m_VideoX;
	info->VideoY = m_VideoY;
	info->VideoRate = m_FrameRate;
	info->FrameNumber = m_CurFrameCount;
	info->TotalBytesToProcess = m_TotalByteToProcess;
	info->TotalBytesProcessed = m_BytesProcessed;
	if( m_Status == EXTRACT_STATUS_RUNNING ){
		if( m_VideoThreadDone && m_AudioThreadDone && m_OutputThreadDone && m_InputThreadDone){
			// then all the threads are done so clean up
			if( m_VideoFileHandle != NULL )
				m_pvr->ClosePvrFile(m_VideoFileHandle);
			m_VideoFileHandle = NULL;
			if( m_AudioFileHandle != NULL )
				m_pvr->ClosePvrFile(m_AudioFileHandle);
			m_AudioFileHandle = NULL;
			//delete m_VideoPipe;
			//delete m_AudioPipe;
			//delete m_VideoRepack;
			//delete m_AudioRepack;
#ifdef __MS__
			//if( m_Streams->m_VideoChk || m_Streams->m_RenderChk ){
			//	CloseHandle(m_OutputThreadHandle);
			//	CloseHandle(m_VideoThreadHandle );
			//} else {
			//	CloseHandle(m_OutputThreadHandle);
			//}
			//CloseHandle(m_AudioThreadHandle);
			//CloseHandle(m_InputWorkerThread);
#endif
			m_Status = EXTRACT_STATUS_IDLE;
			dbgLog.PrintLnLog("<< File Extraction Complete. >>");
		}
	}
	info->Status = m_Status;
	if( m_Streams->m_DebugChk ){ 
		int		a,b,c;
		//then process debug info here
		dbgLog.PrintLnLog("<< Extraction Engine Status >>");
		m_VideoPipe->GetPipeStatus(&a,&b,&c);
		dbgLog.Sprintf("Video Pipe Status: Head = %4d, Size = %4d, Blocked = %1d",a,b,c);
		m_AudioPipe->GetPipeStatus(&a,&b,&c);
		dbgLog.Sprintf("Audio Pipe Status: Head = %4d, Size = %4d, Blocked = %1d",a,b,c);
		a = m_VideoRepack->GetBucketStatus(&b);
		dbgLog.Sprintf("Video repack pipe Status: %4d Buckets out of %4d used",a,b);
		a = m_AudioRepack->GetBucketStatus(&b);
		dbgLog.Sprintf("Audio repack pipe Status: %4d Buckets out of %4d used",a,b);
	}
}


void 
ExtractProgram::Cancel()
{
	//start off by signalling the cancel 
	dbgLog.PrintLnLog("Canceling Extraction and muxing!");

	m_IsCanceled = true; //!
	//now signel the pipes so that they can release any blocks
	m_VideoPipe->Cancel();
	m_AudioPipe->Cancel();
	m_VideoRepack->Cancel();
	m_AudioRepack->Cancel();
	while( !m_VideoThreadDone || !m_AudioThreadDone || !m_OutputThreadDone || !m_InputThreadDone){
#ifdef __MS__
		Sleep(200);
#endif
#ifdef __MAC__
		usleep(200000);
#endif
	}
	if( m_VideoFileHandle != NULL )
		m_pvr->ClosePvrFile(m_VideoFileHandle);
	m_VideoFileHandle = NULL;
	if( m_AudioFileHandle != NULL )
		m_pvr->ClosePvrFile(m_AudioFileHandle);
	m_AudioFileHandle = NULL;
	//delete m_VideoPipe;
	//delete m_AudioPipe;
	//delete m_VideoRepack;
	//delete m_AudioRepack;
	//m_VideoPipe = NULL;
	//m_AudioPipe = NULL;
	//m_VideoRepack = NULL;
	//m_AudioRepack = NULL;
#ifdef __MS__
	if( m_Streams->m_VideoChk || m_Streams->m_RenderChk ){
		CloseHandle(m_OutputThreadHandle);
		CloseHandle(m_VideoThreadHandle );
	} else {
		CloseHandle(m_OutputThreadHandle);
	}
	CloseHandle(m_AudioThreadHandle);
	CloseHandle(m_InputWorkerThread);
	m_OutputThreadHandle = NULL;
	m_VideoThreadHandle = NULL;
	m_AudioThreadHandle = NULL;
	m_InputWorkerThread = NULL;
#endif
	m_Status = EXTRACT_STATUS_CANCELED;
}

///////////////////////////////////////////////////////////////
// Private methods
//////////////////////////////////////////////////////////////
bool
ExtractProgram::ReadTransportStream( unsigned char *TransportBuffer, int *InputTail)
{
	int		AmountRead,length,cnt;

	length = INPUT_BUFFER_SIZE - *InputTail;
	cnt = length / 4096;  //lets chop this up into 4K amounts
	length = cnt * 4096;
	if( length == 0 )
		return(true);  //this is not an error
	AmountRead = m_pvr->ReadPvrFile(m_VideoFileHandle,&TransportBuffer[*InputTail],length);
	*InputTail += AmountRead;
	m_BytesProcessed += AmountRead;
	if( AmountRead == 0 ){
		dbgLog.PrintLnLog("Transport stream read EOF");
		return(false);
	}
	return(true);

}

void
ExtractProgram::ProcessTransportStream( unsigned char *TransportBuffer, unsigned char *VPipeBuffer, unsigned char *APipeBuffer, 
									int *InputTail, int *InputPtr,
									int *VTail, int *VPtr,
									int *ATail, int *APtr )
{
	int		j,k,dataLen;
	int		tei,psti,tp,pid,tsc,cc,afc,afl;
	unsigned long	PesHeaderSig;
	
	j = *InputPtr;
	while( (*InputPtr + 188) <= *InputTail ){
		//find the packet header...
		while( j < *InputTail && TransportBuffer[j] != 0x47)
			j++;
		if( j >= *InputTail ){ // then we did not find one
			k = *InputTail - *InputPtr;
			if( k > 0 && k < 188){
				memcpy((void *)&TransportBuffer[0],(void *)&TransportBuffer[*InputPtr],k);
				*InputPtr = 0;
				*InputTail = k;
				return;  //we are done
			}
			//if we get here then the buffer is garbage throw it out
			*InputPtr = 0;
			*InputTail = 0;
			return;
		}
		// if we get here then we might have a ts packet
		k = j + 1;
		tei = ( TransportBuffer[k] & 0x80 )? 1:0;
		psti = ( TransportBuffer[k] & 0x40 )? 1:0;
		tp = ( TransportBuffer[k] & 0x20 )? 1:0;
		pid = TransportBuffer[k++] & 0x1F;
		pid <<= 8;
		pid |= TransportBuffer[k++];
		tsc = (TransportBuffer[k] >> 6);
		cc = (TransportBuffer[k] & 0x0F);
		afc = ((TransportBuffer[k++] & 0x30) >> 4);
		afl = 0;
		if( afc == 2 || afc == 3 ){
				afl = TransportBuffer[k++];
				k += afl;
		}
		//we now have all the position info from the header now verify the header
		if( tsc != 0 || afc == 0 || tei != 0 ){
			//then asume bad
			j++;
			continue;
		}
		// ok the packet is valid, then process it
		if( m_VideoPID == -1 || m_AudioPID == -1 ){
			if( psti > 0 ){
				//then we have a PES packet header look at it!
				if( *(unsigned long *)&TransportBuffer[k] == PES_VIDEO_SIGNATURE && m_VideoPID == -1){
					//then this is the Video PID
					m_VideoPID = pid;
				}
				if( *(unsigned long *)&TransportBuffer[k] == PES_AUDIO_SIGNATURE && m_AudioPID == -1 && !m_Streams->m_AudioIsAC3){
					//then this is the Video PID
					if( (pid & 0x00FF) == 0x0023 && !m_Streams->m_AlterAudioChk)
						m_AudioPID = pid;
					if( (pid & 0x00FF) != 0x0023 && m_Streams->m_AlterAudioChk)
						m_AudioPID = pid;
				}
				if( *(unsigned long *)&TransportBuffer[k] == PES_AC3AUDIO_SIGNATURE && m_AudioPID == -1 && m_Streams->m_AudioIsAC3){
					//then this is the Video PID
					m_AudioPID = pid;
				}
			} 
		}
		// get the packet data length
		dataLen = (j + 188) - k;
		if( psti > 0 ){
			if( dataLen > 4 ){
				//then validate the PES header
				PesHeaderSig = *(unsigned long *)&TransportBuffer[k];
				if( (PesHeaderSig & PES_HEADER_MASK) != PES_HEADER_SIG ){
					if( m_AudioPID == -1 && m_VideoPID == -1){
						if( m_Streams->m_DebugChk ){
							dbgLog.Sprintf(" Possible Encryped Stream : PES Header Sig = %X",PesHeaderSig);			
						}
						// we have a possible encrypted stream!
					}
				}
			}
		}
		if( pid == m_VideoPID ){
			//then we have a video packet
			if( psti > 0 ){
				//here we need to check for Non Dish Packs like in the 721
				if( (TransportBuffer[k + 7] & 0xC0) == 0 ){
					k += 9 + TransportBuffer[k + 8];
					dataLen -= (9 + TransportBuffer[k + 8]);
					if( dataLen < 0 )
						dataLen = 0;
				}
			}
			if( dataLen <= (MAX_VPIPE_SIZE - *VTail)){
				if( dataLen > 0 ){
					memcpy((void *)&VPipeBuffer[*VTail], (void *)&TransportBuffer[k],dataLen);
					*VTail = *VTail + dataLen;
				}
			} else {
				//not enough room so bail!
				//OutPutDebugString("Error processing transport stream Video Buffer too small!");
				//m_InputThreadDone = true;
				//Cancel();
				if( dataLen > 0 ) //then we cannot output it
					return;
			}
		} else {
			if( pid == m_AudioPID ){
				//then an audio packet
				//see if there is enough room to output it
				if( dataLen <= (MAX_APIPE_SIZE - *ATail) && dataLen > 0){
					memcpy((void *)&APipeBuffer[*ATail], (void *)&TransportBuffer[k],dataLen);
					*ATail = *ATail + dataLen;
				} else {
					//not enough room so bail!
				//	OutPutDebugString("Error processing transport stream Audio Buffer too small!");
				//	m_InputThreadDone = true;
				//	Cancel();
					if( dataLen > 0 ) //then we cannot output it
						return;
				}
			}
		}
		//then the next packet
		j += 188;
		*InputPtr = j;
	}
	dataLen = *InputTail - *InputPtr;
	if( dataLen > 0 ){
		memcpy((void *)&TransportBuffer[0], (void *)&TransportBuffer[*InputPtr],dataLen);
		*InputPtr = 0;
		*InputTail = dataLen;
	} else {
		*InputPtr = 0;
		*InputTail = 0;
	}
}

bool
ExtractProgram::ReadVideoFile( unsigned char *VPipeBuffer, int *VTail )
{
	int		AmountRead,length;
	int		cnt;

	if( m_VideoFileHandle == NULL )
		return(false);
	length = MAX_VPIPE_SIZE - *VTail;
	cnt = length / (4 * 1024);  //lets make sure that the read is some block based!
	length = cnt * (4 * 1024);
	if( length == 0 )
		return(true);  //this is not an error
	AmountRead = m_pvr->ReadPvrFile(m_VideoFileHandle,&VPipeBuffer[*VTail],length);
	*VTail += AmountRead;
	length -= AmountRead;
	m_BytesProcessed += AmountRead;
	if( AmountRead == 0 )
		return(false);  //indicate that the file is done!
	return( true );
}

bool
ExtractProgram::ReadAudioFile( unsigned char *APipeBuffer, int *ATail )
{
	int		AmountRead,length;
	int		cnt;

	if( m_AudioFileHandle == NULL )
		return(false);
	length = MAX_APIPE_SIZE - *ATail;
	cnt = length / (4 * 1024);  //lets make sure that the read is some block based!
	length = cnt * (4 * 1024);
	if( length == 0 )
		return(true);  //this is not an error
	//ReadFile(m_AudioFileHandle,&APipeBuffer[*ATail],length,(DWORD *)&AmountRead,NULL);
	AmountRead = m_pvr->ReadPvrFile(m_AudioFileHandle,&APipeBuffer[*ATail],length);
	*ATail += AmountRead;
	length -= AmountRead;
	m_BytesProcessed += AmountRead;
	if( AmountRead == 0 )
		return(false);  //indicate that the file is done!
	return( true );

}

//-------------------------------------------------------------------------
// this function will process buffer untill buffer empty or pipe is full
//   returns false if pipe full, true if buffer empty
bool	
ExtractProgram::ProcessVStream( unsigned char *VPipeBuffer, int *VTail, int *VPtr, bool InputDone )
{
	FRAME_INFO		fInfo;
	int				v,j,k;
	unsigned char	c;
	unsigned char	*PacBuffer;

	//first see if there is room to even process a buffer?
	if( m_VideoPipe->IsFull() )
		return(false);
	v = *VPtr;  //this should always be 0 comming in
	if( v != 0 ){
		//then we need to check this out!
		j = *VPtr;
	}
	//make sure that VPtr is pointing to the begining of a video header
	if( *(unsigned long *)&VPipeBuffer[v] != PES_VIDEO_SIGNATURE){
		//this should only be once
		while( v <= (*VTail - 4) && *(unsigned long *)&VPipeBuffer[v] != PES_VIDEO_SIGNATURE)
			v++;
		if( v > (*VTail - 4)){  //then we did not find one in the entire stream! trash it!
			*VTail = 0;
			*VPtr = 0;
			return(true);
		} else {
			*VPtr = v;
		}
	}
	while( !m_IsCanceled ){
		//lets process a video frame
		v += 4;
		// now find the next one
		while( v <= (*VTail - 4) && *(unsigned long *)&VPipeBuffer[v] != PES_VIDEO_SIGNATURE )
			v++;
		if( v <= (*VTail - 4)){
			//then we have a frame!
			fInfo.FrameLength = v - *VPtr;
			j = *VPtr;
			*VPtr = *VPtr + fInfo.FrameLength;
			fInfo.HeaderOffset = 9 + VPipeBuffer[j + 8];
			fInfo.PTS = fInfo.DTS = 0;
			if( (VPipeBuffer[j + 7] & 0x80) == 0x80 ){ //then there is a pts
				fInfo.PTS = ((VPipeBuffer[j + 9] >> 1) & 0x07);  //32..30
				fInfo.PTS <<= 8;
				fInfo.PTS |= VPipeBuffer[j + 10];				//29..15
				fInfo.PTS <<= 7;
				fInfo.PTS |= ((VPipeBuffer[j + 11] >> 1) & 0x7F);
				fInfo.PTS <<= 8;
				fInfo.PTS |= VPipeBuffer[j + 12];				//14..0
				fInfo.PTS <<= 7;
				fInfo.PTS |= ((VPipeBuffer[j + 13] >> 1) & 0x7F);
			}
			k = fInfo.HeaderOffset;
			// now get the frame type
			while( k < (fInfo.FrameLength - 4) && *(unsigned long *)&VPipeBuffer[j + k] != PICTURE_HEADER_SIG)
				k++;
			fInfo.frameType = '?';
			if( *(unsigned long *)&VPipeBuffer[j + k] == PICTURE_HEADER_SIG ){
				c = ((VPipeBuffer[j + k + 5] >> 3) & 0x07); 
				if( c == 1 )
					fInfo.frameType = 'I';
				if( c == 2 )
					fInfo.frameType = 'P';
				if( c == 3 )
					fInfo.frameType = 'B';
				// now we have all the info then send it to the next threads
				PacBuffer = (unsigned char *)malloc(fInfo.FrameLength + sizeof(FRAME_INFO));
				if( PacBuffer == NULL ){
#ifdef __MS__
					Sleep(500);
#endif
#ifdef __MAC__
					usleep(500000);
#endif
					PacBuffer = (unsigned char *)malloc(fInfo.FrameLength + sizeof(FRAME_INFO));
					if( PacBuffer == NULL ){
						dbgLog.PrintLnLog("VStream Processing memory allocation Failed!");
						//this is bad
						m_InputThreadDone = true;
						Cancel();
						return(true);
					}
				}
				memcpy((void *)PacBuffer,(void *)&fInfo,sizeof(FRAME_INFO));
				memcpy((void *)&PacBuffer[sizeof(FRAME_INFO)],(void *)&VPipeBuffer[j],fInfo.FrameLength);
				m_VideoPipe->PutPESFrame((void *)PacBuffer);
				if( m_VideoPipe->IsFull() )
					return(false);
			} else {
				//this also is bad throw this frAme away
				dbgLog.PrintLnLog("Invalid Video Frame Type!");
				fInfo.frameType = '!';
			}
		} else {
			if( *VPtr > 0 ){
				memcpy((void *)&VPipeBuffer[0],(void *)&VPipeBuffer[*VPtr],(*VTail - *VPtr));
				*VTail = (*VTail - *VPtr);
				*VPtr = 0;
				v = 0;
			} else {
				//we need to see ifthere is somthing wrong and is so trach this buffer and continue
				if( (MAX_VPIPE_SIZE - *VTail) < 4096){
					*VTail = 0;	//then we need to just chuck this buffer
				}
				if( InputDone ){
					*VTail = 0;
				}
				*VPtr = 0;
				v = 0;
			}
			return(true);
		}
	}
	return(true);
}

//-------------------------------------------------------------------------
// this function will process buffer untill buffer empty or pipe is full
//   returns false if pipe full, true if buffer empty
bool	
ExtractProgram::ProcessAStream( unsigned char *APipeBuffer, int *ATail, int *APtr, bool InputDone )
{
	unsigned long	findWhat;
	FRAME_INFO		fInfo;
	unsigned char	*PacBuffer;
	int				a;

	if( m_AudioPipe->IsFull() )
		return(false);
	a = *APtr;
	if( a != 0 ){
		a = *APtr;
	}
	findWhat = PES_AUDIO_SIGNATURE;
	if( m_Streams->m_AudioIsAC3 )
		findWhat = PES_AC3AUDIO_SIGNATURE;
	if( m_Streams->m_AudioIsAC3 && m_Streams->m_AlterAudioChk)
		findWhat = PES_ALT_AUDIO_SIGNATURE;
	if( *(unsigned long *)&APipeBuffer[a] != findWhat){
		//this should only be once
		while( a <= (*ATail - 4) && *(unsigned long *)&APipeBuffer[a] != findWhat)
			a++;
		if( a > (*ATail - 4)){
			*ATail = 0;
			*APtr = 0;
			return(true);
		} else {
			*APtr = a;
		}
	}
	while( !m_IsCanceled ){
		// now do an audio frame
		fInfo.FrameLength = MAX_APIPE_SIZE;
		if( *(unsigned long *)&APipeBuffer[a] == findWhat ){
			fInfo.FrameLength = APipeBuffer[a+4];
			fInfo.FrameLength <<= 8;
			fInfo.FrameLength |= APipeBuffer[a+5];
			fInfo.FrameLength += 6;
		} else {
			while( a <= (*ATail - 4) && *(unsigned long *)&APipeBuffer[a] != findWhat ) //this should point to on already
				a++;
			if( a <= (*ATail - 4))
				continue;
		}
		if( (a + fInfo.FrameLength) > (*ATail)){				
			if( *APtr > 0 ){ //then normal processing!
				memcpy((void *)&APipeBuffer[0],(void *)&APipeBuffer[a],(*ATail - a));
				*ATail = (*ATail - a);
			} else {
				if( InputDone )
					*ATail = 0;  //we are done so good by!
			}
			*APtr = 0;
			return(true);
		}
		*APtr = a + fInfo.FrameLength;
		fInfo.HeaderOffset = 9 + APipeBuffer[a + 8];
		fInfo.PTS = fInfo.DTS = 0;
		if( (APipeBuffer[a + 7] & 0x80) == 0x80 ){ //then there is a pts
			fInfo.PTS = ((APipeBuffer[a + 9] >> 1) & 0x07);  //32..30
			fInfo.PTS <<= 8;
			fInfo.PTS |= APipeBuffer[a + 10];				//29..15
			fInfo.PTS <<= 7;
			fInfo.PTS |= ((APipeBuffer[a + 11] >> 1) & 0x7F);
			fInfo.PTS <<= 8;
			fInfo.PTS |= APipeBuffer[a + 12];				//14..0
			fInfo.PTS <<= 7;
			fInfo.PTS |= ((APipeBuffer[a + 13] >> 1) & 0x7F);
		}
		// now we have all the info then send it to the next threads
		PacBuffer = (unsigned char *)malloc(fInfo.FrameLength + sizeof(FRAME_INFO));
		if( PacBuffer == NULL ){
#ifdef __MS__
			Sleep(500);
#endif
#ifdef __MAC__
			usleep(500000);
#endif
			PacBuffer = (unsigned char *)malloc(fInfo.FrameLength + sizeof(FRAME_INFO));
			if( PacBuffer == NULL ){
				//this is bad
				dbgLog.PrintLnLog("AStream Processing memory allocation Failed!");
				m_InputThreadDone = true;
				Cancel();
				return(true);
			}
		}
		memcpy((void *)PacBuffer,(void *)&fInfo,sizeof(FRAME_INFO));
		memcpy((void *)&PacBuffer[sizeof(FRAME_INFO)],(void *)&APipeBuffer[a],fInfo.FrameLength);
		m_AudioPipe->PutPESFrame((void *)PacBuffer);
		if( m_AudioPipe->IsFull() )
			return(false);
		a = *APtr;
	}
	return(true);	
}

//----------------------------------------------------------
// this function will locate the starting frames in the pipes
void 
ExtractProgram::LocateStartingFrames()
{
	
	unsigned long	I1,I2,I3,A2;
	int64			Diff1,Diff2,I1pts,I2pts,I3pts;
	pFRAME_INFO		VidFrame,AudFrame;
	unsigned long	CurVidFrame, CurAudFrame;
	unsigned long	SkipCount;
	unsigned long	RetryCount;
	unsigned char	*ptr;

	dbgLog.PrintLnLog("Locating starting frames.");
	//now scan for the first I frame
	I1 = 0;
	VidFrame = (pFRAME_INFO)m_VideoPipe->PeekPESFrame(I1);
	if( VidFrame == NULL )
		return;
	I1pts = VidFrame->PTS;
	while( VidFrame != NULL && VidFrame->frameType != 'I' ){
		I1++;
		VidFrame = (pFRAME_INFO)m_VideoPipe->PeekPESFrame(I1);
		if( VidFrame == NULL )
			return;
		I1pts = VidFrame->PTS;
	}
	//now scan for the second I frame
	I2 = I1 + 1;
	VidFrame = (pFRAME_INFO)m_VideoPipe->PeekPESFrame(I2);
	if( VidFrame == NULL )
		return;
	I2pts = VidFrame->PTS;
	while( VidFrame != NULL && VidFrame->frameType != 'I' ){
		I2++;
		VidFrame = (pFRAME_INFO)m_VideoPipe->PeekPESFrame(I2);
		if( VidFrame == NULL )
			return;
		I2pts = VidFrame->PTS;
	}
	//now scan for the third I frame
	I3 = I2 + 1;
	VidFrame = (pFRAME_INFO)m_VideoPipe->PeekPESFrame(I3);
	if( VidFrame == NULL )
		return;
	I3pts = VidFrame->PTS;
	while( VidFrame != NULL && VidFrame->frameType != 'I' ){
		I3++;
		VidFrame = (pFRAME_INFO)m_VideoPipe->PeekPESFrame(I3);
		if( VidFrame == NULL )
			return;
		I3pts = VidFrame->PTS;
	}
	if( I1pts > I2pts ){
		Diff1 = 255;
	} else {
		Diff1 = I2pts - I1pts;
		Diff1 = ((Diff1 * 300) / (int64)27000000);
	}
	if( I2pts > I3pts ){
		Diff2 = 255;
	} else {
		Diff2 = I3pts - I2pts;
		Diff2 = ((Diff2 * 300) / (int64)27000000);
	}
	if( Diff2 <= 4 ){
		if( Diff1 < 4 ){
			CurVidFrame = I1;
		} else {
			CurVidFrame = I2;
		}
	} else {
		CurVidFrame = I3;
	}
	// ok now get rid of the ones we do not want!
	for( SkipCount = 0; SkipCount < CurVidFrame; SkipCount++){
		ptr = (unsigned char *)m_VideoPipe->GetPESFrame();
		if( ptr != NULL )
			free((void *)ptr);
	}
	CurVidFrame = 0;
	//ok Now we have an I frame starting point.
	VidFrame = (pFRAME_INFO)m_VideoPipe->PeekPESFrame(CurVidFrame);
	if( VidFrame == NULL )
		return;
	CurAudFrame = 0;
	RetryCount = 0;
	AudFrame = (pFRAME_INFO)m_AudioPipe->PeekPESFrame( CurAudFrame );
	if( AudFrame == NULL )
		return;
	if( AudFrame->PTS > VidFrame->PTS ){ //ok this is corner case where audio pts is greater than videpp start
		//ok now set a time out or limit
		A2 = 100;
		while( AudFrame->PTS > VidFrame->PTS ){
			CurAudFrame++;
			AudFrame = (pFRAME_INFO)m_AudioPipe->PeekPESFrame( CurAudFrame );
			if( AudFrame == NULL )
				return;
			A2--;
			if( A2 == 0 ){ //then we have a case where the I frames start before the Audio
				//ok so we need to advance the video untill we are close
				CurAudFrame = 4;
				AudFrame = (pFRAME_INFO)m_AudioPipe->PeekPESFrame( CurAudFrame );
				if( AudFrame == NULL )
					return;
				// ok now get rid of the ones we do not want!
				for( SkipCount = 0; SkipCount < CurVidFrame; SkipCount++){
					ptr = (unsigned char *)m_VideoPipe->GetPESFrame();
					if( ptr != NULL )
						free((void *)ptr);
				}
				CurVidFrame = 0;
			
				CurVidFrame++;
				VidFrame = (pFRAME_INFO)m_VideoPipe->PeekPESFrame(CurVidFrame);
				if( VidFrame == NULL )
					return;
				while( VidFrame != NULL && VidFrame->frameType != 'I' ){
					CurVidFrame++;
					VidFrame = (pFRAME_INFO)m_VideoPipe->PeekPESFrame(CurVidFrame);
					if( VidFrame == NULL )
						return;
				}
				RetryCount++;
				A2 = 100;
				if( RetryCount > 10 ){
					dbgLog.PrintLnLog("Unable to locate starting frames!");
					//if we get here then there is something very screwy about the audio files!
					return;
				}
			}
		}
	}
	Diff1 = AudFrame->PTS;
	CurAudFrame++;
	AudFrame = (pFRAME_INFO)m_AudioPipe->PeekPESFrame( CurAudFrame );
	if( AudFrame == NULL )
		return;
	while( AudFrame != NULL && AudFrame->PTS <= VidFrame->PTS){ //now find the Pes frame we need
		CurAudFrame++;
		AudFrame = (pFRAME_INFO)m_AudioPipe->PeekPESFrame( CurAudFrame );
		if( AudFrame == NULL )
			return;
		if( CurAudFrame > 2 ){
			//then get rid of the really old ones
			ptr = (unsigned char *)m_AudioPipe->GetPESFrame();
			if( ptr != NULL )
				free((void *)ptr);
			else
				return;
			CurAudFrame--;
		}
	}
	//now back off one
	CurAudFrame--;
	AudFrame = (pFRAME_INFO)m_AudioPipe->PeekPESFrame( CurAudFrame );
	if( AudFrame == NULL )
		return;

	AdjustSliceData( CurAudFrame, VidFrame->PTS);
	
	if( m_Streams->m_VideoChk || m_Streams->m_RenderChk ) 
		m_AudioOffset = (double)((AudFrame->PTS - VidFrame->PTS) * 300) / (double)27000000;
	//now set the previous frames to be droped.
	for( SkipCount = 0; SkipCount < CurAudFrame; SkipCount++){
		//ok get rid of the ones we do not want!
		ptr = (unsigned char *)m_AudioPipe->GetPESFrame();
		if( ptr != NULL )
			free((void *)ptr);
	}
	// now do the video
	for( SkipCount = 0; SkipCount < CurVidFrame; SkipCount++){
		//ok get rid of the ones we do not want!
		ptr = (unsigned char *)m_VideoPipe->GetPESFrame();
		if( ptr != NULL )
			free((void *)ptr);
	}
	dbgLog.PrintLnLog("starting frames Located");
}

//---------------------------------------------------
// this function will adjust the temproal reference in the picture header
void 
ExtractProgram::AdjustTemporalReference(unsigned long FrameNum)
{
	
	pFRAME_INFO		frm;
	int				j;
	unsigned long	RefNum;
	unsigned char	*Data;

	frm = (pFRAME_INFO)m_VideoPipe->PeekPESFrame(FrameNum);
	if( frm == NULL )
		return; // bail
	Data = (unsigned char *)frm;
	Data += sizeof(FRAME_INFO);
	for( j = frm->HeaderOffset; j < frm->FrameLength; j++){
	// locate the picture header
		if( *(unsigned long *)&Data[j] == PICTURE_HEADER_SIG ){ // then a picture header
			// now adjust the reference
			RefNum = Data[j + 4];
			RefNum <<= 8;
			RefNum |= Data[j + 5];
			RefNum >>= 6;
			if( RefNum >= 2 ){
				RefNum -= 2;
				RefNum <<= 6;
				Data[j + 5] &= 0x3F;
				Data[j + 5] |= (unsigned char)(RefNum & 0xC0);
				RefNum >>= 8;
				Data[j + 4] = (unsigned char)RefNum;
			}
			break;
		}
	}
}

//-------------------------------------------------------------
// this function will adjust the gop time code
void 
ExtractProgram::SetGOPTimeCode( int64 PTS, pFRAME_INFO frm)
{
	unsigned char	Hr,Min,Sec,Fr;
	int				j;
	int64			TimeCode;
	unsigned char	*Data;

	GetTime(PTS,&Hr,&Min,&Sec,&Fr);
	Data = (unsigned char *)frm;
	Data += sizeof(FRAME_INFO);
	for( j = frm->HeaderOffset; j < frm->FrameLength; j++){
	// locate the GOP header
		if( *(unsigned long *)&Data[j] == GOP_HEADER_SIG ){ // then a GOP header
			TimeCode = (Data[j+4] & 0x80)? 1:0;
			TimeCode <<= 5;
			TimeCode |= Hr;
			TimeCode <<= 6;
			TimeCode |= Min;
			TimeCode <<= 1;
			TimeCode |= 0x01;
			TimeCode <<= 6;
			TimeCode |= Sec;
			TimeCode <<= 6;
			TimeCode |= Fr;
			// now put it into the data stream
			Data[j+7] &= 0x7F;
			if( (TimeCode & 0x01) == 1 )
				Data[j+7] |= 0x80;
			TimeCode >>= 1;
			Data[j+6] = (unsigned char)(TimeCode & 0xFF);
			TimeCode >>= 8;
			Data[j+5] = (unsigned char)(TimeCode & 0xFF);
			TimeCode >>= 8;
			Data[j+4] = (unsigned char)(TimeCode & 0xFF);
			break;
		}
	}
}


//---------------------------------------------------
// this function will adjust the temproal reference in the picture header
bool 
ExtractProgram::IsRFF(unsigned char *Data, unsigned long len)
{
	int		j;

	j = 0;
	while( len > 0 && *(unsigned long *)&Data[j] != EXTENTION_START_SIG){
		j++;
		len--;
	}
	if( len > 0 ){
		if( Data[j + 7] & 0x02 )
			return(true);
	}
	return(false);
}

//---------------------------------------------------
//this function will extract the ptsfrom a PES header
int64		
ExtractProgram::GetPTS( unsigned char *Data){

	int64		rc,x;
	
	
	rc = 0;
	if( Data[0] != 0x00 && Data[1] != 0x00 && Data[2] != 0x01 )
		return(rc);  // bad header
	if( Data[3] != 0xE0 && Data[3] != 0xC0 && Data[3] != 0xBD && Data[3] != 0xC1)
		return(rc);  // bad header
	if( (Data[6] & 0xC0) != 0x80 )
		return(rc);  // bad header
	if( (Data[7] & 0xC0) != 0x80 && (Data[7] & 0xC0) != 0xC0)
		return(rc);  // bad header
	rc = Data[9] & 0x0E;
	rc <<= 29;
	x = Data[10];
	x <<= 7;
	x |= (Data[11] >> 1);
	x <<= 15;
	rc |= x;
	x = Data[12];
	x <<= 7;
	x |= (Data[13] >> 1);
	rc |= x;
	return(rc);
}

//---------------------------------------------------
// this function will Get the video info from the data
void 
ExtractProgram::GetVideoInfo(unsigned char *Data, unsigned long len)
{
	int		j;

	j = 0;
	while( len > 0 && *(unsigned long *)&Data[j] != VIDEO_SEQUENCE_SIG){
		j++;
		len--;
	}
	if( len > 0 ){
		m_VideoX = 	Data[j + 4];
		m_VideoX <<= 4;
		m_VideoX |= ((Data[j +  5] >> 4) & 0x0F);
		m_VideoY = 	(Data[j + 5] & 0x0F);
		m_VideoY <<= 8;
		m_VideoY |= Data[j + 6];
		if( (Data[j + 7] & 0x0F) == 1 )
			m_FrameRate = 23.976;
		if( (Data[j + 7] & 0x0F) == 2 )
			m_FrameRate = 24.0;
		if( (Data[j + 7] & 0x0F) == 3 )
			m_FrameRate = 25.0;
		if( (Data[j + 7] & 0x0F) == 4 )
			m_FrameRate = 29.97;
		if( (Data[j + 7] & 0x0F) == 5 )
			m_FrameRate = 30.0;
		if( (Data[j + 7] & 0x0F) == 6 )
			m_FrameRate = 50.0;
		if( (Data[j + 7] & 0x0F) == 7 )
			m_FrameRate = 59.94;
	}
}
///////////////////////////////////////////////////////////////
// private methods 
//////////////////////////////////////////////////////////////

//--------------------------------------------
// this function will get the time from PTS
void 
ExtractProgram::GetTime( int64 Pts, unsigned char *Hour, unsigned char *Min, unsigned char *Sec, unsigned char *Frame)
{
	double			time;
	unsigned long	intTime;

	time = (double)Pts;
	time = time * 300;
	time = time / 27000000.0;
	intTime = (unsigned long)time;
	time = time - intTime;
	*Hour = (unsigned char)(intTime / (60 * 60));
	intTime -= *Hour * (60 * 60);
	*Min = (unsigned char)(intTime / 60);
	intTime -= *Min * 60;
	*Sec = (unsigned char)intTime;
	*Frame = (unsigned char)(time * 29.97);
}

void 
ExtractProgram::AdjustSliceData(int FrameNum, int64 Pts)
{
	SAMPLE_SCAN		Scan;
	pFRAME_INFO		frm;
	unsigned long	j;
	int64			AudioPTSFrameRate;
	unsigned char	*Data;
	

	AudioPTSFrameRate = (m_Streams->m_AudioIsAC3 )? 0xB40 : 0x870;
	frm = (pFRAME_INFO)m_AudioPipe->PeekPESFrame( FrameNum );
	if( frm == NULL )
		return;  //die
	Data = (unsigned char *)frm;
	Data += sizeof(FRAME_INFO);
	ScanForSamples( &Data[ frm->HeaderOffset ],(frm->FrameLength - frm->HeaderOffset),&Scan);
	for( j = 0; j < Scan.NumSamples; j++){
		Scan.Samples[j].Pts = frm->PTS + (j * AudioPTSFrameRate);
		if( Scan.Samples[j].Pts <= Pts && (Scan.Samples[j].Pts + AudioPTSFrameRate) > Pts)
			break;
	}
	if( j == Scan.NumSamples)
		j = 0;
	frm->HeaderOffset += Scan.Samples[j].DataOffset;
	frm->PTS = Scan.Samples[j].Pts;

}

void 
ExtractProgram::ScanForSamples(unsigned char *Data, unsigned long Length, pSAMPLE_SCAN samples)
{
	int		j,rc;

	rc = 0;			//scan the data for Frame header in the Mpeg Audio data or AC3
	if( !m_Streams->m_AudioIsAC3 )
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
					samples->Samples[rc].DataOffset = j;
					rc++;
				}
			}
		}
	} 
	else 
	{ //AC3 
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
					samples->Samples[rc].DataOffset = j;
					rc++;
				}
			}
		}
	}
	samples->NumSamples = rc;
	return;
}
///////////////////////////////////////////////////////////////
// thread procs 
//////////////////////////////////////////////////////////////
#ifdef __MS__
DWORD WINAPI 
#endif
#ifdef __MAC__
void *
#endif
InputWorkerThreadProc(void *Param)
{
	ExtractProgram	*cls;
	unsigned char	*VPipeBuffer;
	unsigned char	*APipeBuffer;
	unsigned char	*TransportBuffer;
	int				InputHead,InputTail,VHead,VTail,AHead,ATail,InputPtr,VPtr,APtr;
	bool			bVBufferDone,bABufferDone,bInputDone,bTstream,bVPipeFlushed,bAPipeFlushed;
//	int				Vx,Ax;

	cls = (ExtractProgram *)Param;
	dbgLog.PrintLnLog("Input thread Starting...");
	// now allocate the needed buffers
	TransportBuffer = NULL;
	if( cls->m_pvr->IsSingleTransport() ){
		TransportBuffer = (unsigned char *)malloc(INPUT_BUFFER_SIZE);	
		if( TransportBuffer == NULL ){
			dbgLog.PrintLnLog("Unable to allocate transport buffer!");
			cls->m_InputThreadDone = true;
			cls->Cancel();
			cls->m_Status = EXTRACT_STATUS_ERROR;
			return(0);
		}
	}
	// allocate video pipe buffer
	VPipeBuffer = NULL;
	if( cls->m_Streams->m_RenderChk || cls->m_Streams->m_VideoChk ){
		VPipeBuffer = (unsigned char *)malloc(MAX_VPIPE_SIZE);
		if( VPipeBuffer == NULL ){
			//then we shoudl die
			dbgLog.PrintLnLog("Unable to allocate Video stream buffer!");
			cls->m_InputThreadDone = true;
			cls->Cancel();
			cls->m_Status = EXTRACT_STATUS_ERROR;
			if( TransportBuffer != NULL )
				free((void *)TransportBuffer);
			return(0);
		}
	}
	// allocate the audio pipe buffer
	APipeBuffer = NULL;
	APipeBuffer = (unsigned char *)malloc(MAX_APIPE_SIZE);
	if( APipeBuffer == NULL ){
		//then we shoudl die
		if( TransportBuffer != NULL )
			free((void *)TransportBuffer);
		if( VPipeBuffer != NULL )
			free((void *)VPipeBuffer);
		dbgLog.PrintLnLog("Unable to allocate Audio stream buffer!");
		cls->m_InputThreadDone = true;
		cls->Cancel();
		cls->m_Status = EXTRACT_STATUS_ERROR;
		return(0);
	}
	//ok now we have all the buffers and we should be ready to go
	InputHead = InputTail = VHead = VTail = AHead = ATail = 0;
	InputPtr = VPtr = APtr = 0;
	bInputDone = false;
	bVBufferDone = false;
	bABufferDone = false;
	while( !cls->m_IsCanceled && !bInputDone) {
		if( cls->m_pvr->IsSingleTransport() ){ 
			// then process the transport stream untill the Vinput buffer is full
			// or the audio input buffer is full
			bTstream = false;
			while( !bInputDone && !bTstream ){
				if( !cls->ReadTransportStream( TransportBuffer, &InputTail)){
					bVBufferDone = true;
					bABufferDone = true;
					bTstream = true;
				}
				cls->ProcessTransportStream( TransportBuffer, VPipeBuffer, APipeBuffer, 
										&InputTail, &InputPtr, &VTail, &VPtr, &ATail, &APtr );
				if( (MAX_VPIPE_SIZE - VTail) < 188 )
					bTstream = true;
				if( (MAX_APIPE_SIZE - ATail) < 188 )
					bTstream = true;
			}						
			//if( cls->m_BytesProcessed >= cls->m_TotalByteToProcess)
			//	bTstream = true;
		} else {
			// the read in the files first
			bVBufferDone = cls->ReadVideoFile( VPipeBuffer,  &VTail);
			bVBufferDone = ( bVBufferDone )? false:true; 
			bABufferDone = cls->ReadAudioFile( APipeBuffer,  &ATail);
			bABufferDone = ( bABufferDone )? false:true; 
		}
		bVPipeFlushed = bAPipeFlushed = false;
		while( !bVPipeFlushed && !bAPipeFlushed && !cls->m_IsCanceled){
			bVPipeFlushed = cls->ProcessVStream( VPipeBuffer, &VTail, &VPtr, bVBufferDone);
			bAPipeFlushed = cls->ProcessAStream( APipeBuffer, &ATail, &APtr, bABufferDone);
		}
		if( bVBufferDone && bABufferDone && bVPipeFlushed && bAPipeFlushed)
			bInputDone = true;
	}
	if( TransportBuffer != NULL )
		free((void *)TransportBuffer);
	if( VPipeBuffer != NULL )
		free((void *)VPipeBuffer);
	if( APipeBuffer != NULL )
		free((void *)APipeBuffer);
	if( cls->m_AudioFileHandle != NULL )
		cls->m_pvr->ClosePvrFile(cls->m_AudioFileHandle);
		//CloseHandle(cls->m_AudioFileHandle);
	cls->m_AudioFileHandle = NULL;
	if( cls->m_VideoFileHandle != NULL )
		cls->m_pvr->ClosePvrFile(cls->m_VideoFileHandle);
		//CloseHandle(cls->m_VideoFileHandle);
	cls->m_VideoFileHandle = NULL;
	cls->m_InputThreadDone = true;
	cls->m_VideoPipe->PutPESFrame((void *)NULL);  //send the ending signal to the pipes
	cls->m_VideoPipe->PutPESFrame((void *)NULL);  //send the ending signal to the pipes
	cls->m_AudioPipe->PutPESFrame((void *)NULL);  //send the ending signal to the pipes
	cls->m_AudioPipe->PutPESFrame((void *)NULL);  //send the ending signal to the pipes
	dbgLog.PrintLnLog("Input thread exiting...");
	return(0);
}
#ifdef __MS__
DWORD WINAPI 
#endif
#ifdef __MAC__
void *
#endif
DePESVThreadProc(void *Param)
{
	pFRAME_INFO		VidFrame,tFrm;
	ExtractProgram	*cls;
	unsigned char	*Data;
	unsigned long	x;
	int64			OffsetPts,NextPts,origPts;
	char			FileName[300];
#ifdef __MS__
	HANDLE			m2vFileHandle;
#endif
#ifdef __MAC__
	int				m2vFileHandle;
#endif


	dbgLog.PrintLnLog("Video thread Starting...");
	cls = (ExtractProgram *)Param;

	/*VidFrame = (pFRAME_INFO)cls->m_VideoPipe->GetPESFrame();
	while(	VidFrame != NULL ){
		Data = (unsigned char *)VidFrame;
		free((void *)Data);
		VidFrame = (pFRAME_INFO)cls->m_VideoPipe->GetPESFrame();
	}
	if( VidFrame == NULL ){
		cls->m_VideoThreadDone = true;  //bail something is wrong
		return(0);
	}*/
	
	
	//first thing is to locate the starting frames
	cls->LocateStartingFrames();
	//now allow the audio thread to continue
#ifdef __MS__
	ReleaseSemaphore(cls->m_AudioSyncSem,1,NULL);
#endif
#ifdef __MAC__
	sem_post(cls->m_AudioSyncSem);
#endif
	if( cls->m_IsCanceled){
		cls->m_VideoThreadDone = true;
		dbgLog.PrintLnLog("Video thread exiting...");
		return(0);
	}
	//ok now we are ready to go...	
	//first off we need to fix up the first GOP
	cls->AdjustTemporalReference(0);
	cls->m_CurFrameCount = 0;
	VidFrame = (pFRAME_INFO)cls->m_VideoPipe->GetPESFrame();
	NextPts = VidFrame->PTS;
	
	//here we need to get the video information from the first I frame( this chould be one!)
	if( VidFrame != NULL ){
		Data = (unsigned char *)VidFrame;
		Data += sizeof(FRAME_INFO);
			
		cls->GetVideoInfo(&Data[VidFrame->HeaderOffset], (VidFrame->FrameLength - VidFrame->HeaderOffset));
	} else {
		dbgLog.PrintLnLog("Video thread exiting due to Null Packet!");
		cls->m_VideoThreadDone = true;  //bail something is wrong
		return(0);
	}
	//we need to drop the two B frames that follow the first frame
	x = 0;
	tFrm = (pFRAME_INFO)cls->m_VideoPipe->PeekPESFrame(x);
	x++;
	if( tFrm->PTS < NextPts ){
		Data = (unsigned char *)cls->m_VideoPipe->GetPESFrame();
		if( Data != NULL ){
			free((void *)Data);  //throw this frame away
		} else {
			dbgLog.PrintLnLog("Video thread exiting due to Null Packet!");
			cls->m_VideoThreadDone = true;  //bail something is wrong
			return(0);
		}
		x--;
	}
	tFrm = (pFRAME_INFO)cls->m_VideoPipe->PeekPESFrame(x);
	x++;
	if( tFrm->PTS < NextPts ){
		Data = (unsigned char *)cls->m_VideoPipe->GetPESFrame();
		if( Data != NULL ){
			free((void *)Data);  //throw this frame away
		} else {
			dbgLog.PrintLnLog("Video thread exiting due to Null Packet!");
			cls->m_VideoThreadDone = true;  //bail something is wrong
			return(0);
		}
		x--;
	}
	tFrm = (pFRAME_INFO)cls->m_VideoPipe->PeekPESFrame(x);
	while( tFrm != NULL && tFrm->frameType != 'I' ){
		cls->AdjustTemporalReference(x++);
		tFrm = (pFRAME_INFO)cls->m_VideoPipe->PeekPESFrame(x);
	}
	// now start	
	OffsetPts = VidFrame->PTS - 0x551B;
	NextPts = VidFrame->PTS;
	origPts = VidFrame->PTS;   //this is to see if we wrapped arround
	//if the video flag is set then create the m2v file
	if( cls->m_Streams->m_VideoChk )
	{

		strcpy(FileName,(char *)cls->m_Streams->m_DefaultPath);
		if( FileName[strlen(FileName)-1] == '/' || FileName[strlen(FileName)-1] == '\\')
			FileName[strlen(FileName)-1] = 0x00;
#ifdef __MS__
		strcat(FileName,"\\");
#else
		strcat(FileName,"/");
#endif
		strcat(FileName,cls->m_FileNameBase);
		strcat(FileName,".m2v");
#ifdef __MS__
		m2vFileHandle = CreateFile(FileName,GENERIC_READ | GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if(m2vFileHandle == INVALID_HANDLE_VALUE ) 
#endif
#ifdef __MAC__
		m2vFileHandle = open(FileName,O_CREAT | O_RDWR | O_TRUNC,0x777);
		if(m2vFileHandle < 0 ) 
#endif
		{
			cls->m_Streams->m_VideoChk = false; //if failed then forget it
			dbgLog.Sprintf("Output File: [ %s ] can not be Created.",FileName);
		} 
		else 
		{
			dbgLog.Sprintf("Output File: [ %s ] Created.",FileName);
		}
	}
	while( !cls->m_IsCanceled){
		if( VidFrame == NULL )
			break;
		if( VidFrame->frameType == 'I' ){
			// now we need to set the GOP Time Code
			cls->SetGOPTimeCode(VidFrame->PTS - (OffsetPts + 0x551B),VidFrame);
			if( cls->m_VideoX < 0 ){
				Data = (unsigned char *)VidFrame;
				Data += sizeof(FRAME_INFO);
				cls->GetVideoInfo(&Data[VidFrame->HeaderOffset], (VidFrame->FrameLength - VidFrame->HeaderOffset));
			}
		}
		if( VidFrame->frameType != 'I' &&  VidFrame->frameType != 'P' && VidFrame->frameType != 'B' ){
			//then something is wrong!
			dbgLog.PrintLnLog("Invalid Video frame!");
			Data = (unsigned char *)VidFrame;
			free((void *)Data);
			VidFrame = (pFRAME_INFO)cls->m_VideoPipe->GetPESFrame();
		}
		//ok now ship out the Frame
		Data = (unsigned char *)VidFrame;
		Data += sizeof(FRAME_INFO);
		if( VidFrame->PTS > 0 && VidFrame->PTS < origPts){
			//then the PTS has Wrapped arround!
			VidFrame->PTS += (int64)0x200000000;
		}
		cls->m_VideoRepack->RePESData(&Data[VidFrame->HeaderOffset], (VidFrame->FrameLength - VidFrame->HeaderOffset), VidFrame->PTS - OffsetPts, VidFrame->frameType);
		//now calculate the nextPTS
		//NextPts = VidFrame->PTS + 0xBBB;
		//if( cls->IsRFF(&Data[VidFrame->HeaderOffset], (VidFrame->FrameLength - VidFrame->HeaderOffset)) )
		//		NextPts += 0xBBB/2;
		//now if the Video flag is set then write out the frame to the file
		if( cls->m_Streams->m_VideoChk  )
		{
#ifdef __MS__
			WriteFile(m2vFileHandle,(void *)&Data[VidFrame->HeaderOffset],(VidFrame->FrameLength - VidFrame->HeaderOffset),&x,NULL);
#endif
#ifdef __MAC__
			x = write(m2vFileHandle,(void *)&Data[VidFrame->HeaderOffset],(VidFrame->FrameLength - VidFrame->HeaderOffset));
#endif
		}
		cls->m_CurFrameCount++;
		Data = (unsigned char *)VidFrame;
		free((void *)Data);
		VidFrame = (pFRAME_INFO)cls->m_VideoPipe->GetPESFrame();
	}
	//if the video flag is set then close the file
	if( cls->m_Streams->m_VideoChk ){
#ifdef __MS__
		if( m2vFileHandle != INVALID_HANDLE_VALUE)
			CloseHandle(m2vFileHandle);
#endif
#ifdef __MAC__
		if( m2vFileHandle > 0 )
			close(m2vFileHandle);
#endif
	}
	cls->m_VideoRepack->Flush();
//	while( !cls->m_VideoRepack->IsFlushed() && !cls->m_IsCanceled)
//#ifdef __MS__
//		Sleep(200);
//#endif
//#ifdef __MAC__
//		sleep(1);
//#endif
	cls->m_VideoThreadDone = true;
	dbgLog.PrintLnLog("Video thread exiting...");
	return(0);
}

#ifdef __MS__
DWORD WINAPI 
#endif
#ifdef __MAC__
void *
#endif
DePESAThreadProc(void *Param)
{
	pFRAME_INFO		AudFrame;
	ExtractProgram	*cls;
	unsigned char	*Data;
	unsigned long	x;
	int64			OffsetPts,NextPts,AudioPTSFrameRate,origPTS;
	char			FileName[300];
//	SAMPLE_SCAN		Scan;
	char			Line[128];
#ifdef __MS__
	HANDLE			mp2FileHandle;
#endif
#ifdef __MAC__
	int				mp2FileHandle;
#endif

	
	dbgLog.PrintLnLog("Audio thread Starting...");
	cls = (ExtractProgram *)Param;
	
/*	AudFrame = (pFRAME_INFO)cls->m_AudioPipe->GetPESFrame();
	while(	AudFrame != NULL ){
		Data = (unsigned char *)AudFrame;
		free((void *)Data);
		AudFrame = (pFRAME_INFO)cls->m_AudioPipe->GetPESFrame();
	}
	if( AudFrame == NULL ){
		cls->m_AudioThreadDone = true;  //bail something is wrong
		return(0);
	}*/
	
	// wait for the syncing semaphore to fire
#ifdef __MS__
	WaitForSingleObject(cls->m_AudioSyncSem,INFINITE);	//wait for them to be precessed	
	CloseHandle(cls->m_AudioSyncSem);
#endif
#ifdef __MAC__
	sem_wait(cls->m_AudioSyncSem);
	sem_close(cls->m_AudioSyncSem);
	sem_unlink("AudSync");
#endif
	if( cls->m_IsCanceled){
		cls->m_AudioThreadDone = true;
		dbgLog.PrintLnLog("Audio thread exiting...");
		return(0);
	}
	//ok now we are ready to go...	
	//get the audio Info

	AudioPTSFrameRate = (cls->m_Streams->m_AudioIsAC3 )? 0xB40 : 0x870;
	AudFrame = (pFRAME_INFO)cls->m_AudioPipe->GetPESFrame();
	OffsetPts = AudFrame->PTS - 0x551B;  //here we could adjust the audio to be perfect!!!!
	NextPts = AudFrame->PTS;
	origPTS = AudFrame->PTS;   //just in case we wrapp
	//if the audio flag is set then create the mp2 file
	if( cls->m_Streams->m_AudioChk ){
		strcpy(FileName,(char *)cls->m_Streams->m_DefaultPath);
		if( FileName[strlen(FileName)-1] == '/' || FileName[strlen(FileName)-1] == '\\')
			FileName[strlen(FileName)-1] = 0x00;
#ifdef __MS__
		strcat(FileName,"\\");
#else
		strcat(FileName,"/");
#endif
		strcat(FileName,cls->m_FileNameBase);
		//now check for AC3
		if(  cls->m_Streams->m_AudioIsAC3 ){
			strcat(FileName,".ac3");
		} else {
			strcat(FileName,".mp2");
		}
#ifdef __MS__
		mp2FileHandle = CreateFile(FileName,GENERIC_READ | GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if(mp2FileHandle == INVALID_HANDLE_VALUE ) 
#endif
#ifdef __MAC__
		mp2FileHandle = open(FileName,O_CREAT | O_RDWR | O_TRUNC,0x777);
		if(mp2FileHandle < 0 ) 
#endif
		{
			cls->m_Streams->m_AudioChk = false; //if failed then forget it
			sprintf(Line,"Output File: [ %s ] can not be Created.",FileName);
			OutPutDebugString(Line);
			//Log.PrintLnLog(Line);
		} 
		else 
		{
			sprintf(Line,"Output File: [ %s ] Created.",FileName);
			OutPutDebugString(Line);
			//Log.PrintLnLog(Line);
		}
	}
	while( !cls->m_IsCanceled){
		if( AudFrame == NULL )
			break;
		if( AudFrame->FrameLength < 0 || AudFrame->FrameLength > 65535 ){
			//then this fram is bogus!
			Data = (unsigned char *)AudFrame;
			free((void *)Data);
			AudFrame = (pFRAME_INFO)cls->m_AudioPipe->GetPESFrame();

		}
		//ok now ship out the Frame
		Data = (unsigned char *)AudFrame;
		Data += sizeof(FRAME_INFO);
		if( AudFrame->PTS > 0 && AudFrame->PTS < origPTS ){
			//then we have wrapped arround
			AudFrame->PTS += 0x200000000;
		}
		cls->m_AudioRepack->RePESData(&Data[AudFrame->HeaderOffset], (AudFrame->FrameLength - AudFrame->HeaderOffset), AudFrame->PTS - OffsetPts);
		//now calculate the nextPTS
		//cls->ScanForSamples(&Data[AudFrame->HeaderOffset], (AudFrame->FrameLength - AudFrame->HeaderOffset),&Scan);
		//NextPts = AudFrame->PTS + (Scan.NumSamples * AudioPTSFrameRate);
		//now if the Audio flag is set then write out the frame to the file
		if( cls->m_Streams->m_AudioChk ){
#ifdef __MS__
			WriteFile(mp2FileHandle,(void *)&Data[AudFrame->HeaderOffset],(AudFrame->FrameLength - AudFrame->HeaderOffset),&x,NULL);
#endif
#ifdef __MAC__
			x = write(mp2FileHandle,(void *)&Data[AudFrame->HeaderOffset],(AudFrame->FrameLength - AudFrame->HeaderOffset));
#endif
		}
		//test
		if( !cls->m_Streams->m_VideoChk && !cls->m_Streams->m_RenderChk ) {
			cls->m_CurFrameCount++;
		}
		Data = (unsigned char *)AudFrame;
		free((void *)Data);
		AudFrame = (pFRAME_INFO)cls->m_AudioPipe->GetPESFrame();
	}
	//if the audio flag is set then close the file
	if( cls->m_Streams->m_AudioChk ){
#ifdef __MS__
		if( mp2FileHandle != INVALID_HANDLE_VALUE)
			CloseHandle(mp2FileHandle);
#endif
#ifdef __MAC__
		if( mp2FileHandle > 0 )
			close(mp2FileHandle);
#endif
	}
	
	cls->m_AudioRepack->Flush();
//	while( !cls->m_AudioRepack->IsFlushed() && !cls->m_IsCanceled)
//#ifdef __MS__
//		Sleep(200);
//#endif
//#ifdef __MAC__
//		usleep(200000);
//#endif
	cls->m_AudioThreadDone = true;
	dbgLog.PrintLnLog("Audio thread exiting...");
	return(0);
}

#ifdef __MS__
DWORD WINAPI 
#endif
#ifdef __MAC__
void *
#endif
PackOutThreadProc(void *Param)
{
	ExtractProgram	*cls;
	int64			VideoPts,AudioPts,LastPts,LastAPts;
	int64			LastClockReference;
	unsigned char	*VBuffer,*ABuffer;
	unsigned long	AudioVideoDelay;
	PackOut			*Pack;
	char			FileName[256];
	char			RetryName[256];
	int				retry;
#ifdef __MS__
	HANDLE			mpgFileHandle = INVALID_HANDLE_VALUE;
#endif
#ifdef __MAC__
	int				mpgFileHandle = -1;
#endif
	
	cls = (ExtractProgram *)Param;
	dbgLog.PrintLnLog("Output thread starting...");

	
	Pack = (PackOut *)new PackOut(cls->m_Streams->m_AudioIsAC3);

	strcpy(FileName,(char *)cls->m_Streams->m_DefaultPath);
	if( FileName[strlen(FileName)-1] == '/' || FileName[strlen(FileName)-1] == '\\')
		FileName[strlen(FileName)-1] = 0x00;
#ifdef __MS__
	strcat(FileName,"\\");
#else
	strcat(FileName,"/");
#endif
	strcat(FileName,cls->m_FileNameBase);
	strcat(FileName,".mpg");
	retry = 0;
	while( cls->m_Streams->m_RenderChk ){
#ifdef __MS__
		mpgFileHandle = CreateFile(FileName,GENERIC_READ | GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
		if(mpgFileHandle == INVALID_HANDLE_VALUE ) 
#endif
#ifdef __MAC__
		mpgFileHandle = open(FileName,O_CREAT | O_RDWR | O_EXCL,0xFFF);
		if(mpgFileHandle < 0 ) 
#endif
		{
			dbgLog.Sprintf("Output File: [ %s ] can not be Created.",FileName);
			//so retry with a slightly different name
			strcpy(RetryName,(char *)cls->m_Streams->m_DefaultPath);
			if( RetryName[strlen(RetryName)-1] == '/' || RetryName[strlen(RetryName)-1] == '\\')
				RetryName[strlen(RetryName)-1] = 0x00;
#ifdef __MS__
			strcat(RetryName,"\\");
#else
			strcat(RetryName,"/");
#endif
			strcat(RetryName,cls->m_FileNameBase);
			sprintf(FileName,"%s(%d).mpg",RetryName,retry);
			retry++;
			if(retry >= 10 ){ //THE WE CAN JUST BAIL
				cls->Cancel();
				break;
			}
		} else {
			dbgLog.Sprintf("Output File: [ %s ] Created.",FileName);
			break;
		}
	}
	Pack->SetFileHandle(mpgFileHandle);  //set the file handle
	//let the pipes fill up some
#ifdef __MS__
		Sleep(1000);   //sleep 1s
#endif
#ifdef __MAC__
		sleep(1);
#endif

	//AudioVideoDelay = 0x2000;
	//AudioVideoDelay = 0;
	//set up the clock reference
	LastClockReference = 0x4B64 * 300;
	LastClockReference += ((int64)2048 * (int64)27000000) / Pack->GetMuxRate();
	//get the first Video and Audio and output them
	VideoPts = 0;
	VBuffer = cls->m_VideoRepack->GetNextPES();
	while( VBuffer == NULL && !cls->m_IsCanceled){
#ifdef __MS__
		Sleep(1000);   //sleep 1s
#endif
#ifdef __MAC__
		sleep(1);
#endif
		VBuffer = cls->m_VideoRepack->GetNextPES();
	}
	if( VBuffer != NULL ){
		Pack->OutPutPackHeader(LastClockReference);		//pack out the first headers
		LastClockReference += ((int64)2048 * (int64)27000000) / Pack->GetMuxRate();
		Pack->WriteCachedFile(VBuffer,BUCKET_SIZE);
		delete []VBuffer;
		VBuffer = cls->m_VideoRepack->GetNextPES();
		if( VBuffer != NULL )
			VideoPts = cls->GetPTS(VBuffer);
	}
	AudioPts = 0;
	ABuffer = cls->m_AudioRepack->GetNextPES( );
	while( ABuffer == NULL && !cls->m_IsCanceled){
#ifdef __MS__
		Sleep(1000);   //sleep 1s
#endif
#ifdef __MAC__
		sleep(1);
#endif
		ABuffer = cls->m_AudioRepack->GetNextPES( );
	}
	if( ABuffer != NULL ){
		Pack->OutPutPackHeader(LastClockReference);		//pack out the first headers
		LastClockReference += ((int64)2048 * (int64)27000000) / Pack->GetMuxRate();
		Pack->WriteCachedFile(ABuffer,BUCKET_SIZE);
		delete []ABuffer;
		ABuffer = cls->m_AudioRepack->GetNextPES( );
		if( ABuffer != NULL )
			AudioPts = cls->GetPTS(ABuffer);
	}
	while( !cls->m_IsCanceled && !(VBuffer == NULL && cls->m_VideoThreadDone)){
		if( VideoPts > 0 ){
			LastPts = VideoPts;
			AudioVideoDelay = cls->GetAudioMinPtsOffset(VBuffer);
			while( AudioPts > 0 && (AudioPts + AudioVideoDelay) < VideoPts ){
				if( AudioPts <= (( LastClockReference / 300) + 0x4000)){
					Pack->OutPutPackHeader(LastClockReference);		//pack out the first headers
					Pack->WriteCachedFile(ABuffer,BUCKET_SIZE);
					delete []ABuffer;
					LastAPts = AudioPts;
					AudioPts = 0;
					ABuffer = cls->m_AudioRepack->GetNextPES( );
					if( ABuffer != NULL ){
						AudioPts = cls->GetPTS(ABuffer);
						// now check to see if the pts is bogus!
						while( AudioPts > 0 && ((AudioPts - LastAPts) < 0 || (AudioPts - LastAPts) > 0x20000)){
							delete []ABuffer;
							ABuffer = cls->m_AudioRepack->GetNextPES( );
							AudioPts = 0;
							if( ABuffer != NULL ){
								AudioPts = cls->GetPTS(ABuffer);
							}
						}
					}
				}
				LastClockReference += ((int64)2048 * (int64)27000000) / Pack->GetMuxRate();
			}
		}
		if( VBuffer != NULL ){
			Pack->OutPutPackHeader(LastClockReference);		//pack out the first headers
			LastClockReference += ((int64)2048 * (int64)27000000) / Pack->GetMuxRate();
			Pack->WriteCachedFile(VBuffer,BUCKET_SIZE);
			delete []VBuffer;
		} else {
#ifdef __MS__
			Sleep(1);   //sleep 1ms
#endif
#ifdef __MAC__
			usleep(1000);
#endif
		}
		VBuffer = cls->m_VideoRepack->GetNextPES();
		VideoPts = 0;
		if( VBuffer != NULL )
			VideoPts = cls->GetPTS(VBuffer);
		if( ABuffer == NULL ){
			ABuffer = cls->m_AudioRepack->GetNextPES( );
			if( ABuffer != NULL )
				AudioPts = cls->GetPTS(ABuffer);
		}
	}
	while( !cls->m_IsCanceled && !(ABuffer == NULL && cls->m_AudioThreadDone)){	
		if( ABuffer != NULL )
			AudioPts = cls->GetPTS(ABuffer);
		if( AudioPts <= LastPts ){
			Pack->OutPutPackHeader(LastClockReference);		//pack out the first headers
			LastClockReference += ((int64)2048 * (int64)27000000) / Pack->GetMuxRate();
			Pack->WriteCachedFile(ABuffer,BUCKET_SIZE);
			delete []ABuffer;
			AudioPts = 0;
			ABuffer = cls->m_AudioRepack->GetNextPES( );
		} else {
			delete []ABuffer;  //throw it away
			AudioPts = 0;
			ABuffer = cls->m_AudioRepack->GetNextPES( );
		}
	}
		
	Pack->FlushCache();
#ifdef __MS__
	if( mpgFileHandle != INVALID_HANDLE_VALUE )
		CloseHandle(mpgFileHandle);
#endif
#ifdef __MAC__
	if( mpgFileHandle > 0 )
		close(mpgFileHandle);
#endif
	delete Pack;
	cls->m_OutputThreadDone = true;
	dbgLog.PrintLnLog("Output thread exiting...");
	return(0);
}

#ifdef __MS__
DWORD WINAPI 
#endif
#ifdef __MAC__
void *
#endif
StubPackOutThreadProc(void *Param)
{
	ExtractProgram	*cls;
	unsigned char	*Buffer;
	
	cls = (ExtractProgram *)Param;
	dbgLog.PrintLnLog("Output thread starting...");
	Buffer = NULL;
	while( !cls->m_IsCanceled ){
		Buffer = cls->m_AudioRepack->GetNextPES( );
		if( Buffer != NULL ){  //then we have a packet
			delete []Buffer;
		} else {
			if( cls->m_AudioThreadDone){
				break;
			}
#ifdef __MS__
			Sleep(1);   //sleep 1ms
#endif
#ifdef __MAC__
			usleep(1000);
#endif
		}
	}
	cls->m_OutputThreadDone = true;
	dbgLog.PrintLnLog("Output thread exiting...");
	return(0);
}



//this function will determine the muxed audio offset depending on the frame type of the video
//normally this should be determined by the B frames that preceed the I or P frame, but in this case 
//we do not know this and have no good way of getting it so assume this amount

unsigned long ExtractProgram::GetAudioMinPtsOffset(unsigned char *Buffer)
{
	unsigned long rc;
	int				j;
	unsigned char c;

	rc = 0;
	j = 0;
	while( j < 2034 ){
		if( *(unsigned long *)&Buffer[j] == PICTURE_HEADER_SIG ){
			c = ((Buffer[j + 5] >> 3) & 0x07); 
			if( c == 1 || c == 2)
				rc = 0x2000;
			break;
		}
		j++;
	}
	return(rc);
}
