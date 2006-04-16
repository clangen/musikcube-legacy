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
//
//   CmusikPlaylistView
//
// Filename(s): 
//
//   musikPlaylistView.h, musikPlaylistView.cpp
//
// Information:
//
//   A CWnd derived object that maintains a CmusikPlaylistCtrl
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikCube.h"
#include "musikPlaylistCtrl.h"
#include "musikPlaylistInfoCtrl.h"
#include "musikPlaylistDropTarget.h"

#include <musikCore.h>

///////////////////////////////////////////////////

class CmusikPlaylistView : public CWnd
{
public:

	// construct and destruct
	CmusikPlaylistView( CFrameWnd* mainwnd, UINT dropid_l, UINT dropid_r, UINT dropid_sources, UINT dropid_selection_l, UINT dropid_selection_r, bool use_playlist_info = true, int type = PLAYLISTCTRL_NORMAL );
	virtual ~CmusikPlaylistView();

	// get the playlist control
	CmusikPlaylistCtrl* GetCtrl(){ return m_Playlist; }

	// playlist info
	void InitPlaylistInfo();
	void DeinitPlaylistInfo();
	void UpdatePlaylistInfo();
    void SetUsePlaylistInfo( bool use ){ m_UsePlaylistInfo = use; }
    void SetDrawLineAtBottom( bool draw ){ m_DrawLineAtBottom = draw; }

	// mfc message maps
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
    afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);

protected:

	// playlist GUI object
	CmusikPlaylistCtrl* m_Playlist;
	CmusikPlaylistInfoCtrl* m_PlaylistInfo;

	// pointer to main window so we can
	// messages to it, such as updating the UI
	// after files have been dropped
	CWnd* m_Parent;
    bool m_UsePlaylistInfo;
    bool m_DrawLineAtBottom;
	
	// macros
	DECLARE_DYNAMIC(CmusikPlaylistView)
	DECLARE_MESSAGE_MAP()

};

///////////////////////////////////////////////////
