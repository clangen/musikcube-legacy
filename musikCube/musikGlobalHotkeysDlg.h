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

#pragma once

///////////////////////////////////////////////////

class CmusikGlobalHotkeysDlg : public CDialog
{
public:             //methods

    CmusikGlobalHotkeysDlg(CWnd* pParent = NULL);
    virtual ~CmusikGlobalHotkeysDlg();

    virtual void OnOK();
    virtual BOOL OnInitDialog();

    enum { IDD = IDD_GLOBAL_HOTKEYS };

protected:          // methods

    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_DYNAMIC(CmusikGlobalHotkeysDlg)
    DECLARE_MESSAGE_MAP()

    void SaveHotkeys();
    void LoadHotkeys();
    void SetHotkeyControl(CHotKeyCtrl& control, DWORD value);

    afx_msg void OnBnClickedOK();
    afx_msg void OnBnClickedCancel();

protected:          // variables

    CHotKeyCtrl m_PlayPause;
    CHotKeyCtrl m_Stop;
    CHotKeyCtrl m_NextTrack;
    CHotKeyCtrl m_PrevTrack;
    CHotKeyCtrl m_VolumeUp;
    CHotKeyCtrl m_VolumeDown;
    CHotKeyCtrl m_NextArtist;
    CHotKeyCtrl m_PrevArtist;
    CHotKeyCtrl m_NextAlbum;
    CHotKeyCtrl m_PrevAlbum;
    CButton m_EnableHotkeys;
	CButton m_EnableVolkeys;
};

///////////////////////////////////////////////////
