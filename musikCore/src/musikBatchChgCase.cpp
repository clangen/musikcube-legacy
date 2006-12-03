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
#include "../include/musikFilename.h"
#include "../include/musikBatchChgCase.h"
#include "../include/musikPlaylist.h"

///////////////////////////////////////////////////

using namespace musikCore;

///////////////////////////////////////////////////

// BatchChgCase

///////////////////////////////////////////////////

BatchChgCase::BatchChgCase()
{
    m_Functor = NULL;
    m_Library = NULL;
    m_UpdatedTags = NULL;
    m_CallFunctorOnAbort = false;
    m_DeleteUpdatedTags = true;
    m_Type = false;
}

///////////////////////////////////////////////////

BatchChgCase::BatchChgCase(Library* pLibrary, Functor* pFunctor, SongInfoArray* pUpdatedTags, int nType)
{
    m_Functor = pFunctor;
    m_Library = pLibrary;
    m_UpdatedTags = pUpdatedTags;
    m_CallFunctorOnAbort = false;
    m_DeleteUpdatedTags = true;
    m_Type = nType;
}

///////////////////////////////////////////////////

BatchChgCase::~BatchChgCase()
{
    if (m_DeleteUpdatedTags)
        delete m_UpdatedTags;
}

///////////////////////////////////////////////////

// BatchChgCaseTask

///////////////////////////////////////////////////

BatchChgCaseTask::BatchChgCaseTask()
    : Task()
{
    m_Params = NULL;
    m_Type = MUSIK_TASK_TYPE_BATCHCHGCASE;
}

///////////////////////////////////////////////////

BatchChgCaseTask::~BatchChgCaseTask()
{
    if (m_Params)
    {
        delete m_Params;
    }
    
    cout << "musikCore::BatchChgCaseTask: deleted\n";    
}

///////////////////////////////////////////////////

void BatchChgCaseTask::run()
{
    cout << "musikCore::BatchChgCaseTask: starged\n";
    m_Stop = false;
    m_Finished = false;
    m_Active = true;

    SongInfoArray* info = m_Params->m_UpdatedTags;

    if (m_Params)
    {
        if (m_Params->m_Functor)
            m_Params->m_Functor->OnTaskStart();
        
        size_t last_prog = 0;

        m_Params->m_Library->BeginTransaction();
        for(size_t i = 0; i < m_Params->m_UpdatedTags->size(); i++)
        {
            // check abort flag
            if (m_Stop)
            {
                cout << "musikCore::BatchChgCaseTask: worker function aborted\n";
                break;
            }

            StringArray lower_words;
            m_Params->m_Library->GetAllChgCase(lower_words);

            switch (m_Params->m_Type)
            {
            case MUSIK_CHGCASE_TOLOWER:
                info->at(i).SetArtist(info->at(i).GetArtist().ToLower());
                info->at(i).SetAlbum(info->at(i).GetAlbum().ToLower());
                info->at(i).SetTitle(info->at(i).GetTitle().ToLower());
                info->at(i).SetGenre(info->at(i).GetGenre().ToLower());
                break;

            case MUSIK_CHGCASE_TOUPPER:
                info->at(i).SetArtist(info->at(i).GetArtist().ToUpper());
                info->at(i).SetAlbum(info->at(i).GetAlbum().ToUpper());
                info->at(i).SetTitle(info->at(i).GetTitle().ToUpper());
                info->at(i).SetGenre(info->at(i).GetGenre().ToUpper());
                break;

            case MUSIK_CHGCASE_CAPITALIZE:

                info->at(i).SetArtist(Capitalize(lower_words, info->at(i).GetArtist()));
                info->at(i).SetAlbum(Capitalize(lower_words, info->at(i).GetAlbum()));
                info->at(i).SetTitle(Capitalize(lower_words, info->at(i).GetTitle()));
                info->at(i).SetGenre(Capitalize(lower_words, info->at(i).GetGenre()));
                break;

            case MUSIK_CHGCASE_CLEAN:
                
                info->at(i).SetArtist(Clean(lower_words, info->at(i).GetArtist()));
                info->at(i).SetAlbum(Clean(lower_words, info->at(i).GetAlbum()));
                info->at(i).SetTitle(Clean(lower_words, info->at(i).GetTitle()));
                info->at(i).SetGenre(Clean(lower_words, info->at(i).GetGenre()));
                break;

            case MUSIK_CHGCASE_REMOVEWHITESPACE:
                info->at(i).SetArtist(RemoveWhite(info->at(i).GetArtist()));
                info->at(i).SetAlbum(RemoveWhite(info->at(i).GetAlbum()));
                info->at(i).SetTitle(RemoveWhite(info->at(i).GetTitle()));
                info->at(i).SetGenre(RemoveWhite(info->at(i).GetGenre()));
                break;
            }

            info->at(i).SetDirtyFlag("1");
            m_Params->m_Library->SetSongInfo(info->at(i));

            // post progress to the functor
            m_Progress = (100 * i) / m_Params->m_UpdatedTags->size();
            if (m_Progress != last_prog)
            {
                if (m_Params->m_Functor)
                    m_Params->m_Functor->OnTaskProgress(m_Progress, this);
                last_prog = m_Progress;
            }

        }
        m_Params->m_Library->EndTransaction();
    }

    m_Finished = true;
    m_Progress = 100;
    
    if (m_Params->m_Functor)
    {
        if (!m_Stop || (m_Stop && m_Params->m_CallFunctorOnAbort))
            m_Params->m_Functor->OnTaskEnd(this);
    }
}

///////////////////////////////////////////////////

void BatchChgCaseTask::CapWord(String& str)
{
    if(str.GetLength() > 0)
    {
        int firstLetter = 0;

        bool validFirstChar = false;
        while (!validFirstChar)
        {
            switch (str[firstLetter])
            {
            case (wchar_t)'[':
            case (wchar_t)'(':
            case (wchar_t)'{':
                firstLetter++;
                break;

            default:
                validFirstChar = true;
            }

            if (firstLetter >= str.GetLength())
            {
                return;
            }
        }

        str[firstLetter] = toupper(str[firstLetter]);
    }
}

///////////////////////////////////////////////////

String BatchChgCaseTask::Capitalize(const StringArray& lower_words, const String& str)
{
    StringArray target;
    Filename::DelimitStr(str, " ", target, false, true, true);
    size_t i = 0;
    size_t j = 0;

    // pick it apart, fix it...
    for (i = 0; i < target.size(); i++)
    {
        target.at(i).ToLower();

        if (i == 0 || i == target.size() - 1)
        {
            BatchChgCaseTask::CapWord(target.at(i));
        }
        else
        {
            
            bool found = false;
            for (j = 0; j < lower_words.size(); j++)
            {
                if (target.at(i) == lower_words.at(j))
                {
                    found = true;
                    break;
                }
            }
            
            if (!found)
                CapWord(target.at(i));
        }
    }

    // and put it back together
    String ret;
    for (i = 0; i < target.size(); i++)
    {
        ret += target.at(i);

        if (i != target.size() - 1)
            ret += " ";
    }

    return ret;
}

///////////////////////////////////////////////////


String BatchChgCaseTask::RemoveWhite(String str)
{
    while (str.Replace(_T("  "), _T(""))){}
    str.TrimLeft();
    str.TrimRight();
    return str;
}

///////////////////////////////////////////////////

String BatchChgCaseTask::Clean(const StringArray& lower_words, const String& input)
{
    String str(input);
    str.Replace(_T("("), _T("("));
    str.Replace(_T("[ "), _T("["));
    str.Replace(_T("{ "), _T("{"));
    str.Replace(_T(") "), _T(")"));
    str.Replace(_T("] "), _T("]"));
    str.Replace(_T("} "), _T("}"));
    str.Replace(_T("_"), _T(" "));
    str.Replace(_T("\r"), _T(""));
    str.Replace(_T("\t"), _T(""));
    
    while (str.Replace(_T("  "), _T(""))){}

    str.Trim();
    str.MakeLower();

    String ret = Capitalize(lower_words, str);
    return ret;
}

///////////////////////////////////////////////////


