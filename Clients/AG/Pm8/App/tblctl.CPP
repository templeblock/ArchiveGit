/*
// $Workfile: tblctl.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/tblctl.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 4     6/17/98 11:09a Jayn
// 
// 3     6/03/98 1:46p Jayn
// Table Autoformat changes
// 
// 2     6/02/98 3:00p Jayn
// 
// 1     6/01/98 9:57a Jayn
*/ 

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"

#include "tblctl.h"
#include "tblobj.h"
#include "util.h"
#include "utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern BYTE far screen_gamma_curve[];
extern CPalette * near pOurPal;

/////////////////////////////////////////////////////////////////////////////
// CTableFormatControl

WNDPROC CTableFormatControl::m_pfnSuperWndProc = NULL;

LONG CALLBACK AFX_EXPORT CTableFormatControl::FirstMsgOnlyWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	/*
	// Construct the object and then attach the hWnd to it.
	*/

	CTableFormatControl* pCellOutlineControl = new CTableFormatControl;
	pCellOutlineControl->Attach(hWnd);

	pCellOutlineControl->m_fCreatedByClassName = TRUE;

	/*
	// Subclass the window by changing the window procedure to AfxWndProc
	// and pass the current message to it.
	*/

	::SetWindowLong(hWnd, GWL_WNDPROC, (DWORD)AfxWndProc);

	return ::CallWindowProc((WNDPROC)AfxWndProc, hWnd, msg, wParam, lParam);
}

BOOL CTableFormatControl::RegisterMyClass()
{
	WNDCLASS wcls;

	m_pfnSuperWndProc = ::DefWindowProc;

	static const char szClassName[] = "TableAutoFormatControl";

	if (::GetClassInfo(AfxGetInstanceHandle(), szClassName, &wcls))
	{
		return (wcls.lpfnWndProc == CTableFormatControl::FirstMsgOnlyWndProc);
	}
	
	wcls.style = CS_GLOBALCLASS | CS_DBLCLKS;
	wcls.cbClsExtra = 0;
	wcls.cbWndExtra = 0;
	wcls.lpfnWndProc = CTableFormatControl::FirstMsgOnlyWndProc;
	wcls.hIcon = NULL;
	wcls.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcls.hInstance = AfxGetInstanceHandle();
//	wcls.hbrBackground = (HBRUSH)(COLOR_BACKGROUND+1);
	wcls.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);
	wcls.lpszMenuName = NULL;
	wcls.lpszClassName = szClassName;

	return (RegisterClass(&wcls) != 0);
}

void CTableFormatControl::PostNcDestroy()
{
	if (m_fCreatedByClassName)
	{
		delete this;
	}
}

CTableFormatControl::CTableFormatControl()
{
	m_fCreatedByClassName = FALSE;

	m_pTable = NULL;
	m_pDoc = NULL;
}

CTableFormatControl::~CTableFormatControl()
{
	// Get rid of the document if we created one.
	if (m_pDoc != NULL)
	{
		m_pDoc->OnCloseDocument();
	}
	// The table object will be deleted by the document closure.
}

void CTableFormatControl::Init(void)
{
	// First, get the client rect.
	CRect crClient;
	GetClientRect(crClient);
	ASSERT(crClient.left == 0);
	ASSERT(crClient.top == 0);

	// Do whatever we want to the client rect.
	crClient.InflateRect(-4, -4);		 // Leave some room around the outside.

	// Set it for later use.
	m_crClient = crClient;

	// Set up a redisplay context to use for generating widths, etc.
	// Most of what we need is set default by the rc constructor.
	m_rc.scaled_source_x0 = 0;
	m_rc.scaled_source_y0 = 0;
	m_rc.destination_x0 = m_crClient.left;
	m_rc.destination_y0 = m_crClient.top;
	m_rc.destination_rect = m_crClient;
	m_rc.clip_rect = m_rc.destination_rect;

	m_rc.outline_gamma_curve =
		m_rc.bitmap_gamma_curve = screen_gamma_curve;

	CDC* pDC = GetDC();
	if (pDC != NULL)
	{
		m_rc.x_resolution = pDC->GetDeviceCaps(LOGPIXELSX);
		m_rc.y_resolution = pDC->GetDeviceCaps(LOGPIXELSY);
		ReleaseDC(pDC);
	}
	else
	{
		ASSERT(FALSE);
		m_rc.x_resolution = m_rc.y_resolution = 96;
	}

	// Compute the source that maps into this.
	PBOX Bound;
	m_rc.screen_to_pbox(&m_rc.destination_rect, &Bound);
	ASSERT(Bound.x0 == 0);
	ASSERT(Bound.y0 == 0);
	m_rc.source_pbox = Bound;

	// Create the document we need to show the object.
	m_pDoc = GET_PMWAPP()->NewHiddenDocument();
	if (m_pDoc == NULL)
	{
		ASSERT(FALSE);
		return;
	}

	// Create a table object to use for previewing.
	PMGDatabase* pDatabase = m_pDoc->get_database();
	TABLE_CREATE_STRUCT tcs;
	tcs.info.m_nRows = 5;
	tcs.info.m_nColumns = 5;
	tcs.bound = m_rc.source_pbox;
	m_pTable = (CTableObject*)pDatabase->new_object(OBJECT_TYPE_Table, &tcs);
	if (m_pTable == NULL)
	{
		return;
	}
	m_pDoc->append_object(m_pTable);
}

BEGIN_MESSAGE_MAP(CTableFormatControl, CWnd)
	//{{AFX_MSG_MAP(CTableFormatControl)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTableFormatControl message handlers

void CTableFormatControl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if (m_pTable != NULL)
	{
		CRect crInterior;
		GetClientRect(crInterior);
		Util::Draw3dBorder(
								 &dc,
								 crInterior,
								 GetSysColor(COLOR_BTNHILIGHT),
								 GetSysColor(COLOR_BTNSHADOW),
								 GetSysColor(COLOR_BTNFACE),
								 GetSysColor(COLOR_BTNTEXT),
								 RGB(255, 255, 255),
								 1,
								 TRUE,
								 TRUE,
								 FALSE);

		// Set the DC.
		m_rc.destination_hdc = dc.GetSafeHdc();
		m_rc.set_info(m_rc.destination_hdc);

		// Establish the palettes if necessary.
		HPALETTE hOurPal = (HPALETTE)pOurPal->GetSafeHandle();
		HPALETTE hOldPal = NULL;
		if (m_rc.info.type == RASTER_TYPE_PALETTE)
		{
			hOldPal = ::SelectPalette(m_rc.destination_hdc, hOurPal, FALSE);
			::RealizePalette(m_rc.destination_hdc);
		}

		REFRESH_EXTENT re;

		// Build the refresh extent.
		re.extent = m_rc.source_pbox;
		re.refresh_type = REFRESH_ALL;
		re.my_object = NULL;
		re.update_object = NULL;
		re.update_state = NULL;

		BOOL fDrawn = !m_pDoc->draw_objects(&re, &m_rc);
		ASSERT(fDrawn);

		if (hOldPal != NULL)
		{
			::SelectPalette(m_rc.destination_hdc, hOldPal, FALSE);
		}
	}
}
