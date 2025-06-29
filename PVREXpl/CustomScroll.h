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

#if !defined(AFX_CUSTOMSCROLL_H__DEC0203A_4DE4_45A8_A305_6C8974A8E0BC__INCLUDED_)
#define AFX_CUSTOMSCROLL_H__DEC0203A_4DE4_45A8_A305_6C8974A8E0BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomScroll.h : header file
//
#include "CustomButton.h"

/////////////////////////////////////////////////////////////////////////////
// CCustomScroll window

class CCustomScroll : public CWnd
{
// Construction
public:
	CCustomScroll(UINT ResourceNum, int bx, int by, UINT ControlID, CWnd *Parent);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomScroll)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCustomScroll();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCustomScroll)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int m_BtnSize;
	int m_SizeY;
	int m_SizeX;
	void		GetAndXorMasks(CBitmap *Src, CBitmap *And, CBitmap *Xor);

	CCustomButton * m_DnBtn;
	CCustomButton * m_UpBtn;
	CBitmap		m_Track;
	CBitmap		m_Thump;
	CBitmap		m_ThumpAnd;
	CBitmap		m_ThumpXor;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMSCROLL_H__DEC0203A_4DE4_45A8_A305_6C8974A8E0BC__INCLUDED_)
