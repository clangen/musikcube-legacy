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

#include "stdafx.h"
#include "../include/musikDir.h"
#include "../include/musikFilename.h"
#include "../include/musikPlayer.h"

///////////////////////////////////////////////////

using namespace musikCore;

///////////////////////////////////////////////////

#if defined(WIN32)

void Dir::OpenDir( String dir, StringArray* target )
{
	// stopping conditions
	if ( dir.IsEmpty() || !target )
		return;

	// find the first file in the directory...
	WIN32_FIND_DATA lpFindFileData;
	HANDLE hDir = FindFirstFile( dir.c_str(), &lpFindFileData );

	// assure the first file exists...
	if ( hDir != INVALID_HANDLE_VALUE )
	{
		String fn, temp;
		Filename MFN;
		do
		{
			if ( lpFindFileData.cFileName[0] != '.' )
			{
				// file is a directory
				if ( lpFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				{
					fn = dir.Left( dir.GetLength() - 3 );	// remove *.* from full path
					fn += lpFindFileData.cFileName;			// append to make new path

					if ( fn.Left( 0 ) != _T( "\\" ) )
						fn += _T( "\\*.*" );					// add \\*.* to the end of the new path
					else
						fn += _T( "*.*" );						// add *.* to the end of the new path

					Dir::OpenDir( fn, target );
				}

				// file is a file...
				else
				{
					fn = dir.Left( dir.GetLength() - 3 );	// remove *.* from full path...
					fn += lpFindFileData.cFileName;			// append filename
					
					MFN.SetFilename( fn );
					for ( size_t i = 0; i < musikCore::g_PlayerFormats.size(); i++ )
					{
						if ( MFN.GetExtension() == musikCore::g_PlayerFormats.at( i ) )
						{
							target->push_back( fn );
							break;
						}

					}
				}
			}

		}
		while ( FindNextFile( hDir, &lpFindFileData ) );

		FindClose( hDir );
	}
}

#else

void Dir::OpenDir( String dir, StringArray* target )
{
	if ( dir.IsEmpty() || !target )
		return;

	DIR* pDir = opendir ( utf16to8( dir ).c_str() );
	if ( !pDir ) 
        return;

	dirent* pEntry;

	String fn;
	Filename MFN;
	while ( ( pEntry = readdir ( pDir ) ) ) 
    {
	    // got a directory, so recurse...
        if ( DT_DIR & pEntry->d_type && wcscmp( utf8to16( pEntry->d_name ).c_str(), _T( "." ) ) && wcscmp( utf8to16( pEntry->d_name ).c_str(), _T( ".." ) ) ) 
        {
			String sSubDir;
			if ( dir == _T( "/" ) )
				sSubDir = dir + String( pEntry->d_name );
			else
        		sSubDir = dir + String ( _T( "/" ) ) + String ( pEntry->d_name );

        	OpenDir ( sSubDir, target );
		}

		// not a directory, so check file attributes
		else
		{
			if ( dir == _T( "/" ) )
				fn = _T( "/" ) + String( pEntry->d_name );	
			else
				fn = dir + _T( "/" ) + String( pEntry->d_name );

			MFN.SetFilename( fn );
			if ( MFN.GetExtension() == _T( "mp3" ) || MFN.GetExtension() == _T( "ogg" ) )
				target->push_back( fn );
		}
    }

	closedir( pDir );
}

#endif

///////////////////////////////////////////////////
