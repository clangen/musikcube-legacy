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
//   CmusikPropertyPage, CmusikPropertySheet
//
// Filename(s): 
//
//   musikPropertyPage.h, musikPropertyPage.cpp
//
// Information:
//
//   Property page that accepts a pointer to a CmusikPrefs obj
// 
// Usage: 
//
//   Use like a regular property page
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "musikPropertyPage.h"

///////////////////////////////////////////////////

// CmusikPropertyPage

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC( CmusikPropertyPage, CPropertyPage )

///////////////////////////////////////////////////

CmusikPropertyPage::CmusikPropertyPage( UINT nIDTemplate )
	: CPropertyPage( nIDTemplate )
{
	m_Modified = false;
}

///////////////////////////////////////////////////

// CmusikPropertySheet

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC( CmusikPropertySheet, CPropertySheet )

///////////////////////////////////////////////////

CmusikPropertySheet::CmusikPropertySheet( LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage )
	: CTreePropSheet( pszCaption, pParentWnd, iSelectPage )
{

}

///////////////////////////////////////////////////

BOOL CmusikPropertySheet::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch ( wParam )
	{
	case IDCANCEL:

		return CTreePropSheet::OnCommand( IDCANCEL, lParam );
		break;

	case IDOK:

		CommitChanges();
		return CTreePropSheet::OnCommand( IDCANCEL, lParam );

		return true;
		break;

	case ID_APPLY_NOW:

		CommitChanges();

		return true;
		break;
	}
	
	return CTreePropSheet::OnCommand( wParam, lParam );
}

///////////////////////////////////////////////////

void CmusikPropertySheet::CommitChanges()
{
	CmusikPropertyPage* ptrPage = NULL;
	for ( int i = 0; i < GetPageCount(); i++ )
	{
		ptrPage = (CmusikPropertyPage*)GetPage( i );
		if ( ptrPage->IsModified() )
		{
			ptrPage->CommitChanges();
			ptrPage->SetModified( FALSE );
		}
	}	
}

///////////////////////////////////////////////////
