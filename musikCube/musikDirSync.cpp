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
#include "musikDirSync.h"
#include "musikCube.h"
#include ".\musikdirsync.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikDirSync, CDialog)

///////////////////////////////////////////////////

CmusikDirSync::CmusikDirSync(CWnd* pParent, musikCore::Library* pLibrary)
    : CDialog(CmusikDirSync::IDD, pParent)
{
    if ( ! pLibrary)
    {
        throw new exception("CmusikDirSync::ctor: Library == NULL");
    }

    this->m_Changed = false;
    this->m_Library = pLibrary;
}

///////////////////////////////////////////////////

CmusikDirSync::~CmusikDirSync()
{
}

///////////////////////////////////////////////////

void CmusikDirSync::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_DIRS, m_wndPaths);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikDirSync, CDialog)
    ON_BN_CLICKED(IDC_REMOVE, OnBnClickedRemove)
    ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
    ON_WM_CHAR()
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_OK, OnBnClickedOk)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikDirSync::OnBnClickedOk()
{
    CDialog::OnOK();
}

///////////////////////////////////////////////////

void CmusikDirSync::OnBnClickedRemove()
{
    DeleteSel();
}

///////////////////////////////////////////////////

void CmusikDirSync::DeleteSel()
{
    int count = m_wndPaths.GetCount();

    CString sStr;

    for (int i = 0; i < count; i++)
    {
        if (m_wndPaths.GetSel(i))
        {
            m_wndPaths.GetText(i, sStr);
            this->m_Library->RemovePath((musikCore::String)sStr);
        }
    }

    ResetFromLibrary();
}

///////////////////////////////////////////////////

void CmusikDirSync::OnBnClickedAdd()
{
    TCHAR path[MAX_PATH];
    BROWSEINFO bi = { 0 };
    bi.lpszTitle = _T("Please select a directory containing music.");
    bi.hwndOwner = GetSafeHwnd();

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

        CString str_path = path;
        if (str_path.Right(1) != _T("\\"))
            str_path += _T("\\");    
        
        if (ValidatePath(str_path))
        {
            this->m_Library->AddPath(str_path.GetBuffer(NULL));
            ResetFromLibrary();

            m_Changed = true;
        }
    }
}

///////////////////////////////////////////////////

BOOL CmusikDirSync::OnInitDialog()
{
    CDialog::OnInitDialog();

    ResetFromLibrary();

    return TRUE;
}

void CmusikDirSync::ResetFromLibrary()
{
    musikCore::StringArray paths;
    this->m_Library->GetAllPaths(paths, false);

    m_wndPaths.ResetContent();
    
    for (size_t i = 0; i < paths.size(); i++)
    {
        m_wndPaths.AddString(paths.at(i));
    }
}

///////////////////////////////////////////////////

bool CmusikDirSync::ValidatePath(const CString& path)
{
    if (m_wndPaths.GetCount() == 0)
        return true;

    CString    original_str, conflict_str, conflict_err;
    musikCore::IntArray conflict_arr;

    original_str = path;
    original_str.MakeLower();

    for (int i = 0; i < m_wndPaths.GetCount(); i++)
    {
        m_wndPaths.GetText(i, conflict_str);
        conflict_str.MakeLower();

        // if the new path equals the old path then return
        if (conflict_str == original_str)
        {
            MessageBox(
                _T("This path is already setup for synchronization."), 
                _T(MUSIK_VERSION_STR), 
                MB_ICONINFORMATION);

            return false;
        }

        // if new path is longer than path we're checking
        // against, it will be a child folder. see if they
        // have the same root
        if (original_str.GetLength() > conflict_str.GetLength())
        {
            if (original_str.Find(conflict_str) > -1)
            {
                MessageBox(
                    _T("The path entered is already contained within the following path's scope:\n\n") + conflict_str, 
                    _T(MUSIK_VERSION_STR), 
                    MB_ICONINFORMATION);

                return false;
            }
        }

        // if the old path is longer than the path we're
        // checking against, it may be a path's parent dir
        else 
        {
            if (conflict_str.Find(original_str) > -1)
            {
                conflict_err += conflict_str + _T("\n");
                conflict_arr.push_back(i);
            }
        }
    }

    // display conflicts and ask user what to do
    if (conflict_arr.size())
    {
        if (MessageBox(_T("The path entered conflicts with the following paths:\n\n") + conflict_err + _T("\nWould you like musikCube to fix this conflict for you?"), _T(MUSIK_VERSION_STR), MB_ICONINFORMATION | MB_YESNO) == IDYES)
        {
            for (size_t i = 0; i < conflict_arr.size(); i++)
            {
                CString invalidPath;
                m_wndPaths.GetText(conflict_arr[i], invalidPath); 
                this->m_Library->RemovePath(invalidPath.GetBuffer());
            }

            ResetFromLibrary();

            return true;
        }
        else 
            return false;
    }
    

    return true;
}

///////////////////////////////////////////////////

BOOL CmusikDirSync::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
    {
        this->OnClose();
        return TRUE;
    }

    return CDialog::PreTranslateMessage(pMsg);
}


