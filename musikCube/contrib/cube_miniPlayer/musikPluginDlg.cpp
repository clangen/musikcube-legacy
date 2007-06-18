///////////////////////////////////////////////////
// 
// Info:
//
//    ...
//
// Credits:
//
//    musikCube by Casey Langen
//    Plugin by Martin Dittus
//
///////////////////////////////////////////////////

#include "stdafx.h"

#include "musikPluginDlg.h"

#ifdef COMPILE_STANDALONE
#include "globals.h"
#endif

#include <cmath>

#define IDT_TIMER (WM_USER + 200)

// command ids for standard/dynamic playlists in the context menu
#define ID_STDPLAYLIST (WM_USER + 300)
#define MAXID_STDPLAYLIST (WM_USER + 399)
#define ID_DYNPLAYLIST (WM_USER + 400)
#define MAXID_DYNPLAYLIST (WM_USER + 499)
#define ID_ADDTOPLAYLIST (WM_USER + 500)
#define MAXID_ADDTOPLAYLIST (WM_USER + 599)

// index positions of context menu entries
#define MENU_INDEX_STDPLAYLIST 0
#define MENU_INDEX_DYNPLAYLIST 1
#define MENU_INDEX_ADDTOPLAYLIST 2
#define MENU_INDEX_SONGSBYTHEARTIST 8
#define MENU_INDEX_SONGSFROMTHEALBUM 9


// -- ... -----------------------------------

IMPLEMENT_DYNAMIC(CAboutDlg, CDialog)

CAboutDlg::CAboutDlg(LPCTSTR caption, CWnd* pParent /*=NULL*/)
    : CDialog(CAboutDlg::IDD, pParent), m_Caption(caption)
{
}

CAboutDlg::~CAboutDlg()
{
    m_BoldFont.DeleteObject();
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_ABOUT_NAME, m_cPluginName);
    DDX_Control(pDX, IDC_ABOUT_LINK, m_cLink);
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    SetWindowText(m_Caption);

// link control
    m_cLink.SetWindowText(m_link);
    //m_cLink.SetLink(m_link);

// plugin name
    m_cPluginName.SetWindowText(m_pluginName);

    CFont *pFont;
    LOGFONT bold;
    pFont = m_cPluginName.GetFont();
    pFont->GetLogFont(&bold);
    bold.lfWeight = FW_BOLD;
    m_BoldFont.CreateFontIndirect(&bold);
    m_cPluginName.SetFont(&m_BoldFont);

// etc

    return TRUE;    // as we did not set the focus to a control
}


void CAboutDlg::SetPluginName(LPCTSTR pluginName)
{
    m_pluginName = pluginName;
}

void CAboutDlg::SetLink(LPCTSTR link)
{
    m_link = link;
}


// -- ... -----------------------------------


IMPLEMENT_DYNAMIC(CConfigDlg, CDialog)
CConfigDlg::CConfigDlg(LPCTSTR caption, CWnd* pParent /*=NULL*/)
    : CDialog(CConfigDlg::IDD, pParent), m_Caption(caption), m_Ontop(true)
{
}

CConfigDlg::~CConfigDlg()
{
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CONFIG_ONTOP, m_cOntop);
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
    ON_BN_CLICKED(IDC_CONFIG_ONTOP, OnBnClickedConfigOntop)
END_MESSAGE_MAP()


BOOL CConfigDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    SetWindowText(m_Caption);

    if (m_Ontop)
        m_cOntop.SetCheck(BST_CHECKED);
    else
        m_cOntop.SetCheck(BST_UNCHECKED);

// etc

    return TRUE;    // as we did not set the focus to a control
}


void CConfigDlg::OnBnClickedConfigOntop()
{
    // is "always on top" checked?
    m_Ontop = (m_cOntop.GetCheck() == BST_CHECKED);
}

// -- ... -----------------------------------

IMPLEMENT_DYNAMIC(CMainDlg, CDialog)
CMainDlg::CMainDlg(LPCTSTR caption, musikCore::Player* player, CWnd* pParent /*=NULL*/)
    : CDialog(CMainDlg::IDD, NULL/*pParent*/), m_Caption(caption), m_cSearch( this ), m_Player(player), m_ShowRemaining(false),
    m_ShowRating(false), m_AlwaysOnTop(false), m_XPos(-1), m_YPos(-1), m_QueryLimit(50), m_AlbumLimit(8)

{
    //Create(IDD, pParent);
    m_AppWnd = pParent;                        // main app; we need this to send window messages
    Create(IDD, pParent);                    // desktop window is parent window
    this->SetIcon(AfxGetApp()->LoadIcon(IDI_ICON1), TRUE);
}

#ifdef COMPILE_STANDALONE

CMainDlg::CMainDlg( musikCore::Player* player )
 :    m_cSearch( this ),
     CDialog( IDD_MAIN, NULL )
{
    
    m_Player = player;
    m_Caption = _T( "miniPlayer" );
}

#endif

CMainDlg::~CMainDlg()
{
    /*
    m_BoldFont.DeleteObject();
    KillTimer(m_TimerID);
    */
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_MAIN_TIME, m_cTime);
    DDX_Control(pDX, IDC_MAIN_INDEX, m_cIndex);
    DDX_Control(pDX, IDC_MAIN_PREV, m_cPrev);
    DDX_Control(pDX, IDC_MAIN_PLAY, m_cPlay);
    DDX_Control(pDX, IDC_MAIN_STOP, m_cStop);
    DDX_Control(pDX, IDC_MAIN_NEXT, m_cNext);
    DDX_Control(pDX, IDC_MAIN_POSITION, m_cPosition);
    DDX_Control(pDX, IDC_MAIN_VOLUME, m_cVolume);
    DDX_Control(pDX, IDC_MAIN_SEARCH, m_cSearch);
    DDX_Control(pDX, IDC_MAIN_ARTIST, m_cArtist);
    DDX_Control(pDX, IDC_MAIN_TITLE, m_cTitle);
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
    ON_REGISTERED_MESSAGE(WM_ENTEREDIT_ENTER, OnEditComplete )
    //{{AFX_MSG_MAP(CMainDlg)

    // context menu
    ON_COMMAND(ID_RANDOMNEW, OnRandomNew)
    ON_COMMAND(ID_RANDOMPOPULAR, OnRandomPopular)
    ON_COMMAND(ID_RANDOMOLD, OnRandomOld)
    ON_COMMAND(ID_SURPRISEME, OnSurpriseMe)
    ON_COMMAND(ID_ALBUMMODE, OnAlbumMode)
    ON_COMMAND(ID_SONGSBYTHEARTIST, OnSongsByTheArtist)
    ON_COMMAND(ID_SONGSFROMTHEALBUM, OnSongsFromTheAlbum)
    ON_COMMAND(ID_RANDOM, OnRandom)
    ON_COMMAND(ID_REPEATPLAYLIST, OnRepeatPlaylist)
    ON_COMMAND(ID_REPEATSONG, OnRepeatSong)
    ON_COMMAND(ID_INTRO, OnIntro)
    ON_COMMAND(ID_SHOWRATING, OnShowRating)
    ON_COMMAND(ID_ALWAYSONTOP, OnAlwaysOnTop)
    ON_COMMAND(ID_EXIT, OnExit)

    // controls
    ON_BN_CLICKED(IDC_MAIN_PREV, OnBnClickedMainPrev)
    ON_BN_CLICKED(IDC_MAIN_PLAY, OnBnClickedMainPlay)
    ON_BN_CLICKED(IDC_MAIN_STOP, OnBnClickedMainStop)
    ON_BN_CLICKED(IDC_MAIN_NEXT, OnBnClickedMainNext)
    ON_STN_CLICKED(IDC_MAIN_TIME, OnStnClickedMainTime)
    ON_STN_CLICKED(IDC_MAIN_INDEX, OnStnClickedMainIndex)
    ON_MESSAGE(WM_TRACKSLIDER_MOVED, OnTrackSliderMoved)
    ON_MESSAGE(WM_TRACKSLIDER_MOVING, OnTrackSliderMoving)
    ON_EN_SETFOCUS(IDC_MAIN_SEARCH, OnEnSetfocusMainSearch)
    ON_EN_KILLFOCUS(IDC_MAIN_SEARCH, OnEnKillfocusMainSearch)

    // etc
    ON_WM_TIMER()
    ON_WM_SHOWWINDOW()
    ON_WM_CONTEXTMENU()
    ON_WM_NCHITTEST()
    ON_WM_NCRBUTTONUP()
    ON_WM_GETDLGCODE()
    ON_WM_KEYDOWN()
    ON_WM_MOUSEWHEEL()
    //}}AFX_MSG_MAP
    ON_WM_CLOSE()
END_MESSAGE_MAP()


BOOL CMainDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    SetWindowText(m_Caption);

    m_cPosition.SetRange(0, 100);
    m_cVolume.SetPageSize(5);
    m_cPosition.HandleKeys(FALSE);

    m_cVolume.SetRange(0, 255);
    m_cVolume.SetPageSize(10);
    m_cVolume.SetVertical(TRUE);
    m_cVolume.HandleKeys(FALSE);
    m_cVolume.SetPos(m_Player->GetMaxVolume());

    m_cSearch.SetWindowText(_T("Search"));

    m_iPrev = ::LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_BTN_PREV));
    m_iPlay = ::LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_BTN_PLAY));
    m_iPause = ::LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_BTN_PAUSE));
    m_iStop = ::LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_BTN_STOP));
    m_iNext = ::LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_BTN_NEXT));

    m_cPrev.SetIcon(m_iPrev);
    m_cPlay.SetIcon(m_iPlay);
    m_cStop.SetIcon(m_iStop);
    m_cNext.SetIcon(m_iNext);

// hide window title?    
    //ModifyStyle(WS_CAPTION, WS_BORDER, 0);
    //ModifyStyle(WS_BORDER, WS_CAPTION, 0);

    // standard window
    //ModifyStyle(0, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER | WS_OVERLAPPED, 0);
    //ModifyStyle(0, WS_CAPTION | WS_POPUPWINDOW | WS_MINIMIZE, 0);
    // WS_MINIMIZE 
    // WS_CAPTION

    // WS_BORDER 
    //ModifyStyle(WS_OVERLAPPED, WS_POPUP, 0);

    // Winspy says: to change the default window style to noncaption + border, do this:
    // - WS_OVERLAPPEDWINDOW
    // + WS_BORDER
    // ...and then resize (by how much?)

    //DWORD style = GetWindowLong(this->m_hWnd, GWL_STYLE);
    //style &= ~WS_CAPTION;
    //SetWindowLong(m_hWnd, GWL_STYLE, style);
    //::SetWindowPos(m_hWnd,0,0,0,0,0,39) ;

    // http://support.microsoft.com/default.aspx?scid=kb;en-us;111011

// bold font for title field
    CFont *pFont;
    LOGFONT bold;
    pFont = m_cTitle.GetFont();
    pFont->GetLogFont(&bold);
    bold.lfWeight = FW_BOLD;
    m_BoldFont.CreateFontIndirect(&bold);
    //m_cArtist.SetFont(&m_BoldFont);
    m_cTitle.SetFont(&m_BoldFont);

// default + star font
    m_DefaultFont.CreateStockObject( DEFAULT_GUI_FONT );
    m_StarFont.CreatePointFont( 100, _T( "musik" ) );

// etc
    RefreshDisplay();

    // start timer
    m_TimerID = SetTimer(IDT_TIMER, 1000, NULL);

    srand(time(NULL));

    return TRUE;    // as we did not set the focus to a control
}

void CMainDlg::SetAlwaysOnTop(bool alwaysontop)
{
    m_AlwaysOnTop = alwaysontop;

    if (alwaysontop)
    {
        SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
    }
    else
    {
        SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
    }
}

void CMainDlg::SetWinPosXY(int xpos, int ypos)
{ 
    m_XPos = xpos; 
    m_YPos = ypos;
    
    if (m_XPos >= 0 && m_YPos >=0)
        SetWindowPos(NULL, m_XPos, m_YPos, 0, 0, SWP_NOSIZE);
}

void CMainDlg::GetWinPosXY(int &xpos, int &ypos)
{
    CRect rect;
    GetWindowRect(rect);
    xpos = rect.left;
    ypos = rect.top;
}



void CMainDlg::UpdateTime()
{
    CString str;
    musikCore::SongInfo* songinfo = m_Player->GetPlaying();

    //if (!IsWindowVisible())
    //    return;

    // time
    if (songinfo == NULL || songinfo->GetID() == -1)
    {
        str = _T("0:00/0:00");
    }
    else
    {
        int now;
        
        if (m_ShowRemaining && 
            !(m_Player->GetFormat() == /*musikCore::*/MUSIK_LIBRARY_FORMAT_NET))        // does not work for netradio
        {
            now = m_Player->GetTimeRemain(musikCore::MUSIK_TIME_MSECS);
        }
        else
        {
            now = m_Player->GetTimeNow(musikCore::MUSIK_TIME_MSECS);
        }

        str.Format(_T("%s%s/%s"), 
            m_ShowRemaining ? _T("-") : _T(""),
            m_Player->GetTimeStr( now ).c_str(), 
            m_Player->GetTimeStr( m_Player->GetDuration(musikCore::MUSIK_TIME_MSECS) ).c_str());
            //
    }
    m_cTime.SetWindowText(str);

    // sliders
    m_cPosition.SetPos(m_Player->GetTimeNowPer());
    //m_cVolume.SetPos(m_Player->GetMaxVolume());
}

void CMainDlg::UpdateNowPlaying()
{
    CString str;

    //musikCore::SongInfo* songinfo = m_Player->GetPlaying();
    
    // reload all info from the library, in case the user has made changes:
    musikCore::SongInfo songinfo;
    musikCore::Library* songLib = m_Player->GetPlaying()->GetLibrary();

    int ret = -1;
    if (songLib != NULL)
    {
        ret = songLib->GetSongInfoFromID( m_Player->GetPlaying()->GetID(), songinfo);
    }

    //if (!IsWindowVisible())
    //    return;

    if (/*songinfo == NULL*/ ret != SQLITE_OK || !m_Player->IsPlaying())
    {
        // artist/title fields
        m_cTitle.SetWindowText(m_Caption);
        m_cArtist.SetWindowText(_T("--"));

        // window title
        SetWindowText(m_Caption);
    }
    else
    {
        CString title, artist, album;
        CString info;

    // title
        if (songinfo.GetTitle().size() == 0)
        {
            //musikCore::Filename fn(songinfo.GetFilename());
            //title = fn.GetJustFilenameNoExt();
        }
        else
        {
            title = songinfo.GetTitle();
        }

        m_cTitle.SetWindowText(title);

    // artist/album
        if (songinfo.GetArtist().size() == 0)
        {
            //artist = _T("unknown artist");
        }
        else
        {
            artist = songinfo.GetArtist();
        }

        if (songinfo.GetAlbum().size() == 0)
        {
            //album = _T("No Album");
            
            info = artist;
        }
        else
        {
            album = songinfo.GetAlbum();
            
            info.Format(
                _T("%s - %s"),
                artist, 
                album);
        }

        m_cArtist.SetWindowText(info);

        // window title
        str.Format(
            _T("[%s - %s] %s"), 
            title, 
            artist,
            m_Caption);
        SetWindowText(str);
    }

    str.Empty();

    // rating/index in playlist
    if (m_ShowRating)
    {
        // rating
        CString sRating = _T( ",,,,," );
        if (m_Player->IsPlaying())
        {
            int nRating = musikCore::StringToInt(m_Player->GetPlaying()->GetRating());
            switch ( nRating )
            {
            case 1:
                sRating = _T( "-,,,," );
                break;
            case 2:
                sRating = _T( "--,,," );
                break;
            case 3:
                sRating = _T( "---,," );
                break;
            case 4:
                sRating = _T( "----," );
                break;
            case 5:
                sRating = _T( "-----" );
                break;
            }
        }
        m_cIndex.SetFont(&m_StarFont, FALSE);
        m_cIndex.SetWindowText(sRating);
    }
    else
    {
        // index in playlist
        if (m_Player->GetPlaylist()->GetCount() == 0)
        {
            str = _T("-/-");
        }
        else
        {
            str.Format(
                _T("%d/%d"), 
                m_Player->GetIndex() + 1, 
                m_Player->GetPlaylist()->GetCount());
        }
        m_cIndex.SetFont(&m_DefaultFont, FALSE);
        m_cIndex.SetWindowText(str);
    }
}

void CMainDlg::UpdateControls()
{
    //if (!IsWindowVisible())
    //    return;

    // play button
    if (m_Player->IsPaused() || !m_Player->IsPlaying()) 
    {
        //m_cPlay.SetWindowText(_T(">"));
        m_cPlay.SetIcon(m_iPlay);
    }
    else 
    {
        //m_cPlay.SetWindowText(_T("||"));
        m_cPlay.SetIcon(m_iPause);
    }
    // repeat button
    /*if (m_Player->GetPlaymode() & musikCore::MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST)
    {
        m_cRepeat.SetWindowText(_T("loop"));
    }
    else
    {
        m_cRepeat.SetWindowText(_T("---"));
    }*/
}

// sends the playlist to "now playing"
// if shift key is pressed: enqueue
// else: replace current playlist, start playback from beginning
// always: send NOWPLAYINGUPDATED message to parent
void CMainDlg::SendPlaylistAndPlay(musikCore::Playlist playlist)
{
    if (GetKeyState(VK_SHIFT) & (2 << 15))
    {
        // shift key pressed -> enqueue
        for (size_t i=0; i<playlist.size(); i++)
            m_Player->GetPlaylist()->Add(playlist.GetSong(i));
    }
    else
    {
        // set
        m_Player->SetPlaylist(playlist);
        m_Player->Play(0);
    }

    // tell main app to update the "now playing" view
#ifndef COMPILE_STANDALONE
    int WM_NOWPLAYINGUPDATED = RegisterWindowMessage(_T( "NOWPLAYINGUPDATED"));
    m_AppWnd->SendMessage(WM_NOWPLAYINGUPDATED);
#endif

    // update playlist index display
    //UpdateNowPlaying();
}
    
// queries the library and plays back the result
// returns true if songs were found
bool CMainDlg::QueryAndPlay(CString query)
{
    musikCore::Playlist playlist;

    if (m_AlbumMode)
    {
        query.Format(
            m_AlbumQuery,
            query,
            m_AlbumLimit);
    }
    m_Player->GetLibrary()->QuerySongs((musikCore::String)query, playlist);
    if (playlist.GetCount() != 0)
    {
        SendPlaylistAndPlay(playlist);
        return true;
    }
    //MessageBox(query, _T("empty result!"));
    return false;
}


void CMainDlg::OnRandomNew()
{
    CString query;
    query.Format(m_QueryRndNew, m_QueryLimit);
    if (!QueryAndPlay(query))
    {
        // second attempt: search unplayed
        query.Format(m_QueryRndUnpl, m_QueryLimit);
        if (!QueryAndPlay(query))
        {
            // no new or unplayed music
            MessageBox(_T("You have not added any music recently."), _T("Sorry, I found nothing"));
        }
    }
}

void CMainDlg::OnRandomPopular()
{
    CString query;
    query.Format(m_QueryRndPop, m_QueryLimit);
    QueryAndPlay(query);
    if (!QueryAndPlay(query))
    {
        // this query should always work
        MessageBox(_T("Hm, either you have an empty library or I screwed up."), _T("Sorry, I found nothing"));
    }
}

void CMainDlg::OnRandomOld()
{
    CString query;
    query.Format(m_QueryRndOld, m_QueryLimit);
    if (!QueryAndPlay(query))
    {
        // second attempt: search unplayed
        query.Format(m_QueryRndUnpl, m_QueryLimit);
        if (!QueryAndPlay(query))
        {
            // no old or unplayed music
            MessageBox(_T("Your collection is brand new! Give it a couple of weeks."), _T("Sorry, I found nothing"));
        }
    }
}

void CMainDlg::OnSurpriseMe()
{
    bool success = false;
    int i = 0;
    
    // repeat until one query has succeeded
    while (!success) 
    {
        CString query, str;
        int r = ((float)rand() / (float)RAND_MAX * 10);
        switch (r)
        {
        case 0:
        case 1:
            str = m_QueryRndNew;
            break;
        case 2:
            str = m_QueryRndPop;
            break;
        case 3:
        case 4:
        case 5:
            str = m_QueryRndOld;
            break;
        case 6:
        case 7:
        case 8:
        case 9:
            str = m_QueryRndUnpl;
            break;
        }
        query.Format(str, m_QueryLimit);
        success = QueryAndPlay(query);
        i++;

        if (i > 50)
        {
            // probably an empty library?
            MessageBox(_T("Hm, either you have an empty library or I screwed up."), _T("Sorry, I found nothing"));
            return;
        }
    }
}

void CMainDlg::OnAlbumMode()
{
    m_AlbumMode = !m_AlbumMode;
}

void CMainDlg::OnSongsByTheArtist()
{
    CString query;
    query.Format(
        _T("LOWER(artist) LIKE LOWER('%%%s%%') ORDER BY year, album, tracknum"),
        musikCore::q_str(m_Player->GetPlaying()->GetArtist()).c_str());
    QueryAndPlay(query);
}

void CMainDlg::OnSongsFromTheAlbum()
{
    CString query;
    query.Format(
        _T("LOWER(album) LIKE LOWER('%%%s%%') ORDER BY tracknum"),
        musikCore::q_str(m_Player->GetPlaying()->GetAlbum()).c_str());
    QueryAndPlay(query);
}

void CMainDlg::OnRandom()
{
    unsigned long mode = m_Player->GetPlaymode();
    mode = mode ^ musikCore::MUSIK_PLAYER_PLAYMODE_RANDOM;
    m_Player->SetPlaymode(mode);
}

void CMainDlg::OnRepeatPlaylist()
{
    unsigned long mode = m_Player->GetPlaymode();
    mode = mode ^ musikCore::MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST;
    m_Player->SetPlaymode(mode);
}

void CMainDlg::OnRepeatSong()
{
    unsigned long mode = m_Player->GetPlaymode();
    mode = mode ^ musikCore::MUSIK_PLAYER_PLAYMODE_REPEAT_SINGLE;
    m_Player->SetPlaymode(mode);
}

void CMainDlg::OnIntro()
{
    unsigned long mode = m_Player->GetPlaymode();
    mode = mode ^ musikCore::MUSIK_PLAYER_PLAYMODE_INTRO;
    m_Player->SetPlaymode(mode);
}

void CMainDlg::OnShowRating()
{
    m_ShowRating = !m_ShowRating;
    UpdateNowPlaying();
}

void CMainDlg::OnAlwaysOnTop()
{
    SetAlwaysOnTop(!GetAlwaysOnTop());
}

void CMainDlg::OnExit()
{
    //CloseWindow();
    ShowWindow(SW_HIDE);
    EndModalLoop( NULL );
}


// for dynamic menus
BOOL CMainDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    if (nCode == CN_COMMAND)
    {
        musikCore::Playlist playlist;
        int id = -1;    // playlist id

        if (nID >= ID_STDPLAYLIST && nID <= MAXID_STDPLAYLIST)
        {
            id = nID - ID_STDPLAYLIST;
            m_Player->GetLibrary()->GetStdPlaylist(id, playlist);
        }
        else if (nID >= ID_DYNPLAYLIST && nID <= MAXID_DYNPLAYLIST)
        {
            id = nID - ID_DYNPLAYLIST;
            m_Player->GetLibrary()->QueryDynPlaylist(id, playlist);
        }
        else if (nID >= ID_ADDTOPLAYLIST && nID <= MAXID_ADDTOPLAYLIST)
        {
            id = nID - ID_ADDTOPLAYLIST;

            musikCore::StringArray files;
            files.push_back(m_Player->GetPlaying()->GetFilename());

            
            
            m_Player->GetLibrary()->AppendStdPlaylist(id, files);
            
            
            // notify player
            //int WM_UPDATEPLAYLIST = RegisterWindowMessage(_T( "UPDATEPLAYLIST"));
            //m_AppWnd->SendMessage(WM_UPDATEPLAYLIST);
            
        }

        if (playlist.GetCount() != 0)
        {
            SendPlaylistAndPlay(playlist);
            
            if (id != -1)
                return TRUE;
        }
    }

    return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CMainDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
    CMenu rootmenu;
    rootmenu.LoadMenu(IDR_CONTEXTMENU);
    CMenu* menu = rootmenu.GetSubMenu(0);
    CMenu* submenu;

    musikCore::PlaylistInfoArray items;

    // add playlists/sources
    submenu = menu->GetSubMenu(MENU_INDEX_STDPLAYLIST);                    // standard playlists
    for (UINT i=0; i<submenu->GetMenuItemCount(); i++)
        submenu->RemoveMenu(0, MF_BYPOSITION);
    m_Player->GetLibrary()->GetAllStdPlaylists(items);
    for ( size_t i = 0; i < items.size(); i++ )
    {
        submenu->InsertMenu(i, MF_BYPOSITION, ID_STDPLAYLIST + items[i].GetID(), items[i].GetName());
    }
    
    submenu = menu->GetSubMenu(MENU_INDEX_DYNPLAYLIST);                    // dynamic playlists
    for (UINT i=0; i<submenu->GetMenuItemCount(); i++)
        submenu->RemoveMenu(0, MF_BYPOSITION);
    m_Player->GetLibrary()->GetAllDynPlaylists(items);
    for ( size_t i = 0; i < items.size(); i++ )
    {
        submenu->InsertMenu(i, MF_BYPOSITION, ID_DYNPLAYLIST + items[i].GetID(), items[i].GetName());
    }

    submenu = menu->GetSubMenu(MENU_INDEX_ADDTOPLAYLIST);                    // add to playlist...
    for (UINT i=0; i<submenu->GetMenuItemCount(); i++)
        submenu->RemoveMenu(0, MF_BYPOSITION);
    m_Player->GetLibrary()->GetAllStdPlaylists(items);
    for ( size_t i = 0; i < items.size(); i++ )
    {
        submenu->InsertMenu(i, MF_BYPOSITION, ID_ADDTOPLAYLIST + items[i].GetID(), items[i].GetName());
    }

    // check states
    if (m_Player->GetPlaymode() & musikCore::MUSIK_PLAYER_PLAYMODE_RANDOM)
        menu->CheckMenuItem(ID_RANDOM, MF_BYCOMMAND | MF_CHECKED);
    else
        menu->CheckMenuItem(ID_RANDOM, MF_BYCOMMAND | MF_UNCHECKED);

    if (m_Player->GetPlaymode() & musikCore::MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST)
        menu->CheckMenuItem(ID_REPEATPLAYLIST, MF_BYCOMMAND | MF_CHECKED);
    else
        menu->CheckMenuItem(ID_REPEATPLAYLIST, MF_BYCOMMAND | MF_UNCHECKED);

    if (m_Player->GetPlaymode() & musikCore::MUSIK_PLAYER_PLAYMODE_REPEAT_SINGLE)
        menu->CheckMenuItem(ID_REPEATSONG, MF_BYCOMMAND | MF_CHECKED);
    else
        menu->CheckMenuItem(ID_REPEATSONG, MF_BYCOMMAND | MF_UNCHECKED);

    if (m_Player->GetPlaymode() & musikCore::MUSIK_PLAYER_PLAYMODE_INTRO)
        menu->CheckMenuItem(ID_INTRO, MF_BYCOMMAND | MF_CHECKED);
    else
        menu->CheckMenuItem(ID_INTRO, MF_BYCOMMAND | MF_UNCHECKED);

    if (m_AlbumMode)
        menu->CheckMenuItem(ID_ALBUMMODE, MF_BYCOMMAND | MF_CHECKED);
    else
        menu->CheckMenuItem(ID_ALBUMMODE, MF_BYCOMMAND | MF_UNCHECKED);

    if (m_ShowRating)
        menu->CheckMenuItem(ID_SHOWRATING, MF_BYCOMMAND | MF_CHECKED);
    else
        menu->CheckMenuItem(ID_SHOWRATING, MF_BYCOMMAND | MF_UNCHECKED);

    if (m_AlwaysOnTop)
        menu->CheckMenuItem(ID_ALWAYSONTOP, MF_BYCOMMAND | MF_CHECKED);
    else
        menu->CheckMenuItem(ID_ALWAYSONTOP, MF_BYCOMMAND | MF_UNCHECKED);

    // songs by the artist
    CString artist = m_Player->GetPlaying()->GetArtist().c_str();
    if (!m_Player->IsPlaying() || artist.GetLength() == 0)
    {
        menu->ModifyMenu(ID_SONGSBYTHEARTIST, MF_BYCOMMAND | MF_STRING, ID_SONGSBYTHEARTIST, _T("Songs by the Artist"));
        menu->EnableMenuItem(ID_SONGSBYTHEARTIST, MF_BYCOMMAND | MF_GRAYED);
    }
    else
    {
        CString str;
        str.Format(
            _T("Find Songs by %s"),
            artist);
        menu->ModifyMenu(ID_SONGSBYTHEARTIST, MF_BYCOMMAND | MF_STRING, ID_SONGSBYTHEARTIST, str);
        menu->EnableMenuItem(ID_SONGSBYTHEARTIST, MF_BYCOMMAND | MF_ENABLED);
    }

    // songs from the album
    CString album = m_Player->GetPlaying()->GetAlbum().c_str();
    if (!m_Player->IsPlaying() || album.GetLength() == 0)
    {
        menu->ModifyMenu(ID_SONGSFROMTHEALBUM, MF_BYCOMMAND | MF_STRING, ID_SONGSFROMTHEALBUM, _T("Find the Album"));
        menu->EnableMenuItem(ID_SONGSFROMTHEALBUM, MF_BYCOMMAND | MF_GRAYED);
    }
    else
    {
        CString str;
        str.Format(
            _T("Find Album \"%s\""),
            album);
        menu->ModifyMenu(ID_SONGSFROMTHEALBUM, MF_BYCOMMAND | MF_STRING, ID_SONGSFROMTHEALBUM, str);
        menu->EnableMenuItem(ID_SONGSFROMTHEALBUM, MF_BYCOMMAND | MF_ENABLED);
    }

    // show
    menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}


LRESULT CMainDlg::OnEditComplete(WPARAM wParam, LPARAM lParam)
{
    CEnterEdit* edit = (CEnterEdit*)wParam;
    if (edit == &m_cSearch)
    {
        CString query;
        edit->GetWindowText(query);

        if (query.Compare(_T("Search")) != 0 && !query.IsEmpty())
        {
            musikCore::Playlist playlist;
            m_Player->GetLibrary()->QuickQuery((musikCore::String)query, playlist);
            if (playlist.GetCount() != 0)
            {
#if defined COMPILE_STANDALONE
                g_PlaylistDlg->ShowWindow( SW_SHOWNORMAL );
                g_PlaylistDlg->SetPlaylist( playlist );
                g_PlaylistDlg->SetFocus();
#else
                SendPlaylistAndPlay(playlist);
                this->SetFocus();
#endif
                edit->SetWindowText(_T("Search"));
            }
        }
    }
    return 0;
}
    
void CMainDlg::OnEnSetfocusMainSearch()
{
    CString str;
    m_cSearch.GetWindowText(str);
    if (str.Compare(_T("Search")) == 0)
        m_cSearch.SetWindowText(_T(""));
}

void CMainDlg::OnEnKillfocusMainSearch()
{
    CString str;
    m_cSearch.GetWindowText(str);
    if (str.GetLength() == 0)
        m_cSearch.SetWindowText(_T("Search"));
    this->SetFocus();
}


void CMainDlg::OnTimer(UINT nIDEvent)
{
    if (IsWindowVisible())
    {
        // only update while playing
        if (!m_Player->IsPaused() && m_Player->IsPlaying())
        {
            UpdateTime();
            
            // until there is a mechanism to get notified of song info/playlist changes:
            // manually update all info
            UpdateNowPlaying();
        }

        m_cVolume.SetPos(m_Player->GetMaxVolume());
        
        // always update buttons (as there is no functor message when playback mode changes)
        //UpdateControls();
    }

    CDialog::OnTimer(nIDEvent);
}


void CMainDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    if (nStatus == SW_PARENTCLOSING)
    {
    }
    else if (nStatus == SW_PARENTOPENING)
    {
    }
    else
    {
        if (bShow)
        {
            RefreshDisplay();
        }
    }
}

void CMainDlg::OnStnClickedMainTime()
{
    m_ShowRemaining = !m_ShowRemaining;
    UpdateTime();
}

void CMainDlg::OnStnClickedMainIndex()
{
    if (!m_ShowRating)
    {
        m_ShowRating = true;
        UpdateNowPlaying();
    }
    else if (m_Player->IsPlaying())
    {
        CPoint ptCurr;
        GetCursorPos(&ptCurr);
        m_cIndex.ScreenToClient(&ptCurr);
        
        RECT rect;
        m_cIndex.GetClientRect(&rect);

        int nRating;
        if (ptCurr.x < 8)
            nRating = 0;
        else
            nRating = (ptCurr.x - 4) * 5 / (rect.right - rect.left) + 1;

        m_Player->GetLibrary()->SetSongRating(m_Player->GetPlaying()->GetID(), nRating);
        m_Player->GetPlaying()->SetRating(musikCore::IntToString(nRating));
        
        UpdateNowPlaying();
    }
}

void CMainDlg::OnBnClickedMainPrev()
{
    m_Player->Prev();
}

void CMainDlg::OnBnClickedMainPlay()
{
    // if it's not playing, play a song
    if ( !m_Player->IsPlaying() )
    {
#ifdef COMPILE_STANDALONE
        m_Player->Play( 0 );
#else
        int PLAYER_PLAYSEL = RegisterWindowMessage( _T( "PLAYER_PLAYSEL" ) );
        m_AppWnd->SendMessage( PLAYER_PLAYSEL );
#endif
    }

    else if ( m_Player->IsPlaying() )
    {
        // if it is playing, pause...
        if ( !m_Player->IsPaused() )
            m_Player->Pause();

        // is paused, resume
        else
            m_Player->Resume();
    }
}

void CMainDlg::OnBnClickedMainStop()
{
    m_Player->Stop();
}

void CMainDlg::OnBnClickedMainNext()
{
    m_Player->Next();
}
/*
void CMainDlg::OnBnClickedMainRepeat()
{
    int playmode = m_Player->GetPlaymode();
    m_Player->SetPlaymode(playmode ^ musikCore::MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST);
    
    UpdateControls();
}
*/
LRESULT CMainDlg::OnTrackSliderMoving(WPARAM wParam, LPARAM lParam)
{
    switch( wParam ) {
    case IDC_MAIN_POSITION:
        //m_Player->SetTimeNowPer(m_cPosition.GetPos());
        break;
    case IDC_MAIN_VOLUME:
        m_Player->SetMaxVolume(m_cVolume.GetPos(), true);
#ifndef COMPILE_STANDALONE
        int WM_UPDATEVOLUME    = RegisterWindowMessage( _T( "UPDATEVOLUME" ) );
        m_AppWnd->SendMessage( WM_UPDATEVOLUME, NULL, NULL );
#endif
        break;
    }
    return 0;
}

LRESULT CMainDlg::OnTrackSliderMoved(WPARAM wParam, LPARAM lParam)
{
    switch( wParam ) {
    case IDC_MAIN_POSITION:
        m_Player->SetTimeNowPer(m_cPosition.GetPos());
        break;
    case IDC_MAIN_VOLUME:
        m_Player->SetMaxVolume(m_cVolume.GetPos(), true);
        break;
    }
    return 0;
}

// draggable window
LRESULT CMainDlg::OnNcHitTest(CPoint point)
{
    LRESULT hit = CDialog::OnNcHitTest(point);
    if ( hit == HTCLIENT ) 
        return HTCAPTION;
    else
        return hit;
}

// needed to allow context menus with the modified OnNcHitTest handler
void CMainDlg::OnNcRButtonUp(UINT nHitTest, CPoint point)
{
   CRect rect;
   GetClientRect(&rect);
   ClientToScreen(&rect);

   if (nHitTest==HTCAPTION && rect.PtInRect(point)) 
      SendMessage(WM_CONTEXTMENU, (WPARAM)m_hWnd, MAKELPARAM(point.x, point.y));
   else
      CDialog::OnNcRButtonUp(nHitTest, point);
}

// keyboard input
UINT CMainDlg::OnGetDlgCode() 
{
    return  /*CDialog::OnGetDlgCode() |*/ DLGC_WANTALLKEYS;
}

// keyboard input
void CMainDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    // TODO: send window messages after changing values
    switch( nChar ) {

    case VK_LEFT:
        m_cPosition.DecPos();
        break;

    case VK_RIGHT:
        m_cPosition.IncPos();
        break;

    case VK_DOWN:
        m_cVolume.DecPos();
        break;

    case VK_UP:
        m_cVolume.IncPos();
        break;

    case VK_SPACE:
        OnBnClickedMainPlay();
        break;

    // Home
    case VK_HOME:
        m_Player->SetTimeNowPer(0);
        break;

    // End
    case VK_END:
        m_Player->SetTimeNowPer(100);
        break;

    // Page up
    case VK_PRIOR:
        m_cVolume.PgUp();
        break;

    // Page down
    case VK_NEXT:
        m_cVolume.PgDn();
        break;

    case VK_TAB:        
        m_cSearch.SetFocus();
        break;

    default:
        CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
        return;
    }
    
    CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CMainDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
    int steps = zDelta / WHEEL_DELTA;
    if (steps > 0)
    {
        for (int i=abs(steps); i>0; i--)
        {
            m_cVolume.PgUp();
            m_Player->SetMaxVolume(m_cVolume.GetPos(), true);
        }
    }
    else
    {
        for (int i=abs(steps); i>0; i--)
        {
            m_cVolume.PgDn();
            m_Player->SetMaxVolume(m_cVolume.GetPos(), true);
        }
    }
    return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

// -- ... -----------------------------------


BEGIN_MESSAGE_MAP(CmusikPluginApp, CWinApp)
END_MESSAGE_MAP()


CmusikPluginApp::CmusikPluginApp()
{
}

// -- ... -----------------------------------

#ifdef COMPILE_STANDALONE

CMainDlg* g_Main = NULL;
CPlaylistWnd* g_PlaylistDlg = NULL;
musikCore::Player* g_Player = NULL;
musikCore::Library* g_Library = NULL;
Functor* g_Functor = NULL;

musikCore::String GetUserDir()
{
    wchar_t buffer[2000];
    GetEnvironmentVariable( _T( "USERPROFILE" ), buffer, sizeof( buffer ) );
    return musikCore::String( buffer );
}

void initCore()
{
    g_Functor = new Functor();
    g_Library = new musikCore::Library( GetUserDir() + _T( "\\.musikproject\\musik_u.db" ) );
    g_Player = new musikCore::PlayerBASS( g_Functor, g_Library );
    g_Player->InitSound( 0, 0, 44100 );

    g_Library->GetAllSongs( *g_Player->GetPlaylist() );
}
void deinitCore()
{
    g_Player->Stop();
    g_Player->Shutdown();
    g_Library->Shutdown();

    delete g_Player;
    delete g_Library;
    delete g_Functor;
}

#endif

// -- ... -----------------------------------

BOOL CmusikPluginApp::InitInstance()
{
    //AfxEnableControlContainer();
    CWinApp::InitInstance();

#ifdef COMPILE_STANDALONE
    InitCommonControls();
    AfxEnableControlContainer();
    initCore();

    g_Main = new CMainDlg( g_Player );
    g_PlaylistDlg = new CPlaylistWnd( g_Main );

    AfxGetApp()->m_pMainWnd = g_Main;
    g_Main->DoModal();
    delete g_Main;
    g_Main = NULL;
    delete g_PlaylistDlg;
    g_PlaylistDlg = NULL;

    deinitCore();
#endif

    // initialize data here
    return TRUE;
}


CmusikPluginApp theApp;


void CMainDlg::OnClose()
{
#ifdef COMPILE_STANDALONE
    m_BoldFont.DeleteObject();
    KillTimer(m_TimerID);
    EndModalLoop( NULL );
    CDialog::OnClose();
#else
    ShowWindow( SW_HIDE );
#endif
}
