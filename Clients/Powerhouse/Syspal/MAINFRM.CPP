// mainfrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "syspal.h"
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    //{{AFX_MSG_MAP(CMainFrame)
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_SIZE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
    // Create a palette.
    // Allocate a logical color palette and fill it with the
    // color table info.
    LOGPALETTE* pPal = (LOGPALETTE*) malloc(sizeof(LOGPALETTE) 
                                 + 256 * sizeof(PALETTEENTRY));
    if (pPal) {
        pPal->palVersion = 0x300;  // Windows 3.0
        pPal->palNumEntries = 256; // table size
        for (int i=0; i<256; i++) {
            pPal->palPalEntry[i].peRed = i;
            pPal->palPalEntry[i].peGreen = 0;
            pPal->palPalEntry[i].peBlue = 0;
            pPal->palPalEntry[i].peFlags = PC_EXPLICIT;
        }
        m_pal.CreatePalette(pPal);
        free (pPal);
    }
    m_bCaptured = FALSE;
}

CMainFrame::~CMainFrame()
{
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnPaint()
{
    CPaintDC dc(this); // Device context for painting
    dc.SelectPalette(&m_pal, FALSE);
    dc.RealizePalette();
    CRect rc;
    GetClientRect(&rc);
    int i, j, top, left, bottom, right;
    for (j=0, top=0; j<m_iYCells; j++, top=bottom) {
        bottom = (j+1) * rc.bottom / m_iYCells + 1;
        for (i=0, left=0; i<m_iXCells; i++, left=right) {
            right = (i+1) * rc.right / m_iXCells + 1;
            CBrush br (PALETTEINDEX(j * m_iXCells + i));
            CBrush* brold = dc.SelectObject(&br);
            dc.Rectangle(left-1, top-1, right, bottom);
            dc.SelectObject(brold);
        }
    }
}

void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
    SetCapture();
    m_bCaptured = TRUE;
    ShowRGB(point);
}

void CMainFrame::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_bCaptured) {
        ReleaseCapture();
        m_bCaptured = FALSE;
        SetWindowText("SysPal"); // Restore title.
    }
}

void CMainFrame::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_bCaptured) {
        ShowRGB(point);
    }
}

void CMainFrame::ShowRGB(CPoint point)
{
    HDC hDC = ::GetDC(NULL);
    ClientToScreen(&point);
    COLORREF rgb = ::GetPixel(hDC, point.x, point.y);
    char buf[64];
    sprintf(buf, "RGB(%d,%d,%d)",
            GetRValue(rgb),
            GetGValue(rgb),
            GetBValue(rgb));
    SetWindowText(buf);
    ::ReleaseDC(NULL, hDC);
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
    CFrameWnd::OnSize(nType, cx, cy);
    
    m_iYCells = GetXCells(cx, cy);
    m_iXCells = 256 / m_iYCells;
}

int CMainFrame::GetXCells(int cx, int cy)
{
    for (int i=1; i<256; i*=2) {
        int j = 256 / i;
        if (i * cy / j >= cx) break;
    }
    return i;
}
