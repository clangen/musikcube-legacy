/* BASSFLAC 2.2 C/C++ header file, copyright (c) 2004-2005 Ian Luck.
   Requires BASS 2.2 - available from www.un4seen.com

   See the BASSFLAC.CHM file for more complete documentation */

#ifndef BASSFLAC_H
#define BASSFLAC_H

#include "bass.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BASSFLACDEF
#define BASSFLACDEF(f) WINAPI f
#endif

// BASS_CHANNELINFO type
#define BASS_CTYPE_STREAM_FLAC	0x10900


HSTREAM BASSFLACDEF(BASS_FLAC_StreamCreateFile)(BOOL mem, const void *file, DWORD offset, DWORD length, DWORD flags);
HSTREAM BASSFLACDEF(BASS_FLAC_StreamCreateURL)(const char *url, DWORD offset, DWORD flags, DOWNLOADPROC *proc, DWORD user);
HSTREAM BASSFLACDEF(BASS_FLAC_StreamCreateFileUser)(BOOL buffered, DWORD flags, STREAMFILEPROC *proc, DWORD user);

#ifdef __cplusplus
}
#endif

#endif
