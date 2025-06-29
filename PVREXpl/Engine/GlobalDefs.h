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

// GlobalDefs.h: 
//
//////////////////////////////////////////////////////////////////////

#include "Platform.h"

#ifndef __GLOBALDEFS__
#define __GLOBALDEFS__

//-------------- generic headers ------------
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#ifdef __MS__
	//header files specific to windows
#include <windows.h>
#include <time.h>
#include <stdio.h>

typedef __int64 int64;

#endif

#ifdef __MAC__
	//header files for mac
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <ctype.h>
#include <errno.h>
//#include <sys/stat.h>

typedef long long int64;

#define HANDLE void *	
#define stricmp strcasecmp
#define MAX_PATH	255

#endif


typedef unsigned long LBAPTR;

enum {
	PVR_TYPE_FILE,
	PVR_TYPE_50X,
	PVR_TYPE_522,
	PVR_TYPE_721,
	PVR_TYPE_622
};

#define		HAS_AUDIO_ENG	0x01
#define		HAS_AUDIO_AC3	0x02
#define		HAS_AUDIO_ALT	0x04
#define		HAS_VIDEO		0x08
#define		HAS_INDEX		0x10
#define		HAS_CD			0x20

#ifdef __x86__
#define 	PES_VIDEO_SIGNATURE			0xE0010000
#define 	PES_AUDIO_SIGNATURE			0xC0010000
#define 	PES_AC3AUDIO_SIGNATURE		0xBD010000
#define 	PES_ALT_AUDIO_SIGNATURE		0xC1010000
#define     VIDEO_SEQUENCE_SIG			0xB3010000
#define     PICTURE_HEADER_SIG			0x00010000
#define     GOP_HEADER_SIG				0xB8010000
#define     EXTENTION_START_SIG			0xB5010000

#define		PES_HEADER_MASK				0x00FFFFFF
#define     PES_HEADER_SIG				0x00010000

#endif
#ifdef __PwPC__
#define 	PES_VIDEO_SIGNATURE			0x000001E0
#define 	PES_AUDIO_SIGNATURE			0x000001C0
#define 	PES_AC3AUDIO_SIGNATURE		0x000001BD
#define 	PES_ALT_AUDIO_SIGNATURE		0x000001C1
#define     VIDEO_SEQUENCE_SIG			0x000001B3
#define     PICTURE_HEADER_SIG			0x00000100
#define     GOP_HEADER_SIG				0x000001B8
#define     EXTENTION_START_SIG			0x000001B5

#define		PES_HEADER_MASK				0xFFFFFF00
#define     PES_HEADER_SIG				0x00000100
#endif

#endif