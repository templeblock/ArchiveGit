#include "stdafx.h"
#include "FileTab.h"
#include "TabControl.h"
#include "MainDlg.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CFileTab, CBaseTab)
BEGIN_MESSAGE_MAP(CFileTab, CBaseTab)
	ON_BN_CLICKED(IDC_IMAGE_OPEN, OnImageOpen)
	ON_BN_CLICKED(IDC_IMAGE_SAVE, OnImageSave)
	ON_BN_CLICKED(IDC_IMAGE_CLOSE, OnImageClose)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CFileTab::CFileTab(CWnd* pParent/*=NULL*/)
	: CBaseTab(CFileTab::IDD, pParent)
{
}

/////////////////////////////////////////////////////////////////////////////
CFileTab::~CFileTab()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CFileTab::OnInitDialog()
{
	// Initialize the resize dialog base class
	CBaseTab::OnInitDialog();

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CFileTab::DoDataExchange(CDataExchange* pDX)
{
	CBaseTab::DoDataExchange(pDX);
}

/////////////////////////////////////////////////////////////////////////////
void CFileTab::OnImageOpen()
{
	DoCommand(_T("Open"), _T(""));
}

/////////////////////////////////////////////////////////////////////////////
void CFileTab::OnImageSave()
{
	DoCommand(_T("Save"), _T(""));
}

/////////////////////////////////////////////////////////////////////////////
void CFileTab::OnImageClose()
{
	DoCommand(_T("Close"), _T(""));
}

