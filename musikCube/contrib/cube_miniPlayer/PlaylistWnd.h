///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#if defined (COMPILE_STANDALONE)
#include <musikCore.h>

///////////////////////////////////////////////////

class CPlaylistWnd : public CDialog
{
public:

	CPlaylistWnd(CWnd* pParent = NULL);
	virtual ~CPlaylistWnd();
	enum { IDD = IDD_PLAYLIST };

	void SetPlaylist( musikCore::Playlist& playlist );

protected:

	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_DYNAMIC(CPlaylistWnd)
	DECLARE_MESSAGE_MAP()

	musikCore::SongInfoArray m_Info;
public:
	afx_msg void OnLbnDblclkPlbox();
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////

