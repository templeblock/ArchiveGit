/*
// $Workfile: CONGRATS.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/CONGRATS.CPP $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.2   13 Dec 1995 12:35:18   FRED
// E, EM, EP, and EMP configurations
// 
//    Rev 1.1   05 Dec 1995 13:02:36   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.0   22 Nov 1995 08:52:04   FRED
// Initial revision.
// 
//    Rev 1.4   16 Nov 1995 10:31:18   FRED
//  
// 
//    Rev 1.3   31 Oct 1995 15:09:12   FRED
// Uses CWnd::CenterWindow()
// 
//    Rev 1.2   30 Oct 1995 12:11:48   FRED
//  
// 
//    Rev 1.1   26 Oct 1995 10:27:54   FRED
//  
*/ 

// congrats.cpp : implementation file
//

#include "stdafx.h"
#include "msregx.h"
#include "congrats.h"
#include "util.h"
#include "propctl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

UINT CCongratulationsDialog::DialogIds[] =
{
	IDD_CONGRATULATIONS_E_EP,
	IDD_CONGRATULATIONS_E_EP,
	IDD_CONGRATULATIONS_EM,
	IDD_CONGRATULATIONS_EPM
};

/////////////////////////////////////////////////////////////////////////////
// CCongratulationsDialog dialog


CCongratulationsDialog::CCongratulationsDialog(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CCongratulationsDialog::DialogId(), pParent)
{
	//{{AFX_DATA_INIT(CCongratulationsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCongratulationsDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCongratulationsDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCongratulationsDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CCongratulationsDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCongratulationsDialog message handlers

BOOL CCongratulationsDialog::OnInitDialog() 
{
	CBaseDialog::OnInitDialog();
	
	CMlsPropertyControl::SubclassPropertyControls(this);
	
	CenterWindow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
