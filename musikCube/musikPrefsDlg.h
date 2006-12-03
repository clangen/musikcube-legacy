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
#include "musikPrefsFileAssociations.h"
#include <musikCore.h>

///////////////////////////////////////////////////

// Interface::Workflow

///////////////////////////////////////////////////

class CmusikPrefsInterfaceWorkflow : public CmusikPropertyPage
{

public:

    // construct / destruct
    CmusikPrefsInterfaceWorkflow();
    virtual ~CmusikPrefsInterfaceWorkflow();

    // saving / loading
    virtual void CommitChanges();
    virtual void LoadPrefs();

    // dialog data
    enum { IDD = IDD_PROPPAGE_INTERFACE_WORKFLOW };

    // macros
    DECLARE_DYNAMIC(CmusikPrefsInterfaceWorkflow)
    DECLARE_MESSAGE_MAP()

protected:

    // misc
    virtual void DoDataExchange(CDataExchange* pDX); 

    // controls
    CButton m_AddEntireToNP;
    CButton m_PromptDropOnPlaylist;
};

///////////////////////////////////////////////////

// Sound::Driver

///////////////////////////////////////////////////

class CmusikPrefsSoundDriver : public CmusikPropertyPage
{
public:

    // construct / destruct
    CmusikPrefsSoundDriver();
    virtual ~CmusikPrefsSoundDriver();

    // dialog data
    enum { IDD = IDD_PROPPAGE_SOUND_DRIVER };

    // saving / loading
    virtual void CommitChanges();
    virtual void LoadPrefs();

    // macros
    DECLARE_DYNAMIC(CmusikPrefsSoundDriver)
    DECLARE_MESSAGE_MAP()

protected:

    // misc
    virtual void DoDataExchange(CDataExchange* pDX);

    // for getting sound devices
    void GetSoundDrivers(bool populate = true);
    void GetSoundPlaybackRates(bool populate = true);

    // controls
    CComboBox m_SoundDriver;
    CComboBox m_SoundPlaybackRate;
    CEdit m_SoundBufferSize;

    // arrays that hold sound driver / device info
    musikCore::StringArray m_DriverInfo;
    musikCore::StringArray m_PlaybackRateInfo;
};

///////////////////////////////////////////////////

// Sound::Crossfader

///////////////////////////////////////////////////

class CmusikPrefsSoundCrossfader : public CmusikPropertyPage
{

public:

    // construct and destruct
    CmusikPrefsSoundCrossfader();
    virtual ~CmusikPrefsSoundCrossfader();
    
    // from CmusikPropertyPage
    virtual void CommitChanges();
    virtual void LoadPrefs();

    // mfc vars and messages
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    afx_msg void OnBnClickedReset();
    afx_msg void OnBnClickedAdd();
    afx_msg void OnBnClickedDelete();
    afx_msg void OnLbnSelchangePresetbox();

    CEdit m_NewSong;
    CEdit m_PauseResume;
    CEdit m_Stop;
    CEdit m_Exit;
    CListBox m_PresetBox;

protected:

    // misc
    virtual void DoDataExchange(CDataExchange* pDX);
    enum { IDD = IDD_PROPPAGE_SOUND_CROSSFADER };
    void Populate(const musikCore::Crossfader& fader);
    void RevPopulate(musikCore::Crossfader& fader);
    int GetIndex();

    musikCore::IntArray m_IDs;

    // macros
    DECLARE_DYNAMIC(CmusikPrefsSoundCrossfader)
    DECLARE_MESSAGE_MAP()

};

///////////////////////////////////////////////////

// Interface::Transparency

///////////////////////////////////////////////////

class CmusikPrefsInterfaceTrans : public CmusikPropertyPage
{

public:

    // construct and destruct
    CmusikPrefsInterfaceTrans();
    virtual ~CmusikPrefsInterfaceTrans();

    // from CmusikPropertyPage
    virtual void CommitChanges();
    virtual void LoadPrefs();

    // objects
    CButton m_TransEnabled;
    CButton m_TransAdaptive;
    CEdit m_TransFocused;
    CEdit m_TransUnfocused;
    CEdit m_TransFadeDuration;

protected:

    // misc
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    enum { IDD = IDD_PROPPAGE_INTERFACE_TRANPARENCY };

    // macros
    DECLARE_DYNAMIC(CmusikPrefsInterfaceTrans)
    DECLARE_MESSAGE_MAP()

};


///////////////////////////////////////////////////

// CmusikPrefsTunage

///////////////////////////////////////////////////

class CmusikPrefsTunage : public CmusikPropertyPage
{

public:

    // construct and destruct
    CmusikPrefsTunage();
    virtual ~CmusikPrefsTunage();

    // from CmusikPropertyPage
    virtual void CommitChanges();
    virtual void LoadPrefs();

    // objects
    CButton m_Enabled;
    CButton m_PostURLEnabled;
    CEdit m_URL;
    CButton m_WriteFileEnabled;
    CEdit m_Filename;
    CEdit m_Fileline;
    CButton m_Append;
    CButton m_RunAppEnabled;
    CEdit m_CommandLine;

protected:

    // misc
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    enum { IDD = IDD_PROPPAGE_TUNAGE };

    // macros
    DECLARE_DYNAMIC(CmusikPrefsTunage)
    DECLARE_MESSAGE_MAP()

};


///////////////////////////////////////////////////

class CmusikPrefsAutoCapitalize : public CmusikPropertyPage
{
public:

    CmusikPrefsAutoCapitalize();
    virtual ~CmusikPrefsAutoCapitalize();

    virtual void LoadPrefs();

    CListBox m_ListBox;

    afx_msg void OnBnClickedAdd();
    afx_msg void OnBnClickedDelete();

protected:

    // misc
    virtual void DoDataExchange(CDataExchange* pDX);
    enum { IDD = IDD_PROPPAGE_AUTOCAPITALIZE };

    musikCore::StringArray m_Items;

    DECLARE_DYNCREATE(CmusikPrefsAutoCapitalize)
    DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

class CmusikPrefsInterfaceGeneral : public CmusikPropertyPage
{
public:

    CmusikPrefsInterfaceGeneral();
    virtual ~CmusikPrefsInterfaceGeneral();

    // overrides
    virtual void LoadPrefs();
    virtual void CommitChanges();
    afx_msg void OnBnGlobalHotkeys();

protected:

    // misc
    virtual void DoDataExchange(CDataExchange* pDX);
    enum { IDD = IDD_PROPPAGE_INTERFACE_GENERAL };
    
    // controls
    CButton m_EnableTrayIcon;
    CButton m_StartWithWindows;
    CButton m_AutoSynchronize;

    // startup
    void EnableStartup();
    void DisableStartup();
    bool GetStartup();

    DECLARE_DYNAMIC(CmusikPrefsInterfaceGeneral)
    DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

class CmusikPrefsInterfaceDevices : public CmusikPropertyPage
{
public:

    CmusikPrefsInterfaceDevices();
    virtual ~CmusikPrefsInterfaceDevices();

    // overrides
    virtual void LoadPrefs();
    virtual void CommitChanges();

    afx_msg void OnBnClickedHidedev();
    afx_msg void OnBnClickedShowdev();

    CListBox m_lstVisible;
    CListBox m_lstHidden;

protected:

    // misc
    virtual void DoDataExchange(CDataExchange* pDX);
    enum { IDD = IDD_PROPPAGE_INTERFACE_DEVICES };

    musikCore::StringArray m_Drives;

    DECLARE_DYNAMIC(CmusikPrefsInterfaceDevices)
    DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

// CmusikPrefsInternet

///////////////////////////////////////////////////

class CmusikPrefsInternet : public CmusikPropertyPage
{

public:
    CmusikPrefsInternet();
    virtual ~CmusikPrefsInternet();

    // overrides
    virtual void LoadPrefs();
    virtual void CommitChanges();

    enum { IDD = IDD_PROPPAGE_INTERNET };

protected:

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_DYNAMIC(CmusikPrefsInternet)
    DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////


