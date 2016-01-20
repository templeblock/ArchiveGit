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
	ON_BN_CLICKED(IDC_REDEYE_FIX, OnRedEyeFix)
	ON_BN_CLICKED(IDC_REDEYE_RESET, OnRedEyeReset)
END_MESSAGE_MAP()

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
void CRedEyeTab::DoDataExchange(CDataExchange* pDX)
{
	CBaseTab::DoDataExchange(pDX);
}

/////////////////////////////////////////////////////////////////////////////
void CRedEyeTab::OnRedEyeFix()
{
	if (!DoCommand(_T("RedEyeFix"), _T("-1")))
		return;
}

/////////////////////////////////////////////////////////////////////////////
void CRedEyeTab::OnRedEyeReset()
{
	if (!DoCommand(_T("RedEyeReset"), _T("-1")))
		return;
}
