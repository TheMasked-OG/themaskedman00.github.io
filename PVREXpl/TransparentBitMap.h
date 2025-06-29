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

#if !defined(AFX_TRANSPARENTBITMAP_H__1A2884B9_B12D_4C75_B336_B9256B1C0AE5__INCLUDED_)
#define AFX_TRANSPARENTBITMAP_H__1A2884B9_B12D_4C75_B336_B9256B1C0AE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TransparentBitMap.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TransparentBitMap window

class TransparentBitMap : public CWnd
{
// Construction
public:
	TransparentBitMap(UINT ResourceNum, int bx, int by, CWnd *Parent);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TransparentBitMap)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~TransparentBitMap();

	// Generated message map functions
protected:
	//{{AFX_MSG(TransparentBitMap)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void	DrawIt(HDC Dest);
	void	GetAndXorMasks(CBitmap *Src, CBitmap *And, CBitmap *Xor);

	
	int m_SizeY;
	int m_SizeX;
	CBitmap m_XorMask;
	CBitmap m_AndMask;
	CBitmap m_BitMap;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSPARENTBITMAP_H__1A2884B9_B12D_4C75_B336_B9256B1C0AE5__INCLUDED_)
