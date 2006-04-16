///////////////////////////////////////////////////

#include "stdafx.h"
#include "musikCube.h"
#include "musikAddNetstreamDlg.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikAddNetstreamDlg, CDialog)

///////////////////////////////////////////////////

CmusikAddNetstreamDlg::CmusikAddNetstreamDlg()
	: CDialog(CmusikAddNetstreamDlg::IDD, NULL)
{
}

///////////////////////////////////////////////////

CmusikAddNetstreamDlg::~CmusikAddNetstreamDlg()
{
}

///////////////////////////////////////////////////

void CmusikAddNetstreamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

///////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(CmusikAddNetstreamDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikAddNetstreamDlg::OnBnClickedOk()
{
	CString strStream;
	GetDlgItem( IDC_NETSTREAM_ADDR )->GetWindowText( strStream );

	if ( strStream.IsEmpty() )
	{
		MessageBox( 
			_T( "Please enter a valid netstream URL." ),
			_T( "musikCube" ),
			MB_ICONWARNING | MB_OK );
		return;
	}

	if ( strStream.Left( 4 ).MakeLower() != _T( "http" ) )
	{
		strStream = _T( "http://" ) + strStream;
	}

	musikCube::g_Library->AddSong( musikCore::String( strStream ) );

	GetDlgItem( IDC_NETSTREAM_TITLE )->GetWindowText( strStream );
	if ( !strStream.IsEmpty() )
	{
		// set the title
		musikCore::SongInfo info;
		musikCore::Song song;
		musikCube::g_Library->GetSongAttrFromFilename( musikCore::String( strStream ), song );
		musikCube::g_Library->GetSongInfoFromID( song.m_ID, info );
		//
		info.SetTitle( strStream.GetBuffer() );
		//
		musikCube::g_Library->SetSongInfo( info );
	}

	OnOK();
}

///////////////////////////////////////////////////


