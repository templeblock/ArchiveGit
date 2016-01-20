// expirat.cpp : implementation file
//
/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/EXPIRAT.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:19:46   Fred
// Initial revision.
// 
//    Rev 1.1   10 Jun 1997 15:43:38   Fred
// Cleanup
// 
//    Rev 1.0   20 May 1997 10:41:38   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/

#include "stdafx.h"
#include "license.h"
#include "expirat.h"

#include "propctl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExpirationDateDialog dialog


CExpirationDateDialog::CExpirationDateDialog(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CExpirationDateDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExpirationDateDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CExpirationDateDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExpirationDateDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CExpirationDateDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CExpirationDateDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CExpirationDateDialog message handlers

BOOL CExpirationDateDialog::OnInitDialog()
{
	CBaseDialog::OnInitDialog();
	
	CMlsPropertyControl::SubclassPropertyControls(this);
	
	CenterWindow();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}
