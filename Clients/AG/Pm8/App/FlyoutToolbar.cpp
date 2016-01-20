// FlyoutToolbar.cpp : implementation file
//

//#include "RenaissanceIncludes.h"
#include "stdafx.h"

ASSERTNAME
#include "FlyoutToolbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_TIMER_WAIT   0xE000  // timer while waiting to show status
#define ID_TIMER_CHECK  0xE001  // timer to check for removal of status

/////////////////////////////////////////////////////////////////////////////
// RFlyoutToolbar

RFlyoutToolbar::RFlyoutToolbar( int nRows ):
	m_nRows(nRows)
{
}

RFlyoutToolbar::~RFlyoutToolbar()
{
}


BEGIN_MESSAGE_MAP(RFlyoutToolbar, CToolBar)
	//{{AFX_MSG_MAP(RFlyoutToolbar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RFlyoutToolbar message handlers

CSize RFlyoutToolbar::GetSize()
{
	//calculate the size of the desired window based on the number of buttons and
	// the number of rows desired.
	CSize szResult = m_sizeButton;
	int nNumButtons = GetCount();
	int nColumns = nNumButtons / m_nRows;

	szResult.cy = szResult.cy * m_nRows + 2;
	szResult.cx = szResult.cx * nColumns;
	return szResult;

}

int RFlyoutToolbar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	//Set all the borders to 0.  This makes the toolbar the exact size of the
	//buttons
	m_cxLeftBorder = 0;
	m_cyTopBorder = 0;
	m_cxRightBorder = 0;
	m_cyBottomBorder = 0;

	if (CToolBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	
	return 0;
}


void RFlyoutToolbar::OnSize(UINT nType, int cx, int cy) 
{
	//need to set this so the toolbar will dynamically resize to the window size.
	//I had to add this to get multiple rows.
	m_nMRUWidth = cx;
	CToolBar::OnSize(nType, cx, cy);
	
	
}

