/*
// $Workfile: REGBONUS.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/REGBONUS.CPP $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.2   05 Dec 1995 13:02:38   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.1   30 Nov 1995 11:40:56   FRED
// Fixed focus problems on RegBonus dialog
// 
//    Rev 1.0   22 Nov 1995 10:26:48   FRED
// Initial revision.
// 
//    Rev 1.3   17 Nov 1995 08:00:44   FRED
// Bad registration code dialog now has parent.
// 
// 
//    Rev 1.2   14 Nov 1995 09:40:14   JAY
// Base dialog supporting events.
*/ 

#include "stdafx.h"
#include "MSREGX.h"
#include "propctl.h"
#include "regbonus.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegisterForBonusDialog dialog


CRegisterForBonusDialog::CRegisterForBonusDialog(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CRegisterForBonusDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegisterForBonusDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRegisterForBonusDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegisterForBonusDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		CString csRegistrationCode;
		BOOL fCodeValid = TRUE;
		CEdit* pCode = (CEdit*)GetDlgItem(IDC_REGISTRATION_CODE);

		TRY
		{
			pCode->GetWindowText(csRegistrationCode);

			if (!csRegistrationCode.IsEmpty())
			{
				fCodeValid = GetApp()->BonusEnableUser(csRegistrationCode, this);
			}
		}
		END_TRY

		if (!fCodeValid)
		{
			pCode->SetFocus();
			pCode->SetSel(0, -1);
			AfxThrowUserException();
		}
	}
}


BEGIN_MESSAGE_MAP(CRegisterForBonusDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CRegisterForBonusDialog)
	ON_BN_CLICKED(IDC_REGISTER_NOW, OnRegisterNow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRegisterForBonusDialog message handlers

BOOL CRegisterForBonusDialog::OnInitDialog() 
{
	CBaseDialog::OnInitDialog();
	
	CMlsPropertyControl::SubclassPropertyControls(this);
	
	CenterWindow();

	((CEdit*)GetDlgItem(IDC_REGISTRATION_CODE))->LimitText(64);

	if (GetApp()->UserIsRegistered())
	{
		GetDlgItem(IDC_REGISTRATION_CODE)->SetFocus();
		return FALSE;
	}

#ifndef WIN32
	// This bit of wierd code is to get around an apparent bug in WIN16
	// which leaves the default button border around OK even if it's not
	// the focused button. This seems to correct the situation.
	GetDlgItem(IDCANCEL)->SetFocus();
	NextDlgCtrl();
#endif
	return TRUE;   // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}

void CRegisterForBonusDialog::OnRegisterNow() 
{
	EndDialog(IDC_REGISTER_NOW);
}
