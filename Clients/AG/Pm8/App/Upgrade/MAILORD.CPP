/*
// $Workfile: MAILORD.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/MAILORD.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 2     1/21/98 4:14p Fredf
// Year 2000 compliance.
// 
//    Rev 1.0   14 Aug 1997 15:41:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:33:18   Fred
// Initial revision.
// 
//    Rev 1.2   09 Jul 1997 09:55:20   Fred
// Moved strings to resource file
// 
//    Rev 1.1   14 Oct 1996 13:34:32   Fred
// International Version
// 
//    Rev 1.0   08 Sep 1996 09:40:30   Fred
// Initial revision.
// 
//    Rev 1.2   01 Mar 1996 13:43:58   FRED
// IL sales Tax, and type-in info on mail upgrade
// 
//    Rev 1.1   12 Feb 1996 13:53:56   JAY
// Extra validation.
// 
//    Rev 1.0   02 Feb 1996 08:58:26   JAY
// Initial revision.
// 
//    Rev 1.27   22 Dec 1995 14:35:54   FRED
// Changed CDATE to LDATE.
// 
// 
//    Rev 1.26   07 Dec 1995 08:03:48   FRED
// Registers and Bonus Enables user
// 
//    Rev 1.25   10 Apr 1995 14:48:48   FRED
// Puts non-toll number in dialog.
// 
// 
//    Rev 1.24   01 Mar 1995 16:55:02   FRED
// No change.
// 
//    Rev 1.23   27 Feb 1995 13:57:36   FRED
// Department name now matches the one on the printed
// order form... includes offer code.
// 
// 
//    Rev 1.22   27 Feb 1995 13:33:26   FRED
// Does not list or indent for unmarked item with no collections name.
// 
// 
//    Rev 1.21   24 Feb 1995 14:34:54   FRED
// Restructured product tree. Got rid of CProductForSale
// 
//    Rev 1.20   23 Feb 1995 09:54:14   FRED
// Tweaked printed order form: Added "(please print clearly)"
// and duplicate Offer Code.
// 
// 
//    Rev 1.19   19 Jan 1995 14:20:36   JAY
// New free pictures!
// 
//    Rev 1.18   13 Jan 1995 13:47:02   FRED
//  
// 
//    Rev 1.17   10 Jan 1995 10:48:58   FRED
// Does not use "discount".
// 
// 
//    Rev 1.16   06 Jan 1995 18:21:22   FRED
// 
//    Rev 1.15   06 Jan 1995 18:08:08   FRED
//  
// 
//    Rev 1.14   16 Dec 1994 17:09:52   FRED
//  
// 
//    Rev 1.13   14 Dec 1994 18:00:50   FRED
// 
//    Rev 1.12   13 Dec 1994 18:29:24   FRED
//  
// 
//    Rev 1.11   06 Dec 1994 14:35:12   JAY
// Uses member variable.
// 
//    Rev 1.10   06 Dec 1994 13:38:48   FRED
//  
// 
//    Rev 1.9   06 Dec 1994 10:40:36   FRED
//  
// 
//    Rev 1.8   05 Dec 1994 16:31:34   JAY
//  
// 
//    Rev 1.7   05 Dec 1994 15:11:06   FRED
//  
// 
//    Rev 1.6   05 Dec 1994 11:38:16   JAY
//  
// 
//    Rev 1.5   10 Nov 1994 15:05:40   FRED
// 
//    Rev 1.4   09 Nov 1994 18:25:28   FRED
//  
// 
//    Rev 1.3   09 Nov 1994 17:23:36   JAY
//  
// 
//    Rev 1.2   09 Nov 1994 17:16:40   JAY
//  
// 
//    Rev 1.1   08 Nov 1994 18:27:58   FRED
//  
*/

// mailord.cpp : implementation file
//

#include "stdafx.h"
#include <time.h>

#include "upgrade.h"
#include "cwmf.h"
#include "cinstall.h"
#include "orderfrm.h"
#include "mailord.h"
#include "msregini.h"
#include "propctl.h"
#include "util.h"
#include "credit.h"
#include "salestax.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

void PriceString(CString& csPrice, long lPrice, BOOL fDollarSign = TRUE, BOOL fBlankIfZero = FALSE);
extern void PerformIniKeySubstitution(CString& csValue);

static UINT PaymentMethodNames[] =
{
	IDS_VISA,
	IDS_MASTERCARD,
	IDC_AMERICAN_EXPRESS,
	IDC_DISCOVER,
	IDC_CHECK_OR_MONEY_ORDER
};

/////////////////////////////////////////////////////////////////////////////
// COrderByMailDialog dialog


COrderByMailDialog::COrderByMailDialog(CInstallInfo* pInfo, CWnd* pParent /*=NULL*/)
	: CBaseDialog(COrderByMailDialog::IDD, pParent)
{
	m_pInfo = pInfo;
	//{{AFX_DATA_INIT(COrderByMailDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pOrderForm = NULL;
}

void COrderByMailDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COrderByMailDialog)
	DDX_Control(pDX, IDC_ACCESS_CODE, m_CodeCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COrderByMailDialog, CBaseDialog)
	//{{AFX_MSG_MAP(COrderByMailDialog)
	ON_BN_CLICKED(IDC_PRINTORDERFORM, OnPrintorderform)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COrderByMailDialog message handlers

BOOL COrderByMailDialog::OnInitDialog()
{
	CenterWindow();
	
	CBaseDialog::OnInitDialog();

	CMlsPropertyControl::SubclassPropertyControls(this);

	// Build the product list and get some prices.
	
	m_pInfo->GetLRCFromProducts(m_csLRC);

	m_CodeCtrl.LimitText(12);

	BOOL fProductsOrdered = GetApp()->Offer()->CountProductFlags(PRODUCT_ITEM_Selected) > 0;
	
	CWnd* pWnd = GetDlgItem(IDC_PRINTORDERFORM);
	
	if (pWnd != NULL)
	{
		pWnd->EnableWindow(fProductsOrdered);
	}

	CWnd* pFocus = GetDlgItem(fProductsOrdered ? IDC_PRINTORDERFORM : IDC_ACCESS_CODE);
	
	if (pFocus != NULL)
	{
		pFocus->SetFocus();
	}

	return pFocus == NULL;
}

void COrderByMailDialog::OnPrintorderform()
{
	COrderFormInformationDialog Dialog(m_pInfo, this);
	Dialog.m_csLRC = m_csLRC;
	Dialog.m_lSubTotal = m_lSubTotal;
	Dialog.m_lTax = m_lTax;
	Dialog.m_lShipping = m_lShipping;
	Dialog.m_pOrderForm = m_pOrderForm;
	Dialog.DoModal();
}

void COrderByMailDialog::OnOK()
{
	CString csText;

	m_CodeCtrl.GetWindowText(csText);

	// Assume an empty edit field means no code yet. Just leave the dialog.

	if (csText.IsEmpty())
	{
		OnCancel();
	}
	
	else
	{
		// In this dialog, we expect to get the full XXX-YYY code.

		/*
		// Make an extra check: make sure the code typed in has the correct
		// prefix since we don't validate what they mail ordered before.
		*/

		int nLRCLength = m_csLRC.GetLength();
		if ((csText.GetLength() > nLRCLength				// Large enough for prefix
			 && csText.Left(nLRCLength) == m_csLRC		// Prefix matches
			 && GetApp()->InstallationData()->ValidateLicenseAccessCode(csText))
#ifdef _DEBUG
		 || csText == "UPGRADE"
#endif
		 )
		{
			// Make sure they're registered.
			GetApp()->UserIniFile().WriteInteger(GetApp()->ApplicationName(), MSREGUSRINI_Application_Registered, 1);
	
			// "Bonus Enable" them.
			GetApp()->UserIniFile().WriteString(GetApp()->ApplicationName(), MSREGUSRINI_Application_RegistrationCode, MODEM_REGISTRATION_CODE);

			// License what is selected (we don't need to muck with the LAC).
			m_pInfo->LicenseProducts(csText);
		}
		else
		{
			AfxMessageBox(IDS_INVALID_ACCESS_CODE);
			return;				// Don't leave the dialog yet.
		}
		
		CBaseDialog::OnOK();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMailOrderProducts dialog

CMailOrderProducts::CMailOrderProducts()
{
}

CMailOrderProducts::~CMailOrderProducts()
{
	RemoveAll();
}

void CMailOrderProducts::RemoveAll(void)
{
	int nProducts = ProductCount();
	
	for (int nProduct = 0; nProduct < nProducts; nProduct++)
	{
		CMailOrderProduct* pProduct = Product(nProduct);
		
		if (pProduct != NULL)
		{
			delete pProduct;
		}
	}
	
	CPtrArray::RemoveAll();
}

long CMailOrderProducts::AddProducts(CProductItems* pItems)
{
	long lPrice = 0;
	AddItems(pItems, 0, lPrice);
	return lPrice;
}

void CMailOrderProducts::AddItems(CProductItems* pItems, int nIndent, long& lPrice)
{
	int nItems = pItems->ItemCount();
	
	for (int nItem = 0; nItem < nItems; nItem++)
	{
		CProductItem* pItem = pItems->Item(nItem);
		
		if (pItem != NULL)
		{
			if (!pItem->IsLicensed())
			{
				if ((pItem->TestFlag(PRODUCT_ITEM_Marked))
				 || (pItem->Items()->CountProductFlags(PRODUCT_ITEM_Marked, CProductItems::NotLicensed) > 0))
				{
					CString csName;
					CString csPrice;
						
					csName = "";
						
					for (int i = 0; i < nIndent; i++)
					{
						csName += "     ";
					}
						
					if (pItem->TestFlag(PRODUCT_ITEM_Marked))
					{
						csName += pItem->Name();
						PriceString(csPrice, pItem->Price());
						lPrice += pItem->Price();
					}
					else
					{
						csName += pItem->CollectionName();
						csPrice = "";
					}
					
					if (!csName.IsEmpty())
					{
						CMailOrderProduct* pMailOrderProduct = new CMailOrderProduct(csName, csPrice);
					
						if (pMailOrderProduct != NULL)
						{
							Add(pMailOrderProduct);
							
							if (!pItem->TestFlag(PRODUCT_ITEM_Marked))
							{
								AddItems(pItem->Items(), nIndent+1, lPrice);
							}
						}
					}
					else
					{
						if (!pItem->TestFlag(PRODUCT_ITEM_Marked))
						{
							AddItems(pItem->Items(), nIndent, lPrice);
						}
					}
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMailOrderProduct dialog

CMailOrderProduct::CMailOrderProduct(CString& csName, CString& csPrice)
{
	m_csName = csName;
	m_csPrice = csPrice;
}

CMailOrderProduct::~CMailOrderProduct()
{
}

void PriceString(CString& csPrice, long lPrice, BOOL fDollarSign /*=TRUE*/, BOOL fBlankIfZero /*=FALSE*/)
{
	CString csFree;
	if (!fBlankIfZero)
	{
		Util::SafeLoadString(IDS_FREE, csFree);
	}

	Util::PriceString(csPrice, lPrice, fDollarSign, csFree);

	if (!csPrice.IsEmpty())
	{
		for (int nPad = 8-csPrice.GetLength(); nPad > 0; nPad--)
		{
			csPrice = "  "+csPrice;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// COrderFormInformationDialog dialog

COrderFormInformationDialog::COrderFormInformationDialog(CInstallInfo* pInfo, CWnd* pParent /*=NULL*/)
	: CRegistrationInformationDialog(COrderFormInformationDialog::IDD, pParent)
{
	m_pInfo = pInfo;
	//{{AFX_DATA_INIT(COrderFormInformationDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_lPrice = 0;
}


void COrderFormInformationDialog::DoDataExchange(CDataExchange* pDX)
{
	CRegistrationInformationDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COrderFormInformationDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COrderFormInformationDialog, CRegistrationInformationDialog)
	//{{AFX_MSG_MAP(COrderFormInformationDialog)
	ON_BN_CLICKED(IDC_AMEX, OnCreditCard)
	ON_BN_CLICKED(IDC_CHECK_OR_MONEY_ORDER, OnCreditCard)
	ON_BN_CLICKED(IDC_DISCOVER, OnCreditCard)
	ON_BN_CLICKED(IDC_MASTERCARD, OnCreditCard)
	ON_BN_CLICKED(IDC_VISA, OnCreditCard)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COrderFormInformationDialog message handlers

BOOL COrderFormInformationDialog::OnInitDialog()
{
	CenterWindow();
	
	CRegistrationInformationDialog::OnInitDialog();

	// Build the product list and get some prices.

	m_lPrice = m_Products.AddProducts(GetApp()->Offer());
	
	return TRUE;
}

void COrderFormInformationDialog::OnCreditCard() 
{
	BOOL fEnable = !IsDlgButtonChecked(IDC_CHECK_MONEY_ORDER);
	GetDlgItem(IDC_CREDIT_CARD_LABEL)->EnableWindow(fEnable);
	GetDlgItem(IDC_CREDIT_CARD)->EnableWindow(fEnable);
	GetDlgItem(IDC_EXPIRATION_DATE_LABEL1)->EnableWindow(fEnable);
	GetDlgItem(IDC_EXPIRATION_DATE_LABEL2)->EnableWindow(fEnable);
	GetDlgItem(IDC_EXPIRATION_DATE)->EnableWindow(fEnable);
}

BOOL COrderFormInformationDialog::PrintOrderForm()
{
	COrderForm OrderForm;

	CString csFormat;
	
	CString csOfferCode;
	Util::SafeLoadString(IDS_OFFER_CODE, csFormat);
	csOfferCode.Format(csFormat, GetApp()->Offer()->OfferCode());
	OrderForm.AddStringMapping("~RCODE", csOfferCode);
	
	CString csFaxNumber = GetApp()->GetFaxNumber();
	OrderForm.AddStringMapping("~LFAXNUMBER", csFaxNumber);
	
	int nIndex = 1;
	CString csValue;
	
	wsprintf(csValue.GetBuffer(128), "%s %s", (LPCSTR)m_csFirstName, (LPCSTR)m_csLastName);
	csValue.ReleaseBuffer();
	OrderForm.AddIndexedStringMapping("~LSHIPADDRESS", nIndex++, csValue);
	
	if (!m_csCompany.IsEmpty())
	{
		OrderForm.AddIndexedStringMapping("~LSHIPADDRESS", nIndex++, m_csCompany);
	}
	
	if (!m_csAddress1.IsEmpty())
	{
		OrderForm.AddIndexedStringMapping("~LSHIPADDRESS", nIndex++, m_csAddress1);
	}
	
	if (!m_csAddress2.IsEmpty())
	{
		OrderForm.AddIndexedStringMapping("~LSHIPADDRESS", nIndex++, m_csAddress2);
	}

	wsprintf(csValue.GetBuffer(128), "%s, %s  %s", (LPCSTR)m_csCity, (LPCSTR)m_csState, (LPCSTR)m_csZip);
	csValue.ReleaseBuffer();
	OrderForm.AddIndexedStringMapping("~LSHIPADDRESS", nIndex++, csValue);
	
	if (m_wCountryId != COUNTRY_UnitedStatesOfAmerica)
	{
		OrderForm.AddIndexedStringMapping("~LSHIPADDRESS", nIndex++, m_csCountry);
	}

	if (!m_csPhone.IsEmpty())
	{
		OrderForm.AddIndexedStringMapping("~LSHIPADDRESS", nIndex++, m_csPhone);
	}
	
	nIndex = 1;

	CString csPaymentMethod;
	Util::SafeLoadString(PaymentMethodNames[m_nPaymentMethod], csPaymentMethod);
	OrderForm.AddIndexedStringMapping("~LPAYMENTMETHOD", nIndex++, csPaymentMethod);
	
	if ((m_nPaymentMethod >= CCreditCard::VISA) && (m_nPaymentMethod <= CCreditCard::DISCOVER))
	{
		csValue = m_csCardNumber;
		CCreditCard::PrettyCreditCardNumber(m_nPaymentMethod, csValue);
		OrderForm.AddIndexedStringMapping("~LPAYMENTMETHOD", nIndex++, csValue);

		Util::SafeLoadString(IDS_EXPIRES, csFormat);
		csValue.Format(csFormat, m_csExpirationDate);
		OrderForm.AddIndexedStringMapping("~LPAYMENTMETHOD", nIndex++, csValue);
	}
	
	CInternationalData* pInternationalData = GetApp()->GetInternationalData();
	if (pInternationalData != NULL)
	{
		CNameList MailList(pInternationalData->m_InstantSales.m_Mail.m_szAddress);
		for (nIndex = 1; nIndex <= MailList.GetSize(); nIndex++)
		{
			csValue = MailList.ElementAt(nIndex-1);
			PerformIniKeySubstitution(csValue);
			OrderForm.AddIndexedStringMapping("~LMAILADDRESS", nIndex, csValue);
		}
	}

	Util::SafeLoadString(IDS_PRINTED, csFormat);

	CString csDate;

#ifdef WIN32
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);

	csDate.Format(
		csFormat,
		(int)SystemTime.wMonth,
		(int)SystemTime.wDay,
		(int)(SystemTime.wYear % 100),
		(int)SystemTime.wHour,
		(int)SystemTime.wMinute,
		(int)SystemTime.wSecond);
#else
	time_t BinaryTime;
	time(&BinaryTime);
	CTime Time(BinaryTime);

	csDate.Format(
		csFormat,
		Time.GetMonth(),
		Time.GetDay(),
		Time.GetYear(),
		Time.GetHour(),
		Time.GetMinute(),
		Time.GetSecond());
#endif

	OrderForm.AddStringMapping("~LDATE", csDate);
	
	CString csSubTotal;
	CString csTaxLabel;
	CString csTax;
	CString csFeeLabel;
	CString csFee;
	CString csTotal;
	
	long lPrice = m_lPrice;
	long lSubTotal = lPrice;
	
	long lTax = CSalesTaxComboBox::StateTax(m_csState, lSubTotal);
	if (lTax > 0)
	{
		Util::SafeLoadString(IDS_TAX, csFormat);
		csTaxLabel.Format(csFormat, m_csState);
	}
	else
	{
		Util::SafeLoadString(IDS_APPLICABLE_TAX_WILL_BE_ADDED, csTaxLabel);
	}
	
	long lFee = GetApp()->Offer()->ProcessingFee();
	
	csFeeLabel.Empty();
	if (lFee != 0)
	{
		Util::SafeLoadString(IDS_PROCESSING_FEE, csFeeLabel);
	}
	PriceString(csFee, lFee, TRUE, TRUE);
	
	long lTotal = lPrice+lTax+lFee;
	
	PriceString(csSubTotal, lSubTotal);
	PriceString(csTax, lTax, TRUE, TRUE);
	PriceString(csTotal, lTotal);
	
	int nProducts = m_Products.ProductCount();
	int nItemNumber = 1;

	for (int nProduct = 0; nProduct < nProducts; nProduct++)
	{
		CMailOrderProduct* pProduct = m_Products.Product(nProduct);
		
		if (pProduct != NULL)
		{
			int nNumber = (pProduct->Price().GetLength() == 0) ? 0 : nItemNumber;
			
			if (nNumber != 0)
			{
				nItemNumber++;
			}
			
			OrderForm.AddItem(nNumber, pProduct->Name(), pProduct->Price(), FALSE);
		}
	}

	Util::SafeLoadString(IDS_SUBTOTAL, csFormat);
	OrderForm.AddItem(-1, csFormat, csSubTotal, TRUE);
	OrderForm.AddItem(-1, csTaxLabel, csTax, FALSE);
	OrderForm.AddItem(-1, csFeeLabel, csFee, FALSE);
	Util::SafeLoadString(IDS_TOTAL, csFormat);
	OrderForm.AddItem(-1, csFormat, csTotal, TRUE);
	Util::SafeLoadString(IDS_REQUEST_CODE, csFormat);
	OrderForm.AddItem(-2, csFormat, m_csLRC, TRUE);
	
	OrderForm.Wmf(AfxGetResourceHandle(), MAKEINTRESOURCE(ID_ORDERFORM_WMF), "METAFILE");
	OrderForm.Bounds(776, 8679, 16147, 192, 12699, MulDiv(284, 3, 4), 284);

	return OrderForm.Print(this);
}

void COrderFormInformationDialog::OnOK()
{
	// Validate the data.
	if (UpdateData(TRUE))
	{
		TRY
		{
			if (PrintOrderForm())
			{
				// The printing of an order form requires us to remember the LRC for
				// the products. Do that now.
			
				m_pInfo->OrderSelectedProducts(m_csLRC);
				EndDialog(IDOK);
			}
		}
		END_TRY
	}
}
