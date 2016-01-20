#include "stdafx.h"
#include "LayoutTab.h"
#include "TabControl.h"
#include "MainDlg.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CLayoutTab, CBaseTab)
BEGIN_MESSAGE_MAP(CLayoutTab, CBaseTab)
	ON_BN_CLICKED(IDC_FLIP_HORIZONTAL, OnFlipHorizontal)
	ON_BN_CLICKED(IDC_FLIP_VERTICAL, OnFlipVertical)
	ON_BN_CLICKED(IDC_ROTATE_LEFT, OnRotateLeft)
	ON_BN_CLICKED(IDC_ROTATE_RIGHT, OnRotateRight)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CLayoutTab::DoDataExchange(CDataExchange* pDX)
{
	CBaseTab::DoDataExchange(pDX);
}

/////////////////////////////////////////////////////////////////////////////
CLayoutTab::CLayoutTab(CWnd* pParent/*=NULL*/)
	: CBaseTab(CLayoutTab::IDD, pParent)
{
}

/////////////////////////////////////////////////////////////////////////////
CLayoutTab::~CLayoutTab()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CLayoutTab::OnInitDialog()
{
	// Initialize the resize dialog base class
	CBaseTab::OnInitDialog();

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CLayoutTab::OnEnterTab()
{
}

/////////////////////////////////////////////////////////////////////////////
void CLayoutTab::OnLeaveTab()
{
}

/////////////////////////////////////////////////////////////////////////////
void CLayoutTab::OnFlipHorizontal()
{
	DoCommand(_T("FlipRotateTab"), _T("FlipHorizontal"));
}

/////////////////////////////////////////////////////////////////////////////
void CLayoutTab::OnFlipVertical()
{
	DoCommand(_T("FlipRotateTab"), _T("FlipVertical"));
}

/////////////////////////////////////////////////////////////////////////////
void CLayoutTab::OnRotateLeft()
{
	DoCommand(_T("FlipRotateTab"), _T("RotateLeft"));
}

/////////////////////////////////////////////////////////////////////////////
void CLayoutTab::OnRotateRight()
{
	DoCommand(_T("FlipRotateTab"), _T("RotateRight"));
}
