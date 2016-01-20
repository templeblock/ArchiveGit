// spview.cpp : implementation of the CShowPalView class
//

#include "stdafx.h"
#include "showpal.h"
#include "dib.h"
#include "dibpal.h"

#include "spdoc.h"
#include "spview.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShowPalView

IMPLEMENT_DYNCREATE(CShowPalView, CView)

BEGIN_MESSAGE_MAP(CShowPalView, CView)
    //{{AFX_MSG_MAP(CShowPalView)
    ON_WM_ERASEBKGND()
    ON_WM_PALETTECHANGED()
    ON_WM_QUERYNEWPALETTE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShowPalView construction/destruction

CShowPalView::CShowPalView()
{
    // TODO: add construction code here
}

CShowPalView::~CShowPalView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CShowPalView drawing

void CShowPalView::OnDraw(CDC* pDC)
{
    CShowPalDoc* pDoc = GetDocument();
    ASSERT(pDoc);
    CDIBPal* pPal = pDoc->GetPalette();
    if (!pPal) return;
    CRect rc;
    GetClientRect(&rc);
    pPal->Draw(pDC, &rc);
}



/////////////////////////////////////////////////////////////////////////////
// CShowPalView diagnostics

#ifdef _DEBUG
void CShowPalView::AssertValid() const
{
    CView::AssertValid();
}

void CShowPalView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CShowPalDoc* CShowPalView::GetDocument() // Non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CShowPalDoc)));
    return (CShowPalDoc*) m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CShowPalView message handlers

BOOL CShowPalView::OnEraseBkgnd(CDC* pDC)
{
    // Fill the background with a hatch brush so that it will show
    // the areas not covered by palette entries.
    CBrush br;
    br.CreateHatchBrush(HS_FDIAGONAL, RGB(128,128,128));
    CRect rc;
    GetClientRect(&rc);
    pDC->FillRect(&rc, &br);
    return TRUE;
}

void CShowPalView::OnPaletteChanged(CWnd* pFocusWnd)
{
    // See if the change was caused by us, and ignore it if not.
    if (pFocusWnd != this) {
        OnQueryNewPalette();
    }
}

// Note: Windows actually ignores the return value
BOOL CShowPalView::OnQueryNewPalette()
{
    // We are going active or the system palette has changed,
    // so realize our palette.
    CShowPalDoc* pDoc = GetDocument();
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
