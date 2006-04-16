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

#ifndef C_MUSIK_FILENAME_H
#define C_MUSIK_FILENAME_H

///////////////////////////////////////////////////

#include "musikConfig.h"
#include "musikPlaylist.h"

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

#ifdef WIN32
	#define MUSIK_PATH_SEPARATOR _T( "\\" )
#else
	#define MUSIK_PATH_SEPARATOR _T( "/" )
#endif

///////////////////////////////////////////////////

/**
An object that is able to manipulate and 
return different paramters of an internal filename, including:
path, extension, or even information to a target SongInfo.

@author Casey Langen
*/
class MUSIKEXPORT Filename
{
public:

	/**
	 * Default constructor.
	 */
	Filename();
	/**
	 * \param fn filename to manage
	 */
	Filename( const String& fn );
	~Filename();

	/**
	 * \return the path separator. "\" for windows, "/" for linux.
	 */
	String GetPathSeparator() const;
	/**
	 * \return "title.mp3" from "c:\title.mp3"
	 */
	String GetJustFilename() const;
	/**
	 * \return "/music/mp3/" from "c:\music\mp3\"
	 */
	String GetPath() const;
	/**
	 * \returns a valid netstream path.
	 */
	String GetNetstreamPath() const;
	/**
	 * \return "\music\mp3" from "c:\music\mp3"
	 */
	String GetTrimPath() const;
	/**
	 * \return "c:\music\title.mp3" from "c:\music\title.mp3"
	 */
	String GetFullFilename() const;
	/**
	 * Windows only.
	 *
	 * \return "\music\mp3\title.mp3" from "c:\music\mp3\title.mp3"
	 */
	String GetTrimFilename() const;
	/**
	 * \param make_lower : the returned string should be lower case
	 * \return "mp3" from "c:\music\title.mp3"
	 */
	String GetExtension( bool make_lower = true ) const;
	/** 
	 * \return "title" from "c:\music\title.mp3"
	 */
	String GetJustFilenameNoExt() const;
    /**
     * \param mask mask to be used. example: $ARTIST\$ALBUM\$TRACK-$TITLE
     * \param target SongInfo to recieve the information
     * \param clear_info clear SongInfo target before processing
     * \param accept_partial accept only a partial match
     * \return the song info based on a mask from the current m_Filename
     */
    bool GetSongInfo( String mask, SongInfo& target, bool clear_info = true, bool accept_partial = true, bool undr_to_spc = false );
	/**
	 * Delimit the path in m_Filename to the array "target"
	 *
	 * \param target array to recieve the delimited information
	 * \param reverse scan the path in reverse
	 * \param trimfilename use only "c:\music" from "c:\music\title.mp3"
	 */
	void GetDelimitedPath( StringArray& target, bool reverse = false, bool trimfilename = false ) const;
	/**
	 * Rename file m_Filename to a mask based on information
	 * contained within "SongInfo& source"
	 *
	 * \param mask mask to be used. example: $ARTIST\$ALBUM\$TRACK-$TITLE
	 * \param source SongInfo containing the information to be used while renaming.
	 * \return returns true if the operation was successful, false otherwise
	 */
	bool RenameFromSongInfo( String mask, SongInfo& source );
	// sets

	/**
	 * Set m_Filename for later parsing.
	 * \param fn filename
	 */
	void SetFilename( const String& fn );

	// misc

	/**
	 * \param fn filename to check
	 * \return true if the file exists, false otherwise
	 */
	static bool FileExists( const String& fn );
	/**
	 * Static function to generate a temporary filename. If 
	 * check_exists is true, only a valid, non-existing filename
	 * will be returned.
	 * \param fn filename
	 * \param check_exists assure the filename exists before returning
	 * \return 
	 */
	static String GetTempFilename( String fn, bool check_exists );
	/**
	 * Static function to recursivly make the specified directory.
	 *
	 * \param dir directory to create
	 * \return true if directory created successfully, false otherwise
	 */
	static bool RecurseMkDir( wchar_t* dir );
	/**
	 * Static function to recieve a filesize in bytes.
	 *
	 * \param fn filename to get size from
	 * \return size of the file, in bytes.
	 */
	static int GetFilesize( const String& fn );
	/**
	 * Static function to delimit the specified string.
	 *
	 * \param str string to be delimited
	 * \param delimiter character to use as a delimiter
	 * \param array array to recieve the delimited information
	 * \param reverse reverse delimit the string
	 * \param trim_l trim delimited values left
	 * \param trim_r trim the delimited values right
	 * \param count limit the amount of delimited values
	 */
	static void DelimitStr( String str, String delimiter, StringArray& array, bool reverse, bool trim_l = false, bool trim_r = false, int count = -1 );

protected:

	/**
	 * Sort an array by character length
	 *
	 * \param array : source and target
	 */
	void SortBySize( StringArray& array );
	/**
	 * Get masked delimiters to a specified array.
	 *
	 * \param mask mask to be used. example: $ARTIST\$ALBUM\$TRACK-$TITLE
	 * \param target array to recieve the delimited values
	 */
	void GetMaskDelimiters( String mask, StringArray& target );
	/**
	 * The filename that is currently being managed.
	 */
	String m_Filename;
};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
