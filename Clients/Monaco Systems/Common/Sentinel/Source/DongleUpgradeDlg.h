// DongleUpgradeDlg.h : header file
//

#if !defined(AFX_DongleUpgradeDLG_H__CD41E48B_D443_11D1_9EE4_006008947D80__INCLUDED_)
#define AFX_DongleUpgradeDLG_H__CD41E48B_D443_11D1_9EE4_006008947D80__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Dongle.h"
#include <afxcmn.h>

/////////////////////////////////////////////////////////////////////////////
// CDongleUpgradeDlg dialog

class CDongleUpgradeDlg : public CDialog
{
// Construction
public:
	CDongleUpgradeDlg(WORD wSerialNumber, WORD wApplication, WORD wVersion, WORD wFeature, CWnd* pParent = NULL);	// standard constructor
	~CDongleUpgradeDlg();	// standard constructor
	void GetDay(SYSTEMTIME& SystemTime, int iDayOffset);
	WORD MakeDate(bool bPurchaseNumber, int iDayOffset);
	void GenerateOrderNumber(CString& OrderNumber);
	bool GeneratePurchaseNumber(WORD* pOrderData, CString& PurchaseNumber);
	bool UnscrambleNumber(CString szNumber, WORD* pOrderData);
	bool ValidateData(WORD* pOrderData, bool bPurchaseCheck);
	void ScrambleData(WORD* pOrderData, CString& ScrambleString);

// Dialog Data
	//{{AFX_DATA(CDongleUpgradeDlg)
	CEdit m_Message;
	CString	m_Application;
	CString m_Version;
	CString	m_OrderNumber;
	CString m_PurchaseNumber;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDongleUpgradeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	WORD m_wSerialNumber;
	WORD m_wApplication;
	WORD m_wVersion;
	WORD m_wFeature;

	// Generated message map functions
	//{{AFX_MSG(CDongleUpgradeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnChangeOrderNumber();
	afx_msg void OnChangePurchaseNumber();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DongleUpgradeDLG_H__CD41E48B_D443_11D1_9EE4_006008947D80__INCLUDED_)
