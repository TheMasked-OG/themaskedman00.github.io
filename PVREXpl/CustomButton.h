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

#if !defined(AFX_CUSTOMBUTTON_H__C4FC5C9C_E2AF_43BA_919C_1DEC707125DC__INCLUDED_)
#define AFX_CUSTOMBUTTON_H__C4FC5C9C_E2AF_43BA_919C_1DEC707125DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCustomButton window

class CCustomButton : public CButton
{
// Construction
public:
	CCustomButton(UINT ResourceNum, int bx, int by, UINT ControlID, CWnd *Parent);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	bool IsLastClicked();
	void SetCheck( int state );
	int GetCheck();
	virtual ~CCustomButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCustomButton)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	void CreateMaskMaps();
	bool m_IsMapsValid;
	bool m_LastClicked;
	int m_State;
	void	DrawButton(HDC Dest, HDC Src, CBitmap *BitMap);
	void	DrawTransparent(HDC Dest, HDC Src, CBitmap *BitMap);

	CRgn	m_rgn;
	CBitmap m_ButtonUp;
	CBitmap m_ButtonDwn;
	CBitmap m_ButtonOver;
	CBitmap m_ButtonAnd;
	CBitmap m_ButtonXor;
	CBitmap m_DisabledMap;
	CBitmap m_ButtonOn;
	CBitmap m_ButtonOff;
	CBitmap m_ButtonFocus;

	int		m_SizeX;
	int		m_SizeY;
	bool m_MouseOver;
	
	BOOL	m_IsThereButtonDwn;
	BOOL	m_IsThereButtonIn;
	BOOL	m_IsThereButtonDis;
	BOOL	m_IsThereButtonOn;
	BOOL	m_IsThereButtonOff;
	BOOL	m_IsThereButtonFocus;
	BOOL	m_IsThereButtonOver;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMBUTTON_H__C4FC5C9C_E2AF_43BA_919C_1DEC707125DC__INCLUDED_)
