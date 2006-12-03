///////////////////////////////////////////////////
//
// License Agreement:
//
// The following are Copyright © 2002-2006, Casey Langen
//
// Sources and Binaries of: musikCore musikCube musikBox
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright 
//      notice, this list of conditions and the following disclaimer in the 
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the author nor the names of other contributors may 
//      be used to endorse or promote products derived from this software 
//      without specific prior written permission. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE. 
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "musikLibraryCallbacks.h"
#include "musikLibrary.h"
#include "musikPlaylist.h"

///////////////////////////////////////////////////

using namespace musikCore;

///////////////////////////////////////////////////

int LibraryCallbacks::AddSongToPlaylist(void *args, int numCols, char **results, char ** columnNames)
{
    if (numCols == 3)
    {
        Song song;
        song.SetID(atoi(results[0])); 
        song.SetDuration(atoi(results[1]) / 1000);
        song.SetFilesize(atoi(results[2]));
        //
        ((Playlist*)args)->Add(song);
    }
    else
    {
        return 1;
    }

    return 0;
}

///////////////////////////////////////////////////

int LibraryCallbacks::GetSongAttr(void *args, int numCols, char **results, char ** columnNames)
{
    Song* p = (Song*)args;

    p->SetID(atoi(results[0])); 
    p->SetDuration(atoi(results[1]) / 1000);
    p->SetFilesize(atoi(results[2]));

    return 0;
}

///////////////////////////////////////////////////

int LibraryCallbacks::GetFieldFromID(void *args, int numCols, char **results, char ** columnNames)
{
    String* p = (String*)args;
    *p = String(utf8to16(results[0])); 

    return 0;
}

///////////////////////////////////////////////////

int LibraryCallbacks::GetCrossfader(void *args, int numCols, char **results, char ** columnNames)
{
    Crossfader* p = (Crossfader*)args;

    p->Set(
        (float)atof(results[0]), 
        (float)atof(results[1]), 
        (float)atof(results[2]), 
        (float)atof(results[3]), 
        (float)atof(results[4]));

    if (numCols == 6)
        p->SetName(utf8to16(results[5]));
    else
        p->SetName(_T("Default"));

    return 0;
}

///////////////////////////////////////////////////

int LibraryCallbacks::GetEqualizer(void *args, int numCols, char **results, char ** columnNames)
{
    EQSettings* p = (EQSettings*)args;

    int offset;
    if (numCols == 37)
        offset = 0;
    else if (numCols == 39)
        offset = 2;
    else 
        return -1;

    p->m_Left[0]        = (float)atof(results[offset + 1]);
    p->m_Left[1]        = (float)atof(results[offset + 2]);
    p->m_Left[2]        = (float)atof(results[offset + 3]);
    p->m_Left[3]        = (float)atof(results[offset + 4]);
    p->m_Left[4]        = (float)atof(results[offset + 5]);
    p->m_Left[5]        = (float)atof(results[offset + 6]);
    p->m_Left[6]        = (float)atof(results[offset + 7]);
    p->m_Left[7]        = (float)atof(results[offset + 8]);
    p->m_Left[8]        = (float)atof(results[offset + 9]);
    p->m_Left[9]        = (float)atof(results[offset + 10]);
    p->m_Left[10]        = (float)atof(results[offset + 11]);
    p->m_Left[11]        = (float)atof(results[offset + 12]);
    p->m_Left[12]        = (float)atof(results[offset + 13]);
    p->m_Left[13]        = (float)atof(results[offset + 14]);
    p->m_Left[14]        = (float)atof(results[offset + 15]);
    p->m_Left[15]        = (float)atof(results[offset + 16]);
    p->m_Left[16]        = (float)atof(results[offset + 17]);
    p->m_Left[17]        = (float)atof(results[offset + 18]);
    p->m_Right[0]        = (float)atof(results[offset + 19]);
    p->m_Right[1]        = (float)atof(results[offset + 20]);
    p->m_Right[2]        = (float)atof(results[offset + 21]);
    p->m_Right[3]        = (float)atof(results[offset + 22]);
    p->m_Right[4]        = (float)atof(results[offset + 23]);
    p->m_Right[5]        = (float)atof(results[offset + 24]);
    p->m_Right[6]        = (float)atof(results[offset + 25]);
    p->m_Right[7]        = (float)atof(results[offset + 26]);
    p->m_Right[8]        = (float)atof(results[offset + 27]);
    p->m_Right[9]        = (float)atof(results[offset + 28]);
    p->m_Right[10]        = (float)atof(results[offset + 29]);
    p->m_Right[11]        = (float)atof(results[offset + 30]);
    p->m_Right[12]        = (float)atof(results[offset + 31]);
    p->m_Right[13]        = (float)atof(results[offset + 32]);
    p->m_Right[14]        = (float)atof(results[offset + 33]);
    p->m_Right[15]        = (float)atof(results[offset + 34]);
    p->m_Right[16]        = (float)atof(results[offset + 35]);
    p->m_Right[17]        = (float)atof(results[offset + 36]);

    if (numCols == 39)
        p->m_Name = utf8to16(results[2]);

    return 0;
}

///////////////////////////////////////////////////

int LibraryCallbacks::GetSongInfoFromID(void *args, int numCols, char **results, char ** columnNames)
{
    SongInfo* p = (SongInfo*)args;

    p->SetTrackNum      (results[0]);
    p->SetArtist        (utf8to16(results[1]));
    p->SetAlbum         (utf8to16(results[2]));
    p->SetGenre         (utf8to16(results[3]));
    p->SetTitle         (utf8to16(results[4]));
    p->SetDuration      (results[5]);
    p->SetFormat        (results[6]);
    p->SetVBR           (results[7]);
    p->SetYear          (results[8]);
    p->SetRating        (results[9]);
    p->SetBitrate       (results[10]);
    p->SetLastPlayed    (results[11]);
    p->SetNotes         (utf8to16(results[12]));
    p->SetTimesPlayed   (results[13]);
    p->SetTimeAdded     (results[14]);
    p->SetFilesize      (results[15]);
    p->SetFilename      (utf8to16(results[16]));
    p->SetEqualizer     (results[17]);
    p->SetDirtyFlag     (results[18]);

    return 0;
}

///////////////////////////////////////////////////

int LibraryCallbacks::AddSongToStringArray(void *args, int numCols, char **results, char ** columnNames)
{
    StringArray* p = (StringArray*)args;
    p->push_back(utf8to16(results[0])); 

    return 0;
}

///////////////////////////////////////////////////

int LibraryCallbacks::GetIntFromRow(void *args, int numCols, char **results, char ** columnNames)
{
    int* n = (int*)args;
    *n = atoi(results[0]); 

    return 0;
}

///////////////////////////////////////////////////

int LibraryCallbacks::GetStringFromRow(void *args, int numCols, char **results, char ** columnNames)
{
    String* str = (String*)args;
    *str = utf8to16(results[0]); 

    return 0;
}

///////////////////////////////////////////////////

int LibraryCallbacks::AppendFileStringFromRow(void *args, int numCols, char **results, char ** columnNames)
{
    String* str = (String*)args;
    *str += utf8to16(results[0]); 
    *str += _T("\0");

    return 0;
}

///////////////////////////////////////////////////

int LibraryCallbacks::AddRowToStringArray(void *args, int numCols, char **results, char ** columnNames)
{
    StringArray* p = (StringArray*)args;
    p->push_back(utf8to16(results[0])); 

    return 0;
}

///////////////////////////////////////////////////

int LibraryCallbacks::AddRowToIntArray(void *args, int numCols, char **results, char ** columnNames)
{
    IntArray* p = (IntArray*)args;
    p->push_back(atoi(results[0])); 

    return 0;
}
///////////////////////////////////////////////////

int LibraryCallbacks::AddStdPlaylistInfoArray(void *args, int numCols, char **results, char ** columnNames)
{
    PlaylistInfoArray* p = (PlaylistInfoArray*)args;

    PlaylistInfo info (utf8to16(results[0]), MUSIK_PLAYLIST_TYPE_STANDARD, atoi(results[1]));
    info.m_Order = atoi(results[2]);
    p->push_back(info); 

    return 0;
}

///////////////////////////////////////////////////

int LibraryCallbacks::AddDynPlaylistInfoArray(void *args, int numCols, char **results, char ** columnNames)
{
    PlaylistInfoArray* p = (PlaylistInfoArray*)args;

    PlaylistInfo info (utf8to16(results[0]), MUSIK_PLAYLIST_TYPE_DYNAMIC, atoi(results[1]));
    info.m_Order = atoi(results[2]);
    p->push_back(info); 

    return 0;
}

///////////////////////////////////////////////////
