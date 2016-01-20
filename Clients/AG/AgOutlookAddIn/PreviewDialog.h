#pragma once
#include "resource.h"
#include <atlframe.h>

class CMsAddInBase;
struct DownloadParms;

class CPreviewDialog :	public CAxDialogImpl<CPreviewDialog>,
						public CDialogResize<CPreviewDialog>		
{
public:
	CPreviewDialog(CMsAddInBase* pAddin);
	~CPreviewDialog(void);

	enum { IDD = IDD_PREVIEWDIALOG };

	BEGIN_MSG_MAP(CPreviewDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		COMMAND_HANDLER(ID_CONTENT_SEND, BN_CLICKED, OnBnClickedButton1)
		COMMAND_HANDLER(ID_CONTENT_ADDTOFAVORITES, BN_CLICKED, OnBnClickedButton2)
		COMMAND_HANDLER(ID_PREV_CLOSE, BN_CLICKED, OnBnClickedButton3)
		COMMAND_HANDLER(ID_OPTIONS_UPGRADENOW, BN_CLICKED, OnBnClickedButton4)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		CHAIN_MSG_MAP(CDialogResize<CPreviewDialog>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CPreviewDialog)

		// WebBrowser control
		DLGRESIZE_CONTROL(IDC_EXPLORER, DLSZ_SIZE_X|DLSZ_SIZE_Y)

		// Buttons
		//BEGIN_DLGRESIZE_GROUP()
            DLGRESIZE_CONTROL(ID_CONTENT_SEND, DLSZ_MOVE_X|DLSZ_MOVE_Y)
			DLGRESIZE_CONTROL(ID_CONTENT_ADDTOFAVORITES, DLSZ_MOVE_X|DLSZ_MOVE_Y)
            DLGRESIZE_CONTROL(ID_PREV_CLOSE, DLSZ_MOVE_X|DLSZ_MOVE_Y)
            DLGRESIZE_CONTROL(ID_OPTIONS_UPGRADENOW, DLSZ_MOVE_X|DLSZ_MOVE_Y)
        //END_DLGRESIZE_GROUP()

    END_DLGRESIZE_MAP()

public:
	BOOL Display(HWND hWndParent, DownloadParms& Parms, int iWidth=0, int iHeight=0 );
	BOOL AddHtmlToPreview(LPCTSTR pstrHtml);
	BOOL Load(LPCTSTR pstrURL);
	void Close();
	void SetAddin(CMsAddInBase* pAddin);

private:
	void PreviewCallback(IDispatch* pDisp1, IDispatch* pDisp2, DISPID id, VARIANT* pVarResult);
	void ShowMenu();
	bool HandleDocLoadComplete(IHTMLDocument2Ptr spDoc);
	BOOL SetMousedownCallback(IHTMLDocument2* pDoc, BOOL bReset);
	BOOL SetBodySelectCallback(IHTMLDocument2* pDoc, BOOL bReset);
	BOOL SetKeyPressCallback(IHTMLDocument2 *pDoc, BOOL bReset);
	BOOL SetDragStartCallback(IHTMLDocument2 *pDoc, BOOL bReset);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBnClickedButton1(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBnClickedButton2(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBnClickedButton3(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBnClickedButton4(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

private:
	CMsAddInBase* m_pAddin;
	DownloadParms* m_pDownloadParms;
	CComPtr<IWebBrowser2> m_pBrowser;
	CComPtr<IHTMLDocument2> m_spCurrentDoc;
	CAxWindow m_BrowserWnd;
	CString m_strURL;
	CString m_strTempFile;
	CString m_strHtml;

public:
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};
