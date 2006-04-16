///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
//   multimedia library. More information at:
//
//     http://musik.berlios.de
//     http://musik.sourceforge.net
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2002-2004
//   Casey Langen   : Lead Developer, Project Manager. E-Mail: casey at musikcube dot com
//   Dustin Carter  : Developer
//   Simon Windmill : Developer
//
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////
//
// Class(es): 
//
//   CmusikPlaylistCtrl
//
// Filename(s): 
//
//   musikPlaylistCtrl.h, musikPlaylistCtrl.cpp
//
// Information:
//
//   musikCube's playlist control, based on a CmusikListCtrl
// 
// Usage: 
//
//   The control is virtual and fairly specialized, read
//   the header file for more information.
//
// Some code adapted from:
//
//   Michael Dunn: http://www.codeproject.com/shell/explorerdragdrop.asp?target=drag%7C%2Bdrop
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikCube.h"
#include "musikTagDlg.h"
#include "musikMaskDlg.h"
#include "musikListCtrl.h"
#include "musikDynDspInfo.h"

#include <musikCore.h>

///////////////////////////////////////////////////

class CmusikPlaylistDropTarget;
class CmusikPlaylistInfoCtrl;

///////////////////////////////////////////////////

enum
{
    PLAYLISTCTRL_CDRIPPER = 1,
    PLAYLISTCTRL_NORMAL = 0
};

///////////////////////////////////////////////////

class CmusikPlaylistCtrl : public CmusikListCtrl
{
	friend class CmusikPlaylistDropTarget;

public:

	// construct and destruct
	CmusikPlaylistCtrl();
	CmusikPlaylistCtrl( CFrameWnd* mainwnd, CmusikPlaylistDropTarget* pDropTarget, UINT dropid_l, UINT dropid_r );
	virtual ~CmusikPlaylistCtrl();

	// update the virtual list control
	void UpdateV( bool redraw = true, bool select_none = false );
	void ScrollToCurr();
	void ResyncItem( size_t songid, int item = -1 );

	// querying items
	void GetSelectedItems( musikCore::IntArray& items );
	void GetSelectedSongs( musikCore::Playlist& songs );
	void GetSelectedSongs( musikCore::SongInfoArray& songs );
	void GetSelectedSongs( musikCore::SongArray& songs );
	int GetFirstSelected();

	// stuff dealing with the playlist header
	int GetColumnPos( int field );
	void AddColumn( int field );
	void RemoveColumn( int field );
	void ToggleColumn ( int field );

	// call when new playlist is set becuase the
	// previous playlist's sorting should not apply
	// to the current one's...
	void HideSortArrow();
    
	// returns TRUE if user has deleted, inserted,
	// or otherwise modified a playlist that may
	// need to be saved...
	bool PlaylistNeedsSave(){ return m_PlaylistNeedsSave; }
	musikCore::Playlist* GetPlaylist(){ return m_Playlist; }
	void SetPlaylist( musikCore::Playlist* playlist );
	void SavePlaylist();

	// used by the main UI to trigger an "item activate"
	// event..
	virtual bool PlayItem( int n = -1 );

	void SetInfoCtrl( CmusikPlaylistInfoCtrl *ctrl );
    int GetType(){ return m_ViewType; }

	virtual void ShowContextMenu();
	virtual void ShowHeaderMenu();
    musikCore::SongInfoArray* m_SongInfoArray;

protected:

    virtual void Init( CFrameWnd* mainwnd, UINT dropid_l, UINT dropid_r );
	vector<int> m_Cols, m_Sizes;

	// is dnd active
	UINT m_DropID_L;
	UINT m_DropID_R;
	bool m_Dragging;

	// dialogs
	CmusikTagDlg* m_TagEdit;
	CmusikMaskDlg* m_MaskDlg;

	// internal play routines
	bool PlayItem_AddNP( int n = -1 );
	bool PlayItem_ReplaceNP( int n = -1 );

	// pointer to main window
	CFrameWnd* m_Parent;

	// pointer to info ctrl
	CmusikPlaylistInfoCtrl* m_InfoCtrl;

	// playlist stuff
	musikCore::Playlist* m_NowPlaying;
	DynDspInfo* m_SongInfoCache;

	musikCore::Playlist* m_Playlist;
    int m_ViewType;

	// drag and drop functions
	virtual void OnDragColumn( int source, int dest );
	void BeginDrag( NMHDR* pNMHDR, bool right_button );
	bool m_Arranging;

	// fonts
	void InitFonts();
	CFont m_StarFont;
	CFont m_ItemFont;
	CFont m_BoldFont;

	// rating ***** text extent
	void GetRatingExtent();
	int m_RatingExtent;
	int m_TwoSpace;

	// manipulating items..
	bool m_PlaylistNeedsSave;
	void GetItemIDs( const musikCore::IntArray& items, musikCore::IntArray* target );
	void DeleteItems( const musikCore::IntArray& items, bool update = true );
	void InsertItems( musikCore::Playlist& items, int first_sel, int at, bool update = true );
	void DeleteSelectedItems( bool from_library = false, bool from_computer = false );
	void RateSelectedItems( int rating );

	// mfc message maps
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnOdcachehint(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPaint();
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnMarqueeBegin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPlaylistcolumnsArtist();
	afx_msg void OnLvnBeginrdrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnEndtrack(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPlaylistcolumnsAlbum();
	afx_msg void OnPlaylistcolumnsYear();
	afx_msg void OnPlaylistcolumnsGenre();
	afx_msg void OnPlaylistcolumnsTitle();
	afx_msg void OnPlaylistcolumnsTracknumber();
	afx_msg void OnPlaylistcolumnsTimeadded();
	afx_msg void OnPlaylistcolumnsLastplayed();
	afx_msg void OnPlaylistcolumnsFilesize();
	afx_msg void OnPlaylistcolumnsFormat();
	afx_msg void OnPlaylistcolumnsDuration();
	afx_msg void OnPlaylistcolumnsRating();
	afx_msg void OnPlaylistcolumnsTimesplayed();
	afx_msg void OnPlaylistcolumnsBitrate();
	afx_msg void OnPlaylistcolumnsFilename();
	afx_msg void OnLvnKeydown(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPlcDeleteFromplaylist();
	afx_msg void OnPlcDeleteFromlibrary();
	afx_msg void OnPlcDeleteFromcomputer();
	afx_msg void OnPlaylistcontextmenuShufflenowplaying();
	afx_msg void OnPlaylistcontextmenuProperties();
    afx_msg void OnPlaylistcontextmenuTagClean();
	afx_msg LRESULT OnTagEditDestroy( WPARAM wParam = NULL, LPARAM lParam = NULL );
	afx_msg LRESULT OnTagNext( WPARAM wParam = NULL, LPARAM lParam = NULL );
	afx_msg LRESULT OnTagPrev( WPARAM wParam = NULL, LPARAM lParam = NULL );
	afx_msg LRESULT OnTagUpdate( WPARAM wParam = NULL, LPARAM lParam = NULL );
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPlaylistcontextmenuBatch();
	afx_msg void OnFilenameTagfromfilename();
	afx_msg void OnFilenameTagtofilename();
	afx_msg void OnPlaylistcontextmenuTagtolowercase();
	afx_msg void OnPlaylistcontextmenuTagtouppercase();
	afx_msg void OnTagAutocapitalize();
	afx_msg void OnRateN1();
	afx_msg void OnRateUn();
	afx_msg void OnRate1();
	afx_msg void OnRate2();
	afx_msg void OnRate3();
	afx_msg void OnRate4();
	afx_msg void OnRate5();
	afx_msg void OnPlaylistcontextmenuReloadtagsfromfile();
	afx_msg void OnPlaylistcolumnsComments();
	afx_msg void OnPlaylistcontextmenuRemovewhitespace();
	afx_msg void OnPlaylistcontextmenuQueueselectedinnowplaying();
	afx_msg void OnPlaylistcontextmenuRewritetagstofile();
	afx_msg void OnPlaylistcontextmenuPlayselected();
	afx_msg void OnPlaylistcontextmenuPlaynext();
	afx_msg void OnPlaylistcontextmenuReplacenowplaying();
	afx_msg void OnPlaylistcontextmenuShowinexplorer();
	afx_msg void OnPlaybackReplacenowplayingandplay();

	// drag and drop
	bool m_DropArrange;
	void OnDropFiles( HDROP hDropInfo );
	DROPEFFECT OnDragOver( CPoint point );
	void OnDragLeave();
	int m_DropOverIndex;

    virtual void UpdateItemCount();
	virtual musikCore::String GetItemAt( int at, int sub );
    virtual int GetSongID( int at );
	virtual CString GetTimeStr( int item );
    virtual void GetPlaylistCols();
    virtual void KeyPress( NMHDR *pNMHDR, LRESULT *pResult );
    virtual int GetSongCount(){ if ( m_Playlist ) return m_Playlist->size(); return 0; }
    virtual void RefreshTag( int no );

	// macros
	DECLARE_DYNAMIC(CmusikPlaylistCtrl)
	DECLARE_MESSAGE_MAP()

private:

	// misc
	int ShowMaskDlg( int state = MUSIK_MASK_TOFILENAME );

	// playlist column stuff
	void ResetColumns( bool update = true );
	void SaveColumns();
	CString GetRating( int item );

	// dnd stuff
	bool m_Marquee;
	size_t m_ClipboardFormat;

	// current sorted column
	void DrawSortArrow( int col );
	int m_LastCol;
	bool m_Ascend;

	CmusikPlaylistDropTarget* m_DropTarget;

};

///////////////////////////////////////////////////

class CmusikPlaylistCDCtrl : public CmusikPlaylistCtrl
{
public:

	CmusikPlaylistCDCtrl( CFrameWnd* mainwnd, UINT dropid_l, UINT dropid_r );

    virtual void Init( CFrameWnd* mainwnd, UINT dropid_l, UINT dropid_r );
    void SetSongInfoArray( musikCore::SongInfoArray* info );

    afx_msg void OnEditTrackInfo();
    afx_msg void OnEditAlbumInfo();
    void SetDiscDrive( int drive ){ m_Drive = drive; }
    int GetDiscDrive(){ return m_Drive; }

    bool m_CanPlay;

protected:

    virtual void UpdateItemCount();
	virtual musikCore::String GetItemAt( int at, int subitem );
    virtual int GetSongID( int at );
    virtual CString GetTimeStr( int item );
    virtual void GetPlaylistCols();
    virtual void ShowContextMenu();
    virtual void ShowHeaderMenu(){}
    virtual void KeyPress( NMHDR *pNMHDR, LRESULT *pResult ){ pResult = 0; }
    virtual int GetSongCount(){ if ( m_SongInfoArray ) return m_SongInfoArray->size(); return 0; }
    virtual bool PlayItem( int n = -1 );
    virtual void RefreshTag( int no );

    int m_Drive;

	// macros
    DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CmusikPlaylistCDCtrl)
};

///////////////////////////////////////////////////


