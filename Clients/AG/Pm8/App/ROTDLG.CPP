/*
// $Workfile: ROTDLG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/ROTDLG.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:18   Fred
// Initial revision.
// 
//    Rev 1.2   28 Jul 1997 15:36:32   Fred
// Selects text
// 
//    Rev 1.1   12 Mar 1997 15:52:16   Jay
// Rotation, etc.
// 
//    Rev 1.0   11 Mar 1997 17:43:20   Jay
// Rotate stuff
*/

#include "stdafx.h"
#include "pmw.h"
#include "RotDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomRotateDialog dialog


CCustomRotateDialog::CCustomRotateDialog(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CCustomRotateDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCustomRotateDialog)
	m_dRotation = 0.0;
	//}}AFX_DATA_INIT
}


void CCustomRotateDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustomRotateDialog)
	DDX_Text(pDX, IDC_ROTATION, m_dRotation);
	DDV_MinMaxDouble(pDX, m_dRotation, -999.0, 999.0);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCustomRotateDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CCustomRotateDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomRotateDialog message handlers

BOOL CCustomRotateDialog::OnInitDialog() 
{
	BOOL fResult = CPmwDialog::OnInitDialog();
	
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_ROTATION);
	if (pEdit != NULL)
	{
		pEdit->LimitText(8); // -xxx.yyy
		pEdit->SetSel(0, -1);
		pEdit->SetFocus();
		fResult = FALSE;		// We set the focus.
	}

	return fResult;
}
