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
	for ( size_t i = 0; i < musikCube::g_Plugins.size(); i++ )
		m_PluginNames.AddString( musikCube::g_Plugins.at( i ).GetPluginName() );

	m_ConfigureBtn.EnableWindow( false );
	m_AboutBtn.EnableWindow( false );
	m_ExecuteBtn.EnableWindow( false );
	m_StopBtn.EnableWindow( false );
}

///////////////////////////////////////////////////

void CmusikPluginManagerDlg::OnLbnSelchangePlugins()
{
	int sel = GetIndex();
	if ( sel > -1 )
	{
		m_Description.SetWindowText( 
			musikCube::g_Plugins.at( sel ).GetPluginDescription() );

		m_ConfigureBtn.EnableWindow( musikCube::g_Plugins.at( sel ).CanConfigure() );
		m_AboutBtn.EnableWindow( musikCube::g_Plugins.at( sel ).CanAbout() );
		m_ExecuteBtn.EnableWindow( musikCube::g_Plugins.at( sel ).CanExecute() );
		m_StopBtn.EnableWindow( musikCube::g_Plugins.at( sel ).CanStop() );
	}
}

///////////////////////////////////////////////////

void CmusikPluginManagerDlg::OnBnClickedExecute()
{
	int sel = GetIndex();
	if ( sel > -1 )
		musikCube::g_Plugins.at( sel ).Execute();
}

///////////////////////////////////////////////////

void CmusikPluginManagerDlg::OnBnClickedAbout()
{
	int sel = GetIndex();
	if ( sel > -1 )
		musikCube::g_Plugins.at( sel ).About();
}

///////////////////////////////////////////////////

int CmusikPluginManagerDlg::GetIndex()
{
	for ( int i = 0; i < m_PluginNames.GetCount(); i++ )
	{
		if ( m_PluginNames.GetSel( i ) )
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
	if ( sel > -1 )
		musikCube::g_Plugins.at( sel ).Configure();
}

///////////////////////////////////////////////////

void CmusikPluginManagerDlg::OnBnClickedStop()
{
	int sel = GetIndex();
	if ( sel > -1 )
		musikCube::g_Plugins.at( sel ).Stop();
}

///////////////////////////////////////////////////


