#if !defined(AFX_BILLINGINFOPAGE_H__0D78BD43_E8BB_11D1_B06E_00A024EA69C0__INCLUDED_)
#define AFX_BILLINGINFOPAGE_H__0D78BD43_E8BB_11D1_B06E_00A024EA69C0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BillingInfoPage.h : header file
//
#include "NumberEdit.h"
#include "StaticSpinCtrl.h"
#include "pmwinet.h"
#include "project.h"
#include "progbar.h"
#include "statice.h"
#include "bmStatic.h"


typedef CArray<CString, CString> PriceFileEntry;
/////////////////////////////////////////////////////////////////////////////
// CBillingInfoPage dialog

class CBillingInfoPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CBillingInfoPage)

// Construction
public:
	CBillingInfoPage(CString csFilePath, PROJECT_TYPE CardType);
	~CBillingInfoPage();


// Dialog Data
	//{{AFX_DATA(CBillingInfoPage)
	enum { IDD = IDD_BILLLING_INFO };
	CBmpStatic	m_cImage;
	CNumberEdit	m_cCardNum;
	CStaticExtended	m_cStaticTotal;
	CStaticExtended	m_cStaticTax;
	CStaticExtended	m_cStaticPrice;
	CStaticExtended	m_cStaticPostage;
	CSpinDate	m_cExpDate;
	CString	m_csBillingName;
	CString	m_csBillingAddr;
	static CString	m_csCardHolderName;
	static int m_nCreditCardType;
	static CString	m_csPhoneNum;
	static CString	m_csCardNumber;
	double	m_dPostage;
	double	m_dPrice;
	double	m_dTax;
	double	m_dTotal;
	//}}AFX_DATA
	CString m_csExpDate;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBillingInfoPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//we have to have a default constructor for dynamic creation
	CBillingInfoPage();
	CString GetFtpFileName();
	CString GetCreditCardType(UINT nType);
#ifndef ARTSTORE
	CFtpConnection* GetFtpConnection(CInternetSession* pSession, CString csFtpSite, CString csLogin, CString csPassWord);
#endif
	CString m_csPriceFileTime;

	CString GetFileEntry(FILE* stream, int nIndex, char cTag, CString csData = "" );
	CString m_csDocFilePath;
	CString m_csProductNum;
	double m_dShipping;
	double m_dHandling;
	PROJECT_TYPE m_CardType;
	CPMWProgressBar m_progressDialog;
	CBitmap m_bmpTitle;

	enum 
	{
		SUCCESS = 0,
		URL_ERR,
		FILE_CREATE_FAILED,
		OPERATION_ABORTED,
		FILE_ERROR,
		ERR_NO_CONNECTION,
		INTERNET_EXCEPTION,
		ERR_PROGRESS_BAR,
		OPERATION_INCOMPLETE
	};
	
	UINT DoFtpExchange(CString csLocalPath);
	CString AddTagsToAddress(CString csAddress, UINT nTagStart, UINT nTagEnd);

	UINT UpdatePriceData(CString csNewPriceFileName, BOOL bPrompt = FALSE);
	void DDV_Empty(CDataExchange* pDX, int nIDC, CString csText );
	void DDX_Price(CDataExchange* pDX, int nIDC, double& dPrice);
	void DDV_CardNum(CDataExchange* pDX, int nCardType, CString csCardNumber);
	void DDX_Date(CDataExchange* pDX, int nIDC, CString& csExpDate);
	// Generated message map functions
	//{{AFX_MSG(CBillingInfoPage)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BILLINGINFOPAGE_H__0D78BD43_E8BB_11D1_B06E_00A024EA69C0__INCLUDED_)
