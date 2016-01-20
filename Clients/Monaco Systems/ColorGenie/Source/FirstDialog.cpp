// FirstDialog.cpp : implementation file
//

#include "stdafx.h"
#include "colorgenie.h"
#include "FirstDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFirstDialog dialog


CFirstDialog::CFirstDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFirstDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFirstDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFirstDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFirstDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFirstDialog, CDialog)
	//{{AFX_MSG_MAP(CFirstDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFirstDialog message handlers
