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
//   CmusikEditInPlace
//
// Filename(s): 
//
//   musikEditInPlace.h, musikEditInPlace.cpp
//
// Information:
//
//   musikCube's EditInPlace control for CmusikListCtrls
// 
// Usage: 
//
//   See retagging stuff in CmusikSelectionCtrl.
//
///////////////////////////////////////////////////

#include "stdafx.h"

#include "musikCube.h"
#include "musikEditInPlace.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikEditInPlace, CEdit)

///////////////////////////////////////////////////

int WM_MUSIKEDITCOMMIT = RegisterWindowMessage( _T( "MUSIKEDITCOMMIT" ) );
int WM_MUSIKEDITCANCEL = RegisterWindowMessage( _T( "MUSIKEDITCANCEL" ) );
int WM_MUSIKEDITCHANGE = RegisterWindowMessage( _T( "MUSIKEDITCHANGE" ) );

///////////////////////////////////////////////////

CmusikEditInPlace::CmusikEditInPlace()
{
	m_IsVisible = false;
	m_Args = NULL;
	m_brBg.CreateSolidBrush( musikCube::g_Prefs->GetListCtrlColor() );
}

///////////////////////////////////////////////////

CmusikEditInPlace::~CmusikEditInPlace()
{
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikEditInPlace, CEdit)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnEnKillfocus)
	ON_WM_SHOWWINDOW()
	ON_CONTROL_REFLECT(EN_CHANGE, OnEnChange)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikEditInPlace::Commit()
{
	GetWindowText( m_Str );
	m_Commited = true;
	m_IsVisible = false;

	if ( GetParent() && !m_MessageSent )
	{
		m_MessageSent = true;
		GetParent()->SendMessage( WM_MUSIKEDITCOMMIT, (WPARAM)this, (LPARAM)m_Args );
	}
}

///////////////////////////////////////////////////

void CmusikEditInPlace::Cancel()
{
	GetWindowText( m_Str );
	ShowWindow( SW_HIDE );
	m_IsVisible = false;
	m_Commited = false;

	if ( GetParent() && !m_MessageSent )
	{
		m_MessageSent = true;
		GetParent()->SendMessage( WM_MUSIKEDITCANCEL, (WPARAM)this, (LPARAM)m_Args );
	}
}

///////////////////////////////////////////////////

void CmusikEditInPlace::OnEnKillfocus()
{
	GetWindowText( m_Str );
	ShowWindow( SW_HIDE );
	m_IsVisible = false;
	m_Commited = false;

	if ( GetParent() && !m_MessageSent )
	{
		m_MessageSent = true;
		GetParent()->SendMessage( WM_MUSIKEDITCANCEL, (WPARAM)this, (LPARAM)m_Args );
	}
}

///////////////////////////////////////////////////

BOOL CmusikEditInPlace::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
	{
		Commit();
		return TRUE;
	}

	else if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE )
	{
		Cancel();
		return TRUE;
	}

	return CEdit::PreTranslateMessage(pMsg);
}

///////////////////////////////////////////////////

void CmusikEditInPlace::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CEdit::OnShowWindow(bShow, nStatus);

	if ( bShow )
	{
		m_Commited = false;
        m_MessageSent = false;

		SetWindowText( m_Str );
		SetSel( 0, m_Str.GetLength() );
		m_IsVisible = true;
	}
}

///////////////////////////////////////////////////

void CmusikEditInPlace::OnEnChange()
{
	if ( GetParent() )
		GetParent()->SendMessage( WM_MUSIKEDITCHANGE, (WPARAM)this, (LPARAM)m_Args );
}

///////////////////////////////////////////////////

bool CmusikEditInPlace::IsVisible()
{
	return m_IsVisible;
}

///////////////////////////////////////////////////

HBRUSH CmusikEditInPlace::CtlColor(CDC* pDC, UINT nCtlColor)
{
	HBRUSH hbr;

	pDC->SetBkColor( musikCube::g_Prefs->GetListCtrlColor() );
	pDC->SetTextColor( musikCube::g_Prefs->GetListCtrlTextColor() );
	hbr = (HBRUSH)m_brBg;

	return hbr;
}

///////////////////////////////////////////////////

