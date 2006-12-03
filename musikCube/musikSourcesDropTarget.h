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

#pragma once

///////////////////////////////////////////////////

struct IDropTargetHelper;
class CmusikSourcesCtrl;

///////////////////////////////////////////////////

class CmusikSourcesDropTarget : public COleDropTarget
{
public:

    /*ctor*/            CmusikSourcesDropTarget (CmusikSourcesCtrl* pSourcesCtrl, UINT uSourceID, UINT uPlaylistID_R, UINT uSelectionID_R);
    virtual /*dtor*/    ~CmusikSourcesDropTarget();

    DROPEFFECT          OnDragEnter (CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    DROPEFFECT          OnDragOver (CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    BOOL                OnDrop (CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
    void                OnDragLeave (CWnd* pWnd);

protected:

    BOOL                ReadHdropData (COleDataObject* pDataObject);

    CmusikSourcesCtrl*  m_pSourcesCtrl;
    IDropTargetHelper*  m_piDropHelper;
    bool                m_RightButton;
    UINT                m_uSourceID;
    UINT                m_uPlaylistID_R;        // if the user holds the right mouse button from either the
    UINT                m_uSelectionID_R;       // selection boxes or the playlist, switch to the target...
};

///////////////////////////////////////////////////
