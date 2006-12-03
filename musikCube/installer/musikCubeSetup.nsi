##########
#
#  Nullsoft Scriptable Install System
#
#  Install script by insomnia
#  Enhanced by Rajiv
#
#    musikCube (c) Casey Langen 2002-2006
#
#    Resources: NSIS Wiki / Forum, Internet
#
#____ Compiler

  CRCCheck force
  SetCompressor /SOLID /FINAL lzma
 
#____ Includes

  !define MAIN_INSTALLER
  !define LOGICLIB_SECTIONCMP
  !include "LogicLib.nsh"
  !include "MUI.nsh"
  !include "WinMessages.nsh"

#____ Product Information

  !define PRODUCT_NAME "musikCube"
  !define PRODUCT_VERSION "1.0"
  !define PRODUCT_NAME_VERSION_DEL "_"
  !define PRODUCT_PUBLISHER "Casey Langen"
  !define PRODUCT_WEB_SITE "http://www.musikcube.com"
  !define PRODUCT_WEB_HELP "http://www.musikcube.com/help"

#____ Definitions

  !define MC_EXE_FILE "musikCube.exe"
  !define MC_EXE_SHORTCUT "${PRODUCT_NAME}.lnk"
  !define MC_EXE_NG_SHORTCUT "${PRODUCT_NAME} (no graphics).lnk"
  !define MC_UNINST_SHORTCUT "Uninstall ${PRODUCT_NAME}.lnk"
  !define MC_INST_FILE "Setup.exe"
  !define MC_UNINST_FILE "uninstall.exe"

  !define MC_APPPATH_KEY "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\${MC_EXE_FILE}"
  !define MC_UNINST_KEY "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"

#____ Installer Information

  Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
  BrandingText "${PRODUCT_WEB_SITE}"
  InstallDir "$PROGRAMFILES\${PRODUCT_NAME}${PRODUCT_NAME_VERSION_DEL}${PRODUCT_VERSION}"
  OutFile "${MC_INST_FILE}"
  ShowInstDetails show
  ShowUninstDetails show

#____ Installer Configuration

  !define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\arrow2-install.ico"
  !define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\arrow2-uninstall.ico"
  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_RIGHT
  !define MUI_HEADERIMAGE_BITMAP "res\musikCubeSetup.bmp"
  !define MUI_HEADERIMAGE_UNBITMAP "res\musikCubeSetup.bmp"

  !define MUI_WELCOMEFINISHPAGE_BITMAP "res\musikCubeSetupLarge.bmp"
  !define MUI_UNWELCOMEFINISHPAGE_BITMAP "res\musikCubeSetupLarge.bmp"

  !define MUI_COMPONENTSPAGE_SMALLDESC
  !define MUI_COMPONENTSPAGE_CHECKBITMAP "${NSISDIR}\Contrib\Graphics\Checks\modern.bmp"

  !define MUI_ABORTWARNING
  !define MUI_UNABORTWARNING

  !define MUI_FINISHPAGE_NOAUTOCLOSE
  !define MUI_UNFINISHPAGE_NOAUTOCLOSE
 
  !define OPTIONS_INI "musikCubeSetup.ini"

  Var STARTMENU_FOLDER
  !define MUI_STARTMENUPAGE_DEFAULTFOLDER "${PRODUCT_NAME}"
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU"
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\${PRODUCT_NAME}"
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "StartMenuFolder"

  !define MUI_FINISHPAGE_RUN "$INSTDIR\${MC_EXE_FILE}"

#____ Installer Pages

  Var CB_SELECTED
  !define MUI_WELCOMEPAGE_TITLE_3LINES
  !insertmacro MUI_PAGE_WELCOME
  !define MUI_LICENSEPAGE_RADIOBUTTONS
  !insertmacro MUI_PAGE_LICENSE "..\license.txt"
  !define MUI_PAGE_CUSTOMFUNCTION_LEAVE CheckDirectory
  !insertmacro MUI_PAGE_DIRECTORY
  Page custom OptionFunction  
  !define MUI_PAGE_CUSTOMFUNCTION_PRE StartMenuFunction  
  !define MUI_STARTMENUPAGE_NODISABLE 
  !insertmacro MUI_PAGE_STARTMENU "StartMenu" $STARTMENU_FOLDER
  !insertmacro MUI_PAGE_INSTFILES
  !define MUI_FINISHPAGE_TITLE_3LINES
  !insertmacro MUI_PAGE_FINISH

#____ Uninstaller Pages

  !define MUI_WELCOMEPAGE_TITLE_3LINES
  !insertmacro MUI_UNPAGE_WELCOME
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !define MUI_FINISHPAGE_TITLE_3LINES 
  !insertmacro MUI_UNPAGE_FINISH

#____ Languanges

  !include "lang\Lang_*.nsh"

#____ Init Function

  Function .onInit
    !insertmacro MUI_INSTALLOPTIONS_EXTRACT ${OPTIONS_INI}
    ; Check Windows version
    ClearErrors
    ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion"
    ${If} ${Errors} ; Windows 95, 98, Me not supported
      MessageBox MB_ICONINFORMATION|MB_OK "$(MB_UnsupportedOS)" IDOK 0
      Abort
    ${Else} ; Windows 2000, XP, 2003 Server supported
      ; Check if setup is already running
      System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${PRODUCT_NAME}_Setup") i .r0 ?e'
      Pop $0
      ${Unless} $0 == 0 ; Setup is already running
        StrLen $0 "$(^Name)"
        IntOp $0 $0 + 1
        ${Do} ; Search window...
          FindWindow $1 '#32770' '' 0 $1
          ${IfThen} $1 = 0 ${|} Abort ${|}
          System::Call 'user32::GetWindowText(i r1, t .r2, i r0) i.'
        ${LoopUntil} $2 == "$(^Name)" ; ...by name...
            System::Call 'user32::SetForegroundWindow(i r1) i.' ; ... and set it to foreground
      ${Else} ; Setup is not running
        ClearErrors
        ReadRegStr $2 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\musikCube_is1" "UninstallString"
        ${Unless} ${Errors} ; musikCube <= 0.92.5 already installed
          MessageBox MB_ICONINFORMATION|MB_OKCANCEL "$(MB_AlreadyInstalledIS)" IDOK +2
            Abort
            ExecWait "$2 /SILENT"
        ${EndUnless}
        ClearErrors
        ReadRegStr $2 HKLM "${MC_UNINST_KEY}" "UninstallString"
        ${Unless} ${Errors} ; musikCube > 0.92.5 && < 1.0 already installed
          MessageBox MB_ICONINFORMATION|MB_OKCANCEL "$(MB_AlreadyInstalledNSIS)" IDOK +2
            Abort
            ExecWait "$2 /S"
        ${EndUnless}
      ${EndUnless}
    ${EndIf}
  FunctionEnd

#____ InstTypes
#
#  InstType "$(IT_Minimal)"
#  InstType "$(IT_Full)"
#  InstType /CUSTOMSTRING=$(IT_Custom)

#____ Sections

  Section "!musikCube" Main
    SectionIn RO
    
  #__ Main Files
    SetOutPath "$INSTDIR"
    File "..\bin\bass.dll"
    File "..\bin\bass_aac.dll"
    File "..\bin\bass_alac.dll"	
    File "..\bin\basscd.dll"
    File "..\bin\bassenc.dll"
	File "..\bin\bassflac.dll"
    File "..\changelog.txt"
    File "..\license.txt"
    File "..\bin\flac.exe"
    File "..\bin\gdiplus.dll"
    File "..\bin\lame.exe"
    File "..\bin\mfc71u.dll"
    File "..\bin\MMShellHook.dll"
    File "..\bin\msvcp71.dll"
    File "..\bin\msvcr71.dll"
    File "..\bin\musikCore_u.dll"
    File "..\bin\oggenc.exe"
    File "..\bin\musikCube.ttf"
    File "..\bin\${MC_EXE_FILE}"
  
  #__ Sources Images
    SetOutPath "$INSTDIR\img"
    File "..\bin\img\cd_f.png"
    File "..\bin\img\cd_uf.png"
    File "..\bin\img\dp_f.png"
    File "..\bin\img\dp_uf.png"
    File "..\bin\img\lib_f.png"
    File "..\bin\img\lib_uf.png"
    File "..\bin\img\np_f.png"
    File "..\bin\img\np_uf.png"
    File "..\bin\img\pl_f.png"
    File "..\bin\img\pl_uf.png"
    File "..\bin\img\qs_f.png"
    File "..\bin\img\qs_uf.png"
    File "..\bin\img\rd_f.png"
    File "..\bin\img\rd_uf.png"
    File "..\bin\img\ns_f.png"
    File "..\bin\img\ns_uf.png"
    File "..\bin\img\plug_uf.png"
    File "..\bin\img\plug_f.png"
    File "..\bin\img\col_uf.png"
    File "..\bin\img\col_f.png"

  #__ Playback Images
    File "..\bin\img\prev_light_f.png"
    File "..\bin\img\prev_light_uf.png"
    File "..\bin\img\prev_dark_f.png"
    File "..\bin\img\prev_dark_uf.png"

    File "..\bin\img\next_light_f.png"
    File "..\bin\img\next_light_uf.png"
    File "..\bin\img\next_dark_f.png"
    File "..\bin\img\next_dark_uf.png"  

    File "..\bin\img\pause_light_f.png"
    File "..\bin\img\pause_light_uf.png"
    File "..\bin\img\pause_dark_f.png"
    File "..\bin\img\pause_dark_uf.png"  
    
    File "..\bin\img\play_light_f.png"
    File "..\bin\img\play_light_uf.png"
    File "..\bin\img\play_dark_f.png"
    File "..\bin\img\play_dark_uf.png"
      
    File "..\bin\img\stop_light_f.png"
    File "..\bin\img\stop_light_uf.png"
    File "..\bin\img\stop_dark_f.png"
    File "..\bin\img\stop_dark_uf.png"

    File "..\bin\img\random_light_f.png"
    File "..\bin\img\random_light_uf.png"
    File "..\bin\img\random_dark_f.png"
    File "..\bin\img\random_dark_uf.png"

    File "..\bin\img\cf_light_f.png"
    File "..\bin\img\cf_light_uf.png"
    File "..\bin\img\cf_dark_f.png"
    File "..\bin\img\cf_dark_uf.png"

    File "..\bin\img\repeat_light_uf.png"
    File "..\bin\img\repeat_light_f.png"
    File "..\bin\img\repeat_dark_uf.png"
    File "..\bin\img\repeat_dark_f.png"

    File "..\bin\img\eq_light_uf.png"
    File "..\bin\img\eq_light_f.png"
    File "..\bin\img\eq_dark_uf.png"
    File "..\bin\img\eq_dark_f.png"

  #__musikCube plugins 
    SetOutPath "$INSTDIR\plugins"
    File "..\bin\plugins\miniPlayer.dll"
    File "..\bin\plugins\mcAmp.dll"

  #__ Base Formats
    SetOutPath "$INSTDIR\formats"
    File "..\bin\formats\core_ape.dll"
    File "..\bin\formats\core_cdda.dll"
    File "..\bin\formats\core_flac.dll"
    File "..\bin\formats\core_m4a.dll"	
    File "..\bin\formats\core_mp3.dll"
    File "..\bin\formats\core_mpc.dll"	
    File "..\bin\formats\core_net.dll"
    File "..\bin\formats\core_ogg.dll"
    File "..\bin\formats\core_wav.dll"
    File "..\bin\formats\core_wma.dll"

  #__ Delete leftover DLL from old mC verions
    Delete /REBOOTOK "$INSTDIR\formats\core_Internal.dll"
  SectionEnd

  Section "-Uninstaller" UninstallerInfo
    WriteUninstaller "$INSTDIR\${MC_UNINST_FILE}"
    WriteRegStr HKLM "${MC_APPPATH_KEY}" "" "$INSTDIR\${MC_EXE_FILE}"
    WriteRegStr HKLM "${MC_APPPATH_KEY}" "Path" "$INSTDIR"
    WriteRegStr HKLM "${MC_UNINST_KEY}" "DisplayName" "${PRODUCT_NAME} ${PRODUCT_VERSION}"
    WriteRegStr HKLM "${MC_UNINST_KEY}" "UninstallString" "$INSTDIR\${MC_UNINST_FILE}"
    WriteRegStr HKLM "${MC_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
    WriteRegStr HKLM "${MC_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
    WriteRegStr HKLM "${MC_UNINST_KEY}" "HelpLink" "${PRODUCT_WEB_HELP}"
    WriteRegStr HKLM "${MC_UNINST_KEY}" "URLUpdateInfo" "${PRODUCT_WEB_SITE}"
  SectionEnd

  Section "-Desktop Icon" DesktopIcon
    SectionIn RO
    !insertmacro MUI_INSTALLOPTIONS_READ $CB_SELECTED ${OPTIONS_INI} "Field 2" "State"
    ${If} $CB_SELECTED == 1
      SetOutPath "$INSTDIR"
      CreateShortCut "$DESKTOP\${MC_EXE_SHORTCUT}" "$INSTDIR\${MC_EXE_FILE}"
    ${EndIf}
  SectionEnd

  Section "-Start Menu Icon" StartMenuIcon
    SectionIn RO
    !insertmacro MUI_INSTALLOPTIONS_READ $CB_SELECTED ${OPTIONS_INI} "Field 3" "State"
    ${If} $CB_SELECTED == 1
      SetOutPath "$INSTDIR"
      !insertmacro MUI_STARTMENU_WRITE_BEGIN StartMenu
        CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
        CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\${MC_EXE_SHORTCUT}" "$INSTDIR\${MC_EXE_FILE}"
        CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\${MC_EXE_NG_SHORTCUT}" "$INSTDIR\${MC_EXE_FILE}" "--nographics"
        CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\${MC_UNINST_SHORTCUT}" "$INSTDIR\${MC_UNINST_FILE}"
      !insertmacro MUI_STARTMENU_WRITE_END
    ${EndIf}
  SectionEnd
  
  Section "-Quick Launch Icon" QuickLaunchIcon
    SectionIn RO
    !insertmacro MUI_INSTALLOPTIONS_READ $CB_SELECTED ${OPTIONS_INI} "Field 4" "State"
    ${If} $CB_SELECTED == 1
      SetOutPath "$INSTDIR"
      CreateShortCut "$QUICKLAUNCH\${MC_EXE_SHORTCUT}" "$INSTDIR\${MC_EXE_FILE}"
    ${EndIf}
  SectionEnd
 
  Section "Uninstall" Uninstaller
    Delete "$INSTDIR\bass.dll"
    Delete "$INSTDIR\bass_aac.dll"
    Delete "$INSTDIR\bass_alac.dll"	
    Delete "$INSTDIR\basscd.dll"
    Delete "$INSTDIR\bassenc.dll"
	Delete "$INSTDIR\bassflac.dll"
    Delete "$INSTDIR\changelog.txt"
    Delete "$INSTDIR\license.txt"
    Delete "$INSTDIR\flac.exe"
    Delete "$INSTDIR\gdiplus.dll"
    Delete "$INSTDIR\lame.exe"
    Delete "$INSTDIR\mfc71u.dll"
    Delete "$INSTDIR\MMShellHook.dll"
    Delete "$INSTDIR\msvcp71.dll"
    Delete "$INSTDIR\msvcr71.dll"
    Delete "$INSTDIR\musikCore_u.dll"
    Delete "$INSTDIR\oggenc.exe"
    Delete "$INSTDIR\musikCube.ttf"
    Delete "$INSTDIR\${MC_EXE_FILE}"	
    Delete "$INSTDIR\${MC_UNINST_FILE}"

  #__ Sources Images
    Delete "$INSTDIR\img\cd_f.png"
    Delete "$INSTDIR\img\cd_uf.png"
    Delete "$INSTDIR\img\dp_f.png"
    Delete "$INSTDIR\img\dp_uf.png"
    Delete "$INSTDIR\img\lib_f.png"
    Delete "$INSTDIR\img\lib_uf.png"
    Delete "$INSTDIR\img\np_f.png"
    Delete "$INSTDIR\img\np_uf.png"
    Delete "$INSTDIR\img\pl_f.png"
    Delete "$INSTDIR\img\pl_uf.png"
    Delete "$INSTDIR\img\qs_f.png"
    Delete "$INSTDIR\img\qs_uf.png"
    Delete "$INSTDIR\img\rd_f.png"
    Delete "$INSTDIR\img\rd_uf.png"
    Delete "$INSTDIR\img\ns_f.png"
    Delete "$INSTDIR\img\ns_uf.png"
    Delete "$INSTDIR\img\cf_f.png"
    Delete "$INSTDIR\img\cf_uf.png"
    Delete "$INSTDIR\img\plug_uf.png"
    Delete "$INSTDIR\img\plug_f.png"
    Delete "$INSTDIR\img\col_uf.png"
    Delete "$INSTDIR\img\col_f.png"

  #__ Playback Images
    Delete "$INSTDIR\img\prev_light_f.png"
    Delete "$INSTDIR\img\prev_light_uf.png"
    Delete "$INSTDIR\img\prev_dark_f.png"
    Delete "$INSTDIR\img\prev_dark_uf.png"

    Delete "$INSTDIR\img\next_light_f.png"
    Delete "$INSTDIR\img\next_light_uf.png"
    Delete "$INSTDIR\img\next_dark_f.png"
    Delete "$INSTDIR\img\next_dark_uf.png"  

    Delete "$INSTDIR\img\pause_light_f.png"
    Delete "$INSTDIR\img\pause_light_uf.png"
    Delete "$INSTDIR\img\pause_dark_f.png"
    Delete "$INSTDIR\img\pause_dark_uf.png"  

    Delete "$INSTDIR\img\play_light_f.png"
    Delete "$INSTDIR\img\play_light_uf.png"
    Delete "$INSTDIR\img\play_dark_f.png"
    Delete "$INSTDIR\img\play_dark_uf.png"

    Delete "$INSTDIR\img\stop_light_f.png"
    Delete "$INSTDIR\img\stop_light_uf.png"
    Delete "$INSTDIR\img\stop_dark_f.png"
    Delete "$INSTDIR\img\stop_dark_uf.png"

    Delete "$INSTDIR\img\random_light_f.png"
    Delete "$INSTDIR\img\random_light_uf.png"
    Delete "$INSTDIR\img\random_dark_f.png"
    Delete "$INSTDIR\img\random_dark_uf.png"

    Delete "$INSTDIR\img\cf_light_f.png"
    Delete "$INSTDIR\img\cf_light_uf.png"
    Delete "$INSTDIR\img\cf_dark_f.png"
    Delete "$INSTDIR\img\cf_dark_uf.png"

    Delete "$INSTDIR\img\repeat_light_uf.png"
    Delete "$INSTDIR\img\repeat_light_f.png"
    Delete "$INSTDIR\img\repeat_dark_uf.png"
    Delete "$INSTDIR\img\repeat_dark_f.png"

    Delete "$INSTDIR\img\eq_light_uf.png"
    Delete "$INSTDIR\img\eq_light_f.png"
    Delete "$INSTDIR\img\eq_dark_uf.png"
    Delete "$INSTDIR\img\eq_dark_f.png"

  #__ miniPlayer
    Delete "$INSTDIR\plugins\miniPlayer.dll"
    Delete "$INSTDIR\plugins\miniPlayer.ini"
    Delete "$INSTDIR\plugins\mcAmp.dll"
	
  #__ Base Formats
	Delete "$INSTDIR\formats\core_ape.dll"
    Delete "$INSTDIR\formats\core_cdda.dll"
    Delete "$INSTDIR\formats\core_flac.dll"
    Delete "$INSTDIR\formats\core_m4a.dll"	
    Delete "$INSTDIR\formats\core_mp3.dll"
    Delete "$INSTDIR\formats\core_mpc.dll"	
    Delete "$INSTDIR\formats\core_net.dll"
    Delete "$INSTDIR\formats\core_ogg.dll"
    Delete "$INSTDIR\formats\core_wav.dll"
    Delete "$INSTDIR\formats\core_wma.dll"

    RMDir "$INSTDIR\img"
    RMDir "$INSTDIR\plugins"
    RMDir "$INSTDIR\formats"
    RMDir "$INSTDIR"
  
  #__ Additional Shortcuts  
    Delete "$DESKTOP\${MC_EXE_SHORTCUT}"
    Delete "$QUICKLAUNCH\${MC_EXE_SHORTCUT}"
  
  #__ Start Menu Shortcuts
    !insertmacro MUI_STARTMENU_GETFOLDER StartMenu $R0
    Delete "$SMPROGRAMS\$R0\${MC_EXE_SHORTCUT}"
    Delete "$SMPROGRAMS\$R0\${MC_EXE_NG_SHORTCUT}"
    Delete "$SMPROGRAMS\$R0\${MC_UNINST_SHORTCUT}"
    RMDir "$SMPROGRAMS\$R0"
  
  #__ Product Deregistration
    DeleteRegKey HKCU "Software\${PRODUCT_NAME}"
    DeleteRegKey HKLM "${MC_APPPATH_KEY}"
    DeleteRegKey HKLM "${MC_UNINST_KEY}"

  SectionEnd
 
#__ Component Descriptions
#
#  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
#    !insertmacro MUI_DESCRIPTION_TEXT ${Main} "$(Desc_Main)"
#  !insertmacro MUI_FUNCTION_DESCRIPTION_END
 
  Function OptionFunction
    !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 1" "Text" "$(IO_ProgIcons)"   
    !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 2" "Text" "$(IO_DeskIcon)" 
    !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 3" "Text" "$(IO_SMPIcon)" 
    !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 4" "Text" "$(IO_QLIcon)"    
    !insertmacro MUI_HEADER_TEXT "$(IO_Header)" "$(IO_SubHeader)"
    !insertmacro MUI_INSTALLOPTIONS_DISPLAY ${OPTIONS_INI}
  FunctionEnd

  Function CheckDirectory
    ${If} ${FileExists} "$INSTDIR\bass*.dll"
    ${OrIf} ${FileExists} "$INSTDIR\formats\*.*"
    ${OrIf} ${FileExists} "$INSTDIR\plugins\*.*"
      MessageBox MB_ICONINFORMATION|MB_OKCANCEL "$(MB_OldFiles)" IDCANCEL +4
        Delete "$INSTDIR\bass*.dll"
        RMDir /r "$INSTDIR\formats"
        RMDir /r "$INSTDIR\plugins"
      Abort
    ${EndIf}
  FunctionEnd
  
  Function StartMenuFunction
    !insertmacro MUI_INSTALLOPTIONS_READ $CB_SELECTED ${OPTIONS_INI} "Field 3" "State"
    ${If} $CB_SELECTED == 0
      Abort
    ${EndIf}
  FunctionEnd
