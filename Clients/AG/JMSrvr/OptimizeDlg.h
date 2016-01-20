#pragma once

#include "resource.h"
#include <atlhost.h>

class COptimizeDlgIntro;
class COptimizeDlgStep1;
class COptimizeDlgFinal;
class COptimizeDlgCancel;

///////////////////////////////////////////////////////////////////////////////
class COptimizeDlg : public CDialogImpl<COptimizeDlg>
{
public:
	friend COptimizeDlgIntro;
	friend COptimizeDlgStep1;
	friend COptimizeDlgFinal;
	friend COptimizeDlgCancel;

	COptimizeDlg();
	~COptimizeDlg();

	enum { IDD = IDD_OPTIMIZEDLG };

	BEGIN_MSG_MAP(COptimizeDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_BACK, BN_CLICKED, OnBack)
		COMMAND_HANDLER(IDC_NEXT, BN_CLICKED, OnNext)
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnDone)
		COMMAND_HANDLER(IDC_DONE, BN_CLICKED, OnDone)
		COMMAND_HANDLER(IDOK, BN_CLICKED, OnOK)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBack(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnNext(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDone(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	void SetButton(UINT wID, LPCSTR pString, bool bEnabled, bool bDefault);
	void ShowSelectedView(int nSelectedView);

protected:
	UINT m_idSelectedView;
	COptimizeDlgIntro* m_pOptimizeIntro;
	COptimizeDlgStep1* m_pOptimizeStep1;
	COptimizeDlgFinal* m_pOptimizeFinal;
	COptimizeDlgCancel* m_pOptimizeCancel;
	bool m_bRegistered;
};

///////////////////////////////////////////////////////////////////////////////
class COptimizeDlgIntro : public CDialogImpl<COptimizeDlgIntro>
{
public:
	friend COptimizeDlg;

	enum { IDD = IDD_OPTIMIZEDLG_INTRO };

	BEGIN_MSG_MAP(COptimizeDlgIntro)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	END_MSG_MAP()

	COptimizeDlgIntro(COptimizeDlg* pParent);
	virtual ~COptimizeDlgIntro() {}

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	COptimizeDlg* GetOptimizeDlg() { return m_pParent; };

protected:
	COptimizeDlg* m_pParent;
};

/////////////////////////////////////////////////////////////////////////////
class COptimizeDlgStep1 : public CDialogImpl<COptimizeDlgStep1>
{
public:
	friend COptimizeDlg;

	enum { IDD = IDD_OPTIMIZEDLG_STEP1 };
	
	BEGIN_MSG_MAP(COptimizeDlgStep1)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_OPTIMIZE_OUTLOOK, BN_CLICKED, OnOptimizeOption)
		COMMAND_HANDLER(IDC_OPTIMIZE_OE, BN_CLICKED, OnOptimizeOption)
		COMMAND_HANDLER(IDC_OPTIMIZE_IE, BN_CLICKED, OnOptimizeOption)
	END_MSG_MAP()

	COptimizeDlgStep1(COptimizeDlg* pParent);
	virtual ~COptimizeDlgStep1() {}

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOptimizeOption(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	COptimizeDlg* GetOptimizeDlg() { return m_pParent; };

protected:
	COptimizeDlg* m_pParent;
	bool m_bOptimizeOutlook;
	bool m_bOptimizeOE;
	bool m_bOptimizeIE;
};

/////////////////////////////////////////////////////////////////////////////
class COptimizeDlgFinal : public CDialogImpl<COptimizeDlgFinal>
{
public:
	friend COptimizeDlg;

	enum { IDD = IDD_OPTIMIZEDLG_FINAL };

	BEGIN_MSG_MAP(COptimizeDlgFinal)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	END_MSG_MAP()

	COptimizeDlgFinal(COptimizeDlg* pParent);
	virtual ~COptimizeDlgFinal() {}

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	COptimizeDlg* GetOptimizeDlg() { return m_pParent; };

protected:
	COptimizeDlg* m_pParent;
};

/////////////////////////////////////////////////////////////////////////////
class COptimizeDlgCancel : public CDialogImpl<COptimizeDlgCancel>
{
public:
	friend COptimizeDlg;

	enum { IDD = IDD_OPTIMIZEDLG_CANCEL };

	BEGIN_MSG_MAP(COptimizeDlgCancel)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	END_MSG_MAP()

	COptimizeDlgCancel(COptimizeDlg* pParent);
	virtual ~COptimizeDlgCancel() {}

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	COptimizeDlg* GetOptimizeDlg() { return m_pParent; };

protected:
	COptimizeDlg* m_pParent;
};
