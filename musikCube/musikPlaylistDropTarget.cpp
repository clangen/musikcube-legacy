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

#include "stdafx.h"
#include "musikPlaylistDropTarget.h"

#include "musikPlaylistCtrl.h"

///////////////////////////////////////////////////

CmusikPlaylistDropTarget::CmusikPlaylistDropTarget( CmusikPlaylistCtrl* pList, UINT uDropID_L, UINT uDropID_R, UINT uSourcesID, UINT uSelectionID_L, UINT uSelectionID_R )
{
	m_pList = pList;
	m_piDropHelper = NULL;
	m_uDropID_L = uDropID_L;
	m_uDropID_R = uDropID_R;
	m_uSourcesID = uSourcesID;
	m_uSelectionID_L = uSelectionID_L;
	m_uSelectionID_R = uSelectionID_R;

    // Create an instance of the shell DnD helper object.
    CoCreateInstance ( 
		CLSID_DragDropHelper, 
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IDropTargetHelper,
		(void**) &m_piDropHelper
	);
}

///////////////////////////////////////////////////

CmusikPlaylistDropTarget::~CmusikPlaylistDropTarget()
{
	m_piDropHelper->Release();
}

///////////////////////////////////////////////////

DROPEFFECT CmusikPlaylistDropTarget::OnDragEnter ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
{
	DROPEFFECT dwEffect = DROPEFFECT_NONE;

	// do not accept a drag and drop from these
	if  (   pDataObject->GetGlobalData( m_uSelectionID_L )  
		 || pDataObject->GetGlobalData( m_uSelectionID_R )
		 || pDataObject->GetGlobalData( m_uSourcesID )
		 || pDataObject->GetGlobalData( m_uDropID_R )
		)
	{
		return DROPEFFECT_NONE;
	}

	if ( pDataObject->GetGlobalData ( m_uDropID_L ) )
	{
		// make sure there is really data to drop
		if ( pDataObject->GetGlobalData ( CF_HDROP ) != NULL )
		{
			dwEffect = DROPEFFECT_COPY;
		}
	}

	// The DnD helper needs an IDataObject interface, which is
	// provide by the IDropTargetHelper.
	IDataObject* piDataObj = pDataObject->GetIDataObject ( FALSE ); 
	m_piDropHelper->DragEnter ( pWnd->GetSafeHwnd(), piDataObj, &point, dwEffect );                                    

    return dwEffect;
}

///////////////////////////////////////////////////

DROPEFFECT CmusikPlaylistDropTarget::OnDragOver ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
{
	// do not accept a drag and drop from these
	if  (   pDataObject->GetGlobalData( m_uSelectionID_L )  
		 || pDataObject->GetGlobalData( m_uSelectionID_R )
		 || pDataObject->GetGlobalData( m_uSourcesID )
		 || pDataObject->GetGlobalData( m_uDropID_R )
		)
	{
		return DROPEFFECT_NONE;
	}

    if ( pDataObject->GetGlobalData ( m_uDropID_L ) )
	{
		m_pList->m_DropArrange = true;
	}
	else
	{
		m_pList->m_DropArrange = false;
	}

	// Look for CF_HDROP data in the data object, and accept the drop if
	// it's there.
	if ( pDataObject->GetGlobalData ( CF_HDROP ) != NULL )
	{
		return m_pList->OnDragOver( point );
	}

    return DROPEFFECT_NONE;
}

///////////////////////////////////////////////////

DROPEFFECT CmusikPlaylistDropTarget::OnDragScroll( CWnd* pWnd, DWORD dwKeyState, CPoint point )
{
	return DROPEFFECT_COPY;
}

///////////////////////////////////////////////////

BOOL CmusikPlaylistDropTarget::OnDrop ( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point )
{
	// The DnD helper needs an IDataObject interface, which is provided
	// by the the IDropTargetHelper
	IDataObject* piDataObj = pDataObject->GetIDataObject ( FALSE ); 
	m_piDropHelper->Drop( piDataObj, &point, dropEffect );

    // Read the CF_HDROP data call list's OnDrop()
	return ReadHdropData ( pDataObject );
}

///////////////////////////////////////////////////

void CmusikPlaylistDropTarget::OnDragLeave ( CWnd* pWnd )
{
	m_pList->OnDragLeave();
	m_piDropHelper->DragLeave();
}

///////////////////////////////////////////////////

// ReadHdropData() reads CF_HDROP data from the passed-in data object, and 
// puts all dropped files/folders into the main window's list control.

BOOL CmusikPlaylistDropTarget::ReadHdropData ( COleDataObject* pDataObject )
{
	HGLOBAL     hg;
	HDROP       hdrop;

	// Get the HDROP data from the data object.
    hg = pDataObject->GetGlobalData ( CF_HDROP );
	if ( hg == NULL )
	{
		return FALSE;
	}

	hdrop = (HDROP) GlobalLock ( hg );
	if ( hdrop == NULL )
	{
		GlobalUnlock ( hg );
		return FALSE;
	}

	m_pList->OnDropFiles( hdrop );

    GlobalUnlock ( hg );

    return TRUE;
}

///////////////////////////////////////////////////
