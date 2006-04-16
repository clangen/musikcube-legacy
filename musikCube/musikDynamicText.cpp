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
//   CmusikNowPlayingCtrl, CmusikNowPlayingBar
//
// Filename(s): 
//
//   musikDynamicText.h, musikDynamicText.cpp
//
// Information:
//
//   Just a window that dynamically sizes itself
//   based on the window's text.
// 
// Usage: 
//
//	 SetDynText() to set the window text, 
//   and UpdateDynSize() to manually resize
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "musikCube.h"
#include "musikDynamicText.h"
#include "memdc.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikDynamicText, CStatic)

///////////////////////////////////////////////////

CmusikDynamicText::CmusikDynamicText()
{
	m_Width = -1;
	m_FontSize = -1;
	pFont = NULL;
}

///////////////////////////////////////////////////

CmusikDynamicText::~CmusikDynamicText()
{
	if ( pFont )
		delete pFont;
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikDynamicText, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

BOOL CmusikDynamicText::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////

void CmusikDynamicText::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(&rect);

	CMemDC memDC(&dc, &rect);

	memDC.FillSolidRect( 
		rect, 
		musikCube::g_Prefs->GetBtnFaceColor() );

	CString text;
	GetWindowText( text );

	UINT alignMode = DT_LEFT;
	if ( ::GetWindowLong( GetSafeHwnd(), GWL_STYLE ) & SS_CENTER )
	{
		alignMode = DT_CENTER;
	}

	memDC.SetTextColor( musikCube::g_Prefs->GetBtnTextColor() );
	HGDIOBJ oldFont = memDC.SelectObject( this->GetFont() );
	memDC.DrawText( text, rect, alignMode ); 

	memDC.SelectObject( oldFont );
}

///////////////////////////////////////////////////

void CmusikDynamicText::SetDynText( const CString& str, bool updatesize, bool movewindow )
{
	SetRedraw( FALSE );

	CString sVerify = str;
	sVerify.Replace( _T( "&" ), _T( "&&" ) );
	SetWindowText( sVerify );

	if ( updatesize )
	{
		UpdateDynSize( movewindow );
	}

	SetRedraw( TRUE );
	Invalidate();
}

///////////////////////////////////////////////////

void CmusikDynamicText::UpdateDynSize( bool movewindow )
{
	// get text
	CString str;
	GetWindowText( str );
	str.Replace( _T( "&&" ), _T( "&" ) );

	// get text extent
	CSize szText = GetTextSize( str );
	m_Width = szText.cx;

	// resize the window
	if ( movewindow )
		MoveWindow( CRect( CPoint( 0, 0 ), szText ) );
}

///////////////////////////////////////////////////

CSize CmusikDynamicText::GetTextSize( const CString& str )
{
	CSize szText;

	HDC	hMemDC	= NULL;
	hMemDC = CreateCompatibleDC( NULL );
	if ( hMemDC )
	{
		CDC* pDC = CDC::FromHandle( hMemDC );
		if ( pDC )
		{
			CFont* pOldFont = pDC->SelectObject( this->GetFont() );
			szText = pDC->GetTextExtent( str );
			pOldFont = pDC->SelectObject( pOldFont );
		}
	}
	DeleteDC( hMemDC );

	return szText;
}

///////////////////////////////////////////////////

void CmusikDynamicText::SetDynFont( int size, int bold, int italic )
{
	if ( pFont )
		delete pFont;

	pFont = new CFont();
	pFont->CreateStockObject( DEFAULT_GUI_FONT );

	// get the font
	LOGFONT pLogFont;
	pFont->GetLogFont( &pLogFont );

	pLogFont.lfHeight = size;

	if ( bold )
		pLogFont.lfWeight = FW_BOLD;
	if ( italic )
		pLogFont.lfItalic = true;

	// delete default font, make a new one
	// from the log font
	delete pFont;
	pFont = new CFont();
	pFont->CreateFontIndirect( &pLogFont );	

	SetFont( pFont, true );
	m_FontSize = abs( pLogFont.lfHeight );
}

///////////////////////////////////////////////////

CSize CmusikDynamicText::GetDynSize()
{
	CString str;
	GetWindowText( str );

	return GetTextSize( str );
}

///////////////////////////////////////////////////
