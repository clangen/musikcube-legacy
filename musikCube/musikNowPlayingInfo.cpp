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
//   CmusikDynamicTextArray, CmusikFontBaseline, CmusikNowPlayingInfo
//
// Filename(s): 
//
//   musikNowPlayingInfo.h, musikNowPlayingInfo.cpp
//
// Information:
//
//   A single line of now playing text that accepts
//   a mask for formatting, including font attributes
//   like size, bold, and italic.
// 
// Usage: 
//
//	 Use Set() to set the mask, that should be
//   it. Follow the following guidelines:
//
//     %a [id]    <-- attribute, such as title, artist, album, etc
//     %f [size]  <-- regular font
//     %b [size]  <-- bold font
//     %i [size]  <-- italic font
//     %m [size]  <-- bold and italic font
//     %c [str]   <-- comment
//
// Example: 
//
//   m_Info->Set( _T( "%b20 %a4 %c from the album %a1" ) );
//
//      %b20 initializes the font to 20 with a bold attribute
//      %a4 prints the current song's title
//      %c prints the comment "from the album" using default font
//      %a1 prints the album name, with the previously specified font  
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "musikCube.h"
#include "musikNowPlayingInfo.h"

#include "MEMDC.H"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikNowPlayingInfo, CWnd)

///////////////////////////////////////////////////

CmusikNowPlayingInfo::CmusikNowPlayingInfo()
{
	m_Stopped.push_back( _T( "%c playback stopped" ) );
}

///////////////////////////////////////////////////

CmusikNowPlayingInfo::~CmusikNowPlayingInfo()
{
	size_t i;

	for ( i = 0; i < m_Captions.size(); i++ )
		delete m_Captions.at( i );

	for ( i = 0; i < m_Comments.size(); i++ )
		delete m_Comments.at( i );
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikNowPlayingInfo, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikNowPlayingInfo::UpdateInfo( bool refresh )
{
	SetRedraw( false );
	int type;
	for ( size_t i = 0; i < m_Captions.size(); i++ )
	{
		type = m_Captions.at( i )->GetType();
		m_Captions.at( i )->SetDynText( (CString)musikCube::g_Player->GetPlaying()->GetField( type ) );				
	}
	SetRedraw( true );

	Layout( refresh );
}

///////////////////////////////////////////////////

void CmusikNowPlayingInfo::Layout( bool refresh )
{
	SetRedraw( false );

	CSize rect = GetSize();

	CmusikFontBaseline blLargest, blCurrent;
	GetBaseline( GetHeight(), &blLargest );

	int nCurrX = 0;
	int nCurrY = 0;

	CRect rcClient;
	CPoint ptLoc;
	for ( size_t i = 0; i < m_LayoutOrder.size(); i++ )
	{
		GetBaseline( m_LayoutOrder.at( i )->GetFontSize(), &blCurrent );

		ptLoc.x = nCurrX;
		ptLoc.y = 0;
		ptLoc.y = blLargest.m_Ascending - blCurrent.m_Ascending;	

		m_LayoutOrder.at( i )->GetClientRect( rcClient );
		m_LayoutOrder.at( i )->MoveWindow( CRect( ptLoc, CSize( rcClient.Width(), blLargest.m_Height ) ) );

		nCurrX += m_LayoutOrder.at( i )->GetWidth();
	}

	SetRedraw( true );

	if ( refresh )
		RedrawWindow( NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ERASENOW | RDW_ALLCHILDREN );
}

///////////////////////////////////////////////////

int CmusikNowPlayingInfo::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

///////////////////////////////////////////////////

void CmusikNowPlayingInfo::Set( CString mask )
{
	if ( mask.IsEmpty() )
		return;

	// reinitalize the objects
	musikCore::Filename::DelimitStr( mask.GetBuffer( NULL ), _T( "%" ), m_Items, false );
	InitObjects();
}

///////////////////////////////////////////////////

void CmusikNowPlayingInfo::InitObjects()
{
	size_t i;

	// clear the old items
	for ( i = 0; i < m_Captions.size(); i++ )
		delete m_Captions.at( i );
	for ( i = 0; i < m_Comments.size(); i++ )
		delete m_Comments.at( i );
	
	m_Captions.clear();
	m_Comments.clear();
	m_LayoutOrder.clear();

	// make new items
	musikCore::StringArray& itemsref = m_Items;
	musikCube::g_Player->IsPlaying() ? itemsref = m_Items : itemsref = m_Items;

    size_t nObjs = itemsref.size();
	CString sCheck, sCurr;
	for ( i = 0; i < nObjs; i++ )
	{
		sCheck = itemsref.at( i ).Left( 1 );
		sCheck.MakeLower();

		// we found a new font
		if ( sCheck == _T( "f" ) || sCheck == _T( "b" ) || sCheck == _T( "i" ) || sCheck == _T( "m" ) )
		{
			sCurr = itemsref.at( i ).Right( itemsref.at( i ).GetLength()  - 1 );
			sCurr.TrimLeft();
			sCurr.TrimRight();

			m_FontSize = StoI( sCurr.GetBuffer( NULL ) );

			if ( sCheck == _T( "b" ) || sCheck == _T( "m" ) )
				m_FontBold = 1;
			else
				m_FontBold = 0;

			if ( sCheck == _T( "i" ) || sCheck == _T( "m" ) )
				m_FontItalic = 1;
			else
				m_FontItalic = 0;
		}

		// found a comment (default font)
		else if ( sCheck == _T( "c" ) )
		{
			// find the comment
			sCurr = itemsref.at( i ).Right( itemsref.at( i ).GetLength()  - 1 );

			// create the object
			CmusikDynamicText* pTemp;
			pTemp = new CmusikDynamicText();
			pTemp->Create( NULL, WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this );
			
			// set it to the right values
			pTemp->SetDynFont( musikCube::g_Prefs->GetNowPlayingCaptionFontSize() );
			pTemp->SetDynText( sCurr );
			pTemp->SetType( -1 );

			// add to comment array
			m_Comments.push_back( pTemp );
			m_LayoutOrder.push_back( pTemp );
		}

		// found a new object, so create it
		// with the current font and text
		else if ( sCheck == _T( "a" ) )
		{
			sCurr = itemsref.at( i ).Right( itemsref.at( i ).GetLength()  - 1 );
			sCurr.TrimLeft();
			sCurr.TrimRight();

			CmusikDynamicText* pTemp;
			pTemp = new CmusikDynamicText();
			pTemp->Create( NULL, WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this );

			pTemp->SetDynFont( m_FontSize, m_FontBold, m_FontItalic );
			pTemp->SetDynText( _T( "Stopped" ) );
			pTemp->SetType( StoI( sCurr.GetBuffer( NULL ) ) );

			m_Captions.push_back( pTemp );
			m_LayoutOrder.push_back( pTemp );
		}
		else
			cout << _T( "Invalid mask specified." );
	}

	if ( musikCube::g_Player->IsPlaying() )
		UpdateInfo();
}

///////////////////////////////////////////////////

int CmusikNowPlayingInfo::GetHeight()
{
	int largest = -1;
	for ( size_t i = 0; i < m_LayoutOrder.size(); i++ )
	{
		if ( m_LayoutOrder.at( i )->GetFontSize() > largest )
			largest = m_LayoutOrder.at( i )->GetFontSize();
	}

	if ( largest < 14 )
		largest = 14;

	return largest;
}

///////////////////////////////////////////////////

int CmusikNowPlayingInfo::GetWidth()
{
	size_t i;
	int width = 0;

	for ( i = 0; i < m_Captions.size(); i++ )
		width += m_Captions.at( i )->GetWidth();

	for ( i = 0; i < m_Comments.size(); i++ )
		width += m_Comments.at ( i )->GetWidth();

	return width;
}

///////////////////////////////////////////////////

CSize CmusikNowPlayingInfo::GetSize()
{
	return CSize( GetWidth(), GetHeight() ); 
}

///////////////////////////////////////////////////

void CmusikNowPlayingInfo::GetBaseline( int font_size, CmusikFontBaseline* baseline )
{
	CFont *temp_font;

	temp_font = new CFont();
	temp_font->CreateStockObject( DEFAULT_GUI_FONT );	
	
	LOGFONT temp_log_font;
	temp_font->GetLogFont( &temp_log_font );
	temp_log_font.lfHeight = font_size;

	delete temp_font;
	temp_font = new CFont();
	temp_font->CreateFontIndirect( &temp_log_font );
    
	CDC *pDC = GetDC();
	TEXTMETRIC metrics;
	CFont* pOldFont = pDC->SelectObject( temp_font );
	pDC->GetTextMetrics( &metrics );
	pDC->SelectObject( pOldFont );

	delete temp_font;

	// SH 2004-01 GDI leak cleanup  -- when doing GetDC, ReleaseDC is a must afterwards.
	ReleaseDC(pDC);
	
	baseline->m_Ascending = metrics.tmAscent;
	baseline->m_Descending = metrics.tmDescent;
	baseline->m_Height = metrics.tmHeight;
}

///////////////////////////////////////////////////

void CmusikNowPlayingInfo::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect( &rect );
	CMemDC memDC( &dc, &rect );

	memDC.FillSolidRect( rect, musikCube::g_Prefs->GetBtnFaceColor() );

	DefWindowProc(WM_PAINT, (WPARAM)memDC->m_hDC, (LPARAM)0);
}

///////////////////////////////////////////////////

BOOL CmusikNowPlayingInfo::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////
