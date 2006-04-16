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
//   Casey Langen   : Lead Developer, Project Manager. E-Mail: casey at musikcube dot com
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
//   CmusikSourcesCtrl, CmusikSourcesBar
//
// Filename(s): 
//
//   musikSourceCtrl.h, musikSourceCtrl.cpp
//
// Information:
//
//   The "Sources" panel. 
// 
// Usage: 
//
//   Void.
//
// Code adapted from:
//
//   CPropTree: http://www.codeproject.com/treectrl/proptree.asp
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikEditInPlace.h"
#include "musikDockBar.h"
#include "musikSourcesDropTarget.h"
#include "3rdparty/musikPropTree.h"
#include "musikRipDlg.h"

#include <musikCore.h>
#include <vector>

///////////////////////////////////////////////////

typedef std::vector<CmusikPropTreeItem*> CmusikSourcesItemPtrArray;

///////////////////////////////////////////////////

class CmusikSourcesCtrl;

///////////////////////////////////////////////////

#ifndef baseCmusikSourcesBar
	#define baseCmusikSourcesBar CmusikDockBar
#endif

///////////////////////////////////////////////////

#define IDC_SOURCES 1000

///////////////////////////////////////////////////

enum
{
	MUSIK_SOURCES_EDITINPLACE_RENAME = 493,
	MUSIK_SOURCES_EDITINPLACE_QUICKSEARCH,
	MUSIK_SOURCES_EDITINPLACE_NEWSTDPLAYLIST,
	MUSIK_SOURCES_EDITINPLACE_NEWDYNPLAYLIST
};

///////////////////////////////////////////////////

// CmusikSourcesBar

///////////////////////////////////////////////////

class CmusikSourcesBar : public baseCmusikSourcesBar
{
	friend class CmusikSourcesCtrl;

public:

	// construct / destruct
	CmusikSourcesBar( CFrameWnd* parent, UINT dropid, UINT pldropid_r, UINT sbdropid_r );
	virtual ~CmusikSourcesBar();

	// options
	virtual void OnOptions(){}
	void ShowMenu( bool force_show = false );

	// visible item types
	afx_msg void TogglePluginsVisible();
	afx_msg void ToggleStdPlaylistsVisible();
	afx_msg void ToggleDynPlaylistsVisible();
	afx_msg void ToggleDevicesVisible();

	// gets
	CmusikSourcesCtrl* GetCtrl(){ return m_wndChild; }

protected:

	// children

    CmusikRipDlg* m_RipDlg;
	CmusikSourcesCtrl* m_wndChild;
	CFont m_Font;
	CFrameWnd* m_Parent;
	musikCore::Library* m_LastLib;	// last library. we store this if
									// we click the collected library

	// message maps
    afx_msg LRESULT OnCloseRipDlg( WPARAM wParam, LPARAM lParam );
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnItemChanged( NMHDR* pNotifyStruct, LRESULT* plResult );
	afx_msg void OnSourcesRename();
	afx_msg void OnSourcesDelete();
	afx_msg void OnSourcesShuffleplaylist();
    afx_msg void OnSourcesCopyTo();
    afx_msg void OnQueryFreeDB();
	afx_msg void OnSourcesEditquery();
    afx_msg void OnSourcesRefreshcd();
	afx_msg void OnSourcesUnmount();
	afx_msg void OnCreateStockDyn();
	afx_msg void OnSourcesMoveup();
	afx_msg void OnSourcesMovedown();
	afx_msg void OnSourcesClear();
	afx_msg void OnSourcesMovefilesto();
	afx_msg void OnSourcesPluginexecute();
	afx_msg void OnSourcesPluginconfigure();
	afx_msg void OnSourcesPluginstop();
	afx_msg void OnSourcesPluginabout();
	afx_msg void OnSourcesShowallsongs();
	afx_msg void OnSourcesSynchronize();
	afx_msg void OnSourcesClearcollected();
	afx_msg void OnSourcesRipcd();

	// macros
	DECLARE_MESSAGE_MAP()

};

///////////////////////////////////////////////////

// CmusikSourcesCtrl

///////////////////////////////////////////////////

#define IDC_EDITINPLACE 1538

///////////////////////////////////////////////////

class CmusikSourcesCtrl : public CmusikPropTree
{
	friend class CmusikSourcesBar;
	friend class CmusikSourcesDropTarget;
    friend class CmusikCubeFrame;

public:

	// icons 
	static Gdiplus::Bitmap* m_pLibOff;
	static Gdiplus::Bitmap* m_pLibOn;
	static Gdiplus::Bitmap* m_pNPOff;
	static Gdiplus::Bitmap* m_pNPOn;
	static Gdiplus::Bitmap* m_pQSOff;
	static Gdiplus::Bitmap* m_pQSOn;
	static Gdiplus::Bitmap* m_pCDOff;
	static Gdiplus::Bitmap* m_pCDOn;
	static Gdiplus::Bitmap* m_pPLOff;
	static Gdiplus::Bitmap* m_pPLOn;
	static Gdiplus::Bitmap* m_pDPOff;
	static Gdiplus::Bitmap* m_pDPOn;
	static Gdiplus::Bitmap* m_pRDOff;
	static Gdiplus::Bitmap* m_pRDOn;
	static Gdiplus::Bitmap* m_pNSOff;
	static Gdiplus::Bitmap* m_pNSOn;
	static Gdiplus::Bitmap* m_pPlugOff;
	static Gdiplus::Bitmap* m_pPlugOn;
	static Gdiplus::Bitmap* m_pColOff;
	static Gdiplus::Bitmap* m_pColOn;
	static void LoadBitmaps();

	// construct / destruct
	CmusikSourcesCtrl( CFrameWnd* parent, UINT dropid, UINT pldropid_r, UINT sbdropid_r );
	virtual ~CmusikSourcesCtrl();

	void RefreshDevices();
	musikCore::String GetDeviceName( const musikCore::String& drive_path );
    static void GetDevices( musikCore::StringArray& target );

	// actions
	void KillFocus( bool redraw = true );
	void FocusLibrary( bool msg_parent = true );
	void FocusNowPlaying( bool msg_parent = true );
	void FocusItem( CmusikPropTreeItem* pItem, bool msg_parent = true );
	void FocusQuickSearch();
	void FocusNone();
	
	void ResetItems();

    void GetSelFilenames( CmusikPropTreeItem* pItem, musikCore::String& files );
    void GetSelFilenames( CmusikPropTreeItem* pItem, musikCore::StringArray& files );
	void RenameItem( CmusikPropTreeItem* pItem = NULL, int mode = MUSIK_SOURCES_EDITINPLACE_RENAME, CPoint loc = CPoint( -1, -1 ) );
	void DeleteSel();
	void QuickSearch();
	void Collected();
	void LoadColLib();

	// overrides
	void DoDrag( CmusikPropTreeItem* pItem );

protected:

	// various
	CFrameWnd* m_Parent;
	CString m_QuickStr;
	musikCore::PlaylistInfo* m_DragAndDropInfo;

	// drag and drop
	void OnDropFiles( HDROP hDropInfo, bool right_button = false );
	DROPEFFECT OnDragOver( CPoint point );
	void OnDragLeave();

   	// mfc message maps
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
	void OnTimer( UINT nIDEvent );

	// custom message maps
	afx_msg LRESULT OnEditCommit( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnEditCancel( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnEditChange( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnMouseLeave( WPARAM wParam, LPARAM lParam );

	// drop target for the playlist
	CmusikSourcesDropTarget* m_DropTarget;
	bool m_DropArrange;

	// create default headers
	CmusikPropTreeItem* m_LibRoot;
	CmusikPropTreeItem* m_Search;
	CmusikPropTreeItem* m_StdPlaylistRoot;
	CmusikPropTreeItem* m_DynPlaylistRoot;

	CmusikPropTreeItem* m_PluginsRoot;
	CmusikPropTreeItem* m_RightClick;
	CmusikPropTreeItem* m_Collected;
	//
	CmusikPropTreeItem* m_DevicesRoot;
	UINT m_DevicesTimer;
	musikCore::StringArray m_VisibleDrives;

	// loading and finding the items
	void LibLoad();
	void StdPlaylistsLoad();
	void DynPlaylistsLoad();
	void PluginsLoad();
	void MoveSelUp();
	void MoveSelDown();
	int GetStdPlaylistIdx( CmusikPropTreeItem* pItem );
	int GetDynPlaylistIdx( CmusikPropTreeItem* pItem );
    bool LoadLib( CmusikPropTreeItem* ptrItem );	// loads a removable device library

	// macros
	DECLARE_DYNAMIC(CmusikSourcesCtrl)
	DECLARE_MESSAGE_MAP()

private:

	// quick search
	void FinishQuickSearch();

    void DeleteLastPlaylist( int type );
    void EditInPlace( CmusikPropTreeItem* pItem, int edit_type, const CString text );
	int GetDriveCount();

	// mouse tracking?
	bool m_MouseTrack;

	bool DropAddToLibrary( musikCore::Library* pLibrary, musikCore::StringArray* pFiles );
	bool DropOnRemovableDevice( musikCore::StringArray* pFiles, CmusikPropTreeItem* pItem );
	bool DropCopyFilesToDevice( musikCore::StringArray* pFiles, CmusikPropTreeItem* pItem );
	bool DropAddNowPlaying( musikCore::StringArray* pFiles );
	bool DropAddToDevice( musikCore::StringArray* pFiles, CmusikPropTreeItem* pItem );

	musikCore::String SourcesFileOp( DWORD mode, const musikCore::StringArray& files, musikCore::String default_path = _T( "" ) );

	// Create... new playlist
	CmusikPropTreeItem* CreateNewStdPlaylist( int type, musikCore::StringArray* files = NULL, CmusikPropTreeItem* pEditInPlace = NULL );
	CmusikPropTreeItem* CreateNewDynPlaylist();

	// startup bool, if true on startup
	// the library will be selected by
	// default...
	bool m_Startup;
    int m_LastPlaylistID;

	// items that will show up in the list
	CmusikSourcesItemPtrArray m_Lib;
	CmusikSourcesItemPtrArray m_StdPlaylists;
	CmusikSourcesItemPtrArray m_DynPlaylists;
	CmusikSourcesItemPtrArray m_Devices;
	CmusikSourcesItemPtrArray m_Plugins;

	void RemoveStdPlaylist( CmusikPropTreeItem* pItem );
	void RemoveDynPlaylist( CmusikPropTreeItem* pItem );
};

///////////////////////////////////////////////////

