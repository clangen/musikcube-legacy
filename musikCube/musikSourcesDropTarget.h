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
//   CmusikSourcesDropTarget
//
// Filename(s): 
//
//   musikSourcesDropTarget.h, musikSourcesDropTarget.cpp
//
// Information:
//
//   A drop target for the "Sources" view to allow files from anywhere,
//   including windows explorer, to be dropped onto a CmusikSourcesCtrl.
// 
// Usage: 
//
//   Construct in a CmusikSourcesCtrl, then use 
//   CmusikSourcesDropTarget::Register() to invoke it, 
//   CmusikSourcesDropTarget::Revoke() to remove it.
//
// Some code adapted from:
//
//   Michael Dunn: http://www.codeproject.com/shell/explorerdragdrop.asp?target=drag%7C%2Bdrop
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

struct IDropTargetHelper;  
class CmusikSourcesCtrl;

///////////////////////////////////////////////////

class CmusikSourcesDropTarget : public COleDropTarget  
{
public:

	DROPEFFECT OnDragEnter ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	DROPEFFECT OnDragOver ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	BOOL OnDrop ( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point );
				
	void OnDragLeave ( CWnd* pWnd );

    CmusikSourcesDropTarget ( CmusikSourcesCtrl* pList, UINT uSourceID, UINT uPlaylistID_R, UINT uSelectionID_R );
	virtual ~CmusikSourcesDropTarget();

protected:

    CmusikSourcesCtrl*	m_pList;
    IDropTargetHelper*	m_piDropHelper;
	bool				m_RightButton;
	UINT				m_uSourceID;
	UINT				m_uPlaylistID_R;	// if the user holds the right mouse button from either the
	UINT				m_uSelectionID_R;	// selection boxes or the playlist, switch to the target...

    BOOL ReadHdropData ( COleDataObject* pDataObject );
};

///////////////////////////////////////////////////
