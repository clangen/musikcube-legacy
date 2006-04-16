##########
#
#  Nullsoft Scriptable Install System
#
#  (28/01/2006)
#
#  Install Script by shouD
#
#    musikCube (c) Casey Langen 2002-2005
#
#    Resources: NSIS Wiki / Forum, Internet
#
#____ Language

  !insertmacro MUI_LANGUAGE "German"

#____ Install Options

  LangString IO_Header ${LANG_GERMAN} "Zusätzliche Aufgaben auswählen"
  LangString IO_SubHeader ${LANG_GERMAN} "Wählen Sie zusätzliche Aufgaben aus, die bei der Installation ausgeführt werden sollen."
  LangString IO_DeskIcon ${LANG_GERMAN} "Desktop-Symbol"
  LangString IO_QLIcon ${LANG_GERMAN} "Schnellstartleisten-Symbol"
  LangString IO_Associate ${LANG_GERMAN} "musikCube verknüpfen mit"
  LangString IO_Shortcuts ${LANG_GERMAN} "Verknüpfungen erstellen"
  LangString IO_PluginsHeader ${LANG_GERMAN} "Zielverzeichnis auswählen"
  LangString IO_PluginsSubHeader ${LANG_GERMAN} "Wählen Sie das Verzeichnis aus, in das $(^NameDA) installiert werden soll."
  LangString IO_PluginsExplanation ${LANG_GERMAN} "$(^NameDA) wird in das unten angegebene Verzeichnis installiert.\n\
  Dies muss dasselbe Verzeichnis sein, in dem auch musikCube installiert wurde!"
  LangString IO_PluginsTasks ${LANG_GERMAN} "Aufgaben"
  LangString IO_PluginsAssociate ${LANG_GERMAN} "Verknüpfe ${PLUGIN_EXT} mit musikCube"

#____ Message Boxes

  LangString MB_UnsupportedOS ${LANG_GERMAN} "${PRODUCT_NAME} unterstützt Windows 2000, XP, 2003 Server.$\n$\n\
  Wir bieten KEINE Unterstützung für Windows 95, 98, oder ME an."
  LangString MB_AlreadyInstalled ${LANG_GERMAN} "Eine ältere Version von ${PRODUCT_NAME} ist bereits installiert.$\n$\n\
  Drücken Sie 'OK' um die vorherige Version zu entfernen oder 'Abbrechen' um diese Installation abzubrechen."
  LangString MB_DetectionError ${LANG_GERMAN} "Der Installer war nicht in der Lage festzustellen ob Sie mit dem Internet verbunden sind.$\n\
  Wenn Sie sich sicher sind, dass dies der Fall ist, drücken Sie 'Ja', anderenfalls drücken Sie 'Nein'. Letzeres wird die Plugin-Installation abbrechen."
  LangString MB_OfflineError ${LANG_GERMAN} "Sie sind offline. Um das Format-Plugin zu installieren, müssen Sie mit dem Internet verbunden sein!$\n\
  Bitte stellen Sie eine Verbindung her und drücken Sie 'Wiederholen'! Wenn Sie das Plugin nicht installieren möchten, drücken Sie 'Abbrechen'."

#____ Descriptions

  LangString IT_Minimal ${LANG_GERMAN} "Minimal"
  LangString IT_Full ${LANG_GERMAN} "Vollständig"
  LangString IT_Custom ${LANG_GERMAN} "Benutzerdefiniert"
  LangString Desc_Main ${LANG_GERMAN} "Installiert musikCube und alle benötigten Dateien."
  LangString Desc_Plugins ${LANG_GERMAN} "Lädt zusätzliche Format-Plugins herunter."
  LangString Desc_APE ${LANG_GERMAN} "APE Format-Plugin."
  LangString Desc_FLAC ${LANG_GERMAN} "FLAC Format-Plugin."
  LangString Desc_M4A ${LANG_GERMAN} "MPEG 4 Audio Format-Plugin."
  LangString Desc_MOD ${LANG_GERMAN} "MOD Format-Plugin."
  LangString Desc_WAV ${LANG_GERMAN} "WAV Format-Plugin."
  LangString Desc_WMA ${LANG_GERMAN} "Widows Media Audio Format Plugin."
  LangString Desc_MPC ${LANG_GERMAN} "MusePack Format-Plugin."
  LangString Desc_WV ${LANG_GERMAN} "WavePack Format-Plugin."
  LangString Desc_InternetRequired ${LANG_GERMAN} "(Internetverbindung benötigt)"

#____ Detail Prints

  LangString DP_BackupCreated ${LANG_GERMAN} "Backup erstellt"
  LangString DP_ExtAssociated ${LANG_GERMAN} "Dateierweiterung verknüpft"
  LangString DP_FCCreated ${LANG_GERMAN} "Dateiklasse erstellt"
  LangString DP_ExtRemoved ${LANG_GERMAN} "Dateierweiterung entfernt"
  LangString DP_BackupRestored ${LANG_GERMAN} "Backup wiederhergestellt"
  LangString DP_FCRemoved ${LANG_GERMAN} "Dateiklasse entfernt"
 
#____ NSISdl

  LangString NSISdl_Down ${LANG_GERMAN} "Lade %s herunter"
  LangString NSISdl_Conn ${LANG_GERMAN} "Verbinde ..."
  LangString NSISdl_Sec ${LANG_GERMAN} "Sekunde"
  LangString NSISdl_Min ${LANG_GERMAN} "Minute"
  LangString NSISdl_Hour ${LANG_GERMAN} "Stunde"
  LangString NSISdl_Plural ${LANG_GERMAN} "n"
  LangString NSISdl_Prog ${LANG_GERMAN} "%dkB (%d%%) von %dkB @ %d.%01dkB/s"
  LangString NSISdl_Remain ${LANG_GERMAN} " (%d %s%s verbleibend)"