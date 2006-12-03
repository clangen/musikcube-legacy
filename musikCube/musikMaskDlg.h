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
#include <musikCore.h>

///////////////////////////////////////////////////

enum
{
    MUSIK_MASK_FROMFILENAME = 0,
    MUSIK_MASK_TOFILENAME
};

///////////////////////////////////////////////////

class CmusikMaskDlg : public CDialog
{

public:

    //construct and destruct
    CmusikMaskDlg(CWnd* pParent, int initial_state = MUSIK_MASK_TOFILENAME); 
    virtual ~CmusikMaskDlg();

    // misc
    enum { IDD = IDD_FILENAME_MASK };
    virtual void DoDataExchange(CDataExchange* pDX);

    // message maps
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedTitle();
    afx_msg void OnBnClickedArtist();
    afx_msg void OnBnClickedAlbum();
    afx_msg void OnBnClickedTrack();
    afx_msg void OnBnClickedGenre();
    afx_msg void OnBnClickedYear();
    afx_msg void OnBnClickedRating();
    afx_msg void OnBnClickedClr();
    afx_msg void OnEnKillfocusMask();
    afx_msg void OnClose();
    afx_msg void OnBnClickedPath2();
    afx_msg void OnBnClickedPath5();
    afx_msg void OnBnClickedPath3();
    afx_msg void OnBnClickedPath4();
    afx_msg void OnBnClickedPath6();
    afx_msg void OnBnClickedPath7();
    afx_msg void OnBnClickedPath8();
    afx_msg void OnBnClickedPath9();
    afx_msg void OnBnClickedButton2();
    afx_msg void OnBnClickedPath10();
    afx_msg void OnCbnSelchangeType();
    afx_msg void OnBnClickedApply();
    afx_msg void OnCbnEditchangeMask();
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnBnClickedButton8();
    afx_msg void OnBnClickedButton9();
    afx_msg void OnCbnSelchangeMask();

protected:

    // vars
    CComboBox m_Type;
    CWnd* m_Parent;
    int m_State;
    CString* m_CurrMask;
    CString m_FromFilename;
    CString m_ToFilename;
    bool m_SpcToUndr;
    bool m_UndrToSpc;
    musikCore::StringArray m_Masks;

    void AppendMask(CString str);
    void PopulateMasks();

    //macros
    DECLARE_DYNAMIC(CmusikMaskDlg)
    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedSpcundr();
};

///////////////////////////////////////////////////


