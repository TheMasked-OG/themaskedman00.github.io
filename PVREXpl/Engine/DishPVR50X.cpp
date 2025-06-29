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

// DishPVR50X.cpp: implementation of the DishPVR50X class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DishPVR50X.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DishPVR50X::DishPVR50X()
{
	m_CatalogData = NULL;
	m_NumberOfPrograms = 0;
}

DishPVR50X::~DishPVR50X()
{
	if( m_CatalogData != NULL )
		delete []m_CatalogData;
	m_CatalogData = NULL;
}

//////////////////////////////////////////////////////////////////////
// Public Methods
//////////////////////////////////////////////////////////////////////

//------------------------------------------
//  this function will get the number of programs available
int 
DishPVR50X::GetNumberOfPrograms()
{
	HANDLE			FileHandle;
	unsigned long	CatFileSize;
	pCATALOG_HEADER	catPtr;

	if( m_CatalogData == NULL ){  //if we do not have the catalog.cat file in memory then get it
		FileHandle = OpenPvrFile("EstarData/Catalog.cat");	
		if( FileHandle == NULL ){
			OutPutDebugString("Unable to open Catalog.cat file!");
			return(0);  //the we have failed
		}
		CatFileSize = (unsigned long)GetPvrFileSize(FileHandle);
		if( CatFileSize == 0 ){
			OutPutDebugString("Catalog.cat file size = 0!");
			ClosePvrFile(FileHandle);
			return(0);  //failed
		}
		m_CatalogData = (unsigned char *)new unsigned char[CatFileSize]; //allocate for the data
		if( m_CatalogData == NULL ){
			OutPutDebugString("Error Allocating Catalog.cat file buffer!");
			ClosePvrFile(FileHandle);
			return(0);  //failed
		}
		ReadPvrFile(FileHandle,m_CatalogData,CatFileSize);
		ClosePvrFile(FileHandle);
		
		catPtr = (pCATALOG_HEADER)m_CatalogData;
		m_NumberOfPrograms = Cvts(catPtr->NumberOfFileEntries);
	}
	
	
	return(m_NumberOfPrograms);

}

//------------------------------------------
// this function will return the desired information about the program
bool 
DishPVR50X::GetProgramInfo(char *ProgramName, char *TimeDate, char *Duration, char *ChannelName, int *Channel, char *Description, int ProgramIndex)
{
	pCATALOG_STRUCT	catPtr;
	unsigned long	durationH;
	unsigned long	durationM;
	int				j;
	char			Session[56];
	char			*PartPtr;
	HANDLE			FileHandle;
	unsigned long	FileSize;
	unsigned char	*Buffer;
	pSESSION_HEADER		sHeadPtr;
	pSESSION_PROG_DATA	sProgData;
	pSESSION_FILE_ENTRY sEntryData;
	Rating				rating;


	catPtr = (pCATALOG_STRUCT)(m_CatalogData+4);
	if( ProgramIndex >= m_NumberOfPrograms ){
		OutPutDebugString("Invalid Program Number!");
		return(false);
	}
	catPtr += ProgramIndex;
	if( ProgramName != NULL )
	{
		strcpy(ProgramName,catPtr->ListName);
		strcat(ProgramName,". {");
		//*(unsigned long *)&rating = Cvt(*(unsigned long *)&catPtr->dwRating);
		rating = catPtr->dwRating;
		if (rating.ratingNR)
			sprintf(ProgramName+strlen(ProgramName)," %s }", "NR");
		if (rating.ratingG)
			sprintf(ProgramName+strlen(ProgramName)," %s }", "G");
		if (rating.ratingPG)
			sprintf(ProgramName+strlen(ProgramName)," %s }", "PG");
		if (rating.ratingPG13)
			sprintf(ProgramName+strlen(ProgramName)," %s }", "PG-13");
		if (rating.ratingR)
			sprintf(ProgramName+strlen(ProgramName)," %s }", "R");
		if (rating.ratingNC17)
			sprintf(ProgramName+strlen(ProgramName)," %s }", "NC-17");
		if (rating.ratingNRAO)
			sprintf(ProgramName+strlen(ProgramName)," %s }", "NR/AO");
		if (rating.ratingXXX)
			sprintf(ProgramName+strlen(ProgramName),"%s }", "XXX");
		if (rating.fratingV+rating.fratingL+rating.fratingN+rating.fratingSC)
		{
			strcat(ProgramName,"; {");
			if (rating.fratingV)
				strcat(ProgramName,"V,");
			if (rating.fratingL)
				strcat(ProgramName,"L,");
			if (rating.fratingN)
				strcat(ProgramName,"N,");
			if (rating.fratingSC)
				strcat(ProgramName,"SC");
			strcat(ProgramName,"}");
		}
	}
	///if( SesionName != NULL )
	//{
	//	strcpy(SesionName,catPtr->SesionPath);
	//}
	if( TimeDate != NULL )
	{
		durationH = catPtr->bHour;
		if( catPtr->bHour < 12 ){
			sprintf(TimeDate,"%d/%d/%d : %d:%02dam", catPtr->bMonth,catPtr->bDay,Cvts(catPtr->wYear),catPtr->bHour,catPtr->bMinute);
		} else {
			if( durationH > 12 )
				durationH -= 12;
			sprintf(TimeDate,"%d/%d/%d : %d:%02dpm", catPtr->bMonth,catPtr->bDay,Cvts(catPtr->wYear),durationH,catPtr->bMinute);
		}
	}
	if( Duration != NULL )
	{
		durationH = Cvt(catPtr->dwDurationM)/60;
		durationM = Cvt(catPtr->dwDurationM)%60;
		sprintf(Duration,"%d:%02d",durationH,durationM);
	}
	if( ChannelName == NULL && Channel == NULL && Description == NULL )
		return(true); //no need to read from disk...
	
	strcpy(Session,catPtr->SesionPath);
	
	//get the information from the session file
	j = 0;
	PartPtr = (char *)&Session[j];
	if( Session[0] == '/' )
		j++;
	PartPtr = (char *)&Session[j];
	FileHandle = OpenPvrFile( PartPtr );
	if( FileHandle == NULL )
		return(false);
	FileSize = (unsigned long)GetPvrFileSize(FileHandle);
	Buffer = (unsigned char *)new unsigned char[FileSize];
	if( Buffer == NULL ){
		OutPutDebugString("Error Allocating Session file buffer!");
		ClosePvrFile(FileHandle);
		return(false);
	}
	ReadPvrFile(FileHandle,Buffer,FileSize);
	ClosePvrFile(FileHandle);
	
	sHeadPtr = (pSESSION_HEADER)Buffer;
	sEntryData =(pSESSION_FILE_ENTRY)(Buffer+sizeof(PVR_SESSION_HEADER));

	j = sizeof(PVR_SESSION_HEADER) + (Cvts(sHeadPtr->NumberOfFileEntries) * sizeof(SESSION_FILE_ENTRY));
	j += 2+44*Buffer[j]; // skip all Unkn entries and a counter
	//int numProg = (unsigned short)Buffer[ j];
	j +=2;
	sProgData = (pSESSION_PROG_DATA)&Buffer[ j]; // point to FIRST PROGRAM ! ( could be more !)

	if( ChannelName != NULL ){
		strcpy(ChannelName,sProgData->Channel);
	}
	if( Channel != NULL ){
		*Channel = Cvt(sProgData->ChanNum);
	}
	if( Description != NULL ){
		if (sProgData->Description[0] == 0)
			strcpy(Description,"<no description>");
		else
			strcpy(Description,sProgData->Description);
	}
	delete []Buffer;
	return(true);
}

//-------------------------------------------
//   this function will get the audio options like AC3, normal
int 
DishPVR50X::GetAudioOptions(int ProgramIndex)
{
	int					j,i;
	char				*PartPtr;
	HANDLE				FileHandle;
	unsigned char		*Buffer;
	unsigned long		FileSize;
	pSESSION_HEADER		sHeadPtr;
	pSESSION_FILE_ENTRY sEntryData;
	int					rc;
	char				Session[56];
	pCATALOG_STRUCT		catPtr;

	catPtr = (pCATALOG_STRUCT)(m_CatalogData+4);
	if( ProgramIndex >= m_NumberOfPrograms ){
		OutPutDebugString("Invalid Program Number!");
		return(false);
	}
	catPtr += ProgramIndex;
	strcpy(Session,catPtr->SesionPath);

	rc = 0;
	j = 0;
	PartPtr = (char *)&Session[j];
	if( Session[0] == '/' )
		j++;
	PartPtr = (char *)&Session[j];

	FileHandle = OpenPvrFile(PartPtr);
	if( FileHandle == NULL ){
		OutPutDebugString("Error Opening session file!");
		return(rc);
	}
	FileSize = (unsigned long)GetPvrFileSize(FileHandle);
	Buffer = (unsigned char *)new unsigned char[FileSize];
	if( Buffer == NULL ){
		OutPutDebugString("Error Allocating session file buffer!");
		ClosePvrFile(FileHandle);
		return(rc);
	}
	ReadPvrFile(FileHandle,Buffer,FileSize);
	ClosePvrFile(FileHandle);
	sHeadPtr = (pSESSION_HEADER)Buffer;
	sEntryData =(pSESSION_FILE_ENTRY)(Buffer+sizeof(PVR_SESSION_HEADER));
	for (i=0; i< Cvts(sHeadPtr->NumberOfFileEntries);i++)
	{
		switch(Cvt(sEntryData[i].FileType))
		{
			case(1): //Video
				rc |= HAS_VIDEO;
				break;
			case(2): // Audio
				if( rc & HAS_AUDIO_ENG ) //if this is the second audio file then there is an alternate
					rc |= HAS_AUDIO_ALT;
				else
					rc |= HAS_AUDIO_ENG;
				break;
			case(3): // AC-3
				rc |= HAS_AUDIO_AC3;
				break;
			case(0x00010000): // Index
				rc |= HAS_INDEX;
				break;
			case(0x00020000): // CD title
				rc |= HAS_CD;
				break;
			default:
				break;
		}
	}
	delete []Buffer;
	return(rc);
}

//-------------------------------------------
//   this function will tell if the Audio and video are seperate
bool 
DishPVR50X::IsSingleTransport()
{
	return(false);

}

//-------------------------------------------
//   this function will Open the video stream, or TS stream file for both
HANDLE 
DishPVR50X::OpenVideoFile(int ProgramIndex)
{

	char				Session[56];
	pCATALOG_STRUCT		catPtr;
	int					options;
	int					j;
	char				*PartPtr;
	HANDLE				FileHandle;
	unsigned char		*Buffer;
	unsigned long		FileSize;
	pSESSION_HEADER		sHeadPtr;
	pSESSION_FILE_ENTRY sEntryData;
	
	options = GetAudioOptions(ProgramIndex);
	if( !(options & HAS_VIDEO) ){
		OutPutDebugString("No Video File Noted!");
		return(NULL);	
	}
	catPtr = (pCATALOG_STRUCT)(m_CatalogData+4);
	if( ProgramIndex >= m_NumberOfPrograms ){
		OutPutDebugString("Invalid Program Number!");
		return(NULL);
	}
	catPtr += ProgramIndex;
	strcpy(Session,catPtr->SesionPath);
	j = 0;
	if( Session[0] == '/' )
		j++;
	PartPtr = (char *)&Session[j];
	
	FileHandle = OpenPvrFile(PartPtr);
	if( FileHandle == NULL ){
		OutPutDebugString("Error Opening session File!");
		return(NULL);
	}
	FileSize = (unsigned long)GetPvrFileSize(FileHandle);
	Buffer = (unsigned char *)new unsigned char[FileSize];
	if( Buffer == NULL ){
		OutPutDebugString("Error Allocating session file buffer");
		ClosePvrFile(FileHandle);
		return(NULL);
	}
	ReadPvrFile(FileHandle,Buffer,FileSize);
	ClosePvrFile(FileHandle);
	sHeadPtr = (pSESSION_HEADER)Buffer;
	sEntryData =(pSESSION_FILE_ENTRY)(Buffer+sizeof(PVR_SESSION_HEADER));
	
	FileHandle = OpenPvrFile(sEntryData->FileName);
	delete []Buffer;
	return(FileHandle);

}

//-------------------------------------------
//	This function will open the audio file with the options listed
HANDLE 
DishPVR50X::OpenAudioFile(int ProgramIndex, int Options)
{
	int					j,i;
	char				*PartPtr;
	HANDLE				FileHandle;
	unsigned char		*Buffer;
	unsigned long		FileSize;
	pSESSION_HEADER		sHeadPtr;
	pSESSION_FILE_ENTRY sEntryData;
	char				Session[56];
	pCATALOG_STRUCT		catPtr;
	HANDLE				rc;
	char				FileName[128];
	bool				HasAlternate;

	rc = NULL;
	catPtr = (pCATALOG_STRUCT)(m_CatalogData+4);
	if( ProgramIndex >= m_NumberOfPrograms ){
		OutPutDebugString("Invalid Program Number!");
		return(rc);
	}
	catPtr += ProgramIndex;
	strcpy(Session,catPtr->SesionPath);

	j = 0;
	PartPtr = (char *)&Session[j];
	if( Session[0] == '/' )
		j++;
	PartPtr = (char *)&Session[j];

	FileHandle = OpenPvrFile(PartPtr);
	if( FileHandle == NULL ){
		OutPutDebugString("Error opening Session file!");
		return(rc);
	}
	FileSize = (unsigned long)GetPvrFileSize(FileHandle);
	Buffer = (unsigned char *)new unsigned char[FileSize];
	if( Buffer == NULL ){
		OutPutDebugString("Error Allocating Session file Buffer");
		ClosePvrFile(FileHandle);
		return(rc);
	}
	ReadPvrFile(FileHandle,Buffer,FileSize);
	ClosePvrFile(FileHandle);
	sHeadPtr = (pSESSION_HEADER)Buffer;
	sEntryData =(pSESSION_FILE_ENTRY)(Buffer+sizeof(PVR_SESSION_HEADER));
	FileName[0] = 0x00;
	FileHandle = NULL;
	HasAlternate = false;
	for (i=0; i< Cvts(sHeadPtr->NumberOfFileEntries);i++)
	{
		if( (Options & HAS_AUDIO_ENG) && (Cvt(sEntryData[i].FileType) == 2)){
			strcpy(FileName,sEntryData[i].FileName);
			break;
		}
		if( (Options & HAS_AUDIO_AC3) && (Cvt(sEntryData[i].FileType) == 3)){
			strcpy(FileName,sEntryData[i].FileName);
			break;
		}
		if( (Options & HAS_AUDIO_ALT) && (Cvt(sEntryData[i].FileType) == 2) && HasAlternate){
			strcpy(FileName,sEntryData[i].FileName);  
			break;
		}
		if( Cvt(sEntryData[i].FileType) == 2 || Cvt(sEntryData[i].FileType) == 3)
			HasAlternate = true;  //bugbug if the alternate audio comes before AC3 then we will miss it
	}
	if( strlen(FileName) )
		FileHandle = OpenPvrFile(FileName);
	if( FileHandle != NULL )
		AdjustAudioFileSize(FileHandle);
	delete []Buffer;
	return(FileHandle);
}

//////////////////////////////////////////////////////////////////////
// Private Methods
//////////////////////////////////////////////////////////////////////
//---------------------------------------------------
// endien conversion
unsigned long	
DishPVR50X::Cvt( unsigned long x)
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
//---------------------------------------------------
// endien conversion
unsigned short	
DishPVR50X::Cvts( unsigned short x)
{
#ifdef __x86__
	return(x);
#endif
#ifdef __PwPC__
	unsigned short rc;
	unsigned char cPtr[5];
	unsigned char t;
	
	*(unsigned short *)&cPtr[0] = x;
	t = cPtr[0];
	cPtr[0] = cPtr[1];
	cPtr[1] = t;
	rc = *(unsigned short *)&cPtr[0];
	return(rc);
#endif
}
