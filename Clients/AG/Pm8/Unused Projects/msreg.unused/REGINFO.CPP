/*
// $Workfile: REGINFO.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/REGINFO.CPP $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.8   04 Oct 1996 15:29:56   FRED
// Uses international DLL (INTLxx.DLL)
// 
//    Rev 1.7   01 Oct 1996 10:38:18   FRED
//  
// 
//    Rev 1.6   10 Jul 1996 13:02:08   FRED
// Fixes to international changes
// 
//    Rev 1.5   26 Jun 1996 07:03:24   FRED
// International Registration
// 
//    Rev 1.4   11 Mar 1996 15:34:20   FRED
// Printed registration form
// 
//    Rev 1.3   29 Jan 1996 09:54:26   FRED
// Lists only required fields
// 
//    Rev 1.2   05 Dec 1995 13:02:36   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.1   29 Nov 1995 09:49:08   FRED
// Moves focus to field which failed validation.
// 
//    Rev 1.0   22 Nov 1995 08:52:04   FRED
// Initial revision.
// 
//    Rev 1.14   21 Nov 1995 12:07:58   FRED
// Fixed Email
// 
//    Rev 1.13   20 Nov 1995 16:25:36   FRED
// Removed Title, added Email
// 
//    Rev 1.12   17 Nov 1995 07:48:46   FRED
// Require info and phone number dialogs now have parents
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
#include "msregx.h"
#include "dialopts.h"
#include "phonenum.h"
#include "required.h"
#include "msregini.h"
#include "util.h"
#include "intl\intldll.h"
#include "reginfo.h"

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
		((CDialog*)GetParent())->NextDlgCtrl();
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
	//}}AFX_DATA_INIT

	m_wCountryId = 0;
	m_csCountry = _T("");
}

void CRegistrationInformationDialog::DoDataExchange(CDataExchange* pDX)
{
	CString csRequiredFields;
	CString csFirstNameLabel;
	CString csLastNameLabel;
	CString csAddressLabel;
	CString csCityLabel;
	CString csStateLabel;
	CString csZipLabel;
	
	TRY
	{
		// Move the data from the controls into member variables.
		CBaseDialog::DoDataExchange(pDX);
		
		//{{AFX_DATA_MAP(CRegistrationInformationDialog)
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
		//}}AFX_DATA_MAP

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

			// Check if there are any fields that are required
			// but not yet filled in.
	
			CWnd* pFocusControl = NULL;

			csFirstNameLabel.LoadString(IDS_FirstName);
			csLastNameLabel.LoadString(IDS_LastName);
			csAddressLabel.LoadString(IDS_Address);
			csCityLabel.LoadString(IDS_City);
			csStateLabel.LoadString(IDS_State);
			csZipLabel.LoadString(IDS_Zip);
	
			// We attempt to report the missing fields in a format
			// the is similar to how the fields are layed out on
			// the registration dialog.
			CollectRequiredFields(
				pFocusControl,
				csRequiredFields,
				&m_FirstNameEdit,      (LPCSTR)csFirstNameLabel,
				&m_LastNameEdit,       (LPCSTR)csLastNameLabel,
				(LPCSTR)-1,
				&m_Address1Edit,       (LPCSTR)csAddressLabel,
				(LPCSTR)-1,
				&m_CityEdit,           (LPCSTR)csCityLabel,
				&m_StateEdit,          (LPCSTR)csStateLabel,
				(LPCSTR)-1,
				&m_ZipEdit,            (LPCSTR)csZipLabel,
				(LPCSTR)-1,
				(LPCSTR)NULL);
				
			// Check if there are any missing fields to report.
			if (!csRequiredFields.IsEmpty())
			{
				ReportMissingRequiredFields(csRequiredFields);
				
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
				ReportInvalidPhoneNumber();
				m_PhoneEdit.SetFocus();
				m_PhoneEdit.SetSel(0,-1);
				AfxThrowUserException();
			}
	
			// Record selections for future reference.
			GetApp()->UserIniFile().WriteString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_FirstName, m_csFirstName);
			GetApp()->UserIniFile().WriteString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_LastName, m_csLastName);
			GetApp()->UserIniFile().WriteString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_Company, m_csCompany);
			GetApp()->UserIniFile().WriteString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_Title, "");
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

int CRegistrationInformationDialog::ReportMissingRequiredFields(LPCSTR pszRequiredFields)
{
	CRequiredInformationDialog Dialog(pszRequiredFields, this);
	return Dialog.DoModal();
}

int CRegistrationInformationDialog::ReportInvalidPhoneNumber(void)
{
	CPhoneNumberDialog Dialog;
	return Dialog.DoModal();
}

BEGIN_MESSAGE_MAP(CRegistrationInformationDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CRegistrationInformationDialog)
	ON_BN_CLICKED(IDC_DIALOUTPREFIX, OnDialoutprefix)
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
