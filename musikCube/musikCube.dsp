# Microsoft Developer Studio Project File - Name="musikCube" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=musikCube - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "musikCube.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "musikCube.mak" CFG="musikCube - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "musikCube - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "musikCube - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "musikCube - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /I ""../musikCore/3rdparty/taglib-1.3.1/taglib/"" /I ""../musikCore/3rdparty/taglib-1.3.1/taglib/taglib/"" /I "../musikCore/3rdparty/OpenThreads/include/" /I "../musikCore/3rdparty/OpenThreads/include/OpenThreads/" /I "../musikCore/3rdparty/sqlite3/" /I "../musikCore/3rdparty/bass/include" /I "../musikCore/include/" /ZI /W3 /Od /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "_AFXDLL" /D "_UNICODE" /Gm /GR /Yu"stdafx.h" /GZ /c /GX 
# ADD CPP /nologo /MDd /I ""../musikCore/3rdparty/taglib-1.3.1/taglib/"" /I ""../musikCore/3rdparty/taglib-1.3.1/taglib/taglib/"" /I "../musikCore/3rdparty/OpenThreads/include/" /I "../musikCore/3rdparty/OpenThreads/include/OpenThreads/" /I "../musikCore/3rdparty/sqlite3/" /I "../musikCore/3rdparty/bass/include" /I "../musikCore/include/" /ZI /W3 /Od /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "_AFXDLL" /D "_UNICODE" /Gm /GR /Yu"stdafx.h" /GZ /c /GX 
# ADD BASE MTL /nologo /D"_DEBUG" /win32 
# ADD MTL /nologo /D"_DEBUG" /win32 
# ADD BASE RSC /l 1033 /d "_DEBUG" /i "$(IntDir)" 
# ADD RSC /l 1033 /d "_DEBUG" /i "$(IntDir)" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo 
# ADD BSC32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib bass.lib bassenc.lib basscd.lib musikCore_uD.lib taglib_uD.lib OpenThreads_uD.lib /nologo /out:"bin\musikCubeD.exe" /incremental:yes /libpath:"../musikCore/lib/" /debug /pdbtype:sept /subsystem:windows /machine:ix86 
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib bass.lib bassenc.lib basscd.lib musikCore_uD.lib taglib_uD.lib OpenThreads_uD.lib /nologo /out:"bin\musikCubeD.exe" /incremental:yes /libpath:"../musikCore/lib/" /debug /pdbtype:sept /subsystem:windows /machine:ix86 

!ELSEIF  "$(CFG)" == "musikCube - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /I ""../musikCore/3rdparty/taglib-1.3.1/taglib/"" /I ""../musikCore/3rdparty/taglib-1.3.1/taglib/taglib/"" /I "../musikCore/3rdparty/OpenThreads/include/" /I "../musikCore/3rdparty/OpenThreads/include/OpenThreads/" /I "../musikCore/3rdparty/sqlite3/" /I "../musikCore/3rdparty/bass/include" /I "../musikCore/include/" /W3 /Ox /Og /Ot /GA /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_AFXDLL" /D "_UNICODE" /GR /Yu"stdafx.h" /c /GX 
# ADD CPP /nologo /MD /I ""../musikCore/3rdparty/taglib-1.3.1/taglib/"" /I ""../musikCore/3rdparty/taglib-1.3.1/taglib/taglib/"" /I "../musikCore/3rdparty/OpenThreads/include/" /I "../musikCore/3rdparty/OpenThreads/include/OpenThreads/" /I "../musikCore/3rdparty/sqlite3/" /I "../musikCore/3rdparty/bass/include" /I "../musikCore/include/" /W3 /Ox /Og /Ot /GA /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_AFXDLL" /D "_UNICODE" /GR /Yu"stdafx.h" /c /GX 
# ADD BASE MTL /nologo /D"NDEBUG" /win32 
# ADD MTL /nologo /D"NDEBUG" /win32 
# ADD BASE RSC /l 1033 /d "NDEBUG" /i "$(IntDir)" 
# ADD RSC /l 1033 /d "NDEBUG" /i "$(IntDir)" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo 
# ADD BSC32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib bass.lib bassenc.lib basscd.lib musikCore_u.lib taglib_u.lib OpenThreads_u.lib /nologo /out:"bin\musikCube.exe" /incremental:no /libpath:"../musikCore/lib/" /pdbtype:sept /subsystem:windows /opt:ref /opt:icf /machine:ix86 
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib bass.lib bassenc.lib basscd.lib musikCore_u.lib taglib_u.lib OpenThreads_u.lib /nologo /out:"bin\musikCube.exe" /incremental:no /libpath:"../musikCore/lib/" /pdbtype:sept /subsystem:windows /opt:ref /opt:icf /machine:ix86 

!ENDIF

# Begin Target

# Name "musikCube - Win32 Debug"
# Name "musikCube - Win32 Release"
# Begin Group "Main Frame"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\res\m3icon.ico
# End Source File
# Begin Source File

SOURCE=.\musikCubeFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\musikCubeFrame.h
# End Source File
# Begin Source File

SOURCE=.\musikCubeMessages.cpp
# End Source File
# Begin Source File

SOURCE=.\musikCubeMessages.h
# End Source File
# Begin Source File

SOURCE=.\musikGenres.h
# End Source File
# Begin Source File

SOURCE=.\musikTunage.cpp
# End Source File
# Begin Source File

SOURCE=.\musikTunage.h
# End Source File
# Begin Group "Controls"

# PROP Default_Filter ""
# Begin Group "musikListCtrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\musikListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\musikListCtrl.h
# End Source File
# End Group
# Begin Group "musikDockBar"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\musikDockBar.cpp
# End Source File
# Begin Source File

SOURCE=.\musikDockBar.h
# End Source File
# End Group
# Begin Group "musikEditInPlace"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\musikEditInPlace.cpp
# End Source File
# Begin Source File

SOURCE=.\musikEditInPlace.h
# End Source File
# End Group
# Begin Group "musikTrackCtrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\musikTrackCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\musikTrackCtrl.h
# End Source File
# End Group
# Begin Group "musikPlaylistCtrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\musikDynDspInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\musikDynDspInfo.h
# End Source File
# Begin Source File

SOURCE=.\musikPlaylistCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\musikPlaylistCtrl.h
# End Source File
# Begin Source File

SOURCE=.\musikPlaylistDropTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\musikPlaylistDropTarget.h
# End Source File
# Begin Source File

SOURCE=.\musikPlaylistInfoCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\musikPlaylistInfoCtrl.h
# End Source File
# Begin Source File

SOURCE=.\musikPlaylistView.cpp
# End Source File
# Begin Source File

SOURCE=.\musikPlaylistView.h
# End Source File
# End Group
# Begin Group "musikSourcesCtrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\musikSourcesCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\musikSourcesCtrl.h
# End Source File
# Begin Source File

SOURCE=.\musikSourcesDropTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\musikSourcesDropTarget.h
# End Source File
# Begin Group "musikPropTree"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\3rdparty\musikPropTree.cpp
# End Source File
# Begin Source File

SOURCE=.\3rdparty\musikPropTree.h
# End Source File
# Begin Source File

SOURCE=.\3rdparty\musikPropTreeItem.cpp
# End Source File
# Begin Source File

SOURCE=.\3rdparty\musikPropTreeItem.h
# End Source File
# End Group
# End Group
# Begin Group "musikNowPlayingCtrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\musikNowPlayingCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\musikNowPlayingCtrl.h
# End Source File
# Begin Group "musikNowPlayingInfo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\musikDynamicText.cpp
# End Source File
# Begin Source File

SOURCE=.\musikDynamicText.h
# End Source File
# Begin Source File

SOURCE=.\musikNowPlayingInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\musikNowPlayingInfo.h
# End Source File
# End Group
# Begin Group "musikVolumeCtrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\musikVolumeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\musikVolumeCtrl.h
# End Source File
# End Group
# Begin Group "musikTimeCtrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\musikTimeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\musikTimeCtrl.h
# End Source File
# End Group
# End Group
# Begin Group "musikSelectionCtrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\musikSelectionCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\musikSelectionCtrl.h
# End Source File
# End Group
# Begin Group "musikEqualizerCtrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\musikEqualizerCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\musikEqualizerCtrl.h
# End Source File
# End Group
# End Group
# Begin Group "Dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\musikAddNetstreamDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\musikAddNetstreamDlg.h
# End Source File
# Begin Source File

SOURCE=.\musikAlarmSetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\musikAlarmSetDlg.h
# End Source File
# Begin Source File

SOURCE=.\musikBatchTagDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\musikBatchTagDlg.h
# End Source File
# Begin Source File

SOURCE=.\musikDirSync.cpp
# End Source File
# Begin Source File

SOURCE=.\musikDirSync.h
# End Source File
# Begin Source File

SOURCE=.\musikDynPlaylistDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\musikDynPlaylistDlg.h
# End Source File
# Begin Source File

SOURCE=.\musikEqualizerSets.cpp
# End Source File
# Begin Source File

SOURCE=.\musikEqualizerSets.h
# End Source File
# Begin Source File

SOURCE=.\musikFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\musikFileDialog.h
# End Source File
# Begin Source File

SOURCE=.\musikFileDrop.cpp
# End Source File
# Begin Source File

SOURCE=.\musikFileDrop.h
# End Source File
# Begin Source File

SOURCE=.\musikMaskDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\musikMaskDlg.h
# End Source File
# Begin Source File

SOURCE=.\musikNameEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\musikNameEntry.h
# End Source File
# Begin Source File

SOURCE=.\musikPluginManagerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\musikPluginManagerDlg.h
# End Source File
# Begin Source File

SOURCE=.\musikTagDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\musikTagDlg.h
# End Source File
# Begin Group "musikPrefsDlg"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\musikPrefsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\musikPrefsDlg.h
# End Source File
# Begin Source File

SOURCE=.\musikPropertyPage.cpp
# End Source File
# Begin Source File

SOURCE=.\musikPropertyPage.h
# End Source File
# Begin Group "PropPage"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\3rdparty\PropPageFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\3rdparty\PropPageFrame.h
# End Source File
# Begin Source File

SOURCE=.\3rdparty\PropPageFrameDefault.cpp
# End Source File
# Begin Source File

SOURCE=.\3rdparty\PropPageFrameDefault.h
# End Source File
# Begin Source File

SOURCE=.\3rdparty\TreePropSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\3rdparty\TreePropSheet.h
# End Source File
# End Group
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe;resx"
# Begin Source File

SOURCE=.\idb_down.bmp
# End Source File
# Begin Source File

SOURCE=.\idb_up.bmp
# End Source File
# Begin Source File

SOURCE=.\musikCube.rc
# End Source File
# Begin Source File

SOURCE=.\res\musikCube.rc2
# End Source File
# End Group
# End Group
# Begin Group "Functor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\musikCubeFunctor.cpp
# End Source File
# Begin Source File

SOURCE=.\musikCubeFunctor.h
# End Source File
# End Group
# Begin Group "Rip"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\musikRip.cpp
# End Source File
# Begin Source File

SOURCE=.\musikRip.h
# End Source File
# Begin Source File

SOURCE=.\musikRipDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\musikRipDlg.h
# End Source File
# End Group
# End Group
# Begin Group "musikCubeApp"

# PROP Default_Filter "h;hpp;hxx;hm;inl;inc;xsd"
# Begin Source File

SOURCE=.\musikCube.cpp
# End Source File
# Begin Source File

SOURCE=.\musikCube.h
# End Source File
# Begin Source File

SOURCE=.\res\musikCube.manifest
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp

!IF  "$(CFG)" == "musikCube - Win32 Debug"

# ADD CPP /nologo /Yc"stdafx.h" /GZ /GX 
!ELSEIF  "$(CFG)" == "musikCube - Win32 Release"

# ADD CPP /nologo /Yc"stdafx.h" /GX 
!ENDIF

# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Group "3rdparty"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\3rdparty\BtnST.cpp
# End Source File
# Begin Source File

SOURCE=.\3rdparty\BtnST.h
# End Source File
# Begin Source File

SOURCE=.\3rdparty\CShellFileOp.cpp
# End Source File
# Begin Source File

SOURCE=.\3rdparty\CShellFileOp.h
# End Source File
# Begin Source File

SOURCE=.\Memdc.h
# End Source File
# Begin Source File

SOURCE=.\3rdparty\Mfccddb.cpp
# End Source File
# Begin Source File

SOURCE=.\3rdparty\Mfccddb.h
# End Source File
# Begin Source File

SOURCE=.\3rdparty\sizecbar.cpp
# End Source File
# Begin Source File

SOURCE=.\3rdparty\sizecbar.h
# End Source File
# End Group
# End Group
# Begin Group "Prefs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\musikPrefs.cpp
# End Source File
# Begin Source File

SOURCE=.\musikPrefs.h
# End Source File
# Begin Group "CIniEx"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\3rdparty\iniFile.cpp
# End Source File
# Begin Source File

SOURCE=.\3rdparty\iniFile.h
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\changelog.txt
# End Source File
# End Target
# End Project

