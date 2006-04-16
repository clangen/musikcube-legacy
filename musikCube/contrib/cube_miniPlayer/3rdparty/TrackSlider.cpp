// TrackSlider.cpp : implementation file
//

#include "stdafx.h"
#include "TrackSlider.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//
// CTrackSlider -- 
// a variation of CBitmapSlider v1.5
//
// It's free for everywhere - 16/September/2003 - Joon-ho Ryu
//
/////////////////////////////////////////////////////////////////////////////


CTrackSlider::CTrackSlider()
{
	m_nPos = m_nMin = 0;
	m_nMax = 100;
	m_nPage = 20;

	m_nMarginLeft = m_nMarginTop = 2;
	m_nMarginRight = m_nMarginBottom = 2;

	m_bVertical = m_bLButtonDown = FALSE;

	m_bFocusRect = m_bFocus = FALSE;
	m_bDrawFocusRect = TRUE;

	m_bEnable = TRUE;

	m_bHandleKeys = TRUE;
}

CTrackSlider::~CTrackSlider()
{
}


BEGIN_MESSAGE_MAP(CTrackSlider, CStatic)
	//{{AFX_MSG_MAP(CTrackSlider)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrackSlider message handlers

BOOL CTrackSlider::OnEraseBkgnd(CDC* pDC) 
{	
	// Do not erase background
	return TRUE;
}

// Draw
void CTrackSlider::OnPaint() 
{
	CPaintDC dcOrigin( this );

	CRect rect, slrect;
	GetClientRect( &rect );

	// "Flicker Free Drawing In MFC" by Keith Rule
	CMemDC dc( &dcOrigin, &rect );

	CDC dcMem;
	dcMem.CreateCompatibleDC( &dc );

	// border
	//dc.Draw3dRect( rect, GetSysColor( COLOR_3DSHADOW ), GetSysColor( COLOR_3DHILIGHT ) );
	dc.Draw3dRect( rect, GetSysColor( COLOR_3DSHADOW ), GetSysColor( COLOR_3DSHADOW ) );

	// bg
	slrect = rect;
	slrect.left++; slrect.right--; slrect.top++; slrect.bottom--;
	dc.FillSolidRect( slrect, GetSysColor( COLOR_BTNFACE ) );

	// slider
	slrect = rect;
	slrect.left += m_nMarginLeft; 
	slrect.right -= m_nMarginRight;
	slrect.top += m_nMarginTop; 
	slrect.bottom -= m_nMarginBottom;
	if ( m_bVertical ) {
		slrect.top = slrect.top + ( ( ( m_nMax - m_nMin ) - m_nPos ) * slrect.Height() / ( m_nMax - m_nMin ) );
	}
	else {
		slrect.right = slrect.left + (m_nPos * slrect.Width() / ( m_nMax - m_nMin ) );
	}
	if ( m_bEnable )
		dc.FillSolidRect( slrect, GetSysColor( COLOR_ACTIVECAPTION ) );
	else
		dc.FillSolidRect( slrect, GetSysColor( COLOR_INACTIVECAPTION ) );

	// Draw focus rectangle
	/*
	if( m_bDrawFocusRect && m_bFocus && m_bEnable ) {

		slrect = rect;
		slrect.InflateRect( m_nMarginLeft, m_nMarginTop, m_nMarginRight, m_nMarginBottom );
		dc.DrawFocusRect( slrect );
		m_bFocusRect = TRUE;
	}
	*/

	dcMem.DeleteDC();
}


// Sets the maximum range for the slider.
//
// Parameters:
//		[IN]	nMax
//				Maximum position for the slider.
//		[IN]	bRedraw
//				TRUE to redraw after the range is set.
//				FALSE to only change maximum position.
//
void CTrackSlider::SetRangeMax(int nMax, BOOL bRedraw)
{
	m_nMax = nMax;
	if( bRedraw )
		Invalidate();
}

// Sets the minimum range for the slider.
//
// Parameters:
//		[IN]	nMin
//				Minimum position for the slider.
//		[IN]	bRedraw
//				TRUE to redraw after the range is set.
//				FALSE to only change minimum position.
//
void CTrackSlider::SetRangeMin(int nMin, BOOL bRedraw)
{
	m_nMin = nMin;
	if( bRedraw )
		Invalidate();
}

// Sets the range (minimum and maximum positions) for the slider.
//
// Parameters:
//		[IN]	nMin
//				Minimum position for the slider.
//		[IN]	nMax
//				Maximum position for the slider.
//		[IN]	bRedraw
//				TRUE to redraw after the range is set.
//				FALSE to only change the range.
//
void CTrackSlider::SetRange(int nMin, int nMax, BOOL bRedraw)
{
	SetRangeMin( nMin, FALSE );
	SetRangeMax( nMax, bRedraw );
}

// Sets the current position of the slider.
//
// Parameters:
//		[IN]	nPos
//				Specifies the new slider position.
//
void CTrackSlider::SetPos(int nPos)
{
	// not if the user is currently dragging
	if (m_bLButtonDown)
		return;

	IntSetPos( nPos );
}

void CTrackSlider::IntSetPos( int nPos )
{
	m_nPos = nPos;

	// Boundary check
	if( m_nPos > m_nMax )
		m_nPos = m_nMax;
	if( m_nPos < m_nMin )
		m_nPos = m_nMin;

	Invalidate();
}

// Sets the size of the page for a control.
//
// Parameters:
//		[IN]	nSize
//				The new page size of the control.
//
// Return value:
//		The previous page size.
//
int CTrackSlider::SetPageSize(int nSize)
{
	int nRet = m_nPage;

	m_nPage = nSize;

	return nRet;
}

// Sets the left, top, right, and bottom margins for a control
//
void CTrackSlider::SetMargin(int nLeft, int nTop, int nRight, int nBottom )
{
	SetMarginLeft( nLeft );
	SetMarginTop( nTop );
	SetMarginRight( nRight );
	SetMarginBottom( nBottom );
}

// Enables or disables control.
//
//		[IN]	bEnable
//				TRUE to enable control.
//				FALSE to disable control.
//
void CTrackSlider::Enable(BOOL bEnable)
{
	m_bEnable = bEnable;

	// If control is disabled during dragging
	if( !m_bEnable && m_bLButtonDown ) {

		m_bLButtonDown = FALSE;
		ReleaseCapture();
	}

	Invalidate();
}

// Specify whether draw focus rectangle or not.
//
//		[IN]	bDraw
//				TRUE to draw focus rectangle.
//				FALSE to hide focus rectangle.
//
//		[IN]	bRedraw
//				TRUE to redraw status is changed.
//				FALSE to only change the status.
//
void CTrackSlider::DrawFocusRect(BOOL bDraw, BOOL bRedraw)
{
	m_bDrawFocusRect = bDraw;

	if( bRedraw )
		Invalidate();
}

// OnLButtonDown
//
// Dragging is started
//
void CTrackSlider::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( !m_bEnable )
		return;

	SetCapture();
	SetFocus();

	m_bLButtonDown = TRUE;

	OnMouseMove( nFlags, point );
	Invalidate();

	CStatic::OnLButtonDown(nFlags, point);
}

// OnMouseMove
//
// During dragging
//
void CTrackSlider::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( !m_bLButtonDown || !m_bEnable )
		return;

	CRect rect;
	GetClientRect( &rect );

	int nPixel;

	// Boundary check
	if( m_bVertical ) {

		nPixel = point.y;

		if( nPixel > rect.Height() - m_nMarginBottom )
			nPixel = rect.Height() - m_nMarginBottom;

		if( nPixel < m_nMarginTop )
			nPixel = m_nMarginTop;

	} else {

		nPixel = point.x;

		if( nPixel < m_nMarginLeft )
			nPixel = m_nMarginLeft;

		if( nPixel > rect.Width() - m_nMarginRight )
			nPixel = rect.Width() - m_nMarginRight;
	}

	// Apply change
	if( Pos2Pixel(m_nPos) != nPixel ) {

		IntSetPos( Pixel2Pos( nPixel ) );

		::PostMessage(
			GetParent()->GetSafeHwnd(), WM_TRACKSLIDER_MOVING,
			GetDlgCtrlID(), m_nPos );
	}

	CStatic::OnMouseMove(nFlags, point);
}

// OnLButtonUp
//
// Dragging is finished
//
void CTrackSlider::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( !m_bEnable )
		return;

	ReleaseCapture();
	m_bLButtonDown = FALSE;

	Invalidate();

	::PostMessage(
		GetParent()->GetSafeHwnd(),	WM_TRACKSLIDER_MOVED,
		GetDlgCtrlID(), m_nPos );

	CStatic::OnLButtonUp(nFlags, point);
}

// Calculate point of thumb from position value
//
int CTrackSlider::Pos2Pixel(int nPos)
{
	CRect rect;
	GetClientRect( &rect );
	rect.DeflateRect( m_nMarginLeft, m_nMarginTop, m_nMarginRight, m_nMarginBottom );

	if( m_bVertical ) 
		return rect.Height() - ( m_nMarginTop + nPos * rect.Height() / ( m_nMax - m_nMin ) );
	else
		return m_nMarginLeft + nPos * rect.Width() / ( m_nMax - m_nMin );
}

// Calculate position value from point of mouse
//
int CTrackSlider::Pixel2Pos(int nPixel)
{
	CRect rect;
	GetClientRect( &rect );
	rect.DeflateRect( m_nMarginLeft, m_nMarginTop, m_nMarginRight, m_nMarginBottom );

	if( m_bVertical ) 
		return ( m_nMax - m_nMin ) - ( m_nMin + nPixel * ( m_nMax - m_nMin ) / rect.Height() );
	else 
		return m_nMin + nPixel * ( m_nMax - m_nMin ) / rect.Width();
}


// To get keyboard input
//
UINT CTrackSlider::OnGetDlgCode() 
{
	//if( GetKeyState(VK_TAB) >= 0 ) {
	if ( m_bHandleKeys == TRUE ) {

		return  DLGC_WANTALLKEYS;
	}
	
	return DLGC_STATIC;
	//return CStatic::OnGetDlgCode();
}

// Handling keyboard input
//
void CTrackSlider::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( !m_bEnable || !m_bHandleKeys )
		return;

	switch( nChar ) {

	case VK_LEFT :
	case VK_DOWN :

		IntSetPos( m_nPos-1 );
		break;

	case VK_RIGHT :
	case VK_UP :

		IntSetPos( m_nPos+1 );
		break;

	// Home
	case VK_HOME :

		IntSetPos( m_nMax );
		break;

	// End
	case VK_END :

		IntSetPos( m_nMin );
		break;

	// Page up
	case VK_PRIOR :

		IntSetPos( m_nPos + m_nPage );
		break;

	// Page down
	case VK_NEXT :

		IntSetPos( m_nPos - m_nPage );
		break;

	default :

		CStatic::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}
	
	::PostMessage(
		GetParent()->GetSafeHwnd(),	WM_TRACKSLIDER_MOVED,
		GetDlgCtrlID(), m_nPos );

	CStatic::OnKeyDown(nChar, nRepCnt, nFlags);
}

// Control looses its focus
//
void CTrackSlider::OnKillFocus(CWnd* pNewWnd) 
{
	CStatic::OnKillFocus(pNewWnd);

	m_bFocus = FALSE;
	Invalidate();
}

// This control gains its focus
//
void CTrackSlider::OnSetFocus(CWnd* pOldWnd) 
{
	CStatic::OnSetFocus(pOldWnd);

	m_bFocus = TRUE;
	Invalidate();
}

// Release resources
//
void CTrackSlider::OnDestroy() 
{
	CStatic::OnDestroy();
}
