// ConnMgrPpg.cpp : Implementation of the CConnMgrPropPage property page class.

#include "stdafx.h"
#include "ConnMgr.h"
#include "ConnMgrPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CConnMgrPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CConnMgrPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CConnMgrPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CConnMgrPropPage, "CONNMGR.ConnMgrPropPage.1",
	0x94fb2c47, 0x512e, 0x11d2, 0x8b, 0xef, 0, 0xa0, 0xc9, 0xb1, 0x2c, 0x3d)


/////////////////////////////////////////////////////////////////////////////
// CConnMgrPropPage::CConnMgrPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CConnMgrPropPage

BOOL CConnMgrPropPage::CConnMgrPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_CONNMGR_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CConnMgrPropPage::CConnMgrPropPage - Constructor

CConnMgrPropPage::CConnMgrPropPage() :
	COlePropertyPage(IDD, IDS_CONNMGR_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CConnMgrPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CConnMgrPropPage::DoDataExchange - Moves data between page and properties

void CConnMgrPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CConnMgrPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CConnMgrPropPage message handlers
