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

class CmusikEditInPlace : public CEdit
{
public:

    // construct and destruct
    CmusikEditInPlace();
    virtual ~CmusikEditInPlace();

    // set string
    void SetString(CString str){ m_Str = str; }
    CString GetString(){ return m_Str; }
    void SetArgs(void* pArgs){ m_Args = pArgs; };
    CWnd* GetMessageTarget() { return this->m_MessageTarget == NULL ? GetParent() : this->m_MessageTarget; }
    void SetMessageTarget(CWnd* wnd) { this->m_MessageTarget = wnd; }

    // attributes
    bool IsVisible();

    // what happened?
    void Commit();
    void Cancel();

    // overrides
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);

protected:

    // mfc message maps
    afx_msg void OnEnKillfocus();
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnEnChange();

    // message target
    CWnd* m_MessageTarget;

    // args
    void* m_Args;

    // last know string
    CString m_Str;
    CBrush m_brBg;

    // visible?
    bool m_IsVisible;
    bool m_Commited;
    bool m_MessageSent;

    // macros
    DECLARE_DYNAMIC(CmusikEditInPlace)
    DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

