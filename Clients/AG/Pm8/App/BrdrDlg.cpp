// $Header: /PM8/App/BrdrDlg.cpp 1     3/03/99 6:03p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
//
// $Log: /PM8/App/BrdrDlg.cpp $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 5     2/05/98 11:58a Hforman
// passing back values in OnOK()
// 
// 4     1/09/98 6:52p Hforman
// 
// 3     1/09/98 11:05a Hforman
// interim checkin
// 
// 2     1/06/98 7:05p Hforman
// work-in-progress
// 
// 1     12/24/97 12:54p Hforman
// 

#include "stdafx.h"
#include "pmw.h"
#include "BrdrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddBorderDlg dialog


CAddBorderDlg::CAddBorderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddBorderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddBorderDlg)
	m_nChoice = FromScratch;
	//}}AFX_DATA_INIT
}


void CAddBorderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddBorderDlg)
	DDX_Radio(pDX, IDC_CREATE_NEW, m_nChoice);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddBorderDlg, CDialog)
	//{{AFX_MSG_MAP(CAddBorderDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddBorderDlg message handlers

void CAddBorderDlg::OnOK() 
{
	UpdateData();
	if (m_nChoice == FromScratch)
		EndDialog(ID_BORDER_CREATE);
	else
		EndDialog(ID_BORDER_SELECT);
}
