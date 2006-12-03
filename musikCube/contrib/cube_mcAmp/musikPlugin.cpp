///////////////////////////////////////////////////
//
// License Agreement:
//
// The following are Copyright © 2002-2006, Casey Langen
//
// Sources and Binaries of: musikCore musikCube musikBox
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright 
//      notice, this list of conditions and the following disclaimer in the 
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the author nor the names of other contributors may 
//      be used to endorse or promote products derived from this software 
//      without specific prior written permission. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE. 
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "musikPlugin.h"
#include "windows.h"
#include "iniFile.h"

///////////////////////////////////////////////////

// *** THESE NEED TO BE SET BY YOU ***

musikCore::String g_Name = _T("Winamp Emulator");
musikCore::String g_Description = _T("Winamp window event emulation plugin (i.e. musikCube pretends to be Winamp)");
bool g_CanConfigure = false;
bool g_CanAbout = true;
bool g_CanExecute = false;
bool g_CanStop = false;
bool g_InSources = false;        // visible in the sources panel?
bool g_WantsPrefs = false;        // musikCube can take care of loading preferences for you

///////////////////////////////////////////////////

// these will get initialized automatically by musikCube

HMODULE g_Instance = NULL;
musikCore::Library* g_Library = NULL;
musikCore::Player* g_Player = NULL;
HWND g_MainWnd = NULL;
musikCore::Playlist* g_VisiblePlaylist = NULL;
CIniFile* g_Prefs = NULL;
int g_PluginRevision = 1;

///////////////////////////////////////////////////

// mcAmp vars

HWND g_WinampHWND = NULL;

///////////////////////////////////////////////////

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved    )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}

///////////////////////////////////////////////////

// do not modify these unless you know what you're doing

void SetPluginInstance(HMODULE instance)
{ g_Instance = instance; }

HMODULE GetPluginInstance()
{ return g_Instance; }

void Initialize(musikCore::Library* lib, musikCore::Player* player, HWND hwnd)
{ g_Library = lib; g_Player = player; g_MainWnd = hwnd; OnPluginAttach(); }

const musikCore::String& GetPluginName()
{ return g_Name; }

const musikCore::String& GetPluginDescription()
{ return g_Description; }

void SetPluginLibrary(musikCore::Library* lib)
{ g_Library = lib; }

void Unload()
{ OnPluginDetach(); }

void SetVisiblePlaylist(musikCore::Playlist* playlist)
{ g_VisiblePlaylist = playlist; }

void SetPrefs(CIniFile* prefs)
{ g_Prefs = prefs; }

///////////////////////////////////////////////////

// implement me

void Configure()
{
}

///////////////////////////////////////////////////

// implement me

void About()
{
    MessageBox(
        g_MainWnd,
        _T("mcAmp intercepts messages from other programs ment for\n")
        _T("Winamp, and transfers the respective commands to the\n")
        _T("musikCube interface, where they are processed and emulated.\n")
        _T("This works great for programs like EvilLyrics and FoxyTunes.")
        _T("\n\ncopyright (c) casey langen, 2005"),
        _T("musikCube: about mcAmp"),
        MB_OK | MB_ICONINFORMATION);
}

///////////////////////////////////////////////////

// implement me

void Execute()
{
}

///////////////////////////////////////////////////

// implement me

void Stop()
{
}

///////////////////////////////////////////////////

// optional

void OnPluginAttach()
{
    CreateWinampWnd();
}

///////////////////////////////////////////////////

// optional

void OnPluginDetach()
{
    // add your code here
    DestroyWindow(g_WinampHWND);
}

///////////////////////////////////////////////////

void OnSources()
{
    About();
}

///////////////////////////////////////////////////

void Functor::OnNewSong()
{
    if (g_WinampHWND)
    {
        musikCore::String winamp_title;
        
        winamp_title.Format(
            _T("%d. %s - %s - Winamp"), 
            g_Player->GetIndex() + 1,
            g_Player->GetPlaying()->GetArtist().c_str(),
            g_Player->GetPlaying()->GetTitle().c_str());

        ::SetWindowText(g_WinampHWND, winamp_title);
    }
}

///////////////////////////////////////////////////

void CreateWinampWnd()
{
    WNDCLASSEX wndclass;

    wndclass.cbSize = sizeof(wndclass);
    wndclass.style =  CS_PARENTDC | CS_VREDRAW;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = NULL;
    wndclass.hIcon = 0;
    wndclass.hCursor = (HICON) LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH) NULL;
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = _T("Winamp v1.x");
    wndclass.lpfnWndProc = WinampWndProc;
    wndclass.hIconSm = (HICON) NULL;

    // register the class
    if (!RegisterClassEx(&wndclass))
    {
        MessageBox(
            NULL, 
            _T("Winamp emulation window failed to register."), 
            _T("Emulation"), 
            MB_ICONWARNING);
    }

    // create the fake window
    g_WinampHWND = CreateWindowEx(
        0,
        _T("Winamp v1.x"), 
        _T("musikCube"), 
        0,  //style
        -4000, -4000, 256, 16,    // make sure to place it well off screen
        (HWND)NULL, (HMENU) NULL, NULL, NULL);


    ShowWindow(g_WinampHWND, SW_HIDE);
}

///////////////////////////////////////////////////

void ShowCube()
{
    ShowWindow(g_WinampHWND, SW_HIDE);

    int SHOWMUSIKCUBE = RegisterWindowMessage(_T("SHOWMUSIKCUBE"));
    ::SendMessage(g_MainWnd, SHOWMUSIKCUBE, NULL, NULL);
    ::BringWindowToTop(g_MainWnd);
}

///////////////////////////////////////////////////

void HideCube()
{
    ShowWindow(g_WinampHWND, SW_HIDE);
    
    int HIDEMUSIKCUBE = RegisterWindowMessage(_T("HIDEMUSIKCUBE"));
    ::SendMessage(g_MainWnd, HIDEMUSIKCUBE, NULL, NULL);
}

///////////////////////////////////////////////////

LRESULT CALLBACK WinampWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // setup some basic winamp handlers. thus far 
    // this is very incomplete, and only supports
    // a handfull of commands. more will come in time.
    switch (msg)
    {
    case WM_SYSCOMMAND:
        {
            switch (wParam)
            {
            case SC_MINIMIZE:
                HideCube();
                goto done;
            }
        }
        break;

    case WM_SIZE:
        {
            switch(wParam)
            {
            case SIZE_MINIMIZED:
                HideCube();
                goto done;
                break;
            }
        }

    case WM_SHOWWINDOW:
        {
            switch (wParam)
            {
            case TRUE:
                ShowCube();
                break;

                /*
            case FALSE:
                HideCube();
                break;
                */
            }

            goto done;
            break;
        }

    case WM_ACTIVATE:
        {
            switch (wParam)
            {
            case WA_ACTIVE:
            case WA_CLICKACTIVE:
                ShowCube();
                break;

                /*
            case WA_INACTIVE:
                HideCube();
                break;
                */
            }
            goto done;
            break;
        }

    

    case WM_COMMAND:
        {
            switch(wParam)
            {
            case 40044:                    // previous track
                g_Player->Prev();
                break;

            case 40048:                    // next track
                g_Player->Next();
                break;

            case 40046:                    // pause/resume
                {
                    if (g_Player->IsPlaying())
                    {
                        if (g_Player->IsPaused())
                            g_Player->Resume();
                        else
                            g_Player->Pause();
                    }
                }
                break;

            case 40045:                    // play
                {
                    if (g_Player->IsPlaying())
                    {
                        if (g_Player->IsPaused())
                            g_Player->Resume();
                    }
                    else
                    {
                        if (g_Player->GetPlaylist()->GetCount())
                            g_Player->Play(0);
                        else
                        {
                            int PLAYER_PLAYSEL = RegisterWindowMessage(_T("PLAYER_PLAYSEL"));
                            ::SendMessage(g_MainWnd, PLAYER_PLAYSEL, NULL, NULL);
                        }
                    }
                }
                break;

            case 40154:                    // start of playlist
                g_Player->Play(0);
                break;

            case 40158:                    // go to end of playlist
                {
                    int sz = g_Player->GetPlaylist()->GetCount();
                    if (sz)
                        g_Player->Play(sz - 1);
                }
                break;

            case 40047:                    // stop
            case 40147:                    // fade out and stop
                g_Player->Stop();
                break;

            case 40060:                    // fast forward 5 seconds    (Karl-Henrik Henriksson)
            case 40061:                    // rewind 5 seconds            (Karl-Henrik Henriksson)
                {
                    int dur = g_Player->GetDuration(musikCore::MUSIK_TIME_MSECS);
                    int time = g_Player->GetTimeNow(musikCore::MUSIK_TIME_MSECS);

                    time = (wParam == 40060) ? time + 5000 : time - 5000;
                    int per = (time * 100) / dur;
                    g_Player->SetTimeNowPer(per);
                }

            case 40058:                    // increase volume 1%    (Karl-Henrik Henriksson)
            case 40059:                    // decrease volume 1%    (Karl-Henrik Henriksson)
                {
                    int vol = g_Player->GetMaxVolume();
                    vol = (wParam == 40058) ? vol + 25 : vol - 25;
                    g_Player->SetMaxVolume(vol);
                }
                break;
            }
        }
        break;

    case WM_USER:
        {
            switch(lParam)
            {
            case 0:
                return (LRESULT)0x2000;
                break;

            case 104:            // is playing?
                {
                    if (g_Player->IsPlaying())
                    {
                        if (g_Player->IsPaused())
                            return 3L;
                        return 1L;
                    }

                    return 0L;
                }
                break;

            case 105:            // elapsed time and track length
                {
                    if (wParam == 0)    // elapsed time
                        return (LPARAM)g_Player->GetTimeNow(musikCore::MUSIK_TIME_MSECS);
                    else                // length
                        return (LPARAM)g_Player->GetDuration(musikCore::MUSIK_TIME_SECS);
                }                
                break;

            case 106:            // set time
                {
                    int duration = g_Player->GetDuration(musikCore::MUSIK_TIME_MSECS);
                    int pos = ((int)wParam * 100) / duration;
                    g_Player->SetTimeNowPer(pos);
                }
                break;

            case 122:
                {
                    if ((int)wParam == -666)    // get volume, not set...
                        return (LRESULT)g_Player->GetMaxVolume();    
                    else
                    {
                        g_Player->SetMaxVolume((int)wParam);
                        int WM_UPDATEVOLUME    = RegisterWindowMessage(_T("UPDATEVOLUME"));
                        ::SendMessage(g_MainWnd, WM_UPDATEVOLUME, NULL, NULL);
                    }
                }
                break;

            case 124:            // playlist size
                return (LPARAM)g_Player->GetPlaylist()->GetCount();
                break;

            case 125:            // playlist pos
                return (LPARAM)g_Player->GetIndex();
                break;

            case 126:            // sample rate, bitrate, number of channels
                {                    
                    if (wParam == 0)
                        return 44L;            // sample rate
                    else if (wParam == 1)    // bitrate
                        return (LPARAM)musikCore::StringToInt(g_Player->GetPlaying()->GetBitrate());
                    else                    // channels
                        return 2L;
                    }
                break;
            }
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);

done:
    return 1L;
}

///////////////////////////////////////////////////

