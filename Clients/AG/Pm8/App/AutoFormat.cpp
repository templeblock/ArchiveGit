// AutoFormat.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "AutoFormat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoFormat dialog


CAutoFormat::CAutoFormat(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoFormat::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAutoFormat)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAutoFormat::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoFormat)
	DDX_Control(pDX, IDC_STYLE_LIST, m_list);
	DDX_Control(pDX, IDC_AUTO_FORMAT, m_tableformat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAutoFormat, CDialog)
	//{{AFX_MSG_MAP(CAutoFormat)
	ON_CBN_SELCHANGE(IDC_AUTO_FORMAT, OnSelchangeTableFormat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoFormat message handlers

void CAutoFormat::OnSelchangeTableFormat() 
{
	// TODO: Add your control notification handler code here
	CAutoFormat dlg;
	dlg.DoModal();
	switch(m_tableformat.GetCurSel())
	{
		case 0:
			break;
		default:
			break;
	}

	
}
