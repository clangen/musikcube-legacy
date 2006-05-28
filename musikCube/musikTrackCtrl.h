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
//   CmusikTrackCtrl
//
// Filename(s): 
//
//   musikTrackCtrl.h, musikTrackCtrl.cpp
//
// Information:
//
//   musikCube's custom slider control, based off of
//   CSliderCtrl
// 
// Usage: 
//
//   See CmusikVolumeCtrl, CmusikTimeCtrl
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikCube.h"
#include <musikCore.h>

///////////////////////////////////////////////////

class CmusikTrackCtrl : public CSliderCtrl
{
public:

	// construct / destruct
	CmusikTrackCtrl();
	virtual ~CmusikTrackCtrl();

	// misc
	void SetLockIfNotPlaying( bool lock = true ){ m_LockIfNotPlaying = lock; }
	bool IsLockSet(){ return m_LockIfNotPlaying; }
	bool IsDragging(){ return m_IsCapturing; }

	void SetPos( int nPos );

	// public mfc message maps
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();

protected:	

	// mouse tracking
	CPoint m_Cursor;
	int m_LastPos;
	bool m_LeftDown;
	bool m_IsCapturing;
	bool m_DrawThumb;
	void SetPosFromMouse( bool call_changed = true );
	virtual void OnPosChanged();
	virtual void OnBeginDrag();
	virtual void OnFinishDrag();

	// lock movement if no playback
	bool m_LockIfNotPlaying;
    bool m_RealtimeUpdate;

	// drawing...
	void DrawChannel( CDC* pDC, const CRect& rect, BOOL bHoriz );
	CRect m_ChannelRect;

	// message maps
	DECLARE_DYNAMIC(CmusikTrackCtrl)
	DECLARE_MESSAGE_MAP()


};

///////////////////////////////////////////////////

