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
#____ Plugins ( Uncomment the plugin which should be compiled, one at a time )

  ;!define COMPILE_APE
  ;!define COMPILE_FLAC
  ;!define COMPILE_M4A
  ;!define COMPILE_MOD
  ;!define COMPILE_WAV
  ;!define COMPILE_WMA
  ;!define COMPILE_MPC
  ;!define COMPILE_WV

#____ Compiler

  CRCCheck force
  SetCompressor lzma
 
#____ Includes

  !define LOGICLIB_SECTIONCMP
  !include "LogicLib.nsh"
  !include "MUI.nsh"
  !include "header\musikCubeCommon.nsh"
 
#____ Installer Information

  Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
  BrandingText "${PRODUCT_NAME} ${PRODUCT_VERSION} - ${PRODUCT_PUBLISHER}"
  OutFile "./plugins/${PLUGIN_INST}"
  ShowInstDetails show
  ShowUninstDetails show

#____ Installer Configuration

  !define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\arrow2-install.ico"
  !define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\arrow2-uninstall.ico"
  
  !define OPTIONS_INI "musikCubePlugins.ini"

#____ Installer Pages

  Var CB_SELECTED
  Page custom OptionFunction
  !insertmacro MUI_PAGE_INSTFILES

#____ Uninstaller Pages

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

#____ Languanges

  !include "lang\Lang_*.nsh"

#____ Init Function

  Function .onInit
    !insertmacro MUI_INSTALLOPTIONS_EXTRACT ${OPTIONS_INI}
    ReadRegStr $INSTDIR HKLM "${MC_APPPATH_KEY}" "Path"
  FunctionEnd

#____ Sections

  Section "${PRODUCT_NAME}"
  
    !ifdef COMPILE_APE
      SetOutPath "$INSTDIR\formats"
      File "..\bin\formats\core_ape.dll"
      WriteUninstaller "$INSTDIR\formats\${PLUGIN_UNINST_APE}"
    !endif
    
    !ifdef COMPILE_FLAC  
      SetOutPath "$INSTDIR"
      File "..\bin\bassflac.dll"
      SetOutPath "$INSTDIR\formats"
      File "..\bin\formats\core_flac.dll"
      WriteUninstaller "$INSTDIR\formats\${PLUGIN_UNINST_FLAC}"
    !endif
    
    !ifdef COMPILE_M4A
      SetOutPath "$INSTDIR"
      File "..\bin\bass_aac.dll"
      SetOutPath "$INSTDIR\formats"
      File "..\bin\formats\core_m4a.dll"
      WriteUninstaller "$INSTDIR\formats\${PLUGIN_UNINST_M4A}"
    !endif
    
    !ifdef COMPILE_MOD
      SetOutPath "$INSTDIR\formats"
      File "..\bin\formats\core_mod.dll"
      WriteUninstaller "$INSTDIR\formats\${PLUGIN_UNINST_MOD}"   
    !endif
    
    !ifdef COMPILE_WAV
      SetOutPath "$INSTDIR\formats"
      File "..\bin\formats\core_wav.dll"
      WriteUninstaller "$INSTDIR\formats\${PLUGIN_UNINST_WAV}"    
    !endif
    
    !ifdef COMPILE_WMA
      SetOutPath "$INSTDIR"
      File "..\bin\basswma.dll"
      SetOutPath "$INSTDIR\formats"
      File "..\bin\formats\core_wma.dll"
      WriteUninstaller "$INSTDIR\formats\${PLUGIN_UNINST_WMA}"    
    !endif
    
    !ifdef COMPILE_MPC
      SetOutPath "$INSTDIR"
      File "..\bin\bass_mpc.dll"
      SetOutPath "$INSTDIR\formats"
      File "..\bin\formats\core_mpc.dll"
      WriteUninstaller "$INSTDIR\formats\${PLUGIN_UNINST_MPC}"    
    !endif
    
    !ifdef COMPILE_WV
      SetOutPath "$INSTDIR"
      File "..\bin\bass_wv.dll"
      SetOutPath "$INSTDIR\formats"
      File "..\bin\formats\core_wv.dll"
      WriteUninstaller "$INSTDIR\formats\${PLUGIN_UNINST_WV}"    
    !endif
    
    !insertmacro MUI_INSTALLOPTIONS_READ $CB_SELECTED ${OPTIONS_INI} "Field 1" "State"
    ${If} $CB_SELECTED == 1
      !ifdef COMPILE_APE
        ${AssociateExt} "${MC_EXT_APE}" "${MC_FC_APE}" "${MC_ICON_LIB},2"
      !endif
      
      !ifdef COMPILE_FLAC
        ${AssociateExt} "${MC_EXT_FLAC}" "${MC_FC_FLAC}" "${MC_ICON_LIB},3"
      !endif
      
      !ifdef COMPILE_M4A
        ${AssociateExt} "${MC_EXT_M4A}" "${MC_FC_M4A}" "${MC_ICON_LIB},4"
      !endif
      
      !ifdef COMPILE_MOD
        ${AssociateExt} "${MC_EXT_MOD}" "${MC_FC_MOD}" "${MC_ICON_LIB},5"
      !endif
      
      !ifdef COMPILE_WAV
        ${AssociateExt} "${MC_EXT_WAV}" "${MC_FC_WAV}" "${MC_ICON_LIB},6"
      !endif
      
      !ifdef COMPILE_WMA
        ${AssociateExt} "${MC_EXT_WMA}" "${MC_FC_WMA}" "${MC_ICON_LIB},7"
      !endif
      
      !ifdef COMPILE_MPC
        ${AssociateExt} "${MC_EXT_MPC}" "${MC_FC_MPC}" "${MC_ICON_LIB},8"
      !endif
      
      !ifdef COMPILE_WV
        ${AssociateExt} "${MC_EXT_WV}" "${MC_FC_WV}" "${MC_ICON_LIB},9"
      !endif
    ${EndIf}
    
  SectionEnd
 
  Section "Uninstall"
  
    !ifdef COMPILE_APE
      Delete "$INSTDIR\core_ape.dll"
      Delete "$INSTDIR\${PLUGIN_UNINST_APE}"
      ${DisAssociateExt} "${MC_EXT_APE}" "${MC_FC_APE}"
    !endif
    
    !ifdef COMPILE_FLAC
      Delete "$INSTDIR\core_flac.dll"
      Delete "$INSTDIR\${PLUGIN_UNINST_FLAC}"
      ${DisAssociateExt} "${MC_EXT_FLAC}" "${MC_FC_FLAC}"
    !endif
    
    !ifdef COMPILE_M4A
      Delete "$INSTDIR\..\bass_aac.dll"
      Delete "$INSTDIR\core_m4a.dll"
      Delete "$INSTDIR\${PLUGIN_UNINST_M4A}"
      ${DisAssociateExt} "${MC_EXT_M4A}" "${MC_FC_M4A}"
    !endif
    
    !ifdef COMPILE_MOD
      Delete "$INSTDIR\core_mod.dll"
      Delete "$INSTDIR\${PLUGIN_UNINST_MOD}"    
      ${DisAssociateExt} "${MC_EXT_MOD}" "${MC_FC_MOD}"     
    !endif
    
    !ifdef COMPILE_WAV
      Delete "$INSTDIR\core_wav.dll"
      Delete "$INSTDIR\${PLUGIN_UNINST_WAV}"   
      ${DisAssociateExt} "${MC_EXT_WAV}" "${MC_FC_WAV}"
    !endif
    
    !ifdef COMPILE_WMA
      Delete "$INSTDIR\..\basswma.dll"
      Delete "$INSTDIR\core_wma.dll"
      Delete "$INSTDIR\${PLUGIN_UNINST_WMA}" 
      ${DisAssociateExt} "${MC_EXT_WMA}" "${MC_FC_WMA}" 
    !endif
    
    !ifdef COMPILE_MPC
      Delete "$INSTDIR\..\bass_mpc.dll"
      Delete "$INSTDIR\core_mpc.dll"
      Delete "$INSTDIR\${PLUGIN_UNINST_MPC}"  
      ${DisAssociateExt} "${MC_EXT_MPC}" "${MC_FC_MPC}"         
    !endif
    
    !ifdef COMPILE_WV
      Delete "$INSTDIR\..\bass_wv.dll"
      Delete "$INSTDIR\core_wv.dll"
      Delete "$INSTDIR\${PLUGIN_UNINST_WV}"  
      ${DisAssociateExt} "${MC_EXT_WV}" "${MC_FC_WV}"      
    !endif
    
  SectionEnd
  
  Function OptionFunction
    !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 1" "Text" "$(IO_PluginsAssociate)"  
    !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 2" "State" "$INSTDIR"
    !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 2" "Text" "$(^DirBrowseText)"
    !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 3" "Text" "$(IO_PluginsExplanation)"
    !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 4" "Text" "$(^DirSubText)"
    !insertmacro MUI_INSTALLOPTIONS_WRITE ${OPTIONS_INI} "Field 5" "Text" "$(IO_PluginsTasks)"

    !insertmacro MUI_HEADER_TEXT "$(IO_PluginsHeader)" "$(IO_PluginsSubHeader)"
    !insertmacro MUI_INSTALLOPTIONS_DISPLAY ${OPTIONS_INI} 
  FunctionEnd
