#include "stdafx.h"
#include "BaseTab.h"
#include "TabControl.h"
#include "MainDlg.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CBaseTab, CResizeDlg)
BEGIN_MESSAGE_MAP(CBaseTab, CResizeDlg)
//j	ON_WM_ERASEBKGND()
	ON_WM_ENABLE()
	ON_BN_CLICKED(IDC_BACK, OnBack)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CBaseTab::CBaseTab(const UINT resID, CWnd* pParent)
	 : CResizeDlg(resID, pParent)
{
}

/////////////////////////////////////////////////////////////////////////////
CBaseTab::~CBaseTab()
{
}

/////////////////////////////////////////////////////////////////////////////
bool CBaseTab::EnableControl(UINT idControl, bool bEnable)
{
	CWnd* pControl = GetDlgItem(idControl);
	if (!pControl || !pControl->m_hWnd)
		return false;

	pControl->EnableWindow(bEnable);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CBaseTab::ShowControl(UINT idControl, UINT nCmdShow)
{
	CWnd* pControl = GetDlgItem(idControl);
	if (!pControl || !pControl->m_hWnd)
		return false;

	pControl->ShowWindow(nCmdShow);
	CRect rect;
	pControl->GetClientRect(&rect);
	CTabControl* pTabControl = dynamic_cast<CTabControl*>(GetParent());
	pControl->MapWindowPoints(pTabControl, &rect);
	pTabControl->InvalidateRect(&rect, true/*fErase*/);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CBaseTab::DoCommand(LPCTSTR strCommand, LPCTSTR strValue)
{
	CTabControl* pTabControl = dynamic_cast<CTabControl*>(GetParent());
	CMainDlg* pMainDlg = dynamic_cast<CMainDlg*>(pTabControl->GetParent());
	return pMainDlg->DoCommand(strCommand, strValue);
}

/////////////////////////////////////////////////////////////////////////////
long CBaseTab::GetCommand(LPCTSTR strCommand)
{
	CTabControl* pTabControl = dynamic_cast<CTabControl*>(GetParent());
	CMainDlg* pMainDlg = dynamic_cast<CMainDlg*>(pTabControl->GetParent());
	return pMainDlg->GetCommand(strCommand);
}

/////////////////////////////////////////////////////////////////////////////
CImageControl& CBaseTab::GetImageControl()
{
	CTabControl* pTabControl = dynamic_cast<CTabControl*>(GetParent());
	CMainDlg* pMainDlg = dynamic_cast<CMainDlg*>(pTabControl->GetParent());
	return pMainDlg->GetImageControl();

}

/////////////////////////////////////////////////////////////////////////////
void CBaseTab::PreSubclassWindow()
{
	ModifyStyle(WS_CAPTION|WS_THICKFRAME|WS_CLIPSIBLINGS|DS_3DLOOK|DS_MODALFRAME|WS_VISIBLE/*dwRemove*/,
				DS_CONTROL|WS_DISABLED|WS_CLIPCHILDREN/*dwAdd*/);
	ModifyStyleEx(WS_EX_DLGMODALFRAME|WS_EX_WINDOWEDGE|WS_EX_CLIENTEDGE|WS_EX_STATICEDGE/*dwRemove*/,
				0/*dwAdd*/);
//j				WS_EX_TRANSPARENT/*dwAdd*/);

	CResizeDlg::PreSubclassWindow();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBaseTab::OnEraseBkgnd(CDC* pDC)
{
	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CBaseTab::OnEnable(BOOL bEnable)
{
	CResizeDlg::OnEnable(bEnable);

	if (bEnable)
		OnEnterTab();
	else
		OnLeaveTab();
}

/////////////////////////////////////////////////////////////////////////////
void CBaseTab::OnBack()
{
	CTabControl* pParent = (CTabControl*)GetParent();
	if (pParent) pParent->SetCurrentTab(CMainTab::IDD);
}
