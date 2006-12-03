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

#include "musikCubeVersion.h"
#include "resource.h"       // main symbols
#include "musikPrefs.h"
#include "musikCubeFunctor.h"
#include <musikCore.h>

///////////////////////////////////////////////////

class CmusikCubeApp : public CWinApp
{
public:

    static CString RelToAbs(CString& path);
    static CString AbsToRel(CString& path);

    // construct and destruct
    CmusikCubeApp();
    virtual ~CmusikCubeApp();

    // overrides
    virtual BOOL InitInstance();
    virtual int ExitInstance();

    // message maps
    afx_msg void OnAppAbout();
    static CString GetUserDir();
    static CString GetWorkingDir(bool include_filename = false);

    // here for lack of a better place
    void GetCrossfader(musikCore::Crossfader* fader);

    // macros
    DECLARE_MESSAGE_MAP()

    void initCore();
    void deinitCore();
    void reloadLibrary();

private:

    static BOOL shared_IsRunning;
    static HWND shared_MainHwnd;
    ULONG_PTR m_gdiplusToken;
    void Play(const CString& fn);
};

///////////////////////////////////////////////////

void SetMainLibrary(musikCore::Library* library);

///////////////////////////////////////////////////


enum 
{
    PBSTRTYPE_NOWPLAYING_TITLE = 0,
    PBSTRTYPE_NOWPLAYING_ALBUM_ARTIST,
    PBSTRTYPE_TASKTRAY,
    PBSTRTYPE_BALLOONTITLE,
    PBSTRTYPE_BALLOONARTISTALBUM
};
CString GetPlaybackStr(int type);

///////////////////////////////////////////////////

struct CmusikPlugin
{

public: // typedefs
    
    typedef void    (*SETPLUGINLIBRARY)(musikCore::Library*);
    typedef void    (*INITIALIZE)(musikCore::Library*, musikCore::Player*, HWND);
    typedef void    (*UNLOAD)();
    typedef HMODULE    (*GETPLUGININSTANCE)();
    typedef void    (*SETPLUGININSTANCE)(HMODULE);
    typedef void    (*CONFIGURE)();
    typedef void    (*ABOUT)();
    typedef void    (*EXECUTE)();
    typedef void    (*STOP)();
    typedef bool    (*CANCONFIGURE)();
    typedef bool    (*CANABOUT)();
    typedef bool    (*CANEXECUTE)();
    typedef bool    (*CANSTOP)();
    typedef    void    (*SETVISIBLEPLAYLIST)(musikCore::Playlist*);
    typedef void    (*ONSOURCES)();
    typedef bool    (*INSOURCES)();
    typedef bool    (*WANTSPREFS)();
    typedef void    (*SETPREFS)(CIniFile*);
    typedef musikCore::Functor&            (*GETPLUGINFUNCTOR)();
    typedef const musikCore::String&    (*GETPLUGINNAME)();
    typedef const musikCore::String&    (*GETPLUGINDESCRIPTION)();
    typedef int        (*GETPLUGINREVISION)();

    int GetRevision(){ return m_Revision; }
    void SetRevision(int rev){ m_Revision = rev; }

public: // implementation

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

namespace musikCube
{
    extern musikCore::Player*           g_Player;
    extern musikCore::Library*          g_Library;
    extern musikCore::Library*          g_MainLibrary;
    extern musikCore::Library*          g_Collected;
    extern CmusikPrefs*                 g_Prefs;
    extern CmusikCubeFunctor*           g_Functor;
    extern bool                         g_DrawGraphics;
    extern std::vector<CmusikPlugin>    g_Plugins;

    void LoadPlugins();
    void UnloadPlugins();
    void SynchronizeDirs(musikCore::Library* library);
}

extern COLORREF ShadeColor(COLORREF inColor, int inShiftAmount);

///////////////////////////////////////////////////

extern CmusikCubeApp theApp;

///////////////////////////////////////////////////
