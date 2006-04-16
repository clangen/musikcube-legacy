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
//
// Class(es): 
//
//   DspInfoRange, 
//   DynDspInfo
//
// Filename(s): 
//
//   musikDynDspInfo.h,
//
// Information:
//
//   This is musik's dynamic display (playlist) view
//   cache. It is designed to be a generic way for any
//   UI to effectively manage memory when dealing with
//   Playlist objects. 
// 
// Example: 
//
//   MFC's CListCtrl's Cache-Hint mechanism sends
//   a range of items in a control must be managed.
//
// Usage: 
//
//   Construct it with a pointer to a Playlist object
//   to be managed, and a CmusikLary objecibrt to query.
//
//   When the range of managed songs is known, pass it via
//   SetRange. The DynDspInfo() class is now ready
//   to be queryed.
//
///////////////////////////////////////////////////

#ifndef C_MUSIK_SONG_INFO_CACHE
#define C_MUSIK_SONG_INFO_CACHE

///////////////////////////////////////////////////

#include <musikCore.h>
#include <iostream>

///////////////////////////////////////////////////

class DspInfoRange
{
public:

	DspInfoRange(){ m_First = m_Last = 0; }
	~DspInfoRange(){}

	void Set( int first, int last ){ m_First = first; m_Last = last; }

	int GetDistance(){ return m_Last - m_First; }
	int GetFirst(){ return m_First; }
	int GetLast(){ return m_Last; }
	bool IsNull(){ return ( GetDistance() == 0 ? true : false ); }

	int m_First, m_Last;
};

///////////////////////////////////////////////////

class DynDspInfo
{
public:

	// construct and destruct
	DynDspInfo( musikCore::Playlist* songs );
	~DynDspInfo(){}

	// sets
	void SetPlaylist( musikCore::Playlist* playlist );
	void Set( int from, int to, bool force_update );

	// gets
	musikCore::String GetValue( int id, int field );	
	musikCore::SongInfoArray* items();
	int GetDistance();
	int GetFirst();
	int GetLast();

	// actions
	bool ResyncItem( int songid );

	DspInfoRange m_LastItemRange;

private:

	DspInfoRange m_ItemRange;
	musikCore::SongInfoArray m_Items;
	musikCore::Playlist* m_Songs;

};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
