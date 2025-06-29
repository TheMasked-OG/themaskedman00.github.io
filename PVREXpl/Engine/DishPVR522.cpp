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

// DishPVR522.cpp: implementation of the DishPVR50X class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DishPVR522.h"
#include "522Catalog.h"	

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DishPVR522::DishPVR522()
{
	m_CatalogData = NULL;
}

DishPVR522::~DishPVR522()
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
DishPVR522::GetNumberOfPrograms()
{

	HANDLE			FileHandle;
	unsigned long	CatalogFileSize;
	pCATALOG_522	p522cat;
	//int				j,k;


	m_NumberOfPrograms = 0;
	FileHandle = OpenPvrFile( "0/catalog.cat");
	if( FileHandle == NULL ){
		OutPutDebugString("Unable to open Catalog.cat file!");
		return(0);
	}
	CatalogFileSize = (unsigned long)GetPvrFileSize(FileHandle);
	m_CatalogData = (unsigned char *)new unsigned char[CatalogFileSize];
	if( m_CatalogData == NULL ){
		OutPutDebugString("Error Allocating Catalog buffer File!");
		ClosePvrFile(FileHandle);
		return(0);
	}
	ReadPvrFile(FileHandle,m_CatalogData,CatalogFileSize);
	ClosePvrFile(FileHandle);
	p522cat = (pCATALOG_522)m_CatalogData;
			
	m_NumberOfPrograms = Cvts(p522cat->NumberOfElements);
	//for( j = 0; j < m_NumberOfPrograms; j++){
	//	k = GetIndexFromIndex(j);
	//	p522cat->Program[k].DateTime = 0;
	//}
	
	return(m_NumberOfPrograms);
}

//------------------------------------------
// this function will return the desired information about the program
bool 
DishPVR522::GetProgramInfo(char *ProgramName, char *TimeDate, char *Duration, char *ChannelName, int *Channel, char *Description, int ProgramIndex)
{

	pCATALOG_522	p522;
	unsigned long	t;
//	unsigned short  s;
	int				h,m,year,month,day;
	char			ampm[8];
	
	if( ProgramIndex >= m_NumberOfPrograms){
		OutPutDebugString("Invalid Program Number!");
		return(false);
	}
	ProgramIndex = GetIndexFromIndex(ProgramIndex);
	p522 = (pCATALOG_522)m_CatalogData;
	if( ProgramName != NULL ){
		strcpy(ProgramName,p522->Program[ProgramIndex].ProgramName);
	}
	if( TimeDate != NULL ){
		t = Cvt(p522->Program[ProgramIndex].DateTime);
		m = t & 0x000000FF;
		h = (t >> 8 ) & 0x1F;
		day = (t >> 16) & 0x1F;
		month = (t >> 21 ) & 0x1F;
		year = ( t >> 26 ) & 0x3F;
		year += 2000;
		if( h >= 12)
			strcpy(ampm,"pm");
		else
			strcpy(ampm,"am");
		if( h > 12 )
			h -= 12;
		if( h == 0 )
			h = 12;
		sprintf(TimeDate,"%02d/%02d/%d  %d:%02d%s",month,day,year,h,m,ampm);

	}
	if( Duration != NULL ){
		t = Cvts(p522->Program[ProgramIndex].Unknown4);
		h = t / 60;
		m = t % 60;
		sprintf(Duration,"%d:%02d",h,m);
	}
	if( ChannelName != NULL ){
		strcpy(ChannelName,p522->Program[ProgramIndex].ChannelName);
	}
	if( Channel != NULL ){
		*Channel = Cvts(p522->Program[ProgramIndex].CHannelNumber);
	}
	if( Description != NULL ){
		strcpy(Description,p522->Program[ProgramIndex].ProgramDesc);
	}
	return(true);
}

//-------------------------------------------
//   this function will get the audio options like AC3, normal
int 
DishPVR522::GetAudioOptions(int ProgramIndex)
{

	HANDLE		FileHandle;
	unsigned char *Buffer;
	int			BufferSize;
	int			Options;
	int			j,k,lp;
	int			tei,psti,tp,pid,tsc,cc,afc,afl;
	
	//this is more interesting because we should open the TS stream
	//then scann the stream for a while looking for audio PID options
	FileHandle = OpenVideoFile( ProgramIndex );
	if( FileHandle == NULL )
		return(0);
	BufferSize = 188 * 2048;
	Buffer = (unsigned char *)malloc(BufferSize);
	if( Buffer == NULL ){
		ClosePvrFile(FileHandle);
		return(0);
	}
	Options = 0;
	while( Options == 0 ){  //loop untill we find something that makes sense
		lp = ReadPvrFile(FileHandle,Buffer,BufferSize);
		if( lp == 0 )
			break;  //then at the end of the file
		j = 0;
		while( (j + 188) <= BufferSize ){
			//find the packet header...
			while( j < BufferSize && Buffer[j] != 0x47)
				j++;
			if( j >= BufferSize ) // then we did not find one
				break;
			// if we get here then we might have a ts packet
			k = j + 1;
			tei = ( Buffer[k] & 0x80 )? 1:0;
			psti = ( Buffer[k] & 0x40 )? 1:0;
			tp = ( Buffer[k] & 0x20 )? 1:0;
			pid = Buffer[k++] & 0x1F;
			pid <<= 8;
			pid |= Buffer[k++];
			tsc = (Buffer[k] >> 6);
			cc = (Buffer[k] & 0x0F);
			afc = ((Buffer[k++] & 0x30) >> 4);
			afl = 0;
			if( afc == 2 || afc == 3 ){
					afl = Buffer[k++];
					k += afl;
			}
			//we now have all the position info from the header now verify the header
			if( tsc != 0 || afc == 0 || tei != 0 ){
				//then asume bad
				j++;
				continue;
			}
			// ok the packet is valid, then process it
			if( psti > 0 ){
				//then we have a PES packet header look at it!
				if( *(unsigned long *)&Buffer[k] == PES_VIDEO_SIGNATURE ){
					//then this is the Video PID
					Options |= HAS_VIDEO;
				}
				if( *(unsigned long *)&Buffer[k] == PES_AUDIO_SIGNATURE ){
					//then this is the regular Audio PID
					if( (pid & 0x00FF) == 0x23 )  //then audio english
						Options |= HAS_AUDIO_ENG;
					else
						Options |= HAS_AUDIO_ALT;
				}
				if( *(unsigned long *)&Buffer[k] == PES_AC3AUDIO_SIGNATURE ){
					//then this is the AC3 PID
					Options |= HAS_AUDIO_AC3;
				}
			} 
			j += 188;
		}
	}
	ClosePvrFile(FileHandle);
	free(Buffer);
	return(Options);
}

//-------------------------------------------
//   this function will tell if the Audio and video are seperate
bool 
DishPVR522::IsSingleTransport()
{
	return(true);

}

//-------------------------------------------
//   this function will Open the video stream, or TS stream file for both
HANDLE 
DishPVR522::OpenVideoFile(int ProgramIndex)
{
	pCATALOG_522	p522;
	char			FileName[128];
	HANDLE			FileHandle;

	
	if( ProgramIndex >= m_NumberOfPrograms){
		OutPutDebugString("Invalid Program Number!");
		return(NULL);
	}
	ProgramIndex = GetIndexFromIndex(ProgramIndex);
	p522 = (pCATALOG_522)m_CatalogData;
	sprintf(FileName,"2/es%d.tsp",Cvts(p522->Program[ProgramIndex].TsFileNameID));
	FileHandle = OpenPvrFile(FileName);
	return(FileHandle);
}

//-------------------------------------------
//	This function will open the audio file with the options listed
HANDLE 
DishPVR522::OpenAudioFile(int ProgramIndex, int Options)
{
	//sorry this is a single trqansport stream device
	return(NULL);
}

//////////////////////////////////////////////////////////////////////
// Private Methods
//////////////////////////////////////////////////////////////////////
//---------------------------------------------------
// endien conversion
unsigned long	
DishPVR522::Cvt( unsigned long x)
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
DishPVR522::Cvts( unsigned short x)
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


bool 
DishPVR522::IsEntryValid(int AbsIndex)
{
	pCATALOG_522	p522cat;
	unsigned char	BitByte;
	unsigned char	shift,mask;

	if( m_CatalogData == NULL )
		return(false);
	p522cat = (pCATALOG_522)m_CatalogData;
	
	BitByte = p522cat->IndexBitMap[AbsIndex / 8];
	shift = AbsIndex % 8;
	shift = 7 - shift;
	mask = 0x01;
	if( shift > 0 )
		mask <<= shift;
	if( BitByte & mask )
		return(false);
	return(true);
}

int DishPVR522::GetIndexFromIndex(int Index)
{
	pCATALOG_522	p522cat;
	int				AbsIndex,j;

	p522cat = (pCATALOG_522)m_CatalogData;
	if( m_CatalogData == NULL )
		return(-1);
	AbsIndex = 0;
	for(j = 0; j < 1000; j++){  //looks like there is 1000 possible entries
		if( IsEntryValid(j) ){
			if( Index == AbsIndex)
				break;
			AbsIndex++;
		}
	}
	return(j);
}
