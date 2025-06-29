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

// DishPVR50X.h: interface for the DishPVR50X class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DISHPVR50X__
#define __DISHPVR50X__

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

typedef struct {
	unsigned char	MajorVer;
	unsigned char	MinorVer;
	unsigned short	NumberOfFileEntries;
} CATALOG_HEADER, *pCATALOG_HEADER;

typedef struct Rating
{
#ifdef __MS__
   unsigned long ratingNR  : 1;  // 00000000 00000000 00000000 0000000?
   unsigned long ratingG   : 1;  // 00000000 00000000 00000000 000000?0
   unsigned long ratingPG  : 1;  // 00000000 00000000 00000000 00000?00
   unsigned long ratingPG13: 1;  // 00000000 00000000 00000000 0000?000
   unsigned long ratingR   : 1;  // 00000000 00000000 00000000 000?0000
   unsigned long ratingNC17: 1;  // 00000000 00000000 00000000 00?00000
   unsigned long ratingNRAO: 1;  // 00000000 00000000 00000000 0?000000
   unsigned long ratingXXX : 1;  // 00000000 00000000 00000000 ?0000000
   unsigned long rsv1      : 8;  // 00000000 00000000 ???????? 00000000
   unsigned long fratingV  : 1;  // 00000000 0000000? 00000000 00000000
   unsigned long fratingL  : 1;  // 00000000 000000?0 00000000 00000000
   unsigned long fratingN  : 1;  // 00000000 00000?00 00000000 00000000
   unsigned long fratingSC : 1;  // 00000000 0000?000 00000000 00000000
   unsigned long fratingRSV: 4;  // 00000000 ????0000 00000000 00000000
   unsigned long rsv2      : 8;  // ???????? 00000000 00000000 00000000
#endif
#ifdef __MAC__
   unsigned long ratingXXX : 1;  // 00000000 00000000 00000000 ?0000000
   unsigned long ratingNRAO: 1;  // 00000000 00000000 00000000 0?000000
   unsigned long ratingNC17: 1;  // 00000000 00000000 00000000 00?00000
   unsigned long ratingR   : 1;  // 00000000 00000000 00000000 000?0000
   unsigned long ratingPG13: 1;  // 00000000 00000000 00000000 0000?000
   unsigned long ratingPG  : 1;  // 00000000 00000000 00000000 00000?00
   unsigned long ratingG   : 1;  // 00000000 00000000 00000000 000000?0
   unsigned long ratingNR  : 1;  // 00000000 00000000 00000000 0000000?

   unsigned long rsv1      : 8;  // 00000000 00000000 ???????? 00000000

   unsigned long fratingRSV: 4;  // 00000000 ????0000 00000000 00000000
   unsigned long fratingSC : 1;  // 00000000 0000?000 00000000 00000000
   unsigned long fratingN  : 1;  // 00000000 00000?00 00000000 00000000
   unsigned long fratingL  : 1;  // 00000000 000000?0 00000000 00000000
   unsigned long fratingV  : 1;  // 00000000 0000000? 00000000 00000000

   unsigned long rsv2      : 8;  // ???????? 00000000 00000000 00000000

#endif
} sRating; 

typedef struct {
//	UCHAR		Unknown[56];

		unsigned long	dwStamp;
		unsigned long	dwProvider;
		unsigned long	dwDurationM;
		unsigned char	bMonth; // start date/time stamp
		unsigned char	bDay;
		unsigned short	wYear;
		unsigned char	bWDay;
		unsigned char	bHour;
		unsigned char	bMinute;
		unsigned char	bSeconds;
		unsigned char	bEMonth; // end date/time stamp
		unsigned char	bEDay;
		unsigned short	wEYear;
		unsigned char	bEWDay;
		unsigned char	bEHour;
		unsigned char	bEMinute;
		unsigned char	bESeconds;
		unsigned long	dwUnkn2; // ?
		unsigned long	dwUnkn3; // ?
		unsigned long	dwChannel;
		sRating	dwRating;
		unsigned long	dwUnkn4; // ?
		unsigned long	dwUnkn5; // same as dwDurationM
		char			ListName[37];
		char			SesionPath[47];
		unsigned long	dwDurationS;
		unsigned long	Unknown2[3];
} CATALOG_STRUCT, *pCATALOG_STRUCT;

typedef struct {
	unsigned char		MajorVer;
	unsigned char		MinorVer;
	unsigned short		NumberOfFileEntries;
} PVR_SESSION_HEADER, *pSESSION_HEADER;

#define		FILE_TYPE_VIDEO		1
#define     FILE_TYPE_AUDIO		2
#define		FILE_TYPE_INDEX		3

typedef struct {
	char			FileName[32];
	unsigned long	FileType;
	char			Lang[4];
	unsigned long	Unknown[2];
	unsigned long	FileID;
	unsigned long	Unknown2;
} SESSION_FILE_ENTRY, *pSESSION_FILE_ENTRY;

typedef struct {
	unsigned long	unkn[4];
	unsigned long	ChanNum;
	unsigned long	unkn1;
	char			ProgramTitle[37];
	char			Channel[7];
	unsigned long	unkn2[2];
	char			Description[256];
	unsigned char	unkn3[0x7c];
	unsigned char	pid_hdr[0x2c];
	unsigned char	pid[5][0x1c];
	unsigned char	unkn4[26];
} SESSION_PROG_DATA, *pSESSION_PROG_DATA;

typedef struct {
	unsigned char	Unknown[29];
	unsigned char	PossibleHeaderType;
	unsigned char	PossibleFrameSecType;
	unsigned char	Unknown1;
	unsigned long	SecondsCounter;
	unsigned long	FrameDataSize;
	unsigned long	PossibleSomething;
	unsigned char	Unknown2;
	unsigned char	PossibleIncSomthing;
	char			FrameType;
	unsigned char	PossibleFrameIndicator;
	unsigned long	PossibleIFrameCounter;
	unsigned long	SomeSignature;
	unsigned long	Unknown3;
	unsigned long	VideoFileOffset;
	unsigned char	unknown4[5];
	unsigned long	SomeFrameCounter;
	unsigned long	unknown5;
	unsigned char	PossibleTypeField;
	unsigned short	unknown6;
	unsigned char	unknown7[48];
} INDEX_HEADER, *pINDEX_HEADER;

#ifdef __MS__
#pragma pack( pop )
#endif
#ifdef __MAC__
#pragma pack()
#endif

//------------------------------- classes -----------------------

class DishPVR50X : public DishPvr  
{
public:
	HANDLE OpenAudioFile( int ProgramIndex, int Options );
	HANDLE OpenVideoFile( int ProgramIndex);
	bool IsSingleTransport();
	int GetAudioOptions( int ProgramIndex);
	bool GetProgramInfo( char *Name, char *DateTime, char *Duration, char *ChannelName, int *Channel, char *Description, int ProgramIndex);
	int GetNumberOfPrograms();
	DishPVR50X();
	virtual ~DishPVR50X();

private:
	unsigned short	Cvts( unsigned short x);
	unsigned long	Cvt( unsigned long x);
	
	unsigned char	*m_CatalogData;
	int				m_NumberOfPrograms;
};

#endif // __DISHPVR50X__
