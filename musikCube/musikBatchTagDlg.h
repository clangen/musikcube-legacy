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

#include "Resource.h"
#include "afxwin.h"

#include <musikCore.h>

///////////////////////////////////////////////////

class CmusikBatchTagDlg : public CDialog
{

public:

    // construct and destruct
    CmusikBatchTagDlg(CWnd* parent, musikCore::SongInfoArray* songs);
    virtual ~CmusikBatchTagDlg();

    // mfc messages and overrides
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedEnbtitle();
    afx_msg void OnBnClickedEnbartist();
    afx_msg void OnBnClickedEnbalbum();
    afx_msg void OnBnClickedEnbtrack();
    afx_msg void OnBnClickedEnbgenre();
    afx_msg void OnBnClickedEnbyear();
    afx_msg void OnBnClickedEnbrating();
    virtual BOOL OnInitDialog();

    // misc
    enum { IDD = IDD_BATCH_TAG };
    virtual void DoDataExchange(CDataExchange* pDX);

protected:

    // objects
    musikCore::SongInfoArray* m_Songs;
    CWnd* m_Parent;

    // controls
    CEdit m_Title;
    CEdit m_Artist;
    CEdit m_Album;
    CEdit m_Track;
    CComboBox m_Genre;
    CEdit m_Year;
    CComboBox m_Rating;

    // helpers
    void Apply();

    // macros
    DECLARE_DYNAMIC(CmusikBatchTagDlg)
    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedEnbcomment();
    CEdit m_Comment;
};

///////////////////////////////////////////////////
