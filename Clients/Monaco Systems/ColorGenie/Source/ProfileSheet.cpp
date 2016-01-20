// ProfileSheet.cpp : implementation file
//

#include "stdafx.h"
#include "colorgenie.h"
#include "ProfileSheet.h"
#include "PropertyPages.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Initialize the class's static list of images for the tabs
CBitmap* CProfileSheet::m_pTabBitmap[] = {NULL};
int CProfileSheet::m_pSheetCount = 0;

IMPLEMENT_DYNCREATE(CProfileSheet, CPropertySheet)

// Message Map
BEGIN_MESSAGE_MAP(CProfileSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CProfileSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CProfileSheet::CProfileSheet()
{
	//{{AFX_DATA_INIT(CProfileSheet)
	//}}AFX_DATA_INIT

	// If this is the first document, load the tab image bitmaps
	m_pSheetCount++;		   
	if (!m_pTabBitmap[0])
	{
		for (int i=0; i<MAX_TAB_IMAGE_BITMAPS; i++)
		{
			m_pTabBitmap[i] = new CBitmap();
			if (m_pTabBitmap[i])
				m_pTabBitmap[i]->LoadBitmap(MAKEINTRESOURCE(IDB_TAB00+i));
		}
	}

	// Create an image list for the tabs, and load it with the bitmaps
	m_pTabImageList = new CImageList();
	if (!m_pTabImageList)
		return;

	BITMAP bm; m_pTabBitmap[0]->GetObject(sizeof(BITMAP), &bm);
	m_pTabImageList->Create(bm.bmWidth, bm.bmHeight, ILC_COLOR24 | ILC_MASK,
		MAX_TABS/*nTabCount*/, 0 /*No growing images*/);

	// Pure green is transparent.  If you change this to a different color, stick
	// to a "pure" saturated color (using 255's), or...
	// to a "half" saturated color (using 127's),
	// in order to prevent problems at various screens depths
	#define TAB_TRANS_COLOR RGB(0,255,0)
	// After the first document, the bitmap background bcomes black for some reason
	long lColor = (m_pSheetCount <= 1 ? TAB_TRANS_COLOR : RGB(0,0,0));
	for (int i=0; i<MAX_TAB_IMAGE_BITMAPS; i++)
		m_pTabImageList->Add( m_pTabBitmap[i], lColor );

	m_iTabSelected = GetActiveIndex();
}

/////////////////////////////////////////////////////////////////////////////
CProfileSheet::~CProfileSheet()
{
	if (--m_pSheetCount == 0)
	{
		for (int i=0; i<MAX_TAB_IMAGE_BITMAPS; i++)
		{
			if (m_pTabBitmap[i])
				m_pTabBitmap[i]->DeleteObject();
			delete m_pTabBitmap[i];
			m_pTabBitmap[i] = NULL;
		}
	}

	delete m_pTabImageList;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileSheet::OnInitDialog()
{
	// Assign the image list to the tab control
	// Note: Can't call pTab=GetTabControl() and pTab->SetImageList(m_pTabImageList)
	// until after InitDialog().  This is equivalent, and this must be done before InitDialog()
	HWND hWnd;
	if (hWnd = ::GetDlgItem(m_hWnd, AFX_IDC_TAB_CONTROL))
		TabCtrl_SetImageList(hWnd, m_pTabImageList->GetSafeHandle());

	// Call the default InitDialog() function
	BOOL bResult = CPropertySheet::OnInitDialog();

	// Set each tab's state for drawing and activation to "disabled"
	for (int iTab=0 ; iTab<GetPageCount(); iTab++)
		EnableTab(iTab, FALSE);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
int CProfileSheet::TabIsEnabled(int iTab)
{
	return m_bTabEnabled[iTab];
}

/////////////////////////////////////////////////////////////////////////////
void CProfileSheet::EnableTab(int iTab, BOOL bEnable)
{
	m_bTabEnabled[iTab] = bEnable;
	UpdateTab(iTab);
	m_iTabSelected = GetActiveIndex();
}

/////////////////////////////////////////////////////////////////////////////
void CProfileSheet::UpdateTab(int iTab)
{
	// Determine which bitmap to draw
	// State				Start	End
	// enabled, unselected	0		4
	// enabled, selected	5		9
	// disabled				10		14
	int iImageOffset = iTab;
	if (!m_bTabEnabled[iTab])		// If disabled...
		iImageOffset += 2*MAX_TABS;
	else
	if (iTab == GetActiveIndex())	// If selected...
		iImageOffset += MAX_TABS;

	TC_ITEM item;
	memset(&item, 0, sizeof(item));
	item.mask = TCIF_IMAGE;
	item.iImage = iImageOffset;

	HWND hWnd;
	if (hWnd = ::GetDlgItem(m_hWnd, AFX_IDC_TAB_CONTROL))
		TabCtrl_SetItem(hWnd, iTab, &item);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileSheet::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR* pnmh = (NMHDR*)lParam;
	
	// tab is about to changed
	if (TCN_SELCHANGING == pnmh->code)      
	{
		m_iTabSelected = GetActiveIndex();
	}
	else
	// tab has been changed
	if (TCN_SELCHANGE == pnmh->code)      
	{
		int iTab = GetActiveIndex();
		
		//test for Tuning Tab
		if (iTab == 4)
		{
			CTuningPage* tuningpage = (CTuningPage*)GetPage(4);
			if (!TabIsEnabled(iTab)) // the new tab is disabled, so...
				SetActivePage(m_iTabSelected); // activate the previous tab
			else
			{
				if( !tuningpage->IsPageOkToShow() )
				{
					SetActivePage(m_iTabSelected); // activate the previous tab
				}
				else
				{
					UpdateTab(m_iTabSelected);
					m_iTabSelected = iTab;
					UpdateTab(m_iTabSelected);
				}
			}
		}
		else		
		if (!TabIsEnabled(iTab)) // the new tab is disabled, so...
			SetActivePage(m_iTabSelected); // activate the previous tab
		else
		{
			UpdateTab(m_iTabSelected);
			m_iTabSelected = iTab;
			UpdateTab(m_iTabSelected);
		}
	} 
	
	return CPropertySheet::OnNotify(wParam, lParam, pResult);
}
