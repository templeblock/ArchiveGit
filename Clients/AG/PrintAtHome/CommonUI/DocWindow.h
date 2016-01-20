#pragma once

#include "resource.h"
#include "Ctp.h"
#include "AGDC.h"
#include "AGDoc.h"
#include "AGMatrix.h"
#include "AGSym.h"
#include "Select.h"
#include "EditFunctions.h"
#include "Undo.h"
#include "Grid.h"
#include "ProgressDlg.h"
#include "Print.h"
#include "EnvelopeWizDlg.h"
#include "HTMLDialogEx.h"
#include "Highlight.h"
#include "DWebBrowserEventsImpl.h"
#include "WebPage.h"
#include "pah_constants.h"


#define POS_LEFT	1
#define POS_CENTER	2
#define POS_RIGHT	3
#define POS_TOP		4
#define POS_MIDDLE	5
#define POS_BOTTOM	6

#define TOOLCODE_ABORT	-1
#define TOOLCODE_UNUSED	 0
#define TOOLCODE_DONE	 1
#define TOOLCODE_UPDATE	 2

struct OPENFILENAMEX
{
	OPENFILENAME of;
	void* pvReserved;
	DWORD dwReserved;
	DWORD FlagsEx;
};
struct LINKEDDOC
{
	bool bModified;
    bool bSwitchInProgress;
    AGDOCTYPE DocType;
	CString szFilename;
    CString szEnvelopeFilePath;
    CString szCardFilePath;
};
class CEnvelopeWizard;

class CDocWindow :
	public CWindowImpl<CDocWindow>,
	public DWebBrowserEventsImpl

{
	friend class CCtp;
    friend class CSelect;
	friend class CHighlight;

public:
	CDocWindow(CCtp* pMainWnd);
	~CDocWindow();

	CAGDoc* GetDoc()
		{ return m_pAGDoc; }
	void SetDoc(CAGDoc* pAGDoc)
		{ if (m_pAGDoc) delete m_pAGDoc; m_pAGDoc = pAGDoc; }

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// Events
	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPluginReady(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// Commands
	#include "CtpCommands.h"

	CUndo& GetUndo()
		{ return m_Undo; }
	CSelect& GetSelect()
		{ return m_Select; }
	HHOOK GetKeyboardHook()
		{ return m_hKeyboardHook; }
	CAGSym* GetSelectedItem()
		{ return m_Select.GetSym(); }
	CAGText* GetText()
		{ return ((m_Select.SymIsText() || m_Select.SymIsAddAPhoto()) ? (CAGSymText*)m_Select.GetSym() : NULL); }
	CAGSymCalendar* GetCalendar()
		{ return m_Select.GetCalendarSym(); }
	void SymbolSelect(CAGSym* pSym, POINT* pClickPoint = NULL)
		{ m_Select.SymbolSelect(pSym, pClickPoint); }
	void SymbolUnselect(bool bClearPointer, bool fSetPanel = true)
		{ m_Select.SymbolUnselect(bClearPointer, fSetPanel); }
	bool HasFocus()
		{ return m_bHasFocus; }
	bool SpecialsEnabled()
		{ return m_bSpecialsEnabled; }
	void SetSpecialsEnabled(bool bEnabled)
		{ m_bSpecialsEnabled = bEnabled; }
	bool GetShowHidden() { return m_bShowHiddenSymbols; }
	bool GetHiddenFlag()
		{ return (m_bShowHiddenSymbols ? PROCESS_HIDDEN : 0); }
	void PageInvalidate();
	void Repaint(const RECT& Rect);
	void UpdateInfo();
	static DWORD WINAPI UpdateInfoProc(void* pVoid);
	DWORD UpdateInfoProc();
	void DisplaySavedFileSize();
	static DWORD WINAPI DisplaySavedFileSizeProc(void* pVoid);
	DWORD DisplaySavedFileSizeProc();
	void SymbolInvalidate(CAGSym* pSym);
	bool DoCommand(const CString& strCommand);
	DWORD GetCommand(const CString& strCommand);
	bool IsProgressDlgUp();
	void DrawProgressDlg(LPCSTR pMessage, bool bCreateIfNeeded = true);
	CAGClientDC* GetClientDC() { return m_pClientDC;}
	bool DoFYSDataTransfer(CString OrderId, CString CorrelationId, CString FYSInfo, CString& strError);
	void CloseModelessDlg();
	LRESULT DoSpellCheck();
	bool HasLinkedDoc();
	bool CallJavaScript(LPCTSTR szFunction, LPCTSTR szArg1="");
	bool GetWebBrowserInterface(IWebBrowser2 ** pBrowser);
	bool GetHtmlDoc(IHTMLDocument2 ** pDoc);
	virtual void DocumentComplete(IDispatch *pDisp, VARIANT *URL);
	virtual void BeforeNavigate2(IDispatch *pDisp, VARIANT *url, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, VARIANT_BOOL *Cancel);




	//void SetHTMLDlgHWND(HWND hWnd) { m_HTMLDlghWnd = hWnd; }

protected:
	void Free();
	void ActivateNewDoc();
    void CleanupLinkedDoc(bool bExit=true);
    bool SwitchToEnvelope(bool bAtPrintTime=false);
    bool SwitchToCard();
    bool SaveToTemp();
    bool RemoveTempFilesByType(AGDOCTYPE DocType);
    bool RemoveAllTempFiles();
    bool CloseLinkedDoc(bool bExit);
    bool Refresh(LPCTSTR szFilename);
    void InitializeLinkedDoc();


public:
	void ActivateNewPage();

protected:
	void DoDropImage(LPCSTR strFileName, POINTL Pt);
	void DoDragOver(POINTL Pt);
	void DoDragLeave();
	CAGSym* GetSymUnderPt(POINTL Pt);
	void DeleteAddAPhotoImage(CAGLayer* pActiveLayer, CAGSym* pAddAPhotoSym);
	void SetupPageView(bool bSetScrollbars, bool bOffset);
	void SetupScrollbars(int xPos, int yPos, int xSize, int ySize, int xVisible, int yVisible, bool bRedraw);
	void EliminateScrollbars(bool bRedraw);
	void ResetScrollPos(int nBar, int nPos, bool bRedraw);
	void SetScrollPage(int nBar, UINT nPage, bool bRedraw);
	UINT GetScrollPage(int nBar);
	static LRESULT CALLBACK CtpKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
	static UINT CALLBACK OpenFileHookProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);

public:
	BEGIN_MSG_MAP(CDocWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow) // window 'hide' is the logical OnClose
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_RANGE_HANDLER(WM_CTLCOLORMSGBOX, WM_CTLCOLORSTATIC, OnCtlColor)
		MESSAGE_HANDLER(WM_CHAR, OnChar)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_RBUTTONUP, OnRButtonUp)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_HSCROLL, OnScroll)
		MESSAGE_HANDLER(WM_VSCROLL, OnScroll)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(UWM_PLUGINREADY, OnPluginReady)

		// Commands
		#include "CtpCommandMessages.h"
	END_MSG_MAP()

public:
	CCtp* m_pCtp;
	CPrint m_Print;
    static LPCTSTR TEMP_ENVELOPES_DIR;
    static LPCTSTR TEMP_CARDS_DIR;

protected:
	static CDocWindow* g_pDocWindow;
	CAGDoc* m_pAGDoc;
	HHOOK m_hKeyboardHook;
	CAGClientDC* m_pClientDC;
	CProgressDlg* m_pProgressDlg;
	SIZE m_ShadowSize;
	bool m_bSpecialsEnabled;
	POINT m_ptCascade;
	RECT m_PageRect;
	bool m_bHasFocus;
	bool m_bEditMasterLayer;
	bool m_bShowHiddenSymbols;
	bool m_bMetricMode;
	bool m_bIgnoreSetCursor;
	CGrid m_Grid;
	int m_iZoom;
	double m_fZoomScale;
	CEditFunctions Clipboard;
	IHTMLWindow2* m_pHelpWindow;
	HANDLE m_hUpdateInfoThread;
	CEnvelopeWizard* m_pEnvelopeWizard;
    CSelect m_Select;
    CUndo m_Undo;
	CHighlight m_Highlight;
    CString m_szTempDir;
    CRecentDocumentList m_mruList;
	CHTMLDialogEx m_HTMLModelessDlg;
    CMenu m_popupFile;
    CMenu m_menuFile;
    LINKEDDOC m_LinkedDoc;
	CComPtr<IWebBrowser2> m_spBrowser;
	CWebPage* m_pWebPage;
//j	CToolTipCtrl m_ToolTip;
};
