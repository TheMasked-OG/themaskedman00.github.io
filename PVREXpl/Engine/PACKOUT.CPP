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

// PackOut.cpp: implementation of the PackOut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PackOut.h"


unsigned char	PackHeadDataAC3[PACK_HEADER_SIZE] = { 0x00,0x00,0x01,0xBA,0x44,0x00,0x04,0x00,0x04,0x01,0x01,0x89,0xC3,0xF8 };
unsigned char	PackHeadData[PACK_HEADER_SIZE] = { 0x00,0x00,0x01,0xBA,0x44,0x00,0x04,0x00,0x04,0x01,0x01,0x89,0xC3,0xF8 };

unsigned char	SystemHeaderDataAC3[18] = { 0x00,0x00,0x01,0xBB,0x00,0x0C,0x80,0x3A,0x07,0x04,0xE1,0x7F,0xE0,0xE1,0x02,0xBD,0xE0,0x04 };
unsigned char	SystemHeaderData[18] = { 0x00,0x00,0x01,0xBB,0x00,0x0C,0x80,0x2A,0x31,0x04,0xE1,0x7F,0xE0,0xE1,0x02,0xC0,0xC0,0x20 };
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PackOut::PackOut(bool AC3)
{
	m_TimeBase = 125;
	m_PackCount = 1;
	m_IsAC3 = AC3;
}

PackOut::~PackOut()
{

}

int64 PackOut::GetMuxRate()
{
	int64	rc;

	rc = 25200 * 50;
	return(rc);
}

void PackOut::OutPutPackHeader(int64 Clock)
{
	unsigned char	Buffer[14];
	unsigned char	shft;
	int64			ratio,MuxRate;

	if( m_IsAC3 )
		memcpy(Buffer,PackHeadDataAC3,14);
	else
		memcpy(Buffer,PackHeadData,14);

	Clock -= 0x4B64 * 300;
	ratio = Clock % 300; //this is ECRE
	MuxRate = Clock / 300;			//this is ECRB

	shft = (unsigned char)((MuxRate >> 30) & 0x07);  //bits 30-33
	shft |= 0x44;
	Buffer[4] = shft;
	shft = (unsigned char)((MuxRate >> 28) & 0x03);  //bits 28-29
	Buffer[4] |= shft;
	Buffer[5] = (unsigned char)((MuxRate >> 20) & 0xFF); //bits 20-27
	shft = (unsigned char)((MuxRate >> 15) & 0x1F);  //bits 15-19
	shft <<= 3;
	shft |= 0x04;
	Buffer[6] = shft;
	shft = (unsigned char)((MuxRate >> 13) & 0x03);  //bits 13-14
	Buffer[6] |= shft;
	Buffer[7] = (unsigned char)((MuxRate >> 5) & 0xFF); //bits 5-12
	shft = (unsigned char)((MuxRate ) & 0x1F);  //bits 15-19
	shft <<= 3;
	shft |= 0x04;
	Buffer[8] = shft;
					
	shft = (unsigned char)((ratio >> 7) & 0x03);  //bits 7-8
	Buffer[8] |= shft;
	shft = (unsigned char)((ratio << 1) & 0xFF);  //bits 0-6
	Buffer[9] = shft | 0x01;

	WriteCachedFile(Buffer,14);
	m_PackCount++;

}

void PackOut::SystemHeaderOut()
{

	if( m_IsAC3 )
		WriteCachedFile(SystemHeaderDataAC3,18);
	else
		WriteCachedFile(SystemHeaderData,18);
}
