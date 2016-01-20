/*
// $Workfile: PHONENUM.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/PHONENUM.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:42:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:33:26   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:40:36   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:10   JAY
// Initial revision.
// 
//    Rev 1.0   29 Nov 1995 10:27:38   JAY
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
#include "upgrade.h"
#include "phonenum.h"

#include "propctl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPhoneNumberDialog dialog


CPhoneNumberDialog::CPhoneNumberDialog(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CPhoneNumberDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPhoneNumberDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
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
