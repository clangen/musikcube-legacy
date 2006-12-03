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
#include "musikPlaylistView.h"
#include "MEMDC.H"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikPlaylistView, CWnd)

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikPlaylistView, CWnd)
    // mfc message maps
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_NCPAINT()
    ON_WM_ERASEBKGND()
    ON_WM_CONTEXTMENU()
    ON_WM_NCCALCSIZE()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

CmusikPlaylistView::CmusikPlaylistView(CFrameWnd* mainwnd, UINT dropid_l, UINT dropid_r, UINT dropid_sources, UINT dropid_selection_l, UINT dropid_selection_r, bool use_playlist_info, int type)
{
    if (type == PLAYLISTCTRL_NORMAL)
    {
        CmusikPlaylistDropTarget* pDropTarget =    new 
            CmusikPlaylistDropTarget(
                m_Playlist, 
                dropid_l, 
                dropid_r, 
                dropid_sources, 
                dropid_selection_l, 
                dropid_selection_r
       );

        m_Playlist = new CmusikPlaylistCtrl(mainwnd, pDropTarget, dropid_l, dropid_r);
    }
    else if (type == PLAYLISTCTRL_CDRIPPER)
    {
        m_Playlist = new CmusikPlaylistCDCtrl(mainwnd, dropid_l, dropid_r);
    }

    m_Parent        = mainwnd;
    m_PlaylistInfo    = NULL;
    m_UsePlaylistInfo = use_playlist_info;
    m_DrawLineAtBottom = true;
}

///////////////////////////////////////////////////

CmusikPlaylistView::~CmusikPlaylistView()
{
    DeinitPlaylistInfo();
    delete m_Playlist;
}

///////////////////////////////////////////////////

int CmusikPlaylistView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    // view
    ModifyStyleEx(WS_EX_STATICEDGE, NULL);
    ModifyStyle(WS_BORDER, 0);

    if (m_UsePlaylistInfo)
        InitPlaylistInfo();

    // child
    long dwStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_OWNERDATA | WS_VSCROLL | WS_HSCROLL;
    long dwStyleEx = LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP;

    m_Playlist->Create(dwStyle, CRect(0, 0, 0, 0), this, 123);
    m_Playlist->SetExtendedStyle(dwStyleEx);

    return 0;
}

///////////////////////////////////////////////////

void CmusikPlaylistView::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    if (m_UsePlaylistInfo)
    {
        if (!m_PlaylistInfo)
            InitPlaylistInfo();

        m_PlaylistInfo->MoveWindow(2, 1, cx - 4, 20);
        m_Playlist->MoveWindow(3, 24, cx - 6, cy - 27);

        return;
    }

    // variable not set, but playlist info
    // window still exists, destroy it..
    else if (m_PlaylistInfo)
        DeinitPlaylistInfo();

    m_Playlist->MoveWindow(3, 3, cx - 6, cy - 3);
}

///////////////////////////////////////////////////

void CmusikPlaylistView::OnNcPaint()
{
    // get window DC that is clipped to the non-client area
    CWindowDC dc(this);

    // rect of the CListCtrl
    CRect rcClient;
    GetClientRect(rcClient);
    ClientToScreen(rcClient);
    rcClient.OffsetRect(-rcClient.TopLeft());

    // rect of entire window
    CRect rcWindow;
    GetWindowRect(rcWindow);
    rcWindow.OffsetRect(-rcWindow.TopLeft());

    // memory device context to render to
    CMemDC pDC(&dc);

    // erase the NC background
    pDC.FillSolidRect(rcWindow, musikCube::g_Prefs->GetBtnFaceColor());

    // draw a simple border
    CRect rcBorder = rcWindow;
    CRect rcInfo;
    if (!m_UsePlaylistInfo)
    {
        rcBorder.left += 2;
        rcBorder.top += 2;
        rcBorder.right -= 2;
        rcBorder.bottom -= 4;
        pDC.Draw3dRect(rcBorder, musikCube::g_Prefs->GetBtnShadowColor(), musikCube::g_Prefs->GetBtnHilightColor());
    }
    else
    {
        rcBorder.left += 2;
        rcBorder.top += 23;
        rcBorder.right -= 2;
        rcBorder.bottom -= 4;
        pDC.Draw3dRect(rcBorder, musikCube::g_Prefs->GetBtnShadowColor(), musikCube::g_Prefs->GetBtnHilightColor());

        // don't draw over the playist info ctrl
        rcInfo = rcWindow;
        rcInfo.top = 1;
        rcInfo.left = 2;
        rcInfo.bottom = 21;
        rcInfo.right -= 2;
        dc.ExcludeClipRect(rcInfo);
    }

    // line at bottom.. this is for the main UI
    // its kludge :(
    if(m_DrawLineAtBottom)
    {
        CRect rcBottom;
        rcBottom.left = 0;
        rcBottom.right = rcClient.Width();
        rcBottom.top = rcClient.Height();
        rcBottom.bottom = rcClient.Height() + 2;
        pDC.Draw3dRect(rcBottom, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHILIGHT));
    }

    // client area is not our bussiness
    GetWindowRect(rcWindow);
    ScreenToClient(rcWindow);
    dc.IntersectClipRect(rcWindow);

    CRect rcDraw = rcBorder;
    rcDraw.left += 1;
    rcDraw.top += 1;
    rcDraw.right -= 1;
    rcDraw.bottom -= 1;
    dc.ExcludeClipRect(rcDraw);
}

///////////////////////////////////////////////////

BOOL CmusikPlaylistView::OnEraseBkgnd(CDC* pDC)
{
    return false;
}

///////////////////////////////////////////////////

void CmusikPlaylistView::InitPlaylistInfo() 
{
    if (!m_PlaylistInfo)
    {
        m_PlaylistInfo = new CmusikPlaylistInfoCtrl(GetCtrl());
        m_PlaylistInfo->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 123);
        m_Playlist->SetInfoCtrl(m_PlaylistInfo);
    }
}

///////////////////////////////////////////////////

void CmusikPlaylistView::DeinitPlaylistInfo()
{
    if (m_PlaylistInfo)
    {    
        delete m_PlaylistInfo;
        m_PlaylistInfo = NULL;
    }    
}

///////////////////////////////////////////////////

void CmusikPlaylistView::UpdatePlaylistInfo()
{
    if (m_PlaylistInfo)
    {
        m_PlaylistInfo->UpdateInfo();
    }

}

///////////////////////////////////////////////////

void CmusikPlaylistView::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
    CRect rcHeader, rcPlaylist;
    GetCtrl()->GetHeaderCtrl()->GetClientRect(rcHeader);
    GetCtrl()->GetClientRect(rcPlaylist);

    CPoint pos;
    GetCursorPos(&pos);
    ScreenToClient(&pos);

    if (m_UsePlaylistInfo)
    {
        rcHeader.OffsetRect(0, VIZ_HEIGHT + 2 + 4 );

        rcPlaylist.OffsetRect(0, VIZ_HEIGHT + 2 + 4);
        rcPlaylist.OffsetRect(0, rcHeader.Height());
    }

    if (rcHeader.PtInRect(pos))
        GetCtrl()->ShowHeaderMenu();
    else if (rcPlaylist.PtInRect(pos))
        GetCtrl()->ShowContextMenu();
}

///////////////////////////////////////////////////

void CmusikPlaylistView::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
    CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
    lpncsp->rgrc->bottom -= 2;
}

///////////////////////////////////////////////////


