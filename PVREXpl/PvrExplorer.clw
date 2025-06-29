; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CCustomText
LastTemplate=generic CWnd
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "PvrExplorer.h"

ClassCount=9
Class1=CPvrExplorerApp
Class2=CPvrExplorerDlg

ResourceCount=4
Resource1=IDR_MAINFRAME
Class5=TransparentBitMap
Class6=CCustomScroll
Class3=CCustomButton
Class4=CCustomProgress
Resource2=IDD_OPTIONS
Class7=Options
Resource3=IDD_PVREXPLORER_DIALOG
Class8=AboutBox
Class9=CCustomText
Resource4=IDD_ABOUT

[CLS:CPvrExplorerApp]
Type=0
HeaderFile=PvrExplorer.h
ImplementationFile=PvrExplorer.cpp
Filter=N
LastObject=CPvrExplorerApp

[CLS:CPvrExplorerDlg]
Type=0
HeaderFile=PvrExplorerDlg.h
ImplementationFile=PvrExplorerDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CPvrExplorerDlg



[DLG:IDD_PVREXPLORER_DIALOG]
Type=1
Class=CPvrExplorerDlg
ControlCount=0

[CLS:CCustomButton]
Type=0
HeaderFile=CustomButton.h
ImplementationFile=CustomButton.cpp
BaseClass=CButton
Filter=D
VirtualFilter=BWC

[CLS:CCustomProgress]
Type=0
HeaderFile=CustomProgress.h
ImplementationFile=CustomProgress.cpp
BaseClass=CProgressCtrl
Filter=W
VirtualFilter=NWC

[CLS:TransparentBitMap]
Type=0
HeaderFile=TransparentBitMap.h
ImplementationFile=TransparentBitMap.cpp
BaseClass=CWnd
Filter=W
VirtualFilter=WC
LastObject=TransparentBitMap

[CLS:CCustomScroll]
Type=0
HeaderFile=CustomScroll.h
ImplementationFile=CustomScroll.cpp
BaseClass=CWnd
Filter=W
VirtualFilter=WC
LastObject=CCustomScroll

[DLG:IDD_OPTIONS]
Type=1
Class=Options
ControlCount=18
Control1=IDCANCEL,button,1342242816
Control2=IDC_VIDEO_CHK,button,1342242819
Control3=IDC_AUDIO_CHK,button,1342242819
Control4=IDC_RENDER_CHK,button,1342242819
Control5=IDC_AUTO_NAME,button,1342242819
Control6=IDC_STATIC,static,1342308352
Control7=IDC_OUTPUT_PATH,static,1342312448
Control8=IDC_BROWSE,button,1342242816
Control9=IDC_STATIC,button,1342177287
Control10=IDC_LOG_FILE_CHK,button,1342242819
Control11=IDC_CLOSE_CHK,button,1342242819
Control12=IDC_DEBUG_CHK,button,1342242819
Control13=IDC_STATIC,button,1342177287
Control14=IDC_APPLY,button,1342242817
Control15=IDC_AUTO_FORMAT,edit,1350631552
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,static,1342312448
Control18=IDC_STATIC,static,1342308352

[CLS:Options]
Type=0
HeaderFile=Options.h
ImplementationFile=Options.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_APPLY

[DLG:IDD_ABOUT]
Type=1
Class=AboutBox
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342308353
Control3=IDC_STATIC,static,1342312448
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1350565902

[CLS:AboutBox]
Type=0
HeaderFile=AboutBox.h
ImplementationFile=AboutBox.cpp
BaseClass=CDialog
Filter=D
LastObject=AboutBox

[CLS:CCustomText]
Type=0
HeaderFile=CustomText.h
ImplementationFile=CustomText.cpp
BaseClass=CWnd
Filter=W
VirtualFilter=WC

