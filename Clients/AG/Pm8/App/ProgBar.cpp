// $Workfile: ProgBar.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.

// Revision History:
//
// $Log: /PM8/App/ProgBar.cpp $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 6     7/15/98 12:07p Dennis
// Added reset of cancel state on window destruction
// 
// 5     5/20/98 5:19p Mwilson
// changed to diable and enable main window as necessary.
// 
// 4     4/15/98 4:28p Dennis
// Changed include reference
// 
// 3     12/09/97 3:30p Dennis
// Added Revision History
// 

#include "stdafx.h"
#include "resource.h"
#include "ProgBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void (*CPMWProgressBar::m_pCancelMethod)() = NULL;

/////////////////////////////////////////////////////////////////////////////
// CPMWProgressBar dialog


CPMWProgressBar::CPMWProgressBar(CWnd* pParent /*=NULL*/)
	: CDialog(CPMWProgressBar::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPMWProgressBar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CPMWProgressBar::SetCancelMethod(void (*pCancelMethod)())
   {
      m_pCancelMethod = pCancelMethod;
   }

void CPMWProgressBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPMWProgressBar)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPMWProgressBar, CDialog)
	//{{AFX_MSG_MAP(CPMWProgressBar)
	ON_BN_CLICKED(ID_CANCEL, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPMWProgressBar message handlers

void CPMWProgressBar::OnCancel() 
{
   if(m_pCancelMethod)
      m_pCancelMethod();
   m_status.SetCancel(TRUE);
}

BOOL CPMWProgressBar::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	AfxGetMainWnd()->EnableWindow(FALSE);
	EnableWindow(TRUE);

	m_status.SetCancel(FALSE);
	CenterWindow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPMWProgressBar::DestroyWindow() 
{
	m_status.SetCancel(FALSE);
   
	AfxGetMainWnd()->EnableWindow(TRUE);
	
	return CDialog::DestroyWindow();
}
