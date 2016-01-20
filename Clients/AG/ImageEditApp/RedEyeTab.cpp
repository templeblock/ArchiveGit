#include "stdafx.h"
#include "RedEyeTab.h"
#include "TabControl.h"
#include "MainDlg.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CRedEyeTab, CBaseTab)
BEGIN_MESSAGE_MAP(CRedEyeTab, CBaseTab)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_RESET, OnReset)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CRedEyeTab::DoDataExchange(CDataExchange* pDX)
{
	CBaseTab::DoDataExchange(pDX);
}

/////////////////////////////////////////////////////////////////////////////
CRedEyeTab::CRedEyeTab(CWnd* pParent/*=NULL*/)
	: CBaseTab(CRedEyeTab::IDD, pParent)
{
}

/////////////////////////////////////////////////////////////////////////////
CRedEyeTab::~CRedEyeTab()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CRedEyeTab::OnInitDialog()
{
	// Initialize the resize dialog base class
	CBaseTab::OnInitDialog();

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CRedEyeTab::OnEnterTab()
{
	bool bOK = DoCommand(_T("RedEyeTab"), Str(_T("%d"), EDIT_START));
}

/////////////////////////////////////////////////////////////////////////////
void CRedEyeTab::OnLeaveTab()
{
	bool bOK = DoCommand(_T("RedEyeTab"), Str(_T("%d"), EDIT_END));
}

/////////////////////////////////////////////////////////////////////////////
void CRedEyeTab::OnApply()
{
	bool bOK = DoCommand(_T("RedEyeTab"), Str(_T("%d"), EDIT_APPLY));
	OnEnterTab();
}

/////////////////////////////////////////////////////////////////////////////
void CRedEyeTab::OnReset()
{
	bool bOK = DoCommand(_T("RedEyeTab"), Str(_T("%d"), EDIT_RESET));
	OnEnterTab();
}
