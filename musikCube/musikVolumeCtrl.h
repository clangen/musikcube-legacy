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

#pragma once

///////////////////////////////////////////////////

#include "musikCube.h"
#include "musikTrackCtrl.h"

#include <musikCore.h>

///////////////////////////////////////////////////

class CmusikVolumeCtrl : public CmusikTrackCtrl
{
public:

	// construct and destruct
	CmusikVolumeCtrl();
	~CmusikVolumeCtrl();

    void UpdateVolume();

private:

	// position changed
	virtual void OnPosChanged();

	// message maps
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	// macros
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////