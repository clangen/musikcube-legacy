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

#pragma once

///////////////////////////////////////////////////

#include "musikCube.h"

///////////////////////////////////////////////////

class CmusikDynamicText : public CStatic
{
public:

    // construct and destruct
    CmusikDynamicText();
    virtual ~CmusikDynamicText();

    // gets
    int GetFontSize(){ return abs(m_FontSize); }
    int GetHeight() { return GetFontSize(); }
    int GetWidth() { return m_Width; }
    int GetType(){ return m_Type; }
    CSize GetSize() { return CSize(GetWidth(), GetHeight()); }

    // sets
    void SetDynText(const CString& str, bool updatesize = true, bool movewindow = true);
    void SetDynFont(int size = -1, int bold = 0, int italic = 0);
    void SetType(int n){ m_Type = n; }

    // misc
    void UpdateDynSize(bool movewindow = true);
    CSize GetTextSize(const CString& str);
    CSize GetDynSize();

protected:

    // fonts
    CFont* pFont;

    // vars
    int m_Width;
    int m_Type;
    int m_FontSize;

    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

    // macros
    DECLARE_DYNAMIC(CmusikDynamicText)
    DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////