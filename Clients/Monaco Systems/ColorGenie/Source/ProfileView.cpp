// ProfileView.cpp : implementation of the CProfileView class
//

#include "stdafx.h"
#include "ColorGenie.h"

#include "ProfilerDoc.h"
#include "ProfileView.h"
#include "ProfileSheet.h"
#include "PropertyPages.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProfileView

IMPLEMENT_DYNCREATE(CProfileView, CView)

BEGIN_MESSAGE_MAP(CProfileView, CView)
	//{{AFX_MSG_MAP(CProfileView)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProfileView construction/destruction

/////////////////////////////////////////////////////////////////////////////
CProfileView::CProfileView()
{
	//{{AFX_DATA_INIT(CProfileView)
		// NOTE: the ClassWizard will add member initialization here
	m_pPropSheet = NULL;
	m_pPageCalibrate = NULL;
	m_pPageToneAdjust = NULL;
	m_pPageColorData = NULL;
	m_pPageSettings = NULL;
	m_pPageTuning = NULL;
	m_hWndFocus = NULL;
	//}}AFX_DATA_INIT
}

/////////////////////////////////////////////////////////////////////////////
CProfileView::~CProfileView()
{
	delete m_pPropSheet;
	delete m_pPageCalibrate;
	delete m_pPageToneAdjust;
	delete m_pPageColorData;
	delete m_pPageSettings;
	delete m_pPageTuning;
}

/////////////////////////////////////////////////////////////////////////////
void CProfileView::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CProfileView)
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
void CProfileView::OnDraw(CDC* pDC)
{
	// we don't draw because the child window will do it all
}

/////////////////////////////////////////////////////////////////////////////
// CProfileView diagnostics

#ifdef _DEBUG
void CProfileView::AssertValid() const
{
	CView::AssertValid();
}

/////////////////////////////////////////////////////////////////////////////
void CProfileView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

/////////////////////////////////////////////////////////////////////////////
CProfileDoc* CProfileView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CProfileDoc)));
	return (CProfileDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
void CProfileView::OnSize(UINT nType, int cx, int cy)
{
	if (cx >= 0 || cy >= 0 || !m_pPropSheet)
	{
		CView::OnSize(nType, cx, cy);
		return;
	}

	// get the size of the property sheet
	CRect rectSized;
	m_pPropSheet->GetWindowRect(rectSized);

	// calculate the size of the document frame
	CFrameWnd* pFrame = GetParentFrame();
	if (!pFrame)
		return;

	pFrame->CalcWindowRect(rectSized);
	CWnd* pParent = pFrame->GetParent();
	if (pParent)
		pParent->ScreenToClient(rectSized);

	// resize and reposition the document frame
	pFrame->MoveWindow(rectSized);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
	DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	ASSERT(pParentWnd != NULL);
	ASSERT_KINDOF(CFrameWnd, pParentWnd);

	if (!CWnd::Create(lpszClassName, lpszWindowName, dwStyle | WS_CLIPCHILDREN, rect, pParentWnd, nID, pContext))
		return FALSE;

	// Create the property sheet
	m_pPropSheet = new CProfileSheet();
	if (!m_pPropSheet)
		return FALSE;

	m_pPropSheet->EnableStackedTabs(FALSE);

	// Create the property sheet pages
	m_pPageCalibrate = new CCalibratePage;
	m_pPageToneAdjust = new CToneAdjustPage;
	m_pPageColorData = new CColorDataPage;
	m_pPageSettings = new CSettingsPage;
	m_pPageTuning = new CTuningPage(this);

	// Add the property pages to the property sheet
	m_pPropSheet->AddPage(m_pPageCalibrate);
	m_pPropSheet->AddPage(m_pPageToneAdjust);
	m_pPropSheet->AddPage(m_pPageColorData);
	m_pPropSheet->AddPage(m_pPageSettings);
	m_pPropSheet->AddPage(m_pPageTuning);

	// Create the property sheet window, but don't display it
	if (!m_pPropSheet->Create(this, DS_CONTEXTHELP | DS_SETFONT | WS_CHILD))
	{
		DestroyWindow();
		return FALSE;
	}

	// Make sure the Color Data tab is enabled and selected
	// Be sure to do this after the Create(), which calls InitDialog()
	m_pPropSheet->SetActivePage(m_pPageCalibrate);	
	CApp* app = (CApp*)AfxGetApp( );
	if(app && app->mhasDongle) m_pPropSheet->SetActivePage(m_pPageColorData);	

	// Move the property sheet to the upper left corner (0,0) of the parent
	m_pPropSheet->SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);

	// we use the style from the template - but make sure that
	// the WS_BORDER bit is correct.
	// the WS_BORDER bit will be whatever is in dwRequestedStyle
	m_pPropSheet->ModifyStyle(WS_BORDER|WS_CAPTION, dwStyle & (WS_BORDER|WS_CAPTION));

	// Force the parent to be just big enough to hold the child
	OnSize(SIZE_RESTORED, -1, -1);

	m_pPropSheet->ShowWindow(SW_NORMAL);
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
void CProfileView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// Called when a document's view changes
	// Update the displayed pages here
	if (lHint == 1)
	{
		CProfileDoc* pDoc = GetDocument();
		((CCalibratePage*)m_pPageCalibrate)->GetDataFromDoc(pDoc);
		((CToneAdjustPage*)m_pPageToneAdjust)->GetDataFromDoc(pDoc);
		((CColorDataPage*)m_pPageColorData)->GetDataFromDoc(pDoc);
		((CSettingsPage*)m_pPageSettings)->GetDataFromDoc(pDoc);
		((CTuningPage*)m_pPageTuning)->GetDataFromDoc(pDoc);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CProfileView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	if (SaveFocusControl())
		return;     // don't call base class when focus is already set

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

/////////////////////////////////////////////////////////////////////////////
void CProfileView::OnActivateFrame(UINT nState, CFrameWnd* /*pFrameWnd*/)
{
	if (nState == WA_INACTIVE)
		SaveFocusControl();     // save focus when frame loses activation
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileView::SaveFocusControl()
{
	// save focus window if focus is on this window's controls
	HWND hWndFocus = ::GetFocus();
	if (hWndFocus != NULL && ::IsChild(m_hWnd, hWndFocus))
	{
		m_hWndFocus = hWndFocus;
		return TRUE;
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
void CProfileView::OnSetFocus(CWnd*)
{
	if (!::IsWindow(m_hWndFocus))
	{
		// invalid or unknown focus window... let windows handle it
		m_hWndFocus = NULL;
		Default();
		return;
	}

	// otherwise, set focus to the last known focus window
	::SetFocus(m_hWndFocus);
}
