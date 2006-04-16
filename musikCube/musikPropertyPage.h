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

#pragma once

///////////////////////////////////////////////////

#include "musikCube.h"

#include "3rdparty/TreePropSheet.h"
#include <musikCore.h>

///////////////////////////////////////////////////

using namespace TreePropSheet;

///////////////////////////////////////////////////

// CmusikPropertyPage

///////////////////////////////////////////////////

class CmusikPropertyPage : public CPropertyPage
{

public:

	CmusikPropertyPage( UINT nIDTemplate );
	virtual ~CmusikPropertyPage(){}

	bool IsModified(){ return m_Modified; }

	virtual void CommitChanges(){}
	virtual void LoadPrefs(){}

protected:

	bool m_Modified;
	DECLARE_DYNAMIC( CmusikPropertyPage )

};

///////////////////////////////////////////////////

// CmusikPropertySheet

///////////////////////////////////////////////////

class CmusikPropertySheet : public CTreePropSheet
{
public:

	CmusikPropertySheet( LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );
	virtual ~CmusikPropertySheet(){}

protected:
    
	void CommitChanges();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_DYNAMIC( CmusikPropertySheet )
};

///////////////////////////////////////////////////