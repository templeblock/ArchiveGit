/*
// $Workfile: REQUIRED.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/REQUIRED.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:42:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:33:32   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:40:38   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:08   JAY
// Initial revision.
// 
//    Rev 1.1   30 Jan 1996 08:10:22   FRED
// Lists only empty required fields.
// Valdiates credit card numbers.
// 
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

// required.cpp : implementation file
//

#include "stdafx.h"
#include "upgrade.h"
#include "required.h"

#include "propctl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRequiredInformationDialog dialog


CRequiredInformationDialog::CRequiredInformationDialog(LPCSTR pszRequiredFieldNames, CWnd* pParent /*=NULL*/)
	: CBaseDialog(CRequiredInformationDialog::IDD, pParent)
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
