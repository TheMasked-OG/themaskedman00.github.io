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

#if !defined(AFX_CUSTOMPROGRESS_H__D26090AA_2C05_4EDB_98FA_8900E202F5E9__INCLUDED_)
#define AFX_CUSTOMPROGRESS_H__D26090AA_2C05_4EDB_98FA_8900E202F5E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomProgress.h : header file
//
#include "CustomButton.h"

/////////////////////////////////////////////////////////////////////////////
// CCustomProgress window

class CCustomProgress : public CWnd
{
// Construction
public:
	CCustomProgress(UINT ResourceNum, int bx, int by, UINT ControlID, CWnd *Parent);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomProgress)
	//}}AFX_VIRTUAL

// Implementation
public:
	void IncPosition(ULONG Value);
	ULONG GetPosition();
	void SetPosition(ULONG Value);
	void SetMax( ULONG Value);
	void SetMin( ULONG Value);
	virtual ~CCustomProgress();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCustomProgress)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	void GetAndXorMasks( CBitmap *Src, CBitmap *And, CBitmap *Xor);
	int m_BodyX;
	int m_HeadX;
	int m_TailX;
	ULONG m_CurrentPos;
	ULONG m_Max;
	ULONG m_Min;
	void	DrawTransparent(HDC Dest, HDC Src, CBitmap *BitMap, CBitmap *AndMap, CBitmap *XorMap,int bx, int bxx);

	
	int m_SizeY;
	int m_SizeX;
	CBitmap m_Track;
	CBitmap m_Tail;
	CBitmap m_Head;
	CBitmap m_Body;
	
	CBitmap m_TrackAnd;
	CBitmap m_TrackXor;
	CBitmap m_TailAnd;
	CBitmap m_TailXor;
	CBitmap m_HeadAnd;
	CBitmap m_HeadXor;
	CBitmap m_BodyAnd;
	CBitmap m_BodyXor;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMPROGRESS_H__D26090AA_2C05_4EDB_98FA_8900E202F5E9__INCLUDED_)
