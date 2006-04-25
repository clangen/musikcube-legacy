///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
//   multimedia library. More information at:
//
//     http://www.musikcube.com
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
//   CmusikCubeApp
//
// Filename(s): 
//
//   musikCube.h, musik.cpp
//
// Information:
//
//   musikCube Entry Point
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikCubeVersion.h"
#include "resource.h"       // main symbols
#include "musikPrefs.h"
#include "musikCubeFunctor.h"
#include <musikCore.h>

///////////////////////////////////////////////////

class CmusikCubeApp : public CWinApp
{
public:

    static CString RelToAbs( CString& path );
    static CString AbsToRel( CString& path );

	// construct and destruct
	CmusikCubeApp();
	virtual ~CmusikCubeApp();

	// overrides
	virtual BOOL InitInstance();
    virtual int ExitInstance();

	// message maps
	afx_msg void OnAppAbout();
	static CString GetUserDir();
	static CString GetWorkingDir( bool include_filename = false );

	// here for lack of a better place
	void GetCrossfader( musikCore::Crossfader* fader );

	// macros
	DECLARE_MESSAGE_MAP()

	void initCore();
	void deinitCore();
    void reloadLibrary();

private:

	static BOOL shared_IsRunning;
	static HWND shared_MainHwnd;
    ULONG_PTR m_gdiplusToken;
	void Play( const CString& fn );
};

///////////////////////////////////////////////////

void UseGlobalHotkeys( bool use = true );
void SetMainLibrary( musikCore::Library* library );

enum 
{
	PBSTRTYPE_NOWPLAYING_TITLE = 0,
	PBSTRTYPE_NOWPLAYING_ALBUM_ARTIST,
	PBSTRTYPE_TASKTRAY,
	PBSTRTYPE_BALLOONTITLE,
	PBSTRTYPE_BALLOONARTISTALBUM
};
CString GetPlaybackStr( int type );

///////////////////////////////////////////////////

class CmusikPlugin
{

public:
	// callbacks
	typedef void	(*SETPLUGINLIBRARY)( musikCore::Library* );
	typedef void	(*INITIALIZE)( musikCore::Library*, musikCore::Player*, HWND );
	typedef void	(*UNLOAD)();
	typedef HMODULE	(*GETPLUGININSTANCE)();
	typedef void	(*SETPLUGININSTANCE)( HMODULE );
	typedef void	(*CONFIGURE)();
	typedef void	(*ABOUT)();
	typedef void	(*EXECUTE)();
	typedef void	(*STOP)();
	typedef bool	(*CANCONFIGURE)();
	typedef bool	(*CANABOUT)();
	typedef bool	(*CANEXECUTE)();
	typedef bool	(*CANSTOP)();
	typedef	void	(*SETVISIBLEPLAYLIST)( musikCore::Playlist* );
	typedef void	(*ONSOURCES)();
	typedef bool	(*INSOURCES)();
	typedef bool	(*WANTSPREFS)();
	typedef void	(*SETPREFS)( CIniFile* );
	typedef musikCore::Functor&			(*GETPLUGINFUNCTOR)();
	typedef const musikCore::String&	(*GETPLUGINNAME)();
	typedef const musikCore::String&	(*GETPLUGINDESCRIPTION)();
	typedef int		(*GETPLUGINREVISION)();

	int GetRevision(){ return m_Revision; }
	void SetRevision( int rev ){ m_Revision = rev; }

public:

	CONFIGURE Configure;
	ABOUT About;
	SETPLUGININSTANCE SetPluginInstance;
	GETPLUGININSTANCE GetPluginInstance;
	INITIALIZE Initialize;
	SETPLUGINLIBRARY SetPluginLibrary;
	EXECUTE Execute;
	GETPLUGINNAME GetPluginName;
	GETPLUGINDESCRIPTION GetPluginDescription;
	STOP Stop;
	UNLOAD Unload;
	GETPLUGINFUNCTOR GetPluginFunctor;
	CANCONFIGURE CanConfigure;
	CANABOUT CanAbout;
	CANEXECUTE CanExecute;
	CANSTOP CanStop;
	SETVISIBLEPLAYLIST SetVisiblePlaylist;
	ONSOURCES OnSources;
	INSOURCES InSources;
	WANTSPREFS WantsPrefs;
	SETPREFS SetPrefs;
	GETPLUGINREVISION GetPluginRevision;

	int m_Revision;
};

namespace musikCube {
extern musikCore::Player* g_Player;
extern musikCore::Library* g_Library;
extern musikCore::Library* g_MainLibrary;
extern musikCore::Library* g_Collected;
// extern musikCore::AlarmMonitor* g_AlarmMonitor;
extern CmusikPrefs* g_Prefs;
extern CmusikCubeFunctor* g_Functor;
extern bool g_DrawGraphics;
extern int g_PlaylistType;
extern musikCore::PlaylistInfo g_PlaylistInfo;
extern std::vector<CmusikPlugin> g_Plugins;
}

void loadPlugins();
void unloadPlugins();
void synchronizeDirs();

extern COLORREF ShadeColor( COLORREF inColor, int inShiftAmount );

///////////////////////////////////////////////////

extern CmusikCubeApp theApp;

///////////////////////////////////////////////////