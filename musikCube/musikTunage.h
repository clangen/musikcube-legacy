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
//   CmusikTunage
//
// Filename(s): 
//
//   musikTunage.h, musikTunage.cpp
//
// Information:
//
//   Does something related to current song info, such as post to a website,
//   write to a text file, etc.
// 
// Usage: 
//
//	 Called when a song changes or is stopped
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikCube.h"
#include <musikCore.h>
#include <map>

///////////////////////////////////////////////////

class CmusikTunage
{
	friend class CmusikTunage;

public:

	CmusikTunage();
	~CmusikTunage();

	void Execute();

	void Execute_PostURL();
	void Execute_RunApp();
	void Execute_WriteFile();

	static void ParseTags( musikCore::String& str, bool encode );
	static void URLEncode( musikCore::String& str );

};

///////////////////////////////////////////////////
