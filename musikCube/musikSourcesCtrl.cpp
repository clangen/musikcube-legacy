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

#include "musikCube.h"
#include "musikCubeFrame.h"
#include "musikSourcesCtrl.h"
#include "musikDynPlaylistDlg.h"
#include "3rdparty/CShellFileOp.h"

#include <Winbase.h>
#include <basscd.h>
#include <musikFilename.h>
#include ".\musiksourcesctrl.h"

///////////////////////////////////////////////////

#pragma warning (disable : 4312)    // conversion from int to void* of greater size

///////////////////////////////////////////////////

using namespace Gdiplus;
static Gdiplus::Bitmap* m_pLibOff = NULL;
static Gdiplus::Bitmap* m_pLibOn = NULL;
static Gdiplus::Bitmap* m_pNPOff = NULL;
static Gdiplus::Bitmap* m_pNPOn = NULL;
static Gdiplus::Bitmap* m_pQSOff = NULL;
static Gdiplus::Bitmap* m_pQSOn = NULL;
static Gdiplus::Bitmap* m_pCDOff = NULL;
static Gdiplus::Bitmap* m_pCDOn = NULL;
static Gdiplus::Bitmap* m_pPLOff = NULL;
static Gdiplus::Bitmap* m_pPLOn = NULL;
static Gdiplus::Bitmap* m_pDPOff = NULL;
static Gdiplus::Bitmap* m_pDPOn = NULL;
static Gdiplus::Bitmap* m_pRDOff = NULL;
static Gdiplus::Bitmap* m_pRDOn = NULL;
static Gdiplus::Bitmap* m_pNSOff = NULL;
static Gdiplus::Bitmap* m_pNSOn = NULL;
static Gdiplus::Bitmap* m_pPlugOff = NULL;
static Gdiplus::Bitmap* m_pPlugOn = NULL;
static Gdiplus::Bitmap* m_pColOff = NULL;
static Gdiplus::Bitmap* m_pColOn = NULL;

///////////////////////////////////////////////////

// CmusikSourcesCtrl

///////////////////////////////////////////////////

CmusikSourcesCtrl::CmusikSourcesCtrl(CFrameWnd* parent, UINT dropid, UINT pldropid_r, UINT sbdropid_r)
    : CmusikDockBar()
{
    m_DropTarget                = new CmusikSourcesDropTarget(this, dropid, pldropid_r, sbdropid_r);

    m_Parent                    = parent;
    this->m_DropID              = dropid;
    m_BrowseRootItem            = NULL;
    m_StdPlaylistRootItem       = NULL;
    m_DynPlaylistRootItem       = NULL;
    m_DevicesRootItem           = NULL;
    m_PluginsRootItem           = NULL;
    m_CollectedItem             = NULL;
    m_DevicesTimerID            = NULL;
    m_SearchTimerID             = NULL;
    m_DragAndDropInfo           = NULL;

    m_PropTree = new CmusikPropTree(dropid);
}

///////////////////////////////////////////////////

CmusikSourcesCtrl::~CmusikSourcesCtrl()
{
    m_DropTarget->Revoke();
    delete m_DropTarget;

    delete m_PropTree;
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::RefreshDevices()
{
    if (this->m_DevicesRootItem != NULL)
    {
        musikCore::PlaylistInfo info;

        musikCore::StringArray hidden = musikCube::g_Prefs->GetHiddenDevices();
        musikCore::String src_str, dev_str;

        //
        // get a list of the drives currently on the system
        //
        musikCore::StringArray curr_drives;
        FindAttachedDevices(curr_drives);

        //
        // remove devices that no longer exist
        //
        for (size_t i = 0; i < m_VisibleDrives.size(); i++)
        {
            bool is_active_drive = false;
            for (size_t j = 0; j < curr_drives.size(); j++)
            {
                if (m_VisibleDrives[i] == curr_drives[j])
                {
                    is_active_drive = true;
                    break;
                }
            }

            // is the drive hidden?
            bool is_hidden_device = false;
            for (size_t j = 0; j < hidden.size(); j++)
            {
                if(m_VisibleDrives[i] == hidden[j])
                {
                    is_hidden_device = true;
                    break;
                }
            }

            if (!is_active_drive || is_hidden_device)
            {
                CmusikPropTreeItem* pItem = m_DeviceItems[i];

                if (pItem == this->m_PropTree->GetFocusedItem())
                {
                    this->m_PropTree->SetFocusedItem(NULL);
                }

                pItem->Detach();
                delete pItem;
                m_DeviceItems.erase(m_DeviceItems.begin() + i);

                m_VisibleDrives.erase(m_VisibleDrives.begin() + i);
                --i;
            }
        }
            

        //
        // now we add drives that don't exist
        //
        for (size_t i = 0; i < curr_drives.size(); i++)
        {
            std::wstring curr_drive = curr_drives[i];

            // is the drive already visible?
            bool already_visible = false;
            for(size_t j = 0; j < m_VisibleDrives.size(); j++)
            {
                if(m_VisibleDrives[j] == curr_drive)
                {
                    already_visible = true;
                    break;
                }
            }

            // is the drive hidden?
            bool is_hidden_device = false;
            for (size_t j = 0; j < hidden.size(); j++)
            {
                if(hidden[j] == curr_drive)
                {
                    is_hidden_device = true;
                    break;
                }
            }

            // ... if its either, we don't want to display it
            if (already_visible || is_hidden_device)
            {
                continue;
            }

            UINT type = GetDriveType(curr_drives[i].c_str());
            
            switch(type)
            {
            case DRIVE_REMOVABLE:
            case DRIVE_FIXED:
            case DRIVE_REMOTE:
                {
                    musikCore::String driveName = GetDeviceVolumeDescription(curr_drives[i]);

                    info = musikCore::PlaylistInfo(driveName, musikCore::MUSIK_PLAYLIST_TYPE_REMOVABLE, -1);

                    CmusikPropTreeItem *removable = 
                        this->m_PropTree->InsertItem(new CmusikPropTreeItem(), m_DevicesRootItem);

                    removable->SetPlaylistInfo(info);
                    removable->SetBitmaps(m_pRDOff, m_pRDOn);

                    m_VisibleDrives.push_back(curr_drive);
                    m_DeviceItems.push_back(removable);
                }
                break;

            case DRIVE_CDROM:
                {
                    int curr_drive_id = curr_drives[i][0];

                    int count = 0;
                    const char* pStr = BASS_CD_GetDriveDescription(count);
                    while (pStr != 0)
                    {
                        int id = (int)BASS_CD_GetDriveLetter(count) + (int)'A';

                        if (id == curr_drive_id)
                        {
                            curr_drive_id = count;
                            break;
                        }

                        count++;
                        pStr = BASS_CD_GetDriveDescription(count);
                    }

                    musikCore::String driveName = GetDeviceVolumeDescription(curr_drives[i]);

                    info = musikCore::PlaylistInfo(driveName, musikCore::MUSIK_PLAYLIST_TYPE_CDPLAYER, curr_drive_id);

                    CmusikPropTreeItem *cdplayer = 
                        this->m_PropTree->InsertItem(new CmusikPropTreeItem(), m_DevicesRootItem);

                    cdplayer->SetPlaylistInfo(info);
                    cdplayer->SetBitmaps(m_pCDOff, m_pCDOn);

                    m_VisibleDrives.push_back(curr_drive);
                    m_DeviceItems.push_back(cdplayer);
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
    int dwSize = (int)GetLogicalDriveStrings(1024, logdev);

    musikCore::StringArray drives;
    int count = 0;
    for (int i = 0; i < dwSize; i++)
    {
        if (logdev[i] == (wchar_t)'\0')
            count++;
    }

    return count;
}

///////////////////////////////////////////////////

musikCore::String CmusikSourcesCtrl::GetDeviceVolumeDescription(const musikCore::String& drive_name)
{
    wchar_t volName[128] = _T("");

    GetVolumeInformation(
        drive_name.c_str(),
        volName,    // volume name
        128,        // volume name buffer size
        NULL,        // serial number
        NULL,
        NULL,
        NULL,
        NULL
   );

    musikCore::String drivestr = volName;

    if (drivestr == _T(""))
    {
        drivestr = _T("unnamed");
    }

    drivestr += _T(" (");
    drivestr += drive_name;
    drivestr += _T(")");

    drivestr.Replace(_T("\\"), _T(""));

    return drivestr;
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::FindAttachedDevices(musikCore::StringArray& target)
{
    target.clear();

    wchar_t logdev[1024];
    int dwSize = (int)GetLogicalDriveStrings(1024, logdev);

    wchar_t buffer[32];
    musikCore::StringArray drives;
    int at = 0;
    int count = 0;
    for (int i = 0; i < dwSize; i++)
    {
        wchar_t cur = logdev[i];
        buffer[at] = cur;
        
        if (cur == (wchar_t)'\0')
        {   
            target.push_back(buffer);
            count++;
            at = -1;
        }
        at++;
    }
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnMenuCreateStockDynamicPlaylists()
{
    musikCube::g_Library->InitDefaultDynPlaylists();
    this->LoadDynPlaylistItems();
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnItemDragged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
    LPNMPROPTREE pNMPropTree = (LPNMPROPTREE) pNotifyStruct;
    *plResult = 0;

    if (pNMPropTree->pItem)
    {
        switch(pNMPropTree->pItem->GetPlaylistType())
        {
        case MUSIK_SOURCES_TYPE_NEWSTDPLAYLIST:
        case MUSIK_SOURCES_TYPE_QUICKSEARCH:
        case MUSIK_SOURCES_TYPE_NEWDYNPLAYLIST:
        case musikCore::MUSIK_PLAYLIST_TYPE_CDPLAYER:
            return;
            break;
        }

        m_DragAndDropInfo = new musikCore::PlaylistInfo(pNMPropTree->pItem->GetPlaylistInfo());
        //
        DoDrag(pNMPropTree->pItem);
        //
        delete m_DragAndDropInfo;
        m_DragAndDropInfo = NULL;
    }
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
    LPNMPROPTREE pNMPropTree = (LPNMPROPTREE)pNotifyStruct;
      *plResult = 0;  

    this->NotifyParentItemChanged(pNMPropTree->pItem);
}

///////////////////////////////////////////////////

bool CmusikSourcesCtrl::LoadRemovableLibrary(CmusikPropTreeItem* ptrItem)
{
    musikCore::String drive_path;
    musikCore::String drive_letter = ptrItem->GetPlaylistInfo().GetName();

    int pos = drive_letter.find(_T("(")) + 1;
    int len = drive_letter.size();
    drive_path = drive_letter.Right(len - pos);
    drive_path = drive_path.Left(drive_path.length() - 1);
    drive_path += "\\";

    musikCore::String fn = drive_path + _T(".musikproject\\musik_removable_u.db");
    musikCore::RemLibrary* pNewLib = NULL;

    if (musikCore::Filename::FileExists(fn))
    {
        pNewLib = new musikCore::RemLibrary(fn);
        pNewLib->SetRemPath(drive_path, true);
    }

    else
    {
        int res = MessageBox(
            _T("You have not yet built a library for this drive, would you like to do so now?"),
            _T("musikCube"),
            MB_YESNO | MB_ICONQUESTION);

        if (res == IDYES)
        {
            pNewLib = new musikCore::RemLibrary(fn);
            pNewLib->AddPath(drive_path);
            pNewLib->SetRemPath(drive_path, false);

            musikCore::BatchAdd* params = new musikCore::BatchAdd(
                NULL,
                drive_path + _T("*.*"),
                NULL,
                pNewLib,
                NULL,
                musikCube::g_Functor);

            musikCore::BatchAddTask* ptask = new musikCore::BatchAddTask();
            ptask->m_Params = params;

            pNewLib->QueueTask(ptask);
        }
        else
        {
            this->DeselectAllItems();
            return false;
        }
    }

    if (pNewLib)
    {
        if (ptrItem->m_Library)
            delete ptrItem->m_Library;

        ptrItem->m_Library = pNewLib;
    }

    return true;
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::ShowDockBarMenu()
{
    if (this->m_EditInPlace.IsVisible())
    {
        this->m_EditInPlace.Cancel();
    }

    CPoint pos;
    ::GetCursorPos(&pos);
    ScreenToClient(&pos);
    
    CmusikPropTreeItem* pItem = this->m_PropTree->FindItem(pos);

    if (!pItem || pItem->IsRootLevel())
    {
        if (pItem != this->m_DynPlaylistRootItem)
        {
            return;
        }
    }

    int type = musikCore::MUSIK_PLAYLIST_TYPE_UNKNOWN;
    type = pItem->GetPlaylistType();

    CMenu main_menu;
    CMenu* popup_menu;

    main_menu.LoadMenu(IDR_SOURCES_MENU);
    popup_menu = main_menu.GetSubMenu(0);

    // shuffle / clear now playing
    if (type == musikCore::MUSIK_PLAYLIST_TYPE_NOWPLAYING)
    {
        popup_menu->EnableMenuItem(ID_SOURCES_SHUFFLEPLAYLIST, MF_ENABLED);
        popup_menu->EnableMenuItem(ID_SOURCES_CLEAR, MF_ENABLED);
    }
    else 
    {    
        popup_menu->RemoveMenu(ID_SOURCES_SHUFFLEPLAYLIST, MF_BYCOMMAND);
        popup_menu->RemoveMenu(ID_SOURCES_CLEAR, MF_BYCOMMAND);
    }

    // rename, delete
    if (type == musikCore::MUSIK_PLAYLIST_TYPE_STANDARD || 
         type == musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC)
    {
        popup_menu->EnableMenuItem(ID_SOURCES_RENAME, MF_ENABLED);
        popup_menu->EnableMenuItem(ID_SOURCES_DELETE, MF_ENABLED);
        popup_menu->EnableMenuItem(ID_SOURCES_MOVEUP, MF_ENABLED);
        popup_menu->EnableMenuItem(ID_SOURCES_MOVEDOWN, MF_ENABLED);
    }
    else
    {
        popup_menu->RemoveMenu(ID_SOURCES_RENAME, MF_BYCOMMAND);
        popup_menu->RemoveMenu(ID_SOURCES_DELETE, MF_BYCOMMAND);
        popup_menu->RemoveMenu(ID_SOURCES_MOVEUP, MF_BYCOMMAND);
        popup_menu->RemoveMenu(ID_SOURCES_MOVEDOWN, MF_BYCOMMAND);
    }

    // copy to / move to
    if (type == musikCore::MUSIK_PLAYLIST_TYPE_STANDARD || 
         type == musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC || 
         type == musikCore::MUSIK_PLAYLIST_TYPE_NOWPLAYING)
    {
        popup_menu->EnableMenuItem(ID_SOURCES_COPYTO, MF_ENABLED);
        popup_menu->EnableMenuItem(ID_SOURCES_MOVEFILESTO, MF_ENABLED);
    }
    else
    {
        popup_menu->RemoveMenu(ID_SOURCES_COPYTO, MF_BYCOMMAND);
        popup_menu->RemoveMenu(ID_SOURCES_MOVEFILESTO, MF_BYCOMMAND);
    }

    // edit query
    if (type == musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC)
        popup_menu->EnableMenuItem(ID_SOURCES_EDITQUERY, MF_ENABLED);
    else
        popup_menu->RemoveMenu(ID_SOURCES_EDITQUERY, MF_BYCOMMAND);
    
    // cdrom functions
    if (type == musikCore::MUSIK_PLAYLIST_TYPE_CDPLAYER)
    {
        popup_menu->EnableMenuItem(ID_SOURCES_RIPCD, MF_ENABLED);    
        popup_menu->EnableMenuItem(ID_SOURCES_QUERYFREEDB, MF_ENABLED);
        popup_menu->EnableMenuItem(ID_SOURCES_REFRESHCD, MF_ENABLED);
    }
    else
    {
        popup_menu->RemoveMenu(ID_SOURCES_RIPCD, MF_BYCOMMAND);
        popup_menu->RemoveMenu(ID_SOURCES_QUERYFREEDB, MF_BYCOMMAND);
        popup_menu->RemoveMenu(ID_SOURCES_REFRESHCD, MF_BYCOMMAND);
    }

    if (type == MUSIK_SOURCES_TYPE_PLUGIN)
    {
        int id = pItem->GetPlaylistID();
        CmusikPlugin& plugin = musikCube::g_Plugins.at(id);
        popup_menu->EnableMenuItem(ID_SOURCES_PLUGINEXECUTE, plugin.CanExecute() ? MF_ENABLED : (MF_DISABLED|MF_GRAYED));    
        popup_menu->EnableMenuItem(ID_SOURCES_PLUGINCONFIGURE, plugin.CanConfigure() ? MF_ENABLED : (MF_DISABLED|MF_GRAYED));
        popup_menu->EnableMenuItem(ID_SOURCES_PLUGINSTOP, plugin.CanStop() ? MF_ENABLED : (MF_DISABLED|MF_GRAYED));
        popup_menu->EnableMenuItem(ID_SOURCES_PLUGINABOUT, plugin.CanAbout() ? MF_ENABLED : (MF_DISABLED|MF_GRAYED));
    }
    else
    {
        popup_menu->RemoveMenu(ID_SOURCES_PLUGINEXECUTE, MF_BYCOMMAND);
        popup_menu->RemoveMenu(ID_SOURCES_PLUGINCONFIGURE, MF_BYCOMMAND);
        popup_menu->RemoveMenu(ID_SOURCES_PLUGINSTOP, MF_BYCOMMAND);
        popup_menu->RemoveMenu(ID_SOURCES_PLUGINABOUT, MF_BYCOMMAND);
    }

    // LIBRARY: "show all songs"
    if (type == musikCore::MUSIK_PLAYLIST_TYPE_LIBRARY)
        popup_menu->EnableMenuItem(ID_SOURCES_SHOWALLSONGS, MF_ENABLED);    
    else
        popup_menu->RemoveMenu(ID_SOURCES_SHOWALLSONGS, MF_BYCOMMAND);

    // DEVICES AND COLLECTED: "unload device" & "synchronize device"
    if ((type == musikCore::MUSIK_PLAYLIST_TYPE_REMOVABLE) || (type == MUSIK_SOURCES_TYPE_COLLECTED))
    {
        if (pItem->m_Library != NULL)
        {
            popup_menu->EnableMenuItem(ID_SOURCES_SYNCHRONIZE, MF_ENABLED);
            popup_menu->EnableMenuItem(ID_SOURCES_SYNCHRONIZEDPATHS, MF_ENABLED);
        }
    }
    else
    {
        popup_menu->RemoveMenu(ID_SOURCES_SYNCHRONIZE, MF_BYCOMMAND);
        popup_menu->RemoveMenu(ID_SOURCES_SYNCHRONIZEDPATHS, MF_BYCOMMAND);
    }

    // DEVICES: "unload device"
    if (type == musikCore::MUSIK_PLAYLIST_TYPE_REMOVABLE)
    {
        popup_menu->EnableMenuItem(ID_SOURCES_UNMOUNT, MF_ENABLED);
    }
    else
    {
        popup_menu->RemoveMenu(ID_SOURCES_UNMOUNT, MF_BYCOMMAND);
    }

    if (pItem == this->m_DynPlaylistRootItem)
        popup_menu->EnableMenuItem(ID_SOURCES_CREATESTOCK, MF_ENABLED);
    else
        popup_menu->RemoveMenu(ID_SOURCES_CREATESTOCK, MF_BYCOMMAND);

    ClientToScreen(&pos);
    popup_menu->TrackPopupMenu(0, pos.x, pos.y, this);
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnMenuQueryFreeDB()
{
    int WM_SOURCESQUERYFREEDB = RegisterWindowMessage(_T("SOURCESQUERYFREEDB"));
    m_Parent->SendMessage(WM_SOURCESQUERYFREEDB, NULL);
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::TogglePluginsVisible()
{
    musikCube::g_Prefs->SetPluginsVisible(!musikCube::g_Prefs->IsPluginsVisible());
    this->ReloadAllItems();
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::ToggleDynPlaylistsVisible()
{
    musikCube::g_Prefs->SetDynPlaylistsVisible(!musikCube::g_Prefs->IsDynPlaylistsVisible());
    this->ReloadAllItems();
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::ToggleStdPlaylistsVisible()
{
    musikCube::g_Prefs->SetPlaylistsVisible(!musikCube::g_Prefs->IsPlaylistsVisible());
    this->ReloadAllItems();
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::ToggleDevicesVisible()
{
    musikCube::g_Prefs->SetDevicesVisible(!musikCube::g_Prefs->IsDevicesVisible());
    this->ReloadAllItems();
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnMenuCopyFilesTo()
{
    musikCore::StringArray files;
    this->GetSelFilenames(this->m_PropTree->GetFocusedItem(), files);
    //
    this->SourcesFileOp(FO_COPY, files);
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnMenuMoveFilesTo()
{
    musikCore::StringArray files;
    this->GetSelFilenames(this->m_PropTree->GetFocusedItem(), files);
    //
    this->SourcesFileOp(FO_MOVE, files);
}

///////////////////////////////////////////////////

// http://www.codeguru.com/forum/printthread.php?t=309472

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
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

musikCore::String CmusikSourcesCtrl::SourcesFileOp(DWORD mode, const musikCore::StringArray& files, musikCore::String default_path)
{
    TCHAR path[MAX_PATH] = _T("");

    if (files.size())
    {
        BROWSEINFO bi = { 0 };
        bi.lpszTitle = _T("Please select a target directory.");
        bi.hwndOwner = GetSafeHwnd();
        bi.ulFlags |= BIF_NEWDIALOGSTYLE;
        bi.pidlRoot = NULL;

        if (default_path != _T(""))
        {
            bi.lParam =  (long)default_path.c_str(); 
            bi.lpfn    = BrowseCallbackProc;
        }

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
            
            // use Michael Dunn's convenient SHFileOperation wrapper to
            // perform the copy using Window's shell api...
            BOOL apicalled;
            CShellFileOp copy;

            for (size_t i = 0; i < files.size(); i++)
            {
                copy.AddSourceFile(files.at(i));
            }
            copy.AddDestFile(path);
            
            copy.SetOperationFlags(
                mode,             // op type
                AfxGetMainWnd(), // handle to main window
                FALSE,           // flag - silent mode?
                FALSE,           // flag - allow undo?
                FALSE,           // flag - should wild cards affect files only?
                FALSE,           // flag - suppress confirmation messages?
                TRUE,            // flag - suppress confirmation messages when making directories?
                FALSE,           // flag - rename files when name collisions occur?
                FALSE);         // flag - simple progress dialog?

            if (!copy.Go(&apicalled))
                MessageBox(
                _T("File operation was unsuccessful."),
                _T("musikCube"),
                MB_ICONWARNING | MB_OK);
        }
    }

    return path;
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnMenuRenameItem()
{
    this->RenameItem();
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnMenuEditQuery()
{
    CmusikDynPlaylistDlg dlg;
    
    CmusikPropTreeItem* pItem = this->m_PropTree->GetFocusedItem();
    if (pItem)
    {
        musikCore::String name, query;

        musikCube::g_Library->GetDynPlaylistInfo(
            pItem->GetPlaylistID(),
            name,
            query);

        dlg.SetName(name);
        dlg.SetQuery(query);
        this->DeselectAllItems(false);
        int ret = dlg.DoModal();

        if (ret == IDOK)
        {
            musikCube::g_Library->RewriteDynPlaylistQuery(
                pItem->GetPlaylistID(),
                dlg.GetQuery());

            if (name != dlg.GetName())
            {
                musikCube::g_Library->RenameDynPlaylist(
                    pItem->GetPlaylistID(),
                    dlg.GetName());
                this->LoadDynPlaylistItems();
            }
        }
    }
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnMenuDeleteItem()
{
    this->DeleteSelectedItem();
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnMenuShuffleNowPlaying()
{
    musikCube::g_Player->Shuffle();

    CmusikPropTreeItem* pItem = this->m_PropTree->GetFocusedItem();
    if (pItem)
    {
        if (pItem->GetPlaylistType() == musikCore::MUSIK_PLAYLIST_TYPE_NOWPLAYING)
        {
            int WM_UPDATEPLAYLIST = RegisterWindowMessage(_T("UPDATEPLAYLIST"));
            m_Parent->SendMessage(WM_UPDATEPLAYLIST);
        }
    }
}

///////////////////////////////////////////////////

// CmusikSourcesCtrl

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikSourcesCtrl, CmusikDockBar)

///////////////////////////////////////////////////

int WM_SOURCES_EDIT_COMMIT = RegisterWindowMessage(_T("MUSIKEDITCOMMIT"));
int WM_SOURCES_EDIT_CANCEL = RegisterWindowMessage(_T("MUSIKEDITCANCEL"));
int WM_SOURCES_EDIT_CHANGE = RegisterWindowMessage(_T("MUSIKEDITCHANGE"));
//
const unsigned int SOURCES_DEVICE_TIMER_ID = WM_USER + 25;
const unsigned int SOURCES_SEARCH_TIMER_ID = WM_USER + 26;

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikSourcesCtrl, CmusikDockBar)

    // mfc message maps
    ON_WM_CREATE()
    ON_WM_SHOWWINDOW()
    ON_WM_KEYDOWN()
    ON_WM_CONTEXTMENU()
    ON_WM_TIMER()

    // custom message maps
    ON_REGISTERED_MESSAGE(WM_SOURCES_EDIT_COMMIT, OnItemEditCommit)
    ON_REGISTERED_MESSAGE(WM_SOURCES_EDIT_CANCEL, OnItemEditCancel)
    ON_REGISTERED_MESSAGE(WM_SOURCES_EDIT_CHANGE, OnItemEditChange)

    // menu
    ON_COMMAND(ID_SOURCES_RENAME, OnMenuRenameItem)
    ON_COMMAND(ID_SOURCES_DELETE, OnMenuDeleteItem)
    ON_COMMAND(ID_SOURCES_SHUFFLEPLAYLIST, OnMenuShuffleNowPlaying)
    ON_COMMAND(ID_SOURCES_COPYTO, OnMenuCopyFilesTo)
    ON_COMMAND(ID_SOURCES_EDITQUERY, OnMenuEditQuery)
    ON_COMMAND(ID_SOURCES_QUERYFREEDB, OnMenuQueryFreeDB)
    ON_COMMAND(ID_SOURCES_RIPCD, OnMenuRipSelectedCD)
    ON_COMMAND(ID_SOURCES_REFRESHCD, OnMenuRefreshCD)
    ON_COMMAND(ID_SOURCES_UNMOUNT, OnMenuDeviceUnloadLibrary)
    ON_COMMAND(ID_SOURCES_CREATESTOCK, OnMenuCreateStockDynamicPlaylists)
    ON_COMMAND(ID_SOURCES_MOVEUP, OnMenuMoveItemUp)
    ON_COMMAND(ID_SOURCES_MOVEDOWN, OnMenuMoveItemDown)
    ON_COMMAND(ID_SOURCES_CLEAR, OnSourcesNowPlayingClear)
    ON_COMMAND(ID_SOURCES_MOVEFILESTO, OnMenuMoveFilesTo)
    ON_COMMAND(ID_SOURCES_PLUGINEXECUTE, OnMenuPluginExecute)
    ON_COMMAND(ID_SOURCES_PLUGINCONFIGURE, OnMenuPluginConfigure)
    ON_COMMAND(ID_SOURCES_PLUGINSTOP, OnMenuPluginStop)
    ON_COMMAND(ID_SOURCES_PLUGINABOUT, OnMenuPluginAbout)
    ON_COMMAND(ID_SOURCES_SHOWALLSONGS, OnMenuLibraryShowAllSongs)
    ON_COMMAND(ID_SOURCES_SYNCHRONIZE, OnMenuSynchronizeDevice)

    // proptree
    ON_NOTIFY(PTN_SELCHANGE, IDC_SOURCES, OnItemChanged)
    ON_NOTIFY(PTN_ITEMDRAG, IDC_SOURCES, OnItemDragged)

    ON_COMMAND(ID_SOURCES_SYNCHRONIZEDPATHS, OnMenuSynchronizedPaths)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikSourcesCtrl::ReloadAllItems()
{
    this->m_PropTree->SetRedraw(FALSE);
    this->m_PropTree->DetachAllItems();

    musikCore::PlaylistInfo info;

    // browse
    if ( ! m_BrowseRootItem)
    {
        info = musikCore::PlaylistInfo(_T("Browse"), -1, -1);
        //
        m_BrowseRootItem = new CmusikPropTreeItem();
        m_BrowseRootItem->SetPlaylistInfo(info);
        m_BrowseRootItem->Expand(true);
    }
    //
    this->m_PropTree->InsertItem(m_BrowseRootItem);
    LoadBrowseItems();

    // devices
    if (m_DevicesTimerID)
    {
        KillTimer(m_DevicesTimerID);
        m_DevicesTimerID = NULL;
    }
    //
    if (musikCube::g_Prefs->IsDevicesVisible())
    {
        if ( ! m_DevicesRootItem)
        {
            info = musikCore::PlaylistInfo(_T("Devices"), -1, -1);
            m_DevicesRootItem = new CmusikPropTreeItem();
            m_DevicesRootItem->SetPlaylistInfo(info);
            m_DevicesRootItem->Expand(true);
        }
        //
        this->m_PropTree->InsertItem(m_DevicesRootItem);
        //
        for (size_t i = 0; i < m_DeviceItems.size(); i++)
        {
            this->m_PropTree->InsertItem(m_DeviceItems[i], m_DevicesRootItem);
        }
        //
        RefreshDevices();
        m_DevicesTimerID = this->SetTimer(SOURCES_DEVICE_TIMER_ID, 2000, NULL);
    }
    
    // standard playlists
    if (musikCube::g_Prefs->IsPlaylistsVisible())
    {
        if ( ! m_StdPlaylistRootItem)
        {
            info = musikCore::PlaylistInfo(_T("Playlists"), -1, -1);
            m_StdPlaylistRootItem = new CmusikPropTreeItem();
            m_StdPlaylistRootItem->SetPlaylistInfo(info);
            m_StdPlaylistRootItem->Expand(true);
        }
        //
        this->m_PropTree->InsertItem(m_StdPlaylistRootItem);
        LoadStdPlaylistItems();
    }

    // dynamic playlists
    if (musikCube::g_Prefs->IsDynPlaylistsVisible())
    {
        if ( ! m_DynPlaylistRootItem)
        {
            info = musikCore::PlaylistInfo(_T("Dynamic Playlists"), -1, -1);
            m_DynPlaylistRootItem = new CmusikPropTreeItem();
            m_DynPlaylistRootItem->SetPlaylistInfo(info);
            m_DynPlaylistRootItem->Expand(true);
        }
        //
        this->m_PropTree->InsertItem(m_DynPlaylistRootItem);
        LoadDynPlaylistItems();
    }

    // plugins
    if ( musikCube::g_Prefs->IsPluginsVisible())
    {
        if ( ! m_PluginsRootItem)
        {
            info = musikCore::PlaylistInfo(_T("Plugins"), -1, -1);
            m_PluginsRootItem = new CmusikPropTreeItem();
            m_PluginsRootItem->SetPlaylistInfo(info);
            m_PluginsRootItem->Expand(TRUE);
        }
        //
        this->m_PropTree->InsertItem(m_PluginsRootItem);
        LoadPluginItems();
    }

    this->m_PropTree->UpdateScrollbar();

    this->m_PropTree->SetRedraw(TRUE);
    this->m_PropTree->RedrawWindow();
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::FocusLibrary()
{
    this->FocusItem(this->m_BrowseItems.at(0));
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::FocusQuickSearch()
{
    this->FocusItem(m_SearchItem);
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::FocusNowPlaying()
{
    this->FocusItem(m_BrowseItems.at(2));
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::FocusItem(CmusikPropTreeItem* pItem)
{
    if ( ! pItem)
    {
        this->DeselectAllItems(true);
        return;
    }

    this->DeselectAllItems(false);
    this->m_PropTree->SetFocusedItem(pItem);

    // emulate a mouse click
    this->NotifyParentItemChanged(pItem);
}

///////////////////////////////////////////////////

int CmusikSourcesCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CmusikDockBar::OnCreate(lpCreateStruct) == -1)
    {
        return -1;
    }

    m_DropTarget->Register(this);

    // default font
    //
    if ( ! this->m_DefaultFont.CreateStockObject(DEFAULT_GUI_FONT))
    {
        return -1;
    }

    // dock bar properties
    //
    m_ForceDrawBorder = false;
    SetBarStyle(GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
    EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
    ShowGripper(false);

    // proptree
    //
    if ( ! this->m_PropTree->Create(WS_CHILD | WS_VISIBLE | PTS_NOTIFY, CRect(0, 0, 0, 0), this, IDC_SOURCES))
    {
        return -1;
    }

    // edit in place
    //
    m_EditInPlace.Create(WS_CHILD | WS_CLIPCHILDREN, CRect(0, 0, 0, 0), m_PropTree, IDC_EDITINPLACE);
    m_EditInPlace.EnableWindow(FALSE);
    m_EditInPlace.SetFont(&this->m_DefaultFont);
    m_EditInPlace.SetMessageTarget(this);

    this->m_PropTree->SetFont(&this->m_DefaultFont);

    // devices
    //
    m_DevicesTimerID = this->SetTimer(SOURCES_DEVICE_TIMER_ID, 2000, NULL);

    // just as it says...
    //
    ReloadAllItems();

    return 0;
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::LoadBrowseItems()
{
    if (m_BrowseItems.size() > 0)
    {
        for (size_t i = 0; i < m_BrowseItems.size(); i++)
        {
            this->m_PropTree->InsertItem(m_BrowseItems[i], m_BrowseRootItem);
        }
    }

    else
    {
        musikCore::PlaylistInfo info;
        CmusikPropTreeItem *lib, *nowplaying, *netstreams, *search;

        // library
        info = musikCore::PlaylistInfo("Library", musikCore::MUSIK_PLAYLIST_TYPE_LIBRARY, NULL);
        lib = new CmusikPropTreeItem();
        lib->SetPlaylistInfo(info);
        lib->SetBitmaps(m_pLibOff, m_pLibOn);
        lib->m_Library = musikCube::g_Library;
        this->m_PropTree->InsertItem(lib, m_BrowseRootItem);
        m_BrowseItems.push_back(lib);

        // collected
        info = musikCore::PlaylistInfo("Collected", MUSIK_SOURCES_TYPE_COLLECTED, NULL);
        m_CollectedItem = new CmusikPropTreeItem();
        m_CollectedItem->SetPlaylistInfo(info);
        m_CollectedItem->SetBitmaps(m_pColOff, m_pColOn);
        this->m_PropTree->InsertItem(m_CollectedItem, m_BrowseRootItem);
        m_BrowseItems.push_back(m_CollectedItem);

        // now playing
        info = musikCore::PlaylistInfo("Now Playing", musikCore::MUSIK_PLAYLIST_TYPE_NOWPLAYING, NULL);
        nowplaying = new CmusikPropTreeItem();
        nowplaying->SetPlaylistInfo(info);
        nowplaying->SetBitmaps(m_pNPOff, m_pNPOn);
        this->m_PropTree->InsertItem(nowplaying, m_BrowseRootItem);
        m_BrowseItems.push_back(nowplaying);

        // net radio
        info = musikCore::PlaylistInfo("Net Radio", MUSIK_SOURCES_TYPE_NETSTREAMS, NULL);
        netstreams = new CmusikPropTreeItem();
        netstreams->SetPlaylistInfo(info);
        netstreams->SetBitmaps(m_pNSOff, m_pNSOn);
        this->m_PropTree->InsertItem(netstreams, m_BrowseRootItem);
        m_BrowseItems.push_back(netstreams);

        // search
        info = musikCore::PlaylistInfo("Search...", MUSIK_SOURCES_TYPE_QUICKSEARCH, -1); 
        search = new CmusikPropTreeItem();
        search->SetPlaylistInfo(info);
        search->SetBitmaps(m_pQSOff, m_pQSOn);
        this->m_PropTree->InsertItem(search, m_BrowseRootItem);
        m_BrowseItems.push_back(search);
        m_SearchItem = search;
    }
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::LoadPluginItems()
{
    // load the plugins...
    if (m_PluginItems.size())
    {
        for (size_t i = 0; i < m_PluginItems.size(); i++)
        {
            this->m_PropTree->DeleteItem(m_PluginItems.at(i));
        }
    }
    m_PluginItems.clear();

    CmusikPropTreeItem* temp;
    for (size_t i = 0; i < musikCube::g_Plugins.size(); i++)
    {
        if (musikCube::g_Plugins.at(i).InSources())
        {
            musikCore::PlaylistInfo info;
            info = musikCore::PlaylistInfo(musikCube::g_Plugins.at(i).GetPluginName(), MUSIK_SOURCES_TYPE_PLUGIN, i);
            
            temp = this->m_PropTree->InsertItem(new CmusikPropTreeItem(), m_PluginsRootItem);
            temp->SetPlaylistInfo(info);
            temp->SetBitmaps(m_pPlugOff, m_pPlugOn);

            m_PluginItems.push_back(temp);
        }
    }
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::LoadStdPlaylistItems()
{
    if (!musikCube::g_Prefs->IsPlaylistsVisible())
        return;

    // load the playlists...
    if (m_StdPlaylistItems.size())
    for (size_t i = 0; i < m_StdPlaylistItems.size(); i++)
    {
        this->m_PropTree->DeleteItem(m_StdPlaylistItems.at(i));
    }
    m_StdPlaylistItems.clear();

    musikCore::PlaylistInfoArray items;
    musikCube::g_Library->GetAllStdPlaylists(items);

    CmusikPropTreeItem* temp;
    for (size_t i = 0; i < items.size(); i++)
    {
        temp = this->m_PropTree->InsertItem(new CmusikPropTreeItem(), m_StdPlaylistRootItem);
        temp->SetPlaylistInfo(items.at(i));
        temp->SetBitmaps(m_pPLOff, m_pPLOn);
        m_StdPlaylistItems.push_back(temp);
    }

    // load the "Create..."
    musikCore::PlaylistInfo info;
    info = musikCore::PlaylistInfo("Create...", MUSIK_SOURCES_TYPE_NEWSTDPLAYLIST, -1); 
    temp = this->m_PropTree->InsertItem(new CmusikPropTreeItem(), m_StdPlaylistRootItem);
    temp->SetPlaylistInfo(info);
    temp->SetBitmaps(m_pPLOff, m_pPLOn);
    m_StdPlaylistItems.push_back(temp);
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::DeleteLastCreatedPlaylist(int type)
{
    musikCore::PlaylistInfoArray items;

    switch (type)
    {
    case musikCore::MUSIK_PLAYLIST_TYPE_STANDARD:
        {
            musikCube::g_Library->GetAllStdPlaylists(items);
            musikCube::g_Library->DeleteStdPlaylist(items.at(items.size() - 1).GetID());
        }
        break;

    case musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC:
        {
            musikCube::g_Library->GetAllDynPlaylists(items);
            musikCube::g_Library->DeleteDynPlaylist(items.at(items.size() - 1).GetID());
        }
        break;
    }
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::LoadDynPlaylistItems()
{
    if (!musikCube::g_Prefs->IsDynPlaylistsVisible())
    {
        return;
    }

    // load the playlists...
    for (size_t i = 0; i < m_DynPlaylistItems.size(); i++)
    {
        this->m_PropTree->DeleteItem(m_DynPlaylistItems.at(i));
    }

    m_DynPlaylistItems.clear();

    musikCore::PlaylistInfoArray items;
    musikCube::g_Library->GetAllDynPlaylists(items, false);

    CmusikPropTreeItem* temp;
    for (size_t i = 0; i < items.size(); i++)
    {
        temp = this->m_PropTree->InsertItem(new CmusikPropTreeItem(), m_DynPlaylistRootItem);
        temp->SetPlaylistInfo(items.at(i));
        temp->SetBitmaps(m_pDPOff, m_pDPOn);

        m_DynPlaylistItems.push_back(temp);
    }

    // load the "Create..."
    musikCore::PlaylistInfo info;
    info = musikCore::PlaylistInfo("Create...", MUSIK_SOURCES_TYPE_NEWDYNPLAYLIST, -1); 
    temp = this->m_PropTree->InsertItem(new CmusikPropTreeItem(), m_DynPlaylistRootItem);
    temp->SetPlaylistInfo(info);
    temp->SetBitmaps(m_pDPOff, m_pDPOn);
    m_DynPlaylistItems.push_back(temp);
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnDropFiles(HDROP hDropInfo, bool right_button)
{
    // set cursor back to hour glass
    SetCursor(LoadCursor(NULL, IDC_WAIT));

    // see if the drag landed on an existing
    // playlist, if it did, we'll append
    CPoint pos;
    ::GetCursorPos(&pos);
    ScreenToClient(&pos);

    CmusikPropTreeItem* pItem = this->m_PropTree->FindItem(pos);

    // no selected item to drop on
    if (!pItem || pItem->IsRootLevel())
    {
        DragFinish(hDropInfo);
        return;
    }

    // dnd vars
    size_t nNumFiles;
    TCHAR szNextFile [MAX_PATH];
    SHFILEINFO  rFileInfo;
    nNumFiles = DragQueryFile (hDropInfo, -1, NULL, 0);

    // build a list of dnd filenames
    musikCore::StringArray* files = new musikCore::StringArray();
    musikCore::String sTemp;
    for (size_t i = 0; i < nNumFiles; i++)
    {
        if (DragQueryFile(hDropInfo, i, szNextFile, MAX_PATH) > 0)
        {
            // at a dir: recurse 
            SHGetFileInfo(szNextFile, 0, &rFileInfo, sizeof(rFileInfo), SHGFI_ATTRIBUTES);
            if (rFileInfo.dwAttributes & SFGAO_FOLDER)
            {
                sTemp = szNextFile;
                sTemp += "\\*.*";

                musikCore::Dir::OpenDir(sTemp, files);
            }        

            // att a file: add to list
            else
            {
                files->push_back(szNextFile);
            }
        }
    }

    // distribute the drop event
    bool delete_files = false;
    bool success = false;
    switch(pItem->GetPlaylistType())
    {
    case musikCore::MUSIK_PLAYLIST_TYPE_STANDARD:
        success = (musikCore::MUSIK_LIBRARY_OK == musikCube::g_Library->AppendStdPlaylist(pItem->GetPlaylistID(), *files));
        break;

    case musikCore::MUSIK_PLAYLIST_TYPE_LIBRARY:
        success = this->DropAddToLibrary(musikCube::g_MainLibrary, files);
        delete_files = false;
        break;

    case MUSIK_SOURCES_TYPE_COLLECTED:
        success = this->DropAddToCollected(files);
        delete_files = false;
        break;

    case musikCore::MUSIK_PLAYLIST_TYPE_REMOVABLE:
        success = this->DropOnRemovableDevice(files, pItem);
        delete_files = false;
        break;

    case musikCore::MUSIK_PLAYLIST_TYPE_NOWPLAYING:
        success = this->DropAddNowPlaying(files);
        break;

    default:
        break;
    }

    DragFinish(hDropInfo);
    if (delete_files)
    {
        delete files;
    }

    // if the mouse is no longer in the window, be sure
    // to set the hovered item to NULL and redraw.
    CRect rcWindow;
    ::GetWindowRect(this->GetSafeHwnd(), &rcWindow);

    CPoint mousePos;
    ::GetCursorPos(&mousePos);

    if (!rcWindow.PtInRect(mousePos))
    {
        OnDragLeave();
    }
}

///////////////////////////////////////////////////

bool CmusikSourcesCtrl::DropAddToCollected(musikCore::StringArray* pFiles)
{
    if (! this->m_CollectedItem->m_Library)
    {
        this->LoadCollectedLibrary();
    }

    return this->DropAddToLibrary(this->m_CollectedItem->m_Library, pFiles);
}

///////////////////////////////////////////////////

bool CmusikSourcesCtrl::DropAddToLibrary(musikCore::Library* pLibrary, musikCore::StringArray* pFiles)
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
    
    pLibrary->QueueTask(batchAddThread);

    return true;
}

///////////////////////////////////////////////////

bool CmusikSourcesCtrl::DropOnRemovableDevice(musikCore::StringArray* pFiles, CmusikPropTreeItem* pItem)
{
    musikCore::Library*& pItemLib = pItem->m_Library;

    if (!pItemLib)
    {
        this->LoadRemovableLibrary(pItem);
    }

    bool success = false;
    if (pItemLib)
    {
        CPoint pos;
        ::GetCursorPos(&pos);

        CMenu popup;
        popup.LoadMenu(IDR_DROPDEVICE_MENU);

        CMenu* pSubMenu = popup.GetSubMenu(0);
        int selMenu = (int)pSubMenu->TrackPopupMenu(TPM_RETURNCMD, pos.x, pos.y, this, NULL);

        switch(selMenu)
        {
        case ID_DROPDEVICE_COPYFILESTODEVICE:
            success = this->DropCopyFilesToDevice(pFiles, pItem);
            break;

        case ID_DROPDEVICE_ADDTODEVICE:
            success = this->DropAddToDevice(pFiles, pItem);
            break;
        }
    }
    else
    {
        MessageBox(
            _T("No library has been built for this device yet. Please select it to build a library."),
            _T("musikCube"),
            MB_OK | MB_ICONINFORMATION
       );

        delete pFiles;
    }

    return success;
}

///////////////////////////////////////////////////

bool CmusikSourcesCtrl::DropCopyFilesToDevice(musikCore::StringArray* pFiles, CmusikPropTreeItem* pItem)
{
    musikCore::String copyPath = SourcesFileOp(
            FO_COPY, 
            *pFiles, 
            ((musikCore::RemLibrary*)pItem->m_Library)->GetRemPath() 
       );

    if (copyPath != _T(""))
    {
        musikCore::BatchAdd* batchAddParams;
        musikCore::BatchAddTask* batchAddThread;

        batchAddParams = new musikCore::BatchAdd();
        batchAddParams->m_Library = pItem->m_Library;
        batchAddParams->m_Directory = copyPath + _T("\\*.*");
        batchAddParams->m_Functor = musikCube::g_Functor;

        batchAddThread = new musikCore::BatchAddTask();
        batchAddThread->m_Params = batchAddParams;
        
        pItem->m_Library->QueueTask(batchAddThread);
    }

    delete pFiles;
    return true;
}

///////////////////////////////////////////////////

bool CmusikSourcesCtrl::DropAddToDevice(musikCore::StringArray* pFiles, CmusikPropTreeItem* pItem)
{
    bool success = this->DropAddToLibrary(pItem->m_Library, pFiles);

    if (!success)
    {
        delete pFiles;
    }

    return success;
}

///////////////////////////////////////////////////

bool CmusikSourcesCtrl::DropAddNowPlaying(musikCore::StringArray* files)
{
    bool was_nowplaying_empty = musikCube::g_Player->GetPlaylist()->GetCount() ? false : true;
    bool begin_trans = false;

    musikCube::g_Library->BeginTransaction();
    musikCore::Song song;
    for (size_t i = 0; i < files->size(); i++)
    {
        // add song to library, if necessary
        musikCube::g_Library->AddSong(files->at(i));

        // get the song, add it to the pl aylist
        if (musikCube::g_Library->GetSongAttrFromFilename(files->at(i), song) == musikCore::MUSIK_LIBRARY_OK)
        {
            musikCube::g_Player->GetPlaylist()->Add(song);
        }
    }
    musikCube::g_Library->EndTransaction();

    // post a message to the main frame, letting
    // it know that drag and drop has completed
    if (was_nowplaying_empty)
    {
        if (!musikCube::g_Player->IsPlaying())
            musikCube::g_Player->Play(0, 0, musikCore::MUSIK_PLAYER_SONG_USER_ADVANCE, true);
        else
            musikCube::g_Player->FindNewIndex(musikCube::g_Player->GetPlaying()->GetID());
    }

    return true;
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::CreateNewStdPlaylist()
{
    CmusikPropTreeItem* pItem = this->m_StdPlaylistItems[this->m_StdPlaylistItems.size() - 1];
    musikCube::g_Library->CreateStdPlaylist(L"New Playlist", musikCore::StringArray(), m_StdPlaylistItems.size() - 1);
    EditItemInPlace(pItem, MUSIK_SOURCES_EDITINPLACE_NEWSTDPLAYLIST, L"New Playlist");
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::CreateNewDynPlaylist()
{
    CmusikPropTreeItem* pItem = this->m_DynPlaylistItems[this->m_DynPlaylistItems.size() - 1];
    EditItemInPlace(pItem, MUSIK_SOURCES_EDITINPLACE_NEWDYNPLAYLIST, CString("New Playlist"));
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::DeselectAllItems(bool redraw)
{
    CmusikPropTreeItem* pItem = this->m_PropTree->GetFocusedItem();
    if (pItem)
    {
        pItem->Select(FALSE);
        this->m_PropTree->SetFocusedItem(NULL);

        if (redraw)
        {
            Invalidate();
        }
    }
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::GetSelFilenames(CmusikPropTreeItem* pItem, musikCore::String& files)
{
    int nMode = pItem->GetPlaylistType();

    switch (nMode)
    {
    case musikCore::MUSIK_PLAYLIST_TYPE_STANDARD:
        musikCube::g_Library->GetStdPlaylistFns(pItem->GetPlaylistID(), files);
        break;

    case musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC:
        musikCube::g_Library->GetDynPlaylistFns(pItem->GetPlaylistID(), files); 
        break;

    case musikCore::MUSIK_PLAYLIST_TYPE_NOWPLAYING:
        musikCube::g_Library->GetStdPlaylistFns(*musikCube::g_Player->GetPlaylist(), files);
        break;

    case musikCore::MUSIK_PLAYLIST_TYPE_LIBRARY:
        musikCube::g_Library->GetAllFns(files);
        break;

    case musikCore::MUSIK_PLAYLIST_TYPE_REMOVABLE:
        if (pItem->m_Library)
            pItem->m_Library->GetAllFns(files);
        break;

    default:
        break;
    } 
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::GetSelFilenames(CmusikPropTreeItem* pItem, musikCore::StringArray& files)
{
    int nMode = pItem->GetPlaylistType();
    
    switch (nMode)
    {
    case musikCore::MUSIK_PLAYLIST_TYPE_STANDARD:
        musikCube::g_Library->GetStdPlaylistFns(pItem->GetPlaylistID(), files, false);
        break;

    case musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC:
        musikCube::g_Library->GetDynPlaylistFns(pItem->GetPlaylistID(), files, false); 
        break;

    case musikCore::MUSIK_PLAYLIST_TYPE_NOWPLAYING:
        musikCube::g_Library->GetStdPlaylistFns(*musikCube::g_Player->GetPlaylist(), files, false);
        break;


    case musikCore::MUSIK_PLAYLIST_TYPE_LIBRARY:
        musikCube::g_Library->RawQueryStringArray(_T(" SELECT DISTINCT filename FROM songs; "), files);
        break;

    case musikCore::MUSIK_PLAYLIST_TYPE_REMOVABLE:
        if (pItem->m_Library)
            pItem->m_Library->RawQueryStringArray(_T(" SELECT DISTINCT filename FROM songs; "), files);
        break;

    default:
        break;
    }     
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::DoDrag(CmusikPropTreeItem* pItem)
{
    if (!pItem || 
        pItem->GetPlaylistType() == musikCore::MUSIK_PLAYLIST_TYPE_CDPLAYER ||
        pItem->GetPlaylistType() == MUSIK_SOURCES_TYPE_PLUGIN)
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
    GetSelFilenames(pItem, files);

    if (files.size())
    {
        // CStringList containing files
        for (size_t i = 0; i < files.size(); i++)
        {
            lsDraggedFiles.AddTail(files.at(i));
            uBuffSize += files.at(i).GetLength() + 1;
        }

        files.clear();
    }

    
    // Add 1 extra for the final null char, and the size of the DROPFILES struct.
    uBuffSize = sizeof(DROPFILES) + sizeof(TCHAR) * (uBuffSize + 1);

    // Allocate memory from the heap for the DROPFILES struct.
    hgDrop = GlobalAlloc (GHND | GMEM_SHARE, uBuffSize);

    if (!hgDrop)
        return;

    pDrop = (DROPFILES*) GlobalLock (hgDrop);

    if (!pDrop)
    {
        GlobalFree (hgDrop);
        return;
    }

    // Fill in the DROPFILES struct.
    pDrop->pFiles = sizeof(DROPFILES);

    pDrop->fWide = TRUE;

    // Copy all the filenames into memory after the end of the DROPFILES struct.
    pos = lsDraggedFiles.GetHeadPosition();
    pszBuff = (TCHAR*) (LPBYTE(pDrop) + sizeof(DROPFILES));

    while (pos)
    {
        wcscpy (pszBuff, (LPCTSTR) lsDraggedFiles.GetNext (pos));
        pszBuff = 1 + _tcschr (pszBuff, '\0');
    }

    GlobalUnlock (hgDrop);

    // Put the data in the data source.
    datasrc.CacheGlobalData (CF_HDROP, hgDrop, &etc);

    // Add in our own custom data, so we know that the drag originated from our 
    // window.  CMyDropTarget::DragEnter() checks for this custom format, and
    // doesn't allow the drop if it's present.  This is how we prevent the user
    // from dragging and then dropping in our own window.
    // The data will just be a dummy bool.
    HGLOBAL hgBool;

    hgBool = GlobalAlloc (GHND | GMEM_SHARE, sizeof(bool));

    if (NULL == hgBool)
    {
        GlobalFree (hgDrop);
        return;
    }

    // Put the data in the data source.
    etc.cfFormat = m_DropID;
    datasrc.CacheGlobalData (m_DropID, hgBool, &etc);

    // Start the drag 'n' drop!
    DROPEFFECT dwEffect = datasrc.DoDragDrop (DROPEFFECT_COPY | DROPEFFECT_MOVE);

    // If the DnD completed OK, we remove all of the dragged items from our
    // list.
    switch (dwEffect)
    {
        case DROPEFFECT_COPY:
        case DROPEFFECT_MOVE:
        {
            // the copy completed successfully
            // do whatever we need to do here
            cout << _T("DND from playlist completed successfully. The data has a new owner.\n");
        }
        break;

        case DROPEFFECT_NONE:
        {
            // the copy completed successfully
            // on a windows nt machine.
            // do whatever we need to do here

            bool bDeletedAnything = false;
            if (! bDeletedAnything)
            {
                // The DnD operation wasn't accepted, or was canceled, so we 
                // should call GlobalFree() to clean up.
                GlobalFree (hgDrop);
                GlobalFree (hgBool);

                cout << _T("DND had a problem. We had to manually free the data.\n");
            }
        }

        break;
    }    
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::RenameItem(CmusikPropTreeItem* pItem, int mode, CPoint pos)
{
    if ( ! pItem)
    {
        pItem = this->m_PropTree->GetFocusedItem();
    }

    if ( ! pItem)
    {
        return;
    }

    // not trying to rename a root
    if (pItem->IsRootLevel())
    {
        return;
    }

    // not renaming the library, now playing,
    // or quick search
    if (pItem == m_BrowseItems.at(0) || pItem == m_BrowseItems.at(1))
        return;

    CPoint nPos;
    if (pos.x == -1 && pos.y == -1)
        nPos = pItem->GetLocation();
    else
        nPos = pos;

    CRect rcClient;
    GetClientRect(rcClient);
    
    int left;
    musikCube::g_DrawGraphics ? left = 32 : left = 12;
    CRect rect(left, nPos.y + 4, rcClient.Width() - 1, nPos.y + PROPTREEITEM_DEFHEIGHT - 2);

    if (this->m_PropTree->IsScrollVisible())
    {
        rect.right -= 5;
    }

    EditItemInPlace(pItem, mode, pItem->GetLabelText());
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::RemoveStdPlaylist(CmusikPropTreeItem* pItem)
{
    for (size_t i = 0; i < m_StdPlaylistItems.size(); i++)
    {
        if (pItem == m_StdPlaylistItems.at(i))
        {
            m_StdPlaylistItems.erase(m_StdPlaylistItems.begin() + i);
            this->m_PropTree->DeleteItem(pItem);
            return;
        }
    }
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::RemoveDynPlaylist(CmusikPropTreeItem* pItem)
{
    for (size_t i = 0; i < m_DynPlaylistItems.size(); i++)
    {
        if (pItem == m_DynPlaylistItems.at(i))
        {
            m_DynPlaylistItems.erase(m_DynPlaylistItems.begin() + i);
            this->m_PropTree->DeleteItem(pItem);
            return;
        }
    }
}


///////////////////////////////////////////////////

void CmusikSourcesCtrl::DeleteSelectedItem()
{
    CmusikPropTreeItem* pItem = this->m_PropTree->GetFocusedItem();
    if (pItem)
    {
        switch(pItem->GetPlaylistType())
        {
        case musikCore::MUSIK_PLAYLIST_TYPE_STANDARD:
        case musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC:
            {
                CmusikSourcesItemPtrArray* ptrItems;

                // remove the playlist from the database
                if (pItem->GetPlaylistType() == musikCore::MUSIK_PLAYLIST_TYPE_STANDARD)
                {
                    ptrItems = &m_StdPlaylistItems;
                    musikCube::g_Library->DeleteStdPlaylist(pItem->GetPlaylistID());
                }
                else
                {
                    ptrItems = &m_DynPlaylistItems;
                    musikCube::g_Library->DeleteDynPlaylist(pItem->GetPlaylistID());
                }

                // find selection index
                int SelAt = 0;
                for (size_t i = 0; i < ptrItems->size(); i++)
                {
                    if (ptrItems->at(i) == pItem)
                    {
                        SelAt = i;
                        break;
                    }
                }

                pItem->Detach();
                delete pItem;
                ptrItems->erase(ptrItems->begin() + SelAt);

                // after detaching, if there is anothing playlist
                // in the same position, select that.
                if (ptrItems->size() > 1)
                {
                    if (SelAt == (int)ptrItems->size() - 1)
                    {
                        SelAt--;
                    }

                    FocusItem(ptrItems->at(SelAt));
                }
                // no playlists left to select! focus the library or
                // now playing.
                else
                {
                    if (!musikCube::g_Player->IsPlaying())
                    {
                        FocusLibrary();
                    }
                    else
                    {
                        FocusNowPlaying();
                    }
                }

                this->m_PropTree->UpdateScrollbar();    // may need to hide scrollbar
            }
            break;
        }
    }
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // user pressed f2 to rename an entry
    if (nChar == VK_F2)
        RenameItem();

    // user requested playlist deletion
    if (nChar == VK_DELETE)
    {
        DeleteSelectedItem();
    }

    if (GetKeyState(VK_CONTROL) < 0)
    {
        if (nChar == VK_UP)
            MoveSelectedItemUp();
        if (nChar == VK_DOWN)
            MoveSelectedItemDown();
    }
}

///////////////////////////////////////////////////

LRESULT CmusikSourcesCtrl::OnItemEditCancel(WPARAM wParam, LPARAM lParam)
{
    int cmd = (int)lParam;

    this->m_EditInPlaceItem->GetPlaylistInfo().m_Name = this->m_PreEditInPlaceText;
    this->m_EditInPlaceItem = false;

    switch(cmd)
    {
    case MUSIK_SOURCES_EDITINPLACE_NEWSTDPLAYLIST:
        DeleteLastCreatedPlaylist(musikCore::MUSIK_PLAYLIST_TYPE_STANDARD);
        break;

    case MUSIK_SOURCES_EDITINPLACE_NEWDYNPLAYLIST:
        DeleteLastCreatedPlaylist(musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC);
        break;

    case MUSIK_SOURCES_EDITINPLACE_QUICKSEARCH:
        FinishQuickSearch();
        break;
    }

    m_EditInPlace.EnableWindow(FALSE);
    this->DeselectAllItems();

    return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikSourcesCtrl::OnItemEditCommit(WPARAM wParam, LPARAM lParam)
{
    int cmd = (int)lParam;
    CmusikEditInPlace* pSender = (CmusikEditInPlace*)wParam;
    CmusikPropTreeItem* pItem = this->m_PropTree->GetFocusedItem();
    CString sName = pSender->GetString();

    this->m_EditInPlaceItem->GetPlaylistInfo().m_Name = this->m_PreEditInPlaceText;
    this->m_EditInPlaceItem = false;

    switch (cmd)
    {
    case MUSIK_SOURCES_EDITINPLACE_RENAME:
        {
            int type = pItem->GetPlaylistType();
            switch (type)
            {
            case musikCore::MUSIK_PLAYLIST_TYPE_STANDARD:
                musikCube::g_Library->RenameStdPlaylist(pItem->GetPlaylistID(), (musikCore::String)sName);
                break;

            case musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC:
                musikCube::g_Library->RenameDynPlaylist(pItem->GetPlaylistID(), (musikCore::String)sName);
                break;
            }

            pItem->GetPlaylistInfo().m_Name = sName;
            Invalidate();
        }
        break;

    case MUSIK_SOURCES_EDITINPLACE_QUICKSEARCH:
        {
            if (pItem)
            {
                m_SearchItem->GetPlaylistInfo().m_Name = _T("Search...");
                FinishQuickSearch();
            }
        }
        break;

    case MUSIK_SOURCES_EDITINPLACE_NEWSTDPLAYLIST:
        {
            LoadStdPlaylistItems();
            pItem = m_StdPlaylistItems[m_StdPlaylistItems.size() - 2];
            //
            musikCube::g_Library->RenameStdPlaylist(pItem->GetPlaylistID(), (musikCore::String)sName);
            m_StdPlaylistItems[m_StdPlaylistItems.size() - 2]->GetPlaylistInfo().m_Name = sName;
            this->m_PropTree->UpdateScrollbar();
        }
        break;

    case MUSIK_SOURCES_EDITINPLACE_NEWDYNPLAYLIST:
        {
            this->DeselectAllItems();
            Invalidate();

            CmusikDynPlaylistDlg dlg(m_EditInPlace.GetString());
            int ret = dlg.DoModal();

            if (ret == IDOK)
            {
                musikCube::g_Library->CreateDynPlaylist(dlg.GetName(), dlg.GetQuery(), m_DynPlaylistItems.size() - 1);
                LoadDynPlaylistItems();
                this->m_PropTree->UpdateScrollbar();
            }
        }
        break;

    default:
        
        break;
    }

    m_EditInPlace.EnableWindow(FALSE);
    this->DeselectAllItems();

    SetFocus();
    return 0L;
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
    ScreenToClient(&point);
    m_RightClick = this->m_PropTree->FindItem(point);
	if (!(m_RightClick == NULL))
	{
    this->ShowDockBarMenu();
	}
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::EnableQuickSearch()
{   
    CPoint nPos = m_SearchItem->GetLocation();

    CRect rcClient;
    GetClientRect(rcClient);

    CString tmp = _T("");
    EditItemInPlace(m_SearchItem, MUSIK_SOURCES_EDITINPLACE_QUICKSEARCH, tmp);

    this->m_QuickStr = L"";

    int WM_SOURCESQUICKSEARCH = RegisterWindowMessage(_T("SOURCESQUICKSEARCH"));
    this->m_Parent->SendMessage(WM_SOURCESQUICKSEARCH);
}

///////////////////////////////////////////////////

LRESULT CmusikSourcesCtrl::OnItemEditChange(WPARAM wParam, LPARAM lParam)
{
    int cmd = (int)lParam;

    switch (lParam)
    {
    case MUSIK_SOURCES_EDITINPLACE_QUICKSEARCH:
        {
            CString editInPlaceText;
            m_EditInPlace.GetWindowText(editInPlaceText);

            if ((editInPlaceText != this->m_QuickStr) && (editInPlaceText != L""))
            {
                this->m_QuickStr = editInPlaceText;
                //
                if (this->m_SearchTimerID) { this->KillTimer(this->m_SearchTimerID); }
                this->m_SearchTimerID = this->SetTimer(SOURCES_SEARCH_TIMER_ID, 700, NULL);
            }
        }
        break;
    }

    return 0L;
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::FinishQuickSearch()
{
    this->KillTimer(m_SearchTimerID);
    this->m_SearchItem->GetPlaylistInfo().m_Name = L"Search...";
    this->m_SearchItem->Select(FALSE);
    this->m_PropTree->SetFocusedItem(NULL);
    this->DeselectAllItems(true);
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnDragLeave()
{
    CmusikPropTree::m_Hovered = NULL;
    RedrawWindow();
}

///////////////////////////////////////////////////

DROPEFFECT CmusikSourcesCtrl::OnDragOver(CPoint point)
{
    DROPEFFECT dwReturn = DROPEFFECT_NONE;
    CmusikPropTreeItem* pItem = this->m_PropTree->FindItem(point);

    if (pItem != NULL)
    {
        switch(pItem->GetPlaylistType())
        {
        case musikCore::MUSIK_PLAYLIST_TYPE_REMOVABLE:
        case musikCore::MUSIK_PLAYLIST_TYPE_LIBRARY:
        case MUSIK_SOURCES_TYPE_COLLECTED:
            dwReturn = DROPEFFECT_COPY;
            break;

        case musikCore::MUSIK_PLAYLIST_TYPE_STANDARD:

            dwReturn = DROPEFFECT_COPY;
            //
            if  (m_DragAndDropInfo != NULL)
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
            if  (m_DragAndDropInfo != NULL)
            {
                // disallow drag and drop from Now Playing to Now Playing
                if (m_DragAndDropInfo->GetType() == pItem->GetPlaylistType())
                {
                    dwReturn = DROPEFFECT_NONE;
                }
            }
            //
            break;
        }

        if (pItem != CmusikPropTree::m_Hovered)
        {
            CmusikPropTree::m_Hovered = pItem;
            RedrawWindow();
        }
    }

    return dwReturn;
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::EditItemInPlace(CmusikPropTreeItem* pItem, int args, const CString text)
{
    if (pItem)
    {
        CPoint nPos = pItem->GetLocation();
        CRect rcClient;
        GetClientRect(rcClient);

        int left;
        musikCube::g_DrawGraphics ? left = 32 : left = 12;
        int scrollPos = this->m_PropTree->GetScrollPosition();
        CRect rect(left, nPos.y + 4 - scrollPos, rcClient.Width() - 1, nPos.y + PROPTREEITEM_DEFHEIGHT - 2 - scrollPos);

        if (this->m_PropTree->IsScrollVisible())
        {
            rect.right -= 5;
        }

        this->m_EditInPlaceItem = pItem;
        this->m_PreEditInPlaceText = this->m_EditInPlaceItem->GetPlaylistInfo().m_Name;
        this->m_EditInPlaceItem->GetPlaylistInfo().m_Name = L"";

        m_EditInPlace.EnableWindow(TRUE);
        m_EditInPlace.SetArgs((void*)args);
        m_EditInPlace.MoveWindow(rect);
        m_EditInPlace.SetFocus();
        m_EditInPlace.SetString(text);
        m_EditInPlace.ShowWindow(SW_SHOWDEFAULT);
    }
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnMenuRipSelectedCD()
{
    CmusikRipDlg* ripDialog;
    CmusikPropTreeItem* pItem = this->m_PropTree->GetFocusedItem();

    if (pItem && pItem->GetPlaylistType() == musikCore::MUSIK_PLAYLIST_TYPE_CDPLAYER)
    {
        ripDialog = new CmusikRipDlg(this, this->m_PropTree->GetFocusedItem()->GetPlaylistID());
    }
    else
    {
        ripDialog = new CmusikRipDlg(this);
    }

    ripDialog->DoModal();

    delete ripDialog;
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnMenuRefreshCD()
{
    int WM_SOURCESCDAUDIO = RegisterWindowMessage(_T("SOURCESCDAUDIO"));
    m_Parent->SendMessage(WM_SOURCESCDAUDIO, this->m_PropTree->GetFocusedItem()->GetPlaylistID());
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnMenuDeviceUnloadLibrary()
{
    CmusikPropTreeItem *pSel = this->m_RightClick;
    if (pSel)
    {
        pSel->UnloadLibrary();
        this->DeselectAllItems();
        this->FocusLibrary();
    }

}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnMenuMoveItemUp()
{
    this->MoveSelectedItemUp();
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnMenuMoveItemDown()
{
    this->MoveSelectedItemDown();
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::MoveSelectedItemUp()
{
    CmusikPropTreeItem* pItem = this->m_PropTree->GetFocusedItem();
    if (pItem && pItem->GetPrev())
    {
        // moving up takes the current playlist and decreases it's
        // order, then increases the other playlist's order.
        switch(pItem->GetPlaylistType())
        {
        case musikCore::MUSIK_PLAYLIST_TYPE_STANDARD:
        case musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC:
            {
                CmusikPropTreeItem* pA = pItem;
                CmusikPropTreeItem* pB = pItem->GetPrev();

                int a = pA->GetPlaylistInfo().m_Order;
                int b = pB->GetPlaylistInfo().m_Order;

                pA->GetPlaylistInfo().m_Order = b;
                pB->GetPlaylistInfo().m_Order = a;

                musikCube::g_Library->UpdatePlaylistOrder(pA->GetPlaylistInfo());
                musikCube::g_Library->UpdatePlaylistOrder(pB->GetPlaylistInfo());

                // move A above B
                pA->Detach();
                CmusikPropTreeItem* pFirst = pB->GetPrev();
                pB->SetPrev(pA);
                pA->SetNext(pB);
                pA->SetPrev(pFirst);

                if (pFirst)
                    pFirst->SetNext(pA);

                if (pB->GetParent()->GetChild() == pB)
                    pB->GetParent()->SetChild(pA);

                Invalidate();
            }
            break;
        }
    }
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::MoveSelectedItemDown()
{
    CmusikPropTreeItem* pItem = this->m_PropTree->GetFocusedItem();
    if (pItem && pItem->GetNext())
    {
        // moving up takes the current playlist and decreases it's
        // order, then increases the other playlist's order.
        switch(pItem->GetPlaylistType())
        {
        case musikCore::MUSIK_PLAYLIST_TYPE_STANDARD:
        case musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC:
            {
                CmusikPropTreeItem* pA = pItem;
                CmusikPropTreeItem* pB = pItem->GetNext();
    
                // need to make sure we don't move the item below "Create..."
                if (m_StdPlaylistItems.size() && pB == m_StdPlaylistItems.at(m_StdPlaylistItems.size() - 1))
                    return;
                if (m_DynPlaylistItems.size() && pB == m_DynPlaylistItems.at(m_DynPlaylistItems.size() - 1))
                    return;

                // swap the order in the database
                int a = pA->GetPlaylistInfo().m_Order;
                int b = pB->GetPlaylistInfo().m_Order;

                pA->GetPlaylistInfo().m_Order = b;
                pB->GetPlaylistInfo().m_Order = a;

                musikCube::g_Library->UpdatePlaylistOrder(pA->GetPlaylistInfo());
                musikCube::g_Library->UpdatePlaylistOrder(pB->GetPlaylistInfo());

                // move B above A
                pB->Detach();        
                CmusikPropTreeItem* pFirst = pA->GetPrev();
                pA->SetPrev(pB);
                pB->SetNext(pA);
                pB->SetPrev(pFirst);

                if (pFirst)
                    pFirst->SetNext(pB);

                if (pA->GetParent()->GetChild() == pA)
                    pA->GetParent()->SetChild(pB);

                Invalidate();
            }
            break;
        }
    }
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnSourcesNowPlayingClear()
{
    musikCube::g_Player->GetPlaylist()->Clear();
    this->FocusNowPlaying();    // forces update of playlist in main ui
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnMenuPluginExecute()
{
    CmusikPropTreeItem* pFocus = this->m_PropTree->GetFocusedItem();
    if (pFocus)
    {
        musikCube::g_Plugins.at(pFocus->GetPlaylistID()).Execute();
    }
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnMenuPluginConfigure()
{
    CmusikPropTreeItem* pFocus = this->m_PropTree->GetFocusedItem();
    if (pFocus)
    {
        musikCube::g_Plugins.at(pFocus->GetPlaylistID()).Configure();
    }
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnMenuPluginStop()
{
    CmusikPropTreeItem* pFocus = this->m_PropTree->GetFocusedItem();
    if (pFocus)
    {
        musikCube::g_Plugins.at(pFocus->GetPlaylistID()).Stop();
    }
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnMenuPluginAbout()
{
    CmusikPropTreeItem* pFocus = this->m_PropTree->GetFocusedItem();
    if (pFocus)
        musikCube::g_Plugins.at(pFocus->GetPlaylistID()).About();
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnMenuLibraryShowAllSongs()
{
    this->FocusLibrary();
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnMenuSynchronizeDevice()
{
    if (GetFocusedItem()->m_Library)
    {
        musikCube::SynchronizeDirs(GetFocusedItem()->m_Library);
    }
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::LoadCollectedLibrary()
{
    if (m_CollectedItem->m_Library)
    {
        m_CollectedItem->UnloadLibrary();
    }

    CString oldpath_rel, fn;
    oldpath_rel = _T("~/.musikproject/musik_collected_u.db");
    fn = CmusikCubeApp::RelToAbs(oldpath_rel);

    musikCore::Library* pNewLib = new musikCore::Library(fn.GetBuffer());

    m_CollectedItem->m_Library = pNewLib;
    musikCube::g_Collected = pNewLib;
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::NotifyParentItemChanged(CmusikPropTreeItem* pItem)
{
    if (pItem)
    {
        // root level items are for display purposes only. we also don't wanwt
        if (pItem->IsRootLevel())
        {
            return;
        }

        int playlistType = pItem->GetPlaylistType();

        // Send the MainFrame a preliminary notification, letting it know that the
        // selection has changed. In the future this should be the only message we
        // send to the MainFrame, and we should provide an optional default playlist.
        int WM_SOURCESITEMCLICKED = RegisterWindowMessage(_T("SOURCESITEMCLICKED"));
        m_Parent->SendMessage(WM_SOURCESITEMCLICKED, NULL);

        switch (playlistType)
        {
        case musikCore::MUSIK_PLAYLIST_TYPE_LIBRARY:
            {
                SetMainLibrary(musikCube::g_MainLibrary);
                this->LoadDynPlaylistItems();
                this->LoadStdPlaylistItems();

                int WM_SOURCESLIBRARY = RegisterWindowMessage(_T("SOURCESLIBRARY"));
                m_Parent->SendMessage(WM_SOURCESLIBRARY, NULL);
            }
            break;

        case musikCore::MUSIK_PLAYLIST_TYPE_REMOVABLE:
        case MUSIK_SOURCES_TYPE_COLLECTED:                  // surprise! its just a device!
            {
                if ( ! pItem->m_Library)
                {
                    switch (playlistType)
                    {
                    case musikCore::MUSIK_PLAYLIST_TYPE_REMOVABLE:
                        this->LoadRemovableLibrary(pItem);
                        break;

                    case MUSIK_SOURCES_TYPE_COLLECTED:
                        this->LoadCollectedLibrary();
                        break;
                    }
                }

                if ( (! pItem) || ( ! pItem->m_Library)) { return; }

                ::SetMainLibrary(pItem->m_Library);
                this->LoadStdPlaylistItems();
                this->LoadDynPlaylistItems();
                //
                int WM_SOURCESREMOVABLEDEV = RegisterWindowMessage(_T("SOURCESREMOVABLEDEV"));
                m_Parent->SendMessage(WM_SOURCESREMOVABLEDEV, NULL);

                this->TellUserAboutCollected();
            }
            break;

        case musikCore::MUSIK_PLAYLIST_TYPE_NOWPLAYING:
            {
                int WM_SOURCESNOWPLAYING = RegisterWindowMessage(_T("SOURCESNOWPLAYING"));
                m_Parent->SendMessage(WM_SOURCESNOWPLAYING, NULL);
            }
            break;

        case musikCore::MUSIK_PLAYLIST_TYPE_STANDARD:
            {
                int WM_SOURCESSTDPLAYLIST = RegisterWindowMessage(_T("SOURCESSTDPLAYLIST"));
                m_Parent->SendMessage(WM_SOURCESSTDPLAYLIST, NULL);
            }
            break;

        case musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC:
            {
                int WM_SOURCESDYNPLAYLIST = RegisterWindowMessage(_T("SOURCESDYNPLAYLIST"));
                m_Parent->SendMessage(WM_SOURCESDYNPLAYLIST, NULL);
            }
            break;

        case MUSIK_SOURCES_TYPE_QUICKSEARCH:
            this->EnableQuickSearch();
            break;

        case MUSIK_SOURCES_TYPE_NEWSTDPLAYLIST:
            this->CreateNewStdPlaylist();
            break;

        case MUSIK_SOURCES_TYPE_NEWDYNPLAYLIST:
            this->CreateNewDynPlaylist();
            break;

        case MUSIK_SOURCES_TYPE_NETSTREAMS:
            {
                int WM_SOURCESNETSTREAMS = RegisterWindowMessage(_T("SOURCESNETSTREAMS"));
                m_Parent->SendMessage(WM_SOURCESNETSTREAMS, NULL);
            }
            break;

        case musikCore::MUSIK_PLAYLIST_TYPE_CDPLAYER:
            {
                int WM_SOURCESCDAUDIO = RegisterWindowMessage(_T("SOURCESCDAUDIO"));
                m_Parent->SendMessage(WM_SOURCESCDAUDIO, pItem->GetPlaylistID());
            }
            break;

        case MUSIK_SOURCES_TYPE_PLUGIN:
            {
                int pluginIndex = pItem->GetPlaylistID();
                musikCube::g_Plugins[pluginIndex].OnSources();
            }
            break;
        }
    }
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::LoadBitmaps()
{
    CmusikCubeApp* ptrApp = (CmusikCubeApp*)AfxGetApp();
    CString workingdir = ptrApp->GetWorkingDir();

    m_pLibOff = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\lib_uf.png"));
    m_pLibOn = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\lib_f.png"));

    m_pNPOff = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\np_uf.png"));
    m_pNPOn = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\np_f.png"));

    m_pQSOff = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\qs_uf.png"));
    m_pQSOn = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\qs_f.png"));

    m_pCDOff = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\cd_uf.png"));
    m_pCDOn = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\cd_f.png"));

    m_pPLOff = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\pl_uf.png"));
    m_pPLOn = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\pl_f.png"));

    m_pDPOff = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\dp_uf.png"));
    m_pDPOn = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\dp_f.png"));

    m_pRDOff = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\rd_uf.png"));
    m_pRDOn = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\rd_f.png"));

    m_pPlugOff = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\plug_uf.png"));
    m_pPlugOn = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\plug_f.png"));

    m_pNSOff = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\ns_uf.png"));
    m_pNSOn = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\ns_f.png"));

    m_pColOff = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\col_uf.png"));
    m_pColOn = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\col_f.png"));
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::DeviceTimerTick()
{
    this->RefreshDevices();
    this->m_PropTree->RefreshItems();
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::SearchTimerTick()
{
    int WM_SOURCESQUICKSEARCHSTR = RegisterWindowMessage(_T("SOURCESQUICKSEARCHSTR"));
    m_Parent->SendMessage(WM_SOURCESQUICKSEARCHSTR, (WPARAM)&m_QuickStr);

    this->KillTimer(this->m_SearchTimerID);
    this->m_SearchTimerID = NULL;
}


///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnTimer(UINT nIDEvent)
{
    if (nIDEvent == this->m_DevicesTimerID)
    {
        this->DeviceTimerTick();
    }
    else if (nIDEvent == this->m_SearchTimerID)
    {
        this->SearchTimerTick();
    }
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnMenuSynchronizedPaths()
{
    CmusikDirSync dirSyncDlg(this, this->GetFocusedItem()->m_Library);
    dirSyncDlg.DoModal();
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::TellUserAboutCollected()
{
    if ( ! musikCube::g_Prefs->GetUserKnowsAboutCollected())
    {
        MessageBox(
            L"It appears this is your first time using the \"Collected\" library.\n\r\n\r"
            L"\"Collected\" has its own set of standard and dynamic playlists, and can be thought\r\n"
            L"of as a \"holding tank\" for songs that you want to play, but don't necessarily want\r\n"
            L"in your main library yet (or at all).\n\r\n\r"
            L"This message will only be displayed once.",
            L"musikCube",
            MB_OK | MB_ICONINFORMATION);

        musikCube::g_Prefs->SetUserKnowsAboutCollected(true);
        musikCube::g_Prefs->SavePrefs();
    }
}