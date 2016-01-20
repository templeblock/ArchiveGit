/*
// $Workfile: CALLFAIL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/CALLFAIL.CPP $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.4   13 Dec 1995 12:34:40   FRED
// E, EM, EP, and EMP configurations
// 
//    Rev 1.3   12 Dec 1995 17:55:34   FRED
// Modem Only Version
// 
//    Rev 1.2   05 Dec 1995 13:02:38   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.1   28 Nov 1995 08:47:18   FRED
// Phone Switch
// 
//    Rev 1.0   22 Nov 1995 08:52:06   FRED
// Initial revision.
// 
//    Rev 1.4   16 Nov 1995 10:31:20   FRED
//  
// 
//    Rev 1.3   31 Oct 1995 15:09:14   FRED
// Uses CWnd::CenterWindow()
// 
//    Rev 1.2   30 Oct 1995 12:11:48   FRED
//  
// 
//    Rev 1.1   26 Oct 1995 10:27:54   FRED
//  
*/ 

// callfail.cpp : implementation file
//

#include "stdafx.h"
#include "msregx.h"
#include "callfail.h"
#include "util.h"
#include "propctl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

UINT CCallFailedDialog::DialogIds[] =
{
	IDD_CALL_FAILED_E,
	IDD_CALL_FAILED_EP,
	IDD_CALL_FAILED_EM,
	IDD_CALL_FAILED_EPM
};

/////////////////////////////////////////////////////////////////////////////
// CCallFailedDialog dialog


CCallFailedDialog::CCallFailedDialog(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CCallFailedDialog::DialogId(), pParent)
{
	//{{AFX_DATA_INIT(CCallFailedDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCallFailedDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCallFailedDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCallFailedDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CCallFailedDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCallFailedDialog message handlers

BOOL CCallFailedDialog::OnInitDialog() 
{
	CBaseDialog::OnInitDialog();
	
	CMlsPropertyControl::SubclassPropertyControls(this);
	
	CenterWindow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
