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

// EnumPVRDrives.cpp: implementation of the EnumPVRDrives class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Platform.h"
#include "EnumPVRDrives.h"
#include "HDD50X.h"
#include "HDD522.h"
#include "HDD721.h"
#include "HDD622.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EnumPVRDrives::EnumPVRDrives()
{

}

EnumPVRDrives::~EnumPVRDrives()
{

}

//////////////////////////////////////////////////////////////////////
// Public Methods
//////////////////////////////////////////////////////////////////////

//------------------------
// this function will scan all HDD's and locate the propore ones
//		also will fill in the memder data for later retrieval
void	
EnumPVRDrives::ScanForDrives()
{
	int		j;
	char	Path[MAX_PATH];
	HDD50X	drv50x;
	HDD522  drv522;
	HDD721	drv721;
	HDD622	drv622;
	bool	found;
	
	m_NumberPVRDrives = 0;
	for( j = 0; j < MAX_DRIVES; j++){
#ifdef __MS__
		sprintf(Path,"\\\\.\\PHYSICALDRIVE%d",j);
#endif
#ifdef __MAC__
		sprintf(Path,"/dev/rdisk%d",j);
#endif
		found = false;
		if( drv50x.OpenPVRDrive(Path)){ //ok we have a drive with media...
			if( drv50x.IsPvrDrive()){   // it is also now a pvr drive
				sprintf(m_Drive[m_NumberPVRDrives].DriveFriendlyName,"Drive%d (501/508/510)",j);
				strcpy(m_Drive[m_NumberPVRDrives].DrivePath,Path);
				m_Drive[m_NumberPVRDrives].DriveType = PVR_TYPE_50X;
				//LogFile  m_Drive[m_NumberPVRDrives].DriveFriendlyName
				m_NumberPVRDrives++;
				found = true;
			}
			drv50x.ClosePvrDrive();
		}
		if( !found ){  //then try 522
			if( drv522.OpenPVRDrive(Path)){ //ok we have a drive with media...
				if( drv522.IsPvrDrive()){   // it is also now a pvr drive
					sprintf(m_Drive[m_NumberPVRDrives].DriveFriendlyName,"Drive%d (522/625)",j);
					strcpy(m_Drive[m_NumberPVRDrives].DrivePath,Path);
					m_Drive[m_NumberPVRDrives].DriveType = PVR_TYPE_522;
					//LogFile  m_Drive[m_NumberPVRDrives].DriveFriendlyName
					m_NumberPVRDrives++;
					found = true;
				}
				drv522.ClosePvrDrive();
			}
		}
		if( !found ){  //then try 721
			if( drv721.OpenPVRDrive(Path)){ //ok we have a drive with media...
				if( drv721.IsPvrDrive()){   // it is also now a pvr drive
					sprintf(m_Drive[m_NumberPVRDrives].DriveFriendlyName,"Drive%d (721/921)",j);
					strcpy(m_Drive[m_NumberPVRDrives].DrivePath,Path);
					m_Drive[m_NumberPVRDrives].DriveType = PVR_TYPE_721;
					//LogFile  m_Drive[m_NumberPVRDrives].DriveFriendlyName
					m_NumberPVRDrives++;
					found = true;
				}
				drv721.ClosePvrDrive();
			}
		}
		if( !found ){  //then try 622
			if( drv622.OpenPVRDrive(Path)){ //ok we have a drive with media...
				if( drv622.IsPvrDrive()){   // it is also now a pvr drive
					sprintf(m_Drive[m_NumberPVRDrives].DriveFriendlyName,"Drive%d (622)",j);
					strcpy(m_Drive[m_NumberPVRDrives].DrivePath,Path);
					m_Drive[m_NumberPVRDrives].DriveType = PVR_TYPE_622;
					//LogFile  m_Drive[m_NumberPVRDrives].DriveFriendlyName
					m_NumberPVRDrives++;
					found = true;
				}
				drv622.ClosePvrDrive();
			}
		}
	}
}

//---------------------------
// this function will return the friendly name of the hdd for printing
void 
EnumPVRDrives::GetDriveFriendlyName(char *Name, int Index)
{
	if( NULL != Name && Index < m_NumberPVRDrives ){
		strcpy(Name,m_Drive[Index].DriveFriendlyName);
	}
}

//---------------------------
// this function will return the full path name of the HDD for accessing
void 
EnumPVRDrives::GetDriveName(char *Name, int Index)
{
	if( NULL != Name && Index < m_NumberPVRDrives ){
		strcpy(Name,m_Drive[Index].DrivePath);
	}
}

//---------------------------
// this function will return the type of pvr hdd
int 
EnumPVRDrives::GetPVRDriveType(int Index)
{
	if( Index < m_NumberPVRDrives ){
		return(m_Drive[Index].DriveType);
	}
	return(-1);
}

//---------------------------
// this function will return the number of found pvr hdds
int 
EnumPVRDrives::GetNumberOfPVRDrives()
{
	return(m_NumberPVRDrives);
}

//---------------------------
// this function will return the number of found pvr hdds
void	
EnumPVRDrives::AddPVRDrive( char *Path, int Type)
{
	
	strcpy(m_Drive[m_NumberPVRDrives].DrivePath,Path);
	m_Drive[m_NumberPVRDrives].DriveType = Type;
	if( Type == PVR_TYPE_50X) //ok we have a drive with media...
		sprintf(m_Drive[m_NumberPVRDrives].DriveFriendlyName,"Drive(Image) (501/508/510)");
	if( Type == PVR_TYPE_522) //ok we have a drive with media...
		sprintf(m_Drive[m_NumberPVRDrives].DriveFriendlyName,"Drive(Image) (522/625)");
	if( Type == PVR_TYPE_721) //ok we have a drive with media...
		sprintf(m_Drive[m_NumberPVRDrives].DriveFriendlyName,"Drive(Image) (721/921)");
	if( Type == PVR_TYPE_FILE) //ok we have a Local file type
		sprintf(m_Drive[m_NumberPVRDrives].DriveFriendlyName,"Local Folder (*.tsp files)");
	m_NumberPVRDrives++;
}
