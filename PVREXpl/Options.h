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

#if !defined(AFX_OPTIONS_H__A47E4038_1228_461D_8DEA_5583CD62B826__INCLUDED_)
#define AFX_OPTIONS_H__A47E4038_1228_461D_8DEA_5583CD62B826__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Options.h : header file
//
#include ".\engine\WhichStreams.h"

/////////////////////////////////////////////////////////////////////////////
// Options dialog

class Options : public CDialog
{
// Construction
public:
	WhichStreams * m_Options;
	Options(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Options)
	enum { IDD = IDD_OPTIONS };
	CButton	m_VideoChk;
	CButton	m_RenderChk;
	CButton	m_LogChk;
	CButton	m_DebugChk;
	CButton	m_CloseChk;
	CButton	m_AutoNameChk;
	CButton	m_AudioChk;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Options)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Options)
	afx_msg void OnApply();
	afx_msg void OnBrowse();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONS_H__A47E4038_1228_461D_8DEA_5583CD62B826__INCLUDED_)
