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

// LogFile.h: interface for the LogFile class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __LOGFILE__
#define __LOGFILE__

//----------------------------- headers ------------------------
#include "Platform.h"
#include "GlobalDefs.h"


//----------------------------- definitions --------------------
#define		LOG_FILE_NEW	1
#define		LOG_FILE_APPEND	2

// just to test everything out...
#define     OutPutDebugString printf
//------------------------------- classes -----------------------
class LogFile  
{
public:
	bool IsLogFileOpen();
	void PrintData(unsigned long *Date, int length);
	void PrintData( unsigned char *Date, int length);
	void Sprintf(char *str1, unsigned long x, unsigned long y, unsigned long z);
	void Sprintf(char *str1, unsigned long x, unsigned long y);
	void Sprintf(char *str1, int x, int y, int z);
	void Sprintf( char *str1, int x, int y);
	void Sprintf( char *str1, unsigned long x);
	void Sprintf( char *str1, int x);
	void Sprintf( char *str1, char *str2);
	void PrintLnLog( char *String);
	void PrintLog( char *String );
	void CloseLogFile();
	bool OpenLogFile( char *LogFileName, int AppendOverWrite );
	LogFile();
	virtual ~LogFile();

private:
#ifdef __MS__
	HANDLE	m_FileHandle;
#endif
#ifdef __MAC__
	int		m_FileHandle;
#endif
};

#endif // __LOGFILE__
