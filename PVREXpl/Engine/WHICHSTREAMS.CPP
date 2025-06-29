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

// WhichStreams.cpp : implementation file
//

#include "stdafx.h"
#include "WhichStreams.h"

//----------- globals ----------------
#ifdef __MAC__
extern char			gbResPath[256];
extern char			gbUser[256];
#endif


WhichStreams::WhichStreams()
{
	LoadOptions();
}
WhichStreams::~WhichStreams()
{

}

/////////////////////////////////////////////////////////////////////////////
// WhichStreams message handlers


void WhichStreams::LoadOptions()
{
	unsigned long	Settings;
#ifdef __MS__
	long			rc;
	HKEY			key;
	unsigned long	type;
	unsigned long	Size;

	Settings = 0x8C;
	//lstrcpy(m_DefaultPath,"C:\\MyDocuments
	rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\DishExtract\\Options",0,KEY_READ | KEY_WRITE,&key);
	if( rc == ERROR_SUCCESS ){
		Size = 4;
		rc = RegQueryValueEx(key,"Settings",NULL,&type,(UCHAR *)&Settings,&Size);
		//read in the def path
		Size = 360;
		rc = RegQueryValueEx(key,"DefaultPath",NULL,&type,(UCHAR *)m_DefaultPath,&Size);
		//if error then create the path
		if( rc != ERROR_SUCCESS ){
			SHGetSpecialFolderPath(GetTopWindow(NULL),(char *)m_DefaultPath,CSIDL_PERSONAL,FALSE);
		}
		Size = 64;
		rc = RegQueryValueEx(key,"AutoFormat",NULL,&type,(UCHAR *)m_AutoFormat,&Size);
		if( rc != ERROR_SUCCESS ){
			strcpy(m_AutoFormat,"%n(%i)");
		}
		RegCloseKey(key);
	} else {
		rc = RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\DishExtract\\Options",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&key,&type);
		if( rc == ERROR_SUCCESS ){
			rc = RegSetValueEx(key,"Settings",0,REG_DWORD,(UCHAR *)&Settings,sizeof(ULONG));
			if( SHGetSpecialFolderPath(GetTopWindow(NULL),(char *)m_DefaultPath,CSIDL_PERSONAL,FALSE)){
				lstrcat((char *)m_DefaultPath,"\\My Videos");
				RegSetValueEx(key,"DefaultPath",0,REG_SZ,(UCHAR *)m_DefaultPath,lstrlen((char *)m_DefaultPath));
				strcpy(m_AutoFormat,"%n(%i)");
				RegSetValueEx(key,"AutoFormat",0,REG_SZ,(UCHAR *)m_AutoFormat,lstrlen((char *)m_AutoFormat));
			}
			RegCloseKey(key);	
		}
	}
#endif
#ifdef __MAC__
	//do something here like preferences...
	//open the preferences file
	char	Path[256];
	int		FileHandle;
	
	strcpy(Path,gbResPath);
	if( Path[strlen(Path) - 1] != '/')
		strcat(Path,"/");
	strcat(Path,"Preference.Data");
	FileHandle = open(Path,O_RDONLY,0xFFF);
	if( FileHandle > 0 ){
		read(FileHandle,(void *)&Settings,4);
		read(FileHandle,(void *)&m_DefaultPath[0],sizeof(m_DefaultPath));
		close(FileHandle);
	} else {
		Settings = 0x8C;
		sprintf((char *)m_DefaultPath,"/Users/%s",gbUser);
	}
#endif
	m_VideoChk = false;
	if( Settings & 0x00000001)
		m_VideoChk = true;
	m_AudioChk = false;
	if( Settings & 0x00000002)
		m_AudioChk = true;
	m_RenderChk = false;
	if( Settings & 0x00000004)
		m_RenderChk = true;
	m_AutoGenNameChk = false;
	if( Settings & 0x00000008)
		m_AutoGenNameChk = true;
	m_CloseChk = false;
	if( Settings & 0x00000010)
		m_CloseChk = true;
	m_AlterAudioChk = false;
	if( Settings & 0x00000020)
		m_AlterAudioChk = true;
	m_DebugChk = false;
	if( Settings & 0x00000040)
		m_DebugChk = true;
	m_LogChk = false;
	if( Settings & 0x00000080)
		m_LogChk = true;
	m_AudioIsAC3 = false;
	if( Settings & 0x00000200)
		m_AudioIsAC3 = true;


}

void WhichStreams::SaveOptions()
{
	unsigned long	Settings = 0;
#ifdef __MS__
	long	rc;
	HKEY	key;
#endif

	if( m_VideoChk )
		Settings |= 0x00000001;
	if( m_AudioChk )
		Settings |= 0x00000002;
	if( m_RenderChk )
		Settings |= 0x00000004;
	if( m_AutoGenNameChk )
		Settings |= 0x00000008;
	if( m_CloseChk )
		Settings |= 0x00000010;
	if( m_AlterAudioChk )
		Settings |= 0x00000020;
	if( m_DebugChk )
		Settings |= 0x00000040;
	if( m_LogChk )
		Settings |= 0x00000080;
	if( m_AudioIsAC3 )
		Settings |= 0x00000200;

#ifdef __MS__
	rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\DishExtract\\Options",0,KEY_ALL_ACCESS,&key);
	if( rc == ERROR_SUCCESS )
	{
		RegSetValueEx(key,"Settings",0,REG_DWORD,(UCHAR *)&Settings,sizeof(ULONG));
		//save the def path also
		RegSetValueEx(key,"DefaultPath",0,REG_SZ,(UCHAR *)m_DefaultPath,lstrlen((char *)m_DefaultPath));
		RegSetValueEx(key,"AutoFormat",0,REG_SZ,(UCHAR *)m_AutoFormat,lstrlen((char *)m_AutoFormat));
		RegCloseKey(key);
	}	
#endif

#ifdef __MAC__
	char	Path[256];
	int		FileHandle;
	
	strcpy(Path,gbResPath);
	if( Path[strlen(Path) - 1] != '/')
		strcat(Path,"/");
	strcat(Path,"Preference.Data");
	FileHandle = open(Path,O_CREAT | O_RDWR | O_TRUNC,0xFFF);
	if( FileHandle > 0 ){
		write(FileHandle,(void *)&Settings,4);
		write(FileHandle,(void *)&m_DefaultPath[0],sizeof(m_DefaultPath));
		close(FileHandle);
	}
#endif


}
