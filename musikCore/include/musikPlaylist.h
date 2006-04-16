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

#ifndef C_MUSIK_PLAYLIST_H
#define C_MUSIK_PLAYLIST_H

///////////////////////////////////////////////////

#include "musikConfig.h"

#include <sqlite3.h>
#include <vector>
#include <iostream>

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

class Library;
class Player;

///////////////////////////////////////////////////

/**
* Once the application initializes it's musikCore::Player derived 
* object, he needs to call musikCore::SetPlayer() to let musikCore
* know where it is located. musikCore will use the reference
* to this object to do various things, specifically querying
* plugins for tag information. 
*
* \param library address of the active musikCore::Library object.
*/
MUSIKEXPORT void SetPlayer( Player* player );

/**
 * Retrieve a pointer to the player used by the application, as set
 * with musikCore::SetPlayer().
 *
 * \return a pointer to the application's Player.
 */
MUSIKEXPORT Player* GetPlayer();
extern musikCore::Player* g_SongPlayer;

///////////////////////////////////////////////////

/**
A small class that only contains the (unique) ID of the song as
it exists in the loaded database, as well as a few utility functions 
to access it's respective information. 

If you create one of these objects yourself, assure it points to the proper 
library. See the Song::SetLibrary() routine. This typically isn't necessary because
song's are queried from a Library object. If this is the case, the internal
Library pointer is initialized correctly.

@author Casey Langen
*/
struct MUSIKEXPORT Song
{
	/**
	 * Default constructor.
	 */
	Song( Library* owner );
	Song();
	~Song();

	/**
	 * \return our ID according to the library.
	 */
	int GetID() const
		{ return m_ID; }
	/*     *
	 * \return the filesize of the song. this can be useful when 
	 * calculating playlist size quickly.
	 */
	int GetFilesize() const
		{ return m_Filesize; }
	/**
	 * \return the duration of the the song. this can be useful when
	 * calculating playlist runtime.
	 */
	int GetDuration() const
		{ return m_Duration; }
	/**
	 * \param field which field? MUSIK_LIBRARY_TYPE_xxx
	 * \return the string value of the desired field.
	 */
	String GetField( int field ) const;	 
	
	/**
	 * Sets the current song id an optinally requeries attributes automatically.
	 *
	 * \param id new ID.
	 * \param requery requery the database to get new file attributes (duration,
	 * filesize)? Defaults false.
	 */
	void SetID( int id, bool requery = false );

	/** 
	 * \return a pointer to the library this song is associated with
	 */
	Library* GetLibrary()
	{ return m_Library; }

	/** 
	 * Set a pointer to the library this song is associated with. Library
	 * will typically do this for you automatically.
	 *
	 * \param library 
	 */
	void SetLibrary( Library* library )
	{ m_Library = library;	}

	// the following are reserved for musikCore::Library. Please do not use.
	void SetDuration( int duration )
		{ m_Duration = duration; }

	void SetFilesize( int filesize )
		{ m_Filesize = filesize; }

	int m_Duration;
	int m_Filesize;
	int m_ID;
	Library* m_Library;
};


typedef MUSIKEXPORT std::vector<Song> SongArray;

///////////////////////////////////////////////////

/**
A Playlist is a wrapper around a SongArray object, which
is basically a typedef of a std::vector<Song>
  
@author Casey Langen
*/
class MUSIKEXPORT Playlist : public SongArray
{
	friend class Library;

public:

	/**
	 * Default constructor.
	 */
	Playlist();
	~Playlist();

	// gets
	/**
	 * \param index access song at position(index)
	 * \param field MUSIK_LIBRARY_TYPE_xxx
	 * \return the string value of field requested
	 */
	String GetField( int index, int field ) const;
	/**
	 * \param index access song at position(index)
	 * \return the song id at position(index)
	 */
	int GetSongID( int index ) const;
	/**
	 * \return the amount of songs in the playlist
	 */
	size_t GetCount() const;
	/**
	 * \return the playlist's id according to the Library
	 */
	int GetPlaylistID() const { return m_ID; }
	/**
	 * \param index access song at position(index)
	 * \return the song at the requested index.
	 */
	Song GetSong( int index ) const;
	/**
	 * \return the type of playlist.
	 */
	int GetType() const { return m_Type; }

	/**
	 * \param index song at position(index)
	 * \param field MUSIK_LIBRARY_TYPE_xxx
	 */
	void SetField( int index, int field );
	/**
	 * \param type the type of playlist this is. it is up to the developer
	 * to manage the type in most cases. Player will set the type to
	 * MUSIK_PLAYLIST_TYPE_NOWPLAYING if it is assigned the playlist via
	 * Player::SetPlaylist. Usage is optional.
	 */
	void SetType( int type ){ m_Type = type; }
	/**
	 * Reserved, do not use unless you know what you are doing.
	 */
	void SetID( int id ){ m_ID = id; }

	/**
	 * Add a new song to the bottom of the playlist.
	 *
	 * \param song Song to add.
	 */
	void Add( const Song& song );
	/**
	 * Remove the song at position(index)
	 *
	 * \param index position of item to be deleted.
	 */
	void DeleteAt( size_t index );
	/**
	 * Insert a single song at position(index)
	 *
	 * \param song Song to be inserted.
	 * \param index Position to insert song.
	 */
	void InsertAt( Song& song, int index );
	/**
	 * Insert an array of songs at position(index)
	 *
	 * \param songs array of songs to insert
	 * \param index position in playlist to insert at.
	 */
	void InsertBefore( SongArray& songs, int index );
	void InsertAfter( SongArray& songs, int index );
	/**
	 * Clear the playlist.
	 */
	void Clear();

	/**
	 * \return The total playlist runtime.
	 */
	size_t GetTotalTime() const { return m_TotalTime; }
	/**
	 * \return The sum of the filesize of all the songs in bytes.
	 */
	double GetTotalSize() const { return m_TotalSize; }

	// the following are reserved for Library usage ONLY.
	int m_ID;
	void SetTotalTime( size_t tot ) { m_TotalTime = tot; }
	void SetTotalSize( double tot ) { m_TotalSize = tot; }

protected:

	// m_TotalSize is a double becuase type int
	// and longint are too small for us. treat
	// it as if it were an integer.
	size_t m_TotalTime;
	double m_TotalSize;

	// reserved for Player

	// playlist type
	int m_Type;
	
};

typedef MUSIKEXPORT std::vector<Playlist> PlaylistArray;

///////////////////////////////////////////////////

/**
A data structure, that interfaces can use to manage playlist information
The information contained in a PlaylistInfo is typically set automatically
by Library.

@author Casey Langen
*/
struct MUSIKEXPORT PlaylistInfo
{
	/**
	 * Default constructor.
	 */
	PlaylistInfo();
	/**
	 * \param name The string name of the playlist.
	 * \param type The type of playlist.
	 * \param id The playlist's ID according to the library.
	 */
	PlaylistInfo( String name, int type, int id );
	~PlaylistInfo();
	
	/**
	 * \return The string name of the playlist.
	 */
	String GetName() const 
		{ return m_Name; }
	/**
	 * \return The type of playlist.
	 */
	int GetType() const 
		{ return m_Type; }
	/**
	 * \return The playlist's ID according to the library.
	 */
	int GetID() const
		{ return m_ID; }

	/**
	 * \ return the order the playlist should show up
	 */
	int GetOrder() const
		{ return m_Order; }

	/**
	 * Set the stored information. This will NOT be written back
	 * to the database automatically.
	 *
	 * \param name The string name of the playlist.
	 * \param type Type type of playlist.
	 * \param id The playlist ID.
	 */
	void Set( String name, int type, int id );

	String m_Name;
	int m_Type;
	int m_ID;
	int m_Order;
};

/**
PlaylistInfoArray is a typedef std::vector<PlaylistInfo>.

@author Casey Langen
*/
typedef MUSIKEXPORT std::vector<PlaylistInfo> PlaylistInfoArray;

///////////////////////////////////////////////////

/**
A larger class than Song that actually stores all of the database  
information in memory (via String objects), rather than querying the 
database to retrieve it. You can directly access a Song's artist,
album, genre, etc.

@author Casey Langen
*/
class MUSIKEXPORT SongInfo
{
	friend class Library;
	friend class Player;
public:

	/**
	 * Default constructor.
	 */
	SongInfo( Library* owner );
	SongInfo();
	~SongInfo();

    /**
     * Loads the tag information from the specified filename.
     *
     * \param fn filename to get in formation from.
     */
	bool LoadInfo( const String& fn );

    /**
     * Writes the tag information from the specified song info.
     *
     * \param info song to write to file.
     */
    bool WriteInfo();

	/**
	 * \return Song ID of the current information.
	 */
	int	GetID() const { return m_ID; }
	/**
	 * \param field Field to be queryed. MUSIK_LIBRARY_TYPE_xxx
	 * \return The requested field.
	 */
	String GetField( int field ) const;
	/**
	 * Updates the specified field with a new string value. This information
	 * is NOT automatically written back to the database. It is up to the developer
	 * to call Library::SetSongInfo()
	 *
	 * \param field MUSIK_LIBRARY_TYPE_xxx
	 * \param str 
	 */
	void SetField( int field, const String& str );
	/**
	 * Sets a new song ID and optionally resynchronises with the database.
	 *
	 * \param id New ID of the song info.
	 * \param requery If true the song values will be automatically requeryed
	 * and updated from Song::m_musikLibrary.
	 */
	void SetID( int id, bool requery = false );
	void Requery();

	/**
	 */
	String	GetFilename() const
		{ return m_Filename; }
	/**
	 */
	String	GetTitle() const 
		{ return m_Title; }
	/**
	 */
	String	GetTrackNum() const 
		{ return m_TrackNum; }
	/**
	 */
	String	GetArtist() const 
		{ return m_Artist; }
	/**
	 */
	String	GetAlbum() const 
		{ return m_Album; }
	/**
	 */
	String	GetGenre() const 
		{ return m_Genre; }
	/**
	 */
	String	GetDuration() const 
		{ return m_Duration; }
	/**
	 */
	String	GetFormat() const 
		{ return m_Format; }
	/**
	 */
	String	GetVBR() const 
		{ return m_VBR; }
	/**
	 */
	String	GetYear() const 
		{ return m_Year; }
	/**
	 */
	String	GetRating() const 
		{ return m_Rating; }
	/**
	 */
	String	GetBitrate() const 
		{ return m_Bitrate; }
	/**
	 */
	String	GetLastPlayed() const 
		{ return m_LastPlayed; }
	/**
	 */
	String	GetNotes() const 
		{ return m_Notes; }
	/**
	 */
	String	GetTimesPlayed() const 
		{ return m_TimesPlayed; }
	/**
	 */
	String	GetTimeAdded() const 
		{ return m_TimeAdded; }
	/**
	 */
	String	GetFilesize() const 
		{ return m_Filesize; }
	/**
	 */
	String	GetDirtyFlag() const 
		{ return m_Dirty; }
	/**
	 */
	String	GetEqualizer() const 
		{ return m_Equalizer; }

	/** 
	 */
	Library* GetLibrary() const
		{ return m_Library; }
	
	/**
	 */
	void SetFilename( const String& s )
		{ m_Filename = s; }
	/**
	 */
	void SetTitle( const String& s )
		{ m_Title = s; }
	/**
	 */
	void SetTrackNum( const String& s )
		{ m_TrackNum = s; }
	/**
	 */
	void SetArtist( const String& s )
		{ m_Artist = s; }
	/**
	 */
	void SetAlbum( const String& s )
		{ m_Album = s; }
	/**
	 */
	void SetGenre( const String& s )
		{ m_Genre = s; }
	/**
	 */
	void SetDuration( const String& s )
		{ m_Duration = s; }
	/**
	 */
	void SetFormat( const String& s )
		{ m_Format = s; }
	/**
	 */
	void SetVBR( const String& s )
		{ m_VBR = s; }
	/**
	 */
	void SetYear( const String& s )
		{ m_Year = s; }
	/**
	 */
	void SetRating( const String& s )
		{ m_Rating = s; }
	/**
	 */
	void SetBitrate( const String& s )
		{ m_Bitrate = s; }
	/**
	 */
	void SetLastPlayed( const String& s )
		{ m_LastPlayed = s; }
	/**
	 */
	void SetNotes( const String& s )
		{ m_Notes = s; }
	/**
	 */
	void SetTimesPlayed( const String& s )
		{ m_TimesPlayed = s; }
	/**
	 */
	void SetTimeAdded( const String& s )
		{ m_TimeAdded = s; }
	/**
	 */
	void SetFilesize( const String& s )
		{ m_Filesize = s; }
	/**
	 */
	void SetDirtyFlag( const String& s )
		{ m_Dirty = s; }
	/**
	 */
	void SetEqualizer( const String& s )
		{ m_Equalizer = s; }

	/** 
	 */
	void SetLibrary( Library* library )
		{ m_Library = library;	}

protected:

	// reserved for Player and Library
	void SetPlayTime( int playtime ){ m_PlayTime = playtime; }
	void SetUserChoose(){ m_UserChoose = 1; }
	void SetAutoChoose(){ m_UserChoose = 0; }

	int m_UserChoose;
	int m_PlayTime;

	// other vars
	int m_ID;

	Library* m_Library;

	String	m_Rating, m_Bitrate, m_Duration, m_Dirty, m_Filesize,
					m_TimesPlayed, m_TrackNum, m_Format, m_VBR, m_Filename,
					m_Title, m_Artist, m_Album, m_Year, m_Genre, m_LastPlayed,
					m_TimeAdded, m_Notes, m_Equalizer;
};

/**
SongInfoArray is a typedef std::vector<SongInfo>.

@author Casey Langen
*/
typedef MUSIKEXPORT std::vector<SongInfo> SongInfoArray;

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
