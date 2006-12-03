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

#include <wmsdk.h>
#include <string>

class WmaMetadataEditor
{
public:

    WmaMetadataEditor(const wchar_t* filename);
    ~WmaMetadataEditor();

    std::wstring GetStrAttribute(const wchar_t* attribute);
    DWORD GetDWORDAttribute(const wchar_t* attribute);
    QWORD GetQWORDAttribute(const wchar_t* attribute);
    BOOL GetBOOLAttribute(const wchar_t* attribute);

    HRESULT SetStrAttribute(const wchar_t* attribute, const wchar_t* value);
    HRESULT SetDWORDAttribute(const wchar_t* attribute, DWORD value);
    HRESULT SetQWORDAttribute(const wchar_t* attribute, QWORD value);
    HRESULT SetBOOLAttribute(const wchar_t* attribute, BOOL value);

    bool IsValid(){ return (this->headerInfo != NULL); }

    bool Write();

private:

    HRESULT GetHeaderAttribute(const wchar_t* attribute, BYTE** ppbValue);
    HRESULT LoadFromFile();
    void ReleaseCOMObjects();

    IWMMetadataEditor* editor;
    IWMHeaderInfo* headerInfo;
    std::wstring filename;
};