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

#include "stdafx.h"
#include "musikCubeMessages.h"

///////////////////////////////////////////////////

int WM_SELBOXUPDATE                 = RegisterWindowMessage(_T("SELBOXUPDATE"));
int WM_SELBOXEDITCOMMIT             = RegisterWindowMessage(_T("SELBOXEDITCOMMIT"));
int WM_SELBOXADDREMOVE              = RegisterWindowMessage(_T("SELBOXADDREMOVE"));
int WM_SELBOXREQUESTUPDATE          = RegisterWindowMessage(_T("SELBOXREQUESTUPDATE"));
int WM_SELECTIONCTRLDELETESELECTED  = RegisterWindowMessage(_T("SELECTIONCTRLDELETESELECTED"));
int WM_SELBOXRESET                  = RegisterWindowMessage(_T("SELBOXRESET"));
int WM_SELBOXACTIVATE               = RegisterWindowMessage(_T("SELBOXACTIVATE"));
int WM_SOURCESLIBRARY               = RegisterWindowMessage(_T("SOURCESLIBRARY"));
int WM_SOURCESNOWPLAYING            = RegisterWindowMessage(_T("SOURCESNOWPLAYING"));
int WM_SOURCESSTDPLAYLIST           = RegisterWindowMessage(_T("SOURCESSTDPLAYLIST"));
int WM_SOURCESDYNPLAYLIST           = RegisterWindowMessage(_T("SOURCESDYNPLAYLIST"));
int WM_SOURCESQUICKSEARCH           = RegisterWindowMessage(_T("SOURCESQUICKSEARCH"));
int WM_SOURCESQUICKSEARCHSTR        = RegisterWindowMessage(_T("SOURCESQUICKSEARCHSTR"));
int WM_SOURCESITEMCLICKED           = RegisterWindowMessage(_T("SOURCESITEMCLICKED"));
int WM_SOURCESCDAUDIO               = RegisterWindowMessage(_T("SOURCESCDAUDIO"));
int WM_SOURCESUNSEL                 = RegisterWindowMessage(_T("SOURCESUNSEL"));
int WM_SOURCESQUERYFREEDB           = RegisterWindowMessage(_T("SOURCESQUERYFREEDB"));
int WM_SOURCESREMOVABLEDEV          = RegisterWindowMessage(_T("SOURCESREMOVABLEDEV"));
int WM_SOURCESREFRESH               = RegisterWindowMessage(_T("SOURCESREFRESH"));
int WM_SOURCESRESET                 = RegisterWindowMessage(_T("SOURCESRESET"));
int WM_SOURCESNETSTREAMS            = RegisterWindowMessage(_T("SOURCESNETSTREAMS"));
int WM_UPDATEPLAYLIST               = RegisterWindowMessage(_T("UPDATEPLAYLIST"));
int WM_NOWPLAYINGUPDATED            = RegisterWindowMessage(_T("NOWPLAYINGUPDATED"));
int WM_DRAGSTART                    = RegisterWindowMessage(_T("DRAGSTART"));
int WM_DRAGEND                      = RegisterWindowMessage(_T("DRAGEND"));
int WM_PLAYSONG                     = RegisterWindowMessage(_T("PLAYSONG"));
int WM_SONGCHANGE                   = RegisterWindowMessage(_T("SONGCHANGE"));
int WM_SONGSTOP                     = RegisterWindowMessage(_T("SONGSTOP"));
int WM_SONGPAUSE                    = RegisterWindowMessage(_T("SONGPAUSE"));
int WM_SONGRESUME                   = RegisterWindowMessage(_T("SONGRESUME"));
int WM_EQUALIZERCHANGE              = RegisterWindowMessage(_T("EQUALIZERCHANGE"));
int WM_PLAYER_PLAYSEL               = RegisterWindowMessage(_T("PLAYER_PLAYSEL"));
int WM_PLAYER_NEXT                  = RegisterWindowMessage(_T("PLAYER_NEXT"));
int WM_PLAYER_PREV                  = RegisterWindowMessage(_T("PLAYER_PREV"));
int WM_PLAYER_STOP                  = RegisterWindowMessage(_T("PLAYER_STOP"));
int WM_TASK_PROGRESS                = RegisterWindowMessage(_T("TASK_PROGRESS"));
int WM_TASK_END                     = RegisterWindowMessage(_T("TASK_END"));
int WM_INITTRANS                    = RegisterWindowMessage(_T("INITTRANS"));
int WM_DEINITTRANS                  = RegisterWindowMessage(_T("DEINITTRANS"));
int WM_BATCHADD_VERIFY_PLAYLIST     = RegisterWindowMessage(_T("BATCHADD_VERIFY_PLAYLIST"));
int WM_TASKBARCREATED               = RegisterWindowMessage(_T("TaskbarCreated"));
int WM_SHOWMUSIKCUBE                = RegisterWindowMessage(_T("SHOWMUSIKCUBE"));
int WM_HIDEMUSIKCUBE                = RegisterWindowMessage(_T("HIDEMUSIKCUBE"));
int WM_UPDATEVOLUME                 = RegisterWindowMessage(_T("UPDATEVOLUME"));

///////////////////////////////////////////////////