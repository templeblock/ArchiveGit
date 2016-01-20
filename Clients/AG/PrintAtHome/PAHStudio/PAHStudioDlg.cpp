// PAHStudioDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PAHStudio.h"
#include "PAHStudioDlg.h"
#include "DlgProxy.h"
#include "PAH_Constants.h"

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib") // for ::timeGetTime

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	EnableActiveAccessibility();
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CPAHStudioDlg dialog

BEGIN_DHTML_EVENT_MAP(CPAHStudioDlg)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
	DHTML_EVENT_ONCLICK(_T("ButtonHelp"), OnButtonHelp)
	//DHTML_EVENT_AXCONTROL(2, _T("pah"), OnPluginReady)
END_DHTML_EVENT_MAP()



IMPLEMENT_DYNAMIC(CPAHStudioDlg, CDHtmlDialog);

CPAHStudioDlg::CPAHStudioDlg(CWnd* pParent /*=NULL*/, LPCTSTR szFilePath /*=NULL*/)
	: CDHtmlDialog(CPAHStudioDlg::IDD, CPAHStudioDlg::IDH, pParent)
{
	EnableActiveAccessibility();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = NULL;
	m_szFilePath = szFilePath;
}

CPAHStudioDlg::~CPAHStudioDlg()
{
	// If there is an automation proxy for this dialog, set
	//  its back pointer to this dialog to NULL, so it knows
	//  the dialog has been deleted.
	if (m_pAutoProxy != NULL)
		m_pAutoProxy->m_pDialog = NULL;
}

void CPAHStudioDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
	DDX_DHtml_AxControl( pDX, _T("pah"), _T("src") /* Value */, CComVariant(m_szFilePath));

}

BEGIN_MESSAGE_MAP(CPAHStudioDlg, CDHtmlDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_WM_HELPINFO()
	ON_MESSAGE(UWM_PLUGINREADY, OnPluginReady)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
bool CALLBACK CPAHStudioDlg::MyBrokerCallback(WPARAM wParam, LPARAM lParam, IDispatch * pDisp)
{
	CPAHStudioDlg* pPAHDlg = (CPAHStudioDlg*)lParam;
	if (pPAHDlg && NULL != pDisp) 
	{
		pPAHDlg->SetPlugin(pDisp);
		pPAHDlg->LoadFile();
		return true;
	}

	return false;
}
// CPAHStudioDlg message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CPAHStudioDlg::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	SetWindowPos(NULL, 0, 0, 800, 600, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
	CenterWindow();
	if (m_pBrowserApp)
	{
		m_pBrowserApp->put_RegisterAsDropTarget(VARIANT_FALSE);
		m_pBrowserApp->put_ToolBar(VARIANT_FALSE);
		m_pBrowserApp->put_StatusBar(VARIANT_TRUE);
		m_pBrowserApp->put_MenuBar(VARIANT_FALSE);
		m_pBrowserApp->put_Resizable(VARIANT_TRUE);
		m_pBrowserApp->put_AddressBar(VARIANT_FALSE);
	}
	
	HRESULT hr = m_spBroker.CoCreateInstance(CComBSTR("PAHBrokerManager.Broker"));
	if (SUCCEEDED(hr) && NULL != m_spBroker)
	{
		CComQIPtr<IDispatch> spDispatch = m_spBroker;
		SetExternalDispatch(m_spBroker);
	}

	//Navigate(_T("C:\\My Projects\\AgApplications\\PrintAtHome\\PAHStudio\\Pah.htm"), navNoHistory/*dwFlags*/, _T("_self")/*lpszTargetFrameName*/, NULL/*lpszHeaders*/, NULL/*lpvPostData*/, 0/*dwPostDataLen*/);
	LoadFromResource(IDR_HTML_PAH);

	return TRUE;  // return TRUE  unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
void CPAHStudioDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDHtmlDialog::OnSysCommand(nID, lParam);
	}
}
/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CPAHStudioDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDHtmlDialog::OnPaint();
	}
}
/////////////////////////////////////////////////////////////////////////////
// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPAHStudioDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
/////////////////////////////////////////////////////////////////////////////
// Automation servers should not exit when a user closes the UI
//  if a controller still holds on to one of its objects.  These
//  message handlers make sure that if the proxy is still in use,
//  then the UI is hidden but the dialog remains around if it
//  is dismissed.
/////////////////////////////////////////////////////////////////////////////
void CPAHStudioDlg::OnClose()
{
	if (CanExit())
		CDHtmlDialog::OnClose();
}
/////////////////////////////////////////////////////////////////////////////
void CPAHStudioDlg::OnOK()
{
	if (CanExit())
		CDHtmlDialog::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
void CPAHStudioDlg::OnCancel()
{
	if (CanExit())
		CDHtmlDialog::OnCancel();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPAHStudioDlg::CanExit()
{
	m_spBroker = NULL;
	// If the proxy object is still around, then the automation
	//  controller is still holding on to this application.  Leave
	//  the dialog around, but hide its UI.
	if (m_pAutoProxy != NULL)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CPAHStudioDlg::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CPAHStudioDlg::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CPAHStudioDlg::OnButtonHelp(IHTMLElement* /*pElement*/)
{
	OnHelp();
	return S_OK;
}
///////////////////////////////////////////////////////////////////////////////
//void __stdcall CPAHStudioDlg::OnPluginReady()
//{
//	LoadFile();
//	return;
//}
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPAHStudioDlg::GetDropTarget(IDropTarget* pDropTarget, IDropTarget** ppDropTarget)
{
	CComPtr<IDispatch> spDisp;

	HRESULT hr = GetControlDispatch(_T("pah"), &spDisp);
	if (FAILED(hr))
		return E_FAIL;

	CComQIPtr<IDropTarget> spCtpDropTarget(spDisp);
	if (NULL == spCtpDropTarget)
		return E_FAIL;

	*ppDropTarget = spCtpDropTarget;

	//*ppDropTarget = this;
	return S_OK;
}
///////////////////////////////////////////////////////////////////////////////
bool CPAHStudioDlg::Wait(DWORD dwExpire, bool bBusyOverride)
{
	// Wait for the document to become ready, or time out
	VARIANT_BOOL bBusy = true; 
	DWORD dwMaxTime = ::timeGetTime() + dwExpire;
	while (bBusy || bBusyOverride)
	{
		m_pBrowserApp->get_Busy(&bBusy);
		MSG msg;
		BOOL bRet = ::GetMessage(&msg, NULL, 0, 0);
		if (!bRet)
			return false; // WM_QUIT, exit message loop

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);

		if (::timeGetTime() > dwMaxTime)
			return false;
	}

	return true;//(!(BOOL)bBusy);
}
/////////////////////////////////////////////////////////////////////////////
bool CPAHStudioDlg::GetHtmlDoc(IHTMLDocument2 ** pDoc)
{	
	if (!m_pBrowserApp)
		return false;

	CComPtr<IDispatch> spDisp;
	HRESULT hr = m_pBrowserApp->get_Document(&spDisp);
	CComQIPtr<IHTMLDocument2> spDoc(spDisp);
	if (FAILED(hr))
		return false;
	
	*pDoc = spDoc.p;
	(*pDoc)->AddRef();

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CPAHStudioDlg::LoadFile()
{
	if (m_szFilePath.IsEmpty())
		return false;

	if (NULL == m_spCtp)
		return false;

	HRESULT hr = m_spCtp->put_Src(CComBSTR(m_szFilePath));
	if (FAILED(hr))
		return false;

	
	return true;
}
/////////////////////////////////////////////////////////////////////////////
void CPAHStudioDlg::SetFile(LPCTSTR szFilePath)
{
	m_szFilePath = szFilePath;
}
/////////////////////////////////////////////////////////////////////////////
bool CPAHStudioDlg::SetPlugin(IDispatch *pDisp)
{
	m_spCtp = NULL;
	CComQIPtr<ICtp> spCtp(pDisp);
	if (!spCtp)
		return false;

	m_spCtp = spCtp;

	return true;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPAHStudioDlg::CanAccessExternal()
{
	return true;
}
/////////////////////////////////////////////////////////////////////////////
void CPAHStudioDlg::OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
	CComPtr<IHTMLDocument2>spDoc;
	if (!GetHtmlDoc(&spDoc))
		return;

	if (spDoc)
	{
		VARIANT_BOOL bSuccess = false;
		m_spBroker->SetDocument(spDoc);
		m_spBroker->CallJavaScript(_T("LoadProject"), CComVariant(m_szFilePath), &bSuccess);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPAHStudioDlg::OnNavigateComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
	//LoadFile();
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CPAHStudioDlg::OnFrameWindowActivate(BOOL fActivate)
{
	//LoadFile();
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPAHStudioDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	// TODO: Add your message handler code here and/or call default

	return CDHtmlDialog::OnHelpInfo(pHelpInfo);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CPAHStudioDlg::OnPluginReady(WPARAM wParam, LPARAM lParam)
{
	

	return S_OK;
}
