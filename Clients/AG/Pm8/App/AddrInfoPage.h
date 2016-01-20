#if !defined(AFX_ADDRINFOPAGE_H__0D78BD42_E8BB_11D1_B06E_00A024EA69C0__INCLUDED_)
#define AFX_ADDRINFOPAGE_H__0D78BD42_E8BB_11D1_B06E_00A024EA69C0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AddrInfoPage.h : header file
//
#include "NumberEdit.h"
#include "StaticSpinCtrl.h"
#include "statice.h"
#include "bmStatic.h"

/////////////////////////////////////////////////////////////////////////////
// CAddrInfoPage dialog

class CAddrInfoPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CAddrInfoPage)

// Construction
public:
	CAddrInfoPage();
	~CAddrInfoPage();

// Dialog Data
	//{{AFX_DATA(CAddrInfoPage)
	enum { IDD = IDD_ADDR_INFO };
	CBmpStatic	m_cImage;
	CComboBox	m_cState;
	CSpinDate	m_cDate;
	CEdit	m_cName;
	CEdit	m_cAddress;
	CEdit	m_cCity;
	CNumberEdit	m_cZip;
	CEdit	m_cCountry;
	CStatic	m_cStaticState;
	CStaticExtended	m_cStaticCode;
	CStatic	m_cStaticCountry;
	CStatic	m_cStaticCity;
	int	m_nDay;
	int	m_nMonth;
	int	m_nYear;
	CString	m_csReturnAddress;
	CString	m_csName;
	CString	m_csTempName;
	CString	m_csAddress;
	CString	m_csTempAddress;
	CString	m_csCity;
	CString	m_csTempCity;
	CString	m_csState;
	CString	m_csZip;
	CString	m_csTempZip;
	CString	m_csCountry;
	int		m_bInternational;
	//}}AFX_DATA
	CString m_csDate;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAddrInfoPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	void PASCAL DDV_Zip(CDataExchange* pDX, int nIDC);
	void PASCAL DDV_Date(CDataExchange* pDX);
	void PASCAL DDV_State(CDataExchange* pDX, CString csState);
	void PASCAL DDV_Empty(CDataExchange* pDX, int nIDC, CString csText);
	void DDX_Date(CDataExchange* pDX, int nIDC, CString& csDate);

// Implementation
protected:
	void OnChangeMonth(BOOL bUp);

	// Generated message map functions
	//{{AFX_MSG(CAddrInfoPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnInternational();
	afx_msg void OnUsa();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CBitmap m_bmpTitle;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDRINFOPAGE_H__0D78BD42_E8BB_11D1_B06E_00A024EA69C0__INCLUDED_)
