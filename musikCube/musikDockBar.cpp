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
#include "musikDockBar.h"
#include "MEMDC.H"
#include ".\musikdockbar.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikDockBar, basemusikDockBar);

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikDockBar, basemusikDockBar)
    ON_WM_NCLBUTTONUP()
    ON_WM_NCHITTEST()
    ON_WM_NCPAINT()
    ON_MESSAGE(WM_SETTEXT, OnSetText)
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_SIZE()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

CmusikDockBar::CmusikDockBar(CMenu* menu, bool show_options_button)
{
    m_ForceDrawBorder = false;
    m_Menu = menu;

    ShowGripper(true);
    m_cyGripper = 12;

    m_ShowButton = show_options_button;

    if (m_ShowButton)
        m_biOptions = new CmusikDockbarBtn('6');
    else
        m_biOptions = NULL;
}

///////////////////////////////////////////////////

CmusikDockBar::~CmusikDockBar()
{
    if (m_biOptions)
        delete m_biOptions;
}

///////////////////////////////////////////////////

void CmusikDockBar::ShowGripper(bool show)
{
    m_ShowGripper = show;
}

///////////////////////////////////////////////////

void CmusikDockBar::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
    if (nHitTest == HTCLOSE)
    {
        OnOptions();
        return;
    }

    basemusikDockBar::OnNcLButtonUp(nHitTest, point);
}

///////////////////////////////////////////////////

void CmusikDockBar::NcCalcClient(LPRECT pRc, UINT nDockBarID)
{
    // save the bar rect
    CRect rcBar(pRc); 

    // subtract edges
    basemusikDockBar::NcCalcClient(pRc, nDockBarID);

    CRect rc(pRc); 
    if (IsFloating() || (HasGripper() && m_ShowGripper))
    {
        // the client rect as calculated by 
        // the base class
        rc.DeflateRect(0, m_cyGripper, 0, 0);

        // set position for option button
        CPoint ptOptBtn = CPoint(rc.right - 13, rc.top - 14);

        if (m_ShowButton && m_biOptions)
            m_biOptions->Move(ptOptBtn - rcBar.TopLeft());
    }
    else
        rc.DeflateRect(0, -2, 0, 0);

    *pRc = rc;
}

///////////////////////////////////////////////////

void CmusikDockBar::OnNcPaint()
{
    CWindowDC dc(this);

    CRect rcClient, rcBar;
    GetClientRect(rcClient);
    ClientToScreen(rcClient);

    GetWindowRect(rcBar);
    rcClient.OffsetRect(-rcBar.TopLeft());
    rcBar.OffsetRect(-rcBar.TopLeft());

    CMemDC mdc(&dc);
    
    // erase the NC background
    CRect rcDraw = rcBar;
    rcDraw.InflateRect(1, 1, 1, 1);
    mdc.FillSolidRect(rcDraw, musikCube::g_Prefs->GetBtnFaceColor());
    NcPaintGripper(&mdc, rcClient);

    // draw border and gripper...
    if ((m_ShowGripper || m_ForceDrawBorder) && !IsFloating())
    {
        rcClient.InflateRect(1, 1, 1, 1);
        mdc.Draw3dRect(rcClient, musikCube::g_Prefs->GetBtnShadowColor(), musikCube::g_Prefs->GetBtnHilightColor());
        rcClient.DeflateRect(1, 1, 1, 1);
    }

    // client area is not our bussiness
    dc.IntersectClipRect(rcBar);
    dc.ExcludeClipRect(rcClient);
}

///////////////////////////////////////////////////

BOOL CmusikDockBar::Create(LPCTSTR lpszWindowName, CWnd* pParentWnd, UINT nID, DWORD dwStyle)
{
    // must have a parent
    ASSERT_VALID(pParentWnd);

    // cannot be both fixed and dynamic
    // (CBRS_SIZE_DYNAMIC is used for resizng when floating)
    ASSERT (!((dwStyle & CBRS_SIZE_FIXED) &&
              (dwStyle & CBRS_SIZE_DYNAMIC)));

    // save the control bar styles
    m_dwStyle = dwStyle & CBRS_ALL; 

    // register and create the window - skip CControlBar::Create()
    HBRUSH back = CreateSolidBrush((COLORREF)musikCube::g_Prefs->GetBtnFaceColor());
    CString wndclass = ::AfxRegisterWndClass(CS_DBLCLKS,
        ::LoadCursor(NULL, IDC_ARROW), 
        back, 0);

    // keep only the generic window styles, as 
    // well as WS_CLIPCHILDREN to prevent flashing
    dwStyle &= ~CBRS_ALL;
    dwStyle |= WS_CLIPCHILDREN;

    // create it
    if (!CWnd::Create(wndclass, lpszWindowName, dwStyle, CRect(0, 0, 0, 0), pParentWnd, nID))
        return FALSE;

    return TRUE;
}

///////////////////////////////////////////////////

void CmusikDockBar::NcPaintGripper(CDC* pDC, CRect rcClient)
{
    if (!IsFloating() && (!HasGripper() || !m_ShowGripper) )
        return;

    // compute the caption rectangle
    CRect rcGrip = rcClient;
    
    CRect rcBtn = rcClient;
    if (m_ShowButton && m_biOptions)
        rcBtn = m_biOptions->GetRect();

    // gripper at top

    if (IsFloating())
        rcGrip.left = 2;
    else
        rcGrip.left = 4;

    rcGrip.top = 4;
    rcGrip.bottom = 14;

    if (m_ShowButton)
      rcGrip.right = rcBtn.left - 2;

    rcGrip.InflateRect(0, 1);

    // draw the caption background
    COLORREF clrCptn = musikCube::g_Prefs->GetActiveCaptionColor();
    pDC->FillSolidRect(&rcGrip, clrCptn);

    // select a font for the caption text
    // (7.5 points at 96 ppi)
    CFont font;
    int ppi = pDC->GetDeviceCaps(LOGPIXELSX);
    int pointsize = MulDiv(75, 96, ppi);
    BOOL bFont = font.CreatePointFont(pointsize, m_sFontFace);

    // draw it
    if (bFont)
    {
        // get the text color
        COLORREF clrCptnText = musikCube::g_Prefs->GetActiveCaptionTextColor();

        int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
        COLORREF clrOldText = pDC->SetTextColor(clrCptnText);

        CFont* pOldFont = pDC->SelectObject(&font);
        CString sTitle;
        GetWindowText(sTitle);

        CPoint ptOrg = CPoint(rcGrip.left + 3, rcGrip.top);

        pDC->ExtTextOut(ptOrg.x, ptOrg.y,
            ETO_CLIPPED, rcGrip, sTitle, NULL);

        pDC->SelectObject(pOldFont);
        pDC->SetBkMode(nOldBkMode);
        pDC->SetTextColor(clrOldText);
    }

    // draw the buttons
    if (m_ShowButton && m_biOptions)
        m_biOptions->Paint(pDC);
}

///////////////////////////////////////////////////

UINT CmusikDockBar::OnNcHitTest(CPoint point)
{
    CRect rcBar;
    GetWindowRect(rcBar);

    // hit client area
    UINT nRet = basemusikDockBar::OnNcHitTest(point);
    if (nRet != HTCLIENT)
        return nRet;

    // hit the options button
    if (m_ShowButton && m_biOptions)
    {
        CRect rc = m_biOptions->GetRect();
        rc.OffsetRect(rcBar.TopLeft());
        if (rc.PtInRect(point))
            return HTCLOSE;
    }

    return HTCLIENT;
}

///////////////////////////////////////////////////

void CmusikDockBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
    UNUSED_ALWAYS(bDisableIfNoHndler);
    UNUSED_ALWAYS(pTarget);

    // was the options buton pressed?
    if (HasGripper() && m_ShowButton && m_biOptions)
    {  
        // what was hit?
        CPoint pt;
        ::GetCursorPos(&pt);
           BOOL bLButtonDown = (::GetKeyState(VK_LBUTTON) < 0);
        UINT nHit = OnNcHitTest(pt);

        BOOL bHideHit            = (nHit == HTCLOSE);
        BOOL bWasHidePushed        = m_biOptions->bPushed;
        m_biOptions->bPushed    = bHideHit && bLButtonDown;
        BOOL bWasHideRaised        = m_biOptions->bRaised;
        m_biOptions->bRaised    = bHideHit && !bLButtonDown;

        // needs painting
        BOOL bNeedsPaint = FALSE;
        bNeedsPaint |=    (m_biOptions->bPushed ^ bWasHidePushed) ||
                        (m_biOptions->bRaised ^ bWasHideRaised);

        SendMessage(WM_NCPAINT);
            return;
    }
}

///////////////////////////////////////////////////

LRESULT CmusikDockBar::OnSetText(WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = basemusikDockBar::OnSetText(wParam, lParam);

    SendMessage(WM_NCPAINT);

    return lResult;
}

///////////////////////////////////////////////////

CmusikDockbarBtn::CmusikDockbarBtn(char btn)
{
    bRaised = FALSE;
    bPushed = FALSE;
    m_Btn = btn;
}

///////////////////////////////////////////////////

void CmusikDockbarBtn::Paint(CDC* pDC)
{
    CRect rc = GetRect();

    // if pushed, redraw as raised
    if (bPushed)
        pDC->Draw3dRect(rc, musikCube::g_Prefs->GetBtnShadowColor(), musikCube::g_Prefs->GetBtnHilightColor());

    // if raised, redraw as pushed
    else if (bRaised)
            pDC->Draw3dRect(rc, musikCube::g_Prefs->GetBtnHilightColor(), musikCube::g_Prefs->GetBtnShadowColor());

    // set text color
    COLORREF clrOldTextColor = pDC->GetTextColor();
    pDC->SetTextColor(musikCube::g_Prefs->GetBtnTextColor());
    int nPrevBkMode = pDC->SetBkMode(TRANSPARENT);

    // set text font, 6 point at 96 ppi
    CFont font;
    int ppi = pDC->GetDeviceCaps(LOGPIXELSX);
    int pointsize = MulDiv(60, 96, ppi);
    font.CreatePointFont(pointsize, _T("Marlett"));
    CFont* oldfont = pDC->SelectObject(&font);

    // draw the text
    pDC->TextOut(ptOrg.x + 2, ptOrg.y + 3, CString(m_Btn));

    // select the old object
    pDC->SelectObject(oldfont);
    pDC->SetBkMode(nPrevBkMode);
    pDC->SetTextColor(clrOldTextColor);

}

///////////////////////////////////////////////////

BOOL CmusikDockBar::HasGripper() const
{
    #if defined(_SCB_MINIFRAME_CAPTION) || !defined(_SCB_REPLACE_MINIFRAME)
        // if the miniframe has a caption, 
        // don't display the gripper
        if (IsFloating())
            return FALSE;
    #endif

    return TRUE;
}

///////////////////////////////////////////////////

#if defined(_SCB_REPLACE_MINIFRAME) && !defined(_SCB_MINIFRAME_CAPTION)
void CmusikDockBar::EnableDocking(DWORD dwDockStyle)
{
    // must be CBRS_ALIGN_XXX or CBRS_FLOAT_MULTI only
    // cannot have the CBRS_FLOAT_MULTI style
    // the bar must have CBRS_SIZE_DYNAMIC style
    ASSERT((dwDockStyle & ~(CBRS_ALIGN_ANY|CBRS_FLOAT_MULTI)) == 0);
    ASSERT((dwDockStyle & CBRS_FLOAT_MULTI) == 0);
    ASSERT((m_dwStyle & CBRS_SIZE_DYNAMIC) != 0);

    m_dwDockStyle = dwDockStyle;
    if (m_pDockContext == NULL)
        m_pDockContext = new CmusikDockContext(this);

    // permanently wire the bar's owner to its current parent
    if (m_hWndOwner == NULL)
        m_hWndOwner = ::GetParent(m_hWnd);
}
#endif

///////////////////////////////////////////////////

BOOL CmusikDockBar::OnEraseBkgnd(CDC* pDC)
{
    return false;
}

///////////////////////////////////////////////////

void CmusikDockBar::OnOptions()
{
    cout << _T("Options button pressed\n");
}

///////////////////////////////////////////////////

void CmusikDockBar::ForceDockedSize(const CSize& size, int dockstate, bool layout)
{
    if (dockstate == LM_HORZDOCK)
        m_szHorz = size;
    if (dockstate == LM_VERTDOCK)
        m_szVert = size;

    if (layout)
        Layout();
}

///////////////////////////////////////////////////

void CmusikDockBar::ForceFloatSize(const CSize& size)
{
    m_szFloat = size;
}

///////////////////////////////////////////////////

CmusikDockContext::CmusikDockContext(CControlBar* pBar)
    : CDockContext(pBar)
{
}

///////////////////////////////////////////////////

static void AdjustRectangle(CRect& rect, CPoint pt)
{
    int nXOffset = (pt.x < rect.left) ? (pt.x - rect.left) :
                    (pt.x > rect.right) ? (pt.x - rect.right) : 0;
    int nYOffset = (pt.y < rect.top) ? (pt.y - rect.top) :
                    (pt.y > rect.bottom) ? (pt.y - rect.bottom) : 0;
    rect.OffsetRect(nXOffset, nYOffset);
}

///////////////////////////////////////////////////

void CmusikDockContext::StartDrag(CPoint pt)
{
    int WM_INITTRANS    = RegisterWindowMessage(_T("INITTRANS"));
    int WM_DEINITTRANS    = RegisterWindowMessage(_T("DEINITTRANS"));

    bool trans_enb = false;
    if (musikCube::g_Prefs->IsTransEnabled())
        trans_enb = true;

    if (trans_enb)
        AfxGetApp()->m_pMainWnd ->SendMessage(WM_DEINITTRANS);

    ASSERT_VALID(m_pBar);
    m_bDragging = TRUE;

    InitLoop();

    ASSERT((m_pBar->m_dwStyle & CBRS_SIZE_DYNAMIC) != 0);

    // get true bar size (including borders)
    CRect rect;
    m_pBar->GetWindowRect(rect);
    m_ptLast = pt;
    CSize sizeHorz = m_pBar->CalcDynamicLayout(0, LM_HORZ | LM_HORZDOCK);
    CSize sizeVert = m_pBar->CalcDynamicLayout(0, LM_VERTDOCK);
    CSize sizeFloat = m_pBar->CalcDynamicLayout(0, LM_HORZ | LM_MRUWIDTH);

    m_rectDragHorz = CRect(rect.TopLeft(), sizeHorz);
    m_rectDragVert = CRect(rect.TopLeft(), sizeVert);

    // calculate frame dragging rectangle
    m_rectFrameDragHorz = CRect(rect.TopLeft(), sizeFloat);

#ifdef _MAC
    CMiniFrameWnd::CalcBorders(&m_rectFrameDragHorz,
        WS_THICKFRAME, WS_EX_FORCESIZEBOX);
#else
    CMiniFrameWnd::CalcBorders(&m_rectFrameDragHorz, WS_THICKFRAME);
#endif
    m_rectFrameDragHorz.DeflateRect(2, 2);
    m_rectFrameDragVert = m_rectFrameDragHorz;
    
    // adjust rectangles so that point is inside
    AdjustRectangle(m_rectDragHorz, pt);
    AdjustRectangle(m_rectDragVert, pt);
    AdjustRectangle(m_rectFrameDragHorz, pt);
    AdjustRectangle(m_rectFrameDragVert, pt);

    // initialize tracking state and enter tracking loop
    m_dwOverDockStyle = CanDock();
    Move(pt);   // call it here to handle special keys
    Track();

    if (trans_enb)
        AfxGetApp()->m_pMainWnd->PostMessage(WM_INITTRANS);
}

///////////////////////////////////////////////////
void CmusikDockBar::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    //dc.FillSolidRect(rc2, RGB(0, 0, 0));

    CWnd* parent = GetParent();
    parent->ModifyStyle(WS_BORDER | WS_CAPTION | WS_DLGFRAME | WS_POPUP | WS_POPUPWINDOW | WS_SIZEBOX | WS_THICKFRAME, NULL, NULL);
    parent->ModifyStyleEx(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE, NULL, NULL);
}

void CmusikDockBar::OnSize(UINT nType, int cx, int cy)
{
    CSizingControlBar::OnSize(nType, cx, cy);

    CRect clientRect;
    GetClientRect(&clientRect);

    this->GetMainWindow()->MoveWindow(0, 0, clientRect.Width(), clientRect.Height());
}
