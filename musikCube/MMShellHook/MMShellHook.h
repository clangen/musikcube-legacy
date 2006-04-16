/*
 *  MMShellHook.h
 *
 *  MMShellHook is a quick little DLL that is used
 *	to handle Windows Multimedia Keyboards
 *  
 *  Copyright (c) 2003 Gunnar Roth
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

///////////////////////////////////////////////////
//
// Originally written for the wxMusik project by Gunnar Roth,
// it has been slightly modified to work with musikCube by
// Casey Langen.
//
///////////////////////////////////////////////////

///////////////////////////////////////////////////

// thanks, gunnar....
#ifndef WM_APPCOMMAND
	#define WM_APPCOMMAND                   0x0319
	#define FAPPCOMMAND_MASK  0xF000
	#define GET_APPCOMMAND_LPARAM(lParam) ((short)(HIWORD(lParam) & ~FAPPCOMMAND_MASK))
	#define APPCOMMAND_MEDIA_NEXTTRACK        11
	#define APPCOMMAND_MEDIA_PREVIOUSTRACK    12
	#define APPCOMMAND_MEDIA_STOP             13
	#define APPCOMMAND_MEDIA_PLAY_PAUSE       14
#endif

///////////////////////////////////////////////////

#if !defined(_MMShellHook_DLL_)
#define _MMShellHook_DLL_

///////////////////////////////////////////////////

#include <windows.h>

///////////////////////////////////////////////////

#define DllImport __declspec( dllimport )
#define DllExport __declspec( dllexport )

///////////////////////////////////////////////////

extern "C"
{
	DllExport BOOL SetMMShellHook	( HWND hWnd	);	
	DllExport BOOL UnSetMMShellHook	( HWND hWnd );	
}

///////////////////////////////////////////////////

#endif 

///////////////////////////////////////////////////
