/*
// $Header: /PM8/UNINST/DELDLG.CPP 1     3/03/99 6:36p Gbeddow $
//
// $Log: /PM8/UNINST/DELDLG.CPP $
// 
// 1     3/03/99 6:36p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:40:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:30:46   Fred
// Initial revision.
// 
//    Rev 1.0   18 Jul 1996 12:38:10   Fred
// Initial revision.
// 
//    Rev 1.1   17 Oct 1995 09:36:44   JAY
//  
*/

#include "stdafx.h"
#include "Uninstal.h"
#include "deldlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSharedDllDialog dialog

CSharedDllDialog::CSharedDllDialog(LPCSTR pszName, CWnd* pParent /*=NULL*/)
	: CDialog(CSharedDllDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSharedDllDialog)
	m_csFileToDelete = pszName;
	//}}AFX_DATA_INIT
}

void CSharedDllDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSharedDllDialog)
	DDX_Text(pDX, IDC_FILE_TO_DELETE, m_csFileToDelete);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSharedDllDialog, CDialog)
	//{{AFX_MSG_MAP(CSharedDllDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSharedDllDialog message handlers

BOOL CSharedDllDialog::OnInitDialog() 
{
	CenterWindow();
	return CDialog::OnInitDialog();
}
