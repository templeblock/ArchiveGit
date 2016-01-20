/*
// $Workfile: REGINFO.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/
/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/REGINFO.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:42   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:20:04   Fred
// Initial revision.
// 
//    Rev 1.1   10 Jun 1997 15:44:04   Fred
// Cleanup
// 
//    Rev 1.0   20 May 1997 10:41:54   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/REGINFO.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.1   14 Oct 1996 13:34:42   Fred
// International Version
// 
//    Rev 1.0   08 Sep 1996 09:40:38   Fred
// Initial revision.
// 
//    Rev 1.2   01 Mar 1996 13:53:36   FRED
// Includes stdarg.h
// 
//    Rev 1.1   01 Mar 1996 13:44:38   FRED
// IL sales Tax, and type-in info on mail upgrade
// 
//    Rev 1.0   02 Feb 1996 08:58:08   JAY
// Initial revision.
// 
//    Rev 1.6   30 Jan 1996 08:10:22   FRED
// Lists only empty required fields.
// Valdiates credit card numbers.
// 
// 
//    Rev 1.5   03 Jan 1996 12:45:50   FRED
// Credit card expiration date month must be <= 12
// 
//    Rev 1.4   18 Dec 1995 10:31:22   FRED
// Updates order form tax stuff along with reginfo
// 
//    Rev 1.3   15 Dec 1995 17:58:22   FRED
// Forces tax on CA modem orders.
// 
//    Rev 1.2   05 Dec 1995 12:11:42   FRED
// Order by Modem
// 
//    Rev 1.1   29 Nov 1995 11:25:14   JAY
//  
// 
//    Rev 1.0   29 Nov 1995 10:27:32   JAY
// Initial revision.
// 
//    Rev 1.11   16 Nov 1995 10:31:18   FRED
//  
// 
//    Rev 1.10   09 Nov 1995 17:39:10   FRED
// First name field is focused if it's blank.
// 
// 
//    Rev 1.9   08 Nov 1995 10:54:56   FRED
// Phone number optional; International warnings
// 
//    Rev 1.8   31 Oct 1995 15:09:12   FRED
// Uses CWnd::CenterWindow()
// 
//    Rev 1.7   30 Oct 1995 13:52:26   FRED
// 
//    Rev 1.6   27 Oct 1995 14:06:58   FRED
// Fixed phone number formatting problem
// 
//    Rev 1.5   27 Oct 1995 14:03:18   FRED
// Now restores registation information to edit controls.
// 
// 
//    Rev 1.4   26 Oct 1995 10:27:52   FRED
//  
*/ 

// reginfo.cpp : implementation file
//

#include "stdafx.h"
#include <stdarg.h>
#include "license.h"
#include "dialopts.h"
#include "phonenum.h"
#include "expirat.h"
#include "required.h"
#include "msregini.h"
#include "util.h"
#include "reginfo.h"
#include "credit.h"
#include "salestax.h"
#include "upgdlg.h"

#include "propctl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

void DigitsOnly(CString& csString)
{
	CString csDigits;

	TRY
	{
		csDigits.Empty();
		
		int nLength = csString.GetLength();
		
		for (int i = 0; i < nLength; i++)
		{
			if ((csString[i] >= '0') && (csString[i] <= '9'))
			{
				csDigits += csString[i];
			}
		}

		csString = csDigits;
	}
	END_TRY
}

BOOL PrettyTenDigitPhoneNumber(CString& csString)
{
	BOOL fResult = FALSE;

	CString csDigits;

	TRY
	{
		csDigits = csString;
		DigitsOnly(csDigits);

		if (csDigits.GetLength() == 10)
		{
			csString = csDigits.Mid(0,3)+" "+csDigits.Mid(3,3)+"-"+csDigits.Mid(6,4);
			fResult = TRUE;
		}
	}
	END_TRY

	return fResult;
}

void CollectRequiredFields(CWnd*& pFocus, CString& csRequiredFields, CWnd* pControl, ...)
{
	CString csLine;
	CString csValue;
	
	TRY
	{
		csLine.Empty();
	
		va_list pList;
		va_start(pList, pControl);
		
		while (pControl != NULL)
		{
			if (pControl == (CWnd*)-1)
			{
				if (!csLine.IsEmpty())
				{
					if (!csRequiredFields.IsEmpty())
					{
						csRequiredFields += "\n\n";
					}
			
					csRequiredFields += csLine;
					csLine.Empty();
				}
			}
			
			else
			{
				LPCSTR pszName = va_arg(pList, LPCSTR);
				
				pControl->GetWindowText(csValue);
				
				if (csValue.IsEmpty())
				{
					csLine += pszName;
					csLine += "     ";
					
					if (pFocus == NULL)
					{
						pFocus = pControl;
					}
				}
			}
			
			pControl = va_arg(pList, CWnd*);
		}
		va_end(pList);
	}
	END_TRY
}

/////////////////////////////////////////////////////////////////////////////
// CNameList

CNameList::CNameList(LPCSTR pszStrings /*=NULL*/, char cSeparator /*='|'*/)
{
	SetNames(pszStrings, cSeparator);
}

CNameList::~CNameList()
{
}

void CNameList::SetNames(LPCSTR pszStrings, char cSeparator /*='|'*/)
{
	CString csNames;
	CString csName;
	TRY
	{
		RemoveAll();
		if (pszStrings != NULL)
		{
			csNames = pszStrings;
			while (GetNextName(csName, csNames, cSeparator))
			{
				Add(csName);
			}
		}
	}
	END_TRY
}

BOOL CNameList::GetNextName(CString& csName, CString& csNames, char cSeparator)
{
	BOOL fResult = FALSE;
	TRY
	{
		csName.Empty();
		while (csName.IsEmpty())
		{
			int nIndex = csNames.Find(cSeparator);
			if (nIndex == -1)
			{
				csName = csNames;
				csNames.Empty();
			}
			if (nIndex > 0)
			{
				csName = csNames.Left(nIndex);
				csNames = csNames.Mid(nIndex+1);
			}

			int nLength = csName.GetLength();
			int nStart = 0;
			int nEnd = nLength-1;
			
			for (; nStart < nLength; nStart++)
			{
				if ((csName[nStart] != ' ') && (csName[nStart] != '\t'))
				{
					break;
				}
			}
			
			for (; nEnd >= 0; nEnd--)
			{
				if ((csName[nEnd] != ' ') && (csName[nEnd] != '\t'))
				{
					break;
				}
			}
	
			if (nStart > nEnd)
			{
				csName.Empty();
			}
			else
			{
				csName = csName.Mid(nStart, nEnd-nStart+1);
			}
	
			if (csNames.IsEmpty())
			{
				break;
			}
		}
		fResult = !csName.IsEmpty();
	}
	CATCH_ALL(e)
	{
		TRY
		{
			csName.Empty();
		}
		END_TRY
		fResult = FALSE;
	}
	END_CATCH_ALL

	return fResult;
}

/////////////////////////////////////////////////////////////////////////////
// CCountryList Combo Box

CCountryList::CCountryList()
{
	m_fReturnGoesToNextControl = FALSE;
}

CCountryList::~CCountryList()
{
}

BEGIN_MESSAGE_MAP(CCountryList, CComboBox)
	//{{AFX_MSG_MAP(CRegistrationEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CCountryList::Fill(WORD wCountryId /*=0*/)
{
	GetApp()->FillCountryComboBox(GetSafeHwnd(), wCountryId);
}

WORD CCountryList::GetCountryId(void)
{
	WORD wResult = 0;

	TRY
	{
		int nIndex = GetCurSel();
		if (nIndex != CB_ERR)
		{
			DWORD dwData = GetItemData(nIndex);
			if (dwData != (DWORD)CB_ERR)
			{
				wResult = (WORD)dwData;
			}
		}
	}
	END_TRY

	return wResult;
}

/////////////////////////////////////////////////////////////////////////////
// CCountryList message handlers

void CCountryList::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ((nChar != VK_RETURN) || (!m_fReturnGoesToNextControl))
	{
		CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

void CCountryList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ((nChar == VK_RETURN) && (m_fReturnGoesToNextControl))
	{
		((CDialog*)GetParent())->NextDlgCtrl();
	}
	else
	{
		CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

UINT CCountryList::OnGetDlgCode()
{
	UINT uResult = CComboBox::OnGetDlgCode();

	const MSG* pGetDlgCodeMsg = GetCurrentMessage();

	if (m_fReturnGoesToNextControl)
	{
		if (pGetDlgCodeMsg->lParam != 0)
		{
			MSG* pMsg = (LPMSG)(pGetDlgCodeMsg->lParam);

			if ((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_CHAR))
			{
				 if (pMsg->wParam == VK_RETURN)
				{
					uResult |= DLGC_WANTMESSAGE;
				}
			}
		}
	}

	return uResult;
}

/////////////////////////////////////////////////////////////////////////////
// CRegistrationEdit

CRegistrationEdit::CRegistrationEdit()
{
}

CRegistrationEdit::~CRegistrationEdit()
{
}

BEGIN_MESSAGE_MAP(CRegistrationEdit, CEdit)
	//{{AFX_MSG_MAP(CRegistrationEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegistrationEdit message handlers

void CRegistrationEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar != VK_RETURN)
	{
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

void CRegistrationEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_RETURN)
	{
		((CBaseDialog*)GetParent())->NextDlgCtrl();
	}
	else
	{
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

UINT CRegistrationEdit::OnGetDlgCode()
{
	UINT uResult = CEdit::OnGetDlgCode();

	const MSG* pGetDlgCodeMsg = GetCurrentMessage();

	if (pGetDlgCodeMsg->lParam != 0)
	{
		MSG* pMsg = (LPMSG)(pGetDlgCodeMsg->lParam);

		if ((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_CHAR))
		{
			 if (pMsg->wParam == VK_RETURN)
			{
				uResult |= DLGC_WANTMESSAGE;
			}
		}
	}

	return uResult;
}

/////////////////////////////////////////////////////////////////////////////
// CRegistrationInformationDialog dialog


CRegistrationInformationDialog::CRegistrationInformationDialog(UINT uID, CWnd* pParent /*=NULL*/)
	: CBaseDialog(uID != (UINT)-1 ? uID : CRegistrationInformationDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegistrationInformationDialog)
	m_csFirstName = _T("");
	m_csLastName = _T("");
	m_csCompany = _T("");
	m_csAddress1 = _T("");
	m_csAddress2 = _T("");
	m_csCity = _T("");
	m_csState = _T("");
	m_csZip = _T("");
	m_csPhone = _T("");
	m_csEmail = _T("");
	m_csCardNumber = _T("");
	m_csExpirationDate = _T("");
	m_nPaymentMethod = 0;
	//}}AFX_DATA_INIT
	
	m_wCountryId = 0;
	m_csCountry = _T("");
	
	m_lSubTotal = 0;
	m_lShipping = 0;
	m_lTax = 0;
	m_pOrderForm = NULL;
	
	m_fModemOrder = FALSE;
}

void CRegistrationInformationDialog::DoDataExchange(CDataExchange* pDX)
{
	CString csRequiredFields;
	
	TRY
	{
		// Move the data from the controls into member variables.
		CBaseDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CRegistrationInformationDialog)
		DDX_Control(pDX, IDC_EXPIRATION_DATE, m_ExpirationDateEdit);
		DDX_Control(pDX, IDC_CREDIT_CARD, m_CardNumberEdit);
		DDX_Control(pDX, IDC_EMAIL, m_EmailEdit);
		DDX_Control(pDX, IDC_PHONE, m_PhoneEdit);
		DDX_Control(pDX, IDC_COUNTRY_LIST, m_CountryList);
		DDX_Control(pDX, IDC_ZIP, m_ZipEdit);
		DDX_Control(pDX, IDC_STATE, m_StateEdit);
		DDX_Control(pDX, IDC_CITY, m_CityEdit);
		DDX_Control(pDX, IDC_ADDRESS2, m_Address2Edit);
		DDX_Control(pDX, IDC_ADDRESS1, m_Address1Edit);
		DDX_Control(pDX, IDC_COMPANY, m_CompanyEdit);
		DDX_Control(pDX, IDC_LASTNAME, m_LastNameEdit);
		DDX_Control(pDX, IDC_FIRSTNAME, m_FirstNameEdit);
		DDX_Text(pDX, IDC_FIRSTNAME, m_csFirstName);
		DDX_Text(pDX, IDC_LASTNAME, m_csLastName);
		DDX_Text(pDX, IDC_COMPANY, m_csCompany);
		DDX_Text(pDX, IDC_ADDRESS1, m_csAddress1);
		DDX_Text(pDX, IDC_ADDRESS2, m_csAddress2);
		DDX_Text(pDX, IDC_CITY, m_csCity);
		DDX_Text(pDX, IDC_STATE, m_csState);
		DDX_Text(pDX, IDC_ZIP, m_csZip);
		DDX_Text(pDX, IDC_PHONE, m_csPhone);
		DDX_Text(pDX, IDC_EMAIL, m_csEmail);
		DDX_Text(pDX, IDC_CREDIT_CARD, m_csCardNumber);
		DDX_Text(pDX, IDC_EXPIRATION_DATE, m_csExpirationDate);
		DDX_Radio(pDX, IDC_VISA, m_nPaymentMethod);
		//}}AFX_DATA_MAP
		
		// Update the display of the total amount.
		UpdatePaymentAmount();

		// Validate the user input.
		if (pDX->m_bSaveAndValidate)
		{
			// Trim the input strings (leading and trailing spaces).
			Util::Trim(m_csFirstName);
			Util::Trim(m_csLastName);
			Util::Trim(m_csCompany);
			Util::Trim(m_csAddress1);
			Util::Trim(m_csAddress2);
			Util::Trim(m_csCity);
			Util::Trim(m_csState);
			Util::Trim(m_csZip);
			Util::Trim(m_csPhone);
			Util::Trim(m_csEmail);
			Util::Trim(m_csCardNumber);
			Util::Trim(m_csExpirationDate);

			// Get selected country.
			m_wCountryId = m_CountryList.GetCountryId();
			GetApp()->GetCountryName(m_wCountryId, m_csCountry);

			// Special validation for phone number in US or canada.
			BOOL fPhoneNumberValid = TRUE;
			if ((m_wCountryId == COUNTRY_UnitedStatesOfAmerica)
			 || (m_wCountryId == COUNTRY_Canada))
			{
				fPhoneNumberValid = m_csPhone.IsEmpty() || PrettyTenDigitPhoneNumber(m_csPhone);
			}

			BOOL fCreditCardValid = TRUE;
			if ((m_nPaymentMethod >= CCreditCard::VISA) && (m_nPaymentMethod <= CCreditCard::DISCOVER))
			{
				fCreditCardValid = CCreditCard::Validate(m_nPaymentMethod, m_csCardNumber, m_csExpirationDate);
			}

			// Set the control text back to the trimmed text.
			m_FirstNameEdit.SetWindowText(m_csFirstName);
			m_LastNameEdit.SetWindowText(m_csLastName);
			m_CompanyEdit.SetWindowText(m_csCompany);
			m_Address1Edit.SetWindowText(m_csAddress1);
			m_Address2Edit.SetWindowText(m_csAddress2);
			m_CityEdit.SetWindowText(m_csCity);
			m_StateEdit.SetWindowText(m_csState);
			m_ZipEdit.SetWindowText(m_csZip);
			m_PhoneEdit.SetWindowText(m_csPhone);
			m_EmailEdit.SetWindowText(m_csEmail);
			m_CardNumberEdit.SetWindowText(m_csCardNumber);
			m_ExpirationDateEdit.SetWindowText(m_csExpirationDate);

			CWnd* pFocusControl = NULL;
	
			// We attempt to report the missing fields in a format
			// the is similar to how the fields are layed out on
			// the registration dialog.
			CollectRequiredFields(
				pFocusControl,
				csRequiredFields,
				&m_FirstNameEdit,      (LPCSTR)"First Name",
				&m_LastNameEdit,       (LPCSTR)"Last Name",
				(LPCSTR)-1,
				&m_Address1Edit,       (LPCSTR)"Address",
				(LPCSTR)-1,
				&m_CityEdit,           (LPCSTR)"City",
				&m_StateEdit,          (LPCSTR)"State",
				(LPCSTR)-1,
				&m_ZipEdit,            (LPCSTR)"Zip",
				(LPCSTR)-1,
				&m_PhoneEdit,          (LPCSTR)"Phone",
				(LPCSTR)-1,
				(LPCSTR)NULL);
				
			if ((m_nPaymentMethod >= CCreditCard::VISA) && (m_nPaymentMethod <= CCreditCard::DISCOVER))
			{
				CollectRequiredFields(
					pFocusControl,
					csRequiredFields,
					&m_CardNumberEdit,     (LPCSTR)"Credit Card Number",
					&m_ExpirationDateEdit, (LPCSTR)"Exp. Date",
					(LPCSTR)-1,
					(LPCSTR)NULL);
			}
			
			// Check if there are any missing fields to report.
			if (!csRequiredFields.IsEmpty())
			{
				CRequiredInformationDialog Dialog(csRequiredFields, this);
				Dialog.DoModal();
				
				// Focus the first required field that is missing.
				if (pFocusControl != NULL)
				{
					pFocusControl->SetFocus();
				}
			}
			if (!csRequiredFields.IsEmpty())
			{
				AfxThrowUserException();
			}
	
			// Report invalid phone number.		
			if (!fPhoneNumberValid)
			{
				CPhoneNumberDialog Dialog;
				Dialog.DoModal();
				m_PhoneEdit.SetFocus();
				m_PhoneEdit.SetSel(0,-1);
			}
			if (!fPhoneNumberValid)
			{
				AfxThrowUserException();
			}
	
			// Report invalid credit card information.
			if (!fCreditCardValid)
			{
				CExpirationDateDialog Dialog;
				Dialog.DoModal();
				m_CardNumberEdit.SetFocus();
				m_CardNumberEdit.SetSel(0,-1);
			}
			if (!fCreditCardValid)
			{
				AfxThrowUserException();
			}
	
			// Record selections for future reference.
			GetApp()->UserIniFile().WriteString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_FirstName, m_csFirstName);
			GetApp()->UserIniFile().WriteString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_LastName, m_csLastName);
			GetApp()->UserIniFile().WriteString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_Company, m_csCompany);
			GetApp()->UserIniFile().WriteString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_Address1, m_csAddress1);
			GetApp()->UserIniFile().WriteString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_Address2, m_csAddress2);
			GetApp()->UserIniFile().WriteString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_City, m_csCity);
			GetApp()->UserIniFile().WriteString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_State, m_csState);
			GetApp()->UserIniFile().WriteString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_Zip, m_csZip);
			GetApp()->UserIniFile().WriteInteger(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_Country, m_wCountryId);
			GetApp()->UserIniFile().WriteString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_Phone, m_csPhone);
			GetApp()->UserIniFile().WriteString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_Email, m_csEmail);
		}
	}
	CATCH_ALL(e)
	{
		csRequiredFields.Empty();
		THROW_LAST();
	}
	END_CATCH_ALL
}

void CRegistrationInformationDialog::UpdatePaymentAmount(void)
{
	CString csState;
	TRY
	{
		// Get the state name.
		m_StateEdit.GetWindowText(csState);
	}
	END_TRY
	
   // Update the IDC_PAYMENT_AMOUNT text.
	CWnd* pPaymentAmount = GetDlgItem(IDC_PAYMENT_AMOUNT);
	if (pPaymentAmount != NULL)
	{
		CString csTotal;
		TRY
		{
			long lTotal = m_lSubTotal;

			// Compute the tax for specified state.
			long lTax = CSalesTaxComboBox::StateTax(csState, lTotal);
			
			// Add in tax.
			lTotal += lTax;
			
			// Add in shipping.
			lTotal += m_lShipping;
		
			// Set the text to the total.
			Util::PriceString(csTotal, lTotal);
			if (lTax != 0)
			{
				csTotal += "   (includes ";
				csTotal += csState;
				csTotal += " tax)";
			}
			pPaymentAmount->SetWindowText(csTotal);
		}
		END_TRY
	}
	
	// Update the order form based on the information the user has entered.
	if (m_pOrderForm != NULL)
	{
		m_pOrderForm->SetTaxIndex(CSalesTaxComboBox::StateTaxIndex(csState));
	}
}


BEGIN_MESSAGE_MAP(CRegistrationInformationDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CRegistrationInformationDialog)
	ON_BN_CLICKED(IDC_DIALOUTPREFIX, OnDialoutprefix)
	ON_EN_CHANGE(IDC_STATE, UpdatePaymentAmount)
	ON_EN_KILLFOCUS(IDC_STATE, UpdatePaymentAmount)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRegistrationInformationDialog message handlers

BOOL CRegistrationInformationDialog::OnInitDialog() 
{
	m_csFirstName = GetApp()->UserIniFile().GetString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_FirstName);
	m_csLastName = GetApp()->UserIniFile().GetString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_LastName);
	m_csCompany = GetApp()->UserIniFile().GetString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_Company);
	m_csAddress1 = GetApp()->UserIniFile().GetString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_Address1);
	m_csAddress2 = GetApp()->UserIniFile().GetString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_Address2);
	m_csCity = GetApp()->UserIniFile().GetString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_City);
	m_csState = GetApp()->UserIniFile().GetString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_State);
	m_csZip = GetApp()->UserIniFile().GetString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_Zip);
	m_wCountryId = GetApp()->UserIniFile().GetInteger(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_Country, 0);
	m_csPhone = GetApp()->UserIniFile().GetString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_Phone);
	m_csEmail = GetApp()->UserIniFile().GetString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_Email);

	if (m_wCountryId == 0)
	{
		m_wCountryId = GetApp()->GetInternationalData()->m_wCountry;
	}

	if (m_csState.IsEmpty() && (m_lTax != 0))
	{
		if (m_pOrderForm != NULL)
		{
			int nIndex = m_pOrderForm->GetTaxIndex();
			if (nIndex >= 0)
			{
				m_pOrderForm->m_TaxList.GetLBText(nIndex, m_csState);
			}
		}
	}
	
	CBaseDialog::OnInitDialog();
	
	CMlsPropertyControl::SubclassPropertyControls(this);

	CenterWindow();
	
	m_CountryList.Fill(m_wCountryId);
	m_CountryList.ReturnGoesToNextControl(TRUE);
	
	m_FirstNameEdit.LimitText(15);
	m_LastNameEdit.LimitText(20);
	m_CompanyEdit.LimitText(40);
	m_Address1Edit.LimitText(30);
	m_Address2Edit.LimitText(30);
	m_CityEdit.LimitText(20);
	m_StateEdit.LimitText(15);
	m_ZipEdit.LimitText(15);
	m_PhoneEdit.LimitText(24);
	m_EmailEdit.LimitText(255);
	m_CardNumberEdit.LimitText(19);
	m_ExpirationDateEdit.LimitText(10);

	if (m_fModemOrder)
	{
		CWnd* pCheckOrMoneyOrder = GetDlgItem(IDC_CHECK_MONEY_ORDER);
		if (pCheckOrMoneyOrder != NULL)
		{
			pCheckOrMoneyOrder->EnableWindow(FALSE);
			pCheckOrMoneyOrder->ShowWindow(SW_HIDE);
		}
	}
	else
	{
		CWnd* pDialOutPrefix = GetDlgItem(IDC_DIALOUTPREFIX);
		if (pDialOutPrefix != NULL)
		{
			pDialOutPrefix->EnableWindow(FALSE);
			pDialOutPrefix->ShowWindow(SW_HIDE);
		}
	}

	if (m_csFirstName.IsEmpty())
	{
		m_FirstNameEdit.SetFocus();
		return FALSE;
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRegistrationInformationDialog::OnDialoutprefix() 
{
	CDialingOptionsDialog Dialog(this);

	if (Dialog.DoModal() == IDOK)
	{
	}
}
