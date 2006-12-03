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
#include <musikCore.h>

///////////////////////////////////////////////////

class CmusikTrackCtrl : public CSliderCtrl
{
public:

    // construct / destruct
    CmusikTrackCtrl();
    virtual ~CmusikTrackCtrl();

    // misc
    void SetLockIfNotPlaying(bool lock = true){ m_LockIfNotPlaying = lock; }
    bool IsLockSet(){ return m_LockIfNotPlaying; }
    bool IsDragging(){ return m_IsCapturing; }

    void SetPos(int nPos);

    // public mfc message maps
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnPaint();

protected:    

    // mouse tracking
    CPoint m_Cursor;
    int m_LastPos;
    bool m_LeftDown;
    bool m_IsCapturing;
    bool m_DrawThumb;
    void SetPosFromMouse(bool call_changed = true);
    virtual void OnPosChanged();
    virtual void OnBeginDrag();
    virtual void OnFinishDrag();

    // lock movement if no playback
    bool m_LockIfNotPlaying;
    bool m_RealtimeUpdate;

    // drawing...
    void DrawChannel(CDC* pDC, const CRect& rect, BOOL bHoriz);
    CRect m_ChannelRect;

    // message maps
    DECLARE_DYNAMIC(CmusikTrackCtrl)
    DECLARE_MESSAGE_MAP()


};

///////////////////////////////////////////////////

