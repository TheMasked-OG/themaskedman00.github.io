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

// EnumPVRDrives.h: interface for the EnumPVRDrives class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __ENUMPVRDRIVES__
#define __ENUMPVRDRIVES__

//----------------------------- headers ------------------------
#include "Platform.h"
#include "GlobalDefs.h"

//----------------------------- definitions --------------------


#define  MAX_DRIVES		10    //we will only deal with upt to 10 physical drives
#define  MAX_NAME_SIZE	256

typedef struct {
	char	DriveFriendlyName[MAX_NAME_SIZE];
	char	DrivePath[MAX_NAME_SIZE];
	int		DriveType;
} DRIVE_ENTRY, *pDRIVE_ENTRY;

//------------------------------- classes -----------------------
class EnumPVRDrives 
{
public:
	void	AddPVRDrive( char *Path, int Type);
	int		GetNumberOfPVRDrives();
	int		GetPVRDriveType( int Index );
	void	GetDriveName( char *Name, int Index );
	void	GetDriveFriendlyName( char *Name, int Index );
	void	ScanForDrives();
	EnumPVRDrives();
	virtual ~EnumPVRDrives();

private:
	DRIVE_ENTRY		m_Drive[MAX_DRIVES];
	int				m_NumberPVRDrives;
};

#endif __ENUMPVRDRIVES__