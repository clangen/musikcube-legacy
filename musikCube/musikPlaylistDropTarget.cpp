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
#include "musikPlaylistDropTarget.h"

#include "musikPlaylistCtrl.h"

///////////////////////////////////////////////////

CmusikPlaylistDropTarget::CmusikPlaylistDropTarget(CmusikPlaylistCtrl* pList, UINT uDropID_L, UINT uDropID_R, UINT uSourcesID, UINT uSelectionID_L, UINT uSelectionID_R)
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

DROPEFFECT CmusikPlaylistDropTarget::OnDragEnter (CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
    DROPEFFECT dwEffect = DROPEFFECT_NONE;

    // do not accept a drag and drop from these
    if  (  pDataObject->GetGlobalData(m_uSelectionID_L)  
         || pDataObject->GetGlobalData(m_uSelectionID_R)
         || pDataObject->GetGlobalData(m_uSourcesID)
         || pDataObject->GetGlobalData(m_uDropID_R)
       )
    {
        return DROPEFFECT_NONE;
    }

    if (pDataObject->GetGlobalData (m_uDropID_L))
    {
        // make sure there is really data to drop
        if (pDataObject->GetGlobalData (CF_HDROP) != NULL)
        {
            dwEffect = DROPEFFECT_COPY;
        }
    }

    // The DnD helper needs an IDataObject interface, which is
    // provide by the IDropTargetHelper.
    IDataObject* piDataObj = pDataObject->GetIDataObject (FALSE); 
    m_piDropHelper->DragEnter (pWnd->GetSafeHwnd(), piDataObj, &point, dwEffect);                                    

    return dwEffect;
}

///////////////////////////////////////////////////

DROPEFFECT CmusikPlaylistDropTarget::OnDragOver (CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
    // do not accept a drag and drop from these
    if  (  pDataObject->GetGlobalData(m_uSelectionID_L)  
         || pDataObject->GetGlobalData(m_uSelectionID_R)
         || pDataObject->GetGlobalData(m_uSourcesID)
         || pDataObject->GetGlobalData(m_uDropID_R)
       )
    {
        return DROPEFFECT_NONE;
    }

    if (pDataObject->GetGlobalData (m_uDropID_L))
    {
        m_pList->m_DropArrange = true;
    }
    else
    {
        m_pList->m_DropArrange = false;
    }

    // Look for CF_HDROP data in the data object, and accept the drop if
    // it's there.
    if (pDataObject->GetGlobalData (CF_HDROP) != NULL)
    {
        return m_pList->OnDragOver(point);
    }

    return DROPEFFECT_NONE;
}

///////////////////////////////////////////////////

DROPEFFECT CmusikPlaylistDropTarget::OnDragScroll(CWnd* pWnd, DWORD dwKeyState, CPoint point)
{
    CRect windowRect;
    m_pList->GetWindowRect(&windowRect);
    m_pList->ScreenToClient(&windowRect);

    CRect headerRect;
    m_pList->GetHeaderCtrl()->GetWindowRect(&headerRect);

    if (point.y < windowRect.top + 8 + headerRect.Height())
    {
        this->m_pList->Scroll(CSize(0, -16));
        return DROPEFFECT_SCROLL;
    }
    else if (point.y > windowRect.bottom - 8)
    {
        this->m_pList->Scroll(CSize(0, 16));
        return DROPEFFECT_SCROLL;
    }

    return DROPEFFECT_COPY;
}

///////////////////////////////////////////////////

BOOL CmusikPlaylistDropTarget::OnDrop (CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
    // The DnD helper needs an IDataObject interface, which is provided
    // by the the IDropTargetHelper
    IDataObject* piDataObj = pDataObject->GetIDataObject (FALSE); 
    m_piDropHelper->Drop(piDataObj, &point, dropEffect);

    // Read the CF_HDROP data call list's OnDrop()
    return ReadHdropData (pDataObject);
}

///////////////////////////////////////////////////

void CmusikPlaylistDropTarget::OnDragLeave (CWnd* pWnd)
{
    m_pList->OnDragLeave();
    m_piDropHelper->DragLeave();
}

///////////////////////////////////////////////////

// ReadHdropData() reads CF_HDROP data from the passed-in data object, and 
// puts all dropped files/folders into the main window's list control.

BOOL CmusikPlaylistDropTarget::ReadHdropData (COleDataObject* pDataObject)
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

    m_pList->OnDropFiles(hdrop);

    GlobalUnlock (hg);

    return TRUE;
}

///////////////////////////////////////////////////
