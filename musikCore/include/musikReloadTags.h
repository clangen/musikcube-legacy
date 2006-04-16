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

#ifndef C_MUSIK_RELOADTAGS_H
#define C_MUSIK_RELOADTAGS_H

///////////////////////////////////////////////////

#include "musikLibrary.h"
#include "musikPlayer.h"
#include "musikFunctor.h"
#include "musikTask.h"

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

struct MUSIKEXPORT ReloadTags
{
	ReloadTags( Functor* functor = NULL, Playlist* playlist = NULL, Library* library = NULL );
	~ReloadTags();

	/**
	 * Optional. Playlist to add files to once verified and
	 * added to the library. m_UpdatePlaylist must be true.
	 */
	Playlist* m_Playlist;
	/**
	 * Required. Library to add files to
	 */
	Library* m_Library;
	/**
	* Required. Functor to post events to
	*/
	Functor* m_Functor;

	bool m_CallFunctorOnAbort;
};

///////////////////////////////////////////////////

class MUSIKEXPORT ReloadTagsTask : public Task
{

public:

	ReloadTagsTask();
	~ReloadTagsTask();

	/**
	 * Call this to start the task going. m_Params must be set
	 * prior to calling run().
	 */
	void run();

	/**
	 * If true the thread will call "delete this" once its run() has completed.
	 * This only has an effect in non-win32 (linux) environments. In win32 the
	 * thread cannot delete itself. If true in a win32 environment an assertion 
	 * will be thrown.
	 */
	ReloadTags* m_Params;
};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
