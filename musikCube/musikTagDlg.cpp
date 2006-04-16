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
//   CmusikTagDlg
//
// Filename(s): 
//
//   musikTagDlg.h, musikTagDlg.cpp
//
// Information:
//
//   Dialog used for tag editing
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "musikCube.h"
#include "musikTagDlg.h"

#include <musikCore.h>

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikTagDlg, CDialog)

///////////////////////////////////////////////////

CmusikTagDlg::CmusikTagDlg( CWnd* parent, musikCore::Playlist* playlist )
	: CDialog(CmusikTagDlg::IDD, parent)
{
	m_Playlist = playlist;
	m_Parent = parent;
	m_Modified = false;
	m_Updating = false;
	m_SongIndex = 0;
    m_Mode = MODE_PLAYLIST;
}

///////////////////////////////////////////////////

CmusikTagDlg::CmusikTagDlg( CWnd* parent, musikCore::SongInfoArray* songinfo )
	: CDialog(CmusikTagDlg::IDD, parent)
{
	m_SongInfoArray = songinfo;
	m_Parent = parent;
	m_Modified = false;
	m_Updating = false;
	m_SongIndex = 0;
    m_Mode = MODE_SONGINFO;
}

///////////////////////////////////////////////////

CmusikTagDlg::~CmusikTagDlg()
{
}

///////////////////////////////////////////////////

void CmusikTagDlg::UpdateSel( int index )
{
    if ( m_Mode == MODE_PLAYLIST )
    {
	    if ( m_Playlist && m_Playlist->size() > (size_t)index )
	    {
		    if ( index < 0 || index <= (int)m_Playlist->GetCount() )
		    {
			    m_SongIndex = index;
				musikCore::Library* curr = m_Playlist->at( index ).GetLibrary();
			    curr->GetSongInfoFromID( m_Playlist->GetSongID( index ), m_Song );		
			    UpdateDlg();
		    }
	    }
    }
    else	// CDROM
    {
        if ( index > -1 && m_SongInfoArray->size() )
        {
			m_SongIndex = index;
            m_Song = m_SongInfoArray->at( index );
            UpdateDlg();
        }
    }
}

///////////////////////////////////////////////////

void CmusikTagDlg::UpdateDlg()
{
	m_Updating = true;

	CStatic* ptrCurr;
	CString temp;

	ptrCurr = (CStatic*)GetDlgItem( IDC_BITRATE );
	ptrCurr->SetWindowText( m_Song.GetBitrate() );

	ptrCurr = (CStatic*)GetDlgItem( IDC_TITLE );
	ptrCurr->SetWindowText( m_Song.GetTitle() );

	ptrCurr = (CStatic*)GetDlgItem( IDC_ALBUM );
	ptrCurr->SetWindowText( m_Song.GetAlbum() );

	ptrCurr = (CStatic*)GetDlgItem( IDC_ARTIST );
	ptrCurr->SetWindowText( m_Song.GetArtist() );

	ptrCurr = (CStatic*)GetDlgItem( IDC_TRACK );
	ptrCurr->SetWindowText( m_Song.GetTrackNum() );

	ptrCurr = (CStatic*)GetDlgItem( IDC_YEAR );
	ptrCurr->SetWindowText( m_Song.GetYear() );

	ptrCurr = (CStatic*)GetDlgItem( IDC_TIMEADDED );
	ptrCurr->SetWindowText( m_Song.GetTimeAdded() );

	ptrCurr = (CStatic*)GetDlgItem( IDC_LASTPLAYED );
	temp = m_Song.GetLastPlayed();
	if ( temp.IsEmpty() || temp[0] == '-' )
		temp = _T( "never" );
	ptrCurr->SetWindowText( temp );

	CComboBox* cmbtemp = (CComboBox*)GetDlgItem( IDC_RATING );
	int sel = 0;
	temp = m_Song.GetRating();
	if ( temp == _T( "0" ) )
		temp = _T( "Unrated" );
	cmbtemp->SelectString( -1, temp );

	ptrCurr = (CStatic*)GetDlgItem( IDC_TIMESPLAYED );
	ptrCurr->SetWindowText( m_Song.GetTimesPlayed() );

	ptrCurr = (CStatic*)GetDlgItem( IDC_GENRE );
	ptrCurr->SetWindowText( m_Song.GetGenre() );

	ptrCurr = (CStatic*)GetDlgItem( IDC_COMMENTS );
	ptrCurr->SetWindowText( m_Song.GetNotes() );

	ptrCurr = (CStatic*)GetDlgItem( IDC_DURATION );
	ptrCurr->SetWindowText( musikCore::Player::GetTimeStr( StoI( m_Song.GetDuration() ) ) );

	CString format = m_Song.GetFormat();
	ptrCurr = (CStatic*)GetDlgItem( IDC_FORMAT );
	ptrCurr->SetWindowText( format );

	CEdit* ptrEdit = (CEdit*)GetDlgItem( IDC_FILENAME );
	if ( format == MUSIK_LIBRARY_FORMAT_NET )
	{
		ptrEdit->SetReadOnly( FALSE );
		ptrEdit->SetWindowText( m_Song.GetFilename() );
	}
	else
	{
		wchar_t realfn[512];
		if ( GetLongPathName( m_Song.GetFilename(), realfn, 512 ) <= 0 )
		{
			::wcscpy( realfn, _T( "File not found" ) );
		}

		ptrEdit->SetWindowText( realfn );
		ptrEdit->SetReadOnly( TRUE );
	}

	ptrCurr = (CStatic*)GetDlgItem( IDC_APPLY );
	ptrCurr->EnableWindow( FALSE );

	SelectFocused();

	m_Updating = false;
	m_Modified = false;
}

///////////////////////////////////////////////////

void CmusikTagDlg::SaveCurr()
{
	m_Updating = true;
	if ( m_Modified )
	{
		CString curr;
		CWnd* ptrCurr;

		// get new info
		ptrCurr = (CStatic*)GetDlgItem( IDC_TITLE );
		ptrCurr->GetWindowText( curr );
		m_Song.SetTitle( curr.GetBuffer( NULL ) );

		ptrCurr = (CStatic*)GetDlgItem( IDC_TRACK );
		ptrCurr->GetWindowText( curr );
        if ( curr.IsEmpty() )
            curr = _T( "0" );
		m_Song.SetTrackNum( curr.GetBuffer( NULL ) );

		ptrCurr = (CStatic*)GetDlgItem( IDC_ARTIST );
		ptrCurr->GetWindowText( curr );
		m_Song.SetArtist( curr.GetBuffer( NULL ) );

		ptrCurr = (CStatic*)GetDlgItem( IDC_ALBUM );
		ptrCurr->GetWindowText( curr );
		m_Song.SetAlbum( curr.GetBuffer( NULL ) );

		ptrCurr = (CStatic*)GetDlgItem( IDC_GENRE );
		ptrCurr->GetWindowText( curr );
		m_Song.SetGenre( curr.GetBuffer( NULL ) );

		ptrCurr = (CStatic*)GetDlgItem( IDC_YEAR );
		ptrCurr->GetWindowText( curr );
        if ( curr.IsEmpty() )
            curr = _T( "0" );
		m_Song.SetYear( curr.GetBuffer( NULL ) );

		ptrCurr = (CStatic*)GetDlgItem( IDC_COMMENTS );
		ptrCurr->GetWindowText( curr );
		m_Song.SetNotes( curr.GetBuffer( NULL ) );

		ptrCurr = (CStatic*)GetDlgItem( IDC_RATING );
		ptrCurr->GetWindowText( curr );
		if ( curr == _T( "Unrated" ) )
			curr = _T( "0" );
		m_Song.SetRating( curr.GetBuffer( NULL ) );

		ptrCurr = (CStatic*)GetDlgItem( IDC_APPLY );
		ptrCurr->EnableWindow( FALSE );

		musikCore::String format = m_Song.GetFormat();
		if ( format == MUSIK_LIBRARY_FORMAT_NET )
		{
			ptrCurr = (CStatic*)GetDlgItem( IDC_FILENAME );
			ptrCurr->GetWindowText( curr );
			if ( !curr.IsEmpty() )
				m_Song.SetFilename( curr.GetBuffer( NULL ) );
		}

		// save it
        if ( m_Mode == MODE_PLAYLIST )
        {
		    m_Song.GetLibrary()->SetSongInfo( m_Song );
		    m_Song.SetDirtyFlag( _T( "0" ) );
        }
        else
        {
            m_SongInfoArray->at( m_SongIndex ) = m_Song;
        }

		// send a message to update the playlist ctrl
		int WM_TAGUPDATE = RegisterWindowMessage( _T( "TAGUPDATE" ) );
		m_Parent->PostMessage( WM_TAGUPDATE, (WPARAM)m_Song.GetID() );

		m_Modified = false;
	}
	m_Updating = false;
}

///////////////////////////////////////////////////

void CmusikTagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikTagDlg, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)
	ON_BN_CLICKED(IDC_NEXT, OnBnClickedNext)
	ON_BN_CLICKED(IDC_PREV, OnBnClickedPrev)
	ON_EN_CHANGE(IDC_FILENAME, OnChangeField)
	ON_EN_CHANGE(IDC_TITLE, OnChangeField)
	ON_EN_CHANGE(IDC_COMMENTS, OnChangeField)
	ON_EN_CHANGE(IDC_TRACK, OnChangeField)
	ON_EN_CHANGE(IDC_ARTIST, OnChangeField)
	ON_EN_CHANGE(IDC_ALBUM, OnChangeField)
	ON_EN_CHANGE(IDC_YEAR, OnChangeField)
	ON_CBN_EDITCHANGE(IDC_GENRE, OnChangeField)
	ON_CBN_EDITCHANGE(IDC_RATING, OnChangeRating)
	ON_CBN_SELCHANGE(IDC_GENRE, OnChangeField)
	ON_CBN_SELCHANGE(IDC_RATING, OnChangeRating)
	ON_BN_CLICKED(IDC_APPLY, OnBnClickedApply)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikTagDlg::OnClose()
{
	int WM_TAGPROPERTIESDESTROY = RegisterWindowMessage( _T( "TAGPROPERTIESDESTROY" ) );
	m_Parent->PostMessage( WM_TAGPROPERTIESDESTROY );
}

///////////////////////////////////////////////////

BOOL CmusikTagDlg::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN )
	{
		if ( pMsg->wParam == VK_ESCAPE )
		{
			OnClose();
			return true;
		}
		else
		{
			if ( pMsg->wParam == VK_RETURN )
			{
				if ( GetKeyState( VK_SHIFT ) < 0 )
					OnShiftEnter();
				else
					OnEnter();
			
				return true;
			}
		}
	}

	return CDialog::PreTranslateMessage( pMsg );
}

///////////////////////////////////////////////////

BOOL CmusikTagDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CComboBox* combo = (CComboBox*)GetDlgItem( IDC_RATING );
	combo->AddString( _T( "-1" ) );
	combo->AddString( _T( "Unrated" ) );
	combo->AddString( _T( "1" ) );
	combo->AddString( _T( "2" ) );
	combo->AddString( _T( "3" ) );
    combo->AddString( _T( "4" ) );
	combo->AddString( _T( "5" ) );

	combo = (CComboBox*)GetDlgItem( IDC_GENRE );
	
	int i;
	for (i = 0; i < ID3_NR_OF_V1_GENRES; i++ )
		combo->AddString( ID3_v1_genre_description[i] );

	// get the genres currently listed in the database and add them
	//  to the beginning of the list
	musikCore::StringArray current_genres;
	musikCube::g_Library->GetAllDistinct(3, current_genres);
	for (i = 0; i < (int) current_genres.size(); i++)
		combo->InsertString(i,current_genres[i].c_str());

	// insert a separator between these and the standard ID3v1 list
	//  (is there a better way to do this?)
	combo->InsertString(i,_T("-----"));

	return TRUE;
}

///////////////////////////////////////////////////

void CmusikTagDlg::OnBnClickedClose()
{
	OnClose();
}

///////////////////////////////////////////////////

void CmusikTagDlg::OnBnClickedNext()
{
	PromptSave();
	int WM_TAGNEXT = RegisterWindowMessage( _T( "TAGNEXT" ) );
	m_Parent->SendMessage( WM_TAGNEXT, (WPARAM)m_SongIndex );
}

///////////////////////////////////////////////////

void CmusikTagDlg::OnBnClickedPrev()
{
	PromptSave();
	int WM_TAGPREV = RegisterWindowMessage( _T( "TAGPREV" ) );
	m_Parent->SendMessage( WM_TAGPREV, (WPARAM)m_SongIndex );
}

///////////////////////////////////////////////////

void CmusikTagDlg::OnEnter()
{
	SaveCurr();
	int WM_TAGNEXT = RegisterWindowMessage( _T( "TAGNEXT" ) );
	m_Parent->SendMessage( WM_TAGNEXT, (WPARAM)m_SongIndex );
}

///////////////////////////////////////////////////

void CmusikTagDlg::OnShiftEnter()
{
	SaveCurr();
	int WM_TAGPREV = RegisterWindowMessage( _T( "TAGPREV" ) );
	m_Parent->SendMessage( WM_TAGPREV, (WPARAM)m_SongIndex );
}

///////////////////////////////////////////////////

void CmusikTagDlg::PromptSave()
{
	if ( m_Modified )
	{
		if ( MessageBox( 
                _T( "Save current changes to song information?" ), 
                _T( MUSIK_VERSION_STR ), MB_YESNO ) == IDYES )
			SaveCurr();
	}
}

///////////////////////////////////////////////////

void CmusikTagDlg::OnChangeField()
{
	if ( !m_Updating && !m_Modified )
	{
		CWnd* ptrCurr = (CWnd*)GetDlgItem( IDC_APPLY );
		ptrCurr->EnableWindow( TRUE );
		m_Song.SetDirtyFlag( _T( "1" ) );

		m_Modified = true;
	}
}

///////////////////////////////////////////////////

void CmusikTagDlg::OnChangeRating()
{
	if ( !m_Updating && !m_Modified )
	{
		CWnd* ptrCurr = (CWnd*)GetDlgItem( IDC_APPLY );
		ptrCurr->EnableWindow( TRUE );

		m_Modified = true;
	}
}

///////////////////////////////////////////////////

void CmusikTagDlg::OnBnClickedApply()
{
	SaveCurr();
	this->OnClose();
}

///////////////////////////////////////////////////

void CmusikTagDlg::SelectFocused()
{
	CWnd* focus = GetFocus();
	CEdit* editTemp = NULL;

	// edit fields first
	if ( focus == (CWnd*)GetDlgItem( IDC_TITLE ) )
		editTemp = (CEdit*)GetDlgItem( IDC_TITLE );
	if ( focus == (CWnd*)GetDlgItem( IDC_ARTIST ) )
		editTemp = (CEdit*)GetDlgItem( IDC_ARTIST );
	if ( focus == (CWnd*)GetDlgItem( IDC_ALBUM ) )
		editTemp = (CEdit*)GetDlgItem( IDC_ALBUM );
	if ( focus == (CWnd*)GetDlgItem( IDC_TRACK ) )
		editTemp = (CEdit*)GetDlgItem( IDC_TRACK );
	if ( focus == (CWnd*)GetDlgItem( IDC_YEAR ) )
		editTemp = (CEdit*)GetDlgItem( IDC_YEAR );

	if ( editTemp )
	{
		CString temp;
		editTemp->GetWindowText( temp );
		editTemp->SetSel( 0, temp.GetLength() );
	}
}

///////////////////////////////////////////////////


