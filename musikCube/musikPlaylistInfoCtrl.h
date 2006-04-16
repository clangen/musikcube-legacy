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
//   CmusikPlaylistInfoCtrl
//
// Filename(s): 
//
//   musikPlaylistInfoCtrl.h, musikPlaylistInfoCtrl.cpp
//
// Information:
//
//   Shows information about playlist, such as number of songs, size, etc.
//   ..and has a visualization in the background
// 
// Usage: 
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikCube.h"
#include <musikCore.h>

///////////////////////////////////////////////////

#define VIZ_WIDTH		128
#define VIZ_HEIGHT		19
#define MUSIK_VIZ_TIMER	WM_USER + 2

///////////////////////////////////////////////////

enum PLAYLIST_INFO_VIZ_STYLE
{
	PLAYLIST_INFO_VIZ_STYLE_NONE = 0,
	PLAYLIST_INFO_VIZ_STYLE_EQ,
};

///////////////////////////////////////////////////

class CmusikPlaylistInfoCtrl;

///////////////////////////////////////////////////

class CmusikPlaylistInfoWorker : public musikCore::Task
{
public:

	void run();
	CmusikPlaylistInfoCtrl* m_Parent;
};

///////////////////////////////////////////////////

class CmusikPlaylistInfoCtrl : public CWnd
{
	// friend
	friend class CmusikPlaylistInfoWorker;

public:

	// construct and destruct
	CmusikPlaylistInfoCtrl( CmusikPlaylistCtrl* parent );
	virtual ~CmusikPlaylistInfoCtrl();

	// called to update the info string
	void UpdateInfo();

protected:

	// pointer to the list
	CmusikPlaylistCtrl* m_ListCtrl;

	// draws the eq bars in the bg
	void DrawEQ( HDC hdc );

	HBITMAP m_hBGBitmap;
	COLORREF *m_pBGBitmapBits;

	// actual string to be printed
	musikCore::String	m_strInfo;

	// message maps
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		
	// default font
	CFont m_Font;

	// worker thread
	void UpdateStr( bool is_calc = false );
	CmusikPlaylistInfoWorker* m_InfoWorker;

	// macros
	DECLARE_DYNAMIC(CmusikPlaylistInfoCtrl)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////