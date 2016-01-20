//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/PGSOrWiz.h 1     3/03/99 6:09p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/PGSOrWiz.h $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 29    11/23/98 5:31p Johno
// 
// 28    11/23/98 3:59p Johno
// Changes for initial checked order item
// 
// 27    11/21/98 6:25p Johno
// Changes for separate collection / cd listboxes
// 
// 26    11/02/98 8:46p Psasse
// added combo boxes for month/year in Confirm dialog
// 
// 25    10/29/98 8:30p Psasse
// check for properly filled in edit fields
// 
// 24    10/29/98 4:52p Jayn
// Fixed the country combo code when the combo is sorted.
// 
// 23    10/28/98 9:11p Psasse
// Added SendAccount info to end of purchase procedure
// 
// 22    10/28/98 1:48p Jayn
// More polish for ordering and downloading.
// 
// 21    10/27/98 9:35p Psasse
// owner draw list box in for order confirmation as well as shipping info
// updated
// 
// 20    10/27/98 7:08p Jayn
// 
// 20    10/27/98 7:08p Jayn
// 
// 19    10/27/98 4:47p Jayn
// More download manager and order wizard refinements.
// 
// 18    10/26/98 9:07p Psasse
// owner-draw buttons in Confirmation dialog
// 
// 17    10/25/98 9:15p Psasse
// Changes to the orderconfirmation dialog, including shipping
// 
// 16    10/24/98 9:14p Psasse
// added purchase and compute total functionality
// 
// 15    10/23/98 9:14p Psasse
// filling in confirm dialog listbox
// 
// 14    10/23/98 6:25p Jayn
// New License routines. Changes to content server API.
// 
// 13    10/22/98 9:20p Psasse
// More work on PriceOrder
// 
// 12    10/22/98 7:05p Jayn
// Fixed the price alignment.
// 
// 11    10/22/98 4:00p Jayn
// 
// 10    10/21/98 9:06p Psasse
// added PriceOrder support
// 
// 9     10/21/98 7:01p Jayn
// Added COnlineOrderFormItem code.
// 
// 8     10/21/98 4:14p Jayn
// More improvements to the order process.
// 
// 7     10/20/98 9:04p Psasse
// Worked on "ConfirmOrder" dialog of online wizard
// 
// 6     10/20/98 5:32p Jayn
// Improving the online order form.
// 
// 5     10/19/98 9:18p Psasse
// added user information functionality to online purchasing
// 
// 4     10/19/98 5:41p Jayn
// Beginnings of the online order form.
// 
// 3     10/19/98 11:17a Psasse
// 
// 2     10/14/98 9:17p Psasse
// initial add of Online OrderForm wizard code
// 
// 1     10/14/98 7:08p Psasse
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef __PGSORWIZ_H__
#define __PGSORWIZ_H__

#include "pmwwiz.h"
#include "pmwdlg.h"
#include "ipathmgr.h"
#include "icontsrv.h"
#include "icontmgr.h"

struct CServerOrderFormInfo;
class CPGSOrderWizard;

/////////////////////////////////////////////////////////////////////////////
// COnlineOrderFormItem

struct COnlineOrderFormItem
{
	BOOL m_fChecked;					// Are we checked?
	BOOL m_fPurchased;				// Is this already purchased?
	BOOL m_fIsCD;						// Is this a CD line item?
	int m_nInfoIndex;					// The index of the info associated with this.
	int m_nListIndex;					// Index in list box	 
};

typedef CArray<COnlineOrderFormItem, COnlineOrderFormItem&> COnlineOrderFormItemArray;

/////////////////////////////////////////////////////////////////////////////
// CPGSWizardPage property page

class CPGSWizardPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CPGSWizardPage)

// Construction
public:
	CPGSWizardPage(UINT uID);

protected:

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPGSWizardPage)
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	virtual BOOL OnSetActive();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL	

protected:

	// Generated message map functions
	//{{AFX_MSG(CPGSWizardPage)
	virtual BOOL OnInitDialog();
   //}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CPGSOrderWizard* GetSheet(void);
	const CServerOrderFormInfo* GetOrderFormInfo(void);
	CServerAccountInfo& GetAccountInfo(void);
	COnlineOrderFormItemArray& GetOrderFormItems(void);

	// For drawing prices on a page.
	void DrawPrice(LPDRAWITEMSTRUCT lpDrawItemStruct);

	// Fill out an item info structure from the currently chosen items.
	void FillItemInfo(CServerPurchaseItemInfo& ItemInfo);

	// Fill out a location info structure.
	void FillLocationInfo(CServerLocationInfo& LocationInfo);

	//for filling out price order request
	void FillPriceOrderInfo(CServerPriceOrderInfo& OrderInfo);
};

/////////////////////////////////////////////////////////////////////////////
// COrderFormListBox

class COrderFormListBox : public CCheckListBox
{
public:
	COrderFormListBox();
	virtual ~COrderFormListBox();
	virtual void DrawItem(LPDRAWITEMSTRUCT pDrawItemStruct);

	void SetPriceAlign(int nPriceAlign)
		{ m_nPriceAlign = nPriceAlign; }
	void SetOrderFormItems(COnlineOrderFormItemArray* pItems)
		{ m_pItems = pItems; }
protected:
	int m_nPriceAlign;
	COnlineOrderFormItemArray* m_pItems;
};
/////////////////////////////////////////////////////////////////////////////
// CPGSOrderFormPage dialog

class CPGSOrderFormPage : public CPGSWizardPage
{
	DECLARE_DYNAMIC(CPGSOrderFormPage)

// Construction
public:
	CPGSOrderFormPage();

// Dialog Data
	//{{AFX_DATA(CPGSOrderFormPage)
	enum { IDD = IDD_ORDER_FORM };
	//}}AFX_DATA
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPGSOrderFormPage)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnSetActive(); 
	//}}AFX_VIRTUAL	

	CString	m_csInitialCheck;

protected:

	// Generated message map functions
	//{{AFX_MSG(CPGSOrderFormPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnComputeTotals();
	afx_msg void OnChkchangeProductList();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void BuildOrderList(void);
	void AddCollectionItem(int nIndex, CString &str);
	void AddCDItem(int nIndex);

	void UpdateItemsFromList(void);
	BOOL SomethingChecked(void);
	void UpdateButtons(void);

protected:
	COrderFormListBox m_ListBox;
	COrderFormListBox	m_ListBoxCD;
	int m_nPriceAlign;
};

/////////////////////////////////////////////////////////////////////////////
// CPGSOrderUserInformationPage dialog

class CPGSOrderUserInformationPage : public CPGSWizardPage
{
	DECLARE_DYNAMIC(CPGSOrderUserInformationPage)

private:

// Construction
public:
	CPGSOrderUserInformationPage();

	// Dialog Data
	//{{AFX_DATA(CPGSOrderUserInformationPage)
	enum { IDD = IDD_ORDER_FORM_INFORMATION };
	CString m_strFirstName;
	CString m_strLastName;
	CString m_strStreet1;
	CString m_strStreet2;
	CString m_strCity;
	CString m_strState;
	CString m_strZipCode;
	CString m_strPhone;
	CString m_strEMail;
	CComboBox m_cbCountry;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPGSOrderUserInformationPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:

	void FromAccountInfo(const CServerAccountInfo& AccountInfo);
	void ToAccountInfo(CServerAccountInfo& AccountInfo);
	CString GetSelectedCountry(void);
	void SetSelectedCountry(LPCSTR pszCountry, LPCSTR pszDefault);


protected:
	// Generated message map functions
	//{{AFX_MSG(CPGSOrderUserInformationPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CConfirmedOrderFormListBox

class CConfirmedOrderFormListBox : public CListBox
{
public:
	CConfirmedOrderFormListBox();
	virtual ~CConfirmedOrderFormListBox();
	virtual void DrawItem(LPDRAWITEMSTRUCT pDrawItemStruct);

	void SetPriceAlign(int nPriceAlign)
		{ m_nPriceAlign = nPriceAlign; }
	void SetOrderFormItems(COnlineOrderFormItemArray* pItems)
		{ m_pItems = pItems; }
protected:
	int m_nPriceAlign;
	COnlineOrderFormItemArray* m_pItems;
};


/////////////////////////////////////////////////////////////////////////////
// CPGSOrderConfirmationPage dialog

class CPGSOrderConfirmationPage : public CPGSWizardPage
{
	DECLARE_DYNAMIC(CPGSOrderConfirmationPage)

// Construction
public:
	CPGSOrderConfirmationPage();

	// Dialog Data
	//{{AFX_DATA(CPGSOrderConfirmationPage)
	enum { IDD = IDD_ORDER_CONFIRM };
	CString	m_strCreditCardNumber;
	CString	m_strCreditCardExpirationMonth;
	CString	m_strCreditCardExpirationYear;
	CString	m_strTotalPrice;
	CListBox m_lbOrderList;
	CComboBox	m_cbCreditCardType;
	CComboBox	m_cbShipping;
	CComboBox	m_cbExpirationMonth;
	CComboBox	m_cbExpirationYear;
	CButton		m_btnSubTotal;
	CButton		m_btnFee;
	CButton		m_btnTax;
	CButton		m_btnTotal;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPGSOrderConfirmationPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnWizardFinish();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPGSOrderConfirmationPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSelchangeShipping();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	void FillCreditCardList(void);
	BOOL FillConfirmedOrderList(void);
	void FillShippingList(void);
	void FillCreditCardExpirationYearList(void);

	void SetButtons(CServerPriceInfo& PriceInfo);
	void LicenseNewProducts(void);

	// Shipping methods filled in by the server.
	CServerShippingInfoArray m_ShippingMethods;

	CString GetSelectedCreditCardType(void);
	CString GetSelectedShippingMethod(void);
	void ToPaymentInfo(CServerPaymentInfo& PaymentInfo);
	void UpdatePrices(void);

	CConfirmedOrderFormListBox m_ListBox;
	int m_nPriceAlign;

};

/////////////////////////////////////////////////////////////////////////////
// CPGSOrderWizard

class CPGSOrderWizard : public CPmwWizard
{
	INHERIT(CPGSOrderWizard, CPmwWizard)
// Construction
public:
	CPGSOrderWizard(const CServerOrderFormInfo* pOrderFormInfo, CServerAccountInfo& pAccountInfo, const CServerCountryInfoArray& Countries, IContentManager* pContentManager, IPathManager* pPathManager, LPCSTR InitialCheck = NULL);

// Attributes
public:

// Operations
public:

	virtual void InitDialog(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPGSOrderWizard)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPGSOrderWizard();

	const CServerOrderFormInfo* GetOrderFormInfo(void) const
		{ return m_pOrderFormInfo; }

	CServerAccountInfo& GetAccountInfo(void)
		{ return m_AccountInfo; }

	COnlineOrderFormItemArray& GetOrderFormItems(void)
		{ return m_Items; }

	const CServerCountryInfoArray& GetCountries(void)
		{ return m_Countries; }

	IContentManager* GetContentManager(void) const
		{ return m_pContentManager; }

	// Generated message map functions
protected:
	//{{AFX_MSG(CPGSOrderWizard)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	// Pages!
	CPGSOrderFormPage m_OrderFormPage;
	CPGSOrderUserInformationPage m_UserInformationPage;
	CPGSOrderConfirmationPage m_ConfirmationPage;

	const CServerOrderFormInfo* m_pOrderFormInfo;
	CServerAccountInfo m_AccountInfo;
	CServerCountryInfoArray m_Countries;
	IContentManager* m_pContentManager;
	COnlineOrderFormItemArray m_Items;
};

#endif // __PGSORWIZ_H__
