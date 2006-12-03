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

#include "../musikCube.h"
#include "../musikPrefs.h"

#include <musikCore.h>
#include <atlimage.h>
#include <Gdiplus.h>

///////////////////////////////////////////////////

class CmusikPropTree;

///////////////////////////////////////////////////

class CmusikPropTreeItem
{
public:

	// construct / destruct
	CmusikPropTreeItem();
	virtual ~CmusikPropTreeItem();

    // each item has playlist info, including
	// id and type...
	void SetPlaylistInfo( const musikCore::PlaylistInfo& info );
	musikCore::PlaylistInfo& GetPlaylistInfo() { return m_PlaylistInfo; }
    musikCore::Library* m_Library;
	int GetPlaylistType();
	int GetPlaylistID();
	void SetBitmaps( Gdiplus::Bitmap* off, Gdiplus::Bitmap* on );

	// treeItem states
	BOOL IsExpanded();
	BOOL IsSelected();
	BOOL IsActivated();

	// set states
	void Select(BOOL bSelect = TRUE);
	void Expand(BOOL bExpand = TRUE);
	void UnloadLibrary();

	// returns TRUE if the point is on the expand button
	BOOL HitExpand(const POINT& pt);

	// returns TRUE if the item is on the root level. 
	// root level items don't have attribute areas
	BOOL IsRootLevel();

	// returns the total height of the item and 
	// all its children
	LONG GetTotalHeight();

	// Return the items label text
	LPCTSTR GetLabelText();

	// Set the item's ID
	void SetCtrlID(UINT nCtrlID);

	// Return the item's ID
	UINT GetCtrlID();

	// draw the item's non attribute area
	LONG DrawItem(CDC* pDC, const RECT& rc, LONG x, LONG y);

	// call to mark attribute changes
	void CommitChanges();

	// call to activate item attribute
	void Activate();
	void Deactivate();

	//
	// Overrideables
	//

	// Return the height of the item
	virtual LONG GetHeight();

	// Retrieve the item's attribute value
	virtual LPARAM GetItemValue();

	// Set the item's attribute value
	virtual void SetItemValue(LPARAM lParam);

	// Called when attribute area has changed size
	virtual void OnMove();

	// Called when the item needs to refresh its data
	virtual void OnRefresh();

	// Called when the item needs to commit its changes
	virtual void OnCommit();

	// Called to activate the item
	virtual void OnActivate();

	//
	// Usually only CmusikPropTree should calls these
	//

	void SetPropOwner(CmusikPropTree* pProp);

	// Return the location of the PropItem
	const POINT& GetLocation();

	// TreeItem link pointer access
	CmusikPropTreeItem* GetParent();
	CmusikPropTreeItem* GetChild();
	CmusikPropTreeItem* GetNext();
	CmusikPropTreeItem* GetPrev(){ return m_pPrev; }

	// misc
	void SetParent(CmusikPropTreeItem* pParent);
	void SetChild(CmusikPropTreeItem* pChild);
	void SetNext(CmusikPropTreeItem* pNext );
	void SetPrev(CmusikPropTreeItem* pPrev );
	CmusikPropTreeItem* Detach();

protected:

	// CmusikPropTree class that this class belongs
	CmusikPropTree*			m_pProp;

	// TreeItem label name
	CString				m_sLabel;

	// TreeItem location
	CPoint				m_loc;

	// TreeItem attribute size
	CRect				m_rc;

	// user defined LPARAM value
	LPARAM				m_lParam;

	// ID of control item (should be unique)
	UINT				m_nCtrlID;

	// MUSIK SPECIFIC: item's playlist information
	musikCore::PlaylistInfo m_PlaylistInfo;
	Gdiplus::Bitmap* m_pBmpOff;
	Gdiplus::Bitmap* m_pBmpOn;

private:
	enum TreeItemStates
	{
		TreeItemSelected =		0x00000001,
		TreeItemExpanded =		0x00000002,
		TreeItemActivated =		0x00000010 // 0x00000020 = next
	};

	// TreeItem state
	DWORD				m_dwState;

	// TRUE if item is activated
	BOOL				m_bActivated;

	// TRUE if item has been commited once (activation)
	BOOL				m_bCommitOnce;

	// Rectangle position of the expand button (if contains one)
	CRect				m_rcExpand;

	// link pointers
	CmusikPropTreeItem*		m_pParent;
	CmusikPropTreeItem*		m_pChild;
	CmusikPropTreeItem*		m_pNext;
	CmusikPropTreeItem*		m_pPrev;
};

///////////////////////////////////////////////////
