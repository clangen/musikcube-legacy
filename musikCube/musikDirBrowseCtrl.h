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
// Version: alpha 6, March 28, 2004
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikCube.h"
#include "musikDockBar.h"
#include "musikTrackCtrl.h"
#include "musikListCtrl.h"

#include <musikCore.h>
#include <afxcmn.h>

///////////////////////////////////////////////////

class CmusikDirBrowseCtrl;

///////////////////////////////////////////////////

#ifndef baseCmusikDirBrowseBar
	#define baseCmusikDirBrowseBar CmusikDockBar
#endif

///////////////////////////////////////////////////

// CmusikDirBrowseBar

///////////////////////////////////////////////////

class CmusikDirBrowseBar : public baseCmusikDirBrowseBar
{
	friend class CmusikDirBrowseSets;

public:

	// construct / destruct
	CmusikDirBrowseBar();
	virtual ~CmusikDirBrowseBar();

	// virtual
	virtual void OnOptions();

	// gets
	CmusikDirBrowseCtrl* GetCtrl(){ return m_wndChild; }

protected:

	// children
	CmusikDirBrowseCtrl* m_wndChild;

	// child's font
	CFont m_Font;

	// macros
	DECLARE_DYNAMIC(CmusikDirBrowseBar)
	DECLARE_MESSAGE_MAP()

public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
};

///////////////////////////////////////////////////

// CmusikDirBrowseCtrl

///////////////////////////////////////////////////

class CmusikDirBrowseCtrl : public CTreeCtrl
{
	friend class CmusikDirBrowseSets;
	friend class CmusikDirBrowseBar;

public:

	// construct and destruct
	CmusikDirBrowseCtrl();
	virtual ~CmusikDirBrowseCtrl();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    void Refresh();

protected:

	// controls
    CFont m_Font;

    void LoadDirs( musikCore::String dir, HTREEITEM parent );

	// macros
	DECLARE_DYNAMIC(CmusikDirBrowseCtrl)
	DECLARE_MESSAGE_MAP()

};

///////////////////////////////////////////////////


