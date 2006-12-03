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
#____ Language

  !insertmacro MUI_LANGUAGE "English"

#____ Install Options
  
  LangString IO_Header ${LANG_ENGLISH} "Set up Shortcuts"
  LangString IO_SubHeader ${LANG_ENGLISH} "Create Program Icons"
  LangString IO_ProgIcons ${LANG_ENGLISH} "Create icons for musikCube:"  
  LangString IO_DeskIcon ${LANG_ENGLISH} "On the Desktop"
  LangString IO_SMPIcon ${LANG_ENGLISH} "In the Start Menu Programs folder"  
  LangString IO_QLIcon ${LANG_ENGLISH} "In the Quick Launch bar"
  LangString IO_PluginsHeader ${LANG_ENGLISH} "Choose Install Location"
  LangString IO_PluginsSubHeader ${LANG_ENGLISH} "Choose the folder in which to install $(^NameDA)."
  LangString IO_PluginsExplanation ${LANG_ENGLISH} "Setup will install $(^NameDA) in the following folder.\n\
  This must be the same folder in which musikCube was installed!"
  LangString IO_PluginsTasks ${LANG_ENGLISH} "Tasks"
  LangString IO_PluginsAssociate ${LANG_ENGLISH} "Associate musikCube with ${PLUGIN_EXT}" 

#____ Message Boxes

  LangString MB_UnsupportedOS ${LANG_ENGLISH} "${PRODUCT_NAME} supports Windows 2000, XP, 2003 Server.$\n$\n\
  We do NOT provide support for Windows 95, 98, or ME."
  LangString MB_AlreadyInstalledIS ${LANG_ENGLISH} "An older version of ${PRODUCT_NAME} is already installed.$\n$\n\
  Click 'OK' to remove the previous version or 'Cancel' to cancel this installation."
  LangString MB_AlreadyInstalledNSIS ${LANG_ENGLISH} "An older version of ${PRODUCT_NAME} is already installed.$\n$\n\
  Click 'OK' to remove the previous version or 'Cancel' to cancel this installation."  
  LangString MB_OldFiles ${LANG_ENGLISH} "There are some old files in the target directory that need to be removed by the installer.$\n$\n\
  Click 'OK' to remove these files or 'Cancel' to change the installation directory."
  LangString MB_DetectionError ${LANG_ENGLISH} "The installer was not able to detect if you are connected to the internet.$\n\
  If you are sure you are, click 'Yes', otherwise click 'No'. Latter will cancel plugin install."
  LangString MB_OfflineError ${LANG_ENGLISH} "You are offline. To install the format plugin, you need to be connected to the internet!$\n\
  Please connect and click 'Retry'! If you don't want to install the plugin, click 'Cancel'."

#____ Descriptions
#
#  LangString IT_Minimal ${LANG_ENGLISH} "Minimal"
#  LangString IT_Full ${LANG_ENGLISH} "Full"
#  LangString IT_Custom ${LANG_ENGLISH} "Custom"
#  LangString Desc_Main ${LANG_ENGLISH} "Installs musikCube and all required files."
#  LangString Desc_Plugins ${LANG_ENGLISH} "Downloads additional format plugins."
#  LangString Desc_APE ${LANG_ENGLISH} "APE format plugin."
#  LangString Desc_FLAC ${LANG_ENGLISH} "FLAC format plugin."
#  LangString Desc_M4A ${LANG_ENGLISH} "MPEG 4 Audio format plugin."
#  LangString Desc_MOD ${LANG_ENGLISH} "MOD format plugin."
#  LangString Desc_WAV ${LANG_ENGLISH} "WAV format plugin."
#  LangString Desc_WMA ${LANG_ENGLISH} "Windows Media Audio format plugin."
#  LangString Desc_MPC ${LANG_ENGLISH} "MusePack format plugin."
#  LangString Desc_WV ${LANG_ENGLISH} "WavePack format plugin."
#  LangString Desc_InternetRequired ${LANG_ENGLISH} "(internet connection required)"

#____ Detail Prints

  LangString DP_BackupCreated ${LANG_ENGLISH} "Backup created"
  LangString DP_ExtAssociated ${LANG_ENGLISH} "File extension associated"
  LangString DP_FCCreated ${LANG_ENGLISH} "File class created"
  LangString DP_ExtRemoved ${LANG_ENGLISH} "File extension removed"
  LangString DP_BackupRestored ${LANG_ENGLISH} "Backup restored"
  LangString DP_FCRemoved ${LANG_ENGLISH} "File class removed"
 
#____ NSISdl
#
#  LangString NSISdl_Down ${LANG_ENGLISH} "Downloading %s"
#  LangString NSISdl_Conn ${LANG_ENGLISH} "Connecting ..."
#  LangString NSISdl_Sec ${LANG_ENGLISH} "second"
#  LangString NSISdl_Min ${LANG_ENGLISH} "minute"
#  LangString NSISdl_Hour ${LANG_ENGLISH} "hour"
#  LangString NSISdl_Plural ${LANG_ENGLISH} "s"
#  LangString NSISdl_Prog ${LANG_ENGLISH} "%dkB (%d%%) of %dkB @ %d.%01dkB/s"
#  LangString NSISdl_Remain ${LANG_ENGLISH} " (%d %s%s remaining)"
