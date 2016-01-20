#ifndef __PROP_SHEET_H__
#define __PROP_SHEET_H__

#include <commctrl.h>

template <class T>
class ATL_NO_VTABLE CPropertyPageImpl : public CDialogImplBase
{
public:
	PROPSHEETPAGE m_psp;

	operator PROPSHEETPAGE*() { return &m_psp; }

// Construction
	CPropertyPageImpl(LPCTSTR lpszTitle = NULL)
	{
		// initialize PROPSHEETPAGE struct
		memset(&m_psp, 0, sizeof(PROPSHEETPAGE));
		m_psp.dwSize = sizeof(PROPSHEETPAGE);
		m_psp.dwFlags = PSP_USECALLBACK;
		m_psp.hInstance = _Module.GetResourceInstance();
		m_psp.pszTemplate = MAKEINTRESOURCE(T::IDD);
		m_psp.pfnDlgProc = (DLGPROC)T::StartDialogProc;
		m_psp.pfnCallback = T::PropPageCallback;
		m_psp.lParam = (LPARAM)this;

		if (lpszTitle != NULL)
		{
			m_psp.pszTitle = lpszTitle;
			m_psp.dwFlags |= PSP_USETITLE;
		}
	}

	static UINT CALLBACK PropPageCallback(HWND hWnd, UINT uMsg, LPPROPSHEETPAGE ppsp)
	{
		_ASSERTE(hWnd == NULL);
		if (uMsg == PSPCB_CREATE)
		{
			CDialogImplBase* pPage = (CDialogImplBase*)ppsp->lParam;
			_Module.AddCreateWndData(&pPage->m_thunk.cd, pPage);
		}
		return 1;
	}

	HPROPSHEETPAGE Create()
	{
		return ::CreatePropertySheetPage(&m_psp);
	}

	BOOL EndDialog(int)
	{
		// do nothing here, calling ::EndDialog will close the whole sheet
		_ASSERTE(false);
		return false;
	}

// Operations
	void CancelToClose()
	{
		_ASSERTE(::IsWindow(m_hWnd));
		_ASSERTE(GetParent() != NULL);

		::SendMessage(GetParent(), PSM_CANCELTOCLOSE, 0, 0L);
	}
	void SetModified(BOOL bChanged = true)
	{
		_ASSERTE(::IsWindow(m_hWnd));
		_ASSERTE(GetParent() != NULL);

		if (bChanged)
			::SendMessage(GetParent(), PSM_CHANGED, (WPARAM)m_hWnd, 0L);
		else
			::SendMessage(GetParent(), PSM_UNCHANGED, (WPARAM)m_hWnd, 0L);
	}
	void SetWizardButtons(DWORD dwFlags)
	{
		_ASSERTE(::IsWindow(m_hWnd));
		_ASSERTE(GetParent() != NULL);

		::PostMessage(GetParent(), PSM_SETWIZBUTTONS, 0, (LPARAM) dwFlags);
	}
	LRESULT QuerySiblings(WPARAM wParam, LPARAM lParam)
	{
		_ASSERTE(::IsWindow(m_hWnd));
		_ASSERTE(GetParent() != NULL);

		return ::SendMessage(GetParent(), PSM_QUERYSIBLINGS, wParam, lParam);
	}

// Message handler
	LRESULT OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		_ASSERTE(::IsWindow(m_hWnd));
		NMHDR* pNMHDR = (NMHDR*)lParam;

		// don't handle messages not from the page/sheet itself
		if (pNMHDR->hwndFrom != m_hWnd && pNMHDR->hwndFrom != ::GetParent(m_hWnd))
		{
			bHandled = false;
			return 1;
		}

		T* pT = (T*)this;
		LRESULT lResult = 0;
		// handle default
		switch (pNMHDR->code)
		{
		case PSN_SETACTIVE:
			lResult = pT->OnSetActive() ? 0 : -1;
			break;
		case PSN_KILLACTIVE:
			lResult = !pT->OnKillActive();
			break;
		case PSN_APPLY:
			lResult = pT->OnApply() ? PSNRET_NOERROR : PSNRET_INVALID_NOCHANGEPAGE;
			break;
		case PSN_RESET:
			pT->OnReset();
			break;
		case PSN_QUERYCANCEL:
			lResult = !pT->OnQueryCancel();
			break;
		case PSN_WIZNEXT:
			lResult = !pT->OnWizardNext();
			break;
		case PSN_WIZBACK:
			lResult = !pT->OnWizardBack();
			break;
		case PSN_WIZFINISH:
			lResult = !pT->OnWizardFinish();
			break;
		case PSN_HELP:
			lResult = pT->OnHelp();
			break;
		default:
			bHandled = false;	// not handled
		}

		return lResult;
	}

// Overridables
	BOOL OnSetActive()
	{
		return true;
	}
	BOOL OnKillActive()
	{
		return true;
	}
	BOOL OnApply()
	{
		return true;
	}
	void OnReset()
	{
	}
	BOOL OnQueryCancel()
	{
		return true;	// ok to cancel
	}
	BOOL OnWizardBack()
	{
		return true;
	}
	BOOL OnWizardNext()
	{
		return true;
	}
	BOOL OnWizardFinish()
	{
		return true;
	}
	BOOL OnHelp()
	{
		return true;
	}

public:
	BEGIN_MSG_MAP(CPropertyPageImpl<T>)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
	END_MSG_MAP()
};

#endif //__PROP_SHEET_H__
