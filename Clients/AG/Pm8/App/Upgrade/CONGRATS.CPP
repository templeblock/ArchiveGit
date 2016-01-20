/*
// $Workfile: CONGRATS.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/CONGRATS.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 2     9/01/98 12:22p Dennis
// Change Congratulations dialog to get product name from msreg.ini
// 
//    Rev 1.0   14 Aug 1997 15:41:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:33:08   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:40:24   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:14   JAY
// Initial revision.
// 
//    Rev 1.0   30 Nov 1995 15:58:32   JAY
// Initial revision.
// 
//    Rev 1.0   22 Nov 1995 08:52:04   FRED
// Initial revision.
// 
//    Rev 1.4   16 Nov 1995 10:31:18   FRED
//  
// 
//    Rev 1.3   31 Oct 1995 15:09:12   FRED
// Uses CWnd::CenterWindow()
// 
//    Rev 1.2   30 Oct 1995 12:11:48   FRED
//  
// 
//    Rev 1.1   26 Oct 1995 10:27:54   FRED
//  
*/ 

// congrats.cpp : implementation file
//

#include "stdafx.h"
#include "upgrade.h"
#include "congrats.h"
#include "util.h"
#include "propctl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCongratulationsDialog dialog


CCongratulationsDialog::CCongratulationsDialog(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CCongratulationsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCongratulationsDialog)
	m_csCongrats = _T("");
	//}}AFX_DATA_INIT
}


void CCongratulationsDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCongratulationsDialog)
	DDX_Text(pDX, IDC_CONGRATS_UPGRADED, m_csCongrats);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCongratulationsDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CCongratulationsDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCongratulationsDialog message handlers

BOOL CCongratulationsDialog::OnInitDialog() 
{
   CString csResource, csKeyValue;

   csResource.LoadString(IDS_CONGRATS);
   csKeyValue = GetApp()->MsRegIniFile().GetString("Registry", "Name", "PrintMaster");
	m_csCongrats.Format(csResource.GetBuffer(1),
      csKeyValue.GetBuffer(1),
      csKeyValue.GetBuffer(1),
      csKeyValue.GetBuffer(1),
      csKeyValue.GetBuffer(1));

	CBaseDialog::OnInitDialog();
	
	CMlsPropertyControl::SubclassPropertyControls(this);

	CenterWindow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
