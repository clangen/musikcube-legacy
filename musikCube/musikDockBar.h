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

#ifndef C_MUSIK_DOCKBAR_H
#define C_MUSIK_DOCKBAR_H

///////////////////////////////////////////////////

#include "musikCube.h"

///////////////////////////////////////////////////

class CmusikDockbarBtn;
class CmusikDockContext;

///////////////////////////////////////////////////

#ifndef baseCmusikDockBar
    #define basemusikDockBar CSizingControlBar
#endif

///////////////////////////////////////////////////

#define HTOPTIONS (HTCLOSE + 56)

///////////////////////////////////////////////////

class CmusikDockBar : public basemusikDockBar
{
public:

    // construct / destruct
    CmusikDockBar(CMenu* menu = NULL, bool show_options_button = true );
    ~CmusikDockBar();

    // override default create method
    virtual BOOL Create(LPCTSTR lpszWindowName, CWnd* pParentWnd,
        UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP);

    // setting size
    void ForceDockedSize(const CSize& size, int orientation, bool layout = false);
    void ForceFloatSize(const CSize& size);
    void Layout(){ CalcDynamicLayout(NULL, LM_VERTDOCK | LM_HORZDOCK); }

    // show / hide gripper
    virtual BOOL HasGripper() const;
    void ShowGripper(bool show = true);

protected:

    virtual CWnd* GetMainWindow() = 0;

    // mfc message maps and overrides
    virtual void NcPaintGripper(CDC* pDC, CRect rcClient);
    virtual void NcCalcClient(LPRECT pRc, UINT nDockBarID);
    virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
    afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
    afx_msg void OnNcPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);

    void EnableDocking(DWORD dwDockStyle);

    // custom message maps
    afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);

    // virtual function to call when the 
    // option button is pressed
    virtual void OnOptions();

    // appearance
    int     m_cyGripper;
    bool    m_ShowGripper;
    CString m_sFontFace;
    bool    m_ShowButton;
    bool    m_ForceDrawBorder;

    // gripper icons
    CmusikDockbarBtn* m_biOptions;

    // musik stuff
    CMenu* m_Menu;
    
    // macros
    DECLARE_DYNAMIC(CmusikDockBar)
    DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

class CmusikDockContext : public CDockContext
{
public:
    // construction
    CmusikDockContext(CControlBar* pBar);

    // drag Operations
    virtual void StartDrag(CPoint pt);
};

///////////////////////////////////////////////////

class CmusikDockbarBtn 
{
public:
    CmusikDockbarBtn(char btn = 'r');
    ~CmusikDockbarBtn(){}

    void Move(CPoint ptTo) {ptOrg = ptTo; };
    CRect GetRect() { return CRect(ptOrg, CSize(12, 12)); };
    void Paint(CDC* pDC);

    BOOL    bPushed;
    BOOL    bRaised;

protected:

    CPoint  ptOrg;
    char    m_Btn;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
