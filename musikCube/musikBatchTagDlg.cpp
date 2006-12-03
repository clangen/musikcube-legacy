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

#include "stdafx.h"
#include "musikCube.h"
#include "musikBatchTagDlg.h"
#include "musikGenres.h"
#include ".\musikbatchtagdlg.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikBatchTagDlg, CDialog)

///////////////////////////////////////////////////

CmusikBatchTagDlg::CmusikBatchTagDlg(CWnd* parent, musikCore::SongInfoArray* songs)
    : CDialog(CmusikBatchTagDlg::IDD, parent)
{
    m_Songs = songs;
    m_Parent = parent;
}

///////////////////////////////////////////////////

void CmusikBatchTagDlg::Apply()
{
    SetWindowText(_T("musikCube Batch Edit (scanning...)"));
    if (m_Songs)
    {
        int title, artist, album, track, genre, year, rating, comment;
        CString title_str, artist_str, album_str, track_str, genre_str, year_str, rating_str, comment_str;

        CButton* ptrButton;

        ptrButton = (CButton*)GetDlgItem(IDC_ENBTITLE);
        title = ptrButton->GetCheck();
        m_Title.GetWindowText(title_str);

        ptrButton = (CButton*)GetDlgItem(IDC_ENBARTIST);
        artist = ptrButton->GetCheck();
        m_Artist.GetWindowText(artist_str);

        ptrButton = (CButton*)GetDlgItem(IDC_ENBALBUM);
        album = ptrButton->GetCheck();
        m_Album.GetWindowText(album_str);

        ptrButton = (CButton*)GetDlgItem(IDC_ENBTRACK);
        track = ptrButton->GetCheck();
        m_Track.GetWindowText(track_str);

        ptrButton = (CButton*)GetDlgItem(IDC_ENBGENRE);
        genre = ptrButton->GetCheck();
        m_Genre.GetWindowText(genre_str);

        ptrButton = (CButton*)GetDlgItem(IDC_ENBYEAR);
        year = ptrButton->GetCheck();
        m_Year.GetWindowText(year_str);

        ptrButton = (CButton*)GetDlgItem(IDC_ENBRATING);
        rating = ptrButton->GetCheck();
        m_Rating.GetWindowText(rating_str);

        ptrButton = (CButton*)GetDlgItem(IDC_ENBCOMMENT);
        comment = ptrButton->GetCheck();
        m_Comment.GetWindowText(comment_str);

        if (rating_str == _T("Unrated"))
            rating_str = _T("0");

        for (size_t i = 0; i < m_Songs->size(); i++)
        {
            if (title)
                m_Songs->at(i).SetTitle(title_str.GetBuffer(NULL));

            if (artist)
                m_Songs->at(i).SetArtist(artist_str.GetBuffer(NULL));

            if (album)
                m_Songs->at(i).SetAlbum(album_str.GetBuffer(NULL));

            if (track)
                m_Songs->at(i).SetTrackNum(track_str.GetBuffer(NULL));

            if (genre)
                m_Songs->at(i).SetGenre(genre_str.GetBuffer(NULL));

            if (year)
                m_Songs->at(i).SetYear(year_str.GetBuffer(NULL));
            
            if (comment)
                m_Songs->at(i).SetNotes(comment_str.GetBuffer(NULL));

            if (rating)
                m_Songs->at(i).SetRating(rating_str.GetBuffer(NULL));
        }
    }
    SetWindowText(_T("musikCube Batch Edit"));
}

///////////////////////////////////////////////////

CmusikBatchTagDlg::~CmusikBatchTagDlg()
{
}

///////////////////////////////////////////////////

void CmusikBatchTagDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TITLE, m_Title);
    DDX_Control(pDX, IDC_ARTIST, m_Artist);
    DDX_Control(pDX, IDC_ALBUM, m_Album);
    DDX_Control(pDX, IDC_TRACK, m_Track);
    DDX_Control(pDX, IDC_GENRE, m_Genre);
    DDX_Control(pDX, IDC_YEAR, m_Year);
    DDX_Control(pDX, IDC_RATING, m_Rating);
    DDX_Control(pDX, IDC_COMMENT, m_Comment);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikBatchTagDlg, CDialog)
    ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
    ON_BN_CLICKED(IDC_ENBTITLE, OnBnClickedEnbtitle)
    ON_BN_CLICKED(IDC_ENBARTIST, OnBnClickedEnbartist)
    ON_BN_CLICKED(IDC_ENBALBUM, OnBnClickedEnbalbum)
    ON_BN_CLICKED(IDC_ENBTRACK, OnBnClickedEnbtrack)
    ON_BN_CLICKED(IDC_ENBGENRE, OnBnClickedEnbgenre)
    ON_BN_CLICKED(IDC_ENBYEAR, OnBnClickedEnbyear)
    ON_BN_CLICKED(IDC_ENBRATING, OnBnClickedEnbrating)
    ON_BN_CLICKED(IDC_ENBCOMMENT, OnBnClickedEnbcomment)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikBatchTagDlg::OnBnClickedCancel()
{
    OnCancel();
}

///////////////////////////////////////////////////

void CmusikBatchTagDlg::OnBnClickedOk()
{
    Apply();
    OnOK();
}

///////////////////////////////////////////////////

BOOL CmusikBatchTagDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // combo boxes
    m_Rating.AddString(_T("-1"));
    m_Rating.AddString(_T("Unrated"));
    m_Rating.AddString(_T("1"));
    m_Rating.AddString(_T("2"));
    m_Rating.AddString(_T("3"));
    m_Rating.AddString(_T("4"));
    m_Rating.AddString(_T("5"));

    for (int i = 0; i < ID3_NR_OF_V1_GENRES; i++)
        m_Genre.AddString(ID3_v1_genre_description[i]);

    // default values
    if (m_Songs->size())
    {
        m_Title.SetWindowText(m_Songs->at(0).GetTitle());
        m_Artist.SetWindowText(m_Songs->at(0).GetArtist());
        m_Album.SetWindowText(m_Songs->at(0).GetAlbum());
        m_Track.SetWindowText(m_Songs->at(0).GetTrackNum());
        m_Genre.SetWindowText(m_Songs->at(0).GetGenre());
        m_Year.SetWindowText(m_Songs->at(0).GetYear());
        m_Comment.SetWindowText(m_Songs->at(0).GetNotes());

        if (m_Songs->at(0).GetRating().IsEmpty() || m_Songs->at(0).GetRating() == _T("0"))
            m_Rating.SelectString(-1, _T("Unrated"));
        else
            m_Rating.SelectString(-1, m_Songs->at(0).GetRating());
    }    

    m_Title.EnableWindow(FALSE);
    m_Artist.EnableWindow(FALSE);
    m_Album.EnableWindow(FALSE);
    m_Track.EnableWindow(FALSE);
    m_Genre.EnableWindow(FALSE);
    m_Year.EnableWindow(FALSE);
    m_Rating.EnableWindow(FALSE);
    m_Comment.EnableWindow(FALSE);

    return TRUE;
}

///////////////////////////////////////////////////

void CmusikBatchTagDlg::OnBnClickedEnbtitle()
{
    CButton* ptrButton;
    ptrButton = (CButton*)GetDlgItem(IDC_ENBTITLE);
    if (ptrButton->GetCheck())
    {
        m_Title.EnableWindow(TRUE);
        m_Title.SetFocus();

        CString text;
        m_Title.GetWindowText(text);
        m_Title.SetSel(0, text.GetLength());
    }
    else
        m_Title.EnableWindow(FALSE);

    CString text;
    m_Title.GetWindowText(text);
    m_Title.SetSel(0, text.GetLength());
}

///////////////////////////////////////////////////

void CmusikBatchTagDlg::OnBnClickedEnbartist()
{
    CButton* ptrButton;
    ptrButton = (CButton*)GetDlgItem(IDC_ENBARTIST);
    if (ptrButton->GetCheck())
    {
        m_Artist.EnableWindow(TRUE);
        m_Artist.SetFocus();

        CString text;
        m_Artist.GetWindowText(text);
        m_Artist.SetSel(0, text.GetLength());
    }
    else
        m_Artist.EnableWindow(FALSE);


}

///////////////////////////////////////////////////

void CmusikBatchTagDlg::OnBnClickedEnbalbum()
{
    CButton* ptrButton;
    ptrButton = (CButton*)GetDlgItem(IDC_ENBALBUM);
    if (ptrButton->GetCheck())
    {
        m_Album.EnableWindow(TRUE);
        m_Album.SetFocus();

        CString text;
        m_Album.GetWindowText(text);
        m_Album.SetSel(0, text.GetLength());    
    }
    else
        m_Album.EnableWindow(FALSE);
}

///////////////////////////////////////////////////

void CmusikBatchTagDlg::OnBnClickedEnbtrack()
{
    CButton* ptrButton;
    ptrButton = (CButton*)GetDlgItem(IDC_ENBTRACK);
    if (ptrButton->GetCheck())
    {
        m_Track.EnableWindow(TRUE);
        m_Track.SetFocus();

        CString text;
        m_Track.GetWindowText(text);
        m_Track.SetSel(0, text.GetLength());    
    }
    else
        m_Track.EnableWindow(FALSE);
}

///////////////////////////////////////////////////

void CmusikBatchTagDlg::OnBnClickedEnbgenre()
{
    CButton* ptrButton;
    ptrButton = (CButton*)GetDlgItem(IDC_ENBGENRE);
    if (ptrButton->GetCheck())
    {
        m_Genre.EnableWindow(TRUE);
        m_Genre.SetFocus();
    }
    else
        m_Genre.EnableWindow(FALSE);
}

///////////////////////////////////////////////////

void CmusikBatchTagDlg::OnBnClickedEnbyear()
{
    CButton* ptrButton;
    ptrButton = (CButton*)GetDlgItem(IDC_ENBYEAR);
    if (ptrButton->GetCheck())
    {
        m_Year.EnableWindow(TRUE);
        m_Year.SetFocus();

        CString text;
        m_Year.GetWindowText(text);
        m_Year.SetSel(0, text.GetLength());    
    }
    else
        m_Year.EnableWindow(FALSE);
}

///////////////////////////////////////////////////

void CmusikBatchTagDlg::OnBnClickedEnbrating()
{
    CButton* ptrButton;
    ptrButton = (CButton*)GetDlgItem(IDC_ENBRATING);
    if (ptrButton->GetCheck())
    {
        m_Rating.EnableWindow(TRUE);
        m_Rating.SetFocus();
    }
    else
        m_Rating.EnableWindow(FALSE);
}

///////////////////////////////////////////////////

void CmusikBatchTagDlg::OnBnClickedEnbcomment()
{
    CButton* ptrButton;
    ptrButton = (CButton*)GetDlgItem(IDC_ENBCOMMENT);
    if (ptrButton->GetCheck())
    {
        m_Comment.EnableWindow(TRUE);
        m_Comment.SetFocus();
    }
    else
        m_Comment.EnableWindow(FALSE);
}

///////////////////////////////////////////////////


