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

// PvrExplorerDlg.h : header file
//

#if !defined(AFX_PVREXPLORERDLG_H__5A576DA5_3687_4CB5_B13F_60DD0850B664__INCLUDED_)
#define AFX_PVREXPLORERDLG_H__5A576DA5_3687_4CB5_B13F_60DD0850B664__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CustomButton.h"
#include "TransparentBitMap.h"
#include "CustomProgress.h"
#include "CustomText.h"

#include ".\engine\Platform.h"
#include ".\engine\GlobalDefs.h"
#include ".\engine\EnumPVRDrives.h"
#include ".\engine\DishPvr.h"
#include ".\engine\DishPVR50X.h"
#include ".\engine\DishPVR522.h"
#include ".\engine\DishPVR721.h"
#include ".\engine\DishPVRFile.h"
#include ".\engine\DishPVR622.h"
#include ".\engine\ExtractProgram.h"
#include ".\engine\ExtractCopy.h"
#include ".\engine\WhichStreams.h"
#include ".\engine\LogFile.h"

#define MAX_SCREEN_LINES 10

#define SEL_LINE_WIDTH	25
/////////////////////////////////////////////////////////////////////////////
// CPvrExplorerDlg dialog

class CPvrExplorerDlg : public CDialog
{
// Construction
public:
	CPvrExplorerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPvrExplorerDlg)
	enum { IDD = IDD_PVREXPLORER_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPvrExplorerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPvrExplorerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSysClose();
	afx_msg void OnOptionBtn();
	afx_msg void OnExtractBtn();
	afx_msg void OnCancelBtn();
	afx_msg void OnAboutBtn();
	afx_msg void OnAllBtn();
	afx_msg void OnSelectionBtn();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelchangeDrive();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg UINT OnNcHitTest(CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void UpdateButtons();
	bool m_OkToPaintList;
	int m_TicCount;
	bool m_ExtractingMode;
	char m_CurProgramDesc[512];
	int m_CurProgramCh;
	char m_CurProgramChName[32];
	void DisplayProgramList(CDC *dc);
	CComboBox m_DrvSelect;
	CCustomButton * m_OptionButton;
	CCustomButton * m_SysButton;
	CCustomButton * m_AboutButton;
	CCustomButton * m_AllButton;
	CCustomButton * m_ExtractButton;
	CCustomText	  * m_ChannelInfoText;
	CCustomText	  * m_DescText;
	CCustomText	  * m_TimeText;
	CCustomText	  * m_FileNameText;
	CCustomButton *m_chk[MAX_SCREEN_LINES];
	TransparentBitMap *m_BtnBase;
	CCustomProgress   *m_Progress;
	CCustomButton * m_CancelButton;
	CScrollBar	ScrolBar;

	TransparentBitMap *m_501;
	TransparentBitMap *m_522;
	TransparentBitMap *m_721;

	int			m_SizeY;
	int			m_SizeX;
	CBitmap		m_hBitmap;
	EnumPVRDrives	m_DriveEnum;
	DishPvr			*m_pvr;
	int			m_NumPrograms;
	int			m_TopOfPage;
	int			*m_Selected;
	int			m_CurSelected;
	WhichStreams	m_Options;
	ExtractProgram	*m_ext;
	ExtractCopy * m_ecpy;
	LogFile		m_Log;
	char		m_VideoInfoStr[128];
	char		m_AudioInfoStr[128];
	char		m_BytesInfoStr[128];
	char		m_FrameTimeStr[128];
	char		m_ProcessRate[64];
	char		m_TimeRemaining[64];
	char		m_OutputFileName[256];
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PVREXPLORERDLG_H__5A576DA5_3687_4CB5_B13F_60DD0850B664__INCLUDED_)
