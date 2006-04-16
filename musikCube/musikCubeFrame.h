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
//   CmusikCubeFrame,
//
// Filename(s): 
//
//   musikCubeFrame.h, musikCubeFrame.cpp
//
// Information:
//
//   musikCube's main SDI window.
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikCubeFunctor.h"
#include "musikSourcesCtrl.h"
#include "musikNowPlayingCtrl.h"
#include "musikSelectionCtrl.h"
#include "musikEqualizerCtrl.h"
#include "musikPlaylistCtrl.h"
#include "musikPlaylistView.h"
#include "musikDirSync.h"
#include "musikRipDlg.h"
#include "musikPluginManagerDlg.h"
#include "MMShellHook/MMShellHook.h"

#include <musikCore.h>

#include <vector>

#include <OpenThreads/Mutex>
using namespace OpenThreads;

///////////////////////////////////////////////////

// these need to remain constant, or
// problems may arise. they are the
// ids for the dock bars

#define ID_CROSSFADER 1342
#define ID_EQUALIZER 1343
#define ID_NOWPLAYING 1344
#define ID_SOURCESBOX 1345
#define ID_DIRBOX 1341
#define ID_SELECTIONBOX_START 1346

///////////////////////////////////////////////////

// task tray notification stuff

#define ID_NOTIFY_ICON 125
#define TRAY_NOTIFY_EVT (WM_USER+1245)

///////////////////////////////////////////////////

enum 
{
	SYSMENU_RESETDIALOG = 85904,
	SYSMENU_PLAYSEL,
	SYSMENU_PAUSERESUME,
	SYSMENU_NEXT,
	SYSMENU_PREV,
	SYSMENU_STOP
};

///////////////////////////////////////////////////

class CmusikCubeFrame;

///////////////////////////////////////////////////

// transparency

#ifndef WS_EX_LAYERED
	#define WS_EX_LAYERED           0x00080000
	#define LWA_COLORKEY            0x00000001
	#define LWA_ALPHA               0x00000002
#endif

///////////////////////////////////////////////////

// these are our library imports

///////////////////////////////////////////////////

// transparency

typedef BOOL (WINAPI *lpfnSetLayeredWindowAttributes)(HWND hWnd, 
								COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

///////////////////////////////////////////////////

typedef std::vector<CmusikSelectionBar*> CmusikSelBarArray;

///////////////////////////////////////////////////

class CmusikCubeUpdater : public musikCore::Task
{

public:

	void run();
	CmusikCubeFrame* m_Parent;
};

///////////////////////////////////////////////////

class CmusikCubeFrame : public CFrameWnd
{

	// the threads and certain drag and drop operations
	// are allowed access to our private members.
	friend void CmusikSourcesCtrl::DoDrag( CmusikPropTreeItem* pItem );
	friend class CmusikCubeFader;
	friend class CmusikCubeUpdater;

public:

	// construct and destruct
	CmusikCubeFrame( bool autostart = false );
	virtual ~CmusikCubeFrame();

	// overrides
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext );
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// debug stuffs
	#ifdef _DEBUG
		virtual void AssertValid() const;
		virtual void Dump(CDumpContext& dc) const;
	#endif

	// misc
	bool PlayCmd( const CString& fn );
	void RestoreFromTray();
    void SendToTray();
	void SaveWindowState();
	int  GetSelPlaylistType();

	int m_Progress;
	CString m_Caption;

	// tray icon
	void ShowTrayIcon();
	void HideTrayIcon();
	bool m_IsMinimized;

	// to save window state
	virtual BOOL DestroyWindow();
	BOOL VerifyBarState( LPCTSTR lpszProfileName );

	// unique drag identifiers
	UINT m_uSourcesDrop;
	UINT m_uPlaylistDrop_L;
	UINT m_uPlaylistDrop_R;
	UINT m_uSelectionDrop_L;
	UINT m_uSelectionDrop_R;

	// transparency
	lpfnSetLayeredWindowAttributes m_pSetLayeredWindowAttributes;
	void SetTransparency( int trans );
	void InitTrans( bool set_trans = false );
	void DeinitTrans();
	bool IsTransEnb(){ return m_TransEnb; }
	void OnFirstStart();

    // ui updating
    enum
    {
        MUSIKUI_NONE = 0,
        MUSIKUI_RESET = 1,
        MUSIKUI_REQUERY = 2,
        MUSIKUI_REFRESH = 3
    };
    void ResetUI( bool coordinates_only = false );
    void RefreshUI( int sources, int selboxes, int playlist, CmusikSelectionCtrl* sender = NULL, bool title = true );
    void UpdateTitle();
    void ResetSelBoxes();
    void RequeryPlaylist( CmusikSelectionCtrl* sender = NULL );
	void RequerySelBoxes( CmusikSelectionCtrl* sender = NULL );
	musikCore::String GetSelQuery( CmusikSelectionCtrl* target = NULL, bool for_playlist = false, bool is_playlist = false );

protected: 

    Mutex m_GUI_Update;

	// init and clean musik core components
	void initCube();
	void deinitCube();
	void InitDragTypes();
	
	// caching library selction
	void CacheSelBoxes();
	void UncacheSelBoxes();
	CmusikSelectionCtrl* GetLastSelBox();

	// resetting controls
	void LoadDlgSize();

	// icons
	HICON m_hIcon16;
	HICON m_hIcon32;
	HICON m_Tray_Playing;
	HICON m_Tray_Stopped;

	// current playlist, only a pointer
	void SetPlaylist( bool hide_sort_arrow = true, bool select_none = false );
	void SetNowPlaying();
	musikCore::Playlist* m_Playlist;

	// the actual controls
	CmusikPlaylistView*		m_wndView;
	CmusikSourcesBar*		m_wndSources;
	CmusikNowPlayingBar*	m_wndNowPlaying;
	CmusikSelBarArray		m_wndSelectionBars;
	CmusikEqualizerBar*		m_wndEqualizer;
    //CmusikDirBrowseBar*     m_wndDirBrowse;
    CmusikRipDlg*           m_RipDlg;

	void ShowSelectionBoxes( bool show = true, bool force = false );

	// utility functions
	void DockBarLeftOf( CSizingControlBar* Bar, CSizingControlBar* LeftOf );

	// custom message maps
	afx_msg LRESULT OnUpdateSel( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSongChange( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnEqualizerChange( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSongStop( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSongPause( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSongResume( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesLibrary( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesNowPlaying( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesSubLib( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesDynPlaylist( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesStdPlaylist( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesQuickSearch( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesQuickSearchStr( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnDragStart( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnDragEnd( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSelBoxEditCommit( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnTaskProgress( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnTaskEnd( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnPlayerPlaySel( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnVerifyPlaylist( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCloseDirSync( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSelBoxAddRemove( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSelBoxRequestUpdate( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSelBoxDeSel( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnUpdateCurrPlaylist( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSelBoxDelSel( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesItemClicked( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnInitTrans( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnDeinitTrans( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnPlaySong( WPARAM wParam, LPARAM lParam = NULL );
	afx_msg LRESULT OnNowPlayingUpdated( WPARAM wParam = NULL, LPARAM lParam = NULL );
	afx_msg LRESULT OnPlayerUpdate( WPARAM wParam = NULL, LPARAM lParam = NULL );
    afx_msg LRESULT OnHotKey( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnSourcesCDAudio( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnSourcesQueryFreeDB( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnSelBoxFilter( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnSourcesRemovableDev( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesRefresh( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesReset( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesNetstreams( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSelectionBoxReset( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSelectionBoxActivate( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesCollected( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSourcesUnsel( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnTaskbarCreated( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnShowCube( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnHideCube( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnUpdateVolume( WPARAM wParam, LPARAM lParam );

	// mfc message maps
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSysColorChange();
	afx_msg void OnFilePreferences();
	afx_msg void OnDestroy();
	afx_msg void OnAddFiles();
	afx_msg void OnAddDirectory();
	afx_msg void OnFileSaveplaylist();
	afx_msg void OnViewPlaylistinformation();
	afx_msg void OnUpdateViewSources(CCmdUI *pCmdUI);
	afx_msg void OnViewSources();
	afx_msg void OnUpdateViewSelectionboxes(CCmdUI *pCmdUI);
	afx_msg void OnViewNowplaying();
	afx_msg void OnUpdateViewNowplaying(CCmdUI *pCmdUI);
	afx_msg void OnAudioEqualizerEnabled();
	afx_msg void OnUpdateAudioEqualizerEnabled(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAudioCrossfaderEnabled(CCmdUI *pCmdUI);
	afx_msg void OnAudioCrossfaderEnabled();
	afx_msg void OnUpdateViewPlaylistinformation(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileSaveplaylist(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePlaybackmodeRepeatsingle(CCmdUI *pCmdUI);
	afx_msg void OnPlaybackmodeRepeatsingle();
	afx_msg void OnUpdatePlaybackmodeRepeatplaylist(CCmdUI *pCmdUI);
	afx_msg void OnPlaybackmodeRepeatplaylist();
	afx_msg void OnUpdatePlaybackmodeIntro(CCmdUI *pCmdUI);
	afx_msg void OnPlaybackmodeIntro();
	afx_msg void OnUnsynchronizedtagsView();
	afx_msg void OnUnsynchronizedtagsWritetofile();
	afx_msg void OnUnsynchronizedtagsFinalizefordatabaseonly();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnViewEqualizer();
	afx_msg void OnUpdateViewEqualizer(CCmdUI *pCmdUI);
	afx_msg void OnFileSynchronizeddirectories();
	afx_msg void OnUpdateFileSynchronizeddirectories(CCmdUI *pCmdUI);
	afx_msg void OnViewResetinterfacetodefault();
	afx_msg void OnNotificationtrayExit();
	afx_msg void OnNotificationtrayRestore();
	afx_msg void OnUpdateNotificationtrayPlay(CCmdUI *pCmdUI);
	afx_msg void OnNotificationtrayPlay();
	afx_msg void OnUpdateNotificationtrayPause(CCmdUI *pCmdUI);
	afx_msg void OnNotificationtrayPause();
	afx_msg void OnUpdateNotificationtrayNext(CCmdUI *pCmdUI);
	afx_msg void OnNotificationtrayNext();
	afx_msg void OnUpdateNotificationtrayPrev(CCmdUI *pCmdUI);
	afx_msg void OnNotificationtrayPrev();
	afx_msg void OnUpdateNotificationtrayStop(CCmdUI *pCmdUI);
	afx_msg void OnNotificationtrayStop();
	afx_msg void OnFileClearlibrary();
	afx_msg void OnLibrarySynchronizedirectoriesnow();
	afx_msg void OnPlaybackmodeShufflecurrentplaylist();
	afx_msg void OnUpdatePlaybackmodeShufflecurrentplaylist(CCmdUI *pCmdUI);
	afx_msg void OnViewVisualization();
	afx_msg void OnUpdateViewVisualization(CCmdUI *pCmdUI);
	afx_msg void OnViewAlwaysontop();
	afx_msg void OnUpdateViewAlwaysontop(CCmdUI *pCmdUI);
	afx_msg void OnViewFullscreen();
	afx_msg void OnUpdateViewFullscreen(CCmdUI *pCmdUI);
	afx_msg void OnAddNetstream();
	afx_msg void OnRatecurrentN1();
	afx_msg void OnRatecurrentUn();
	afx_msg void OnRatecurrent1();
	afx_msg void OnRatecurrent2();
	afx_msg void OnRatecurrent3();
	afx_msg void OnRatecurrent4();
	afx_msg void OnRatecurrent5();
	afx_msg void OnFileOpenlibrary();
	afx_msg void OnFileExportlibrary();
	afx_msg void OnFileAttempttosanitisetags();
	afx_msg void OnPlaybackRandom();
	afx_msg void OnUpdatePlaybackRandom(CCmdUI *pCmdUI);
	afx_msg void OnTagsRemoveextrawhitespace();
	afx_msg void OnTagsAuto();
	afx_msg void OnFilePluginmanager();
	afx_msg void OnHelpOnlinehelp();
	afx_msg void OnSourcesShowdevices();
	afx_msg void OnUpdateSourcesShowdevices(CCmdUI *pCmdUI);
	afx_msg void OnSourcesShowplaylists();
	afx_msg void OnUpdateSourcesShowplaylists(CCmdUI *pCmdUI);
	afx_msg void OnSourcesShowdynamicplaylists();
	afx_msg void OnUpdateSourcesShowdynamicplaylists(CCmdUI *pCmdUI);
	afx_msg void OnSourcesShowplugins();
	afx_msg void OnUpdateSourcesShowplugins(CCmdUI *pCmdUI);
	afx_msg void OnFileThemeeditor();

	// tasks
	void KillTasks( bool updater = true, bool childtasks = true );

	// batch functors tasks will use
	CmusikCubeUpdater* m_Updater;

	// macros
	DECLARE_DYNAMIC(CmusikCubeFrame)
	DECLARE_MESSAGE_MAP()

	// tray icon
	NOTIFYICONDATA m_TrayIcon;
	bool m_TrayIconVisible;
	bool m_SelBoxesVisible;
    bool m_ReloadMainLib;
	void InitTrayIcon();
    void ShowBalloonTip( const CString& title, const CString& info );

	// full screen
	bool m_FullScreen;
	bool m_GoingFullScreen;
	void GoFullScreen();
	void RetFullScreen(); 
	void RestoreWindow();

	// various
	bool m_AutoStart;
	bool m_TransEnb;
	int m_Trans;
	void ImportTrans();
	CmusikDirSync* m_DirSyncDlg;
	
	// internal selection box query routine
	CmusikSelectionCtrl* UpdateSelHierarchical( int nSenderID );

    CView* SwitchMainView ( CView* pNewView );
    CView* m_MainView;
    CView* m_CDView;
	//CmusikPlayerView* m_PlayerView;

    void HideCDView();
public:
	afx_msg void OnViewSelectionboxes();
};

///////////////////////////////////////////////////

