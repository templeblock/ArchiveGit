/*
// $Workfile: BYMAIL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/BYMAIL.CPP $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.9   14 Oct 1996 13:57:12   FRED
// Turned off sound for IDD_SEND_FORM
// 
//    Rev 1.8   04 Oct 1996 15:29:52   FRED
// Uses international DLL (INTLxx.DLL)
// 
//    Rev 1.7   01 Oct 1996 10:38:12   FRED
//  
// 
//    Rev 1.6   09 Sep 1996 23:26:34   FRED
// Disabled sound until new sound is recorded.
// 
//    Rev 1.5   26 Jun 1996 07:05:20   FRED
// International Registration
// 
//    Rev 1.4   22 Mar 1996 08:51:56   FRED
// Placeholders in MSREG.INI
// Mail registration form uses Configuration/Bonus from MSREG.INI
// 
//    Rev 1.3   12 Mar 1996 12:59:36   FRED
// Date is left adjusted (LDATE)
// 
//    Rev 1.2   11 Mar 1996 15:34:32   FRED
// Printed registration form
// 
//    Rev 1.1   05 Dec 1995 13:02:34   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.0   22 Nov 1995 08:52:02   FRED
// Initial revision.
// 
//    Rev 1.5   16 Nov 1995 10:31:16   FRED
//  
// 
//    Rev 1.4   01 Nov 1995 11:54:44   FRED
// Added Register by Phone
// 
//    Rev 1.3   31 Oct 1995 15:09:12   FRED
// Uses CWnd::CenterWindow()
// 
//    Rev 1.2   30 Oct 1995 12:11:48   FRED
//  
// 
//    Rev 1.1   26 Oct 1995 10:27:52   FRED
//  
*/ 

// bymail.cpp : implementation file
//

#include "stdafx.h"
#include "msregx.h"
#include "msregini.h"
#include "propctl.h"
#include "util.h"
#include "regform.h"
#include "required.h"
#include "phonenum.h"
#include "bymail.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern void PerformIniKeySubstitution(CString& csValue);

/////////////////////////////////////////////////////////////////////////////
// CRegisterByMailFormDialog dialog

CRegisterByMailFormDialog::CRegisterByMailFormDialog(CWnd* pParent /*=NULL*/)
	: CRegistrationInformationDialog(CRegisterByMailFormDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegisterByMailFormDialog)
	//}}AFX_DATA_INIT
}


void CRegisterByMailFormDialog::DoDataExchange(CDataExchange* pDX)
{
	CRegistrationInformationDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegisterByMailFormDialog)
	//}}AFX_DATA_MAP
}


int CRegisterByMailFormDialog::ReportMissingRequiredFields(LPCSTR pszRequiredFields)
{
	CRequiredInformationDialog Dialog(pszRequiredFields, this);
	return Dialog.DoModal();
}

int CRegisterByMailFormDialog::ReportInvalidPhoneNumber(void)
{
	CPhoneNumberDialog Dialog;
	return Dialog.DoModal();
}

BEGIN_MESSAGE_MAP(CRegisterByMailFormDialog, CRegistrationInformationDialog)
	//{{AFX_MSG_MAP(CRegisterByMailFormDialog)
	ON_BN_CLICKED(IDC_PRINT_FORM, OnPrintForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRegisterByMailFormDialog message handlers

BOOL CRegisterByMailFormDialog::OnInitDialog() 
{
	return CRegistrationInformationDialog::OnInitDialog();
}

BOOL CRegisterByMailFormDialog::PrintRegistrationForm(void)
{
	CRegistrationForm RegistrationForm;

	CString csProductCode;

	if (GetApp()->RegistrationProductCode(csProductCode))
	{
		RegistrationForm.AddStringMapping("~LPRODUCTCODE", csProductCode);
	}

	CString csFaxIt;
	TRY
	{
		csFaxIt.LoadString(IDS_FaxIt);
		PerformIniKeySubstitution(csFaxIt);
		RegistrationForm.AddStringMapping("~LFAXIT", csFaxIt);
	}
	END_TRY

	CString csBonus = GetApp()->IniFile().GetString(MSREGINI_Section_Configuration, MSREGINI_Configuration_Bonus);
	csBonus += '!';
	RegistrationForm.AddStringMapping("~LBONUS", csBonus);
	
	int nIndex = 1;
	CString csValue;
	
	wsprintf(csValue.GetBuffer(128), "%s %s", (LPCSTR)m_csFirstName, (LPCSTR)m_csLastName);
	csValue.ReleaseBuffer();
	RegistrationForm.AddIndexedStringMapping("~LRETURNADDRESS", nIndex++, csValue);
	
	if (!m_csCompany.IsEmpty())
	{
		RegistrationForm.AddIndexedStringMapping("~LRETURNADDRESS", nIndex++, m_csCompany);
	}
	
	if (!m_csAddress1.IsEmpty())
	{
		RegistrationForm.AddIndexedStringMapping("~LRETURNADDRESS", nIndex++, m_csAddress1);
	}
	
	if (!m_csAddress2.IsEmpty())
	{
		RegistrationForm.AddIndexedStringMapping("~LRETURNADDRESS", nIndex++, m_csAddress2);
	}

	wsprintf(csValue.GetBuffer(128), "%s, %s  %s", (LPCSTR)m_csCity, (LPCSTR)m_csState, (LPCSTR)m_csZip);
	csValue.ReleaseBuffer();
	RegistrationForm.AddIndexedStringMapping("~LRETURNADDRESS", nIndex++, csValue);

	if (m_wCountryId != COUNTRY_UnitedStatesOfAmerica)
	{
		RegistrationForm.AddIndexedStringMapping("~LRETURNADDRESS", nIndex++, m_csCountry);
	}

	TRY
	{
		CNameList MailAddress(GetApp()->GetInternationalData()->m_Registration.m_Mail.m_szAddress);
		for (nIndex = 0; nIndex < MailAddress.GetSize(); nIndex++)
		{
			CString csLine = MailAddress.GetAt(nIndex);
			PerformIniKeySubstitution(csLine);
			RegistrationForm.AddIndexedStringMapping("~LREGISTERADDRESS", nIndex+1, (LPCSTR)csLine);
		}
	}
	END_TRY

	RegistrationForm.AddStringMapping("~LFIRSTNAME", m_csFirstName);
	RegistrationForm.AddStringMapping("~LLASTNAME", m_csLastName);
	RegistrationForm.AddStringMapping("~LCOMPANY", m_csCompany);
	RegistrationForm.AddStringMapping("~LADDRESS1", m_csAddress1);
	RegistrationForm.AddStringMapping("~LADDRESS2", m_csAddress2);
	RegistrationForm.AddStringMapping("~LCITY", m_csCity);
	RegistrationForm.AddStringMapping("~LZIP", m_csZip);
	RegistrationForm.AddStringMapping("~LSTATE", m_csState);
	RegistrationForm.AddStringMapping("~LCOUNTRY", m_csCountry);
	RegistrationForm.AddStringMapping("~LPHONE", m_csPhone);
	RegistrationForm.AddStringMapping("~LEMAIL", m_csEmail);

	time_t BinaryTime;
	time(&BinaryTime);
	CTime Time(BinaryTime);

	// For now we punt on the %B - you just get the number
	CString csDate;
	LPSTR pBuffer = csDate.GetBuffer(128);

	char cAMPM = 'A';
	int nHour = Time.GetHour();
	if (nHour == 0)
	{
		nHour = 12;
	}
	else if (nHour >= 12)
	{
		nHour -= 12;
		cAMPM = 'P';
	}

	if (nHour == 0)
	{
		nHour = 12;
	}

	sprintf(pBuffer, "Printed %d/%d/%d  %d:%02d:%02d %cM",
					Time.GetMonth(),
					Time.GetDay(),
					Time.GetYear(),
					nHour,
					Time.GetMinute(),
					Time.GetSecond(),
					cAMPM);
	csDate.ReleaseBuffer();

	RegistrationForm.AddStringMapping("~LDATE", csDate);

	RegistrationForm.Wmf(
		AfxGetResourceHandle(),
		MAKEINTRESOURCE(IDC_MAIL_FORM_TOP_WMF),
		MAKEINTRESOURCE(IDC_MAIL_FORM_MIDDLE_WMF),
		MAKEINTRESOURCE(IDC_MAIL_FORM_BOTTOM_WMF),
		"METAFILE");

	return RegistrationForm.Print(this);
}

void CRegisterByMailFormDialog::OnPrintForm()
{
	// Validate the data.
	if (UpdateData(TRUE))
	{
		TRY
		{
			if (PrintRegistrationForm())
			{
				CSendFormDialog Dialog(this);
				Dialog.DoModal();
				EndDialog(IDOK);
			}
		}
		END_TRY
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSendFormDialog dialog

CSendFormDialog::CSendFormDialog(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CSendFormDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSendFormDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSendFormDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendFormDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendFormDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CSendFormDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSendFormDialog message handlers

BOOL CSendFormDialog::OnInitDialog() 
{
	CBaseDialog::OnInitDialog();
	
	CMlsPropertyControl::SubclassPropertyControls(this);
	
	CenterWindow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
