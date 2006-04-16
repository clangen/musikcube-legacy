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

#include "stdafx.h"
#include "musikCube.h"
#include "musikFileDialog.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikFileDialog, CFileDialog)

///////////////////////////////////////////////////

CmusikFileDialog::CmusikFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
    m_bParsed = false;
    m_pstrFolder = NULL;
    m_pstrFiles = NULL;
}

///////////////////////////////////////////////////

CmusikFileDialog::~CmusikFileDialog()
{
	CleanStrings();
}

///////////////////////////////////////////////////

int CmusikFileDialog::DoModal()
{
	CleanStrings();

    int nRet = (int)CFileDialog::DoModal();

    if ( nRet == IDCANCEL )
    {
        DWORD dErr = CommDlgExtendedError();
        if ( dErr == 0x3003/*buffer to small*/ && m_pstrFiles )
            nRet = IDOK;
    }

	return nRet;
}

///////////////////////////////////////////////////

void CmusikFileDialog::CleanStrings()
{
    if ( m_pstrFiles )
    {
        delete[] m_pstrFiles;
        m_pstrFiles = NULL;
	}

	if ( m_pstrFolder )
	{
        delete[] m_pstrFolder;
        m_pstrFolder = NULL;
    }
}

///////////////////////////////////////////////////

CString CmusikFileDialog::GetNextPathName( POSITION &pos ) const
{
    if ( !m_pstrFiles )
        return CFileDialog::GetNextPathName( pos );

    ASSERT( pos );    
    TCHAR *ptrStr = (TCHAR *)pos;

    CString sRet = m_pstrFolder;
    sRet += _T("\\");
    sRet += ptrStr;

    ptrStr += _tcslen( ptrStr ) + 1;
    if ( *ptrStr )
        pos = (POSITION)ptrStr;
    else
        pos = NULL;

    return sRet;
}

///////////////////////////////////////////////////

POSITION CmusikFileDialog::GetStartPosition()
{
	if ( !m_pstrFiles )
		return CFileDialog::GetStartPosition();

	if ( !m_bParsed )
    {
		CString temp = m_pstrFiles;
		temp.Replace( _T( "\" \"" ), _T( "\"") );
		temp.Delete( 0, 1 );                      // remove leading quote mark
		temp.Delete( temp.GetLength() - 1, 1 );   // remove trailing space
    
		_tcscpy(m_pstrFiles, temp);
    
		TCHAR *ptrStr = m_pstrFiles;
		while ( *ptrStr )
		{
			if ( '\"' == *ptrStr )
				*ptrStr = '\0';

			++ptrStr;
		}

		m_bParsed = TRUE;
    }
    
    return (POSITION)m_pstrFiles;
}

///////////////////////////////////////////////////

void CmusikFileDialog::OnFileNameChange()
{
    TCHAR dummy_buffer;
    
    // Get the required size for the 'files' buffer
    UINT nfiles = CommDlg_OpenSave_GetSpec( GetParent()->m_hWnd, &dummy_buffer, 1 );

    // Get the required size for the 'folder' buffer
    UINT nfolder = CommDlg_OpenSave_GetFolderPath( GetParent()->m_hWnd, &dummy_buffer, 1 );

    // Check if lpstrFile and nMaxFile are large enough
    if ( nfiles + nfolder > m_ofn.nMaxFile )
    {
        m_bParsed = FALSE;

		// files
		if ( m_pstrFiles )
            delete[] m_pstrFiles;

        m_pstrFiles = new TCHAR[nfiles + 1];
        CommDlg_OpenSave_GetSpec( GetParent()->m_hWnd, m_pstrFiles, nfiles );

		// folder
        if ( m_pstrFolder )
            delete[] m_pstrFolder;

        m_pstrFolder = new TCHAR[nfolder + 1];
        CommDlg_OpenSave_GetFolderPath( GetParent()->m_hWnd, m_pstrFolder, nfolder );
    }
	else if ( m_pstrFiles )
		CleanStrings();

    CFileDialog::OnFileNameChange();
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikFileDialog, CFileDialog)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

