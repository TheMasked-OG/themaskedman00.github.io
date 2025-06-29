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

// Header file for the on disk structure of the 522 Catalog.cat file.

// Revision B 08/25/05 - S
// Revision A 08/xx/05 - T

#ifndef __522_CATALOG_CAT__
#define __522_CATALOG_CAT__

#ifdef __MS__
#pragma pack( push )
#pragma pack(1)
#endif
#ifdef __MAC__
#pragma pack(1)
#endif


typedef struct {
	unsigned short		TsFileNameID;		// this is the number of the ts file in the third partition
											// ie TsFileNameID = 8002, then file = es8002.tsp
	unsigned long		Flags1;				// flags
											// 00 00 01 00 - scrambled channel
											// 00 00 00 00 - FTA, like 580
	unsigned long		Flags2;				// flags
											// 00 00 10 00 - Resume Mode
											// 00 00 20 00 - Play Mode (?)
											// 00 00 01 00 - Protected Mode
	unsigned long		Flags3;				// flags
	char				ProgramName[36];	// the name of the program
	char				ProgramDesc[256];	// the program description
	unsigned short		Unknown4;			//not sure what this is
	unsigned long		DateTime;			// the date/time ( what format ? )
	unsigned short		CHannelNumber;		// the channel number
	char				ChannelName[6];		// the name of the channel
	unsigned long		Unknown5;			//not sure
	unsigned long		Unknown6;			//not sure
	unsigned long		Unknown7;			//not sure
	unsigned long		DateTime2;			// this is the same as the one above...
	unsigned long		Unknown8;			//not sure
	unsigned long		TimeLeft;			// for Resume Mode
	unsigned long		Unknown10;			//not sure
	unsigned long		Unknown11;			//not sure
	unsigned long		Unknown12;			//not sure
	unsigned long		Constant1;			//this seems to always be 0x9B
	unsigned long		Unknown13;			//not sure
	unsigned long		Unknown14;			//not sure
	unsigned long		Unknown15;			//not sure
	unsigned long		Unknown16;			//not sure
	unsigned long		Unknown17;			//not sure
	unsigned long		Unknown18;			//not sure
	unsigned long		Constant2;			//this seems to always be 1
	unsigned long		Blank[13];			//the rest seems to be 0
} PROGRAM_ELEMENT, *pPROGRAM_ELEMENT;   // this structure should be 440 bytes long

typedef struct {
	unsigned short		FileVersion;		// this CAT file version [0101]
	unsigned short		NumberOfElements;   // this the number of elements in the file
	unsigned char		IndexBitMap[125];   // * bytes 5-129 are a bitmap that designate which elements
											// * are filled and which have been deleted by the dvr
											// * (0 represents filled, 1 empty)
	unsigned char		term;				//this is 0x00
	PROGRAM_ELEMENT	Program[1];				// this is the array of PROGRAM_ELEMENTS[NumberOfElements];
} CATALOG_522, *pCATALOG_522;

#ifdef __MS__
#pragma pack( pop )
#endif
#ifdef __MAC__
#pragma pack()
#endif


#endif
