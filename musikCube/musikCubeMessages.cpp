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

#include "stdafx.h"
#include "musikCubeMessages.h"

///////////////////////////////////////////////////

int WM_SELBOXUPDATE			= RegisterWindowMessage( _T( "SELBOXUPDATE" ) );
int WM_SELBOXFILTER         = RegisterWindowMessage( _T( "SELBOXFILTER" ) );
int WM_SELBOXEDITCOMMIT		= RegisterWindowMessage( _T( "SELBOXEDITCOMMIT" ) );
int WM_SELBOXADDREMOVE		= RegisterWindowMessage( _T( "SELBOXADDREMOVE" ) );
int WM_SELBOXREQUESTUPDATE	= RegisterWindowMessage( _T( "SELBOXREQUESTUPDATE" ) );
int WM_SELBOXDELSEL			= RegisterWindowMessage( _T( "SELBOXDELSEL" ) );
int WM_SELBOXDESEL			= RegisterWindowMessage( _T( "SELBOXDESEL" ) );
int WM_SELBOXRESET			= RegisterWindowMessage( _T( "SELBOXRESET" ) );
int	WM_SELBOXACTIVATE		= RegisterWindowMessage( _T( "SELBOXACTIVATE" ) );
int WM_SOURCESLIBRARY		= RegisterWindowMessage( _T( "SOURCESLIBRARY" ) );
int WM_SOURCESNOWPLAYING	= RegisterWindowMessage( _T( "SOURCESNOWPLAYING" ) );
int WM_SOURCESSTDPLAYLIST	= RegisterWindowMessage( _T( "SOURCESSTDPLAYLIST" ) );
int WM_SOURCESDYNPLAYLIST	= RegisterWindowMessage( _T( "SOURCESDYNPLAYLIST" ) );
int WM_SOURCESQUICKSEARCH	= RegisterWindowMessage( _T( "SOURCESQUICKSEARCH" ) );
int WM_SOURCESQUICKSEARCHSTR= RegisterWindowMessage( _T( "SOURCESQUICKSEARCHSTR" ) );
int WM_SOURCESITEMCLICKED	= RegisterWindowMessage( _T( "SOURCESITEMCLICKED" ) );
int WM_SOURCESCDAUDIO       = RegisterWindowMessage( _T( "SOURCESCDAUDIO" ) );
int WM_SOURCESUNSEL			= RegisterWindowMessage( _T( "SOURCESUNSEL" ) );
int WM_SOURCESQUERYFREEDB   = RegisterWindowMessage( _T( "SOURCESQUERYFREEDB" ) );
int WM_SOURCESRIPCD         = RegisterWindowMessage( _T( "SOURCESRIPCD" ) );
int WM_SOURCESREMOVABLEDEV  = RegisterWindowMessage( _T( "SOURCESREMOVABLEDEV" ) );
int WM_SOURCESREFRESH		= RegisterWindowMessage( _T( "SOURCESREFRESH" ) );
int WM_SOURCESRESET			= RegisterWindowMessage( _T( "SOURCESRESET" ) );
int WM_SOURCESNETSTREAMS	= RegisterWindowMessage( _T( "SOURCESNETSTREAMS" ) );
int WM_SOURCESCOLLECTED		= RegisterWindowMessage( _T( "SOURCESCOLLECTED" ) );
int WM_UPDATEPLAYLIST		= RegisterWindowMessage( _T( "UPDATEPLAYLIST" ) );
int WM_NOWPLAYINGUPDATED	= RegisterWindowMessage( _T( "NOWPLAYINGUPDATED" ) );
int WM_PLAYERUPDATE			= RegisterWindowMessage( _T( "PLAYERUPDATE" ) );
int WM_DRAGSTART			= RegisterWindowMessage( _T( "DRAGSTART" ) );
int WM_DRAGEND				= RegisterWindowMessage( _T( "DRAGEND" ) );
int WM_PLAYSONG				= RegisterWindowMessage( _T( "PLAYSONG" ) );
int WM_CLOSEDIRSYNC			= RegisterWindowMessage( _T( "CLOSEDIRSYNC" ) );
int WM_SONGCHANGE			= RegisterWindowMessage( _T( "SONGCHANGE" ) );
int WM_SONGSTOP				= RegisterWindowMessage( _T( "SONGSTOP" ) );
int WM_SONGPAUSE			= RegisterWindowMessage( _T( "SONGPAUSE" ) );
int WM_SONGRESUME			= RegisterWindowMessage( _T( "SONGRESUME" ) );
int WM_EQUALIZERCHANGE		= RegisterWindowMessage( _T( "EQUALIZERCHANGE" ) );
int WM_PLAYER_PLAYSEL		= RegisterWindowMessage( _T( "PLAYER_PLAYSEL" ) );
int WM_PLAYER_NEXT			= RegisterWindowMessage( _T( "PLAYER_NEXT" ) );
int WM_PLAYER_PREV			= RegisterWindowMessage( _T( "PLAYER_PREV" ) );
int WM_PLAYER_STOP			= RegisterWindowMessage( _T( "PLAYER_STOP" ) );
int WM_TASK_PROGRESS		= RegisterWindowMessage( _T( "TASK_PROGRESS" ) );
int WM_TASK_END				= RegisterWindowMessage( _T( "TASK_END" ) );
int WM_INITTRANS			= RegisterWindowMessage( _T( "INITTRANS" ) );
int WM_DEINITTRANS			= RegisterWindowMessage( _T( "DEINITTRANS" ) );
int WM_BATCHADD_VERIFY_PLAYLIST	= RegisterWindowMessage( _T( "BATCHADD_VERIFY_PLAYLIST" ) );
int WM_TASKBARCREATED		= RegisterWindowMessage( _T( "TaskbarCreated" ) );
int WM_SHOWMUSIKCUBE		= RegisterWindowMessage( _T( "SHOWMUSIKCUBE" ) );
int WM_HIDEMUSIKCUBE		= RegisterWindowMessage( _T( "HIDEMUSIKCUBE" ) );
int WM_UPDATEVOLUME			= RegisterWindowMessage( _T( "UPDATEVOLUME" ) );

///////////////////////////////////////////////////

