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

// DishPVR721.h: interface for the DishPVR50X class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DISHPVR721__
#define __DISHPVR721__

//----------------------------- headers ------------------------
#include "Platform.h"
#include "GlobalDefs.h"
#include "DishPvr.h"
#include "LogFile.h"

//----------------------------- definitions --------------------

#ifdef __MS__
#pragma pack( push )
#pragma pack(1)
#endif
#ifdef __MAC__
#pragma pack(1)
#endif


typedef struct tagPTVDataHeader {
	int		SlotCount;
	int		NA1;
	int		NA2;
	int		NA3;
	int		NA4;
	int		NA5;
} PTVDataHeader;

typedef struct tagPTVDataIndex {
	int		Index;
	int		NA1;
	char	Directory[100];
} PTVDataIndex;

typedef struct tagPTVDataRecording721 {
	// total size 2156
	char	Directory[100];
	char	NA1[8];
	int		Date1;
	int		NA2[6];
	int		Start;
	int		End;
	int		NA3[7];
	char	NA4[2];
	char	Title[64];
	char	ChannelName[26];
	int		Channel;
	char	NA5[25];
	char	Episode[400];
	char	NA0[1463];
} PTVDataRecording721;

typedef struct tagPTVDataRecording921 {
	// total size 2156
	char	Directory[100];
	char	NA1[10];
	int		Date1;
	int		NA2[6];
	int		Start;
	int		End;
	int		NA3[8];
	char	NA4[2];
	char	Title[64];
	char	ChannelName[26];
	int		Channel;
	char	NA5[28];
	char	Episode[400];
	char	NA0[1452];
} PTVDataRecording921;


typedef struct tagPTVDataRecording {
	// total size 2156
	char	Directory[100];
	int		Date1;
	int		Start;
	int		End;
	char	Title[64];
	char	ChannelName[26];
	int		Channel;
	char	Episode[400];
} PTVDataRecording;

typedef struct tagPTVDataRecordingBuff {
	// total size 2156
	char	buff[2156];
} PTVDataRecordingBuff;

#ifdef __MS__
#pragma pack( pop )
#endif
#ifdef __MAC__
#pragma pack()
#endif

//------------------------------- classes -----------------------

class DishPVR721 : public DishPvr  
{
public:
	HANDLE OpenAudioFile( int ProgramIndex, int Options );
	HANDLE OpenVideoFile( int ProgramIndex);
	bool IsSingleTransport();
	int GetAudioOptions( int ProgramIndex);
	bool GetProgramInfo( char *Name, char *DateTime, char *Duration, char *ChannelName, int *Channel, char *Description, int ProgramIndex);
	int GetNumberOfPrograms();
	DishPVR721();
	virtual ~DishPVR721();
private:
	unsigned long	Cvt( unsigned long x);
	PTVDataRecording* GetPVRRecording(void* PVRBuff, PTVDataRecording* PVR);

	unsigned char	*m_CatalogData;
	int				m_NumberOfPrograms;

};

#endif // __DISHPVR721__
