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

#pragma once

///////////////////////////////////////////////////

#include "3rdparty/iniFile.h"
#include <sstream>
#include <musikCore.h>

#define ToInt(x) musikCore::utf16toint(x)

///////////////////////////////////////////////////

using namespace std;

///////////////////////////////////////////////////

class CmusikPrefs
{
	friend class CmusikThemeEditorDlg;

public:

	// construct and destruct
    CmusikPrefs( musikCore::String& fn );
	~CmusikPrefs();

	//
	// basic stuff
	//

	void LoadPrefs();
	void SavePrefs();

	//
	// main dialog
	//

    string GetLibraryPath(){ return m_Lib_Path; }
    void SetLibraryPath( wstring& str ){ m_Lib_Path = musikCore::utf16to8( str, false ); }

	CSize GetDlgSize(){ return m_Dlg_Size; }
	CPoint GetDlgPos(){ return m_Dlg_Pos; }
	bool IsMaximized(){ return m_Dlg_Maximized; }
	bool ResetUI(){ return m_Dlg_ResetUI; }
	int GetFileDropPrompt(){ return m_Dlg_FileDrop_Prompt; }
	bool PlaylistInfoVisible(){ return m_Dlg_PlaylistInfoVisible; }
	int  GetPlaylistInfoVizStyle() { return m_Dlg_PlaylistInfoVizStyle; };
	bool SynchronizeOnStartup(){ return m_Dlg_SynchronizeOnStartup; }
	bool WriteTagsToFile(){ return m_Dlg_WriteTagsToFile; }
	bool MinimizeToTray(){ return m_Dlg_MinimizeToTray; }
    bool GetShowBalloonTips(){ return m_Dlg_ShowBalloonTips; }
	bool AddEntireToNP(){ return m_Dlg_AddEntireToNP; }
	bool IsAlwaysOnTop(){ return m_Dlg_AlwaysOnTop; }
	bool GetUseUTC(){ return m_Dlg_UseUTC; }
    bool GetUseGlobalHotkeys(){ return m_Dlg_GlobalHotkeys; }
	bool GetUseWMediaKeys(){ return m_Dlg_WMediaKeys; }
	bool GetCloseToSystemTray(){ return m_Dlg_CloseToSystemTray; }
	int  GetDlgLastPlayed(){ return m_Dlg_LastPlayed; }
	bool GetSelBoxShowAll(){ return m_Dlg_SelBoxShowAll; }
	string GetFreeDBServer(){ return m_Dlg_FreeDBServer; }
	bool GetDlgUseMMHook(){ return m_Dlg_UseMMHook; }
	bool ShowSelectionBoxes(){ return m_Dlg_ShowSelBoxes; }

	void SetDlgSize( const CSize sz ){ m_Dlg_Size = sz; }
	void SetDlgPos( const CPoint pt ){ m_Dlg_Pos = pt; }
	void SetMaximized( bool max ){ m_Dlg_Maximized = max; }
	void SetDlgResetUI( bool reset ){ m_Dlg_ResetUI = reset; }
	void SetFileDropPrompt( int prompt ){ m_Dlg_FileDrop_Prompt = prompt; }
	void SetPlaylistInfoVisible( bool visible )	{ m_Dlg_PlaylistInfoVisible = visible; }
	void SetPlaylistInfoVizStyle( int vizstyle ) { m_Dlg_PlaylistInfoVizStyle = vizstyle; }
	void SetSynchronizeOnStartup( bool synchronize ){ m_Dlg_SynchronizeOnStartup = synchronize; }
	void SetWriteTagsToFile( bool write ){ m_Dlg_WriteTagsToFile = write; }
	void SetMinimizeToTray( bool tray ){ m_Dlg_MinimizeToTray = tray; }
    void SetShowBalloonTips( bool show ){ m_Dlg_ShowBalloonTips = show; }
	void SetAddEntireToNP( bool entire ){ m_Dlg_AddEntireToNP = entire; }
	void SetAlwaysOnTop( bool on_top ){ m_Dlg_AlwaysOnTop = on_top; }
	void SetUseUTC( bool use_utc ){ m_Dlg_UseUTC = use_utc; }
    void SetUseGlobalHotkeys( bool use ){ m_Dlg_GlobalHotkeys = use; }
	void SetUseWMediaKeys( bool use ){ m_Dlg_WMediaKeys = use; }
	void SetCloseToSystemTray( bool close ){ m_Dlg_CloseToSystemTray = close; }
	void SetDlgLastPlayed( int last ){ m_Dlg_LastPlayed = last; }
	void SetSelBoxShowAll( bool showall	) { m_Dlg_SelBoxShowAll = showall; }
	void SetFreeDBServer( const wstring& str ){ m_Dlg_FreeDBServer = musikCore::utf16to8( str, false ); }
	bool SetDlgUseMMHook( bool use ){ m_Dlg_UseMMHook = use; }
	void SetShowSelectionBoxes(bool show){ m_Dlg_ShowSelBoxes = show; }

	void AppendPlaylistColOrder( int col ){ m_Playlist_Order.push_back( col ); }
	void AppendPlaylistColSize( int size ){ m_Playlist_Sizes.push_back( size ); }
	void RemovePlaylistColOrder( int at ){ m_Playlist_Order.erase( m_Playlist_Order.begin() + at ); }
	void RemovePlaylistColSize( int at ){ m_Playlist_Sizes.erase( m_Playlist_Sizes.begin() + at ); }

	//
	// tagging
	//

	string GetTagToFilename(){ return m_Tag_ToFilename; }
	string GetTagFromFilename(){ return m_Tag_FromFilename; }
	bool GetTagsFromFilename(){ return m_Tag_GlobalFromFilename; }
	bool GetTagSpacesToUndr(){ return m_Tag_SpacesToUndr; }
	bool GetTagUndrToSpaces(){ return m_Tag_UndrToSpaces; }

	void SetTagToFilename( const wstring& str ){ m_Tag_ToFilename = musikCore::utf16to8( str, false ); }
	void SetTagFromFilename( const wstring& str ){ m_Tag_FromFilename = musikCore::utf16to8( str, false ); }
	void SetGetTagsFromFilename( bool set ){ m_Tag_GlobalFromFilename = set; }
	void SetTagSpacesToUndr( bool set ){ m_Tag_SpacesToUndr = set; }
	void SetTagUndrToSpaces( bool set ){ m_Tag_UndrToSpaces = set; }

	//
	// selection box
	//

	size_t GetSelBoxCount(){ return m_SelectionBox_Types.size(); }
	int GetSelBoxType( int index ){ return m_SelectionBox_Types.at( index ); }

	void SetSelBoxType( int index, int type ){ m_SelectionBox_Types.at( index ) = type; }
	void SetNewSelBoxType( int type ){ m_SelectionBox_Types.push_back( type ); }
	void SetSelBoxTypes( const musikCore::IntArray& types ){ m_SelectionBox_Types = types; }

	//
	// sources
	//

	bool IsSubLibsVisible(){ return m_Sources_SubLibsVisible; }
	bool IsPlaylistsVisible(){ return m_Sources_PlaylistsVisible; }
	bool IsDevicesVisible(){ return m_Sources_DevicesVisible; }
	bool IsDynPlaylistsVisible(){ return m_Sources_DynPlaylistsVisible; }
	bool IsPluginsVisible(){ return m_Sources_PluginsVisible; }
	musikCore::StringArray GetHiddenDevices(){ return m_Sources_Hidden_Devices; }

	void SetSubLibsVisible( bool visible ){ m_Sources_SubLibsVisible = visible; }
	void SetPlaylistsVisible( bool visible ){ m_Sources_PlaylistsVisible = visible; }
	void SetDevicesVisible( bool visible ){ m_Sources_DevicesVisible = visible; }	
	void SetDynPlaylistsVisible( bool visible ){ m_Sources_DynPlaylistsVisible = visible; }
	void SetPluginsVisible( bool visible ){ m_Sources_PluginsVisible = visible; }
	void SetHiddenDevices( musikCore::StringArray& devices ){ m_Sources_Hidden_Devices = devices; }

	//
	// playlist 
	//

	musikCore::IntArray GetPlaylistOrder(){ return m_Playlist_Order; }
	musikCore::IntArray GetPlaylistSizes(){ return m_Playlist_Sizes; }
	int GetPlaylistCol( int n ){ return m_Playlist_Order.at( n ); }
	int	GetPlaylistColWidth( int n ){ return m_Playlist_Sizes.at( n ); }
	size_t GetPlaylistColCount(){ return m_Playlist_Order.size(); }
	int GetPlaylistVizDur(){ return m_PlaylistInfo_VizDur; }

	void SetPlaylistOrder( musikCore::IntArray n ){ m_Playlist_Order = n; }	
	void SetPlaylistSizes( musikCore::IntArray n ){ m_Playlist_Sizes = n; }
	void SetPlaylistVizDur( int duration ) { m_PlaylistInfo_VizDur = duration; }

	//
	// transparency
	//

	bool IsTransEnabled(){ return m_TransEnabled; }
	int GetTransFocus(){ return m_TransFocus; }
	int GetTransUnFocus(){ return m_TransUnfocus; }

	void SetTransEnabled( bool enabled ){ m_TransEnabled = enabled; }
	void SetTransFocus( int trans ){ m_TransFocus = trans; }
	void SetTransUnFocus( int trans ){ m_TransUnfocus = trans; }

	//
	// now playing
	//

	int	GetNowPlayingCaptionFontSize(){ return m_NowPlaying_CaptionFont; }
	void SetNowPlayingCaptionFontSize( int y ){ m_NowPlaying_CaptionFont = y; }

	//
	// player
	//

	int	GetPlayerDriver(){ return m_Player_Driver; }					
	int	GetPlayerRate(){ return m_Player_Rate; }
	int	GetPlayerVolume(){ return m_Player_Volume; }
	int GetPlayerBuffer(){ return m_Player_Buffer; }
	unsigned long GetPlayerPlaymode(){ return m_Player_Playmode; }

	void SetPlayerDriver( int n ){ m_Player_Driver = n; }
	void SetPlayerRate( int n ){ m_Player_Rate = n; }
	void SetPlayerVolume( int n ){ m_Player_Volume = n; }
	void SetPlayerBuffer( int n ){ m_Player_Buffer = n;	}
	void SetPlayerPlaymode( unsigned long un ){ m_Player_Playmode = un; }

	//
	// crossfader
	//
	
	int  GetCrossfader(){ return m_Crossfader_Current; }
	bool IsCrossfaderEnabled(){ return m_Crossfader_Enabled; }

	void SetCrossfader( int n ){ m_Crossfader_Current = n; }
	void SetCrossfaderEnabled( bool b ){ m_Crossfader_Enabled = b; }

	//
	// equalizer
	//

	int GetEqualizer(){ return m_Equalizer_Current; }
	bool IsEqualizerEnabled(){ return m_Equalizer_Enabled; }
	int GetEqualizerBandState(){ return m_Equalizer_Band_State; }
	bool IsEqualizerChannelsLocked(){ return m_Equalizer_Locked; }
	bool EqualizerUseGlobal(){ return m_Equalizer_Global; }
	float GetEqualizerMultiplier(){ return m_Equalizer_Multiplier; }

	void SetEqualizer( int n ){ m_Equalizer_Current = n; }
	void SetEqualizerEnabled( bool b ){ m_Equalizer_Enabled = b; }
	void SetEqualizerBandState( int n ){ m_Equalizer_Band_State = n; }
	void SetEqualizerChannelsLocked( bool b ){ m_Equalizer_Locked = b; }
	void SetEqualizerUseGlobal( bool b ){ m_Equalizer_Global = b; }
	void SetEqualizerMultiplier( const float& multiplier ){ m_Equalizer_Multiplier = multiplier; }

	bool GetTunageEnabled() { return m_Tunage_Enabled; }
	bool GetTunageWriteFile() { return m_Tunage_WriteFile; }
	string GetTunageFilename() { return m_Tunage_Filename; }
	string GetTunageFileLine() { return m_Tunage_FileLine; }
	bool GetTunageAppendFile() { return m_Tunage_AppendFile; }
	bool GetTunagePostURL() { return m_Tunage_PostURL; }
	string GetTunageURL() { return m_Tunage_URL; }
	bool GetTunageRunApp() { return m_Tunage_RunApp; }
	string GetTunageCmdLine() { return m_Tunage_CmdLine; }
	bool GetTunageRunOnStop() { return m_Tunage_RunOnStop; }
	string GetTunageStoppedText() { return m_Tunage_StoppedText; }

	void SetTunageEnabled( bool val ) { m_Tunage_Enabled = val; }
	void SetTunageWriteFile( bool val ) { m_Tunage_WriteFile = val; }
	void SetTunageFilename( wstring str ) { m_Tunage_Filename = musikCore::utf16to8( str, false ); }
	void SetTunageFileLine( wstring str ) { m_Tunage_FileLine = musikCore::utf16to8( str, false ); }
	void SetTunageAppendFile( bool val ) {  m_Tunage_AppendFile = val; }
	void SetTunagePostURL( bool val ) {  m_Tunage_PostURL = val; }
	void SetTunageURL( wstring str ) {  m_Tunage_URL = musikCore::utf16to8( str, false ); }
	void SetTunageRunApp( bool val ) { m_Tunage_RunApp = val; }
	void SetTunageCmdLine( wstring str ) {  m_Tunage_CmdLine = musikCore::utf16to8( str, false ); }
	void SetTunageRunOnStop( bool val ) {  m_Tunage_RunOnStop = val; }
	void SetTunageStoppedText( wstring str ) { m_Tunage_StoppedText = musikCore::utf16to8( str, false ); }


    // 
    // ripping
    // 
    string GetRipPath(){ return m_Rip_Path; }
    int GetRipDrive(){ return m_Rip_Drive; }
    int GetRipEnc(){ return m_Rip_Enc; }
    int GetRipQuality(){ return m_Rip_Quality; }
    bool GetRipRename(){ return m_Rip_Rename; }
    bool GetRipWriteTag(){ return m_Rip_WriteTag; }
    bool GetRipID3V1(){ return m_Rip_ID3V1; }
    bool GetRipID3V2(){ return m_Rip_ID3V2; }

    void SetRipPath( wstring str ){ m_Rip_Path = musikCore::utf16to8( str, false ); }
    void SetRipDrive( int val ){ m_Rip_Drive = val; }
    void SetRipEnc( int val ){ m_Rip_Enc = val; }
    void SetRipQuality( int val ){ m_Rip_Quality = val; }
    void SetRipRename( bool val ){ m_Rip_Rename = val; }
    void SetRipWriteTag( bool val ){ m_Rip_WriteTag = val; }
    void SetRipID3V1( bool val ){ m_Rip_ID3V1 = val; }
    void SetRipID3V2( bool val ){ m_Rip_ID3V2 = val; }


	//
	// theme
	//
	inline COLORREF GetColor( COLORREF& inSource, int inSysColorIndex )
	{
		if ( inSource == AUTOMATIC_COLOR )
		{
			return GetSysColor( inSysColorIndex );
		}
		return inSource;
	}
	
	bool UseLightIconTheme(){ return m_UseLightIconTheme; }
	void SetUseLightIconTheme( bool inUseLightTheme ){ m_UseLightIconTheme = inUseLightTheme; }

	COLORREF GetActiveCaptionColor(){ return GetColor( MUSIK_COLOR_ACTIVECAPTION, COLOR_ACTIVECAPTION ); }
	COLORREF GetActiveCaptionTextColor(){ return GetColor( MUSIK_COLOR_CAPTIONTEXT, COLOR_CAPTIONTEXT ); }
	COLORREF GetInactiveCaptionColor(){ return GetColor( MUSIK_COLOR_INACTIVECAPTION, COLOR_INACTIVECAPTION ); }
	COLORREF GetInactiveCaptionTextColor(){ return GetColor( MUSIK_COLOR_INACTIVECAPTIONTEXT, COLOR_INACTIVECAPTIONTEXT ); }
	COLORREF GetBtnFaceColor(){ return GetColor( MUSIK_COLOR_BTNFACE, COLOR_BTNFACE ); }
	COLORREF GetBtnTextColor(){ return GetColor( MUSIK_COLOR_BTNTEXT, COLOR_BTNTEXT ); }
	COLORREF GetBtnHilightColor(){ return GetColor( MUSIK_COLOR_BTNHILIGHT, COLOR_BTNHILIGHT ); }
	COLORREF GetBtnShadowColor(){ return GetColor( MUSIK_COLOR_BTNSHADOW, COLOR_BTNSHADOW ); }
	COLORREF GetListCtrlColor(){ return GetColor( MUSIK_COLOR_LISTCTRL, COLOR_BTNHILIGHT ); }
	COLORREF GetListCtrlTextColor(){ return GetColor( MUSIK_COLOR_LISTCTRLTEXT, COLOR_BTNTEXT ); }
	COLORREF GetHilightColor(){ return GetColor( MUSIK_COLOR_HIGHLIGHT, COLOR_HIGHLIGHT ); }
	COLORREF GetHilightTextColor(){ return GetColor( MUSIK_COLOR_HIGHLIGHTTEXT, COLOR_HIGHLIGHTTEXT ); }

protected:

	CIniFile* config;
	string GetDefPlaylistOrder();
	string GetDefPlaylistSizes();

private:

	// theme
	COLORREF MUSIK_COLOR_ACTIVECAPTION;
	COLORREF MUSIK_COLOR_CAPTIONTEXT;
	COLORREF MUSIK_COLOR_INACTIVECAPTION;
	COLORREF MUSIK_COLOR_INACTIVECAPTIONTEXT;
	COLORREF MUSIK_COLOR_BTNFACE;
	COLORREF MUSIK_COLOR_BTNTEXT;
	COLORREF MUSIK_COLOR_BTNHILIGHT;
	COLORREF MUSIK_COLOR_BTNSHADOW;
	COLORREF MUSIK_COLOR_LISTCTRL;
	COLORREF MUSIK_COLOR_LISTCTRLTEXT;
	COLORREF MUSIK_COLOR_HIGHLIGHT;
	COLORREF MUSIK_COLOR_HIGHLIGHTTEXT;
	bool m_UseLightIconTheme;

	// main dialog
	CSize	m_Dlg_Size;
	CPoint	m_Dlg_Pos;
	bool	m_Dlg_Maximized;
	bool	m_Dlg_ResetUI;
	int		m_Dlg_FileDrop_Prompt;
	bool	m_Dlg_PlaylistInfoVisible;
	int		m_Dlg_PlaylistInfoVizStyle;
	bool	m_Dlg_SynchronizeOnStartup;
	bool	m_Dlg_WriteTagsToFile;
	bool	m_Dlg_MinimizeToTray;
	bool	m_Dlg_AddEntireToNP;
	bool	m_Dlg_AlwaysOnTop;
	bool	m_Dlg_UseUTC;
    bool    m_Dlg_ShowBalloonTips;
    bool    m_Dlg_GlobalHotkeys;
	bool    m_Dlg_WMediaKeys;
    string  m_Lib_Path;
	bool	m_Dlg_CloseToSystemTray;
	int		m_Dlg_LastPlayed;
	bool	m_Dlg_SelBoxShowAll;
	string	m_Dlg_FreeDBServer;
	bool	m_Dlg_UseMMHook;
	bool	m_Dlg_ShowSelBoxes;

	// tagging
	string	m_Tag_ToFilename;
	string	m_Tag_FromFilename;
	bool m_Tag_GlobalFromFilename;
	bool m_Tag_SpacesToUndr;
	bool m_Tag_UndrToSpaces;

	// sources
	bool m_Sources_SubLibsVisible;
	bool m_Sources_PlaylistsVisible;
	bool m_Sources_DevicesVisible;
	bool m_Sources_DynPlaylistsVisible;
	bool m_Sources_PluginsVisible;
	musikCore::StringArray m_Sources_Hidden_Devices;

	// transparency
	bool m_TransEnabled;
	int m_TransFocus;
	int m_TransUnfocus;

	// selection area
	musikCore::IntArray m_SelectionBox_Types;

	// playlist
	musikCore::IntArray m_Playlist_Order;
	musikCore::IntArray m_Playlist_Sizes;
	int m_PlaylistInfo_VizDur;

	// now playing
	int m_NowPlaying_CaptionFont;

	// player
	int m_Player_Device;
	int m_Player_Driver;
	int m_Player_Rate;
	int m_Player_Max_Channels;
	int m_Player_Volume;
	int m_Player_Buffer;
	unsigned long m_Player_Playmode;

	// crossfader
	int m_Crossfader_Current;
	bool m_Crossfader_Enabled;

	// equalizer
	int m_Equalizer_Current;
	bool m_Equalizer_Enabled;
	int m_Equalizer_Band_State;
	bool m_Equalizer_Locked;
	bool m_Equalizer_Global;
	float m_Equalizer_Multiplier;

	// theme aware stuff
	bool m_ActiveCaptionDef;
	bool m_CaptionDef;
	bool m_InactiveCaptionDef;
	bool m_InactiveCaptionTextDef;
	bool m_BtnFaceDef;
	bool m_BtnTextDef;
	bool m_BtnHilightDef;
	bool m_BtnShadowDef;
	bool m_ListCtrlDef;
	bool m_ListCtrlTextDef;
	bool m_HilightDef;
	bool m_HilightTextDef;

	// tunage
	bool m_Tunage_Enabled;
	bool m_Tunage_WriteFile;
	string m_Tunage_Filename;
	string m_Tunage_FileLine;
	bool m_Tunage_AppendFile;
	bool m_Tunage_PostURL;
	string m_Tunage_URL;
	bool m_Tunage_RunApp;
	string m_Tunage_CmdLine;
	bool m_Tunage_RunOnStop;
	string m_Tunage_StoppedText;

    // rip
    string m_Rip_Path;
    int m_Rip_Drive;
    int m_Rip_Enc;
    int m_Rip_Quality;
    bool m_Rip_Rename;
    bool m_Rip_WriteTag;
    bool m_Rip_ID3V1;
    bool m_Rip_ID3V2;

};

///////////////////////////////////////////////////

// handy conversion functions

///////////////////////////////////////////////////

inline int StringToInt( const string str )
{
	return atoi( str.c_str() );	
}

///////////////////////////////////////////////////

inline string IntToString( const int &n )
{
	char buffer[20];
	itoa( n, buffer, 10 );
	string str = buffer;
    return str;	
}

///////////////////////////////////////////////////

#define StoI(x) musikCore::StringToInt(x)
#define StoF(x) musikCore::StringToFloat(x)

inline wstring ItoS( const int &n )
{
    musikCore::String str;
    str.Format( _T( "%d" ), n );
    return str;    
}

///////////////////////////////////////////////////

inline float StringToFloat( string s )
{
	float fRet;

	istringstream aStream;
	aStream.str( s );
	aStream >> (float)fRet;

	return fRet;
}

///////////////////////////////////////////////////

inline string FloatToString( float f )
{
	ostringstream aStream;
	aStream << f << "\0";

	string str = aStream.str();

	return str;
}

///////////////////////////////////////////////////

inline CString FloatToCString( float f )
{
	CString str;
    str.Format( _T( "%.2f" ), f );
	return str;
}

///////////////////////////////////////////////////

inline CString IntToCString( int n )
{
	CString str;
    str.Format( _T( "%d" ), n );
	return str;
}

///////////////////////////////////////////////////

inline unsigned long StringToULong( string s )
{
	unsigned long nRet;

	istringstream aStream;
	aStream.str( s );
	aStream >> (unsigned long)nRet;

	return nRet;
}

///////////////////////////////////////////////////

inline string ULongToString( unsigned long ul )
{
	ostringstream aStream;
	aStream << ul << "\0";

	string str = aStream.str();

	return str;
}

///////////////////////////////////////////////////

inline CSize StringToCSize( const string &str )
{
	CSize ret;

	musikCore::StringArray tokens;
	musikCore::Filename::DelimitStr( str.c_str(), _T( "x" ), tokens, false, true, true );

	if ( tokens.size() >= 2 )
	{
		ret.cx = ToInt( tokens.at( 0 ) );
		ret.cy = ToInt( tokens.at( 1 ) );
	}
	else
	{
		ret.cx = 0;
		ret.cy = 0;
	}

	return ret;
}

///////////////////////////////////////////////////

inline string CSizeToString( const CSize &size )
{
	string ret;
	char buffer_w[10];
	char buffer_h[10];

	itoa( size.cx, buffer_w, 10 );
	itoa( size.cy, buffer_h, 10 );

	string str;
	str += buffer_w;
	str += "x";
	str += buffer_h;

	return str;
}

///////////////////////////////////////////////////

inline CPoint StringToCPoint( const string &str )
{
	CPoint ret;

	musikCore::StringArray tokens;
	musikCore::Filename::DelimitStr( str.c_str(), _T( "," ), tokens, false, true, true );

	if ( tokens.size() >= 2 )
	{
		ret.x = ToInt( tokens.at( 0 ) );
		ret.y = ToInt( tokens.at( 1 ) );
	}
	else
	{
		ret.x = 0;
		ret.y = 0;
	}

	return ret;
}

///////////////////////////////////////////////////

inline string CPointToString( const CPoint &pt )
{
	string ret;
	char buffer_x[10];
	char buffer_y[10];

	itoa( pt.x, buffer_x, 10 );
	itoa( pt.y, buffer_y, 10 );

	string str;
	str += buffer_x;
	str += ",";
	str += buffer_y;

	return str;
}

///////////////////////////////////////////////////

// this could be optimised, but is not a big deal
// becuase for our use the list will probably never,
// ever, ever grow above 10 items...

inline musikCore::IntArray StringToCIntArray( const string &str )
{
	musikCore::IntArray ret;

	musikCore::StringArray tokens;
	musikCore::Filename::DelimitStr( str.c_str(), _T( "," ), tokens, false, true, true );

	for ( size_t i = 0; i < tokens.size(); i++ )
		ret.push_back( ToInt( tokens.at( i ) ) );

	return ret;
}

///////////////////////////////////////////////////

inline string CIntArrayToString( const musikCore::IntArray &array )
{
    string ret;

	for ( size_t i = 0; i < array.size(); i++ )
	{
		ret += IntToString( array.at( i ) );
		if ( i != ( array.size() - 1 ) )
			ret += ",";
	}
	return ret;
}

///////////////////////////////////////////////////

inline COLORREF StringToCOLORREF( const string str )
{
	char* pEnd;
	return (COLORREF)strtoul( str.c_str(), &pEnd, 0 );
}

///////////////////////////////////////////////////

inline string COLORREFToString( const COLORREF color )
{
    std::stringstream sstr;
    sstr << (DWORD)color;
	return sstr.str();
}

///////////////////////////////////////////////////

inline bool StringToBool( const string str )
{
	if ( str == "1" )
		return true;

	return false;
}

///////////////////////////////////////////////////

inline string BoolToString( const bool val )
{
	if ( val )
		return "1";

	return "0";
}

///////////////////////////////////////////////////

inline string StringArrayToString( musikCore::StringArray& array )
{
	musikCore::String target;
	for ( size_t i = 0; i < array.size(); i++ )
	{
		if ( i != 0 )
			target += _T( "," );
		target += array.at( i );
	}

	return musikCore::utf16to8( target, false );
}

///////////////////////////////////////////////////
