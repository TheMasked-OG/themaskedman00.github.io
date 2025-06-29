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

// DishPVRFile.h: interface for the DishPVRFile class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DISHPVRFILE__
#define __DISHPVRFILE__

//----------------------------- headers ------------------------
#include "Platform.h"
#include "GlobalDefs.h"
#include "DishPvr.h"
#include "LogFile.h"

//----------------------------- definitions --------------------


//------------------------------- classes -----------------------

class DishPVRFile : public DishPvr  
{
public:
	HANDLE OpenAudioFile( int ProgramIndex, int Options );
	HANDLE OpenVideoFile( int ProgramIndex);
	bool IsSingleTransport();
	int GetAudioOptions( int ProgramIndex);
	bool GetProgramInfo( char *Name, char *DateTime, char *Duration, char *ChannelName, int *Channel, char *Description, int ProgramIndex);
	int GetNumberOfPrograms();
	DishPVRFile();
	virtual ~DishPVRFile();
private:
	char m_FileName[128][256];
	int m_NumFilesFound;

};

#endif // __DISHPVRFILE__

