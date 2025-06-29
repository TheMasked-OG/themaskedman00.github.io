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

// DishPVR721.cpp: implementation of the DishPVR50X class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DishPVR721.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DishPVR721::DishPVR721()
{

}

DishPVR721::~DishPVR721()
{

}

//////////////////////////////////////////////////////////////////////
// Public Methods
//////////////////////////////////////////////////////////////////////

//------------------------------------------
//  this function will get the number of programs available
int 
DishPVR721::GetNumberOfPrograms()
{

	PTVDataHeader	*pPTVHeader;
	PTVDataIndex	*pPTVIndex;
	HANDLE			FileHandle;
	unsigned long	CatalogFileSize;
	unsigned long	Slot;


	m_NumberOfPrograms = 0;
	FileHandle = OpenPvrFile( "PTVData.dat");
	if( FileHandle == NULL ){
		OutPutDebugString("Unable to open PTVData.dat file!");
		return(0);
	}
	CatalogFileSize = (unsigned long)GetPvrFileSize(FileHandle);
	m_CatalogData = (unsigned char *)new unsigned char[CatalogFileSize];
	if( m_CatalogData == NULL ){
		OutPutDebugString("Unable to Allocate PTVData.dat file buffer!");
		ClosePvrFile(FileHandle);
		return(0);
	}
	ReadPvrFile(FileHandle,m_CatalogData,CatalogFileSize);
	ClosePvrFile(FileHandle);

	pPTVHeader = (PTVDataHeader *)m_CatalogData;
	pPTVIndex = (PTVDataIndex *)&m_CatalogData[sizeof(PTVDataHeader)];
	for( Slot = 0; Slot < Cvt(pPTVHeader->SlotCount); Slot++){
		if( strlen(pPTVIndex->Directory) != 0 ){
			m_NumberOfPrograms++;
		}
		pPTVIndex++;
	}
	return(m_NumberOfPrograms);
}

//------------------------------------------
// this function will return the desired information about the program
bool 
DishPVR721::GetProgramInfo(char *ProgramName, char *TimeDate, char *Duration, char *ChannelName, int *Channel, char *Description, int ProgramIndex)
{

	PTVDataHeader		*pPTVHeader;
	PTVDataIndex		*pPTVIndex;
	PTVDataRecording721	*pData;
	PTVDataRecording	PTVData;	
	int					Slot,cnt;
	time_t				t;
    struct tm			*ltime;
	unsigned long		durationH;
	unsigned long		durationM;
	unsigned long		durationS;

	
	pPTVHeader = (PTVDataHeader *)m_CatalogData;
	pPTVIndex = (PTVDataIndex *)&m_CatalogData[sizeof(PTVDataHeader)];
	pData = (PTVDataRecording721 *)&m_CatalogData[ sizeof(PTVDataHeader) + (sizeof(PTVDataIndex) * Cvt(pPTVHeader->SlotCount))];
	cnt = 0;
	for( Slot = 0; Slot < (int)Cvt(pPTVHeader->SlotCount); Slot++){
		if( strlen(pPTVIndex->Directory) != 0 ){
			if( cnt == ProgramIndex ){ // then we have fount the wanted name
				break;
			}
			cnt++;
		}
		pPTVIndex++;
		pData++;
	}
	if( Slot < (int)Cvt(pPTVHeader->SlotCount) ){ // then found it
		GetPVRRecording((void *)pData,&PTVData);
		// now format this thing
		if( ProgramName != NULL )
			strcpy(ProgramName,PTVData.Title);
		if( TimeDate != NULL ){
			t = PTVData.Date1;
			ltime = localtime(&t);
			sprintf(TimeDate,"%d/%02d/%d %d:%02d",ltime->tm_mon,ltime->tm_mday,ltime->tm_year + 1900,ltime->tm_hour,ltime->tm_min);
		}
		if( Duration != NULL ){
			durationS = (PTVData.End - PTVData.Start);
			durationH = durationS / 3600;
			durationS -= (durationH * 3600);
			durationM = durationS / 60;
			durationS -= (durationM * 60);
			sprintf(Duration,"%d:%02d",durationH,durationM);
		}
		if( ChannelName != NULL ){
			strcpy(ChannelName,PTVData.ChannelName);
		}
		if( Channel != NULL ){
			*Channel = 	PTVData.Channel;
		}
		if( Description != NULL ){
			strcpy(Description,PTVData.Episode);
		}
	} else {
		OutPutDebugString("Unable to Locate program Slot");
		return(false);
	}
	return(true);
}

//-------------------------------------------
//   this function will get the audio options like AC3, normal
int 
DishPVR721::GetAudioOptions(int ProgramIndex)
{

	//this is more interesting because we should open the TS stream
	//then scann the stream for a while looking for audio PID options
	
	// for now just say it is english
	return(HAS_VIDEO | HAS_AUDIO_ENG);
}

//-------------------------------------------
//   this function will tell if the Audio and video are seperate
bool 
DishPVR721::IsSingleTransport()
{
	return(true);

}

//-------------------------------------------
//   this function will Open the video stream, or TS stream file for both
HANDLE 
DishPVR721::OpenVideoFile(int ProgramIndex)
{
	PTVDataHeader		*pPTVHeader;
	PTVDataIndex		*pPTVIndex;
	PTVDataRecording721	*pData;
	PTVDataRecording	PTVData;	
	int					Slot,cnt;
	char				FileName[128];
	HANDLE				FileHandle;

	pPTVHeader = (PTVDataHeader *)m_CatalogData;
	pPTVIndex = (PTVDataIndex *)&m_CatalogData[sizeof(PTVDataHeader)];
	pData = (PTVDataRecording721 *)&m_CatalogData[ sizeof(PTVDataHeader) + (sizeof(PTVDataIndex) * Cvt(pPTVHeader->SlotCount))];
	cnt = 0;
	for( Slot = 0; Slot < (int)Cvt(pPTVHeader->SlotCount); Slot++){
		if( strlen(pPTVIndex->Directory) != 0 ){
			if( cnt == ProgramIndex ){ // then we have fount the wanted name
				break;
			}
			cnt++;
		}
		pPTVIndex++;
		pData++;
	}
	FileHandle = NULL;
	if( Slot < (int)Cvt(pPTVHeader->SlotCount) ){ // then found it
		GetPVRRecording((void *)pData,&PTVData);
		sprintf(FileName,"%s/REC_TRANSPORT_DATA.DAT",PTVData.Directory);
		FileHandle = OpenPvrFile(FileName);
	
	} else {
		OutPutDebugString("Unable to Locate program Slot");
		return(NULL);
	}
	return(FileHandle);
}

//-------------------------------------------
//	This function will open the audio file with the options listed
HANDLE 
DishPVR721::OpenAudioFile(int ProgramIndex, int Options)
{
	//sorry this is a single trqansport stream device
	return(NULL);
}

//////////////////////////////////////////////////////////////////////
// Private Methods
//////////////////////////////////////////////////////////////////////

PTVDataRecording* 
DishPVR721::GetPVRRecording(void* PVRBuff, PTVDataRecording* PVR)
{
	PTVDataRecording721* PVR7 = (PTVDataRecording721*) PVRBuff;
	PTVDataRecording921* PVR9 = (PTVDataRecording921*) PVRBuff;

	if((Cvt(PVR7->Date1) > 1000000000) && (Cvt(PVR7->Date1) < 2000000000))
	{
		strcpy(PVR->Directory, PVR7->Directory);
		strcpy(PVR->Title, PVR7->Title);
		strcpy(PVR->ChannelName, PVR7->ChannelName);
		strcpy(PVR->Episode, PVR7->Episode);
		PVR->Date1 = Cvt(PVR7->Date1);
		PVR->Start = Cvt(PVR7->Start);
		PVR->End = Cvt(PVR7->End);
		PVR->Channel = Cvt(PVR7->Channel);
	}
	if((Cvt(PVR9->Date1) > 1000000000) && (Cvt(PVR9->Date1) < 2000000000))
	{
		strcpy(PVR->Directory, PVR9->Directory);
		strcpy(PVR->Title, PVR9->Title);
		strcpy(PVR->ChannelName, PVR9->ChannelName);
		strcpy(PVR->Episode, PVR9->Episode);
		PVR->Date1 = Cvt(PVR9->Date1);
		PVR->Start = Cvt(PVR9->Start);
		PVR->End = Cvt(PVR9->End);
		PVR->Channel = Cvt(PVR9->Channel);
	}

	return PVR;
}

//---------------------------------------------------
// endien conversion
unsigned long	
DishPVR721::Cvt( unsigned long x)
{
#ifdef __x86__
	return(x);
#endif
#ifdef __PwPC__
	unsigned long rc;
	unsigned char cPtr[5];
	unsigned char t;
	
	*(unsigned long *)&cPtr[0] = x;
	t = cPtr[0];
	cPtr[0] = cPtr[3];
	cPtr[3] = t;
	t = cPtr[1];
	cPtr[1] = cPtr[2];
	cPtr[2] = t;
	rc = *(unsigned long *)&cPtr[0];
	return(rc);
#endif
}
