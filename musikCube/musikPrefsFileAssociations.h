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

#include "afxwin.h"
#include "musikCube.h"
#include "musikPropertyPage.h"
#include <musikCore.h>
#include <map>

///////////////////////////////////////////////////

class CmusikPrefsFileAssociations : public CmusikPropertyPage
{
public:
    CmusikPrefsFileAssociations();
    virtual ~CmusikPrefsFileAssociations();

    virtual void LoadPrefs();
    virtual void CommitChanges();
    virtual BOOL OnInitDialog();

    enum { IDD = IDD_PROPPAGE_FILE_ASSOCIATIONS };

protected:

    class Association
    {
    public:
        Association(const std::wstring& extension);
        virtual ~Association(){}

        void SetAssociation();
        void UnsetAssociation();

        bool IsAssociated();

    private:

        bool SetFileClass(const std::wstring& newFileClass, const std::wstring& oldFileClass);
        void LoadFileClass();

        std::wstring m_CurrentFileClass;
        std::wstring m_OldFileClass;
        std::wstring m_Extension;

        static const std::wstring s_OldKeyName;
    };

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_DYNAMIC(CmusikPrefsFileAssociations)
    DECLARE_MESSAGE_MAP()

    std::map<int, Association*> m_AssocationMap;
    CListCtrl m_ExtensionListCtrl;

    void CreateOrUpdateFileClass();
    static const std::wstring s_CubeFileClass;
};

///////////////////////////////////////////////////

