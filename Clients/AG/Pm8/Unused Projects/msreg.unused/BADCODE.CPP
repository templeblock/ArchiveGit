/*
// $Workfile: BADCODE.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/BADCODE.CPP $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.3   30 Aug 1996 09:19:36   FRED
// Fixed bad reg code crash
// 
//    Rev 1.2   13 Dec 1995 17:39:00   FRED
// Product code for phone registration
// 
//    Rev 1.1   05 Dec 1995 13:02:40   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.0   22 Nov 1995 10:26:50   FRED
// Initial revision.
// 
//    Rev 1.1   14 Nov 1995 09:40:24   JAY
// Base dialog supporting events.
*/ 

#include "stdafx.h"
#include "MSREGX.h"
#include "msregini.h"
#include "propctl.h"
#include "badcode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBadRegistrationCodeDialog dialog


CBadRegistrationCodeDialog::CBadRegistrationCodeDialog(CWnd* pParent /*=NULL*/)
	: CBaseDialog(DialogId(), pParent)
{
	//{{AFX_DATA_INIT(CBadRegistrationCodeDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CBadRegistrationCodeDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBadRegistrationCodeDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBadRegistrationCodeDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CBadRegistrationCodeDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBadRegistrationCodeDialog message handlers

BOOL CBadRegistrationCodeDialog::OnInitDialog() 
{
	CBaseDialog::OnInitDialog();
	
	CMlsPropertyControl::SubclassPropertyControls(this);
	
	CString csProductCodeFormat;
	CString csProductCodeText;

	TRY
	{
		CWnd* pwndProductCode = GetDlgItem(IDC_PRODUCT_CODE);

		if (pwndProductCode != NULL)
		{
			pwndProductCode->GetWindowText(csProductCodeFormat);
			wsprintf(
				csProductCodeText.GetBuffer(256),
				csProductCodeFormat,
				(LPCSTR)(GetApp()->IniFile().GetString(MSREGINI_Section_Configuration, MSREGINI_Configuration_ParentPartNumber)));
			pwndProductCode->SetWindowText(csProductCodeText);
		}
	}
	END_TRY

	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
