// colordlg.cpp : implementation file
//

#include "stdafx.h"
#include "dib.h"
#include "dibpal.h"
#include "palette.h"
#include "colordlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorDlg dialog

CColorDlg::CColorDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CColorDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CColorDlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    m_red = 128;
    m_green = 128;
    m_blue = 128;
    m_pPal = NULL;
}

CColorDlg::~CColorDlg()
{
    if (m_pPal) {
        delete m_pPal;
    }
}

void CColorDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CColorDlg)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CColorDlg, CDialog)
    //{{AFX_MSG_MAP(CColorDlg)
    ON_WM_VSCROLL()
    ON_BN_CLICKED(IDC_DELETE, OnClickedDelete)
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorDlg message handlers

void CColorDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    int iPos = pScrollBar->GetScrollPos();
    switch (nSBCode) {
    case SB_TOP:
        iPos = 0;
        break;
    case SB_LINEDOWN:
        iPos++;
        break;
    case SB_PAGEDOWN:
        iPos += 10;
        break;
    case SB_BOTTOM:
        iPos = 255;
        break;
    case SB_LINEUP:
        iPos--;
        break;
    case SB_PAGEUP:
        iPos -= 10;
        break;
    case SB_THUMBTRACK:
    case SB_THUMBPOSITION:
        iPos = nPos;
        break;
    default:
        break;
    }
    if (iPos < 0) iPos = 0;
    if (iPos > 255) iPos = 255;
    pScrollBar->SetScrollPos(iPos, TRUE);

    PALETTEENTRY pe;
    pe.peRed = m_psbRed->GetScrollPos();
    pe.peGreen = m_psbGreen->GetScrollPos();
    pe.peBlue = m_psbBlue->GetScrollPos();
    pe.peFlags = PC_RESERVED;
    CDC* pDC = GetDC();
    CPalette* pOldPal = pDC->SelectPalette(m_pPal, FALSE);
    m_pPal->AnimatePalette(0, 1, &pe);
    pDC->RealizePalette();
    pDC->SelectPalette(pOldPal, FALSE);
    ReleaseDC(pDC);
}

BOOL CColorDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    
    m_psbRed = (CScrollBar*)GetDlgItem(IDC_RED);
    m_psbGreen = (CScrollBar*)GetDlgItem(IDC_GREEN);
    m_psbBlue = (CScrollBar*)GetDlgItem(IDC_BLUE);
    m_psbRed->SetScrollRange(0, 255, FALSE);
    m_psbGreen->SetScrollRange(0, 255, FALSE);
    m_psbBlue->SetScrollRange(0, 255, FALSE);
    m_psbRed->SetScrollPos(m_red, TRUE);
    m_psbGreen->SetScrollPos(m_green, TRUE);
    m_psbBlue->SetScrollPos(m_blue, TRUE);

    // Create a palette with one color we can animate.
    m_pPal = new CDIBPal;
    LOGPALETTE lp;
    lp.palVersion = 0x300;
    lp.palNumEntries = 1;
    lp.palPalEntry[0].peRed = m_red;
    lp.palPalEntry[0].peGreen = m_green;
    lp.palPalEntry[0].peBlue = m_blue;
    lp.palPalEntry[0].peFlags = PC_RESERVED;
    m_pPal->CreatePalette(&lp);

    return TRUE;  // Return TRUE unless you set the focus to a control.
}

// Paint the color rectangle according to the current
// scroll bar positions.
VOID CColorDlg::PaintColor()
{
    // Select the palette into our sample window DC.
    CDC* pDC = GetDC();
    CPalette* pOldPal = pDC->SelectPalette(m_pPal, FALSE);
    pDC->RealizePalette();
    // Create a brush to use.
    CBrush br (PALETTEINDEX(0));
    // Paint the rectangle in the sample window.
    CRect rc (150,135,280,250);
    pDC->FillRect(&rc, &br);
    // Done with DC, brush, and palette.
    pDC->SelectPalette(pOldPal, FALSE);
    ReleaseDC(pDC);
}

void CColorDlg::OnOK()
{
    // Update the return values.
    m_red = m_psbRed->GetScrollPos();
    m_green = m_psbGreen->GetScrollPos();
    m_blue = m_psbBlue->GetScrollPos();
    
    CDialog::OnOK();
}


void CColorDlg::OnClickedDelete()
{
    EndDialog(IDC_DELETE);
}

void CColorDlg::OnPaint()
{
    CDialog::OnPaint();
    PaintColor();
}

