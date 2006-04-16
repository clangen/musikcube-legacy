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

extern int WM_SELBOXUPDATE;
extern int WM_SELBOXFILTER;
extern int WM_SELBOXEDITCOMMIT;
extern int WM_SELBOXADDREMOVE;
extern int WM_SELBOXREQUESTUPDATE;
extern int WM_SELBOXDELSEL;
extern int WM_SELBOXDESEL;
extern int WM_SELBOXRESET;
extern int WM_SELBOXACTIVATE;

extern int WM_SOURCESLIBRARY;
extern int WM_SOURCESNOWPLAYING;
extern int WM_SOURCESSTDPLAYLIST;
extern int WM_SOURCESDYNPLAYLIST;
extern int WM_SOURCESQUICKSEARCH;
extern int WM_SOURCESQUICKSEARCHSTR;
extern int WM_SOURCESITEMCLICKED;
extern int WM_SOURCESCDAUDIO;
extern int WM_SOURCESQUERYFREEDB;
extern int WM_SOURCESRIPCD ;
extern int WM_SOURCESREMOVABLEDEV;
extern int WM_SOURCESREFRESH;
extern int WM_SOURCESRESET;
extern int WM_SOURCESNETSTREAMS;
extern int WM_SOURCESCOLLECTED;
extern int WM_SOURCESUNSEL;

extern int WM_UPDATEPLAYLIST;

extern int WM_NOWPLAYINGUPDATED;
extern int WM_PLAYERUPDATE;
extern int WM_DRAGSTART;
extern int WM_DRAGEND;
extern int WM_PLAYSONG;
extern int WM_UPDATEVOLUME;

extern int WM_CLOSEDIRSYNC;

// we get these ones from the player
// via a CmusikFrmFunctor after a
// certain operation as completed
extern int WM_SONGCHANGE;
extern int WM_SONGSTOP;
extern int WM_SONGPAUSE;
extern int WM_SONGRESUME;
extern int WM_EQUALIZERCHANGE;

// these will come from all over the UI,
// such as the now playing control and 
// task tray button. they are commands to
// send the player.
extern int WM_PLAYER_PLAYSEL;
extern int WM_PLAYER_NEXT;
extern int WM_PLAYER_PREV;
extern int WM_PLAYER_STOP;

// functors post us...
extern int WM_TASK_PROGRESS;
extern int WM_TASK_END;
extern int WM_BATCHADD_VERIFY_PLAYLIST;

// other dialogs, such as dockable windows, can
// call these to disable and enable transparency
// effects
extern int WM_INITTRANS;
extern int WM_DEINITTRANS;

extern int WM_TASKBARCREATED;
extern int WM_SHOWMUSIKCUBE;
extern int WM_HIDEMUSIKCUBE;

///////////////////////////////////////////////////