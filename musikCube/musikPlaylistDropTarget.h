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
//   CmusikPlaylistDropTarget
//
// Filename(s): 
//
//   musikPlaylistDropTarget.h, musikPlaylistDropTarget.cpp
//
// Information:
//
//   A drop target for the playlist to allow files from anywhere,
//   including windows explorer, to be dropped onto a musikCore::Playlist.
// 
// Usage: 
//
//   Construct in a musikCore::Playlist, then use 
//   CmusikPlaylistDropTarget::Register() to invoke it, 
//   CmusikPlaylistDropTarget::Revoke() to remove it.
//
// Some code adapted from:
//
//   Michael Dunn: http://www.codeproject.com/shell/explorerdragdrop.asp?target=drag%7C%2Bdrop
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

struct IDropTargetHelper;  
class CmusikPlaylistCtrl;

///////////////////////////////////////////////////

class CmusikPlaylistDropTarget : public COleDropTarget  
{
	friend class CmusikPlaylistCtrl;

public:
	DROPEFFECT OnDragEnter ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	DROPEFFECT OnDragOver ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	BOOL OnDrop ( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point );

	DROPEFFECT OnDragScroll( CWnd* pWnd, DWORD dwKeyState, CPoint point );

	void OnDragLeave ( CWnd* pWnd );

    CmusikPlaylistDropTarget ( CmusikPlaylistCtrl* pList, UINT uDropID_L, UINT uDropID_R, UINT uSourcesID, UINT uSelectionID_L, UINT uSelectionID_R );
	virtual ~CmusikPlaylistDropTarget();

protected:

    CmusikPlaylistCtrl*	m_pList;
    IDropTargetHelper*	m_piDropHelper;
	UINT				m_uDropID_L;
	UINT				m_uDropID_R;
	UINT				m_uSourcesID;
	UINT				m_uSelectionID_L;
	UINT				m_uSelectionID_R;

    BOOL ReadHdropData ( COleDataObject* pDataObject );
};

///////////////////////////////////////////////////
