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

#include "stdafx.h"
#include <shlobj.h>   

#include "musikCube.h"
#include "musikCubeFrame.h"

#include "musikFileDialog.h"
#include "musikThemeEditorDlg.h"
#include "musikPrefsDlg.h"
#include "musikPropertyPage.h"
#include "musikTunage.h"
#include "musikAddNetStreamDlg.h"
#include "musikCubeMessages.h"
#include "GlobalHotkeys.h"
#include "memdc.h"

#include <io.h>
#include <Direct.h>
#include <Windows.h>

#include "3rdparty/CShellFileOp.h"
#include ".\musikcubeframe.h"

///////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikCubeFrame, CFrameWnd)

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikCubeFrame, CFrameWnd)

    // mfc message maps
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_SYSCOLORCHANGE()
    ON_WM_SYSCOMMAND()
    ON_MESSAGE(WM_HOTKEY,OnHotKey)
    ON_WM_NCCALCSIZE()

    // menu 
    ON_COMMAND(ID_FILE_PREFERENCES, OnFilePreferences)
    ON_COMMAND(ID_ADD_FILES, OnMenuAddFiles)
    ON_COMMAND(ID_ADD_DIRECTORY, OnMenuAddDirectory)
    ON_COMMAND(ID_VIEW_PLAYLISTINFORMATION, OnViewPlaylistinformation)
    ON_COMMAND(ID_SOURCES_VISIBLE, OnViewSources)
    ON_COMMAND(ID_VIEW_NOWPLAYING, OnViewNowplaying)
    ON_COMMAND(ID_AUDIO_EQUALIZER_ENABLED, OnAudioEqualizerEnabled)
    ON_COMMAND(ID_AUDIO_CROSSFADER_ENABLED, OnAudioCrossfaderEnabled)
    ON_COMMAND(ID_PLAYBACKMODE_REPEATSINGLE, OnPlaybackmodeRepeatsingle)
    ON_COMMAND(ID_PLAYBACKMODE_REPEATPLAYLIST, OnPlaybackmodeRepeatplaylist)
    ON_COMMAND(ID_PLAYBACKMODE_INTRO, OnPlaybackmodeIntro)
    ON_COMMAND(ID_VIEWMODIFIEDTAGS_VIEW, OnMenuViewModifiedTags)
    ON_COMMAND(ID_UNSYNCHRONIZEDTAGS_WRITETOFILE, OnUnsynchronizedtagsWritetofile)
    ON_COMMAND(ID_UNSYNCHRONIZEDTAGS_FINALIZEFORDATABASEONLY, OnUnsynchronizedtagsFinalizefordatabaseonly)
    ON_COMMAND(ID_VIEW_EQUALIZER, OnViewEqualizer)
    ON_COMMAND(ID_FILE_SYNCHRONIZEDDIRECTORIES, OnFileSynchronizeddirectories)
    ON_COMMAND(ID_VIEW_RESETINTERFACETODEFAULT, OnViewResetinterfacetodefault)
    ON_COMMAND(ID_NOTIFICATIONTRAY_EXIT, OnNotificationtrayExit)
    ON_COMMAND(ID_NOTIFICATIONTRAY_RESTORE, OnNotificationtrayRestore)
    ON_COMMAND(ID_NOTIFICATIONTRAY_PLAY, OnNotificationtrayPlay)
    ON_COMMAND(ID_NOTIFICATIONTRAY_STOP, OnNotificationtrayStop)
    ON_COMMAND(ID_NOTIFICATIONTRAY_PAUSE, OnNotificationtrayPause)
    ON_COMMAND(ID_NOTIFICATIONTRAY_NEXT, OnNotificationtrayNext)
    ON_COMMAND(ID_NOTIFICATIONTRAY_PREV, OnNotificationtrayPrev)
    ON_COMMAND(ID_FILE_CLEARLIBRARY, OnMenuEraseLibrary)
    ON_COMMAND(ID_LIBRARY_SYNCHRONIZEDIRECTORIESNOW, OnMenuSynchronize)
    ON_COMMAND(ID_PLAYBACKMODE_SHUFFLECURRENTPLAYLIST, OnPlaybackmodeShufflecurrentplaylist)
    ON_COMMAND(ID_VIEW_VISUALIZATION, OnViewVisualization)
    ON_COMMAND(ID_VIEW_ALWAYSONTOP, OnViewAlwaysontop)
    ON_COMMAND(ID_VIEW_FULLSCREEN, OnViewFullscreen)
    ON_COMMAND(ID_ADD_NETSTREAM, OnMenuAddNetRadio)
    ON_COMMAND(ID_RATECURRENT_N1, OnRatecurrentN1)
    ON_COMMAND(ID_RATECURRENT_UNRATED, OnRatecurrentUn)
    ON_COMMAND(ID_RATECURRENT_1, OnRatecurrent1)
    ON_COMMAND(ID_RATECURRENT_2, OnRatecurrent2)
    ON_COMMAND(ID_RATECURRENT_3, OnRatecurrent3)
    ON_COMMAND(ID_RATECURRENT_4, OnRatecurrent4)
    ON_COMMAND(ID_RATECURRENT_5, OnRatecurrent5)
    ON_COMMAND(ID_FILE_EXPORTLIBRARY, OnMenuExportLibrary)
    ON_COMMAND(ID_FILE_ATTEMPTTOSANITISETAGS, OnFileAttempttosanitisetags)
    ON_COMMAND(ID_PLAYBACK_RANDOM, OnPlaybackRandom)
    ON_COMMAND(ID_TAGS_REMOVEEXTRAWHITESPACE, OnTagsRemoveextrawhitespace)
    ON_COMMAND(ID_TAGS_AUTO, OnTagsAuto)
    ON_COMMAND(ID_FILE_PLUGINMANAGER, OnFilePluginmanager)
    ON_COMMAND(ID_HELP_ONLINEHELP, OnHelpOnlinehelp)
    ON_COMMAND(ID_SOURCES_SHOWDEVICES, OnSourcesShowdevices)
    ON_COMMAND(ID_SOURCES_SHOWPLAYLISTS, OnSourcesShowplaylists)
    ON_COMMAND(ID_SOURCES_SHOWDYNAMICPLAYLISTS, OnSourcesShowdynamicplaylists)
    ON_COMMAND(ID_SOURCES_SHOWPLUGINS, OnSourcesShowplugins)
    ON_COMMAND(ID_FILE_THEMEEDITOR, OnFileThemeeditor)

    // update ui
    ON_UPDATE_COMMAND_UI(ID_SOURCES_VISIBLE, OnUpdateViewSources)
    ON_UPDATE_COMMAND_UI(ID_VIEW_SELECTIONBOXES, OnUpdateViewSelectionboxes)
    ON_UPDATE_COMMAND_UI(ID_VIEW_NOWPLAYING, OnUpdateViewNowplaying)
    ON_UPDATE_COMMAND_UI(ID_AUDIO_EQUALIZER_ENABLED, OnUpdateAudioEqualizerEnabled)
    ON_UPDATE_COMMAND_UI(ID_AUDIO_CROSSFADER_ENABLED, OnUpdateAudioCrossfaderEnabled)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PLAYLISTINFORMATION, OnUpdateViewPlaylistinformation)
    ON_UPDATE_COMMAND_UI(ID_PLAYBACKMODE_REPEATSINGLE, OnUpdatePlaybackmodeRepeatsingle)
    ON_UPDATE_COMMAND_UI(ID_PLAYBACKMODE_REPEATPLAYLIST, OnUpdatePlaybackmodeRepeatplaylist)
    ON_UPDATE_COMMAND_UI(ID_PLAYBACKMODE_INTRO, OnUpdatePlaybackmodeIntro)
    ON_UPDATE_COMMAND_UI(ID_VIEW_EQUALIZER, OnUpdateViewEqualizer)
    ON_UPDATE_COMMAND_UI(ID_NOTIFICATIONTRAY_PLAY, OnUpdateNotificationtrayPlay)
    ON_UPDATE_COMMAND_UI(ID_NOTIFICATIONTRAY_PAUSE, OnUpdateNotificationtrayPause)
    ON_UPDATE_COMMAND_UI(ID_NOTIFICATIONTRAY_NEXT, OnUpdateNotificationtrayNext)
    ON_UPDATE_COMMAND_UI(ID_NOTIFICATIONTRAY_PREV, OnUpdateNotificationtrayPrev)
    ON_UPDATE_COMMAND_UI(ID_NOTIFICATIONTRAY_STOP, OnUpdateNotificationtrayStop)
    ON_UPDATE_COMMAND_UI(ID_PLAYBACKMODE_SHUFFLECURRENTPLAYLIST, OnUpdatePlaybackmodeShufflecurrentplaylist)
    ON_UPDATE_COMMAND_UI(ID_VIEW_ALWAYSONTOP, OnUpdateViewAlwaysontop)
    ON_UPDATE_COMMAND_UI(ID_VIEW_VISUALIZATION, OnUpdateViewVisualization)
    ON_UPDATE_COMMAND_UI(ID_VIEW_FULLSCREEN, OnUpdateViewFullscreen)
    ON_UPDATE_COMMAND_UI(ID_PLAYBACK_RANDOM, OnUpdatePlaybackRandom)
    ON_UPDATE_COMMAND_UI(ID_SOURCES_SHOWDEVICES, OnUpdateSourcesShowdevices)
    ON_UPDATE_COMMAND_UI(ID_SOURCES_SHOWPLAYLISTS, OnUpdateSourcesShowplaylists)
    ON_UPDATE_COMMAND_UI(ID_SOURCES_SHOWDYNAMICPLAYLISTS, OnUpdateSourcesShowdynamicplaylists)
    ON_UPDATE_COMMAND_UI(ID_SOURCES_SHOWPLUGINS, OnUpdateSourcesShowplugins)
    ON_COMMAND(ID_VIEW_SELECTIONBOXES, OnViewSelectionboxes)
    ON_COMMAND(ID_MUSICLIBRARY_CHANGE, OnMenuChangeLibrary)

    // custom message maps
    ON_REGISTERED_MESSAGE(WM_SOURCESUNSEL, OnSourcesUnsel)
    ON_REGISTERED_MESSAGE(WM_SELBOXUPDATE, OnSelectionCtrlChanged)
    ON_REGISTERED_MESSAGE(WM_SONGCHANGE, OnSongChange)
    ON_REGISTERED_MESSAGE(WM_SONGSTOP, OnSongStop)
    ON_REGISTERED_MESSAGE(WM_SONGPAUSE, OnSongPause)
    ON_REGISTERED_MESSAGE(WM_SONGRESUME, OnSongResume)
    ON_REGISTERED_MESSAGE(WM_SOURCESLIBRARY, OnSourcesLibrary)
    ON_REGISTERED_MESSAGE(WM_SOURCESNOWPLAYING, OnSourcesNowPlaying)
    ON_REGISTERED_MESSAGE(WM_SOURCESDYNPLAYLIST, OnSourcesDynPlaylist)
    ON_REGISTERED_MESSAGE(WM_SOURCESSTDPLAYLIST, OnSourcesStdPlaylist)
    ON_REGISTERED_MESSAGE(WM_SOURCESQUICKSEARCH, OnSourcesQuickSearch)
    ON_REGISTERED_MESSAGE(WM_SOURCESQUICKSEARCHSTR, OnSourcesQuickSearchStr)
    ON_REGISTERED_MESSAGE(WM_DRAGSTART, OnDragStart)
    ON_REGISTERED_MESSAGE(WM_DRAGEND, OnDragEnd)
    ON_REGISTERED_MESSAGE(WM_PLAYSONG, OnPlaySong)
    ON_REGISTERED_MESSAGE(WM_SELBOXEDITCOMMIT, OnSelectionCtrlEditCommit)
    ON_REGISTERED_MESSAGE(WM_SELECTIONCTRLDELETESELECTED, OnSelectionCtrlDeleteSelected)
    ON_REGISTERED_MESSAGE(WM_TASK_PROGRESS, OnTaskProgress)
    ON_REGISTERED_MESSAGE(WM_TASK_END, OnTaskEnd)
    ON_REGISTERED_MESSAGE(WM_PLAYER_PLAYSEL, OnPlayerPlaySel)
    ON_REGISTERED_MESSAGE(WM_BATCHADD_VERIFY_PLAYLIST, OnVerifyPlaylist)
    ON_REGISTERED_MESSAGE(WM_SELBOXADDREMOVE, OnSelBoxAddRemove)
    ON_REGISTERED_MESSAGE(WM_SELBOXREQUESTUPDATE, OnSelBoxRequestUpdate)
    ON_REGISTERED_MESSAGE(WM_UPDATEPLAYLIST, OnUpdateCurrPlaylist)
    ON_REGISTERED_MESSAGE(WM_EQUALIZERCHANGE, OnEqualizerChange)
    ON_REGISTERED_MESSAGE(WM_SOURCESITEMCLICKED, OnSourcesItemClicked)
    ON_REGISTERED_MESSAGE(WM_INITTRANS, OnInitTrans)
    ON_REGISTERED_MESSAGE(WM_DEINITTRANS, OnDeinitTrans)
    ON_REGISTERED_MESSAGE(WM_NOWPLAYINGUPDATED, OnNowPlayingUpdated)
    ON_REGISTERED_MESSAGE(WM_SOURCESCDAUDIO, OnSourcesCDAudio)
    ON_REGISTERED_MESSAGE(WM_SOURCESQUERYFREEDB, OnSourcesQueryFreeDB)
    ON_REGISTERED_MESSAGE(WM_SOURCESREMOVABLEDEV, OnSourcesRemovableDev)
    ON_REGISTERED_MESSAGE(WM_SOURCESREFRESH, OnSourcesRefresh)
    ON_REGISTERED_MESSAGE(WM_SOURCESRESET, OnSourcesReset)
    ON_REGISTERED_MESSAGE(WM_SOURCESNETSTREAMS, OnSourcesNetRadio)
    ON_REGISTERED_MESSAGE(WM_SELBOXRESET, OnSelectionBoxReset)
    ON_REGISTERED_MESSAGE(WM_SELBOXACTIVATE, OnSelectionBoxActivate)
    ON_REGISTERED_MESSAGE(WM_TASKBARCREATED, OnTaskbarCreated)
    ON_REGISTERED_MESSAGE(WM_SHOWMUSIKCUBE, OnShowCube)
    ON_REGISTERED_MESSAGE(WM_HIDEMUSIKCUBE, OnHideCube)
    ON_REGISTERED_MESSAGE(WM_UPDATEVOLUME, OnUpdateVolume)
    ON_COMMAND(ID_HELP_CHECKFORUPDATES, OnHelpCheckForUpdates)
    END_MESSAGE_MAP()

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnHotKey(WPARAM wParam, LPARAM lParam)
{
    std::wstring statusString;

    if (GlobalHotkeys::ProcessHotkey((GlobalHotkeys::Hotkey) wParam, statusString))
    {
        if (musikCube::g_Prefs->GetShowBalloonTips())
        {
            ShowBalloonTip(L"musikCube", statusString.c_str());
        }
    }

    return 0L;
}

///////////////////////////////////////////////////

// TODO: MOVE INTO TIMER!

void CmusikCubeUpdater::run()
{
    m_Stop = false;
    m_Active = true;
    m_Finished = false;
    bool is_frame_focused = false;
        
    size_t pos = 0;

    int task_count = 0, update_count = 0;

    CString sCaption;
    while (!m_Stop)
    {
        // attempt refresh the title every second. this is ugly, fix it.
        m_Parent->UpdateTitle();

        // check transparency every 1 seconds. ugh, move this to SetFocus(), KillFocus()
        if (musikCube::g_Prefs->IsTransEnabled() && m_Parent->IsTransparencyEnabled())
        {
            if (!is_frame_focused && GetForegroundWindow() == m_Parent->GetSafeHwnd())
            {
                m_Parent->SetTransparency(musikCube::g_Prefs->GetTransFocus());
                is_frame_focused = true;
            }
            else if (is_frame_focused && GetForegroundWindow() != m_Parent->GetSafeHwnd())
            {
                m_Parent->SetTransparency(musikCube::g_Prefs->GetTransUnFocus());
                is_frame_focused = false;
            }
        }

        Sleep(1000);
    }

    m_Finished = true;
}

///////////////////////////////////////////////////

CmusikCubeFrame::CmusikCubeFrame(bool autostart)
{
    m_AutoStart         = autostart;
    m_TransEnb          = false;
    m_FullScreen        = false;
    m_IsMinimized       = false;
    m_GoingFullScreen   = false;
    m_Progress          = 0;
    m_CDView            = NULL;
    m_PlaylistView      = NULL;
    m_MainView          = NULL;
    m_SourcesView       = NULL;

    m_hIcon16 = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_MAINFRAME), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
    m_hIcon32 = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_MAINFRAME), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);

    InitDragTypes();
}

///////////////////////////////////////////////////

CmusikCubeFrame::~CmusikCubeFrame()
{
    // Save selection control floating window states
    //
    musikCore::IntArray sel_modes;
    //
    for (size_t i = 0; i < m_AllFilterViews.size(); i++)
    {
        sel_modes.push_back(m_AllFilterViews.at(i)->GetCtrl()->GetType());
        delete m_AllFilterViews.at(i);
    }
    //
    musikCube::g_Prefs->SetSelBoxTypes(sel_modes);

    delete m_PlaylistView;
    delete m_SourcesView;
    delete m_NowPlayingView;
    delete m_CDView;
    delete m_EqualizerView;

    ::UnSetMMShellHook(GetSafeHwnd());
}

///////////////////////////////////////////////////

void CmusikCubeFrame::InitDragTypes()
{
    m_uPlaylistDrop_L   = RegisterClipboardFormat (_T("musikPlaylist_3BCFE9D1_6D61_4cb6_9D0B_3BB3F643CA82"));
    m_uPlaylistDrop_R   = RegisterClipboardFormat (_T("musikPlaylist_9543E137-7131-49fd-ADB5-208F0EF7A05C"));
    m_uSourcesDrop      = RegisterClipboardFormat (_T("musikSources_3BCFE9D1_6D61_4cb6_9D0B_3BB3F643CA82"));
    m_uSelectionDrop_R  = RegisterClipboardFormat (_T("musikSelection_070D6C9E-0E34-4b56-990D-5E2A2FA719A0"));
    m_uSelectionDrop_L  = RegisterClipboardFormat (_T("musikSelection_374DA040-66D6-4b2d-9E8C-592421FF499D"));
}

///////////////////////////////////////////////////

void CmusikCubeFrame::LoadDlgSize()
{
    CRect rcNormal = CRect(musikCube::g_Prefs->GetDlgPos(), musikCube::g_Prefs->GetDlgSize());
    MoveWindow(rcNormal);
    
    if (musikCube::g_Prefs->IsMaximized())
    {
        WINDOWPLACEMENT max;
        max.showCmd = SW_MAXIMIZE;
        max.rcNormalPosition = rcNormal;
        SetWindowPlacement(&max);
        return;
    }
    else
        MoveWindow(rcNormal);

}

///////////////////////////////////////////////////

void CmusikCubeFrame::ResetDialogPosition()
{
    MoveWindow(0, 0, 800, 600);

    CSize size;
    size.cx = 800;
    size.cy = 48;

    // equalizer
    size.cx = 800;
    size.cy = 84;
    m_EqualizerView->ForceDockedSize(size, LM_HORZDOCK, true);

    size.cx = 520;
    size.cy = 130;
    m_EqualizerView->ForceFloatSize(size);

    // now playing
    size.cx = 800;
    size.cy = 48;
    m_NowPlayingView->ForceDockedSize(size, LM_HORZDOCK, true);
    m_NowPlayingView->ForceFloatSize(size);

    // sources
    size.cx = 140;
    size.cy = 600;
    m_SourcesView->ForceDockedSize(size, LM_VERTDOCK, true);
    m_SourcesView->ForceFloatSize(size);

    // selection box
    size.cx = NULL;
    size.cy = 120;
    for (size_t i = 0; i < m_AllFilterViews.size(); i++)
    {
        m_AllFilterViews[i]->ForceDockedSize(size, LM_HORZDOCK, true);
    }

    // hide equalizer
    ShowControlBar(m_EqualizerView, FALSE, TRUE);
}

///////////////////////////////////////////////////

void CmusikCubeFrame::ResetSelectionCtrls()
{
    for (size_t i = 0; i < this->m_AllFilterViews.size(); i++)
    {
        m_AllFilterViews[i]->GetCtrl()->Reset(this->GetPlaylistInfoAsFilter());
    }
    //
    this->m_ActiveFilterViews.clear();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::DockBarLeftOf(CSizingControlBar* pBar, CSizingControlBar* pTargetBar)
{
    ASSERT(pBar != NULL);
    ASSERT(pTargetBar != NULL);
    ASSERT(pBar != pTargetBar);

    // the neighbor must be already docked
    CDockBar* pDockBar = pTargetBar->m_pDockBar;
    ASSERT(pDockBar != NULL);
    UINT nDockBarID = pTargetBar->m_pDockBar->GetDlgCtrlID();
    ASSERT(nDockBarID != AFX_IDW_DOCKBAR_FLOAT);

    bool bHorz = (nDockBarID == AFX_IDW_DOCKBAR_TOP ||
        nDockBarID == AFX_IDW_DOCKBAR_BOTTOM);

    // dock normally (inserts a new row)
    DockControlBar(pBar, nDockBarID);

    // delete the new row (the bar pointer and the row end mark)
    pDockBar->m_arrBars.RemoveAt(pDockBar->m_arrBars.GetSize() - 1);
    pDockBar->m_arrBars.RemoveAt(pDockBar->m_arrBars.GetSize() - 1);

    // find the target bar
    for (int i = 0; i < pDockBar->m_arrBars.GetSize(); i++)
    {
        void* p = pDockBar->m_arrBars[i];
        if (p == pTargetBar) // and insert the new bar after it
            pDockBar->m_arrBars.InsertAt(i + 1, pBar);
    }

    // move the new bar into position
    CRect rBar;
    pTargetBar->GetWindowRect(rBar);
    rBar.OffsetRect(bHorz ? 1 : 0, bHorz ? 0 : 1);
    pBar->MoveWindow(rBar);
}

///////////////////////////////////////////////////

int CmusikCubeFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    // setup hotkeys
    if(musikCube::g_Prefs->GetUseGlobalHotkeys())
    {
        GlobalHotkeys::ReloadHotkeys();
    }

    // tray icon
    
    InitTrayIcon();
    if (musikCube::g_Prefs->MinimizeToTray() || musikCube::g_Prefs->GetCloseToSystemTray())
    {
        ShowTrayIcon();
    }

    // initialize plugins
    musikCube::LoadPlugins();

    SetIcon(m_hIcon32, true);
    SetIcon(m_hIcon16, false);

    // set default dock order
    EnableDocking(CBRS_ALIGN_BOTTOM);
    EnableDocking(CBRS_ALIGN_LEFT);
    EnableDocking(CBRS_ALIGN_RIGHT);
    EnableDocking(CBRS_ALIGN_TOP);

    #ifdef _SCB_REPLACE_MINIFRAME
        m_pFloatingFrameClass = RUNTIME_CLASS(CSCBMiniDockFrameWnd);
    #endif

    // create the view
    m_PlaylistView = new CmusikPlaylistView(this, m_uPlaylistDrop_L, m_uPlaylistDrop_R, m_uSourcesDrop, m_uSelectionDrop_L, m_uSelectionDrop_R, musikCube::g_Prefs->PlaylistInfoVisible());
    m_PlaylistView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL);
    m_MainView = (CView*)m_PlaylistView;

    // sources control
    m_SourcesView = new CmusikSourcesCtrl(this, m_uSourcesDrop, m_uPlaylistDrop_R, m_uSelectionDrop_R);
    m_SourcesView->Create(_T("Sources"), this, ID_SOURCESBOX);
    DockControlBar(m_SourcesView, AFX_IDW_DOCKBAR_LEFT);

    // now playing control
    m_NowPlayingView = new CmusikNowPlayingBar(this);
    m_NowPlayingView->Create(_T("musik Now Playing"), this, ID_NOWPLAYING);
    DockControlBar(m_NowPlayingView, AFX_IDW_DOCKBAR_BOTTOM);
    
    // equalizer control
    m_EqualizerView = new CmusikEqualizerBar();
    m_EqualizerView->Create(_T("Equalizer"), this, ID_EQUALIZER);
    m_EqualizerView->GetCtrl()->SetBandState(musikCube::g_Prefs->GetEqualizerBandState());
    FloatControlBar(m_EqualizerView, CPoint(28, 28));
    ShowControlBar(m_EqualizerView, FALSE, FALSE);

    // selection controls
    CmusikSelectionBar* pBar;
    for (size_t i = 0; i < musikCube::g_Prefs->GetSelBoxCount(); i++)
    {
        pBar = new CmusikSelectionBar(this, musikCube::g_Prefs->GetSelBoxType(i), i, m_uSelectionDrop_L, m_uSelectionDrop_R);
        pBar->Create(_T("musik Selection Box"), this, ID_SELECTIONBOX_START + i);
        if (i == 0)
            DockControlBar(pBar);
        else
            DockBarLeftOf(pBar, m_AllFilterViews.at(i - 1));

        m_AllFilterViews.push_back(pBar);
    }

    // append the system menu
    CMenu* pMenu = GetSystemMenu(false);
    if (pMenu)
        pMenu->InsertMenu(0, MF_STRING, SYSMENU_RESETDIALOG, _T("Reset Window"));

    // load dockbar sizes and positions
    
    ResetDialogPosition();
    LoadDlgSize();

    if (VerifyBarState(_T("musikProfile0.90")))
    {
        CSizingControlBar::GlobalLoadState(this, _T("musikProfile0.90"));
        LoadBarState(_T("musikProfile0.90"));
    }

    // fire the updater task off
    m_Updater = new CmusikCubeUpdater;
    m_Updater->m_Parent = this;
    m_Updater->start();
    
    ImportTrans();
    if (musikCube::g_Prefs->IsTransEnabled())
        InitTrans();

    // always on top?
    if (musikCube::g_Prefs->IsAlwaysOnTop())
    {
        CRect rcWnd;
        GetWindowRect(rcWnd);

        ::SetWindowPos(m_hWnd, 
            HWND_TOPMOST, 
            rcWnd.TopLeft().x, 
            rcWnd.TopLeft().y, 
            rcWnd.Width(), 
            rcWnd.Height(), 
            NULL);
    }

    // restore last known playlist
    musikCube::g_Library->GetAllTempSongs(*musikCube::g_Player->GetPlaylist());
    musikCube::g_Library->ClearTempSongTable();

    // focus now playing
    if(musikCube::g_Player->GetPlaylist()->GetCount())
    {
        m_SourcesView->FocusNowPlaying();

        int last = musikCube::g_Prefs->GetDlgLastPlayed();
        //
        if (last >= 0)
        {
            musikCube::g_Player->FindNewIndex(last);
            last = musikCube::g_Player->GetIndex();
            //
            if (last >= 0)
            {
                m_PlaylistView->GetCtrl()->EnsureVisible(last, FALSE);
                m_PlaylistView->GetCtrl()->SetSelectionMark(last);
                m_PlaylistView->GetCtrl()->SetItemState(last, LVIS_SELECTED, LVIS_SELECTED);
            }
        }
    }
    else
    {
        this->m_SourcesView->FocusLibrary();
    }

    ::SetMMShellHook(GetSafeHwnd());

    return 0;
}

///////////////////////////////////////////////////

void CmusikCubeFrame::GivePlaylistToView(musikCore::Playlist* playlist, bool giveCopy)
{
    m_PlaylistView->GetCtrl()->SavePlaylist();

    musikCore::Playlist *playlistToGive = playlist;
    //
    if (giveCopy)
    {
        playlistToGive = new musikCore::Playlist(*playlist);
    }

    m_PlaylistView->GetCtrl()->SetPlaylist(playlistToGive);
}

///////////////////////////////////////////////////

bool CmusikCubeFrame::PlayTrackFromExplorer(const CString& fn)
{
    if (fn.IsEmpty())
        return false;

    if (!m_SourcesView)
        return false;

    if ( ! musikCube::g_Collected)
    {
        m_SourcesView->LoadCollectedLibrary();
    }

    // add song to library, if necessary
    if (!musikCube::g_Collected->AddSong((musikCore::String)fn))
        return false;

    // get player's playlist
    musikCore::Playlist* pPlaylist = musikCube::g_Player->GetPlaylist();
    if (pPlaylist)
    {
        // get the song we just added...
        musikCore::Song song;
        musikCube::g_Collected->GetSongAttrFromFilename((musikCore::String)fn, song);

        // add to the control's playlist
        if (song.GetID() >= 0)
        {
            pPlaylist->Add(song);

            // set the player's playlist, and play
            if (musikCube::g_Player)
            {
                musikCube::g_Player->Play(pPlaylist->GetCount() - 1);
                UpdateFilters(UpdateAction::None, UpdateAction::Requery);
                m_SourcesView->FocusNowPlaying();
            }

            UpdateFilters(UpdateAction::Query, UpdateAction::Requery);

            return true;
        }
    }

    return false;
}

///////////////////////////////////////////////////

#define MUSIKWINDOWCLASSNAME _T("musikCubeWindow1.0")

BOOL CmusikCubeFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if(!CFrameWnd::PreCreateWindow(cs))
        return FALSE;

    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();

    // assure class not already registered
    if(!(::GetClassInfo(hInst, MUSIKWINDOWCLASSNAME, &wndcls)))   
    {
        // get default MFC class settings
        if(::GetClassInfo(hInst, cs.lpszClass, &wndcls))        
        {
            wndcls.lpszClassName = MUSIKWINDOWCLASSNAME;
            wndcls.style |= CS_OWNDC;                               
            wndcls.hbrBackground = NULL;

            if (!AfxRegisterClass(&wndcls)) 
            {
                AfxThrowResourceException();
            }
        }
        else
        {
            AfxThrowResourceException();
        }
    }

    cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
    cs.lpszClass = MUSIKWINDOWCLASSNAME;

    return TRUE;
}

///////////////////////////////////////////////////

#ifdef _DEBUG
void CmusikCubeFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::Dump(CDumpContext& dc) const
{
    CFrameWnd::Dump(dc);
}

#endif //_DEBUG

///////////////////////////////////////////////////

BOOL CmusikCubeFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    // let the view have first crack at the command

    // otherwise, do default handling
    return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

///////////////////////////////////////////////////

BOOL CmusikCubeFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
    return TRUE;
}

///////////////////////////////////////////////////

BOOL CmusikCubeFrame::DestroyWindow()
{
    // make sure all the tasks are finished
    // before destroying the window...
    KillTasks(true, true);

    // stop all plugins
    musikCube::UnloadPlugins();

    CSizingControlBar::GlobalSaveState(this, _T("musikProfile0.90"));
    SaveBarState(_T("musikProfile0.90"));

    HideTrayIcon();

    return CFrameWnd::DestroyWindow();
}

///////////////////////////////////////////////////

// This function is Copyright (c) 2000, Cristi Posea.
// See www.datamekanix.com for more control bars tips&tricks.

BOOL CmusikCubeFrame::VerifyBarState(LPCTSTR lpszProfileName)
{
    CDockState state;
    state.LoadState(lpszProfileName);

    for (int i = 0; i < state.m_arrBarInfo.GetSize(); i++)
    {
        CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];
        ASSERT(pInfo != NULL);
        int nDockedCount = (int)pInfo->m_arrBarID.GetSize();
        if (nDockedCount > 0)
        {
            // dockbar
            for (int j = 0; j < nDockedCount; j++)
            {
                UINT nID = (UINT) pInfo->m_arrBarID[j];
                if (nID == 0) continue; // row separator
                if (nID > 0xFFFF)
                    nID &= 0xFFFF; // placeholder - get the ID
                if (GetControlBar(nID) == NULL)
                    return FALSE;
            }
        }
        
        if (!pInfo->m_bFloating) // floating dockbars can be created later
            if (GetControlBar(pInfo->m_nBarID) == NULL)
                return FALSE; // invalid bar ID
    }

    return TRUE;
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnDestroy()
{
    CSizingControlBar::GlobalSaveState(this, _T("musikDockBars0.90"));
    
    SaveWindowState();

    CFrameWnd::OnDestroy();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::SaveWindowState()
{
    WINDOWPLACEMENT max;
    GetWindowPlacement(&max);

    // let full screen do its own thing, other wise set
    // the maximize flag.
    if (!m_GoingFullScreen && !m_FullScreen)
    {
        if (max.showCmd == SW_SHOWMAXIMIZED && !m_IsMinimized)
            musikCube::g_Prefs->SetMaximized(true);
        else if (!m_IsMinimized)
        {
            CRect rc_dlg;
            GetWindowRect(&rc_dlg);

            musikCube::g_Prefs->SetDlgSize(CSize(rc_dlg.right - rc_dlg.left, rc_dlg.bottom - rc_dlg.top));
            musikCube::g_Prefs->SetDlgPos(CPoint(rc_dlg.left, rc_dlg.top));
            musikCube::g_Prefs->SetMaximized(false);
        }
            
    }
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnSelectionCtrlEditCommit(WPARAM wParam, LPARAM lParam)
{
    // setup vars
    CmusikSelectionCtrl* pSel = (CmusikSelectionCtrl*)wParam;
    int nLibType = (int)lParam;
    musikCore::String sNew = pSel->GetEditCommitStr();

    // the selected items are whats currently visible in the playlist
    musikCore::Playlist selectedSongs;
    this->QueryPlaylist(selectedSongs);

    // create a new musikCore::SongInfoArray and update all the respective values...
    if (selectedSongs.GetCount())
    {
        // get songs    
        musikCore::SongInfoArray* pSongInfoArray = new musikCore::SongInfoArray();
        musikCube::g_Library->GetInfoArrayFromPlaylist(selectedSongs, *pSongInfoArray, nLibType, sNew, true);

        // setup params
        musikCore::BatchRetag* params = new musikCore::BatchRetag(
            musikCube::g_Library, 
            musikCube::g_Functor, 
            pSongInfoArray);

        params->m_WriteToFile = false;
        params->m_DeleteUpdatedTags = true;

        // spawn the task
        musikCore::BatchRetagTask* task = new musikCore::BatchRetagTask;
        task->m_Params = params;
        musikCube::g_Library->QueueTask(task);

        return 1L;
    }

    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnSelectionCtrlChanged(WPARAM wParam, LPARAM lParam)
{
    CmusikSelectionCtrl* senderCtrl = (CmusikSelectionCtrl*) wParam;
    CmusikSelectionCtrl* parentCtrl = this->GetParentSelectionCtrl();

    // Show All... was selected
    //
    if (senderCtrl->IsItemSelected(0))
    {
        UpdateFilters(UpdateAction::Reset, UpdateAction::Reset);
        return 0L;
    }

    // If there is no parent control, let the sender be the parent
    //
    if (parentCtrl == NULL)
    {
        senderCtrl->SetParent(true);
        senderCtrl->RedrawWindow();
    }

    // We are now part of the filter! if we are not part of the filter
    // "senderCtrl" will be added to. if we are already a part of the
    // filter, child filters may be pruned.
    //
    this->UpdateSelectionCtrlOrder(senderCtrl);

    UpdateFilters(UpdateAction::Query, UpdateAction::Query);

    return 0L;
}

///////////////////////////////////////////////////

// author: Per-Erik Nordlund 
// from: http://www.codeproject.com/w2k/win2k_transparent.asp
// modified by Casey Langen

void CmusikCubeFrame::SetTransparency(int trans)
{
    if (trans < 20)
    {
        trans = 20;
    }

    m_pSetLayeredWindowAttributes(GetSafeHwnd(), 0, trans, LWA_ALPHA);
    m_Trans = trans;
}

///////////////////////////////////////////////////

void CmusikCubeFrame::ImportTrans()
{
    HMODULE hUser32 = GetModuleHandle(_T("USER32.DLL"));
    m_pSetLayeredWindowAttributes = 
                        (lpfnSetLayeredWindowAttributes)GetProcAddress(hUser32, 
                        "SetLayeredWindowAttributes");
}


///////////////////////////////////////////////////

void CmusikCubeFrame::DeinitTrans()
{
    if (m_TransEnb)
    {
        SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(GetSafeHwnd() , GWL_EXSTYLE) & ~WS_EX_LAYERED);
        RedrawWindow();

        m_TransEnb = false;
    }
}

///////////////////////////////////////////////////

void CmusikCubeFrame::InitTrans(bool set_trans)
{
    if (!m_TransEnb)
    {
        SetWindowLong(
            GetSafeHwnd(),
            GWL_EXSTYLE, 
            GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_LAYERED 
       );

        if (set_trans)
        {
            SetTransparency(m_Trans);
        }
        else
        {
            RedrawWindow();
        }

        m_TransEnb = true;
    }
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnInitTrans(WPARAM wParam, LPARAM lParam)
{
    InitTrans(true);
    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnDeinitTrans(WPARAM wParam, LPARAM lParam)
{
    DeinitTrans();
    return 0L;
}

///////////////////////////////////////////////////

void CmusikCubeFrame::RequerySelectionCtrls()
{
    for (size_t i = 0; i < this->m_AllFilterViews.size(); i++)
    {
        CmusikSelectionCtrl* currentCtrl = this->m_AllFilterViews[i]->GetCtrl();
        currentCtrl->Requery();
    }
}

///////////////////////////////////////////////////

void CmusikCubeFrame::QuerySelectionCtrls()
{
    for (size_t i = 0; i < this->m_AllFilterViews.size(); i++)
    {
        CmusikSelectionCtrl* currentCtrl = this->m_AllFilterViews[i]->GetCtrl();

        int currentOrder = this->GetSelectionCtrlOrder(currentCtrl);
        //
        if (currentOrder == -1)     // only Query boxes that are not filtered
        {
            currentCtrl->Query(this->GetSelectionCtrlFilter(currentCtrl));
        }
    }
}

///////////////////////////////////////////////////

void CmusikCubeFrame::QueryPlaylist(musikCore::Playlist& playlist)
{
    // if there are selection boxes visible
    musikCore::String query = this->GetPlaylistQuery();
    //
    if (query != L"")
    {
        // see what the order should be by...
        int orderByType = musikCore::MUSIK_LIBRARY_TYPE_ARTIST;
        CmusikSelectionCtrl* parent = this->GetParentSelectionCtrl();
        if (parent)
        {
            orderByType = parent->GetType();
        }

        musikCube::g_Library->RawQuerySongs(query, playlist);
    }
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnSelBoxRequestUpdate(WPARAM wParam, LPARAM lParam)
{
    UpdateFilters(UpdateAction::Query, UpdateAction::None);
    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnEqualizerChange(WPARAM wParam, LPARAM lParam)
{
    m_EqualizerView->GetCtrl()->LoadCurrSong();

    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnSongChange(WPARAM wParam, LPARAM lParam)
{
    // tell the child windows to redraw their
    // state accordingly
    if ((CmusikPlaylistView*)m_MainView == m_PlaylistView)
    {
        if (m_PlaylistView->GetCtrl()->GetPlaylist() == musikCube::g_Player->GetPlaylist())
            m_PlaylistView->GetCtrl()->ScrollToCurr();

        m_PlaylistView->GetCtrl()->ResyncItem(musikCube::g_Player->GetPrevSong());
        m_PlaylistView->GetCtrl()->ResyncItem(musikCube::g_Player->GetPlaying()->GetID());
    }
    else if (m_MainView == (CView*) m_CDView)
    {
        m_CDView->RedrawWindow();
    }

    m_NowPlayingView->GetCtrl()->GetTimeCtrl()->OnNewSong();
    m_NowPlayingView->GetCtrl()->UpdateInfo();

    for (size_t i = 0; i < m_AllFilterViews.size(); i++)
    {
        m_AllFilterViews.at(i)->GetCtrl()->OnNewSong();
        m_AllFilterViews.at(i)->GetCtrl()->RedrawWindow();
    }
    
    // if the player is playing, then we need
    // to do our own updating
    if (musikCube::g_Player->IsPlaying())
    {
        this->UpdateTitle();
    }

    // update notification tooltip
    if (musikCube::g_Prefs->MinimizeToTray())
    {
        CString tray = ::GetPlaybackStr(PBSTRTYPE_TASKTRAY);
        if (tray.GetLength() > 127)
            tray = tray.Left(127); 
        wcscpy(m_TrayIcon.szTip, tray.GetBuffer());
        Shell_NotifyIcon(NIM_MODIFY, &m_TrayIcon);

        if (musikCube::g_Prefs->GetShowBalloonTips())
        {
            ShowBalloonTip(
                ::GetPlaybackStr(PBSTRTYPE_BALLOONTITLE), 
                ::GetPlaybackStr(PBSTRTYPE_BALLOONARTISTALBUM));
        }
    }

    if (musikCube::g_Prefs->GetTunageEnabled())
    {
        CmusikTunage tunage;
        tunage.Execute();
    }

    CmusikPropTreeItem* focused = m_SourcesView->GetFocusedItem();
    if (focused && focused->GetPlaylistType() == musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC)
    {
        m_PlaylistView->GetCtrl()->Refresh();
    }

    return 0L;
}   

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnSongStop(WPARAM wParam, LPARAM lParam)
{
    cout << _T("Song stop signal caught\n");

    m_NowPlayingView->GetCtrl()->GetTimeCtrl()->OnNewSong();
    m_NowPlayingView->GetCtrl()->UpdateInfo();

    m_PlaylistView->GetCtrl()->ResyncItem(musikCube::g_Player->GetPrevSong());
    m_PlaylistView->GetCtrl()->RedrawWindow();
    for (size_t i = 0; i < m_AllFilterViews.size(); i++)
    {
        m_AllFilterViews.at(i)->GetCtrl()->RedrawWindow();
    }

    // update title bar
    this->UpdateTitle();

    if (musikCube::g_Prefs->MinimizeToTray())
    {
        CString tray;
        wcscpy (m_TrayIcon.szTip, _T(MUSIK_VERSION_STR));

        // clears balloon tip info
        wcscpy(m_TrayIcon.szInfoTitle, _T("\0"));
        wcscpy(m_TrayIcon.szInfo, _T("\0"));

        Shell_NotifyIcon(NIM_MODIFY, &m_TrayIcon);
    }

    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnSongPause(WPARAM wParam, LPARAM lParam)
{
    cout << _T("Pause song signal caught\n");
    m_NowPlayingView->GetCtrl()->UpdateInfo();
    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnSongResume(WPARAM wParam, LPARAM lParam)
{
    cout << _T("Resume song signal caught\n");
    m_NowPlayingView->GetCtrl()->UpdateInfo();
    return 0L;
}

///////////////////////////////////////////////////

// SELECTING AN ITEM ALWAYS SETS ITSELF AS THE NEW GLOBALLY SELECTED PLAYLIST INFO

LRESULT CmusikCubeFrame::OnSourcesItemClicked(WPARAM wParam, LPARAM lParam)
{
    this->m_PlaylistInfo = this->m_SourcesView->GetFocusedItem()->GetPlaylistInfo();
    HideCDView();

    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnSourcesLibrary(WPARAM wParam, LPARAM lParam)
{
    UpdateFilters(UpdateAction::Reset, UpdateAction::Reset);
    ShowSelectionBoxes(true);

    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnSourcesQuickSearchStr(WPARAM wParam, LPARAM lParam)
{
    if (wParam)
    {
        musikCore::Playlist* playlist = new musikCore::Playlist();
        CString* sPattern = (CString*)wParam;
        if (*sPattern != L"")
        {
            musikCube::g_Library->QuickQuery((musikCore::String)*sPattern, *playlist);
        }

        this->m_PlaylistInfo = playlist->GetPlaylistInfo();
        this->GivePlaylistToView(playlist);
        //
        UpdateFilters(UpdateAction::Query, UpdateAction::Requery);
    }

    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnSourcesQuickSearch(WPARAM wParam, LPARAM lParam)
{
    ShowSelectionBoxes(true);
    UpdateFilters(UpdateAction::Clear, UpdateAction::Clear);

    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnSourcesNowPlaying(WPARAM wParam, LPARAM lParam)
{
    if (musikCube::g_Player->GetPlayerType() == musikCore::MUSIK_PLAYER_CDROM)
    {
        OnSourcesCDAudio((WPARAM)musikCube::g_Player->GetDiscDrive(), NULL);
        return 0L;
    }

    this->GivePlaylistToView(musikCube::g_Player->GetPlaylist());
    UpdateFilters(UpdateAction::None, UpdateAction::Requery);
    m_PlaylistView->GetCtrl()->ScrollToCurr();

    ShowSelectionBoxes(false);

    return 0L;
}

///////////////////////////////////////////////////

void CmusikCubeFrame::UpdateFilters(enum UpdateAction selectionUpdateAction, enum UpdateAction playlistUpdateAction)
{
    switch (selectionUpdateAction)
    {
    case CmusikCubeFrame::UpdateAction::Reset:
        ResetSelectionCtrls();
        break;

    case CmusikCubeFrame::UpdateAction::Requery:
        RequerySelectionCtrls();
        break;

    case CmusikCubeFrame::UpdateAction::Query:
        QuerySelectionCtrls();
        break;

    case CmusikCubeFrame::UpdateAction::Clear:
        ClearSelectionCtrls();
        break;
    }

    switch (playlistUpdateAction)
    {
    case CmusikCubeFrame::UpdateAction::Query:
        this->QueryPlaylistCtrl();
        break;
 
    case CmusikCubeFrame::UpdateAction::Reset:
        this->ResetPlaylistCtrl();
        break;

    case CmusikCubeFrame::UpdateAction::Requery:
        this->RequeryPlaylistCtrl();
        break;

    case CmusikCubeFrame::UpdateAction::Clear:
        this->ClearPlaylistCtrl();
        break;
    }
}

///////////////////////////////////////////////////

void CmusikCubeFrame::UpdateTitle()
{
    CString s, caption;

    if (musikCube::g_Player->IsPlaying())
    {
        s.Format(_T("[%s - %s] "), 
            musikCube::g_Player->GetPlaying()->GetTitle().c_str(), 
            musikCube::g_Player->GetPlaying()->GetArtist().c_str());

        if (musikCube::g_Player->GetFormat() == MUSIK_LIBRARY_FORMAT_NET)
            s += _T(" (streaming audio) ");
    }

    caption += s;
    caption += _T(MUSIK_VERSION_STR);

    size_t task_count = musikCube::g_Library->GetTaskCount();
    if (task_count)
    {      
        int type = musikCube::g_Library->GetTaskType();
        switch (type)
        {
        case musikCore::MUSIK_TASK_TYPE_BATCHADD:
            s.Format(_T(" (adding files, %d%%) "), m_Progress);
            break;
        case musikCore::MUSIK_TASK_TYPE_REMOVEOLD:
            s.Format(_T(" (removing non-existent files, %d%%) "), m_Progress);
            break;
        case musikCore::MUSIK_TASK_TYPE_BATCHRETAG:
            s.Format(_T(" (retagging files, %d%%) "), m_Progress);
            break;
        case musikCore::MUSIK_TASK_TYPE_TAGFROMFN:
            s.Format(_T(" (converting filename to tag, %d%%) "), m_Progress);
            break;
        case musikCore::MUSIK_TASK_TYPE_TAGTOFN:
            s.Format(_T(" (renaming files based on tag, %d%%) "), m_Progress);
            break;
        case musikCore::MUSIK_TASK_TYPE_BATCHCHGCASE:
            s.Format(_T(" (changing tag case, %d%%) "), m_Progress);
            break;
        case musikCore::MUSIK_TASK_TYPE_RELOAD_TAGS:
            s.Format(_T(" (reloading tags from file, %d%%) "), m_Progress);
            break;
        default:
            s.Format(_T(" (background processing) "));
            break;
        }
        caption += s;
    }

    if (this->m_LastCaption != caption)
    {
        SetWindowText(caption);
        this->m_LastCaption = caption;
    }
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnSourcesStdPlaylist(WPARAM wParam, LPARAM lParam)
{
    musikCore::Playlist* standardPlaylist = new musikCore::Playlist();
    //
    // m_PlaylistInfo already set from OnSourcesItemClicked
    musikCube::g_Library->GetStdPlaylist(
        this->m_PlaylistInfo.GetID(),
        *standardPlaylist,
        true);
    //
    this->m_PlaylistInfo = standardPlaylist->GetPlaylistInfo();      // m_Query will now be valid
    this->GivePlaylistToView(standardPlaylist);
    UpdateFilters(UpdateAction::Reset, UpdateAction::Requery);
    //
    ShowSelectionBoxes(true);

    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnSourcesDynPlaylist(WPARAM wParam, LPARAM lParam)
{
    musikCore::Playlist* dynamicPlaylist = new musikCore::Playlist();
    //
    // m_PlaylistInfo already set from OnSourcesItemClicked
    musikCube::g_Library->QueryDynPlaylist(
        this->m_PlaylistInfo.GetID(),
        *dynamicPlaylist,
        true);
    //
    this->m_PlaylistInfo = dynamicPlaylist->GetPlaylistInfo();      // m_Query will now be valid
    this->GivePlaylistToView(dynamicPlaylist);
    this->UpdateFilters(UpdateAction::Reset, UpdateAction::Requery);
    //
    this->ShowSelectionBoxes(true);

    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnDragStart(WPARAM wParam, LPARAM lParam)
{
    cout << _T("DND Started.\n");

    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnDragEnd(WPARAM wParam, LPARAM lParam)
{
    cout << _T("DND Ended.\n");

    return 0L;
}

///////////////////////////////////////////////////

BOOL CmusikCubeFrame::OnEraseBkgnd(CDC* pDC)
{
    return false;
}


///////////////////////////////////////////////////

void CmusikCubeFrame::OnPaint()
{
    CPaintDC dc(this);
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnSysColorChange()
{
    CFrameWnd::OnSysColorChange();

    RedrawWindow(
        NULL,
        NULL,
        RDW_ERASE|RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN
   );
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnFilePreferences()
{
    // initialize the property pages
    CmusikPrefsInterfaceGeneral wndPageInterfaceGeneral;
    CmusikPrefsInterfaceWorkflow wndPageInterfaceWorkflow;
    CmusikPrefsInterfaceDevices wndPageInterfaceDevices;
    CmusikPrefsInterfaceTrans wndPageInterfaceTrans;
    CmusikPrefsSoundCrossfader wndPageSoundCrossfader;
    CmusikPrefsSoundDriver wndPageSoundDriver;
    CmusikPrefsTunage wndPageTunage;
    CmusikPrefsInternet wndPageInternet;
    CmusikPrefsAutoCapitalize wndPageAutoCapitalize;
    CmusikPrefsFileAssociations wndFileAssociations;

    // remove help icon from gripper
    wndPageInterfaceGeneral.m_psp.dwFlags&=     ~PSP_HASHELP;
    wndPageInterfaceWorkflow.m_psp.dwFlags&=    ~PSP_HASHELP;
    wndPageInterfaceDevices.m_psp.dwFlags&=     ~PSP_HASHELP;
    wndPageInterfaceTrans.m_psp.dwFlags&=       ~PSP_HASHELP;
    wndPageSoundCrossfader.m_psp.dwFlags&=      ~PSP_HASHELP;
    wndPageSoundDriver.m_psp.dwFlags&=          ~PSP_HASHELP;
    wndPageTunage.m_psp.dwFlags&=               ~PSP_HASHELP;
    wndPageInternet.m_psp.dwFlags&=             ~PSP_HASHELP;
    wndPageAutoCapitalize.m_psp.dwFlags&=       ~PSP_HASHELP;
    wndFileAssociations.m_psp.dwFlags&=          ~PSP_HASHELP;

    // initialize the CTreePropSheet class 
    // and remove help icon from gripper
    CmusikPropertySheet PrefSheet(_T("Preferences - musikCube"));
    PrefSheet.m_psh.dwFlags&= ~PSH_HASHELP;
    PrefSheet.m_psh.dwFlags|= PSH_NOCONTEXTHELP|PSH_NOAPPLYNOW;

    // physically add the preference sheets
    PrefSheet.AddPage(&wndPageInterfaceGeneral);
    PrefSheet.AddPage(&wndPageInterfaceWorkflow);
    PrefSheet.AddPage(&wndPageInterfaceDevices);
    PrefSheet.AddPage(&wndPageInterfaceTrans);
    PrefSheet.AddPage(&wndPageSoundCrossfader);
    PrefSheet.AddPage(&wndPageSoundDriver);
    PrefSheet.AddPage(&wndPageTunage);
    PrefSheet.AddPage(&wndPageInternet);
    PrefSheet.AddPage(&wndPageAutoCapitalize);
    PrefSheet.AddPage(&wndFileAssociations);

    PrefSheet.SetEmptyPageText(_T("Please select one of the items under \"%s.\""));

    // remember the amount of hidden devices we started with
    int hiddencnt = musikCube::g_Prefs->GetHiddenDevices().size();

    // set some miscellaneous tree view properties
    PrefSheet.SetTreeViewMode(true, true, false);
    PrefSheet.SetActivePage(&wndPageInterfaceGeneral);
    PrefSheet.DoModal();

    //
    // update ui after preferences
    //

    if (!musikCube::g_Prefs->GetShowBalloonTips())
    {
        wcscpy(m_TrayIcon.szInfoTitle, _T(""));
        wcscpy(m_TrayIcon.szInfo, _T(""));
        Shell_NotifyIcon(NIM_MODIFY, &m_TrayIcon);
    }

    if (!musikCube::g_Prefs->IsTransEnabled() && m_Trans != 255)
        SetTransparency(255);

    if (musikCube::g_Prefs->MinimizeToTray() || musikCube::g_Prefs->GetCloseToSystemTray())
    {
        if (!m_TrayIconVisible)
        {
            ShowTrayIcon();
        }
    }
    else if (m_TrayIconVisible)
    {
        HideTrayIcon();
    }

    // update the player's buffer size
    musikCube::g_Player->SetBufferSize(musikCube::g_Prefs->GetPlayerBuffer());

    // hack! requery the playlist view if UTC has changed.
    this->UpdateFilters(UpdateAction::None, UpdateAction::Requery);
} 

///////////////////////////////////////////////////

void CmusikCubeFrame::OnMenuAddFiles()
{
    // string used to populate the File Types dropdown box. the first
    CString str_types = _T("Supported Media Files |");
    const musikCore::StringArray& formats = musikCore::GetPlayerFormats();

    for (size_t i = 0; i < formats.size(); i++)
    {
        CString curr_type = formats.at(i);
        if (curr_type == _T("cdda") || curr_type == _T("net"))
            continue;

        str_types += _T("*.");
        str_types += curr_type;
        str_types += _T(";");
    }

    str_types += _T("|");

    for (size_t i = 0; i < formats.size(); i++)
    {
        CString curr_type = formats.at(i);
        if (curr_type == _T("cdda") || curr_type == _T("net"))
        {
            continue;
        }

        str_types += curr_type.MakeUpper();
        str_types += _T(" files (*.");
        str_types += curr_type.MakeLower();
        str_types += _T(")|*.");
        str_types += curr_type;
        str_types += _T("|");
    }

    str_types += _T("|");

    // create the open dialog object
    CmusikFileDialog opendlg(
        TRUE, 
        _T("mp3"), 
        NULL, OFN_ALLOWMULTISELECT | OFN_EXPLORER, 
        str_types);

    if (opendlg.DoModal() == IDOK)
    {
        musikCore::StringArray* files = new musikCore::StringArray();

        POSITION posCurr = opendlg.GetStartPosition();
        while (posCurr)
            files->push_back((musikCore::String)opendlg.GetNextPathName(posCurr));

        if (files->size() > 0)
        {
            // setup params
            musikCore::BatchAdd* params = new musikCore::BatchAdd(
                files, 
                "",
                NULL, 
                musikCube::g_MainLibrary, 
                NULL, 
                musikCube::g_Functor, 
                0, 
                0, 
                1);

            // spawn task
            musikCore::BatchAddTask* task = new musikCore::BatchAddTask;
            task->m_Params = params;
            musikCube::g_MainLibrary->QueueTask(task);
        }
        else
            delete files;
    }
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnTaskProgress(WPARAM wParam, LPARAM lParam)
{
    m_Progress = (int)wParam;
    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnTaskEnd(WPARAM wParam, LPARAM lParam)
{
    UpdateFilters(UpdateAction::Requery, UpdateAction::Query);
    m_Progress = 0;

    return 0L;
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnMenuAddDirectory()
{
    TCHAR path[MAX_PATH];
    BROWSEINFO bi = { 0 };
    bi.lpszTitle = _T("Please select a directory containing music.");
    bi.hwndOwner = GetSafeHwnd();

    LPITEMIDLIST pidl = SHBrowseForFolder (&bi);

    if (pidl != 0)
    {
        // get the name of the folder and put it in path
        SHGetPathFromIDList (pidl, path);

        // free memory used
        IMalloc * imalloc = 0;
        if (SUCCEEDED(SHGetMalloc (&imalloc)))
        {
            imalloc->Free (pidl);
            imalloc->Release ();
        }

        musikCore::StringArray* files = new musikCore::StringArray();

        // get all the musik related files
        // in the directory we just found...
        musikCore::String sPath = path;
        if (sPath.Right(1) != _T("\\"))
            sPath += _T("\\*.*");
        else
            sPath += _T("*.*");

        // setup params
        musikCore::BatchAdd* params = new musikCore::BatchAdd(
            files, 
            sPath,
            NULL, 
            musikCube::g_MainLibrary, 
            NULL, 
            musikCube::g_Functor, 
            0, 
            0, 
            1);
        
        // start it up
        musikCore::BatchAddTask* task = new musikCore::BatchAddTask;
        task->m_Params = params;
        musikCube::g_MainLibrary->QueueTask(task);
    }
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnPlayerPlaySel(WPARAM wParam, LPARAM lParam)
{
    // player is paused, so resume...
    if (musikCube::g_Player->IsPlaying() && musikCube::g_Player->IsPaused())
        musikCube::g_Player->Resume();
    
    else
    {
        if (musikCube::g_Player->GetPlaymode() & musikCore::MUSIK_PLAYER_PLAYMODE_RANDOM)
        {
            int first = musikCore::Library::Rand(0, (int)musikCube::g_Player->GetPlaylist()->GetCount());
            musikCube::g_Player->Play(first);
        }

        else if (!m_PlaylistView->GetCtrl()->PlayItem())
            musikCube::g_Player->Play(0);
    }

    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnVerifyPlaylist(WPARAM wParam, LPARAM lParam)
{
    musikCore::Playlist* playlist = (musikCore::Playlist*)wParam;

    if (playlist == NULL || m_PlaylistView->GetCtrl()->GetPlaylist() == NULL)
        return 0L;

    if (playlist == m_PlaylistView->GetCtrl()->GetPlaylist())
        return 1L;

    return 0L;
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnViewSources()
{
    if (m_SourcesView->IsVisible())
        ShowControlBar(m_SourcesView, FALSE, FALSE);
    else
        ShowControlBar(m_SourcesView, TRUE, FALSE);
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUpdateViewSources(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_SourcesView->IsVisible());
}

///////////////////////////////////////////////////

void CmusikCubeFrame::ShowSelectionBoxes(bool show)
{
    if (musikCube::g_Prefs->ShowSelectionBoxes() == false)
    {
        show = false;
    }

    if (show)
    {
        CSizingControlBar::GlobalLoadState(this, _T("musikProfile0.90"));
    }
    else
    {
        CSizingControlBar::GlobalSaveState(this, _T("musikProfile0.90"));
    }

    for (size_t i = 0; i < m_AllFilterViews.size(); i++)
    {
        ShowControlBar(m_AllFilterViews[i], show, TRUE);
    }
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnViewNowplaying()
{
    if (m_NowPlayingView->IsVisible())
        ShowControlBar(m_NowPlayingView, FALSE, FALSE);
    else
        ShowControlBar(m_NowPlayingView, TRUE, FALSE);
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUpdateViewNowplaying(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_NowPlayingView->IsVisible());
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnAudioEqualizerEnabled()
{
    if (musikCube::g_Prefs->IsEqualizerEnabled())
    {
        musikCube::g_Prefs->SetEqualizerEnabled(false);
        musikCube::g_Player->EnableEqualizer(false, musikCube::g_Prefs->EqualizerUseGlobal());
    }
    else
    {
        musikCube::g_Prefs->SetEqualizerEnabled(true);
        musikCube::g_Player->EnableEqualizer(true, musikCube::g_Prefs->EqualizerUseGlobal());
        musikCore::SetEQPreampLevel(musikCube::g_Prefs->GetEqualizerMultiplier());
        musikCube::g_Player->UpdateEQSettings();
    }

    m_NowPlayingView->GetCtrl()->UpdateCheckboxes();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUpdateAudioEqualizerEnabled(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(musikCube::g_Prefs->IsEqualizerEnabled());
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUpdateAudioCrossfaderEnabled(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(musikCube::g_Prefs->IsCrossfaderEnabled());
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnAudioCrossfaderEnabled()
{
    if (musikCube::g_Prefs->IsCrossfaderEnabled())
    {
        musikCube::g_Prefs->SetCrossfaderEnabled(false);
        musikCube::g_Player->EnableCrossfader(false);
    }
    else
    {
        musikCube::g_Prefs->SetCrossfaderEnabled(true);

        musikCore::Crossfader fade;

        // function to get default crossfader
        CmusikCubeApp* ptrApp = (CmusikCubeApp*)AfxGetApp();
        ptrApp->GetCrossfader(&fade);

        musikCube::g_Player->EnableCrossfader(true);
        musikCube::g_Player->SetCrossfader(fade);
    }

    m_NowPlayingView->GetCtrl()->UpdateCheckboxes();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnViewPlaylistinformation()
{
    if (musikCube::g_Prefs->PlaylistInfoVisible())
    {
        musikCube::g_Prefs->SetPlaylistInfoVisible(false);
    
        if (!musikCube::g_Prefs->GetPlaylistInfoVizStyle())
        {
            m_PlaylistView->SetUsePlaylistInfo(false); 
            m_PlaylistView->DeinitPlaylistInfo();
        }
    }
    else
    {
        m_PlaylistView->SetUsePlaylistInfo(true);    
        musikCube::g_Prefs->SetPlaylistInfoVisible(true);
    }
    // send a "dummy" size event to the
    // window, so it re-creates or removes
    // the now playing window.
    CRect lpRect;
    m_PlaylistView->GetClientRect(&lpRect);
    m_PlaylistView->OnSize(NULL, lpRect.Width(), lpRect.Height());
    RedrawWindow();
    m_PlaylistView->UpdatePlaylistInfo();
    
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUpdateViewPlaylistinformation(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(musikCube::g_Prefs->PlaylistInfoVisible());
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUpdatePlaybackmodeRepeatsingle(CCmdUI *pCmdUI)
{
    if (musikCube::g_Player->GetPlaymode() & musikCore::MUSIK_PLAYER_PLAYMODE_REPEAT_SINGLE)
        pCmdUI->SetCheck(true);
    else
        pCmdUI->SetCheck(false);
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnPlaybackmodeRepeatsingle()
{
    if (musikCube::g_Player->GetPlaymode() & musikCore::MUSIK_PLAYER_PLAYMODE_REPEAT_SINGLE)
        musikCube::g_Player->ModifyPlaymode(NULL, musikCore::MUSIK_PLAYER_PLAYMODE_REPEAT_SINGLE);
    else
        musikCube::g_Player->ModifyPlaymode(musikCore::MUSIK_PLAYER_PLAYMODE_REPEAT_SINGLE, NULL);

    musikCube::g_Prefs->SetPlayerPlaymode(musikCube::g_Player->GetPlaymode());
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUpdatePlaybackmodeRepeatplaylist(CCmdUI *pCmdUI)
{
    if (musikCube::g_Player->GetPlaymode() & musikCore::MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST)
        pCmdUI->SetCheck(true);
    else
        pCmdUI->SetCheck(false);
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnPlaybackmodeRepeatplaylist()
{
    if (musikCube::g_Player->GetPlaymode() & musikCore::MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST)
        musikCube::g_Player->ModifyPlaymode(NULL, musikCore::MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST);
    else
        musikCube::g_Player->ModifyPlaymode(musikCore::MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST, NULL);

    musikCube::g_Prefs->SetPlayerPlaymode(musikCube::g_Player->GetPlaymode());
    m_NowPlayingView->GetCtrl()->UpdateCheckboxes();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUpdatePlaybackmodeIntro(CCmdUI *pCmdUI)
{
    if (musikCube::g_Player->GetPlaymode() & musikCore::MUSIK_PLAYER_PLAYMODE_INTRO)
        pCmdUI->SetCheck(true);
    else
        pCmdUI->SetCheck(false);
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnPlaybackmodeIntro()
{
    if (musikCube::g_Player->GetPlaymode() & musikCore::MUSIK_PLAYER_PLAYMODE_INTRO)
        musikCube::g_Player->ModifyPlaymode(NULL, musikCore::MUSIK_PLAYER_PLAYMODE_INTRO);
    else
        musikCube::g_Player->ModifyPlaymode(musikCore::MUSIK_PLAYER_PLAYMODE_INTRO, NULL);

    musikCube::g_Prefs->SetPlayerPlaymode(musikCube::g_Player->GetPlaymode());
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnMenuViewModifiedTags()
{
    ShowSelectionBoxes(true);
    m_SourcesView->DeselectAllItems();

    musikCore::Playlist* unsyncTags = new musikCore::Playlist();
    musikCube::g_Library->GetDirtySongs(*unsyncTags);
    //
    this->m_PlaylistInfo = unsyncTags->GetPlaylistInfo();
    this->GivePlaylistToView(unsyncTags);
    this->UpdateFilters(UpdateAction::Reset, UpdateAction::Requery);
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUnsynchronizedtagsWritetofile()
{
    // get dirty items into a new playlist
    musikCore::Playlist playlist;
    musikCube::g_Library->GetDirtySongs(playlist);

    // create a new musikCore::SongInfoArray and update
    // all the respective values...
    if (playlist.GetCount())
    {
        musikCore::SongInfoArray* pSongInfoArray = new musikCore::SongInfoArray();
        musikCube::g_Library->GetInfoArrayFromPlaylist(playlist, *pSongInfoArray);

        // setup params
        musikCore::BatchRetag* params = new musikCore::BatchRetag(
            musikCube::g_Library, 
            musikCube::g_Functor,
            pSongInfoArray);
        params->m_WriteToFile = true;

        // setup and fire off task
        musikCore::BatchRetagTask* task = new musikCore::BatchRetagTask;
        task->m_Params = params;
        musikCube::g_Library->QueueTask(task);
    }
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUnsynchronizedtagsFinalizefordatabaseonly()
{
    musikCube::g_Library->FinalizeDirtySongs();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::SendToTray()
{
    SaveWindowState();
    ShowWindow(SW_MINIMIZE);
    m_IsMinimized = true;
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
    if (nID == SYSMENU_RESETDIALOG)
    {
        this->ResetDialogPosition();
        return;
    }

    if (nID == SC_MINIMIZE)
    {
        SendToTray();
        if (musikCube::g_Prefs->MinimizeToTray())
            ShowWindow(SW_HIDE);
        return;
    }

    if (nID == SC_CLOSE)
    {
        if (musikCube::g_Prefs->GetCloseToSystemTray())
        {
            SendToTray();
            if (musikCube::g_Prefs->GetCloseToSystemTray())
                ShowWindow(SW_HIDE);
            return;
        }
    }

    // dunno what it is, but 61730 gets
    // called when double clicking the title
    // bar to restore...
    else if (nID == SC_RESTORE || nID == 61730)
    {
        if (m_FullScreen)
        {
            KillFullScreen();
            return;
        }
        else if (m_IsMinimized)
        {
            ShowWindow(SW_RESTORE);
            RestoreDialogPosition();
        }
        else
        {
            ShowWindow(SW_RESTORE);
        }

        return;
    }

    else if (nID == SC_MAXIMIZE /*|| nID == 61458*/)
        SaveWindowState();

    CFrameWnd::OnSysCommand(nID, lParam);
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnViewEqualizer()
{
    if (m_EqualizerView->IsVisible())
        ShowControlBar(m_EqualizerView, FALSE, FALSE);
    else
        ShowControlBar(m_EqualizerView, TRUE, FALSE);
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUpdateViewEqualizer(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_EqualizerView->IsVisible());
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnFileSynchronizeddirectories()
{
    CmusikDirSync dirSyncDlg(this, musikCube::g_MainLibrary);
    dirSyncDlg.DoModal();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::InitTrayIcon()
{
    ZeroMemory(&m_TrayIcon, sizeof(NOTIFYICONDATA));

    m_TrayIcon.cbSize = sizeof(NOTIFYICONDATA); 
    m_TrayIcon.hWnd   = GetSafeHwnd(); 
    m_TrayIcon.uID    = ID_NOTIFY_ICON;
    m_TrayIcon.hIcon  = m_hIcon16; 
    m_TrayIcon.uCallbackMessage = TRAY_NOTIFY_EVT; 
    m_TrayIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_INFO; 
    m_TrayIcon.dwInfoFlags = NIIF_INFO;
    m_TrayIcon.uTimeout = 5000;

    wcscpy (m_TrayIcon.szTip, _T(MUSIK_VERSION_STR));

    m_TrayIconVisible = false;
}

///////////////////////////////////////////////////

void CmusikCubeFrame::ShowTrayIcon()
{
    if (!m_TrayIconVisible)
    {
        Shell_NotifyIcon(NIM_ADD, &m_TrayIcon);   
        m_TrayIconVisible = true;
    }
}

///////////////////////////////////////////////////

void CmusikCubeFrame::ShowBalloonTip(const CString& str_title, const CString& str_info)
{
    CString title = str_title;
    if (title.GetLength() > 63)
    {
        title = title.Left(59);
        title += _T("...");
    }

    CString info = str_info;
    if (info.GetLength() > 250)
    {
        info = info.Left(250);
        info += _T("...");
    }

    // show it
    wcsncpy(m_TrayIcon.szInfoTitle, title, 64);
    wcsncpy(m_TrayIcon.szInfo, info, 256);
    m_TrayIcon.uTimeout = 2000;
    m_TrayIcon.dwInfoFlags |= NIIF_NOSOUND;
    Shell_NotifyIcon(NIM_MODIFY, &m_TrayIcon);
}

///////////////////////////////////////////////////

void CmusikCubeFrame::HideTrayIcon()
{
    if (m_TrayIconVisible)
    {
        m_TrayIcon.uFlags = NIF_ICON;
        Shell_NotifyIcon(NIM_DELETE, &m_TrayIcon);
        m_TrayIconVisible = false;
    }
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // process messages from the tray
    if (message == TRAY_NOTIFY_EVT)
    {
        switch(lParam)
        {
        case WM_MBUTTONDOWN:
            {
                if (m_IsMinimized)
                    RestoreFromTray();
                else
                    ::SetForegroundWindow(GetSafeHwnd());
            }
            break;

        case WM_LBUTTONDOWN:
            {
                if (m_IsMinimized)
                    RestoreFromTray();
                else
                {
                    SendToTray();
                    if (musikCube::g_Prefs->MinimizeToTray())
                        ShowWindow(SW_HIDE);
                }

                return 1L;
            }
            break;

        case WM_RBUTTONDOWN:
        case WM_CONTEXTMENU:
            {
                CPoint pos;
                ::GetCursorPos(&pos);

                CMenu main_menu;
                CMenu* popup_menu;

                main_menu.LoadMenu(IDR_TRAY_MENU);
                popup_menu = main_menu.GetSubMenu(0);
                
                // check the correct rating
                if (musikCube::g_Player->IsPlaying())
                {
                    int rating = StoI(musikCube::g_Player->GetPlaying()->GetRating());
                    switch(rating)
                    {   
                    case -1:
                        popup_menu->CheckMenuItem(ID_RATECURRENT_N1, MF_CHECKED);
                        break;
                    case 0:
                        popup_menu->CheckMenuItem(ID_RATECURRENT_UNRATED, MF_CHECKED);
                        break;
                    case 1:
                        popup_menu->CheckMenuItem(ID_RATECURRENT_1, MF_CHECKED);
                        break;
                    case 2:
                        popup_menu->CheckMenuItem(ID_RATECURRENT_2, MF_CHECKED);
                        break;
                    case 3:
                        popup_menu->CheckMenuItem(ID_RATECURRENT_3, MF_CHECKED);
                        break;
                    case 4:
                        popup_menu->CheckMenuItem(ID_RATECURRENT_4, MF_CHECKED);
                        break;
                    case 5:
                        popup_menu->CheckMenuItem(ID_RATECURRENT_5, MF_CHECKED);
                        break;
                    }
                }

                SetForegroundWindow();
                popup_menu->TrackPopupMenu(0, pos.x, pos.y, this);
                PostMessage(WM_NULL, 0, 0);

                return 1L;
            }
            break;
        }
    }

    return CFrameWnd::WindowProc(message, wParam, lParam);
}

///////////////////////////////////////////////////

void CmusikCubeFrame::RestoreFromTray()
{
    RestoreDialogPosition();
    SetForegroundWindow();
    SetFocus();

    if (m_AutoStart)
    {
        if (musikCube::g_Prefs->SynchronizeOnStartup())
        {
            musikCube::SynchronizeDirs(musikCube::g_Library);
        }

        m_AutoStart = false;
    }
}

///////////////////////////////////////////////////

void CmusikCubeFrame::RestoreDialogPosition()
{
    CRect rcNormal = CRect(musikCube::g_Prefs->GetDlgPos(), musikCube::g_Prefs->GetDlgSize());

    // kludge.. i cannot figure out for the
    // life of me why this is happening
    if (rcNormal.left == -32000 || rcNormal.top == -32000)
    {
        rcNormal.left = 0;
        rcNormal.top = 0;
        rcNormal.right = 800;
        rcNormal.bottom = 600;
    }

    if (m_IsMinimized && musikCube::g_Prefs->IsMaximized())
    {
        ShowWindow(SW_NORMAL);
        MoveWindow(rcNormal);

        WINDOWPLACEMENT max;
        max.showCmd = SW_MAXIMIZE;
        max.rcNormalPosition = rcNormal;
        SetWindowPlacement(&max);

        musikCube::g_Prefs->SetMaximized(true);
    }
    else
    {
        ShowWindow(SW_NORMAL);
        MoveWindow(rcNormal);
        musikCube::g_Prefs->SetMaximized(false);
    }

    m_IsMinimized = false;
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnViewResetinterfacetodefault()
{
    ResetDialogPosition();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnNotificationtrayExit()
{
    RestoreFromTray();
    OnClose();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnNotificationtrayRestore()
{
    RestoreFromTray();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUpdateNotificationtrayPlay(CCmdUI *pCmdUI)
{
    if (musikCube::g_Player->IsPlaying())
        pCmdUI->Enable(false);
    else
        pCmdUI->Enable(true);
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnNotificationtrayPlay()
{
    if (musikCube::g_Player->GetPlaylist())
        musikCube::g_Player->Play(0);
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUpdateNotificationtrayPause(CCmdUI *pCmdUI)
{
    if (musikCube::g_Player->IsPlaying() && !musikCube::g_Player->IsPaused())
        pCmdUI->SetText(_T("Pause"));
    else
        pCmdUI->SetText(_T("Resume"));  

    if (musikCube::g_Player->IsPlaying())
        pCmdUI->Enable(true);
    else
        pCmdUI->Enable(false);
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnNotificationtrayPause()
{
    if (musikCube::g_Player->IsPlaying() && !musikCube::g_Player->IsPaused())
        musikCube::g_Player->Pause();
    else
        musikCube::g_Player->Resume();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUpdateNotificationtrayNext(CCmdUI *pCmdUI)
{
    if (musikCube::g_Player->IsPlaying())
        pCmdUI->Enable(true);
    else
        pCmdUI->Enable(false);
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnNotificationtrayNext()
{
    if (musikCube::g_Player->IsPlaying())
        musikCube::g_Player->Next();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUpdateNotificationtrayPrev(CCmdUI *pCmdUI)
{
    if (musikCube::g_Player->IsPlaying())
        pCmdUI->Enable(true);
    else
        pCmdUI->Enable(false);
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnNotificationtrayPrev()
{
    if (musikCube::g_Player->IsPlaying())
        musikCube::g_Player->Prev();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUpdateNotificationtrayStop(CCmdUI *pCmdUI)
{
    if (musikCube::g_Player->IsPlaying())
        pCmdUI->Enable(true);
    else
        pCmdUI->Enable(false);
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnNotificationtrayStop()
{
    if (musikCube::g_Player->IsPlaying())
        musikCube::g_Player->Stop();
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnSelBoxAddRemove(WPARAM wParam, LPARAM lParam)
{
    BOOL add_new = (BOOL)wParam;
    bool save = false;

    CmusikSelectionBar* pBar;

    if (add_new)
    {
        pBar = new CmusikSelectionBar(this, musikCore::MUSIK_LIBRARY_TYPE_ARTIST, 123 + m_AllFilterViews.size(), m_uSelectionDrop_L, m_uSelectionDrop_R);
        pBar->Create(_T("musik Selection Box"), this, ID_SELECTIONBOX_START + m_AllFilterViews.size());
        FloatControlBar(pBar, CPoint(0, 0));

        m_AllFilterViews.push_back(pBar);
        this->UpdateFilters(UpdateAction::Requery, UpdateAction::None);

        save = true;
    }

    else
    {
        pBar = (CmusikSelectionBar*)lParam;

        // remove the bar
        for (size_t i = 0; i < m_AllFilterViews.size(); i++)
        {
            if (pBar == m_AllFilterViews.at(i))
            {
                m_AllFilterViews.at(i)->DestroyWindow();
                delete m_AllFilterViews.at(i);
                m_AllFilterViews.erase(m_AllFilterViews.begin() + i);

                save = true;
                break;
            }
        }

        RecalcLayout();
    }

    // save new state to prefs
    if (save)
    {
        musikCore::IntArray new_selboxes;
        for (size_t i = 0; i < m_AllFilterViews.size(); i++)
            new_selboxes.push_back(m_AllFilterViews.at(i)->GetCtrl()->GetType());

        musikCube::g_Prefs->SetSelBoxTypes(new_selboxes);
    }

    return 0L;
}

///////////////////////////////////////////////////

void CmusikCubeFrame::KillTasks(bool updater, bool helpers)
{
    if (updater)
    {
        if (m_Updater)
        {
            m_Updater->StopWait(4);

            delete m_Updater;
            m_Updater = NULL;
        }
    }

    if (helpers)
        musikCube::g_Library->FlushTaskQueue();

    cout << _T("All tasks killed.\n");
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnMenuEraseLibrary()
{
    int prompt = MessageBox(
        _T("Are you sure you want to erase your music library?"), 
        _T(MUSIK_VERSION_STR), 
        MB_ICONWARNING | MB_YESNO);

    if (prompt != IDYES)
    {
        return;
    }

    // kill all tasks that may be accessing the database
    KillTasks(true, true);
    this->UpdateTitle();

    // stop player
    musikCube::g_Player->Stop();

    while (musikCube::g_Player->IsPlaying() || musikCube::g_Player->IsPaused())
    {
        Sleep(50);
    }

    musikCube::g_Player->GetPlaylist()->Clear();

    // clear
    musikCube::g_MainLibrary->ClearLibrary(true);

    m_SourcesView->ReloadAllItems();
    m_SourcesView->FocusLibrary();

    OnSourcesLibrary(NULL, NULL);
    UpdateFilters(UpdateAction::Reset, UpdateAction::None);

    // start the updater again
    m_Updater = new CmusikCubeUpdater;
    m_Updater->m_Parent = this;
    m_Updater->start();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnMenuSynchronize()
{
    musikCube::SynchronizeDirs(musikCube::g_MainLibrary);
}

///////////////////////////////////////////////////

BOOL CmusikCubeFrame::PreTranslateMessage(MSG* pMsg)
{
    switch(pMsg->message)
    {
    case WM_KEYDOWN:
        {
            if (pMsg->wParam == VK_ESCAPE && musikCube::g_Library->GetTaskCount())
            {
                KillTasks(false, true);
                this->UpdateTitle();
                return TRUE;
            }

            if (pMsg->wParam == VK_F5)
            {
                musikCore::Playlist* playlist = m_PlaylistView->GetCtrl()->GetPlaylist();
                if (playlist && playlist->GetType() == musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC)
                {
                    musikCube::g_Library->QueryDynPlaylist(
                        playlist->GetID(),
                        *m_PlaylistView->GetCtrl()->GetPlaylist());

                    // its possible that we re-arranged the now playing
                    // playlist. if we did, have it relocate the currently
                    // playing song. also, after querying a dynamic playlist
                    // it's type becomes MUSIK_PLAYLIST_TYPE_DYNAMIC, we need to
                    // toggle it back to MUSIK_PLAYLIST_TYPE_NOWPLAYING.
                    if (m_PlaylistView->GetCtrl()->GetPlaylist() == musikCube::g_Player->GetPlaylist())
                    {
                        int playid = musikCube::g_Player->GetPlaying()->GetID();
                        musikCube::g_Player->FindNewIndex(playid);
                        musikCube::g_Player->GetPlaylist()->SetType(musikCore::MUSIK_PLAYLIST_TYPE_NOWPLAYING);
                    }

                    m_PlaylistView->GetCtrl()->Refresh();
                    return TRUE;
                }

            }

            if ((wchar_t)pMsg->wParam == 'f' || (wchar_t)pMsg->wParam == 'F')
            {
                if (GetKeyState(VK_CONTROL) < 0)
                {
                    m_SourcesView->FocusQuickSearch();
                    m_SourcesView->EnableQuickSearch();
                    return TRUE;
                }
            }
        }
        break;

    case WM_APPCOMMAND:
        {
            switch(GET_APPCOMMAND_LPARAM(pMsg->lParam))
            {

            case APPCOMMAND_MEDIA_NEXTTRACK:
                {
                    if (musikCube::g_Player->IsPlaying())
                        musikCube::g_Player->Next();
                }
                break;

            case APPCOMMAND_MEDIA_PREVIOUSTRACK:
                {
                    if (musikCube::g_Player->IsPlaying())
                        musikCube::g_Player->Prev();
                }
                break;

            case APPCOMMAND_MEDIA_STOP:
                {
                    if (musikCube::g_Player->IsPlaying())
                        musikCube::g_Player->Stop();
                }
                break;

            case APPCOMMAND_MEDIA_PLAY_PAUSE:
                {
                    if (musikCube::g_Player->IsPlaying())
                    {
                        if (!musikCube::g_Player->IsPaused())
                        {
                            musikCube::g_Player->Pause();
                        }
                        else
                        {
                            musikCube::g_Player->Resume();
                        }
                    }
                    else
                    {
                        musikCore::Playlist* nowPlayingPlaylist = musikCube::g_Player->GetPlaylist();
                        if (nowPlayingPlaylist->GetCount() == 0)
                        {
                            musikCube::g_Library->GetAllSongs(*nowPlayingPlaylist);

                            if (m_PlaylistView->GetCtrl()->GetPlaylist() == nowPlayingPlaylist)
                            {
                                UpdateFilters(UpdateAction::None, UpdateAction::Requery);
                            }
                        }

                        musikCube::g_Player->Play();
                    }
                }
                break;
            }

            return true;
        }
        break;
    }

    return CFrameWnd::PreTranslateMessage(pMsg);
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnUpdateCurrPlaylist(WPARAM wParam, LPARAM lParam)
{
    UpdateFilters(UpdateAction::None, UpdateAction::Requery);
    m_PlaylistView->GetCtrl()->ScrollToCurr();
    return 0L;
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnPlaybackmodeShufflecurrentplaylist()
{
    musikCube::g_Player->Shuffle();

    if (m_PlaylistView->GetCtrl()->GetPlaylist() == musikCube::g_Player->GetPlaylist())
    {
        UpdateFilters(UpdateAction::None, UpdateAction::Requery);
        m_PlaylistView->GetCtrl()->ScrollToCurr();
    }
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnSelectionCtrlDeleteSelected(WPARAM wParam, LPARAM lParam)
{
    // setup vars
    CmusikSelectionCtrl* pSel = (CmusikSelectionCtrl*)wParam;
    BOOL fromComputer = (BOOL)lParam;
    BOOL fromLibrary = TRUE;

    // the selected items are whats currently visible in the playlist
    musikCore::Playlist playlist;
    this->QueryPlaylist(playlist);

    if (fromComputer)
    {
        CShellFileOp deleteFileOp;
        deleteFileOp.SetOperationFlags(FO_DELETE, this, FOF_ALLOWUNDO);
        //
        musikCore::String filename;
        for (size_t i = 0; i < playlist.GetCount(); i++)
        {
            musikCube::g_Library->GetFieldFromID(
                playlist.GetSong(i).GetID(), 
                musikCore::MUSIK_LIBRARY_TYPE_FILENAME, 
                filename);
            //
            if (musikCore::Filename::FileExists(filename))
            {
                deleteFileOp.AddSourceFile(filename.c_str());
            }
        }

        BOOL notUsed, apiResult, aborted;
        if (deleteFileOp.GetSourceFileList().GetCount())
        {
            deleteFileOp.Go(&notUsed, &apiResult, &aborted);
            if (aborted || apiResult)
            {
                fromLibrary = false;
            }
        }
    }
    
    // delete from library
    if (fromLibrary)
    {
        musikCube::g_Library->DeleteSongs(playlist);
        UpdateFilters(UpdateAction::Requery, UpdateAction::Query);

        // notify Player to update its internal playlist
        musikCube::g_Player->UpdatePlaylist();
    }

    return 0L;
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUpdatePlaybackmodeShufflecurrentplaylist(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(musikCube::g_Player->GetPlaylist()->GetCount());
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnViewVisualization()
{
    if (musikCube::g_Prefs->GetPlaylistInfoVizStyle())
    {
        musikCube::g_Prefs->SetPlaylistInfoVizStyle(NULL);

        if (!musikCube::g_Prefs->PlaylistInfoVisible())
        {
            m_PlaylistView->SetUsePlaylistInfo(false);
            m_PlaylistView->DeinitPlaylistInfo();
        }
    }
    else
    {
        musikCube::g_Prefs->SetPlaylistInfoVizStyle(1);
        m_PlaylistView->SetUsePlaylistInfo(true);
        m_PlaylistView->InitPlaylistInfo();
    }

    // send a "dummy" size event to the
    // window, so it re-creates or removes
    // the now playing window.
    CRect lpRect;
    m_PlaylistView->GetClientRect(&lpRect);
    m_PlaylistView->OnSize(NULL, lpRect.Width(), lpRect.Height());
    RedrawWindow();
    m_PlaylistView->UpdatePlaylistInfo();

}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUpdateViewVisualization(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(musikCube::g_Prefs->GetPlaylistInfoVizStyle());
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnViewAlwaysontop()
{
    SetRedraw(FALSE);

    CRect rcWnd;
    GetWindowRect(rcWnd);

    if (musikCube::g_Prefs->IsAlwaysOnTop())
    {
        ::SetWindowPos(m_hWnd, 
            HWND_NOTOPMOST, 
            rcWnd.TopLeft().x, 
            rcWnd.TopLeft().y, 
            rcWnd.Width(), 
            rcWnd.Height(), 
            NULL);
        m_EqualizerView->GetCtrl()->OnAlwaysOnTop();
        musikCube::g_Prefs->SetAlwaysOnTop(false);
    }

    else
    {
        ::SetWindowPos(m_hWnd, 
            HWND_TOPMOST, 
            rcWnd.TopLeft().x, 
            rcWnd.TopLeft().y, 
            rcWnd.Width(), 
            rcWnd.Height(), 
            NULL);

        musikCube::g_Prefs->SetAlwaysOnTop(true);
    }

    SetRedraw(TRUE);
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUpdateViewAlwaysontop(CCmdUI *pCmdUI)
{
    if (musikCube::g_Prefs->IsAlwaysOnTop())
        pCmdUI->SetCheck(TRUE);
    else
        pCmdUI->SetCheck(FALSE);
}

///////////////////////////////////////////////////

void CmusikCubeFrame::GoFullScreen()
{
    m_GoingFullScreen = true;

    // backup original size and state
    CSizingControlBar::GlobalSaveState(this, _T("musikProfile0.90"));
    SaveBarState(_T("musikProfile0.90"));
    SaveWindowState();

    // maximize and grab window size
    CRect rcWnd(musikCube::g_Prefs->GetDlgPos(), musikCube::g_Prefs->GetDlgSize());

    WINDOWPLACEMENT max;
    max.showCmd = SW_MAXIMIZE;
    max.rcNormalPosition = rcWnd;
    SetWindowPlacement(&max);

    GetWindowRect(rcWnd);

    // grab desktop size
    int cx, cy; 
    HDC dc = ::GetDC(NULL); 
    cx = GetDeviceCaps(dc,HORZRES) + 
        GetSystemMetrics(SM_CXBORDER); 
    cy = GetDeviceCaps(dc,VERTRES) +
        GetSystemMetrics(SM_CYBORDER); 
    ::ReleaseDC(0, dc); 

    // remove caption and border
    SetWindowLong(m_hWnd, 
        GWL_STYLE, 
        GetWindowLong(m_hWnd, GWL_STYLE) & (~(WS_CAPTION | WS_BORDER))); 

    // Put window on top and expand it to fill screen
    ::SetWindowPos(m_hWnd, 
        HWND_TOPMOST, 
        rcWnd.TopLeft().x + GetSystemMetrics(SM_CXBORDER) + 1, 
        rcWnd.TopLeft().y + GetSystemMetrics(SM_CYBORDER) + 1,
        cx + 1,
        cy + 1, 
        NULL); 

    m_FullScreen = true;
    m_GoingFullScreen = false;
}

///////////////////////////////////////////////////

void CmusikCubeFrame::KillFullScreen()
{
    // restore
    WINDOWPLACEMENT max;
    max.showCmd = SW_RESTORE;
    SetWindowPlacement(&max);

    // add caption bar and border
    SetWindowLong(m_hWnd, GWL_STYLE, 
        GetWindowLong(m_hWnd, GWL_STYLE) | (WS_CAPTION | WS_BORDER));

    // restore window "always on top" status...
    ::SetWindowPos(m_hWnd, 
        musikCube::g_Prefs->IsAlwaysOnTop() ? HWND_TOPMOST : HWND_NOTOPMOST, 
        musikCube::g_Prefs->GetDlgPos().x, 
        musikCube::g_Prefs->GetDlgPos().y, 
        musikCube::g_Prefs->GetDlgSize().cx,
        musikCube::g_Prefs->GetDlgSize().cy, 
        NULL); 

    // set position
    MoveWindow(CRect(musikCube::g_Prefs->GetDlgPos(), musikCube::g_Prefs->GetDlgSize()));

    // restore prior window state
    if (VerifyBarState(_T("musikProfile0.90")))
    {
        CSizingControlBar::GlobalLoadState(this, _T("musikProfile0.90"));
        LoadBarState(_T("musikProfile0.90"));
    }

    m_FullScreen = false;
}

///////////////////////////////////////////////////

// www.codeproject.com/dialog/dlgboxtricks.asp

void CmusikCubeFrame::OnViewFullscreen()
{
    if (!m_FullScreen)
        GoFullScreen();
    else
        KillFullScreen();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUpdateViewFullscreen(CCmdUI *pCmdUI)
{
    if (m_FullScreen)
        pCmdUI->SetCheck(TRUE);
    else
        pCmdUI->SetCheck(FALSE);
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnMenuAddNetRadio()
{
    CmusikAddNetstreamDlg dlg(musikCube::g_MainLibrary);
    if (dlg.DoModal() == IDOK)
    {
        CmusikPropTreeItem* pItem = m_SourcesView->GetFocusedItem();
        if (pItem && pItem->GetPlaylistType() == MUSIK_SOURCES_TYPE_NETSTREAMS)
        {
            this->OnSourcesNetRadio(NULL, NULL);
        }
    }
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnPlaySong(WPARAM wParam, LPARAM lParam)
{
    ATOM atom = (ATOM)wParam;

    wchar_t str_buf[256];
    ::GlobalGetAtomName(atom, str_buf, sizeof(str_buf));
    ::GlobalDeleteAtom(atom);

    PlayTrackFromExplorer(CString(str_buf));

    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnNowPlayingUpdated(WPARAM wParam, LPARAM lParam)
{
    // if now playing is visible then update it
    if (m_PlaylistView->GetCtrl()->GetPlaylist() == musikCube::g_Player->GetPlaylist())
    {
        UpdateFilters(UpdateAction::None, UpdateAction::Requery);
        m_PlaylistView->UpdatePlaylistInfo();
    }


    return 0L;
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnRatecurrentN1()
{
    musikCube::g_Library->SetSongRating(musikCube::g_Player->GetPlaying()->GetID(), -1);
    m_PlaylistView->GetCtrl()->ResyncItem(musikCube::g_Player->GetPlaying()->GetID());

    if (musikCube::g_Player->IsPlaying())
        musikCube::g_Player->GetPlaying()->SetRating(_T("-1"));
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnRatecurrentUn()
{
    musikCube::g_Library->SetSongRating(musikCube::g_Player->GetPlaying()->GetID(), 0);
    m_PlaylistView->GetCtrl()->ResyncItem(musikCube::g_Player->GetPlaying()->GetID());

    if (musikCube::g_Player->GetPlaying())
        musikCube::g_Player->GetPlaying()->SetRating(_T("0"));
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnRatecurrent1()
{
    musikCube::g_Library->SetSongRating(musikCube::g_Player->GetPlaying()->GetID(), 1);
    m_PlaylistView->GetCtrl()->ResyncItem(musikCube::g_Player->GetPlaying()->GetID());

    if (musikCube::g_Player->GetPlaying())
        musikCube::g_Player->GetPlaying()->SetRating(_T("1"));
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnRatecurrent2()
{
    musikCube::g_Library->SetSongRating(musikCube::g_Player->GetPlaying()->GetID(), 2);
    m_PlaylistView->GetCtrl()->ResyncItem(musikCube::g_Player->GetPlaying()->GetID());

    if (musikCube::g_Player->GetPlaying())
        musikCube::g_Player->GetPlaying()->SetRating(_T("2"));
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnRatecurrent3()
{
    musikCube::g_Library->SetSongRating(musikCube::g_Player->GetPlaying()->GetID(), 3);
    m_PlaylistView->GetCtrl()->ResyncItem(musikCube::g_Player->GetPlaying()->GetID());

    if (musikCube::g_Player->GetPlaying())
        musikCube::g_Player->GetPlaying()->SetRating(_T("3"));
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnRatecurrent4()
{
    musikCube::g_Library->SetSongRating(musikCube::g_Player->GetPlaying()->GetID(), 4);
    m_PlaylistView->GetCtrl()->ResyncItem(musikCube::g_Player->GetPlaying()->GetID());

    if (musikCube::g_Player->GetPlaying())
        musikCube::g_Player->GetPlaying()->SetRating(_T("4"));
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnRatecurrent5()
{
    musikCube::g_Library->SetSongRating(musikCube::g_Player->GetPlaying()->GetID(), 5);
    m_PlaylistView->GetCtrl()->ResyncItem(musikCube::g_Player->GetPlaying()->GetID());

    if (musikCube::g_Player->GetPlaying())
        musikCube::g_Player->GetPlaying()->SetRating(_T("5"));
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnSourcesCDAudio(WPARAM wParam, LPARAM lParam)
{
    if (!m_CDView)
    {
        m_CDView = new CmusikPlaylistView(this, NULL, NULL, NULL, NULL, NULL, false, PLAYLISTCTRL_CDRIPPER);
        m_CDView->Create(NULL, NULL, NULL, CRect(0, 0, 0, 0), this, 42452, NULL);

        CmusikPlaylistView* view = (CmusikPlaylistView*)m_CDView;
        CmusikPlaylistCDCtrl* ctrl = (CmusikPlaylistCDCtrl*)view->GetCtrl();
        ctrl->SetSongInfoArray(new musikCore::SongInfoArray());      
    }

    ShowSelectionBoxes(false);
    SwitchMainView((CView*)m_CDView);

    CmusikPlaylistView* view = (CmusikPlaylistView*)m_CDView;
    CmusikPlaylistCDCtrl* ctrl = (CmusikPlaylistCDCtrl*)view->GetCtrl();
    musikCore::SongInfoArray* target = ctrl->m_SongInfoArray;

    if ((int)wParam != ctrl->GetDiscDrive() || target->size() != CDSystem::GetDiscTrackCount((int)wParam))
    {
        ctrl->SetDiscDrive((int)wParam);
        CDSystem::FillWithBlanks(ctrl, target, (int)wParam);
    }

    return 0L;
}

///////////////////////////////////////////////////

CView* CmusikCubeFrame::SwitchMainView(CView* pNewView)
{
    if (pNewView == m_MainView)
        return pNewView;

    CView* oldview = m_MainView;

    // set the correct window attributes
    ::SetWindowLong(m_MainView->m_hWnd, GWL_ID, 0);
    ::SetWindowLong(pNewView->m_hWnd, GWL_ID, AFX_IDW_PANE_FIRST);

    // show new, hide old
    pNewView->ShowWindow(SW_SHOW);
    oldview->ShowWindow(SW_HIDE);

    m_MainView = pNewView;
        
    RecalcLayout();

    return m_MainView;
}

///////////////////////////////////////////////////

void CmusikCubeFrame::HideCDView()
{
    if (m_MainView == (CView*) m_CDView)
    {
        SwitchMainView((CView*) m_PlaylistView);
    }
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnSourcesQueryFreeDB(WPARAM wParam, LPARAM lParam)
{
    CmusikPlaylistView* pview = (CmusikPlaylistView*)m_CDView;
    CmusikPlaylistCDCtrl* pctrl = (CmusikPlaylistCDCtrl*)pview->GetCtrl();
    CDSystem::QueryToPlaylist(pctrl, pctrl->m_SongInfoArray, pctrl->GetDiscDrive());
    if (pctrl->m_SongInfoArray->size() == 0)
        CDSystem::FillWithBlanks(pctrl, pctrl->m_SongInfoArray, pctrl->GetDiscDrive());

    musikCube::g_Player->SetCDInfo(*pctrl->m_SongInfoArray, pctrl->GetDiscDrive());
    return 0L;
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnMenuExportLibrary()
{
    // create the open dialog object
    CmusikFileDialog savedlg(
        FALSE, 
        _T("db"), 
        _T("musik_u.db"),  
        OFN_ALLOWMULTISELECT | OFN_EXPLORER, 
        _T("musikCube database files |*.db||")
   );

    if (savedlg.DoModal() == IDOK)
    {
        CString fn(musikCube::g_Prefs->GetLibraryPath().c_str());
        fn = CmusikCubeApp::RelToAbs(fn);
        CopyFile(
            fn,
            savedlg.GetPathName(),
            true);
    }
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnFileAttempttosanitisetags()
{
    musikCore::Playlist pl;
    musikCube::g_Library->GetAllSongs(pl);
    
    musikCore::SongInfoArray* all = new musikCore::SongInfoArray();

    musikCube::g_Library->BeginTransaction();
    musikCore::SongInfo song;
    for (size_t i = 0; i < pl.size(); i++)
    {
        musikCube::g_Library->GetSongInfoFromID(
            pl.at(i).GetID(),
            song);
        all->push_back(song);
    }
    musikCube::g_Library->EndTransaction();

    if (all->size())
    {
        musikCore::BatchChgCase* params;

        params = new musikCore::BatchChgCase(
            musikCube::g_Library, 
            musikCube::g_Functor,
            all, 
            musikCore::MUSIK_CHGCASE_CLEAN);

        params->m_DeleteUpdatedTags = true;

        musikCore::BatchChgCaseTask* task = new musikCore::BatchChgCaseTask;
        task->m_Params = params;
        musikCube::g_Library->QueueTask(task);
    }
    else
        delete all; 
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnPlaybackRandom()
{
    if (musikCube::g_Player->GetPlaymode() & musikCore::MUSIK_PLAYER_PLAYMODE_RANDOM)
        musikCube::g_Player->ModifyPlaymode(NULL, musikCore::MUSIK_PLAYER_PLAYMODE_RANDOM);
    else
        musikCube::g_Player->ModifyPlaymode(musikCore::MUSIK_PLAYER_PLAYMODE_RANDOM, NULL);

    musikCube::g_Prefs->SetPlayerPlaymode(musikCube::g_Player->GetPlaymode());
    m_NowPlayingView->GetCtrl()->UpdateCheckboxes();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUpdatePlaybackRandom(CCmdUI *pCmdUI)
{
    if (musikCube::g_Player->GetPlaymode() & musikCore::MUSIK_PLAYER_PLAYMODE_RANDOM)
        pCmdUI->SetCheck(true);
    else
        pCmdUI->SetCheck(false);
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnSourcesRemovableDev(WPARAM wParam, LPARAM lParam)
{
    UpdateFilters(UpdateAction::Reset, UpdateAction::Reset);
    ShowSelectionBoxes(true);
    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnSourcesRefresh(WPARAM wParam, LPARAM lParam)
{
    m_SourcesView->RedrawWindow();
    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnSourcesReset(WPARAM wParam, LPARAM lParam)
{
    m_SourcesView->ReloadAllItems();
    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnSourcesNetRadio(WPARAM wParam, LPARAM lParam)
{
    ShowSelectionBoxes(false);

    musikCore::String query;
    //
    query.Format(
        _T(" SELECT songid, duration, filesize ")
        _T(" FROM songs ")
        _T(" WHERE format = 'net';"),
        MUSIK_LIBRARY_FORMAT_NET);
    //
    musikCore::Playlist* netStreams = new musikCore::Playlist();
    musikCube::g_MainLibrary->RawQuerySongs(query, *netStreams);
    //
    this->GivePlaylistToView(netStreams);
    this->UpdateFilters(UpdateAction::None, UpdateAction::Requery);

    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnSelectionBoxReset(WPARAM wParam, LPARAM lParam)
{
    UpdateFilters(UpdateAction::Reset, UpdateAction::Reset);
    return 0L;
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnTagsRemoveextrawhitespace()
{
    musikCore::Playlist pl;
    musikCube::g_Library->GetAllSongs(pl);

    musikCore::SongInfoArray* all = new musikCore::SongInfoArray();

    musikCube::g_Library->BeginTransaction();
    musikCore::SongInfo song;
    for (size_t i = 0; i < pl.size(); i++)
    {
        musikCube::g_Library->GetSongInfoFromID(
            pl.at(i).GetID(),
            song);
        all->push_back(song);
    }
    musikCube::g_Library->EndTransaction();

    if (all->size())
    {
        musikCore::BatchChgCase* params;

        params = new musikCore::BatchChgCase(
            musikCube::g_Library, 
            musikCube::g_Functor,
            all, 
            musikCore::MUSIK_CHGCASE_REMOVEWHITESPACE);

        params->m_DeleteUpdatedTags = true;

        musikCore::BatchChgCaseTask* task = new musikCore::BatchChgCaseTask;
        task->m_Params = params;
        musikCube::g_Library->QueueTask(task);
    }
    else
        delete all; 
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnTagsAuto()
{
    musikCore::Playlist pl;
    musikCube::g_Library->GetAllSongs(pl);

    musikCore::SongInfoArray* all = new musikCore::SongInfoArray();

    musikCube::g_Library->BeginTransaction();
    musikCore::SongInfo song;
    for (size_t i = 0; i < pl.size(); i++)
    {
        musikCube::g_Library->GetSongInfoFromID(
            pl.at(i).GetID(),
            song);
        all->push_back(song);
    }
    musikCube::g_Library->EndTransaction();

    if (all->size())
    {
        musikCore::BatchChgCase* params;

        params = new musikCore::BatchChgCase(
            musikCube::g_Library, 
            musikCube::g_Functor,
            all, 
            musikCore::MUSIK_CHGCASE_CAPITALIZE);

        params->m_DeleteUpdatedTags = true;

        musikCore::BatchChgCaseTask* task = new musikCore::BatchChgCaseTask;
        task->m_Params = params;
        musikCube::g_Library->QueueTask(task);
    }
    else
        delete all; 
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnFilePluginmanager()
{
    CmusikPluginManagerDlg dlg;
    dlg.DoModal();
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnSelectionBoxActivate(WPARAM wParam, LPARAM lParam)
{
    if (musikCube::g_Prefs->AddEntireToNP())
    {
        // we want to send the visible playlist to the player.
        musikCube::g_Player->SetPlaylist(m_PlaylistView->GetCtrl()->GetPlaylist());
        if (musikCube::g_Player->GetPlaylist()->GetCount())
            musikCube::g_Player->Play(0);
    }
    else
    {
        musikCore::Playlist* pl = m_PlaylistView->GetCtrl()->GetPlaylist();
        int size = musikCube::g_Player->GetPlaylist()->GetCount();
        for (size_t i = 0; i < pl->size(); i++)
            musikCube::g_Player->GetPlaylist()->Add(pl->at(i));

        if (!musikCube::g_Player->IsPlaying())
            musikCube::g_Player->Play(size);
    }

    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnSourcesUnsel(WPARAM wParam, LPARAM lParam)
{
    m_SourcesView->DeselectAllItems();
    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnTaskbarCreated(WPARAM wParam, LPARAM lParam)
{
    if (m_TrayIconVisible)
    {
        InitTrayIcon();
        ShowTrayIcon();
    }
    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnShowCube(WPARAM wParam, LPARAM lParam)
{
    if (m_IsMinimized )
    {
        if (musikCube::g_Prefs->MinimizeToTray() || musikCube::g_Prefs->GetCloseToSystemTray())
            RestoreFromTray();
        else
            ShowWindow(SW_RESTORE);
    }

    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnHideCube(WPARAM wParam, LPARAM lParam)
{
    if (!m_IsMinimized)
    {
        if (musikCube::g_Prefs->MinimizeToTray() || musikCube::g_Prefs->GetCloseToSystemTray())
        {       
            SendToTray();
            ShowWindow(SW_HIDE);
        }
        else
            ShowWindow(SW_MINIMIZE);
    }

    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikCubeFrame::OnUpdateVolume(WPARAM wParam, LPARAM lParam)
{
    m_NowPlayingView->GetCtrl()->UpdateVolume();
    return 0L;
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnHelpOnlinehelp()
{
    ShellExecute(
        NULL,
        _T("OPEN"),
        _T("http://musikcube.com/help"),
        NULL,
        NULL,
        SW_SHOWNORMAL
   );
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnSourcesShowdevices()
{
    m_SourcesView->ToggleDevicesVisible();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUpdateSourcesShowdevices(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck((BOOL)musikCube::g_Prefs->IsDevicesVisible());
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnSourcesShowplaylists()
{
    m_SourcesView->ToggleStdPlaylistsVisible();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUpdateSourcesShowplaylists(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck((BOOL)musikCube::g_Prefs->IsPlaylistsVisible());
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnSourcesShowdynamicplaylists()
{
    m_SourcesView->ToggleDynPlaylistsVisible();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUpdateSourcesShowdynamicplaylists(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck((BOOL)musikCube::g_Prefs->IsDynPlaylistsVisible());
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnSourcesShowplugins()
{
    m_SourcesView->TogglePluginsVisible();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUpdateSourcesShowplugins(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck((BOOL)musikCube::g_Prefs->IsPluginsVisible());
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnFileThemeeditor()
{
    CmusikThemeEditorDlg dlg;
    dlg.DoModal();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnViewSelectionboxes()
{
    musikCube::g_Prefs->SetShowSelectionBoxes( ! musikCube::g_Prefs->ShowSelectionBoxes());
    this->ShowSelectionBoxes(musikCube::g_Prefs->ShowSelectionBoxes());
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnUpdateViewSelectionboxes(CCmdUI *pCmdUI)
{
    CmusikPropTreeItem* pItem = m_SourcesView->GetFocusedItem();

    if (!pItem)
    {
        pCmdUI->SetCheck(musikCube::g_Prefs->ShowSelectionBoxes());
        return;
    }

    switch(pItem->GetPlaylistType())
    {
    case musikCore::MUSIK_PLAYLIST_TYPE_REMOVABLE:
    case musikCore::MUSIK_PLAYLIST_TYPE_LIBRARY:
    case musikCore::MUSIK_PLAYLIST_TYPE_STANDARD:
    case musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC:
        pCmdUI->SetCheck(musikCube::g_Prefs->ShowSelectionBoxes());
        pCmdUI->Enable(1);
        break;

    default:
        pCmdUI->SetCheck(0);
        pCmdUI->Enable(0);
        break;
    }
}

///////////////////////////////////////////////////

CmusikSelectionCtrl* CmusikCubeFrame::GetParentSelectionCtrl()
{
    if (this->m_AllFilterViews.size())
    {
        return this->m_AllFilterViews[0]->GetCtrl();
    }

    return NULL;
}

///////////////////////////////////////////////////

musikCore::String CmusikCubeFrame::GetPlaylistSelectionFilter()
{
    // Loops through all the SelectionCtrls that are currently being used as filters and
    // concatenates their criteria: (ARTIST IN(...)) AND (ALBUM IN(...)) ... etc

    musikCore::String selectedFilter = L"";

    size_t selectionCtrlCount = this->m_ActiveFilterViews.size();
    //
    if (selectionCtrlCount)
    {
        for (size_t i = 0; i < selectionCtrlCount; i++)
        {
            selectedFilter += _T(" (");
            selectedFilter += this->m_ActiveFilterViews[i]->GetSelectedItemsQuery();
            selectedFilter += _T(")");

            if (i < (selectionCtrlCount - 1)) { selectedFilter += L" AND "; }
        }
    }
    else
    {
        return L"";
    }

    selectedFilter += L" ";

    return selectedFilter;
}

///////////////////////////////////////////////////

musikCore::String CmusikCubeFrame::GetPlaylistQuery()
{
    musikCore::String query = L" SELECT songid, duration, filesize FROM songs ";

    // append selection box filter
    //
    musikCore::String selectionFilter = this->GetPlaylistSelectionFilter();
    //
    if (selectionFilter != L"")
    {
        query += L"WHERE ";
        query += selectionFilter;
        query += L" ";
    }

    // append playlist filter, if any (i.e. Standard or Dynamic Playlist selected)
    //
    musikCore::String playlistFilter = this->GetPlaylistInfoAsFilter();
    //
    if (playlistFilter != L"")
    {
        if (selectionFilter != L"")
        {
            query += L" AND ";
        }
        else
        {
            query += L" WHERE ";
        }

        query += playlistFilter;
    }

    query += musikCore::Library::GetOrder(musikCore::MUSIK_LIBRARY_TYPE_ARTIST, true, true);

    return query;
}

///////////////////////////////////////////////////

musikCore::String CmusikCubeFrame::GetPlaylistInfoAsFilter()
{
    musikCore::String filter;
    musikCore::String playlistQuery = this->m_PlaylistInfo.GetQuery();
    //
    if (playlistQuery != L"")
    {
        // the query contained within the PlaylistInfo will be the full query,
        // so we need to project the "songid" out of it.
        filter += " songid IN( SELECT songid FROM (";
        filter += playlistQuery;
        filter += "))";
    }

    return filter;
}

///////////////////////////////////////////////////

musikCore::String CmusikCubeFrame::GetSelectionCtrlFilter(CmusikSelectionCtrl* selectionCtrl)
{
    // The selection CTRL needs a filterCriteria. Examples:
    //    ARTIST IN(...) AND ALBUM IN(...)

    musikCore::String filter;

    // Get a concatenation of all selection control filters that we must listen to.
    //
    int start = this->GetSelectionCtrlOrder(selectionCtrl);
    if (start < 0) start = 0;       // not an active filter; must honor all active filters
    //
    int end = this->m_ActiveFilterViews.size();
    //
    if (end - start > 0)
    {
        CmusikSelectionCtrl* currentControl;
        for (int i = start; i < end - 1; i++)
        {
            currentControl = this->m_ActiveFilterViews[i];
            //
            filter += L"(";
            filter += currentControl->GetSelectedItemsQuery();
            filter += L") AND ";
        }

        currentControl = this->m_ActiveFilterViews[i];
            filter += L"(";
            filter += currentControl->GetSelectedItemsQuery();
            filter += L")";
    }

    // append playlist filter, if any (i.e. Standard or Dynamic Playlist selected)
    //
    musikCore::String playlistFilter = this->GetPlaylistInfoAsFilter();
    //
    if (playlistFilter != L"")
    {
        if (filter != L"")
        {
            filter += L" AND ";
        }

        filter += playlistFilter;
    }

    return filter;
}

///////////////////////////////////////////////////

void CmusikCubeFrame::UpdateSelectionCtrlOrder(CmusikSelectionCtrl* newSelection)
{
    int selectedOrder = this->GetSelectionCtrlOrder(newSelection);
    //
    if (selectedOrder > -1)
    {
        int invalidatedCount = this->m_ActiveFilterViews.size() - selectedOrder - 1;
        while (invalidatedCount)
        {
            this->m_ActiveFilterViews.pop_back();
            invalidatedCount--;
        }
    }
    else
    {
        this->m_ActiveFilterViews.push_back(newSelection);
    }
}

///////////////////////////////////////////////////

int CmusikCubeFrame::GetSelectionCtrlOrder(CmusikSelectionCtrl* selectionCtrl)
{
    for (size_t i = 0; i < this->m_ActiveFilterViews.size(); i++)
    {
        if (this->m_ActiveFilterViews[i] == selectionCtrl)
        {
            return i;
        }
    }

    return -1;
}

///////////////////////////////////////////////////

void CmusikCubeFrame::ClearSelectionCtrls()
{
    this->m_ActiveFilterViews.clear();
    for (size_t i = 0; i < this->m_AllFilterViews.size(); i++)
    {
        m_AllFilterViews[i]->GetCtrl()->Clear();
    }
}

///////////////////////////////////////////////////

void CmusikCubeFrame::ResetPlaylistCtrl()
{
    musikCore::Playlist* resetPlaylist = new musikCore::Playlist();
    this->QueryPlaylist(*resetPlaylist);
    this->GivePlaylistToView(resetPlaylist);
    this->m_PlaylistView->GetCtrl()->Refresh();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::QueryPlaylistCtrl()
{
    this->ResetPlaylistCtrl();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::RequeryPlaylistCtrl()
{
    this->m_PlaylistView->GetCtrl()->Refresh();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::ClearPlaylistCtrl()
{
    musikCore::Playlist* resetPlaylist = new musikCore::Playlist();
    this->GivePlaylistToView(resetPlaylist);
    this->m_PlaylistView->GetCtrl()->Refresh();
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnMenuChangeLibrary()
{
    // create the open dialog object
    CmusikFileDialog opendlg(
        TRUE, 
        _T("db"), 
        NULL, OFN_ALLOWMULTISELECT | OFN_EXPLORER, 
        _T("musikCore database files |*.db||"));

    if (opendlg.DoModal() == IDOK)
    {
        if (musikCube::g_Player->IsPlaying())
        {
            musikCube::g_Player->Stop();
        }

        musikCube::g_Player->GetPlaylist()->Clear();
        KillTasks(false, true);

        musikCore::String p = CmusikCubeApp::AbsToRel(opendlg.GetPathName());
        musikCube::g_Prefs->SetLibraryPath(p);
        musikCube::g_Library->SetFilename(opendlg.GetPathName().GetBuffer());

        this->UpdateFilters(UpdateAction::Reset, UpdateAction::Reset);
    }
}

///////////////////////////////////////////////////

void CmusikCubeFrame::OnHelpCheckForUpdates()
{
    ShellExecute(
        NULL,
        _T("OPEN"),
        _T("http://www.musikcube.com/updatecheck?p=musikcube;v=1.0.0"),
        NULL,
        NULL,
        SW_SHOWNORMAL);
}

///////////////////////////////////////////////////
