// Qd3dControl.cpp : implementation file
//

#include "stdafx.h"
#include "Qd3dControl.h"
#include "Qd3dExtras.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQd3dControl

BEGIN_MESSAGE_MAP(CQd3dControl, CWnd)
	//{{AFX_MSG_MAP(CQd3dControl)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CQd3dControl::CQd3dControl(RECT rect, CWnd* pWnd, CQd3dGamutBase* pQd3dGamut, int nID)
{
	m_pQd3dGamut = pQd3dGamut;

	// Register the new control class
	CString szClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
		LoadCursor(NULL, IDC_ARROW),			// HCURSOR hCursor
		(HBRUSH)GetStockObject(BLACK_BRUSH),	// HBRUSH hbrBackground
		NULL);									// HICON hIcon

	BOOL bOK = CreateEx( WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE,
		szClass,	// pointer to registered class name
		NULL,		// pointer to window name
		WS_CHILD |
		WS_VISIBLE,	// window style
		rect,		// rect in parent client coord.
		pWnd,		// handle to parent or owner window
		nID			// nID The ID of the child window
	);

	if (!bOK)
		return;
}

/////////////////////////////////////////////////////////////////////////////
CQd3dControl::~CQd3dControl()
{
}

/////////////////////////////////////////////////////////////////////////////
// CQd3dControl message handlers

/////////////////////////////////////////////////////////////////////////////
void CQd3dControl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (m_pQd3dGamut)
		m_pQd3dGamut->DocumentDraw3DData();
}
