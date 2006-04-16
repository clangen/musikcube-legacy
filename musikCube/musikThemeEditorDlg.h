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
		m_strCustomText = _T( "More Colors..." );
	}

	virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
	{
		this->SetRedraw( FALSE );
		this->SetWindowText( ( this->GetColour() == AUTOMATIC_COLOR ) ? _T( "Auto" ) : _T( "" ) );
		this->SetRedraw( TRUE );
		CColourPicker::DrawItem( lpDrawItemStruct );
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

	afx_msg LONG OnColorChanged( UINT lParam, LONG wParam );
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
