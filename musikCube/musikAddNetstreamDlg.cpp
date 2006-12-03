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
#include "musikAddNetstreamDlg.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikAddNetstreamDlg, CDialog)

///////////////////////////////////////////////////

CmusikAddNetstreamDlg::CmusikAddNetstreamDlg(musikCore::Library* library)
    : CDialog(CmusikAddNetstreamDlg::IDD, NULL)
{
    if ( ! library)
    {
        throw new exception("CmusikAddNetstreamDlg::ctor: library == NULL");
    }

    this->m_Library = library;
}

///////////////////////////////////////////////////

CmusikAddNetstreamDlg::~CmusikAddNetstreamDlg()
{
}

///////////////////////////////////////////////////

void CmusikAddNetstreamDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

///////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(CmusikAddNetstreamDlg, CDialog)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikAddNetstreamDlg::OnBnClickedOk()
{
    CString strStream, strTitle;
    GetDlgItem(IDC_NETSTREAM_ADDR)->GetWindowText(strStream);
    GetDlgItem(IDC_NETSTREAM_TITLE)->GetWindowText(strTitle);

    if (strStream.IsEmpty())
    {
        MessageBox(
            _T("Please enter a valid netstream URL."),
            _T("musikCube"),
            MB_ICONWARNING | MB_OK);
        return;
    }

    if (strStream.Left(4).MakeLower() != _T("http"))
    {
        strStream = _T("http://") + strStream;
    }

    this->m_Library->AddSong(musikCore::String(strStream));

    if (!strTitle.IsEmpty())
    {
        // set the title
        musikCore::SongInfo info;
        musikCore::Song song;
        this->m_Library->GetSongAttrFromFilename(musikCore::String(strStream), song);
        this->m_Library->GetSongInfoFromID(song.GetID(), info);
        //
        info.SetTitle(strTitle.GetBuffer());
        //
        this->m_Library->SetSongInfo(info);
    }

    OnOK();
}

///////////////////////////////////////////////////