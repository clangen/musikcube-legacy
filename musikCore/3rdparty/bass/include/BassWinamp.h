#ifndef BASSWINAMP_H
#define BASSWINAMP_H

#include <wtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BASSWINAMPDEF
#define BASSWINAMPDEF(f) WINAPI f
#endif

typedef DWORD HPLUGIN;


#define BASS_CTYPE_STREAM_WINAMP	0x10400

#define BASS_WINAMP_SYNC_BITRATE	100

// BASS_WINAMP_SetConfig flags
#define BASS_WINAMP_CONFIG_INPUT_TIMEOUT	1  // Set the time to wait until timing out because
                                                   // the plugin is not using the output system

// BASS_WINAMP_FindPlugin flags
#define BASS_WINAMP_FIND_INPUT		1
#define BASS_WINAMP_FIND_RECURSIVE	4
// return value type
#define BASS_WINAMP_FIND_COMMALIST	8
    // Delphi's comma list style (item1,item2,"item with , commas",item4,"item with space")
  // the list ends with single NULL character


HPLUGIN BASSWINAMPDEF(BASS_WINAMP_LoadPlugin)(char *f, DWORD win);
void BASSWINAMPDEF(BASS_WINAMP_UnloadPlugin)(HPLUGIN handle);
char* BASSWINAMPDEF(BASS_WINAMP_GetName)(HPLUGIN handle);
char* BASSWINAMPDEF(BASS_WINAMP_GetExtentions)(HPLUGIN handle);
BOOL BASSWINAMPDEF(BASS_WINAMP_GetFileInfo)(char *f, char *title, int *lenms);
BOOL BASSWINAMPDEF(BASS_WINAMP_InfoDlg)(char *f, DWORD win);
void BASSWINAMPDEF(BASS_WINAMP_ConfigPlugin)(HPLUGIN handle, DWORD win);
void BASSWINAMPDEF(BASS_WINAMP_AboutPlugin)(HPLUGIN handle, DWORD win);
DWORD BASSWINAMPDEF(BASS_WINAMP_StreamCreate)(char *f, DWORD flags);

DWORD BASSWINAMPDEF(BASS_WINAMP_GetConfig)(DWORD option);
void BASSWINAMPDEF(BASS_WINAMP_SetConfig)(DWORD option, DWORD value);
char* BASSWINAMPDEF(BASS_WINAMP_FindPlugins)(char *pluginpath, DWORD flags);

#ifdef __cplusplus
}
#endif

#endif
