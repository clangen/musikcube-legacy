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
//   CmusikPlaylistInfoCtrl
//
// Filename(s): 
//
//   musikPlaylistInfoCtrl.h, musikPlaylistInfoCtrl.cpp
//
// Information:
//
//   Shows information about playlist, such as number of songs, size, etc.
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#include "stdafx.h"

#include "musikCube.h"
#include "musikPlaylistView.h"
#include "musikPlaylistInfoCtrl.h"

#include "MEMDC.H"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikPlaylistInfoCtrl, CWnd)

///////////////////////////////////////////////////

void CmusikPlaylistInfoWorker::run()
{
	// set flags
	m_Stop = false;
	m_Active = true;
	m_Finished = false;

	// this is the loop.. set it to true from 
	// anywhere and it will abort. threads run
	// very passivly, as to not interfere with 
	// the interface
	while ( !m_Stop )
	{
		if ( musikCube::g_Prefs->GetPlaylistInfoVizStyle() != PLAYLIST_INFO_VIZ_STYLE_NONE )
		{
			m_Parent->Invalidate();
		}

		Sleep( 100 );
	}

	// be sure to flag as finished
	m_Finished = true;
}

///////////////////////////////////////////////////
 
CmusikPlaylistInfoCtrl::CmusikPlaylistInfoCtrl( CmusikPlaylistCtrl* parent )
{
	m_ListCtrl = parent;

	m_InfoWorker = NULL;
	m_hBGBitmap = NULL;
}

///////////////////////////////////////////////////

CmusikPlaylistInfoCtrl::~CmusikPlaylistInfoCtrl()
{
	if ( m_hBGBitmap )
		DeleteObject ( m_hBGBitmap );

	if ( m_ListCtrl )
		m_ListCtrl->SetInfoCtrl( NULL );
	
	if ( m_InfoWorker )
	{
		m_InfoWorker->Stop();
		m_InfoWorker->join();
		delete m_InfoWorker;
		m_InfoWorker = NULL;
	}
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikPlaylistInfoCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikPlaylistInfoCtrl::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect( &rect );
	CRect innerrect(rect);
	innerrect.DeflateRect( 1, 1, 1, 1 );
	CMemDC memDC( &dc, &rect );

	memDC.Draw3dRect( rect, musikCube::g_Prefs->GetBtnShadowColor(), musikCube::g_Prefs->GetBtnHilightColor() );
    memDC.FillSolidRect( innerrect, musikCube::g_Prefs->GetBtnFaceColor() );	

	int style = musikCube::g_Prefs->GetPlaylistInfoVizStyle();
    if ( musikCube::g_Player->IsPlaying() && !musikCube::g_Player->IsPaused() )
    {
	    // draw the bitchin' background
		switch ( musikCube::g_Prefs->GetPlaylistInfoVizStyle() )
		{
		case PLAYLIST_INFO_VIZ_STYLE_EQ:
			DrawEQ( memDC.GetSafeHdc() );
			break;
		}
    }

	// put the text over it
	if ( musikCube::g_Prefs->PlaylistInfoVisible() )
	{
		// select the proper font
		CFont* oldfont = memDC.SelectObject( &m_Font );

		SetTextAlign( memDC.GetSafeHdc(), TA_CENTER );
		SetBkMode( memDC.GetSafeHdc(), TRANSPARENT ); 
		SetTextColor( memDC.GetSafeHdc(), musikCube::g_Prefs->GetBtnTextColor() );
		memDC.TextOut( rect.Width() / 2, 3, m_strInfo.c_str() );

		// get the old font back
		memDC.SelectObject( oldfont );
	}
}

///////////////////////////////////////////////////

int CmusikPlaylistInfoCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	
	m_Font.CreateFont(-10,0,0,0,FW_BOLD,FALSE,FALSE,
			0,ANSI_CHARSET,OUT_TT_PRECIS,
			CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
			FF_SWISS, _T( "Tahoma" ) );
	
	CDC* pDC = GetDC();
	pDC->SetBkColor( GetSysColor( COLOR_BTNHILIGHT ) );
	ReleaseDC( pDC );

	BITMAPINFO bminfo;	
	ZeroMemory ( &bminfo, sizeof (BITMAPINFO) );
	bminfo.bmiHeader.biWidth  = VIZ_WIDTH;
    bminfo.bmiHeader.biHeight = VIZ_HEIGHT;
    bminfo.bmiHeader.biPlanes = 1;
    bminfo.bmiHeader.biBitCount = 32;
    bminfo.bmiHeader.biCompression = BI_RGB;
    bminfo.bmiHeader.biXPelsPerMeter = 0;
    bminfo.bmiHeader.biYPelsPerMeter = 0;
    bminfo.bmiHeader.biClrUsed = 0;
    bminfo.bmiHeader.biClrImportant = 0;
	bminfo.bmiHeader.biSize = sizeof (BITMAPINFOHEADER);
	bminfo.bmiHeader.biSizeImage = VIZ_WIDTH * VIZ_HEIGHT * 4;
	
	m_hBGBitmap = CreateDIBSection ( NULL, &bminfo, DIB_RGB_COLORS, (void **)&m_pBGBitmapBits, NULL, NULL ); 

	// startup task
	m_InfoWorker = new CmusikPlaylistInfoWorker;
	m_InfoWorker->m_Parent = this;
	m_InfoWorker->start();

	return 0;
}

///////////////////////////////////////////////////

BOOL CmusikPlaylistInfoCtrl::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////

// draws a visualization of the current sound spectrum
// to a small bitmap, then stretches it to the size of
// the control to paint it

void CmusikPlaylistInfoCtrl::DrawEQ( HDC hdc )
{
    float spectrum[512];
    musikCube::g_Player->GetSpectrum( spectrum );

    if ( !spectrum )
        return;
	

	COLORREF clrBack = musikCube::g_Prefs->GetBtnFaceColor();
	clrBack = RGB(					// swap channels when drawing to a DIB
		GetBValue( clrBack ), 
		GetGValue( clrBack ), 
		GetRValue( clrBack ) 
	);

	COLORREF colorEQ = ::ShadeColor( musikCube::g_Prefs->GetBtnFaceColor(), -30 );
	colorEQ = RGB(					// swap channels when drawing to a DIB
		GetBValue( colorEQ ), 
		GetGValue( colorEQ ), 
		GetRValue( colorEQ ) 
	);

	// fill with the proper bg color	
	for ( int n = 0; n < (VIZ_WIDTH * VIZ_HEIGHT)-1; n++ )
	{
		m_pBGBitmapBits[n] = clrBack;
	}

	// now draw the viz directly to the bitmap bits

    // we're only going to work with the lower frequencies (0 - 6.5khz) because the higher
	// ones don't really show much
    for ( int x = 0; x < VIZ_WIDTH-1; x++ )
    {
		// get the "height" of the sample
		// seeing if doing it without the min stops the odd crash
        //int sample_y = min( (int)(spectrum[x] * 4.0f * (float)VIZ_HEIGHT), (VIZ_HEIGHT - 1) );
		int sample_y = (int)(spectrum[x] * 4.0f * (float)VIZ_HEIGHT);
		if ( sample_y > (VIZ_HEIGHT-1) )
			sample_y = VIZ_HEIGHT-1;
        

        for ( int y = 0; y < sample_y; y++ )
        {
			// every other pixel for fun effects
			if ( x % 2 )
			{
				if ( y % 2 )
				{
    				m_pBGBitmapBits[(VIZ_WIDTH * y) + x] = colorEQ;
				}
			}
        }
    }

    CRect rcClient;
	GetClientRect( &rcClient );

	// create a DC to work with the viz bitmap
	HDC hSrcDC;
	hSrcDC = CreateCompatibleDC( hdc );
	HGDIOBJ hOldBmp = ::SelectObject( hSrcDC, m_hBGBitmap );
	
	// and draw it
	::StretchBlt ( hdc,
				1,					// destination rect
				1,					// '
				rcClient.right-2,	// '
				rcClient.bottom-2,	// '
				hSrcDC,
				0,					// source rect
				0,					// '
				VIZ_WIDTH-1,		// '
				VIZ_HEIGHT-1,		// '
				SRCCOPY );
	

	// restore stuff
	::SelectObject (hSrcDC, hOldBmp);
	::DeleteDC(hSrcDC);
}

///////////////////////////////////////////////////

void CmusikPlaylistInfoCtrl::UpdateInfo()
{
	if ( m_ListCtrl->GetPlaylist() && m_ListCtrl->GetPlaylist()->GetCount() != 0 )
	{
		m_strInfo.Format( 
            _T( "number of songs: %d    (calculating...)" ),
            m_ListCtrl->GetPlaylist()->GetCount() );

		m_ListCtrl->RedrawWindow();

		size_t runtime = m_ListCtrl->GetPlaylist()->GetTotalTime();
		double totsize = m_ListCtrl->GetPlaylist()->GetTotalSize();
		
		CTimeSpan span( 0, 0, 0, runtime );
		CString strtime;
		if ( runtime > 172800 )
			strtime = span.Format( _T( "%D days, %H:%M:%S" ) );
		else if ( runtime > 86400 )
			strtime = span.Format( _T( "%D day, %H:%M:%S" ) );
		else
			strtime = span.Format( _T( "%H:%M:%S" ) );
	
		CString strsize( _T( "0.0 MB" ) );
		if ( totsize < 1024.0 )
			strsize.Format( _T( "%.2f B" ), totsize );
		else if ( totsize < ( 1024.0 * 1024.0 ) )
			strsize.Format( _T( "%.2f KB" ), totsize / 1024.0 );
		else if ( totsize < ( 1024.0 * 1024.0 * 1024.0 ) )
			strsize.Format( _T( "%.2f MB" ), totsize / 1024.0 / 1024.0 );
		else if ( totsize < ( 1024.0 * 1024.0 * 1024.0 * 1024.0 ) )
			strsize.Format( _T( "%.2f GB" ), totsize / 1024.0 / 1024.0 / 1024.0 );

		m_strInfo.Format( 
            _T( "number of songs: %d    running time: %s    size: %s" ), 
            m_ListCtrl->GetPlaylist()->GetCount(), 
            strtime, 
            strsize );
	}
	else
		m_strInfo = _T( "empty playlist" );

	Invalidate();
}

///////////////////////////////////////////////////

