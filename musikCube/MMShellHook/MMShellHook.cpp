/*
 *  MMShellHook.cpp
 *
 *  MMShellHook is a quick little DLL that is used
 *    to handle Windows Multimedia Keyboards
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

#include "MMShellHook.h"
#include <tchar.h>
#include <windows.h>
#include <winuser.h>
#include <crtdbg.h>

///////////////////////////////////////////////////

// setup shared data
#pragma data_seg(".shared") 
HWND    hNotifyWnd = NULL;
HHOOK    hShellHook = NULL;
#pragma data_seg()

#pragma section(".shared",read,write,shared)

///////////////////////////////////////////////////

// per instance dll varaibles
HINSTANCE hInstance = NULL;
LRESULT CALLBACK ShellProc (int nCode, WPARAM wParam, LPARAM lParam);

///////////////////////////////////////////////////

BOOL WINAPI DllMain (HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

        _RPT0(_CRT_WARN, "MMShellHook: Hook DLL loaded\n");
        hInstance = (HINSTANCE)hInst;    

        // ALWAYS return TRUE to avoid breaking unhookable applications    ---//                
        return TRUE;                        

    case DLL_PROCESS_DETACH:
        _RPT0(_CRT_WARN, "MMShellHook: Hook DLL unloaded\n");
        return TRUE;

    default:
        return TRUE;
    }
}

///////////////////////////////////////////////////

DllExport BOOL SetMMShellHook(HWND hWnd)
{
    if (hWnd == NULL || hNotifyWnd != NULL)
        return FALSE;

    hShellHook = SetWindowsHookEx(WH_SHELL, (HOOKPROC) ShellProc, hInstance, 0L);                            

    if (hShellHook != NULL)
    {
        hNotifyWnd = hWnd;
        _RPT1(_CRT_WARN, "Notify Window is: %d\n", hWnd);
        return TRUE;
    }

    // fail
    return FALSE;
}

///////////////////////////////////////////////////

DllExport BOOL UnSetMMShellHook(HWND hWnd)
{
    BOOL unHooked = TRUE;

    if (hWnd == NULL || hWnd != hNotifyWnd)
        return FALSE;

    // unhook the process
    if (hNotifyWnd != NULL)
    {
        unHooked = UnhookWindowsHookEx(hShellHook);
        hShellHook = NULL;
    }

    // if unhook was succeeded, reset the handle
    if (unHooked)
        hNotifyWnd = NULL;

    return unHooked;
}

///////////////////////////////////////////////////

// shell hook callback

LRESULT CALLBACK ShellProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HSHELL_APPCOMMAND)
    {
        if (hNotifyWnd != NULL)
        {
            short AppCommand = GET_APPCOMMAND_LPARAM(lParam);    
            switch (AppCommand)
            {
            case APPCOMMAND_MEDIA_NEXTTRACK:
            case APPCOMMAND_MEDIA_PLAY_PAUSE:
            case APPCOMMAND_MEDIA_PREVIOUSTRACK:
            case APPCOMMAND_MEDIA_STOP:
                ::PostMessage(hNotifyWnd,WM_APPCOMMAND,wParam,lParam);
                return 0;
            }
        }
    }

    return CallNextHookEx (hShellHook, nCode, wParam, lParam);
}

///////////////////////////////////////////////////