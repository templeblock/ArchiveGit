// PrinterCal.h : Declaration of the CPrinterCal

#pragma once

#include "resource.h"       // main symbols
#include <atlhost.h>


// CPrinterCal

class CPrinterCal : 
	public CAxDialogImpl<CPrinterCal>
{
public:
	CPrinterCal()
	{
	}

	~CPrinterCal()
	{
	}

	enum { IDD = IDD_PRINTERCAL };

BEGIN_MSG_MAP(CPrinterCal)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
	COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
	COMMAND_HANDLER(IDC_PRINT, BN_CLICKED, OnBnClickedPrint)
	CHAIN_MSG_MAP(CAxDialogImpl<CPrinterCal>)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CAxDialogImpl<CPrinterCal>::OnInitDialog(uMsg, wParam, lParam, bHandled);
		return 1;  // Let the system set the focus
	}

	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		TCHAR szXValue[MAX_PATH];
		SendMessage(GetDlgItem(IDC_X_OFFSET), WM_GETTEXT, MAX_PATH, (LPARAM)(LPCSTR)&szXValue);
		m_nXOffset = atoi(szXValue);

		TCHAR szYValue[MAX_PATH];
		SendMessage(GetDlgItem(IDC_Y_OFFSET), WM_GETTEXT, MAX_PATH, (LPARAM)(LPCSTR)&szYValue);
		m_nYOffset = atoi(szYValue);

		EndDialog(wID);
		return 0;
	}

	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}

	int GetXOffset(int* pXOffset)
	{
		*pXOffset = m_nXOffset;
		return 0;
	}

	int GetYOffset(int* pYOffset)
	{
		*pYOffset = m_nYOffset;
		return 0;
	}

	int m_nXOffset;
	int m_nYOffset;
	LRESULT OnBnClickedPrint(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};


