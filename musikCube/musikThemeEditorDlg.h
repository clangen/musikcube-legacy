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
#include "3rdparty/ColourPicker.h"
#include <map>

///////////////////////////////////////////////////

class CmusikColourPicker : public CColourPicker
{
public:

    CmusikColourPicker()
        : CColourPicker()
    {
        m_strCustomText = _T("More Colors...");
    }

    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
    {
        this->SetRedraw(FALSE);
        this->SetWindowText((this->GetColour() == AUTOMATIC_COLOR) ? _T("Auto") : _T(""));
        this->SetRedraw(TRUE);
        CColourPicker::DrawItem(lpDrawItemStruct);
    }
};

///////////////////////////////////////////////////

class CmusikThemeEditorDlg : public CDialog
{
public:
    CmusikThemeEditorDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CmusikThemeEditorDlg();

    enum { IDD = IDD_THEME_EDITOR };

protected:

    // mfc stuff
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support    
    afx_msg void OnBnClickedButtonLoad();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedButtonSave();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnChangeButtonIconTheme();
    virtual BOOL OnInitDialog();

    afx_msg LONG OnColorChanged(UINT lParam, LONG wParam);
    void ReloadColors();

    std::map<UINT, COLORREF*> m_ID_ToColorMap;
    std::map<UINT, CmusikColourPicker*> m_ID_ToControlMap;

    CmusikColourPicker 
        m_ActiveCaptionPicker,
        m_ActiveCaptionTextPicker,
        m_InActiveCaptionPicker,
        m_InActiveCaptionTextPicker,
        m_BtnFacePicker,
        m_BtnTextPicker,
        m_BtnHilightPicker,
        m_BtnShadowPicker,
        m_ListCtrlPicker,
        m_ListCtrlTextPicker,
        m_HighLightPicker,
        m_HighLightTextPicker;

    DECLARE_DYNAMIC(CmusikThemeEditorDlg)
    DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////
