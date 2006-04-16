##########
#
#  Nullsoft Scriptable Install System
#
#  (18/02/2006)
#
#  Install Script by shouD
#  Enhanced by Rajiv
#
#    musikCube (c) Casey Langen 2002-2005
#
#    Original file icons by
#
#      derek985 and
#      Mateo (http://bookenligne.neuf.fr/goodies.htm)
#
#    Resources: NSIS Wiki / Forum, Internet
#
#____ Compiler

  CRCCheck force
  SetCompressor lzma
 
#____ Includes

  !define MAIN_INSTALLER
  !define LOGICLIB_SECTIONCMP
  !include "LogicLib.nsh"
  !include "MUI.nsh"
  !include "WinMessages.nsh"
  !include "header\musikCubeCommon.nsh"

#____ Product Information

  !define PRODUCT_NAME "musikCube"
  !define PRODUCT_VERSION "1.0-pre1"
  !define PRODUCT_PUBLISHER "Casey Langen"
  !define PRODUCT_WEB_SITE "http://www.musikcube.com"
  !define PRODUCT_WEB_HELP "http://www.musikcube.com/help"

#____ Installer Information

  Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
  BrandingText "${PRODUCT_WEB_SITE}"
  InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"
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
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_STARTMENU "StartMenu" $STARTMENU_FOLDER
  !insertmacro MUI_PAGE_COMPONENTS
  Page custom OptionFunction
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
          MessageBox MB_ICONINFORMATION|MB_OKCANCEL "$(MB_AlreadyInstalled)" IDOK +2
            Abort
            ExecWait "$2 /SILENT"
        ${EndUnless}
      ${EndUnless}
    ${EndIf}
  FunctionEnd

#____ InstTypes

  InstType "$(IT_Minimal)"
  InstType "$(IT_Full)"
  InstType /CUSTOMSTRING=$(IT_Custom)

#____ Sections

  Section "!musikCube" Main
    SectionIn RO
    SetOutPath "$INSTDIR"
    
  #__ Main Files
    File "..\changelog.txt"
    File "..\license.txt"
    File "..\bin\bass.dll"
    File "..\bin\basscd.dll"
    File "..\bin\bassenc.dll"
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
    File "..\res\musikCube.exe.manifest"
  
    SetOutPath "$INSTDIR\img"
  
  #__ Sources Images
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

  #__ miniPlayer  
    SetOutPath "$INSTDIR\plugins"
    File "..\bin\plugins\miniPlayer.dll"

  #__ Base Formats
    SetOutPath "$INSTDIR\formats"
    File "..\bin\formats\core_cdda.dll"
    File "..\bin\formats\core_mp3.dll"
    File "..\bin\formats\core_net.dll"
    File "..\bin\formats\core_ogg.dll"

  #__ Font Installation  (NO LONGER NECESSARY!)
  #  SetOutPath "$FONTS"
  #  File "..\${MC_FONT_FILE}"

  #  ; Fonts registry key
  #  StrCpy $0 "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Fonts"
 
  #  ; Register font in system and refresh
  #  System::Call 'GDI32::AddFontResourceA(t) i ("${MC_FONT_FILE}") .s'
  #  WriteRegStr HKLM "$0" "${MC_FONT_NAME}" "${MC_FONT_FILE}"
  #  SendMessage ${HWND_BROADCAST} ${WM_FONTCHANGE} 0 0 /TIMEOUT=5000

  #__ Start Menu Shortcut
    SetOutPath "$INSTDIR"
    !insertmacro MUI_STARTMENU_WRITE_BEGIN StartMenu
      CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
      CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\${MC_EXE_SHORTCUT}" "$INSTDIR\${MC_EXE_FILE}"
      CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\${MC_EXE_NG_SHORTCUT}" "$INSTDIR\${MC_EXE_FILE}" "--nographics"
      CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\${MC_UNINST_SHORTCUT}" "$INSTDIR\${MC_UNINST_FILE}"
   !insertmacro MUI_STARTMENU_WRITE_END

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

  SectionGroup "Plugins $(Desc_InternetRequired)" Plugins
    Section /o ".ape" APE_P
      SectionIn 2
      ${DownloadPlugin} "${PLUGIN_INST_APE}" "${MC_EXT_APE}" "${MC_FC_APE}" "${MC_ICON_LIB},2"
    SectionEnd
    Section /o ".flac" FLAC_P
      SectionIn 2
      ${DownloadPlugin} "${PLUGIN_INST_FLAC}" "${MC_EXT_FLAC}" "${MC_FC_FLAC}" "${MC_ICON_LIB},3"
    SectionEnd
    Section /o ".m4a" M4A_P
      SectionIn 2
      ${DownloadPlugin} "${PLUGIN_INST_M4A}" "${MC_EXT_M4A}" "${MC_FC_M4A}" "${MC_ICON_LIB},4"
    SectionEnd
    Section /o ".mod" MOD_P
      SectionIn 2
      ${DownloadPlugin} "${PLUGIN_INST_MOD}" "${MC_EXT_MOD}" "${MC_FC_MOD}" "${MC_ICON_LIB},5"
    SectionEnd
    Section /o ".wav" WAV_P
      SectionIn 2
      ${DownloadPlugin} "${PLUGIN_INST_WAV}" "${MC_EXT_WAV}" "${MC_FC_WAV}" "${MC_ICON_LIB},6"
    SectionEnd
    Section /o ".wma" WMA_P
      SectionIn 2
      ${DownloadPlugin} "${PLUGIN_INST_WMA}" "${MC_EXT_WMA}" "${MC_FC_WMA}" "${MC_ICON_LIB},7"
    SectionEnd
    Section /o ".mpc" MPC_P
      SectionIn 2
      ${DownloadPlugin} "${PLUGIN_INST_MPC}" "${MC_EXT_MPC}" "${MC_FC_MPC}" "${MC_ICON_LIB},8"
    SectionEnd
    Section /o ".wv" WV_P
      SectionIn 2
      ${DownloadPlugin} "${PLUGIN_INST_WV}" "${MC_EXT_WV}" "${MC_FC_WV}" "${MC_ICON_LIB},9"
    SectionEnd
  SectionGroupEnd

  Section "-.mp3" MP3_A
    SectionIn RO
    !insertmacro MUI_INSTALLOPTIONS_READ $CB_SELECTED ${OPTIONS_INI} "Field 1" "State"
    ${If} $CB_SELECTED == 1
      ${AssociateExt} "${MC_EXT_MP3}" "${MC_FC_MP3}" "${MC_ICON_LIB},0"
    ${EndIf}
  SectionEnd
  Section "-.ogg" OGG_A
    SectionIn RO
    !insertmacro MUI_INSTALLOPTIONS_READ $CB_SELECTED ${OPTIONS_INI} "Field 2" "State"
    ${If} $CB_SELECTED == 1
      ${AssociateExt} "${MC_EXT_OGG}" "${MC_FC_OGG}" "${MC_ICON_LIB},1"
    ${EndIf}
  SectionEnd
  Section "-.ape" APE_A
    SectionIn RO
    !insertmacro MUI_INSTALLOPTIONS_READ $CB_SELECTED ${OPTIONS_INI} "Field 3" "State"
    ${If} $CB_SELECTED == 1
      ${AssociateExt} "${MC_EXT_APE}" "${MC_FC_APE}" "${MC_ICON_LIB},2"
    ${EndIf}
  SectionEnd
  Section "-.flac" FLAC_A
    SectionIn RO
    !insertmacro MUI_INSTALLOPTIONS_READ $CB_SELECTED ${OPTIONS_INI} "Field 4" "State"
    ${If} $CB_SELECTED == 1
      ${AssociateExt} "${MC_EXT_FLAC}" "${MC_FC_FLAC}" "${MC_ICON_LIB},3"
    ${EndIf}
  SectionEnd
  Section "-.m4a" M4A_A
    SectionIn RO
    !insertmacro MUI_INSTALLOPTIONS_READ $CB_SELECTED ${OPTIONS_INI} "Field 5" "State"
    ${If} $CB_SELECTED == 1
      ${AssociateExt} "${MC_EXT_M4A}" "${MC_FC_M4A}" "${MC_ICON_LIB},4"
    ${EndIf}
  SectionEnd
  Section "-.mod" MOD_A
    SectionIn RO
    !insertmacro MUI_INSTALLOPTIONS_READ $CB_SELECTED ${OPTIONS_INI} "Field 6" "State"
    ${If} $CB_SELECTED == 1
      ${AssociateExt} "${MC_EXT_MOD}" "${MC_FC_MOD}" "${MC_ICON_LIB},5"
    ${EndIf}
  SectionEnd
  Section "-.wav" WAV_A
    SectionIn RO
    !insertmacro MUI_INSTALLOPTIONS_READ $CB_SELECTED ${OPTIONS_INI} "Field 7" "State"
    ${If} $CB_SELECTED == 1
      ${AssociateExt} "${MC_EXT_WAV}" "${MC_FC_WAV}" "${MC_ICON_LIB},6"
    ${EndIf}
  SectionEnd
  Section "-.wma" WMA_A
    SectionIn RO
    !insertmacro MUI_INSTALLOPTIONS_READ $CB_SELECTED ${OPTIONS_INI} "Field 8" "State"
    ${If} $CB_SELECTED == 1
      ${AssociateExt} "${MC_EXT_WMA}" "${MC_FC_WMA}" "${MC_ICON_LIB},7"
    ${EndIf}
  SectionEnd
  Section "-.mpc" MPC_A
    SectionIn RO
    !insertmacro MUI_INSTALLOPTIONS_READ $CB_SELECTED ${OPTIONS_INI} "Field 9" "State"
    ${If} $CB_SELECTED == 1
      ${AssociateExt} "${MC_EXT_MPC}" "${MC_FC_MPC}" "${MC_ICON_LIB},8"
    ${EndIf}
  SectionEnd
  Section "-.wv" WV_A
    SectionIn RO
    !insertmacro MUI_INSTALLOPTIONS_READ $CB_SELECTED ${OPTIONS_INI} "Field 10" "State"
    ${If} $CB_SELECTED == 1
      ${AssociateExt} "${MC_EXT_WV}" "${MC_FC_WV}" "${MC_ICON_LIB},9"
    ${EndIf}
  SectionEnd

  Section "-Desktop Icon" DesktopIcon
    SectionIn RO
    !insertmacro MUI_INSTALLOPTIONS_READ $CB_SELECTED ${OPTIONS_INI} "Field 11" "State"
    ${If} $CB_SELECTED == 1
      SetOutPath "$INSTDIR"
      CreateShortCut "$DESKTOP\${MC_EXE_SHORTCUT}" "$INSTDIR\${MC_EXE_FILE}"
    ${EndIf}
  SectionEnd
  Section "-Quick Launch Icon" QuickLaunchIcon
    SectionIn RO
    !insertmacro MUI_INSTALLOPTIONS_READ $CB_SELECTED ${OPTIONS_INI} "Field 12" "State"
    ${If} $CB_SELECTED == 1
      SetOutPath "$INSTDIR"
      CreateShortCut "$QUICKLAUNCH\${MC_EXE_SHORTCUT}" "$INSTDIR\${MC_EXE_FILE}"
    ${EndIf}
  SectionEnd
 
  Section "Uninstall" Uninstaller
    Delete "$INSTDIR\bass.dll"
    Delete "$INSTDIR\basscd.dll"
    Delete "$INSTDIR\bassflac.dll"
    Delete "$INSTDIR\bassenc.dll"
    Delete "$INSTDIR\changelog.txt"
    Delete "$INSTDIR\flac.exe"
    Delete "$INSTDIR\gdiplus.dll"
    Delete "$INSTDIR\lame.exe"
    Delete "$INSTDIR\license.txt"
    Delete "$INSTDIR\mfc71u.dll"
    Delete "$INSTDIR\MMShellHook.dll"
    Delete "$INSTDIR\msvcp71.dll"
    Delete "$INSTDIR\msvcr71.dll"
    Delete "$INSTDIR\${MC_EXE_FILE}"
    Delete "$INSTDIR\musikCore_u.dll"
    Delete "$INSTDIR\musikCube.exe.manifest"
    Delete "$INSTDIR\oggenc.exe"
    Delete "$INSTDIR\${MC_UNINST_FILE}"

    Delete "$INSTDIR\${MC_ICON_LIB}"

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

  #__ Base Formats
    Delete "$INSTDIR\formats\core_cdda.dll"
    Delete "$INSTDIR\formats\core_mp3.dll"
    Delete "$INSTDIR\formats\core_net.dll"
    Delete "$INSTDIR\formats\core_ogg.dll"

  #__ Formats
    ExecWait "$INSTDIR\formats\${PLUGIN_UNINST_APE} /S"
    ExecWait "$INSTDIR\formats\${PLUGIN_UNINST_FLAC} /S"    
    ExecWait "$INSTDIR\formats\${PLUGIN_UNINST_M4A} /S"
    ExecWait "$INSTDIR\formats\${PLUGIN_UNINST_MOD} /S"
    ExecWait "$INSTDIR\formats\${PLUGIN_UNINST_WAV} /S"
    ExecWait "$INSTDIR\formats\${PLUGIN_UNINST_WMA} /S"
    ExecWait "$INSTDIR\formats\${PLUGIN_UNINST_MPC} /S"
    ExecWait "$INSTDIR\formats\${PLUGIN_UNINST_WV} /S"

    RMDir "$INSTDIR\img"
    RMDir "$INSTDIR\plugins"
    RMDir "$INSTDIR\formats"
    RMDir "$INSTDIR"

  #__ Font Deinstallation (NO LONGER NECESSARY!)
  #
  #  ; Fonts registry key
  #  StrCpy $0 "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Fonts"

  #  ; Deregister font from system and refresh
  #  System::Call 'GDI32::RemoveFontResourceA(t) i ("${MC_FONT_FILE}") .s'
  #  DeleteRegValue HKLM "$0" "${MC_FONT_NAME}"
  #  Delete /REBOOTOK "$FONTS\${MC_FONT_FILE}"
  #  SendMessage ${HWND_BROADCAST} ${WM_FONTCHANGE} 0 0 /TIMEOUT=5000
  
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
  
  #__ File Disassociation
    ${DisAssociateExt} "${MC_EXT_MP3}" "${MC_FC_MP3}"
    ${DisAssociateExt} "${MC_EXT_OGG}" "${MC_FC_OGG}"
    ${DisAssociateExt} "${MC_EXT_APE}" "${MC_FC_APE}"
    ${DisAssociateExt} "${MC_EXT_FLAC}" "${MC_FC_FLAC}"
    ${DisAssociateExt} "${MC_EXT_M4A}" "${MC_FC_M4A}"
    ${DisAssociateExt} "${MC_EXT_MOD}" "${MC_FC_MOD}"
    ${DisAssociateExt} "${MC_EXT_WAV}" "${MC_FC_WAV}"
    ${DisAssociateExt} "${MC_EXT_WMA}" "${MC_FC_WMA}"
    ${DisAssociateExt} "${MC_EXT_MPC}" "${MC_FC_MPC}"
    ${DisAssociateExt} "${MC_EXT_WV}" "${MC_FC_WV}"
  SectionEnd
 
#__ Component Descriptions

  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${Main} "$(Desc_Main)"
    !insertmacro MUI_DESCRIPTION_TEXT ${Plugins} "$(Desc_Plugins)"
    !insertmacro MUI_DESCRIPTION_TEXT ${APE_P} "$(Desc_APE)"
    !insertmacro MUI_DESCRIPTION_TEXT ${FLAC_P} "$(Desc_FLAC)"
    !insertmacro MUI_DESCRIPTION_TEXT ${M4A_P} "$(Desc_M4A)"
    !insertmacro MUI_DESCRIPTION_TEXT ${MOD_P} "$(Desc_MOD)"
    !insertmacro MUI_DESCRIPTION_TEXT ${WAV_P} "$(Desc_WAV)"
    !insertmacro MUI_DESCRIPTION_TEXT ${WMA_P} "$(Desc_WMA)"
    !insertmacro MUI_DESCRIPTION_TEXT ${MPC_P} "$(Desc_MPC)"
    !insertmacro MUI_DESCRIPTION_TEXT ${WV_P} "$(Desc_WV)"
  !insertmacro MUI_FUNCTION_DESCRIPTION_END
 
  Function OptionFunction
    !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 11" "Text" "$(IO_DeskIcon)" 
    !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 12" "Text" "$(IO_QLIcon)" 
    !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 13" "Text" "$(IO_Associate)" 
    !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 14" "Text" "$(IO_Shortcuts)" 
   
    ${If} ${SectionIsSelected} ${APE_P}
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 3" "Flags" "" 
    ${Else}
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 3" "State" "0"
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 3" "Flags" "DISABLED"
    ${Endif}
  
    ${If} ${SectionIsSelected} ${FLAC_P}
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 4" "Flags" "" 
    ${Else}
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 4" "State" "0"
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 4" "Flags" "DISABLED"
    ${Endif}
  
    ${If} ${SectionIsSelected} ${M4A_P}
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 5" "Flags" "" 
    ${Else}
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 5" "State" "0"
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 5" "Flags" "DISABLED"
    ${Endif}
  
    ${If} ${SectionIsSelected} ${MOD_P}
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 6" "Flags" "" 
    ${Else}
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 6" "State" "0"
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 6" "Flags" "DISABLED"
    ${Endif}
  
    ${If} ${SectionIsSelected} ${WAV_P}
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 7" "Flags" "" 
    ${Else}
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 7" "State" "0"
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 7" "Flags" "DISABLED"
    ${Endif}
  
    ${If} ${SectionIsSelected} ${WMA_P}
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 8" "Flags" "" 
    ${Else}
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 8" "State" "0"
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 8" "Flags" "DISABLED"
    ${Endif}
  
    ${If} ${SectionIsSelected} ${MPC_P}
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 9" "Flags" "" 
    ${Else}
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 9" "State" "0"
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 9" "Flags" "DISABLED"
    ${Endif}
  
    ${If} ${SectionIsSelected} ${WV_P}
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 10" "Flags" "" 
    ${Else}
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 10" "State" "0"
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 10" "Flags" "DISABLED"
    ${Endif}
  
    !insertmacro MUI_HEADER_TEXT "$(IO_Header)" "$(IO_SubHeader)"
    !insertmacro MUI_INSTALLOPTIONS_DISPLAY ${OPTIONS_INI}
  FunctionEnd