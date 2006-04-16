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

#ifndef C_MUSIK_BATCHADD_H
#define C_MUSIK_BATCHADD_H

///////////////////////////////////////////////////

#include "musikLibrary.h"
#include "musikPlayer.h"
#include "musikFunctor.h"
#include "musikTask.h"

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

/**
A struct that contains information that will be passed
to a BatchAddTask. Contained information includes a list
of files to add, a playlist to add the files to, a library to
store information to, a player, and a functor to call for
various events.

@author Casey Langen
@version alpha 0.7
 */
struct MUSIKEXPORT BatchAdd
{
	BatchAdd();
	BatchAdd( StringArray* pFiles, const String& path, Playlist* pPlaylist, Library* pLibrary, Player* pPlayer, 
		Functor* pFunctor, bool bUpdatePlaylist = false, bool bAddToPlayer = false, 
        bool bDeleteFilelist = true );

	~BatchAdd();

	/**
	 * Required. List of files to be added to the library.
	 */
	StringArray* m_Files;
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
	 * Optional. If m_AddToPlayer is true, songs will be added
	 * to the m_Player's current playlist once verified.
	 */
	Player* m_Player;
	/**
	 * Optional. Functor to post events to. Defaults to NULL.
	 */
	Functor* m_Functor;
	/**
	 * Optional. Once a song has been added to the library, append it to
	 * m_Playlist as well. Defaults to false.
	 */
	bool m_UpdatePlaylist;
	/**
	 * Optional. Delete the passed file list once thread execution completes.
	 * Defaults to true.
	 */
	bool m_DeleteFilelist;
	/**
	 * Optional. If true, files will be added to m_Player's Playlist once
	 * verified and added to the library. Defaults to false.
	 */
	bool m_AddToPlayer;			
	/**
	 * Optional. Call m_Functor->OnThreadEnd() if process is aborted? Defaults
	 * to false.
	 */
	bool m_CallFunctorOnAbort;
    /**
     * Optional. Will add files in this directory.
     */
    String m_Directory;

    bool m_Clean;
};

///////////////////////////////////////////////////

/**
A task (thread) that batch adds a list of files to a Library
as specified in a BatchAddTask

@author Casey Langen
@version alpha 0.7
 */
class MUSIKEXPORT BatchAddTask : public Task
{

public:

	BatchAddTask();
	~BatchAddTask();

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
	BatchAdd* m_Params;
};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
