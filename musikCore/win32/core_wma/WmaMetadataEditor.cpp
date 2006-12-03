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
#include "WmaMetadataEditor.h"

WmaMetadataEditor::WmaMetadataEditor(const wchar_t* filename)
{
    this->filename = filename;
    this->headerInfo = NULL;
    this->editor = NULL;
    this->headerInfo = NULL;

    if (this->LoadFromFile() != S_OK)
    {
        this->ReleaseCOMObjects();
    }
}

WmaMetadataEditor::~WmaMetadataEditor()
{
    this->ReleaseCOMObjects();
}

std::wstring WmaMetadataEditor::GetStrAttribute(const wchar_t* attribute)
{
    if (this->IsValid())
    {
        BYTE* pResult = NULL;
        this->GetHeaderAttribute(attribute,  &pResult);

        if (pResult)
        {
            std::wstring result((wchar_t*)pResult);
            delete pResult;
            return result;
        }
    }

    return L"";
}

DWORD WmaMetadataEditor::GetDWORDAttribute(const wchar_t* attribute)
{
    if (this->IsValid())
    {
        BYTE* pResult;
        this->GetHeaderAttribute(attribute, &pResult);

        if (pResult)
        {
            DWORD result = *(DWORD*)pResult;
            delete pResult;
            return result;
        }
    }

    return 0;
}

QWORD WmaMetadataEditor::GetQWORDAttribute(const wchar_t* attribute)
{
    if (this->IsValid())
    {
        BYTE* pResult;
        this->GetHeaderAttribute(attribute, &pResult);

        if (pResult)
        {
            QWORD result = *(QWORD*)pResult;
            delete pResult;
            return result;
        }
    }

    return 0;
}

BOOL WmaMetadataEditor::GetBOOLAttribute(const wchar_t* attribute)
{
    if (this->IsValid())
    {
        BYTE* pResult;
        this->GetHeaderAttribute(attribute, &pResult);

        if (pResult)
        {
            BOOL result = *(BOOL*)pResult;
            delete pResult;
            return result;
        }
    }

    return false;
}

HRESULT WmaMetadataEditor::SetStrAttribute(const wchar_t* attribute, const wchar_t* value)
{
    if (this->IsValid())
    {
        return this->headerInfo->SetAttribute(
            0, 
            attribute,
            WMT_TYPE_STRING,
            (BYTE*)value,
            (WORD)(sizeof(wchar_t) * ::wcslen(value)));
    }

    return S_FALSE;
}

HRESULT WmaMetadataEditor::SetDWORDAttribute(const wchar_t* attribute, DWORD value)
{
    if (this->IsValid())
    {
        return this->headerInfo->SetAttribute(
            0, 
            attribute,
            WMT_TYPE_DWORD,
            (BYTE*)&value,
            sizeof(DWORD));
    }

    return S_FALSE;
}

HRESULT WmaMetadataEditor::SetQWORDAttribute(const wchar_t* attribute, QWORD value)
{
    if (this->IsValid())
    {
        return this->headerInfo->SetAttribute(
            0, 
            attribute,
            WMT_TYPE_QWORD,
            (BYTE*)&value,
            sizeof(QWORD));
    }

    return S_FALSE;
}

HRESULT WmaMetadataEditor::SetBOOLAttribute(const wchar_t* attribute, BOOL value)
{
    if (this->IsValid())
    {
        return this->headerInfo->SetAttribute(
            0, 
            attribute,
            WMT_TYPE_BOOL,
            (BYTE*)&value,
            sizeof(BOOL));
    }

    return S_FALSE;
}
HRESULT WmaMetadataEditor::GetHeaderAttribute(const wchar_t* attribute, BYTE** ppbValue)
{
    BYTE                *pbValue = NULL;
    HRESULT             hr = S_OK;
    WMT_ATTR_DATATYPE   wmtType;
    WORD                wStreamNum = 0;
    WORD                cbLength = 0;

    //
    // Sanity check
    //
    if(! this->headerInfo)
    {
        return E_UNEXPECTED;
    }

    if(! ppbValue)
    {
        return E_INVALIDARG;
    }

    //
    // Get the count of bytes to be allocated for pbValue
    //
    hr = this->headerInfo->GetAttributeByName(&wStreamNum, attribute, &wmtType, NULL, &cbLength);
    if(FAILED(hr) && (ASF_E_NOTFOUND != hr))
    {
        return hr;
    }

    //
    // No such an attribute, so return
    //
    if(ASF_E_NOTFOUND == hr)
    {
        *ppbValue = NULL;
        return S_OK;
    }

    pbValue = new BYTE[cbLength];
    if(! pbValue)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    //
    // Get the actual value
    //
    this->headerInfo->GetAttributeByName(&wStreamNum, attribute, &wmtType, pbValue, &cbLength);
    *ppbValue = pbValue;

    return S_OK;
}

bool WmaMetadataEditor::Write()
{
    bool result = false;

    if (this->IsValid())
    {
        result = (this->editor->Flush() == S_OK);
    }

    this->LoadFromFile();

    return result;
}

HRESULT WmaMetadataEditor::LoadFromFile()
{
    this->ReleaseCOMObjects();

    HRESULT hr = ::WMCreateEditor(&this->editor);
    if (hr != S_OK)
    {
        return hr;
    }

    hr = this->editor->Open(this->filename.c_str());
    if (hr != S_OK)
    {
        return hr;
    }

    hr = this->editor->QueryInterface(IID_IWMHeaderInfo, (void**)&headerInfo);

    return hr;
}

void WmaMetadataEditor::ReleaseCOMObjects()
{
    if (this->headerInfo)
    {
        this->headerInfo->Release();
        this->headerInfo = NULL;
    }

    if (this->editor)
    {
        this->editor->Release();
        this->editor = NULL;
    }
}