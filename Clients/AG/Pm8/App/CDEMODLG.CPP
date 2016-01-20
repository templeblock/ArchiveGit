/*
// $Workfile: CDEMODLG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/CDEMODLG.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:30   Fred
// Initial revision.
// 
//    Rev 1.2   15 May 1996 08:45:46   Jay
// From PMW2
// 
//    Rev 1.2   13 May 1996 11:20:08   FRED
// Trial Version
// 
//    Rev 1.1   26 Apr 1996 14:24:18   JAY
// Added DoProductCatalog
// 
//    Rev 1.0   26 Mar 1996 12:29:04   FRED
// Demo dialog double-click on icon brings up catalog
*/

#include "stdafx.h"
#include "pmw.h"
#include "mainfrm.h"
#include "cdemodlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDemoDescriptionDialog dialog

CDemoDescriptionDialog::CDemoDescriptionDialog(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CDemoDescriptionDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDemoDescriptionDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDemoDescriptionDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDemoDescriptionDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDemoDescriptionDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CDemoDescriptionDialog)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDemoDescriptionDialog message handlers

BOOL CDemoDescriptionDialog::OnInitDialog() 
{
	CPmwDialog::OnInitDialog();

	// Set remaining trial count in dialog.
	CString csFormat;
	CString csText;
	TRY
	{
		CWnd* pWnd = GetDlgItem(IDC_COUNT_REMAINING);
		pWnd->GetWindowText(csFormat);
		csText.Format(csFormat, GET_PMWAPP()->m_installation_info.nTrialCount+1);
		pWnd->SetWindowText(csText);
	}
	END_TRY
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDemoDescriptionDialog::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd* pWnd = ChildWindowFromPoint(point);
	
	if (pWnd != NULL)
	{
		if (pWnd->GetDlgCtrlID() == IDC_DISCOUNT_CD_CATALOG)
		{
			// Start up the discount CD catalog!
			((CMainFrame*)(AfxGetMainWnd()))->DoProductCatalog();
			return;
		}
	}
	
	CPmwDialog::OnLButtonDblClk(nFlags, point);
}
