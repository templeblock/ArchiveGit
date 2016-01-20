/*
// $Workfile: CALLFAIL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/CALLFAIL.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:41:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:33:04   Fred
// Initial revision.
// 
//    Rev 1.1   14 Oct 1996 13:34:26   Fred
// International Version
// 
//    Rev 1.0   08 Sep 1996 09:40:20   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:16   JAY
// Initial revision.
// 
//    Rev 1.1   05 Dec 1995 12:12:26   FRED
// Order by Modem
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
//    Rev 1.2   30 Oct 1995 12:11:48   FRED
//  
// 
//    Rev 1.1   26 Oct 1995 10:27:54   FRED
//  
*/ 

// callfail.cpp : implementation file
//

#include "stdafx.h"
#include "upgrade.h"
#include "callfail.h"
#include "propctl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCallFailedDialog dialog


CCallFailedDialog::CCallFailedDialog(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CCallFailedDialog::IDD, pParent)
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
	
	// If there is no help number, get rid of the reference.
	CWnd* pHelp = GetDlgItem(IDC_HELP_INFO);
	if (pHelp != NULL)
	{
		CInternationalData* pInternationalData = GetApp()->GetInternationalData();
		if (pInternationalData != NULL)
		{
			if (!pInternationalData->m_InstantSales.m_Help.Exists())
			{
				pHelp->ShowWindow(SW_HIDE);
			}
		}
	}
	
	CenterWindow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
