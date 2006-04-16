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
//   to be managed, and a Library object to query.
//
//   When the range of managed songs is known, pass it via
//   SetRange. The DynDspInfo() class is now ready
//   to be queryed.
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "musikDynDspInfo.h"

///////////////////////////////////////////////////

DynDspInfo::DynDspInfo( musikCore::Playlist* songs )
{
	m_Songs = songs;
	m_LastItemRange.Set( -999, -999 );
}

///////////////////////////////////////////////////

void DynDspInfo::SetPlaylist( musikCore::Playlist* playlist )
{
	m_Songs = playlist;
}

///////////////////////////////////////////////////

void DynDspInfo::Set( int from, int to, bool force_update )
{ 
	// set the current item range
	m_ItemRange.Set( from, to + 1 );

	// see if our new item range is the same as our 
	// last item range... if so, return
	if ( !force_update )
	{
		if ( m_ItemRange.GetFirst() == m_LastItemRange.GetFirst() &&
			m_ItemRange.GetLast() == m_LastItemRange.GetLast() )
			return;
	}

	// we got a 0,0 range, so check to see if there
	// are any songs in the playlist. if there are
	// not then that means nothing has been selected,
	// and this cache hint was sent even though its not
	// needed. just return.
	if ( m_ItemRange.GetLast() > (int)m_Songs->GetCount() )
		return;

	// grab all the new items
	musikCore::Library::GetInfoArrayFromPlaylist(
		*m_Songs, 
		m_Items,
		(size_t)m_ItemRange.GetFirst(), 
		(size_t)m_ItemRange.GetLast() );

	/*
	SongInfo item;
	int nID;
	for ( int i = m_ItemRange.GetFirst(); i < m_ItemRange.GetLast(); i++ )
	{
		nID = m_Songs->GetSongID( i );

		musikCore::Library* curr_lib = m_Songs->at( i ).GetLibrary();
        curr_lib->GetSongInfoFromID( nID, item );

		m_Items.push_back( item );
	}
	*/

	
	// store the last known range, so we
	// can later compare it to anew one
	m_LastItemRange = m_ItemRange;
}	

///////////////////////////////////////////////////

musikCore::String DynDspInfo::GetValue( int id, int field )
{
	// this is kludge, but right now i don't have
	// the time to figure out why exactly its happening...


	try
	{
		if ( id > (int)m_Items.size() || id < 0 )
		{
			#ifdef _DEBUG
				musikCore::String sErr;
				sErr.Format( _T( "DynDspInfo failed. Item %d was requested, but there are only %d items\n" ), id, m_Items.size()-1 );
				cout << sErr.c_str();
			#endif
			
			return _T( "[musik.cache]" );
		}

		return m_Items.at( id ).GetField( field );
	}
	catch (...)
	{
#ifdef WIN32
		cout << _T( "DynDspInfo::GetValue: error" ) << endl; 
#endif
	}

	return _T( "[musik.cache]" );
}	

///////////////////////////////////////////////////

bool DynDspInfo::ResyncItem( int songid )
{
	if ( songid == -1 )
		return false;

	for ( size_t i = 0; i < m_Items.size(); i++ )
	{
		if ( m_Items.at( i ).GetID() == songid )
		{
			musikCore::SongInfo item;
			musikCore::Library* curr_lib = m_Items.at( i ).GetLibrary();
			curr_lib->GetSongInfoFromID( songid, item );

			m_Items.at( i ) = item;
			return true;
		}
	}

	return false;
}

///////////////////////////////////////////////////	

musikCore::SongInfoArray* DynDspInfo::items()
{ 
	return &m_Items; 
}

///////////////////////////////////////////////////

int DynDspInfo::GetDistance()
{
	return m_ItemRange.GetDistance();
}

///////////////////////////////////////////////////

int DynDspInfo::GetFirst()
{
	return m_ItemRange.GetFirst();
}

///////////////////////////////////////////////////

int DynDspInfo::GetLast()
{
	return m_ItemRange.GetLast();
}

///////////////////////////////////////////////////

