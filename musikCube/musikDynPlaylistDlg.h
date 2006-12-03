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
#include <musikCore.h>
#include "afxwin.h"

///////////////////////////////////////////////////

class CmusikDynPlaylistDlg : public CDialog
{
public:

    // construct / destruct
    CmusikDynPlaylistDlg(CWnd* pParent = NULL);   // standard constructor
    CmusikDynPlaylistDlg(CString& name);
    virtual ~CmusikDynPlaylistDlg();

    // message maps
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();

    // gets
    musikCore::String GetName(){ return musikCore::String(m_Name); }
    musikCore::String GetQuery(){ return musikCore::String(m_Query); }
    void SetName(musikCore::String& name){ m_Name = name; }
    void SetQuery(musikCore::String& query){ m_Query = query; }
    virtual BOOL OnInitDialog();
    CEdit m_EditName;
    CEdit m_EditQuery;

    // dialog data
    enum { IDD = IDD_DYNPLAYLISTEDITOR };

protected:

    // vars
    CString m_Name;
    CString m_Query;

    // misc
    virtual void DoDataExchange(CDataExchange* pDX);

    // macros
    DECLARE_DYNAMIC(CmusikDynPlaylistDlg)
    DECLARE_MESSAGE_MAP()

};

///////////////////////////////////////////////////

