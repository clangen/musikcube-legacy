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
//   CmusikVolumeCtrl
//
// Filename(s): 
//
//   musikVolumeCtrl.h, musikVolumeCtrl.cpp
//
// Information:
//
//   The volume adjustment slider in the "Now Playing" control.
// 
// Usage: 
//
//    Void.
//
///////////////////////////////////////////////////

#include "stdafx.h"

#include "musikVolumeCtrl.h"
#include "MEMDC.H"

///////////////////////////////////////////////////

int WM_NOWPLAYING_VOLUME_CHANGED = RegisterWindowMessage(_T("WM_NOWPLAYING_VOLUME_CHANGED"));

///////////////////////////////////////////////////

CmusikVolumeCtrl::CmusikVolumeCtrl()
	: CmusikTrackCtrl()
{
}

///////////////////////////////////////////////////

CmusikVolumeCtrl::~CmusikVolumeCtrl()
{
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikVolumeCtrl, CmusikTrackCtrl)
	ON_WM_CREATE()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

int CmusikVolumeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CmusikTrackCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetRange(0, 255);
    UpdateVolume();

	return 0;
}

///////////////////////////////////////////////////

void CmusikVolumeCtrl::UpdateVolume()
{
	if (musikCube::g_Player)
		SetPos(musikCube::g_Player->GetMaxVolume());
}

///////////////////////////////////////////////////

void CmusikVolumeCtrl::OnPosChanged()
{
	if ( musikCube::g_Player )
	{
		musikCube::g_Player->SetMaxVolume( GetPos(), true );
	}

	if ( musikCube::g_Prefs )
		musikCube::g_Prefs->SetPlayerVolume( GetPos() );

	this->GetParent()->PostMessage(WM_NOWPLAYING_VOLUME_CHANGED);
}

///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikVolumeWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(WM_NOWPLAYING_VOLUME_CHANGED, OnVolumeChanged)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

CmusikVolumeWnd::~CmusikVolumeWnd()
{
	delete m_VolumeLabel;
	m_VolumeLabel = NULL;
}

///////////////////////////////////////////////////

LRESULT CmusikVolumeWnd::OnVolumeChanged(WPARAM wParam, LPARAM lParam)
{
	m_VolumeCtrl->UpdateVolume();

	wchar_t volume[5];
	wsprintf(volume, _T("%3d"), (100 * musikCube::g_Player->GetMaxVolume())/255);

	m_VolumeLabel->SetDynText(volume, false, false);

	return 0;
}

///////////////////////////////////////////////////

int CmusikVolumeWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// create slider
	m_VolumeCtrl = new CmusikVolumeCtrl();
	if (!m_VolumeCtrl->Create(TBS_HORZ | TBS_NOTICKS | WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 123))
		return -1;
	m_VolumeCtrl->SetLockIfNotPlaying(false);

	// create label
	m_VolumeLabel = new CmusikDynamicText();
	m_VolumeLabel->Create(NULL, WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this);
	m_VolumeLabel->SetDynText(_T("0"));
	m_VolumeLabel->SetDynFont(12, 0, 0);
	//
	mLabelWidth = m_VolumeLabel->GetDynSize().cx;

	// create caption
	m_Caption = new CmusikDynamicText();
	m_Caption->Create(NULL, WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this);
	m_Caption->SetDynText(_T("vol:"));
	m_Caption->SetDynFont(12, 0, 0);
	
	
	OnVolumeChanged(0, 0);

	return 0;
}

///////////////////////////////////////////////////

void CmusikVolumeWnd::SetLabelWidth(int inWidth)
{
	mLabelWidth = inWidth;

	// fake size event
	CRect clientRect;
	this->GetClientRect(&clientRect);
	this->OnSize(NULL, clientRect.Width(), clientRect.Height());
}

///////////////////////////////////////////////////

void CmusikVolumeWnd::UpdateVolume()
{
	//this->OnVolumeChanged(NULL, NULL);
}

///////////////////////////////////////////////////

void CmusikVolumeWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	CRect clientRect;
	this->GetClientRect(clientRect);

	// caption
	CRect captionRect(
		CPoint(
			0,	// x
			0	// y
		),
		CSize(
			m_Caption->GetDynSize().cx,					// width
			m_Caption->GetDynSize().cy					// height
		)
	);

	m_VolumeCtrl->MoveWindow(captionRect);

	// slider
	CRect sliderRect(
		CPoint(
			captionRect.Width() + 2,	// x
			0							// y
		),
		CSize(
			clientRect.Width() - mLabelWidth -	// width
				 captionRect.Width() - 2,		// width
			16									// height
		)
	);

	m_VolumeCtrl->MoveWindow(sliderRect);

	// readout
	CRect labelRect(
		CPoint(
			clientRect.right - mLabelWidth,						// x
			0													// y
		),
		CSize(
			mLabelWidth,										// width
			m_VolumeLabel->GetDynSize().cy						// height
		)
	);

	m_VolumeLabel->MoveWindow(labelRect);
}

///////////////////////////////////////////////////

void CmusikVolumeWnd::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	CMemDC memDC(&dc, &rect);

	memDC.FillSolidRect(rect, musikCube::g_Prefs->GetBtnFaceColor());
}

///////////////////////////////////////////////////

BOOL CmusikVolumeWnd::OnEraseBkgnd(CDC* pDC)
{
	return true;
}

///////////////////////////////////////////////////
