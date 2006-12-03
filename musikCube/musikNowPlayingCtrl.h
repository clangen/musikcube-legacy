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
#include "musikDockBar.h"
#include "musikNowPlayingInfo.h"
#include "musikVolumeCtrl.h"
#include "musikTimeCtrl.h."
#include "musikToggleBtn.h"

#include <musikCore.h>

///////////////////////////////////////////////////

class CmusikNowPlayingCtrl;

///////////////////////////////////////////////////

enum
{
    MUSIK_NOWPLAYING_BTN_PLAY = 4957,
    MUSIK_NOWPLAYING_BTN_PREV,
    MUSIK_NOWPLAYING_BTN_NEXT,
    MUSIK_NOWPLAYING_BTN_STOP,
    MUSIK_NOWPLAYING_ENB_CROSSFADER,
    MUSIK_NOWPLAYING_ENB_EQUALIZER,
    MUSIK_NOWPLAYING_ENB_RANDOM,
    MUSIK_NOWPLAYING_ENB_REPEATPLAYLIST
};

#define MUSIK_NOWPLAYING_BTN_CNT 4
#define MUSIK_NOWPLAYING_TOGGLE_CNT 4

///////////////////////////////////////////////////

// CmusikNowPlayingBar

///////////////////////////////////////////////////

#ifndef baseCmusikNowPlayingBar
    #define baseCmusikNowPlayingBar CmusikDockBar
#endif


class CmusikNowPlayingBar : public baseCmusikNowPlayingBar
{
public:

    // construct and destruct
    CmusikNowPlayingBar(CFrameWnd* parent);
    virtual ~CmusikNowPlayingBar();

    // get control
    CmusikNowPlayingCtrl* GetCtrl(){ return m_wndChild; }
    virtual CSize CalcDynamicLayout(int nLength, DWORD dwMode);

protected:

    virtual CWnd* GetMainWindow() { return (CWnd*) this->m_wndChild; }

    // default stuff
    CmusikNowPlayingCtrl* m_wndChild;
    CFont m_Font;

    // message map
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    // macros
    DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

// CmusikNowPlayingCtrl

///////////////////////////////////////////////////

class CmusikNowPlayingCtrl : public CWnd
{
public:

    // construct and destruct
    CmusikNowPlayingCtrl(CFrameWnd* parent);
    virtual ~CmusikNowPlayingCtrl();

    // getting children
    CmusikTimeCtrl* GetTimeCtrl(){ return m_Track; }
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    // layout and update
    void UpdateInfo(bool redraw = true);
    void UpdateVolume();
    void UpdateCheckboxes();
    void RescaleInfo();

protected:

    // pointer to the main dialog
    CFrameWnd* m_MainWnd;

    afx_msg void OnSysColorChange();

    void LoadBitmaps();
    void UnloadBitmaps();
    void SetButtonBitmaps();
    void SetPlayingButton();

    // message maps
    afx_msg void OnPaint();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDestroy();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnBtnPlay();
    afx_msg void OnBtnPrev();
    afx_msg void OnBtnNext();
    afx_msg void OnPlayrandomArtist();
    afx_msg void OnPlayrandomAlbum();
    afx_msg void OnPlayrandomGenre();
    afx_msg void OnPlayrandomYear();
    afx_msg void OnQueuerandomArtist();
    afx_msg void OnQueuerandomAlbum();
    afx_msg void OnQueuerandomGenre();
    afx_msg void OnQueuerandomYear();
    afx_msg void OnBtnStop();
    afx_msg void OnEnbCrossfader();
    afx_msg void OnEnbEqualizer();
    afx_msg void OnEnbRandom();
    afx_msg void OnEnbRepeat();
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);

    // info object 1: title, 2 artist/album
    CmusikNowPlayingInfo* m_Info1;
    CmusikNowPlayingInfo* m_Info2;

    // volume slider
    CmusikVolumeWnd *m_Volume;

    // seek slider
    CmusikTimeCtrl* m_Track;

    // buttons
    CmusikToggleBtn* m_Play;
    CmusikToggleBtn* m_Prev;
    CmusikToggleBtn* m_Next;
    CmusikToggleBtn* m_Stop;
    CmusikToggleBtn* m_EnbCrossfader;
    CmusikToggleBtn* m_EnbEqualizer;
    CmusikToggleBtn* m_EnbRandom;
    CmusikToggleBtn* m_EnbRepeatPlaylist;

    // default font
    CFont m_Font;
    CToolTipCtrl m_Tips;

    void PlayRandom(int field);
    void QueueRandom(int field);

    // macros
    DECLARE_DYNAMIC(CmusikNowPlayingCtrl)
    DECLARE_MESSAGE_MAP()

private:
    
    Gdiplus::Bitmap* m_pPrevOff;
    Gdiplus::Bitmap* m_pPrevOn;
    Gdiplus::Bitmap* m_pPlayOff;
    Gdiplus::Bitmap* m_pPlayOn;
    Gdiplus::Bitmap* m_pPauseOff;
    Gdiplus::Bitmap* m_pPauseOn;
    Gdiplus::Bitmap* m_pStopOff;
    Gdiplus::Bitmap* m_pStopOn;
    Gdiplus::Bitmap* m_pNextOff;
    Gdiplus::Bitmap* m_pNextOn;
    Gdiplus::Bitmap* m_pCfOff;
    Gdiplus::Bitmap* m_pCfOn;
    Gdiplus::Bitmap* m_pEqOff;
    Gdiplus::Bitmap* m_pEqOn;
    Gdiplus::Bitmap* m_pRepeatOff;
    Gdiplus::Bitmap* m_pRepeatOn;
    Gdiplus::Bitmap* m_pRandomOff;
    Gdiplus::Bitmap* m_pRandomOn;
};

///////////////////////////////////////////////////
