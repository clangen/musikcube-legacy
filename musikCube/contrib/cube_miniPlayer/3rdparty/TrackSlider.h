#if !defined(AFX_TRACKSLIDER_H__BED36788_B60C_4C9E_AC56_FE430B93A0FD__INCLUDED_)
#define AFX_TRACKSLIDER_H__BED36788_B60C_4C9E_AC56_FE430B93A0FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BitmapSlider.h : header file
//

/////////////////////////////////////////////////////////////////////////////
//
// CTrackSlider -- 
// a variation of CBitmapSlider v1.5
//
// It's free for everywhere - 16/September/2003 - Joon-ho Ryu
//
/////////////////////////////////////////////////////////////////////////////

#include "memdc.h" // "Flicker Free Drawing In MFC" by Keith Rule

// window messages
#define WM_TRACKSLIDER_MOVING		WM_USER + 9425
#define WM_TRACKSLIDER_MOVED		WM_USER + 9426



class CTrackSlider : public CStatic
{
// Construction
public:
	CTrackSlider();
	
public:

	void GetRange( int &nMin, int &nMax ) { nMin = m_nMin; nMax = m_nMax; };
	int GetRangeMax() { return m_nMax; };
	int GetRangeMin() { return m_nMin; };

	int GetPos() { return m_nPos; };

	void SetRange( int nMin, int nMax, BOOL bRedraw=FALSE );
	void SetRangeMin(int nMin, BOOL bRedraw = FALSE);
	void SetRangeMax( int nMax, BOOL bRedraw = FALSE );

	void SetPos( int nPos );
	int SetPageSize( int nSize );

	void IncPos() { IntSetPos( m_nPos+1 ); }
	void DecPos() { IntSetPos( m_nPos-1 ); }
	void PgUp() { IntSetPos( m_nPos+m_nPage ); }
	void PgDn() { IntSetPos( m_nPos-m_nPage ); }

	void SetMargin( int nLeft, int nTop, int nRight, int nBottom );
	void SetMarginTop( int nMargin ) { m_nMarginTop = nMargin; };
	void SetMarginLeft( int nMargin ) { m_nMarginLeft = nMargin; };
	void SetMarginRight( int nMargin ) { m_nMarginRight = nMargin; };
	void SetMarginBottom( int nMargin ) { m_nMarginBottom = nMargin; };

	void SetVertical( BOOL bVertical=TRUE ) { m_bVertical = bVertical; };

	void Enable( BOOL bEnable = TRUE );

	void HandleKeys( BOOL bHandleKeys ) { m_bHandleKeys = bHandleKeys; }

	void DrawFocusRect( BOOL bDraw = TRUE, BOOL bRedraw = FALSE );

	virtual ~CTrackSlider();

	// Generated message map functions
protected:

	void IntSetPos( int nPos );

	int Pixel2Pos( int nPixel );
	int Pos2Pixel( int nPos );

	int m_nMax, m_nMin, m_nPos, m_nPage;

	int m_nMarginLeft, m_nMarginRight, m_nMarginTop, m_nMarginBottom;

	BOOL m_bVertical;
	BOOL m_bChannelActive, m_bThumbActive;
	BOOL m_bLButtonDown, m_bFocus, m_bFocusRect, m_bDrawFocusRect;
	BOOL m_bEnable;
	BOOL m_bHandleKeys;


	//{{AFX_MSG(CTrackSlider)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	// This is CStatic method
	void SetBitmap( HBITMAP hBitmap ) {};
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRACKSLIDER_H__BED36788_B60C_4C9E_AC56_FE430B93A0FD__INCLUDED_)
