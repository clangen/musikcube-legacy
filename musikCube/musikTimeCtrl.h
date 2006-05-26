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
//   CmusikTimeCtrl
//
// Filename(s): 
//
//   musikTimeCtrl.h, musikTimeCtrl.cpp
//
// Information:
//
//   The time seek slider in the "Now Playing" control.
// 
// Usage: 
//
//    Void.
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikCube.h"
#include "musikDynamicText.h"
#include "musikTrackCtrl.h"

#include <musikCore.h>

///////////////////////////////////////////////////

class CmusikTimeCtrl : public CWnd
{
public:

	// construct and destruct
	CmusikTimeCtrl();
	virtual ~CmusikTimeCtrl();

	// misc
	void OnNewSong();

	// size
	void SetSize( const CSize& size );
	int GetTotalTimeSz(){return m_TotalTime->GetDynSize().cx;}

	// message maps
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnTrackChange( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnTrackBegin( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnTrackFinish( WPARAM wParam, LPARAM lParam );
	afx_msg void OnClicked();

protected:

	// our children...
	CmusikTrackCtrl* m_TimeCtrl;
	CmusikDynamicText* m_CurTime;
	CmusikDynamicText* m_TotalTime;
	void RescaleInfo( int cx );
	size_t m_CurChars;
	size_t m_TotalChars;

	// size
	CSize m_Size;

	// we seeking playback time?
	bool m_TimeDrag;

	// toggle for normal timer vs. countdown timer
	bool m_CountDown;

	// timer
	UINT_PTR m_TimerID;

	// macros
	DECLARE_DYNAMIC(CmusikTimeCtrl)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

