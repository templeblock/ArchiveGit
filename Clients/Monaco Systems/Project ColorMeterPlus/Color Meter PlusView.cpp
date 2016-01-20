// Color Meter PlusView.cpp : implementation of the CColorMeterPlusView class
//

#include "stdafx.h"
#include "Color Meter Plus.h"

#include "Color Meter PlusDoc.h"
#include "Color Meter PlusView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorMeterPlusView

IMPLEMENT_DYNCREATE(CColorMeterPlusView, CView)

BEGIN_MESSAGE_MAP(CColorMeterPlusView, CView)
	//{{AFX_MSG_MAP(CColorMeterPlusView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorMeterPlusView construction/destruction

CColorMeterPlusView::CColorMeterPlusView()
{
	// TODO: add construction code here

}

CColorMeterPlusView::~CColorMeterPlusView()
{
}

BOOL CColorMeterPlusView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CColorMeterPlusView drawing

void CColorMeterPlusView::OnDraw(CDC* pDC)
{
	CColorMeterPlusDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CColorMeterPlusView diagnostics

#ifdef _DEBUG
void CColorMeterPlusView::AssertValid() const
{
	CView::AssertValid();
}

void CColorMeterPlusView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CColorMeterPlusDoc* CColorMeterPlusView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CColorMeterPlusDoc)));
	return (CColorMeterPlusDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CColorMeterPlusView message handlers
