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

#ifndef __MUSIK_CORE_H__
#define __MUSIK_CORE_H__

///////////////////////////////////////////////////

#if defined (WIN32) || defined(__MINGW32__)
#define SQLITE_VER _T("3.2.6")
#define TAGLIB_VER _T("1.4")
#define OPENTHREADS_VER _T("1.2dev2")
#define BASS_VER _T("2.3.0.1")
#endif

#include "musikConfig.h"
#include "musikLibrary.h"
#include "musikPlayer.h"
#include "musikTask.h"
#include "musikFilename.h"
#include "musikBatchAdd.h"
#include "musikRemoveOld.h"
#include "musikBatchRetag.h"
#include "musikBatchChgCase.h"
#include "musikReloadTags.h"
#include "musikTagToFn.h"
#include "musikTagFromFn.h"
#include "musikDir.h"
#include "musikTaskQueue.h"
#include "musikAlarm.h"
#include "musikPlayerBASS.h"

#ifdef WIN32
#include "musikPlayerBASS.h"
#endif

#if defined (USE_XINE)
#include "musikPlayerXINE.h"
#endif

#if defined (USE_FMOD)
#include "musikPlayerFMOD.h"
#endif

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////

