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
//   CmusikTagDlg
//
// Filename(s): 
//
//   musikTagDlg.h, musikTagDlg.cpp
//
// Information:
//
//   The tag editing dialog 
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "Resource.h"
#include "musikGenres.h"
#include <musikCore.h>

///////////////////////////////////////////////////

class CmusikTagDlg : public CDialog
{
	
public:
	
    enum
    {
        MODE_PLAYLIST = 0,
        MODE_SONGINFO
    };

	// construct and destruct 
	CmusikTagDlg( CWnd* parent, musikCore::Playlist* playlist ); 
    CmusikTagDlg( CWnd* parent, musikCore::SongInfoArray* songinfo );

	virtual ~CmusikTagDlg();

	// helpers
	void UpdateSel( int index = 0 );
	void UpdatePlaylist( musikCore::Playlist* playlist ){ m_Playlist = playlist; }

	// misc
	enum { IDD = IDD_TAG_PROPERTIES };
	virtual void DoDataExchange(CDataExchange* pDX);  
	void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	// mfc messages
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedNext();
	afx_msg void OnBnClickedPrev();
	afx_msg void OnChangeField();
	afx_msg void OnChangeRating();
	afx_msg void OnBnClickedApply();

protected:

	// update info
	void UpdateDlg();
	void SaveCurr();
	void PromptSave();

	void OnEnter();
	void OnShiftEnter();

	void SelectFocused();

	bool m_Modified;
	bool m_Updating;

	// from parent
	CWnd* m_Parent;
	musikCore::Playlist* m_Playlist;
    musikCore::SongInfoArray* m_SongInfoArray;
	musikCore::SongInfo m_Song;

    int m_Mode;
	int m_SongIndex;

	// macros
	DECLARE_DYNAMIC(CmusikTagDlg)
	DECLARE_MESSAGE_MAP()

};

///////////////////////////////////////////////////
