///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
//   multimedia library. More information at:
//
//     http://www.musikcube.com
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

#ifndef __MUSIK_CORE_H__
#define __MUSIK_CORE_H__

///////////////////////////////////////////////////

#if defined (WIN32) || defined( __MINGW32__ )
#define SQLITE_VER _T( "3.2.6" )
#define TAGLIB_VER _T( "1.4" )
#define OPENTHREADS_VER _T( "1.2dev2" )
#define BASS_VER _T( "2.2.0.3" )
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

