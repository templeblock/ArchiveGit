#ifndef __CTLPANEL_H_
#define __CTLPANEL_H_

#include "resource.h"
#include "AGDoc.h"
#include "AGText.h"
#include "Font.h"

class CCtp;

/////////////////////////////////////////////////////////////////////////////
// CCtlPanel
class CCtlPanel : 
	public CDialogImpl<CCtlPanel>
{
public:
	CCtlPanel(CCtp* pMainWnd);
	~CCtlPanel();

	enum { IDD = IDD_CTLPANEL };

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtlColorDlg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtlColorStatic(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPrint(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnHorzJust(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnFont(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPtSize(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnColor(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	HWND Create(HWND hWndParent);
	FONTARRAY& GetFontArray();
	void SetDoc(CAGDoc* pAGDoc);
	void SetFonts();
	void UpdateControls(const CAGText* pText);

public:
	BEGIN_MSG_MAP(CCtlPanel)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CTLCOLORDLG, OnCtlColorDlg)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)
		MESSAGE_HANDLER(WM_MEASUREITEM, OnMeasureItem)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		COMMAND_ID_HANDLER(IDC_PAGE1, OnPage)
		COMMAND_ID_HANDLER(IDC_PAGE2, OnPage)
		COMMAND_ID_HANDLER(IDC_PAGE3, OnPage)
		COMMAND_ID_HANDLER(IDC_PAGE4, OnPage)
		COMMAND_ID_HANDLER(IDC_LEFT, OnHorzJust)
		COMMAND_ID_HANDLER(IDC_CENTER, OnHorzJust)
		COMMAND_ID_HANDLER(IDC_RIGHT, OnHorzJust)
		COMMAND_ID_HANDLER(IDC_PRINT, OnPrint)
		COMMAND_ID_HANDLER(IDC_FONT, OnFont)
		COMMAND_ID_HANDLER(IDC_PTSIZE, OnPtSize)
		COMMAND_ID_HANDLER(IDC_COLOR, OnColor)
	END_MSG_MAP()

protected:
	CCtp* m_pMainWnd;
	CAGDoc* m_pAGDoc;
	HGLOBAL m_hDevMode;
	HGLOBAL m_hDevNames;
	int m_PageMap[MAX_AGPAGE];
	int m_nFontHeight;
	HGLOBAL m_hDlg;
};

#endif //__CTLPANEL_H_
