///////////////////////////////////////////////////

#ifndef __C_MUSIK_LIBRARY_CALLBACKS_H__
#define __C_MUSIK_LIBRARY_CALLBACKS_H__

///////////////////////////////////////////////////

#include "musikLibrary.h"
#include "musikPlaylist.h"

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

static int sqlite_AddSongToPlaylist( void *args, int numCols, char **results, char ** columnNames )
{
	Song *pLibItem = new Song();
	pLibItem->SetID( atoi( results[0] ) ); 

	if ( numCols == 3 )
	{
		pLibItem->SetDuration( atoi( results[1] ) / 1000 );
		pLibItem->SetFilesize( atoi( results[2] ) );
	}

	Playlist* p = (Playlist*)args;
	p->Add( *pLibItem );

	delete pLibItem;
	return 0;
}

///////////////////////////////////////////////////

static int sqlite_GetSongAttr( void *args, int numCols, char **results, char ** columnNames )
{
	Song* p = (Song*)args;

	p->SetID( atoi( results[0] ) ); 
	p->SetDuration( atoi( results[1] ) / 1000 );
	p->SetFilesize( atoi( results[2] ) );

	return 0;
}

///////////////////////////////////////////////////

static int sqlite_GetFieldFromID( void *args, int numCols, char **results, char ** columnNames )
{
	String* p = (String*)args;
	*p = String( utf8to16( results[0] ) ); 

	return 0;
}

///////////////////////////////////////////////////

static int sqlite_GetCrossfader( void *args, int numCols, char **results, char ** columnNames )
{
	Crossfader* p = (Crossfader*)args;

	p->Set( 
		(float)atof( results[0] ), 
		(float)atof( results[1] ), 
		(float)atof( results[2] ), 
		(float)atof( results[3] ), 
		(float)atof( results[4] ) );

	if ( numCols == 6 )
		p->SetName( utf8to16( results[5] ) );
	else
		p->SetName( _T( "Default" ) );

	return 0;
}

///////////////////////////////////////////////////

static int sqlite_GetEqualizer( void *args, int numCols, char **results, char ** columnNames )
{
	EQSettings* p = (EQSettings*)args;

	int offset;
	if ( numCols == 37 )
		offset = 0;
	else if ( numCols == 39 )
		offset = 2;
	else 
		return -1;

	p->m_Left[0]		= (float)atof( results[offset + 1] );	
	p->m_Left[1]		= (float)atof( results[offset + 2] );
	p->m_Left[2]		= (float)atof( results[offset + 3] );
	p->m_Left[3]		= (float)atof( results[offset + 4] );
	p->m_Left[4]		= (float)atof( results[offset + 5] );
	p->m_Left[5]		= (float)atof( results[offset + 6] );
	p->m_Left[6]		= (float)atof( results[offset + 7] );	
	p->m_Left[7]		= (float)atof( results[offset + 8] );
	p->m_Left[8]		= (float)atof( results[offset + 9] );
	p->m_Left[9]		= (float)atof( results[offset + 10] );
	p->m_Left[10]		= (float)atof( results[offset + 11] );
	p->m_Left[11]		= (float)atof( results[offset + 12] );
	p->m_Left[12]		= (float)atof( results[offset + 13] );	
	p->m_Left[13]		= (float)atof( results[offset + 14] );
	p->m_Left[14]		= (float)atof( results[offset + 15] );
	p->m_Left[15]		= (float)atof( results[offset + 16] );
	p->m_Left[16]		= (float)atof( results[offset + 17] );
	p->m_Left[17]		= (float)atof( results[offset + 18] );
	p->m_Right[0]		= (float)atof( results[offset + 19] );	
	p->m_Right[1]		= (float)atof( results[offset + 20] );
	p->m_Right[2]		= (float)atof( results[offset + 21] );
	p->m_Right[3]		= (float)atof( results[offset + 22] );
	p->m_Right[4]		= (float)atof( results[offset + 23] );
	p->m_Right[5]		= (float)atof( results[offset + 24] );
	p->m_Right[6]		= (float)atof( results[offset + 25] );	
	p->m_Right[7]		= (float)atof( results[offset + 26] );
	p->m_Right[8]		= (float)atof( results[offset + 27] );
	p->m_Right[9]		= (float)atof( results[offset + 28] );
	p->m_Right[10]		= (float)atof( results[offset + 29] );
	p->m_Right[11]		= (float)atof( results[offset + 30] );
	p->m_Right[12]		= (float)atof( results[offset + 31] );	
	p->m_Right[13]		= (float)atof( results[offset + 32] );
	p->m_Right[14]		= (float)atof( results[offset + 33] );
	p->m_Right[15]		= (float)atof( results[offset + 34] );
	p->m_Right[16]		= (float)atof( results[offset + 35] );
	p->m_Right[17]		= (float)atof( results[offset + 36] );

	if ( numCols == 39 )
		p->m_Name = utf8to16( results[2] );

	return 0;
}

///////////////////////////////////////////////////

static int sqlite_GetSongInfoFromID( void *args, int numCols, char **results, char ** columnNames )
{
	SongInfo* p = (SongInfo*)args;

	p->SetTrackNum		( results[0] );
	p->SetArtist		( utf8to16( results[1] ) );
	p->SetAlbum			( utf8to16( results[2] ) );
	p->SetGenre			( utf8to16( results[3] ) );
	p->SetTitle			( utf8to16( results[4] ) );
	p->SetDuration		( results[5] );
	p->SetFormat		( results[6] );
	p->SetVBR			( results[7] );
	p->SetYear			( results[8] );
	p->SetRating		( results[9] );
	p->SetBitrate		( results[10] );
	p->SetLastPlayed	( results[11] );
	p->SetNotes			( utf8to16( results[12] ) );
	p->SetTimesPlayed	( results[13] );
	p->SetTimeAdded		( results[14] );
	p->SetFilesize		( results[15] );
	p->SetFilename		( utf8to16( results[16] ) );
	p->SetEqualizer		( results[17] );
	p->SetDirtyFlag		( results[18] );

	return 0;
}

///////////////////////////////////////////////////

static int sqlite_AddSongToStringArray( void *args, int numCols, char **results, char ** columnNames )
{
	StringArray* p = (StringArray*)args;
	p->push_back( utf8to16( results[0] ) ); 

	return 0;
}

///////////////////////////////////////////////////

static int sqlite_GetIntFromRow( void *args, int numCols, char **results, char ** columnNames )
{
	int* n = (int*)args;
	*n = atoi( results[0] ); 

	return 0;
}

///////////////////////////////////////////////////

static int sqlite_GetStringFromRow( void *args, int numCols, char **results, char ** columnNames )
{
	String* str = (String*)args;
	*str = utf8to16( results[0] ); 

	return 0;
}

///////////////////////////////////////////////////

static int sqlite_AppendFileStringFromRow( void *args, int numCols, char **results, char ** columnNames )
{
	String* str = (String*)args;
	*str += utf8to16( results[0] ); 
	*str += _T( "\0" );

	return 0;
}

///////////////////////////////////////////////////

static int sqlite_AddRowToStringArray( void *args, int numCols, char **results, char ** columnNames )
{
	StringArray* p = (StringArray*)args;
	p->push_back( utf8to16( results[0] ) ); 

	return 0;
}

///////////////////////////////////////////////////

static int sqlite_AddRowToIntArray( void *args, int numCols, char **results, char ** columnNames )
{
	IntArray* p = (IntArray*)args;
	p->push_back( atoi( results[0] ) ); 

	return 0;
}
///////////////////////////////////////////////////

static int sqlite_AddStdPlaylistInfoArray( void *args, int numCols, char **results, char ** columnNames )
{
	PlaylistInfoArray* p = (PlaylistInfoArray*)args;

	PlaylistInfo info ( utf8to16( results[0] ), MUSIK_PLAYLIST_TYPE_STANDARD, atoi( results[1] ) );
	info.m_Order = atoi( results[2] );
	p->push_back( info ); 

	return 0;
}

///////////////////////////////////////////////////

static int sqlite_AddDynPlaylistInfoArray( void *args, int numCols, char **results, char ** columnNames )
{
	PlaylistInfoArray* p = (PlaylistInfoArray*)args;

	PlaylistInfo info ( utf8to16( results[0] ), MUSIK_PLAYLIST_TYPE_DYNAMIC, atoi( results[1] ) );
	info.m_Order = atoi( results[2] );
	p->push_back( info ); 

	return 0;
}

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////

