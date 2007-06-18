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
#include "musikCube.h"
#include "musikGlobalHotkeysDlg.h"
#include "musikCubeMessages.h"
#include "musikCubeFrame.h"
#include "GlobalHotkeys.h"

#pragma warning(disable : 4800) // forcing bool to int

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikGlobalHotkeysDlg, CDialog)

///////////////////////////////////////////////////

CmusikGlobalHotkeysDlg::CmusikGlobalHotkeysDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CmusikGlobalHotkeysDlg::IDD, pParent)
{
}

CmusikGlobalHotkeysDlg::~CmusikGlobalHotkeysDlg()
{
}

///////////////////////////////////////////////////

void CmusikGlobalHotkeysDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PLAYPAUSE_HOTKEY, m_PlayPause);
    DDX_Control(pDX, IDC_STOP_HOTKEY, m_Stop);
    DDX_Control(pDX, IDC_NEXTTRACK_HOTKEY, m_NextTrack);
    DDX_Control(pDX, IDC_PREVTRACK_HOTKEY, m_PrevTrack);
    DDX_Control(pDX, IDC_VOLUMEUP_HOTKEY, m_VolumeUp);
    DDX_Control(pDX, IDC_VOLUMEDOWN_HOTKEY, m_VolumeDown);
    DDX_Control(pDX, IDC_NEXTARTIST_HOTKEY, m_NextArtist);
    DDX_Control(pDX, IDC_PREVARTIST_HOTKEY, m_PrevArtist);
    DDX_Control(pDX, IDC_NEXTALBUM_HOTKEY, m_NextAlbum);
    DDX_Control(pDX, IDC_PREVALBUM_HOTKEY, m_PrevAlbum);
    DDX_Control(pDX, IDC_ENABLEHOTKEYS_CHECKBOX, m_EnableHotkeys);
	DDX_Control(pDX, IDC_VOLUMEKEYS_CONTROL_MUSIKCUBE_VOL_CHECKBOX, m_EnableVolkeys);
}

///////////////////////////////////////////////////

void CmusikGlobalHotkeysDlg::SetHotkeyControl(CHotKeyCtrl& control, DWORD value)
{
    WORD virtualKey, modifiers;
    GlobalHotkeys::DeserializeHotkey(value, virtualKey, modifiers);
    control.SetHotKey(virtualKey, modifiers);
}

///////////////////////////////////////////////////

void CmusikGlobalHotkeysDlg::LoadHotkeys()
{
    this->m_EnableHotkeys.SetCheck(musikCube::g_Prefs->GetUseGlobalHotkeys());
    //
    this->SetHotkeyControl(this->m_PlayPause, musikCube::g_Prefs->GetPlayPauseHotkey());
    this->SetHotkeyControl(this->m_Stop, musikCube::g_Prefs->GetStopHotkey());
    this->SetHotkeyControl(this->m_NextTrack, musikCube::g_Prefs->GetNextTrackHotkey());
    this->SetHotkeyControl(this->m_PrevTrack, musikCube::g_Prefs->GetPrevTrackHotkey());
    this->SetHotkeyControl(this->m_VolumeUp, musikCube::g_Prefs->GetVolumeUpHotkey());
    this->SetHotkeyControl(this->m_VolumeDown, musikCube::g_Prefs->GetVolumeDownHotkey());
    this->SetHotkeyControl(this->m_NextArtist, musikCube::g_Prefs->GetNextArtistHotkey());
    this->SetHotkeyControl(this->m_PrevArtist, musikCube::g_Prefs->GetPrevArtistHotkey());
    this->SetHotkeyControl(this->m_NextAlbum, musikCube::g_Prefs->GetNextAlbumHotkey());
    this->SetHotkeyControl(this->m_PrevAlbum, musikCube::g_Prefs->GetPrevAlbumHotkey());
	this->m_EnableVolkeys.SetCheck(musikCube::g_Prefs->GetUseVolumeHotkeys());

}

///////////////////////////////////////////////////

void CmusikGlobalHotkeysDlg::SaveHotkeys()
{
    musikCube::g_Prefs->SetUseGlobalHotkeys(this->m_EnableHotkeys.GetCheck());
    //
    musikCube::g_Prefs->SetPlayPauseHotkey(this->m_PlayPause.GetHotKey());
    musikCube::g_Prefs->SetStopHotkey(this->m_Stop.GetHotKey());
    musikCube::g_Prefs->SetNextTrackHotkey(this->m_NextTrack.GetHotKey());
    musikCube::g_Prefs->SetPrevTrackHotkey(this->m_PrevTrack.GetHotKey());
    musikCube::g_Prefs->SetVolumeUpHotkey(this->m_VolumeUp.GetHotKey());
    musikCube::g_Prefs->SetVolumeDownHotkey(this->m_VolumeDown.GetHotKey());
    musikCube::g_Prefs->SetNextArtistHotkey(this->m_NextArtist.GetHotKey());
    musikCube::g_Prefs->SetPrevArtistHotkey(this->m_PrevArtist.GetHotKey());
    musikCube::g_Prefs->SetNextAlbumHotkey(this->m_NextAlbum.GetHotKey());
    musikCube::g_Prefs->SetPrevAlbumHotkey(this->m_PrevAlbum.GetHotKey());
	musikCube::g_Prefs->SetUseVolumeHotkeys(this->m_EnableVolkeys.GetCheck());

    if (musikCube::g_Prefs->GetUseGlobalHotkeys())
    {
        GlobalHotkeys::ReloadHotkeys();
    }
    else
    {
        GlobalHotkeys::UnloadHotkeys();
    }
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikGlobalHotkeysDlg, CDialog)
    ON_BN_CLICKED(IDC_HOTKEYS_OK_BUTTON, OnBnClickedOK)
    ON_BN_CLICKED(IDC_CANCEL_HOTKEYS_CANCEL_BUTTON, OnBnClickedCancel)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikGlobalHotkeysDlg::OnBnClickedOK()
{
    this->SaveHotkeys();
    this->OnOK();
}

///////////////////////////////////////////////////

void CmusikGlobalHotkeysDlg::OnOK()
{
    CDialog::OnOK();
}

///////////////////////////////////////////////////

void CmusikGlobalHotkeysDlg::OnBnClickedCancel()
{
    this->OnCancel();
}

///////////////////////////////////////////////////

BOOL CmusikGlobalHotkeysDlg::OnInitDialog()
{
    BOOL result = CDialog::OnInitDialog();
    this->LoadHotkeys();
    return result;
}

///////////////////////////////////////////////////