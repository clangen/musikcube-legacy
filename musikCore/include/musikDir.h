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

#ifndef C_MUSIK_DIR_H
#define C_MUSIK_DIR_H

///////////////////////////////////////////////////

#include "musikConfig.h"
#include "musikFunctor.h"

#if !defined(WIN32)
	#include <dirent.h>
#endif

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

/**
A crossplatform directory traverser (scanner/walker).

@author Casey Langen
@version alpha 0.7
*/
class MUSIKEXPORT Dir
{
public:

	/**
	 * \param dir directory to be traversed.
	 * \param target array to accept filenames.
	 */
	static void OpenDir( String dir, StringArray* target );
};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
