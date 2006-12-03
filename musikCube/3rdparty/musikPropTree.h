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

#include "musikPropTreeItem.h"
#include "../musikEditInPlace.h"
#include "../musikPrefs.h"

#include <musikCore.h>

///////////////////////////////////////////////////

class CmusikPropTree;
class CmusikPropTreeItem;

///////////////////////////////////////////////////

#define PROPTREEITEM_SPACE              5           
#define PROPTREEITEM_DEFHEIGHT          22          // default heigt of an item

///////////////////////////////////////////////////

enum 
{
    MUSIK_SOURCES_TYPE_QUICKSEARCH = musikCore::MUSIK_PLAYLIST_TYPE_COUNT,
    MUSIK_SOURCES_TYPE_NEWSTDPLAYLIST,
    MUSIK_SOURCES_TYPE_NEWDYNPLAYLIST,
    MUSIK_SOURCES_TYPE_NETSTREAMS,
    MUSIK_SOURCES_TYPE_PLUGIN,
    MUSIK_SOURCES_TYPE_COLLECTED
};

///////////////////////////////////////////////////

typedef BOOL (CALLBACK* ENUMPROPITEMPROC)(CmusikPropTree*, CmusikPropTreeItem*, LPARAM);

///////////////////////////////////////////////////

// CmusikPropTree window styles

#define PTS_NOTIFY                  0x00000001

///////////////////////////////////////////////////

// CmusikPropTree HitTest return codes

#define HTPROPFIRST                 50
#define HTLABEL                     (HTPROPFIRST + 0)
#define HTEXPAND                    (HTPROPFIRST + 2)
#define HTATTRIBUTE                 (HTPROPFIRST + 3)
#define RTCLKITEM                   778997
#define LFCLKITEM                   778998

///////////////////////////////////////////////////

// CmusikPropTree WM_NOTIFY notification structure

typedef struct _NMPROPTREE
{
    NMHDR           hdr;
    CmusikPropTreeItem* pItem;
} NMPROPTREE, *PNMPROPTREE, FAR *LPNMPROPTREE;

///////////////////////////////////////////////////

// CmusikPropTree specific Notification Codes

#define PTN_FIRST                   (0U-1100U)
#define PTN_INSERTITEM              (PTN_FIRST-1)
#define PTN_DELETEITEM              (PTN_FIRST-2)
#define PTN_DELETEALLITEMS          (PTN_FIRST-3)
#define PTN_ITEMCHANGED             (PTN_FIRST-4)
#define PTN_ITEMBUTTONCLICK         (PTN_FIRST-5)
#define PTN_SELCHANGE               (PTN_FIRST-6)
#define PTN_ITEMEXPANDING           (PTN_FIRST-7)
#define PTN_PROPCLICK               (PTN_FIRST-8)
#define PTN_MOUSEOVERNEW            (PTN_FIRST-9)
#define PTN_ITEMDRAG                (PTN_FIRST-10)

///////////////////////////////////////////////////

class CmusikPropTree : public CWnd
{
    // so they can access the musik stuff
    friend class CmusikPropTreeItem;

public:

    // item counter...
    int m_Count;

    // construct / destruct
    CmusikPropTree( UINT dropid );
    virtual ~CmusikPropTree();

    // overrides
    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

    // fonts
    static CFont* GetNormalFont();
    static CFont* GetBoldFont();

    // item gets
    CmusikPropTreeItem* GetRootItem();
    CmusikPropTreeItem* GetFocusedItem();
    static CmusikPropTreeItem* m_Hovered;

    // item sets
    void SetFocusedItem(CmusikPropTreeItem* pItem);

    // items actions
    CmusikPropTreeItem* InsertItem(CmusikPropTreeItem* pItem, CmusikPropTreeItem* pParent = NULL);
    void DeleteItem(CmusikPropTreeItem* pItem, bool notify = true);
    void DeleteAllItems( bool notify = true );
    void EnsureVisible(CmusikPropTreeItem* pItem);
    virtual CmusikPropTreeItem* FindItem(const POINT& pt);
    CmusikPropTreeItem* FindItem(UINT nCtrlID);
    void DetachAllItems();

    // item checks
    BOOL IsItemVisible(CmusikPropTreeItem* pItem);

    // functions used by CmusikPropTreeItem...
    // should not call these directly
    void AddToVisibleList(CmusikPropTreeItem* pItem);
    void ClearVisibleList();
    void SetOriginOffset(LONG nOffset);
    void UpdatedItems();
    void UpdateMoveAllItems();
    void RefreshItems(CmusikPropTreeItem* pItem = NULL);
    void SelectItems(CmusikPropTreeItem* pItem, BOOL bSelect = TRUE);

    // misc
    const int& GetOrigin();
    void UpdateScrollbar();

    // item focus controls
    CmusikPropTreeItem *FocusPrev();
    CmusikPropTreeItem *FocusNext();
    vector<CmusikPropTreeItem*> m_pVisibleList;
    void CheckScrollVisible();
    bool IsScrollVisible(){ return m_ScrollVisible; }

    // Enumerates an item and all its child items
    BOOL EnumItems(CmusikPropTreeItem* pItem, ENUMPROPITEMPROC proc, LPARAM lParam = 0L);

    // do a hit test on the control 
    // HitTest returns a HTxxxx code
    // HitTextEx returns the item
    LONG HitTest(const POINT& pt);
    CmusikPropTreeItem* HitTestEx(const POINT& pt);

    // non-mfc or custom messages
    LRESULT SendNotify(UINT nNotifyCode, CmusikPropTreeItem* pItem = NULL);

    // mfc message maps
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

    // F2...
    CmusikEditInPlace m_EditInPlace;
    int m_LastBtn;

    int GetScrollPosition() { return this->m_ScrollPos; }

protected:

    // drag and drop related stuff
    UINT m_DropID;
    bool m_IsWinNT;
    bool m_ScrollVisible;

    // bitmap back buffer for flicker free drawing
    CBitmap m_BackBuffer;
    CSize m_BackBufferSize;

    // Initialize global resources, brushes, 
    // fonts, etc.
    void InitGlobalResources();

    // Free global resources, brushes, fonts, etc.
    void FreeGlobalResources();

    // Recursive version of DeleteItem
    void Delete(CmusikPropTreeItem* pItem);

    // Root level tree item
    CmusikPropTreeItem m_Root;

    // Pointer to the focused item (selected)
    CmusikPropTreeItem* m_pFocus;

    // PropTree scroll position
    int m_ScrollPos;

    // auto generated last created ID
    UINT m_nLastUID;

    // Number of CmusikPropTree controls in the current application
    static UINT s_nInstanceCount;

    // fonts
    static CFont* s_pNormalFont;
    static CFont* s_pBoldFont;

    // Used for enumeration
    static CmusikPropTreeItem*  s_pFound;

    // misc internal functions
    void RecreateBackBuffer(int cx, int cy);
    void CheckVisibleFocus();

    // protected mfc message maps
    afx_msg void OnEnable(BOOL bEnable);
    afx_msg void OnSysColorChange();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnPaint();
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg UINT OnGetDlgCode();
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);

    // macros
    DECLARE_MESSAGE_MAP()
    DECLARE_DYNAMIC(CmusikPropTree)

    // mouse
    // (events)
private:    afx_msg void                OnMouseMove(UINT nFlags, CPoint point);
private:    afx_msg LRESULT             OnMouseLeave(WPARAM wParam, LPARAM lParam);
    // (variables)
private:    bool                        m_MouseTrack;

private:

    // callbacks
    static BOOL CALLBACK EnumFindItem(CmusikPropTree* pProp, CmusikPropTreeItem* pItem, LPARAM lParam);
    static BOOL CALLBACK EnumSelectAll(CmusikPropTree*, CmusikPropTreeItem* pItem, LPARAM lParam);
    static BOOL CALLBACK EnumMoveAll(CmusikPropTree*, CmusikPropTreeItem* pItem, LPARAM);
    static BOOL CALLBACK EnumRefreshAll(CmusikPropTree*, CmusikPropTreeItem* pItem, LPARAM);

};

///////////////////////////////////////////////////
