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
//   CmusikSourcesCtrl, CmusikSourcesBar
//
// Filename(s): 
//
//   musikSourceCtrl.h, musikSourceCtrl.cpp
//
// Information:
//
//   The "Sources" panel. 
// 
// Usage: 
//
//   Void.
//
// Code adapted from:
//
//   CPropTree: http://www.codeproject.com/treectrl/proptree.asp
//
///////////////////////////////////////////////////

#include "stdafx.h"

#include "musikCube.h"
#include "musikCubeFrame.h"
#include "musikSourcesCtrl.h"
#include "musikDynPlaylistDlg.h"
#include "3rdparty/CShellFileOp.h"

#include <Winbase.h>
#include <basscd.h>
#include <musikFilename.h>

///////////////////////////////////////////////////

#pragma warning (disable : 4312)	// conversion from int to void* of greater size

///////////////////////////////////////////////////

using namespace Gdiplus;
Gdiplus::Bitmap* CmusikSourcesCtrl::m_pLibOff = NULL;
Gdiplus::Bitmap* CmusikSourcesCtrl::m_pLibOn = NULL;
Gdiplus::Bitmap* CmusikSourcesCtrl::m_pNPOff = NULL;
Gdiplus::Bitmap* CmusikSourcesCtrl::m_pNPOn = NULL;
Gdiplus::Bitmap* CmusikSourcesCtrl::m_pQSOff = NULL;
Gdiplus::Bitmap* CmusikSourcesCtrl::m_pQSOn = NULL;
Gdiplus::Bitmap* CmusikSourcesCtrl::m_pCDOff = NULL;
Gdiplus::Bitmap* CmusikSourcesCtrl::m_pCDOn = NULL;
Gdiplus::Bitmap* CmusikSourcesCtrl::m_pPLOff = NULL;
Gdiplus::Bitmap* CmusikSourcesCtrl::m_pPLOn = NULL;
Gdiplus::Bitmap* CmusikSourcesCtrl::m_pDPOff = NULL;
Gdiplus::Bitmap* CmusikSourcesCtrl::m_pDPOn = NULL;
Gdiplus::Bitmap* CmusikSourcesCtrl::m_pRDOff = NULL;
Gdiplus::Bitmap* CmusikSourcesCtrl::m_pRDOn = NULL;
Gdiplus::Bitmap* CmusikSourcesCtrl::m_pNSOff = NULL;
Gdiplus::Bitmap* CmusikSourcesCtrl::m_pNSOn = NULL;
Gdiplus::Bitmap* CmusikSourcesCtrl::m_pPlugOff = NULL;
Gdiplus::Bitmap* CmusikSourcesCtrl::m_pPlugOn = NULL;
Gdiplus::Bitmap* CmusikSourcesCtrl::m_pColOff = NULL;
Gdiplus::Bitmap* CmusikSourcesCtrl::m_pColOn = NULL;

///////////////////////////////////////////////////

// CmusikSourcesBar

///////////////////////////////////////////////////

void CmusikSourcesCtrl::RefreshDevices()
{
	if ( this->m_DevicesRoot != NULL )
	{
		musikCore::PlaylistInfo info;

		musikCore::StringArray hidden = musikCube::g_Prefs->GetHiddenDevices();
		musikCore::String src_str, dev_str;

		//
		// get a list of the drives currently on the system
		//
		musikCore::StringArray curr_drives;
		GetDevices( curr_drives );

		//
		// remove devices that no longer exist
		//
		for ( size_t i = 0; i < m_VisibleDrives.size(); i++ )
		{
			bool is_active_drive = false;
			for ( size_t j = 0; j < curr_drives.size(); j++ )
			{
				if ( m_VisibleDrives[i] == curr_drives[j] )
				{
					is_active_drive = true;
					break;
				}
			}

			// is the drive hidden?
			bool is_hidden_device = false;
			for ( size_t j = 0; j < hidden.size(); j++ )
			{
				if( m_VisibleDrives[i] == hidden[j] )
				{
					is_hidden_device = true;
					break;
				}
			}

			if ( !is_active_drive || is_hidden_device )
			{
				CmusikPropTreeItem* pItem = m_Devices[i];

				if ( pItem == this->GetFocusedItem() )
				{
					this->SetFocusedItem( NULL );
				}		

				pItem->Detach();
				delete pItem;
				m_Devices.erase( m_Devices.begin() + i );

				m_VisibleDrives.erase( m_VisibleDrives.begin() + i );
				--i;
			}
		}
			

		//
		// now we add drives that don't exist
		//
		for ( size_t i = 0; i < curr_drives.size(); i++ )
		{
			std::wstring curr_drive = curr_drives[i];

			// is the drive already visible?
			bool already_visible = false;
			for( size_t j = 0; j < m_VisibleDrives.size(); j++ )
			{
				if( m_VisibleDrives[j] == curr_drive )
				{
					already_visible = true;
					break;
				}
			}

			// is the drive hidden?
			bool is_hidden_device = false;
			for ( size_t j = 0; j < hidden.size(); j++ )
			{
				if( hidden[j] == curr_drive )
				{
					is_hidden_device = true;
					break;
				}
			}

			// ... if its either, we don't want to display it
			if ( already_visible || is_hidden_device )
			{
				continue;
			}

			UINT type = GetDriveType( curr_drives[i].c_str() );
			
			switch( type )
			{
			case DRIVE_REMOVABLE:
			case DRIVE_FIXED:
			case DRIVE_REMOTE:
				{
					musikCore::String driveName = GetDeviceName( curr_drives[i] );

					info.Set( driveName, musikCore::MUSIK_PLAYLIST_TYPE_REMOVABLE, -1 );
					
					CmusikPropTreeItem *removable = InsertItem( new CmusikPropTreeItem(), m_DevicesRoot );
					removable->SetPlaylistInfo( info );
					removable->SetBitmaps( m_pRDOff, m_pRDOn );

					m_VisibleDrives.push_back( curr_drive );
					m_Devices.push_back( removable );
				}
				break;

			case DRIVE_CDROM:
				{
					int curr_drive_id = curr_drives[i][0];

					int count = 0;
					char* pStr = BASS_CD_GetDriveDescription( count );
					while ( pStr != 0 )
					{
						int id = (int)BASS_CD_GetDriveLetter( count ) + (int)'A';

						if ( id == curr_drive_id )
						{
							curr_drive_id = count;
							break;
						}

						count++;
						pStr = BASS_CD_GetDriveDescription( count );
					}

					musikCore::String driveName = GetDeviceName( curr_drives[i] );

					info.Set( driveName, musikCore::MUSIK_PLAYLIST_TYPE_CDPLAYER, curr_drive_id );

					CmusikPropTreeItem *cdplayer = InsertItem( new CmusikPropTreeItem(), m_DevicesRoot );
					cdplayer->SetPlaylistInfo( info );
					cdplayer->SetBitmaps( m_pCDOff, m_pCDOn );

					m_VisibleDrives.push_back( curr_drive );
					m_Devices.push_back( cdplayer );
				}
				break;
			default:
				break;            
			}
		}
	}
}

///////////////////////////////////////////////////

int CmusikSourcesCtrl::GetDriveCount()
{
    wchar_t logdev[1024];
    int dwSize = (int)GetLogicalDriveStrings( 1024, logdev );

    musikCore::StringArray drives;
    int count = 0;
    for ( int i = 0; i < dwSize; i++ )
    {
        if ( logdev[i] == (wchar_t)'\0' )
			count++;
    }

	return count;
}

///////////////////////////////////////////////////

musikCore::String CmusikSourcesCtrl::GetDeviceName( const musikCore::String& drive_name )
{
	wchar_t volName[128] = _T( "" );

	GetVolumeInformation(
		drive_name.c_str(),
		volName,	// volume name
		128,		// volume name buffer size
		NULL,		// serial number
		NULL,
		NULL,
		NULL,
		NULL
	);

	musikCore::String drivestr = volName;

	if ( drivestr == _T( "" ) )
	{
		drivestr = _T( "unnamed" );
	}

	drivestr += _T( " (" );
	drivestr += drive_name;
	drivestr += _T( ")" );

	drivestr.Replace( _T( "\\" ), _T( "" ) );

	return drivestr;
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::GetDevices( musikCore::StringArray& target )
{
    target.clear();

    wchar_t logdev[1024];
    int dwSize = (int)GetLogicalDriveStrings( 1024, logdev );

    wchar_t buffer[32];
    musikCore::StringArray drives;
    int at = 0;
	int count = 0;
    for ( int i = 0; i < dwSize; i++ )
    {
        wchar_t cur = logdev[i];
        buffer[at] = cur;
        
        if ( cur == (wchar_t)'\0' )
        {   
            target.push_back( buffer );
			count++;
            at = -1;
        }
        at++;
    }
}

///////////////////////////////////////////////////

CmusikSourcesBar::CmusikSourcesBar( CFrameWnd* parent, UINT dropid, UINT pldropid_r, UINT sbdropid_r )
	: CmusikDockBar( NULL, false )
{
	m_wndChild = new CmusikSourcesCtrl( parent, dropid, pldropid_r, sbdropid_r );
	m_Parent = parent;
    m_RipDlg = NULL;

	m_ForceDrawBorder = true;
}

///////////////////////////////////////////////////

CmusikSourcesBar::~CmusikSourcesBar()
{
    if ( m_RipDlg )
        delete m_RipDlg;

	delete m_wndChild;
}

///////////////////////////////////////////////////

int WM_CLOSERIPDLG = RegisterWindowMessage( _T( "CLOSERIPDLG" ) );

BEGIN_MESSAGE_MAP(CmusikSourcesBar, baseCmusikSourcesBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY( PTN_SELCHANGE, IDC_SOURCES, OnItemChanged )
	ON_COMMAND(ID_SOURCES_RENAME, OnSourcesRename)
	ON_COMMAND(ID_SOURCES_DELETE, OnSourcesDelete)
	ON_COMMAND(ID_SOURCES_SHUFFLEPLAYLIST, OnSourcesShuffleplaylist)
    ON_COMMAND(ID_SOURCES_COPYTO, OnSourcesCopyTo)
	ON_COMMAND(ID_SOURCES_EDITQUERY, OnSourcesEditquery)
    ON_COMMAND(ID_SOURCES_QUERYFREEDB, OnQueryFreeDB)
    ON_COMMAND(ID_SOURCES_RIPCD, OnSourcesRipcd)
    ON_REGISTERED_MESSAGE( WM_CLOSERIPDLG, OnCloseRipDlg )
    ON_COMMAND(ID_SOURCES_REFRESHCD, OnSourcesRefreshcd)
	ON_COMMAND(ID_SOURCES_UNMOUNT, OnSourcesUnmount)
	ON_COMMAND(ID_SOURCES_CREATESTOCK, OnCreateStockDyn)
	ON_COMMAND(ID_SOURCES_MOVEUP, OnSourcesMoveup)
	ON_COMMAND(ID_SOURCES_MOVEDOWN, OnSourcesMovedown)
	ON_COMMAND(ID_SOURCES_CLEAR, OnSourcesClear)
	ON_COMMAND(ID_SOURCES_MOVEFILESTO, OnSourcesMovefilesto)
	ON_COMMAND(ID_SOURCES_PLUGINEXECUTE, OnSourcesPluginexecute)
	ON_COMMAND(ID_SOURCES_PLUGINCONFIGURE, OnSourcesPluginconfigure)
	ON_COMMAND(ID_SOURCES_PLUGINSTOP, OnSourcesPluginstop)
	ON_COMMAND(ID_SOURCES_PLUGINABOUT, OnSourcesPluginabout)
	ON_COMMAND(ID_SOURCES_SHOWALLSONGS, OnSourcesShowallsongs)
	ON_COMMAND(ID_SOURCES_SYNCHRONIZE, OnSourcesSynchronize)
	ON_COMMAND(ID_SOURCES_CLEARCOLLECTED, OnSourcesClearcollected)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

int CmusikSourcesBar::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	if ( baseCmusikSourcesBar::OnCreate(lpCreateStruct) == -1 )
		return -1;

	// dock bar
    SetBarStyle( GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
	EnableDocking( CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT );

	// child
	if ( !m_wndChild->Create( WS_CHILD | WS_VISIBLE | PTS_NOTIFY, CRect( 0, 0, 0, 0 ), this, IDC_SOURCES ) )
		return -1;

	if ( !m_Font.CreateStockObject( DEFAULT_GUI_FONT ) )
		return -1;

	m_wndChild->SetFont( &m_Font );

	ShowGripper( false );
	m_ForceDrawBorder = false;

	return 0;
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnSize(UINT nType, int cx, int cy)
{
	CmusikDockBar::OnSize(nType, cx, cy);

	CRect client_size;
	GetClientRect( &client_size );

	m_wndChild->MoveWindow( 0, 0, client_size.Width(), client_size.Height() );
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnCreateStockDyn()
{
	musikCube::g_Library->InitDefaultDynPlaylists();
	GetCtrl()->DynPlaylistsLoad();
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnItemChanged( NMHDR* pNotifyStruct, LRESULT* plResult )
{
	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE)pNotifyStruct;
  	*plResult = 0;  

	if ( pNMPropTree->pItem )
	{
		// we don't care about root level items,
		// they just act as headers.
		if ( pNMPropTree->pItem->IsRootLevel() || pNMPropTree->pItem->IsActivated() )
			return;

		int playlist_type = pNMPropTree->pItem->GetPlaylistType();

		// if the active library is the collected library, it means
		// that the last item clicked was a collected item.  if we 
		// clicked on something else we need to swap the library
		// back with the last library
		if ( musikCube::g_Library == musikCube::g_Collected )
		{
			if ( playlist_type != MUSIK_SOURCES_TYPE_COLLECTED )
				SetMainLibrary( m_LastLib );
		}

		// send the main frame a notification first, this
		// allows the pre-playlist-switch routine to be called
		int WM_SOURCESITEMCLICKED = RegisterWindowMessage( _T( "SOURCESITEMCLICKED" ) );
		m_Parent->SendMessage( WM_SOURCESITEMCLICKED, NULL );

		switch ( playlist_type )
		{
		case musikCore::MUSIK_PLAYLIST_TYPE_LIBRARY:
			{
				SetMainLibrary( musikCube::g_MainLibrary );
				GetCtrl()->DynPlaylistsLoad();
				GetCtrl()->StdPlaylistsLoad();
				GetCtrl()->FocusLibrary( true );
			}
			break;

		case MUSIK_SOURCES_TYPE_RIPCD:
			{
				GetCtrl()->FocusNone();
				int WM_SOURCESRIPCD = RegisterWindowMessage( _T( "SOURCESRIPCD" ) );
				m_Parent->PostMessage( WM_SOURCESRIPCD, NULL );
			}
			break;

		case MUSIK_SOURCES_TYPE_QUICKSEARCH:
			GetCtrl()->QuickSearch();
			break;

		case MUSIK_SOURCES_TYPE_NEWSTDPLAYLIST:
			GetCtrl()->CreateNewStdPlaylist( musikCore::MUSIK_PLAYLIST_TYPE_STANDARD );
			break;

		case MUSIK_SOURCES_TYPE_NEWDYNPLAYLIST:
			GetCtrl()->CreateNewDynPlaylist();
			break;

		case musikCore::MUSIK_PLAYLIST_TYPE_NOWPLAYING:
			GetCtrl()->FocusNowPlaying();
			break;

		case MUSIK_SOURCES_TYPE_NETSTREAMS:
			{
				int WM_SOURCESNETSTREAMS = RegisterWindowMessage( _T( "SOURCESNETSTREAMS" ) );
				m_Parent->PostMessage( WM_SOURCESNETSTREAMS, NULL );
			}
			break;

		case musikCore::MUSIK_PLAYLIST_TYPE_CDPLAYER:
			{
				int WM_SOURCESCDAUDIO = RegisterWindowMessage( _T( "SOURCESCDAUDIO" ) );
				m_Parent->PostMessage( WM_SOURCESCDAUDIO, dynamic_cast<CmusikPropTreeItem*>( pNMPropTree->pItem )->GetPlaylistID() );
			}
			break;
		
		case musikCore::MUSIK_PLAYLIST_TYPE_STANDARD:
			{
				int WM_SOURCESSTDPLAYLIST = RegisterWindowMessage( _T( "SOURCESSTDPLAYLIST" ) );
				m_Parent->PostMessage( WM_SOURCESSTDPLAYLIST, NULL );
			}
			break;

		case musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC:
			{
				int WM_SOURCESDYNPLAYLIST = RegisterWindowMessage( _T( "SOURCESDYNPLAYLIST" ) );
				m_Parent->PostMessage( WM_SOURCESDYNPLAYLIST, NULL );
			}
			break;	

		case MUSIK_SOURCES_TYPE_COLLECTED:
			{
				m_LastLib = musikCube::g_Library;

				CmusikPropTreeItem* ptrItem = GetCtrl()->GetFocusedItem();
				
				if ( !ptrItem->m_Library )
						GetCtrl()->LoadColLib();

				if ( ptrItem->m_Library )
				{
					SetMainLibrary( ptrItem->m_Library );
					int WM_SOURCESCOLLECTED = RegisterWindowMessage( _T( "SOURCESCOLLECTED" ) );
					m_Parent->PostMessage( WM_SOURCESCOLLECTED, NULL );
				}
			}
			break;

		case musikCore::MUSIK_PLAYLIST_TYPE_REMOVABLE:
			{
				CmusikPropTreeItem* ptrItem = GetCtrl()->GetFocusedItem();
				if ( !ptrItem->m_Library )
				{       
					if ( !GetCtrl()->LoadLib( ptrItem ) )
					{
						return;
					}
				}

				if ( ptrItem->m_Library )
				{
					SetMainLibrary( ptrItem->m_Library );
					GetCtrl()->ResetItems();
					int WM_SOURCESREMOVABLEDEV = RegisterWindowMessage( _T( "SOURCESREMOVABLEDEV" ) );
					m_Parent->PostMessage( WM_SOURCESREMOVABLEDEV, NULL );
				}
			}
			break;

		case MUSIK_SOURCES_TYPE_PLUGIN:
			{
				CmusikPropTreeItem* ptrItem = GetCtrl()->GetFocusedItem();
				int id = ptrItem->GetPlaylistID();
				
				// only send the message on a left click
				if ( GetCtrl()->m_LastBtn == LFCLKITEM )
					musikCube::g_Plugins.at( id ).OnSources();
			}
			break;
		}
	}
}

///////////////////////////////////////////////////

bool CmusikSourcesCtrl::LoadLib( CmusikPropTreeItem* ptrItem )
{
	musikCore::String drive_path;
    musikCore::String drive_letter = ptrItem->GetPlaylistInfo()->GetName();

    int pos = drive_letter.find( _T( "(" ) ) + 1;
	int len = drive_letter.size();
    drive_path = drive_letter.Right( len - pos );
    drive_path = drive_path.Left( drive_path.length() - 1 );
	drive_path += "\\";

    musikCore::String fn = drive_path + _T( ".musikproject\\musik_removable_u.db" );
    musikCore::RemLibrary* pNewLib = NULL;

    if ( musikCore::Filename::FileExists( fn ) )
	{
        pNewLib = new musikCore::RemLibrary( fn );
		pNewLib->SetRemPath( drive_path, true );
	}

    else
    {
        int res = MessageBox( 
            _T( "You have not yet built a library for this drive, would you like to do so now?" ),
            _T( "musikCube" ),
            MB_YESNO | MB_ICONQUESTION );

        if ( res == IDYES )
        {
            pNewLib = new musikCore::RemLibrary( fn );
            pNewLib->AddPath( drive_path );
			pNewLib->SetRemPath( drive_path, false );

            musikCore::BatchAdd* params = new musikCore::BatchAdd(
                NULL,
                drive_path + _T( "*.*" ),
                NULL,
                pNewLib,
                NULL,
                musikCube::g_Functor );

            musikCore::BatchAddTask* ptask = new musikCore::BatchAddTask();
            ptask->m_Params = params;

            pNewLib->QueueTask( ptask );
        }
        else
        {
            KillFocus();
            return false;
        }
    }

    if ( pNewLib )
    {
        if ( ptrItem->m_Library )
            delete ptrItem->m_Library;

        ptrItem->m_Library = pNewLib;
    }

    return true;
}

///////////////////////////////////////////////////

void CmusikSourcesBar::ShowMenu( bool force_show )
{
	if ( GetCtrl()->m_EditInPlace.IsVisible() )
	{
		GetCtrl()->m_EditInPlace.Cancel();
	}

	CPoint pos;
	::GetCursorPos( &pos );
	ScreenToClient( &pos );
	
	CmusikPropTreeItem* pItem = GetCtrl()->FindItem( pos );

	if ( !pItem || pItem->IsRootLevel() )
	{
		if ( pItem != GetCtrl()->m_DynPlaylistRoot )
		{
			return;
		}
	}

	int type = musikCore::MUSIK_PLAYLIST_TYPE_UNKNOWN;
	type = pItem->GetPlaylistType();

	CMenu main_menu;
	CMenu* popup_menu;

	main_menu.LoadMenu( IDR_SOURCES_MENU );
	popup_menu = main_menu.GetSubMenu( 0 );

    // shuffle / clear now playing
	if ( type == musikCore::MUSIK_PLAYLIST_TYPE_NOWPLAYING )
	{
		popup_menu->EnableMenuItem( ID_SOURCES_SHUFFLEPLAYLIST, MF_ENABLED );
		popup_menu->EnableMenuItem( ID_SOURCES_CLEAR, MF_ENABLED );
	}
    else 
	{	
		popup_menu->RemoveMenu( ID_SOURCES_SHUFFLEPLAYLIST, MF_BYCOMMAND );
		popup_menu->RemoveMenu( ID_SOURCES_CLEAR, MF_BYCOMMAND );
	}

	// rename, delete
	if ( type == musikCore::MUSIK_PLAYLIST_TYPE_STANDARD || 
		 type == musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC )
	{
		popup_menu->EnableMenuItem( ID_SOURCES_RENAME, MF_ENABLED );
		popup_menu->EnableMenuItem( ID_SOURCES_DELETE, MF_ENABLED );
		popup_menu->EnableMenuItem( ID_SOURCES_MOVEUP, MF_ENABLED );
		popup_menu->EnableMenuItem( ID_SOURCES_MOVEDOWN, MF_ENABLED );
	}
    else
    {
		popup_menu->RemoveMenu( ID_SOURCES_RENAME, MF_BYCOMMAND );
		popup_menu->RemoveMenu( ID_SOURCES_DELETE, MF_BYCOMMAND );
		popup_menu->RemoveMenu( ID_SOURCES_MOVEUP, MF_BYCOMMAND );
		popup_menu->RemoveMenu( ID_SOURCES_MOVEDOWN, MF_BYCOMMAND );
	}

    // copy to / move to
	if ( type == musikCore::MUSIK_PLAYLIST_TYPE_STANDARD || 
		 type == musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC || 
         type == musikCore::MUSIK_PLAYLIST_TYPE_NOWPLAYING )
    {
        popup_menu->EnableMenuItem( ID_SOURCES_COPYTO, MF_ENABLED );
        popup_menu->EnableMenuItem( ID_SOURCES_MOVEFILESTO, MF_ENABLED );
	}
    else
    {
		popup_menu->RemoveMenu( ID_SOURCES_COPYTO, MF_BYCOMMAND );
        popup_menu->RemoveMenu( ID_SOURCES_MOVEFILESTO, MF_BYCOMMAND );
	}

    // edit query
	if ( type == musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC )
		popup_menu->EnableMenuItem( ID_SOURCES_EDITQUERY, MF_ENABLED );
	else
		popup_menu->RemoveMenu( ID_SOURCES_EDITQUERY, MF_BYCOMMAND );
	
    // cdrom functions
    if ( type == musikCore::MUSIK_PLAYLIST_TYPE_CDPLAYER )
    {
        popup_menu->EnableMenuItem( ID_SOURCES_RIPCD, MF_ENABLED );	
        popup_menu->EnableMenuItem( ID_SOURCES_QUERYFREEDB, MF_ENABLED );
        popup_menu->EnableMenuItem( ID_SOURCES_REFRESHCD, MF_ENABLED );
    }
	else
    {
		popup_menu->RemoveMenu( ID_SOURCES_RIPCD, MF_BYCOMMAND );
		popup_menu->RemoveMenu( ID_SOURCES_QUERYFREEDB, MF_BYCOMMAND );
		popup_menu->RemoveMenu( ID_SOURCES_REFRESHCD, MF_BYCOMMAND );
    }

	if ( type == MUSIK_SOURCES_TYPE_PLUGIN )
	{
		int id = pItem->GetPlaylistID();
		CmusikPlugin& plugin = musikCube::g_Plugins.at( id );
		popup_menu->EnableMenuItem( ID_SOURCES_PLUGINEXECUTE, plugin.CanExecute() ? MF_ENABLED : (MF_DISABLED|MF_GRAYED) );	
		popup_menu->EnableMenuItem( ID_SOURCES_PLUGINCONFIGURE, plugin.CanConfigure() ? MF_ENABLED : (MF_DISABLED|MF_GRAYED) );
		popup_menu->EnableMenuItem( ID_SOURCES_PLUGINSTOP, plugin.CanStop() ? MF_ENABLED : (MF_DISABLED|MF_GRAYED) );
		popup_menu->EnableMenuItem( ID_SOURCES_PLUGINABOUT, plugin.CanAbout() ? MF_ENABLED : (MF_DISABLED|MF_GRAYED) );
	}
	else
	{
		popup_menu->RemoveMenu( ID_SOURCES_PLUGINEXECUTE, MF_BYCOMMAND );
		popup_menu->RemoveMenu( ID_SOURCES_PLUGINCONFIGURE, MF_BYCOMMAND );
		popup_menu->RemoveMenu( ID_SOURCES_PLUGINSTOP, MF_BYCOMMAND );
		popup_menu->RemoveMenu( ID_SOURCES_PLUGINABOUT, MF_BYCOMMAND );
	}

	if ( type == MUSIK_SOURCES_TYPE_COLLECTED )
		popup_menu->EnableMenuItem( ID_SOURCES_CLEARCOLLECTED, MF_ENABLED );	
	else
		popup_menu->RemoveMenu( ID_SOURCES_CLEARCOLLECTED, MF_BYCOMMAND );

	// "show all songs"
	if ( type == musikCore::MUSIK_PLAYLIST_TYPE_LIBRARY )
		popup_menu->EnableMenuItem( ID_SOURCES_SHOWALLSONGS, MF_ENABLED );	
	else
		popup_menu->RemoveMenu( ID_SOURCES_SHOWALLSONGS, MF_BYCOMMAND );

	// "unload device" & "synchronize device"
	if ( type == musikCore::MUSIK_PLAYLIST_TYPE_REMOVABLE && pItem->m_Library != NULL )
	{
		popup_menu->EnableMenuItem( ID_SOURCES_UNMOUNT, MF_ENABLED );	
		popup_menu->EnableMenuItem( ID_SOURCES_SYNCHRONIZE, MF_ENABLED );	
	}
	else
	{
		popup_menu->RemoveMenu( ID_SOURCES_UNMOUNT, MF_BYCOMMAND );
		popup_menu->RemoveMenu( ID_SOURCES_SYNCHRONIZE, MF_BYCOMMAND );	
	}

	if ( pItem == GetCtrl()->m_DynPlaylistRoot )
		popup_menu->EnableMenuItem( ID_SOURCES_CREATESTOCK, MF_ENABLED );
	else
		popup_menu->RemoveMenu( ID_SOURCES_CREATESTOCK, MF_BYCOMMAND );

	ClientToScreen( &pos );
	popup_menu->TrackPopupMenu( 0, pos.x, pos.y, this );
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnQueryFreeDB()
{
	int WM_SOURCESQUERYFREEDB = RegisterWindowMessage( _T( "SOURCESQUERYFREEDB" ) );
	m_Parent->PostMessage( WM_SOURCESQUERYFREEDB, NULL );
}

///////////////////////////////////////////////////

void CmusikSourcesBar::TogglePluginsVisible()
{
	musikCube::g_Prefs->SetPluginsVisible( !musikCube::g_Prefs->IsPluginsVisible() );
	GetCtrl()->ResetItems();
}

///////////////////////////////////////////////////

void CmusikSourcesBar::ToggleDynPlaylistsVisible()
{
	musikCube::g_Prefs->SetDynPlaylistsVisible( !musikCube::g_Prefs->IsDynPlaylistsVisible() );
	GetCtrl()->ResetItems();
}

///////////////////////////////////////////////////

void CmusikSourcesBar::ToggleStdPlaylistsVisible()
{
	musikCube::g_Prefs->SetPlaylistsVisible( !musikCube::g_Prefs->IsPlaylistsVisible() );
	GetCtrl()->ResetItems();
}

///////////////////////////////////////////////////

void CmusikSourcesBar::ToggleDevicesVisible()
{
	musikCube::g_Prefs->SetDevicesVisible( !musikCube::g_Prefs->IsDevicesVisible() );
	GetCtrl()->ResetItems();
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnSourcesCopyTo()
{
    musikCore::StringArray files;
    GetCtrl()->GetSelFilenames( GetCtrl()->GetFocusedItem(), files );
	//
	GetCtrl()->SourcesFileOp( FO_COPY, files );
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnSourcesMovefilesto()
{
    musikCore::StringArray files;
    GetCtrl()->GetSelFilenames( GetCtrl()->GetFocusedItem(), files );
	//
	GetCtrl()->SourcesFileOp( FO_MOVE, files );
}

///////////////////////////////////////////////////

// http://www.codeguru.com/forum/printthread.php?t=309472

int CALLBACK BrowseCallbackProc( HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData )
{
	switch (uMsg)
	{
	case BFFM_INITIALIZED :
		if(lpData)
		  ::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		break;
	default:
		break;
	}
	return 0; 
}

///////////////////////////////////////////////////

musikCore::String CmusikSourcesCtrl::SourcesFileOp( DWORD mode, const musikCore::StringArray& files, musikCore::String default_path )
{
    TCHAR path[MAX_PATH] = _T( "" );

	if ( files.size() )
    {
	    BROWSEINFO bi = { 0 };
	    bi.lpszTitle = _T( "Please select a target directory." );
	    bi.hwndOwner = GetSafeHwnd();
        bi.ulFlags |= BIF_NEWDIALOGSTYLE;
		bi.pidlRoot = NULL;

		if ( default_path != _T( "" ) )
		{
			bi.lParam =  (long)default_path.c_str(); 
			bi.lpfn	= BrowseCallbackProc;
		}

	    LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );

        if ( pidl != 0 )
        {
		    // get the name of the folder and put it in path
		    SHGetPathFromIDList ( pidl, path );

            // free memory used
		    IMalloc * imalloc = 0;
		    if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
		    {
			    imalloc->Free ( pidl );
			    imalloc->Release ( );
		    }
			
			// use Michael Dunn's convenient SHFileOperation wrapper to
			// perform the copy using Window's shell api...
			BOOL apicalled;
			CShellFileOp copy;

			for ( size_t i = 0; i < files.size(); i++ )
			{
				copy.AddSourceFile( files.at( i ) );
			}
			copy.AddDestFile( path );
			
			copy.SetOperationFlags(
				mode,		     // op type
				AfxGetMainWnd(), // handle to main window
				FALSE,           // flag - silent mode?
				FALSE,           // flag - allow undo?
				FALSE,           // flag - should wild cards affect files only?
				FALSE,           // flag - suppress confirmation messages?
				TRUE,            // flag - suppress confirmation messages when making directories?
				FALSE,           // flag - rename files when name collisions occur?
				FALSE );         // flag - simple progress dialog?

			if ( !copy.Go( &apicalled ) )
				MessageBox(
				_T( "File operation was unsuccessful." ),
				_T( "musikCube" ),
				MB_ICONWARNING | MB_OK );
        }
    }

	return path;
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnSourcesRename()
{
	GetCtrl()->RenameItem();
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnSourcesEditquery()
{
	CmusikDynPlaylistDlg dlg;
	
	CmusikPropTreeItem* pItem = GetCtrl()->GetFocusedItem();
	if ( pItem )
	{
		musikCore::String name, query;

		musikCube::g_Library->GetDynPlaylistInfo( 
			pItem->GetPlaylistID(),
			name,
			query );

		dlg.SetName( name );
		dlg.SetQuery( query );
        GetCtrl()->FocusNone();
		int ret = dlg.DoModal();

		if ( ret == IDOK )
		{
			musikCube::g_Library->RewriteDynPlaylistQuery( 
				pItem->GetPlaylistID(),
				dlg.GetQuery() );

			if ( name != dlg.GetName() )
			{
				musikCube::g_Library->RenameDynPlaylist(
					pItem->GetPlaylistID(),
					dlg.GetName() );
				GetCtrl()->DynPlaylistsLoad();
			}
		}
	}
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnSourcesDelete()
{
	GetCtrl()->DeleteSel();
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnSourcesShuffleplaylist()
{
	musikCube::g_Player->Shuffle();

	CmusikPropTreeItem* pItem = GetCtrl()->GetFocusedItem();
	if ( pItem )
	{
		if ( pItem->GetPlaylistType() == musikCore::MUSIK_PLAYLIST_TYPE_NOWPLAYING )
		{
			int WM_UPDATEPLAYLIST = RegisterWindowMessage( _T( "UPDATEPLAYLIST" ) );
			GetCtrl()->m_Parent->PostMessage( WM_UPDATEPLAYLIST );
		}
	}
}

///////////////////////////////////////////////////

// CmusikSourcesCtrl

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC( CmusikSourcesCtrl, CmusikPropTree )

///////////////////////////////////////////////////

int WM_SOURCES_EDIT_COMMIT = RegisterWindowMessage( _T( "MUSIKEDITCOMMIT" ) );
int WM_SOURCES_EDIT_CANCEL = RegisterWindowMessage( _T( "MUSIKEDITCANCEL" ) );
int WM_SOURCES_EDIT_CHANGE = RegisterWindowMessage( _T( "MUSIKEDITCHANGE" ) );
const unsigned int SOURCES_DEVICE_TIMER_ID = WM_USER + 25;

///////////////////////////////////////////////////

CmusikSourcesCtrl::CmusikSourcesCtrl( CFrameWnd* parent, UINT dropid, UINT pldropid_r, UINT sbdropid_r )
	: CmusikPropTree( dropid )
{
	m_DropTarget		= new CmusikSourcesDropTarget( this, dropid, pldropid_r, sbdropid_r );
	m_Parent			= parent;
	m_LibRoot			= NULL;
	m_StdPlaylistRoot	= NULL;
	m_DynPlaylistRoot	= NULL;
	m_DevicesRoot		= NULL;
	m_PluginsRoot		= NULL;
	m_Collected			= NULL;
	m_DropArrange		= false;
	m_Startup			= true;
	m_MouseTrack		= false;
	m_DevicesTimer		= NULL;
	m_DragAndDropInfo	= NULL;
}

///////////////////////////////////////////////////

CmusikSourcesCtrl::~CmusikSourcesCtrl()
{
	if ( m_DropTarget )
	{
		m_DropTarget->Revoke();
		delete m_DropTarget;
	}

	this->DeleteAllItems( false );
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP( CmusikSourcesCtrl, CmusikPropTree )

	// mfc message maps
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_KEYDOWN()
	ON_WM_CONTEXTMENU()
	ON_WM_TIMER()

	// custom message maps
	ON_REGISTERED_MESSAGE(WM_SOURCES_EDIT_COMMIT, OnEditCommit)
	ON_REGISTERED_MESSAGE(WM_SOURCES_EDIT_CANCEL, OnEditCancel)
	ON_REGISTERED_MESSAGE(WM_SOURCES_EDIT_CHANGE, OnEditChange)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)

END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnTimer( UINT nIDEvent )
{
	if ( nIDEvent == m_DevicesTimer )
	{
		RefreshDevices();
		this->RefreshItems();
	}
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::ResetItems()
{
	this->SetRedraw( FALSE );

	DetachAllItems();

	musikCore::PlaylistInfo info;
	
	// browse
	if ( !m_LibRoot )
	{
		info.Set( _T( "Browse" ), -1, -1 );

		m_LibRoot = new CmusikPropTreeItem();
		m_LibRoot->SetPlaylistInfo( info );
		m_LibRoot->Expand( true );	
	}

    InsertItem( m_LibRoot );
	LibLoad();

	// devices
	if ( m_DevicesTimer )
	{
		KillTimer( m_DevicesTimer );
		m_DevicesTimer = NULL;
	}

	if ( musikCube::g_Prefs->IsDevicesVisible() )
	{
		if ( !m_DevicesRoot )
		{
			info.Set( _T( "Devices" ), -1, -1 );
			m_DevicesRoot = new CmusikPropTreeItem();
			m_DevicesRoot->SetPlaylistInfo( info );
			m_DevicesRoot->Expand( true );
		}

		InsertItem( m_DevicesRoot );

		for ( size_t i = 0; i < m_Devices.size(); i++ )
		{
			InsertItem( m_Devices[i], m_DevicesRoot );
		}

		RefreshDevices();
		m_DevicesTimer = SetTimer( SOURCES_DEVICE_TIMER_ID, 2000, NULL );
	}	
    
	// standard playlists
	if ( musikCube::g_Prefs->IsPlaylistsVisible() )
	{
		if ( !m_StdPlaylistRoot )
		{
			info.Set( _T( "Playlists" ), -1, -1 );
			m_StdPlaylistRoot = new CmusikPropTreeItem();
			m_StdPlaylistRoot->SetPlaylistInfo( info );
			m_StdPlaylistRoot->Expand( true );
		}

		InsertItem( m_StdPlaylistRoot );
		StdPlaylistsLoad();
	}

	// dynamic playlists
	if ( musikCube::g_Prefs->IsDynPlaylistsVisible() )
	{
		if ( !m_DynPlaylistRoot )
		{
			info.Set( _T( "Dynamic Playlists" ), -1, -1 );
			m_DynPlaylistRoot = new CmusikPropTreeItem();
			m_DynPlaylistRoot->SetPlaylistInfo( info );
			m_DynPlaylistRoot->Expand( true );
		}

		InsertItem( m_DynPlaylistRoot );
		DynPlaylistsLoad();
	}

	// plugins
	if (  musikCube::g_Prefs->IsPluginsVisible() )
	{
		if ( !m_PluginsRoot )
		{
			info.Set( _T( "Plugins" ), -1, -1 );
			m_PluginsRoot = new CmusikPropTreeItem();
			m_PluginsRoot->SetPlaylistInfo( info );
			m_PluginsRoot->Expand( TRUE );
		}

		InsertItem( m_PluginsRoot );
		PluginsLoad();
	}

	UpdateResize();

	this->SetRedraw( TRUE );
	this->RedrawWindow();
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::FocusLibrary( bool msg_parent )
{
	KillFocus( false );

	m_Lib.at( 0 )->Select( TRUE );
	SetFocusedItem( m_Lib.at( 0 ) );
	musikCube::g_PlaylistType = musikCore::MUSIK_PLAYLIST_TYPE_LIBRARY;

	if ( msg_parent )
	{
		int WM_SOURCESLIBRARY = RegisterWindowMessage( _T( "SOURCESLIBRARY" ) );
		m_Parent->PostMessage( WM_SOURCESLIBRARY, NULL );
	}
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::FocusQuickSearch()
{
	KillFocus( false );

	m_Search->Select( TRUE );
	SetFocusedItem( m_Search );
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::FocusNowPlaying( bool msg_parent )
{
	KillFocus( false );

	m_Lib.at( 1 )->Select( TRUE );
	SetFocusedItem( m_Lib.at( 1 ) );
	musikCube::g_PlaylistType = musikCore::MUSIK_PLAYLIST_TYPE_NOWPLAYING;

	if ( msg_parent )
	{
		int WM_SOURCESNOWPLAYING = RegisterWindowMessage( _T( "SOURCESNOWPLAYING" ) );
		m_Parent->SendMessage( WM_SOURCESNOWPLAYING, NULL );	// needs to happen synchronously, especially
																// during startup to remember last played
	}
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::FocusItem( CmusikPropTreeItem* pItem, bool msg_parent )
{
	if ( !pItem )
		return;

	m_Hovered = pItem;
	pItem->Select( TRUE );
	KillFocus( false );
	SetFocusedItem( pItem );
	musikCube::g_PlaylistType = pItem->GetPlaylistType();
	
	if ( msg_parent )
	{
		switch ( musikCube::g_PlaylistType )
		{
		case musikCore::MUSIK_PLAYLIST_TYPE_LIBRARY:
			{
				int WM_SOURCESLIBRARY = RegisterWindowMessage( _T( "SOURCESLIBRARY" ) );
				m_Parent->SendMessage( WM_SOURCESLIBRARY, NULL );
			}
			break;

		case musikCore::MUSIK_PLAYLIST_TYPE_NOWPLAYING:
			{
				int WM_SOURCESNOWPLAYING = RegisterWindowMessage( _T( "SOURCESNOWPLAYING" ) );
				m_Parent->SendMessage( WM_SOURCESNOWPLAYING, NULL );	
			}
			break;

		case musikCore::MUSIK_PLAYLIST_TYPE_CDPLAYER:
			{
				int WM_SOURCESCDAUDIO = RegisterWindowMessage( _T( "SOURCESCDAUDIO" ) );
				m_Parent->SendMessage( WM_SOURCESCDAUDIO, NULL );
			}
			break;

		case musikCore::MUSIK_PLAYLIST_TYPE_STANDARD:
			{
				int WM_SOURCESSTDPLAYLIST = RegisterWindowMessage( _T( "SOURCESSTDPLAYLIST" ) );
				m_Parent->SendMessage( WM_SOURCESSTDPLAYLIST, NULL );
			}
			break;

		case musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC:
			{	
				int WM_SOURCESDYNPLAYLIST = RegisterWindowMessage( _T( "SOURCESDYNPLAYLIST" ) );
				m_Parent->SendMessage( WM_SOURCESDYNPLAYLIST, NULL );
			}
			break;

		case musikCore::MUSIK_PLAYLIST_TYPE_REMOVABLE:
			{
				int WM_SOURCESREMOVABLEDEV = RegisterWindowMessage( _T( "SOURCESREMOVABLEDEV" ) );
				m_Parent->SendMessage( WM_SOURCESREMOVABLEDEV, NULL );	
			}		
			break;
		}
	}
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::FocusNone()
{
	KillFocus( false );
}

///////////////////////////////////////////////////

int CmusikSourcesCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CmusikPropTree::OnCreate(lpCreateStruct) == -1 )
		return -1;

	if ( m_DropTarget )
	{
		m_DropTarget->Register( this );
	}

	// quick search
	m_EditInPlace.Create( WS_CHILD | WS_CLIPCHILDREN, CRect( 0, 0, 0, 0 ), this, IDC_EDITINPLACE );
	m_EditInPlace.EnableWindow( FALSE );

	CFont font;
	font.CreateStockObject( DEFAULT_GUI_FONT );

	m_EditInPlace.SetFont( &font );

	ResetItems();

	m_DevicesTimer = SetTimer( SOURCES_DEVICE_TIMER_ID, 5000, NULL );

	return 0;
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::LibLoad()
{
	if ( m_Lib.size() > 0 )
	{
		for ( size_t i = 0; i < m_Lib.size(); i++ )
		{
			InsertItem( m_Lib[i], m_LibRoot );
		}
	}

	else
	{
		musikCore::PlaylistInfo info;
		CmusikPropTreeItem *lib, *nowplaying, *netstreams, *search;

		// library
		info.Set( "Library", musikCore::MUSIK_PLAYLIST_TYPE_LIBRARY, NULL );
		lib = new CmusikPropTreeItem();
		lib->SetPlaylistInfo( info );
		lib->SetBitmaps( m_pLibOff, m_pLibOn );
		InsertItem( lib, m_LibRoot );
		m_Lib.push_back( lib );

		// now playing
		info.Set( "Now Playing", musikCore::MUSIK_PLAYLIST_TYPE_NOWPLAYING, NULL );
		nowplaying = new CmusikPropTreeItem();
		nowplaying->SetPlaylistInfo( info );
		nowplaying->SetBitmaps( m_pNPOff, m_pNPOn );
		InsertItem( nowplaying, m_LibRoot );
		m_Lib.push_back( nowplaying );

		// collected
		info.Set( "Collected", MUSIK_SOURCES_TYPE_COLLECTED, NULL );
		m_Collected = new CmusikPropTreeItem();
		m_Collected->SetPlaylistInfo( info );
		m_Collected->SetBitmaps( m_pColOff, m_pColOn );
		InsertItem( m_Collected, m_LibRoot );
		m_Lib.push_back( m_Collected );
		

		// net radio
		info.Set( "Net Radio", MUSIK_SOURCES_TYPE_NETSTREAMS, NULL );
		netstreams = new CmusikPropTreeItem();
		netstreams->SetPlaylistInfo( info );
		netstreams->SetBitmaps( m_pNSOff, m_pNSOn );
		InsertItem( netstreams, m_LibRoot );
		m_Lib.push_back( netstreams );

		// search
		info.Set( "Search...", MUSIK_SOURCES_TYPE_QUICKSEARCH, -1 ); 
		search = new CmusikPropTreeItem();
		search->SetPlaylistInfo( info );
		search->SetBitmaps( m_pQSOff, m_pQSOn );
		InsertItem( search, m_LibRoot );
		m_Lib.push_back( search );
		m_Search = search;
	}
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::PluginsLoad()
{
	// load the plugins...
	if ( m_Plugins.size() )
	{
		for ( size_t i = 0; i < m_Plugins.size(); i++ )
		{
			DeleteItem( m_Plugins.at( i ) );
		}
	}
	m_Plugins.clear();

	CmusikPropTreeItem* temp;
	for ( size_t i = 0; i < musikCube::g_Plugins.size(); i++ )
	{
		if ( musikCube::g_Plugins.at( i ).InSources() )
		{
			musikCore::PlaylistInfo info;
			info.Set( musikCube::g_Plugins.at( i ).GetPluginName(), MUSIK_SOURCES_TYPE_PLUGIN, i );
			
			temp = InsertItem( new CmusikPropTreeItem(), m_PluginsRoot );
			temp->SetPlaylistInfo( info );
			temp->SetBitmaps( m_pPlugOff, m_pPlugOn );

			m_Plugins.push_back( temp );
		}
	}
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::StdPlaylistsLoad()
{
	if ( !musikCube::g_Prefs->IsPlaylistsVisible() )
		return;

	// load the playlists...
	if ( m_StdPlaylists.size() )
	for ( size_t i = 0; i < m_StdPlaylists.size(); i++ )
		DeleteItem( m_StdPlaylists.at( i ) );
	m_StdPlaylists.clear();

	musikCore::PlaylistInfoArray items;
	musikCube::g_Library->GetAllStdPlaylists( items );

	CmusikPropTreeItem* temp;
	for ( size_t i = 0; i < items.size(); i++ )
	{
		temp = InsertItem( new CmusikPropTreeItem(), m_StdPlaylistRoot );
		temp->SetPlaylistInfo( items.at( i ) );
		temp->SetBitmaps( m_pPLOff, m_pPLOn );
		m_StdPlaylists.push_back( temp );
	}

	// load the "Create..."
	musikCore::PlaylistInfo info;
	info.Set( "Create...", MUSIK_SOURCES_TYPE_NEWSTDPLAYLIST, -1 ); 
	temp = InsertItem( new CmusikPropTreeItem(), m_StdPlaylistRoot );
	temp->SetPlaylistInfo( info );
	temp->SetBitmaps( m_pPLOff, m_pPLOn );
	m_StdPlaylists.push_back( temp );
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::DeleteLastPlaylist( int type )
{
	musikCore::PlaylistInfoArray items;

    switch ( type )
    {
    case musikCore::MUSIK_PLAYLIST_TYPE_STANDARD:
        {
            musikCube::g_Library->GetAllStdPlaylists( items );
            musikCube::g_Library->DeleteStdPlaylist( items.at( items.size() - 1 ).GetID() );
        }
        break;

    case musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC:
        {
	        musikCube::g_Library->GetAllDynPlaylists( items );
            musikCube::g_Library->DeleteDynPlaylist( items.at( items.size() - 1 ).GetID() );
        }
        break;
    }
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::DynPlaylistsLoad()
{
	if ( !musikCube::g_Prefs->IsDynPlaylistsVisible() )
		return;

	// load the playlists...
	for ( size_t i = 0; i < m_DynPlaylists.size(); i++ )
		DeleteItem( m_DynPlaylists.at( i ) );

	m_DynPlaylists.clear();

	musikCore::PlaylistInfoArray items;
	musikCube::g_Library->GetAllDynPlaylists( items, false );

	CmusikPropTreeItem* temp;
	for ( size_t i = 0; i < items.size(); i++ )
	{
		temp = InsertItem( new CmusikPropTreeItem(), m_DynPlaylistRoot );
		temp->SetPlaylistInfo( items.at( i ) );
		temp->SetBitmaps( m_pDPOff, m_pDPOn );

		m_DynPlaylists.push_back( temp );
	}

	// load the "Create..."
	musikCore::PlaylistInfo info;
	info.Set( "Create...", MUSIK_SOURCES_TYPE_NEWDYNPLAYLIST, -1 ); 
	temp = InsertItem( new CmusikPropTreeItem(), m_DynPlaylistRoot );
	temp->SetPlaylistInfo( info );
	temp->SetBitmaps( m_pDPOff, m_pDPOn );
	m_DynPlaylists.push_back( temp );
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnDropFiles( HDROP hDropInfo, bool right_button )
{
	// set cursor back to hour glass
	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	// see if the drag landed on an existing
	// playlist, if it did, we'll append
	CPoint pos;
	::GetCursorPos( &pos );
	ScreenToClient( &pos );

	CmusikPropTreeItem* pItem = FindItem( pos );

	// no selected item to drop on
	if ( !pItem || pItem->IsRootLevel() )
	{
		DragFinish( hDropInfo );
		return;
	}

	// dnd vars
	size_t nNumFiles;
	TCHAR szNextFile [MAX_PATH];
	SHFILEINFO  rFileInfo;
	nNumFiles = DragQueryFile ( hDropInfo, -1, NULL, 0 );

	// build a list of dnd filenames
	musikCore::StringArray* files = new musikCore::StringArray();
	musikCore::String sTemp;
	for ( size_t i = 0; i < nNumFiles; i++ )
	{
		if ( DragQueryFile( hDropInfo, i, szNextFile, MAX_PATH ) > 0 )
		{
			// at a dir: recurse 
			SHGetFileInfo( szNextFile, 0, &rFileInfo, sizeof( rFileInfo ), SHGFI_ATTRIBUTES );
			if ( rFileInfo.dwAttributes & SFGAO_FOLDER )
			{
				sTemp = szNextFile;
				sTemp += "\\*.*";

                musikCore::Dir::OpenDir( sTemp, files );
			}		

			// att a file: add to list
			else
			{
				files->push_back( szNextFile );
			}
		}
	}

	// distribute the drop event
	bool delete_files = true;
	bool success = false;
	switch( pItem->GetPlaylistType() )
	{
	case musikCore::MUSIK_PLAYLIST_TYPE_STANDARD:
		success = ( musikCore::MUSIK_LIBRARY_OK == musikCube::g_Library->AppendStdPlaylist( pItem->GetPlaylistID(), *files ) );
		break;

	case musikCore::MUSIK_PLAYLIST_TYPE_LIBRARY:
		success = this->DropAddToLibrary( musikCube::g_MainLibrary, files );
		delete_files = false;
		break;

	case musikCore::MUSIK_PLAYLIST_TYPE_REMOVABLE:
		success = this->DropOnRemovableDevice( files, pItem );
		delete_files = false;
		break;

	case musikCore::MUSIK_PLAYLIST_TYPE_NOWPLAYING:
		success = this->DropAddNowPlaying( files );
		break;

	default:
		break;
	}

	DragFinish( hDropInfo );
	if ( delete_files )
	{
		delete files;
	}

	// if the mouse is no longer in the window, be sure
	// to set the hovered item to NULL and redraw.
	CRect rcWindow;
	::GetWindowRect( this->GetSafeHwnd(), &rcWindow );

	CPoint mousePos;
	::GetCursorPos( &mousePos );

	if ( !rcWindow.PtInRect( mousePos ) )
	{
		OnDragLeave();
	}
}

///////////////////////////////////////////////////

bool CmusikSourcesCtrl::DropAddToLibrary( musikCore::Library* pLibrary, musikCore::StringArray* pFiles )
{
	musikCore::BatchAdd* batchAddParams;
	musikCore::BatchAddTask* batchAddThread;

	batchAddParams = new musikCore::BatchAdd();
	batchAddParams->m_Library = pLibrary;
	batchAddParams->m_Files = pFiles;
	batchAddParams->m_DeleteFilelist = true;
	batchAddParams->m_Functor = musikCube::g_Functor;

	batchAddThread = new musikCore::BatchAddTask();
	batchAddThread->m_Params = batchAddParams;
	
	pLibrary->QueueTask( batchAddThread );

	return true;
}

///////////////////////////////////////////////////

bool CmusikSourcesCtrl::DropOnRemovableDevice( musikCore::StringArray* pFiles, CmusikPropTreeItem* pItem )
{
	musikCore::Library*& pItemLib = pItem->m_Library;

	if ( !pItemLib )
	{
		this->LoadLib( pItem );
	}

	bool success = false;
	if ( pItemLib )
	{
		CPoint pos;
		::GetCursorPos( &pos );

		CMenu popup;
		popup.LoadMenu( IDR_DROPDEVICE_MENU );

		CMenu* pSubMenu = popup.GetSubMenu( 0 );
		int selMenu = (int)pSubMenu->TrackPopupMenu( TPM_RETURNCMD, pos.x, pos.y, this, NULL );

		switch( selMenu )
		{
		case ID_DROPDEVICE_COPYFILESTODEVICE:
			success = this->DropCopyFilesToDevice( pFiles, pItem );
			break;

		case ID_DROPDEVICE_ADDTODEVICE:
			success = this->DropAddToDevice( pFiles, pItem );
			break;
		}
	}
	else
	{
		MessageBox(
			_T( "No library has been built for this device yet. Please select it to build a library." ),
			_T( "musikCube" ),
			MB_OK | MB_ICONINFORMATION
		);

		delete pFiles;
	}

	return success;
}

///////////////////////////////////////////////////

bool CmusikSourcesCtrl::DropCopyFilesToDevice( musikCore::StringArray* pFiles, CmusikPropTreeItem* pItem )
{
	musikCore::String copyPath = SourcesFileOp( 
			FO_COPY, 
			*pFiles, 
			((musikCore::RemLibrary*)pItem->m_Library)->GetRemPath() 
		);

	if ( copyPath != _T( "" ) )
	{
		musikCore::BatchAdd* batchAddParams;
		musikCore::BatchAddTask* batchAddThread;

		batchAddParams = new musikCore::BatchAdd();
		batchAddParams->m_Library = pItem->m_Library;
		batchAddParams->m_Directory = copyPath + _T( "\\*.*" );
		batchAddParams->m_Functor = musikCube::g_Functor;

		batchAddThread = new musikCore::BatchAddTask();
		batchAddThread->m_Params = batchAddParams;
		
		pItem->m_Library->QueueTask( batchAddThread );
	}

	delete pFiles;
	return true;
}

///////////////////////////////////////////////////

bool CmusikSourcesCtrl::DropAddToDevice( musikCore::StringArray* pFiles, CmusikPropTreeItem* pItem )
{
	bool success = this->DropAddToLibrary( pItem->m_Library, pFiles );

	if ( !success )
	{
		delete pFiles;
	}

    return success;
}

///////////////////////////////////////////////////

bool CmusikSourcesCtrl::DropAddNowPlaying( musikCore::StringArray* files )
{
	bool was_nowplaying_empty = musikCube::g_Player->GetPlaylist()->GetCount() ? false : true;
	bool begin_trans = false;

	musikCube::g_Library->BeginTransaction();
	musikCore::Song song;
	for ( size_t i = 0; i < files->size(); i++ )
	{
		// add song to library, if necessary
		musikCube::g_Library->AddSong( files->at( i ) );

		// get the song, add it to the pl aylist
		if ( musikCube::g_Library->GetSongAttrFromFilename( files->at( i ), song ) == musikCore::MUSIK_LIBRARY_OK )
		{
			musikCube::g_Player->GetPlaylist()->Add( song );
		}
	}
	musikCube::g_Library->EndTransaction();

	// post a message to the main frame, letting
	// it know that drag and drop has completed
	if ( was_nowplaying_empty )
	{
		if ( !musikCube::g_Player->IsPlaying() )
			musikCube::g_Player->Play( 0, 0, musikCore::MUSIK_PLAYER_SONG_USER_ADVANCE, true );
		else
			musikCube::g_Player->FindNewIndex( musikCube::g_Player->GetPlaying()->GetID() );
	}

	return true;
}

///////////////////////////////////////////////////

CmusikPropTreeItem* CmusikSourcesCtrl::CreateNewStdPlaylist( int type, musikCore::StringArray* files, CmusikPropTreeItem* pItem )
{
    if ( !pItem )
		pItem = GetFocusedItem();
	else
	{
		// dropped on Create...
		KillFocus( false );
		SetFocusedItem( pItem );
		pItem->Select( TRUE );
	}

	CmusikSourcesItemPtrArray* ptrItems;
	musikCore::String playlist_str;
	int editinplace_type;
	
	if ( type == musikCore::MUSIK_PLAYLIST_TYPE_STANDARD )
	{
		ptrItems = &m_StdPlaylists;
		playlist_str = _T( "Playlist" );
		editinplace_type = MUSIK_SOURCES_EDITINPLACE_NEWSTDPLAYLIST;
	}
	else
		return NULL;	

	bool no_files = false;
	if ( !files )
	{
		files = new musikCore::StringArray();
		no_files = true;
	}

	// create the playlist
	musikCube::g_Library->CreateStdPlaylist( playlist_str, *files, m_StdPlaylists.size() - 1 );
    EditInPlace( pItem, editinplace_type, CString( playlist_str ) );

	if ( no_files )
		delete files;

	return pItem;
}

///////////////////////////////////////////////////

CmusikPropTreeItem* CmusikSourcesCtrl::CreateNewDynPlaylist()
{
    CmusikPropTreeItem* pItem = GetFocusedItem();
    EditInPlace( pItem, MUSIK_SOURCES_EDITINPLACE_NEWDYNPLAYLIST, CString( "Playlist" ) );

	return pItem;
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::KillFocus( bool redraw )
{
	CmusikPropTreeItem* pItem = GetFocusedItem();
	if ( pItem )
	{
		pItem->Select( FALSE );
		SetFocusedItem( NULL );

		if ( redraw )
			Invalidate();
	}
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::GetSelFilenames( CmusikPropTreeItem* pItem, musikCore::String& files )
{
    int nMode = pItem->GetPlaylistType();

    switch ( nMode )
    {
    case musikCore::MUSIK_PLAYLIST_TYPE_STANDARD:
		musikCube::g_Library->GetStdPlaylistFns( pItem->GetPlaylistID(), files );
        break;

    case musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC:
        musikCube::g_Library->GetDynPlaylistFns( pItem->GetPlaylistID(), files ); 
        break;

    case musikCore::MUSIK_PLAYLIST_TYPE_NOWPLAYING:
        musikCube::g_Library->GetStdPlaylistFns( *musikCube::g_Player->GetPlaylist(), files );
        break;

	case musikCore::MUSIK_PLAYLIST_TYPE_LIBRARY:
		musikCube::g_Library->GetAllFns( files );
		break;

	case musikCore::MUSIK_PLAYLIST_TYPE_REMOVABLE:
		if ( pItem->m_Library )
			pItem->m_Library->GetAllFns( files );
		break;

    default:
        break;
    } 
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::GetSelFilenames( CmusikPropTreeItem* pItem, musikCore::StringArray& files )
{
	int nMode = pItem->GetPlaylistType();
	
    switch ( nMode )
    {
    case musikCore::MUSIK_PLAYLIST_TYPE_STANDARD:
		musikCube::g_Library->GetStdPlaylistFns( pItem->GetPlaylistID(), files, false );
        break;

    case musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC:
        musikCube::g_Library->GetDynPlaylistFns( pItem->GetPlaylistID(), files, false ); 
        break;

    case musikCore::MUSIK_PLAYLIST_TYPE_NOWPLAYING:
        musikCube::g_Library->GetStdPlaylistFns( *musikCube::g_Player->GetPlaylist(), files, false );
        break;


	case musikCore::MUSIK_PLAYLIST_TYPE_LIBRARY:
		musikCube::g_Library->RawQueryStringArray( _T( " SELECT DISTINCT filename FROM songs; " ), files );
		break;

	case musikCore::MUSIK_PLAYLIST_TYPE_REMOVABLE:
		if ( pItem->m_Library )
			pItem->m_Library->RawQueryStringArray( _T( " SELECT DISTINCT filename FROM songs; " ), files );
		break;

    default:
        break;
    }     
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::DoDrag( CmusikPropTreeItem* pItem )
{
    if ( !pItem || 
		pItem->GetPlaylistType() == musikCore::MUSIK_PLAYLIST_TYPE_CDPLAYER ||
		pItem->GetPlaylistType() == MUSIK_SOURCES_TYPE_PLUGIN )
		return;

	COleDataSource datasrc;
	HGLOBAL        hgDrop;
	DROPFILES*     pDrop;
	CStringList    lsDraggedFiles;
	POSITION       pos;
	CString        sFile;
	UINT           uBuffSize = 0;
	TCHAR*         pszBuff;
	FORMATETC      etc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

	// get a list of filenames with the currently
	// selected items...
	musikCore::StringArray files;
    GetSelFilenames( pItem, files );

	if ( files.size() )
	{
		// CStringList containing files
		for ( size_t i = 0; i < files.size(); i++ )
		{
			lsDraggedFiles.AddTail( files.at( i ) );
			uBuffSize += files.at( i ).GetLength() + 1;
		}

		files.clear();
	}

	
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

	pDrop->fWide = TRUE;

	// Copy all the filenames into memory after the end of the DROPFILES struct.
	pos = lsDraggedFiles.GetHeadPosition();
	pszBuff = (TCHAR*) (LPBYTE(pDrop) + sizeof(DROPFILES));

	while ( pos )
	{
		wcscpy ( pszBuff, (LPCTSTR) lsDraggedFiles.GetNext ( pos ) );
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

	// Put the data in the data source.
	etc.cfFormat = m_DropID;
	datasrc.CacheGlobalData ( m_DropID, hgBool, &etc );

	// Start the drag 'n' drop!
	DROPEFFECT dwEffect = datasrc.DoDragDrop ( DROPEFFECT_COPY | DROPEFFECT_MOVE );

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

void CmusikSourcesCtrl::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CmusikPropTree::OnShowWindow(bShow, nStatus);

	if ( m_Startup )
	{
		FocusLibrary( false );
		m_Startup = false;
	}
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::RenameItem( CmusikPropTreeItem* pItem, int mode, CPoint pos )
{
	if ( !pItem )
		pItem = GetFocusedItem();

	if ( !pItem )
		return;

	// not trying to rename a root
	if ( pItem->IsRootLevel() )
		return;

	// not renaming the library, now playing,
	// or quick search
	if ( pItem == m_Lib.at( 0 ) || pItem == m_Lib.at( 1 ) )
		return;

	CPoint nPos;
	if ( pos.x == -1 && pos.y == -1 )
		nPos = pItem->GetLocation();
	else
		nPos = pos;

	CRect rcClient;
	GetClientRect( rcClient );
	
    int left;
    musikCube::g_DrawGraphics ? left = 32 : left = 12;
	CRect rect( left, nPos.y + 4, rcClient.Width() - 1, nPos.y + PROPTREEITEM_DEFHEIGHT - 2 );

	if ( IsScrollVisible() )
		rect.right -= 5;

	EditInPlace( pItem, mode, pItem->GetLabelText() );
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::RemoveStdPlaylist( CmusikPropTreeItem* pItem )
{
	for ( size_t i = 0; i < m_StdPlaylists.size(); i++ )
	{
		if ( pItem == m_StdPlaylists.at( i ) )
		{
			m_StdPlaylists.erase( m_StdPlaylists.begin() + i );
			DeleteItem( pItem );
			return;
		}
	}
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::RemoveDynPlaylist( CmusikPropTreeItem* pItem )
{
	for ( size_t i = 0; i < m_DynPlaylists.size(); i++ )
	{
		if ( pItem == m_DynPlaylists.at( i ) )
		{
			m_DynPlaylists.erase( m_DynPlaylists.begin() + i );
			DeleteItem( pItem );
			return;
		}
	}
}


///////////////////////////////////////////////////

void CmusikSourcesCtrl::DeleteSel()
{
	CmusikPropTreeItem* pItem = GetFocusedItem();
	if ( pItem )
	{
		switch( pItem->GetPlaylistType() )
		{
		case musikCore::MUSIK_PLAYLIST_TYPE_STANDARD:
		case musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC:
			{
				CmusikSourcesItemPtrArray* ptrItems;

				// remove the playlist from the database
				if ( pItem->GetPlaylistType() == musikCore::MUSIK_PLAYLIST_TYPE_STANDARD )
				{
					ptrItems = &m_StdPlaylists;
					musikCube::g_Library->DeleteStdPlaylist( pItem->GetPlaylistID() );
				}
				else
				{
					ptrItems = &m_DynPlaylists;
					musikCube::g_Library->DeleteDynPlaylist( pItem->GetPlaylistID() );
				}

				// find selection index
				int SelAt = 0;
				for ( size_t i = 0; i < ptrItems->size(); i++ )
				{
					if ( ptrItems->at( i ) == pItem )
					{
						SelAt = i;
						break;
					}
				}

				pItem->Detach();
				delete pItem;
				ptrItems->erase( ptrItems->begin() + SelAt );

				// after detaching, if there is anothing playlist
				// in the same position, select that.
                if ( ptrItems->size() > 1 )
				{
					if ( SelAt == (int)ptrItems->size() - 1 )
					{
						SelAt--;
					}

					FocusItem( ptrItems->at( SelAt ) );
				}
				// no playlists left to select! focus the library or
				// now playing.
				else
				{
					if ( !musikCube::g_Player->IsPlaying() )
					{
						FocusLibrary( true );
					}
					else
					{
						FocusNowPlaying();
					}
				}

				UpdateResize();	// may need to hide scrollbar
			}
			break;
		}
	}
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// user pressed f2 to rename an entry
	if ( nChar == VK_F2 )
		RenameItem();

	// user requested playlist deletion
	if ( nChar == VK_DELETE )
		DeleteSel();

	if ( GetKeyState( VK_CONTROL ) < 0 )
	{
		if ( nChar == VK_UP )
			MoveSelUp();
		if ( nChar == VK_DOWN )
			MoveSelDown();
	}
}

///////////////////////////////////////////////////

LRESULT CmusikSourcesCtrl::OnEditCancel( WPARAM wParam, LPARAM lParam )
{
	int cmd = (int)lParam;

	CmusikEditInPlace* sender = (CmusikEditInPlace*)wParam;
	
	CmusikPropTreeItem* pItem = GetFocusedItem();
	SetFocusedItem( NULL );

    switch( cmd )
    {
    case MUSIK_SOURCES_EDITINPLACE_NEWSTDPLAYLIST:
        {
		    m_StdPlaylists.at( m_StdPlaylists.size() - 1 )->SetLabelText( _T( "Create..." ) );
            DeleteLastPlaylist( musikCore::MUSIK_PLAYLIST_TYPE_STANDARD );
        }
        break;

    case MUSIK_SOURCES_EDITINPLACE_NEWDYNPLAYLIST:
        {
		    m_DynPlaylists.at( m_DynPlaylists.size() - 1 )->SetLabelText( _T( "Create..." ) );
		    if ( pItem )
		    {
			    musikCube::g_Library->DeleteDynPlaylist( pItem->GetPlaylistID() );
			    DynPlaylistsLoad();
		    }
        }
        break;

    case MUSIK_SOURCES_EDITINPLACE_QUICKSEARCH:
        {
		    FinishQuickSearch();
		    m_Search->SetLabelText( _T( "Search..." ) );
        }
        break;

    default:
        break;
    }

    m_EditInPlace.EnableWindow( FALSE );
    SelectItems( NULL, FALSE );

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikSourcesCtrl::OnEditCommit( WPARAM wParam, LPARAM lParam )
{
	int cmd = (int)lParam;
	CmusikEditInPlace* sender = (CmusikEditInPlace*)wParam;
	CmusikPropTreeItem* pItem = GetFocusedItem();
	CString sName = sender->GetString();

    switch ( cmd )
    {
    case MUSIK_SOURCES_EDITINPLACE_RENAME:
        {
            int type = pItem->GetPlaylistType();
            switch ( type )
            {
            case musikCore::MUSIK_PLAYLIST_TYPE_STANDARD:
				musikCube::g_Library->RenameStdPlaylist( pItem->GetPlaylistID(), (musikCore::String)sName );
                break;

			case musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC:
				musikCube::g_Library->RenameDynPlaylist( pItem->GetPlaylistID(), (musikCore::String)sName );
				break;
            }

			pItem->SetLabelText( sName );
			Invalidate();
        }
        break;

    case MUSIK_SOURCES_EDITINPLACE_QUICKSEARCH:
        {
		    if ( pItem )
		    {
			    m_Search->SetLabelText( _T( "Search..." ) );
			    FinishQuickSearch();
		    }
        }
        break;

    case MUSIK_SOURCES_EDITINPLACE_NEWSTDPLAYLIST:
        {
            StdPlaylistsLoad();
            pItem = m_StdPlaylists.at( m_StdPlaylists.size() - 2 );
            if ( pItem )
            {
			    musikCube::g_Library->RenameStdPlaylist( pItem->GetPlaylistID(), (musikCore::String)sName );
                m_StdPlaylists.at( m_StdPlaylists.size() - 2 )->SetLabelText( sName );
				UpdateResize();
			}
        }
        break;

    case MUSIK_SOURCES_EDITINPLACE_NEWDYNPLAYLIST:
        {
            SelectItems( NULL, FALSE );
            Invalidate();

	        CmusikDynPlaylistDlg dlg( m_EditInPlace.GetString() );
	        int ret = dlg.DoModal();

	        if ( ret == IDOK )
	        {
		        musikCube::g_Library->CreateDynPlaylist( dlg.GetName(), dlg.GetQuery(), m_DynPlaylists.size() - 1 );
		        DynPlaylistsLoad();
				UpdateResize();
	        }
        }
        break;

    default:
        
        break;
    }

	m_EditInPlace.EnableWindow( FALSE );
    SelectItems( NULL, FALSE );

	SetFocus();
	return 0L;
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint point )
{
	CmusikSourcesBar* pBar = (CmusikSourcesBar*)GetParent();
	ScreenToClient( &point );
	m_RightClick = FindItem( point );
	pBar->ShowMenu();
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::QuickSearch()
{
	CmusikPropTreeItem* pTemp = m_Search;

	pTemp->SetLabelText( _T( "" ) );
	
	CPoint nPos = pTemp->GetLocation();		

	CRect rcClient;
	GetClientRect( rcClient );

	CString tmp = _T( "" );
	EditInPlace( pTemp, MUSIK_SOURCES_EDITINPLACE_QUICKSEARCH, tmp );

	int WM_SOURCESQUICKSEARCH = RegisterWindowMessage( _T( "SOURCESQUICKSEARCH" ) );
	CmusikSourcesBar* pBar = (CmusikSourcesBar*)GetParent();
	pBar->m_Parent->PostMessage( WM_SOURCESQUICKSEARCH );
}

///////////////////////////////////////////////////

LRESULT CmusikSourcesCtrl::OnEditChange( WPARAM wParam, LPARAM lParam )
{
	int cmd = (int)lParam;

	if ( cmd == MUSIK_SOURCES_EDITINPLACE_QUICKSEARCH )
	{
		m_EditInPlace.GetWindowText( m_QuickStr );

		int WM_SOURCESQUICKSEARCHSTR = RegisterWindowMessage( _T( "SOURCESQUICKSEARCHSTR" ) );
		CmusikSourcesBar* pBar = (CmusikSourcesBar*)GetParent();
		pBar->m_Parent->PostMessage( WM_SOURCESQUICKSEARCHSTR, (WPARAM)&m_QuickStr );
	}

	return 0L;
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::FinishQuickSearch()
{
	CmusikSourcesBar* parent = (CmusikSourcesBar*)GetParent();

	m_Search->Select( FALSE );
	SetFocusedItem( NULL );
	KillFocus( true );
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnDragLeave()
{
	CmusikPropTree::m_Hovered = NULL;
	RedrawWindow();
}

///////////////////////////////////////////////////

DROPEFFECT CmusikSourcesCtrl::OnDragOver( CPoint point )
{
	DROPEFFECT dwReturn = DROPEFFECT_NONE;
    CmusikPropTreeItem* pItem = FindItem( point );

	if ( pItem != NULL )
	{
		switch( pItem->GetPlaylistType() )
		{
		case musikCore::MUSIK_PLAYLIST_TYPE_REMOVABLE:
		case musikCore::MUSIK_PLAYLIST_TYPE_LIBRARY:
		case MUSIK_SOURCES_TYPE_COLLECTED:
			dwReturn = DROPEFFECT_COPY;
			break;

		case musikCore::MUSIK_PLAYLIST_TYPE_STANDARD:

			dwReturn = DROPEFFECT_COPY;
			//
			if  ( m_DragAndDropInfo != NULL )
			{
				if (
					   m_DragAndDropInfo->GetType() == pItem->GetPlaylistType()
					&& m_DragAndDropInfo->GetID() == pItem->GetPlaylistID()
					)
				{
					dwReturn = DROPEFFECT_NONE;
				}
			}
			//
			break;

		case musikCore::MUSIK_PLAYLIST_TYPE_NOWPLAYING:

			dwReturn = DROPEFFECT_COPY;
			//
			if  ( m_DragAndDropInfo != NULL )
			{
				// disallow drag and drop from Now Playing to Now Playing
				if ( m_DragAndDropInfo->GetType() == pItem->GetPlaylistType() )
				{
					dwReturn = DROPEFFECT_NONE;
				}
			}
			//
			break;
		}

		if ( pItem != CmusikPropTree::m_Hovered )
		{
			CmusikPropTree::m_Hovered = pItem;
			RedrawWindow();
		}
	}

	return dwReturn;
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( !m_MouseTrack && !( nFlags & MK_LBUTTON ) )
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof( tme );
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_hWnd;
		tme.dwHoverTime = HOVER_DEFAULT;
		::_TrackMouseEvent( &tme );

		m_MouseTrack = true; 	
	}

	// test for drag and drop
	if ( m_MouseTrack && ( nFlags & MK_LBUTTON ) )
	{
		CmusikPropTreeItem* pItem = GetFocusedItem();
		if ( pItem )
		{
			switch( pItem->GetPlaylistType() )
			{
			case MUSIK_SOURCES_TYPE_NEWSTDPLAYLIST:
			case MUSIK_SOURCES_TYPE_QUICKSEARCH:
			case MUSIK_SOURCES_TYPE_NEWDYNPLAYLIST:
			case musikCore::MUSIK_PLAYLIST_TYPE_CDPLAYER:
				return;
				break;
			}

			m_DragAndDropInfo = new musikCore::PlaylistInfo( *pItem->GetPlaylistInfo() );
			//
			DoDrag( pItem );
			//
			delete m_DragAndDropInfo;
			m_DragAndDropInfo = NULL;
		}
	}

    CmusikPropTreeItem* pItem = FindItem( point );
    if ( pItem != CmusikPropTree::m_Hovered )
    {
        CmusikPropTree::m_Hovered = pItem;
        RedrawWindow();
    }
}

///////////////////////////////////////////////////

LRESULT CmusikSourcesCtrl::OnMouseLeave( WPARAM wParam, LPARAM lParam )
{
	m_MouseTrack = false;
    CmusikPropTree::m_Hovered = NULL;
    Invalidate();
	return 1L;
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::EditInPlace( CmusikPropTreeItem* pItem, int args, const CString text )
{
    if ( pItem )
    {
	    CPoint nPos = pItem->GetLocation();		
	    CRect rcClient;
	    GetClientRect( rcClient );

        int left;
        musikCube::g_DrawGraphics ? left = 32 : left = 12;
	    CRect rect( left, nPos.y + 4 - m_ScrollPos, rcClient.Width() - 1, nPos.y + PROPTREEITEM_DEFHEIGHT - 2 - m_ScrollPos );

		if ( IsScrollVisible() )
			rect.right -= 5;

	    m_EditInPlace.EnableWindow( TRUE );
	    m_EditInPlace.SetArgs( (void*)args );
	    m_EditInPlace.MoveWindow( rect );
	    m_EditInPlace.SetFocus();
	    m_EditInPlace.SetString( text );
	    m_EditInPlace.ShowWindow( SW_SHOWDEFAULT );
    }
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnSourcesRipcd()
{
    if ( !m_RipDlg )
    {
		CmusikPropTreeItem* pItem = GetCtrl()->GetFocusedItem();
		if ( pItem && pItem->GetPlaylistType() == musikCore::MUSIK_PLAYLIST_TYPE_CDPLAYER )
			m_RipDlg = new CmusikRipDlg( this, GetCtrl()->GetFocusedItem()->GetPlaylistID() );
		else
			m_RipDlg = new CmusikRipDlg( this );

        m_RipDlg->Create( IDD_RIP, this );
	    m_RipDlg->ShowWindow( SW_SHOWDEFAULT );
    }
}

///////////////////////////////////////////////////

LRESULT CmusikSourcesBar::OnCloseRipDlg( WPARAM wParam, LPARAM lParam )
{
    delete m_RipDlg;
    m_RipDlg = NULL;
    return 0L;
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnSourcesRefreshcd()
{
	int WM_SOURCESCDAUDIO = RegisterWindowMessage( _T( "SOURCESCDAUDIO" ) );
	m_Parent->PostMessage( WM_SOURCESCDAUDIO, GetCtrl()->GetFocusedItem()->GetPlaylistID() );
}

///////////////////////////////////////////////////

BOOL CmusikSourcesCtrl::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    return CmusikPropTree::PreTranslateMessage(pMsg);
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnSourcesUnmount()
{
	CmusikPropTreeItem *pSel = GetCtrl()->m_RightClick;
	if ( pSel )
	{
		pSel->UnloadLibrary();
		GetCtrl()->KillFocus();
		GetCtrl()->FocusLibrary( true );
	}

}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnSourcesMoveup()
{
	GetCtrl()->MoveSelUp();
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnSourcesMovedown()
{
	GetCtrl()->MoveSelDown();
}

///////////////////////////////////////////////////

int CmusikSourcesCtrl::GetStdPlaylistIdx( CmusikPropTreeItem* pItem )
{
	for ( size_t i = 0; i < m_StdPlaylists.size(); i++ )
	{
		if ( m_StdPlaylists.at( i ) == pItem )
			return i;
	}

	return -1;
}

///////////////////////////////////////////////////

int CmusikSourcesCtrl::GetDynPlaylistIdx( CmusikPropTreeItem* pItem )
{
	for ( size_t i = 0; i < m_StdPlaylists.size(); i++ )
	{
		if ( m_StdPlaylists.at( i ) == pItem )
			return i;
	}

	return -1;
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::MoveSelUp()
{
	CmusikPropTreeItem* pItem = GetFocusedItem();
	if ( pItem && pItem->GetPrev() )
	{
		// moving up takes the current playlist and decreases it's
		// order, then increases the other playlist's order.
		switch( pItem->GetPlaylistType() )
		{
		case musikCore::MUSIK_PLAYLIST_TYPE_STANDARD:
		case musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC:
			{
				CmusikPropTreeItem* pA = pItem;
				CmusikPropTreeItem* pB = pItem->GetPrev();

				int a = pA->GetPlaylistInfo()->m_Order;
				int b = pB->GetPlaylistInfo()->m_Order;

                pA->GetPlaylistInfo()->m_Order = b;
				pB->GetPlaylistInfo()->m_Order = a;
		
				musikCube::g_Library->UpdatePlaylistOrder( *pA->GetPlaylistInfo() );
				musikCube::g_Library->UpdatePlaylistOrder( *pB->GetPlaylistInfo() );
			
				// move A above B
				pA->Detach();		
				CmusikPropTreeItem* pFirst = pB->GetPrev();
				pB->SetPrev( pA );
				pA->SetNext( pB );
				pA->SetPrev( pFirst );

				if ( pFirst )
					pFirst->SetNext( pA );

				if ( pB->GetParent()->GetChild() == pB )
					pB->GetParent()->SetChild( pA );

				Invalidate();
			}
			break;
		}
	}
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::MoveSelDown()
{
	CmusikPropTreeItem* pItem = GetFocusedItem();
	if ( pItem && pItem->GetNext() )
	{
		// moving up takes the current playlist and decreases it's
		// order, then increases the other playlist's order.
		switch( pItem->GetPlaylistType() )
		{
		case musikCore::MUSIK_PLAYLIST_TYPE_STANDARD:
		case musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC:
			{
				CmusikPropTreeItem* pA = pItem;
				CmusikPropTreeItem* pB = pItem->GetNext();
	
				// need to make sure we don't move the item below "Create..."
				if ( m_StdPlaylists.size() && pB == m_StdPlaylists.at( m_StdPlaylists.size() - 1 ) )
					return;
				if ( m_DynPlaylists.size() && pB == m_DynPlaylists.at( m_DynPlaylists.size() - 1 ) )
					return;

				// swap the order in the database
				int a = pA->GetPlaylistInfo()->m_Order;
				int b = pB->GetPlaylistInfo()->m_Order;

				pA->GetPlaylistInfo()->m_Order = b;
				pB->GetPlaylistInfo()->m_Order = a;

				musikCube::g_Library->UpdatePlaylistOrder( *pA->GetPlaylistInfo() );
				musikCube::g_Library->UpdatePlaylistOrder( *pB->GetPlaylistInfo() );

				// move B above A
				pB->Detach();		
				CmusikPropTreeItem* pFirst = pA->GetPrev();
				pA->SetPrev( pB );
				pB->SetNext( pA );
				pB->SetPrev( pFirst );

				if ( pFirst )
					pFirst->SetNext( pB );

				if ( pA->GetParent()->GetChild() == pA )
					pA->GetParent()->SetChild( pB );

				Invalidate();
			}
			break;
		}
	}
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnSourcesClear()
{
	musikCube::g_Player->GetPlaylist()->Clear();
	GetCtrl()->FocusNowPlaying();	// forces update of playlist in main ui
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnSourcesPluginexecute()
{
	CmusikPropTreeItem* pFocus = GetCtrl()->GetFocusedItem();
	if ( pFocus )
	{
		musikCube::g_Plugins.at( pFocus->GetPlaylistID() ).Execute();
	}
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnSourcesPluginconfigure()
{
	CmusikPropTreeItem* pFocus = GetCtrl()->GetFocusedItem();
	if ( pFocus )
		musikCube::g_Plugins.at( pFocus->GetPlaylistID() ).Configure();
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnSourcesPluginstop()
{
	CmusikPropTreeItem* pFocus = GetCtrl()->GetFocusedItem();
	if ( pFocus )
		musikCube::g_Plugins.at( pFocus->GetPlaylistID() ).Stop();
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnSourcesPluginabout()
{
	CmusikPropTreeItem* pFocus = GetCtrl()->GetFocusedItem();
	if ( pFocus )
		musikCube::g_Plugins.at( pFocus->GetPlaylistID() ).About();
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnSourcesShowallsongs()
{
	GetCtrl()->FocusLibrary();
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnSourcesSynchronize()
{
	synchronizeDirs();
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::LoadColLib()
{
	if ( m_Collected->m_Library )
		m_Collected->UnloadLibrary();

	CString oldpath_rel, fn;
	oldpath_rel = _T( "~/.musikproject/musik_collected_u.db" );
	fn = CmusikCubeApp::RelToAbs( oldpath_rel );

	musikCore::Library* pNewLib = new musikCore::Library( 
		fn.GetBuffer(), musikCube::g_Prefs->GetUseUTC );

	m_Collected->m_Library = pNewLib;
	musikCube::g_Collected = pNewLib;
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnSourcesClearcollected()
{
	if ( MessageBox( 
		_T( "Are you sure you want to clear your collected music?" ),
		_T( "musikCube" ),
		MB_ICONINFORMATION | MB_YESNO ) == IDYES )
	{
		GetCtrl()->m_Collected->m_Library->ClearLibrary( true );
		int WM_SOURCESCOLLECTED = RegisterWindowMessage( _T( "SOURCESCOLLECTED" ) );
		m_Parent->PostMessage( WM_SOURCESCOLLECTED, NULL );
	}
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::LoadBitmaps()
{
	CmusikCubeApp* ptrApp = (CmusikCubeApp*)AfxGetApp();
	CString workingdir = ptrApp->GetWorkingDir();

	m_pLibOff = Gdiplus::Bitmap::FromFile( 
		workingdir + _T( "\\img\\lib_uf.png" ) );
	m_pLibOn = Gdiplus::Bitmap::FromFile( 
		workingdir + _T( "\\img\\lib_f.png" ) );

	m_pNPOff = Gdiplus::Bitmap::FromFile( 
		workingdir + _T( "\\img\\np_uf.png" ) );
	m_pNPOn = Gdiplus::Bitmap::FromFile( 
		workingdir + _T( "\\img\\np_f.png" ) );

	m_pQSOff = Gdiplus::Bitmap::FromFile( 
		workingdir + _T( "\\img\\qs_uf.png" ) );
	m_pQSOn = Gdiplus::Bitmap::FromFile( 
		workingdir + _T( "\\img\\qs_f.png" ) );

	m_pCDOff = Gdiplus::Bitmap::FromFile( 
		workingdir + _T( "\\img\\cd_uf.png" ) );
	m_pCDOn = Gdiplus::Bitmap::FromFile( 
		workingdir + _T( "\\img\\cd_f.png" ) );

	m_pPLOff = Gdiplus::Bitmap::FromFile( 
		workingdir + _T( "\\img\\pl_uf.png" ) );
	m_pPLOn = Gdiplus::Bitmap::FromFile( 
		workingdir + _T( "\\img\\pl_f.png" ) );

	m_pDPOff = Gdiplus::Bitmap::FromFile( 
		workingdir + _T( "\\img\\dp_uf.png" ) );
	m_pDPOn = Gdiplus::Bitmap::FromFile( 
		workingdir + _T( "\\img\\dp_f.png" ) );

	m_pRDOff = Gdiplus::Bitmap::FromFile( 
		workingdir + _T( "\\img\\rd_uf.png" ) );
	m_pRDOn = Gdiplus::Bitmap::FromFile( 
		workingdir + _T( "\\img\\rd_f.png" ) );

	m_pPlugOff = Gdiplus::Bitmap::FromFile( 
		workingdir + _T( "\\img\\plug_uf.png" ) );
	m_pPlugOn = Gdiplus::Bitmap::FromFile( 
		workingdir + _T( "\\img\\plug_f.png" ) );

	m_pNSOff = Gdiplus::Bitmap::FromFile( 
		workingdir + _T( "\\img\\ns_uf.png" ) );
	m_pNSOn = Gdiplus::Bitmap::FromFile( 
		workingdir + _T( "\\img\\ns_f.png" ) );

	m_pColOff = Gdiplus::Bitmap::FromFile( 
		workingdir + _T( "\\img\\col_uf.png" ) );
	m_pColOn = Gdiplus::Bitmap::FromFile( 
		workingdir + _T( "\\img\\col_f.png" ) );
}

///////////////////////////////////////////////////