///////////////////////////////////////////////////

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers
#endif

///////////////////////////////////////////////////

#ifndef _WIN32_WINNT            // Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0501        // Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif                        

#ifndef _WIN32_IE                // Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0600        // Change this to the appropriate value to target IE 5.0 or later.
#endif

///////////////////////////////////////////////////

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS    // some CString constructors will be explicit
#define _AFX_ALL_WARNINGS                    // turns off MFC's hiding of some common and often safely ignored warning messages

///////////////////////////////////////////////////

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>        // MFC support for Internet Explorer 4 Common Controls
#include <afxcmn.h>            // MFC support for Windows Common Controls
#include <shlwapi.h>
#include <afxole.h>
#include <afxdlgs.h>
#include <afxctl.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxsock.h>        // MFC socket support
#include <afxtempl.h>       // MFC template support
#include <afxpriv.h>

///////////////////////////////////////////////////

#include <mmsystem.h>
#include <gdiplus.h>

///////////////////////////////////////////////////

#define _SCB_REPLACE_MINIFRAME
#include "3rdparty/sizecbar.h"

///////////////////////////////////////////////////

#ifndef MUSIK_VERSION_STR
#define MUSIK_VERSION_STR "musikCube" 
#endif

///////////////////////////////////////////////////

#define AUTOMATIC_COLOR 4278190080    

///////////////////////////////////////////////////

