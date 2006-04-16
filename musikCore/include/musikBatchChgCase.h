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

#ifndef C_MUSIK_BATCHCHGCASE_H
#define C_MUSIK_BATCHCHGCASE_H

///////////////////////////////////////////////////

#include "musikConfig.h"
#include "musikLibrary.h"
#include "musikFunctor.h"

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

MUSIKEXPORT enum
{
	MUSIK_CHGCASE_TOLOWER,
	MUSIK_CHGCASE_TOUPPER,
	MUSIK_CHGCASE_CAPITALIZE,
    MUSIK_CHGCASE_CLEAN,
	MUSIK_CHGCASE_REMOVEWHITESPACE
};

///////////////////////////////////////////////////
	
/**
A struct that contains information that will be passed
to a BatchChgCaseTask. Contained information includes a pointer
to a Library to store data to, a functor to post events to, and
a SongInfoArray containing the information to be written to file.

@author Casey Langen
@version alpha 0.7
 */
struct MUSIKEXPORT BatchChgCase
{
	/**
	 * Default constructor.
	 */
	BatchChgCase();
	/**
	 * \param pLibrary library used for transactions. Must not be NULL.
	 * \param pFunctor functor to receive events. Must not be NULL.
	 * \param pUpdatedTags tags to be updated. Must not be NULL.
	 */
	BatchChgCase( Library* pLibrary, Functor* pFunctor, SongInfoArray* pUpdatedTags, int nType );
	~BatchChgCase();

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
	 * The type of operation to perform on the passed songs. Valid types
	 * include MUSIK_CHGCASE_TOLOWER, MUSIK_CHGCASE_TOUPPER, 
	 * MUSIK_CHGCASE_CAPITALIZE
	 */
	int m_Type;
};

///////////////////////////////////////////////////

/**
A task (thread) that changes the case of selected
tags.

@author Casey Langen
@version alpha 0.7
 */
class MUSIKEXPORT BatchChgCaseTask : public Task
{

public:

	BatchChgCaseTask();
	~BatchChgCaseTask();

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
	BatchChgCase* m_Params;

private:

	void CapWord( String& inStr );
	String Capitalize( const StringArray& lower_words, const String& str );
	String RemoveWhite( String str );
	String Clean( const StringArray& lower_words, const String& input );
};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
