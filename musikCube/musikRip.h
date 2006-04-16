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

#ifndef __C_MUSIK_RIP_H__
#define __C_MUSIK_RIP_H__

///////////////////////////////////////////////////

#include <iostream>
#include <musikCore.h>
#include <bass.h>
#include <bassenc.h>
#include <bassflac.h>
#include <basscd.h>

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

enum
{
    MUSIK_RIP_OGG = 0,
    MUSIK_RIP_FLAC,
    MUSIK_RIP_MP3
};

///////////////////////////////////////////////////

struct RipData
{
    String m_Path;
    int m_Format;
    int m_Quality;
    DWORD m_DriveID;
    bool m_SelectAllTracks;
    SongInfoArray* m_Songs;
    Functor* m_Functor;
    bool m_WriteTags;
    int m_Progress;
    int m_CurrTrack;
    int m_NumComplete;
    vector<int> m_Tracks;
    bool m_Error;
    String m_Mask;
    Library* m_Library;
    bool m_ID3V1;
    bool m_ID3V2;
};

///////////////////////////////////////////////////

class Rip : public Task
{
public:

    Rip( RipData* m_Params );
    Rip();
    ~Rip();

    void SetParams( RipData* params ){ m_Params = params; }

    virtual void run();
    bool m_Error;
    RipData* m_Params;

private:

    int GetTrackCount();
    void GetAllTracks();   
};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////

