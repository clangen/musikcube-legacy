//
// Static Text control with clickable link(URL-EMail) and multiline tooltip support
// by Bulent Ozkir February 10, 2001
// http://www.mindcracker.com/mindcracker/c_cafe/controls/staticlink.asp
//
// StaticLink.cpp : implementation file
//

#include "stdafx.h"

#include "StaticLink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CStaticLink

CStaticLink::CStaticLink()
{
    m_colorUnvisited = RGB(0,0,255);       // blue
    m_colorVisited   = RGB(128,0,128);     // purple
    m_bVisited       = FALSE;              // not visited yet
	//m_hLinkCursor	 = NULL;
	m_hLinkCursor = ::LoadCursor(NULL, IDC_HAND); // IDC_HAND requires WINVER >= 0x0500
	//m_strLink		 = _T("mailto:bulentozkir@hotmail.com");	
}

CStaticLink::~CStaticLink()
{
}


BEGIN_MESSAGE_MAP(CStaticLink, CStatic)
	//{{AFX_MSG_MAP(CStaticLink)
	ON_CONTROL_REFLECT(STN_CLICKED, OnClicked)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_SETCURSOR()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticLink message handlers


void CStaticLink::OnClicked() 
{
	CWaitCursor wait;

	if (m_strLink.IsEmpty())         // if URL/filename not set..
        GetWindowText(m_strLink);    // ..get it from window text

    // Call ShellExecute to run the file.
    // For an URL, this means opening it in the browser.
    //
    HINSTANCE h = ShellExecute(NULL, _T("open"), m_strLink, NULL, NULL, SW_SHOWNORMAL);
    if ((UINT)h > 32)
	{
        m_bVisited = TRUE;       // (not really--might not have found link)
        Invalidate();            // repaint to show visited color
    } else 
	{
        MessageBeep(0);          // unable to execute file!
        TRACE(_T("*** WARNING: CStaticLink: unable to execute file %s\n"),
              (LPCTSTR)m_strLink);
    }

}

HBRUSH CStaticLink::CtlColor(CDC* pDC, UINT nCtlColor)
{
    ASSERT(nCtlColor == CTLCOLOR_STATIC);
    DWORD dwStyle = GetStyle();
    if (!(dwStyle & SS_NOTIFY))
	{
        // Turn on notify flag to get mouse messages and STN_CLICKED.
        // Otherwise, I'll never get any mouse clicks!
        ::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle | SS_NOTIFY);
    }
    
    HBRUSH hbr = NULL;
    if ((dwStyle & 0xFF) <= SS_RIGHT) 
	{

        // this is a text control: set up font and colors
        if (!(HFONT)m_font) 
		{
            // first time init: create font
            LOGFONT lf;
            GetFont()->GetObject(sizeof(lf), &lf);
            lf.lfUnderline = TRUE;
            m_font.CreateFontIndirect(&lf);
        }

        // use underline font and visited/unvisited colors
        pDC->SelectObject(&m_font);
        pDC->SetTextColor(m_bVisited ? m_colorVisited : m_colorUnvisited);
        pDC->SetBkMode(TRANSPARENT);

        // return hollow brush to preserve parent background color
        hbr = (HBRUSH)::GetStockObject(HOLLOW_BRUSH);
    }
    return hbr;

	UNREFERENCED_PARAMETER(nCtlColor);
}


BOOL CStaticLink::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// If a cursor was specified then use it!
	if (m_hLinkCursor != NULL)
	{
		::SetCursor(m_hLinkCursor);
		return TRUE;
	}

	return CStatic::OnSetCursor(pWnd, nHitTest, message);
}



BOOL CStaticLink::PreTranslateMessage(MSG* pMsg) 
{
	{
		// Let the ToolTip process this message.
		m_tooltip.RelayEvent(pMsg);
	}
	
	return CStatic::PreTranslateMessage(pMsg);
}

void CStaticLink::PreSubclassWindow() 
{
	{
		// Create the ToolTip control.
		m_tooltip.Create(this);
		m_tooltip.Activate(TRUE);

		// TODO: Use one of the following forms to add controls:
		m_tooltip.AddTool(this, m_strLink);
		m_tooltip.SetMaxTipWidth(120);
	}
	
	CStatic::PreSubclassWindow();
}

void CStaticLink::SetLink(LPCTSTR lpszLink)
{
	m_strLink = lpszLink;
	this->m_tooltip.UpdateTipText(lpszLink, this);
	AdjustSize();
}

void CStaticLink::SetWindowText(LPCTSTR lpszString)
{
	::CStatic::SetWindowText(lpszString);
	AdjustSize();
}

void CStaticLink::AdjustSize()
{
	// adjust width of link text
	CClientDC dc1(this);
	CFont *pFontOld = dc1.SelectObject(this->GetFont());

	CString str;
	GetWindowText(str);
	CSize size = dc1.GetTextExtent(str);

	this->SetWindowPos(NULL, 0, 0, size.cx, size.cy, SWP_NOZORDER | SWP_NOMOVE);
	dc1.SelectObject(pFontOld);
	DeleteObject(dc1);
}