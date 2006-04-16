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
//   CmusikEqualizerBar, CmusikEqualizerCtrl
//
// Filename(s): 
//
//   musikEqualizerCtrl.h, musikEqualizertrl.cpp
//
// Information:
//
//   musikCube's equalizer control.
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "musikCube.h"

#include "musikEqualizerCtrl.h"

#include "MEMDC.H"

///////////////////////////////////////////////////

// CmusikEqualizerBar

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikEqualizerBar, baseCmusikEqualizerBar)

///////////////////////////////////////////////////

int WM_CLOSEEQUALIZERPRESETS = RegisterWindowMessage( _T( "CLOSEEQUALIZERPRESETS" ) );

///////////////////////////////////////////////////

CmusikEqualizerBar::CmusikEqualizerBar()
	: CmusikDockBar()
{
	m_wndChild = new CmusikEqualizerCtrl();
	m_Presets = NULL;
}

///////////////////////////////////////////////////

CmusikEqualizerBar::~CmusikEqualizerBar()
{
	OnClosePresets( NULL, NULL );
	delete m_wndChild;
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikEqualizerBar, baseCmusikEqualizerBar)
	// mfc maps
	ON_WM_CREATE()
	ON_WM_SIZE()

	// custom messages
	ON_REGISTERED_MESSAGE( WM_CLOSEEQUALIZERPRESETS, OnClosePresets )

	// menu commands
	ON_COMMAND(ID_EQUALIZER_PRESETS, OnEqualizerPresets)
	ON_COMMAND(ID_EQUALIZER_STATE_18BAND, OnEqualizerState18band)
	ON_COMMAND(ID_EQUALIZER_STATE_6BAND, OnEqualizerState6band)
	ON_COMMAND(ID_EQUALIZER_SET_AS_DEFAULT, OnEqualizerSetAsDefault)
	ON_COMMAND(ID_EQUALIZER_RESETDEFAULT, OnEqualizerResetdefault)
	ON_COMMAND(ID_EQUALIZER_USE_GLOBAL, OnEqualizerAutomaticallySave)
	ON_COMMAND(ID_EQUALIZER_CLOSE_VIEW, OnEqualizerCloseView)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

int CmusikEqualizerBar::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	if ( baseCmusikEqualizerBar::OnCreate(lpCreateStruct) == -1 )
		return -1;

	// dock bar
	SetBarStyle( GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
	EnableDocking( CBRS_ALIGN_ANY );

	// child
     musikCore::SetEQPreampLevel( musikCube::g_Prefs->GetEqualizerMultiplier() );
	if ( !m_wndChild->Create( NULL, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, CRect( 0, 0, 0, 0), this, 123 ) )
		return -1;

	if ( !m_Font.CreateStockObject(DEFAULT_GUI_FONT) )
		return -1;

	m_wndChild->SetFont( &m_Font );

	return 0;
}

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnOptions()
{
	CPoint pos;
	::GetCursorPos( &pos );

	CMenu main_menu;
	CMenu* popup_menu;

	main_menu.LoadMenu( IDR_EQUALIZER_MENU );
	popup_menu = main_menu.GetSubMenu( 0 );

	// update ui does not work becuase the
	// menu does not belong to a CView or
	// CFrameWnd inherited class, so this
	// updates the menu items...
	popup_menu->EnableMenuItem( ID_EQUALIZER_PRESETS, ( m_Presets == 0 ) ? MF_ENABLED : MF_GRAYED | MF_DISABLED );
	popup_menu->CheckMenuItem( ID_EQUALIZER_USE_GLOBAL, ( musikCube::g_Prefs->EqualizerUseGlobal() == true ) ? MF_CHECKED : MF_UNCHECKED );
	popup_menu->CheckMenuItem( ID_EQUALIZER_ENABLED2, ( musikCube::g_Prefs->IsEqualizerEnabled() == true ) ? MF_CHECKED : MF_UNCHECKED );

	int band_state = GetCtrl()->GetBandState();
	if ( band_state == MUSIK_EQUALIZER_CTRL_18BANDS )
		popup_menu->CheckMenuItem( ID_EQUALIZER_STATE_18BAND, MF_CHECKED );
	else if ( band_state == MUSIK_EQUALIZER_CTRL_6BANDS )
		popup_menu->CheckMenuItem( ID_EQUALIZER_STATE_6BAND, MF_CHECKED );
	else
		ASSERT( 0 );

	popup_menu->TrackPopupMenu( 0, pos.x, pos.y, this );
} 

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnEqualizerPresets()
{
	if ( !m_Presets )
	{
		m_Presets = new CmusikEqualizerSets( this );
		m_Presets->Create( IDD_EQUALIZER_SETS, this );
		m_Presets->ShowWindow( SW_SHOW );
	}	
}

///////////////////////////////////////////////////

LRESULT CmusikEqualizerBar::OnClosePresets( WPARAM wParam, LPARAM lParam )
{
	if ( m_Presets )
	{
		delete m_Presets;
		m_Presets = NULL;
	}

	return 0L;
}

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnSize(UINT nType, int cx, int cy)
{
	CSizingControlBar::OnSize(nType, cx, cy);

	CRect rcClient;
	GetClientRect( &rcClient );

	m_wndChild->MoveWindow( rcClient );
}

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnEqualizerState18band()
{
	GetCtrl()->SetBandState( MUSIK_EQUALIZER_CTRL_18BANDS );
	GetCtrl()->LayoutNewState();
	GetCtrl()->DisplayChanged();

	musikCube::g_Prefs->SetEqualizerBandState( MUSIK_EQUALIZER_CTRL_18BANDS );
}

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnEqualizerState6band()
{
	GetCtrl()->SetBandState( MUSIK_EQUALIZER_CTRL_6BANDS );
	GetCtrl()->LayoutNewState();
	GetCtrl()->DisplayChanged();

	musikCube::g_Prefs->SetEqualizerBandState( MUSIK_EQUALIZER_CTRL_6BANDS );
}

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnEqualizerSetAsDefault()
{
	GetCtrl()->SetAsDefault();
}

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnEqualizerResetdefault()
{
	musikCore::EQSettings settings;
	GetCtrl()->ResetDefault();
}

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnEqualizerAutomaticallySave()
{
	if ( musikCube::g_Prefs->EqualizerUseGlobal() )
		musikCube::g_Prefs->SetEqualizerUseGlobal( false );
	else
		musikCube::g_Prefs->SetEqualizerUseGlobal( true );

    musikCube::g_Player->UseGlobalEQSettings( musikCube::g_Prefs->EqualizerUseGlobal() );
}

///////////////////////////////////////////////////

void CmusikEqualizerBar::OnEqualizerCloseView()
{
	CFrameWnd* parent = (CFrameWnd*)GetParent();
	parent->ShowControlBar( this, FALSE, FALSE );
}

///////////////////////////////////////////////////

CmusikEQSlider::CmusikEQSlider()
: CmusikTrackCtrl()
{
    m_RealtimeUpdate = false;
}

///////////////////////////////////////////////////

void CmusikEQSlider::OnPosChanged()
{
    /*
    if ( musikCube::g_Player->IsPlaying() && musikCube::g_Prefs->IsEqualizerEnabled() )
    {
        if ( m_Parent )
            m_Parent->SendEQToPlayer();
    }
    */
}

///////////////////////////////////////////////////

// CmusikEqualizerCtrl

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikEqualizerCtrl, CWnd)

///////////////////////////////////////////////////

int WM_BANDCHANGE = RegisterWindowMessage( _T( "TRACKDRAGFINISH" ) );

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::SendEQToPlayer()
{
	musikCore::EQSettings& settings = musikCube::g_Player->GetEQSettings();
	BandsToEQSettings( &settings );

	if ( musikCube::g_Prefs->EqualizerUseGlobal() )
		musikCube::g_MainLibrary->UpdateDefaultEqualizer( settings );

	musikCube::g_Player->UpdateEQSettings( true, false );
}

///////////////////////////////////////////////////

CmusikEqualizerCtrl::CmusikEqualizerCtrl()
{
    for ( int i = 0; i < 19; i++ )
        m_LeftBands[i].m_Parent = this;

	m_BandState = MUSIK_EQUALIZER_CTRL_18BANDS;
	m_FullRange = 96;
}

///////////////////////////////////////////////////

CmusikEqualizerCtrl::~CmusikEqualizerCtrl()
{
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikEqualizerCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(WM_BANDCHANGE, OnBandChange)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	CMemDC memDC(&dc, &rect);
	
	CRect clip;
	memDC.GetClipBox(&clip);
	memDC.FillSolidRect( clip, musikCube::g_Prefs->GetBtnFaceColor() );

	DefWindowProc( WM_PAINT, (WPARAM)memDC->m_hDC, (LPARAM)0 );
}

///////////////////////////////////////////////////

BOOL CmusikEqualizerCtrl::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::OnAlwaysOnTop()
{
	::SetWindowPos( m_Tips.GetSafeHwnd(), 
		HWND_TOPMOST, 
		0, 
		0, 
		0, 
		0, 
		NULL );
}

///////////////////////////////////////////////////

int CmusikEqualizerCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 
	// small font
	//
    CDC* pDC = GetDC();
    int ppi = pDC->GetDeviceCaps( LOGPIXELSX );
    int pointsize = MulDiv( 65, 96, ppi );
    m_Font.CreatePointFont( pointsize, _T( "" ) );
    ReleaseDC( pDC );

	CString sTemp;
	m_Tips.Create( this, TTS_ALWAYSTIP );

	//
	// bands
	//
	for ( size_t i = 0; i < 18; i++ )
	{
		m_LeftBands[i].Create( WS_CHILD | WS_VISIBLE | TBS_VERT, CRect( 0, 0, 0, 0 ), this, 123 );
		m_LeftBands[i].SetRange( 0, m_FullRange );
		m_LeftBands[i].SetPos( m_FullRange / 2 );

        m_Labels[i].Create(
			CString( IntToString( musikCore::GetEQFreqs()[i] ).c_str() ),
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            CRect( 0, 0, 0, 0 ),
            this,
            123 );
        m_Labels[i].SetFont( &m_Font );

		sTemp.Format( _T( "left: %d hz" ), musikCore::GetEQFreqs()[i] );
		m_Tips.AddTool( &m_LeftBands[i], sTemp );
	}	

	//
	// preamp
	//
	m_LeftBands[18].Create( WS_CHILD | WS_VISIBLE | TBS_VERT, CRect( 0, 0, 0, 0 ), this, 123 );
	m_LeftBands[18].SetRange( 0, m_FullRange );
	m_LeftBands[18].SetPos( musikCube::g_Prefs->GetEqualizerMultiplier() );

    m_Labels[18].Create(
        _T( "pre" ),
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        CRect( 0, 0, 0, 0 ),
        this,
        123 );
    m_Labels[18].SetFont( &m_Font );

	// makes sure the tooltips are always displayed, even
	// if the main window is specified as always on top
	::SetWindowPos( m_Tips.GetSafeHwnd(), 
		HWND_TOPMOST, 
		0, 
		0, 
		0, 
		0, 
		NULL );

	LoadDefault();

    m_BandState = musikCube::g_Prefs->GetEqualizerBandState();
    LayoutNewState();
	SetBandsFrom( musikCube::g_Player->GetEQSettings() );

	return 0;
}

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	int width_remaining = ( ( cx - ( 16 * ( GetBandState() + 1 ) ) ) / ( GetBandState() + 1 ) );

	CRect curr;
	int space;
	if ( GetBandState() == MUSIK_EQUALIZER_CTRL_18BANDS )
	{
        // preamp
        int i = 18;
        space = ( width_remaining / 2 );
		curr.top = 12;
		curr.bottom = cy;
		curr.left = space;
		curr.right = curr.left + 16;
		m_LeftBands[18].MoveWindow( curr );
        curr.left = curr.left - 8;
        curr.top = 2;
        curr.bottom = 12;
        curr.right = curr.right + 6;
        m_Labels[18].MoveWindow( curr );

        int j = 0;
		for ( int i = 0; i < 18; i++ )
		{
            j++;
			space = ( width_remaining / 2 ) + ( j * width_remaining );

			curr.top = 12;
			curr.bottom = cy;
			curr.left = ( j * 16 ) + space;
			curr.right = curr.left + 16;

			m_LeftBands[i].MoveWindow( curr );

            curr.left = curr.left - 8;
            curr.top = 2;
            curr.bottom = 12;
            curr.right = curr.right + 6;
            m_Labels[i].MoveWindow( curr );
		}
	}

	else if ( GetBandState() == MUSIK_EQUALIZER_CTRL_6BANDS )
	{
		int band_at;
		for ( size_t i = 0; i < 7; i++ )
		{
			switch( i )
			{
			case 0:
				band_at = 18;
				break;
            case 1:
				band_at = 0;
				break;
			case 2:
				band_at = 3;
				break;
			case 3:
				band_at = 7;
				break;
			case 4: 
				band_at = 10;
				break;
			case 5:
				band_at = 14;
				break;
			case 6:
				band_at = 17;
				break;
			}

			space = ( width_remaining / 2 ) + ( i * width_remaining );

			curr.top = 12;
			curr.bottom = cy;
			curr.left = ( ( i ) * 16 ) + space;
			curr.right = curr.left + 16;

			m_LeftBands[band_at].MoveWindow( curr );

            curr.left = curr.left - 8;
            curr.top = 2;
            curr.bottom = 12;
            curr.right = curr.right + 6;
            m_Labels[band_at].MoveWindow( curr );
		}
	}
}

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::LayoutNewState()
{
	CRect rcClient;
	GetClientRect( &rcClient );
	OnSize( NULL, rcClient.Width(), rcClient.Height() );

	if ( GetBandState() == MUSIK_EQUALIZER_CTRL_18BANDS )
	{
		SetBandsFrom( musikCube::g_Player->GetEQSettings() );	// we may have interpolated, so reload
		for ( size_t i = 0; i < 18; i++ )
        {
			m_LeftBands[i].ShowWindow( SW_SHOW );
            m_Labels[i].ShowWindow( SW_SHOW );
        }
	}

	else if ( GetBandState() == MUSIK_EQUALIZER_CTRL_6BANDS )
	{
		m_LeftBands[0].ShowWindow( SW_SHOW );
		m_LeftBands[1].ShowWindow( SW_HIDE );
		m_LeftBands[2].ShowWindow( SW_HIDE );
		m_LeftBands[3].ShowWindow( SW_SHOW );
		m_LeftBands[4].ShowWindow( SW_HIDE );
		m_LeftBands[5].ShowWindow( SW_HIDE );
		m_LeftBands[6].ShowWindow( SW_HIDE );
		m_LeftBands[7].ShowWindow( SW_SHOW );
		m_LeftBands[8].ShowWindow( SW_HIDE );
		m_LeftBands[9].ShowWindow( SW_HIDE );
		m_LeftBands[10].ShowWindow( SW_SHOW );
		m_LeftBands[11].ShowWindow( SW_HIDE );
		m_LeftBands[12].ShowWindow( SW_HIDE );
		m_LeftBands[13].ShowWindow( SW_HIDE );
		m_LeftBands[14].ShowWindow( SW_SHOW );
		m_LeftBands[15].ShowWindow( SW_HIDE );
		m_LeftBands[16].ShowWindow( SW_HIDE );
		m_LeftBands[17].ShowWindow( SW_SHOW );

		m_Labels[0].ShowWindow( SW_SHOW );
		m_Labels[1].ShowWindow( SW_HIDE );
		m_Labels[2].ShowWindow( SW_HIDE );
		m_Labels[3].ShowWindow( SW_SHOW );
		m_Labels[4].ShowWindow( SW_HIDE );
		m_Labels[5].ShowWindow( SW_HIDE );
		m_Labels[6].ShowWindow( SW_HIDE );
		m_Labels[7].ShowWindow( SW_SHOW );
		m_Labels[8].ShowWindow( SW_HIDE );
		m_Labels[9].ShowWindow( SW_HIDE );
		m_Labels[10].ShowWindow( SW_SHOW );
		m_Labels[11].ShowWindow( SW_HIDE );
		m_Labels[12].ShowWindow( SW_HIDE );
		m_Labels[13].ShowWindow( SW_HIDE );
		m_Labels[14].ShowWindow( SW_SHOW );
		m_Labels[15].ShowWindow( SW_HIDE );
		m_Labels[16].ShowWindow( SW_HIDE );
		m_Labels[17].ShowWindow( SW_SHOW );
	}

	m_LeftBands[18].ShowWindow( SW_SHOW );
	m_Labels[18].ShowWindow( SW_SHOW );

	for ( size_t i = 0; i < 19; i++ )
	{
		m_LeftBands[i].RedrawWindow();
		m_Labels[i].RedrawWindow();
	}

	Invalidate();
}

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::SetBandState( int state )
{
	m_BandState = state;
}

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::LoadCurrSong()
{
	if ( musikCube::g_Player->IsPlaying() && musikCube::g_Player->IsEqualizerEnabled() )
		SetBandsFrom( musikCube::g_Player->GetEQSettings() );
}


///////////////////////////////////////////////////

void CmusikEqualizerCtrl::LoadDefault()
{
	if ( musikCube::g_Player->IsEqualizerEnabled() )
	{
		musikCube::g_MainLibrary->GetDefaultEqualizer( musikCube::g_Player->GetEQSettings() );
		musikCube::g_Player->UpdateEQSettings();
		SetBandsFrom( musikCube::g_Player->GetEQSettings() );
	}
}

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::SetBandsFrom( const musikCore::EQSettings& settings )
{
	int left = 0;
	for ( size_t i = 0; i < 18; i++ )
		m_LeftBands[i].SetPos( settings.m_Left[i] );

	Invalidate();
}

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::BandsToEQSettings( musikCore::EQSettings* settings )
{
	if ( !settings )
		return;

	float left_chan[18];

	for ( size_t i = 0; i < 18; i++ )
		left_chan[i] = m_LeftBands[i].GetPos();

	// must interpolate
	if ( GetBandState() == MUSIK_EQUALIZER_CTRL_6BANDS )
	{
		float chan_set_l[6];

		chan_set_l[0] = (float)m_LeftBands[0].GetPos();
		chan_set_l[1] = (float)m_LeftBands[3].GetPos();
		chan_set_l[2] = (float)m_LeftBands[7].GetPos();
		chan_set_l[3] = (float)m_LeftBands[10].GetPos();
		chan_set_l[4] = (float)m_LeftBands[14].GetPos();
		chan_set_l[5] = (float)m_LeftBands[17].GetPos();
		
		// interpolate the 12 missing bands
		left_chan[0]	= chan_set_l[0];
		left_chan[1]	= left_chan[0]  + ( ( chan_set_l[1] - chan_set_l[0] ) / 4.0f );
		left_chan[2]	= left_chan[1]  + ( ( chan_set_l[1] - chan_set_l[0] ) / 4.0f );
		left_chan[3]	= chan_set_l[1];
		left_chan[4]	= left_chan[3]  + ( ( chan_set_l[2] - chan_set_l[1] ) / 6.0f );
		left_chan[5]	= left_chan[4]  + ( ( chan_set_l[2] - chan_set_l[1] ) / 6.0f );
		left_chan[6]	= left_chan[5]  + ( ( chan_set_l[2] - chan_set_l[1] ) / 6.0f );
		left_chan[7]	= chan_set_l[2];					
		left_chan[8]	= left_chan[7]  + ( ( chan_set_l[3] - chan_set_l[2] ) / 4.0f );		
		left_chan[9]	= left_chan[8]  + ( ( chan_set_l[3] - chan_set_l[2] ) / 4.0f );							
		left_chan[10]	= chan_set_l[3];
		left_chan[11]	= left_chan[10] + ( ( chan_set_l[4] - chan_set_l[3] ) / 6.0f );
		left_chan[12]	= left_chan[11] + ( ( chan_set_l[4] - chan_set_l[3] ) / 6.0f );
		left_chan[13]	= left_chan[12] + ( ( chan_set_l[4] - chan_set_l[3] ) / 6.0f );
		left_chan[14]	= chan_set_l[4];
		left_chan[15]	= left_chan[14] + ( ( chan_set_l[5] - chan_set_l[4] ) / 4.0f );
		left_chan[16]	= left_chan[15] + ( ( chan_set_l[5] - chan_set_l[4] ) / 4.0f );
		left_chan[17]	= chan_set_l[5];
	}

	settings->Set( musikCore::MUSIK_EQ_SETTINGS_LEFT_BAND, left_chan );
	settings->Set( musikCore::MUSIK_EQ_SETTINGS_RIGHT_BAND, left_chan );

    musikCore::SetEQPreampLevel( m_LeftBands[18].GetPos() );
    musikCube::g_Prefs->SetEqualizerMultiplier( m_LeftBands[18].GetPos() );
}

///////////////////////////////////////////////////

LRESULT CmusikEqualizerCtrl::OnBandChange( WPARAM wParam, LPARAM lParam )
{
    SendEQToPlayer();
	return 0L;
}

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::SetAsDefault()
{
	musikCore::EQSettings curr_eq;
	BandsToEQSettings( &curr_eq );
	musikCube::g_MainLibrary->UpdateDefaultEqualizer( curr_eq );
}

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::DisplayChanged()
{
	if ( musikCube::g_Player->IsEqualizerEnabled() )
		SetBandsFrom( musikCube::g_Player->GetEQSettings() );
}

///////////////////////////////////////////////////

void CmusikEqualizerCtrl::ResetDefault()
{
	// get default
	musikCore::EQSettings reset;
	SetBandsFrom( reset );

	// send to player
	SendEQToPlayer();
}

///////////////////////////////////////////////////

BOOL CmusikEqualizerCtrl::PreTranslateMessage(MSG* pMsg)
{
	m_Tips.RelayEvent( pMsg );
	return CWnd::PreTranslateMessage(pMsg);
}

///////////////////////////////////////////////////


