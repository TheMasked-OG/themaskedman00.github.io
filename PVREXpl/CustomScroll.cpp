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

// CustomScroll.cpp : implementation file
//

#include "stdafx.h"
#include "PvrExplorer.h"
#include "CustomScroll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomScroll

CCustomScroll::CCustomScroll(UINT ResourceNum, int bx, int by, UINT ControlID, CWnd *Parent)
{
	BITMAP		bmInfo;
	CBitmap		tmpBitMap;
	RECT		rec;

	m_Track.LoadBitmap(ResourceNum);
	m_Thump.LoadBitmap(ResourceNum+1);
	GetAndXorMasks(&m_Track, &m_ThumpAnd, &m_ThumpXor);
	if( m_Track.GetBitmap(&bmInfo)){
		m_SizeX = bmInfo.bmWidth;
		m_SizeY = bmInfo.bmHeight;

		tmpBitMap.LoadBitmap(ResourceNum + 2);
		tmpBitMap.GetBitmap(&bmInfo);
		m_BtnSize = bmInfo.bmHeight;
	}
	
	rec.left = bx;
	rec.top = by;
	rec.right = rec.left + m_SizeX;
	rec.bottom = rec.top + m_SizeY;

	Create(NULL,"",WS_CHILD | WS_VISIBLE ,rec,Parent,ControlID);
	
	m_UpBtn = (CCustomButton *)new CCustomButton( ResourceNum + 2,0,0,ControlID+1,(CWnd *)this);
	m_DnBtn = (CCustomButton *)new CCustomButton( ResourceNum + 9,0,m_SizeY - m_BtnSize,ControlID+2,(CWnd *)this);

}

CCustomScroll::~CCustomScroll()
{
	delete m_UpBtn;
	delete m_DnBtn;
}


BEGIN_MESSAGE_MAP(CCustomScroll, CWnd)
	//{{AFX_MSG_MAP(CCustomScroll)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCustomScroll message handlers
void 
CCustomScroll::GetAndXorMasks(CBitmap *Src, CBitmap *And, CBitmap *Xor)
{
	BITMAP		bmInfo;
	ULONG		x,y,BufferSize,j,k;
	UCHAR		*Buffer,*AndBuffer,*XorBuffer;
	ULONG		TransparentColor;
	ULONG		*aPtr,*xPtr,*uPtr;

	if( Src->GetBitmap(&bmInfo)){
		x = bmInfo.bmWidth;
		y = bmInfo.bmHeight;
		BufferSize = bmInfo.bmHeight * bmInfo.bmWidthBytes;
		Buffer = (UCHAR *)new UCHAR[BufferSize];
		Src->GetBitmapBits(BufferSize,(void *)Buffer);
		// get the first color.
		TransparentColor = *(DWORD *)Buffer;  //this shold be the transparent color
		AndBuffer = (UCHAR *)new UCHAR[BufferSize];
		XorBuffer = (UCHAR *)new UCHAR[BufferSize];
		aPtr = (ULONG *)AndBuffer;
		xPtr = (ULONG *)XorBuffer;
		uPtr = (ULONG *)Buffer;
		for( j = 0; j < y;j++ ){
			for( k = 0; k < x; k++){
				if( *uPtr == TransparentColor ){
					*aPtr = 0x00FFFFFF;
					*xPtr = *uPtr;
				} else {
					*aPtr = 0;
					*xPtr = 0;
				}
				aPtr++;
				uPtr++;
				xPtr++;
			}
		}
		bmInfo.bmBits = (void *)AndBuffer;
		And->CreateBitmapIndirect(&bmInfo);
		bmInfo.bmBits = (void *)XorBuffer;
		Xor->CreateBitmapIndirect(&bmInfo);
		delete []AndBuffer;
		delete []XorBuffer;
	}
}

void CCustomScroll::OnPaint() 
{
	CDC  CompDC;

	CPaintDC dc(this); // device context for painting
	if( m_SizeX > 0 ){
		CompDC.CreateCompatibleDC(&dc);	//create tmp DC
	
		CompDC.SelectObject(&m_Track);
		dc.BitBlt(0,0,m_SizeX,m_SizeY,&CompDC,0,0,SRCCOPY); //We do not have to do trasparent because the region did it for us
		CompDC.DeleteDC();				//no memory leaks!
	}
	
}

void CCustomScroll::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CCustomScroll::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnLButtonUp(nFlags, point);
}

void CCustomScroll::OnMouseMove(UINT nFlags, CPoint point) 
{
	
	CWnd::OnMouseMove(nFlags, point);
}

BOOL CCustomScroll::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}




