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

// DishPVRFile.cpp: implementation of the DishPVRFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DishPVRFile.h"

#ifdef __MAC__
#include <sys/dirent.h>
#include <dirent.h>
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DishPVRFile::DishPVRFile()
{

}

DishPVRFile::~DishPVRFile()
{

}

//////////////////////////////////////////////////////////////////////
// Public Methods
//////////////////////////////////////////////////////////////////////
HANDLE 
DishPVRFile::OpenAudioFile( int ProgramIndex, int Options )
{
	return(NULL); //assuming tsp files only for now
}

HANDLE 
DishPVRFile::OpenVideoFile( int ProgramIndex)
{
	if( ProgramIndex >= m_NumFilesFound)
		return(NULL);
	return( OpenPvrFile(&m_FileName[ProgramIndex][0]));
}

bool 
DishPVRFile::IsSingleTransport()
{
	return(true);  //assume stream file
}

int 
DishPVRFile::GetAudioOptions( int ProgramIndex)
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


bool 
DishPVRFile::GetProgramInfo( char *Name, char *DateTime, char *Duration, char *ChannelName, int *Channel, char *Description, int ProgramIndex)
{
	if( ProgramIndex >= m_NumFilesFound){
		OutPutDebugString("Invalid Program Number!");
		return(false);
	}
	if( Name != NULL ){
		strcpy(Name,&m_FileName[ProgramIndex][0]);
	}
	if( DateTime != NULL ){
		strcpy(DateTime," ");
	}
	if( Duration != NULL ){
		strcpy(Duration," ");
	}
	if( ChannelName != NULL ){
		strcpy(ChannelName,"File");
	}
	if( Channel != NULL ){
		*Channel = 0;
	}
	if( Description != NULL ){
		strcpy(Description,"Local File");
	}
	return(true);
}


int 
DishPVRFile::GetNumberOfPrograms()
{

#ifdef __MAC__
	DIR				*FindDir;
	struct dirent	*dr;
#endif

	m_NumFilesFound = 0;	

#ifdef __MS__
	char			Path[256];
	HANDLE			FindHandle;
	WIN32_FIND_DATA info;

	
	strcpy(Path,m_FileRootPath);
	if( Path[strlen(Path) - 1] != '\\')
		strcat(Path,"\\");
	strcat(Path,"*.tsp");
	FindHandle = FindFirstFile(Path,&info);
	while( FindHandle != INVALID_HANDLE_VALUE ){
		strcpy(&m_FileName[m_NumFilesFound][0],info.cFileName);
		m_NumFilesFound++;
		if( !FindNextFile(FindHandle,&info)){
			FindClose(FindHandle);
			break;
		}
	}
#endif
#ifdef __MAC__
	FindDir = opendir((char *)m_FileRootPath);
	if( FindDir == NULL )
		return(m_NumFilesFound);
	dr = readdir(FindDir);
	if( dr == NULL ){
			//some read error
		closedir(FindDir);
		return(m_NumFilesFound);
	}
	while( dr != NULL) {
		if( dr->d_fileno != 0 ){
			if( DT_REG == dr->d_type ){
				if( dr->d_namlen > 4 && !stricmp(&dr->d_name[ strlen(dr->d_name) - 4],".tsp")){
					strcpy(&m_FileName[m_NumFilesFound][0],dr->d_name);
					m_NumFilesFound++;
				}
			}
		}
		dr = readdir(FindDir);
	}
	closedir(FindDir);
#endif
	return(m_NumFilesFound);
}

//////////////////////////////////////////////////////////////////////
// Private Methods
//////////////////////////////////////////////////////////////////////
