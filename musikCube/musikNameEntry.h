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
//   CmusikNameEntry
//
// Filename(s): 
//
//   musikNameEntry.h, musikNameEntry.cpp
//
// Information:
//
//   Dialog used for prompting user for a string.
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#pragma once
#include "afxwin.h"

///////////////////////////////////////////////////

class CmusikNameEntry : public CDialog
{
public:

	// construct and destruct
	CmusikNameEntry( CWnd* pParent, CString* str );
	virtual ~CmusikNameEntry();

	// mfc messages / overrides
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	// dialog data
	enum { IDD = IDD_NAME_ENTRY };

protected:

	// misc
	virtual void DoDataExchange(CDataExchange* pDX);

	//controls
	CEdit m_Edit;

	// string
	CString* m_Str;

	// macros
	DECLARE_DYNAMIC(CmusikNameEntry)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

