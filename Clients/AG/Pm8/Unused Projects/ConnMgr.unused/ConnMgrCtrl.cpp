// ConnMgrCtl.cpp : Implementation of the CConnMgrCtrl ActiveX Control class.

#include "stdafx.h"
#include "ConnMgr.h"
#include "ConnMgrCtrl.h"
#include "ConnMgrPpg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CConnMgrCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CConnMgrCtrl, COleControl)
	//{{AFX_MSG_MAP(CConnMgrCtrl)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CConnMgrCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CConnMgrCtrl)
	DISP_FUNCTION(CConnMgrCtrl, "Connect", Connect, VT_I4, VTS_NONE)
	DISP_FUNCTION(CConnMgrCtrl, "IsConnected", IsConnected, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CConnMgrCtrl, "EditPrefs", EditPrefs, VT_I4, VTS_NONE)
	DISP_FUNCTION(CConnMgrCtrl, "HangUp", HangUp, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CConnMgrCtrl, "EnableIsConnectedCheck", EnableIsConnectedCheck, VT_BOOL, VTS_BOOL)
	DISP_FUNCTION(CConnMgrCtrl, "EnableHangUpPrompt", EnableHangUpPrompt, VT_BOOL, VTS_BOOL)
	DISP_FUNCTION(CConnMgrCtrl, "SetURL", SetURL, VT_EMPTY, VTS_BSTR)
	DISP_STOCKPROP_CAPTION()
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CConnMgrCtrl, COleControl)
	//{{AFX_EVENT_MAP(CConnMgrCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CConnMgrCtrl, 1)
	PROPPAGEID(CConnMgrPropPage::guid)
END_PROPPAGEIDS(CConnMgrCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CConnMgrCtrl, "CONNMGR.ConnMgrCtrl.1",
	0x94fb2c46, 0x512e, 0x11d2, 0x8b, 0xef, 0, 0xa0, 0xc9, 0xb1, 0x2c, 0x3d)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CConnMgrCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DConnMgr =
		{ 0x94fb2c44, 0x512e, 0x11d2, { 0x8b, 0xef, 0, 0xa0, 0xc9, 0xb1, 0x2c, 0x3d } };
const IID BASED_CODE IID_DConnMgrEvents =
		{ 0x94fb2c45, 0x512e, 0x11d2, { 0x8b, 0xef, 0, 0xa0, 0xc9, 0xb1, 0x2c, 0x3d } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwConnMgrOleMisc =
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CConnMgrCtrl, IDS_CONNMGR, _dwConnMgrOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CConnMgrCtrl::CConnMgrCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CConnMgrCtrl

BOOL CConnMgrCtrl::CConnMgrCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_CONNMGR,
			IDB_CONNMGR,
			afxRegApartmentThreading,
			_dwConnMgrOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CConnMgrCtrl::CConnMgrCtrl - Constructor

CConnMgrCtrl::CConnMgrCtrl()
{
	InitializeIIDs(&IID_DConnMgr, &IID_DConnMgrEvents);

	// TODO: Initialize your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CConnMgrCtrl::~CConnMgrCtrl - Destructor

CConnMgrCtrl::~CConnMgrCtrl()
{
	// TODO: Cleanup your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CConnMgrCtrl::OnDraw - Drawing function

void CConnMgrCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	// TODO: Replace the following code with your own drawing code.
	pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
	pdc->Ellipse(rcBounds);
}


/////////////////////////////////////////////////////////////////////////////
// CConnMgrCtrl::DoPropExchange - Persistence support

void CConnMgrCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CConnMgrCtrl::GetControlFlags -
// Flags to customize MFC's implementation of ActiveX controls.
//
// For information on using these flags, please see MFC technical note
// #nnn, "Optimizing an ActiveX Control".
DWORD CConnMgrCtrl::GetControlFlags()
{
	DWORD dwFlags = COleControl::GetControlFlags();


	// The control can activate without creating a window.
	// TODO: when writing the control's message handlers, avoid using
	//		the m_hWnd member variable without first checking that its
	//		value is non-NULL.
//	dwFlags |= windowlessActivate;
	return dwFlags;
}


/////////////////////////////////////////////////////////////////////////////
// CConnMgrCtrl::OnResetState - Reset control to default state

void CConnMgrCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CConnMgrCtrl message handlers

long CConnMgrCtrl::Connect() 
{
   return m_connMgr.Connect();
}

BOOL CConnMgrCtrl::IsConnected() 
{
   return m_connMgr.IsConnected();
}

long CConnMgrCtrl::EditPrefs() 
{
   return m_connMgr.EditPrefs();
}

BOOL CConnMgrCtrl::HangUp() 
{
   m_connMgr.HangUp();
	return TRUE;
}

int CConnMgrCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

   CConnMgrApp *pApp = (CConnMgrApp *) AfxGetApp();
   m_connMgr.SetParent(lpCreateStruct->hwndParent);
   pApp->SetMainWnd(lpCreateStruct->hwndParent);
	
	return 0;
}

BOOL CConnMgrCtrl::EnableIsConnectedCheck(BOOL bEnable) 
{
   m_connMgr.EnableIsConnectedCheck(bEnable);

	return TRUE;
}

BOOL CConnMgrCtrl::EnableHangUpPrompt(BOOL bEnable) 
{
   m_connMgr.EnableHangUpPrompt(bEnable);

	return TRUE;
}

void CConnMgrCtrl::SetURL(LPCTSTR szURL) 
{
   m_connMgr.SetURL(szURL);
}
