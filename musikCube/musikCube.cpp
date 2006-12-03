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

#include <OpenThreads/Thread>
using namespace OpenThreads;

///////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////

#pragma data_seg("SharedSection")
BOOL CmusikCubeApp::shared_IsRunning = FALSE;
HWND CmusikCubeApp::shared_MainHwnd = NULL;
#pragma data_seg()

#pragma comment(linker, "/SECTION:SharedSection,RWS")

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikCubeApp, CWinApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

namespace musikCube 
{
    musikCore::Player*          g_Player = NULL;
    musikCore::Library*         g_Library = NULL;
    musikCore::Library*         g_MainLibrary = NULL;
    musikCore::Library*         g_Collected = NULL;
    CmusikPrefs*                g_Prefs = NULL;
    CmusikCubeFunctor*          g_Functor = NULL;
    bool                        g_DrawGraphics = true;
    std::vector<CmusikPlugin>   g_Plugins;
}

///////////////////////////////////////////////////

void FindPlugins(musikCore::String dir, musikCore::StringArray* target)
{
    // stopping conditions
    if (dir.IsEmpty() || !target)
        return;

    // find the first file in the directory...
    WIN32_FIND_DATA lpFindFileData;
    HANDLE hDir = FindFirstFile(dir.c_str(), &lpFindFileData);

    // assure the first file exists...
    if (hDir != INVALID_HANDLE_VALUE)
    {
        musikCore::String fn, temp;
        musikCore::Filename MFN;
        do
        {
            if (lpFindFileData.cFileName[0] != '.')
            {
                // file is a directory
                if (lpFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    fn = dir.Left(dir.GetLength() - 3);    // remove *.* from full path
                    fn += lpFindFileData.cFileName;            // append to make new path

                    if (fn.Left(0) != _T("\\"))
                        fn += _T("\\*.*");                    // add \\*.* to the end of the new path
                    else
                        fn += _T("*.*");                        // add *.* to the end of the new path

                    FindPlugins(fn, target);
                }

                // file is a file...
                else
                {
                    fn = dir.Left(dir.GetLength() - 3);    // remove *.* from full path...
                    fn += lpFindFileData.cFileName;            // append filename

                    MFN.SetFilename(fn);
                    if (MFN.GetExtension() == _T("dll"))
                        target->push_back(fn);
                }
            }

        }
        while (FindNextFile(hDir, &lpFindFileData));

        FindClose(hDir);
    }
}

///////////////////////////////////////////////////

void musikCube::SynchronizeDirs(musikCore::Library* library)
{
    // tasks to add new files
    musikCore::StringArray synchs;
    library->GetAllPaths(synchs, false);
    for (size_t i = 0; i < synchs.size(); i++)
    {
        musikCore::StringArray* files = new musikCore::StringArray();

        // setup params
        musikCore::BatchAdd* params = new musikCore::BatchAdd(
            files, 
            synchs.at(i) + _T("*.*"),
            NULL, 
            library, 
            NULL, 
            musikCube::g_Functor, 
            0, 
            0, 
            1);

        // setup and start task
        musikCore::BatchAddTask* task = new musikCore::BatchAddTask;
        task->m_Params = params;
        library->QueueTask(task);
    }    

    // task to remove obselete entries
    musikCore::RemoveOld* params = new musikCore::RemoveOld(
        library, 
        musikCube::g_Functor);

    // spawn it
    musikCore::RemoveOldTask* task = new musikCore::RemoveOldTask();
    task->m_Params = params;
    library->QueueTask(task);
}

///////////////////////////////////////////////////

void musikCube::LoadPlugins()
{
    // make sure plugin directory exists
    musikCore::String path = CmusikCubeApp::GetWorkingDir() + _T("\\plugins\\");
    musikCore::Filename::RecurseMkDir(path.GetBuffer());

    // scan path
    path = CmusikCubeApp::GetWorkingDir() + _T("\\plugins\\*.*");
    musikCore::StringArray dllfiles;
    FindPlugins(path, &dllfiles);

    size_t count = 0;
    for (size_t i = 0; i < dllfiles.size(); i++)
    {
        HMODULE mod = LoadLibrary(dllfiles.at(i));
        
        CmusikPlugin plugin;

        plugin.m_Revision = 0;
        plugin.Configure = (CmusikPlugin::CONFIGURE)GetProcAddress(mod, "Configure");
        plugin.SetPluginLibrary = (CmusikPlugin::SETPLUGINLIBRARY)GetProcAddress(mod, "SetPluginLibrary");
        plugin.Initialize = (CmusikPlugin::INITIALIZE)GetProcAddress(mod, "Initialize");
        plugin.Unload = (CmusikPlugin::UNLOAD)GetProcAddress(mod, "Unload");
        plugin.Execute = (CmusikPlugin::EXECUTE)GetProcAddress(mod, "Execute");
        plugin.About = (CmusikPlugin::ABOUT)GetProcAddress(mod, "About");
        plugin.SetPluginInstance = (CmusikPlugin::SETPLUGININSTANCE)GetProcAddress(mod, "SetPluginInstance");
        plugin.GetPluginInstance = (CmusikPlugin::GETPLUGININSTANCE)GetProcAddress(mod, "GetPluginInstance");
        plugin.GetPluginName = (CmusikPlugin::GETPLUGINNAME)GetProcAddress(mod, "GetPluginName");
        plugin.GetPluginDescription = (CmusikPlugin::GETPLUGINDESCRIPTION)GetProcAddress(mod, "GetPluginDescription");
        plugin.Stop = (CmusikPlugin::STOP)GetProcAddress(mod, "Stop");
        plugin.GetPluginFunctor = (CmusikPlugin::GETPLUGINFUNCTOR)GetProcAddress(mod, "GetPluginFunctor");
        plugin.CanConfigure = (CmusikPlugin::CANCONFIGURE)GetProcAddress(mod, "CanConfigure");
        plugin.CanAbout = (CmusikPlugin::CANABOUT)GetProcAddress(mod, "CanAbout");
        plugin.CanExecute = (CmusikPlugin::CANCONFIGURE)GetProcAddress(mod, "CanExecute");
        plugin.CanStop = (CmusikPlugin::CANSTOP)GetProcAddress(mod, "CanStop");
        plugin.SetVisiblePlaylist = (CmusikPlugin::SETVISIBLEPLAYLIST)GetProcAddress(mod, "SetVisiblePlaylist");
        plugin.OnSources = (CmusikPlugin::ONSOURCES)GetProcAddress(mod, "OnSources");
        plugin.InSources = (CmusikPlugin::INSOURCES)GetProcAddress(mod, "InSources");
        plugin.WantsPrefs = (CmusikPlugin::WANTSPREFS)GetProcAddress(mod, "WantsPrefs");
        plugin.SetPrefs = (CmusikPlugin::SETPREFS)GetProcAddress(mod, "SetPrefs");
        plugin.GetPluginRevision = (CmusikPlugin::GETPLUGINREVISION)GetProcAddress(mod, "GetPluginRevision");

        if (plugin.Configure && plugin.SetPluginLibrary && plugin.Initialize && plugin.Unload &&
             plugin.Execute && plugin.About && plugin.SetPluginInstance && plugin.Stop && 
             plugin.GetPluginInstance && plugin.GetPluginName && plugin.GetPluginDescription && plugin.GetPluginFunctor &&
             plugin.CanConfigure && plugin.CanAbout && plugin.CanExecute && plugin.CanStop &&
             plugin.SetVisiblePlaylist && plugin.OnSources && plugin.InSources && plugin.WantsPrefs && plugin.SetPrefs)
        {
            plugin.Initialize(musikCube::g_Library, musikCube::g_Player, AfxGetApp()->GetMainWnd()->GetSafeHwnd());
            plugin.SetPluginInstance(mod);

            if (! (&plugin.GetPluginRevision == NULL))
                plugin.SetRevision(0);
            else
                plugin.SetRevision(plugin.GetPluginRevision());

            // this plugin wants to use preferences, so get them loaded.
            if (plugin.WantsPrefs())
            {
                musikCore::Filename MFN(dllfiles.at(i));
                musikCore::String inifn = MFN.GetPath();
                inifn += MFN.GetJustFilenameNoExt();
                inifn += _T(".ini");

                CIniFile* pini = new CIniFile(musikCore::utf16to8(inifn, false));
                plugin.SetPrefs(pini);
            }

            musikCube::g_Plugins.push_back(plugin);
        }
        else
            FreeLibrary(mod);
    }
}

///////////////////////////////////////////////////

void musikCube::UnloadPlugins()
{
    for (size_t i = 0; i < musikCube::g_Plugins.size(); i++)
    {
        musikCube::g_Plugins.at(i).Stop();
        musikCube::g_Plugins.at(i).Unload();    
        FreeLibrary(musikCube::g_Plugins.at(i).GetPluginInstance());
    }

    musikCube::g_Plugins.clear();
}

///////////////////////////////////////////////////

CmusikCubeApp::CmusikCubeApp()
{
    Thread::Init();

    CString fontStr = GetWorkingDir() + _T("\\musikCube.ttf");
    if (AddFontResourceEx(fontStr, FR_PRIVATE, 0) <= 0)
    {
        MessageBox(
            NULL,
            _T("Failed to register musikCube rating font."),
            _T("musikCube"),
            MB_ICONWARNING
       );
    }
    else
    {
        ::PostMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
    }
}

///////////////////////////////////////////////////

void CmusikCubeApp::reloadLibrary()
{
    musikCore::String path = musikCube::g_Prefs->GetLibraryPath();
    CString realpath = RelToAbs(CString(path)); 
    musikCube::g_MainLibrary->SetFilename(musikCore::String(realpath));
}

///////////////////////////////////////////////////

void CmusikCubeApp::initCore()
{
    // assure user directory exists
    CString user_dir = GetUserDir();
    user_dir += _T("\\.musikproject\\");
    musikCore::Filename::RecurseMkDir(user_dir.GetBuffer(NULL));

    musikCube::g_Functor = new CmusikCubeFunctor(GetMainWnd());

    musikCore::String inifile = user_dir + _T("musikCube_u.ini");
    musikCube::g_Prefs = new CmusikPrefs(inifile);

    musikCube::g_MainLibrary = new musikCore::Library("");

    musikCore::String oldpath_rel = _T("~/.musikproject/musik.db");
    CString oldpath_abs = RelToAbs(CString(oldpath_rel));
    if (musikCore::Filename::FileExists(oldpath_abs.GetBuffer()))
    {
        MessageBox(
            NULL,
            _T("It appears that you had a version of musikCube prior to 0.90 previously installed. ")
            _T("Version 0.90 and 0.91 use a radically different database format, which means your library ")
            _T("will need to be rebuilt. The old library will maintain intact in the following ")
            _T("path:\n\nc:\\Documents and Settings\\username\\.musikproject\\musik.db.old.\n\n")
            _T("This file can be loaded as an external database in all versions of musikCube ")
            _T("prior to 0.91.\n\nSorry for any inconvenience this may cause."),
            _T("musikCube"),
            MB_OK | MB_ICONINFORMATION);

        CString rnm = oldpath_abs + _T(".old");
        _wrename(oldpath_abs.GetBuffer(), rnm);
    }

    oldpath_rel = musikCore::utf8to16(musikCube::g_Prefs->GetLibraryPath());
    oldpath_abs = RelToAbs(CString(oldpath_rel));

    reloadLibrary();

#if defined (USE_FMOD)
    musikCube::g_Player    = new musikCore::PlayerFMOD(
        musikCube::g_Functor, 
        musikCube::g_MainLibrary);
#else 
#if defined (USE_BASS)
    musikCube::g_Player    = new musikCore::PlayerBASS(
        musikCube::g_Functor, 
        musikCube::g_MainLibrary);
#endif

    musikCore::SetPlayer(musikCube::g_Player);

    musikCube::g_Player->SetBufferSize(musikCube::g_Prefs->GetPlayerBuffer());
    musikCube::g_Player->SetMaxVolume(musikCube::g_Prefs->GetPlayerVolume());
    musikCube::g_Player->InitSound(0, musikCube::g_Prefs->GetPlayerDriver(), musikCube::g_Prefs->GetPlayerRate());
    musikCube::g_Player->SetPlaymode(musikCube::g_Prefs->GetPlayerPlaymode());
    musikCube::g_Player->UseGlobalEQSettings(musikCube::g_Prefs->EqualizerUseGlobal());
    musikCube::g_Player->start();

    // give player a crossfader, it will take
    // care of loading equalizer settings itself...
    if (musikCube::g_Prefs->IsCrossfaderEnabled())
    {
        musikCore::Crossfader fade;
        GetCrossfader(&fade);

        musikCube::g_Player->EnableCrossfader();
        musikCube::g_Player->SetCrossfader(fade);
    }

    // enable the equalizer...
    float pre =  musikCube::g_Prefs->GetEqualizerMultiplier();
    if (musikCube::g_Prefs->IsEqualizerEnabled())
        musikCube::g_Player->EnableEqualizer(true, musikCube::g_Prefs->EqualizerUseGlobal());

    // set UTC
    musikCore::Library::SetUseUTC(musikCube::g_Prefs->GetUseUTC());
}

///////////////////////////////////////////////////

void CmusikCubeApp::GetCrossfader(musikCore::Crossfader* fader)
{
    if (musikCube::g_MainLibrary->GetDefaultCrossfader(*fader) != musikCore::MUSIK_LIBRARY_OK)
    {
        fader->Set(2.0f, 0.5f, 0.2f, 1.0f, 3.0f);
        musikCube::g_MainLibrary->UpdateDefaultCrossfader(*fader);
    }
}

///////////////////////////////////////////////////

CmusikCubeApp::~CmusikCubeApp()
{
    deinitCore();

    CString fontStr = GetWorkingDir() + _T("\\musikCube.ttf");
    if (RemoveFontResourceEx(fontStr, FR_PRIVATE, 0) == FALSE)
    {
        MessageBox(
            NULL,
            _T("Failed to unregister musikCube rating font."),
            _T("musikCube"),
            MB_ICONWARNING
       );
    }
    else
    {
        ::PostMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
    }
}

///////////////////////////////////////////////////

void CmusikCubeApp::deinitCore()
{
    int lastplayed = 0;
    if (musikCube::g_Player)
    {
        lastplayed = musikCube::g_Player->GetIndex();
        if ((size_t)lastplayed < musikCube::g_Player->GetPlaylist()->size())
            lastplayed = musikCube::g_Player->GetPlaylist()->at(lastplayed).GetID();
        else
            lastplayed = 0;

        musikCube::g_Player->Exit();

        // store now playing before exiting
        if (musikCube::g_Player->GetLibrary() == musikCube::g_MainLibrary)
            musikCube::g_MainLibrary->PopulateTempSongTable(*musikCube::g_Player->GetPlaylist());

        delete musikCube::g_Player;
        musikCube::g_Player = NULL;
    }

    if (musikCube::g_Prefs)        
    {
        musikCube::g_Prefs->SetDlgLastPlayed(lastplayed);
        delete musikCube::g_Prefs;
        musikCube::g_Prefs = NULL;
    }

    if (musikCube::g_MainLibrary)
        delete musikCube::g_MainLibrary;

    if (musikCube::g_Functor)    
    {
        delete musikCube::g_Functor;
        musikCube::g_Functor = NULL;
    }
}

///////////////////////////////////////////////////

CmusikCubeApp theApp;

///////////////////////////////////////////////////

CString CmusikCubeApp::GetUserDir()
{
    CString userdir;

    wchar_t buffer[2000];
    GetEnvironmentVariable(_T("USERPROFILE"), buffer, sizeof(buffer));

    userdir = buffer;
    return userdir;
}

///////////////////////////////////////////////////

CString CmusikCubeApp::GetWorkingDir(bool include_filename)
{
    CString path;

    wchar_t buffer[2000];
    GetModuleFileName(NULL, buffer, sizeof(buffer));
    path = buffer;

    if (!include_filename)
        path = path.Left(path.ReverseFind('\\'));

    return path;
}

///////////////////////////////////////////////////

BOOL CmusikCubeApp::InitInstance()
{
    // only allow a single instance to run
    if (shared_IsRunning)
    {
        if (shared_MainHwnd)
        {
            CString sCmdLine = m_lpCmdLine;
            if (!sCmdLine.IsEmpty())
            {
                if (sCmdLine.Left(1) == "\"")
                {
                    sCmdLine.Delete(0);
                    sCmdLine.Delete(sCmdLine.GetLength() - 1);

                    ATOM atom = ::GlobalAddAtom(sCmdLine);

                    int WM_PLAYSONG = RegisterWindowMessage(_T("PLAYSONG"));
                    SendMessage(shared_MainHwnd, WM_PLAYSONG, (WPARAM)atom, (LPARAM)0);
                }
            }
        }
        return FALSE;
    }

    shared_IsRunning = TRUE;

    AfxOleInit();
    InitCommonControls();
    SetRegistryKey(_T("musikCube"));

    // see if we opened a file
    CString sCmdLine = m_lpCmdLine;
    bool autostart = false;
    bool play_song = false;
    if (!sCmdLine.IsEmpty())
    {
        if (sCmdLine.Left(1) == "\"")
        {
            sCmdLine.Delete(0);
            sCmdLine.Delete(sCmdLine.GetLength() - 1);
            play_song = true;
        }

        else if (sCmdLine.Find(_T("--autostart")) > -1)
            autostart = true;

        if (sCmdLine.Find(_T("--nographics")) > -1)
            musikCube::g_DrawGraphics = false;
        else
            musikCube::g_DrawGraphics = true;
    }

    // load gdi plus
    if (musikCube::g_DrawGraphics)
    {
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
        CmusikSourcesCtrl::LoadBitmaps();
    }

    // such as the name of your company or organization
    // To create the main window, this code creates a new frame window
    // object and then sets it as the application's main window object
    CmusikCubeFrame* pFrame = new CmusikCubeFrame(autostart);
    m_pMainWnd = pFrame;

    initCore();
    musikCube::g_Library = musikCube::g_MainLibrary;

    if (!pFrame)
        return FALSE;

    // create and load the frame with its resources
    pFrame->LoadFrame(IDR_MAINFRAME,
        WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
        NULL);

    if (!autostart)
    {
        pFrame->ShowWindow(SW_SHOW);
        pFrame->UpdateWindow();

        if (musikCube::g_Prefs && musikCube::g_Prefs->SynchronizeOnStartup())
        {
            musikCube::SynchronizeDirs(musikCube::g_MainLibrary);
        }
    }
    else
    {
        pFrame->ShowWindow(SW_MINIMIZE);
        pFrame->ShowWindow(SW_HIDE);
        pFrame->m_IsMinimized = true;
    }

    shared_MainHwnd = pFrame->GetSafeHwnd();

    if (play_song)
        Play(sCmdLine);

    // call DragAcceptFiles only if there's a suffix
    //  In an SDI app, this should occur after ProcessShellCommand
    return TRUE;
}

///////////////////////////////////////////////////

void CmusikCubeApp::Play(const CString& fn)
{
    if (m_pMainWnd)
    {
        CmusikCubeFrame* pMain = (CmusikCubeFrame*) m_pMainWnd;
        pMain->PlayTrackFromExplorer(fn);
    }
}

///////////////////////////////////////////////////

class CAboutDlg : public CDialog
{
public:

    // construct and destruct
    CAboutDlg();

    // enumberations
    enum { IDD = IDD_ABOUTBOX };

protected:

    // overides
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    // macros
    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
};

///////////////////////////////////////////////////

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

///////////////////////////////////////////////////

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikCubeApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

///////////////////////////////////////////////////

CString CmusikCubeApp::RelToAbs(CString& path)
{
    CString abs = CmusikCubeApp::GetUserDir();
    CString ret(path);
    ret.Replace(_T("~"), abs);
    ret.Replace(_T("/"), _T("\\"));

    return ret;
}

///////////////////////////////////////////////////

CString CmusikCubeApp::AbsToRel(CString& path)
{
    CString rel = path;
    path.Replace(CmusikCubeApp::GetUserDir(), _T("~/"));  

    return path;
}

///////////////////////////////////////////////////

int CmusikCubeApp::ExitInstance()
{
    if (musikCube::g_DrawGraphics)
        Gdiplus::GdiplusShutdown(m_gdiplusToken);
    
    return CWinApp::ExitInstance();
}

///////////////////////////////////////////////////

void SetMainLibrary(musikCore::Library* library)
{
    musikCube::g_Library = library;

    // make sure the plugins reflect the active library
    for (size_t i = 0; i < musikCube::g_Plugins.size(); i++)
    {
        musikCube::g_Plugins.at(i).SetPluginLibrary(library);
    }
}

///////////////////////////////////////////////////


BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    CWnd* ptrWnd = GetDlgItem(IDC_EDITBOX);

    CString wndTxt =
        
        _T("-- Project Manager, Lead Programmer --\r\n\r\n")
        _T("Casey Langen\r\n")
        _T("casey\100musikcube\056com\r\n\r\n")

        _T("-- Webmaster --\r\n\r\n")
        _T("Rajiv Makhijani\r\n")
        _T("rajiv\100musikcube\056com\r\n\r\n")        

        _T("-- Documentation --\r\n\r\n")
        _T("Erik Carpenter (Urban)\r\n\r\n")

        _T("-- Installer --\r\n\r\n")
        _T("insomnia\r\n\r\n")

        _T("-- Artwork --\r\n\r\n")
        _T("derek985\r\n")
        _T("tango! icon set\r\n")
        _T("phlewp\r\n")
        _T("Mateo\r\n\r\n")

        _T("-- Contributors --\r\n\r\n")
        _T("Otto\r\n")
        _T("martind\r\n\r\n")
        _T("Yann Le Moigne\r\n")
        _T("#musik\r\n")
        _T("#coolpowers\r\n\r\n")

        _T("-- Translations --\r\n\r\n")
        _T("insomnia [German]\r\n")
        _T("Antoine Turmel [French]\r\n\r\n")

        _T("-- Quality Assurance --\r\n\r\n")
        _T("Dustin Carter\r\n")
        _T("Colby Langen\r\n")
        _T("Simon Windmill\r\n\r\n")

        _T("-- Donors --\r\n\r\n")
        _T("Sven Van Bouchout\r\n")
        _T("Dmitri Seals\r\n")
        _T("Jared Westgate\r\n")
        _T("DonTomaso\r\n\r\n")

        _T("-- Props --\r\n\r\n")
        _T("Malnilion\r\n")
        _T("DiGiTaLFX\r\n")
        _T("DocTriv\r\n")
        _T("melz\r\n")
        _T("willT\r\n")
        _T("Garmund Chan\r\n")
        _T("Jared Dickinson\r\n")
        _T("aliumalik\r\n")
        _T("Mondo\r\n")
        _T("PegaS\r\n")
        _T("Duncan Lock\r\n")
        _T("totaldarkness\r\n");

    ptrWnd->SetWindowText(wndTxt);

    CString version;

    ptrWnd = GetDlgItem(IDC_CUBEVER);
    version.Format(_T("musikCube: %s"), MUSIKCUBE_VER);    
    ptrWnd->SetWindowText(version);

    ptrWnd = GetDlgItem(IDC_SQLITEVER);
    version.Format(_T("sqlite3: %s"), SQLITE_VER);
    ptrWnd->SetWindowText(version);

    ptrWnd = GetDlgItem(IDC_OPENTHREADSVER);
    version.Format(_T("OpenThreads: %s"), OPENTHREADS_VER);
    ptrWnd->SetWindowText(version);

    ptrWnd = GetDlgItem(IDC_BASSVER);
    version.Format(_T("BASS: %s"), BASS_VER);
    ptrWnd->SetWindowText(version);

    return TRUE;
}

///////////////////////////////////////////////////

CString GetPlaybackStr(int type)
{
    musikCore::SongInfo* info = musikCube::g_Player->GetPlaying();
    bool album_empty = info->GetAlbum().IsEmpty();
    bool artist_empty = info->GetArtist().IsEmpty();
    bool title_empty = info->GetTitle().IsEmpty();
    CString set_str;

    /*
    if (type == PBSTRTYPE_NOWPLAYING)
    {
        if (!musikCube::g_Player->IsPlaying())
        {
            set_str += _T("%c not playing");
            goto finish;
        }
            
        if (title_empty)
            set_str += _T("%c unknown artist");    // no title
        else
            set_str += _T("%b14 %a4 ");            // title exists, so show it

        if (!artist_empty)
            set_str += _T("%c by %b14 %a0 ");        // show artist, if exists

        if (!album_empty)
            set_str += _T("%c from the album %b14 %a1");    // show album if exists

        if (set_str == _T("%c unknown artist"))
            set_str = _T("%c no song information available");
    }
    */

    if (type == PBSTRTYPE_NOWPLAYING_TITLE)
    {
        if (!musikCube::g_Player->IsPlaying())
            set_str += _T("%cNot Playing");

        else if (title_empty)
            set_str += _T("%cuntitled");

        else
            set_str += _T("%b18 %a4");
    }

    else if (type == PBSTRTYPE_NOWPLAYING_ALBUM_ARTIST)
    {
        if (!musikCube::g_Player->IsPlaying())
        {
            set_str += _T("%cNot Playing");
            goto finish;
        }

        if (!artist_empty)
            set_str += _T("%cby %b14 %a0 ");        // show artist, if exists

        if (!album_empty)
            set_str += _T("%c from the album %b14 %a1");    // show album if exists

        if (set_str.IsEmpty())
            set_str += _T("%cno song information available");
    }

    else if (type == PBSTRTYPE_TASKTRAY)
    {
        set_str += _T("[ ");
        
        if (!title_empty)
            set_str += info->GetTitle();

        if (!title_empty && !artist_empty)
            set_str += _T(" - ");

        if (!artist_empty)
            set_str += info->GetArtist();

        if (title_empty && artist_empty)
            set_str += _T("no song information available");

        set_str += _T(" ] ");
        set_str += MUSIK_VERSION_STR;
    }

    else if (type == PBSTRTYPE_BALLOONTITLE)
    {
        if (title_empty)
            set_str += _T("unknown title");
        else
            set_str +=    info->GetTitle();
    }

    else if (type == PBSTRTYPE_BALLOONARTISTALBUM)
    {
        if (!artist_empty)
        {
            set_str += _T("by \"");
            set_str += musikCube::g_Player->GetPlaying()->GetArtist();
            set_str += _T("\"");
        }

        if (!album_empty)
        {
            if (!artist_empty)
                set_str += _T(" ");
            set_str += _T("from the album \"");
            set_str += musikCube::g_Player->GetPlaying()->GetAlbum();
            set_str += _T("\"");
        }

        if (set_str.IsEmpty())
            set_str += _T("no song information available");
    }

finish:

    return set_str;
}

///////////////////////////////////////////////////

COLORREF ShadeColor(COLORREF inColor, int inShiftAmount)
{
    int rVal = GetRValue(inColor) + inShiftAmount;
    int gVal = GetGValue(inColor) + inShiftAmount;
    int bVal = GetBValue(inColor) + inShiftAmount;

    if (rVal > 255) rVal = 255;
    else if (rVal < 0) rVal = 0;

    if (gVal > 255) gVal = 255;
    else if (gVal < 0) gVal = 0;

    if (bVal > 255) bVal = 255;
    else if (bVal < 0) bVal = 0;

    return RGB(rVal, gVal, bVal);
}

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////


