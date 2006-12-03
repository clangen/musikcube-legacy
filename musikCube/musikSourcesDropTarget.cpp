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

#include "stdafx.h"

#include "musikSourcesDropTarget.h"
#include "musikSourcesCtrl.h"

///////////////////////////////////////////////////

CmusikSourcesDropTarget::CmusikSourcesDropTarget(CmusikSourcesCtrl* pSourcesControl, UINT uSourceID, UINT uPlaylistID_R, UINT uSelectionID_R )
{
    m_pSourcesCtrl      = pSourcesControl;
    m_piDropHelper      = NULL;
    m_uSourceID         = uSourceID;
    m_uPlaylistID_R     = uPlaylistID_R;
    m_uSelectionID_R    = uSelectionID_R;

    // Create an instance of the shell DnD helper object.
    CoCreateInstance(
        CLSID_DragDropHelper, 
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_IDropTargetHelper,
        (void**) &m_piDropHelper);
}

///////////////////////////////////////////////////

CmusikSourcesDropTarget::~CmusikSourcesDropTarget()
{
    m_piDropHelper->Release();
}

///////////////////////////////////////////////////

DROPEFFECT CmusikSourcesDropTarget::OnDragEnter (CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
    DROPEFFECT dwEffect = DROPEFFECT_NONE;

    // Check for our own custom clipboard format in the data object.  If it's
    // present, then the DnD was initiated from our own window, and we won't
    // accept the drop.
    // If it's not present, then we check for CF_HDROP data in the data object.
    if    (  pDataObject->GetGlobalData(m_uPlaylistID_R) != NULL 
         || pDataObject->GetGlobalData(m_uSelectionID_R) != NULL 
       )
    {
        m_RightButton = true;
    }
    else
    {
        m_RightButton = false;
    }

    if (pDataObject->GetGlobalData (m_uSourceID) == NULL)
    {
        // Look for CF_HDROP data in the data object, and accept the drop if
        // it's there.
        if (pDataObject->GetGlobalData (CF_HDROP) != NULL)
        {
            dwEffect = DROPEFFECT_COPY;
        }
    }

    IDataObject* piDataObj = pDataObject->GetIDataObject (FALSE); 
    m_piDropHelper->DragEnter (pWnd->GetSafeHwnd(), piDataObj, &point, dwEffect);

    return dwEffect;
}

///////////////////////////////////////////////////

DROPEFFECT CmusikSourcesDropTarget::OnDragOver (CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
    return m_pSourcesCtrl->OnDragOver(point);
}

///////////////////////////////////////////////////

BOOL CmusikSourcesDropTarget::OnDrop (CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
    IDataObject* piDataObj = pDataObject->GetIDataObject(FALSE);
    m_piDropHelper->Drop (piDataObj, &point, dropEffect);

    // Read the CF_HDROP data call list's OnDrop()
    return ReadHdropData(pDataObject);
}

///////////////////////////////////////////////////

void CmusikSourcesDropTarget::OnDragLeave (CWnd* pWnd)
{
    m_pSourcesCtrl->OnDragLeave();
    m_piDropHelper->DragLeave();
}

///////////////////////////////////////////////////

// ReadHdropData() reads CF_HDROP data from the passed-in data object, and 
// puts all dropped files/folders into the main window's list control.

BOOL CmusikSourcesDropTarget::ReadHdropData (COleDataObject* pDataObject)
{
    HGLOBAL     hg;
    HDROP       hdrop;

    // Get the HDROP data from the data object.
    hg = pDataObject->GetGlobalData (CF_HDROP);
    if (hg == NULL)
    {
        return FALSE;
    }

    hdrop = (HDROP) GlobalLock (hg);
    if (hdrop == NULL)
    {
        GlobalUnlock (hg);
        return FALSE;
    }

    m_pSourcesCtrl->OnDropFiles(hdrop, m_RightButton);

    GlobalUnlock (hg);

    return TRUE;
}

///////////////////////////////////////////////////
