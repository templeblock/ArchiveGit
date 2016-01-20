#if !defined(AFX_DEVICESHEET_H__B4C4CE72_6579_11D1_A43E_000094794634__INCLUDED_)
#define AFX_DEVICESHEET_H__B4C4CE72_6579_11D1_A43E_000094794634__INCLUDED_

#include "PatchFormatPage.h"	// Added by ClassView
#include "DeviceSetupPage.h"	// Added by ClassView
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DeviceSheet.h : header file
//


//structure contains the device setup
typedef struct DeviceSetup{
	int numPorts;
	int deviceType;
	int	port;
	int baudRate;
	int calFrequency;
	unsigned char portNames[20];
} DeviceSetup;
	

/////////////////////////////////////////////////////////////////////////////
// CDeviceSheet

class CDeviceSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CDeviceSheet)

// Construction
public:
	CDeviceSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CDeviceSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeviceSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	CDeviceSetupPage m_deviceSetting;
	CPatchFormatPage m_patchFormat;
	virtual ~CDeviceSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDeviceSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEVICESHEET_H__B4C4CE72_6579_11D1_A43E_000094794634__INCLUDED_)
