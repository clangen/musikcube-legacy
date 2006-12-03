##########
#
#  Nullsoft Scriptable Install System
#
#  Install script by insomnia
#
#    musikCube (c) Casey Langen 2002-2006
#
#    Resources: NSIS Wiki / Forum, Internet
#
#____ Definitions

  !define MC_EXE_FILE "musikCube.exe"
  !define MC_EXE_SHORTCUT "${PRODUCT_NAME}.lnk"
  !define MC_EXE_NG_SHORTCUT "${PRODUCT_NAME} (no graphics).lnk"
  !define MC_UNINST_SHORTCUT "Uninstall ${PRODUCT_NAME}.lnk"
  !define MC_INST_FILE "Setup.exe"
  !define MC_UNINST_FILE "uninstall.exe"

  !define MC_APPPATH_KEY "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\${MC_EXE_FILE}"
  !define MC_UNINST_KEY "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"

  !define MC_FILE_NAME "musikCube.audio"
  !define MC_BACKUP "musikCube.backup"

  !define NSISDL_COMMAND "/TRANSLATE $(NSISdl_Down) $(NSISdl_Conn) $(NSISdl_Sec) $(NSISdl_Min) \
  $(NSISdl_Hour) $(NSISdl_Plural) $(NSISdl_Prog) $(NSISdl_Remain) /TIMEOUT=30000 /NOIEPROXY"
  
  Var REFRESH_REQUIRED

#____ Functions

  Function AssociateExt
      !define AssociateExt '!insertmacro AssociateExtCall'
      !macro AssociateExtCall EXT FILE_CLASS ICON
        Push ${EXT}        ; $0 |
        Push ${FILE_CLASS} ; $1 | in function
        Push ${ICON}       ; $2 |
        Call AssociateExt
      !macroend
    Exch $2
    Exch
    Exch $1
    Exch 2
    Exch $0
    Exch 2
    Exch
    Push $3
    StrCpy $REFRESH_REQUIRED 0
       
    ReadRegStr $3 HKCR "$0" ""
    ${If} $3 != $1 ; Check if EXT is not associated with FILE_CLASS
      ${If} $3 != ""  ; Check if EXT is associated with another file class
        WriteRegStr HKCR "$0" "${MC_BACKUP}" "$3" ; Create backup of previous file class
        DetailPrint "$0: $(DP_BackupCreated)"
      ${EndIf}
      WriteRegStr HKCR "$0" "" "$1" ; Associate with FILE_CLASS
      DetailPrint "$0->$1: $(DP_ExtAssociated)"
      StrCpy $REFRESH_REQUIRED 1 ; Shell refresh required
    ${EndIf}
    
    ReadRegStr $3 HKCR "$1" ""
    ${If} $3 == "" ; Check if FILE_CLASS exists, if not, create it
      WriteRegStr HKCR "$1" "" "${MC_FILE_NAME}"
      WriteRegStr HKCR "$1\DefaultIcon" "" "$INSTDIR\$2"
      WriteRegStr HKCR "$1\shell" "" "open"
      WriteRegStr HKCR "$1\shell\open\command" "" '"$INSTDIR\musikCube.exe" "%1"'
      DetailPrint "$1: $(DP_FCCreated)"
      StrCpy $REFRESH_REQUIRED 1 ; Shell refresh required
    ${EndIf}
    
    ${If} $REFRESH_REQUIRED = 1
      System::Call 'shell32.dll::SHChangeNotify(i, i, i, i) v (0x08000000, 0, 0, 0)' ; Force refresh
    ${EndIf}
    Pop $3
    Pop $2
    Pop $1
    Pop $0
  FunctionEnd

  Function un.DisAssociateExt
      !define DisAssociateExt '!insertmacro DisAssociateExtCall'
      !macro DisAssociateExtCall EXT FILE_CLASS
        Push ${EXT}        ; $0 | in
        Push ${FILE_CLASS} ; $1 | function
        Call un.DisAssociateExt
      !macroend
    Exch $1
    Exch
    Exch $0
    Exch
    Push $2
    StrCpy $REFRESH_REQUIRED 0
    
    ReadRegStr $2 HKCR "$0" ""
    ${If} $2 == $1 ; Check if EXT is associated with FILE_CLASS
      ReadRegStr $2 HKCR "$0" "${MC_BACKUP}"
      ${If} $2 != "" ; Check if backup of previous file class exists
        WriteRegStr HKCR "$0" "" "$2" ; Restore backup of previous file class
        DeleteRegValue HKCR "$0" "${MC_BACKUP}" ; Delete backup
        DetailPrint "$0: $(DP_BackupRestored)"
      ${Else} ; $2 == "" No backup exists
        DeleteRegKey HKCR "$0" ; Remove EXT
        DetailPrint "$0: $(DP_ExtRemoved)"
      ${EndIf}       
      StrCpy $REFRESH_REQUIRED 1 ; Shell refresh required  
    ${EndIf}
    
    ReadRegStr $2 HKCR "$1" ""
    ${If} $2 != ""
      DeleteRegKey HKCR "$1"
      DetailPrint "$1: $(DP_FCRemoved)"
      StrCpy $REFRESH_REQUIRED 1 ; Shell refresh required
    ${EndIf}
      
    ${If} $REFRESH_REQUIRED = 1
      System::Call 'shell32.dll::SHChangeNotify(i, i, i, i) v (0x08000000, 0, 0, 0)' ; Force refresh
    ${EndIf}
    Pop $2
    Pop $1
    Pop $0
  FunctionEnd

  !ifdef MAIN_INSTALLER
  
  Function DownloadPlugin
      !define DownloadPlugin '!insertmacro DownloadPluginCall'
      !macro DownloadPluginCall PLUGIN EXT FILE_CLASS ICON
        Push ${PLUGIN}     ; $0 |
        Push ${EXT}        ; $1 | in
        Push ${FILE_CLASS} ; $2 | function
        Push ${ICON}       ; $3 |
        Call DownloadPlugin
      !macroend
    Exch $3
    Exch
    Exch $2
    Exch 2
    Exch $1
    Exch 3
    Exch $0
    Exch 3
    Exch 2
    Exch
    Start:
      Push $4
      ClearErrors
      Dialer::GetConnectedState
      Pop $4
      ; Check if there was a detection error
      ${If} ${Errors}
        MessageBox MB_ICONINFORMATION|MB_YESNO "$(MB_DetectionError)" IDYES Online IDNO End
      ${EndIf}
      ; Check if status is offline
      ${If} $4 == "offline"
        MessageBox MB_ICONINFORMATION|MB_RETRYCANCEL "$(MB_OfflineError)" IDRETRY 0 IDCANCEL End
        Goto Start
      ${EndIf}
    Online:
      NSISdl::download ${NSISDL_COMMAND} "${PLUGINS_REMOTE_PATH}/$0" "${PLUGINS_LOCAL_PATH}\$0"
      Pop $4
      ; Check if download was succesful
      ${If} $4 == "success"
        ExecWait "${PLUGINS_LOCAL_PATH}\$0 /S /D=$INSTDIR" ; Execute plugin installer
        Delete "${PLUGINS_LOCAL_PATH}\$0" ; Delete plugin installer
        ; ${AssociateExt} "$1" "$2" "$3" ; Only needed if file type association is connected with download
      ${EndIf}
    End:
    Pop $4
    Pop $3
    Pop $2
    Pop $1
    Pop $0
  FunctionEnd
  
  !endif
