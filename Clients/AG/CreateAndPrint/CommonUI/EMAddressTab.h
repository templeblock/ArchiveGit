#pragma once

#include "resource.h"
#include "EMBaseTab.h"

class CAGSymImage;
class CDocWindow;
///////////////////////////////////////////////////////////////////////////////
// CEMAddressTab dialog
class CEMAddressTab : public CEMBaseTab<CEMAddressTab>
{
public:
	CEMAddressTab(CEMDlg * pEMDlg);
	virtual ~CEMAddressTab();

	BEGIN_MSG_MAP_EX(CEMAddressTab)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_HANDLER_EX(IDC_ADDRESSBOOK_BUTTON, BN_CLICKED, OnAddressbook)
		COMMAND_HANDLER_EX(IDC_SEND_ADDRESS, EN_CHANGE, OnApplySendAddress)
		COMMAND_HANDLER_EX(IDC_RETURN_ADDRESS, EN_CHANGE, OnApplyReturnAddress)
		/*COMMAND_HANDLER_EX(IDC_SEND_ADDRESS, EN_SETFOCUS, OnSelectSendAddress)
		COMMAND_HANDLER_EX(IDC_RETURN_ADDRESS, EN_SETFOCUS, OnSelectReturnAddress)*/
		COMMAND_RANGE_HANDLER_EX(IDC_RADIO_MANUAL, IDC_RADIO_NO_ADDRESS, OnMailingAddress)
		COMMAND_RANGE_HANDLER_EX(IDC_RADIO_RETURN_MAN, IDC_RADIO_RETURN_BLANK, OnReturnAddress)
		CHAIN_MSG_MAP(CEMBaseTab<CEMAddressTab>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(CEMAddressTab)
		DDX_CONTROL_HANDLE(IDC_ADDRESSBOOK_BUTTON, m_btnAddressbook);
		DDX_CONTROL_HANDLE(IDC_RADIO_MANUAL, m_radioManualEntry);
		DDX_CONTROL_HANDLE(IDC_RADIO_ADDRESSBOOK, m_radioAddressbook);
		DDX_CONTROL_HANDLE(IDC_RADIO_NO_ADDRESS, m_radioNoAddress);
		DDX_CONTROL_HANDLE(IDC_RADIO_RETURN_MAN, m_radioReturnManualEntry);
		DDX_CONTROL_HANDLE(IDC_RADIO_RETURN_BLANK, m_radioReturnNoAddress);
	END_DDX_MAP()

	enum { IDD = IDD_EMADDRESS_TAB };

	BOOL OnInitDialog(HWND hWnd, LPARAM lParam);
	void OnAddressbook(WORD wNotifyCode, WORD wID, HWND hWndCtl);
    void OnApplySendAddress(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	void OnApplyReturnAddress(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	//void OnSelectSendAddress(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	//void OnSelectReturnAddress(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	void OnMailingAddress(UINT code, UINT id, HWND hWnd);
	void OnReturnAddress(UINT code, UINT id, HWND hWnd);

	CString& GetReturnAddress()
		{ return m_strReturnAddress; }

	CString& GetSendAddress()
		{ return m_strSendAddress; }

public:
	void InitEnvelopeAddress();
	void UpdateMailingAddress();
	void UpdateReturnAddress();
	bool SetMailAddress(LPCTSTR szAddress, int nID, COLORREF clrFillColor=RGB(0,0,0));


protected:
	virtual void OnEnterTab(bool bFirstTime);
	virtual void OnLeaveTab();
	void GetTextFromAddressDisplay(int nId, CString& szValue);
	void SetTextToAddressDisplay(int nId, LPCTSTR szText);


protected:
	CButton m_radioAddressbook;
	CButton m_radioManualEntry;
	CButton m_radioNoAddress;
	CButton m_radioReturnManualEntry;
	CButton m_radioReturnNoAddress;
	CButton m_btnAddressbook;
	CString m_strSendAddress;
	CString m_strReturnAddress;
	CDocWindow* m_pDocWindow;
	


};
