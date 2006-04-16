///////////////////////////////////////////////////

#pragma once
#include <musikCore.h>

///////////////////////////////////////////////////

class CmusikAddNetstreamDlg : public CDialog
{

public:

	CmusikAddNetstreamDlg();
	virtual ~CmusikAddNetstreamDlg();

	enum { IDD = IDD_ADD_NETSTREAM };

	// message maps
	afx_msg void OnBnClickedOk();

protected:

	// misc
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_DYNAMIC(CmusikAddNetstreamDlg)
	DECLARE_MESSAGE_MAP()

};

///////////////////////////////////////////////////

