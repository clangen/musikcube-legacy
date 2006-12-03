///////////////////////////////////////////////////
// 
// Info:
//
//    ...
//
// Credits:
//
//    musikCube by Casey Langen
//    Plugin by Martin Dittus
//
///////////////////////////////////////////////////

#ifndef __C_MUSIKPLUGIN_H__
#define __C_MUSIKPLUGIN_H__

///////////////////////////////////////////////////

#ifndef COMPILE_STANDALONE

///////////////////////////////////////////////////

#include <musikCore.h>
#include "iniFile.h"

///////////////////////////////////////////////////

#ifdef MUSIKPLUGIN_EXPORTS
#define MUSIKPLUGIN_API __declspec(dllexport)
#else
#define MUSIKPLUGIN_API __declspec(dllimport)
#endif

///////////////////////////////////////////////////

extern "C" {

// forward declarations
void LoadPrefs();

///////////////////////////////////////////////////

// these can be implemented and/or initialized 

extern bool g_CanConfigure;
extern bool g_CanAbout;
//extern bool g_CanExecute;
//extern bool g_CanStop;
extern bool g_WantsPrefs;
extern bool g_InSources;
extern musikCore::String g_Name;
extern musikCore::String g_Description;    
extern musikCore::Playlist* g_VisiblePlaylist;
extern CIniFile *g_Prefs;
MUSIKPLUGIN_API void Configure();
MUSIKPLUGIN_API void About();
MUSIKPLUGIN_API void Execute();
MUSIKPLUGIN_API void Stop();
MUSIKPLUGIN_API void OnSources();
void OnPluginAttach();
void OnPluginDetach();

///////////////////////////////////////////////////

// override any of these that you need to, as they will get
// called automatically by musikCube so you can respond
// to common events from musikCore. feel free to add your
// own methods and variables to this class

class Functor : public musikCore::Functor
{
public:

    Functor(){}
    virtual ~Functor(){}

    virtual void OnNewSong();
    virtual void OnPause();
    virtual void OnResume();
    virtual void OnStop();
    virtual void OnTaskStart(){}
    virtual void OnTaskEnd( musikCore::Task* task_addr = NULL ){}
    virtual void OnTaskProgress( size_t progress, musikCore::Task* task_addr = NULL ){}
    virtual void OnPlaybackFailed();
    virtual void OnNewEqualizer(){}
    virtual void OnAlarmTrigger(){}
    virtual void OnSeek(){}

} g_Functor;

///////////////////////////////////////////////////

// *** YOU DO NOT WANT TO MODIFY ANY OF THESE ***
// musikCube uses them for communication between the
// plugin and the main UI.

extern HMODULE g_Instance;
extern musikCore::Library* g_Library;
extern musikCore::Player* g_Player;
extern HWND g_MainWnd;
extern int g_PluginRevision;

MUSIKPLUGIN_API    void SetPluginInstance( HMODULE instance );
MUSIKPLUGIN_API    void SetVisiblePlaylist( musikCore::Playlist* visible );
MUSIKPLUGIN_API void SetPrefs( CIniFile* prefs );
MUSIKPLUGIN_API    HMODULE GetPluginInstance();
MUSIKPLUGIN_API void SetPluginLibrary( musikCore::Library* lib );
MUSIKPLUGIN_API void Initialize( musikCore::Library*, musikCore::Player*, HWND mainwnd );
MUSIKPLUGIN_API const musikCore::String& GetPluginName();
MUSIKPLUGIN_API const musikCore::String& GetPluginDescription();
MUSIKPLUGIN_API void Unload();
MUSIKPLUGIN_API bool CanConfigure(){ return g_CanConfigure; }
MUSIKPLUGIN_API bool CanAbout(){ return g_CanAbout; }
MUSIKPLUGIN_API bool CanExecute(); /*{ return g_CanExecute; }*/
MUSIKPLUGIN_API bool CanStop(); /*{ return g_CanStop; }*/
MUSIKPLUGIN_API bool WantsPrefs(){ return g_WantsPrefs; }
MUSIKPLUGIN_API bool InSources(){ return g_InSources; }
MUSIKPLUGIN_API musikCore::Functor& GetPluginFunctor(){ return g_Functor; }
MUSIKPLUGIN_API int GetPluginRevision(){ return g_PluginRevision; }

///////////////////////////////////////////////////

}    // extern "C"

///////////////////////////////////////////////////

#endif    // COMPILE_STANDALONE

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////

