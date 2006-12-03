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
// 
// Usage: 
//
//     Use Set() to set the mask, that should be
//   it. Follow the following guidelines:
//
//     %a [id]    <-- attribute, such as title, artist, album, etc
//     %f [size]  <-- regular font
//     %b [size]  <-- bold font
//     %i [size]  <-- italic font
//     %m [size]  <-- bold and italic font
//     %c [str]   <-- comment
//
// Example: 
//
//   m_Info->Set(_T("%b20 %a4 %c from the album %a1"));
//
//      %b20 initializes the font to 20 with a bold attribute
//      %a4 prints the current song's title
//      %c prints the comment "from the album" using default font
//      %a1 prints the album name, with the previously specified font  
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikCube.h"
#include "musikDynamicText.h"

#include <musikCore.h>
#include <vector>

///////////////////////////////////////////////////

typedef std::vector<CmusikDynamicText*> CmusikDynamicTextArray;

///////////////////////////////////////////////////

class CmusikFontBaseline
{
public:

    CmusikFontBaseline()
    {
        m_Height = -1;
        m_Ascending = -1;
        m_Descending = -1;
    }

    ~CmusikFontBaseline()
    {

    }

    int m_Height;
    int m_Ascending;
    int m_Descending;
};

///////////////////////////////////////////////////

class CmusikNowPlayingInfo : public CWnd
{
public:
    // construct and destruct
    CmusikNowPlayingInfo();
    virtual ~CmusikNowPlayingInfo();

    // getting
    CSize GetSize();
    int GetHeight();
    int GetWidth();
    void GetBaseline(int font_size, CmusikFontBaseline* baseline);

    // setting
    void Set(CString mask);

    // updating
    void UpdateInfo(bool refresh = true);
    void Layout(bool refresh = true);

    // mfc messages
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

protected:

    // initalizing the objects
    void InitObjects();

    // current font size
    int m_FontSize;
    int m_FontBold;
    int m_FontItalic;

    // arrays so we can easily keep track
    // of which font goes to which item...
    CmusikDynamicTextArray m_Captions;
    CmusikDynamicTextArray m_Comments;
    CmusikDynamicTextArray m_LayoutOrder;
    musikCore::StringArray m_Items;
    musikCore::StringArray m_Stopped;

    // macros
    DECLARE_DYNAMIC(CmusikNowPlayingInfo)
    DECLARE_MESSAGE_MAP()

};

///////////////////////////////////////////////////

