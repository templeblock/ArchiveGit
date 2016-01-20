#if !defined(AFX_DEVICESETUPPAGE_H__E9AE8AC4_A882_11D1_9EE4_006008947D80__INCLUDED_)
#define AFX_DEVICESETUPPAGE_H__E9AE8AC4_A882_11D1_9EE4_006008947D80__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DeviceSetupPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDeviceSetupPage dialog

#define		MAX_NUM_DEVICES (5)
class CDeviceSetupPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDeviceSetupPage)

// Construction
public:
	CDeviceSetupPage();
	~CDeviceSetupPage();

// Dialog Data
	//{{AFX_DATA(CDeviceSetupPage)
	enum { IDD = IDD_SETUP_PAGE };
	CComboBox	m_comboDevice;
	int		m_baudRate;
	int		m_port;
	//}}AFX_DATA

	int		m_deviceType;
	BOOL	m_deviceList[MAX_NUM_DEVICES];

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDeviceSetupPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDeviceSetupPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeDeviceCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEVICESETUPPAGE_H__E9AE8AC4_A882_11D1_9EE4_006008947D80__INCLUDED_)
