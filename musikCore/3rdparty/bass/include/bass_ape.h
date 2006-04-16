#ifndef BASSAPE_H
#define BASSAPE_H

#include "bass.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BASSAPEDEF
#define BASSAPEDEF(f) WINAPI f
#endif

// Additional tags available from BASS_StreamGetTags
#define BASS_TAG_APE		6	// APE tags

// BASS_CHANNELINFO type
#define BASS_CTYPE_STREAM_APE	0x10700


HSTREAM BASSAPEDEF(BASS_APE_StreamCreateFile)(const char *file, DWORD flags);

#ifdef __cplusplus
}
#endif

#endif
