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
//   CmusikFileDialog
//
// Filename(s): 
//
//   musikFileDialog.h, musikFileDialog.cpp
//
// Information:
//
//   Class basically stolen from P.J Arends, see
//   http://www.codeproject.com/dialog/PJA_MultiSelect.asp
//
//   CFileDialog requires a static buffer size to retrieve
//   a list of files. This is not good, becuase we will
//   get an error if we select too many files. This dynamically
//   allocates memory for us so we don't have to worry about it.
//
//   Much thanks to the original author, P.J Arends.
// 
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

class CmusikFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CmusikFileDialog)

public:
	CmusikFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

	virtual ~CmusikFileDialog();

	// overrides
	virtual int DoModal();
	CString GetNextPathName( POSITION &pos ) const;
	POSITION GetStartPosition();

protected:
	BOOL m_bParsed;
	TCHAR *m_pstrFolder;
	TCHAR *m_pstrFiles;

	virtual void OnFileNameChange();
	void CleanStrings();

	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////


