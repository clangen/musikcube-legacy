///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
//   multimedia library. More information at:
//
//     http://musik.berlios.de
//     http://musik.sourceforge.net
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2002-2004
//   Casey Langen   : Lead Developer, Project Manager. E-Mail: casey at musikcube dot com
//   Dustin Carter  : Developer
//   Simon Windmill : Developer
//
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////
//
// Class(es): 
//
//   CmusikPrefsInterfaceWorkflow
//
// Filename(s): 
//
//   musikPrefsDlg.h, musikPrefsDlg.cpp
//
// Information:
//
//   All the stuff relating to preferences.
// 
// Usage: 
//
//   Void.
//
// Some code adapated / borrowed from...
//
//   http://www.codeproject.com/shell/create_shortcut.asp?target=shortcut (create shortcut)
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "afxwin.h"
#include "musikCube.h"
#include "musikPropertyPage.h"
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

	// overrides
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	// macros
	DECLARE_DYNAMIC(CmusikPrefsInterfaceWorkflow)
	DECLARE_MESSAGE_MAP()

protected:

	// misc
	virtual void DoDataExchange( CDataExchange* pDX ); 

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

	// overrides
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	// macros
	DECLARE_DYNAMIC( CmusikPrefsSoundDriver )
	DECLARE_MESSAGE_MAP()

protected:

	// misc
	virtual void DoDataExchange(CDataExchange* pDX);

	// for getting sound devices
	void GetSoundDrivers( bool populate = true );
	void GetSoundPlaybackRates( bool populate = true );

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
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
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
	void Populate( const musikCore::Crossfader& fader );
	void RevPopulate( musikCore::Crossfader& fader );
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

	// overrides
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

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


	// overrides
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

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
	virtual BOOL OnInitDialog();

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
    virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    afx_msg void OnBnClickedButton1();

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
    virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

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
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	enum { IDD = IDD_PROPPAGE_INTERNET };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_DYNAMIC(CmusikPrefsInternet)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////


