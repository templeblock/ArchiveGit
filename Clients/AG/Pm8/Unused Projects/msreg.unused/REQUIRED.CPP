/*
// $Workfile: REQUIRED.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/REQUIRED.CPP $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.6   26 Jun 1996 07:03:28   FRED
// International Registration
// 
//    Rev 1.5   11 Mar 1996 15:34:24   FRED
// Printed registration form
// 
//    Rev 1.4   29 Jan 1996 09:54:22   FRED
// Lists only required fields
// 
//    Rev 1.3   13 Dec 1995 12:34:36   FRED
// E, EM, EP, and EMP configurations
// 
//    Rev 1.2   12 Dec 1995 17:56:30   FRED
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

// required.cpp : implementation file
//

#include "stdafx.h"
#include "msregx.h"
#include "required.h"

#include "propctl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRequiredInformationDialog dialog

CRequiredInformationDialog::CRequiredInformationDialog(const LPCSTR pszRequiredFieldNames, CWnd* pParent /*=NULL*/)
	: CBaseDialog(IDD_REQUIRED_INFORMATION, pParent)
{
	//{{AFX_DATA_INIT(CRequiredInformationDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	TRY
	{
		m_csRequiredFieldNames = pszRequiredFieldNames;
	}
	END_TRY
}


void CRequiredInformationDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRequiredInformationDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRequiredInformationDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CRequiredInformationDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRequiredInformationDialog message handlers

BOOL CRequiredInformationDialog::OnInitDialog() 
{
	CBaseDialog::OnInitDialog();

	if (!m_csRequiredFieldNames.IsEmpty())
	{
		CWnd* pWnd = GetDlgItem(IDC_REQUIRED_FIELD_NAMES);

		if (pWnd != NULL)
		{
			pWnd->SetWindowText(m_csRequiredFieldNames);
		}
	}
	
	CMlsPropertyControl::SubclassPropertyControls(this);
	
	CenterWindow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
