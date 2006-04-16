///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
//   multimedia library. More information at:
//
//     http://musik.berlios.de
//     http://musik.sourceforge.net
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2002-2004
//   Casey Langen   : Lead Developer, Project Manager. E-Mail: casey@bak.rr.com
//   Dustin Carter  : Developer
//   Simon Windmill : Developer
//
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////
//
// Class(es): 
//
//   CmusikSelectionCtrl, CmusikSelectionBar
//
// Filename(s): 
//
//   musikSelectionCtrl.h, musikSelectionCtrl.cpp
//
// Information:
//
//   musikCube's artist / album / genre / etc selection control
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikCube.h"
#include "musikListCtrl.h"
#include "musikEditInPlace.h"
#include "musikDockBar.h"

#include <musikCore.h>
#include <OpenThreads/Mutex>

using namespace OpenThreads;

///////////////////////////////////////////////////

class CmusikSelectionCtrl;

///////////////////////////////////////////////////

// CmusikSelectionBar

///////////////////////////////////////////////////

#ifndef baseCmusikSelectionBar
	#define baseCmusikSelectionBar CmusikDockBar
#endif

///////////////////////////////////////////////////

class CmusikSelectionBar : public baseCmusikSelectionBar
{
	friend class CmusikSelectionCtrl;

public:

	// construct / destruct
	CmusikSelectionBar( CFrameWnd* parent, int type, int ctrlid, UINT dropid_l, UINT dropid_r );
	virtual ~CmusikSelectionBar();

	// gets
	CmusikSelectionCtrl* GetCtrl(){ return m_wndChild; }

	// mfc message maps
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelectionboxRename();
	afx_msg void OnSelectionboxRemove();
	afx_msg void OnSelectionboxAddnew();
	afx_msg void OnChangetypeArtist();
	afx_msg void OnChangetypeAlbum();
	afx_msg void OnChangetypeYear();
	afx_msg void OnChangetypeGenre();
	afx_msg void OnChangetypeTracknumber();
	afx_msg void OnChangetypeTimeadded();
	afx_msg void OnChangetypeLastplayed();
	afx_msg void OnChangetypeFormat();
	afx_msg void OnChangetypeRating();
	afx_msg void OnChangetypeTimesplayed();
	afx_msg void OnChangetypeBitrate();
	afx_msg void OnDeleteFromlibrary();
	afx_msg void OnDeleteFromcomputer();
	afx_msg void OnSelectionboxcontextPlayselected();
	afx_msg void OnSelectionboxcontextQueueselected();
	afx_msg void OnSelectionboxcontextPlaynext();
	afx_msg void OnSelectionboxcontextReplacenowplaying();
	afx_msg void OnSelectionboxcontextReplacenowplayingandplay();

	// misc
	void ShowOptionsMenu();
	void ShowContextMenu();
	void ReqSelBoxUpdate();

	// showing options menu
	virtual void OnOptions();

protected:

	// child
	CmusikSelectionCtrl* m_wndChild;
	CFont m_Font;

	// macros
	DECLARE_DYNAMIC(CmusikSelectionBar)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

// CmusikSelectionCtrl

///////////////////////////////////////////////////

struct CmusikSelectionCache
{
	musikCore::StringArray m_SelItems;
	musikCore::StringArray m_Items;
	int m_ChildOrder;
	int m_ScrollPos;
	bool m_Parent;

	void Reset()
	{
		m_ChildOrder = -1;
		m_ScrollPos = 0;
		m_Parent = false;
	}
};

///////////////////////////////////////////////////


class CmusikSelectionCtrl : public CmusikListCtrl
{
	friend class CmusikSelectionBar;
	static bool m_Updating;
	static int m_Count;

public:

	// construct / destruct
	CmusikSelectionCtrl( CFrameWnd* parent, int type, int ctrlid, UINT dropid_l, UINT dropid_r );
	virtual ~CmusikSelectionCtrl();

	// gets
	int GetCtrlID (){ return m_ID; }
	CString GetTypeDB();
	CString GetTypeStr();
	int	GetType(){ return m_Type; }
	void GetSelItems( musikCore::StringArray& items, bool format_query = true );
	musikCore::String GetSelQuery();
	musikCore::String GetEditCommitStr();

	// sets
	void SetParent( bool parent = true ){ m_ParentBox = parent; m_ChildOrder = -1; }
	static void SetUpdating( bool updating = true ){ m_Updating = updating; }
	void SetType( int type, bool update = true );

	// order
	int GetChildOrder() { return m_ChildOrder; }
	void ResetOrder() { m_ChildOrder = -1; }

	static void IncChildOrder() { m_Count++; }
	static void ResetChildOrder() { m_Count = -1; }

	// misc
	void RescaleColumn();
	void UpdateV( bool requery = true );
	void UpdateV( musikCore::String query );
	void UpdateV( musikCore::String& query, const musikCore::Playlist& source );
	void UpdateV( const musikCore::Playlist& source );
	void RenameSel();
	void DelSel( bool from_file = false );
	void OnNewSong();

	// queries
	bool IsItemSelected( int item );
	bool IsParent(){ return m_ParentBox; }

	// mfc message maps
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLvnMarqueeBegin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnBeginrdrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeydown(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnNMKillfocus(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult);

	// custom message maps
	LRESULT OnEditCommit( WPARAM wParam, LPARAM lParam );
	LRESULT OnEditCancel( WPARAM wParam, LPARAM lParam );

	// caching
	void StoreCache();
	void RestoreCache();

	// custom message maps
	afx_msg LRESULT OnMouseLeave( WPARAM wParam, LPARAM lParam );

protected:

	int m_ChildOrder;
	bool m_SelTrigger;

	// misc
	void InitFonts();
	void UpdateWindowCap( bool resize_col = true );

	// cache to hold previous library selection
	CmusikSelectionCache m_Cache;
    musikCore::String m_SearchBuf;
		
	// colors
	musikCore::String m_CurrPlayingStr;

	// drag / drop id
	UINT m_DropID_L;
	UINT m_DropID_R;
	void BeginDrag( bool right_button );
    void InsertFirstItem();

	bool m_IsFocused;
	bool m_Updated;
    bool m_IsFiltering;

	// F2...
	CmusikEditInPlace m_EditInPlace;
	musikCore::String m_CommitStr;

	// core
	CFrameWnd* m_Parent;
	musikCore::StringArray m_Items;
	int m_Type;
	int m_ID;
	bool m_ParentBox;

	// fonts
	CFont m_Regular;
	CFont m_Bold;
	CFont m_StarFont;

	Mutex m_ProtectingUpdate;

	// macros
	DECLARE_DYNAMIC(CmusikSelectionCtrl)
	DECLARE_MESSAGE_MAP()

};

///////////////////////////////////////////////////

