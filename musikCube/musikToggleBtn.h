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

#pragma once

///////////////////////////////////////////////////

#include "musikCube.h"
#include "3rdparty/BtnST.h"

///////////////////////////////////////////////////

class CmusikToggleBtn : public CButtonST
{
public:

	CmusikToggleBtn( Gdiplus::Bitmap* inUnchecked = NULL, Gdiplus::Bitmap* inChecked = NULL );
	void SetBitmaps( Gdiplus::Bitmap* inUnchecked, Gdiplus::Bitmap* inChecked );

protected:

	Gdiplus::Bitmap* m_UncheckedBmp;
	Gdiplus::Bitmap* m_CheckedBmp;

	COLORREF m_Checked;

    virtual DWORD OnDrawBorder( CDC* pDC, CRect* rect );
    virtual DWORD OnDrawBackground( CDC* pDC, CRect* rect );
};  

///////////////////////////////////////////////////

