/*
// $Workfile: BYPHONE.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/BYPHONE.CPP $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.5   11 Mar 1996 15:35:48   FRED
// Printed registration form
// 
//    Rev 1.4   13 Dec 1995 17:39:00   FRED
// Product code for phone registration
// 
//    Rev 1.3   05 Dec 1995 13:02:34   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.2   28 Nov 1995 16:13:46   FRED
// No Free variations
// 
//    Rev 1.1   22 Nov 1995 13:34:02   FRED
// Supports code entry.
// 
//    Rev 1.0   22 Nov 1995 08:52:10   FRED
// Initial revision.
// 
//    Rev 1.1   16 Nov 1995 10:31:22   FRED
//  
// 
//    Rev 1.0   01 Nov 1995 11:55:16   FRED
// Added Register by Phone
*/ 

// byphone.cpp : implementation file
//

#include "stdafx.h"
#include "msregx.h"
#include "msregini.h"
#include "propctl.h"
#include "util.h"
#include "byphone.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegisterByPhoneDialog dialog


CRegisterByPhoneDialog::CRegisterByPhoneDialog(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CRegisterByPhoneDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegisterByPhoneDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRegisterByPhoneDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegisterByPhoneDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
#ifndef NO_FREE_BONUS
		CString csRegistrationCode;
		BOOL fCodeValid = TRUE;
		CEdit* pCode = (CEdit*)GetDlgItem(IDC_REGISTRATION_CODE);

		TRY
		{
			pCode->GetWindowText(csRegistrationCode);
			fCodeValid = GetApp()->BonusEnableUser(csRegistrationCode, this);
		}
		END_TRY

		if (!fCodeValid)
		{
			pCode->SetFocus();
			pCode->SetSel(0, -1);
			AfxThrowUserException();
		}
#endif
	}
}


BEGIN_MESSAGE_MAP(CRegisterByPhoneDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CRegisterByPhoneDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRegisterByPhoneDialog message handlers

BOOL CRegisterByPhoneDialog::OnInitDialog() 
{
	CBaseDialog::OnInitDialog();
	
	CMlsPropertyControl::SubclassPropertyControls(this);
	
	CenterWindow();
	
#ifndef NO_FREE_BONUS
	((CEdit*)GetDlgItem(IDC_REGISTRATION_CODE))->LimitText(64);

	GetDlgItem(IDC_REGISTRATION_CODE)->SetFocus();
#endif

	CString csProductCodeFormat;
	CString csProductCodeText;
	CString csProductCodeNumber;

	TRY
	{
		CWnd* pwndProductCode = GetDlgItem(IDC_PRODUCT_CODE);

		if (pwndProductCode != NULL)
		{
			if (GetApp()->RegistrationProductCode(csProductCodeNumber))
			{
				pwndProductCode->GetWindowText(csProductCodeFormat);
				wsprintf(
					csProductCodeText.GetBuffer(256),
					csProductCodeFormat,
					csProductCodeNumber);
				csProductCodeText.ReleaseBuffer();
				pwndProductCode->SetWindowText(csProductCodeText);
			}
		}
	}
	END_TRY
	
	return FALSE; // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRegisterByPhoneDialog::OnOK() 
{
	CBaseDialog::OnOK();

#ifdef NO_FREE_BONUS
	// We assume that the user will follow-through and make the call,
	// so mark them as registered.

	GetApp()->UserIniFile().WriteInteger(GetApp()->ApplicationName(), MSREGUSRINI_Application_Registered, 1);
#endif
}
