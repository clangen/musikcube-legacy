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
#include "musikPropTree.h"

#include "../MEMDC.H"

///////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////

#pragma warning (disable : 4311) // pointer truncation from 'HMENU' to 'UINT'
#pragma warning (disable : 4312) // conversion from 'DWORD' to 'HBRUSH' of greater size

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC( CmusikPropTree, CWnd )

///////////////////////////////////////////////////

static const CString strOfficeFontName	= _T("Tahoma");
static const CString strDefaultFontName = _T("MS Sans Serif");

///////////////////////////////////////////////////

HINSTANCE ghInst;
CmusikPropTreeItem* CmusikPropTree::m_Hovered = NULL;

///////////////////////////////////////////////////

static int CALLBACK FontFamilyProcFonts(const LOGFONT FAR* lplf, const TEXTMETRIC FAR*, ULONG, LPARAM)
{
	ASSERT(lplf != NULL);
	CString strFont = lplf->lfFaceName;
	return strFont.CollateNoCase (strOfficeFontName) == 0 ? 0 : 1;
}

///////////////////////////////////////////////////

UINT CmusikPropTree::s_nInstanceCount;
CFont* CmusikPropTree::s_pNormalFont;
CFont* CmusikPropTree::s_pBoldFont;
CmusikPropTreeItem* CmusikPropTree::s_pFound;

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikPropTree, CWnd)
	ON_WM_ENABLE()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_VSCROLL()
	ON_WM_RBUTTONDOWN()
	ON_WM_NCCALCSIZE()
	ON_WM_SHOWWINDOW()

    // mouse
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

CmusikPropTree::CmusikPropTree( UINT dropid ) :
	m_ScrollPos(0),
	m_nLastUID(1),
	m_pFocus(NULL),
	m_BackBufferSize(0,0),
    m_MouseTrack(false)
{
	m_Root.Expand();

	// dropid
	m_DropID = dropid;
	m_IsWinNT = ( 0 == ( GetVersion() & 0x80000000 ) );

	// init global resources only once
	if (!s_nInstanceCount)
		InitGlobalResources();

	s_nInstanceCount++;
}

///////////////////////////////////////////////////

CmusikPropTree::~CmusikPropTree()
{
	DeleteAllItems(false);

	s_nInstanceCount--;

	// free global resource when ALL CmusikPropTrees are destroyed
	if (!s_nInstanceCount)
		FreeGlobalResources();
}

///////////////////////////////////////////////////

const int& CmusikPropTree::GetOrigin()
{
	return m_ScrollPos;
}

///////////////////////////////////////////////////

BOOL CmusikPropTree::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CWnd* pWnd = this;

	LPCTSTR pszCreateClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW));

	return pWnd->Create(pszCreateClass, _T(""), dwStyle, rect, pParentWnd, nID);
}

///////////////////////////////////////////////////

void CmusikPropTree::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	RecreateBackBuffer(cx, cy);

	UpdateScrollbar();
	Invalidate();
	UpdateWindow();

	// inform all items that a resize has been made
	UpdateMoveAllItems();
}

///////////////////////////////////////////////////

void CmusikPropTree::InitGlobalResources()
{
	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);

	LOGFONT lf;
	memset(&lf, 0, sizeof (LOGFONT));

	CWindowDC dc(NULL);
	lf.lfCharSet = (BYTE)GetTextCharsetInfo(dc.GetSafeHdc(), NULL, 0);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	// check if we should use system font
	_tcscpy(lf.lfFaceName, info.lfMenuFont.lfFaceName);

	BOOL fUseSystemFont = (info.lfMenuFont.lfCharSet > SYMBOL_CHARSET);
	if (!fUseSystemFont)
	{
		// check for "Tahoma" font existance:
		if (::EnumFontFamilies(dc.GetSafeHdc(), NULL, FontFamilyProcFonts, 0)==0)
		{
			// Found! Use MS Office font!
			_tcscpy(lf.lfFaceName, strOfficeFontName);
		}
		else
		{
			// Not found. Use default font:
			_tcscpy(lf.lfFaceName, strDefaultFontName);
		}
	}

	s_pNormalFont = new CFont;
	s_pNormalFont->CreateFontIndirect(&lf);

	lf.lfWeight = FW_BOLD;
	s_pBoldFont = new CFont;
	s_pBoldFont->CreateFontIndirect(&lf);
}

///////////////////////////////////////////////////

void CmusikPropTree::FreeGlobalResources()
{
	if (s_pNormalFont)
	{
		delete s_pNormalFont;
		s_pNormalFont = NULL;
	}

	if (s_pBoldFont)
	{
		delete s_pBoldFont;
		s_pBoldFont = NULL;
	}
}

///////////////////////////////////////////////////

CFont* CmusikPropTree::GetNormalFont()
{
	return s_pNormalFont;
}

///////////////////////////////////////////////////

CFont* CmusikPropTree::GetBoldFont()
{
	return s_pBoldFont;
}

///////////////////////////////////////////////////

CmusikPropTreeItem* CmusikPropTree::GetFocusedItem()
{
	return m_pFocus;
}

///////////////////////////////////////////////////

CmusikPropTreeItem* CmusikPropTree::GetRootItem()
{
	return &m_Root;
}

///////////////////////////////////////////////////

void CmusikPropTree::ClearVisibleList()
{
	m_pVisibleList.clear();
}

///////////////////////////////////////////////////

void CmusikPropTree::AddToVisibleList(CmusikPropTreeItem* pItem)
{
	m_pVisibleList.push_back( pItem );
}

///////////////////////////////////////////////////

BOOL CmusikPropTree::EnumItems(CmusikPropTreeItem* pItem, ENUMPROPITEMPROC proc, LPARAM lParam)
{
	if (!pItem || !proc)
		return FALSE;

	CmusikPropTreeItem* pNext;

	// don't count the root item in any enumerations
	if (pItem!=&m_Root && !proc(this, pItem, lParam))
		return FALSE;

	// recurse thru all child items
	pNext = pItem->GetChild();

	while (pNext)
	{
		if (!EnumItems(pNext, proc, lParam))
			return FALSE;

		pNext = pNext->GetNext();
	}

	return TRUE;
}

///////////////////////////////////////////////////

void CmusikPropTree::SetOriginOffset(LONG nOffset)
{
	m_ScrollPos = nOffset;
}

///////////////////////////////////////////////////
	
void CmusikPropTree::UpdatedItems()
{
	if (!IsWindow(m_hWnd))
		return;

	Invalidate();

	//UpdateScrollbar();
	Invalidate();
}

///////////////////////////////////////////////////

void CmusikPropTree::DeleteAllItems( bool notify )
{
	CmusikPropTreeItem* pFirst = m_Root.GetChild();

	while (pFirst)
	{
		CmusikPropTreeItem* pNext = pFirst->GetNext();
		Delete(pFirst);
		pFirst = pNext;
	}

    m_Root.SetChild(NULL);

	if ( notify )
	{
		UpdatedItems();
	}

	m_nLastUID = 1; // reset uid counter
}

///////////////////////////////////////////////////

void CmusikPropTree::DeleteItem(CmusikPropTreeItem* pItem, bool notify)
{
	Delete(pItem);

	if ( notify )
	{
		if (pItem && pItem!=&m_Root && SendNotify(PTN_DELETEITEM, pItem))
			return;
		UpdatedItems();
	}
}

///////////////////////////////////////////////////

void CmusikPropTree::DetachAllItems()
{
	CmusikPropTreeItem* pFirst = GetRootItem()->GetChild();
	while ( pFirst )
	{
		CmusikPropTreeItem* pNext = pFirst->GetNext();
		//
		pFirst->Detach();
		pFirst = pNext;
	}
}

///////////////////////////////////////////////////

void CmusikPropTree::Delete(CmusikPropTreeItem* pItem)
{
    if (pItem)
    {
        if (pItem->GetChild())
        {
            CmusikPropTreeItem* pChild = pItem->GetChild();
            while (pChild)
            {
                CmusikPropTreeItem* pNext = pChild->GetNext();
                Delete(pChild);
                pChild = pNext;
            }
        }

        if (GetFocusedItem() == pItem)
        {
            SetFocusedItem(NULL);
        }

        pItem->Detach();
        delete pItem;
    }
}

///////////////////////////////////////////////////

void CmusikPropTree::SetFocusedItem(CmusikPropTreeItem* pItem)
{
	m_pFocus = pItem;

	if ( pItem )
    {
        pItem->Select(TRUE);
		EnsureVisible(m_pFocus);
    }

	if (!IsWindow(m_hWnd))
		return;

	Invalidate();
}

///////////////////////////////////////////////////

BOOL CmusikPropTree::IsItemVisible(CmusikPropTreeItem* pItem)
{
	for ( size_t i = 0; i < m_pVisibleList.size(); i++ )
	{
		if ( pItem == m_pVisibleList.at( i ) )
			return TRUE;
	}
	return FALSE;
}

///////////////////////////////////////////////////

void CmusikPropTree::EnsureVisible(CmusikPropTreeItem* pItem)
{
	if (!pItem)
		return;

	// item is not scroll visible (expand all parents)
	if (!IsItemVisible(pItem))
	{
		CmusikPropTreeItem* pParent;

		pParent = pItem->GetParent();
		while (pParent)
		{
			pParent->Expand();
			pParent = pParent->GetParent();
		}

		UpdatedItems();
		UpdateWindow();
	}

	CRect rc;

	GetClientRect(rc);
	rc.OffsetRect(0, m_ScrollPos);
	rc.bottom -= pItem->GetHeight();

	CPoint pt;

	pt = pItem->GetLocation();

	if (!rc.PtInRect(pt))
	{
		LONG oy;

		//if (pt.y < rc.top)
			oy = pt.y;
		//else
		//	oy = pt.y - rc.Height() + pItem->GetHeight();

		OnVScroll(SB_THUMBTRACK, oy, NULL);
	}
}

///////////////////////////////////////////////////

CmusikPropTreeItem* CmusikPropTree::InsertItem(CmusikPropTreeItem* pItem, CmusikPropTreeItem* pParent)
{
	if (!pItem)
		return NULL;

	if (!pParent)
		pParent = &m_Root;

	if ( !pParent->GetChild() )
	{
		pParent->SetChild( pItem );
		pItem->SetPrev( NULL );
		pItem->SetNext( NULL );
		pItem->SetParent( pParent );
	}

	else
	{
		CmusikPropTreeItem* pLast = pParent->GetChild();
		while (pLast->GetNext())
			pLast = pLast->GetNext();

		pLast->SetNext( pItem );
		pItem->SetPrev( pLast );
		pItem->SetNext( NULL );
		pItem->SetParent( pParent );
	}

	pItem->SetPropOwner( this );
	pItem->SetChild( NULL );

	// auto generate a default ID
	pItem->SetCtrlID( m_nLastUID++ );

	SendNotify( PTN_INSERTITEM, pItem );
	UpdatedItems();

	return pItem;
}

///////////////////////////////////////////////////

LONG CmusikPropTree::HitTest(const POINT& pt)
{
	POINT p = pt;

	CmusikPropTreeItem* pItem;

	// convert screen to tree coordinates
	p.y += m_ScrollPos;

    try
    {
	    if ((pItem = FindItem(pt))!=NULL)
	    {
		    if (pItem->IsRootLevel())
			    return HTEXPAND;

		    return HTLABEL;
	    }
    }
    catch (...)
    {

    }

	return HTCLIENT;
}

///////////////////////////////////////////////////

CmusikPropTreeItem* CmusikPropTree::HitTestEx( const POINT& pt )
{
	POINT p = pt;

	CmusikPropTreeItem* pItem;

	// convert screen to tree coordinates
	p.y += m_ScrollPos;

	if ((pItem = FindItem(pt))!=NULL)
		return pItem;

	return NULL;
}

///////////////////////////////////////////////////

CmusikPropTreeItem* CmusikPropTree::FindItem(const POINT& pt)
{
	CPoint p = pt;

	// convert screen to tree coordinates
	p.y += m_ScrollPos;

	// search the visible list for the item
    CPoint curloc;
	CmusikPropTreeItem* pItem = NULL;
    try
    {
		for ( size_t i = 0; i < m_pVisibleList.size(); i++ )
		{
			pItem = m_pVisibleList.at( i );
			curloc = pItem->GetLocation();
			if ( p.y >= curloc.y && p.y < curloc.y + pItem->GetHeight() )   
				return pItem;
		}
    }
    catch (...)
    {
        cout << "a strange, nonharmful event happened.\n";
        return NULL;
    }

	return NULL;
}

///////////////////////////////////////////////////

CmusikPropTreeItem* CmusikPropTree::FindItem(UINT nCtrlID)
{
	s_pFound = NULL;

	EnumItems(&m_Root, EnumFindItem, nCtrlID);

	return s_pFound;
}

///////////////////////////////////////////////////

BOOL CALLBACK CmusikPropTree::EnumFindItem(CmusikPropTree*, CmusikPropTreeItem* pItem, LPARAM lParam)
{
	ASSERT(pItem!=NULL);

	if (pItem->GetCtrlID()==(UINT)lParam)
	{
		s_pFound = pItem;
		return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////

void CmusikPropTree::SelectItems(CmusikPropTreeItem* pItem, BOOL bSelect)
{
	if (!pItem)
		pItem = &m_Root;

	EnumItems(pItem, EnumSelectAll, (LPARAM)bSelect);
}

///////////////////////////////////////////////////

CmusikPropTreeItem* CmusikPropTree::FocusPrev()
{
	CmusikPropTreeItem* pItem = GetFocusedItem();
	if ( pItem->GetPrev() )
	{
		SetFocusedItem( pItem->GetPrev() );
		pItem->GetPrev()->Select();
		return pItem->GetPrev();
	}

	return NULL;
}

///////////////////////////////////////////////////

CmusikPropTreeItem* CmusikPropTree::FocusNext()
{
	CmusikPropTreeItem* pItem = GetFocusedItem();
	if ( pItem->GetNext() )
	{
		SetFocusedItem( pItem->GetNext() );
		pItem->GetNext()->Select();
		return pItem->GetNext();
	}

	return NULL;
}

///////////////////////////////////////////////////

void CmusikPropTree::UpdateMoveAllItems()
{
	EnumItems(&m_Root, EnumMoveAll);
}

///////////////////////////////////////////////////

void CmusikPropTree::RefreshItems(CmusikPropTreeItem* pItem)
{
	if (!pItem)
		pItem = &m_Root;

	EnumItems(pItem, EnumRefreshAll);

	UpdatedItems();
}

///////////////////////////////////////////////////

BOOL CALLBACK CmusikPropTree::EnumSelectAll(CmusikPropTree*, CmusikPropTreeItem* pItem, LPARAM lParam)
{
	if (!pItem)
		return FALSE;

	pItem->Select((BOOL)lParam);

	return TRUE;
}

///////////////////////////////////////////////////

BOOL CALLBACK CmusikPropTree::EnumRefreshAll(CmusikPropTree*, CmusikPropTreeItem* pItem, LPARAM)
{
	if (!pItem)
		return FALSE;

	pItem->OnRefresh();

	return TRUE;
}

///////////////////////////////////////////////////

BOOL CALLBACK CmusikPropTree::EnumMoveAll(CmusikPropTree*, CmusikPropTreeItem* pItem, LPARAM)
{
	if (!pItem)
		return FALSE;

	pItem->OnMove();

	return TRUE;
}

///////////////////////////////////////////////////

LRESULT CmusikPropTree::SendNotify(UINT nNotifyCode, CmusikPropTreeItem* pItem)
{
	if (!IsWindow(m_hWnd))
		return 0L;

	if (!(GetStyle() & PTS_NOTIFY))
		return 0L;

	NMPROPTREE nmmp;
	LPNMHDR lpnm;

	lpnm = NULL;

	switch (nNotifyCode)
	{
		case PTN_INSERTITEM:
		case PTN_DELETEITEM:
		case PTN_DELETEALLITEMS:
		case PTN_ITEMCHANGED:
		case PTN_ITEMBUTTONCLICK:
		case PTN_SELCHANGE:
		case PTN_ITEMEXPANDING:
		case PTN_PROPCLICK:
        case PTN_ITEMDRAG:
			lpnm = (LPNMHDR)&nmmp;
			nmmp.pItem = pItem;
			break;
	}

	if (lpnm)
	{
		UINT id = (UINT)::GetMenu(m_hWnd);
		lpnm->code = nNotifyCode;
		lpnm->hwndFrom = m_hWnd;
		lpnm->idFrom = id;
	
		return GetParent()->SendMessage(WM_NOTIFY, (WPARAM)id, (LPARAM)lpnm);
	}

	return 0L;
}

///////////////////////////////////////////////////

void CmusikPropTree::OnEnable(BOOL bEnable) 
{
	CWnd::OnEnable(bEnable);

	Invalidate();
}

///////////////////////////////////////////////////

void CmusikPropTree::OnSysColorChange() 
{
	CWnd::OnSysColorChange();
	
	Invalidate();	
}

///////////////////////////////////////////////////

void CmusikPropTree::RecreateBackBuffer(int cx, int cy)
{
	if (m_BackBufferSize.cx<cx || m_BackBufferSize.cy<cy)
	{
		m_BackBufferSize = CSize(cx, cy);

		CWindowDC dc(NULL);

		int nPlanes = dc.GetDeviceCaps(PLANES);
		int nBitCount = dc.GetDeviceCaps(BITSPIXEL);

		m_BackBuffer.DeleteObject();
		m_BackBuffer.CreateBitmap(cx, cy, nPlanes, nBitCount, NULL);
	}
}

///////////////////////////////////////////////////

void CmusikPropTree::UpdateScrollbar()
{
	SCROLLINFO si;
	LONG nHeight;
	CRect rc;

	GetClientRect(rc);
	nHeight = rc.Height() + 1;

	ZeroMemory(&si, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE|SIF_PAGE;
	si.nMin = 0;
	si.nMax = GetRootItem()->GetTotalHeight();
	si.nPage = nHeight + PROPTREEITEM_DEFHEIGHT;

	if ((int)si.nPage>si.nMax)
		SetOriginOffset(0);

	SetScrollInfo(SB_VERT, &si, TRUE);
	CheckScrollVisible();	// sets a flag thats used when drawing
}

///////////////////////////////////////////////////

void CmusikPropTree::CheckScrollVisible()
{
	CRect rc;
	GetClientRect(rc);
	if ( rc.Height() < (int)m_pVisibleList.size() * PROPTREEITEM_DEFHEIGHT )
		m_ScrollVisible = true;
	else
		m_ScrollVisible = false;
}

///////////////////////////////////////////////////

void CmusikPropTree::OnPaint() 
{
	CPaintDC dc(this);

	ClearVisibleList();

	// get client rect
	CRect rc;
	GetClientRect( rc );

	// draw control background
	CMemDC memdc( &dc );
	memdc.FillSolidRect( rc, musikCube::g_Prefs->GetBtnFaceColor() );

	CmusikPropTreeItem* pItem;
	LONG nTotal = 0;

	ASSERT(GetRootItem()!=NULL);

	// create clip region
	HRGN hRgn = CreateRectRgn( rc.left, rc.top, rc.right, rc.bottom );
	SelectClipRgn( memdc.m_hDC, hRgn );

	// build a list of visible items prior to drawing. if
	// we decide to later implement trees with more than
	// one level, we will need a recursive function call
	// to do this.

	pItem = GetRootItem()->GetChild();
	while ( pItem )
	{
		m_pVisibleList.push_back( pItem );
		if ( pItem->IsExpanded() && pItem->GetChild() )
		{
			CmusikPropTreeItem* pNext = pItem->GetChild();
			while ( pNext )
			{
				m_pVisibleList.push_back( pNext );
				pNext = pNext->GetNext();
			}
		}
		pItem = pItem->GetNext();
	}

	// determine if the scrollbar is visible
	CheckScrollVisible();

	// actually draw the items
	for ( size_t i = 0; i < m_pVisibleList.size(); i++ )
	{
		LONG nHeight = m_pVisibleList.at( i )->DrawItem(&memdc, rc, 0, nTotal);
		nTotal += nHeight;
	}

	// remove clip region
	SelectClipRgn(memdc.m_hDC, NULL);
	DeleteObject(hRgn);
}

///////////////////////////////////////////////////

BOOL CmusikPropTree::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

///////////////////////////////////////////////////

void CmusikPropTree::OnLButtonDown(UINT flags, CPoint point) 
{
	//
	// hide edit in place
	//
	if ( m_EditInPlace.IsVisible() )
		m_EditInPlace.Cancel();

	SendNotify(NM_CLICK);

	if (!IsWindowEnabled())
		return;

	SetFocus();

	LONG nHit = HitTest(point);

	CmusikPropTreeItem* pItem;
	CRect rc;

	switch (nHit)
	{
		case HTEXPAND:
			if ( flags != RTCLKITEM && (pItem = FindItem(point))!=NULL)
			{
				if (pItem->GetChild() && !SendNotify(PTN_ITEMEXPANDING, pItem))
				{
					pItem->Expand(!pItem->IsExpanded());

					UpdateScrollbar();
					Invalidate();
					UpdateWindow();
					CheckVisibleFocus();
				}
			}
			break;

		default:

			if ((pItem = FindItem(point))!=NULL)
			{
				CmusikPropTreeItem* pOldFocus = GetFocusedItem();

				if ( pItem->IsRootLevel() )
					return;

				SelectItems(NULL, FALSE);
				SetFocusedItem(pItem);

				pItem->Select();

				Invalidate();

				if ( flags == RTCLKITEM )
					m_LastBtn = RTCLKITEM;
				else
					m_LastBtn = LFCLKITEM;

				// don't send a sel change event for removable devices
				if (pItem!=pOldFocus )
					SendNotify(PTN_SELCHANGE, pItem);

				if (nHit==HTATTRIBUTE && !pItem->IsRootLevel())
				{
					if (!SendNotify(PTN_PROPCLICK, pItem))
						pItem->Activate();
				}
			}
			break;
	}
}

///////////////////////////////////////////////////

void CmusikPropTree::OnLButtonDblClk(UINT, CPoint point)
{
	SendNotify(NM_DBLCLK);

	CmusikPropTreeItem* pItem;

	if ( (pItem = FindItem(point))!=NULL )
	{
		if ( pItem->IsRootLevel() )
		{
			if (pItem->GetChild() && !SendNotify(PTN_ITEMEXPANDING, pItem))
			{
				pItem->Expand(!pItem->IsExpanded());

				UpdateScrollbar();
				Invalidate();
				UpdateWindow();
				CheckVisibleFocus();
			}

			return;
		}

		CmusikPropTreeItem* pOldFocus = GetFocusedItem();
		SelectItems(NULL, FALSE);
		SetFocusedItem(pItem);

		pItem->Select();

		Invalidate();

		if ( !pItem->IsRootLevel() && pItem != pOldFocus )
			SendNotify(PTN_SELCHANGE, pItem);
	}

}

///////////////////////////////////////////////////

BOOL CmusikPropTree::OnMouseWheel(UINT, short zDelta, CPoint) 
{
	SCROLLINFO si;

	ZeroMemory(&si, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE;

	GetScrollInfo(SB_VERT, &si);

	CRect rc;
	GetClientRect(rc);

	if (si.nMax - si.nMin < rc.Height())
		return TRUE;

	SetFocus();
	OnVScroll(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0, NULL);

	return TRUE;
}

///////////////////////////////////////////////////

void CmusikPropTree::OnKeyDown(UINT nChar, UINT, UINT) 
{
	CmusikPropTreeItem* pItem;

	switch (nChar)
	{
		case VK_RETURN:
			if ((pItem = GetFocusedItem())!=NULL && !pItem->IsRootLevel())
			{
				pItem->Activate();
			}
			break;

		case VK_LEFT:
			if ((pItem = GetFocusedItem())!=NULL)
			{
				if (!SendNotify(PTN_ITEMEXPANDING, pItem))
				{
					if (pItem->GetChild() && pItem->IsExpanded())
					{
						pItem->Expand(FALSE);
						UpdateScrollbar();
						Invalidate();
						UpdateWindow();
						CheckVisibleFocus();
						break;
					}
				}
			}
			else
				break;
			// pass thru to next case VK_UP
		case VK_UP:
			if (FocusPrev())
				Invalidate();
			break;

		case VK_RIGHT:
			if ((pItem = GetFocusedItem())!=NULL)
			{
				if (!SendNotify(PTN_ITEMEXPANDING, pItem))
				{
					if (pItem->GetChild() && !pItem->IsExpanded())
					{
						pItem->Expand();
						UpdateScrollbar();
						Invalidate();
						UpdateWindow();
						CheckVisibleFocus();
						break;
					}
				}
			}
			else
				break;
			// pass thru to next case VK_DOWN
		case VK_DOWN:
			if (FocusNext())
				Invalidate();
			break;
	}
}

///////////////////////////////////////////////////

UINT CmusikPropTree::OnGetDlgCode() 
{
	return DLGC_WANTARROWS|DLGC_WANTCHARS|DLGC_WANTALLKEYS;
}

///////////////////////////////////////////////////

void CmusikPropTree::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar*) 
{
	SCROLLINFO si;
	CRect rc;
	LONG nHeight;

	SetFocus();

	GetClientRect(rc);
	nHeight = rc.Height() + 1;

	ZeroMemory(&si, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE;

	GetScrollInfo(SB_VERT, &si);

	LONG ny = GetOrigin();

	switch (nSBCode)
	{
		case SB_LINEDOWN:
			ny += PROPTREEITEM_DEFHEIGHT;
			break;

		case SB_LINEUP:
			ny -= PROPTREEITEM_DEFHEIGHT;
			break;

		case SB_PAGEDOWN:
			ny += nHeight;
			break;

		case SB_PAGEUP:
			ny -= nHeight;
			break;

		case SB_THUMBTRACK:
			ny = nPos;
			break;
	}

	ny = __min(__max(ny, si.nMin), si.nMax - nHeight);

	SetOriginOffset(ny);
	si.fMask = SIF_POS;
	si.nPos = ny;

	SetScrollInfo(SB_VERT, &si, TRUE);
	CheckScrollVisible();	// sets a flag thats used when drawing
	Invalidate();
}

///////////////////////////////////////////////////

void CmusikPropTree::CheckVisibleFocus()
{
	CmusikPropTreeItem* pItem;
	
	if ((pItem = GetFocusedItem())==NULL)
		return;
}

///////////////////////////////////////////////////

void CmusikPropTree::OnRButtonDown(UINT nFlags, CPoint point)
{
	CmusikPropTree::OnLButtonDown(RTCLKITEM, point);
}

///////////////////////////////////////////////////

void CmusikPropTree::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
}

///////////////////////////////////////////////////

void CmusikPropTree::OnMouseMove(UINT nFlags, CPoint point)
{
    if (!m_MouseTrack && !(nFlags & MK_LBUTTON))
    {
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(tme);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = m_hWnd;
        tme.dwHoverTime = HOVER_DEFAULT;
        ::_TrackMouseEvent(&tme);

        m_MouseTrack = true;
    }

    // test for drag and drop
    if (m_MouseTrack && (nFlags & MK_LBUTTON))
    {
        CmusikPropTreeItem* pItem = this->GetFocusedItem();
        //
        if (pItem)
        {
            this->SendNotify(PTN_ITEMDRAG, pItem);
        }
    }

    CmusikPropTreeItem* pItem = this->FindItem(point);
    if (pItem != CmusikPropTree::m_Hovered)
    {
        CmusikPropTree::m_Hovered = pItem;
        RedrawWindow();
    }
}

///////////////////////////////////////////////////

LRESULT CmusikPropTree::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    m_MouseTrack = false;
    CmusikPropTree::m_Hovered = NULL;
    Invalidate();
    return 1L;
}

///////////////////////////////////////////////////
