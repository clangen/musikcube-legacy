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

#include "stdafx.h"
#include "musikPrefs.h"
#include "musikCube.h"
#include "musikThemeEditorDlg.h"
#include "musikFileDialog.h"
#include ".\musikthemeeditordlg.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikThemeEditorDlg, CDialog)

///////////////////////////////////////////////////

CmusikThemeEditorDlg::CmusikThemeEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CmusikThemeEditorDlg::IDD, pParent)
{
}

///////////////////////////////////////////////////

CmusikThemeEditorDlg::~CmusikThemeEditorDlg()
{
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikThemeEditorDlg, CDialog)
	ON_MESSAGE( CPN_SELENDOK, OnColorChanged )
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_LOAD, OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_RADIO_USEDARKICONS, OnChangeButtonIconTheme)
	ON_BN_CLICKED(IDC_RADIO_USELIGHTICONS, OnChangeButtonIconTheme)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

LONG CmusikThemeEditorDlg::OnColorChanged( UINT lParam, LONG wParam )
{
	COLORREF newColor = (COLORREF)lParam;
	unsigned int senderID = (unsigned int)wParam;

	CColourPicker& colorPicker = *m_ID_ToControlMap[senderID];

	*m_ID_ToColorMap[senderID] = newColor;		
	colorPicker.SetColour( newColor );

	AfxGetApp()->GetMainWnd()->SendMessage( WM_SYSCOLORCHANGE );

	return 0L;
}

///////////////////////////////////////////////////

void CmusikThemeEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BUTTON_ACTIVECAPTION, m_ActiveCaptionPicker);
	DDX_ColourPicker(pDX, IDC_BUTTON_ACTIVECAPTION, musikCube::g_Prefs->MUSIK_COLOR_ACTIVECAPTION );

	DDX_Control(pDX, IDC_BUTTON_ACTIVECAPTIONTEXT, m_ActiveCaptionTextPicker);
	DDX_ColourPicker(pDX, IDC_BUTTON_ACTIVECAPTIONTEXT, musikCube::g_Prefs->MUSIK_COLOR_CAPTIONTEXT );
	
	DDX_Control(pDX, IDC_BUTTON_INACTIVECAPTION, m_InActiveCaptionPicker);
	DDX_ColourPicker(pDX, IDC_BUTTON_INACTIVECAPTION, musikCube::g_Prefs->MUSIK_COLOR_INACTIVECAPTION );

	DDX_Control(pDX, IDC_BUTTON_INACTIVECAPTIONTEXT, m_InActiveCaptionTextPicker);
	DDX_ColourPicker(pDX, IDC_BUTTON_INACTIVECAPTIONTEXT, musikCube::g_Prefs->MUSIK_COLOR_INACTIVECAPTIONTEXT );

	DDX_Control(pDX, IDC_BUTTON_BTNFACE, m_BtnFacePicker);
	DDX_ColourPicker(pDX, IDC_BUTTON_BTNFACE, musikCube::g_Prefs->MUSIK_COLOR_BTNFACE );

	DDX_Control(pDX, IDC_BUTTON_BTNTEXT, m_BtnTextPicker);
	DDX_ColourPicker(pDX, IDC_BUTTON_BTNTEXT, musikCube::g_Prefs->MUSIK_COLOR_BTNTEXT );

	DDX_Control(pDX, IDC_BUTTON_BTNHILIGHT, m_BtnHilightPicker);
	DDX_ColourPicker(pDX, IDC_BUTTON_BTNHILIGHT, musikCube::g_Prefs->MUSIK_COLOR_BTNHILIGHT );

	DDX_Control(pDX, IDC_BUTTON_BTNSHADOW, m_BtnShadowPicker);
	DDX_ColourPicker(pDX, IDC_BUTTON_BTNSHADOW, musikCube::g_Prefs->MUSIK_COLOR_BTNSHADOW );

	DDX_Control(pDX, IDC_BUTTON_LISTCTRL, m_ListCtrlPicker);
	DDX_ColourPicker(pDX, IDC_BUTTON_LISTCTRL, musikCube::g_Prefs->MUSIK_COLOR_LISTCTRL );

	DDX_Control(pDX, IDC_BUTTON_LISTCTRLTEXT, m_ListCtrlTextPicker);
	DDX_ColourPicker(pDX, IDC_BUTTON_LISTCTRLTEXT, musikCube::g_Prefs->MUSIK_COLOR_LISTCTRLTEXT );

	DDX_Control(pDX, IDC_BUTTON_HIGHLIGHT, m_HighLightPicker);
	DDX_ColourPicker(pDX, IDC_BUTTON_HIGHLIGHT, musikCube::g_Prefs->MUSIK_COLOR_HIGHLIGHT );

	DDX_Control(pDX, IDC_BUTTON_HIGHLIGHTTEXT, m_HighLightTextPicker);
	DDX_ColourPicker(pDX, IDC_BUTTON_HIGHLIGHTTEXT, musikCube::g_Prefs->MUSIK_COLOR_HIGHLIGHTTEXT );
}

///////////////////////////////////////////////////

int CmusikThemeEditorDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_ID_ToControlMap[IDC_BUTTON_ACTIVECAPTION] = &m_ActiveCaptionPicker;
	m_ID_ToColorMap[IDC_BUTTON_ACTIVECAPTION] = &musikCube::g_Prefs->MUSIK_COLOR_ACTIVECAPTION;

	m_ID_ToControlMap[IDC_BUTTON_ACTIVECAPTIONTEXT] = &m_ActiveCaptionTextPicker;
	m_ID_ToColorMap[IDC_BUTTON_ACTIVECAPTIONTEXT] = &musikCube::g_Prefs->MUSIK_COLOR_CAPTIONTEXT;

	m_ID_ToControlMap[IDC_BUTTON_INACTIVECAPTION] = &m_InActiveCaptionPicker;
	m_ID_ToColorMap[IDC_BUTTON_INACTIVECAPTION] = &musikCube::g_Prefs->MUSIK_COLOR_INACTIVECAPTION;

	m_ID_ToControlMap[IDC_BUTTON_INACTIVECAPTIONTEXT] = &m_InActiveCaptionTextPicker;
	m_ID_ToColorMap[IDC_BUTTON_INACTIVECAPTIONTEXT] = &musikCube::g_Prefs->MUSIK_COLOR_INACTIVECAPTIONTEXT;

	m_ID_ToControlMap[IDC_BUTTON_BTNFACE] = &m_BtnFacePicker;
	m_ID_ToColorMap[IDC_BUTTON_BTNFACE] = &musikCube::g_Prefs->MUSIK_COLOR_BTNFACE;

	m_ID_ToControlMap[IDC_BUTTON_BTNTEXT] = &m_BtnTextPicker;
	m_ID_ToColorMap[IDC_BUTTON_BTNTEXT] = &musikCube::g_Prefs->MUSIK_COLOR_BTNTEXT;

	m_ID_ToControlMap[IDC_BUTTON_BTNHILIGHT] = &m_BtnHilightPicker;
	m_ID_ToColorMap[IDC_BUTTON_BTNHILIGHT] = &musikCube::g_Prefs->MUSIK_COLOR_BTNHILIGHT;

	m_ID_ToControlMap[IDC_BUTTON_BTNSHADOW] = &m_BtnShadowPicker;
	m_ID_ToColorMap[IDC_BUTTON_BTNSHADOW] = &musikCube::g_Prefs->MUSIK_COLOR_BTNSHADOW;

	m_ID_ToControlMap[IDC_BUTTON_LISTCTRL] = &m_ListCtrlPicker;
	m_ID_ToColorMap[IDC_BUTTON_LISTCTRL] = &musikCube::g_Prefs->MUSIK_COLOR_LISTCTRL;

	m_ID_ToControlMap[IDC_BUTTON_LISTCTRLTEXT] = &m_ListCtrlTextPicker;
	m_ID_ToColorMap[IDC_BUTTON_LISTCTRLTEXT] = &musikCube::g_Prefs->MUSIK_COLOR_LISTCTRLTEXT;

	m_ID_ToControlMap[IDC_BUTTON_HIGHLIGHT] = &m_HighLightPicker;
	m_ID_ToColorMap[IDC_BUTTON_HIGHLIGHT] = &musikCube::g_Prefs->MUSIK_COLOR_HIGHLIGHT;

	m_ID_ToControlMap[IDC_BUTTON_HIGHLIGHTTEXT] = &m_HighLightTextPicker;
	m_ID_ToColorMap[IDC_BUTTON_HIGHLIGHTTEXT] = &musikCube::g_Prefs->MUSIK_COLOR_HIGHLIGHTTEXT;

	return 0;
}

///////////////////////////////////////////////////

void CmusikThemeEditorDlg::ReloadColors()
{
	((CmusikColourPicker&)*m_ID_ToControlMap[IDC_BUTTON_ACTIVECAPTION]).SetColour( (COLORREF&)*m_ID_ToColorMap[IDC_BUTTON_ACTIVECAPTION] );
	((CmusikColourPicker&)*m_ID_ToControlMap[IDC_BUTTON_ACTIVECAPTIONTEXT]).SetColour( (COLORREF&)*m_ID_ToColorMap[IDC_BUTTON_ACTIVECAPTIONTEXT] );
	((CmusikColourPicker&)*m_ID_ToControlMap[IDC_BUTTON_INACTIVECAPTION]).SetColour( (COLORREF&)*m_ID_ToColorMap[IDC_BUTTON_INACTIVECAPTION] );
	((CmusikColourPicker&)*m_ID_ToControlMap[IDC_BUTTON_INACTIVECAPTIONTEXT]).SetColour( (COLORREF&)*m_ID_ToColorMap[IDC_BUTTON_INACTIVECAPTIONTEXT] );
	((CmusikColourPicker&)*m_ID_ToControlMap[IDC_BUTTON_BTNFACE]).SetColour( (COLORREF&)*m_ID_ToColorMap[IDC_BUTTON_BTNFACE] );
	((CmusikColourPicker&)*m_ID_ToControlMap[IDC_BUTTON_BTNTEXT]).SetColour( (COLORREF&)*m_ID_ToColorMap[IDC_BUTTON_BTNTEXT] );
	((CmusikColourPicker&)*m_ID_ToControlMap[IDC_BUTTON_BTNHILIGHT]).SetColour( (COLORREF&)*m_ID_ToColorMap[IDC_BUTTON_BTNHILIGHT] );
	((CmusikColourPicker&)*m_ID_ToControlMap[IDC_BUTTON_BTNSHADOW]).SetColour( (COLORREF&)*m_ID_ToColorMap[IDC_BUTTON_BTNSHADOW] );
	((CmusikColourPicker&)*m_ID_ToControlMap[IDC_BUTTON_LISTCTRL]).SetColour( (COLORREF&)*m_ID_ToColorMap[IDC_BUTTON_LISTCTRL] );
	((CmusikColourPicker&)*m_ID_ToControlMap[IDC_BUTTON_LISTCTRLTEXT]).SetColour( (COLORREF&)*m_ID_ToColorMap[IDC_BUTTON_LISTCTRLTEXT] );
	((CmusikColourPicker&)*m_ID_ToControlMap[IDC_BUTTON_HIGHLIGHT]).SetColour( (COLORREF&)*m_ID_ToColorMap[IDC_BUTTON_HIGHLIGHT] );
	((CmusikColourPicker&)*m_ID_ToControlMap[IDC_BUTTON_HIGHLIGHTTEXT]).SetColour( (COLORREF&)*m_ID_ToColorMap[IDC_BUTTON_HIGHLIGHTTEXT] );

	if ( musikCube::g_Prefs->UseLightIconTheme() )
	{
		((CButton*)GetDlgItem( IDC_RADIO_USELIGHTICONS ))->SetCheck( 1 );
		((CButton*)GetDlgItem( IDC_RADIO_USEDARKICONS ))->SetCheck( 0 );
	}
	else
	{
		((CButton*)GetDlgItem( IDC_RADIO_USEDARKICONS ))->SetCheck( 1 );
		((CButton*)GetDlgItem( IDC_RADIO_USELIGHTICONS ))->SetCheck( 0 );
	}
}

///////////////////////////////////////////////////

void CmusikThemeEditorDlg::OnBnClickedButtonLoad()
{
	CString filter = _T( "musikCube Theme Files (*.ini)|*.ini||" );
	
	CmusikFileDialog ofdlg( 
		TRUE, 
        _T( "ini" ), 
        NULL, 
		OFN_EXPLORER | OFN_FILEMUSTEXIST, 
		filter
    );

	if ( ofdlg.DoModal() == IDOK )
	{
		std::string fn = musikCore::utf16to8( ofdlg.GetFileName().GetBuffer(), false );
		CIniFile iniFile( fn );

		if ( !iniFile.ReadFile() )
		{
			MessageBox(
				_T( "Failed to read theme file from disk.  Invalid access rights?" ),
				_T( "musikCube" ),
				MB_OK | MB_ICONERROR
			);

			return;
		}

		musikCube::g_Prefs->MUSIK_COLOR_ACTIVECAPTION = 
			StringToCOLORREF( iniFile.GetValue( "Theme", "Active Caption", COLORREFToString( AUTOMATIC_COLOR ) ) );
		musikCube::g_Prefs->MUSIK_COLOR_CAPTIONTEXT = 
			StringToCOLORREF( iniFile.GetValue( "Theme", "Active Caption Text", COLORREFToString( AUTOMATIC_COLOR ) ) );
		musikCube::g_Prefs->MUSIK_COLOR_INACTIVECAPTION = 
			StringToCOLORREF( iniFile.GetValue( "Theme", "Inactive Caption", COLORREFToString( AUTOMATIC_COLOR ) ) );
		musikCube::g_Prefs->MUSIK_COLOR_INACTIVECAPTIONTEXT = 
			StringToCOLORREF( iniFile.GetValue( "Theme", "Inactive Caption Text", COLORREFToString( AUTOMATIC_COLOR ) ) );
		musikCube::g_Prefs->MUSIK_COLOR_BTNFACE = 
			StringToCOLORREF( iniFile.GetValue( "Theme", "Button Face", COLORREFToString( AUTOMATIC_COLOR ) ) );
		musikCube::g_Prefs->MUSIK_COLOR_BTNTEXT = 
			StringToCOLORREF( iniFile.GetValue( "Theme", "Button Text", COLORREFToString( AUTOMATIC_COLOR ) ) );
		musikCube::g_Prefs->MUSIK_COLOR_BTNHILIGHT = 
			StringToCOLORREF( iniFile.GetValue( "Theme", "Button Hilight", COLORREFToString( AUTOMATIC_COLOR ) ) );
		musikCube::g_Prefs->MUSIK_COLOR_BTNSHADOW = 
			StringToCOLORREF( iniFile.GetValue( "Theme", "Button Shadow", COLORREFToString( AUTOMATIC_COLOR ) ) );
		musikCube::g_Prefs->MUSIK_COLOR_LISTCTRL = 
			StringToCOLORREF( iniFile.GetValue( "Theme", "List", COLORREFToString( AUTOMATIC_COLOR ) ) );
		musikCube::g_Prefs->MUSIK_COLOR_LISTCTRLTEXT = 
			StringToCOLORREF( iniFile.GetValue( "Theme", "List Text", COLORREFToString( AUTOMATIC_COLOR ) ) );
		musikCube::g_Prefs->MUSIK_COLOR_HIGHLIGHT = 
			StringToCOLORREF( iniFile.GetValue( "Theme", "Highlight", COLORREFToString( AUTOMATIC_COLOR ) ) );
		musikCube::g_Prefs->MUSIK_COLOR_HIGHLIGHTTEXT = 
			StringToCOLORREF( iniFile.GetValue( "Theme", "Highlight Text", COLORREFToString( AUTOMATIC_COLOR ) ) );
		musikCube::g_Prefs->SetUseLightIconTheme(
			StringToBool( iniFile.GetValue( "Theme", "Use Light Icons", BoolToString( false ) ) ) );

		this->ReloadColors();
		AfxGetApp()->GetMainWnd()->SendMessage( WM_SYSCOLORCHANGE );
	}
}

///////////////////////////////////////////////////

void CmusikThemeEditorDlg::OnBnClickedOk()
{
	OnOK();
}

///////////////////////////////////////////////////

void CmusikThemeEditorDlg::OnBnClickedButtonSave()
{
	CString filter = _T( "musikCube Theme Files (*.ini)|*.ini||" );
	
	CmusikFileDialog sfdlg( 
		FALSE, 
        _T( "ini" ), 
        NULL, 
		OFN_EXPLORER | OFN_OVERWRITEPROMPT, 
		filter
    );

	if ( sfdlg.DoModal() == IDOK )
	{
		CIniFile iniFile( musikCore::utf16to8( sfdlg.GetFileName().GetBuffer(), false ) );

		iniFile.SetValue( 
			"Theme", "Active Caption", COLORREFToString( musikCube::g_Prefs->MUSIK_COLOR_ACTIVECAPTION ) );
		iniFile.SetValue( 
			"Theme", "Active Caption Text", COLORREFToString( musikCube::g_Prefs->MUSIK_COLOR_CAPTIONTEXT ) );
		iniFile.SetValue( 
			"Theme", "Inactive Caption", COLORREFToString( musikCube::g_Prefs->MUSIK_COLOR_INACTIVECAPTION ) );
		iniFile.SetValue( 
			"Theme", "Inactive Caption Text", COLORREFToString( musikCube::g_Prefs->MUSIK_COLOR_INACTIVECAPTIONTEXT ) );
		iniFile.SetValue( 
			"Theme", "Button Face", COLORREFToString( musikCube::g_Prefs->MUSIK_COLOR_BTNFACE ) );
		iniFile.SetValue( 
			"Theme", "Button Text", COLORREFToString( musikCube::g_Prefs->MUSIK_COLOR_BTNTEXT ) );
		iniFile.SetValue( 
			"Theme", "Button Hilight", COLORREFToString( musikCube::g_Prefs->MUSIK_COLOR_BTNHILIGHT ) );
		iniFile.SetValue( 
			"Theme", "Button Shadow", COLORREFToString( musikCube::g_Prefs->MUSIK_COLOR_BTNSHADOW ) );
		iniFile.SetValue( 
			"Theme", "List", COLORREFToString( musikCube::g_Prefs->MUSIK_COLOR_LISTCTRL ) );
		iniFile.SetValue( 
			"Theme", "List Text", COLORREFToString( musikCube::g_Prefs->MUSIK_COLOR_LISTCTRLTEXT ) );
		iniFile.SetValue( 
			"Theme", "Highlight", COLORREFToString( musikCube::g_Prefs->MUSIK_COLOR_HIGHLIGHT ) );
		iniFile.SetValue( 
			"Theme", "Highlight Text", COLORREFToString( musikCube::g_Prefs->MUSIK_COLOR_HIGHLIGHTTEXT ) );
		iniFile.SetValue(
			"Theme", "Use Light Icons", BoolToString( musikCube::g_Prefs->UseLightIconTheme() ) );

		if ( !iniFile.WriteFile() )
		{
			MessageBox(
				_T( "Failed to write theme file to disk.  Invalid access rights?" ),
				_T( "musikCube" ),
				MB_OK | MB_ICONERROR
			);
		}
	}
}

///////////////////////////////////////////////////

void CmusikThemeEditorDlg::OnChangeButtonIconTheme()
{
	int useLightIcons = ((CButton*)GetDlgItem( IDC_RADIO_USELIGHTICONS ))->GetCheck();
	musikCube::g_Prefs->SetUseLightIconTheme( useLightIcons == NULL ? false : true );

	AfxGetApp()->GetMainWnd()->SendMessage( WM_SYSCOLORCHANGE );
}

///////////////////////////////////////////////////

BOOL CmusikThemeEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	ReloadColors();

	return TRUE;
}

///////////////////////////////////////////////////


