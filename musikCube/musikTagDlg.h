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

#include "Resource.h"
#include "musikGenres.h"
#include <musikCore.h>

///////////////////////////////////////////////////

class CmusikTagDlg : public CDialog
{
    
public:
    
    enum
    {
        MODE_PLAYLIST = 0,
        MODE_SONGINFO
    };

    // construct and destruct 
    CmusikTagDlg(CWnd* parent, musikCore::Playlist* playlist); 
    CmusikTagDlg(CWnd* parent, musikCore::SongInfoArray* songinfo);

    virtual ~CmusikTagDlg();

    // helpers
    void UpdateSel(int index = 0);
    void UpdatePlaylist(musikCore::Playlist* playlist){ m_Playlist = playlist; }

    // misc
    enum { IDD = IDD_TAG_PROPERTIES };
    virtual void DoDataExchange(CDataExchange* pDX);  
    void OnClose();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnInitDialog();

    // mfc messages
    afx_msg void OnBnClickedClose();
    afx_msg void OnBnClickedNext();
    afx_msg void OnBnClickedPrev();
    afx_msg void OnChangeField();
    afx_msg void OnChangeRating();
    afx_msg void OnBnClickedApply();

protected:

    // update info
    void UpdateDlg();
    void SaveCurr();
    void PromptSave();

    void OnEnter();
    void OnShiftEnter();

    void SelectFocused();

    bool m_Modified;
    bool m_Updating;

    // from parent
    CWnd* m_Parent;
    musikCore::Playlist* m_Playlist;
    musikCore::SongInfoArray* m_SongInfoArray;
    musikCore::SongInfo m_Song;

    int m_Mode;
    int m_SongIndex;

    // macros
    DECLARE_DYNAMIC(CmusikTagDlg)
    DECLARE_MESSAGE_MAP()

};

///////////////////////////////////////////////////
