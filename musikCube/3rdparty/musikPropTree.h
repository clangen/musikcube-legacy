///////////////////////////////////////////////////
//
// PropTree.h
//
//  Copyright (C) 1998-2001 Scott Ramsay
//	sramsay@gonavi.com
//	http://www.gonavi.com
//
//  This material is provided "as is", with absolutely no warranty expressed
//  or implied. Any use is at your own risk.
// 
//  Permission to use or copy this software for any purpose is hereby granted 
//  without fee, provided the above notices are retained on all copies.
//  Permission to modify the code and to distribute modified code is granted,
//  provided the above notices are retained, and a notice that the code was
//  modified is included with the above copyright notice.
// 
//	If you use this code, drop me an email.  I'd like to know if you find the code
//	useful.
//
//  Modified heavily by Casey Langen for musik MFC port...
//    - Rather than linking to a dll or external lib, it compiles in.
//    - PropTree and PropTreeList combined into one file
//    - Drawing enhancements
//    - Various musik specific functions
//
//  The musik team thanks Scott Ramsay for creating this great
//  class and making it open source.
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

#define PROPTREEITEM_SPACE				5			
#define PROPTREEITEM_DEFHEIGHT			22			// default heigt of an item

///////////////////////////////////////////////////

enum 
{
	MUSIK_SOURCES_TYPE_QUICKSEARCH = musikCore::MUSIK_PLAYLIST_TYPE_COUNT,
	MUSIK_SOURCES_TYPE_NEWSTDPLAYLIST,
	MUSIK_SOURCES_TYPE_NEWDYNPLAYLIST,
    MUSIK_SOURCES_TYPE_RIPCD,
	MUSIK_SOURCES_TYPE_NETSTREAMS,
	MUSIK_SOURCES_TYPE_PLUGIN,
	MUSIK_SOURCES_TYPE_COLLECTED
};

///////////////////////////////////////////////////

typedef BOOL (CALLBACK* ENUMPROPITEMPROC)(CmusikPropTree*, CmusikPropTreeItem*, LPARAM);

///////////////////////////////////////////////////

// CmusikPropTree window styles

#define PTS_NOTIFY					0x00000001

///////////////////////////////////////////////////

// CmusikPropTree HitTest return codes

#define HTPROPFIRST					50
#define HTLABEL						(HTPROPFIRST + 0)
#define HTEXPAND					(HTPROPFIRST + 2)
#define HTATTRIBUTE					(HTPROPFIRST + 3)
#define RTCLKITEM					778997
#define LFCLKITEM					778998

///////////////////////////////////////////////////

// CmusikPropTree WM_NOTIFY notification structure

typedef struct _NMPROPTREE
{
    NMHDR			hdr;
	CmusikPropTreeItem*	pItem;
} NMPROPTREE, *PNMPROPTREE, FAR *LPNMPROPTREE;

///////////////////////////////////////////////////

// CmusikPropTree specific Notification Codes

#define PTN_FIRST					(0U-1100U)
#define PTN_INSERTITEM				(PTN_FIRST-1)
#define PTN_DELETEITEM				(PTN_FIRST-2)
#define PTN_DELETEALLITEMS			(PTN_FIRST-3)
#define PTN_ITEMCHANGED				(PTN_FIRST-4)
#define PTN_ITEMBUTTONCLICK			(PTN_FIRST-5)
#define PTN_SELCHANGE				(PTN_FIRST-6)
#define PTN_ITEMEXPANDING			(PTN_FIRST-7)
#define PTN_PROPCLICK				(PTN_FIRST-8)
#define PTN_MOUSEOVERNEW			(PTN_FIRST-9)

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
	void UpdateResize();

	// item focus controls
	CmusikPropTreeItem *FocusPrev();
	CmusikPropTreeItem *FocusNext();
    vector<CmusikPropTreeItem*> m_pVisibleList;
	void CheckScrollVisible();
	bool IsScrollVisible(){ return m_ScrollVisible;	}

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

protected:

	// drag and drop related stuff
	UINT m_DropID;
	bool m_IsWinNT;
	bool m_ScrollVisible;

	// bitmap back buffer for flicker free drawing
	CBitmap	m_BackBuffer;
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
	static CmusikPropTreeItem*	s_pFound;

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

	void DetachAllItems();

	// macros
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CmusikPropTree)

private:

	// callbacks
	static BOOL CALLBACK EnumFindItem(CmusikPropTree* pProp, CmusikPropTreeItem* pItem, LPARAM lParam);
	static BOOL CALLBACK EnumSelectAll(CmusikPropTree*, CmusikPropTreeItem* pItem, LPARAM lParam);
	static BOOL CALLBACK EnumMoveAll(CmusikPropTree*, CmusikPropTreeItem* pItem, LPARAM);
	static BOOL CALLBACK EnumRefreshAll(CmusikPropTree*, CmusikPropTreeItem* pItem, LPARAM);

};

///////////////////////////////////////////////////
