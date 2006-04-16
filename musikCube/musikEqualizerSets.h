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
//   CmusikEqualizerSets
//
// Filename(s): 
//
//   musikEqualizerSets.h, musikEqualizerSets.cpp
//
// Information:
//
//   Dialog used for adding / removing / managing
//   equalizer presets. It front ends the musikCore::Playlist.
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikCube.h"
#include <musikCore.h>

///////////////////////////////////////////////////

class CmusikEqualizerSets : public CDialog
{
public:

	// construct and destruct
	CmusikEqualizerSets( CWnd* pParent = NULL );
	virtual ~CmusikEqualizerSets();

	// mfc message maps / overrides
	afx_msg void OnBnClickedCloseDlg();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedRenameSel();
	afx_msg void OnLbnSelchangePresetBox();
	afx_msg void OnBnClickedDeleteSel();

	// misc
	enum { IDD = IDD_EQUALIZER_SETS };

protected:

	// misc
	virtual void DoDataExchange(CDataExchange* pDX); 

	int GetIndex();

	// internal list of equalizer ids
	// obtained when when dialog is first
	// intialized, and maintained while the
	// dialog is open
	musikCore::IntArray m_IDs;
	void ReloadEqualizers();

	// getting active
	void GetActiveEqualizer( musikCore::EQSettings* settings );

    // parent window to post events to
	CWnd* m_Parent;

	// controls
	CListBox m_PresetBox;

	// macros
	DECLARE_DYNAMIC(CmusikEqualizerSets)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////


