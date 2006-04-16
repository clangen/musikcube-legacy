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
//   CmusikPrefs
//
// Filename(s): 
//
//   musikPrefs.h, musikPrefs.cpp
//
// Information:
//
//   Preferences class for musikCube
// 
// Usage: 
//
//   LoadPrefs() to load, SavePrefs() to save...
//   GetXXX() and SetXXX() to store and retrieve
//   values.
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "musikPrefs.h"

///////////////////////////////////////////////////

CmusikPrefs::CmusikPrefs( musikCore::String& filename )
{
    string fn = musikCore::utf16to8( filename, false );
	config = new CIniFile( fn );

	if ( !config->ReadFile() )
		config->WriteFile();

	m_ActiveCaptionDef			= false;
	m_CaptionDef				= false;
	m_InactiveCaptionDef		= false;
	m_InactiveCaptionTextDef	= false;
	m_BtnFaceDef				= false;
	m_BtnTextDef				= false;
	m_BtnHilightDef				= false;
	m_BtnShadowDef				= false;
	m_ListCtrlDef				= false;
	m_ListCtrlTextDef			= false;
	m_HilightDef				= false;
	m_HilightTextDef			= false;

	LoadPrefs();
}

///////////////////////////////////////////////////

CmusikPrefs::~CmusikPrefs()
{
	SavePrefs();
	delete config;
}

///////////////////////////////////////////////////

void CmusikPrefs::LoadPrefs()
{
	// main dialog
    m_Lib_Path                  = config->GetValue( "Library", "Path", "~/.musikproject/musik_u.db" );

	m_Dlg_Size					= StringToCSize( config->GetValue( "Dialog", "Dialog Size", "800x600" ) );
	m_Dlg_Pos					= StringToCPoint( config->GetValue( "Dialog", "Dialog Position", "50,50" ) );
	m_Dlg_Maximized				= StringToBool( config->GetValue( "Dialog", "Maximized", "0" ) );
	m_Dlg_ResetUI				= StringToBool( config->GetValue( "Dialog", "Reset UI", "1" ) );
	m_Dlg_FileDrop_Prompt		= StringToInt( config->GetValue( "Dialog", "Prompt File Drop Action", "-1" ) );
	m_Dlg_PlaylistInfoVisible	= StringToBool( config->GetValue( "Dialog", "Playlist Info Visible", "1" ) );
	m_Dlg_PlaylistInfoVizStyle	= StringToInt( config->GetValue( "Dialog", "Playlist Info Viz Style", "1" ) );
	m_Dlg_SynchronizeOnStartup	= StringToBool( config->GetValue( "Dialog", "Synchronize Old Files on Startup", "0" ) );
	m_Dlg_WriteTagsToFile		= StringToBool( config->GetValue( "Dialog", "Write Tags to File", "0" ) );
	m_Dlg_MinimizeToTray		= StringToBool( config->GetValue( "Dialog", "Minimize to Notification Tray", "1" ) );
    m_Dlg_ShowBalloonTips       = StringToBool( config->GetValue( "Dialog", "ShowBalloonTips", "1" ) );
    m_Dlg_AddEntireToNP			= StringToBool( config->GetValue( "Dialog", "Add Entire Library Playlist to Now Playing", "1" ) );
	m_Dlg_UseUTC				= StringToBool( config->GetValue( "Dialog", "Use UTC", "1" ) );
	m_Dlg_AlwaysOnTop			= StringToBool( config->GetValue( "Dialog", "Always on Top", "0" ) );
    m_Dlg_GlobalHotkeys         = StringToBool( config->GetValue( "Dialog", "UseGlobalHotkeys", "0" ) );
	m_Dlg_CloseToSystemTray		= StringToBool( config->GetValue( "Dialog", "CloseToSystemTray", "0" ) );
	m_Dlg_LastPlayed			= StringToInt( config->GetValue( "Dialog", "LastPlayed", "0" ) );
	m_Dlg_SelBoxShowAll			= StringToBool( config->GetValue( "Dialog", "SelectionBoxShowAll", "1" ) );
	m_Dlg_FreeDBServer			= config->GetValue( "Dialog", "FreeDB Server", "uk.freedb.org" );
	m_Dlg_UseMMHook				= StringToBool( config->GetValue( "Dialog", "UseMMHook", "1" ) );
	m_Dlg_ShowSelBoxes			= StringToBool(config->GetValue("Dialog", "ShowSelectionBoxes", "1"));

	// tagging
	m_Tag_ToFilename			= config->GetValue( "Tag", "ToFilename", "" );
	m_Tag_FromFilename			= config->GetValue( "Tag", "FromFilename", "" );
	m_Tag_GlobalFromFilename	= StringToBool( config->GetValue( "Tag", "GlobalFromFilename", "" ) );
	m_Tag_SpacesToUndr			= StringToBool( config->GetValue( "Tag", "SpacesToUndr", "0" ) );
	m_Tag_UndrToSpaces			= StringToBool( config->GetValue( "Tag", "UndrToSpaces", "0" ) );

	// transparency
	m_TransEnabled				= StringToBool( config->GetValue( "Transparency", "Enabled", "0" ) );
	m_TransFocus				= StringToInt( config->GetValue( "Transparency", "Focused", "225" ) );
	m_TransUnfocus				= StringToInt( config->GetValue( "Transparency", "Unfocused", "100" ) );

	// selection area
	m_SelectionBox_Types = StringToCIntArray( config->GetValue( "Selection Area", "Types", "0,1" ) );

	// sources
	m_Sources_SubLibsVisible		= StringToBool( config->GetValue( "Sources", "Show SubLibs", "0" ) );
	m_Sources_PlaylistsVisible		= StringToBool( config->GetValue( "Sources", "Show Playlists", "1" ) );
	m_Sources_DevicesVisible		= StringToBool( config->GetValue( "Sources", "Show Devices", "1" ) );
	m_Sources_DynPlaylistsVisible	= StringToBool( config->GetValue( "Sources", "Show Dynamic Playlists", "1" ) );
	m_Sources_PluginsVisible		= StringToBool( config->GetValue( "Sources", "Show Plugins", "0" ) );
	
 	musikCore::String hidden		= musikCore::utf8to16( config->GetValue( "Sources", "Hidden Devices", "A:\\" ) );
	musikCore::Filename::DelimitStr( hidden, _T( "," ), m_Sources_Hidden_Devices, false, true, true );

	// playlist
	m_Playlist_Order		= StringToCIntArray( config->GetValue( "Playlist", "Column Order", GetDefPlaylistOrder() ) );
	m_Playlist_Sizes		= StringToCIntArray( config->GetValue( "Playlist", "Column Sizes", GetDefPlaylistSizes() ) );
	m_PlaylistInfo_VizDur	= StringToInt( config->GetValue( "Playlist", "Viz Duration", "75" ) );

	// now playing
	m_NowPlaying_CaptionFont = StringToInt( config->GetValue( "Now Playing", "Caption Font Size", "12" ) );
	
	// player
	m_Player_Driver			= StringToInt( config->GetValue( "Player", "Driver", "0" ) );
	m_Player_Rate			= StringToInt( config->GetValue( "Player", "Rate", "44100" ) );
	m_Player_Volume			= StringToInt( config->GetValue( "Player", "Volume", "200" ) );
	m_Player_Playmode		= StringToULong( config->GetValue( "Player", "Playmode", "0" ) );	
	m_Player_Buffer			= StringToInt( config->GetValue( "Player", "Buffer", "500" ) );

	// crossfader
	m_Crossfader_Enabled	= StringToBool( config->GetValue( "Crossfader", "Enabled", "1" ) );
	m_Crossfader_Current	= StringToInt( config->GetValue( "Crossfader", "Set Name", "-1" ) );

	// equalizer
	m_Equalizer_Enabled		= StringToBool( config->GetValue( "Equalizer", "Enabled", "0" ) );
	m_Equalizer_Current		= StringToInt( config->GetValue( "Equalizer", "Set Name", "-1" ) );
	m_Equalizer_Band_State	= StringToInt( config->GetValue( "Equalizer", "Band State", "6" ) );
	m_Equalizer_Locked		= StringToBool( config->GetValue( "Equalizer", "Channels Locked", "1" ) );
	m_Equalizer_Global		= StringToBool( config->GetValue( "Equalizer", "Use Global Equalizer", "1" ) );
	m_Equalizer_Multiplier	= StringToFloat( config->GetValue( "Equalizer", "Multiplier", "0.0" ) );

	// dialog colors
	MUSIK_COLOR_ACTIVECAPTION		= StringToCOLORREF( config->GetValue( "Theme", "Active Caption", COLORREFToString( AUTOMATIC_COLOR ) ) );
	MUSIK_COLOR_CAPTIONTEXT			= StringToCOLORREF( config->GetValue( "Theme", "Active Caption Text", COLORREFToString( AUTOMATIC_COLOR ) ) );
	MUSIK_COLOR_INACTIVECAPTION		= StringToCOLORREF( config->GetValue( "Theme", "Inactive Caption", COLORREFToString( AUTOMATIC_COLOR ) ) );
	MUSIK_COLOR_INACTIVECAPTIONTEXT	= StringToCOLORREF( config->GetValue( "Theme", "Inactive Caption Text", COLORREFToString( AUTOMATIC_COLOR ) ) );
	MUSIK_COLOR_BTNFACE				= StringToCOLORREF( config->GetValue( "Theme", "Button Face", COLORREFToString( AUTOMATIC_COLOR ) ) );
	MUSIK_COLOR_BTNTEXT				= StringToCOLORREF( config->GetValue( "Theme", "Button Text", COLORREFToString( AUTOMATIC_COLOR ) ) );
	MUSIK_COLOR_BTNHILIGHT			= StringToCOLORREF( config->GetValue( "Theme", "Button Highlight", COLORREFToString( AUTOMATIC_COLOR ) ) );
	MUSIK_COLOR_BTNSHADOW			= StringToCOLORREF( config->GetValue( "Theme", "Button Shadow", COLORREFToString( AUTOMATIC_COLOR ) ) );
	MUSIK_COLOR_LISTCTRL			= StringToCOLORREF( config->GetValue( "Theme", "List Background", COLORREFToString( AUTOMATIC_COLOR ) ) );
	MUSIK_COLOR_LISTCTRLTEXT		= StringToCOLORREF( config->GetValue( "Theme", "List Text", COLORREFToString( AUTOMATIC_COLOR ) ) );
	MUSIK_COLOR_HIGHLIGHT			= StringToCOLORREF( config->GetValue( "Theme", "Color Highlight", COLORREFToString( AUTOMATIC_COLOR ) ) );
	MUSIK_COLOR_HIGHLIGHTTEXT		= StringToCOLORREF( config->GetValue( "Theme", "Color Highlight Text", COLORREFToString( AUTOMATIC_COLOR ) ) );
	m_UseLightIconTheme				= StringToBool( config->GetValue( "Theme", "Use Light Icons", BoolToString( false ) ) );

	// tunage
	m_Tunage_Enabled	= StringToBool( config->GetValue( "Tunage", "Enabled", "0" ) );
	m_Tunage_WriteFile	= StringToBool( config->GetValue( "Tunage", "WriteFile", "0" ) );
	m_Tunage_Filename	= config->GetValue( "Tunage", "Filename", "" );
	m_Tunage_FileLine	= config->GetValue( "Tunage", "FileLine", "" );
	m_Tunage_AppendFile	= StringToBool( config->GetValue( "Tunage", "AppendFile", "0" ) );
	m_Tunage_PostURL	= StringToBool( config->GetValue( "Tunage", "PostURL", "0" ) );
	m_Tunage_URL		= config->GetValue( "Tunage", "URL", "" );
	m_Tunage_RunApp		= StringToBool( config->GetValue( "Tunage", "RunApp", "0" ) );
	m_Tunage_CmdLine	= config->GetValue( "Tunage", "CmdLine", "" );
	m_Tunage_RunOnStop	= StringToBool( config->GetValue( "Tunage", "RunOnStop", "0" ) );
	m_Tunage_StoppedText = config->GetValue( "Tunage", "StoppedText", "" );

    // ripping
    m_Rip_Path      = config->GetValue( "Rip", "Path", "" );
    m_Rip_Drive     = StringToInt( config->GetValue( "Rip", "Drive", "0" ) );
    m_Rip_Enc       = StringToInt( config->GetValue( "Rip", "Encoder", "0" ) );
    m_Rip_Quality   = StringToInt( config->GetValue( "Rip", "Quality", "6" ) );
    m_Rip_Rename    = StringToBool( config->GetValue( "Rip", "Rename", "1" ) );
    m_Rip_WriteTag  = StringToBool( config->GetValue( "Rip", "WriteTag", "1" ) );
    m_Rip_ID3V1     = StringToBool( config->GetValue( "Rip", "ID3V1", "1" ) );
    m_Rip_ID3V2     = StringToBool( config->GetValue( "Rip", "ID3V2", "0" ) );
}

///////////////////////////////////////////////////

void CmusikPrefs::SavePrefs()
{
	// main dialog
    config->SetValue( "Library", "Path", m_Lib_Path );

	config->SetValue( "Dialog", "Dialog Size", CSizeToString( m_Dlg_Size ) );
	config->SetValue( "Dialog", "Dialog Position", CPointToString( m_Dlg_Pos ) );
	config->SetValue( "Dialog", "Maximized", BoolToString( m_Dlg_Maximized ) );
	config->SetValue( "Dialog", "Reset UI", BoolToString( m_Dlg_ResetUI ) );
	config->SetValue( "Dialog", "Prompt File Drop Action", IntToString( m_Dlg_FileDrop_Prompt ) );
	config->SetValue( "Dialog", "Playlist Info Visible", BoolToString( m_Dlg_PlaylistInfoVisible ) );
	config->SetValue( "Dialog", "Playlist Info Viz Style", IntToString( m_Dlg_PlaylistInfoVizStyle ) );
	config->SetValue( "Dialog", "Synchronize Old Files on Startup", BoolToString( m_Dlg_SynchronizeOnStartup ) );
	config->SetValue( "Dialog", "Write Tags to File", BoolToString( m_Dlg_WriteTagsToFile ) );
    config->SetValue( "Dialog", "Minimize to Notification Tray", BoolToString( m_Dlg_MinimizeToTray ) );
    config->SetValue( "Dialog", "ShowBalloonTips", BoolToString( m_Dlg_ShowBalloonTips ) );
    config->SetValue( "Dialog", "Add Entire Library Playlist to Now Playing", BoolToString( m_Dlg_AddEntireToNP ) );
	config->SetValue( "Dialog", "Always on Top", BoolToString( m_Dlg_AlwaysOnTop ) );
	config->SetValue( "Dialog", "Use UTC", BoolToString( m_Dlg_UseUTC ) );
    config->SetValue( "Dialog", "UseGlobalHotkeys", BoolToString( m_Dlg_GlobalHotkeys ) );
	config->SetValue( "Dialog", "CloseToSystemTray", BoolToString( m_Dlg_CloseToSystemTray ) );
	config->SetValue( "Dialog", "LastPlayed", IntToString( m_Dlg_LastPlayed ) );
	config->SetValue( "Dialog", "SelectionBoxShowAll", BoolToString( m_Dlg_SelBoxShowAll ) );
	config->SetValue( "Dialog", "FreeDB Server", m_Dlg_FreeDBServer );
	config->SetValue( "Dialog", "UseMMHook", BoolToString( m_Dlg_UseMMHook ) );
	config->SetValue("Dialog", "ShowSelectionBoxes", BoolToString(m_Dlg_ShowSelBoxes));

	// tagging
	config->SetValue( "Tag", "ToFilename", m_Tag_ToFilename );
	config->SetValue( "Tag", "FromFilename", m_Tag_FromFilename );
	config->SetValue( "Tag", "GlobalFromFilename", BoolToString( m_Tag_GlobalFromFilename ) );
	config->SetValue( "Tag", "SpacesToUndr", BoolToString( m_Tag_SpacesToUndr ) );
	config->SetValue( "Tag", "UndrToSpaces", BoolToString( m_Tag_UndrToSpaces ) );

	// transparency
	config->SetValue( "Transparency", "Enabled", BoolToString( m_TransEnabled ) );
	config->SetValue( "Transparency", "Focused", IntToString( m_TransFocus ) );
	config->SetValue( "Transparency", "Unfocused", IntToString( m_TransUnfocus ) );

	// selection area
	if ( m_SelectionBox_Types.empty() )
	{
		m_SelectionBox_Types.push_back( 0 );
		m_SelectionBox_Types.push_back( 1 );
	}

	config->SetValue( "Selection Area", "Types", CIntArrayToString( m_SelectionBox_Types ) );

	// sources
	config->SetValue( "Sources", "Show SubLibs", BoolToString( m_Sources_SubLibsVisible ) );
	config->SetValue( "Sources", "Show Playlists", BoolToString( m_Sources_PlaylistsVisible ) );
	config->SetValue( "Sources", "Show Devices", BoolToString( m_Sources_DevicesVisible ) );
	config->SetValue( "Sources", "Show Dynamic Playlists", BoolToString( m_Sources_DynPlaylistsVisible ) );
	config->SetValue( "Sources", "Show Plugins", BoolToString( m_Sources_PluginsVisible ) );
	config->SetValue( "Sources", "Hidden Devices", StringArrayToString( m_Sources_Hidden_Devices ) );

	// playlist
	config->SetValue( "Playlist", "Column Order", CIntArrayToString( m_Playlist_Order ) );
	config->SetValue( "Playlist", "Column Sizes", CIntArrayToString( m_Playlist_Sizes ) );
	config->SetValue( "Playlist", "Viz Duration", IntToString( m_PlaylistInfo_VizDur ) );

	// now playing
	config->SetValue( "Now Playing", "Caption Font Size", IntToString( m_NowPlaying_CaptionFont ) );

	// player
	config->SetValue( "Player", "Driver", IntToString( m_Player_Driver ) );
	config->SetValue( "Player", "Rate", IntToString( m_Player_Rate ) );
	config->SetValue( "Player", "Volume", IntToString( m_Player_Volume ) );	
	config->SetValue( "Player", "Playmode", ULongToString( m_Player_Playmode ) );
	config->SetValue( "Player", "Buffer", IntToString( m_Player_Buffer ) );

	// crossfader
	config->SetValue( "Crossfader", "Enabled", BoolToString( m_Crossfader_Enabled ) );
	config->SetValue( "Crossfader", "Set Name", IntToString( m_Crossfader_Current ) );	

	// equalizer
	config->SetValue( "Equalizer", "Enabled", BoolToString( m_Equalizer_Enabled ) );
	config->SetValue( "Equalizer", "Set Name", IntToString( m_Equalizer_Current ) );
	config->SetValue( "Equalizer", "Band State", IntToString( m_Equalizer_Band_State ) );
	config->SetValue( "Equalizer", "Channels Locked", BoolToString( m_Equalizer_Locked ) );
	config->SetValue( "Equalizer", "Use Global Equalizer", BoolToString( m_Equalizer_Global ) );
	config->SetValue( "Equalizer", "Multiplier", FloatToString( m_Equalizer_Multiplier ) );

	// dialog colors
	config->SetValue( "Theme", "Active Caption", COLORREFToString( MUSIK_COLOR_ACTIVECAPTION ) );
	config->SetValue( "Theme", "Active Caption Text", COLORREFToString( MUSIK_COLOR_CAPTIONTEXT ) );
	config->SetValue( "Theme", "Inactive Caption", COLORREFToString( MUSIK_COLOR_INACTIVECAPTION ) );
	config->SetValue( "Theme", "Inactive Caption Text", COLORREFToString( MUSIK_COLOR_INACTIVECAPTIONTEXT ) );
	config->SetValue( "Theme", "Button Face", COLORREFToString( MUSIK_COLOR_BTNFACE ) );
	config->SetValue( "Theme", "Button Text", COLORREFToString( MUSIK_COLOR_BTNTEXT ) );
	config->SetValue( "Theme", "Button Highlight", COLORREFToString( MUSIK_COLOR_BTNHILIGHT ) );
	config->SetValue( "Theme", "Button Shadow", COLORREFToString( MUSIK_COLOR_BTNSHADOW ) );
	config->SetValue( "Theme", "List Background", COLORREFToString( MUSIK_COLOR_LISTCTRL ) );
	config->SetValue( "Theme", "List Text", COLORREFToString( MUSIK_COLOR_LISTCTRLTEXT ) );
	config->SetValue( "Theme", "Color Highlight", COLORREFToString( MUSIK_COLOR_HIGHLIGHT ) );
	config->SetValue( "Theme", "Color Highlight Text", COLORREFToString( MUSIK_COLOR_HIGHLIGHTTEXT ) );
	config->SetValue( "Theme", "Use Light Icons", BoolToString( m_UseLightIconTheme ) );

	// tunage
	config->SetValue( "Tunage", "Enabled", BoolToString( m_Tunage_Enabled ) ) ;
	config->SetValue( "Tunage", "WriteFile", BoolToString( m_Tunage_WriteFile ) ) ;
	config->SetValue( "Tunage", "Filename", m_Tunage_Filename );
	config->SetValue( "Tunage", "FileLine", m_Tunage_FileLine );
	config->SetValue( "Tunage", "AppendFile", BoolToString(	m_Tunage_AppendFile ) );
	config->SetValue( "Tunage", "PostURL", BoolToString( m_Tunage_PostURL ) );
	config->SetValue( "Tunage", "URL", m_Tunage_URL ) ;
	config->SetValue( "Tunage", "RunApp", BoolToString(	m_Tunage_RunApp ) );
	config->SetValue( "Tunage", "CmdLine", m_Tunage_CmdLine );
	config->SetValue( "Tunage", "RunOnStop", BoolToString( m_Tunage_RunOnStop ) );
	config->SetValue( "Tunage", "StoppedText", m_Tunage_StoppedText );

    // ripping
    config->SetValue( "Rip", "Path", m_Rip_Path );
    config->SetValue( "Rip", "Drive", IntToString( m_Rip_Drive ) );
    config->SetValue( "Rip", "Encoder", IntToString( m_Rip_Enc ) );
    config->SetValue( "Rip", "Quality", IntToString( m_Rip_Quality ) );
    config->SetValue( "Rip", "Rename", BoolToString( m_Rip_Rename ) );
    config->SetValue( "Rip", "WriteTag", BoolToString( m_Rip_WriteTag ) );
    config->SetValue( "Rip", "ID3V1", BoolToString( m_Rip_ID3V1 ) );
    config->SetValue( "Rip", "ID3V2", BoolToString( m_Rip_ID3V2 ) );

	// write to ini file
	config->WriteFile();
}

///////////////////////////////////////////////////

string CmusikPrefs::GetDefPlaylistOrder()
{
    stringstream sstr;
    sstr << musikCore::MUSIK_LIBRARY_TYPE_TRACKNUM << "," <<
            musikCore::MUSIK_LIBRARY_TYPE_TITLE << "," << 
            musikCore::MUSIK_LIBRARY_TYPE_ARTIST << "," << 
            musikCore::MUSIK_LIBRARY_TYPE_ALBUM << "," << 
            musikCore::MUSIK_LIBRARY_TYPE_GENRE << "," << 
            musikCore::MUSIK_LIBRARY_TYPE_DURATION << "," << 
            musikCore::MUSIK_LIBRARY_TYPE_RATING << ",";

	return sstr.str();
}

///////////////////////////////////////////////////

string CmusikPrefs::GetDefPlaylistSizes()
{
	string sRet = "50,168,100,100,80,60,70";
	return sRet;
}

///////////////////////////////////////////////////