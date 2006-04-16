///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#ifdef COMPILE_STANDALONE

///////////////////////////////////////////////////

#include <musikCore.h>
#include "musikPluginDlg.h"
#include "PlaylistWnd.h"

///////////////////////////////////////////////////

extern CMainDlg* g_Main;
extern CPlaylistWnd* g_PlaylistDlg;

class Functor : public musikCore::Functor
{
public:
	void Functor::OnNewSong() { if (g_Main != NULL) g_Main->RefreshDisplay(); }
	void Functor::OnPause() { if (g_Main != NULL) g_Main->RefreshDisplay(); }
	void Functor::OnResume() { if (g_Main != NULL) g_Main->RefreshDisplay(); }
	void Functor::OnStop() { if (g_Main != NULL) g_Main->RefreshDisplay(); }
	void Functor::OnPlaybackFailed() { if (g_Main != NULL) g_Main->RefreshDisplay(); }

};

extern musikCore::Player* g_Player;
extern musikCore::Library* g_Library;
extern Functor* g_Functor;

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
