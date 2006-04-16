///////////////////////////////////////////////////

#include "stdafx.h"
#include "musikPluginDlg.h"
#include "PlaylistWnd.h"
#include "globals.h"
#include ".\playlistwnd.h"

///////////////////////////////////////////////////

#if defined (COMPILE_STANDALONE)

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CPlaylistWnd, CDialog)

///////////////////////////////////////////////////

CPlaylistWnd::CPlaylistWnd(CWnd* pParent /*=NULL*/)
	: CDialog(CPlaylistWnd::IDD, pParent)
{
	Create( IDD );
}

///////////////////////////////////////////////////

CPlaylistWnd::~CPlaylistWnd()
{
}

///////////////////////////////////////////////////

void CPlaylistWnd::SetPlaylist( musikCore::Playlist& playlist )
{
	g_Library->GetInfoArrayFromPlaylist( playlist, m_Info );
	CString str;
	
	CListBox* box = (CListBox*)GetDlgItem( IDC_PLBOX );
	box->ResetContent();
	for ( size_t i = 0; i < m_Info.size(); i++ )
	{
		musikCore::SongInfo& info = m_Info.at( i );
		str.Format( 
			_T( "%s - %s" ),
			info.GetArtist().c_str(),
			info.GetTitle().c_str() );

		box->AddString( str );
	}
}

///////////////////////////////////////////////////

void CPlaylistWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CPlaylistWnd, CDialog)
	ON_LBN_DBLCLK(IDC_PLBOX, OnLbnDblclkPlbox)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////

void CPlaylistWnd::OnLbnDblclkPlbox()
{
	CListBox* box = (CListBox*)GetDlgItem( IDC_PLBOX );
	int sel = box->GetCurSel();

	musikCore::Song song( g_Library );
	song.SetID( m_Info.at( sel ).GetID() );
	g_Player->GetPlaylist()->Add( song );

	g_Player->Play( 
		g_Player->GetPlaylist()->GetCount() - 1, 
		musikCore::MUSIK_CROSSFADER_NEW_SONG );
}

///////////////////////////////////////////////////


