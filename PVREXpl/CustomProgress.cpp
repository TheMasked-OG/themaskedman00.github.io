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

// CustomProgress.cpp : implementation file
//

#include "stdafx.h"
#include "PvrExplorer.h"
#include "CustomProgress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomProgress

CCustomProgress::CCustomProgress(UINT ResourceNum, int bx, int by, UINT ControlID, CWnd *Parent)
{
	BITMAP		bmInfo;
	RECT		rec;

	m_Track.LoadBitmap(ResourceNum);
	m_Tail.LoadBitmap(ResourceNum+1);
	m_Head.LoadBitmap(ResourceNum+3);
	m_Body.LoadBitmap(ResourceNum+2);
	if( m_Track.GetBitmap(&bmInfo)){
		m_SizeX = bmInfo.bmWidth;
		m_SizeY = bmInfo.bmHeight;
	}
	if( m_Tail.GetBitmap(&bmInfo)){
		m_TailX = bmInfo.bmWidth;
	}
	if( m_Head.GetBitmap(&bmInfo)){
		m_HeadX = bmInfo.bmWidth;
	}
	if( m_Body.GetBitmap(&bmInfo)){
		m_BodyX = bmInfo.bmWidth;
	}
	rec.left = bx;
	rec.top = by;
	rec.right = rec.left + m_SizeX;
	rec.bottom = rec.top + m_SizeY;
	
	Create(NULL,"",WS_CHILD | WS_VISIBLE ,rec,Parent,ControlID);
	CWnd::ShowWindow(SW_HIDE);
	
	GetAndXorMasks(&m_Track,&m_TrackAnd,&m_TrackXor);
	GetAndXorMasks(&m_Tail,&m_TailAnd,&m_TailXor);
	GetAndXorMasks(&m_Head,&m_HeadAnd,&m_HeadXor);
	GetAndXorMasks(&m_Body,&m_BodyAnd,&m_BodyXor);
	
	m_Min = 0;
	m_Max = 100;
	m_CurrentPos = 0;
}

CCustomProgress::~CCustomProgress()
{
}


BEGIN_MESSAGE_MAP(CCustomProgress, CWnd)
	//{{AFX_MSG_MAP(CCustomProgress)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomProgress message handlers

void CCustomProgress::OnPaint() 
{
	CDC		Src;
	int		BitMax;
	double	bits,x;
	CBitmap	tmpMap;

	
	CPaintDC dc(this); // device context for painting
	
	Src.CreateCompatibleDC(&dc);
	DrawTransparent(dc.m_hDC,Src.m_hDC,&m_Track,&m_TrackAnd,&m_TrackXor,0,0);	

	DrawTransparent(dc.m_hDC,Src.m_hDC,&m_Tail,&m_TailAnd,&m_TailXor,0,0);	
	//now figure out the middle
	
	BitMax = m_SizeX - (m_HeadX + m_TailX);
	x = (double)(m_CurrentPos - m_Min) / (double)(m_Max - m_Min);
	bits = (double)BitMax * x;
	BitMax = (int)bits;


	if( BitMax > 0 ){
		DrawTransparent(dc.m_hDC,Src.m_hDC,&m_Body,&m_BodyAnd,&m_BodyXor,m_TailX,BitMax);	
	}

	DrawTransparent(dc.m_hDC,Src.m_hDC,&m_Head,&m_HeadAnd,&m_HeadXor,m_TailX + BitMax,0);	
	Src.DeleteDC();
}

//****************************************************************
//  DrawTransparent
//		This Does the mechanics of drawing a trasparent button
//		(IN) Dest - The destination DC that we are drawing to
//		(IN) Src - The Source DC to do the bitblt to
//		(IN) BitMap - What to draw
//****************************************************************
void 
CCustomProgress::DrawTransparent(HDC Dest, HDC Src, CBitmap *BitMap, CBitmap *AndMap, CBitmap *XorMap,int bx, int bxx)
{

	BITMAP		bmInfo;
	int			x,y;
	HDC	dc;
	HBITMAP	temMap;

	if( BitMap->m_hObject == NULL )
		return;
	if( BitMap->GetBitmap(&bmInfo)){
		x = bmInfo.bmWidth;
		y = bmInfo.bmHeight;
		if(bxx > 0)
			x = bxx;
		dc = CreateCompatibleDC(Dest);		//create a worker DC
		temMap = CreateCompatibleBitmap(Dest,x,y);	//Create a bitmap the we can do the mixin
		SelectObject(dc,temMap);					//this removes the flashing artifacts
		BitBlt(dc,0,0,x,y,Dest,bx,0,SRCCOPY);		// grin from the screen to the work area
		SelectObject(Src,AndMap->m_hObject);
		BitBlt(dc,0,0,x,y,Src,0,0,SRCAND);			// and it with the AND mask
		SelectObject(Src,XorMap->m_hObject);
		BitBlt(dc,0,0,x,y,Src,0,0,SRCINVERT);		// then XORit with the XOR mask
		SelectObject(Src,BitMap->m_hObject);
		BitBlt(dc,0,0,x,y,Src,0,0,SRCINVERT);		// then XOR the bitmap in it
		BitBlt(Dest,bx,0,x,y,dc,0,0,SRCCOPY);		// Copy it back
		DeleteObject(temMap);
		DeleteDC(dc);
	}

}

BOOL CCustomProgress::OnEraseBkgnd(CDC* pDC) 
{
	//return CWnd::OnEraseBkgnd(pDC);
	return(0);
}

void CCustomProgress::SetMin(ULONG Value)
{
	m_Min = Value;
}

void CCustomProgress::SetMax(ULONG Value)
{
	m_Max = Value;
}

void CCustomProgress::SetPosition(ULONG Value)
{
	m_CurrentPos = Value;
	if( m_CurrentPos > m_Max)
		m_CurrentPos = m_Max;
	InvalidateRect(NULL,TRUE);
	UpdateWindow();
}

ULONG CCustomProgress::GetPosition()
{
	return(m_CurrentPos);
}

void CCustomProgress::IncPosition(ULONG Value)
{
	m_CurrentPos += Value;
	if( m_CurrentPos > m_Max)
		m_CurrentPos = m_Max;
	InvalidateRect(NULL,TRUE);
	UpdateWindow();
}

void CCustomProgress::GetAndXorMasks(CBitmap *Src, CBitmap *And, CBitmap *Xor)
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
