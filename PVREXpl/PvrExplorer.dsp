# Microsoft Developer Studio Project File - Name="PvrExplorer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PvrExplorer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PvrExplorer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PvrExplorer.mak" CFG="PvrExplorer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PvrExplorer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "PvrExplorer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PvrExplorer - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /out:"Release/PvrExplorerPro.exe"

!ELSEIF  "$(CFG)" == "PvrExplorer - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/PvrExplorerPro.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "PvrExplorer - Win32 Release"
# Name "PvrExplorer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AboutBox.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomButton.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomProgress.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomText.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\DishPvr.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\DishPVR50X.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\DishPVR522.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\DishPVR622.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\DishPVR721.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\DishPVRFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\DishPvrHdd.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\DriveIOBase.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\EnumPVRDrives.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\ExtractCopy.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\ExtractProgram.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\HDD50X.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\HDD522.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\HDD622.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\HDD721.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\HDDFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\HDDPVR.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\LOGFILE.CPP
# End Source File
# Begin Source File

SOURCE=.\Options.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\PACKOUT.CPP
# End Source File
# Begin Source File

SOURCE=.\Engine\PESPipe.cpp
# End Source File
# Begin Source File

SOURCE=.\PvrExplorer.cpp
# End Source File
# Begin Source File

SOURCE=.\PvrExplorer.rc
# End Source File
# Begin Source File

SOURCE=.\PvrExplorerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\REPES2K.CPP
# End Source File
# Begin Source File

SOURCE=.\Engine\REPESAUDIO.CPP
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TransparentBitMap.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\WHICHSTREAMS.CPP
# End Source File
# Begin Source File

SOURCE=.\Engine\WriteCache.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Engine\522Catalog.h
# End Source File
# Begin Source File

SOURCE=.\AboutBox.h
# End Source File
# Begin Source File

SOURCE=.\CustomButton.h
# End Source File
# Begin Source File

SOURCE=.\CustomProgress.h
# End Source File
# Begin Source File

SOURCE=.\CustomText.h
# End Source File
# Begin Source File

SOURCE=.\Engine\DishPvr.h
# End Source File
# Begin Source File

SOURCE=.\Engine\DishPVR50X.h
# End Source File
# Begin Source File

SOURCE=.\Engine\DishPVR522.h
# End Source File
# Begin Source File

SOURCE=.\Engine\DishPVR622.h
# End Source File
# Begin Source File

SOURCE=.\Engine\DishPVR721.h
# End Source File
# Begin Source File

SOURCE=.\Engine\DishPVRFile.h
# End Source File
# Begin Source File

SOURCE=.\Engine\DishPvrHdd.h
# End Source File
# Begin Source File

SOURCE=.\Engine\DriveIOBase.h
# End Source File
# Begin Source File

SOURCE=.\Engine\EnumPVRDrives.h
# End Source File
# Begin Source File

SOURCE=.\Engine\ExtractCopy.h
# End Source File
# Begin Source File

SOURCE=.\Engine\ExtractProgram.h
# End Source File
# Begin Source File

SOURCE=.\Engine\GlobalDefs.h
# End Source File
# Begin Source File

SOURCE=.\Engine\HDD50X.h
# End Source File
# Begin Source File

SOURCE=.\Engine\HDD522.h
# End Source File
# Begin Source File

SOURCE=.\Engine\HDD622.h
# End Source File
# Begin Source File

SOURCE=.\Engine\HDD721.h
# End Source File
# Begin Source File

SOURCE=.\Engine\HDDPVR.h
# End Source File
# Begin Source File

SOURCE=.\Engine\LOGFILE.H
# End Source File
# Begin Source File

SOURCE=.\Engine\NetDiskPass.h
# End Source File
# Begin Source File

SOURCE=.\Options.h
# End Source File
# Begin Source File

SOURCE=.\Engine\PACKOUT.H
# End Source File
# Begin Source File

SOURCE=.\Engine\Partition.h
# End Source File
# Begin Source File

SOURCE=.\Engine\PESPipe.h
# End Source File
# Begin Source File

SOURCE=.\Engine\Platform.h
# End Source File
# Begin Source File

SOURCE=.\PvrExplorer.h
# End Source File
# Begin Source File

SOURCE=.\PvrExplorerDlg.h
# End Source File
# Begin Source File

SOURCE=.\Engine\REPES2K.H
# End Source File
# Begin Source File

SOURCE=.\Engine\REPESAUDIO.H
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TransparentBitMap.h
# End Source File
# Begin Source File

SOURCE=.\Engine\WHICHSTREAMS.H
# End Source File
# Begin Source File

SOURCE=.\Engine\WriteCache.h
# End Source File
# Begin Source File

SOURCE=.\Engine\xfsLinux.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\50X.bmp
# End Source File
# Begin Source File

SOURCE=.\res\522.bmp
# End Source File
# Begin Source File

SOURCE=.\res\721.bmp
# End Source File
# Begin Source File

SOURCE=.\res\AboutIn.bmp
# End Source File
# Begin Source File

SOURCE=.\res\AboutOut.bmp
# End Source File
# Begin Source File

SOURCE=.\res\AboutOver.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Allin.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Allout.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Allover.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Background2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CancelIn.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CancelInx.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CancelOut.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CancelOutx.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CancelOver.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CancelOverx.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CheckOff.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CheckOn.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CheckOver.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ExtractButtonBase.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ExtractButtonIn.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ExtractButtonOut.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ExtractButtonOver.bmp
# End Source File
# Begin Source File

SOURCE=.\res\harvey.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Optionin.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Optionout.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Optionover.bmp
# End Source File
# Begin Source File

SOURCE=.\res\pic_501.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ProgressBody.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ProgressHead.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ProgressTail.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ProgressTrench.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PvrExplorer.ico
# End Source File
# Begin Source File

SOURCE=.\res\PvrExplorer.rc2
# End Source File
# Begin Source File

SOURCE=.\res\ScrolldnIn.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ScrolldnOut.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ScrolldnOver.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ScrollThumb.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Scrolltrench.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ScrollupIn.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ScrollupOut.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ScrollupOver.bmp
# End Source File
# Begin Source File

SOURCE=.\res\SysClosein.bmp
# End Source File
# Begin Source File

SOURCE=.\res\SysCloseout.bmp
# End Source File
# Begin Source File

SOURCE=.\res\SysCloseover.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
