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
////////////////////////////////////////////////////

#include "stdafx.h"

#include "musikCube.h"
#include "musikPrefsDlg.h"
#include "musikNameEntry.h"
#include "musikSourcesCtrl.h"
#include "musikGlobalHotkeysDlg.h"
#include "GlobalHotkeys.h"

#include <musikCore.h>
#include <atlbase.h>

///////////////////////////////////////////////////

class CmusikPropertyPage;

///////////////////////////////////////////////////

#pragma warning (disable : 4800)    // forcing value bool 'true' to 'false'
#pragma warning(disable : 4805)

///////////////////////////////////////////////////

// Interface::Workflow

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikPrefsInterfaceWorkflow, CmusikPropertyPage)

///////////////////////////////////////////////////

CmusikPrefsInterfaceWorkflow::CmusikPrefsInterfaceWorkflow()
    : CmusikPropertyPage(CmusikPrefsInterfaceWorkflow::IDD)
{
}

///////////////////////////////////////////////////

CmusikPrefsInterfaceWorkflow::~CmusikPrefsInterfaceWorkflow()
{
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceWorkflow::DoDataExchange(CDataExchange* pDX)
{
    CmusikPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_WHOLEPLAYLIST, m_AddEntireToNP);
    DDX_Control(pDX, IDC_CHECK2, m_PromptDropOnPlaylist);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikPrefsInterfaceWorkflow, CmusikPropertyPage)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikPrefsInterfaceWorkflow::LoadPrefs()
{
    m_PromptDropOnPlaylist.SetCheck(musikCube::g_Prefs->GetFileDropPrompt() == -1 ? true : false);

    CButton* ptrBtn;

    // now playing behavior
    ptrBtn = (CButton*)GetDlgItem(IDC_WHOLEPLAYLIST);
    ptrBtn->SetCheck(musikCube::g_Prefs->AddEntireToNP());
    ptrBtn = (CButton*)GetDlgItem(IDC_QUEUESONG);
    ptrBtn->SetCheck(!musikCube::g_Prefs->AddEntireToNP());
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceWorkflow::CommitChanges()
{
    if (!m_PromptDropOnPlaylist.GetCheck())
    {
        if (musikCube::g_Prefs->GetFileDropPrompt() == -1)
            musikCube::g_Prefs->SetFileDropPrompt(1);
    }
    else
        musikCube::g_Prefs->SetFileDropPrompt(-1);

    CButton* ptrBtn;

    // now playing
    ptrBtn = (CButton*)GetDlgItem(IDC_WHOLEPLAYLIST);
    musikCube::g_Prefs->SetAddEntireToNP(ptrBtn->GetCheck());

    m_Modified = false;
    SetModified(FALSE);
}

///////////////////////////////////////////////////

// Sound::Driver

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikPrefsSoundDriver, CmusikPropertyPage)

///////////////////////////////////////////////////

CmusikPrefsSoundDriver::CmusikPrefsSoundDriver()
    : CmusikPropertyPage(CmusikPrefsSoundDriver::IDD)
{
}

///////////////////////////////////////////////////

CmusikPrefsSoundDriver::~CmusikPrefsSoundDriver()
{
}

///////////////////////////////////////////////////

void CmusikPrefsSoundDriver::DoDataExchange(CDataExchange* pDX)
{
    DDX_Control(pDX, IDC_SOUND_DRIVER, m_SoundDriver);
    DDX_Control(pDX, IDC_SOUND_PLAYBACK_RATE, m_SoundPlaybackRate);
    DDX_Control(pDX, IDC_SNDBUFFER, m_SoundBufferSize);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikPrefsSoundDriver, CmusikPropertyPage)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikPrefsSoundDriver::LoadPrefs()
{
    GetSoundDrivers();
    GetSoundPlaybackRates();

    m_SoundBufferSize.SetWindowText(ItoS(musikCube::g_Prefs->GetPlayerBuffer()).c_str());
}

///////////////////////////////////////////////////

void CmusikPrefsSoundDriver::CommitChanges()
{
    CString tmp;

    int orig_driver    = musikCube::g_Prefs->GetPlayerDriver();
    int orig_rate    = musikCube::g_Prefs->GetPlayerRate();

    int new_driver    = m_SoundDriver.GetCurSel();

    m_SoundDriver.GetLBText(new_driver, tmp);
    if (tmp == L"Default") {
        new_driver = -1; /* default */
    }

    m_SoundPlaybackRate.GetWindowText(tmp);
    int new_rate    = StoI((wstring)tmp.GetBuffer());

    bool fmod_needs_restart = false;
    if (orig_driver != new_driver || orig_rate != new_rate)
        fmod_needs_restart = true;

    musikCube::g_Prefs->SetPlayerDriver(new_driver);
    musikCube::g_Prefs->SetPlayerRate(new_rate);

    // note: we could restart using musikCube::g_Player but
    // to be safe we'll post the event to the main
    // frame so it can update itself.

    if (fmod_needs_restart)
    {
        int res = MessageBox(
            _T("The sound system must be stopped and restarted to apply the new changes. If you do not to restart now, the changes will not take effect until musikCube as been restarted. Restart sound system?"),
            _T(MUSIK_VERSION_STR),
            MB_ICONINFORMATION | MB_YESNO);
        if (res == IDYES)
        {
            musikCube::g_Player->Stop();

            while (musikCube::g_Player->IsPlaying())
                Sleep(50);

            musikCube::g_Player->DeinitSound();
            musikCube::g_Player->InitSound(0, musikCube::g_Prefs->GetPlayerDriver(), musikCube::g_Prefs->GetPlayerRate(), musikCore::MUSIK_PLAYER_INIT_RESTART);
        }
    }

    // sound buffer size
    m_SoundBufferSize.GetWindowText(tmp);
    musikCore::String sbuffer = tmp;
    int size = StoI(sbuffer);
    if (size > 0)
    {
        musikCube::g_Prefs->SetPlayerBuffer(size);
        musikCube::g_Player->SetBufferSize(musikCube::g_Prefs->GetPlayerBuffer());
    }
    m_Modified = false;
    SetModified(FALSE);
}

///////////////////////////////////////////////////

void CmusikPrefsSoundDriver::GetSoundDrivers(bool populate)
{
     musikCube::g_Player->GetSoundDrivers(m_DriverInfo);

    if (populate)
    {
        m_SoundDriver.Clear();

        musikCore::String driverInfo;
        for (size_t i = 0; i < m_DriverInfo.size(); i++) {
            m_SoundDriver.InsertString(i, m_DriverInfo.at(i));
        }

        if (m_DriverInfo.size())
        {
            int driverId = musikCube::g_Prefs->GetPlayerDriver();
            if (driverId == -1)
                m_SoundDriver.SelectString(-1, L"Default");
            else if (driverId < (int)m_DriverInfo.size())
                m_SoundDriver.SelectString(-1, m_DriverInfo.at(musikCube::g_Prefs->GetPlayerDriver()));
            else
                m_SoundDriver.SelectString(-1, m_DriverInfo.at(0));
        }
    }
}

///////////////////////////////////////////////////

void CmusikPrefsSoundDriver::GetSoundPlaybackRates(bool populate)
{
    m_PlaybackRateInfo.clear();

    m_PlaybackRateInfo.push_back(_T("8000"));
    m_PlaybackRateInfo.push_back(_T("11025"));
    m_PlaybackRateInfo.push_back(_T("22050"));
    m_PlaybackRateInfo.push_back(_T("44100"));

    if (populate)
    {
        m_SoundPlaybackRate.Clear();

        for (size_t i = 0; i < m_PlaybackRateInfo.size(); i++)
            m_SoundPlaybackRate.InsertString(i, m_PlaybackRateInfo.at(i));

        if (m_PlaybackRateInfo.size())
        {
            CString playback_mode_str;
            playback_mode_str.Format(_T("%d"), musikCube::g_Prefs->GetPlayerRate());

            m_SoundPlaybackRate.SelectString(-1, playback_mode_str);
        }
    }
}

///////////////////////////////////////////////////

// Sound::Crossfader

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikPrefsSoundCrossfader, CmusikPropertyPage)

///////////////////////////////////////////////////

CmusikPrefsSoundCrossfader::CmusikPrefsSoundCrossfader()
    : CmusikPropertyPage(CmusikPrefsSoundCrossfader::IDD)
{
}

///////////////////////////////////////////////////

CmusikPrefsSoundCrossfader::~CmusikPrefsSoundCrossfader()
{
}

///////////////////////////////////////////////////

void CmusikPrefsSoundCrossfader::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_NEWSONG, m_NewSong);
    DDX_Control(pDX, IDC_PAUSERESUME, m_PauseResume);
    DDX_Control(pDX, IDC_STOP, m_Stop);
    DDX_Control(pDX, IDC_EXIT, m_Exit);
    DDX_Control(pDX, IDC_PRESETBOX, m_PresetBox);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikPrefsSoundCrossfader, CmusikPropertyPage)
    ON_BN_CLICKED(IDC_RESET, OnBnClickedReset)
    ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
    ON_BN_CLICKED(IDC_DELETE, OnBnClickedDelete)
    ON_LBN_SELCHANGE(IDC_PRESETBOX, OnLbnSelchangePresetbox)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikPrefsSoundCrossfader::LoadPrefs()
{
    // intervals
    musikCore::Crossfader fader;
    musikCube::g_Library->GetDefaultCrossfader(fader);

    Populate(fader);

    // presets
    musikCore::StringArray items;
    musikCube::g_Library->GetAllCrossfaderPresets(items, m_IDs, true);

    m_PresetBox.ResetContent();
    for (size_t i = 0; i < items.size(); i++)
        m_PresetBox.AddString(items.at(i));
}

///////////////////////////////////////////////////

void CmusikPrefsSoundCrossfader::Populate(const musikCore::Crossfader& fader)
{
    m_NewSong.SetWindowText(FloatToCString(fader.m_NewSong));
    m_PauseResume.SetWindowText(FloatToCString(fader.m_PauseResume));
    m_Stop.SetWindowText(FloatToCString(fader.m_Stop));
    m_Exit.SetWindowText(FloatToCString(fader.m_Exit));
}

///////////////////////////////////////////////////

void CmusikPrefsSoundCrossfader::RevPopulate(musikCore::Crossfader& fader)
{
    CString sWnd;

    m_NewSong.GetWindowText(sWnd);
    fader.m_NewSong = StoF(sWnd.GetBuffer(NULL));

    m_PauseResume.GetWindowText(sWnd);
    fader.m_PauseResume = StoF(sWnd.GetBuffer(NULL));

    fader.m_Seek = 0;

    m_Stop.GetWindowText(sWnd);
    fader.m_Stop = StoF(sWnd.GetBuffer(NULL));

    m_Exit.GetWindowText(sWnd);
    fader.m_Exit = StoF(sWnd.GetBuffer(NULL));
}

///////////////////////////////////////////////////

void CmusikPrefsSoundCrossfader::CommitChanges()
{
    musikCore::Crossfader fader_save;
    RevPopulate(fader_save);

    musikCube::g_Library->UpdateDefaultCrossfader(fader_save);

    if (musikCube::g_Player->IsCrossfaderEnabled())
        musikCube::g_Player->SetCrossfader(fader_save);
}

///////////////////////////////////////////////////

void CmusikPrefsSoundCrossfader::OnBnClickedReset()
{
    musikCube::g_Library->ResetDefaultCrossfader();

    musikCore::Crossfader fader;
    musikCube::g_Library->GetDefaultCrossfader(fader);

    Populate(fader);
}

///////////////////////////////////////////////////

void CmusikPrefsSoundCrossfader::OnBnClickedAdd()
{
    musikCore::Crossfader fader_new;
    RevPopulate(fader_new);

    CString name;
    CmusikNameEntry* pDlg = new CmusikNameEntry(this, &name);
    if (pDlg->DoModal() == IDOK && !name.IsEmpty())
    {
        fader_new.m_Name = name;
        int ret = musikCube::g_Library->CreateCrossfader(fader_new);
        if (ret == musikCore::MUSIK_LIBRARY_OK)
        {
            m_PresetBox.AddString(name);
            m_IDs.push_back(fader_new.m_ID);

            m_PresetBox.SetCurSel(m_PresetBox.GetCount() - 1);
        }
        else if (ret == musikCore::MUSIK_LIBRARY_ID_EXISTS)
            MessageBox(
                _T("Sorry, but a crossfader preset with this name already exists. Please enter a unique name."),
                _T(MUSIK_VERSION_STR),
                MB_OK | MB_ICONWARNING);
    }
    delete pDlg;
}

///////////////////////////////////////////////////

void CmusikPrefsSoundCrossfader::OnBnClickedDelete()
{
    int nSel = GetIndex();

    if (nSel > -1)
    {
        if (musikCube::g_Library->DeleteCrossfader(m_IDs.at(nSel)) == musikCore::MUSIK_LIBRARY_OK)
        {
            m_PresetBox.DeleteString(nSel);
            m_IDs.erase(m_IDs.begin() + nSel);

            --nSel;
            if (nSel == -1)
                nSel = 0;

            m_PresetBox.SetCurSel(nSel);
        }
    }
}

///////////////////////////////////////////////////

int CmusikPrefsSoundCrossfader::GetIndex()
{
    for (size_t i = 0; i < m_IDs.size(); i++)
    {
        if (m_PresetBox.GetSel(i))
            return i;
    }

    return -1;
}

///////////////////////////////////////////////////

void CmusikPrefsSoundCrossfader::OnLbnSelchangePresetbox()
{
    int nSel = GetIndex();

    if (nSel != -1)
    {
        musikCore::Crossfader fader;
        musikCube::g_Library->GetCrossfader(m_IDs.at(nSel), fader);

        Populate(fader);
    }
}

///////////////////////////////////////////////////

BOOL CmusikPrefsSoundCrossfader::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE && GetFocus() == &m_PresetBox)
    {
        OnBnClickedDelete();
        return true;
    }

    return CmusikPropertyPage::PreTranslateMessage(pMsg);
}

///////////////////////////////////////////////////

// Interface::Transparency

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikPrefsInterfaceTrans, CmusikPropertyPage)

///////////////////////////////////////////////////

CmusikPrefsInterfaceTrans::CmusikPrefsInterfaceTrans()
    : CmusikPropertyPage(CmusikPrefsInterfaceTrans::IDD)
{
}

///////////////////////////////////////////////////

CmusikPrefsInterfaceTrans::~CmusikPrefsInterfaceTrans()
{
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceTrans::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CHECK1, m_TransEnabled);
    DDX_Control(pDX, IDC_EDIT1, m_TransFocused);
    DDX_Control(pDX, IDC_EDIT2, m_TransUnfocused);
}

///////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(CmusikPrefsInterfaceTrans, CPropertyPage)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikPrefsInterfaceTrans::LoadPrefs()
{
    if (musikCube::g_Prefs->IsTransEnabled())
        m_TransEnabled.SetCheck(TRUE);

    m_TransFocused.SetWindowText(IntToCString(musikCube::g_Prefs->GetTransFocus()));
    m_TransUnfocused.SetWindowText(IntToCString(musikCube::g_Prefs->GetTransUnFocus()));
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceTrans::CommitChanges()
{
    // toggles
    musikCube::g_Prefs->SetTransEnabled((bool)m_TransEnabled.GetCheck());

    CString temp;
    int focus_per, unfocus_per;

    // focused
    m_TransFocused.GetWindowText(temp);
    focus_per = StoI(temp.GetBuffer(NULL));

    if (focus_per > 255)
        focus_per = 255;
    if (focus_per < 0)
        focus_per = 0;
    musikCube::g_Prefs->SetTransFocus(focus_per);

    // unfocused
    m_TransUnfocused.GetWindowText(temp);
    unfocus_per = StoI(temp.GetBuffer(NULL));

    if (unfocus_per > 255)
        unfocus_per = 255;
    if (unfocus_per < 0)
        unfocus_per = 0;

    musikCube::g_Prefs->SetTransUnFocus(unfocus_per);

    if (musikCube::g_Prefs->IsTransEnabled())
    {
        int WM_INITTRANS = RegisterWindowMessage(_T("INITTRANS"));
        AfxGetApp()->m_pMainWnd ->SendMessage(WM_INITTRANS);
    }
    else
    {
        int WM_DEINITTRANS = RegisterWindowMessage(_T("DEINITTRANS"));
        AfxGetApp()->m_pMainWnd ->SendMessage(WM_DEINITTRANS);
    }

    // reload
    LoadPrefs();

    SetModified(FALSE);
    m_Modified = false;
}

///////////////////////////////////////////////////

// Tunage

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikPrefsTunage, CmusikPropertyPage)

///////////////////////////////////////////////////

CmusikPrefsTunage::CmusikPrefsTunage()
    : CmusikPropertyPage(CmusikPrefsTunage::IDD)
{
}

///////////////////////////////////////////////////

CmusikPrefsTunage::~CmusikPrefsTunage()
{
}

///////////////////////////////////////////////////

void CmusikPrefsTunage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TUNAGEENABLED, m_Enabled);
    DDX_Control(pDX, IDC_TUNAGEURLENABLED, m_PostURLEnabled);
    DDX_Control(pDX, IDC_TUNAGEURL, m_URL);
    DDX_Control(pDX, IDC_TUNAGEFILEENABLED, m_WriteFileEnabled);
    DDX_Control(pDX, IDC_TUNAGEFILENAME, m_Filename);
    DDX_Control(pDX, IDC_TUNAGEFILELINE, m_Fileline);
    DDX_Control(pDX, IDC_TUNAGEAPPEND, m_Append);
    DDX_Control(pDX, IDC_TUNAGERUNAPPENABLED, m_RunAppEnabled);
    DDX_Control(pDX, IDC_TUNAGECOMMANDLINE, m_CommandLine);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikPrefsTunage, CPropertyPage)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikPrefsTunage::LoadPrefs()
{
    m_Enabled.SetCheck(musikCube::g_Prefs->GetTunageEnabled());
    m_PostURLEnabled.SetCheck(musikCube::g_Prefs->GetTunagePostURL());
    m_URL.SetWindowText(musikCore::utf8to16(musikCube::g_Prefs->GetTunageURL()).c_str());
    m_WriteFileEnabled.SetCheck(musikCube::g_Prefs->GetTunageWriteFile());
    m_Filename.SetWindowText(musikCore::utf8to16(musikCube::g_Prefs->GetTunageFilename()).c_str());
    m_Fileline.SetWindowText(musikCore::utf8to16(musikCube::g_Prefs->GetTunageFileLine()).c_str());
    m_Append.SetCheck(musikCube::g_Prefs->GetTunageAppendFile());
    m_RunAppEnabled.SetCheck(musikCube::g_Prefs->GetTunageRunApp());
    m_CommandLine.SetWindowText(musikCore::utf8to16(musikCube::g_Prefs->GetTunageCmdLine()).c_str());
}

///////////////////////////////////////////////////

void CmusikPrefsTunage::CommitChanges()
{
    CString sWnd;

    musikCube::g_Prefs->SetTunageEnabled(m_Enabled.GetCheck() );

    musikCube::g_Prefs->SetTunagePostURL(m_PostURLEnabled.GetCheck() );
    m_URL.GetWindowText(sWnd);
    musikCube::g_Prefs->SetTunageURL(sWnd.GetBuffer());

    musikCube::g_Prefs->SetTunageWriteFile(m_WriteFileEnabled.GetCheck() );
    m_Filename.GetWindowText(sWnd);
    musikCube::g_Prefs->SetTunageFilename(sWnd.GetBuffer());

    m_Fileline.GetWindowText(sWnd);
    musikCube::g_Prefs->SetTunageFileLine(sWnd.GetBuffer());

    musikCube::g_Prefs->SetTunageAppendFile(m_Append.GetCheck() );
    musikCube::g_Prefs->SetTunageRunApp(m_RunAppEnabled.GetCheck() );

    m_CommandLine.GetWindowText(sWnd);
    musikCube::g_Prefs->SetTunageCmdLine(sWnd.GetBuffer());

    // reload
    LoadPrefs();

    SetModified(FALSE);
    m_Modified = false;
}

///////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CmusikPrefsAutoCapitalize, CmusikPropertyPage)

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikPrefsAutoCapitalize, CmusikPropertyPage)
    ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
    ON_BN_CLICKED(IDC_DELETE, OnBnClickedDelete)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

CmusikPrefsAutoCapitalize::CmusikPrefsAutoCapitalize() :
    CmusikPropertyPage(IDD)
{
}

///////////////////////////////////////////////////

CmusikPrefsAutoCapitalize::~CmusikPrefsAutoCapitalize()
{
}

///////////////////////////////////////////////////

void CmusikPrefsAutoCapitalize::DoDataExchange(CDataExchange* pDX)
{
    CmusikPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

///////////////////////////////////////////////////


void CmusikPrefsAutoCapitalize::OnBnClickedAdd()
{
    CString name;
    CmusikNameEntry pDlg(this, &name);
    if (pDlg.DoModal() == IDOK && !name.IsEmpty())
    {
        int ret = musikCube::g_Library->AddChgCase(name.GetBuffer(0));
        if (ret == musikCore::MUSIK_LIBRARY_OK)
        {
            m_ListBox.AddString(name);
            m_Items.push_back(name.GetBuffer(0));

            m_ListBox.SetCurSel(m_ListBox.GetCount() - 1);
        }
        else
            MessageBox(
                _T("Sorry, but this word already exists in the library. Please enter a unique word."),
                _T(MUSIK_VERSION_STR),
                MB_OK | MB_ICONWARNING);
    }
}

///////////////////////////////////////////////////

void CmusikPrefsAutoCapitalize::OnBnClickedDelete()
{
    int nSel = -1;
    for (size_t i = 0; i < m_Items.size(); i++)
    {
        if (m_ListBox.GetSel(i))
        {
            nSel = i;
            break;
        }
    }

    if (nSel > -1)
    {
        int ret = musikCube::g_Library->RemoveChgCase(m_Items.at(nSel));
        if (ret == musikCore::MUSIK_LIBRARY_OK)
        {
            m_ListBox.DeleteString(nSel);
            m_Items.erase(m_Items.begin() + nSel);

            --nSel;
            if (nSel == -1)
                nSel = 0;

            m_ListBox.SetCurSel(nSel);
        }
    }
}

///////////////////////////////////////////////////

void CmusikPrefsAutoCapitalize::LoadPrefs()
{
    musikCube::g_Library->GetAllChgCase(m_Items);

    m_ListBox.ResetContent();
    for (size_t i = 0; i < m_Items.size(); i++)
        m_ListBox.AddString(m_Items.at(i));
}

///////////////////////////////////////////////////

// CmusikPrefsInterfaceGeneral

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikPrefsInterfaceGeneral, CmusikPropertyPage)

///////////////////////////////////////////////////

CmusikPrefsInterfaceGeneral::CmusikPrefsInterfaceGeneral()
    : CmusikPropertyPage(CmusikPrefsInterfaceGeneral::IDD)
{

}

///////////////////////////////////////////////////

CmusikPrefsInterfaceGeneral::~CmusikPrefsInterfaceGeneral()
{
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceGeneral::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CHECK_ENABLETRAYICON, m_EnableTrayIcon);
    DDX_Control(pDX, IDC_CHECK_STARTWITHWINDOWS, m_StartWithWindows);
    DDX_Control(pDX, IDC_CHECK4, m_AutoSynchronize);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikPrefsInterfaceGeneral, CmusikPropertyPage)
    ON_BN_CLICKED(IDC_GLOBALHOTKEYS_BUTTON, OnBnGlobalHotkeys)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikPrefsInterfaceGeneral::LoadPrefs()
{
    m_EnableTrayIcon.SetCheck(musikCube::g_Prefs->MinimizeToTray());
    m_StartWithWindows.SetCheck(GetStartup());
    m_AutoSynchronize.SetCheck(musikCube::g_Prefs->SynchronizeOnStartup());

    CButton* ptrBtn;

    // utc
    ptrBtn = (CButton*)GetDlgItem(IDC_USEUTC);
    ptrBtn->SetCheck(musikCube::g_Prefs->GetUseUTC());

    // balloon tips
    ptrBtn = (CButton*)GetDlgItem(IDC_CHECK_ENABLEBALLOONTIPS);
    ptrBtn->SetCheck(musikCube::g_Prefs->GetShowBalloonTips());

    // close to system tray
    ptrBtn = (CButton*)GetDlgItem(IDC_CLOSETOTRAY);
    ptrBtn->SetCheck(musikCube::g_Prefs->GetCloseToSystemTray());
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceGeneral::CommitChanges()
{
    musikCube::g_Prefs->SetMinimizeToTray(m_EnableTrayIcon.GetCheck());

    if (m_StartWithWindows.GetCheck())
        EnableStartup();
    else
        DisableStartup();

    CButton* ptrBtn;

    // use UTC
    ptrBtn = (CButton*)GetDlgItem(IDC_USEUTC);
    if ((bool)ptrBtn->GetCheck() != musikCube::g_Prefs->GetUseUTC())
    {
        musikCube::g_Library->SetUseUTC(ptrBtn->GetCheck());
        musikCube::g_Prefs->SetUseUTC(ptrBtn->GetCheck());
    }

    // balloon tips
    ptrBtn = (CButton*)GetDlgItem(IDC_CHECK_ENABLEBALLOONTIPS);
    musikCube::g_Prefs->SetShowBalloonTips(ptrBtn->GetCheck());

    // auto sync
    musikCube::g_Prefs->SetSynchronizeOnStartup(m_AutoSynchronize.GetCheck());

    // close to tray
    ptrBtn = (CButton*)GetDlgItem(IDC_CLOSETOTRAY);
    musikCube::g_Prefs->SetCloseToSystemTray(ptrBtn->GetCheck());
}

///////////////////////////////////////////////////

bool CmusikPrefsInterfaceGeneral::GetStartup()
{
    CmusikCubeApp* app = (CmusikCubeApp*)AfxGetApp();
    musikCore::String userdir = app->GetUserDir();
    userdir += _T("\\Start Menu\\Programs\\Startup\\musikCube.lnk");

    return musikCore::Filename::FileExists(userdir);
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceGeneral::EnableStartup()
{
    CmusikCubeApp* app = (CmusikCubeApp*)AfxGetApp();

    // where we want to store the startup shortcut
    musikCore::String shortcut_path;
    shortcut_path = app->GetUserDir();
    shortcut_path += _T("\\Start Menu\\Programs\\Startup\\musikCube.lnk");

    if (musikCore::Filename::FileExists(shortcut_path))
        return;

    // path to musik's exe
    musikCore::String program_path;
    program_path = CmusikCubeApp::GetWorkingDir(true);

    // start code
    HRESULT hRes = E_FAIL;
    DWORD dwRet = 0;
    CComPtr<IShellLink> ipShellLink;

    // buffer that receives the null-terminated string
    // for the drive and path
    TCHAR szPath[MAX_PATH];

    // buffer that receives the address of the final
    // file name component in the path
    LPTSTR lpszFilePart;
    WCHAR wszTemp[MAX_PATH];

    // Retrieve the full path and file name of a specified file
    dwRet = GetFullPathName(program_path.c_str(), sizeof(szPath) / sizeof(TCHAR), szPath, &lpszFilePart);

    if (!dwRet)
        return;

    // Get a pointer to the IShellLink interface
    hRes = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&ipShellLink);

    if (SUCCEEDED(hRes))
    {
        // Get a pointer to the IPersistFile interface
        CComQIPtr<IPersistFile> ipPersistFile(ipShellLink);

        // Set the path to the shortcut target and add the description
        hRes = ipShellLink->SetPath(szPath);
        if (FAILED(hRes))
            return;

        hRes = ipShellLink->SetArguments(_T("--autostart"));
        if (FAILED(hRes))
            return;

        hRes = ipShellLink->SetDescription(_T("musikCube"));
        if (FAILED(hRes))
            return;

        // IPersistFile is using LPCOLESTR, so make sure
        // that the string is Unicode
        #if !defined _UNICODE
            MultiByteToWideChar(CP_ACP, 0, shortcut_path.c_str(), -1, wszTemp, MAX_PATH);
        #else
            wcsncpy(wszTemp, shortcut_path.c_str(), MAX_PATH);
        #endif

        // Write the shortcut to disk
        hRes = ipPersistFile->Save(wszTemp, TRUE);
    }
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceGeneral::DisableStartup()
{
    musikCore::String shortcut_path, program_path;
    wchar_t buffer[2000];

    // where we want to store the startup shortcut
    GetEnvironmentVariable(_T("USERPROFILE"), buffer, sizeof(buffer));
    shortcut_path = buffer;
    shortcut_path += _T("\\Start Menu\\Programs\\Startup\\musikCube.lnk");

    if (musikCore::Filename::FileExists(shortcut_path))
        _wremove(shortcut_path.c_str());

}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceGeneral::OnBnGlobalHotkeys()
{
    CmusikGlobalHotkeysDlg dlg;
   dlg.DoModal();
}

///////////////////////////////////////////////////

// CmusikPrefsInterfaceDevices

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikPrefsInterfaceDevices, CmusikPropertyPage)

///////////////////////////////////////////////////

CmusikPrefsInterfaceDevices::CmusikPrefsInterfaceDevices()
    : CmusikPropertyPage(CmusikPrefsInterfaceDevices::IDD)
{

}

///////////////////////////////////////////////////

CmusikPrefsInterfaceDevices::~CmusikPrefsInterfaceDevices()
{
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceDevices::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST3, m_lstVisible);
    DDX_Control(pDX, IDC_LIST1, m_lstHidden);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikPrefsInterfaceDevices, CmusikPropertyPage)
    ON_BN_CLICKED(IDC_HIDEDEV, OnBnClickedHidedev)
    ON_BN_CLICKED(IDC_SHOWDEV, OnBnClickedShowdev)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikPrefsInterfaceDevices::LoadPrefs()
{
    CmusikSourcesCtrl::FindAttachedDevices(m_Drives);

    musikCore::StringArray hidden = musikCube::g_Prefs->GetHiddenDevices();
    for (size_t i = 0; i < hidden.size(); i++)
        m_lstHidden.AddString(hidden.at(i));

    for (size_t i = 0; i < m_Drives.size(); i++)
    {
        bool found = false;
        for (size_t j = 0; j < hidden.size(); j++)
        {
            if (m_Drives.at(i) == hidden.at(j))
            {
                found = true;
                break;
            }
        }

        if (!found)
            m_lstVisible.AddString(m_Drives.at(i));
    }
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceDevices::CommitChanges()
{
    musikCore::StringArray hidden;
    for (int i = 0; i < m_lstHidden.GetCount(); i++)
    {
        CString target;
        m_lstHidden.GetText(i, target);
        hidden.push_back(musikCore::String(target));
    }

    musikCube::g_Prefs->SetHiddenDevices(hidden);
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceDevices::OnBnClickedHidedev()
{
    for (int i = 0; i < m_lstVisible.GetCount(); i++)
    {
        if (m_lstVisible.GetSel(i))
        {
            CString target;
            m_lstVisible.GetText(i, target);
            m_lstVisible.DeleteString(i);
            m_lstHidden.AddString(target);
            return;
        }
    }
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceDevices::OnBnClickedShowdev()
{
    for (int i = 0; i < m_lstHidden.GetCount(); i++)
    {
        if (m_lstHidden.GetSel(i))
        {
            CString target;
            m_lstHidden.GetText(i, target);
            m_lstHidden.DeleteString(i);
            m_lstVisible.AddString(target);
            return;
        }
    }
}

///////////////////////////////////////////////////

// CmusikPrefsInternet dialog

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikPrefsInternet, CmusikPropertyPage)
CmusikPrefsInternet::CmusikPrefsInternet()
    : CmusikPropertyPage(CmusikPrefsInternet::IDD)
{
}

///////////////////////////////////////////////////

CmusikPrefsInternet::~CmusikPrefsInternet()
{
}

///////////////////////////////////////////////////

void CmusikPrefsInternet::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikPrefsInternet, CmusikPropertyPage)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikPrefsInternet::LoadPrefs()
{
    GetDlgItem(IDC_FREEDBSERVER)->SetWindowText(musikCore::utf8to16(musikCube::g_Prefs->GetFreeDBServer()).c_str());
}

///////////////////////////////////////////////////

void CmusikPrefsInternet::CommitChanges()
{
    CString str;
    GetDlgItem(IDC_FREEDBSERVER)->GetWindowText(str);
    musikCube::g_Prefs->SetFreeDBServer(str.GetBuffer());
}

///////////////////////////////////////////////////


