// DeviceSheet.cpp : implementation file
//

#include "stdafx.h"
#include "DeviceSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeviceSheet

IMPLEMENT_DYNAMIC(CDeviceSheet, CPropertySheet)

CDeviceSheet::CDeviceSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_deviceSetting);
	AddPage(&m_patchFormat);
}

CDeviceSheet::CDeviceSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_deviceSetting);
	AddPage(&m_patchFormat);
}

CDeviceSheet::~CDeviceSheet()
{
}


BEGIN_MESSAGE_MAP(CDeviceSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CDeviceSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeviceSheet message handlers
