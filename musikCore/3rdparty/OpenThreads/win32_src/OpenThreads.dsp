# Microsoft Developer Studio Project File - Name="OpenThreads" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=OpenThreads - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OpenThreads.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OpenThreads.mak" CFG="OpenThreads - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OpenThreads - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "OpenThreads - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OpenThreads - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\../lib/Win32"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\../lib/Win32"
# PROP Intermediate_Dir ".\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /I "../include" /W3 /O2 /Ob1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "OPENTHREADS_EXPORTS" /D "_MBCS" /GF /Gy /GR /YX /Fp".\Release/OpenThreads.pch" /Fo".\Release/" /Fd".\Release/" /c /GX 
# ADD CPP /nologo /MD /I "../include" /W3 /O2 /Ob1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "OPENTHREADS_EXPORTS" /D "_MBCS" /GF /Gy /GR /YX /Fp".\Release/OpenThreads.pch" /Fo".\Release/" /Fd".\Release/" /c /GX 
# ADD BASE MTL /nologo /D"NDEBUG" /mktyplib203 /tlb".\..\lib\Win32\OpenThreads.tlb" /win32 
# ADD MTL /nologo /D"NDEBUG" /mktyplib203 /tlb".\..\lib\Win32\OpenThreads.tlb" /win32 
# ADD BASE RSC /l 1033 /d "NDEBUG" 
# ADD RSC /l 1033 /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo 
# ADD BSC32 /nologo 
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\lib\OpenThreads_u.lib" 
# ADD LIB32 /nologo /out:"..\..\..\lib\OpenThreads_u.lib" 

!ELSEIF  "$(CFG)" == "OpenThreads - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\../lib/Win32"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\../lib/Win32"
# PROP Intermediate_Dir ".\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /I "../include" /ZI /W3 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "OPENTHREADS_EXPORTS" /D "_UNICODE" /GR /YX /Fp".\Debug/OpenThreads.pch" /Fo".\Debug/" /Fd".\Debug/" /GZ /c /GX 
# ADD CPP /nologo /MDd /I "../include" /ZI /W3 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "OPENTHREADS_EXPORTS" /D "_UNICODE" /GR /YX /Fp".\Debug/OpenThreads.pch" /Fo".\Debug/" /Fd".\Debug/" /GZ /c /GX 
# ADD BASE MTL /nologo /D"_DEBUG" /mktyplib203 /tlb".\..\lib\Win32\OpenThreads.tlb" /win32 
# ADD MTL /nologo /D"_DEBUG" /mktyplib203 /tlb".\..\lib\Win32\OpenThreads.tlb" /win32 
# ADD BASE RSC /l 1033 /d "_DEBUG" 
# ADD RSC /l 1033 /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo 
# ADD BSC32 /nologo 
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\lib\OpenThreads_uD.lib" 
# ADD LIB32 /nologo /out:"..\..\..\lib\OpenThreads_uD.lib" 

!ENDIF

# Begin Target

# Name "OpenThreads - Win32 Release"
# Name "OpenThreads - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=WIN32Condition.cpp

!IF  "$(CFG)" == "OpenThreads - Win32 Release"

# ADD CPP /nologo /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OPENTHREADS_EXPORTS" /GX 
!ELSEIF  "$(CFG)" == "OpenThreads - Win32 Debug"

# ADD CPP /nologo /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OPENTHREADS_EXPORTS" /GZ /GX 
!ENDIF

# End Source File
# Begin Source File

SOURCE=Win32Mutex.cpp

!IF  "$(CFG)" == "OpenThreads - Win32 Release"

# ADD CPP /nologo /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OPENTHREADS_EXPORTS" /GX 
!ELSEIF  "$(CFG)" == "OpenThreads - Win32 Debug"

# ADD CPP /nologo /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OPENTHREADS_EXPORTS" /GZ /GX 
!ENDIF

# End Source File
# Begin Source File

SOURCE=Win32Thread.cpp

!IF  "$(CFG)" == "OpenThreads - Win32 Release"

# ADD CPP /nologo /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OPENTHREADS_EXPORTS" /GX 
!ELSEIF  "$(CFG)" == "OpenThreads - Win32 Debug"

# ADD CPP /nologo /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OPENTHREADS_EXPORTS" /GZ /GX 
!ENDIF

# End Source File
# Begin Source File

SOURCE=Win32ThreadBarrier.cpp

!IF  "$(CFG)" == "OpenThreads - Win32 Release"

# ADD CPP /nologo /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OPENTHREADS_EXPORTS" /GX 
!ELSEIF  "$(CFG)" == "OpenThreads - Win32 Debug"

# ADD CPP /nologo /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OPENTHREADS_EXPORTS" /GZ /GX 
!ENDIF

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\OpenThreads\Barrier
# End Source File
# Begin Source File

SOURCE=..\include\OpenThreads\Condition
# End Source File
# Begin Source File

SOURCE=..\include\OpenThreads\Exports
# End Source File
# Begin Source File

SOURCE=..\include\OpenThreads\Mutex
# End Source File
# Begin Source File

SOURCE=..\include\OpenThreads\Thread
# End Source File
# Begin Source File

SOURCE=Win32BarrierPrivateData.h
# End Source File
# Begin Source File

SOURCE=Win32Condition.h
# End Source File
# Begin Source File

SOURCE=Win32ConditionPrivateData.h
# End Source File
# Begin Source File

SOURCE=Win32MutexPrivateData.h
# End Source File
# Begin Source File

SOURCE=Win32ThreadPrivateData.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

