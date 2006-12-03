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
#include "GlobalHotkeys.h"
#include "musikCube.h"
#include "musikCubeMessages.h"
#include "musikCubeFrame.h"
#include <musikCore.h>

///////////////////////////////////////////////////

void GlobalHotkeys::SetHotkey(HWND hwnd, enum Hotkey hotkey, DWORD serializedHotkey)
{
    if (serializedHotkey != 0)
    {
        WORD virtualKey, modifiers;
        GlobalHotkeys::DeserializeHotkey(serializedHotkey, virtualKey, modifiers);

        WORD realModifiers = 0;
        if (modifiers & HOTKEYF_ALT) realModifiers |= MOD_ALT;
        if (modifiers & HOTKEYF_CONTROL) realModifiers |= MOD_CONTROL;
        if (modifiers & HOTKEYF_SHIFT) realModifiers |= MOD_SHIFT;
        if (modifiers & HOTKEYF_EXT) realModifiers |= MOD_WIN;

        ::RegisterHotKey(hwnd, (int) hotkey, realModifiers, virtualKey);
    }
}

///////////////////////////////////////////////////

void GlobalHotkeys::ReloadHotkeys()
{
    CmusikCubeFrame* musikFrame = (CmusikCubeFrame*) AfxGetApp()->GetMainWnd();
    HWND musikHWND = musikFrame->GetSafeHwnd();

    GlobalHotkeys::UnloadHotkeys();

    GlobalHotkeys::SetHotkey(musikHWND, Hotkey::PlayPauseResume, musikCube::g_Prefs->GetPlayPauseHotkey());
    GlobalHotkeys::SetHotkey(musikHWND, Hotkey::Stop, musikCube::g_Prefs->GetStopHotkey());
    GlobalHotkeys::SetHotkey(musikHWND, Hotkey::NextTrack, musikCube::g_Prefs->GetNextTrackHotkey());
    GlobalHotkeys::SetHotkey(musikHWND, Hotkey::PrevTrack, musikCube::g_Prefs->GetPrevTrackHotkey());
    GlobalHotkeys::SetHotkey(musikHWND, Hotkey::VolumeUp, musikCube::g_Prefs->GetVolumeUpHotkey());
    GlobalHotkeys::SetHotkey(musikHWND, Hotkey::VolumeDown, musikCube::g_Prefs->GetVolumeDownHotkey());
    GlobalHotkeys::SetHotkey(musikHWND, Hotkey::NextArtist, musikCube::g_Prefs->GetNextArtistHotkey());
    GlobalHotkeys::SetHotkey(musikHWND, Hotkey::PrevArtist, musikCube::g_Prefs->GetPrevArtistHotkey());
    GlobalHotkeys::SetHotkey(musikHWND, Hotkey::NextAlbum, musikCube::g_Prefs->GetNextAlbumHotkey());
    GlobalHotkeys::SetHotkey(musikHWND, Hotkey::PrevAlbum, musikCube::g_Prefs->GetPrevAlbumHotkey());
    //
    ::RegisterHotKey(musikHWND, Hotkey::MediaKeyPlayPauseResume, 0,VK_MEDIA_PLAY_PAUSE);
    ::RegisterHotKey(musikHWND, Hotkey::MediaKeyStop, 0, VK_MEDIA_STOP);
    ::RegisterHotKey(musikHWND, Hotkey::MediaKeyNextTrack, 0, VK_MEDIA_NEXT_TRACK);
    ::RegisterHotKey(musikHWND, Hotkey::MediaKeyPrevTrack, 0, VK_MEDIA_PREV_TRACK);
    ::RegisterHotKey(musikHWND, Hotkey::MediaKeyVolumeUp, 0, VK_VOLUME_UP);
    ::RegisterHotKey(musikHWND, Hotkey::MediaKeyVolumeDown, 0, VK_VOLUME_DOWN);
}

///////////////////////////////////////////////////

void GlobalHotkeys::UnloadHotkeys()
{
    CmusikCubeFrame* musikFrame = (CmusikCubeFrame*) AfxGetApp()->GetMainWnd();
    HWND musikHWND = musikFrame->GetSafeHwnd();

    ::UnregisterHotKey(musikHWND, Hotkey::PlayPauseResume);
    ::UnregisterHotKey(musikHWND, Hotkey::Stop);
    ::UnregisterHotKey(musikHWND, Hotkey::NextTrack);
    ::UnregisterHotKey(musikHWND, Hotkey::PrevTrack);
    ::UnregisterHotKey(musikHWND, Hotkey::VolumeUp);
    ::UnregisterHotKey(musikHWND, Hotkey::VolumeDown);
    ::UnregisterHotKey(musikHWND, Hotkey::NextArtist);
    ::UnregisterHotKey(musikHWND, Hotkey::PrevArtist);
    ::UnregisterHotKey(musikHWND, Hotkey::NextAlbum);
    ::UnregisterHotKey(musikHWND, Hotkey::PrevAlbum);
    //
    ::UnregisterHotKey(musikHWND, Hotkey::MediaKeyPlayPauseResume);
    ::UnregisterHotKey(musikHWND, Hotkey::MediaKeyStop);
    ::UnregisterHotKey(musikHWND, Hotkey::MediaKeyNextTrack);
    ::UnregisterHotKey(musikHWND, Hotkey::MediaKeyPrevTrack);
    ::UnregisterHotKey(musikHWND, Hotkey::MediaKeyVolumeUp);
    ::UnregisterHotKey(musikHWND, Hotkey::MediaKeyVolumeDown);
}

///////////////////////////////////////////////////

bool GlobalHotkeys::ProcessHotkey(enum GlobalHotkeys::Hotkey hotkey, std::wstring& statusString)
{
    CmusikCubeFrame* musikFrame = (CmusikCubeFrame*) AfxGetApp()->GetMainWnd();
    HWND musikHWND = musikFrame->GetSafeHwnd();

    switch(hotkey)
    {
    ////////////////////////
    // play, pause, resume
    ////////////////////////
    case Hotkey::PlayPauseResume:  
    case Hotkey::MediaKeyPlayPauseResume:
        {
            if (musikCube::g_Player->IsPaused())
            {
                musikCube::g_Player->Resume();
                statusString = L"Resume Playback";
            }
            else if (musikCube::g_Player->IsPlaying())
            {
                statusString = L"Pause Playback";
                musikCube::g_Player->Pause();
            }
            else
            {
                statusString = L"Start Playback";
                ::PostMessage(musikHWND, WM_PLAYER_PLAYSEL, 0, 0);
            }
        }
        return true;

    ////////////////////////
    // stop
    ////////////////////////
    case Hotkey::Stop:
    case Hotkey::MediaKeyStop:
        {
            statusString = L"Stop Playback";
            musikCube::g_Player->Stop();
        }
        break;

    ////////////////////////
    // next track
    ////////////////////////
    case Hotkey::NextTrack:
    case Hotkey::MediaKeyNextTrack:
        if (musikCube::g_Player->IsPlaying())
        {
            statusString = L"Next Song";
            musikCube::g_Player->Next();
        }
        return true;

    ////////////////////////
    // prev track
    ////////////////////////
    case Hotkey::PrevTrack:
    case Hotkey::MediaKeyPrevTrack:
        if (musikCube::g_Player->IsPlaying())
        {
            statusString = L"Prev Song";
            musikCube::g_Player->Prev();
        }
        return true;

    ////////////////////////
    // volume up
    ////////////////////////
    case Hotkey::VolumeUp:
    case Hotkey::MediaKeyVolumeUp:
        {
            int volume = musikCube::g_Player->GetMaxVolume() + 12;
            if (volume > 255) volume = 255;
            musikCube::g_Player->SetMaxVolume(volume);
            musikFrame->m_NowPlayingView->GetCtrl()->UpdateVolume();     // hack
            
            int vol = 100 * musikCube::g_Player->GetMaxVolume() / 255;
            musikCore::String info;
            info.Format(_T("Volume: %d%%"), vol);
            //
            statusString = info;
        }
        return true;

    ////////////////////////
    // volume down
    ////////////////////////
    case Hotkey::VolumeDown:
    case Hotkey::MediaKeyVolumeDown:
        {
            int volume = musikCube::g_Player->GetMaxVolume() - 12;
            if (volume < 0) volume = 0;
            musikCube::g_Player->SetMaxVolume(volume);
            musikFrame->m_NowPlayingView->GetCtrl()->UpdateVolume();     // hack

            int vol = 100 * musikCube::g_Player->GetMaxVolume() / 255;
            musikCore::String info;
            info.Format(L"Volume: %d%%", vol);
            //
            statusString = info;
        }
        return true;

    ////////////////////////
    // previous artist, next artist, prev artist album, next artist album
    ////////////////////////
    case Hotkey::PrevArtist:
    case Hotkey::NextArtist:
    case Hotkey::PrevAlbum:
    case Hotkey::NextAlbum:
        {
            musikCore::String currArtist = musikCube::g_Player->GetPlaying()->GetArtist();
            musikCore::String currAlbum = musikCube::g_Player->GetPlaying()->GetAlbum(); 
            musikCore::Playlist* nowPlaying = musikCube::g_Player->GetPlaylist();

            switch(hotkey)
            {
            case Hotkey::PrevArtist:
                musikCube::g_Library->GetPrevArtist(currArtist, *nowPlaying);
                statusString = L"Previous Artist";
                break;
            case Hotkey::NextArtist:
                musikCube::g_Library->GetNextArtist(currArtist, *nowPlaying);
                statusString = L"Next Artist";
                break;
            case Hotkey::PrevAlbum:
                musikCube::g_Library->GetPrevAlbum(currArtist, currAlbum, *nowPlaying);
                statusString = L"Previous Album by Artist";
                break;
            case Hotkey::NextAlbum:
                musikCube::g_Library->GetNextAlbum(currArtist, currAlbum, *nowPlaying);
                statusString = L"Next Album by Artist";
                break;
            }
            
            if (musikFrame->m_PlaylistView->GetCtrl()->GetPlaylist() == nowPlaying)
            {
                musikFrame->UpdateFilters(CmusikCubeFrame::UpdateAction::None, CmusikCubeFrame::UpdateAction::Requery);   // hack
            }

            if (nowPlaying->size())
            {
                musikCube::g_Player->Play(0);
            }
        }
        return true;
    }

    return false;
}

///////////////////////////////////////////////////

void GlobalHotkeys::DeserializeHotkey(DWORD hotkey, WORD& virtualKey, WORD& modifiers)
{
    short hotkeyShort = (short) hotkey;
    virtualKey = (WORD) (hotkeyShort & 0x00ff);
    modifiers = (WORD) ((hotkeyShort >> 8) & 0x0ff);
}

///////////////////////////////////////////////////
