// ECardDialog.h : Declaration of the CECardDialog
#pragma once
#include "resource.h"
#include "ListBoxECard.h"

class CECardDialog : public CAxDialogImpl<CECardDialog>,
					 public CWinDataExchange<CECardDialog>
{
public:
	CECardDialog();
	~CECardDialog();

	enum { IDD = IDD_ECARD_DIALOG };

	BEGIN_MSG_MAP(CECardDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		COMMAND_HANDLER(IDC_ECARD_LIST, LBN_KILLFOCUS, OnKillFocus)
		NOTIFY_HANDLER(IDC_ECARD_LIST, LVN_ITEMACTIVATE, OnItemActivate)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	

	// Message handlers
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKillFocus(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnItemActivate(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);

	
	// Methods
	bool Display(HWND hParent=NULL);

protected:
	void PositionWindow();
	bool InitListBox();
	int AddImage(DWORD dwID);

private:
	CImageList m_ImageList;
	CListBoxECard m_ECardList;

};
