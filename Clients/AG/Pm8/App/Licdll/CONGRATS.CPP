/*
// $Workfile: CONGRATS.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/CONGRATS.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:19:42   Fred
// Initial revision.
// 
//    Rev 1.1   10 Jun 1997 15:43:28   Fred
// Cleanup
// 
//    Rev 1.0   20 May 1997 10:41:32   johno
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
// $Log: /PM8/App/LICDLL/CONGRATS.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   08 Sep 1996 09:40:24   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:14   JAY
// Initial revision.
// 
//    Rev 1.0   30 Nov 1995 15:58:32   JAY
// Initial revision.
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
#include "license.h"
#include "congrats.h"
#include "util.h"
#include "propctl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCongratulationsDialog dialog


CCongratulationsDialog::CCongratulationsDialog(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CCongratulationsDialog::IDD, pParent)
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
