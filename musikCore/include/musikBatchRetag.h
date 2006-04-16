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

#ifndef C_MUSIK_BATCHRETAG_H
#define C_MUSIK_BATCHRETAG_H

///////////////////////////////////////////////////

#include "musikConfig.h"
#include "musikLibrary.h"
#include "musikFunctor.h"

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

/**
A struct that contains information that will be passed
to a BatchRetagTask. Contained information includes a pointer
to a Library to store data to, a functor to post events to, and
a SongInfoArray containing the information to be written to file.

@author Casey Langen
@version alpha 0.7
 */
struct MUSIKEXPORT BatchRetag
{
	/**
	 * Default constructor.
	 */
	BatchRetag();
	/**
	 * \param pLibrary library used for transactions. Must not be NULL.
	 * \param pFunctor functor to receive events. Must not be NULL.
	 * \param pUpdatedTags tags to be updated. Must not be NULL.
	 */
	BatchRetag( Library* pLibrary, Functor* pFunctor, SongInfoArray* pUpdatedTags );
	~BatchRetag();

	/**
	 * Required. Library that the updated data will be stored to.
	 */
	Library* m_Library;
	/**
	 * Optional. Functor that will recieve events. 
	 */
	Functor* m_Functor;
	/**
	 * Required. The array of updated tags.
	 */
	SongInfoArray* m_UpdatedTags;
	/**
	 * Optional. Call m_Functor->OnThreadEnd() even if aborted. 
	 * Defaults false.
	 */
	bool m_CallFunctorOnAbort;
	/**
	 * Optional. Delete m_UpdatedTags on completion. Defaults
	 * to true.
	 */
	bool m_DeleteUpdatedTags;
	/**
	 * Optional. Write the tags to file as well. Defaults to false.
	 */
	bool m_WriteToFile;
};

///////////////////////////////////////////////////

/**
A task (thread) that batch updates a list of songs based
on a respective SongInfoArray. 

@author Casey Langen
@version alpha 0.7
 */
class MUSIKEXPORT BatchRetagTask : public Task
{

public:

	BatchRetagTask();
	~BatchRetagTask();

	/**
	 * Start task execution. m_Params is requird to be initialized
	 * before calling this function. If m_Params is uninitalized
	 * the task will simply return.
	 */
	void run();

	/**
	 * Required. If uninitialized the task will fail to run and
	 * return before any processing is done. 
	 */
	BatchRetag* m_Params;

};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
