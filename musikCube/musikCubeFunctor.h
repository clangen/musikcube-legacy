///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
//   multimedia library. More information at:
//
//     http://musik.berlios.de
//     http://musik.sourceforge.net
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
//
// Class(es): 
//
//   CmusikCubeFunctor
//
// Filename(s): 
//
//   musikCubeFunctor.h, musikCubeFunctor.cpp
//
// Information:
//
//   musikCore::Functor subclass that is used for communication between
//   the musikCore::Player and CmusikCubeFrame.
// 
///////////////////////////////////////////////////

#pragma once
#include <musikCore.h>

///////////////////////////////////////////////////

class CmusikCubeFunctor : public musikCore::Functor
{

public:

	CmusikCubeFunctor( CWnd* parent);

	virtual void OnNewSong();
	virtual void OnNewEqualizer();
	virtual void OnStop();
	virtual void OnPause();
	virtual void OnResume();
	virtual void OnPlaybackFailed();
	virtual void OnAlarmTrigger();
	virtual void OnSeek();

	virtual void OnTaskStart();
	virtual void OnTaskEnd( musikCore::Task* task_addr = NULL );
	virtual void OnTaskProgress( size_t progress, musikCore::Task* task_addr );
	virtual bool VerifyPlaylist( void* pl_addr = NULL );

private:

	CWnd* m_Parent;

};

///////////////////////////////////////////////////