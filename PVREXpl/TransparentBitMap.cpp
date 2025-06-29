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

// TransparentBitMap.cpp : implementation file
//

#include "stdafx.h"
#include "PvrExplorer.h"
#include "TransparentBitMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TransparentBitMap

TransparentBitMap::TransparentBitMap(UINT ResourceNum, int bx, int by, CWnd *Parent)
{
	BITMAP		bmInfo;
	RECT		rec;

	m_BitMap.LoadBitmap(ResourceNum);
	
	if( m_BitMap.GetBitmap(&bmInfo)){
		m_SizeX = bmInfo.bmWidth;
		m_SizeY = bmInfo.bmHeight;
		
		rec.left = bx;
		rec.top = by;
		rec.right = rec.left + m_SizeX;
		rec.bottom = rec.top + m_SizeY;
		Create(NULL,"", WS_CHILD | WS_VISIBLE ,rec,Parent,500);
		CWnd::ShowWindow(SW_HIDE);

		GetAndXorMasks(&m_BitMap, &m_AndMask, &m_XorMask);

	}
}

TransparentBitMap::~TransparentBitMap()
{
}


BEGIN_MESSAGE_MAP(TransparentBitMap, CWnd)
	//{{AFX_MSG_MAP(TransparentBitMap)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// TransparentBitMap message handlers

void TransparentBitMap::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	DrawIt(dc.m_hDC);
	
	// Do not call CWnd::OnPaint() for painting messages
}


//****************************************************************
//  DrawIt
//		This Does the mechanics of drawing a trasparent button
//		(IN) Dest - The destination DC that we are drawing to
//		(IN) Src - The Source DC to do the bitblt to
//		(IN) BitMap - What to draw
//****************************************************************
void 
TransparentBitMap::DrawIt(HDC Dest)
{
	ULONG	x,y;
	HDC	dc,Src;
	HBITMAP	temMap;

	x = m_SizeX;
	y = m_SizeY;
	dc = CreateCompatibleDC(Dest);		//create a worker DC
	Src = CreateCompatibleDC(Dest);		//create a worker DC
	temMap = CreateCompatibleBitmap(Dest,x,y);	//Create a bitmap the we can do the mixin
	SelectObject(dc,temMap);					//this removes the flashing artifacts
	BitBlt(dc,0,0,x,y,Dest,0,0,SRCCOPY);		// grin from the screen to the work area
	SelectObject(Src,m_AndMask.m_hObject);
	BitBlt(dc,0,0,x,y,Src,0,0,SRCAND);			// and it with the AND mask
	SelectObject(Src,m_XorMask.m_hObject);
	BitBlt(dc,0,0,x,y,Src,0,0,SRCINVERT);		// then XORit with the XOR mask
	SelectObject(Src,m_BitMap.m_hObject);
	BitBlt(dc,0,0,x,y,Src,0,0,SRCINVERT);		// then XOR the bitmap in it
	BitBlt(Dest,0,0,x,y,dc,0,0,SRCCOPY);		// Copy it back
	DeleteDC(dc);
	DeleteDC(Src);
	DeleteObject(temMap);
}

BOOL TransparentBitMap::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	//return CWnd::OnEraseBkgnd(pDC);
	return(0);
}

void TransparentBitMap::GetAndXorMasks(CBitmap *Src, CBitmap *And, CBitmap *Xor)
{
	BITMAP		bmInfo;

	CDC			*dc;
	CDC			SrcDC;
	CDC			AndDC;
	CDC			XorDC;
	COLORREF	TransparentColor;
	int			x,y,j,k;

	dc = GetDC();

	SrcDC.CreateCompatibleDC(dc);
	AndDC.CreateCompatibleDC(dc);
	XorDC.CreateCompatibleDC(dc);

	Src->GetBitmap(&bmInfo);
	x = bmInfo.bmWidth;
	y = bmInfo.bmHeight;
	And->CreateCompatibleBitmap(dc,x,y);
	Xor->CreateCompatibleBitmap(dc,x,y);

	SrcDC.SelectObject(Src);
	AndDC.SelectObject(And);
	XorDC.SelectObject(Xor);

	TransparentColor = SrcDC.GetPixel(0,0);
	for( j = 0; j < y;j++ ){
		for( k = 0; k < x; k++){
			if( SrcDC.GetPixel(k,j) == TransparentColor ){
				AndDC.SetPixel(k,j,RGB(255,255,255));
				XorDC.SetPixel(k,j,TransparentColor);
			} else {
				AndDC.SetPixel(k,j,RGB(0,0,0));
				XorDC.SetPixel(k,j,RGB(0,0,0));
			}
		}
	}
	ReleaseDC(dc);
	SrcDC.DeleteDC();
	AndDC.DeleteDC();
	XorDC.DeleteDC();

}
