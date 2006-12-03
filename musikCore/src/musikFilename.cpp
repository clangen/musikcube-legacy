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
#include <iostream>
#include <fstream>
#include <deque>
#include <stdlib.h>

#include "../include/musikFilename.h"

#ifdef WIN32
#include <io.h>
#include <Direct.h>
#include <stdio.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

///////////////////////////////////////////////////

using namespace musikCore;

///////////////////////////////////////////////////

Filename::Filename()
{
}

///////////////////////////////////////////////////

Filename::Filename(const String& fn)
{
    m_Filename = fn;
}

///////////////////////////////////////////////////

Filename::~Filename()
{
}

///////////////////////////////////////////////////

void Filename::SetFilename(const String& fn)
{
    m_Filename = fn;
}

///////////////////////////////////////////////////

String Filename::GetPathSeparator() const
{
    return String(MUSIK_PATH_SEPARATOR);
}

///////////////////////////////////////////////////

String Filename::GetJustFilename() const
{
    int nPos = m_Filename.rfind(String(MUSIK_PATH_SEPARATOR));
    return m_Filename.Right(m_Filename.GetLength() - nPos - 1);
}

///////////////////////////////////////////////////

String Filename::GetJustFilenameNoExt() const
{
    String fn = GetJustFilename();
    int nPos = fn.rfind(_T("."));

    if (nPos)
        fn = fn.Left(nPos);

    return fn;
}

///////////////////////////////////////////////////

String Filename::GetFullFilename() const
{
    return m_Filename;
}

///////////////////////////////////////////////////

String Filename::GetTrimFilename() const
{
#ifdef WIN32
    return m_Filename.Right(m_Filename.length() - 2);
#endif

    return m_Filename;
}

///////////////////////////////////////////////////

String Filename::GetExtension(bool make_lower) const
{
    int nPos = m_Filename.rfind(_T("."));
    String ret = m_Filename.Right(m_Filename.GetLength() - nPos - 1);

    if (nPos == -1)
        return String(_T(""));

    if (make_lower)
        ret.MakeLower();

    return ret;
}

///////////////////////////////////////////////////

String Filename::GetPath() const
{
    if (m_Filename.Right(1) == MUSIK_PATH_SEPARATOR)
        return m_Filename;

    int nPos = m_Filename.rfind(MUSIK_PATH_SEPARATOR);
    return m_Filename.Left(nPos + 1);
}

///////////////////////////////////////////////////

String Filename::GetNetstreamPath() const
{
    if (m_Filename.Right(1) == _T("/"))
        return m_Filename;

    int nPos = m_Filename.rfind(_T("/"));

    if (nPos == -1)
        return m_Filename;

    return m_Filename.Left(nPos + 1);
}

///////////////////////////////////////////////////

String Filename::GetTrimPath() const
{
    int nPos = m_Filename.rfind(String(MUSIK_PATH_SEPARATOR));
    String trimmed = m_Filename.Left(nPos + 1);

#ifdef WIN32
    trimmed = trimmed.Right(trimmed.length() - 2);
#endif

    return trimmed;
}

///////////////////////////////////////////////////

bool Filename::FileExists(const String& fn)
{
#ifdef WIN32
    return _waccess(fn.c_str(), 1 /*R_OK*/) == 0;
#else
    return access(utf16to8(fn).c_str(), R_OK) == 0;
#endif
}

///////////////////////////////////////////////////

String Filename::GetTempFilename(String fn, bool check_exists)
{
    srand ((size_t)time(NULL));

    Filename MFN(fn);
    String sPath = MFN.GetPath();
    
    String sName;
    sName.Format(_T("%ls%d"), sPath.c_str(), rand());
    
    if (check_exists)
    {
        while (Filename::FileExists(sName))
            sName.Format(_T("%ls%d.tmp"), sPath.c_str(), rand());
    }

    return sName;
}

///////////////////////////////////////////////////

void Filename::GetDelimitedPath(StringArray& target, bool reverse, bool trimfilename) const
{
    target.clear();

    String path;
    if (trimfilename)
        path = GetTrimPath();
    else
        path = GetTrimFilename();

    DelimitStr(path, MUSIK_PATH_SEPARATOR, target, reverse);
}

///////////////////////////////////////////////////

void Filename::DelimitStr(String path, String delimiter, StringArray& target, bool reverse, bool trim_l, bool trim_r, int count)
{
    target.clear();

    if (reverse)
        std::reverse(path.begin(), path.end());
#if defined (WIN32) 
    wchar_t* token = wcstok(path.GetBuffer(), delimiter.c_str());
#else
        wchar_t* punused;
    wchar_t* token = wcstok(path.GetBuffer(), delimiter.c_str(), &punused);
#endif
    int cnt = 1;
    String tokrev;
    while (token)
    {
        tokrev = token;
        
        if (reverse)
        {
            std::reverse(tokrev.begin(), tokrev.end());

            if (trim_l)
                tokrev.TrimLeft();
            if (trim_r)
                tokrev.TrimRight();

            if (tokrev != delimiter)
                target.insert(target.begin(), tokrev);
        }
        else
        {
            if (trim_l)
                tokrev.TrimLeft();
            if (trim_r)
                tokrev.TrimRight();

            if (tokrev != delimiter)
                target.push_back(tokrev);
        }

        if (count != -1 && (cnt == count))
            break;
        
#if defined (WIN32)
        token = wcstok(NULL, delimiter.c_str());
#else
        token = wcstok(NULL, delimiter.c_str(), &punused);
#endif
        cnt++;
    }
}

///////////////////////////////////////////////////

bool Filename::GetSongInfo(String mask, SongInfo& target, bool clear_info, bool partial, bool undr_to_spc)
{
    // get delimiters
    StringArray fn_delimiters;
    GetMaskDelimiters(mask, fn_delimiters);

    // get valid fields in order
    StringArray mask_order;
    String validate = mask;

    size_t i;

    for (i = 0; i < fn_delimiters.size(); i++)
        validate.Replace(fn_delimiters.at(i), MUSIK_PATH_SEPARATOR);

    DelimitStr(validate, MUSIK_PATH_SEPARATOR, mask_order, false, true, true);

    // get the real filename
    String path = GetTrimPath();
    String fn = GetJustFilenameNoExt();

    // setup delimiter(s)
    path += fn;

    for (i = 0; i < fn_delimiters.size(); i++)
        path.Replace(fn_delimiters.at(i), MUSIK_PATH_SEPARATOR);

    if (path.IsEmpty())
        return false;

    // get actual values from filename
    StringArray values;
    DelimitStr(path, MUSIK_PATH_SEPARATOR, values, true, true, true, mask_order.size());

    if (!partial && values.size() < mask_order.size())
        return false;

    if (!values.size())
        return false;

    if (clear_info)
    {
        target.SetTitle(_T(""));
        target.SetArtist(_T(""));
        target.SetAlbum(_T(""));
        target.SetTrackNum(_T(""));
        target.SetGenre(_T(""));
        target.SetYear(_T(""));
        target.SetRating(_T(""));
    }

    if (undr_to_spc)
    {
        for(size_t i = 0; i < values.size(); i++)
            values.at(i).Replace(_T("_"), _T(" "));
    }

    for (i = 0; i < mask_order.size(); i++)
    {
        const String current = mask_order.at(i);

        if (current == _T("$TITLE"))
            target.SetTitle(values.at(i));
        else if (current == _T("$ARTIST"))
            target.SetArtist(values.at(i));
        else if (current == _T("$ALBUM"))
            target.SetAlbum(values.at(i));
        else if (current == _T("$TRACK"))
            target.SetTrackNum(values.at(i));
        else if (current == _T("$GENRE"))
            target.SetGenre(values.at(i));
        else if (current == _T("$YEAR"))
            target.SetYear(values.at(i));
        else if (current == _T("$RATING"))
            target.SetRating(values.at(i));

        if (i + 1 >= values.size())
            break;
    }

    return true;
}

///////////////////////////////////////////////////

void Filename::GetMaskDelimiters(String mask, StringArray& target)
{
    mask.Replace(MUSIK_PATH_SEPARATOR, _T(""));
    mask.Replace(_T("$TITLE"), MUSIK_PATH_SEPARATOR);
    mask.Replace(_T("$ARTIST"), MUSIK_PATH_SEPARATOR);
    mask.Replace(_T("$ALBUM"), MUSIK_PATH_SEPARATOR);
    mask.Replace(_T("$TRACK"), MUSIK_PATH_SEPARATOR);
    mask.Replace(_T("$GENRE"), MUSIK_PATH_SEPARATOR);
    mask.Replace(_T("$YEAR"), MUSIK_PATH_SEPARATOR);
    mask.Replace(_T("$RATING"), MUSIK_PATH_SEPARATOR);

    DelimitStr(mask, MUSIK_PATH_SEPARATOR, target, false);
    SortBySize(target);
}

///////////////////////////////////////////////////

bool Filename::RenameFromSongInfo(String mask, SongInfo& src)
{
    if (m_Filename.IsEmpty())
        return false;

    String out_path = GetPath();
    String out_dir;

    // parase mask
    if (mask.Right(mask.length()) != MUSIK_PATH_SEPARATOR)
        mask += MUSIK_PATH_SEPARATOR;

    mask.Replace(_T("$TITLE"), src.GetTitle());
    mask.Replace(_T("$ARTIST"), src.GetArtist());
    mask.Replace(_T("$ALBUM"), src.GetAlbum());
    mask.Replace(_T("$GENRE"), src.GetGenre());
    mask.Replace(_T("$YEAR"), src.GetYear());
    mask.Replace(_T("$RATING"), src.GetRating());

    String track = src.GetTrackNum();
    if (track.length() == 1)
        track = _T("0") + track;

    mask.Replace(_T("$TRACK"), track);

    // fields that cannot be in a filename, 
    // especially in win32...
    mask.Replace(_T("/"),  _T(""));
    mask.Replace(_T(":"),  _T(""));
    mask.Replace(_T("*"),  _T(""));
    mask.Replace(_T("?"),  _T(""));
    mask.Replace(_T("<"),  _T(""));
    mask.Replace(_T(">"),  _T(""));
    mask.Replace(_T("|"),  _T(""));
    mask.Replace(_T("\""), _T(""));

    // get final output filename
    out_path += mask;
    String ext = GetExtension();
    out_path = out_path.Left(out_path.length() - 1);
    out_path += _T(".");
    out_path += ext;

#if defined (WIN32) || defined (__MINGW32__)
    // the windows API has a but where you can
    // rename a file with multiple periods in 
    // a row... but once you do this the file
    // becomes in accesable
    while (out_path.Replace(_T(".."), _T("."))){}
#endif

    // create directory
    Filename fn(out_path);
    if (Filename::RecurseMkDir(fn.GetPath().GetBuffer()))
        return false;

    // rename file
#if defined (WIN32) || defined (__MINGW32__)
    if (_wrename(m_Filename.c_str(), out_path.c_str()))
#else
    if (rename(utf16to8(m_Filename).c_str(), utf16to8(out_path).c_str()))
#endif
        return false;

    src.SetFilename(out_path);
    return true;
}

///////////////////////////////////////////////////

bool Filename::RecurseMkDir(wchar_t* pszDir)
{
    wchar_t*   pszLastSlash;
    wchar_t    cTmp;

#if defined (WIN32) || defined(__MINGW32__)
    wchar_t path_sep = '\\';
    pszLastSlash = wcsrchr(pszDir, path_sep);
#else
    wchar_t path_sep = '/';
    pszLastSlash = wcschr(pszDir, path_sep);
#endif

    if (pszLastSlash)
    {
        cTmp = *pszLastSlash;
        *pszLastSlash = '\0';

        RecurseMkDir(pszDir);

        *pszLastSlash = cTmp;
    }

#if defined (WIN32) || defined(__MINGW32__)
    if (_wmkdir(pszDir))
        return false;
#else
    const char* dir = utf16to8(String(pszDir)).c_str();
    if (mkdir(dir, 0666))
        return false;
#endif


    return true;
}

///////////////////////////////////////////////////

// a quick bubble sort for delimiters

void Filename::SortBySize(StringArray& array)
{
    if (!array.size())
        return;

    String hold;
    for (size_t i = 0; i < array.size() - 1; i++)
    {
        for (size_t j = 0; j < array.size() - 1 - i; j++)
        {
            if (array.at(j).length() < array.at(j + 1).length())
            {
                hold = array.at(j + 1);
                array.at(j + 1) = array.at(j);
                array.at(j) = hold;
            }
        }
    }
}

///////////////////////////////////////////////////

int Filename::GetFilesize(const String& fn)
{
#if defined (WIN32) || defined(__MINGW32__)
    FILE* pFile = _wfopen(fn.c_str(), _T("rb"));
#else
    FILE* pFile = fopen(utf16to8(fn).c_str(), "rb");
#endif
    if (pFile)
    {
        fseek(pFile, 0, SEEK_END);
        int fs = ftell(pFile);
        fclose(pFile);
        return fs;
    }

    return 0;
}

///////////////////////////////////////////////////
