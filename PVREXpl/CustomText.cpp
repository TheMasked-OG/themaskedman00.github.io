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

// CustomText.cpp : implementation file
//

#include "stdafx.h"
#include "PvrExplorer.h"
#include "CustomText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomText

CCustomText::CCustomText( RECT &rec, CWnd *Parent, int BkResourceNum)
{
	m_Flags = 0;
	Create(NULL,"",WS_CHILD | WS_VISIBLE ,rec,Parent,855);
	m_Flags = DT_CENTER | DT_NOPREFIX;
	m_Mode = TRANSPARENT;
	m_Color = RGB(255,255,255);
	//m_Color = RGB(0,0,0);
	strcpy(m_Text,"PVRExplorer");
	m_Pos.top = rec.top;
	m_Pos.bottom = rec.bottom;
	m_Pos.left = rec.left;
	m_Pos.right = rec.right;
	m_bkmap.LoadBitmap(BkResourceNum);
}

CCustomText::~CCustomText()
{
}


BEGIN_MESSAGE_MAP(CCustomText, CWnd)
	//{{AFX_MSG_MAP(CCustomText)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCustomText message handlers



void CCustomText::OnPaint() 
{
	RECT	rec;
	int		OldMode;
	COLORREF OldColor;
	CDC		CompDC;

	CPaintDC dc(this); // device context for painting
	GetClientRect(&rec);
	OldMode = dc.SetBkMode(m_Mode);
	OldColor = dc.SetTextColor(m_Color);
	CompDC.CreateCompatibleDC(&dc);
	CompDC.SelectObject(&m_bkmap);
	dc.BitBlt(0,0,rec.right,rec.bottom,&CompDC,m_Pos.left,m_Pos.top,SRCCOPY);
	dc.DrawText(m_Text,strlen(m_Text),&rec,m_Flags);
	dc.SetBkMode(OldMode);
	dc.SetTextColor(OldColor);
}

void CCustomText::SetItemText(char *Text)
{
	strcpy(m_Text,Text);
	InvalidateRect(NULL,FALSE);
	UpdateWindow();
}

void CCustomText::SetDrawFlags(int Flags)
{
	m_Flags = Flags;
}

void CCustomText::SetTransparentMode(int mode)
{
	m_Mode = mode;
}

void CCustomText::SetTextColor(COLORREF clr)
{
	m_Color = clr;
}
