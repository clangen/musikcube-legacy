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

#include "musikCube.h"
#include "musikEqualizerSets.h"
#include "musikEqualizerCtrl.h"
#include "musikNameEntry.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikEqualizerSets, CDialog)

///////////////////////////////////////////////////

CmusikEqualizerSets::CmusikEqualizerSets(CWnd* pParent)
    : CDialog(CmusikEqualizerSets::IDD, pParent)
{
    m_Parent = pParent;
}

///////////////////////////////////////////////////

CmusikEqualizerSets::~CmusikEqualizerSets()
{
}

///////////////////////////////////////////////////

void CmusikEqualizerSets::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PRESET_BOX, m_PresetBox);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikEqualizerSets, CDialog)
    ON_BN_CLICKED(IDC_CLOSE_DLG, OnBnClickedCloseDlg)
    ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_RENAME_SEL, OnBnClickedRenameSel)
    ON_LBN_SELCHANGE(IDC_PRESET_BOX, OnLbnSelchangePresetBox)
    ON_BN_CLICKED(IDC_DELETE_SEL, OnBnClickedDeleteSel)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikEqualizerSets::OnBnClickedCloseDlg()
{
    OnClose();
}

///////////////////////////////////////////////////

BOOL CmusikEqualizerSets::OnInitDialog()
{
    CDialog::OnInitDialog();
    ReloadEqualizers();
    return TRUE;
}

///////////////////////////////////////////////////

void CmusikEqualizerSets::ReloadEqualizers()
{
    musikCore::StringArray items;
    musikCube::g_Library->GetAllEqualizerPresets(items, m_IDs, true);

    m_PresetBox.ResetContent();
    for (size_t i = 0; i < items.size(); i++)
        m_PresetBox.AddString(items.at(i));
}

///////////////////////////////////////////////////

int CmusikEqualizerSets::GetIndex()
{
    for (size_t i = 0; i < m_IDs.size(); i++)
    {
        if (m_PresetBox.GetSel(i))
            return i;
    }

    return -1;
}

///////////////////////////////////////////////////

void CmusikEqualizerSets::OnBnClickedAdd()
{    
    musikCore::EQSettings settings;
    CmusikEqualizerBar* pBar = (CmusikEqualizerBar*)m_Parent;
    pBar->GetCtrl()->BandsToEQSettings(&settings);

    CString name;
    CmusikNameEntry* pDlg = new CmusikNameEntry(this, &name);
    if (pDlg->DoModal() == IDOK && !name.IsEmpty())
    {
        settings.m_Name = name;
        int ret = musikCube::g_Library->CreateEqualizer(settings, true);
        if (ret == 0)
        {
            /*
            m_PresetBox.AddString(name);
            m_IDs.push_back(settings.m_ID);
            */
            ReloadEqualizers();
        }
        else if (ret == musikCore::MUSIK_LIBRARY_ID_EXISTS)
            MessageBox(
                _T("Sorry, but an equalizer preset with this name already exists. Please enter a unique name."), 
                _T(MUSIK_VERSION_STR), 
                MB_OK | MB_ICONWARNING);
    }
    delete pDlg;
}

///////////////////////////////////////////////////

void CmusikEqualizerSets::OnClose()
{
    int WM_CLOSEEQUALIZERPRESETS = RegisterWindowMessage(_T("CLOSEEQUALIZERPRESETS"));
    m_Parent->PostMessage(WM_CLOSEEQUALIZERPRESETS);
}

///////////////////////////////////////////////////

BOOL CmusikEqualizerSets::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
    {
        OnClose();
        return true;
    }
    else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE && GetFocus() == &m_PresetBox)
    {
        OnBnClickedDeleteSel();
        return true;
    }

    return CDialog::PreTranslateMessage(pMsg);
}

///////////////////////////////////////////////////

void CmusikEqualizerSets::OnBnClickedRenameSel()
{
    int nSel = GetIndex();

    if (nSel > -1)
    {
        CString rename;
        CmusikNameEntry* pDlg = new CmusikNameEntry(this, &rename);
        if (pDlg->DoModal() == IDOK && !rename.IsEmpty())
        {
            musikCore::EQSettings settings;
            CmusikEqualizerBar* pBar = (CmusikEqualizerBar*)m_Parent;
            pBar->GetCtrl()->BandsToEQSettings(&settings);

            settings.m_ID = m_IDs.at(nSel);
            settings.m_Name = rename;

            if (musikCube::g_Library->UpdateEqualizer(m_IDs.at(nSel), settings) == 0)
            {
                /*
                m_PresetBox.DeleteString(nSel);
                m_IDs.erase(m_IDs.begin() + nSel);

                m_PresetBox.InsertString(nSel, rename);
                m_IDs.insert(m_IDs.begin() + nSel, settings.m_ID);

                m_PresetBox.SetCurSel(m_PresetBox.GetCount() - 1);
                */
                ReloadEqualizers();
            }
        }    
    }
}

///////////////////////////////////////////////////

void CmusikEqualizerSets::OnLbnSelchangePresetBox()
{
    int nSel = GetIndex();

    if (nSel != -1)
    {
        musikCore::EQSettings settings;
        musikCube::g_Library->GetEqualizer(m_IDs.at(nSel), settings);

        CmusikEqualizerBar* pBar = (CmusikEqualizerBar*)m_Parent;
        pBar->GetCtrl()->SetBandsFrom(settings);
        pBar->GetCtrl()->OnBandChange(NULL, NULL);
    }
}

///////////////////////////////////////////////////

void CmusikEqualizerSets::GetActiveEqualizer(musikCore::EQSettings* settings)
{
    CmusikEqualizerBar* pBar = (CmusikEqualizerBar*)m_Parent;
    pBar->GetCtrl()->BandsToEQSettings(settings);
}

///////////////////////////////////////////////////

void CmusikEqualizerSets::OnBnClickedDeleteSel()
{
    int nSel = GetIndex();

    if (nSel > -1)
    {
        if (musikCube::g_Library->DeleteEqualizer(m_IDs.at(nSel)) == musikCore::MUSIK_LIBRARY_OK)
        {
            /*
            m_PresetBox.DeleteString(nSel);
            m_IDs.erase(m_IDs.begin() + nSel);
    
            --nSel;
            if (nSel == -1)
                nSel = 0;

            m_PresetBox.SetCurSel(nSel);
            */
            ReloadEqualizers();
        }
    }
}

///////////////////////////////////////////////////
