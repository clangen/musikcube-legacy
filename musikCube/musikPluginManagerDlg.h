///////////////////////////////////////////////////

#pragma once
#include "afxwin.h"

///////////////////////////////////////////////////

class CmusikPluginManagerDlg : public CDialog
{

public:

	CmusikPluginManagerDlg(CWnd* pParent = NULL);

	virtual ~CmusikPluginManagerDlg();
	virtual BOOL OnInitDialog();
	afx_msg void OnLbnSelchangePlugins();
	afx_msg void OnBnClickedExecute();
	afx_msg void OnBnClickedAbout();

	CEdit m_Description;
	CListBox m_PluginNames;

	void Populate();

	enum { IDD = IDD_PLUGIN_MANAGER };

protected:

	int GetIndex();

	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_DYNAMIC(CmusikPluginManagerDlg)	
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedConfigure();
	afx_msg void OnBnClickedStop();
	CButton m_ConfigureBtn;
	CButton m_ExecuteBtn;
	CButton m_StopBtn;
	CButton m_AboutBtn;
};

///////////////////////////////////////////////////

