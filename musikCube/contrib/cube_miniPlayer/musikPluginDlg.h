///////////////////////////////////////////////////
// 
// Info:
//
//	...
//
// Credits:
//
//	musikCube by Casey Langen
//	Plugin by Martin Dittus
//
///////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "StaticLink.h"
#include "TrackSlider.h"
#include "EnterEdit.h"

#include <musikCore.h>
#include "afxwin.h"
#include "afxcmn.h"

// -- ... -----------------------------------

class CAboutDlg : public CDialog
{
	DECLARE_DYNAMIC(CAboutDlg)

public:
	CAboutDlg(LPCTSTR caption, CWnd* pParent = NULL);
	virtual ~CAboutDlg();

	enum { IDD = IDD_ABOUT };

	BOOL OnInitDialog();

	void SetPluginName(LPCTSTR pluginName);
	void SetLink(LPCTSTR link);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

	CString m_Caption;

	CStatic m_cPluginName;
	CStaticLink m_cLink;

	CFont m_BoldFont;

	CString m_pluginName;
	CString m_link;
};


// -- ... -----------------------------------


class CConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(LPCTSTR caption, CWnd* pParent = NULL); 
	virtual ~CConfigDlg();

	enum { IDD = IDD_CONFIG };

	BOOL OnInitDialog();

	void SetOntop(bool ontop) {	m_Ontop = ontop; }
	bool GetOntop() { return m_Ontop; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

	CString m_Caption;

	CButton m_cOntop;

	bool m_Ontop;
public:
	afx_msg void OnBnClickedConfigOntop();
};


// -- ... -----------------------------------

class CMainDlg : public CDialog
{
	DECLARE_DYNAMIC(CMainDlg)

public:

#ifdef COMPILE_STANDALONE
	CMainDlg( musikCore::Player* player );
#endif

	CMainDlg(LPCTSTR caption, musikCore::Player* player,  CWnd* pParent = NULL);
	virtual ~CMainDlg();

	enum { IDD = IDD_MAIN };

	// overrides
	BOOL OnInitDialog();

	void RefreshDisplay() { UpdateNowPlaying(); UpdateControls(); UpdateTime(); }

	void SetAlwaysOnTop(bool alwaysontop);
	bool GetAlwaysOnTop() { return m_AlwaysOnTop; }

	void SetShowRemaining(bool showRemaining) {	m_ShowRemaining = showRemaining; }
	bool GetShowRemaining() { return m_ShowRemaining; }

	void SetShowRating(bool showRating) {	m_ShowRating = showRating; }
	bool GetShowRating() { return m_ShowRating; }

	void SetWinPosXY(int xpos, int ypos);
	void GetWinPosXY(int &xpos, int &ypos);



	void SetAlbumMode(bool useAlbumMode) { m_AlbumMode = useAlbumMode; }
	bool GetAlbumMode() { return m_AlbumMode; }

	void SetAlbumLimit(int count) { m_AlbumLimit = count; }
	int GetAlbumLimit() { return m_AlbumLimit; }

	void SetAlbumQuery(CString query) { m_AlbumQuery = query; }
	CString GetAlbumQuery() { return m_AlbumQuery; }

	void SetQueryLimit(int count) { m_QueryLimit = count; }
	int GetQueryLimit() { return m_QueryLimit; }

	void SetQueryRndNew(CString query) { m_QueryRndNew = query; }
	CString GetQueryRndNew() { return m_QueryRndNew; }
	void SetQueryRndPop(CString query) { m_QueryRndPop = query; }
	CString GetQueryRndPop() { return m_QueryRndPop; }
	void SetQueryRndOld(CString query) { m_QueryRndOld = query; }
	CString GetQueryRndOld() { return m_QueryRndOld; }
	void SetQueryRndUnpl(CString query) { m_QueryRndUnpl = query; }
	CString GetQueryRndUnpl() { return m_QueryRndUnpl; }


	// search field has changed
	afx_msg LRESULT OnEditComplete(WPARAM, LPARAM);

	// Generated message map functions
    //{{AFX_MSG(CMainDlg)
	// context menu
	afx_msg void OnRandomNew();
	afx_msg void OnRandomPopular();
	afx_msg void OnRandomOld();
	afx_msg void OnSurpriseMe();
	afx_msg void OnAlbumMode();
	afx_msg void OnSongsByTheArtist();
	afx_msg void OnSongsFromTheAlbum();
	afx_msg void OnRandom();
	afx_msg void OnRepeatPlaylist();
	afx_msg void OnRepeatSong();
	afx_msg void OnIntro();
	afx_msg void OnShowRating();
	afx_msg void OnAlwaysOnTop();
	afx_msg void OnExit();

	// dynamic menu messages
	afx_msg BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo); 

	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	// controls
	afx_msg void OnEnSetfocusMainSearch();
	afx_msg void OnEnKillfocusMainSearch();

	afx_msg void OnBnClickedMainPrev();
	afx_msg void OnBnClickedMainPlay();
	afx_msg void OnBnClickedMainStop();
	afx_msg void OnBnClickedMainNext();

	afx_msg void OnStnClickedMainTime();
	afx_msg void OnStnClickedMainIndex();

	afx_msg LRESULT OnTrackSliderMoving(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTrackSliderMoved(WPARAM wParam, LPARAM lParam);

	// etc
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	// keyboard input
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	// mouse wheel
	afx_msg BOOL OnMouseWheel(UINT Flags, short zDelta, CPoint pt);

	// draggable window code
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnNcRButtonUp(UINT nHitTest, CPoint pt);
	//}}AFX_MSG

	afx_msg LRESULT OnEditEnter( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnEditEsc( WPARAM wParam, LPARAM lParam );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	// read all kinds of playing information from Player
	void ReadPlayerData();

	// called by timer
	void UpdateTime();

	// called on song change
	void UpdateNowPlaying();

	// called on state change
	void UpdateControls();

	// sends the playlist to "now playing"
	// if shift key is pressed: enqueue
	// else: replace current playlist, start playback from beginning
	// always: send NOWPLAYINGUPDATED message to parent
	void SendPlaylistAndPlay(musikCore::Playlist playlist);

	// queries the library and plays back the result
	// returns true if songs were found
	bool QueryAndPlay(CString query);
	virtual void OnOK(){}
	virtual void OnCancel(){}



	DECLARE_MESSAGE_MAP()

	CStatic m_cArtist;
	CStatic m_cTitle;
	CStatic m_cTime;
	CStatic m_cIndex;

	CFont m_BoldFont;

	CButton m_cPrev;
	CButton m_cPlay;
	CButton m_cStop;
	CButton m_cNext;

	HICON m_iPrev, m_iPlay, m_iPause, m_iStop, m_iNext;

	CEnterEdit m_cSearch;

	CTrackSlider m_cPosition;
	CTrackSlider m_cVolume;

	CString m_Caption;

	CFont m_DefaultFont, m_StarFont;

	musikCore::Player* m_Player;
	
	// note: actual window parent is the desktop; we use this CWnd to send window messages to the main app
	CWnd* m_AppWnd;

	UINT m_TimerID;

	int m_QueryLimit, m_AlbumLimit;
	CString m_QueryRndNew, m_QueryRndPop, m_QueryRndOld, m_QueryRndUnpl;
	CString m_AlbumQuery;

	bool m_AlbumMode;

	bool m_ShowRemaining;
	bool m_ShowRating;
	bool m_AlwaysOnTop;
	LONG m_XPos, m_YPos;
public:
	afx_msg void OnClose();
};


// -- ... -----------------------------------

class CmusikPluginApp : public CWinApp
{
public:
	CmusikPluginApp();



public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

