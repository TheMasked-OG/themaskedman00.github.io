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

// DishPVR622.h: interface for the DishPVR622 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DISHPVR622__
#define __DISHPVR622__

//----------------------------- headers ------------------------
#include "Platform.h"
#include "GlobalDefs.h"
#include "DishPvr.h"
#include "LogFile.h"
#include "DishPVR50X.h"
//----------------------------- definitions --------------------

#ifdef __MS__
#pragma pack( push )
#pragma pack(1)
#endif
#ifdef __MAC__
#pragma pack(1)
#endif

//----------------------Structures-------------------------
typedef struct {
	char	FileName[64];
	char	ProgramName[64];
	char	Duration[32];
	char	DateTime[32];
	int		ChannelNumber;
	char	ChannelName[16];
	char	Description[256];
} CATALOG_ENTRY_622, *p622_CATALOG_ENTRY;


//---------------------------------------------------------

#ifdef __MS__
#pragma pack( pop )
#endif
#ifdef __MAC__
#pragma pack()
#endif

//------------------------------- classes -----------------------

class DishPVR622 : public DishPvr  
{
public:
	HANDLE OpenAudioFile( int ProgramIndex, int Options );
	HANDLE OpenVideoFile( int ProgramIndex);
	bool IsSingleTransport();
	int GetAudioOptions( int ProgramIndex);
	bool GetProgramInfo( char *Name, char *DateTime, char *Duration, char *ChannelName, int *Channel, char *Description, int ProgramIndex);
	int GetNumberOfPrograms();
	DishPVR622();
	virtual ~DishPVR622();

private:
	unsigned short	Cvts( unsigned short x);
	unsigned long	Cvt( unsigned long x);
	
	unsigned char	*m_DirData;
	unsigned char	*m_CatalogData;
	char			m_ls[512][64];
	int				m_NumberOfPrograms;
	int				m_NumberOfDirEntry;
};

#endif // __DISHPVR622__
