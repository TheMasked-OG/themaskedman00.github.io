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

// Options.cpp : implementation file
//

#include "stdafx.h"
#include "PvrExplorer.h"
#include "Options.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Options dialog


Options::Options(CWnd* pParent /*=NULL*/)
	: CDialog(Options::IDD, pParent)
{
	//{{AFX_DATA_INIT(Options)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void Options::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Options)
	DDX_Control(pDX, IDC_VIDEO_CHK, m_VideoChk);
	DDX_Control(pDX, IDC_RENDER_CHK, m_RenderChk);
	DDX_Control(pDX, IDC_LOG_FILE_CHK, m_LogChk);
	DDX_Control(pDX, IDC_DEBUG_CHK, m_DebugChk);
	DDX_Control(pDX, IDC_CLOSE_CHK, m_CloseChk);
	DDX_Control(pDX, IDC_AUTO_NAME, m_AutoNameChk);
	DDX_Control(pDX, IDC_AUDIO_CHK, m_AudioChk);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Options, CDialog)
	//{{AFX_MSG_MAP(Options)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Options message handlers

void Options::OnApply() 
{
	
	m_Options->m_VideoChk = (m_VideoChk.GetCheck()== BST_CHECKED)? true:false;
	m_Options->m_AudioChk = (m_AudioChk.GetCheck()== BST_CHECKED)? true:false;
	m_Options->m_AutoGenNameChk = (m_AutoNameChk.GetCheck()== BST_CHECKED)? true:false;
	m_Options->m_CloseChk = (m_CloseChk.GetCheck()== BST_CHECKED)? true:false;
	m_Options->m_LogChk = (m_LogChk.GetCheck()== BST_CHECKED)? true:false;
	m_Options->m_RenderChk = (m_RenderChk.GetCheck()== BST_CHECKED)? true:false;
	m_Options->m_DebugChk = (m_DebugChk.GetCheck()== BST_CHECKED)? true:false;
	if( m_Options->m_AutoGenNameChk ){
		GetDlgItemText(IDC_AUTO_FORMAT,(char *)m_Options->m_AutoFormat,64);
	}
	
	m_Options->SaveOptions();
	EndDialog(1);
}

void Options::OnBrowse() 
{
	BROWSEINFO	br;
	char		FolderName[360];
	LPITEMIDLIST	idList;
	char	Line[360];
	int		j;

	CoInitialize( NULL );

	br.hwndOwner = m_hWnd;
	br.pidlRoot = NULL;
	br.pszDisplayName = FolderName;
	br.lpszTitle = "Default Save Folder";
	br.ulFlags = BIF_RETURNONLYFSDIRS;
	br.lpfn = NULL;
	br.lParam = NULL;

	idList = SHBrowseForFolder(&br );
	if( idList != NULL ){
		SHGetPathFromIDList(idList,FolderName);
		strcpy((char *)m_Options->m_DefaultPath,FolderName);
		lstrcpy(Line,(char *)m_Options->m_DefaultPath);
		if( lstrlen((char *)m_Options->m_DefaultPath) > 50 ){
			j = lstrlen((char *)m_Options->m_DefaultPath) - 22;
			Line[22] = 0x00;
			lstrcat(Line," ... ");
			lstrcat(Line,(char *)&m_Options->m_DefaultPath[ lstrlen((char *)m_Options->m_DefaultPath) - 22]);
		}
		SetDlgItemText(IDC_OUTPUT_PATH,(char *)Line);
	}
	
}

BOOL Options::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_VideoChk.SetCheck( (m_Options->m_VideoChk)? BST_CHECKED:BST_UNCHECKED);
	m_AudioChk.SetCheck( (m_Options->m_AudioChk)? BST_CHECKED:BST_UNCHECKED);
	m_AutoNameChk.SetCheck( (m_Options->m_AutoGenNameChk)? BST_CHECKED:BST_UNCHECKED);
	m_CloseChk.SetCheck( (m_Options->m_CloseChk)? BST_CHECKED:BST_UNCHECKED);
	//m_DebugChk.SetCheck( (m_Options->m_DebugChk)? BST_CHECKED:BST_UNCHECKED);
	m_LogChk.SetCheck( (m_Options->m_LogChk)? BST_CHECKED:BST_UNCHECKED);
	m_RenderChk.SetCheck( (m_Options->m_RenderChk)? BST_CHECKED:BST_UNCHECKED);
	m_DebugChk.SetCheck( (m_Options->m_DebugChk)? BST_CHECKED:BST_UNCHECKED);
	
	SetDlgItemText(IDC_OUTPUT_PATH,(char *)m_Options->m_DefaultPath);
	SetDlgItemText(IDC_AUTO_FORMAT,(char *)m_Options->m_AutoFormat);
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Options::OnCancel() 
{
	m_Options->LoadOptions();
	
	CDialog::OnCancel();
}
