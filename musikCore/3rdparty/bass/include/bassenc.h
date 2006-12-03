/* BASSenc 2.3 C/C++ header file, copyright (c) 2003-2006 Ian Luck.
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

typedef DWORD HENCODE;		// encoder handle

// Additional error codes returned by BASS_ErrorGetCode
#define BASS_ERROR_ACM_CANCEL	2000	// ACM codec selection cancelled

// Additional config options
#define BASS_CONFIG_ENCODE_PRIORITY		0x10300 // encoder DSP priority

// BASS_Encode_Start flags
#define BASS_ENCODE_NOHEAD		1	// do NOT send a WAV header to the encoder
#define BASS_ENCODE_FP_8BIT		2	// convert floating-point sample data to 8-bit integer
#define BASS_ENCODE_FP_16BIT	4	// convert floating-point sample data to 16-bit integer
#define BASS_ENCODE_FP_24BIT	6	// convert floating-point sample data to 24-bit integer
#define BASS_ENCODE_BIGEND		16	// big-endian sample data
#define BASS_ENCODE_PAUSE		32	// start encording paused
#define BASS_ENCODE_AUTOFREE	0x40000	// free the encoder when the channel is freed

// BASS_Encode_GetACMFormat flags
#define BASS_ACM_DEFAULT		1	// use the format as default selection
#define BASS_ACM_RATE			2	// only list formats with same sample rate as the source channel
#define BASS_ACM_CHANS			4	// only list formats with same number of channels (eg. mono/stereo)
#define BASS_ACM_SUGGEST		8	// suggest a format (HIWORD=format tag)


typedef void (CALLBACK ENCODEPROC)(HENCODE handle, DWORD channel, const void *buffer, DWORD length, DWORD user);
/* Encoding callback function.
handle : The encoder
channel: The channel handle
buffer : Buffer containing the encoded data
length : Number of bytes
user   : The 'user' parameter value given when calling BASS_EncodeStart */


HENCODE BASSENCDEF(BASS_Encode_Start)(DWORD handle, const char *cmdline, DWORD flags, ENCODEPROC *proc, DWORD user);
DWORD BASSENCDEF(BASS_Encode_IsActive)(DWORD handle);
BOOL BASSENCDEF(BASS_Encode_Stop)(DWORD handle);
BOOL BASSENCDEF(BASS_Encode_SetPaused)(DWORD handle, BOOL paused);
BOOL BASSENCDEF(BASS_Encode_Write)(DWORD handle, const void *buffer, DWORD length);

#ifdef _WIN32
DWORD BASSENCDEF(BASS_Encode_GetACMFormat)(DWORD handle, void *form, DWORD formlen, const char *title, DWORD flags);
HENCODE BASSENCDEF(BASS_Encode_StartACM)(DWORD handle, const void *form, DWORD flags, ENCODEPROC *proc, DWORD user);
HENCODE BASSENCDEF(BASS_Encode_StartACMFile)(DWORD handle, const void *form, DWORD flags, const char *file);
#endif

#ifdef __cplusplus
}
#endif

#endif
