/*
// $Workfile: DEMODLG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/DEMODLG.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:06   Fred
// Initial revision.
// 
//    Rev 1.1   15 May 1996 08:45:50   Jay
// From PMW2
// 
//    Rev 1.2   13 May 1996 13:16:06   FRED
// Trial Version
// 
//    Rev 1.1   12 Feb 1996 09:24:02   FRED
// Instant upgrade tweaks
// 
//    Rev 1.0   25 Jan 1996 09:20:16   JAY
//  
*/ 

#include "stdafx.h"
#include "pmw.h"
#include "demodlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDemoVersionDialog dialog


CDemoVersionDialog::CDemoVersionDialog(LPCSTR lpszMessage, LPCSTR lpszTitle, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CDemoVersionDialog::IDD, pParent)
{
	TRY
		m_csMessage = lpszMessage;
	END_TRY

	TRY
		m_csTitle = lpszTitle;
	END_TRY

	//{{AFX_DATA_INIT(CDemoVersionDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CDemoVersionDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDemoVersionDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDemoVersionDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CDemoVersionDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDemoVersionDialog message handlers

BOOL CDemoVersionDialog::OnInitDialog()
{
	SetDlgItemText(IDC_MESSAGE, m_csMessage);
	
	if (!m_csTitle.IsEmpty())
	{
		SetWindowText(m_csTitle);
	}
	
	CPmwDialog::OnInitDialog();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}
