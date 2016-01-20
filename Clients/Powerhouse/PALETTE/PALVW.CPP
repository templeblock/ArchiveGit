// palvw.cpp : implementation of the CPaletteView class
//

#include "stdafx.h"
#include "palette.h"
#include "dib.h"
#include "dibpal.h"
#include "colordlg.h"
#include "paldoc.h"
#include "palvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaletteView

IMPLEMENT_DYNCREATE(CPaletteView, CView)

BEGIN_MESSAGE_MAP(CPaletteView, CView)
    //{{AFX_MSG_MAP(CPaletteView)
    ON_WM_ERASEBKGND()
    ON_WM_PALETTECHANGED()
    ON_WM_QUERYNEWPALETTE()
    ON_WM_LBUTTONDBLCLK()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaletteView construction/destruction

CPaletteView::CPaletteView()
{
    // TODO: add construction code here
}

CPaletteView::~CPaletteView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CPaletteView drawing

void CPaletteView::OnDraw(CDC* pDC)
{
    CPaletteDoc* pDoc = GetDocument();
    ASSERT(pDoc);
    CDIBPal* pPal = pDoc->GetPalette();
    if (!pPal) return;
    CRect rc;
    GetClientRect(&rc);
    pPal->Draw(pDC, &rc);
}



/////////////////////////////////////////////////////////////////////////////
// CPaletteView diagnostics

#ifdef _DEBUG
void CPaletteView::AssertValid() const
{
    CView::AssertValid();
}

void CPaletteView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CPaletteDoc* CPaletteView::GetDocument() // Non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPaletteDoc)));
    return (CPaletteDoc*) m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPaletteView message handlers

BOOL CPaletteView::OnEraseBkgnd(CDC* pDC)
{
    // Fill the background with a hatch brush so it will show up
    // the areas not covered by palette entries.
    CBrush br;
    br.CreateHatchBrush(HS_FDIAGONAL, RGB(128,128,128));
    CRect rc;
    GetClientRect(&rc);
    pDC->FillRect(&rc, &br);
    return TRUE;
}

void CPaletteView::OnPaletteChanged(CWnd* pFocusWnd)
{
    // See if the change was caused by us, and ignore it if not.
    if (pFocusWnd != this) {
        OnQueryNewPalette();
    }
}

// Note: Windows actually ignores the return value.
BOOL CPaletteView::OnQueryNewPalette()
{
    // We are going active or the system palette has changed
    // so realize our palette.
    CPaletteDoc* pDoc = GetDocument();
    ASSERT(pDoc);
    CDIBPal* pPal = pDoc->GetPalette();
    if (pPal) {
        CDC* pDC = GetDC();
        CPalette* pOldPal = pDC->SelectPalette(pPal, FALSE);
        UINT u = pDC->RealizePalette();
        ReleaseDC(pDC);
        if (u != 0) {
            // Some colors changed so we need to do a repaint.
            InvalidateRect(NULL, TRUE); // Repaint the lot.
        }
    }
    return TRUE; // Windows ignores this.
}

// Update the view to reflect some change in the document.
void CPaletteView::OnUpdate(CView* pSender,
                        LPARAM lHint,
                        CObject* pHint)
{
    CDC* pDC = GetDC();
    OnDraw(pDC);
}

void CPaletteView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    CPaletteDoc* pDoc = GetDocument();
    ASSERT(pDoc);
    CDIBPal* pPal = pDoc->GetPalette();
    if (!pPal) return;
    // Compute the palette index.
    CRect rc;
    GetClientRect(&rc);
    int i = point.x * 16 / rc.right;
    int j = point.y * 16 / rc.bottom;
    int index = j * 16 + i;
    // If the index is greater than the number of colors
    // expand the number of colors by one and alter the
    // index to the new color slot.
    int iColors = pPal->GetNumColors();
    if (index >= iColors) {
        ASSERT(index < 256);
        pPal->ResizePalette(iColors+1);
        index = iColors;
        iColors++;
        pDoc->SetModifiedFlag(TRUE);
    }

    // Get the color.
    PALETTEENTRY pe;
    pPal->GetPaletteEntries(index, 1, &pe);
    // Show the color chooser dialog.
    CColorDlg dlg;
    dlg.m_red = pe.peRed;
    dlg.m_green = pe.peGreen;
    dlg.m_blue = pe.peBlue;
    int iResult = dlg.DoModal();
    // Update the color (or restore the old one).
    pe.peRed = dlg.m_red;
    pe.peGreen = dlg.m_green;
    pe.peBlue = dlg.m_blue;
    pPal->SetPaletteEntries(index, 1, &pe);
    if (iResult == IDOK) {
        pDoc->SetModifiedFlag(TRUE);
    }
    // See if we need to delete it.
    if (iResult == IDC_DELETE) {
        // Get all the entries from the 
        // current index onwards.
        PALETTEENTRY pe[256];
        pPal->GetPaletteEntries(index,
                                iColors-index,
                                pe);
        // Move each entry down one slot, replacing the 
        // current one.
        if (iColors-index > 1) {
            pPal->SetPaletteEntries(index,
                                    iColors-index,
                                    &pe[1]);
        }
        // Resize the palette.
        iColors--;
        pPal->ResizePalette(iColors);
        pDoc->SetModifiedFlag(TRUE);
    }
    // Repaint the affected area.
    Invalidate();

}
