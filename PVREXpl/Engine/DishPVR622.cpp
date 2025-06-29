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

// DishPVR622.cpp: implementation of the DishPVR622 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DishPVR622.h"
#include "HDD50X.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DishPVR622::DishPVR622()
{
	m_CatalogData = NULL;
	m_DirData = NULL;
}

DishPVR622::~DishPVR622()
{
	if( m_CatalogData != NULL )
		delete []m_CatalogData;
	m_CatalogData = NULL;
	if( m_DirData != NULL )
		delete []m_DirData;
	m_DirData = NULL;
}

//////////////////////////////////////////////////////////////////////
// Public Methods
//////////////////////////////////////////////////////////////////////

//------------------------------------------
//  this function will get the number of programs available
int 
DishPVR622::GetNumberOfPrograms()
{
	
	HANDLE			FileHandle;
	pMYDIR_FILE_ENTRY	pEntry;
	int				rc,j,len;
	unsigned char	*Buffer;
	char			FileName[360];
	p622_CATALOG_ENTRY cEntry;
	


	FileHandle = OpenPvrFile("AV_REQ_HD/FOLDER_DIR.DIR");
	rc = 0;
	if( FileHandle != NULL ){
		rc = (int)GetPvrFileSize(FileHandle);
		m_DirData = (unsigned char *)new char[rc];
		ReadPvrFile(FileHandle,m_DirData,rc);
		rc = rc / sizeof(MYDIR_FILE_ENTRY);
		m_NumberOfDirEntry = rc;
		ClosePvrFile(FileHandle);
		// here we could find the number of sets...
		pEntry = (pMYDIR_FILE_ENTRY)m_DirData;
		m_NumberOfPrograms = 0;
		for( j = 0; j < m_NumberOfDirEntry; j++){
			len = strlen(pEntry[j].FileName);
			if( !stricmp(&pEntry[j].FileName[len-3],".BM")){
				//then we have an entry
				if( !strncmp(pEntry[j].FileName,"delay_",6))
					continue;
				m_NumberOfPrograms++;
			}
		}
		// now allocate the storage and get the information
		m_CatalogData = (unsigned char *)new char[sizeof(CATALOG_ENTRY_622) * m_NumberOfPrograms]; 
		Buffer = (unsigned char *)new char[2048];
		rc = 0;
		cEntry = (p622_CATALOG_ENTRY)m_CatalogData;
		for( j = 0; j < m_NumberOfDirEntry; j++){
			len = strlen(pEntry[j].FileName);
			if( !stricmp(&pEntry[j].FileName[len-3],".BM")){
				if( !strncmp(pEntry[j].FileName,"delay_",6))
					continue;
				strcpy(FileName,"AV_REQ_HD/");
				strcat(FileName,pEntry[j].FileName);
				FileHandle = OpenPvrFile(FileName);
				if( FileHandle != NULL ){
					len = ReadPvrFile(FileHandle,Buffer,1024);
					strcpy(cEntry[rc].FileName,FileName);
					strcpy(cEntry[rc].ProgramName,(char *)&Buffer[0xAA]);
					cEntry[rc].ChannelNumber = Cvts(*(unsigned short *)&Buffer[0xCE]);
					strcpy(cEntry[rc].ChannelName,(char *)&Buffer[0xEC]);
					strcpy(cEntry[rc].Description,(char *)&Buffer[0x108]);
					
					strcpy(cEntry[rc].Duration,"??????????");
					strcpy(cEntry[rc].DateTime,"??????????");
					rc++;
					ClosePvrFile(FileHandle);
				}
			}

		}
		rc = m_NumberOfPrograms;
		//then return the number of real programs
	}
	return(rc);

}

//------------------------------------------
// this function will return the desired information about the program
bool 
DishPVR622::GetProgramInfo(char *ProgramName, char *TimeDate, char *Duration, char *ChannelName, int *Channel, char *Description, int ProgramIndex)
{

	p622_CATALOG_ENTRY cEntry;
	

	cEntry = (p622_CATALOG_ENTRY)m_CatalogData;
	if( ProgramName != NULL )
	{
		strcpy(ProgramName,cEntry[ProgramIndex].ProgramName);
	}
	if( TimeDate != NULL )
	{
		strcpy(TimeDate,cEntry[ProgramIndex].DateTime);
	}
	if( Duration != NULL )
	{
		strcpy(Duration,cEntry[ProgramIndex].Duration);
	}
	if( ChannelName != NULL ){
		strcpy(ChannelName,cEntry[ProgramIndex].ChannelName);
	}
	if( Channel != NULL ){
		*Channel = cEntry[ProgramIndex].ChannelNumber;
	}
	if( Description != NULL ){
		strcpy(Description,cEntry[ProgramIndex].Description);
	}
	return(true);
}

//-------------------------------------------
//   this function will get the audio options like AC3, normal
int 
DishPVR622::GetAudioOptions(int ProgramIndex)
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
DishPVR622::IsSingleTransport()
{
	return(true);

}

//-------------------------------------------
//   this function will Open the video stream, or TS stream file for both
HANDLE 
DishPVR622::OpenVideoFile(int ProgramIndex)
{

	char				FileName[128];
	HANDLE				rc;
	int					len;
	p622_CATALOG_ENTRY cEntry;
	

	cEntry = (p622_CATALOG_ENTRY)m_CatalogData;
	strcpy(FileName,cEntry[ProgramIndex].FileName);
	len = strlen(FileName);
	FileName[len-3] = 0x00;
	strcat(FileName,".tsp");

	rc = OpenPvrFile(FileName);

	return(rc);

}

//-------------------------------------------
//	This function will open the audio file with the options listed
HANDLE 
DishPVR622::OpenAudioFile(int ProgramIndex, int Options)
{
	return(NULL);
}

//////////////////////////////////////////////////////////////////////
// Private Methods
//////////////////////////////////////////////////////////////////////
//---------------------------------------------------
// endien conversion
unsigned long	
DishPVR622::Cvt( unsigned long x)
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
DishPVR622::Cvts( unsigned short x)
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
