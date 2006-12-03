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

#include "musikEditInPlace.h"
#include "musikDockBar.h"
#include "musikSourcesDropTarget.h"
#include "3rdparty/musikPropTree.h"
#include "musikRipDlg.h"

#include <musikCore.h>
#include <vector>

///////////////////////////////////////////////////

typedef std::vector<CmusikPropTreeItem*> CmusikSourcesItemPtrArray;

///////////////////////////////////////////////////

class CmusikSourcesCtrl;

///////////////////////////////////////////////////

#define IDC_SOURCES 1000

///////////////////////////////////////////////////

enum
{
    MUSIK_SOURCES_EDITINPLACE_RENAME = 493,
    MUSIK_SOURCES_EDITINPLACE_QUICKSEARCH,
    MUSIK_SOURCES_EDITINPLACE_NEWSTDPLAYLIST,
    MUSIK_SOURCES_EDITINPLACE_NEWDYNPLAYLIST
};

///////////////////////////////////////////////////

// CmusikSourcesCtrl

///////////////////////////////////////////////////

#define IDC_EDITINPLACE 1538

///////////////////////////////////////////////////

class CmusikSourcesCtrl : public CmusikDockBar
{
public:     friend class                CmusikSourcesDropTarget;
public:     friend class                CmusikCubeFrame;
public:     static void                 LoadBitmaps();


    // ctor, dtor
public:     /*ctor*/                    CmusikSourcesCtrl(CFrameWnd* parent, UINT dropid, UINT pldropid_r, UINT sbdropid_r);
public:     /*dtor*/                    virtual ~CmusikSourcesCtrl();


    // CmusikDockBar overrides
protected:  virtual void                OnOptions(){}
protected:  CWnd*                       GetMainWindow() { return (CWnd*) this->m_PropTree; }


    // Control's Instance Variables
private:    CFrameWnd*                  m_Parent;
private:    CmusikPropTreeItem*         m_RightClick;
private:    CmusikPropTree*             m_PropTree;
private:    CFont                       m_DefaultFont;
private:    UINT                        m_DropID;
private:    CmusikEditInPlace           m_EditInPlace;


    // Browse
    // (methods)
private:    void                        LoadBrowseItems();
    // (variables)
private:    CmusikPropTreeItem*         m_BrowseRootItem;
private:    CmusikSourcesItemPtrArray   m_BrowseItems;


    // Library
    // (events)
private:    afx_msg void                OnMenuLibraryShowAllSongs();
    // (methods)
private:    bool                        DropAddToLibrary(musikCore::Library* pLibrary, musikCore::StringArray* pFiles);
public:     void                        FocusLibrary();


    // Now Playing
    // (events)
private:    afx_msg                     void OnSourcesNowPlayingClear();
private:    afx_msg                     void OnMenuShuffleNowPlaying();
    // (methods)
public:     void                        FocusNowPlaying();


    // Standard Playlists
    // (methods)
private:    void                        CreateNewStdPlaylist();
private:    void                        RemoveStdPlaylist(CmusikPropTreeItem* pItem);
private:    void                        LoadStdPlaylistItems();
public:     void                        ToggleStdPlaylistsVisible();
    // (variables)
private:    CmusikPropTreeItem*         m_StdPlaylistRootItem;
private:    CmusikSourcesItemPtrArray   m_StdPlaylistItems;


    // Dynamic Playlists
    // (events)
private:    afx_msg void                OnMenuEditQuery();
private:    afx_msg void                OnMenuCreateStockDynamicPlaylists();
    // (methods)
private:    void                        CreateNewDynPlaylist();
private:    void                        RemoveDynPlaylist(CmusikPropTreeItem* pItem);
private:    void                        LoadDynPlaylistItems();
public:     void                        ToggleDynPlaylistsVisible();
    // (variables)
private:    CmusikPropTreeItem*         m_DynPlaylistRootItem;
private:    CmusikSourcesItemPtrArray   m_DynPlaylistItems;


    // Collected
    // (events)
public:     void                        LoadCollectedLibrary();
public:     void                        TellUserAboutCollected();
    // (variables)
private:    CmusikPropTreeItem*         m_CollectedItem;


    // Devices
    // (events)
private:    void                        OnTimer(UINT nIDEvent);
private:    afx_msg void                OnMenuQueryFreeDB();
private:    afx_msg void                OnMenuRefreshCD();
private:    afx_msg void                OnMenuDeviceUnloadLibrary();
private:    afx_msg void                OnMenuRipSelectedCD();
private:    afx_msg void                OnMenuSynchronizeDevice();
public:     afx_msg void                OnMenuSynchronizedPaths();
            // (methods)
private:    bool                        DropOnRemovableDevice(musikCore::StringArray* pFiles, CmusikPropTreeItem* pItem);
private:    bool                        LoadRemovableLibrary(CmusikPropTreeItem* ptrItem);
private:    int                         GetDriveCount();
private:    void                        DeviceTimerTick();
public:     void                        RefreshDevices();
public:     static void                 FindAttachedDevices(musikCore::StringArray& target);
public:     musikCore::String           GetDeviceVolumeDescription(const musikCore::String& drive_path);
public:     void                        ToggleDevicesVisible();
    // (variables)
private:    CmusikPropTreeItem*         m_DevicesRootItem;
private:    CmusikSourcesItemPtrArray   m_DeviceItems;
private:    musikCore::StringArray      m_VisibleDrives;
private:    UINT                        m_DevicesTimerID;


    // Drag and Drop
    // (events)
private:    void                        OnDropFiles(HDROP hDropInfo, bool right_button = false);
private:    DROPEFFECT                  OnDragOver(CPoint point);
private:    void                        OnDragLeave();
private:    void                        DoDrag(CmusikPropTreeItem* pItem);
    // (methods)
private:    bool                        DropCopyFilesToDevice(musikCore::StringArray* pFiles, CmusikPropTreeItem* pItem);
private:    bool                        DropAddNowPlaying(musikCore::StringArray* pFiles);
private:    bool                        DropAddToDevice(musikCore::StringArray* pFiles, CmusikPropTreeItem* pItem);
private:    bool                        DropAddToCollected(musikCore::StringArray* pFiles);
    // (variables)
private:    musikCore::PlaylistInfo*    m_DragAndDropInfo;
private:    CmusikSourcesDropTarget*    m_DropTarget;


    // Item Editing
    //  (events)
private:    afx_msg LRESULT             OnItemEditCommit(WPARAM wParam, LPARAM lParam);
private:    afx_msg LRESULT             OnItemEditCancel(WPARAM wParam, LPARAM lParam);
private:    afx_msg LRESULT             OnItemEditChange(WPARAM wParam, LPARAM lParam);
    //  (methods)
private:    void                        EditItemInPlace(CmusikPropTreeItem* pItem, int edit_type, const CString text);
public:     void                        RenameItem(CmusikPropTreeItem* pItem = NULL, int mode = MUSIK_SOURCES_EDITINPLACE_RENAME, CPoint loc = CPoint(-1, -1));
    //  (variables)
private:    CmusikPropTreeItem*         m_EditInPlaceItem;
private:    musikCore::String           m_PreEditInPlaceText;

    // Item Manipulation
    //  (events)
private:    afx_msg void                OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
private:    afx_msg void                OnItemDragged(NMHDR* pNotifyStruct, LRESULT* plResult);

private:    afx_msg void                OnMenuMoveItemUp();
private:    afx_msg void                OnMenuMoveItemDown();
private:    afx_msg void                OnMenuRenameItem();
private:    afx_msg void                OnMenuDeleteItem();
    //  (methods)
private:    void                        MoveSelectedItemUp();
private:    void                        MoveSelectedItemDown();
public:     void                        DeleteSelectedItem();
public:     void                        FocusItem(CmusikPropTreeItem* pItem);       // WHY IS THIS USED ONLY ONCE?!
public:     void                        DeselectAllItems(bool redraw = true);
public:     void                        ReloadAllItems();
public:     CmusikPropTreeItem*         GetFocusedItem() { return this->m_PropTree->GetFocusedItem(); }
private:    void                        NotifyParentItemChanged(CmusikPropTreeItem* pItem);

    // Searching
    // (methods)
private:    void                        FinishQuickSearch();
public:     void                        FocusQuickSearch();
public:     void                        EnableQuickSearch();
private:    void                        SearchTimerTick();
    // (variables)
private:    CString                     m_QuickStr;
private:    CmusikPropTreeItem*         m_SearchItem;
private:    UINT                        m_SearchTimerID;

    // Plugins
    // (events)
private:    afx_msg void                OnMenuPluginExecute();
private:    afx_msg void                OnMenuPluginConfigure();
private:    afx_msg void                OnMenuPluginStop();
private:    afx_msg void                OnMenuPluginAbout();
    // (methods)
private:    void                        LoadPluginItems();
public:     void                        TogglePluginsVisible();
    // (variables)
private:    CmusikSourcesItemPtrArray   m_PluginItems;
private:    CmusikPropTreeItem*         m_PluginsRootItem;


    // Keyboard Events
private:    afx_msg void                OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);


    // misc menu events
private:    afx_msg void                OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
private:    afx_msg                     void OnMenuCopyFilesTo();
private:    afx_msg                     void OnMenuMoveFilesTo();


    // window events
private:    afx_msg int                 OnCreate(LPCREATESTRUCT lpCreateStruct);


    // misc
private:    void                        DeleteLastCreatedPlaylist(int type);
private:    musikCore::String           SourcesFileOp(DWORD mode, const musikCore::StringArray& files, musikCore::String default_path = _T(""));
public:     void                        GetSelFilenames(CmusikPropTreeItem* pItem, musikCore::String& files);
public:     void                        GetSelFilenames(CmusikPropTreeItem* pItem, musikCore::StringArray& files);
private:    void                        ShowDockBarMenu();

    // macros
private:    DECLARE_DYNAMIC(CmusikSourcesCtrl)
private:    DECLARE_MESSAGE_MAP()
public:
};

///////////////////////////////////////////////////


