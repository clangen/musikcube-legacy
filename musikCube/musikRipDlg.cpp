///////////////////////////////////////////////////
//
// License Agreement:
//
// The following are Copyright � 2002-2006, Casey Langen
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

#include "stdafx.h"
#include "musikCube.h"
#include "musikRipDlg.h"
#include "musikMaskDlg.h"

///////////////////////////////////////////////////

#pragma warning(disable : 4800)

///////////////////////////////////////////////////

int CDSystem::GetDiscTrackCount(int drive)
{
    return (int)BASS_CD_GetTracks(drive);
}

///////////////////////////////////////////////////

void CDSystem::FillWithBlanks(CmusikPlaylistCDCtrl* playlistctrl, musikCore::SongInfoArray *songs, int drive)
{
    songs->clear();
    int tracks = (int)BASS_CD_GetTracks(drive);
    for (int i = 0; i < tracks; i++)
    {
        musikCore::SongInfo info;
        info.SetTitle(_T("track number ") + ItoS(i + 1));
        info.SetTrackNum(IntToString(i + 1));

        DWORD length = BASS_CD_GetTrackLength(drive, i);
        float secs = (float)(length / (DWORD)176400);
        int msecs = (int)(secs * 1000);
        info.SetDuration(IntToString(msecs));

        info.SetArtist("unknown");
        info.SetAlbum("unknown");
        info.SetFormat(MUSIK_LIBRARY_FORMAT_CDROM);

        songs->push_back(info);
    }

    if (playlistctrl)
        playlistctrl->Refresh();
}

///////////////////////////////////////////////////

void CDSystem::QueryToPlaylist(CmusikPlaylistCDCtrl* playlist, musikCore::SongInfoArray*& target, int driveid)
{
    CArray<CCDDBQueryResult, CCDDBQueryResult&> m_Results;
    CCDDBRecord m_Record;
    CCDDB cddb;

    cddb.SetTimeout(30000);

    CStringArray drives;
    cddb.GetCDROMDrives(drives);

    DWORD dwDiscID;
    cddb.ComputeDiscID(dwDiscID, drives.ElementAt(driveid));

    int track_count = CDSystem::GetDiscTrackCount(driveid);

    musikCore::SongInfoArray* songs = new musikCore::SongInfoArray();

    AfxSocketInit();

    CCDDBSite connection(TRUE);
    connection.m_sSite = musikCore::utf8to16(musikCube::g_Prefs->GetFreeDBServer()).c_str();
    CCDDBStatus status;
    BOOL bSuccess = cddb.Status(connection, status);
    if (bSuccess)     // connection to freedb established
    {
        CArray<CCDDBTrackPosition, CCDDBTrackPosition&> tracks;
        if (cddb.GetTrackPositions(tracks, drives.ElementAt(driveid))) // got tracks
        {
            bool correct_track_count = false;
            cddb.Query(connection, dwDiscID, tracks, m_Results); // got matching cd
            if (m_Results.GetSize())
            {
                for(int j = 0; j < m_Results.GetSize(); j++)
                {
                    CCDDBQueryResult& result = m_Results.ElementAt(j);
                    bool success = cddb.Read(connection, result.m_dwDiscID, result.m_sCategory, m_Record);
                    if (success)
                    {
                        if (m_Record.m_TrackTitles.GetSize() != track_count)
                            continue;
                        else
                        {
                            correct_track_count = true;
                            for (int i = 0; i < m_Record.m_TrackTitles.GetSize(); i++)
                            {
                                musikCore::SongInfo info;
                                info.SetArtist(musikCore::String(result.m_sArtist));
                                info.SetAlbum(musikCore::String(result.m_sTitle));
                                info.SetGenre(musikCore::String(result.m_sCategory));
                                info.SetYear(IntToString(m_Record.m_nYear));
                                info.SetTitle(musikCore::String (m_Record.m_TrackTitles.ElementAt(i)));
                                info.SetTrackNum(IntToString(i + 1));

                                DWORD length = BASS_CD_GetTrackLength(driveid, i);
                                float secs = (float)(length / (DWORD)176400);
                                int msecs = (int)(secs * 1000);
                                info.SetDuration(IntToString(msecs));

                                songs->push_back(info);
                            }
                            goto gottracks;
                        }
                    }

                }
            }
            else
            {
                MessageBox(
                    NULL,
                    _T("No matching CD Audio information was found on the server."),
                    _T("musikCube"),
                    MB_ICONINFORMATION | MB_OK);
                AfxSocketTerm();
                return;
            }
        }
        else
        {
                MessageBox(
                    NULL,
                    _T("Unable to retrieve valid track information from the server."),
                    _T("musikCube"),
                    MB_ICONINFORMATION | MB_OK);
                AfxSocketTerm();
                return;
        }
    }
    else
    {
        MessageBox(
            NULL,
            connection.m_sSite + _T(" does not appear to be responding to the request. Please try again later."),
            _T("musikCube"),
            MB_ICONWARNING | MB_OK);
        AfxSocketTerm();
        return;
    }

gottracks:

    AfxSocketTerm();
    if (!target->size())
    {
        MessageBox(
            NULL,
            _T("Sorry, no accurate information about this cd could be retrieved from the server."),
            _T("musikCube"),
            MB_ICONWARNING | MB_OK);
    }
    else if (playlist)
    {
        playlist->SetSongInfoArray(songs);
        playlist->Refresh();
        target = songs;
        return;
    }
    else
        delete songs;

}

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikRipDlg, CDialog)
CmusikRipDlg::CmusikRipDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CmusikRipDlg::IDD, pParent)
{
    m_Functor = new Functor(this);
    m_Rip = NULL;
    m_DriveID = -1;
    m_Parent = pParent;
}

///////////////////////////////////////////////////

CmusikRipDlg::CmusikRipDlg(CWnd* parent, int driveid)
    : CDialog(CmusikRipDlg::IDD, parent)
{
    m_Functor = new Functor(this);
    m_Rip = NULL;
    m_DriveID = driveid;
    m_Parent = parent;
}

///////////////////////////////////////////////////

CmusikRipDlg::~CmusikRipDlg()
{
    delete m_Functor;
    delete m_RipView;
    if (m_Rip)
        delete m_Rip;
}

///////////////////////////////////////////////////

void CmusikRipDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_DRIVECOMBO, m_DriveSel);
    DDX_Control(pDX, IDC_ENCODER, m_EncoderSel);
    DDX_Control(pDX, IDC_WRITETAGS, m_WriteTags);
    DDX_Control(pDX, IDC_PROGRESS1, m_TrackProgress);
    DDX_Control(pDX, IDC_PROGRESS2, m_TotalProgress);
    DDX_Control(pDX, IDC_CHECK4, m_ID3V1);
    DDX_Control(pDX, IDC_CHECK7, m_ID3V2);
    DDX_Control(pDX, IDC_REFRESH, m_RefreshBtn);
    DDX_Control(pDX, IDC_SLIDER1, m_SliderCtrl);
    DDX_Control(pDX, IDOK, m_RipBtn);
    DDX_Control(pDX, IDC_CHECK1, m_Rename);
    DDX_Control(pDX, IDC_EDIT1, m_Path);
    DDX_Control(pDX, IDC_BROWSEFOLDER, m_BrowseBtn);
    DDX_Control(pDX, IDC_CONFIGMASKS, m_MaskBtn);
    DDX_Control(pDX, IDCANCEL, m_CancelBtn);
    DDX_Control(pDX, IDC_BTN_EDITTRACK, m_EditTrack);
    DDX_Control(pDX, IDC_BTN_EDITALBUM, m_EditInfo);
    DDX_Control(pDX, IDC_BUTTON1, m_RipSelBtn);
    DDX_Control(pDX, IDC_RESCAN, m_RescanBtn);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikRipDlg, CDialog)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
    ON_WM_CREATE()
    ON_BN_CLICKED(IDC_REFRESH, OnBnClickedRefresh)
    ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
    ON_CBN_SELCHANGE(IDC_DRIVECOMBO, OnCbnSelchangeDrivecombo)
    ON_CBN_SELCHANGE(IDC_ENCODER, OnCbnSelchangeEncoder)
    ON_BN_CLICKED(IDC_WRITETAGS, OnBnClickedWritetags)
    ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, OnNMReleasedcaptureSlider1)
    ON_BN_CLICKED(IDC_CONFIGMASKS, OnBnClickedConfigmasks)
    ON_BN_CLICKED(IDC_BROWSEFOLDER, OnBnClickedBrowsefolder)
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_BTN_EDITALBUM, OnBnClickedBtnEditalbum)
    ON_BN_CLICKED(IDC_BTN_EDITTRACK, OnBnClickedBtnEdittrack)
    ON_BN_CLICKED(IDC_RESCAN, OnBnClickedRescan)
    ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikRipDlg::RefreshDriveList()
{
    CString str;
    int count = 0;

    BASS_CD_INFO cdInfo = { 0 };
    BOOL result = BASS_CD_GetInfo(count, &cdInfo);

    while (result)
    {
        int drive = (int)'A';
        drive += (int)(cdInfo.letter);

        str = _T("(");
        str += (wchar_t)drive;
        str += _T(":) ");
        str += cdInfo.product;
        m_DriveSel.AddString(str);

        count++;
        result = BASS_CD_GetInfo(count, &cdInfo);
    }
}

///////////////////////////////////////////////////

void CmusikRipDlg::SetRipState(bool ripping)
{
    m_EncoderSel.EnableWindow(!ripping);
    m_DriveSel.EnableWindow(!ripping);
    m_SliderCtrl.EnableWindow(!ripping);
    m_PlaylistCtrl->EnableWindow(!ripping);
    m_RipBtn.EnableWindow(!ripping);
    m_RefreshBtn.EnableWindow(!ripping);
    m_Path.EnableWindow(!ripping);
    m_BrowseBtn.EnableWindow(!ripping);
    m_MaskBtn.EnableWindow(!ripping);
    m_WriteTags.EnableWindow(!ripping);
    m_Rename.EnableWindow(!ripping);
    m_EditTrack.EnableWindow(!ripping);
    m_EditInfo.EnableWindow(!ripping);
    m_RipSelBtn.EnableWindow(!ripping);
    m_RescanBtn.EnableWindow(!ripping);

    if (ripping)
    {
        m_CancelBtn.SetWindowText(_T("Cancel"));
        m_ID3V1.EnableWindow(false);
        m_ID3V2.EnableWindow(false);
    }
    else
    {
        m_CancelBtn.SetWindowText(_T("Close"));
        OnBnClickedWritetags(); // updates id3 options state
    }
}

///////////////////////////////////////////////////

void CmusikRipDlg::OnBnClickedOk()
{
    RipDisc(false);
}

///////////////////////////////////////////////////

void CmusikRipDlg::RipDisc(bool rip_selected)
{
    m_DriveID = m_DriveSel.GetCurSel();    // make sure we know which drive is selected

    if (musikCube::g_Player->IsPlaying() &&
         musikCube::g_Player->GetPlayerType() == musikCore::MUSIK_PLAYER_CDROM &&
         m_DriveID == musikCube::g_Player->GetDiscDrive())
    {
        MessageBox(
            _T("musikCube cannot rip a cd that is currently playing."),
            _T("musikCube"),
            MB_ICONWARNING);
        return;
    }

    if (!m_Songs->size())
    {
        MessageBox(
            _T("No tracks selected to rip."),
            _T("musikCube"),
            MB_ICONWARNING);
        return;
    }

    if (CDSystem::GetDiscTrackCount(m_DriveID) != m_Songs->size())
    {
        MessageBox(
            _T("The disc currently inserted does not match the list of tracks above. ")
            _T("To correct this error, please press the \"rescan\" button."),
            _T("musikCube"),
            MB_ICONWARNING);

        return;
    }

    if (m_Rip)
        delete m_Rip;

    m_Rip = new musikCore::Rip();

    CString path;
    m_Path.GetWindowText(path);

    if (path.IsEmpty())
    {
        MessageBox(
            _T("Please specify a valid output directory"),
            _T("musikCube"),
            MB_OK | MB_ICONWARNING);
        return;
    }

    if (rip_selected)
    {
        if (m_PlaylistCtrl->GetSelectedCount() == 0)
        {
            if (MessageBox(
                _T("No tracks are selected, rip entire CD?"),
                _T("musikCube"),
                MB_ICONINFORMATION | MB_YESNO) == IDNO)
            {
                return;
            }

            m_Data.m_SelectAllTracks = true;
        }
        else
        {
            m_Data.m_SelectAllTracks = false;
            m_Data.m_Tracks.clear();
            m_PlaylistCtrl->GetSelectedItems(m_Data.m_Tracks);
        }
    }
    else
        m_Data.m_SelectAllTracks = true;

    if (path.Right(1) != _T("\\"))
        path += _T("\\");

    m_Data.m_Path = path;
    m_Data.m_Format = m_EncoderSel.GetCurSel();
    m_Data.m_DriveID = m_DriveSel.GetCurSel();
    m_Data.m_Songs = m_Songs;
    m_Data.m_Functor = m_Functor;
    m_Data.m_WriteTags = m_WriteTags.GetCheck() == 0 ? false : true;
    m_Data.m_ID3V1 = m_ID3V1.GetCheck() == 0 ? false : true;
    m_Data.m_ID3V2 = m_ID3V2.GetCheck() == 0 ? false : true;
    m_Data.m_Library = musikCube::g_Library;

    switch (m_EncoderSel.GetCurSel())
    {
    case 0: // ogg
    case 1: // flac
        m_Data.m_Quality = m_SliderCtrl.GetPos();
        break;
    case 2: // mp3
        m_Data.m_Quality = m_SliderCtrl.GetPos() * 32;
        break;
    }

    if (m_Rename.GetCheck())
    {
        m_Data.m_Mask = musikCube::g_Prefs->GetTagToFilename().c_str();
    }

    SetRipState(true);
    m_Rip->SetParams(&m_Data);
    m_Rip->start();
}

///////////////////////////////////////////////////

BOOL CmusikRipDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_WriteTags.SetCheck(musikCube::g_Prefs->GetRipWriteTag());
    m_ID3V1.SetCheck(musikCube::g_Prefs->GetRipID3V1());
    m_ID3V2.SetCheck(musikCube::g_Prefs->GetRipID3V2());
    m_Rename.SetCheck(musikCube::g_Prefs->GetRipRename());
    m_Path.SetWindowText(CString(musikCube::g_Prefs->GetRipPath().c_str()));

    m_EncoderSel.AddString(_T("OGG Vorbis"));
    m_EncoderSel.AddString(_T("Free Lossless Audio Codec (FLAC)"));
    m_EncoderSel.AddString(_T("LAME MP3"));
    m_EncoderSel.SetCurSel(musikCube::g_Prefs->GetRipEnc());
    OnCbnSelchangeEncoder();

    int quality = musikCube::g_Prefs->GetRipQuality();
    m_SliderCtrl.SetPos(quality);
    switch (m_EncoderSel.GetCurSel())
    {
    case 0: // ogg
    case 1: // flac
        GetDlgItem(IDC_SLIDERVAL)->SetWindowText(ItoS(quality).c_str());
        break;
    case 2: // mp3
        GetDlgItem(IDC_SLIDERVAL)->SetWindowText(ItoS(quality * 32).c_str());
        break;
    };
    m_TrackProgress.SetRange(0, 100);
    m_TotalProgress.SetRange(0, 100);

    RefreshDriveList();
    if (m_DriveID != -1 && m_DriveID < m_DriveSel.GetCount())
        m_DriveSel.SetCurSel(m_DriveID);
    else
        m_DriveSel.SetCurSel(musikCube::g_Prefs->GetRipDrive());

    if (musikCube::g_Player->GetDiscDrive() == m_DriveID && musikCube::g_Player->GetCDInfo()->size())
    {
        *m_Songs = *musikCube::g_Player->GetCDInfo();
        m_PlaylistCtrl->Refresh();
    }
    else
        CDSystem::FillWithBlanks(m_PlaylistCtrl, m_Songs, m_DriveSel.GetCurSel());

    return TRUE;
}

///////////////////////////////////////////////////

int CmusikRipDlg::GetDriveID()
{
    int ret = m_DriveSel.GetCurSel();
    if (ret < 0)
        ret = 0;

    return ret;
}

///////////////////////////////////////////////////

int CmusikRipDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDialog::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_Songs = new musikCore::SongInfoArray();

    m_RipView = new CmusikPlaylistView((CFrameWnd*)this, NULL, NULL, NULL, NULL, NULL, false, PLAYLISTCTRL_CDRIPPER);
    m_RipView->Create(NULL, NULL, NULL, CRect(6, 16, 380, 306), this, 12341, NULL);
    m_RipView->ShowWindow(SW_SHOWNORMAL);
    m_RipView->SetDrawLineAtBottom(false);

    m_RipView->GetCtrl()->ModifyStyle(NULL, WS_BORDER);

    m_PlaylistCtrl = (CmusikPlaylistCDCtrl*)m_RipView->GetCtrl();
    m_PlaylistCtrl->SetSongInfoArray(m_Songs);
    m_PlaylistCtrl->m_CanPlay = false;

    return 0;
}

///////////////////////////////////////////////////

void CmusikRipDlg::OnBnClickedRefresh()
{
    if (!m_Songs->size())
        return;

    m_RefreshBtn.SetWindowText(_T("Querying..."));
    m_RefreshBtn.EnableWindow(FALSE);
    CDSystem::QueryToPlaylist(m_PlaylistCtrl, m_Songs, m_DriveSel.GetCurSel());

    if (m_Songs->size() == 0)
        CDSystem::FillWithBlanks(m_PlaylistCtrl, m_Songs, m_DriveSel.GetCurSel());

    m_RefreshBtn.EnableWindow(TRUE);
    m_RefreshBtn.SetWindowText(_T("Lookup CD"));
}

///////////////////////////////////////////////////

void CmusikRipDlg::UpdateProgress()
{
    int overall_prog = (( m_Data.m_NumComplete * 100) / m_Data.m_Tracks.size()) + (m_Data.m_Progress / m_Data.m_Tracks.size());
    m_TrackProgress.SetPos(m_Data.m_Progress);
    m_TotalProgress.SetPos(overall_prog);

    CString str;
    str.Format(_T("%d%%"), overall_prog);
    GetDlgItem(IDC_STATIC_OVERALLPERCENT)->SetWindowText(str);

    str.Format(_T("%d/%d"), m_Data.m_NumComplete + 1, m_Data.m_Tracks.size());
    GetDlgItem(IDC_STATIC_CURRTRACK)->SetWindowText(str);
}

///////////////////////////////////////////////////


void CmusikRipDlg::OnBnClickedCancel()
{
    if (m_Rip && m_Rip->IsActive())
    {
        m_Rip->StopWait(10);
        m_TotalProgress.SetPos(0);
        m_TrackProgress.SetPos(0);
    }
    else
    {

        OnClose();
    }
}

///////////////////////////////////////////////////

void CmusikRipDlg::OnCbnSelchangeDrivecombo()
{
    CDSystem::FillWithBlanks(m_PlaylistCtrl, m_Songs, m_DriveSel.GetCurSel());
}

///////////////////////////////////////////////////

void CmusikRipDlg::OnCbnSelchangeEncoder()
{
    if (m_WriteTags.GetCheck() && m_EncoderSel.GetCurSel() == 2)  // LAME
    {
        m_ID3V1.EnableWindow(TRUE);
        m_ID3V2.EnableWindow(TRUE);
    }
    else
    {
        m_ID3V1.EnableWindow(FALSE);
        m_ID3V2.EnableWindow(FALSE);
    }

    int sel = m_EncoderSel.GetCurSel();
    switch (sel)
    {
    case 0: // ogg
        GetDlgItem(IDC_SLIDERTXT)->SetWindowText(_T("Quality:"));
        GetDlgItem(IDC_SLIDERVAL)->SetWindowText(_T("6"));
        m_SliderCtrl.SetRange(0, 10);
        m_SliderCtrl.SetTicFreq(1);
        m_SliderCtrl.SetPos(6);
        break;
    case 1: // flac
        GetDlgItem(IDC_SLIDERTXT)->SetWindowText(_T("Compression:"));
        GetDlgItem(IDC_SLIDERVAL)->SetWindowText(_T("8"));
        m_SliderCtrl.SetRange(0, 8);
        m_SliderCtrl.SetTicFreq(1);
        m_SliderCtrl.SetPos(8);
        break;
    case 2: // mp3
        GetDlgItem(IDC_SLIDERTXT)->SetWindowText(_T("Bitrate:"));
        GetDlgItem(IDC_SLIDERVAL)->SetWindowText(_T("128"));
        m_SliderCtrl.SetRange(0, 10);
        m_SliderCtrl.SetTicFreq(1);
        m_SliderCtrl.SetPos(4);
        break;
    }
}

///////////////////////////////////////////////////

void CmusikRipDlg::OnBnClickedWritetags()
{
    if (m_WriteTags.GetCheck() && m_EncoderSel.GetCurSel() == 2)  // lame
    {
        m_ID3V1.EnableWindow(TRUE);
        m_ID3V2.EnableWindow(TRUE);
        return;
    }

    m_ID3V1.EnableWindow(FALSE);
    m_ID3V2.EnableWindow(FALSE);
}

///////////////////////////////////////////////////

void CmusikRipDlg::OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
    switch (m_EncoderSel.GetCurSel())
    {
    case 0: // ogg
    case 1: // flac
        GetDlgItem(IDC_SLIDERVAL)->SetWindowText(ItoS(m_SliderCtrl.GetPos()).c_str());
        break;
    case 2: // mp3
        GetDlgItem(IDC_SLIDERVAL)->SetWindowText(ItoS(m_SliderCtrl.GetPos() * 32).c_str());
        break;
    }
    *pResult = 0;
}

///////////////////////////////////////////////////

void CmusikRipDlg::OnBnClickedConfigmasks()
{
    CmusikMaskDlg dlg(NULL);
    dlg.DoModal();
}

///////////////////////////////////////////////////

void CmusikRipDlg::OnBnClickedBrowsefolder()
{
    TCHAR path[MAX_PATH];
    BROWSEINFO bi = { 0 };
    bi.lpszTitle = _T("Please select a target directory.");
    bi.hwndOwner = GetSafeHwnd();
    bi.ulFlags |= BIF_NEWDIALOGSTYLE;
    LPITEMIDLIST pidl = SHBrowseForFolder (&bi);

    if (pidl != 0)
    {
        // get the name of the folder and put it in path
        SHGetPathFromIDList (pidl, path);

        // free memory used
        IMalloc * imalloc = 0;
        if (SUCCEEDED(SHGetMalloc (&imalloc)))
        {
            imalloc->Free (pidl);
            imalloc->Release ();
        }

        m_Path.SetWindowText(path);
    }
}

///////////////////////////////////////////////////

void CmusikRipDlg::OnClose()
{
    musikCube::g_Prefs->SetRipWriteTag(m_WriteTags.GetCheck());
    musikCube::g_Prefs->SetRipID3V2(m_ID3V2.GetCheck());
    musikCube::g_Prefs->SetRipID3V1(m_ID3V1.GetCheck());
    musikCube::g_Prefs->SetRipRename(m_Rename.GetCheck());
    musikCube::g_Prefs->SetRipEnc(m_EncoderSel.GetCurSel());
    musikCube::g_Prefs->SetRipDrive(m_DriveSel.GetCurSel());
    musikCube::g_Prefs->SetRipQuality(m_SliderCtrl.GetPos());
    CString path;
    m_Path.GetWindowText(path);
    musikCube::g_Prefs->SetRipPath(path.GetBuffer());

    OnCancel();     // no parent, destroy ourself
}

///////////////////////////////////////////////////

void CmusikRipDlg::OnBnClickedBtnEditalbum()
{
    if (m_Songs->size())
        m_PlaylistCtrl->OnEditAlbumInfo();
}

///////////////////////////////////////////////////

void CmusikRipDlg::OnBnClickedBtnEdittrack()
{

    if (m_Songs->size())
        m_PlaylistCtrl->OnEditTrackInfo();
}

///////////////////////////////////////////////////

void CmusikRipDlg::OnBnClickedRescan()
{
    CDSystem::FillWithBlanks(m_PlaylistCtrl, m_Songs, m_DriveSel.GetCurSel());
}

///////////////////////////////////////////////////

void CmusikRipDlg::OnBnClickedButton1()
{
    RipDisc(true);
}

///////////////////////////////////////////////////



