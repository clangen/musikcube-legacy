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
#include "musikCubeFunctor.h"

///////////////////////////////////////////////////

CmusikCubeFunctor::CmusikCubeFunctor(CWnd* parent)
{ 
    m_Parent = parent; 
}

///////////////////////////////////////////////////

void CmusikCubeFunctor::OnNewSong()
{
    int WM_SONGCHANGE = RegisterWindowMessage(_T("SONGCHANGE"));
    m_Parent->PostMessage(WM_SONGCHANGE, NULL);

    for (size_t i = 0; i < musikCube::g_Plugins.size(); i++)
        musikCube::g_Plugins.at(i).GetPluginFunctor().OnNewSong();
}

///////////////////////////////////////////////////

void CmusikCubeFunctor::OnNewEqualizer()
{
    int WM_EQUALIZERCHANGE = RegisterWindowMessage(_T("EQUALIZERCHANGE"));
    m_Parent->PostMessage(WM_EQUALIZERCHANGE, NULL);

    for (size_t i = 0; i < musikCube::g_Plugins.size(); i++)
        musikCube::g_Plugins.at(i).GetPluginFunctor().OnNewEqualizer();
}

///////////////////////////////////////////////////

void CmusikCubeFunctor::OnSeek()
{
    int WM_SONGSEEK = RegisterWindowMessage(_T("SONGSEEK"));
    m_Parent->PostMessage(WM_SONGSEEK, NULL);

    for (size_t i = 0; i < musikCube::g_Plugins.size(); i++)
    {
        if (musikCube::g_Plugins.at(i).GetRevision() > 0)
            musikCube::g_Plugins.at(i).GetPluginFunctor().OnSeek();
    }
}

///////////////////////////////////////////////////

void CmusikCubeFunctor::OnStop()
{
    int WM_SONGSTOP = RegisterWindowMessage(_T("SONGSTOP"));
    m_Parent->PostMessage(WM_SONGSTOP, NULL);

    for (size_t i = 0; i < musikCube::g_Plugins.size(); i++)
        musikCube::g_Plugins.at(i).GetPluginFunctor().OnStop();
}

///////////////////////////////////////////////////

void CmusikCubeFunctor::OnPause()
{
    int WM_SONGPAUSE = RegisterWindowMessage(_T("SONGPAUSE"));
    m_Parent->SendMessage(WM_SONGPAUSE, NULL);

    for (size_t i = 0; i < musikCube::g_Plugins.size(); i++)
        musikCube::g_Plugins.at(i).GetPluginFunctor().OnPause();
}

///////////////////////////////////////////////////

void CmusikCubeFunctor::OnResume()
{
    int WM_SONGRESUME = RegisterWindowMessage(_T("SONGRESUME"));
    m_Parent->SendMessage(WM_SONGRESUME, NULL);

    for (size_t i = 0; i < musikCube::g_Plugins.size(); i++)
        musikCube::g_Plugins.at(i).GetPluginFunctor().OnResume();
}

///////////////////////////////////////////////////

void CmusikCubeFunctor::OnPlaybackFailed()
{
    MessageBox(
        NULL, 
        _T("Playback failed.\n\nThis needs to post an event to the main frame."), 
        _T("musikCube"), 
        MB_ICONINFORMATION);
}

///////////////////////////////////////////////////

void CmusikCubeFunctor::OnTaskEnd(musikCore::Task* task_addr)
{
    int WM_TASK_END = RegisterWindowMessage(_T("TASK_END"));
    m_Parent->SendMessage(WM_TASK_END, (WPARAM)task_addr);    

    for (size_t i = 0; i < musikCube::g_Plugins.size(); i++)
        musikCube::g_Plugins.at(i).GetPluginFunctor().OnTaskEnd(task_addr);
}

///////////////////////////////////////////////////

void CmusikCubeFunctor::OnTaskStart()
{
    OnTaskProgress(0, NULL);
}

///////////////////////////////////////////////////

void CmusikCubeFunctor::OnTaskProgress(size_t progress, musikCore::Task* task_addr)
{
    int WM_TASK_PROGRESS = RegisterWindowMessage(_T("TASK_PROGRESS"));
    m_Parent->PostMessage(WM_TASK_PROGRESS, (WPARAM)progress, NULL);    

    for (size_t i = 0; i < musikCube::g_Plugins.size(); i++)
        musikCube::g_Plugins.at(i).GetPluginFunctor().OnTaskProgress(progress, task_addr);
}

///////////////////////////////////////////////////

bool CmusikCubeFunctor::VerifyPlaylist(void* pl_addr)
{
    if (!pl_addr)
        return false;

    int WM_BATCHADD_VERIFY_PLAYLIST = RegisterWindowMessage(_T("BATCHADD_VERIFY_PLAYLIST"));
    LRESULT res = m_Parent->SendMessage(WM_BATCHADD_VERIFY_PLAYLIST, (WPARAM)pl_addr);

    if (res == 1L)
        return true;
    else
        return false;
}

///////////////////////////////////////////////////

void CmusikCubeFunctor::OnAlarmTrigger()
{
    int WM_ALARM_TRIGGER = RegisterWindowMessage(_T("ALARM_TRIGGER"));
    m_Parent->PostMessage(WM_ALARM_TRIGGER, NULL, NULL);

    for (size_t i = 0; i < musikCube::g_Plugins.size(); i++)
        musikCube::g_Plugins.at(i).GetPluginFunctor().OnAlarmTrigger();
}

///////////////////////////////////////////////////

