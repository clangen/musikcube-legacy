///////////////////////////////////////////////////
//
// PropTreeItem.cpp
//
//  Copyright (C) 1998-2001 Scott Ramsay
//	sramsay@gonavi.com
//	http://www.gonavi.com
//
//  This material is provided "as is", with absolutely no warranty expressed
//  or implied. Any use is at your own risk.
// 
//  Permission to use or copy this software for any purpose is hereby granted 
//  without fee, provided the above notices are retained on all copies.
//  Permission to modify the code and to distribute modified code is granted,
//  provided the above notices are retained, and a notice that the code was
//  modified is included with the above copyright notice.
// 
//	If you use this code, drop me an email.  I'd like to know if you find the code
//	useful.
//
//  Modified heavily by Casey Langen for musik MFC port...
//    - Rather than linking to a dll or external lib, it compiles in.
//    - PropTree and PropTreeList combined into one file
//    - Drawing enhancements
//    - Various musik specific functions
//
//  The musik team thanks Scott Ramsay for creating this great
//  class and making it open source.
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include <Wingdi.h>
#include "musikPropTree.h"
#include "musikPropTreeItem.h"

///////////////////////////////////////////////////

///////////////////////////////////////////////////

CmusikPropTreeItem::CmusikPropTreeItem() :
	m_pProp(NULL),
	m_sLabel(_T("")),
	m_loc(0,0),
	m_rc(0,0,0,0),
	m_lParam(0),
	m_nCtrlID(0),
	m_dwState(0),
	m_bActivated(FALSE),
	m_bCommitOnce(FALSE),
	m_rcExpand(0,0,0,0),
	m_pParent(NULL),
	m_pChild(NULL),
	m_pNext(NULL),
	m_pPrev(NULL),
	m_pBmpOff(NULL),
	m_pBmpOn(NULL)
{
    m_Library = NULL;
	m_PlaylistInfo = new musikCore::PlaylistInfo();
}

///////////////////////////////////////////////////

CmusikPropTreeItem::~CmusikPropTreeItem()
{
	if ( m_PlaylistInfo )
	{
		delete m_PlaylistInfo;
	}

    if ( m_Library )
		UnloadLibrary();
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::SetBitmaps( Gdiplus::Bitmap* off, Gdiplus::Bitmap* on )
{
	m_pBmpOff = off;
	m_pBmpOn = on;
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::SetPlaylistInfo( const musikCore::PlaylistInfo info )
{
	if ( m_PlaylistInfo )
	{
		*m_PlaylistInfo = info;

		SetLabelText( m_PlaylistInfo->GetName() );
	}
}

///////////////////////////////////////////////////

int CmusikPropTreeItem::GetPlaylistType()
{
	return m_PlaylistInfo->GetType();
}

///////////////////////////////////////////////////

int CmusikPropTreeItem::GetPlaylistID()
{
	return m_PlaylistInfo->GetID();
}

///////////////////////////////////////////////////

BOOL CmusikPropTreeItem::IsExpanded()
{
	return (m_dwState & TreeItemExpanded) ? TRUE : FALSE;
}

///////////////////////////////////////////////////

BOOL CmusikPropTreeItem::IsSelected()
{
	return (m_dwState & TreeItemSelected) ? TRUE : FALSE;
}

///////////////////////////////////////////////////

BOOL CmusikPropTreeItem::IsActivated()
{
	return (m_dwState & TreeItemActivated) ? TRUE : FALSE;
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::Select(BOOL bSelect)
{
	if (bSelect)
		m_dwState |= TreeItemSelected;
	else
		m_dwState &= ~TreeItemSelected;
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::Expand(BOOL bExpand)
{
	if (bExpand)
		m_dwState |= TreeItemExpanded;
	else
		m_dwState &= ~TreeItemExpanded;
}

///////////////////////////////////////////////////

BOOL CmusikPropTreeItem::HitExpand(const POINT& pt)
{
	return m_rcExpand.PtInRect(pt);
}

///////////////////////////////////////////////////

BOOL CmusikPropTreeItem::IsRootLevel()
{
	return GetParent() == m_pProp->GetRootItem();
}

///////////////////////////////////////////////////

LONG CmusikPropTreeItem::GetTotalHeight()
{
	CmusikPropTreeItem* pItem;
	LONG nHeight;

	nHeight = GetHeight();

	if (IsExpanded())
	{
        pItem = GetChild();
        while ( pItem )
        {
            nHeight += pItem->GetTotalHeight();
            pItem = pItem->GetNext();
        }
	}

	return nHeight;
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::SetLabelText(LPCTSTR sLabel)
{
	m_sLabel = sLabel;
}

///////////////////////////////////////////////////

LPCTSTR CmusikPropTreeItem::GetLabelText()
{
	return m_sLabel;
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::SetCtrlID(UINT nCtrlID)
{
	m_nCtrlID = nCtrlID;
}

///////////////////////////////////////////////////

UINT CmusikPropTreeItem::GetCtrlID()
{
	return m_nCtrlID;
}

///////////////////////////////////////////////////

LONG CmusikPropTreeItem::GetHeight()
{
	return PROPTREEITEM_DEFHEIGHT;
}

///////////////////////////////////////////////////

LPARAM CmusikPropTreeItem::GetItemValue()
{
	// no items are assocatied with this type
	return 0L;
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::SetItemValue(LPARAM)
{
	// no items are assocatied with this type
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::OnMove()
{
	// no attributes, do nothing
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::OnRefresh()
{
	// no attributes, do nothing
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::OnCommit()
{
	// no attributes, do nothing
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::Activate()
{
	m_bActivated = TRUE;
	m_bCommitOnce = FALSE;

	OnActivate();
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::Deactivate()
{
	CommitChanges();
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::CommitChanges()
{
	m_bActivated = FALSE;

	if (m_bCommitOnce)
		return;

	m_bCommitOnce = TRUE;

	ASSERT(m_pProp!=NULL);

	OnCommit();

	m_pProp->SendNotify(PTN_ITEMCHANGED, this);
	m_pProp->RefreshItems(this);
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::OnActivate()
{
	// no attributes, do nothing
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::SetPropOwner(CmusikPropTree* pProp)
{
	m_pProp = pProp;
}

///////////////////////////////////////////////////

const POINT& CmusikPropTreeItem::GetLocation()
{
	return m_loc;
}

///////////////////////////////////////////////////

CmusikPropTreeItem* CmusikPropTreeItem::GetParent()
{
	return m_pParent;
}

///////////////////////////////////////////////////

CmusikPropTreeItem* CmusikPropTreeItem::GetChild()
{
	return m_pChild;
}

///////////////////////////////////////////////////

CmusikPropTreeItem* CmusikPropTreeItem::GetNext()
{
	return m_pNext;
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::SetParent(CmusikPropTreeItem* pParent)
{
	m_pParent = pParent;
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::SetChild(CmusikPropTreeItem* pChild)
{
    //ASSERT( pChild );
	m_pChild = pChild;
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::SetNext(CmusikPropTreeItem* pNext)
{
	m_pNext = pNext;
}

///////////////////////////////////////////////////

CmusikPropTreeItem* CmusikPropTreeItem::Detach()
{
	CmusikPropTreeItem* pPrev = GetPrev();
	CmusikPropTreeItem* pNext = GetNext();

	// we are in the middle of the list
	if ( pPrev && pNext )
	{
		pPrev->SetNext( pNext );
		pNext->SetPrev( pPrev );
	}

	// no previous, link to our parent to the
	// next node in the list
	else if ( !pPrev && pNext )
	{
		GetParent()->SetChild( pNext );
		pNext->SetPrev( NULL );
	}

	// deleted the last item in the list
	else if ( pPrev && !pNext )
	{
		pPrev->SetNext( NULL );
	}

	// we were the only item (no left or right node)
	else if ( !pPrev && !pNext )
	{
		GetParent()->SetChild( NULL );
	}

	SetPrev( NULL );
	SetNext( NULL );

	return this;
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::SetPrev(CmusikPropTreeItem* pPrev)
{
	m_pPrev = pPrev;
}

///////////////////////////////////////////////////

LONG CmusikPropTreeItem::DrawItem( CDC* pDC, const RECT& rc, LONG x, LONG y )
{
	CPoint pt;
	LONG nTotal;
	CRect drc, ir;

	ASSERT( m_pProp!=NULL );

	int right = rc.right;
	if ( m_pProp->IsScrollVisible() )
	{
		right -= 1;
	}

	// store the item's location
	m_loc = CPoint(x, y);

	// store the items rectangle position
	m_rc.SetRect(PROPTREEITEM_SPACE, m_loc.y, right, m_loc.y + GetHeight()-1);
	m_rc.OffsetRect(0, -m_pProp->GetOrigin());

	// init temp drawing variables
	nTotal = GetHeight();

	// convert item coordinates to screen coordinates
	pt = m_loc;
	pt.y -= m_pProp->GetOrigin();

	// draws the label the whole size
	drc.SetRect(pt.x + 16, pt.y, right, pt.y + nTotal);

		HGDIOBJ hOld;

	// SH 2004-01-03 GDI leak cleanup : was previously plain old Win32 CreateSolidBrush without any variable
	// assignment and cleanup. Changed to CBrush, as its destructor handles cleanup properly.

	CBrush listBrush, gutterBrush;

	//the object
    if ( IsSelected() )
	{
		listBrush.CreateSolidBrush( musikCube::g_Prefs->GetListCtrlColor() );
	}
    else if ( CmusikPropTree::m_Hovered == this || 
			  GetPlaylistType() == MUSIK_SOURCES_TYPE_NEWSTDPLAYLIST || 
			  GetPlaylistType() == MUSIK_SOURCES_TYPE_NEWDYNPLAYLIST )
    {
        COLORREF backColor;

		if ( IsRootLevel() )
		{
			backColor = ::ShadeColor( musikCube::g_Prefs->GetBtnFaceColor(), -40 );
		}
		else
		{
			backColor = ::ShadeColor( musikCube::g_Prefs->GetBtnFaceColor(), 20 );
		}

        listBrush.CreateSolidBrush( backColor ); 
    }
    else
    {
        COLORREF backColor;

		if ( IsRootLevel() )
		{
			backColor = ::ShadeColor( musikCube::g_Prefs->GetBtnFaceColor(), -20 );
		}
		else
		{
			backColor = ::ShadeColor( musikCube::g_Prefs->GetBtnFaceColor(), 10 );
		}

        listBrush.CreateSolidBrush( backColor );
    }

	hOld = pDC->SelectObject( listBrush );
	pDC->PatBlt(rc.left, drc.top, right - rc.left + 1, drc.Height(), PATCOPY);

	pDC->SelectObject(hOld);

    //
	// calc label position
    //

	ir = drc;
	if ( IsRootLevel() )
	{
		ir.left -= 8;
	}
	else
	{
		if ( musikCube::g_DrawGraphics && !IsRootLevel() )
		{
			ir.left += PROPTREEITEM_SPACE + 12;
		}
		else
		{
			ir.left -= 4;
		}
	}

    //
	// draw label
    //
	if ( !m_sLabel.IsEmpty() )
	{

		if ( IsSelected() || IsRootLevel() )
            pDC->SelectObject( CmusikPropTree::GetBoldFont() );
        else
			pDC->SelectObject( CmusikPropTree::GetNormalFont() );

        //
        // color
        //
		if ( IsSelected() )
		{
			pDC->SetTextColor( musikCube::g_Prefs->GetListCtrlTextColor() );
		}
		else
		{
			pDC->SetTextColor( musikCube::g_Prefs->GetBtnTextColor() );
		}

		pDC->SetBkMode(TRANSPARENT);
        pDC->DrawText(
			m_sLabel, 
			&ir, 
			DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS
		);
	}

	// remove clip region
	pDC->SelectObject(hOld);	
	HRGN hRgn = NULL;
	if (hRgn)
	{
		SelectClipRgn(pDC->m_hDC, NULL);
		DeleteObject(hRgn);
	}	

	// border around selected object
    COLORREF bord;
    if ( IsSelected() || this == CmusikPropTree::m_Hovered )
	{
        bord = musikCube::g_Prefs->GetBtnShadowColor();
		
		int width = right;
		if ( !IsRootLevel() )
			width -= 6;
		if ( m_pProp->IsScrollVisible() )
			width += 1;

		pDC->Draw3dRect(
			IsRootLevel() ? 0 : 6,
			pt.y,
			width,
			PROPTREEITEM_DEFHEIGHT,
			bord, bord );
	}

    //
    // draw icon
    //

    if ( musikCube::g_DrawGraphics )
    {
		Gdiplus::Bitmap* bmp = NULL;
		if ( IsSelected() || this == CmusikPropTree::m_Hovered )
			bmp = m_pBmpOn;
		else
			bmp = m_pBmpOff;

        if ( bmp )
        {
            Gdiplus::Graphics graphics( pDC->GetSafeHdc() );
            graphics.DrawImage( bmp, 10, pt.y + 2, 18, 18 );
        }
    }

	//
	// draw the gutter
	//

	if ( !IsRootLevel() )
	{
		if ( IsSelected() )
		{
			gutterBrush.CreateSolidBrush( musikCube::g_Prefs->GetActiveCaptionColor() );
			hOld = pDC->SelectObject( gutterBrush );
			pDC->PatBlt( rc.left, drc.top, 6, drc.Height(), PATCOPY);
		}
		else
		{
			gutterBrush.CreateSolidBrush( musikCube::g_Prefs->GetBtnFaceColor() );
			hOld = pDC->SelectObject( gutterBrush );
			pDC->PatBlt( rc.left, drc.top, 6, drc.Height(), PATCOPY);
		}
	}

	return nTotal;
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::UnloadLibrary()
{
	if ( m_Library )
	{
		musikCube::g_Player->LibraryDetach( m_Library );		// let the player know we've detached, as it
																// may need to remove some songs from this library

		if ( musikCube::g_Library == m_Library )
		{
			SetMainLibrary( musikCube::g_MainLibrary );
		}

		musikCube::g_Player->LibraryDetach( m_Library );

		delete m_Library;
		m_Library = NULL;
	}
}

///////////////////////////////////////////////////
