/* BASSenc 2.2 C/C++ header file, copyright (c) 2003-2005 Ian Luck.
   Requires BASS - available from www.un4seen.com

   See the BASSENC.CHM file for more complete documentation */

#ifndef BASSENC_H
#define BASSENC_H

#include "bass.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BASSENCDEF
#define BASSENCDEF(f) WINAPI f
#endif

// BASS_Encode_Start flags
#define BASS_ENCODE_NOHEAD		1	// do NOT send a WAV header to the encoder
#define BASS_ENCODE_FP_8BIT		2	// convert floating-point sample data to 8-bit integer
#define BASS_ENCODE_FP_16BIT	4	// convert floating-point sample data to 16-bit integer
#define BASS_ENCODE_FP_24BIT	6	// convert floating-point sample data to 24-bit integer
#define BASS_ENCODE_BIGEND		16	// big-endian sample data
#define BASS_ENCODE_PAUSE		32	// start encording paused


typedef void (CALLBACK ENCODEPROC)(DWORD channel, void *buffer, DWORD length, DWORD user);
/* Encoding callback function.
channel: The channel handle
buffer : Buffer containing the encoded data
length : Number of bytes
user   : The 'user' parameter value given when calling BASS_EncodeStart */


DWORD BASSENCDEF(BASS_Encode_Start)(DWORD handle, const char *cmdline, DWORD flags, ENCODEPROC *proc, DWORD user);
BOOL BASSENCDEF(BASS_Encode_IsActive)(DWORD handle);
BOOL BASSENCDEF(BASS_Encode_Stop)(DWORD handle);
BOOL BASSENCDEF(BASS_Encode_SetPaused)(DWORD handle, BOOL paused);
BOOL BASSENCDEF(BASS_Encode_Write)(DWORD handle, const void *buffer, DWORD length);

#ifdef __cplusplus
}
#endif

#endif
