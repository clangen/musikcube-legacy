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

#ifndef C_MUSIK_REMOVEOLD_H
#define C_MUSIK_REMOVEOLD_H

///////////////////////////////////////////////////

#include "musikConfig.h"
#include "musikLibrary.h"
#include "musikFunctor.h"

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

/**
a Struct that contains information that will be passed
to a RemoveOldTask. Contained information includes a pointer
to a Library used to retrieve and store data and a functor
to post events to.

@author Casey Langen
@version alpha 0.7
 */
struct MUSIKEXPORT RemoveOld
{
	RemoveOld();
	RemoveOld( Library* pLibrary, Functor* pFunctor );
	~RemoveOld(){}

	/**
	 * Required. The Library used for transactions.
	 */
	Library* m_Library;
	/**
	 * Optional. The functor used to post events to.
	 */
	Functor* m_Functor;
	/**
	 * Optional. If true m_Functor->OnThreadEnd() will be called
	 * even if the task is aborted. Defaults false.
	 */
	bool m_CallFunctorOnAbort;
};

///////////////////////////////////////////////////

class MUSIKEXPORT RemoveOldTask : public Task
{

public:

	RemoveOldTask();
	~RemoveOldTask();

	/**
	 * Start task execution. m_Params must be initialized.
	 */
	void run();

	/**
	 * Required. If uninitialized the task will return immediately.
	 */
	RemoveOld* m_Params;
};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
