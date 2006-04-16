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
//   CmusikDirSync
//
// Filename(s): 
//
//   musikDirSync.h, musikDirSync.cpp
//
// Information:
//
//   Dialog used for adding / removing paths for
//   directory synchronization
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "musikDirSync.h"
#include "musikCube.h"
#include ".\musikdirsync.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikDirSync, CDialog)

///////////////////////////////////////////////////

CmusikDirSync::CmusikDirSync( CWnd* pParent )
	: CDialog(CmusikDirSync::IDD, pParent)
{
	m_Changed = false;
}

///////////////////////////////////////////////////

CmusikDirSync::~CmusikDirSync()
{
}

///////////////////////////////////////////////////

void CmusikDirSync::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DIRS, m_wndPaths);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikDirSync, CDialog)
	ON_BN_CLICKED(IDC_REMOVE, OnBnClickedRemove)
	ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
	ON_WM_CHAR()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_OK, OnBnClickedOk)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikDirSync::OnBnClickedOk()
{
	OnClose();
}

///////////////////////////////////////////////////

void CmusikDirSync::OnBnClickedRemove()
{
	DeleteSel();
}

///////////////////////////////////////////////////

void CmusikDirSync::DeleteSel()
{
	int count = m_wndPaths.GetCount();

	CString sStr;

	for ( int i = 0; i < count; i++ )
	{
		if ( m_wndPaths.GetSel( i ) )
		{
			m_wndPaths.GetText( i, sStr );
			musikCube::g_Library->RemovePath( (musikCore::String)sStr );
		}
	}

	ResetFromLibrary();
}

///////////////////////////////////////////////////

void CmusikDirSync::OnBnClickedAdd()
{
	TCHAR path[MAX_PATH];
	BROWSEINFO bi = { 0 };
	bi.lpszTitle = _T( "Please select a directory containing music." );
	bi.hwndOwner = GetSafeHwnd();

	LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );

    if ( pidl != 0 )
    {
		// get the name of the folder and put it in path
		SHGetPathFromIDList ( pidl, path );

        // free memory used
		IMalloc * imalloc = 0;
		if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
		{
			imalloc->Free ( pidl );
			imalloc->Release ( );
		}

		CString str_path = path;
		if ( str_path.Right( 1 ) != _T( "\\" ) )
			str_path += _T( "\\" );	
		
		if ( ValidatePath( str_path ) )
		{
			musikCube::g_Library->AddPath( str_path.GetBuffer( NULL ) );
			ResetFromLibrary();

			m_Changed = true;
		}
	}
}

///////////////////////////////////////////////////

BOOL CmusikDirSync::OnInitDialog()
{
	CDialog::OnInitDialog();

	ResetFromLibrary();

	return TRUE;
}

void CmusikDirSync::ResetFromLibrary()
{
	musikCore::StringArray paths;
	musikCube::g_Library->GetAllPaths( paths, false );

	m_wndPaths.ResetContent();
	
	for ( size_t i = 0; i < paths.size(); i++ )
	{
		m_wndPaths.AddString( paths.at( i ) );
	}
}

///////////////////////////////////////////////////

bool CmusikDirSync::ValidatePath( const CString& path )
{
	if ( m_wndPaths.GetCount() == 0 )
		return true;

	CString	original_str, conflict_str, conflict_err;
	musikCore::IntArray conflict_arr;

	original_str = path;
	original_str.MakeLower();

	for ( int i = 0; i < m_wndPaths.GetCount(); i++ )
	{
		m_wndPaths.GetText( i, conflict_str );
		conflict_str.MakeLower();

		// if the new path equals the old path then return
		if ( conflict_str == original_str )
		{
			MessageBox( 
                _T( "This path is already setup for synchronization." ), 
                _T( MUSIK_VERSION_STR ), 
                MB_ICONINFORMATION );

			return false;
		}

		// if new path is longer than path we're checking
		// against, it will be a child folder. see if they
		// have the same root
		if ( original_str.GetLength() > conflict_str.GetLength() )
		{
			if ( original_str.Find( conflict_str ) > -1 )
			{
				MessageBox( 
                    _T( "The path entered is already contained within the following path's scope:\n\n" ) + conflict_str, 
                    _T( MUSIK_VERSION_STR ), 
                    MB_ICONINFORMATION );

				return false;
			}
		}

		// if the old path is longer than the path we're
		// checking against, it may be a path's parent dir
		else 
		{
			if ( conflict_str.Find( original_str ) > -1 )
			{
				conflict_err += conflict_str + _T( "\n" );
				conflict_arr.push_back( i );
			}
		}
	}

	// display conflicts and ask user what to do
	if ( conflict_arr.size() )
	{
		if ( MessageBox( _T( "The path entered conflicts with the following paths:\n\n" ) + conflict_err + _T( "\nWould you like musikCube to fix this conflict for you?" ), _T( MUSIK_VERSION_STR ), MB_ICONINFORMATION | MB_YESNO ) == IDYES )
		{
			for ( size_t i = 0; i < conflict_arr.size(); i++ )
			{
				CString invalidPath;
				m_wndPaths.GetText( conflict_arr[i], invalidPath ); 
				musikCube::g_Library->RemovePath( invalidPath.GetBuffer() );
			}

			ResetFromLibrary();

			return true;
		}
		else 
			return false;
	}
	

	return true;
}

///////////////////////////////////////////////////

void CmusikDirSync::OnClose()
{
	if ( m_Changed )
	{
		if ( MessageBox( _T( "Your path synchronization configuration has changed. Resynchronize now?" ), _T( MUSIK_VERSION_STR ), MB_ICONINFORMATION | MB_YESNO ) == IDNO )
			m_Changed = false;
	}

	int WM_CLOSEDIRSYNC	= RegisterWindowMessage( _T( "CLOSEDIRSYNC" ) );
	GetParent()->PostMessage( WM_CLOSEDIRSYNC, (WPARAM)m_Changed, NULL );
}

///////////////////////////////////////////////////

BOOL CmusikDirSync::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE )
	{
		OnClose();
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

