// ColorControl.cpp : implementation file
//

#include "afxwin.h"
#include "ColorControl.h"
#include "colortran.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CColorControl, CButton)
	//{{AFX_MSG_MAP(CColorControl)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorControl

/////////////////////////////////////////////////////////////////////////////
CColorControl::CColorControl()
{
	m_lColor = RGB(255, 255, 0);
    m_pBrush = new CBrush(m_lColor);
}

/////////////////////////////////////////////////////////////////////////////
CColorControl::~CColorControl()
{
	delete m_pBrush;
}

/////////////////////////////////////////////////////////////////////////////
void CColorControl::SetRGBColor(COLORREF lColor)
{
	delete m_pBrush;
	m_lColor = lColor;
    m_pBrush = new CBrush(m_lColor);
	Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
void CColorControl::SetLABColor(double* pLab)
{
	if (!pLab)
		return;

	BYTE rgb[3];
	labtorgb(pLab, rgb, 1.8/*gamma*/);
	SetRGBColor(RGB(rgb[0], rgb[1], rgb[2]));
}

/////////////////////////////////////////////////////////////////////////////
// CColorControl message handlers

/////////////////////////////////////////////////////////////////////////////
HBRUSH CColorControl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// Remember that buttons must have the BS_OWNERDRAW flag on in order for this to work...

	// TODO: Return a non-NULL brush if the parent's handler should not be called
	if (!IsWindowEnabled())
		return NULL;
    pDC->SetBkColor(m_lColor);
    return *m_pBrush;
}

void CColorControl::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{ 
	return; 
}