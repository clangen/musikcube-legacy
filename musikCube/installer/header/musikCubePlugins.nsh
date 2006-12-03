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

  !define PLUGIN_INST_APE "core_ape.exe"
  !define PLUGIN_INST_FLAC "core_flac.exe"
  !define PLUGIN_INST_M4A "core_m4a.exe"
  !define PLUGIN_INST_MOD "core_mod.exe"
  !define PLUGIN_INST_WAV "core_wav.exe"
  !define PLUGIN_INST_WMA "core_wma.exe"
  !define PLUGIN_INST_MPC "core_mpc.exe"
  !define PLUGIN_INST_WV "core_wv.exe"

  !define PLUGIN_UNINST_APE "core_ape_uninstall.exe"
  !define PLUGIN_UNINST_FLAC "core_flac_uninstall.exe"
  !define PLUGIN_UNINST_M4A "core_m4a_uninstall.exe"
  !define PLUGIN_UNINST_MOD "core_mod_uninstall.exe"
  !define PLUGIN_UNINST_WAV "core_wav_uninstall.exe"
  !define PLUGIN_UNINST_WMA "core_wma_uninstall.exe"
  !define PLUGIN_UNINST_MPC "core_mpc_uninstall.exe"
  !define PLUGIN_UNINST_WV "core_wv_uninstall.exe"

  !define APIVERSION "1.0B"
  !define PLUGINS_REMOTE_PATH "http://www.musikcube.com/webinstall/${APIVERSION}"
  !define PLUGINS_LOCAL_PATH "$INSTDIR\formats"
  
  !ifdef COMPILE_APE
    !define PRODUCT_NAME "musikCube APE Format Plugin"
    !define PRODUCT_VERSION "0.01"
    !define PRODUCT_PUBLISHER "Casey" 
    !define PLUGIN_INST  ${PLUGIN_INST_APE}
    !define PLUGIN_EXT ${MC_EXT_APE}
  !endif
  
  !ifdef COMPILE_FLAC
    !define PRODUCT_NAME "musikCube FLAC Format Plugin"
    !define PRODUCT_VERSION "0.01"
    !define PRODUCT_PUBLISHER "Otto" 
    !define PLUGIN_INST  ${PLUGIN_INST_FLAC}
    !define PLUGIN_EXT ${MC_EXT_FLAC} 
  !endif
  
  !ifdef COMPILE_M4A
    !define PRODUCT_NAME "musikCube M4A/AAC Format Plugin"
    !define PRODUCT_VERSION "0.04"
    !define PRODUCT_PUBLISHER "Otto"
    !define PLUGIN_INST  ${PLUGIN_INST_M4A}
    !define PLUGIN_EXT ${MC_EXT_M4A}      
  !endif
  
  !ifdef COMPILE_MOD
    !define PRODUCT_NAME "musikCube MOD Format Plugin"
    !define PRODUCT_VERSION "0.01"
    !define PRODUCT_PUBLISHER "Otto"
    !define PLUGIN_INST  ${PLUGIN_INST_MOD}
    !define PLUGIN_EXT ${MC_EXT_MOD}    
  !endif
  
  !ifdef COMPILE_WAV
    !define PRODUCT_NAME "musikCube WAV Format Plugin"
    !define PRODUCT_VERSION "0.01"
    !define PRODUCT_PUBLISHER "Otto"
    !define PLUGIN_INST  ${PLUGIN_INST_WAV}
    !define PLUGIN_EXT ${MC_EXT_WAV}
  !endif
  
  !ifdef COMPILE_WMA
    !define PRODUCT_NAME "musikCube WMA Format Plugin"
    !define PRODUCT_VERSION "0.01"
    !define PRODUCT_PUBLISHER "Otto"
    !define PLUGIN_INST  ${PLUGIN_INST_WMA}
    !define PLUGIN_EXT ${MC_EXT_WMA} 
  !endif
  
  !ifdef COMPILE_MPC
    !define PRODUCT_NAME "musikCube MPC Format Plugin"
    !define PRODUCT_VERSION "0.02"
    !define PRODUCT_PUBLISHER "Otto"
    !define PLUGIN_INST  ${PLUGIN_INST_MPC}
    !define PLUGIN_EXT ${MC_EXT_MPC}    
  !endif
  
  !ifdef COMPILE_WV
    !define PRODUCT_NAME "musikCube WV Format Plugin"
    !define PRODUCT_VERSION "0.01"
    !define PRODUCT_PUBLISHER "Otto"
    !define PLUGIN_INST  ${PLUGIN_INST_WV}
    !define PLUGIN_EXT ${MC_EXT_WV}    
  !endif
