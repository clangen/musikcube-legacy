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

#include "stdafx.h"
#include "musikDirBrowseCtrl.h"
#include ".\musikdirbrowsectrl.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikDirBrowseBar, baseCmusikDirBrowseBar)

BEGIN_MESSAGE_MAP(CmusikDirBrowseBar, baseCmusikDirBrowseBar)
    ON_WM_CREATE()
    ON_WM_SIZE()
END_MESSAGE_MAP()

CmusikDirBrowseBar::CmusikDirBrowseBar()
	: CmusikDockBar()
{

}

///////////////////////////////////////////////////

CmusikDirBrowseBar::~CmusikDirBrowseBar()
{
    delete m_wndChild;
}

///////////////////////////////////////////////////

void CmusikDirBrowseBar::OnOptions()
{

}

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikDirBrowseCtrl, CTreeCtrl)

BEGIN_MESSAGE_MAP(CmusikDirBrowseCtrl, CTreeCtrl)
    ON_WM_CREATE()
END_MESSAGE_MAP()


///////////////////////////////////////////////////

CmusikDirBrowseCtrl::CmusikDirBrowseCtrl()
    : CTreeCtrl()
{

}

///////////////////////////////////////////////////

CmusikDirBrowseCtrl::~CmusikDirBrowseCtrl()
{

}

///////////////////////////////////////////////////

int CmusikDirBrowseBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( baseCmusikDirBrowseBar::OnCreate(lpCreateStruct) == -1 )
		return -1;

	// dock bar
	SetBarStyle( GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
	EnableDocking( CBRS_ALIGN_ANY );

	// child
    m_wndChild = new CmusikDirBrowseCtrl();
    if ( !m_wndChild->Create( WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, CRect( 0, 0, 0, 0 ), this, 123 ) )
		return -1;

	if ( !m_Font.CreateStockObject(DEFAULT_GUI_FONT) )
		return -1;

	m_wndChild->SetFont( &m_Font );

	return 0;
}

///////////////////////////////////////////////////

int CmusikDirBrowseCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

    Refresh();

	return 0;
}

///////////////////////////////////////////////////

void CmusikDirBrowseCtrl::Refresh()
{
    DeleteAllItems();

    HTREEITEM root = InsertItem( _T( "C:\\" ) );
    LoadDirs( _T( "C:\\*.*" ), root );
}

///////////////////////////////////////////////////

void CmusikDirBrowseCtrl::LoadDirs( musikCore::String dir, HTREEITEM parent )
{
    /*
	// stopping conditions
	if ( dir.IsEmpty() )
		return;

	// find the first file in the directory...
	WIN32_FIND_DATA lpFindFileData;
	HANDLE hDir = FindFirstFileEx( 
        dir.c_str(), 
        FindExInfoStandard,
        &lpFindFileData,  
        FindExSearchLimitToDirectories, 
        NULL, NULL );

	// assure the first file exists...
	if ( hDir != INVALID_HANDLE_VALUE )
	{
        musikCore::String fn, temp;
        musikCore::Filename MFN;
		do
		{
			if ( lpFindFileData.cFileName[0] != '.' )
			{
				// file is a directory
				if ( lpFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				{
					fn = dir.Left( dir.GetLength() - 3 );	// remove *.* from full path
                    fn += lpFindFileData.cFileName;

                    HTREEITEM new_parent = InsertItem( lpFindFileData.cFileName, parent );

					if ( fn.Left( 0 ) != _T( "\\" ) )
						fn += _T( "\\*.*" );					// add \\*.* to the end of the new path
					else
						fn += _T( "*.*" );						// add *.* to the end of the new path

					LoadDirs( fn, parent );
				}
			}

		}
		while ( FindNextFile( hDir, &lpFindFileData ) );

		FindClose( hDir );
	}
    */
}

///////////////////////////////////////////////////

void CmusikDirBrowseBar::OnSize(UINT nType, int cx, int cy)
{
	CSizingControlBar::OnSize(nType, cx, cy);

	CRect rcClient;
	GetClientRect( &rcClient );

	m_wndChild->MoveWindow( rcClient );
}

///////////////////////////////////////////////////


