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
//   CmusikListCtrl
//
// Filename(s): 
//
//   musikListCtrl.h, musikListCtrl.cpp
//
// Information:
//
//   musikCube's custom list control
// 
// Usage: 
//
//   A few specialized functions, like hiding scrollbars
//   and a better header DND implementation
//
// Some code adapted from:
//
//   Lars [Large] Werner: http://www.codeproject.com/listctrl/listctrlsbhide.asp
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#define LCSB_CLIENTDATA 1
#define LCSB_NCOVERRIDE 2

#define SB_HORZ             0
#define SB_VERT             1
#define SB_BOTH             3

///////////////////////////////////////////////////

class CmusikListCtrl : public CListCtrl
{
public:
	CmusikListCtrl();
	virtual ~CmusikListCtrl();

	// hide scrollbars
	void HideScrollBars( int Type, int Which=SB_BOTH );

protected:

	// message maps
	afx_msg void OnNcCalcSize( BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp );
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg LRESULT OnMouseLeave( WPARAM wParam, LPARAM lParam );

	// function the custom header control
	// will use as a callback after a column
	// has been moved
	virtual void OnDragColumn( int source, int dest );

	// gets a modified color
	COLORREF GetModColor( COLORREF color, int pixel_change = 15 );
	bool m_MouseTrack;

	// macros
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CmusikListCtrl)

private:

	// Prevent default compiler generation of these copy constructors (no bitwise copy)
	CmusikListCtrl& operator=(const CmusikListCtrl& x);
	CmusikListCtrl(const CmusikListCtrl& x);

	// scrollbars
	BOOL NCOverride;
	int Who;
};

///////////////////////////////////////////////////