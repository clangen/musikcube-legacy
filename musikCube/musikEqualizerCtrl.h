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
#include "musikTrackCtrl.h"
#include "musikEqualizerSets.h"
#include "musikTrackCtrl.h"
#include "musikDynamicText.h"

#include <musikCore.h>
#include <afxcmn.h>

///////////////////////////////////////////////////

class CmusikEqualizerCtrl;

///////////////////////////////////////////////////

#ifndef baseCmusikEqualizerBar
    #define baseCmusikEqualizerBar CmusikDockBar
#endif

///////////////////////////////////////////////////

// CmusikEqualizerBar

///////////////////////////////////////////////////

class CmusikEqualizerBar : public baseCmusikEqualizerBar
{
    friend class CmusikEqualizerSets;

public:

    // construct / destruct
    CmusikEqualizerBar();
    virtual ~CmusikEqualizerBar();

    // getting and setting equalizer
    void GetActiveEqualizer(musikCore::EQSettings* settings);
    void SetActiveEqualizer(const musikCore::EQSettings& settings);

    // virtual
    virtual void OnOptions();

    // gets
    CmusikEqualizerCtrl* GetCtrl(){ return m_wndChild; }

protected:

    virtual CWnd* GetMainWindow() { return (CWnd*) this->m_wndChild; }

    // children
    CmusikEqualizerCtrl* m_wndChild;

    // child's font
    CFont m_Font;

    // presets dialog
    CmusikEqualizerSets* m_Presets;

    // message maps
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnEqualizerPresets();
    afx_msg void OnEqualizerState18band();
    afx_msg void OnEqualizerState6band();
    afx_msg void OnEqualizerSetAsDefault();
    afx_msg void OnEqualizerResetdefault();
    afx_msg void OnEqualizerAutomaticallySave();
    afx_msg void OnEqualizerCloseView();

    // custom message maps
    afx_msg LRESULT OnClosePresets(WPARAM wParam, LPARAM lParam);

    // macros
    DECLARE_DYNAMIC(CmusikEqualizerBar)
    DECLARE_MESSAGE_MAP()

};

///////////////////////////////////////////////////

// CmusikEqualizerCtrl

///////////////////////////////////////////////////

enum 
{
    MUSIK_EQUALIZER_CTRL_18BANDS = 18,
    MUSIK_EQUALIZER_CTRL_6BANDS = 6
};

///////////////////////////////////////////////////

class CmusikEqualizerCtrl;

class CmusikEQSlider : public CmusikTrackCtrl
{
public:

    CmusikEQSlider();
    virtual void OnPosChanged();
    CmusikEqualizerCtrl* m_Parent;
};

///////////////////////////////////////////////////

class CmusikEqualizerCtrl : public CWnd
{
    friend class CmusikEqualizerSets;
    friend class CmusikEqualizerBar;

public:

    // construct and destruct
    CmusikEqualizerCtrl();
    virtual ~CmusikEqualizerCtrl();

    // band state
    void SetBandState(int band_state);
    int GetBandState(){ return m_BandState; }
    void LayoutNewState();
    void OnAlwaysOnTop();

    // actions
    void LoadCurrSong();
    void LoadDefault();
    void SetAsDefault();
    void DisplayChanged();
    void ResetDefault();

    // message maps
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    // custom message maps
    afx_msg LRESULT OnBandChange(WPARAM wParam, LPARAM lParam);

    // eq -> bands, bands -> eq
    void SetBandsFrom(const musikCore::EQSettings& settings);
    void BandsToEQSettings(musikCore::EQSettings* settings);

    void SendEQToPlayer();

protected:

    // status
    int m_BandState;
    int m_FullRange;
    int m_HalfRange;

    // controls
    CmusikEQSlider m_LeftBands[19];
    CmusikDynamicText m_Labels[19];
    CToolTipCtrl m_Tips;
    CFont m_Font;

    // macros
    DECLARE_DYNAMIC(CmusikEqualizerCtrl)
    DECLARE_MESSAGE_MAP()

};

///////////////////////////////////////////////////



