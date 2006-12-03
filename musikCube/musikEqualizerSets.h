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
#include <musikCore.h>

///////////////////////////////////////////////////

class CmusikEqualizerSets : public CDialog
{
public:

    // construct and destruct
    CmusikEqualizerSets(CWnd* pParent = NULL);
    virtual ~CmusikEqualizerSets();

    // mfc message maps / overrides
    afx_msg void OnBnClickedCloseDlg();
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedAdd();
    afx_msg void OnClose();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnBnClickedRenameSel();
    afx_msg void OnLbnSelchangePresetBox();
    afx_msg void OnBnClickedDeleteSel();

    // misc
    enum { IDD = IDD_EQUALIZER_SETS };

protected:

    // misc
    virtual void DoDataExchange(CDataExchange* pDX); 

    int GetIndex();

    // internal list of equalizer ids
    // obtained when when dialog is first
    // intialized, and maintained while the
    // dialog is open
    musikCore::IntArray m_IDs;
    void ReloadEqualizers();

    // getting active
    void GetActiveEqualizer(musikCore::EQSettings* settings);

    // parent window to post events to
    CWnd* m_Parent;

    // controls
    CListBox m_PresetBox;

    // macros
    DECLARE_DYNAMIC(CmusikEqualizerSets)
    DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////


