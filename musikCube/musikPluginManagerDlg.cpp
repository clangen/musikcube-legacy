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
#include "musikPluginManagerDlg.h"
#include ".\musikpluginmanagerdlg.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikPluginManagerDlg, CDialog)

///////////////////////////////////////////////////

CmusikPluginManagerDlg::CmusikPluginManagerDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CmusikPluginManagerDlg::IDD, pParent)
{
}

///////////////////////////////////////////////////

CmusikPluginManagerDlg::~CmusikPluginManagerDlg()
{
}

///////////////////////////////////////////////////

void CmusikPluginManagerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_DESCRIPTION, m_Description);
    DDX_Control(pDX, IDC_PLUGINS, m_PluginNames);
    DDX_Control(pDX, IDC_CONFIGURE, m_ConfigureBtn);
    DDX_Control(pDX, IDC_EXECUTE, m_ExecuteBtn);
    DDX_Control(pDX, IDC_STOP, m_StopBtn);
    DDX_Control(pDX, IDC_ABOUT, m_AboutBtn);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikPluginManagerDlg, CDialog)
    ON_LBN_SELCHANGE(IDC_PLUGINS, OnLbnSelchangePlugins)
    ON_BN_CLICKED(IDC_EXECUTE, OnBnClickedExecute)
    ON_BN_CLICKED(IDC_ABOUT, OnBnClickedAbout)
    ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)
    ON_BN_CLICKED(IDC_CONFIGURE, OnBnClickedConfigure)
    ON_BN_CLICKED(IDC_STOP, OnBnClickedStop)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

BOOL CmusikPluginManagerDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    Populate();    
    return TRUE;
}

///////////////////////////////////////////////////

void CmusikPluginManagerDlg::Populate()
{
    for (size_t i = 0; i < musikCube::g_Plugins.size(); i++)
        m_PluginNames.AddString(musikCube::g_Plugins.at(i).GetPluginName());

    m_ConfigureBtn.EnableWindow(false);
    m_AboutBtn.EnableWindow(false);
    m_ExecuteBtn.EnableWindow(false);
    m_StopBtn.EnableWindow(false);
}

///////////////////////////////////////////////////

void CmusikPluginManagerDlg::OnLbnSelchangePlugins()
{
    int sel = GetIndex();
    if (sel > -1)
    {
        m_Description.SetWindowText(
            musikCube::g_Plugins.at(sel).GetPluginDescription());

        m_ConfigureBtn.EnableWindow(musikCube::g_Plugins.at(sel).CanConfigure());
        m_AboutBtn.EnableWindow(musikCube::g_Plugins.at(sel).CanAbout());
        m_ExecuteBtn.EnableWindow(musikCube::g_Plugins.at(sel).CanExecute());
        m_StopBtn.EnableWindow(musikCube::g_Plugins.at(sel).CanStop());
    }
}

///////////////////////////////////////////////////

void CmusikPluginManagerDlg::OnBnClickedExecute()
{
    int sel = GetIndex();
    if (sel > -1)
        musikCube::g_Plugins.at(sel).Execute();
}

///////////////////////////////////////////////////

void CmusikPluginManagerDlg::OnBnClickedAbout()
{
    int sel = GetIndex();
    if (sel > -1)
        musikCube::g_Plugins.at(sel).About();
}

///////////////////////////////////////////////////

int CmusikPluginManagerDlg::GetIndex()
{
    for (int i = 0; i < m_PluginNames.GetCount(); i++)
    {
        if (m_PluginNames.GetSel(i))
            return i;
    }
    return -1;
}

///////////////////////////////////////////////////

void CmusikPluginManagerDlg::OnBnClickedClose()
{
    this->OnCancel();
}

///////////////////////////////////////////////////

void CmusikPluginManagerDlg::OnBnClickedConfigure()
{
    int sel = GetIndex();
    if (sel > -1)
        musikCube::g_Plugins.at(sel).Configure();
}

///////////////////////////////////////////////////

void CmusikPluginManagerDlg::OnBnClickedStop()
{
    int sel = GetIndex();
    if (sel > -1)
        musikCube::g_Plugins.at(sel).Stop();
}

///////////////////////////////////////////////////


