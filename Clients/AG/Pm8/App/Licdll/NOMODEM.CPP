/*
// $Workfile: NOMODEM.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/NOMODEM.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:19:56   Fred
// Initial revision.
// 
//    Rev 1.1   10 Jun 1997 15:43:52   Fred
// Cleanup
// 
//    Rev 1.0   20 May 1997 10:41:46   johno
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
// $Log: /PM8/App/LICDLL/NOMODEM.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.1   14 Oct 1996 13:34:34   Fred
// International Version
// 
//    Rev 1.0   08 Sep 1996 09:40:32   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:04   JAY
// Initial revision.
// 
//    Rev 1.0   29 Nov 1995 10:27:36   JAY
// Initial revision.
// 
//    Rev 1.3   16 Nov 1995 10:31:20   FRED
//  
// 
//    Rev 1.2   31 Oct 1995 15:09:14   FRED
// Uses CWnd::CenterWindow()
// 
//    Rev 1.1   26 Oct 1995 10:27:54   FRED
//  
*/ 

// nomodem.cpp : implementation file
//

#include "stdafx.h"
#include "license.h"
#include "nomodem.h"
#include "propctl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNoModemDialog dialog


CNoModemDialog::CNoModemDialog(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CNoModemDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNoModemDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CNoModemDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNoModemDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNoModemDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CNoModemDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNoModemDialog message handlers

BOOL CNoModemDialog::OnInitDialog() 
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
