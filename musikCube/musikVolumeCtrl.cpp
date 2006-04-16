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
	if ( CmusikTrackCtrl::OnCreate( lpCreateStruct ) == -1 )
		return -1;

	SetRange( 0, 255 );
    UpdateVolume();

	return 0;
}

///////////////////////////////////////////////////

void CmusikVolumeCtrl::UpdateVolume()
{
	if ( musikCube::g_Player )
		SetPos( 255 - musikCube::g_Player->GetMaxVolume() );
}

///////////////////////////////////////////////////

void CmusikVolumeCtrl::OnPosChanged()
{
	if ( musikCube::g_Player )
	{
		musikCube::g_Player->SetMaxVolume( 255 - GetPos(), true );
	}

	if ( musikCube::g_Prefs )
		musikCube::g_Prefs->SetPlayerVolume( 255 - GetPos() );
}

///////////////////////////////////////////////////

