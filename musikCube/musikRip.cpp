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
#include "musikRip.h"

///////////////////////////////////////////////////

using namespace musikCore;

///////////////////////////////////////////////////

Rip::Rip(RipData* params)
: Task()
{
    m_Params = params;
}

///////////////////////////////////////////////////

Rip::Rip()
: Task()
{
}

///////////////////////////////////////////////////

Rip::~Rip()
{
}

///////////////////////////////////////////////////

void Rip::GetAllTracks()
{
    m_Params->m_Tracks.clear();
    int tracks = GetTrackCount();
    for (int i = 0; i < tracks; i++)
        m_Params->m_Tracks.push_back(i);
}

///////////////////////////////////////////////////

void Rip::run()
{
    if (m_Params->m_SelectAllTracks)
        GetAllTracks();

    if (!m_Params->m_Songs || m_Params->m_Path.IsEmpty() || !m_Params->m_Tracks.size())
        return;

    m_Stop = false;
    m_Finished = false;
    m_Active = true;
    m_Error = false;

    Filename mfn;

    m_Params->m_NumComplete = 0;
    m_Params->m_CurrTrack = 0;

    if (m_Params->m_Tracks.size() > m_Params->m_Songs->size())
    {
        MessageBox(
            NULL,
            _T("Ripper out of sync. Please email the musikCube development team with this error."),
            _T("musikCube"),
            MB_ICONWARNING | MB_OK);

            m_Error = true;
            m_Active = false;
            m_Finished = true;
            return;
    }

    for (size_t i = 0; i < m_Params->m_Tracks.size(); i++)
    {
        m_Params->m_CurrTrack = m_Params->m_Tracks.at(i);

        //
        // load the track
        //
        DWORD chan = BASS_CD_StreamCreate(
            m_Params->m_DriveID,
            m_Params->m_Tracks.at(i),
            BASS_STREAM_DECODE);

        //
        // determine filename
        //
        String fn;
        fn.Format(_T("%s%.2d."), m_Params->m_Path.c_str(), m_Params->m_Tracks.at(i) + 1);
        String type;

        //
        // set command line
        //
        String command;
        switch(m_Params->m_Format)
        {
        case MUSIK_RIP_OGG:
            {
                type = MUSIK_LIBRARY_FORMAT_OGG;
                fn += _T("ogg");

                command.Format(
                        _T("oggenc -q %d -o \"%s\" -"),
                        m_Params->m_Quality,
                        fn.c_str());
            }
            break;

        case MUSIK_RIP_FLAC:
            {
                type = MUSIK_LIBRARY_FORMAT_FLAC;
                fn += _T("flac");

                command.Format(
                    _T("flac -%d -o \"%s\" -"),
                    m_Params->m_Quality,
                    fn.c_str());
            }
            break;

        case MUSIK_RIP_MP3:
            {
                type = MUSIK_LIBRARY_FORMAT_MP3;
                fn += _T("mp3");

                command.Format(
                    _T("lame -b %d - \"%s\""),
                    m_Params->m_Quality,
                    fn.c_str());
            }
            break;
        }

        //
        // fire up the command line encoder
        //
        if (!BASS_Encode_Start(chan, utf16to8(command, false).c_str(), 0, NULL, 0))
        {
            printf("Can't start the encoder");
            m_Error = true;

            m_Active = false;
            m_Finished = true;

            BASS_StreamFree(chan);
            BASS_Encode_Stop(chan);

            if (m_Params->m_Functor)
                m_Params->m_Functor->OnTaskEnd(this);

            return;
        }

        DWORD pos = BASS_ChannelGetLength(chan, BASS_POS_BYTE);
        DWORD orig = pos;

        //
        // send the stream through
        //
        int lastprog = 0;
        while (BASS_ChannelIsActive(chan))
        {
            char temp[20000];
            BASS_ChannelGetData(chan, temp, 20000);

            if (BASS_Encode_IsActive(chan) == BASS_ACTIVE_STOPPED)
            {
                printf("Error: The encoder died!\n");
                m_Active = false;
                m_Finished = true;
                m_Params->m_Error = true;
                break;
            }

            // check abort flag
            if (m_Stop)
            {
                cout << "musikCore::Rip: worker function aborted\n";
                m_Active = false;
                m_Finished = true;
                m_Error = true;
                m_Params->m_Error = true;

                BASS_StreamFree(chan);
                BASS_Encode_Stop(chan);

                if (m_Params->m_Functor)
                    m_Params->m_Functor->OnTaskEnd(this);

                return;
            }

            // update status
            pos = BASS_ChannelGetPosition(chan, BASS_POS_BYTE);
            m_Params->m_Progress = (int)(pos / (orig / 100));
            if (m_Params->m_Progress > lastprog)
            {
                if (m_Params->m_Functor)
                    m_Params->m_Functor->OnTaskProgress(0);

                lastprog = m_Params->m_Progress;
            }
        }

        BASS_Encode_Stop(chan);
        BASS_StreamFree(chan);

         int tracknum = m_Params->m_Tracks.at(i);
        SongInfo& info = m_Params->m_Songs->at(tracknum);
        if (!m_Params->m_Mask.IsEmpty())
        {
            mfn.SetFilename(fn);
            if (!mfn.RenameFromSongInfo(m_Params->m_Mask, info))
                info.SetFilename(fn); // failed to rename, set to original
        }
        else
            info.SetFilename(fn);     // no rename, set filename

        // write the tag (we do it here, not through the
        // encoder to assure unicode is used if necessary)
        if (m_Params->m_WriteTags)
            info.WriteInfo();

        if (m_Params->m_Library)
           m_Params->m_Library->AddSong(info.GetFilename());

        m_Params->m_NumComplete++;
    }

    m_Active = false;
    m_Finished = true;

    if (m_Params->m_Functor)
        m_Params->m_Functor->OnTaskEnd(this);
}

///////////////////////////////////////////////////

int Rip::GetTrackCount()
{
    return (int)BASS_CD_GetTracks(m_Params->m_DriveID);
    return 0;
}

///////////////////////////////////////////////////

