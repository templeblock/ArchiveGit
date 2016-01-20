/*
// $Workfile: PHONENUM.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/PHONENUM.CPP $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.6   14 Oct 1996 13:55:58   FRED
// Turned off sound for IDD_SEND_FORM
// 
//    Rev 1.5   26 Jun 1996 07:04:08   FRED
// International Registration
// 
//    Rev 1.4   11 Mar 1996 15:34:22   FRED
// Printed registration form
// 
//    Rev 1.3   13 Dec 1995 12:34:34   FRED
// E, EM, EP, and EMP configurations
// 
//    Rev 1.2   12 Dec 1995 17:55:34   FRED
// Modem Only Version
// 
//    Rev 1.1   05 Dec 1995 13:02:38   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.0   22 Nov 1995 08:52:08   FRED
// Initial revision.
// 
//    Rev 1.4   16 Nov 1995 10:31:20   FRED
//  
// 
//    Rev 1.3   31 Oct 1995 15:09:14   FRED
// Uses CWnd::CenterWindow()
// 
//    Rev 1.2   26 Oct 1995 10:27:54   FRED
//  
*/ 

// phonenum.cpp : implementation file
//

#include "stdafx.h"
#include "msregx.h"
#include "phonenum.h"

#include "propctl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPhoneNumberDialog dialog

CPhoneNumberDialog::CPhoneNumberDialog(CWnd* pParent /*=NULL*/)
	: CBaseDialog(IDD_PHONE_NUMBER, pParent)
{
	//{{AFX_DATA_INIT(CPhoneNumberDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	// No sound recorded for this guy.
	m_nID = (UINT)-1;
}


void CPhoneNumberDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPhoneNumberDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPhoneNumberDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CPhoneNumberDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPhoneNumberDialog message handlers

BOOL CPhoneNumberDialog::OnInitDialog() 
{
	CBaseDialog::OnInitDialog();
	
	CMlsPropertyControl::SubclassPropertyControls(this);
	
	CenterWindow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
