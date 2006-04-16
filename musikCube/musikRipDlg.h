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
#include "musikRip.h"
#include "afxwin.h"
#include "musikplaylistview.h"
#include "3rdparty/Mfccddb.h"
#include "afxcmn.h"

///////////////////////////////////////////////////

class CDSystem
{
public:

    static void FillWithBlanks( CmusikPlaylistCDCtrl* playlistctrl, musikCore::SongInfoArray *songs, int drive );
    static void QueryToPlaylist( CmusikPlaylistCDCtrl* playlist, musikCore::SongInfoArray *& target, int drive );
    static int GetDiscTrackCount( int drive );
};

///////////////////////////////////////////////////

class CmusikRipDlg : public CDialog
{

public:

    CmusikRipDlg( CWnd* parent, int driveid );
	CmusikRipDlg(CWnd* pParent = NULL);   // standard constructor

    CmusikPlaylistView* m_RipView;
    CmusikPlaylistCDCtrl* m_PlaylistCtrl;

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnBnClickedRefresh();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnCbnSelchangeDrivecombo();
    afx_msg void OnCbnSelchangeEncoder();
    afx_msg void OnBnClickedWritetags();
    afx_msg void OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedConfigmasks();
    afx_msg void OnBnClickedBrowsefolder();
    afx_msg void OnClose();
    afx_msg void OnBnClickedBtnEditalbum();
    afx_msg void OnBnClickedBtnEdittrack();
    afx_msg void OnBnClickedRescan();

    virtual BOOL OnInitDialog();
	virtual ~CmusikRipDlg();

protected:

	enum { IDD = IDD_RIP };
    virtual void DoDataExchange(CDataExchange* pDX);

	void RipDisc( bool rip_selected = false );

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CmusikRipDlg)

    musikCore::Rip* m_Rip;
    musikCore::RipData m_Data;
    musikCore::SongInfoArray* m_Songs;
    int m_DriveID;
    void RefreshDriveList();

    int GetDriveID();
    int m_FirstDrive;
    void SetRipState( bool ripping );

	CComboBox m_EncoderSel;
    CButton m_WriteTags;
    CProgressCtrl m_TrackProgress;
    CProgressCtrl m_TotalProgress;
    CButton m_ID3V1;
    CButton m_ID3V2;
    CButton m_RefreshBtn;
    CSliderCtrl m_SliderCtrl;
    CButton m_RipBtn;
    CButton m_Rename;
    CEdit m_Path;
    CButton m_BrowseBtn;
    CButton m_MaskBtn;
    CButton m_CancelBtn;
    CComboBox m_DriveSel;
    CButton m_EditTrack;
    CButton m_EditInfo;

    void UpdateProgress();

    class Functor : public musikCore::Functor
    {
    public:

        Functor( CmusikRipDlg* parent )
            : musikCore::Functor()
        {
            m_Parent = parent;
        }

        virtual void OnTaskProgress( size_t progress, musikCore::Task* task_addr )
        {
            m_Parent->UpdateProgress();
        }

        virtual void OnTaskEnd( musikCore::Task* addr )
        {
            musikCore::Rip* task = (musikCore::Rip*)addr;
            if ( task->m_Error )
            {
				::MessageBox(
					NULL,
					_T( "The CD ripping process was aborted." ),
					_T( "musikCube" ),
					MB_ICONWARNING | MB_OK );
            }
            else
            {
                ::MessageBox(
                    NULL,
                    _T( "The CD ripping process completed successfully" ),
                    _T( "musikCube" ),
                    MB_ICONINFORMATION | MB_OK );
            }

            m_Parent->SetRipState( false );
            m_Parent->m_TotalProgress.SetPos( 0 );
            m_Parent->m_TrackProgress.SetPos( 0 );
            m_Parent->GetDlgItem( IDC_STATIC_CURRTRACK )->SetWindowText( _T( "idle" ) );
            m_Parent->GetDlgItem( IDC_STATIC_OVERALLPERCENT )->SetWindowText( _T( "idle" ) );
        }

    private:

        CmusikRipDlg* m_Parent;
    };

    Functor* m_Functor;
    CWnd* m_Parent;
public:
	afx_msg void OnBnClickedButton1();
	CButton m_RipSelBtn;
};

///////////////////////////////////////////////////


