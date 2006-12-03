///////////////////////////////////////////////////
//
// License Agreement:
//
// The following are Copyright © 2002-2006, Casey Langen
//
// Sources and Binaries of: musikCore musikCube musikBox
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright 
//      notice, this list of conditions and the following disclaimer in the 
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the author nor the names of other contributors may 
//      be used to endorse or promote products derived from this software 
//      without specific prior written permission. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE. 
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

extern int WM_SELBOXUPDATE;
extern int WM_SELBOXEDITCOMMIT;
extern int WM_SELBOXADDREMOVE;
extern int WM_SELBOXREQUESTUPDATE;
extern int WM_SELECTIONCTRLDELETESELECTED;
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
extern int WM_SOURCESREMOVABLEDEV;
extern int WM_SOURCESREFRESH;
extern int WM_SOURCESRESET;
extern int WM_SOURCESNETSTREAMS;
extern int WM_SOURCESUNSEL;

extern int WM_UPDATEPLAYLIST;

extern int WM_NOWPLAYINGUPDATED;
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
