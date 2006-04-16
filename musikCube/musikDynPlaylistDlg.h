///////////////////////////////////////////////////

#pragma once
#include <musikCore.h>
#include "afxwin.h"

///////////////////////////////////////////////////

class CmusikDynPlaylistDlg : public CDialog
{
public:

	// construct / destruct
	CmusikDynPlaylistDlg(CWnd* pParent = NULL);   // standard constructor
    CmusikDynPlaylistDlg(CString& name);
	virtual ~CmusikDynPlaylistDlg();

	// message maps
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	// gets
	musikCore::String GetName(){ return musikCore::String( m_Name ); }
	musikCore::String GetQuery(){ return musikCore::String( m_Query ); }
	void SetName( musikCore::String& name ){ m_Name = name; }
	void SetQuery( musikCore::String& query ){ m_Query = query; }
	virtual BOOL OnInitDialog();
    CEdit m_EditName;
    CEdit m_EditQuery;

	// dialog data
	enum { IDD = IDD_DYNPLAYLISTEDITOR };

protected:

	// vars
	CString m_Name;
	CString m_Query;

	// misc
	virtual void DoDataExchange(CDataExchange* pDX);

	// macros
	DECLARE_DYNAMIC(CmusikDynPlaylistDlg)
	DECLARE_MESSAGE_MAP()

};

///////////////////////////////////////////////////

