/*
// $Workfile: MAILORD.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/MAILORD.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:41:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:42   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:40:52   Fred
// Initial revision.
// 
//    Rev 1.1   01 Mar 1996 13:43:56   FRED
// IL sales Tax, and type-in info on mail upgrade
// 
//    Rev 1.0   02 Feb 1996 08:59:00   JAY
// Initial revision.
// 
//    Rev 1.9   24 Feb 1995 14:36:28   FRED
// Restructured product tree. Got rid of CProductForSale
// 
//    Rev 1.8   06 Jan 1995 18:07:56   FRED
//  
// 
//    Rev 1.7   16 Dec 1994 17:09:44   FRED
//  
// 
//    Rev 1.6   05 Dec 1994 16:31:34   JAY
//  
// 
//    Rev 1.5   05 Dec 1994 15:10:46   FRED
//  
// 
//    Rev 1.4   09 Nov 1994 18:25:38   FRED
//  
// 
//    Rev 1.3   09 Nov 1994 17:23:36   JAY
//  
// 
//    Rev 1.2   09 Nov 1994 17:16:40   JAY
//  
// 
//    Rev 1.1   08 Nov 1994 18:28:00   FRED
//  
*/

class COrderByMailDialog;

#ifndef __MAILORD_H__
#define __MAILORD_H__

class CUpgradeDialog;

#include "basedlg.h"
#include "reginfo.h"

/////////////////////////////////////////////////////////////////////////////
// CMailOrderProduct

class CMailOrderProduct
{

public:

	CMailOrderProduct(CString& csName, CString& csPrice);
	virtual ~CMailOrderProduct();
	
public:

	const CString& Name(void)
		{ return m_csName; }
	const CString& Price(void)
		{ return m_csPrice; }

protected:

	CString m_csName;
	CString m_csPrice;
};

/////////////////////////////////////////////////////////////////////////////
// CMailOrderProducts

class CMailOrderProducts : public CPtrArray
{

public:

	CMailOrderProducts();
	virtual ~CMailOrderProducts();

	void RemoveAll(void);
	
	virtual int ProductCount(void) const
		{ return GetSize(); }
	virtual CMailOrderProduct* Product(int nIndex) const
		{ return (CMailOrderProduct*)(GetAt(nIndex)); }
		
	long AddProducts(CProductItems* pProducts);
	void AddItems(CProductItems* pItems, int nIndent, long& lPrice);
};

/////////////////////////////////////////////////////////////////////////////
// COrderByMailDialog dialog

class COrderByMailDialog : public CBaseDialog
{
// Construction
public:
	COrderByMailDialog(CInstallInfo* pInfo, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(COrderByMailDialog)
	enum { IDD = IDD_ORDERBYMAIL };
	CEdit	m_CodeCtrl;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(COrderByMailDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnPrintorderform();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CUpgradeDialog* m_pOrderForm;
	long m_lSubTotal;
	long m_lTax;
	long m_lShipping;

protected:
	CInstallInfo* m_pInfo;
	CString m_csLRC;
};

/////////////////////////////////////////////////////////////////////////////
// COrderFormInformationDialog dialog

class COrderFormInformationDialog : public CRegistrationInformationDialog
{
// Construction
public:
	COrderFormInformationDialog(CInstallInfo* pInfo, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COrderFormInformationDialog)
	enum { IDD = IDD_ORDER_FORM_INFORMATION };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COrderFormInformationDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COrderFormInformationDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnCreditCard();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	BOOL PrintOrderForm(void);

public:
	CString m_csLRC;

protected:
	CInstallInfo* m_pInfo;
	long m_lPrice;
	CMailOrderProducts m_Products;
};

#endif
