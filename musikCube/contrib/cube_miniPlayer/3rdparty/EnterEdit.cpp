// EnterEdit.cpp : implementation file
//

#include "stdafx.h"
#include "EnterEdit.h"
#include ".\enteredit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int WM_ENTEREDIT_ENTER = RegisterWindowMessage( _T( "ENTEREDIT_ENTER" ) );
int WM_ENTEREDIT_ESC = RegisterWindowMessage( _T( "ENTEREDIT_ESC" ) );

/////////////////////////////////////////////////////////////////////////////
// CEnterEdit

CEnterEdit::CEnterEdit( CWnd* parent )
{
	m_Parent = parent;
}

CEnterEdit::~CEnterEdit()
{
}


BEGIN_MESSAGE_MAP(CEnterEdit, CEdit)

	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)

	ON_WM_CHAR()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnterEdit message handlers

void CEnterEdit::OnKillfocus() 
{
    m_Parent->PostMessage(WM_ENTEREDIT_ENTER, (LPARAM)this, NULL );
	//CEdit::OnKillFocus(NULL);
}

#ifdef COMPILE_STANDALONE
BOOL CEnterEdit::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
	{
		m_Parent->PostMessage(WM_ENTEREDIT_ENTER, (LPARAM)this, NULL );
		return TRUE;
	}

	else if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE )
	{
		m_Parent->SendMessage(WM_ENTEREDIT_ESC, (LPARAM)this, NULL );
		return TRUE;
	}
	return CEdit::PreTranslateMessage(pMsg);
}
#else
void CEnterEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch ( nChar )
	{
	case VK_RETURN:
		{
			m_Parent->PostMessage(WM_ENTEREDIT_ENTER, (LPARAM)this, NULL );
			return;
		}
		break;
	case VK_ESCAPE:
		{
			m_Parent->SendMessage(WM_ENTEREDIT_ESC, (LPARAM)this, NULL );
			return;
		}
		break;
	}
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}
#endif
