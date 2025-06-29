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

#if !defined(AFX_CUSTOMTEXT_H__B51ACD28_8F26_454B_B76D_3D47E2F993C5__INCLUDED_)
#define AFX_CUSTOMTEXT_H__B51ACD28_8F26_454B_B76D_3D47E2F993C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomText.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCustomText window

class CCustomText : public CWnd
{
// Construction
public:
	CCustomText(RECT &rec, CWnd *Parent, int BkResourceNum);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomText)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetTextColor( COLORREF clr);
	void SetTransparentMode( int mode);
	void SetDrawFlags(int Flags);
	void SetItemText( char *Text);
	virtual ~CCustomText();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCustomText)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CBitmap m_bkmap;
	RECT m_Pos;
	COLORREF m_Color;
	int m_Mode;
	int m_Flags;
	char m_Text[512];
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMTEXT_H__B51ACD28_8F26_454B_B76D_3D47E2F993C5__INCLUDED_)
