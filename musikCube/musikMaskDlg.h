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
//   CmusikMaskDlg
//
// Filename(s): 
//
//   musikMaskDlg.h, musikMaskDlg.cpp
//
// Information:
//
//   musikCube's filename mask dialog
// 
// Usage: 
//
//   set masks
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "afxwin.h"
#include "musikCube.h"
#include <musikCore.h>

///////////////////////////////////////////////////

enum
{
	MUSIK_MASK_FROMFILENAME = 0,
	MUSIK_MASK_TOFILENAME
};

///////////////////////////////////////////////////

class CmusikMaskDlg : public CDialog
{

public:

	//construct and destruct
	CmusikMaskDlg( CWnd* pParent, int initial_state = MUSIK_MASK_TOFILENAME ); 
	virtual ~CmusikMaskDlg();

	// misc
	enum { IDD = IDD_FILENAME_MASK };
	virtual void DoDataExchange(CDataExchange* pDX);

	// message maps
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedTitle();
	afx_msg void OnBnClickedArtist();
	afx_msg void OnBnClickedAlbum();
	afx_msg void OnBnClickedTrack();
	afx_msg void OnBnClickedGenre();
	afx_msg void OnBnClickedYear();
	afx_msg void OnBnClickedRating();
	afx_msg void OnBnClickedClr();
	afx_msg void OnEnKillfocusMask();
	afx_msg void OnClose();
	afx_msg void OnBnClickedPath2();
	afx_msg void OnBnClickedPath5();
	afx_msg void OnBnClickedPath3();
	afx_msg void OnBnClickedPath4();
	afx_msg void OnBnClickedPath6();
	afx_msg void OnBnClickedPath7();
	afx_msg void OnBnClickedPath8();
	afx_msg void OnBnClickedPath9();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedPath10();
	afx_msg void OnCbnSelchangeType();
	afx_msg void OnBnClickedApply();
	afx_msg void OnCbnEditchangeMask();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnCbnSelchangeMask();

protected:

	// vars
	CComboBox m_Type;
	CWnd* m_Parent;
	int m_State;
	CString* m_CurrMask;
	CString m_FromFilename;
	CString m_ToFilename;
	bool m_SpcToUndr;
	bool m_UndrToSpc;
	musikCore::StringArray m_Masks;

	void AppendMask( CString str );
	void PopulateMasks();

	//macros
	DECLARE_DYNAMIC(CmusikMaskDlg)
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedSpcundr();
};

///////////////////////////////////////////////////


