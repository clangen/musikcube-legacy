//////////////////////////////////////////////////
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
//   Casey Langen   : Lead Developer, Project Manager. E-Mail: casey@bak.rr.com
//   Dustin Carter  : Developer
//   Simon Windmill : Developer
//
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////

#ifndef __MUSIK_ALARM_H__
#define __MUSIK_ALARM_H__

///////////////////////////////////////////////////

#include <OpenThreads/Mutex>
using namespace OpenThreads;

#include "musikConfig.h"
#include "musikPlayer.h"
#include "musikLibrary.h"

#include "time.h"

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////
    
MUSIKEXPORT enum
{
    MUSIK_ALARM_TYPE_ONESHOT = 0,
    MUSIK_ALARM_TYPE_DAILY
};    

///////////////////////////////////////////////////

MUSIKEXPORT enum
{
    MUSIK_ALARM_STATE_BEEPING = 0,
    MUSIK_ALARM_STATE_SNOOZE,
    MUSIK_ALARM_STATE_NORMAL
};

///////////////////////////////////////////////////

struct MUSIKEXPORT AlarmInfo
{
	AlarmInfo();

	int m_PlaylistID;
    int m_Type;
    int m_MinVol;
	int m_SnoozeTime;
    int m_Hours;
    int m_Minutes;
    int m_Day;
    int m_Month;
    int m_Year;
	int m_ID;
};

///////////////////////////////////////////////////

class MUSIKEXPORT AlarmMonitor : public Task
{
public:
    
    AlarmMonitor();
    AlarmMonitor( Player* player, Library* library );
    ~AlarmMonitor();

    void SetPlayer( Player* player ){ m_Player = player; }
    void SetLibrary( Library* library ){ m_Library = library; }

	void RequeryAlarms();

	void Snooze();
    void StopBeeping();

	virtual void run();
    
private:

	vector<AlarmInfo> m_Alarms;
    
    Player* m_Player;
    Library* m_Library;
	Mutex state_mutex;
	int m_State;

	bool AlarmTriggered( AlarmInfo& curr_info );

	// used by AlarmTriggered()
	time_t curr_time;
	tm *ptm;
	
};

///////////////////////////////////////////////////
    
} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
