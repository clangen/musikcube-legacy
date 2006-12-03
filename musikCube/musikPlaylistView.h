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

#include "musikCube.h"
#include "musikPlaylistCtrl.h"
#include "musikPlaylistInfoCtrl.h"
#include "musikPlaylistDropTarget.h"

#include <musikCore.h>

///////////////////////////////////////////////////

class CmusikPlaylistView : public CWnd
{
public:

    // construct and destruct
    CmusikPlaylistView(CFrameWnd* mainwnd, UINT dropid_l, UINT dropid_r, UINT dropid_sources, UINT dropid_selection_l, UINT dropid_selection_r, bool use_playlist_info = true, int type = PLAYLISTCTRL_NORMAL);
    virtual ~CmusikPlaylistView();

    // get the playlist control
    CmusikPlaylistCtrl* GetCtrl(){ return m_Playlist; }

    // playlist info
    void InitPlaylistInfo();
    void DeinitPlaylistInfo();
    void UpdatePlaylistInfo();
    void SetUsePlaylistInfo(bool use){ m_UsePlaylistInfo = use; }
    void SetDrawLineAtBottom(bool draw){ m_DrawLineAtBottom = draw; }

    // mfc message maps
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnNcPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
    afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);

protected:

    // playlist GUI object
    CmusikPlaylistCtrl* m_Playlist;
    CmusikPlaylistInfoCtrl* m_PlaylistInfo;

    // pointer to main window so we can
    // messages to it, such as updating the UI
    // after files have been dropped
    CWnd* m_Parent;
    bool m_UsePlaylistInfo;
    bool m_DrawLineAtBottom;
    
    // macros
    DECLARE_DYNAMIC(CmusikPlaylistView)
    DECLARE_MESSAGE_MAP()

};

///////////////////////////////////////////////////
