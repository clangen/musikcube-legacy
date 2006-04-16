///////////////////////////////////////////////////

#include "stdafx.h"

#include "musikCube.h"
#include "musikDynPlaylistDlg.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikDynPlaylistDlg, CDialog)

///////////////////////////////////////////////////

CmusikDynPlaylistDlg::CmusikDynPlaylistDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CmusikDynPlaylistDlg::IDD, pParent)
{
}

///////////////////////////////////////////////////

CmusikDynPlaylistDlg::CmusikDynPlaylistDlg( CString& name )
	: CDialog(CmusikDynPlaylistDlg::IDD, NULL)
{
    m_Name = name;

}

///////////////////////////////////////////////////

CmusikDynPlaylistDlg::~CmusikDynPlaylistDlg()
{
}

///////////////////////////////////////////////////

void CmusikDynPlaylistDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_NAME, m_EditName);
    DDX_Control(pDX, IDC_QUERY, m_EditQuery);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikDynPlaylistDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikDynPlaylistDlg::OnBnClickedOk()
{
	GetDlgItem( IDC_NAME )->GetWindowText( m_Name );
	GetDlgItem( IDC_QUERY )->GetWindowText( m_Query );

	OnOK();
}

///////////////////////////////////////////////////

void CmusikDynPlaylistDlg::OnBnClickedCancel()
{
	m_Name = _T( "" );
	m_Query = _T( "" );

	OnCancel();
}

///////////////////////////////////////////////////

BOOL CmusikDynPlaylistDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString instructions = 
        _T( "Dynamic Playlists are queried in the following fashion:\n\n\t" )
		_T( "SELECT songs FROM library WHERE [user_query];\n\n" )
        _T( "Where [user_query] is a user defined SQL statement.\n\n" )
		_T( "Valid columns are: artist, album, genre, title, rating, filename, bitrate, timesplayed, lastplayed, timeadded, tracknum, and notes (for comments)\n\n" )
		_T( "Example Dynamic Playlists:\n\n\t" )
		_T( "\"rating > 3\" : all songs with a rating greater than 3\n\t" )
		_T( "\"artist like '%beatles%'\" : all artists that contain the word \"beatles\"\n\t" )
		_T( "\"filename like '%.ogg'\" : all songs with file extension \".ogg\"" );

	GetDlgItem( IDC_INSTRUCTIONS )->SetWindowText( instructions );
	GetDlgItem( IDC_NAME )->SetWindowText( m_Name );
	GetDlgItem( IDC_QUERY )->SetWindowText( m_Query );

    m_EditName.SetWindowText( m_Name );
	m_EditQuery.SetFocus();
	return TRUE;
}
