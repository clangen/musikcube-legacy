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
//   CmusikPlaylistCtrl
//
// Filename(s): 
//
//   musikPlaylistCtrl.h, musikPlaylistCtrl.cpp
//
// Information:
//
//   musikCube's playlist control, based on a CmusikListCtrl
// 
// Usage: 
//
//   The control is virtual and fairly specialized, read
//   the header file for more information.
//
// Some code adapted from:
//
//   Michael Dunn: http://www.codeproject.com/shell/explorerdragdrop.asp?target=drag%7C%2Bdrop
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "musikCube.h"
#include "musikPlaylistView.h"
#include "musikPlaylistCtrl.h"
#include "musikSourcesCtrl.h"
#include "musikFileDrop.h"
#include "musikBatchTagDlg.h"

#include "MEMDC.H"
#include ".\musikplaylistctrl.h"

///////////////////////////////////////////////////

#pragma warning( disable : 4244 )

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC( CmusikPlaylistCtrl, CWnd )

///////////////////////////////////////////////////

// messages that we can receive

int WM_TAGPROPERTIESDESTROY = RegisterWindowMessage( _T( "TAGPROPERTIESDESTROY" ) );
int WM_TAGNEXT = RegisterWindowMessage( _T( "TAGNEXT" ) );
int WM_TAGPREV = RegisterWindowMessage( _T( "TAGPREV" ) );
int WM_TAGUPDATE = RegisterWindowMessage( _T( "TAGUPDATE" ) );

///////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CmusikPlaylistCtrl, CmusikListCtrl)
	// mfc message maps
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnLvnGetdispinfo)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
	ON_NOTIFY_REFLECT(LVN_ODCACHEHINT, OnLvnOdcachehint)
	ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
	ON_NOTIFY_REFLECT(LVN_ITEMACTIVATE, OnLvnItemActivate)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnLvnBegindrag)
	ON_NOTIFY_REFLECT(LVN_MARQUEEBEGIN, OnLvnMarqueeBegin)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnLvnColumnclick)
	ON_NOTIFY_REFLECT(LVN_BEGINRDRAG, OnLvnBeginrdrag)
	ON_NOTIFY(HDN_ENDTRACKA, 0, OnHdnEndtrack)
	ON_NOTIFY(HDN_ENDTRACKW, 0, OnHdnEndtrack)
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnLvnKeydown)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnLvnItemchanged)

	// menu
	ON_COMMAND(ID_PLAYLISTCOLUMNS_ARTIST, OnPlaylistcolumnsArtist)
	ON_COMMAND(ID_PLC_DELETE_FROMPLAYLIST, OnPlcDeleteFromplaylist)
	ON_COMMAND(ID_PLC_DELETE_FROMLIBRARY, OnPlcDeleteFromlibrary)
	ON_COMMAND(ID_PLC_DELETE_FROMCOMPUTER, OnPlcDeleteFromcomputer)
	ON_COMMAND(ID_PLAYLISTCONTEXTMENU_SHUFFLENOWPLAYING, OnPlaylistcontextmenuShufflenowplaying)
	ON_COMMAND(ID_PLAYLISTCONTEXTMENU_PROPERTIES, OnPlaylistcontextmenuProperties)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_ALBUM, OnPlaylistcolumnsAlbum)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_YEAR, OnPlaylistcolumnsYear)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_GENRE, OnPlaylistcolumnsGenre)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_TITLE, OnPlaylistcolumnsTitle)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_TRACKNUMBER, OnPlaylistcolumnsTracknumber)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_TIMEADDED, OnPlaylistcolumnsTimeadded)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_LASTPLAYED, OnPlaylistcolumnsLastplayed)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_FILESIZE, OnPlaylistcolumnsFilesize)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_FORMAT, OnPlaylistcolumnsFormat)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_DURATION, OnPlaylistcolumnsDuration)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_RATING, OnPlaylistcolumnsRating)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_TIMESPLAYED, OnPlaylistcolumnsTimesplayed)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_BITRATE, OnPlaylistcolumnsBitrate)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_FILENAME, OnPlaylistcolumnsFilename)
	ON_COMMAND(ID_PLAYLISTCONTEXTMENU_BATCH, OnPlaylistcontextmenuBatch)
	ON_COMMAND(ID_FILENAME_TAGFROMFILENAME, OnFilenameTagfromfilename)
	ON_COMMAND(ID_FILENAME_TAGTOFILENAME, OnFilenameTagtofilename)
	ON_COMMAND(ID_PLAYLISTCONTEXTMENU_TAGTOLOWERCASE, OnPlaylistcontextmenuTagtolowercase)
	ON_COMMAND(ID_TAG_CLEAN, OnPlaylistcontextmenuTagClean)
    ON_COMMAND(ID_PLAYLISTCONTEXTMENU_TAGTOUPPERCASE, OnPlaylistcontextmenuTagtouppercase)
	ON_COMMAND(ID_TAG_AUTOCAPITALIZE, OnTagAutocapitalize)
	ON_COMMAND(ID_RATE_N1, OnRateN1)
	ON_COMMAND(ID_RATE_UNRATED, OnRateUn)
	ON_COMMAND(ID_RATE_1, OnRate1)
	ON_COMMAND(ID_RATE_2, OnRate2)
	ON_COMMAND(ID_RATE_3, OnRate3)
	ON_COMMAND(ID_RATE_4, OnRate4)
	ON_COMMAND(ID_RATE_5, OnRate5)
	ON_COMMAND(ID_PLAYLISTCONTEXTMENU_RELOADTAGSFROMFILE, OnPlaylistcontextmenuReloadtagsfromfile)
	ON_COMMAND(ID_PLAYLISTCOLUMNS_COMMENTS, OnPlaylistcolumnsComments)
	ON_COMMAND(ID_PLAYLISTCONTEXTMENU_REMOVEWHITESPACE, OnPlaylistcontextmenuRemovewhitespace)
	ON_COMMAND(ID_PLAYLISTCONTEXTMENU_QUEUESELECTEDINNOWPLAYING, OnPlaylistcontextmenuQueueselectedinnowplaying)
	ON_COMMAND(ID_PLAYLISTCONTEXTMENU_REWRITETAGSTOFILE, OnPlaylistcontextmenuRewritetagstofile)

	// custom messages
	ON_REGISTERED_MESSAGE( WM_TAGPROPERTIESDESTROY, OnTagEditDestroy )
	ON_REGISTERED_MESSAGE( WM_TAGNEXT, OnTagNext )
	ON_REGISTERED_MESSAGE( WM_TAGPREV, OnTagPrev )
	ON_REGISTERED_MESSAGE( WM_TAGUPDATE, OnTagUpdate )
	ON_COMMAND(ID_PLAYLISTCONTEXTMENU_PLAYSELECTED, OnPlaylistcontextmenuPlayselected)
	ON_COMMAND(ID_PLAYLISTCONTEXTMENU_PLAYNEXT, OnPlaylistcontextmenuPlaynext)
	ON_COMMAND(ID_PLAYLISTCONTEXTMENU_REPLACENOWPLAYING, OnPlaylistcontextmenuReplacenowplaying)
	ON_COMMAND(ID_PLAYLISTCONTEXTMENU_SHOWINEXPLORER, OnPlaylistcontextmenuShowinexplorer)
	ON_COMMAND(ID_PLAYLISTCONTEXTMENU_REPLACENOWPLAYINGANDPLAY, OnPlaybackReplacenowplayingandplay)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

CmusikPlaylistCtrl::CmusikPlaylistCtrl( CFrameWnd* mainwnd, CmusikPlaylistDropTarget* pDropTarget, UINT dropid_l, UINT dropid_r )
{
    Init( mainwnd, dropid_l, dropid_r );

	m_DropTarget = pDropTarget;
}

///////////////////////////////////////////////////

CmusikPlaylistCtrl::CmusikPlaylistCtrl()
{
    Init( NULL, 0, 0 );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::Init( CFrameWnd* mainwnd, UINT dropid_l, UINT dropid_r )
{
	// core
	m_Playlist	= NULL;
    m_SongInfoArray = NULL;
    m_ViewType = PLAYLISTCTRL_NORMAL;

	// dialogs
	m_TagEdit = NULL;
	m_MaskDlg = NULL;

	// no sorting yet
	m_LastCol	= -1;

	// is a column being rearranged?
	m_DropTarget = NULL;
	m_Arranging = false;

	// dnd drop id
	m_DropID_L = dropid_l;
	m_DropID_R = dropid_r;

	// main window
	m_Parent = mainwnd;

	// info ctrl
	m_InfoCtrl = NULL;

	// misc
	m_DropArrange = false;
	m_Dragging = false;
	m_DropOverIndex = -1;
	m_PlaylistNeedsSave = false;
	m_SongInfoCache = new DynDspInfo( NULL );

	// dnd variables
	m_Marquee = false;

	// fonts and colors
	InitFonts();

	// get rating extent 
	GetRatingExtent();
}

///////////////////////////////////////////////////

CmusikPlaylistCtrl::~CmusikPlaylistCtrl()
{
	if ( m_Playlist && m_Playlist->GetType() != musikCore::MUSIK_PLAYLIST_TYPE_NOWPLAYING )
		delete m_Playlist;

	if ( m_TagEdit )
		delete m_TagEdit;

	if ( m_MaskDlg )
		delete m_MaskDlg;

    if ( m_SongInfoArray )
        delete m_SongInfoArray;

	delete m_SongInfoCache;

    if ( this->GetType() == PLAYLISTCTRL_NORMAL )
	{
		if ( m_DropTarget )
		{
			m_DropTarget->Revoke();
			delete m_DropTarget;
		}
	}
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::GetPlaylistCols()
{
	m_Cols = musikCube::g_Prefs->GetPlaylistOrder();
	m_Sizes = musikCube::g_Prefs->GetPlaylistSizes();
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::ResetColumns( bool update )
{
	SetRedraw( FALSE );
	GetHeaderCtrl()->SetRedraw( FALSE );

	DeleteAllItems();

	musikCore::Playlist* pList = GetPlaylist();
	m_Playlist = NULL;
	UpdateV();

	while ( DeleteColumn( 0 ) );

    GetPlaylistCols();

	bool rewrite = false;

	size_t times = m_Cols.size();
	for ( size_t i = 0; i < times; i++ )
	{
		if ( m_Cols.at( i ) > -1 )
		{
			// KLUDGE: cannot figure out where or when this happens,
			// which leads me to believe its an MFC bug...
			if ( m_Sizes.at( i ) > 6000 )
			{
				cout << _T( "COLUMN MESSED UP!\n" );
				m_Sizes.at( i ) = 100;
			}

			InsertColumn( (int)i, musikCube::g_Library->GetSongFieldStr( m_Cols.at( i ) ) );
			SetColumnWidth( (int)i, m_Sizes.at( i ) );
		}
		else
		{
			m_Cols.erase( m_Cols.begin() + i );
			m_Sizes.erase( m_Sizes.begin() + i );
			
			i--;
			times--;

			rewrite = true;
		}
	}

	if ( rewrite )
	{
		musikCube::g_Prefs->SetPlaylistOrder( m_Cols );
		musikCube::g_Prefs->SetPlaylistSizes( m_Sizes );
	}

	SetRedraw( TRUE );
	GetHeaderCtrl()->SetRedraw( TRUE );

	m_Playlist = pList;

	if ( update )
		UpdateV();
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::SaveColumns()
{
	musikCore::IntArray last_order;
	musikCore::IntArray last_sizes;

	wchar_t sCol[256];

	LVCOLUMN pColumn;
	pColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	pColumn.pszText = sCol;
	pColumn.cchTextMax = sizeof( sCol );

	int nColCount = GetHeaderCtrl()->GetItemCount();
	LPINT lpiCols = new int[nColCount];

	// get header order array
	GetHeaderCtrl()->GetOrderArray( lpiCols, nColCount );

	for (int i = 0; i < nColCount; i++)
	{
		GetColumn( lpiCols[i], &pColumn );
		last_order.push_back( musikCube::g_Library->GetSongFieldID( pColumn.pszText ) );
		last_sizes.push_back( pColumn.cx );
	} 
	delete[] lpiCols;

	musikCube::g_Prefs->SetPlaylistOrder( last_order );
	musikCube::g_Prefs->SetPlaylistSizes( last_sizes );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::UpdateItemCount()
{
	m_SongInfoCache->Set( 0, 0, true );
    SetItemCountEx( m_Playlist->GetCount(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::UpdateV( bool redraw, bool select_none )
{
	if ( m_Playlist || m_SongInfoArray )
	{
		if ( select_none )
			SetItemState( -1, 0, LVIS_SELECTED );

		int nPos = GetScrollPos( SB_VERT );

		SetRedraw( false );
        UpdateItemCount();

		SetScrollPos( SB_VERT, nPos, false );

		if ( redraw )
		{
			CRect rcClient;
			GetClientRect( &rcClient );
			SetRedraw( true );
			RedrawWindow( rcClient );
		}
	}
}

///////////////////////////////////////////////////

int CmusikPlaylistCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if ( CmusikListCtrl::OnCreate( lpCreateStruct ) == -1 )
		return -1;

	if ( m_DropTarget )
	{
		m_DropTarget->m_pList = this;
		m_DropTarget->Register( this );
	}

    ResetColumns( false );

	return 0;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnDestroy()
{
    if ( m_ViewType == PLAYLISTCTRL_NORMAL )
	    SaveColumns();

	CmusikListCtrl::OnDestroy();
}

///////////////////////////////////////////////////

musikCore::String CmusikPlaylistCtrl::GetItemAt( int nItem, int sub_type )
{
    return m_SongInfoCache->GetValue( nItem - m_SongInfoCache->GetFirst(), sub_type );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	LV_ITEM* pItem = &( pDispInfo->item );

	// only need to worry about item text
	if ( pItem->mask & LVIF_TEXT )
	{
		int nItem = pItem->iItem;
		int nSub  = pItem->iSubItem;
		int nType = musikCube::g_Prefs->GetPlaylistCol( nSub );

		pItem->mask |= LVIF_DI_SETITEM;

		int sub_type = m_Cols.at( nSub );
        musikCore::String sValue = GetItemAt( nItem, sub_type );  

        switch( sub_type )
        {
        case musikCore::MUSIK_LIBRARY_TYPE_RATING:
            {
            	int nRating = StoI( sValue.GetBuffer() );
                sValue = GetRating( nRating );
            }
            break;

        case musikCore::MUSIK_LIBRARY_TYPE_DURATION:
            {
                sValue = GetTimeStr( nItem );
            }
            break;

        case musikCore::MUSIK_LIBRARY_TYPE_TRACKNUM:
        case musikCore::MUSIK_LIBRARY_TYPE_YEAR:
            {
				if ( sValue == _T( "0" ) )
					sValue = _T( "" );
            }
            break;

        case musikCore::MUSIK_LIBRARY_TYPE_TIMESPLAYED:
            {
				if ( sValue == _T( "0" ) )
					sValue = _T( "never" );
            }
            break;

        case musikCore::MUSIK_LIBRARY_TYPE_LASTPLAYED:
            {
				if ( sValue[0] == '-' )
					sValue = _T( "" );
            }
            break;

        case musikCore::MUSIK_LIBRARY_TYPE_FILESIZE:
            {
				float fval = (float)StoI( sValue.GetBuffer( NULL ) );

				fval /= ( 1024.0 * 1024.0 );
				sValue.Format( _T( "%.2f MB" ), fval );
            }
            break;

#if 0
		case musikCore::MUSIK_LIBRARY_TYPE_FILENAME:
			{
				wchar_t realfn[512];
				if ( GetLongPathName( sValue, realfn, 512 ) <= 0 )
				{
					sValue = _T( "File not found" );
				}
				else
				{
					sValue = realfn;
				}
			}
#endif

        default:
            break;

        }

		// copy the buffer of the correct display string
		// to the current LV_ITEM 
		if ( sValue.GetLength() > pItem->cchTextMax )
		{
			sValue = sValue.Left( pItem->cchTextMax - 1 );
		}
		wcscpy( pItem->pszText, sValue.c_str() );
	}

	*pResult = 0;
}

///////////////////////////////////////////////////

BOOL CmusikPlaylistCtrl::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////

int CmusikPlaylistCtrl::GetSongID( int at )
{
    return m_Playlist->GetSongID( at );
}

///////////////////////////////////////////////////


void CmusikPlaylistCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

    // Take the default processing unless we set this to something else below.
    *pResult = CDRF_DODEFAULT;

	COLORREF clrStripe = GetModColor( musikCube::g_Prefs->GetListCtrlColor(), 10 );
	
    // First thing - check the draw stage. If it's the control's prepaint
    // stage, then tell Windows we want messages for every item.
    if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
	{
        *pResult = CDRF_NOTIFYITEMDRAW;
		return;
	}
	
	// we got a paint item event, ignore it, we want to draw the 
	// sub items one by one.
	else if ( pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT  )
	{
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
		return;
	}	

	// draw the sub items
	else if ( pLVCD->nmcd.dwDrawStage == ( CDDS_ITEMPREPAINT | CDDS_SUBITEM ) )
	{
		if (  
               ( 
                 ( m_Playlist && pLVCD->nmcd.dwItemSpec < m_Playlist->GetCount() ) || 
                 ( m_SongInfoArray && pLVCD->nmcd.dwItemSpec < m_SongInfoArray->size() ) 
               ) && !m_Arranging 
            
           )
		{
            int songid = GetSongID( pLVCD->nmcd.dwItemSpec );
            int realsongid = musikCube::g_Player->GetPlaying()->GetID();
			CDC *pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
			
			bool is_playing = false;
		
            // color for currently playing song if the current
			// playlist is NOT now playing...
			if ( musikCube::g_Player->IsPlaying() )
			{
				// if we are looking at the now playing playlist
				// we may have duplicate songs... so go ahead and only
				// highlight the ones thats playing
				if ( m_Playlist == musikCube::g_Player->GetPlaylist() )
				{
					if ( pLVCD->nmcd.dwItemSpec == musikCube::g_Player->GetIndex() )
					{
						is_playing = true;
					}
				}
				else if 
					( 
						( m_ViewType == PLAYLISTCTRL_NORMAL && songid == musikCube::g_Player->GetPlaying()->GetID() ) || 
						( m_ViewType == PLAYLISTCTRL_CDRIPPER && musikCube::g_Player->GetIndex() == pLVCD->nmcd.dwItemSpec && realsongid == -1 )
					)
				{
					if ( m_ViewType == PLAYLISTCTRL_NORMAL )
					{
						if ( musikCube::g_Player->GetPlaying()->GetLibrary() == musikCube::g_Library )
						{
							is_playing = true;
						}
					}
					else if ( m_ViewType == PLAYLISTCTRL_CDRIPPER )
					{
						if ( musikCube::g_Player->GetDiscDrive() == dynamic_cast<CmusikPlaylistCDCtrl*>( this )->GetDiscDrive() )
						{
							is_playing = true;
						}
					}
				}
			}

			pDC->SelectObject( m_ItemFont );

			// currently playing
			if ( is_playing )
			{
				pDC->SelectObject( m_BoldFont );

				pLVCD->clrText = musikCube::g_Prefs->GetHilightTextColor();
				pLVCD->clrTextBk = ::ShadeColor( 
					musikCube::g_Prefs->GetHilightColor(),
					40
				);
			}
			// drag and drop over
			else if ( m_DropOverIndex == pLVCD->nmcd.dwItemSpec )
			{
				pLVCD->clrText = musikCube::g_Prefs->GetListCtrlTextColor();
				pLVCD->clrTextBk = ::ShadeColor( 
					musikCube::g_Prefs->GetBtnFaceColor(),
					20
				);
			}
			// selected
			else if ( GetItemState( pLVCD->nmcd.dwItemSpec, LVIS_SELECTED ) )
			{
				pLVCD->clrText = musikCube::g_Prefs->GetHilightTextColor();
				pLVCD->clrTextBk = musikCube::g_Prefs->GetHilightColor();
			}
			// regular item
			else
			{
				if ( pLVCD->nmcd.dwItemSpec % 2 != 0 )
				{
					// even color
					pLVCD->clrTextBk = clrStripe;
				}
				else
				{
					// odd color
					pLVCD->clrTextBk = musikCube::g_Prefs->GetListCtrlColor();
				}
				//
				pLVCD->clrText = musikCube::g_Prefs->GetListCtrlTextColor();
			}

			int nSubType = musikCube::g_Prefs->GetPlaylistCol( pLVCD->iSubItem );
			if ( nSubType == musikCore::MUSIK_LIBRARY_TYPE_RATING )
			{
				pDC->SelectObject( m_StarFont );
			}		

			*pResult = CDRF_NEWFONT;
			return;
		}
	}
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::InitFonts()
{
	m_ItemFont.CreateStockObject( DEFAULT_GUI_FONT );
	m_StarFont.CreatePointFont( 100, _T( "Musik" ) );

	LOGFONT pBoldFont;
	m_ItemFont.GetLogFont( &pBoldFont );
	pBoldFont.lfWeight = FW_BOLD;
	m_BoldFont.CreateFontIndirect( &pBoldFont );  
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnLvnOdcachehint(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCACHEHINT pCacheHint = reinterpret_cast<LPNMLVCACHEHINT>(pNMHDR);

    if ( m_ViewType == PLAYLISTCTRL_NORMAL )
	    m_SongInfoCache->Set( pCacheHint->iFrom, pCacheHint->iTo, false );

	*pResult = 0;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	CMemDC memDC(&dc, &rect);
	
	CRect headerRect;
	GetDlgItem(0)->GetWindowRect(&headerRect);

    ScreenToClient(&headerRect);
	dc.ExcludeClipRect(&headerRect);
	   
	CRect clip;
	memDC.GetClipBox(&clip);
	memDC.FillSolidRect( clip, musikCube::g_Prefs->GetListCtrlColor() );
	   
	DefWindowProc(WM_PAINT, (WPARAM)memDC->m_hDC, (LPARAM)0);
}

///////////////////////////////////////////////////

CString CmusikPlaylistCtrl::GetTimeStr( int item )
{
	int time_ms = StoI( m_SongInfoCache->GetValue( item - m_SongInfoCache->GetFirst(), musikCore::MUSIK_LIBRARY_TYPE_DURATION ).c_str() );
	return (CString)musikCube::g_Player->GetTimeStr( time_ms );	
}

///////////////////////////////////////////////////

CString CmusikPlaylistCtrl::GetRating( int nRating )
{
	CString sRating;
	switch ( nRating )
	{
	case -1:
		sRating = _T( "" );
		break;
	case 1:
		sRating = _T( "-,,,," );
		break;
	case 2:
		sRating = _T( "--,,," );
		break;
	case 3:
		sRating = _T( "---,," );
		break;
	case 4:
		sRating = _T( "----," );
		break;
	case 5:
        sRating = _T( "-----" );
		break;
	default:
		sRating = _T( ",,,,," );
		break;
	}

	return sRating;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// get cursor pos at time of click in relation
	// to the client
	CPoint ptCurr;
	GetCursorPos( &ptCurr );
	ScreenToClient( &ptCurr );

	// see which item was clicked
	LVHITTESTINFO hit_test;
	hit_test.pt.y = ptCurr.y;
	hit_test.pt.x = 0;
	SubItemHitTest( &hit_test );
	int nItem = hit_test.iItem;

	if ( nItem > -1 )
	{
		int rating_pos = -1;
		for ( size_t i = 0; i < musikCube::g_Prefs->GetPlaylistColCount(); i++ )
		{
			if ( musikCube::g_Prefs->GetPlaylistCol( i ) == musikCore::MUSIK_LIBRARY_TYPE_RATING )
				rating_pos = (int)i;
		}

		if ( rating_pos == -1 )
			return;

		CRect rcRating;
		GetSubItemRect( nItem, rating_pos, LVIR_LABEL, rcRating );

		if ( rcRating.PtInRect( ptCurr ) )
		{
			// if an item is a sub item, there are two
			// spaces in front of the item text -- so
			// shift point that far in the X axis
			int nOffset;
			if ( rating_pos > 0 )
				nOffset = m_TwoSpace;
			else
				nOffset = 2;

			// width of the column and the
			// recalculated click locations...
			int col_width = GetColumnWidth( rating_pos );
			int clk_loc = ( ptCurr.x - rcRating.left );

			int nRating;
			if ( clk_loc <= nOffset )
				nRating = 0;
			else if ( clk_loc >= m_RatingExtent + nOffset )
			{
				if ( clk_loc < m_RatingExtent + nOffset + 12 )
				{
					*pResult = 0;
					return;
				}
					nRating = 5;
			}
				
			else 
			{
				nRating = ( clk_loc / ( m_RatingExtent / 5 ) );

				if ( rating_pos == 0 )
					nRating++;
			}

			musikCore::Song& song = m_Playlist->at( hit_test.iItem );
            song.GetLibrary()->SetSongRating( m_Playlist->GetSongID( hit_test.iItem ), nRating );	
			ResyncItem( m_Playlist->GetSongID( hit_test.iItem ), hit_test.iItem );

			if ( musikCube::g_Player->GetPlaying() )
				musikCube::g_Player->GetPlaying()->SetRating( ItoS( nRating ) );

		}
	}

	*pResult = 0;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::ResyncItem( size_t songid, int item )
{
	if ( m_SongInfoCache->ResyncItem( songid ) )
	{
		if ( item != -1 )
		{
			RedrawItems( item, item );
		}
		else
		{
			RedrawWindow();
		}
	}
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::SavePlaylist()
{
	// if the user wants to be prompted...
	if ( m_PlaylistNeedsSave && m_Playlist->GetType() == musikCore::MUSIK_PLAYLIST_TYPE_STANDARD )
	{
		if ( m_Playlist->GetPlaylistID() == -1 )
			return;

		musikCube::g_Library->RewriteStdPlaylist( *m_Playlist );
		m_PlaylistNeedsSave = false;
	}
}


///////////////////////////////////////////////////

void CmusikPlaylistCtrl::SetPlaylist( musikCore::Playlist* playlist )
{
    HideSortArrow();

	if ( playlist == NULL )
	{
		cout << _T( "Passed a NULL playlist\n" );
		return;
	}

	SetItemState( -1, 0, LVIS_SELECTED );
    m_ViewType = PLAYLISTCTRL_NORMAL;

	// delete any old playlist that is not a now
	// playing type...
	if ( m_Playlist && m_Playlist->GetType() != musikCore::MUSIK_PLAYLIST_TYPE_NOWPLAYING )
		delete m_Playlist;

	// set the playlist and cache it
	m_Playlist = playlist;

	if ( m_SongInfoCache )
		m_SongInfoCache->SetPlaylist( playlist );

	if ( m_InfoCtrl )
		m_InfoCtrl->UpdateInfo();	

	if ( m_TagEdit )
		m_TagEdit->UpdatePlaylist( m_Playlist );

	// we want all the plugins to see the active playlist
	for( size_t i = 0; i < musikCube::g_Plugins.size(); i++ )
		musikCube::g_Plugins.at( i ).SetVisiblePlaylist( m_Playlist );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::SetInfoCtrl( CmusikPlaylistInfoCtrl *ctrl )
{
	m_InfoCtrl = ctrl;
}

///////////////////////////////////////////////////

bool CmusikPlaylistCtrl::PlayItem( int n )
{
	if ( musikCube::g_Prefs->AddEntireToNP() )
		return PlayItem_ReplaceNP( n );

	return PlayItem_AddNP( n );
}

///////////////////////////////////////////////////

bool CmusikPlaylistCtrl::PlayItem_ReplaceNP( int n )
{
	if ( !m_Playlist || m_Playlist->size() < 1 )
		return false;

	if ( n == -1 )
	{
		POSITION pos = GetFirstSelectedItemPosition();
		n = GetNextSelectedItem ( pos );

        if ( n == -1 )
            n = 0;
    }	

	if ( musikCube::g_Player->GetPlaylist() != m_Playlist )
    {
		musikCube::g_Player->SetPlaylist( m_Playlist );
        musikCube::g_Player->SetLibrary( musikCube::g_Library );
    }

	musikCube::g_Player->Play( n, 0, musikCore::MUSIK_PLAYER_SONG_USER_ADVANCE, true );

	SetItemState( -1, 0, LVIS_SELECTED );

	int WM_SOURCESREFRESH = RegisterWindowMessage( _T( "SOURCESREFRESH" ) );
	AfxGetApp()->GetMainWnd()->SendMessage( WM_SOURCESREFRESH, NULL );

	return true;
}

///////////////////////////////////////////////////

bool CmusikPlaylistCtrl::PlayItem_AddNP( int n )
{
	if ( !m_Playlist )
		return false;

	// current view IS now playing, just play that song
	if ( m_Playlist == musikCube::g_Player->GetPlaylist() )
	{
		return musikCube::g_Player->Play( n );
	}

	if ( n == -1 )
	{
		POSITION pos = GetFirstSelectedItemPosition();
		n = GetNextSelectedItem ( pos );

		// no selection, so just start playing the first item
		// in the list. if no first item exists, return false.
		if ( n == -1 && m_Playlist->size() )
			n = 0;
		else
			return false;
	}	

	int insert_at = -1;

	// get activated item's songid
	musikCore::Song song = m_Playlist->GetSong( n );

	// this is a failsafe. we should never actually
	// get inside of this if() statement.
	if ( song.m_Library == NULL )
		song.SetLibrary( musikCube::g_Library );

	// add it and play it
	musikCube::g_Player->GetPlaylist()->Add( song );

	if ( !musikCube::g_Player->IsPlaying() )
		musikCube::g_Player->Play( 
			musikCube::g_Player->GetPlaylist()->size() - 1 );

	SetItemState( -1, 0, LVIS_SELECTED );

	return true;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = NULL;

	int item = pNMIA->iItem;

	PlayItem( item );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::BeginDrag( NMHDR* pNMHDR, bool right_button )
{
	// set cursor back to an arrow
	SetCursor( LoadCursor( NULL, IDC_ARROW ) );

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	COleDataSource datasrc;
	HGLOBAL        hgDrop;
	DROPFILES*     pDrop;
	CStringList    lsDraggedFiles;
	POSITION       pos;
	int            nSelItem;
	CString        sFile;
	UINT           uBuffSize = 0;
	TCHAR*         pszBuff;
	FORMATETC      etc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

    // For every selected item in the list, put the filename into lsDraggedFiles.
    pos = GetFirstSelectedItemPosition();
	while ( pos )
    {
        nSelItem = GetNextSelectedItem ( pos );
		sFile = m_Playlist->GetField( nSelItem, musikCore::MUSIK_LIBRARY_TYPE_FILENAME );
        
        lsDraggedFiles.AddTail ( sFile );

        // Calculate the # of chars required to hold this string.
        uBuffSize += lstrlen ( sFile ) + 1;
    }

	if ( !lsDraggedFiles.GetCount() )
		return;

    // Add 1 extra for the final null char, and the size of the DROPFILES struct.
    uBuffSize = sizeof(DROPFILES) + sizeof(TCHAR) * (uBuffSize + 1);

    // Allocate memory from the heap for the DROPFILES struct.
    hgDrop = GlobalAlloc ( GHND | GMEM_SHARE, uBuffSize );

    if ( !hgDrop )
        return;

    pDrop = (DROPFILES*) GlobalLock ( hgDrop );

    if ( !pDrop )
	{
		GlobalFree ( hgDrop );
		return;
	}

    // Fill in the DROPFILES struct.
    pDrop->pFiles = sizeof(DROPFILES);

	// If we're compiling for Unicode, set the Unicode flag in the struct to
	// indicate it contains Unicode strings.
	#ifdef _UNICODE
		pDrop->fWide = TRUE;
	#endif;

    // Copy all the filenames into memory after the end of the DROPFILES struct.
    pos = lsDraggedFiles.GetHeadPosition();
    pszBuff = (TCHAR*) (LPBYTE(pDrop) + sizeof(DROPFILES));

    while ( pos )
	{
        wcscpy( pszBuff, (LPCTSTR) lsDraggedFiles.GetNext ( pos ) );
        pszBuff = 1 + _tcschr ( pszBuff, '\0' );
	}

    GlobalUnlock ( hgDrop );

    // Put the data in the data source.
    datasrc.CacheGlobalData ( CF_HDROP, hgDrop, &etc );

    // Add in our own custom data, so we know that the drag originated from our 
    // window.  CMyDropTarget::DragEnter() checks for this custom format, and
    // doesn't allow the drop if it's present.  This is how we prevent the user
    // from dragging and then dropping in our own window.
    // The data will just be a dummy bool.
	HGLOBAL hgBool;

    hgBool = GlobalAlloc ( GHND | GMEM_SHARE, sizeof(bool) );

    if ( NULL == hgBool )
    {
        GlobalFree ( hgDrop );
        return;
    }

    // Put the data in the data coosource.
	if ( right_button )
		etc.cfFormat = m_DropID_R;
	else 
		etc.cfFormat = m_DropID_L;

	datasrc.CacheGlobalData ( right_button ? m_DropID_R : m_DropID_L, hgBool, &etc );

	// post a message to the main frame, letting
	// it know that drag and drop has started
	int WM_DRAGSTART = RegisterWindowMessage( _T( "DRAGSTART" ) );
	m_Parent->SendMessage( WM_DRAGSTART, NULL );

    // Start the drag 'n' drop!
	DROPEFFECT dwEffect = datasrc.DoDragDrop ( DROPEFFECT_COPY );

	int WM_DRAGEND = RegisterWindowMessage( _T( "DRAGEND"  ) );
	m_Parent->SendMessage( WM_DRAGEND, NULL );

    // If the DnD completed OK, we remove all of the dragged items from our
    // list.
    switch ( dwEffect )
    {
        case DROPEFFECT_COPY:
        case DROPEFFECT_MOVE:
        {
			// the copy completed successfully
			// do whatever we need to do here
			cout << _T( "DND from playlist completed successfully. The data has a new owner.\n" );
        }
        break;

        case DROPEFFECT_NONE:
        {
			// the copy completed successfully
			// on a windows nt machine.
			// do whatever we need to do here

            bool bDeletedAnything = false;
            for ( size_t i = GetNextItem ( -1, LVNI_SELECTED );	i != -1; i = GetNextItem ( i, LVNI_SELECTED ) )
            {
                CString sFilename = GetItemText ( nSelItem, 0 );

                if ( GetFileAttributes ( sFile ) == 0xFFFFFFFF  &&
                        GetLastError() == ERROR_FILE_NOT_FOUND )
                {
                    // We couldn't read the file's attributes, and GetLastError()
                    // says the file doesn't exist, so remove the corresponding 
                    // item from the list.
                    DeleteItem ( nSelItem );
                
                    nSelItem--;
                    bDeletedAnything = true;
                }
            }

            if ( ! bDeletedAnything )
            {
                // The DnD operation wasn't accepted, or was canceled, so we 
                // should call GlobalFree() to clean up.
                GlobalFree ( hgDrop );
                GlobalFree ( hgBool );

				cout << _T( "DND had a problem. We had to manually free the data.\n" );
			}
		}

        break;
	}
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
    if ( m_ViewType == PLAYLISTCTRL_NORMAL )
	    BeginDrag( pNMHDR, false );

	*pResult = 0;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnDragLeave()
{
	m_DropOverIndex = -1;
	this->Invalidate();
}

///////////////////////////////////////////////////

DROPEFFECT CmusikPlaylistCtrl::OnDragOver( CPoint point )
{	
	if ( m_DropArrange )
	{
		int intDropIndex = this->HitTest( point );

		if ( ( intDropIndex > -1 ) && ( intDropIndex != m_DropOverIndex ) ) 
		{
			m_DropOverIndex = intDropIndex;
			this->Invalidate();
		}

		return DROPEFFECT_MOVE;
	}

	return DROPEFFECT_COPY;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnDropFiles( HDROP hDropInfo )
{
	m_DropOverIndex = -1;

	// if the drag originated from ourself, then
	// we want to rearrange items, and nothing more.
    if ( m_ViewType != PLAYLISTCTRL_NORMAL )
	{
		return;
	}

	if ( m_DropArrange )
	{
		CPoint ptCursor;
		::GetCursorPos( &ptCursor );
		ScreenToClient( &ptCursor );
		
		int nPos = HitTest( ptCursor );

		// get first selected item.. if we can't
		// find one, return.
		int nFirstSel = GetFirstSelected();
		if ( nFirstSel == -1 )
			return;

		// item dragged onto itself
		if ( nPos == nFirstSel )
			return;

		// get selected
		musikCore::IntArray sel;
		GetSelectedItems( sel );
		
		musikCore::Playlist sel_songs;
		GetSelectedSongs( sel_songs );

		// remove selected items from their
		// current position
		DeleteItems( sel, false );
		sel.clear();

		// get the id of the currently playing
		// song, we'll find it once the drag is done
		// to assure the correct song is bold.
		int songid = musikCube::g_Player->GetPlaying()->GetID();

		// insert the items back to their new
		// position...
		InsertItems( sel_songs, nFirstSel, nPos );
		sel_songs.Clear();

		// grab the new index..
		musikCube::g_Player->FindNewIndex( songid );

		if ( m_Playlist->GetType() == musikCore::MUSIK_PLAYLIST_TYPE_STANDARD )
			m_PlaylistNeedsSave = true;

		m_DropArrange = false;
		RedrawWindow();
		return;
	}

	// see if we need to prompt the user for
	// which playlist to insert files to...
	int nRet = musikCube::g_Prefs->GetFileDropPrompt();

	if ( nRet == -1 )
	{
		CmusikFileDrop* pDlg = new CmusikFileDrop( this );
		nRet = pDlg->DoModal();
		delete pDlg;

		if ( nRet == MUSIK_FILEDROP_CANCEL )
		{
			return;
		}
	}

	// if we get here, the drag originated from 
	// somewhere else, such as explorer...
	size_t nNumFiles;
	TCHAR szNextFile [MAX_PATH];
	SHFILEINFO  rFileInfo;
	
	nNumFiles = DragQueryFile ( hDropInfo, -1, NULL, 0 );
	musikCore::StringArray* files = NULL;

	files = new musikCore::StringArray();

    bool is_dir = false;

	musikCore::String sTemp;
	for ( size_t i = 0; i < nNumFiles; i++ )
	{
		if ( DragQueryFile( hDropInfo, i, szNextFile, MAX_PATH ) > 0 )
		{
			// get the filetype. if its a directory
			// that was dropped, we'll want to 
			// recurse it and add all the supported 
			// media files...
			SHGetFileInfo( szNextFile, 0, &rFileInfo, sizeof( rFileInfo ), SHGFI_ATTRIBUTES );
			if ( rFileInfo.dwAttributes & SFGAO_FOLDER )
			{
				sTemp += szNextFile;
				sTemp += _T( "\\*.*||" );
                is_dir = true;
			}		

			// otherwise it was just a file... add it...
			else
				files->push_back( szNextFile );
		}
	}

	bool join_thread = false;
	if ( is_dir || files->size() > 0 )
	{
		musikCore::BatchAdd* params;

		if ( nRet == MUSIK_FILEDROP_ADDNOWPLAYING )
		{
			params = new musikCore::BatchAdd( 
				files, 
                is_dir ? sTemp : "",
				musikCube::g_Player->GetPlaylist(), 
				musikCube::g_Library, 
				musikCube::g_Player, 
				NULL, 
				0, 
				1, 
				1 
			);

			join_thread = true;
		}

		else if ( nRet == MUSIK_FILEDROP_ADDPLAYLIST )
		{
			params = new musikCore::BatchAdd( 
				files, 
                is_dir ? sTemp : _T( "" ),
				m_Playlist, 
				musikCube::g_Library, 
				NULL, 
				NULL, 
				1, 
				0, 
				1 
			);

			m_PlaylistNeedsSave = true;
			join_thread = true;
		}

		else if ( nRet == MUSIK_FILEDROP_ADDLIBRARY )
		{
			params = new musikCore::BatchAdd( 
				files, 
                is_dir ? sTemp : _T( "" ),
				musikCube::g_Player->GetPlaylist(), 
				musikCube::g_Library, 
				NULL, 
				NULL,
				1,
				0,
				1 
			);

			join_thread = false;
		}

		params->m_Functor = musikCube::g_Functor;
		musikCore::BatchAddTask* task = new musikCore::BatchAddTask;
		task->m_Params = params;

		if ( join_thread )
		{
			task->Start();
		}
		else
		{
			musikCube::g_Library->QueueTask( task );
		}
	}
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::GetSelectedItems( musikCore::IntArray& items )
{
	items.clear();

    POSITION pos = GetFirstSelectedItemPosition();
	while ( pos )
        items.push_back( GetNextSelectedItem ( pos ) );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::GetSelectedSongs( musikCore::Playlist& items )
{
	items.Clear();
	
	POSITION pos = GetFirstSelectedItemPosition();
	while ( pos )
        items.Add( m_Playlist->GetSong( GetNextSelectedItem( pos ) ) );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::GetSelectedSongs( musikCore::SongInfoArray& items )
{
	items.clear();

	musikCore::Song curr_song;
	musikCore::SongInfo curr_info;
	
	POSITION pos = GetFirstSelectedItemPosition();
	musikCube::g_Library->BeginTransaction();
	while ( pos )
	{
		curr_song = m_Playlist->GetSong( GetNextSelectedItem( pos ) );
		musikCube::g_Library->GetSongInfoFromID( curr_song.GetID(), curr_info );
		items.push_back( curr_info );
	}
	musikCube::g_Library->EndTransaction();
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::GetSelectedSongs( musikCore::SongArray& items )
{
	items.clear();

	musikCore::Song curr_song;

	POSITION pos = GetFirstSelectedItemPosition();
	musikCube::g_Library->BeginTransaction();
	while ( pos )
	{
		curr_song = m_Playlist->GetSong( GetNextSelectedItem( pos ) );
		items.push_back( curr_song );
	}
	musikCube::g_Library->EndTransaction();
}


///////////////////////////////////////////////////

int CmusikPlaylistCtrl::GetFirstSelected()
{
    POSITION pos = GetFirstSelectedItemPosition();
	int nRet = GetNextSelectedItem ( pos );
    
	return nRet;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::GetItemIDs( const musikCore::IntArray& items, musikCore::IntArray* target )
{
	target->clear();

	for ( size_t i = 0; i < items.size(); i++ )
		target->push_back( m_Playlist->GetSongID( items.at( i ) ) );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::DeleteItems( const musikCore::IntArray& items, bool update )
{
	if ( items.size() <= 0 )
		return;

	int nScrollPos = GetScrollPos( SB_VERT );

	musikCore::SongArray songs;
	musikCore::Song song;

	bool from_playlist = m_Playlist->GetType() == musikCore::MUSIK_PLAYLIST_TYPE_STANDARD ? true : false;
	for ( size_t i = 0; i < items.size(); i++ )
	{
		if ( musikCube::g_Player->GetPlaylist() == m_Playlist )
		{
			if ( musikCube::g_Player->m_Index >= (int)items.at( i ) - (int)i )
				musikCube::g_Player->m_Index--;
		}

		if ( from_playlist )
		{
			song.SetID( m_Playlist->at( items.at( i ) - i ).GetID() );
			songs.push_back( song );
		}

		m_Playlist->DeleteAt( items.at( i ) - i );
	}

	if ( from_playlist )
	{
		musikCube::g_Library->RemoveFromStdPlaylist( songs, musikCube::g_PlaylistInfo.m_ID );
	}

	if ( update )
	{
		if ( from_playlist )
		{
			int WM_SELBOXREQUESTUPDATE	= RegisterWindowMessage( _T( "SELBOXREQUESTUPDATE" ) );
			m_Parent->PostMessage( WM_SELBOXREQUESTUPDATE );
		}

		UpdateV();

		// deselect all, then select first...
		SetItemState( -1, 0, LVIS_SELECTED );
		if ( GetItemCount() > 0 )
		{
			if ( items.at( 0 ) > GetItemCount() )
				SetItemState( 0, LVIS_SELECTED, LVIS_SELECTED );
			else
				SetItemState( items.at( 0 ), LVIS_SELECTED, LVIS_SELECTED );
		}

		SetScrollPos( SB_VERT, nScrollPos );
	}

	if ( m_InfoCtrl )
		m_InfoCtrl->UpdateInfo();	
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::InsertItems( musikCore::Playlist& items, int firstsel, int at, bool update )
{
	if ( items.GetCount() <= 0 )
		return;

	int nScrollPos = GetScrollPos( SB_VERT );

	// see if we need to insert items above
	// or below the target... depends if we
	// are dragging up or down.
	if ( firstsel < at )
	{
		at -= ( items.GetCount() - 1 );
		at--;
		if ( at < -1 )
			at = -1;
	}

	m_Playlist->InsertBefore( items, at );

	if ( update )
	{
		UpdateV();

		// deselect all, then select the items that
		// were just rearranged...
		SetItemState( -1, 0, LVIS_SELECTED );
		if ( at == -1 )
			at = GetItemCount() - items.GetCount();
		for ( size_t i = at; i < at + items.GetCount(); i++ )
			SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );

		SetScrollPos( SB_VERT, nScrollPos );
	}

	if ( m_InfoCtrl )
		m_InfoCtrl->UpdateInfo();	
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnLvnMarqueeBegin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// returning non-zero ignores the message,
	// and marquee never appears.
	//*pResult = 1;

	*pResult = 0;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnDragColumn( int source, int dest )
{
	if ( source == dest )
		return;

	m_Arranging = true;

	musikCore::IntArray order = musikCube::g_Prefs->GetPlaylistOrder();
	musikCore::IntArray sizes = musikCube::g_Prefs->GetPlaylistSizes();

	order.insert( order.begin() + dest, order.at( source ) );
	sizes.insert( sizes.begin() + dest, sizes.at( source ) );

	if ( source > dest )
		source++;

	order.erase( order.begin() + source );
	sizes.erase( sizes.begin() + source );

	musikCube::g_Prefs->SetPlaylistOrder( order );
	musikCube::g_Prefs->SetPlaylistSizes( sizes );

	m_Arranging = false;

	//ResetColumns();
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::GetRatingExtent()
{
	int spc = 0;
	CSize szText;

	HDC	hMemDC	= NULL;
	hMemDC = CreateCompatibleDC( NULL );
	if ( hMemDC )
	{
		CDC* pDC = CDC::FromHandle( hMemDC );
		if ( pDC )
		{
			CFont* pOldFont = pDC->SelectObject( &m_StarFont );

			// list control always adds two spaces after
			// each string in a column, this must be
			// subtracted.
			szText = pDC->GetTextExtent( _T( "  " ) );
			m_TwoSpace = szText.cx;

			// supposed extent of the five stars
			szText = pDC->GetTextExtent( _T( ",,,,," ) );

			pOldFont = pDC->SelectObject( pOldFont );
		}
	}
	DeleteDC( hMemDC );

	m_RatingExtent = szText.cx;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if ( pNMLV->iSubItem >= 0 && m_ViewType == PLAYLISTCTRL_NORMAL )
	{
		// draw sort arrow
		int nCurrCol = pNMLV->iSubItem;
		DrawSortArrow( nCurrCol );

		// sort
		int songid = -1;
		if ( musikCube::g_Player->IsPlaying() )
            songid = musikCube::g_Player->GetPlaying()->GetID();

		int type = musikCube::g_Prefs->GetPlaylistCol( pNMLV->iSubItem );
		musikCube::g_Library->SortPlaylist( m_Playlist, type, m_Ascend ? false : true );

		if ( musikCube::g_Player->IsPlaying() && songid != -1 )
			musikCube::g_Player->FindNewIndex( songid );

		UpdateV();
        
        if ( m_Playlist->GetType() == musikCore::MUSIK_PLAYLIST_TYPE_STANDARD )
			m_PlaylistNeedsSave = true;      
	}

	*pResult = 0;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::DrawSortArrow( int nCurrCol )
{
	if ( nCurrCol == m_LastCol )
		m_Ascend = ( m_Ascend == true ) ? false : true;
	else
		m_Ascend = true;

    // hide old arrow
    HideSortArrow();

	// set new arrow
	HDITEM header_item;
	CHeaderCtrl* header_ctrl = GetHeaderCtrl();

	header_item.mask = HDI_FORMAT | HDI_BITMAP;
	header_ctrl->GetItem( nCurrCol, &header_item );
	header_item.fmt |= HDF_BITMAP | HDF_BITMAP_ON_RIGHT;
	header_item.hbm  = (HBITMAP)LoadImage( AfxGetInstanceHandle(), MAKEINTRESOURCE( m_Ascend ? IDB_UP : IDB_DOWN ), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS );
	header_ctrl->SetItem( nCurrCol, &header_item );

	m_LastCol = nCurrCol;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::HideSortArrow()
{
	HDITEM header_item;
	CHeaderCtrl* header_ctrl = GetHeaderCtrl();

	if ( m_LastCol > -1 && m_LastCol < (int)musikCube::g_Prefs->GetPlaylistColCount() )
	{
        header_item.mask = HDI_FORMAT | HDI_BITMAP;
        header_ctrl->GetItem( m_LastCol, &header_item );
        header_item.fmt &= ~(HDF_BITMAP | HDF_BITMAP_ON_RIGHT);

		if ( header_item.hbm != 0 ) 
		{
			DeleteObject( header_item.hbm );
			header_item.hbm = 0;
		}

        header_ctrl->SetItem( m_LastCol, &header_item );
    }
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnLvnBeginrdrag(NMHDR *pNMHDR, LRESULT *pResult)
{
    if ( m_ViewType == PLAYLISTCTRL_NORMAL )
	    BeginDrag( pNMHDR, true );

	*pResult = 0;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::ScrollToCurr()
{
	if ( musikCube::g_Player->IsPlaying() )
		EnsureVisible( musikCube::g_Player->GetIndex(), FALSE );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::ShowContextMenu()
{
	CPoint pos;
	::GetCursorPos( &pos );

	CMenu main_menu;
	CMenu* popup_menu;

	main_menu.LoadMenu( IDR_PLAYLIST_CONTEXT_MENU );
	popup_menu = main_menu.GetSubMenu( 0 );

	// check / enable / disable menu items
	if ( GetSelectedCount() == 0 )
	{
		popup_menu->EnableMenuItem( ID_PLC_DELETE_FROMPLAYLIST, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_PLC_DELETE_FROMLIBRARY, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_PLC_DELETE_FROMCOMPUTER, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_PLAYLISTCONTEXTMENU_PROPERTIES, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_FILENAME_TAGFROMFILENAME, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_FILENAME_TAGTOFILENAME, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_PLAYLISTCONTEXTMENU_TAGTOLOWERCASE, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_PLAYLISTCONTEXTMENU_TAGTOUPPERCASE, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_TAG_AUTOCAPITALIZE, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_TAG_CLEAN, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_PLAYLISTCONTEXTMENU_QUEUESELECTEDINNOWPLAYING, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_PLAYLISTCONTEXTMENU_REMOVEWHITESPACE, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_PLAYLISTCONTEXTMENU_RELOADTAGSFROMFILE, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_PLAYLISTCONTEXTMENU_REWRITETAGSTOFILE, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_RATE_1, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_RATE_2, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_RATE_3, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_RATE_4, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_RATE_5, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_RATE_UNRATED, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_RATE_N1, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_PLAYLISTCONTEXTMENU_PLAYSELECTED, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_PLAYLISTCONTEXTMENU_REPLACENOWPLAYING, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_PLAYLISTCONTEXTMENU_REPLACENOWPLAYINGANDPLAY, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_PLAYLISTCONTEXTMENU_PLAYNEXT, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_PLAYLISTCONTEXTMENU_SHOWINEXPLORER, MF_DISABLED | MF_GRAYED );
	}	
	if ( GetSelectedCount() <= 1 )
		popup_menu->EnableMenuItem( ID_PLAYLISTCONTEXTMENU_BATCH, MF_DISABLED | MF_GRAYED );

	if ( m_Playlist != musikCube::g_Player->GetPlaylist() )
		popup_menu->EnableMenuItem( ID_PLAYLISTCONTEXTMENU_SHUFFLENOWPLAYING, MF_DISABLED | MF_GRAYED );

	if ( !musikCube::g_Player->GetPlaylist()->GetCount() )
		popup_menu->EnableMenuItem( ID_PLAYLISTCONTEXTMENU_SHUFFLENOWPLAYING, MF_DISABLED | MF_GRAYED );

	if ( m_TagEdit )
		popup_menu->CheckMenuItem( ID_PLAYLISTCONTEXTMENU_PROPERTIES, MF_CHECKED );

	if ( m_MaskDlg )
		popup_menu->CheckMenuItem( ID_FILENAME_CONFIGUREMASKS, MF_CHECKED );

	if ( GetSelectedCount() == 1 )
	{
		int sel_rating = StoI( m_Playlist->GetField( GetFirstSelected(), musikCore::MUSIK_LIBRARY_TYPE_RATING ) );
		switch( sel_rating )
		{	
		case -1:
			popup_menu->CheckMenuItem( ID_RATE_N1, MF_CHECKED );
			break;
		case 0:
			popup_menu->CheckMenuItem( ID_RATE_UNRATED, MF_CHECKED );
			break;
		case 1:
			popup_menu->CheckMenuItem( ID_RATE_1, MF_CHECKED );
			break;
		case 2:
			popup_menu->CheckMenuItem( ID_RATE_2, MF_CHECKED );
			break;
		case 3:
			popup_menu->CheckMenuItem( ID_RATE_3, MF_CHECKED );
			break;
		case 4:
			popup_menu->CheckMenuItem( ID_RATE_4, MF_CHECKED );
			break;
		case 5:
			popup_menu->CheckMenuItem( ID_RATE_5, MF_CHECKED );
			break;
		}
	}

	if ( musikCube::g_Library == musikCube::g_Collected )
	{
		popup_menu->ModifyMenu( 
			ID_PLC_DELETE_FROMPLAYLIST,
			MF_BYCOMMAND | MF_STRING, 
			ID_PLC_DELETE_FROMPLAYLIST, 
			_T( "Remove from Collected" ) );
	}

	popup_menu->TrackPopupMenu( 0, pos.x, pos.y, this );
}

///////////////////////////////////////////////////

int CmusikPlaylistCtrl::GetColumnPos( int field )
{
	for ( size_t i = 0; i < musikCube::g_Prefs->GetPlaylistColCount(); i++ )
	{
		if ( musikCube::g_Prefs->GetPlaylistCol( i ) == field )
			return i;
	}

	return -1;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::ShowHeaderMenu()
{
	CPoint pos;
	::GetCursorPos( &pos );

	CMenu main_menu;
	CMenu* popup_menu;

	main_menu.LoadMenu( IDR_PLAYLIST_COLUMNS_MENU );
	popup_menu = main_menu.GetSubMenu( 0 );

	// check / enable / disable menu items
	musikCore::IntArray items = musikCube::g_Prefs->GetPlaylistOrder();

	for ( size_t i = 0; i < items.size(); i++ )
	{
		switch( items.at( i ) )
		{
		case musikCore::MUSIK_LIBRARY_TYPE_ARTIST:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_ARTIST, MF_CHECKED );
			break;

		case musikCore::MUSIK_LIBRARY_TYPE_ALBUM:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_ALBUM, MF_CHECKED );
			break;

		case musikCore::MUSIK_LIBRARY_TYPE_YEAR:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_YEAR, MF_CHECKED );
			break;

		case musikCore::MUSIK_LIBRARY_TYPE_GENRE:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_GENRE, MF_CHECKED );
			break;

		case musikCore::MUSIK_LIBRARY_TYPE_TITLE:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_TITLE, MF_CHECKED );
			break;

		case musikCore::MUSIK_LIBRARY_TYPE_TRACKNUM:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_TRACKNUMBER, MF_CHECKED );
			break;

		case musikCore::MUSIK_LIBRARY_TYPE_TIMEADDED:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_TIMEADDED, MF_CHECKED );
			break;

		case musikCore::MUSIK_LIBRARY_TYPE_LASTPLAYED:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_LASTPLAYED, MF_CHECKED );
			break;

		case musikCore::MUSIK_LIBRARY_TYPE_FILESIZE:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_FILESIZE, MF_CHECKED );
			break;

		case musikCore::MUSIK_LIBRARY_TYPE_FORMAT:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_FORMAT, MF_CHECKED );
			break;

		case musikCore::MUSIK_LIBRARY_TYPE_DURATION:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_DURATION, MF_CHECKED );
			break;

		case musikCore::MUSIK_LIBRARY_TYPE_RATING:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_RATING, MF_CHECKED );
			break;

		case musikCore::MUSIK_LIBRARY_TYPE_TIMESPLAYED:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_TIMESPLAYED, MF_CHECKED );
			break;

		case musikCore::MUSIK_LIBRARY_TYPE_BITRATE:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_BITRATE, MF_CHECKED );
			break;

		case musikCore::MUSIK_LIBRARY_TYPE_FILENAME:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_FILENAME, MF_CHECKED );
			break;

		case musikCore::MUSIK_LIBRARY_TYPE_COMMENTS:
			popup_menu->CheckMenuItem( ID_PLAYLISTCOLUMNS_COMMENTS, MF_CHECKED );
			break;

		case musikCore::MUSIK_LIBRARY_TYPE_EQUALIZER:
			popup_menu->CheckMenuItem( ID_EQUALIZER_LOCKCHANNELS, MF_CHECKED );
			break;
		}
	}
		
	popup_menu->TrackPopupMenu( 0, pos.x, pos.y, this );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::AddColumn( int field )
{
    SaveColumns();
	musikCube::g_Prefs->AppendPlaylistColOrder( field );
	musikCube::g_Prefs->AppendPlaylistColSize( 50 );
	ResetColumns();
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::RemoveColumn( int field )
{
    SaveColumns();
    musikCore::IntArray cols = musikCube::g_Prefs->GetPlaylistOrder();
    int at = -1;
    for ( size_t i = 0; i < cols.size(); i++ )
    {
        if ( cols.at ( i ) == field )
        {
            at = i;
            break;
        }
    }

    if ( at != -1 )
    {
        musikCube::g_Prefs->RemovePlaylistColOrder( at );
	    musikCube::g_Prefs->RemovePlaylistColSize( at );
	    ResetColumns();
    }
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnHdnEndtrack(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::ToggleColumn( int field )
{
	int pos = GetColumnPos( field );
	if ( pos == -1 )
		AddColumn( field );
	else
		RemoveColumn( field );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsArtist()
{
	ToggleColumn( musikCore::MUSIK_LIBRARY_TYPE_ARTIST );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsAlbum()
{
	ToggleColumn( musikCore::MUSIK_LIBRARY_TYPE_ALBUM );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsYear()
{
	ToggleColumn( musikCore::MUSIK_LIBRARY_TYPE_YEAR );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsGenre()
{
	ToggleColumn( musikCore::MUSIK_LIBRARY_TYPE_GENRE );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsTitle()
{
	ToggleColumn( musikCore::MUSIK_LIBRARY_TYPE_TITLE );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsTracknumber()
{
	ToggleColumn( musikCore::MUSIK_LIBRARY_TYPE_TRACKNUM );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsTimeadded()
{
	ToggleColumn( musikCore::MUSIK_LIBRARY_TYPE_TIMEADDED );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsLastplayed()
{
	ToggleColumn( musikCore::MUSIK_LIBRARY_TYPE_LASTPLAYED );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsFilesize()
{
	ToggleColumn( musikCore::MUSIK_LIBRARY_TYPE_FILESIZE );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsFormat()
{
	ToggleColumn( musikCore::MUSIK_LIBRARY_TYPE_FORMAT );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsDuration()
{
	ToggleColumn( musikCore::MUSIK_LIBRARY_TYPE_DURATION );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsRating()
{
	ToggleColumn( musikCore::MUSIK_LIBRARY_TYPE_RATING );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsTimesplayed()
{
	ToggleColumn( musikCore::MUSIK_LIBRARY_TYPE_TIMESPLAYED );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsBitrate()
{
	ToggleColumn( musikCore::MUSIK_LIBRARY_TYPE_BITRATE );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsFilename()
{
	ToggleColumn( musikCore::MUSIK_LIBRARY_TYPE_FILENAME );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnLvnKeydown( NMHDR *pNMHDR, LRESULT *pResult )
{
    KeyPress( pNMHDR, pResult );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::KeyPress(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	*pResult = 0;

	WORD nChar = pLVKeyDow->wVKey;

	switch ( nChar )
	{
	case VK_DELETE:
		{
			bool delete_from_lib = false;
			bool delete_from_comp = false;

			// delete from library
			if ( GetKeyState( VK_MENU ) < 0 )
				delete_from_lib = true;

			// we also delete straight from the library if
			// a the library is the collected
			if ( musikCube::g_Library == musikCube::g_Collected )
				delete_from_lib = true;

			// delete from computer
			else if ( GetKeyState( VK_CONTROL ) < 0 )
			{
				delete_from_lib = true;
				delete_from_comp = true;
			}

			// delete from playlist
			DeleteSelectedItems( delete_from_lib, delete_from_comp );
			return;
		}
		break;

	case 'A':
	case 'a':
		if ( GetKeyState( VK_CONTROL ) < 0 )
			SetItemState( -1, LVIS_SELECTED, LVIS_SELECTED );
		break;

	case 'D':
	case 'd':
		if ( GetKeyState( VK_CONTROL ) < 0 )
			SetItemState( -1, 0, LVIS_SELECTED );
		break;

	case 'B':
	case 'b':
		if ( GetKeyState( VK_CONTROL ) < 0 )
			OnPlaylistcontextmenuBatch();
		break;

	case 'L':
	case 'l':
		if ( GetKeyState( VK_CONTROL ) < 0 && GetKeyState( VK_SHIFT ) < 0 )
			OnPlaylistcontextmenuTagtolowercase();
		break;

	case 'U':
	case 'u':
		if ( GetKeyState( VK_CONTROL ) < 0 && GetKeyState( VK_SHIFT ) < 0 )
			OnPlaylistcontextmenuTagtouppercase();
		break;

	case 'C':
	case 'c':
		if ( GetKeyState( VK_CONTROL ) < 0 && GetKeyState( VK_SHIFT ) < 0 )
			OnTagAutocapitalize();
		break;

	case 'W':
	case 'w':
		if ( GetKeyState( VK_CONTROL ) < 0 && GetKeyState( VK_SHIFT ) < 0 )
			OnPlaylistcontextmenuRemovewhitespace();
		else if ( GetKeyState( VK_CONTROL ) < 0 && GetKeyState( VK_MENU ) < 0 )
			OnPlaylistcontextmenuTagClean();
		break;

	case 'T':
	case 't':
		if ( GetKeyState( VK_CONTROL ) < 0 )
			OnPlaylistcontextmenuProperties();
	}

}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::DeleteSelectedItems( bool from_library, bool from_computer )
{
	if ( from_library )
	{
		musikCore::Playlist sel_songs;
		GetSelectedSongs( sel_songs );

		if ( from_computer )
		{
			if ( MessageBox( 
                _T( "Are you sure you want to permanently delete these files from your computer?" ), 
                _T( MUSIK_VERSION_STR ), 
                MB_ICONWARNING | MB_YESNO ) == IDNO )
				return;	
		}

		musikCore::Library::DeleteSongs( sel_songs, from_computer );
	}

	musikCore::IntArray sel;
	GetSelectedItems( sel );
	DeleteItems( sel, true );

	if ( from_library )
	{
		int WM_SELBOXREQUESTUPDATE = RegisterWindowMessage( _T( "SELBOXREQUESTUPDATE" ) );
		m_Parent->SendMessage( WM_SELBOXREQUESTUPDATE );

		// notify Player to update its internal playlist
		int WM_PLAYERUPDATE = RegisterWindowMessage( _T( "PLAYERUPDATE" ) );
		m_Parent->SendMessage( WM_PLAYERUPDATE );
	}
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlcDeleteFromplaylist()
{
	DeleteSelectedItems( musikCube::g_Library == musikCube::g_Collected ? true : false );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlcDeleteFromlibrary()
{
	DeleteSelectedItems( true, false );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlcDeleteFromcomputer()
{
	DeleteSelectedItems( true, true );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcontextmenuShufflenowplaying()
{
	musikCube::g_Player->Shuffle();

	if ( GetPlaylist() == musikCube::g_Player->GetPlaylist() )
	{
		UpdateV();
		ScrollToCurr();
	}
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcontextmenuProperties()
{
    int sel = GetSelectedCount();

    if ( sel < 2 )
    {
	    if ( m_TagEdit )
		    OnTagEditDestroy();
        else
	    {
		    if ( !GetSelectedCount() )
			    return;

		    m_TagEdit = new CmusikTagDlg( this, m_Playlist );
		    m_TagEdit->Create( IDD_TAG_PROPERTIES, this );
		    m_TagEdit->ShowWindow( SW_SHOWNORMAL );

		    m_TagEdit->UpdateSel( GetFirstSelected() );
	    }
    }
    else
        OnPlaylistcontextmenuBatch();
}   

///////////////////////////////////////////////////

LRESULT CmusikPlaylistCtrl::OnTagEditDestroy( WPARAM wParam, LPARAM lParam )
{
	m_TagEdit->DestroyWindow();
	delete m_TagEdit;
	m_TagEdit = NULL;

	return 0L;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if ( m_TagEdit && pNMLV->iItem >= 0 && pNMLV->uNewState & LVIS_SELECTED )
		m_TagEdit->UpdateSel( pNMLV->iItem );
	
	*pResult = 0;
}

///////////////////////////////////////////////////

LRESULT CmusikPlaylistCtrl::OnTagNext( WPARAM wParam, LPARAM lParam )
{
    int sel;
    if ( wParam >= 0 )
        sel = (int)wParam;
    else
        sel = GetFirstSelected();

	if ( sel + 1 == GetSongCount() )
		sel = 0;
	else
		sel++;

	SetSelectionMark( sel );
	SetItemState( -1, 0, LVIS_SELECTED );
	SetItemState( sel, LVIS_SELECTED, LVIS_SELECTED );
	EnsureVisible( sel, FALSE );

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikPlaylistCtrl::OnTagPrev( WPARAM wParam, LPARAM lParam )
{
    int sel;
    if ( wParam )
        sel = (int)wParam;
    else
        sel = GetFirstSelected();

	if ( sel - 1 == -1 )
		sel = GetSongCount() - 1;
	else
		sel--;

	SetSelectionMark( sel );
	SetItemState( -1, 0, LVIS_SELECTED );
	SetItemState( sel, LVIS_SELECTED, LVIS_SELECTED );
	EnsureVisible( sel, FALSE );

	return 0L;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::RefreshTag( int songid )
{
	ResyncItem( songid );
}

///////////////////////////////////////////////////

void CmusikPlaylistCDCtrl::RefreshTag( int no )
{
    UpdateV();
}

///////////////////////////////////////////////////

LRESULT CmusikPlaylistCtrl::OnTagUpdate( WPARAM wParam, LPARAM lParam )
{
	if ( wParam )
        RefreshTag( (int)wParam );

	return 0L;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcontextmenuBatch()
{
	musikCore::SongInfoArray* selected = new musikCore::SongInfoArray();
	GetSelectedSongs( *selected );

	CmusikBatchTagDlg batch( this, selected );
	
	if ( selected->size() > 1 && batch.DoModal() == IDOK )
	{
		musikCore::BatchRetag* params;

		params = new musikCore::BatchRetag( 
			musikCube::g_Library, 
			NULL, 
			selected );

		params->m_DeleteUpdatedTags = true;
		params->m_WriteToFile = false;
		params->m_Functor = musikCube::g_Functor;

		musikCore::BatchRetagTask* task = new musikCore::BatchRetagTask;
		task->m_Params = params;
		musikCube::g_Library->QueueTask( task );
	}
	else
		delete selected;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnFilenameTagfromfilename()
{
    if ( ShowMaskDlg( MUSIK_MASK_FROMFILENAME ) != IDOK )
        return;

	musikCore::String mask = musikCube::g_Prefs->GetTagFromFilename();
	if ( mask.IsEmpty() )
	{
		MessageBox( 
            _T( "No Tag to Filename mask has been configured. To read tag information from a filename you must configure a mask." ), 
            _T( MUSIK_VERSION_STR ),
            MB_ICONWARNING );

		return;
	}

	musikCore::SongArray* songs = new musikCore::SongArray();
	GetSelectedSongs( *songs );

	musikCore::TagFromFn* params = new musikCore::TagFromFn( 
		musikCube::g_Library, 
		songs, 
		musikCube::g_Functor,
		mask,
		musikCube::g_Prefs->GetTagUndrToSpaces() );

	musikCore::TagFromFnTask* task = new musikCore::TagFromFnTask();
	task->m_Params = params;
	musikCube::g_Library->QueueTask( task );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnFilenameTagtofilename()
{
    if ( ShowMaskDlg( MUSIK_MASK_TOFILENAME ) != IDOK )
        return;

	musikCore::String mask = musikCube::g_Prefs->GetTagToFilename();

	if ( mask.IsEmpty() )
	{
		MessageBox( 
            _T( "No Tag from Filename mask has been configured. To rename your files based on database tag information you must configure a mask." ), 
            _T( MUSIK_VERSION_STR ), 
            MB_ICONWARNING );

		return;
	}

	musikCore::SongArray* songs = new musikCore::SongArray();
	GetSelectedSongs( *songs );

	musikCore::TagToFn* params = new musikCore::TagToFn(
		musikCube::g_Library,
		songs, 
		musikCube::g_Functor,
		mask,
		musikCube::g_Prefs->GetTagSpacesToUndr() );

	musikCore::TagToFnTask* task = new musikCore::TagToFnTask();
	task->m_Params = params;
	musikCube::g_Library->QueueTask( task );	
}

///////////////////////////////////////////////////

int CmusikPlaylistCtrl::ShowMaskDlg( int state )
{
	m_MaskDlg = new CmusikMaskDlg( this, state );
    int ret = m_MaskDlg->DoModal();
    delete m_MaskDlg;
    m_MaskDlg = NULL;

    return ret;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcontextmenuTagtolowercase()
{
	musikCore::SongInfoArray* selected = new musikCore::SongInfoArray();
	GetSelectedSongs( *selected );

	if ( selected->size() )
	{
		musikCore::BatchChgCase* params;

		params = new musikCore::BatchChgCase( 
			musikCube::g_Library, 
			musikCube::g_Functor,
			selected, 
			musikCore::MUSIK_CHGCASE_TOLOWER );

		params->m_DeleteUpdatedTags = true;

		musikCore::BatchChgCaseTask* task = new musikCore::BatchChgCaseTask;
		task->m_Params = params;
		musikCube::g_Library->QueueTask( task );
	}
	else
		delete selected;	
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcontextmenuTagtouppercase()
{
	musikCore::SongInfoArray* selected = new musikCore::SongInfoArray();
	GetSelectedSongs( *selected );

	if ( selected->size() )
	{
		musikCore::BatchChgCase* params;

		params = new musikCore::BatchChgCase( 
			musikCube::g_Library, 
			musikCube::g_Functor,
			selected, 
			musikCore::MUSIK_CHGCASE_TOUPPER );

		params->m_DeleteUpdatedTags = true;

		musikCore::BatchChgCaseTask* task = new musikCore::BatchChgCaseTask;
		task->m_Params = params;
		musikCube::g_Library->QueueTask( task );
	}
	else
		delete selected;	
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcontextmenuTagClean()
{
	musikCore::SongInfoArray* selected = new musikCore::SongInfoArray();
	GetSelectedSongs( *selected );

	if ( selected->size() )
	{
		musikCore::BatchChgCase* params;

		params = new musikCore::BatchChgCase( 
			musikCube::g_Library, 
			musikCube::g_Functor,
			selected, 
			musikCore::MUSIK_CHGCASE_CLEAN );

		params->m_DeleteUpdatedTags = true;

		musikCore::BatchChgCaseTask* task = new musikCore::BatchChgCaseTask;
		task->m_Params = params;
		musikCube::g_Library->QueueTask( task );
	}
	else
		delete selected;	
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnTagAutocapitalize()
{
	musikCore::SongInfoArray* selected = new musikCore::SongInfoArray();
	GetSelectedSongs( *selected );

	if ( selected->size() )
	{
		musikCore::BatchChgCase* params;

		params = new musikCore::BatchChgCase( 
			musikCube::g_Library, 
			musikCube::g_Functor,
			selected, 
			musikCore::MUSIK_CHGCASE_CAPITALIZE );

		params->m_DeleteUpdatedTags = true;

		musikCore::BatchChgCaseTask* task = new musikCore::BatchChgCaseTask;
		task->m_Params = params;
		musikCube::g_Library->QueueTask( task );
	}
	else
		delete selected;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::RateSelectedItems( int rating )
{
	musikCore::SongArray songs;
	GetSelectedSongs( songs );

	musikCube::g_Library->BeginTransaction();
	for ( size_t i = 0; i < songs.size(); i++ )
		musikCube::g_Library->SetSongRating( songs.at( i ).GetID(), rating );
	musikCube::g_Library->EndTransaction();

	if ( musikCube::g_Player->GetPlaying() )
		musikCube::g_Player->GetPlaying()->SetRating( ItoS( rating ) );
	
	UpdateV();
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnRateN1()
{
	RateSelectedItems( -1 );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnRateUn()
{
	RateSelectedItems( 0 );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnRate1()
{
	RateSelectedItems( 1 );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnRate2()
{
	RateSelectedItems( 2 );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnRate3()
{
	RateSelectedItems( 3 );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnRate4()
{
	RateSelectedItems( 4 );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnRate5()
{
	RateSelectedItems( 5 );
}

///////////////////////////////////////////////////

void CmusikPlaylistCDCtrl::Init( CFrameWnd* mainwnd, UINT dropid_l, UINT dropid_r )
{
    CmusikPlaylistCtrl::Init( mainwnd, dropid_l, dropid_r );
    m_ViewType = PLAYLISTCTRL_CDRIPPER;
}

///////////////////////////////////////////////////

CmusikPlaylistCDCtrl::CmusikPlaylistCDCtrl( CFrameWnd* mainwnd, UINT dropid_l, UINT dropid_r )
: CmusikPlaylistCtrl( mainwnd, NULL, dropid_l, dropid_r )
{
    m_CanPlay = true;
    m_Drive = 0;
    m_ViewType = PLAYLISTCTRL_CDRIPPER;
}

///////////////////////////////////////////////////

CString CmusikPlaylistCDCtrl::GetTimeStr( int item )
{
    if ( m_SongInfoArray )
	{
		int duration = StoI( m_SongInfoArray->at( item ).GetDuration() );
		return (CString)musikCube::g_Player->GetTimeStr( duration );	
	}

    return _T( "" );
}

///////////////////////////////////////////////////

void CmusikPlaylistCDCtrl::SetSongInfoArray( musikCore::SongInfoArray* info )
{
    HideSortArrow();

	if ( info == NULL )
	{
		cout << _T( "Passed a NULL array\n" );
		return;
	}

	SetItemState( -1, 0, LVIS_SELECTED );

	if ( m_SongInfoArray )
		delete m_SongInfoArray;

	// set the playlist and cache it
	m_SongInfoArray = info;
    m_ViewType = PLAYLISTCTRL_CDRIPPER;

	if ( m_InfoCtrl )
		m_InfoCtrl->UpdateInfo();	

	if ( m_TagEdit )
		m_TagEdit->UpdatePlaylist( m_Playlist );
}

///////////////////////////////////////////////////

void CmusikPlaylistCDCtrl::UpdateItemCount()
{
    SetItemCountEx( m_SongInfoArray->size(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL );
}

///////////////////////////////////////////////////

musikCore::String CmusikPlaylistCDCtrl::GetItemAt( int nItem, int sub_type )
{
    if ( m_SongInfoArray->size() < 0 )
        return _T( "" );

    return m_SongInfoArray->at( nItem ).GetField( sub_type );
}

///////////////////////////////////////////////////

int CmusikPlaylistCDCtrl::GetSongID( int at )
{
    return -1;
}

///////////////////////////////////////////////////

void CmusikPlaylistCDCtrl::GetPlaylistCols()
{
    m_Cols.push_back( musikCore::MUSIK_LIBRARY_TYPE_TRACKNUM );
    m_Cols.push_back( musikCore::MUSIK_LIBRARY_TYPE_TITLE );
    m_Cols.push_back( musikCore::MUSIK_LIBRARY_TYPE_DURATION );
    m_Sizes.push_back( 52 );
    m_Sizes.push_back( 210 );
    m_Sizes.push_back( 60 );
}

///////////////////////////////////////////////////

void CmusikPlaylistCDCtrl::ShowContextMenu()
{
	CPoint pos;
	::GetCursorPos( &pos );

	CMenu main_menu;
	CMenu* popup_menu;

	main_menu.LoadMenu( IDR_CDRIP_MENU );
	popup_menu = main_menu.GetSubMenu( 0 );

	// check / enable / disable menu items
	if ( !GetItemCount() )
	{
		popup_menu->EnableMenuItem( ID_CDRIP_EDITALBUMINFO, MF_DISABLED | MF_GRAYED );
		popup_menu->EnableMenuItem( ID_CDRIP_EDITTRACKINFO, MF_DISABLED | MF_GRAYED );
	}	
    else
    {
        if ( m_TagEdit )
            popup_menu->EnableMenuItem( ID_CDRIP_EDITTRACKINFO, MF_DISABLED | MF_GRAYED );
    }

	popup_menu->TrackPopupMenu( 0, pos.x, pos.y, this );
}

///////////////////////////////////////////////////


IMPLEMENT_DYNAMIC( CmusikPlaylistCDCtrl, CWnd )
BEGIN_MESSAGE_MAP(CmusikPlaylistCDCtrl, CmusikPlaylistCtrl)
	ON_COMMAND( ID_CDRIP_EDITALBUMINFO, OnEditAlbumInfo )
	ON_COMMAND( ID_CDRIP_EDITTRACKINFO, OnEditTrackInfo )
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikPlaylistCDCtrl::OnEditAlbumInfo()
{
    CmusikBatchTagDlg batch( this, m_SongInfoArray );
    batch.DoModal();

    UpdateV();
}

///////////////////////////////////////////////////

void CmusikPlaylistCDCtrl::OnEditTrackInfo()
{
    if ( !m_TagEdit )
    {
        m_TagEdit = new CmusikTagDlg( this, m_SongInfoArray );
	    m_TagEdit->Create( IDD_TAG_PROPERTIES, this );
	    m_TagEdit->ShowWindow( SW_SHOWNORMAL );

        int sel = GetFirstSelected();
        if ( sel == -1 ) sel = 0;
        m_TagEdit->UpdateSel( sel );
    }
}

///////////////////////////////////////////////////

bool CmusikPlaylistCDCtrl::PlayItem( int n )
{
    if ( m_CanPlay )
    {
		musikCube::g_Player->SetCDInfo( *m_SongInfoArray, GetDiscDrive() );
        musikCube::g_Player->PlayCD( n, 0 );  
        SetItemState( -1, 0, LVIS_SELECTED );
        RedrawWindow();
		int WM_SOURCESREFRESH = RegisterWindowMessage( _T( "SOURCESREFRESH" ) );
		AfxGetApp()->GetMainWnd()->SendMessage( WM_SOURCESREFRESH, NULL );
		return true;
    }

    return false;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcontextmenuReloadtagsfromfile()
{
	musikCore::Playlist* pPlaylist = new musikCore::Playlist();
	GetSelectedSongs( *pPlaylist );

	if ( pPlaylist->size() )
	{
		musikCore::ReloadTags* pParams = new musikCore::ReloadTags(
			musikCube::g_Functor,
			pPlaylist,
			musikCube::g_Library );

		musikCore::ReloadTagsTask* pTask = new musikCore::ReloadTagsTask();
		pTask->m_Params = pParams;

		musikCube::g_Library->QueueTask( pTask );
	}
	else
		delete pPlaylist;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcolumnsComments()
{
	ToggleColumn( musikCore::MUSIK_LIBRARY_TYPE_COMMENTS );
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcontextmenuRemovewhitespace()
{
	musikCore::SongInfoArray* selected = new musikCore::SongInfoArray();
	GetSelectedSongs( *selected );

	if ( selected->size() )
	{
		musikCore::BatchChgCase* params;

		params = new musikCore::BatchChgCase( 
			musikCube::g_Library, 
			musikCube::g_Functor,
			selected, 
			musikCore::MUSIK_CHGCASE_REMOVEWHITESPACE );

		params->m_DeleteUpdatedTags = true;

		musikCore::BatchChgCaseTask* task = new musikCore::BatchChgCaseTask;
		task->m_Params = params;
		musikCube::g_Library->QueueTask( task );
	}
	else
		delete selected;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcontextmenuRewritetagstofile()
{
	musikCore::Playlist playlist;
	GetSelectedSongs( playlist );

	if ( playlist.size() )
	{
		musikCore::SongInfoArray* pSongInfoArray = new musikCore::SongInfoArray();
		musikCube::g_Library->GetInfoArrayFromPlaylist( playlist, *pSongInfoArray );	

		musikCore::BatchRetag* params = new musikCore::BatchRetag(
			musikCube::g_Library,
			musikCube::g_Functor,
			pSongInfoArray );
		params->m_WriteToFile = true;		// set explicitly to avoid accidental writing to file

		musikCore::BatchRetagTask* pTask = new musikCore::BatchRetagTask();
		pTask->m_Params = params;

		musikCube::g_Library->QueueTask( pTask );
	}
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcontextmenuPlayselected()
{
	musikCore::Playlist* playlist = new musikCore::Playlist();
	GetSelectedSongs( *playlist );
	int selected = musikCube::g_Player->GetIndex();
	musikCube::g_Player->GetPlaylist()->InsertAfter( *playlist, selected );
	musikCube::g_Player->Play( ++selected );

	if ( GetPlaylist() == musikCube::g_Player->GetPlaylist() )
		UpdateV();

	delete playlist;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcontextmenuPlaynext()
{
	musikCore::Playlist* playlist = new musikCore::Playlist();
	GetSelectedSongs( *playlist );
	int selected = musikCube::g_Player->GetIndex();
	musikCube::g_Player->GetPlaylist()->InsertAfter( *playlist, selected );

	if ( GetPlaylist() == musikCube::g_Player->GetPlaylist() )
		UpdateV();

	delete playlist;
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcontextmenuReplacenowplaying()
{
	bool is_np = ( GetPlaylist() == musikCube::g_Player->GetPlaylist() );

	musikCore::Playlist* playlist = new musikCore::Playlist();
	GetSelectedSongs( *playlist );

	if ( is_np )
		SetPlaylist( playlist );

	musikCube::g_Player->SetPlaylist( playlist );
	musikCube::g_Player->FindNewIndex();

	if ( is_np )
		UpdateV();
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcontextmenuQueueselectedinnowplaying()
{
	musikCore::Playlist* playlist = new musikCore::Playlist();
	GetSelectedSongs( *playlist );

	bool is_np = ( musikCube::g_Player->GetPlaylist() == GetPlaylist() );

	for ( size_t i = 0; i < playlist->size(); i++ )
		musikCube::g_Player->GetPlaylist()->Add( playlist->at( i ) );

	if ( is_np )
		UpdateV();
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaybackReplacenowplayingandplay()
{
	bool is_np = ( GetPlaylist() == musikCube::g_Player->GetPlaylist() );

	musikCore::Playlist* playlist = new musikCore::Playlist();
	GetSelectedSongs( *playlist );

	if ( is_np )
		SetPlaylist( playlist );

	musikCube::g_Player->SetPlaylist( playlist );
	musikCube::g_Player->Play( 0 );

	if ( is_np )
		UpdateV();
}

///////////////////////////////////////////////////

void CmusikPlaylistCtrl::OnPlaylistcontextmenuShowinexplorer()
{
	musikCore::Filename fn( m_Playlist->at( GetFirstSelected() ).GetField( musikCore::MUSIK_LIBRARY_TYPE_FILENAME ) );
	
	wchar_t windowsDir[MAX_PATH];
	GetEnvironmentVariable( _T( "WINDIR" ), windowsDir, MAX_PATH );

	musikCore::String commandLine;
	commandLine.Format(
		_T( "/n, /e, /select, %s" ),
		fn.GetFullFilename().c_str()
	);
	
	ShellExecute( NULL, _T( "open" ), _T( "explorer" ), commandLine.c_str(), NULL, SW_SHOWNORMAL );
}

///////////////////////////////////////////////////