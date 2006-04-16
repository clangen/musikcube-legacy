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
//   musikNowPlayingCtrl.h, musikNowPlayingCtrl.cpp
//
// Information:
//
//   The "Now Playing Control"... the dock window at the bottom
//   of musikCube that displays song information with playback
//   controls.
// 
// Usage: 
//
//    Void.
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "musikCube.h"
#include "musikNowPlayingCtrl.h"

#include "Memdc.h"

///////////////////////////////////////////////////

// CmusikNowPlayingBar

///////////////////////////////////////////////////

CmusikNowPlayingBar::CmusikNowPlayingBar( CFrameWnd* parent )
	: CmusikDockBar()
{
	m_wndChild = new CmusikNowPlayingCtrl( parent );
}

///////////////////////////////////////////////////

CmusikNowPlayingBar::~CmusikNowPlayingBar()
{
	delete m_wndChild;
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikNowPlayingBar, baseCmusikNowPlayingBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

int CmusikNowPlayingBar::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	if ( baseCmusikNowPlayingBar::OnCreate(lpCreateStruct) == -1 )
		return -1;

	// dock bar
	SetBarStyle( GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
	EnableDocking( CBRS_ALIGN_BOTTOM | CBRS_ALIGN_TOP );

	ShowGripper( false );

	// child
	if ( !m_wndChild->Create( NULL, NULL, WS_CLIPCHILDREN | WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0), this, 123) )
		return -1;

	if ( !m_Font.CreateStockObject(DEFAULT_GUI_FONT) )
		return -1;

	m_wndChild->SetFont( &m_Font );
	
	return 0;
}

///////////////////////////////////////////////////

void CmusikNowPlayingBar::OnSize(UINT nType, int cx, int cy)
{
	CSizingControlBar::OnSize(nType, cx, cy);

	CRect rcClient;
	GetClientRect( &rcClient );

	m_wndChild->MoveWindow( &rcClient );
}

///////////////////////////////////////////////////

// CmusikNowPlayingCtrl

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikNowPlayingCtrl, CWnd)

///////////////////////////////////////////////////

CmusikNowPlayingCtrl::CmusikNowPlayingCtrl( CFrameWnd* parent )
{
	m_pPrevOff = m_pPrevOn = NULL;
	m_pPlayOff = m_pPlayOn = NULL;
	m_pPauseOff = m_pPauseOn = NULL;
	m_pStopOff = m_pStopOn = NULL;
	m_pNextOff = m_pNextOn = NULL;
	m_pCfOff = m_pCfOn = NULL;
	m_pEqOff = m_pEqOn = NULL;
	m_pRepeatOff = m_pRepeatOn = NULL;
	m_pRandomOff = m_pRandomOn = NULL;

	m_MainWnd = parent;
}

///////////////////////////////////////////////////

CmusikNowPlayingCtrl::~CmusikNowPlayingCtrl()
{
	delete m_Volume;
	delete m_Track;

	delete m_Play;
	delete m_Prev;
	delete m_Next;
    delete m_Stop;

	delete m_EnbCrossfader;
	delete m_EnbEqualizer;
	delete m_EnbRandom;
	delete m_EnbRepeatPlaylist;

	UnloadBitmaps();
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnSysColorChange()
{
	this->LoadBitmaps();
	this->SetButtonBitmaps();
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::LoadBitmaps()
{
	UnloadBitmaps();

    CmusikCubeApp* ptrApp = (CmusikCubeApp*)AfxGetApp();

	std::wstring strType;
	if ( musikCube::g_Prefs->UseLightIconTheme() )
	{
		strType = _T( "light" );
	}
	else
	{
		strType = _T( "dark" );
	}

    m_pPrevOff = Gdiplus::Bitmap::FromFile( 
        ptrApp->GetWorkingDir() + _T( "\\img\\prev_" ) + strType.c_str() + _T( "_uf.png" ) );
    m_pPrevOn = Gdiplus::Bitmap::FromFile( 
        ptrApp->GetWorkingDir() + _T( "\\img\\prev_" ) + strType.c_str() + _T( "_f.png" ) );

    m_pNextOff = Gdiplus::Bitmap::FromFile( 
        ptrApp->GetWorkingDir() + _T( "\\img\\next_" ) + strType.c_str() + _T( "_uf.png" ) );
    m_pNextOn = Gdiplus::Bitmap::FromFile( 
        ptrApp->GetWorkingDir() + _T( "\\img\\next_" ) + strType.c_str() + _T( "_f.png" ) );

    m_pStopOff = Gdiplus::Bitmap::FromFile( 
        ptrApp->GetWorkingDir() + _T( "\\img\\stop_" ) + strType.c_str() + _T( "_uf.png" ) );
    m_pStopOn = Gdiplus::Bitmap::FromFile( 
        ptrApp->GetWorkingDir() + _T( "\\img\\stop_" ) + strType.c_str() + _T( "_f.png" ) );

	m_pCfOff = Gdiplus::Bitmap::FromFile( 
		ptrApp->GetWorkingDir() + _T( "\\img\\cf_" ) + strType.c_str() + _T( "_uf.png" ) );
	m_pCfOn = Gdiplus::Bitmap::FromFile( 
		ptrApp->GetWorkingDir() + _T( "\\img\\cf_" ) + strType.c_str() + _T( "_f.png" ) );

	m_pEqOff = Gdiplus::Bitmap::FromFile( 
		ptrApp->GetWorkingDir() + _T( "\\img\\eq_" ) + strType.c_str() + _T( "_uf.png" ) );
	m_pEqOn = Gdiplus::Bitmap::FromFile( 
		ptrApp->GetWorkingDir() + _T( "\\img\\eq_" ) + strType.c_str() + _T( "_f.png" ) );

	m_pRepeatOff = Gdiplus::Bitmap::FromFile( 
		ptrApp->GetWorkingDir() + _T( "\\img\\repeat_" ) + strType.c_str() + _T( "_uf.png" ) );
	m_pRepeatOn = Gdiplus::Bitmap::FromFile( 
		ptrApp->GetWorkingDir() + _T( "\\img\\repeat_" ) + strType.c_str() + _T( "_f.png" ) );

	m_pRandomOff = Gdiplus::Bitmap::FromFile( 
		ptrApp->GetWorkingDir() + _T( "\\img\\random_" ) + strType.c_str() + _T( "_uf.png" ) );
	m_pRandomOn = Gdiplus::Bitmap::FromFile( 
		ptrApp->GetWorkingDir() + _T( "\\img\\random_" ) + strType.c_str() + _T( "_f.png" ) );

    m_pPlayOff = Gdiplus::Bitmap::FromFile( 
        ptrApp->GetWorkingDir() + _T( "\\img\\play_" ) + strType.c_str() + _T( "_uf.png" ) );
    m_pPlayOn = Gdiplus::Bitmap::FromFile( 
        ptrApp->GetWorkingDir() + _T( "\\img\\play_" ) + strType.c_str() + _T( "_f.png" ) );

    m_pPauseOff = Gdiplus::Bitmap::FromFile( 
        ptrApp->GetWorkingDir() + _T( "\\img\\pause_" ) + strType.c_str() + _T( "_uf.png" ) );
    m_pPauseOn = Gdiplus::Bitmap::FromFile( 
        ptrApp->GetWorkingDir() + _T( "\\img\\pause_" ) + strType.c_str() + _T( "_f.png" ) );
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::UnloadBitmaps()
{
	delete m_pPrevOff;
	delete m_pPrevOn;
	delete m_pNextOff;
	delete m_pNextOn;
	delete m_pStopOff;
	delete m_pStopOn;
	delete m_pCfOff;
	delete m_pCfOn;
	delete m_pEqOff;
	delete m_pEqOn;
	delete m_pRepeatOff;
	delete m_pRepeatOn;
	delete m_pRandomOff;
	delete m_pRandomOn;
	delete m_pPlayOff;
	delete m_pPlayOn;
	delete m_pPauseOff;
	delete m_pPauseOn;
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::SetButtonBitmaps()
{
	m_EnbCrossfader->SetBitmaps( m_pCfOn, m_pCfOff );
	m_EnbEqualizer->SetBitmaps( m_pEqOn, m_pEqOff );
	m_EnbRandom->SetBitmaps( m_pRandomOn, m_pRandomOff );
	m_EnbRepeatPlaylist->SetBitmaps( m_pRepeatOn, m_pRepeatOff );
	m_Prev->SetBitmaps( m_pPrevOn, m_pPrevOff );
	m_Next->SetBitmaps( m_pNextOn, m_pNextOff );
	m_Stop->SetBitmaps(  m_pStopOn, m_pStopOff );

	this->SetPlayingButton();
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikNowPlayingCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SYSCOLORCHANGE()
	ON_BN_CLICKED(MUSIK_NOWPLAYING_BTN_PREV, OnBtnPrev)
	ON_BN_CLICKED(MUSIK_NOWPLAYING_BTN_PLAY, OnBtnPlay)
	ON_BN_CLICKED(MUSIK_NOWPLAYING_BTN_NEXT, OnBtnNext)
	ON_BN_CLICKED(MUSIK_NOWPLAYING_BTN_STOP, OnBtnStop)
	ON_COMMAND(ID_PLAYRANDOM_ARTIST, OnPlayrandomArtist)
	ON_COMMAND(ID_PLAYRANDOM_ALBUM, OnPlayrandomAlbum)
	ON_COMMAND(ID_PLAYRANDOM_GENRE, OnPlayrandomGenre)
	ON_COMMAND(ID_PLAYRANDOM_YEAR, OnPlayrandomYear)
	ON_COMMAND(ID_QUEUERANDOM_ARTIST, OnQueuerandomArtist)
	ON_COMMAND(ID_QUEUERANDOM_ALBUM, OnQueuerandomAlbum)
	ON_COMMAND(ID_QUEUERANDOM_GENRE, OnQueuerandomGenre)
	ON_COMMAND(ID_QUEUERANDOM_YEAR, OnQueuerandomYear)
	ON_BN_CLICKED(MUSIK_NOWPLAYING_ENB_CROSSFADER, OnEnbCrossfader)
	ON_BN_CLICKED(MUSIK_NOWPLAYING_ENB_EQUALIZER, OnEnbEqualizer)
	ON_BN_CLICKED(MUSIK_NOWPLAYING_ENB_RANDOM, OnEnbRandom)
	ON_BN_CLICKED(MUSIK_NOWPLAYING_ENB_REPEATPLAYLIST, OnEnbRepeat)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect( &rect );
	CMemDC memDC( &dc, &rect );

	memDC.FillSolidRect( rect, musikCube::g_Prefs->GetBtnFaceColor() );
}

///////////////////////////////////////////////////

int CmusikNowPlayingCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rcClient;
	GetClientRect( &rcClient );

	m_Tips.Create( this, TTS_ALWAYSTIP );
	m_Font.CreateStockObject( DEFAULT_GUI_FONT );

	LoadBitmaps();

	m_EnbCrossfader = new CmusikToggleBtn();
	if ( !m_EnbCrossfader->Create( _T( "" ), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, CRect( 0, 0, 0, 0 ), this, MUSIK_NOWPLAYING_ENB_CROSSFADER ) )
		return -1;
	m_EnbCrossfader->SetFont( &m_Font );
	m_Tips.AddTool( m_EnbCrossfader, _T( "crossfader" ) );
	if ( !musikCube::g_DrawGraphics )
		m_EnbCrossfader->SetWindowText( _T( "fade" ) );

	m_EnbEqualizer = new CmusikToggleBtn();
	if ( !m_EnbEqualizer->Create( _T( "" ), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, CRect( 0, 0, 0, 0 ), this, MUSIK_NOWPLAYING_ENB_EQUALIZER ) )
		return -1;
	m_EnbEqualizer->SetFont( &m_Font );
	m_Tips.AddTool( m_EnbEqualizer, _T( "equalizer" ) );
	if ( !musikCube::g_DrawGraphics )
		m_EnbEqualizer->SetWindowText( _T( "eq" ) );

	m_EnbRandom = new CmusikToggleBtn();
	if ( !m_EnbRandom->Create( _T( "" ), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, CRect( 0, 0, 0, 0 ), this, MUSIK_NOWPLAYING_ENB_RANDOM ) )
		return -1;
	m_EnbRandom->SetFont( &m_Font );
	m_Tips.AddTool( m_EnbRandom, _T( "random playback" ) );
	if ( !musikCube::g_DrawGraphics )
		m_EnbRandom->SetWindowText( _T( "random" ) );

	m_EnbRepeatPlaylist = new CmusikToggleBtn();
	if ( !m_EnbRepeatPlaylist->Create( _T( "" ), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, CRect( 0, 0, 0, 0 ), this, MUSIK_NOWPLAYING_ENB_REPEATPLAYLIST ) )
		return -1;
	m_EnbRepeatPlaylist->SetFont( &m_Font );
	m_Tips.AddTool( m_EnbRepeatPlaylist, _T( "repeat playlist" ) );
	if ( !musikCube::g_DrawGraphics )
		m_EnbRepeatPlaylist->SetWindowText( _T( "repeat" ) );

	m_Info1 = new CmusikNowPlayingInfo();
	if ( !m_Info1->Create( NULL, NULL, WS_CHILD | WS_VISIBLE, rcClient, this, 123, NULL ) )
		return -1;
	m_Info1->Set( _T( "%cNot Playing" ) );

	m_Info2 = new CmusikNowPlayingInfo();
	if ( !m_Info2->Create( NULL, NULL, WS_CHILD | WS_VISIBLE, rcClient, this, 123, NULL ) )
		return -1;
	m_Info2->Set( _T( "%cNot Playing" ) );

	m_Volume = new CmusikVolumeCtrl();
	if ( !m_Volume->Create( TBS_VERT | TBS_NOTICKS | WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this, 123 ) )
		return -1;
	m_Tips.AddTool( m_Volume, _T( "volume" ) );

	m_Track = new CmusikTimeCtrl();
	if ( !m_Track->Create( NULL, NULL, WS_CLIPCHILDREN | WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this, 123 ) )
		return -1;

	m_Prev = new CmusikToggleBtn();
	if ( !m_Prev->Create( _T( "" ), WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this, MUSIK_NOWPLAYING_BTN_PREV ) )
		return -1;
	m_Prev->SetFont( &m_Font );
	m_Tips.AddTool( m_Prev, _T( "previous song" ) );
	if ( !musikCube::g_DrawGraphics )
		m_Prev->SetWindowText( _T( "prev" ) );

	m_Play = new CmusikToggleBtn();
	if ( !m_Play->Create( _T( "" ), WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this, MUSIK_NOWPLAYING_BTN_PLAY ) )
		return -1;
	m_Play->SetFont( &m_Font );
	m_Tips.AddTool( m_Play, _T( "play / pause" ) );
	if ( !musikCube::g_DrawGraphics )
		m_Play->SetWindowText( _T( "play" ) );

	m_Next = new CmusikToggleBtn();
	if ( !m_Next->Create( _T( "" ), WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this, MUSIK_NOWPLAYING_BTN_NEXT ) )
		return -1;
	m_Next->SetFont( &m_Font );
	m_Tips.AddTool( m_Next, _T( "next song" ) );
	if ( !musikCube::g_DrawGraphics )
		m_Next->SetWindowText( _T( "next" ) );

	m_Stop = new CmusikToggleBtn();
	if ( !m_Stop->Create( _T( "" ), WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this, MUSIK_NOWPLAYING_BTN_STOP ) )
		return -1;
	m_Stop->SetFont( &m_Font );
	m_Tips.AddTool( m_Stop, _T( "stop" ) );
	if ( !musikCube::g_DrawGraphics )
		m_Stop->SetWindowText( _T( "stop" ) );

	SetButtonBitmaps();
	UpdateCheckboxes();

	return 0;
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::UpdateInfo( bool refresh )
{
	SetRedraw( FALSE );
	m_Info1->Set( ::GetPlaybackStr( PBSTRTYPE_NOWPLAYING_TITLE ) );
	m_Info1->UpdateInfo();

	m_Info2->Set( ::GetPlaybackStr( PBSTRTYPE_NOWPLAYING_ALBUM_ARTIST ) );
	m_Info2->UpdateInfo();

	SetRedraw();
	
	SetPlayingButton();	// set button to playing, paused, or resume. logic inside.

	RescaleInfo();
	m_Info2->RedrawWindow();
	m_Info1->RedrawWindow();
	RedrawWindow();
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::UpdateVolume()
{
    m_Volume->UpdateVolume();
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::SetPlayingButton()
{
	if ( musikCube::g_Player->IsPlaying() )
	{
		// playing, not paused
		if ( !musikCube::g_Player->IsPaused() )
		{
			if ( !musikCube::g_DrawGraphics )
			{
				m_Play->SetWindowText( _T( "pause" ) );
			}
			else
			{
				m_Play->SetBitmaps( m_pPauseOn, m_pPauseOff );
			}
		}
		// playing, but paused
		else	
		{
			if ( !musikCube::g_DrawGraphics )
			{
				m_Play->SetWindowText( _T( "resume" ) );
			}
			else
			{
				m_Play->SetBitmaps( m_pPlayOn, m_pPlayOff );
			}
		}
	}
	// not playing (stopped)
	else
	{
		if ( !musikCube::g_DrawGraphics )
		{
			m_Play->SetWindowText( _T( "play" ) );
		}
		else
		{
			m_Play->SetBitmaps( m_pPlayOn, m_pPlayOff );
		}
	}


	m_Play->RedrawWindow();
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnSize(UINT nType, int cx, int cy)
{
	RescaleInfo();
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnDestroy()
{
	CWnd::OnDestroy();

	delete m_Info1;
	delete m_Info2;
}

///////////////////////////////////////////////////

CSize CmusikNowPlayingBar::CalcDynamicLayout( int nLength, DWORD dwMode )
{
	CSize sz = CmusikDockBar::CalcDynamicLayout( nLength, dwMode );
	
	if ( sz.cy < 64 )
		sz.cy = 64;

	return sz;
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::RescaleInfo()
{
	CSize szSize;
	CRect rcClient;
	CRect lpRect;

	int x_offset = 8;

	int btn_x, btn_y;
	if ( musikCube::g_DrawGraphics )
	{
		btn_x = 32;
		btn_y = 24;
	}
	else
	{
		btn_x = 48;
		btn_y = 24;
	}
	int btn_spc = 2;

	GetClientRect( &rcClient );	
	int btn_sum = ( MUSIK_NOWPLAYING_TOGGLE_CNT * btn_x ) + ( MUSIK_NOWPLAYING_TOGGLE_CNT * btn_spc );

	//
	// track
	//

	lpRect = CRect( 
		rcClient.left + btn_sum + x_offset,	// left
		rcClient.bottom - 18,				// top
		rcClient.right - 4 - 18,			// right
		rcClient.bottom - 2 );				// bottom

	m_Track->MoveWindow( lpRect );

	//
	// volume
	//

	lpRect = CRect( 
		rcClient.right - 16,			// left
		rcClient.top,					// top
		rcClient.right,					// right
		rcClient.bottom + 4 );			// bottom

	m_Volume->MoveWindow( lpRect );

	int track_top = rcClient.bottom - 18;
	
	//
	// toggles 
	//

	int btn_start = rcClient.Width() - btn_sum - 16;

	m_EnbCrossfader->MoveWindow( CRect(
		CPoint( rcClient.left + ( 0 * btn_spc ) + ( 0 * btn_x ), 4 ),
		CSize( btn_x, btn_y ) ) );

	m_EnbEqualizer->MoveWindow( CRect(
		CPoint( rcClient.left + ( 1 * btn_spc ) + ( 1 * btn_x ), 4 ),
		CSize( btn_x, btn_y ) ) );

	m_EnbRepeatPlaylist->MoveWindow( CRect(
		CPoint( rcClient.left + ( 2 * btn_spc ) + ( 2 * btn_x ), 4 ),
		CSize( btn_x, btn_y ) ) );

	m_EnbRandom->MoveWindow( CRect(
		CPoint( rcClient.left + ( 3 * btn_spc ) + ( 3 * btn_x ), 4 ),
		CSize( btn_x, btn_y ) ) );

	//
	// playback buttons
	//

	btn_sum = ( MUSIK_NOWPLAYING_BTN_CNT * btn_x ) + ( MUSIK_NOWPLAYING_BTN_CNT * btn_spc );
	btn_start = rcClient.Width() - btn_sum - 16;

	m_Prev->MoveWindow( CRect(
		CPoint( rcClient.left + ( 0 * btn_spc ) + ( 0 * btn_x ), rcClient.bottom - btn_y - 2 ),
		CSize( btn_x, btn_y ) ) );

	m_Play->MoveWindow( CRect(
		CPoint( rcClient.left + ( 1 * btn_spc ) + ( 1 * btn_x ), rcClient.bottom - btn_y - 2 ),
		CSize( btn_x, btn_y ) ) );

	m_Stop->MoveWindow( CRect(
		CPoint( rcClient.left + ( 2 * btn_spc ) + ( 2 * btn_x ), rcClient.bottom - btn_y - 2 ),
		CSize( btn_x, btn_y ) ) );

	m_Next->MoveWindow( CRect(
		CPoint( rcClient.left + ( 3 * btn_spc ) + ( 3 * btn_x ), rcClient.bottom - btn_y - 2 ),
		CSize( btn_x, btn_y ) ) );

	//
	// info lines
	//

	int info_start = btn_sum + 8;

	if ( musikCube::g_Player->IsPlaying() )
	{
		// may be hidden
		m_Info2->ShowWindow( SW_SHOWNORMAL );

		szSize = m_Info1->GetSize();
		szSize.cx = rcClient.Width() - btn_sum - 16 - 8;

		// [title]
		lpRect = CRect(
			CPoint( btn_sum + x_offset, 4 ),
			szSize );

		m_Info1->MoveWindow( lpRect );

		// by [artist] from the album [album]

		int ht = m_Info2->GetHeight();
		int remain = rcClient.Height() - ( m_Info2->GetHeight() + 16 );

		szSize = m_Info2->GetSize();
		szSize.cx = rcClient.Width() - btn_sum - 16 - 8;

		lpRect = CRect(
			CPoint( btn_sum + x_offset, ( remain / 2 ) + ( ht / 2 ) + 2 ),
			szSize );

		m_Info2->MoveWindow( lpRect );
	}
	else
	{
		m_Info2->ShowWindow( SW_HIDE );

		int ht = m_Info1->GetHeight();

		// [not playing]
		lpRect = CRect(
			CPoint( btn_sum + x_offset, ( track_top / 2 ) - ( ht / 2 ) ),
			m_Info2->GetSize() );

		m_Info1->MoveWindow( lpRect );
	}
}

///////////////////////////////////////////////////

BOOL CmusikNowPlayingCtrl::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnBtnPlay()
{
	// if it's not playing, play a song
	if ( !musikCube::g_Player->IsPlaying() )
	{
		int PLAYER_PLAYSEL = RegisterWindowMessage( _T( "PLAYER_PLAYSEL" ) );
		m_MainWnd->SendMessage( PLAYER_PLAYSEL );
	}

	else if ( musikCube::g_Player->IsPlaying() )
	{
		// if it is playing, pause...
		if ( !musikCube::g_Player->IsPaused() )
			musikCube::g_Player->Pause();

		// is paused, resume
		else
			musikCube::g_Player->Resume();
	}
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnBtnPrev()
{
	musikCube::g_Player->Prev();
    GetParent()->SetFocus();
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnBtnNext()
{
	musikCube::g_Player->Next();
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::PlayRandom( int field )
{
	musikCube::g_Library->GetRandom(
		field,
		*musikCube::g_Player->GetPlaylist(),
		true );

	int WM_NOWPLAYINGUPDATED = RegisterWindowMessage( _T( "NOWPLAYINGUPDATED" ) );
	m_MainWnd->SendMessage( WM_NOWPLAYINGUPDATED );

    musikCube::g_Player->Play( 0, 0, musikCore::MUSIK_PLAYER_SONG_USER_ADVANCE, true );	
}	

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::QueueRandom( int field )
{
	musikCube::g_Library->GetRandom(
		field,
		*musikCube::g_Player->GetPlaylist(),
		false );

	int WM_NOWPLAYINGUPDATED = RegisterWindowMessage( _T( "NOWPLAYINGUPDATED" ) );
	m_MainWnd->SendMessage( WM_NOWPLAYINGUPDATED );
}	

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnPlayrandomArtist()
{
	PlayRandom( musikCore::MUSIK_LIBRARY_TYPE_ARTIST );
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnPlayrandomAlbum()
{
	PlayRandom( musikCore::MUSIK_LIBRARY_TYPE_ALBUM );
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnPlayrandomGenre()
{
	PlayRandom( musikCore::MUSIK_LIBRARY_TYPE_GENRE );
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnPlayrandomYear()
{
	PlayRandom( musikCore::MUSIK_LIBRARY_TYPE_YEAR );
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnQueuerandomArtist()
{
	QueueRandom( musikCore::MUSIK_LIBRARY_TYPE_ARTIST );
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnQueuerandomAlbum()
{
	QueueRandom( musikCore::MUSIK_LIBRARY_TYPE_ALBUM );
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnQueuerandomGenre()
{
	QueueRandom( musikCore::MUSIK_LIBRARY_TYPE_GENRE );
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnQueuerandomYear()
{
	QueueRandom( musikCore::MUSIK_LIBRARY_TYPE_YEAR );
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnBtnStop()
{
    if ( musikCube::g_Player->IsPlaying() )
        musikCube::g_Player->Stop();
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnEnbCrossfader()
{
	if ( m_EnbCrossfader->GetCheck() )
	{
		musikCore::Crossfader fader;
		CmusikCubeApp* ptrApp = (CmusikCubeApp*)AfxGetApp();
		ptrApp->GetCrossfader( &fader );

		musikCube::g_Prefs->SetCrossfaderEnabled( true );
		musikCube::g_Player->EnableCrossfader( true );
		musikCube::g_Player->SetCrossfader( fader );
	}
	else
	{
		musikCube::g_Prefs->SetCrossfaderEnabled( false );
		musikCube::g_Player->EnableCrossfader( false );
	}
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnEnbEqualizer()
{
	if ( musikCube::g_Prefs->IsEqualizerEnabled() )
	{
		musikCube::g_Prefs->SetEqualizerEnabled( false );
		musikCube::g_Player->EnableEqualizer( false, musikCube::g_Prefs->EqualizerUseGlobal() );
		return;
	}
	else
	{
		musikCube::g_Prefs->SetEqualizerEnabled( true );
		musikCube::g_Player->EnableEqualizer( true, musikCube::g_Prefs->EqualizerUseGlobal() );
		musikCore::SetEQPreampLevel( musikCube::g_Prefs->GetEqualizerMultiplier() );
		musikCube::g_Player->UpdateEQSettings();
	}
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnEnbRandom()
{
	if ( musikCube::g_Player->GetPlaymode() & musikCore::MUSIK_PLAYER_PLAYMODE_RANDOM )
		musikCube::g_Player->ModifyPlaymode( NULL, musikCore::MUSIK_PLAYER_PLAYMODE_RANDOM );
	else
		musikCube::g_Player->ModifyPlaymode( musikCore::MUSIK_PLAYER_PLAYMODE_RANDOM, NULL );

	musikCube::g_Prefs->SetPlayerPlaymode( musikCube::g_Player->GetPlaymode() );
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnEnbRepeat()
{
	if ( musikCube::g_Player->GetPlaymode() & musikCore::MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST )
		musikCube::g_Player->ModifyPlaymode( NULL, musikCore::MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST );
	else
		musikCube::g_Player->ModifyPlaymode( musikCore::MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST, NULL );

	musikCube::g_Prefs->SetPlayerPlaymode( musikCube::g_Player->GetPlaymode() );
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::UpdateCheckboxes()
{
	m_EnbCrossfader->SetCheck( musikCube::g_Player->IsCrossfaderEnabled() );
	m_EnbEqualizer->SetCheck( musikCube::g_Player->IsEqualizerEnabled() );
	m_EnbRandom->SetCheck( musikCube::g_Player->GetPlaymode() & musikCore::MUSIK_PLAYER_PLAYMODE_RANDOM );
	m_EnbRepeatPlaylist->SetCheck( musikCube::g_Player->GetPlaymode() & musikCore::MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST );
}

///////////////////////////////////////////////////

BOOL CmusikNowPlayingCtrl::PreTranslateMessage(MSG* pMsg)
{
	m_Tips.RelayEvent( pMsg );
	return CWnd::PreTranslateMessage(pMsg);
}

///////////////////////////////////////////////////

void CmusikNowPlayingCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint point )
{
	CMenu menu;
	menu.LoadMenu( IDR_NOWPLAYING_MENU );
	menu.GetSubMenu( 0 )->TrackPopupMenu( NULL, point.x, point.y, this );
}

///////////////////////////////////////////////////



