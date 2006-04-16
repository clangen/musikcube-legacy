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

#include "stdafx.h"

#include "musikSourcesDropTarget.h"
#include "musikSourcesCtrl.h"

///////////////////////////////////////////////////

CmusikSourcesDropTarget::CmusikSourcesDropTarget( CmusikSourcesCtrl* pList, UINT uSourceID, UINT uPlaylistID_R, UINT uSelectionID_R  )
{
	m_pList = pList;
	m_piDropHelper = NULL;

	m_uSourceID = uSourceID;
	m_uPlaylistID_R = uPlaylistID_R;
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

CmusikSourcesDropTarget::~CmusikSourcesDropTarget()
{
	m_piDropHelper->Release();
}

///////////////////////////////////////////////////

DROPEFFECT CmusikSourcesDropTarget::OnDragEnter ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
{
	DROPEFFECT dwEffect = DROPEFFECT_NONE;

    // Check for our own custom clipboard format in the data object.  If it's
    // present, then the DnD was initiated from our own window, and we won't
    // accept the drop.
    // If it's not present, then we check for CF_HDROP data in the data object.
	if	(   pDataObject->GetGlobalData( m_uPlaylistID_R ) != NULL 
		 || pDataObject->GetGlobalData( m_uSelectionID_R ) != NULL 
		)
	{
		m_RightButton = true;
	}
	else
	{
		m_RightButton = false;
	}

    if ( pDataObject->GetGlobalData ( m_uSourceID ) == NULL )
	{
        // Look for CF_HDROP data in the data object, and accept the drop if
        // it's there.
        if ( pDataObject->GetGlobalData ( CF_HDROP ) != NULL )
		{
            dwEffect = DROPEFFECT_COPY;
		}
	}

	IDataObject* piDataObj = pDataObject->GetIDataObject ( FALSE ); 
	m_piDropHelper->DragEnter ( pWnd->GetSafeHwnd(), piDataObj, &point, dwEffect );                                    

    return dwEffect;
}

///////////////////////////////////////////////////

DROPEFFECT CmusikSourcesDropTarget::OnDragOver ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
{
	DROPEFFECT dwEffect = DROPEFFECT_NONE;

    // Check for our own custom clipboard format in the data object.  If it's
    // present, then the DnD was initiated from our own window, and we want to
	// rearrange items. Set the flag in the playlist control.
    if ( pDataObject->GetGlobalData ( m_uSourceID ) != NULL )
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
		dwEffect = DROPEFFECT_COPY;
	}


	return m_pList->OnDragOver( point );
}

///////////////////////////////////////////////////

BOOL CmusikSourcesDropTarget::OnDrop ( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point )
{
	IDataObject* piDataObj = pDataObject->GetIDataObject ( FALSE ); 
	m_piDropHelper->Drop ( piDataObj, &point, dropEffect );

    // Read the CF_HDROP data call list's OnDrop()
	return ReadHdropData ( pDataObject );
}

///////////////////////////////////////////////////

void CmusikSourcesDropTarget::OnDragLeave ( CWnd* pWnd )
{
	m_pList->OnDragLeave();
	m_piDropHelper->DragLeave();
}

///////////////////////////////////////////////////

// ReadHdropData() reads CF_HDROP data from the passed-in data object, and 
// puts all dropped files/folders into the main window's list control.

BOOL CmusikSourcesDropTarget::ReadHdropData ( COleDataObject* pDataObject )
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

	m_pList->OnDropFiles( hdrop, m_RightButton );

    GlobalUnlock ( hg );

    return TRUE;
}

///////////////////////////////////////////////////
