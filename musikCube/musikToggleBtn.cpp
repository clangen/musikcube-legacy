///////////////////////////////////////////////////

#include "stdafx.h"
#include "musikToggleBtn.h"

///////////////////////////////////////////////////

CmusikToggleBtn::CmusikToggleBtn( Gdiplus::Bitmap* inUnchecked, Gdiplus::Bitmap* inChecked )
: CButtonST()
{
	SetBitmaps( inUnchecked, inChecked );

	SetColor( 2, ::ShadeColor( musikCube::g_Prefs->GetBtnFaceColor(), 10 ), FALSE );
	SetColor( 3, musikCube::g_Prefs->GetBtnTextColor(), FALSE );

	SetColor( 4, ::ShadeColor( musikCube::g_Prefs->GetBtnFaceColor(), 10 ), FALSE );
	SetColor( 5, musikCube::g_Prefs->GetBtnTextColor(), FALSE );

	SetColor( 0, ::ShadeColor( musikCube::g_Prefs->GetBtnFaceColor(), 20 ), FALSE );
	SetColor( 1, musikCube::g_Prefs->GetBtnTextColor(), FALSE );

	DrawBorder( TRUE, FALSE );
}

///////////////////////////////////////////////////

void CmusikToggleBtn::SetBitmaps( Gdiplus::Bitmap* inUnchecked, Gdiplus::Bitmap* inChecked )
{
	m_UncheckedBmp = inUnchecked;
	m_CheckedBmp = inChecked;
}

///////////////////////////////////////////////////

DWORD CmusikToggleBtn::OnDrawBorder( CDC* pDC, CRect* rect )
{
    CRect rcBorder = *rect;
    pDC->Draw3dRect( 
        rcBorder, 
        musikCube::g_Prefs->GetBtnShadowColor(), 
        musikCube::g_Prefs->GetBtnShadowColor() );


	if ( m_bIsPressed )
	{
		pDC->Draw3dRect( 
			rcBorder, 
			musikCube::g_Prefs->GetBtnShadowColor(),
			musikCube::g_Prefs->GetBtnShadowColor() );
	}

    return BTNST_OK;
}

///////////////////////////////////////////////////

DWORD CmusikToggleBtn::OnDrawBackground( CDC* pDC, CRect* rect )
{
	COLORREF crColor;
	if ( m_bIsPressed )
	{
		crColor = ::ShadeColor( musikCube::g_Prefs->GetBtnFaceColor(), -40 );
	}
	else if ( m_bMouseOnButton )
	{
		crColor = ::ShadeColor( musikCube::g_Prefs->GetBtnFaceColor(), 20 );
	}
	else
	{
		crColor = ::ShadeColor( musikCube::g_Prefs->GetBtnFaceColor(), 10 );
	}

	CBrush brBackground( crColor );
	pDC->FillRect( rect, &brBackground );

	if ( m_bIsPressed )
	{
		COLORREF clrLight = ::ShadeColor( musikCube::g_Prefs->GetBtnFaceColor(), 20 );

		CRect light = *rect;
		light.DeflateRect( 1, 1, 1, 1 );
		pDC->Draw3dRect( 
			&light, 
			clrLight, 
			clrLight 
		);
	}

	if ( musikCube::g_DrawGraphics )
	{
		Gdiplus::Bitmap* bmp = NULL;
		m_bMouseOnButton ? bmp = m_UncheckedBmp : bmp = m_CheckedBmp;

		if ( bmp )
		{
			int l = ( rect->Width() / 2 ) - 9;
			int t = ( rect->Height() / 2 ) - 9;

			if ( m_bIsPressed && !GetCheck() && m_bMouseOnButton )
			{
				l++;
				t++;
			}

			Gdiplus::Graphics graphics( pDC->GetSafeHdc() );
			graphics.DrawImage( bmp, l, t, 18, 18 );
		}
	}

    return BTNST_OK;
}	

///////////////////////////////////////////////////
