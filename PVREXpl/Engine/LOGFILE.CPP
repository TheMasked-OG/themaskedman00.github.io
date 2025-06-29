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

// LogFile.cpp: implementation of the LogFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogFile.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LogFile::LogFile()
{
#ifdef __MS__
	m_FileHandle = INVALID_HANDLE_VALUE;
#endif
#ifdef __MAC__
	m_FileHandle = -1;
#endif
}

LogFile::~LogFile()
{
#ifdef __MS__
	if( m_FileHandle != INVALID_HANDLE_VALUE )
		CloseLogFile();
	m_FileHandle = INVALID_HANDLE_VALUE;
#endif
#ifdef __MAC__
	if( m_FileHandle > 0 )
		CloseLogFile();
	m_FileHandle = -1;
#endif
}

bool LogFile::OpenLogFile(char *LogFileName, int AppendOverWrite)
{
	
	if( AppendOverWrite == LOG_FILE_NEW )
	{
#ifdef __MS__
		m_FileHandle = CreateFile(LogFileName,GENERIC_READ | GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
#endif
#ifdef __MAC__
	m_FileHandle = open(LogFileName,O_CREAT | O_RDWR | O_TRUNC,0xFFF);
#endif
	}
	if( AppendOverWrite == LOG_FILE_APPEND )
	{
#ifdef __MS__
		m_FileHandle = CreateFile(LogFileName,GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if( m_FileHandle != INVALID_HANDLE_VALUE )
		{
			SetFilePointer(m_FileHandle,0,NULL,FILE_END);
#endif
#ifdef __MAC__
		m_FileHandle = open(LogFileName,O_APPEND | O_RDWR,0x777);
		if( m_FileHandle > 0 )
		{
#endif
			PrintLnLog("Appending...\r\n");
		}
	}
#ifdef __MS__
	if( m_FileHandle == INVALID_HANDLE_VALUE)
#endif
#ifdef __MAC__
	if( m_FileHandle < 0)
#endif
		return(false);
	return(true);
}

void LogFile::CloseLogFile()
{
#ifdef __MS__
	if( m_FileHandle != INVALID_HANDLE_VALUE )
		CloseHandle(m_FileHandle);
	m_FileHandle = INVALID_HANDLE_VALUE;
#endif
#ifdef __MAC__
	if( m_FileHandle > 0 )
		close(m_FileHandle);
	m_FileHandle = -1;
#endif

}

void LogFile::PrintLog(char *String)
{
	unsigned long	x;

	if( !IsLogFileOpen())
		return;
#ifdef __MS__
	WriteFile(m_FileHandle,(void *)String,lstrlen(String),&x,NULL);
#endif
#ifdef __MAC__
	x = write(m_FileHandle,(void *)String,strlen(String));
#endif
}

void LogFile::PrintLnLog(char *String)
{
	if( !IsLogFileOpen())
		return;
	PrintLog(String);
	PrintLog("\r\n");
}

void LogFile::Sprintf(char *str1, char *str2)
{
	char	line[256];

	if( !IsLogFileOpen())
		return;
	sprintf(line,str1,str2);
	PrintLnLog(line);

}

void LogFile::Sprintf(char *str1, int x)
{
	char	line[256];

	if( !IsLogFileOpen())
		return;
	sprintf(line,str1,x);
	PrintLnLog(line);
}

void LogFile::Sprintf(char *str1, int x, int y)
{
	char	line[256];

	if( !IsLogFileOpen())
		return;
	sprintf(line,str1,x,y);
	PrintLnLog(line);
}

void LogFile::Sprintf(char *str1, int x, int y, int z)
{
	char	line[256];

	if( !IsLogFileOpen())
		return;
	sprintf(line,str1,x,y,z);
	PrintLnLog(line);
}
void LogFile::Sprintf(char *str1, unsigned long x)
{
	char	line[256];

	if( !IsLogFileOpen())
		return;
	sprintf(line,str1,x);
	PrintLnLog(line);
}

void LogFile::Sprintf(char *str1, unsigned long x, unsigned long y)
{
	char	line[256];

	if( !IsLogFileOpen())
		return;
	sprintf(line,str1,x,y);
	PrintLnLog(line);
}

void LogFile::Sprintf(char *str1, unsigned long x, unsigned long y, unsigned long z)
{
	char	line[256];

	if( !IsLogFileOpen())
		return;
	sprintf(line,str1,x,y,z);
	PrintLnLog(line);
}

void LogFile::PrintData(unsigned char *Data, int length)
{
	char	line[256];
	char	tmp[16];
	int		j,k;

	if( !IsLogFileOpen())
		return;
	for(j = 0; j < length / 16; j++){
		sprintf(line,"%08X: ",(j * 16));
		for( k = 0; k < 16 && ((j * 16) + k) < length; k++){
			sprintf(tmp,"%02X, ",Data[(j * 16) + k]);
			strcat(line,tmp);
		}
		for( k = 0; k < 16 && ((j * 16) + k) < length; k++){
			if( isprint(Data[(j * 16) + k]))
				sprintf(tmp,"%c",Data[(j * 16) + k]);
			else
				strcpy(tmp,".");
			strcat(line,tmp);
		}
		PrintLnLog(line);
	}

}

void LogFile::PrintData(unsigned long *Data, int length)
{
	char	line[256];
	char	tmp[32];
	int		j,k;

	if( !IsLogFileOpen())
		return;
	for(j = 0; j < length / 4; j++){
		sprintf(line,"%08X: ",(j * 16));
		for( k = 0; k < 4 && ((j * 4) + k) < length; k++){
			sprintf(tmp,"%08X, ",Data[(j * 4) + k]);
			strcat(line,tmp);
		}
		PrintLnLog(line);
	}
}

bool LogFile::IsLogFileOpen()
{
	bool	rc;

	rc = false;
#ifdef __MS__
	if( m_FileHandle != INVALID_HANDLE_VALUE )
		rc = true;
#endif
#ifdef __MAC__
	if( m_FileHandle > 0 )
		rc = true;
#endif
	return(rc);
}
