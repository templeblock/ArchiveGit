#include "stdafx.h"
#include "OptimizeDlg.h"
#include "SecurityMgr.h"
#include "Authenticate.h"
#include "RegKeys.h"

//////////////////////////////////////////////////////////////////////
// The main dialog
COptimizeDlg::COptimizeDlg()
{
	m_idSelectedView = NULL;
	m_pOptimizeIntro = NULL;
	m_pOptimizeStep1 = NULL;
	m_pOptimizeFinal = NULL;
	m_pOptimizeCancel = NULL;

	CAuthenticate Authenticate;
	DWORD dwFail = 0;
	m_bRegistered = Authenticate.IsRegistered(dwFail);
}

//////////////////////////////////////////////////////////////////////
COptimizeDlg::~COptimizeDlg()
{
	// Destroy the wizard and all of the child views
	if (::IsWindow(m_hWnd))
		DestroyWindow();

	delete m_pOptimizeIntro;
	delete m_pOptimizeStep1;
	delete m_pOptimizeFinal;
	delete m_pOptimizeCancel;
}

//////////////////////////////////////////////////////////////////////
LRESULT COptimizeDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Create all of the views
	m_pOptimizeIntro	= new COptimizeDlgIntro(this);
	m_pOptimizeStep1	= new COptimizeDlgStep1(this);
	m_pOptimizeFinal	= new COptimizeDlgFinal(this);
	m_pOptimizeCancel	= new COptimizeDlgCancel(this);

	if (m_pOptimizeIntro)
		m_pOptimizeIntro->Create(m_hWnd);
	if (m_pOptimizeStep1)
		m_pOptimizeStep1->Create(m_hWnd);
	if (m_pOptimizeFinal)
		m_pOptimizeFinal->Create(m_hWnd);
	if (m_pOptimizeCancel)
		m_pOptimizeCancel->Create(m_hWnd);

	// Show the intro view
	ShowSelectedView(IDD_OPTIMIZEDLG_INTRO);

	return 1;  // Set the focus
}

//////////////////////////////////////////////////////////////////////
LRESULT COptimizeDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	switch (wID)
	{
		case IDOK:
		{
			m_bRegistered = !m_bRegistered;
			break;
		}
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT COptimizeDlg::OnBack(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	switch (m_idSelectedView)
	{
		case IDD_OPTIMIZEDLG_INTRO:
		case IDD_OPTIMIZEDLG_STEP1:
		case IDD_OPTIMIZEDLG_FINAL:
		case IDD_OPTIMIZEDLG_CANCEL:
		{
			break;
		}
	}

	if (m_idSelectedView > IDD_OPTIMIZEDLG_INTRO)
		ShowSelectedView(m_idSelectedView - 1);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT COptimizeDlg::OnNext(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	switch (m_idSelectedView)
	{
		case IDD_OPTIMIZEDLG_INTRO:
		{
			break;
		}
		case IDD_OPTIMIZEDLG_STEP1:
		{
			CSecurityMgr Security;
			bool bMustRestart = false;
			if (m_pOptimizeStep1->m_bOptimizeOutlook)
			{
				bMustRestart |= Security.FixupEmailEditor(true/*bSilent*/, true/*bAllowChange*/);
				bMustRestart |= Security.FixupRunActiveX(true/*bSilent*/, CLIENT_TYPE_OUTLOOK);
				bMustRestart |= Security.FixupSecurityZone(true/*bSilent*/, CLIENT_TYPE_OUTLOOK);
			}

			if (m_pOptimizeStep1->m_bOptimizeOE)
			{
				bMustRestart |= Security.FixupSecurityZone(true/*bSilent*/, CLIENT_TYPE_EXPRESS);
				bMustRestart |= Security.FixupRunActiveX(true/*bSilent*/, CLIENT_TYPE_EXPRESS);
				bMustRestart |= Security.FixupHtmlSettings(true/*bSilent*/, CLIENT_TYPE_EXPRESS);
			}

			if (m_pOptimizeStep1->m_bOptimizeIE)
			{
				bMustRestart |= Security.FixupRunActiveX(true/*bSilent*/, CLIENT_TYPE_IE);
				bMustRestart |= Security.EnableBrowserExtensions(true/*bSilent*/);
			}
			break;
		}
		case IDD_OPTIMIZEDLG_FINAL:
		case IDD_OPTIMIZEDLG_CANCEL:
		{
			CAuthenticate::ProcessRegistration(MODE_FIRSTRUN);
			EndDialog(IDOK);
			return S_OK;
		}
	}

	if (m_idSelectedView < IDD_OPTIMIZEDLG_FINAL)
		ShowSelectedView(m_idSelectedView + 1);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT COptimizeDlg::OnDone(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	switch (m_idSelectedView)
	{
		case IDD_OPTIMIZEDLG_INTRO:
		case IDD_OPTIMIZEDLG_STEP1:
		{
			ShowSelectedView(IDD_OPTIMIZEDLG_CANCEL);
			break;
		}
		case IDD_OPTIMIZEDLG_FINAL:
		case IDD_OPTIMIZEDLG_CANCEL:
		{
			EndDialog(IDOK);
			return S_OK;
		}
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
void COptimizeDlg::SetButton(UINT wID, LPCSTR pString, bool bEnabled, bool bDefault)
{
	SetDlgItemText(wID, pString);
	GetDlgItem(wID).EnableWindow(bEnabled);
	GetDlgItem(wID).ModifyStyle((bDefault ? 0 : BS_DEFPUSHBUTTON)/*bRemove*/, (bDefault ? BS_DEFPUSHBUTTON : 0)/*bAdd*/);
	if (bDefault)
		GetDlgItem(wID).SetFocus();
}

//////////////////////////////////////////////////////////////////////
void COptimizeDlg::ShowSelectedView(int idSelectedView)
{
	m_idSelectedView = idSelectedView;

	switch (m_idSelectedView)
	{
		case IDD_OPTIMIZEDLG_INTRO:
		{
			m_pOptimizeIntro->ShowWindow(SW_SHOW);
			m_pOptimizeStep1->ShowWindow(SW_HIDE);
			m_pOptimizeFinal->ShowWindow(SW_HIDE);
			m_pOptimizeCancel->ShowWindow(SW_HIDE);
			SetButton(IDC_BACK, "Back", false/*bEnabled*/, false/*bDefault*/);
			SetButton(IDC_NEXT, "Next", true/*bEnabled*/, true/*bDefault*/);
			SetButton(IDC_DONE, "Cancel", true/*bEnabled*/, false/*bDefault*/);
			break;
		}
		case IDD_OPTIMIZEDLG_STEP1:
		{
			m_pOptimizeIntro->ShowWindow(SW_HIDE);
			m_pOptimizeStep1->ShowWindow(SW_SHOW);
			m_pOptimizeFinal->ShowWindow(SW_HIDE);
			m_pOptimizeCancel->ShowWindow(SW_HIDE);
			SetButton(IDC_BACK, "Back", true/*bEnabled*/, false/*bDefault*/);
			SetButton(IDC_NEXT, "Optimize", true/*bEnabled*/, true/*bDefault*/);
			SetButton(IDC_DONE, "Cancel", true/*bEnabled*/, false/*bDefault*/);
			break;
		}
		case IDD_OPTIMIZEDLG_FINAL:
		{
			m_pOptimizeIntro->ShowWindow(SW_HIDE);
			m_pOptimizeStep1->ShowWindow(SW_HIDE);
			m_pOptimizeFinal->ShowWindow(SW_SHOW);
			m_pOptimizeCancel->ShowWindow(SW_HIDE);
			SetButton(IDC_BACK, "Back", false/*bEnabled*/, false/*bDefault*/);
			SetButton(IDC_NEXT, (m_bRegistered ? "Next" : "Register Now"), !m_bRegistered/*bEnabled*/, !m_bRegistered/*bDefault*/);
			SetButton(IDC_DONE, (m_bRegistered ? "Done" : "Skip Register"), true/*bEnabled*/, m_bRegistered/*bDefault*/);
			m_pOptimizeFinal->GetDlgItem(IDC_STATIC_UNREGISTERED).ShowWindow(m_bRegistered ? SW_HIDE : SW_SHOW);
			break;
		}
		case IDD_OPTIMIZEDLG_CANCEL:
		{
			m_pOptimizeIntro->ShowWindow(SW_HIDE);
			m_pOptimizeStep1->ShowWindow(SW_HIDE);
			m_pOptimizeFinal->ShowWindow(SW_HIDE);
			m_pOptimizeCancel->ShowWindow(SW_SHOW);
			SetButton(IDC_BACK, "Back", false/*bEnabled*/, false/*bDefault*/);
			SetButton(IDC_NEXT, (m_bRegistered ? "Next" : "Register Now"), !m_bRegistered/*bEnabled*/, !m_bRegistered/*bDefault*/);
			SetButton(IDC_DONE, (m_bRegistered ? "Done" : "Skip Register"), true/*bEnabled*/, m_bRegistered/*bDefault*/);
			m_pOptimizeCancel->GetDlgItem(IDC_STATIC_UNREGISTERED).ShowWindow(m_bRegistered ? SW_HIDE : SW_SHOW);
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// COptimizeDlgIntro
//////////////////////////////////////////////////////////////////////
COptimizeDlgIntro::COptimizeDlgIntro(COptimizeDlg* pParent)
{
	m_pParent = pParent;
}

//////////////////////////////////////////////////////////////////////
LRESULT COptimizeDlgIntro::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return true;
}

//////////////////////////////////////////////////////////////////////
// COptimizeDlgStep1
//////////////////////////////////////////////////////////////////////
COptimizeDlgStep1::COptimizeDlgStep1(COptimizeDlg* pParent)
{
	m_pParent = pParent;
	m_bOptimizeOutlook = true;
	m_bOptimizeOE = true;
	m_bOptimizeIE = true;
}

//////////////////////////////////////////////////////////////////////
LRESULT COptimizeDlgStep1::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CheckDlgButton(IDC_OPTIMIZE_OUTLOOK, (m_bOptimizeOutlook ? BST_CHECKED : BST_UNCHECKED));
	CheckDlgButton(IDC_OPTIMIZE_OE, (m_bOptimizeOE ? BST_CHECKED : BST_UNCHECKED));
	CheckDlgButton(IDC_OPTIMIZE_IE, (m_bOptimizeIE ? BST_CHECKED : BST_UNCHECKED));
	return true;
}

//////////////////////////////////////////////////////////////////////
LRESULT COptimizeDlgStep1::OnOptimizeOption(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	switch (wID)
	{
		case IDC_OPTIMIZE_OUTLOOK:
		{
			m_bOptimizeOutlook = !m_bOptimizeOutlook;
			CheckDlgButton(wID, (m_bOptimizeOutlook ? BST_CHECKED : BST_UNCHECKED));
			break;
		}
		case IDC_OPTIMIZE_OE:
		{
			m_bOptimizeOE = !m_bOptimizeOE;
			CheckDlgButton(wID, (m_bOptimizeOE ? BST_CHECKED : BST_UNCHECKED));
			break;
		}
		case IDC_OPTIMIZE_IE:
		{
			m_bOptimizeIE = !m_bOptimizeIE;
			CheckDlgButton(wID, (m_bOptimizeIE ? BST_CHECKED : BST_UNCHECKED));
			break;
		}
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// COptimizeDlgFinal
//////////////////////////////////////////////////////////////////////
COptimizeDlgFinal::COptimizeDlgFinal(COptimizeDlg* pParent)
{
	m_pParent = pParent;
}

//////////////////////////////////////////////////////////////////////
LRESULT COptimizeDlgFinal::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return true;
}


//////////////////////////////////////////////////////////////////////
// COptimizeDlgCancel
//////////////////////////////////////////////////////////////////////
COptimizeDlgCancel::COptimizeDlgCancel(COptimizeDlg* pParent)
{
	m_pParent = pParent;
}

//////////////////////////////////////////////////////////////////////
LRESULT COptimizeDlgCancel::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return true;
}

