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

// PvrExplorerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PvrExplorer.h"
#include "PvrExplorerDlg.h"
#include "Options.h"
#include "AboutBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
//                  Globals 
//////////////////////////////////////////////////////////////////////
LogFile		dbgLog;


/////////////////////////////////////////////////////////////////////////////
// CPvrExplorerDlg dialog

CPvrExplorerDlg::CPvrExplorerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPvrExplorerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPvrExplorerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pvr = NULL;
	m_TopOfPage = 0;
	m_Selected = NULL;
	m_CurSelected = 0;
	strcpy(m_CurProgramChName,"Disk Network");
	m_CurProgramCh = 0;
	strcpy(m_CurProgramDesc," not available ");
	m_ExtractingMode = false;
	m_ext = NULL;
	m_pvr = NULL;
	m_OkToPaintList = false;
	m_NumPrograms = 0;
}

void CPvrExplorerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPvrExplorerDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPvrExplorerDlg, CDialog)
	//{{AFX_MSG_MAP(CPvrExplorerDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND( IDC_SYSCLOSE_CMD, OnSysClose)
	ON_COMMAND( IDC_OPTION_CMD, OnOptionBtn)
	ON_COMMAND( IDC_EXTRACT_CMD, OnExtractBtn)
	ON_COMMAND( IDC_CANCEL_CMD, OnCancelBtn)
	ON_COMMAND( IDC_ABOUT_CMD, OnAboutBtn)
	ON_COMMAND( IDC_ALL_CMD, OnAllBtn)
	ON_COMMAND( IDB_SELECTED_CMD, OnSelectionBtn)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_DRIVE_SELECT, OnSelchangeDrive)
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_NCHITTEST()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPvrExplorerDlg message handlers

BOOL CPvrExplorerDlg::OnInitDialog()
{
	BITMAP				bmInfo;
	int					x,y,j;
	RECT				rec;
	char				DriveName[256];

	CDialog::OnInitDialog();
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	SetWindowText("PvrExplorer-Pro");
	m_SizeX = 0;
	m_hBitmap.LoadBitmap(IDB_BACKGROUND);
	if( m_hBitmap.GetBitmap(&bmInfo)){
		m_SizeX = bmInfo.bmWidth;
		m_SizeY = bmInfo.bmHeight;
		// now resize the window
		x = GetSystemMetrics(SM_CXSCREEN);
		y = GetSystemMetrics(SM_CYSCREEN);
		
		x = (x / 2) - ( m_SizeX / 2 );
		if( x < 0 )
			x = 0;
		y = (y / 2) - ( m_SizeY / 2 );
		if( y < 0 )
			y = 0;
		SetWindowPos(&wndTop,x,y,m_SizeX,m_SizeY, SWP_SHOWWINDOW); 
		m_SysButton = (CCustomButton *) new CCustomButton(IDB_SYSCLOSE_OUT, m_SizeX - 45,20,IDC_SYSCLOSE_CMD,this);
		m_AboutButton = (CCustomButton *) new CCustomButton(IDB_ABOUT_OUT, 20,20,IDC_ABOUT_CMD,this);
		m_AllButton = (CCustomButton *) new CCustomButton(IDB_ALL_OUT, 33,51,IDC_ALL_CMD,this);
		
		m_OptionButton = (CCustomButton *) new CCustomButton(IDB_OPTION_OUT, 130,51,IDC_OPTION_CMD,this);
		m_ExtractButton = (CCustomButton *) new CCustomButton(IDB_EXTRACT_OUT, 80,332,IDC_EXTRACT_CMD,this);
		//m_BtnBase = (TransparentBitMap *)new TransparentBitMap(IDB_BTN_BASE,80,332,this);
		
		m_Progress = (CCustomProgress *)new CCustomProgress(IDB_PROGRESS_TRENCH,125,362,IDC_PROGRESS_CMD,this);

		m_CancelButton = (CCustomButton *) new CCustomButton(IDB_CANCEL_OUT, 440,352,IDC_CANCEL_CMD,this);
		m_CancelButton->ShowWindow(SW_HIDE);
		
		m_501 = (TransparentBitMap *)new TransparentBitMap(IDB_PIC_501,600,332,this);
		m_522 = (TransparentBitMap *)new TransparentBitMap(IDB_PIC_522,600,332,this);
		m_721 = (TransparentBitMap *)new TransparentBitMap(IDB_PIC_721,600,332,this);
		for( x = 0; x < MAX_SCREEN_LINES; x++ ){
			m_chk[x] = (CCustomButton *) new CCustomButton(IDB_CHECK_OUT, 32,76 + (x * SEL_LINE_WIDTH),IDB_SELECTED_CMD,this);
			m_chk[x]->ShowWindow(SW_HIDE);
		}
		
		rec.top = 49;
		rec.left = 380;
		rec.bottom = 180;
		rec.right = 545;
		m_DrvSelect.Create(CBS_DROPDOWNLIST | WS_VISIBLE | WS_CHILD,rec,this,IDC_DRIVE_SELECT);
	
		rec.top = 74;
		rec.left = 500;
		rec.bottom = 328;
		rec.right = 562;
		ScrolBar.Create(SBS_VERT | SBS_RIGHTALIGN | WS_VISIBLE | WS_CHILD,rec,this,IDC_SCROLL_CMD);
		
		rec.top = 70;
		rec.left = 592;
		rec.bottom = 88;
		rec.right = 733;
		m_ChannelInfoText = (CCustomText *)new CCustomText(rec,this,IDB_BACKGROUND);

		rec.top = 97;
		rec.left = 592;
		rec.bottom = 299;
		rec.right = 734;
		m_DescText = (CCustomText *)new CCustomText(rec,this,IDB_BACKGROUND);
		m_DescText->SetDrawFlags(DT_CENTER | DT_WORDBREAK | DT_NOPREFIX);

		rec.top = 311;
		rec.left = 590;
		rec.bottom = 350;
		rec.right = 735;
		m_TimeText = (CCustomText *)new CCustomText(rec,this,IDB_BACKGROUND);
		m_TimeText->SetTextColor(RGB(0,255,0));
		m_TimeText->ShowWindow(SW_HIDE);

		rec.top = 390;
		rec.left = 94;
		rec.bottom = 425;
		rec.right = 440;
		m_FileNameText = (CCustomText *)new CCustomText(rec,this,IDB_BACKGROUND);
		m_TimeText->ShowWindow(SW_HIDE);

		if( m_Options.m_DebugChk ){ //then open the debug log file
			GetModuleFileName(GetModuleHandle(NULL),DriveName,255);
			j = strlen(DriveName);
			while( j > 0 && DriveName[j] != '\\')
				j--;
			DriveName[j] = 0x00;
			if( DriveName[ strlen(DriveName) - 1] != '\\')
				strcat(DriveName,"\\");
			strcat(DriveName,"DebugLog.txt");
			dbgLog.OpenLogFile(DriveName,LOG_FILE_NEW);
			dbgLog.PrintLnLog("<< PVR Explorer Pro Debug Log file >>");
		}

		// now here we scan for PVR HDD's
		m_DriveEnum.ScanForDrives();
		if( m_DriveEnum.GetNumberOfPVRDrives() == 0 ){ //then no drives found
			dbgLog.PrintLnLog("No PVR HDD's found connected to the system!");
			MessageBox("No PVR HDD's found connected to the system!\n Please check cables,jumper settings, and power connectors.","Error No PVR HDD",MB_OK | MB_ICONSTOP);
			j = MessageBox("Open Local Folder with *.tsp files in it?","Open Local Folder?",MB_YESNO | MB_ICONQUESTION);
			if( j == IDYES ){
				BROWSEINFO	br;
				char		FolderName[360];
				LPITEMIDLIST	idList;

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
					m_DriveEnum.AddPVRDrive(FolderName,PVR_TYPE_FILE);
				} else {
					OnSysClose();
					return TRUE;
				}
			} else {
				OnSysClose();
				return TRUE;
			}
		}
		// then add the found hdds to the combo box
		for( j = 0; j < m_DriveEnum.GetNumberOfPVRDrives(); j++ ){
			m_DriveEnum.GetDriveFriendlyName(DriveName,j);
			m_DrvSelect.AddString(DriveName);
		}
		m_DrvSelect.SetCurSel(0);
		m_OkToPaintList = true;
		OnSelchangeDrive();
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

//  when the drive selectio changes
void 
CPvrExplorerDlg::OnSelchangeDrive()
{
	int				j,type;
	DishPVR50X		*pvr50x;
	DishPVR522		*pvr522;
	DishPVR721		*pvr721;
	DishPVRFile		*pvrFile;
	DishPVR622		*pvr622;
	char			DriveName[256];

	j = m_DrvSelect.GetCurSel();	
	type = m_DriveEnum.GetPVRDriveType(j);
	m_DriveEnum.GetDriveName(DriveName,j);
	if( m_pvr != NULL){
		delete m_pvr;
		m_pvr = NULL;
	}
	if( type == PVR_TYPE_50X ){
		m_522->ShowWindow(SW_HIDE);
		m_721->ShowWindow(SW_HIDE);
		m_501->ShowWindow(SW_SHOWNORMAL);
		pvr50x = (DishPVR50X *)new DishPVR50X;
		pvr50x->SetPvrHdd( DriveName, PVR_TYPE_50X);
		m_pvr = (DishPvr *)pvr50x;
		
		dbgLog.PrintLog("<< 501 HDD Found Drive = ");
		dbgLog.PrintLnLog(DriveName);
	}
	if( type == PVR_TYPE_522 ){
		m_501->ShowWindow(SW_HIDE);
		m_721->ShowWindow(SW_HIDE);
		m_522->ShowWindow(SW_SHOWNORMAL);
		pvr522 = (DishPVR522 *)new DishPVR522;
		pvr522->SetPvrHdd( DriveName, PVR_TYPE_522);
		m_pvr = (DishPvr *)pvr522;
		
		dbgLog.PrintLog("<< 522 HDD Found Drive = ");
		dbgLog.PrintLnLog(DriveName);
	}
	if( type == PVR_TYPE_721 ){
		m_501->ShowWindow(SW_HIDE);
		m_522->ShowWindow(SW_HIDE);
		m_721->ShowWindow(SW_SHOWNORMAL);
		pvr721 = (DishPVR721 *)new DishPVR721;
		pvr721->SetPvrHdd( DriveName, PVR_TYPE_721);
		m_pvr = (DishPvr *)pvr721;
		
		dbgLog.PrintLog("<< 721 HDD Found Drive = ");
		dbgLog.PrintLnLog(DriveName);
	}
	if( type == PVR_TYPE_622 ){
		m_501->ShowWindow(SW_HIDE);
		m_522->ShowWindow(SW_HIDE);
		m_721->ShowWindow(SW_SHOWNORMAL);
		pvr622 = (DishPVR622 *)new DishPVR622;
		pvr622->SetPvrHdd( DriveName, PVR_TYPE_622);
		m_pvr = (DishPvr *)pvr622;
		
		dbgLog.PrintLog("<< 622 HDD Found Drive = ");
		dbgLog.PrintLnLog(DriveName);
	}
	if( type == PVR_TYPE_FILE ){
		m_501->ShowWindow(SW_HIDE);
		m_522->ShowWindow(SW_HIDE);
		m_721->ShowWindow(SW_HIDE);
		pvrFile = (DishPVRFile *)new DishPVRFile;
		pvrFile->SetPvrHdd( DriveName, PVR_TYPE_FILE);
		m_pvr = (DishPvr *)pvrFile;
		
		dbgLog.PrintLog("<< *.tsp Folder Found = ");
		dbgLog.PrintLnLog(DriveName);
	}
	if( m_pvr == NULL ){
		dbgLog.PrintLog("Failed to create pvr instance!");
		MessageBox("Failed to create pvr instance!, Exiting...","PVR Explorer Error",MB_OK | MB_ICONSTOP);
		OnSysClose();
		return;
	}
	// now get the number of programs
	m_NumPrograms = m_pvr->GetNumberOfPrograms();
	// now reset the scroll bar
	ScrolBar.SetScrollRange(0,m_NumPrograms,TRUE);
	ScrolBar.SetScrollPos(0,FALSE);
	if(	m_Selected != NULL){
		delete []m_Selected;
		m_Selected = NULL;
	}
	m_Selected = (int *)new int[m_NumPrograms];
	memset((void *)m_Selected,0x00,sizeof(int)*m_NumPrograms);
	m_CurSelected = 0;
	m_pvr->GetProgramInfo(NULL,NULL,NULL,m_CurProgramChName,&m_CurProgramCh,m_CurProgramDesc,0);
	
	sprintf(DriveName,"%d ( %s )",m_CurProgramCh,m_CurProgramChName);
	m_ChannelInfoText->SetItemText(DriveName);
	m_DescText->SetItemText(m_CurProgramDesc);
	InvalidateRect(NULL,FALSE);
	UpdateWindow();
	UpdateButtons();	
}
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPvrExplorerDlg::OnPaint() 
{
	CDC		CompDC;
	RECT	rec;
	//CPen	*OldPen,pen;
	CBrush	br;
	int		x,y;
	bool	paintList;

	
	CPaintDC dc(this); // device context for painting
	if (IsIconic())
	{

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		x = (rect.Width() - cxIcon + 1) / 2;
		y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		if( m_SizeX > 0 ){
			CompDC.CreateCompatibleDC(&dc);	//create tmp DC
			CompDC.SelectObject(&m_hBitmap);
			dc.BitBlt(0,0,m_SizeX,m_SizeY,&CompDC,0,0,SRCCOPY); //We do not have to do trasparent because the region did it for us
			CompDC.DeleteDC();				//no memory leaks!
			paintList = true;
			if( dc.m_ps.rcPaint.left > 525)
				paintList = false;
			if( dc.m_ps.rcPaint.right < 55)
				paintList = false;
			if( dc.m_ps.rcPaint.bottom < 78)
				paintList = false;
			if( dc.m_ps.rcPaint.top > (78 + (MAX_SCREEN_LINES * SEL_LINE_WIDTH)))
				paintList = false;
			if( paintList ){
				DisplayProgramList(&dc);
			}
			// now if we are not extracting then.. disp this
			rec.top = 68;
			rec.left = 590;
			rec.bottom = 90;
			rec.right = 735;

			br.CreateSolidBrush(RGB(0,255,0));
			dc.FrameRect(&rec,&br);

			rec.top = 95;
			rec.left = 590;
			rec.bottom = 300;
			rec.right = 735;
			dc.FrameRect(&rec,&br);

			if( !m_ExtractingMode ){
			//	dc.DrawText(m_CurProgramDesc,strlen(m_CurProgramDesc),&rec,DT_CENTER | DT_WORDBREAK | DT_NOPREFIX);
			} else {
 
			}

		}
			
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPvrExplorerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void 
CPvrExplorerDlg::OnSysClose()
{
	int		x;
	
	if( m_ExtractingMode )
		return;  //sorry no can do now
	delete m_SysButton;
	delete m_AboutButton;
	delete m_OptionButton;
	delete m_ExtractButton;
	delete m_AllButton;
	//delete m_BtnBase;
	delete m_Progress;
	delete	m_501;
	delete	m_522;
	delete	m_721;
	delete m_CancelButton;
	delete m_ChannelInfoText;
	delete m_DescText;
	delete m_TimeText;
	delete m_FileNameText;

	for( x = 0; x < MAX_SCREEN_LINES; x++ ){
		delete m_chk[x];
	}
	if( m_pvr != NULL ){
		delete m_pvr;
		m_pvr = NULL;
	}
	if(	m_Selected != NULL){
		delete []m_Selected;
		m_Selected = NULL;
	}
	if( m_ext != NULL ){
		delete m_ext;
		m_ext = NULL;
	}
	EndDialog(0);
}

void 
CPvrExplorerDlg::OnOptionBtn()
{
	Options		op;
	char		FileName[256];
	int			j;

	op.m_Options = &m_Options;
	op.DoModal();
	
	if( m_Options.m_DebugChk && !dbgLog.IsLogFileOpen()){ //then open the debug log file
		GetModuleFileName(GetModuleHandle(NULL),FileName,255);
		j = strlen(FileName);
		while( j > 0 && FileName[j] != '\\')
			j--;
		FileName[j] = 0x00;
		if( FileName[ strlen(FileName) - 1] != '\\')
			strcat(FileName,"\\");
		strcat(FileName,"DebugLog.txt");
		dbgLog.OpenLogFile(FileName,LOG_FILE_NEW);
		dbgLog.PrintLnLog("<< PVR Explorer Pro Debug Log file >>");
	}
	if( !m_Options.m_DebugChk && dbgLog.IsLogFileOpen()){ //then close the debug log file
		dbgLog.CloseLogFile();
	}
}

void 
CPvrExplorerDlg::OnExtractBtn()
{
	int		j,x,k;
	RECT	rec;
	char	ProgName[256];
	char	DateTime[128];
	char	epg[512];
	char	FileName[256];
	OPENFILENAME	op;
	char			StringFilter[256];
	char			*cPtr;
	int		AudioOptions;

//---------------------------------------------
//	Beta time limits here

//---------------------------------------------
	m_ExtractButton->ShowWindow(SW_HIDE);
	//m_BtnBase->ShowWindow(SW_SHOWNORMAL);
	m_TimeText->ShowWindow(SW_SHOWNORMAL);
	m_FileNameText->ShowWindow(SW_SHOWNORMAL);
	m_Progress->ShowWindow(SW_SHOWNORMAL);
	m_CancelButton->ShowWindow(SW_SHOWNORMAL);
	m_CancelButton->SetWindowPos(&wndTop   ,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW); 
	UpdateButtons();	
	for( j = 0; j < m_NumPrograms; j++){
		if( m_Selected[j] == BST_CHECKED ){
			//then we have something to extract
			m_CurSelected = j;
			if( j < m_TopOfPage || j > (m_TopOfPage + MAX_SCREEN_LINES)){
				m_TopOfPage = j;
			}
			rec.top = 74;
			rec.left = 54;
			rec.bottom = 328;
			rec.right = 562;
			InvalidateRect(&rec,FALSE);
			UpdateWindow();
			m_ExtractingMode = true;
			// now check for the auto generate name
			m_pvr->GetProgramInfo(ProgName,DateTime,NULL,NULL,NULL,epg,j);
			FileName[0] = 0x00;
			if( m_pvr->m_PvrType != PVR_TYPE_FILE){
				for( x = 0; x < (int)strlen(m_Options.m_AutoFormat); x++){
					if( m_Options.m_AutoFormat[x] != '%'){
						//then copy over the char
						FileName[strlen(FileName) + 1] = 0x00;
						FileName[strlen(FileName)] = m_Options.m_AutoFormat[x];
					} else {
						//then process the type
						x++;
						if( m_Options.m_AutoFormat[x] == 'n' || m_Options.m_AutoFormat[x] == 'N' ){
							strcat(FileName,ProgName);
						}
						if( m_Options.m_AutoFormat[x] == 'i' || m_Options.m_AutoFormat[x] == 'I' ){
							sprintf(StringFilter,"%s%d",FileName,j);
							strcpy(FileName,StringFilter);
						}
						if( m_Options.m_AutoFormat[x] == 'd' || m_Options.m_AutoFormat[x] == 'D' ){
							strcat(FileName,DateTime);
						}
						if( m_Options.m_AutoFormat[x] == 'e' || m_Options.m_AutoFormat[x] == 'E' ){
							//only the first line of the EPG
							for( k = 0; k < (int)strlen(epg); k++){
								if( epg[k] == 0x0A || epg[k] == 0x0D){
									epg[k] = 0x00;
									break;
								}
							}
							if( (int)strlen(epg) == 0 ){
								strcat(FileName,"(No Program Info)");
							} else {
								strcat(FileName,epg);
							}
						}
					}
				}
			}
			if( strlen(m_Options.m_AutoFormat) < 1 || !m_Options.m_AutoGenNameChk || m_pvr->m_PvrType == PVR_TYPE_FILE )
				sprintf(FileName,"%s(%d)",ProgName,j);
				
			for( x = 0; x < lstrlen(FileName); x++)
			{
				if( FileName[x] == ':' || FileName[x] == '@' || 
					FileName[x] == ';' || FileName[x] == '/' || 
					FileName[x] == '?' || FileName[x] == '>' || 
					FileName[x] == ',' || FileName[x] == '"' || 
					FileName[x] == '!' || FileName[x] == '&' ||
					FileName[x] >= '~' || FileName[x] <  ' ' )
				{
						FileName[x] = '_';	
				}
			}
			if( !m_Options.m_AutoGenNameChk ){
				op.lStructSize = sizeof(OPENFILENAME);
				op.hwndOwner = NULL;
				memset((void *)StringFilter,0x00,sizeof(StringFilter));
				cPtr = &StringFilter[0];
				lstrcpy(cPtr,"All Files");
				cPtr += 10;
				lstrcpy(cPtr,"*.*");
				op.lpstrFilter = StringFilter;
				op.lpstrCustomFilter = NULL;
				op.nFilterIndex = 0;
				FileName[0] = 0x00;
				op.lpstrFile = FileName;
				op.nMaxFile = 256;
				op.lpstrFileTitle = NULL;
				op.lpstrInitialDir = NULL;
				cPtr = "Save Name Stream files...";
				op.lpstrTitle = cPtr;
				op.Flags = OFN_LONGNAMES;
				op.lpstrDefExt = NULL;
				if( !GetSaveFileName( &op )){
					m_ExtractingMode = false;
					return;
				}					
				x = lstrlen(FileName) - 1;
				while( x > 0 && FileName[x] != '\\')
					x--;
				if( x == 0 ){
					m_ExtractingMode = false;
					return;
				}					
				FileName[x] = 0x00;
				cPtr = &FileName[x+1];
				strcpy((char *)m_Options.m_DefaultPath,FileName);
				strcpy(FileName,cPtr);
			}
			if( m_Options.m_DefaultPath[ strlen((char *)m_Options.m_DefaultPath) - 1] != '\\')
				strcat((char *)m_Options.m_DefaultPath,"\\");
			strcpy(m_OutputFileName,(char *)m_Options.m_DefaultPath);
			strcat(m_OutputFileName,FileName);
			m_FileNameText->SetItemText(m_OutputFileName);
			dbgLog.PrintLnLog("Extraction starting:");

			dbgLog.PrintLnLog("Getting Audio Options...");
			AudioOptions = m_pvr->GetAudioOptions(m_CurSelected);
			dbgLog.Sprintf("Audio Options = %X",AudioOptions);
			if( AudioOptions == 0 ){
				MessageBox("Possible Encrypted stream, unable to extract program!","PvrExplorerPro Warning...",MB_OK | MB_ICONSTOP);
				m_Selected[m_CurSelected] = BST_UNCHECKED;
				UpdateButtons();	
				m_ExtractingMode = false;
				continue;
			}
			m_Options.m_AudioIsAC3 = false;
			if( AudioOptions & HAS_AUDIO_AC3 )
				m_Options.m_AudioIsAC3 = true;

			//m_ecpy = (ExtractCopy *)new  ExtractCopy;
			//m_ecpy->StartExtractProcess(m_pvr,j,(char *)m_Options.m_DefaultPath,FileName);
			
			m_ext = (ExtractProgram *)new ExtractProgram;
			m_ext->m_Log = &m_Log;
			m_TicCount = 0;
			strcpy(m_ProcessRate,"Rate: --\n");
			strcpy(m_TimeRemaining,"Est Time: --\n");
			m_ext->StartExtractProcess(m_pvr,j,&m_Options,FileName);


			m_DescText->SetDrawFlags(DT_LEFT | DT_WORDBREAK | DT_NOPREFIX);
			Sleep(1500);
			SetTimer(1,500,NULL);
			dbgLog.PrintLnLog("Extraction started: timers started:");
			//SetTimer(3,500,NULL);
			return;
		}
	}
	m_ExtractButton->ShowWindow(SW_SHOWNORMAL);
	m_Progress->ShowWindow(SW_HIDE);
	m_CancelButton->ShowWindow(SW_HIDE);
	m_DescText->SetDrawFlags(DT_CENTER | DT_WORDBREAK | DT_NOPREFIX);
	m_TimeText->ShowWindow(SW_HIDE);
	m_FileNameText->ShowWindow(SW_HIDE);
	//if we are here then there is no more to execute so check the exit app button
	if( m_Options.m_CloseChk )
		OnSysClose();
}

void
CPvrExplorerDlg::OnCancelBtn()
{
	m_ext->Cancel();
}



void CPvrExplorerDlg::OnTimer(UINT nIDEvent) 
{
	STREAM_INFO		sInfo;
	bool			done;
	int				h,m,s,f;
	double			x,r;
	char			Line[512];

	if( nIDEvent == 1 ){
		m_TicCount++;
		m_ext->GetProgressInfo(&sInfo);
		x = (double)sInfo.FrameNumber / sInfo.VideoRate;
		h = (int)x / (60 * 60);
		x -= h * (60 * 60);
		m = (int)x / 60;
		x -= m * 60;
		s = (int)x;
		x -= s;
		f = (int)(x * sInfo.VideoRate);
		sprintf(m_VideoInfoStr,"Video: %dx%d at %02.02ffps\n", sInfo.VideoX,sInfo.VideoY,sInfo.VideoRate);
		sprintf(m_AudioInfoStr,"Audio: %s at %dKbs offset:%f s\n", sInfo.AudioType,sInfo.AudioRate,sInfo.AudioOffset);
		sprintf(m_BytesInfoStr,"Bytes Processed: %10I64d\nTotal Bytes %14I64d\n",sInfo.TotalBytesProcessed,sInfo.TotalBytesToProcess);
		sprintf(m_FrameTimeStr,"%d:%02d:%02d:%02d",h,m,s,f);
		
		r = (double)sInfo.TotalBytesProcessed;
		r /= (double)m_TicCount;
		r *= (double)2;    //this is now bytes per second
		x = r / (double)1000000;  // now MB per second

		sprintf(m_ProcessRate,"Rate: %0.02f MBs\n",x);
		x = (double)(sInfo.TotalBytesToProcess - sInfo.TotalBytesProcessed);
		if( r > 0 ){
			x /= r;			//x is now in seconds
			
			h = (int)x / (60 * 60);
			x -= h * (60 * 60);
			m = (int)x / 60;
			x -= m * 60;
			s = (int)x;
		
			sprintf(m_TimeRemaining,"Est Time: %d:%02d:%02d\n",h,m,s);
		}
			
		
		x = 0;
		if( sInfo.TotalBytesToProcess != 0 )
			x = (double)sInfo.TotalBytesProcessed / (double)sInfo.TotalBytesToProcess;
		x *= 100;
		m_Progress->SetPosition((ULONG)x);
		strcpy(Line,m_VideoInfoStr);
		strcat(Line,m_AudioInfoStr);
		strcat(Line,m_BytesInfoStr);
		strcat(Line,m_ProcessRate);
		strcat(Line,m_TimeRemaining);
		m_DescText->SetItemText(Line);
		m_TimeText->SetItemText(m_FrameTimeStr);
		done = false;
		if( sInfo.Status == EXTRACT_STATUS_IDLE ){
			KillTimer(1);
			done = true;
		}
		if( sInfo.Status == EXTRACT_STATUS_CANCELED ){
			KillTimer(1);
			MessageBox("Program extraction canceled.","PVR Explorer Message",MB_OK | MB_ICONINFORMATION);
			done = true;
		}
		if( sInfo.Status == EXTRACT_STATUS_ERROR ){
 			KillTimer(1);
			MessageBox("Program extraction FAILED!","PVR Explorer Message",MB_OK | MB_ICONSTOP);
			done = true;
		} 
		if( done ){
			m_ExtractingMode = false;
			if( m_Options.m_LogChk ){
				m_Log.PrintLnLog("<< Stream Information >> ");
				m_Log.PrintLnLog(" ");
				m_Log.PrintLnLog(m_VideoInfoStr);
				m_Log.PrintLnLog(m_AudioInfoStr);
				m_Log.PrintLnLog(m_BytesInfoStr);
				m_Log.PrintLog("Stream Play Length: ");				
				m_Log.PrintLnLog(m_FrameTimeStr);
				m_Log.PrintLog("Proccessing ");				
				m_Log.PrintLnLog(m_ProcessRate);
				m_Log.PrintLnLog(" ");
				m_Log.PrintLnLog("<< End Log File >> ");
			}
			m_Log.CloseLogFile();
			delete m_ext;
			m_ext = NULL;
			m_Selected[m_CurSelected] = BST_UNCHECKED;
			OnExtractBtn();
		}
	}
	if( nIDEvent == 2 ){
		KillTimer(2);
	}
	if( nIDEvent == 3 ){
		f = m_ecpy->GetProgressPercent();
		m_Progress->SetPosition((ULONG)f);
		if( f == 101 ){
			KillTimer(3);
			m_ExtractingMode = false;
			delete m_ecpy;
			m_ecpy = NULL;
			m_Selected[m_CurSelected] = BST_UNCHECKED;
			OnExtractBtn();
		}
		
	}
	CDialog::OnTimer(nIDEvent);
}

void 
CPvrExplorerDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	RECT	rec;
	
	GetClientRect(&rec);
	rec.top = 74;
	rec.left = 54;
	rec.bottom = 328;
	rec.right = 562;
	switch( nSBCode ){
		case SB_TOP:
			m_TopOfPage = 0;
			pScrollBar->SetScrollPos(0,TRUE);
			InvalidateRect(&rec,FALSE);
			UpdateWindow();
			UpdateButtons();	
			break;
		case SB_BOTTOM:
			if( (MAX_SCREEN_LINES + m_TopOfPage) > m_NumPrograms )
				break;
			m_TopOfPage = m_NumPrograms - MAX_SCREEN_LINES;
			if( m_TopOfPage < 0 )
				m_TopOfPage = 0;
			pScrollBar->SetScrollPos(m_TopOfPage,TRUE);
			InvalidateRect(&rec,FALSE);
			UpdateWindow();
			UpdateButtons();	
			break;
		case SB_LINEUP:
			if( m_TopOfPage > 0 )
			{
				m_TopOfPage--;
				pScrollBar->SetScrollPos(m_TopOfPage,TRUE);
				InvalidateRect(&rec,FALSE);
				UpdateWindow();
				UpdateButtons();	
			}
			break;
		case SB_LINEDOWN:
			if( (MAX_SCREEN_LINES + m_TopOfPage) > m_NumPrograms )
				break;
			m_TopOfPage++;
			pScrollBar->SetScrollPos(m_TopOfPage,TRUE);
			InvalidateRect(&rec,FALSE);
			UpdateWindow();
			UpdateButtons();	
			break;
		case SB_PAGEUP:
			m_TopOfPage -= MAX_SCREEN_LINES;
			if( m_TopOfPage < 0 )
				m_TopOfPage = 0;
			pScrollBar->SetScrollPos(m_TopOfPage,TRUE);
			InvalidateRect(&rec,FALSE);
			UpdateWindow();
			UpdateButtons();	
			break;
		case SB_PAGEDOWN:
			m_TopOfPage += MAX_SCREEN_LINES;
			if( m_TopOfPage >= m_NumPrograms )
			{
				m_TopOfPage = m_NumPrograms - MAX_SCREEN_LINES;
				if( m_TopOfPage < 0 )
					m_TopOfPage = 0;
			}
			pScrollBar->SetScrollPos(m_TopOfPage,TRUE);
			InvalidateRect(&rec,FALSE);
			UpdateWindow();
			UpdateButtons();	
			break;
		case SB_THUMBPOSITION:
			break;
		case SB_THUMBTRACK:
			m_TopOfPage = nPos;
			if( m_TopOfPage >= m_NumPrograms )
				m_TopOfPage = m_NumPrograms - 1;
			pScrollBar->SetScrollPos(m_TopOfPage,TRUE);
			InvalidateRect(&rec,FALSE);
			UpdateWindow();
			UpdateButtons();	
			break;
		case SB_ENDSCROLL:
			break;
	}

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CPvrExplorerDlg::DisplayProgramList(CDC *dc)
{
	int		j;
	char	Name[128];
	char	DateTime[64];
	char	Length[32];
	//CDC		*dc;
	RECT	rec;
	CPen	*OldPen,pen;
	CBrush	br;

	//dc = GetDC();
	if( dc == NULL )
		return;
	if( m_pvr == NULL)
		return;

	
	rec.left = 55;
	rec.right = 525;
	for( j = m_TopOfPage; j < m_NumPrograms && j < (m_TopOfPage + MAX_SCREEN_LINES); j++){
		Name[0] = DateTime[0] = Length[0] = 0;
		m_pvr->GetProgramInfo(Name,DateTime,Length,NULL,NULL,NULL,j);
		//now print the line
		rec.top = 78 + ((j - m_TopOfPage) * SEL_LINE_WIDTH);
		rec.bottom = rec.top + SEL_LINE_WIDTH;

		if( j == m_CurSelected ){
			rec.left = 54;
			rec.bottom -= 7;
			br.CreateSolidBrush(RGB(89,201,238));
			dc->FillRect(&rec,&br);
			rec.bottom = rec.top + SEL_LINE_WIDTH;
			dc->SetBkMode(TRANSPARENT);  
		}
		rec.left = 55;
		dc->DrawText(Name,strlen(Name),&rec,DT_LEFT);
		rec.left = 335;
		dc->DrawText(DateTime,strlen(DateTime),&rec,DT_LEFT);
		rec.left = 485;
		dc->DrawText(Length,strlen(Length),&rec,DT_LEFT);
		// now do the selections
		//m_chk[j - m_TopOfPage]->ShowWindow(SW_SHOWNORMAL);
		//m_chk[j - m_TopOfPage]->SetCheck(m_Selected[j]);
		rec.left = 54;
		rec.bottom -= 7;
		if( j == m_CurSelected ){
			dc->SetBkMode(OPAQUE);  
			pen.CreatePen(PS_DOT,1,RGB(0,0,0));
			//br.CreateHatchBrush(
			//dc->FrameRect(&rec,
			OldPen = dc->SelectObject(&pen);
			dc->MoveTo(rec.left,rec.top);
			dc->LineTo(rec.right,rec.top);
			dc->LineTo(rec.right,rec.bottom);
			dc->LineTo(rec.left,rec.bottom);
			dc->LineTo(rec.left,rec.top);
			dc->SelectObject(OldPen);
		}
	}

}

void CPvrExplorerDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	RECT	rec;
	int		j;
	char	Line[80];

	rec.top = 74;
	rec.left = 54;
	rec.bottom = 328;
	rec.right = 562;
	if( nFlags & MK_LBUTTON ){
		if( point.x >= rec.left && point.x <= rec.right ){
			if( point.y >= rec.top && point.y <= rec.bottom ){
				if( m_ExtractingMode )
					return;
				j = (point.y - rec.top) / SEL_LINE_WIDTH;
				j += m_TopOfPage;
				// then we are in the selection area
				//figure out which line
				if( j < m_NumPrograms && j != m_CurSelected){
				//Invalidate the old selection
					if( m_CurSelected >= m_TopOfPage && m_CurSelected < (m_TopOfPage + MAX_SCREEN_LINES)){
						rec.top = 74 + ((m_CurSelected - m_TopOfPage) * SEL_LINE_WIDTH);
						rec.bottom = rec.top + SEL_LINE_WIDTH;
						InvalidateRect(&rec,FALSE);
					}
					m_CurSelected = j;
					m_pvr->GetProgramInfo(NULL,NULL,NULL,m_CurProgramChName,&m_CurProgramCh,m_CurProgramDesc,j);
					rec.top = 74 + ((m_CurSelected - m_TopOfPage) * SEL_LINE_WIDTH);
					rec.bottom = rec.top + SEL_LINE_WIDTH;
					InvalidateRect(&rec,FALSE);
					InvalidateRect(&rec,FALSE);
					UpdateWindow();
					UpdateButtons();	
					
					sprintf(Line,"%d ( %s )",m_CurProgramCh,m_CurProgramChName);
					m_ChannelInfoText->SetItemText(Line);
					m_DescText->SetItemText(m_CurProgramDesc);
				}
			}
		}
	}
	
	CDialog::OnLButtonDown(nFlags, point);
}

BOOL CPvrExplorerDlg::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	//return CDialog::OnEraseBkgnd(pDC);
	return(0);
}

void 
CPvrExplorerDlg::OnSelectionBtn()
{
	int		j;
	RECT	rec;
	char	Line[80];

	for( j = m_TopOfPage; j < m_NumPrograms && j < (m_TopOfPage + MAX_SCREEN_LINES); j++){
		m_Selected[j] = m_chk[j - m_TopOfPage]->GetCheck();
		if( m_chk[j - m_TopOfPage]->IsLastClicked() ){
			rec.left = 54;
			rec.right = 562;
			if( m_CurSelected >= m_TopOfPage && m_CurSelected < (m_TopOfPage + MAX_SCREEN_LINES)){
				rec.top = 74 + ((m_CurSelected - m_TopOfPage) * SEL_LINE_WIDTH);
				rec.bottom = rec.top + SEL_LINE_WIDTH;
				InvalidateRect(&rec,FALSE);
			}
			if( m_ExtractingMode )
				return;
			m_CurSelected = j;
			m_pvr->GetProgramInfo(NULL,NULL,NULL,m_CurProgramChName,&m_CurProgramCh,m_CurProgramDesc,j);
			rec.top = 74 + ((m_CurSelected - m_TopOfPage) * SEL_LINE_WIDTH);
			rec.bottom = rec.top + SEL_LINE_WIDTH;
			InvalidateRect(&rec,FALSE);
			UpdateWindow();
			sprintf(Line,"%d ( %s )",m_CurProgramCh,m_CurProgramChName);
			m_ChannelInfoText->SetItemText(Line);
			m_DescText->SetItemText(m_CurProgramDesc);
		}
	}
}

void CPvrExplorerDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	RECT	rec;
	int		j;

	rec.top = 74;
	rec.left = 55;
	rec.bottom = 328;
	rec.right = 562;
	if( nFlags & MK_LBUTTON ){
		if( point.x >= rec.left && point.x <= rec.right ){
			if( point.y >= rec.top && point.y <= rec.bottom ){
				// then we are in the selection area
				//figure out which line
				j = (point.y - rec.top) / SEL_LINE_WIDTH;
				j += m_TopOfPage;
				if( j < m_NumPrograms ){
					m_Selected[j] = ( m_Selected[j] == BST_CHECKED )? BST_UNCHECKED: BST_CHECKED;
					m_chk[j - m_TopOfPage]->SetCheck(m_Selected[j]);
				}
			}
		}
	}
	
	CDialog::OnLButtonDblClk(nFlags, point);
}

UINT CPvrExplorerDlg::OnNcHitTest(CPoint point) 
{
	RECT	rec;
	int		x,y;

	GetWindowRect(&rec);
	x = point.x - rec.left;
	y = point.y - rec.top;
	if( x >= 54 && x <= 562 && y >= 74 && y <= 330)
		return(HTCLIENT);
	return(HTCAPTION);
}

void 
CPvrExplorerDlg::OnAboutBtn()
{
	AboutBox	ab;

	ab.DoModal();
}

void 
CPvrExplorerDlg::OnAllBtn()
{
	int	j,state;

	state = BST_CHECKED;
	if( m_Selected[0] == BST_CHECKED && m_Selected[m_NumPrograms - 1] == BST_CHECKED)
		state = BST_UNCHECKED;
	for( j = 0; j < m_NumPrograms ; j++){
		m_Selected[j] = state;
	}
	UpdateButtons();

}




void CPvrExplorerDlg::UpdateButtons()
{
	int		j;

	for( j = m_TopOfPage; j < m_NumPrograms && j < (m_TopOfPage + MAX_SCREEN_LINES); j++){
		// now do the selections
		m_chk[j - m_TopOfPage]->ShowWindow(SW_SHOWNORMAL);
		m_chk[j - m_TopOfPage]->SetCheck(m_Selected[j]);
	}
	// now see if we need to hide some buttons
	j -= m_TopOfPage;
	while( j < MAX_SCREEN_LINES ){
		m_chk[j]->ShowWindow(SW_HIDE);
		j++;
	}

}
