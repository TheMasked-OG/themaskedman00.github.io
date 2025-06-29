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

// DishPvrHdd.cpp: implementation of the DishPvrHdd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DishPvrHdd.h"
#include "HDD50X.h"
#include "HDD522.h"
#include "HDD622.h"
#include "HDD721.h"
#include "HDDFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DishPvrHdd::DishPvrHdd()
{
	m_PvrHdd = NULL;
}

DishPvrHdd::~DishPvrHdd()
{
	if( m_PvrHdd != NULL )
		delete m_PvrHdd;
}

//////////////////////////////////////////////////////////////////////
// Public Methods
//////////////////////////////////////////////////////////////////////

//---------------------------------------------
//  this function will set the path and type of the HDD interface to use.
int 
DishPvrHdd::SetPvrHdd(char *Path, int Type)
{
	HDD50X	*drv50x;
	HDD522  *drv522;
	HDD721	*drv721;
	HDD622	*drv622;
	HDDFile *drvFile;
	
	m_PvrType = Type;
	if( Type == PVR_TYPE_50X ){
		drv50x = (HDD50X *)new HDD50X;
		if( drv50x != NULL ){
			if( drv50x->OpenPVRDrive(Path)){
				m_PvrHdd = (HDDPVR *)drv50x;
				return(1);
			}
		}
	}
	if( Type == PVR_TYPE_522 ){
		drv522 = (HDD522 *)new HDD522;
		if( drv522 != NULL ){
			if( drv522->OpenPVRDrive(Path)){
				m_PvrHdd = (HDDPVR *)drv522;
				return(1);
			}
		}
	}
	if( Type == PVR_TYPE_721 ){
		drv721 = (HDD721 *)new HDD721;
		if( drv721 != NULL ){
			if( drv721->OpenPVRDrive(Path)){
				m_PvrHdd = (HDDPVR *)drv721;
				return(1);
			}
		}
	}
	if( Type == PVR_TYPE_622 ){
		drv622 = (HDD622 *)new HDD622;
		if( drv622 != NULL ){
			if( drv622->OpenPVRDrive(Path) && drv622->OpenAltPVRDrive(Path)){
				drv622->IsPvrDrive();
				m_PvrHdd = (HDDPVR *)drv622;
				return(1);
			}
		}
	}
	if( Type == PVR_TYPE_FILE ){
		strcpy(m_FileRootPath,Path);
		drvFile = (HDDFile *)new HDDFile;
		if( drvFile != NULL ){
			m_PvrHdd = (HDDPVR *)drvFile;
			return(1);
		}
	}
	OutPutDebugString("Invalid PVR HDD Type ID!");
	return(0);
}

//---------------------------------------------
//  this function will Close a PVR File Handle
void			
DishPvrHdd::ClosePvrFile( HANDLE FileHandle )
{
	m_PvrHdd->ClosePvrFile( FileHandle );
}

//---------------------------------------------
//  this function will read a PVR File Handle
unsigned long	
DishPvrHdd::ReadPvrFile( HANDLE fileHandle, unsigned char *Data, unsigned long Length)
{
	return(m_PvrHdd->ReadPvrFile(fileHandle,Data,Length));
}

//---------------------------------------------
//  this function will open a PVR File Handle
HANDLE			
DishPvrHdd::OpenPvrFile( char *FileName )
{
	char	newName[256];

	if( m_PvrType == PVR_TYPE_FILE ){
		strcpy(newName,m_FileRootPath);
#ifdef __MS__
		if( newName[strlen(newName) - 1] != '\\')
			strcat(newName,"\\");
#endif
#ifdef __MAC__
		if( newName[strlen(newName) - 1] != '/')
			strcat(newName,"/");
#endif
		strcat(newName,FileName);
		return(m_PvrHdd->OpenPvrFile(newName));
	} else 
		return(m_PvrHdd->OpenPvrFile(FileName));

}

//------------------------------------------------
//  this will return the file size
int64
DishPvrHdd::GetPvrFileSize( HANDLE FileHandle )
{
	return(m_PvrHdd->GetPvrFileSize(FileHandle) );
}

void
DishPvrHdd::AdjustAudioFileSize( HANDLE FileHandle )
{
	m_PvrHdd->AdjustAudioFileSize( FileHandle );
}

unsigned long	
DishPvrHdd::GetPvrFileTime( char *FileName )
{
	return(m_PvrHdd->GetPvrFileTime(FileName));
}
