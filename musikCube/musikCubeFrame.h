///////////////////////////////////////////////////
//
// License Agreement:
//
// The following are Copyright © 2002-2006, Casey Langen
//
// Sources and Binaries of: musikCore musikCube musikBox
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright 
//      notice, this list of conditions and the following disclaimer in the 
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the author nor the names of other contributors may 
//      be used to endorse or promote products derived from this software 
//      without specific prior written permission. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE. 
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

#define ID_EQUALIZER 1343
#define ID_NOWPLAYING 1344
#define ID_SOURCESBOX 1345
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
    friend void CmusikSourcesCtrl::DoDrag(CmusikPropTreeItem* pItem);
    friend class CmusikCubeUpdater;
    friend class GlobalHotkeys;         // hack from refactoring

public:

    // construct and destruct
    CmusikCubeFrame(bool autostart = false);
    virtual ~CmusikCubeFrame();

    // Main Frame Window
    // (overrides)
protected:  virtual BOOL            PreCreateWindow(CREATESTRUCT& cs);
protected:  virtual BOOL            OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
protected:  virtual BOOL            OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
protected:  virtual LRESULT         WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:  virtual BOOL            PreTranslateMessage(MSG* pMsg);
protected:  virtual BOOL            DestroyWindow();
    // (methods)
protected:  void                    SaveWindowState();
protected:  void                    RestoreDialogPosition();
protected:  void                    ResetDialogPosition();

// Debugging (required?)
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    // Task Tray
    //  (methods)
protected:  void                    SendToTray();
protected:  void                    RestoreFromTray();
protected:  void                    ShowTrayIcon();
protected:  void                    HideTrayIcon();
protected:  void                    InitTrayIcon();
    //  (variables)
protected:  NOTIFYICONDATA          m_TrayIcon;
protected:  bool                    m_TrayIconVisible;

    // Full Screen
    //  (methods)
protected:  void                    GoFullScreen();
protected:  void                    KillFullScreen();
    //  (variables)
protected:  bool                    m_FullScreen;
protected:  bool                    m_GoingFullScreen;

    // misc
public:     bool                    PlayTrackFromExplorer(const CString& fn);




    int m_Progress;
    bool m_IsMinimized;

    // CDockBar related
    BOOL VerifyBarState(LPCTSTR lpszProfileName);

    // unique drag identifiers
    UINT m_uSourcesDrop;
    UINT m_uPlaylistDrop_L;
    UINT m_uPlaylistDrop_R;
    UINT m_uSelectionDrop_L;
    UINT m_uSelectionDrop_R;

    // Transparency
    lpfnSetLayeredWindowAttributes m_pSetLayeredWindowAttributes;
    void SetTransparency(int trans);
    void InitTrans(bool set_trans = false);
    void DeinitTrans();
    bool IsTransparencyEnabled(){ return m_TransEnb; }
    bool m_TransEnb;
    int m_Trans;
    void ImportTrans();



    void UpdateTitle();






protected: 

    // init and clean musik core components
    void InitDragTypes();

    // resetting controls
    void LoadDlgSize();

    // icons
    HICON m_hIcon16;
    HICON m_hIcon32;



    void ShowSelectionBoxes(bool show);

    // utility functions
    void DockBarLeftOf(CSizingControlBar* Bar, CSizingControlBar* LeftOf);

    // custom message maps
    afx_msg LRESULT OnSongChange(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEqualizerChange(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSongStop(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSongPause(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSongResume(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSourcesLibrary(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSourcesNowPlaying(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSourcesSubLib(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSourcesDynPlaylist(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSourcesStdPlaylist(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSourcesQuickSearch(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSourcesQuickSearchStr(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDragStart(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDragEnd(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSelectionCtrlEditCommit(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnTaskProgress(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnTaskEnd(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnPlayerPlaySel(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnVerifyPlaylist(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSelBoxAddRemove(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSelBoxRequestUpdate(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnUpdateCurrPlaylist(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSelectionCtrlDeleteSelected(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSourcesItemClicked(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnInitTrans(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDeinitTrans(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnPlaySong(WPARAM wParam, LPARAM lParam = NULL);
    afx_msg LRESULT OnNowPlayingUpdated(WPARAM wParam = NULL, LPARAM lParam = NULL);
    afx_msg LRESULT OnPlayerUpdate(WPARAM wParam = NULL, LPARAM lParam = NULL);
    afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSourcesCDAudio(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSourcesQueryFreeDB(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSourcesRemovableDev(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSourcesRefresh(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSourcesReset(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSourcesNetRadio(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSelectionBoxReset(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSelectionBoxActivate(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSourcesUnsel(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnTaskbarCreated(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnShowCube(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnHideCube(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnUpdateVolume(WPARAM wParam, LPARAM lParam);

    // mfc message maps
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    afx_msg void OnSysColorChange();
    afx_msg void OnFilePreferences();
    afx_msg void OnDestroy();
    afx_msg void OnMenuAddFiles();
    afx_msg void OnMenuAddDirectory();
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
    afx_msg void OnUpdatePlaybackmodeRepeatsingle(CCmdUI *pCmdUI);
    afx_msg void OnPlaybackmodeRepeatsingle();
    afx_msg void OnUpdatePlaybackmodeRepeatplaylist(CCmdUI *pCmdUI);
    afx_msg void OnPlaybackmodeRepeatplaylist();
    afx_msg void OnUpdatePlaybackmodeIntro(CCmdUI *pCmdUI);
    afx_msg void OnPlaybackmodeIntro();
    afx_msg void OnMenuViewModifiedTags();
    afx_msg void OnUnsynchronizedtagsWritetofile();
    afx_msg void OnUnsynchronizedtagsFinalizefordatabaseonly();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnViewEqualizer();
    afx_msg void OnUpdateViewEqualizer(CCmdUI *pCmdUI);
    afx_msg void OnFileSynchronizeddirectories();
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
    afx_msg void OnMenuEraseLibrary();
    afx_msg void OnMenuSynchronize();
    afx_msg void OnPlaybackmodeShufflecurrentplaylist();
    afx_msg void OnUpdatePlaybackmodeShufflecurrentplaylist(CCmdUI *pCmdUI);
    afx_msg void OnViewVisualization();
    afx_msg void OnUpdateViewVisualization(CCmdUI *pCmdUI);
    afx_msg void OnViewAlwaysontop();
    afx_msg void OnUpdateViewAlwaysontop(CCmdUI *pCmdUI);
    afx_msg void OnViewFullscreen();
    afx_msg void OnUpdateViewFullscreen(CCmdUI *pCmdUI);
    afx_msg void OnMenuAddNetRadio();
    afx_msg void OnRatecurrentN1();
    afx_msg void OnRatecurrentUn();
    afx_msg void OnRatecurrent1();
    afx_msg void OnRatecurrent2();
    afx_msg void OnRatecurrent3();
    afx_msg void OnRatecurrent4();
    afx_msg void OnRatecurrent5();
    afx_msg void OnMenuExportLibrary();
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
    afx_msg void OnMenuChangeLibrary();
    afx_msg void OnHelpCheckForUpdates();

    // tasks
    void KillTasks(bool updater = true, bool childtasks = true);

    // batch functors tasks will use
    CmusikCubeUpdater* m_Updater;

    // macros
    DECLARE_DYNAMIC(CmusikCubeFrame)
    DECLARE_MESSAGE_MAP()

    // tray icon

    void ShowBalloonTip(const CString& title, const CString& info);


    // various
    bool m_AutoStart;


    CView* SwitchMainView (CView* pNewView);

    void HideCDView();
public:
    afx_msg void OnViewSelectionboxes();


////////////////////////////////////////////////////////

private:        // NEW!!!

    // ui updating
private:    enum UpdateAction
            {
                None = 0,
                Reset = 1,
                Query = 2,
                Requery = 3,
                Clear = 4
            };

    void                                UpdateFilters(enum UpdateAction selectionUpdateAction, enum UpdateAction playlistUpdateAction);


    // PlaylistView
    // (methods)
    void                                QueryPlaylist(musikCore::Playlist& playlist);
    musikCore::String                   GetPlaylistQuery();
    musikCore::String                   GetPlaylistSelectionFilter();
    void                                GivePlaylistToView(musikCore::Playlist* playlist, bool giveCopy = false);
    musikCore::String                   GetPlaylistInfoAsFilter();
    //
    void                                ResetPlaylistCtrl();
    void                                RequeryPlaylistCtrl();
    void                                QueryPlaylistCtrl();
    void                                ClearPlaylistCtrl();
    // (variables)
    musikCore::PlaylistInfo             m_PlaylistInfo;

    // SelectionViews
    // (events)
    afx_msg LRESULT                     OnSelectionCtrlChanged(WPARAM wParam, LPARAM lParam);
    // (methods)
    CmusikSelectionCtrl*                GetParentSelectionCtrl();
    musikCore::String                   GetSelectionCtrlFilter(CmusikSelectionCtrl* selectionCtrl);
    void                                UpdateSelectionCtrlOrder(CmusikSelectionCtrl* newSelection);
    int                                 GetSelectionCtrlOrder(CmusikSelectionCtrl* selectionControl);
    //
    void                                ResetSelectionCtrls();
    void                                QuerySelectionCtrls();
    void                                RequerySelectionCtrls();
    void                                ClearSelectionCtrls();
    // (variables)

    // the actual controls
    CView*                              m_MainView;
    //
    CmusikPlaylistView*                 m_PlaylistView;
    CmusikPlaylistView*                 m_CDView;
    //
    CmusikSourcesCtrl*                  m_SourcesView;
    CmusikNowPlayingBar*                m_NowPlayingView;
    CmusikEqualizerBar*                 m_EqualizerView;
    std::vector<CmusikSelectionBar*>    m_AllFilterViews;
    std::vector<CmusikSelectionCtrl*>   m_ActiveFilterViews;

    // misc
    //
    musikCore::String                   m_LastCaption;
};

///////////////////////////////////////////////////


