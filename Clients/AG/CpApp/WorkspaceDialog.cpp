#include "stdafx.h"
#include "resource.h"
#include "WorkspaceDialog.h"
#include <atlwin.h>
#include "AboutDlg.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

BEGIN_DHTML_EVENT_MAP(CWorkspaceDialog)
END_DHTML_EVENT_MAP()

BEGIN_MESSAGE_MAP(CWorkspaceDialog, CDHtmlDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDOK, OnCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CWorkspaceDialog::CWorkspaceDialog(CWnd* pParent, LPCSTR pstrURL, RECT* pRect, LPCSTR pstrName) :
	CDHtmlDialog(CWorkspaceDialog::IDD, NULL/*CWorkspaceDialog::IDH*/, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(GetIconId());
	m_strUrl = pstrURL;
	m_pRect = pRect;
	m_strName = pstrName;
}

/////////////////////////////////////////////////////////////////////////////
CWorkspaceDialog::~CWorkspaceDialog()
{
}

/////////////////////////////////////////////////////////////////////////////
CWorkspaceDialog* CWorkspaceDialog::CreateModeless(CWnd* pParent, LPCSTR pstrURL, RECT* pRect, LPCSTR pstrName)
{
	CWaitCursor Wait;
	
	CWorkspaceDialog* pWorkspace = new CWorkspaceDialog(pParent, pstrURL, pRect, pstrName);
	if (!pWorkspace)
		return NULL;

	if (pWorkspace->Create(IDD, pParent))
		return pWorkspace;

	delete pWorkspace;
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
void CWorkspaceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CWorkspaceDialog::OnInitDialog()
{
	CString strCaption;
	strCaption.LoadString(GetTitleId());
	if (!m_strName.IsEmpty())
		strCaption += " " + m_strName;
	SetWindowText(strCaption);

	// Initialize the url prior to calling CDHtmlDialog::OnInitDialog()
	if (m_strCurrentUrl.IsEmpty())
		m_strCurrentUrl = "about:blank";

	CDHtmlDialog::OnInitDialog();
	m_bUseHtmlTitle = false;

	// This magically makes the scroll bars appear and dis-allows text selection
	DWORD dwFlags = DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_THEME | DOCHOSTUIFLAG_DIALOG; // DOCHOSTUIFLAG_NO3DOUTERBORDER;
	SetHostFlags(dwFlags);

	// Add "About..." menu item to system menu.
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(false);
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

	// Set the icons for this dialog
	SetIcon(m_hIcon, true);		// Set big icon
	SetIcon(m_hIcon, false);	// Set small icon

	// Set the window size and position
	if (!m_pRect)
		CenterWindow();
	else
	{
		CRect Rect = *m_pRect;
		SetWindowPos(NULL, Rect.left, Rect.top, Rect.Width(), Rect.Height(), SWP_NOZORDER | SWP_NOACTIVATE);
	}

	// Copy this to CpDialog.cpp
	if (m_pBrowserApp)
	{
		m_pBrowserApp->put_RegisterAsDropTarget(VARIANT_FALSE);
		m_pBrowserApp->put_ToolBar(VARIANT_FALSE);
		m_pBrowserApp->put_StatusBar(VARIANT_TRUE);
		m_pBrowserApp->put_MenuBar(VARIANT_FALSE);
		m_pBrowserApp->put_Resizable(VARIANT_TRUE);
		m_pBrowserApp->put_AddressBar(VARIANT_FALSE);
	}

	Navigate(m_strUrl, navNoHistory/*dwFlags*/, "_self"/*lpszTargetFrameName*/, NULL/*lpszHeaders*/, NULL/*lpvPostData*/, 0/*dwPostDataLen*/);

	// Wait until the page is loaded
	if (m_pBrowserApp)
	{
		VARIANT_BOOL bBusy = true; // Initialize this to true if you want to wait for the document to load
		int i = 300;
		while (bBusy)
		{
			::Sleep(100);
			m_pBrowserApp->get_Busy(&bBusy);
			if (--i <= 0)
				break;
		}
	}

	ShowWindow(SW_NORMAL);

	DragAcceptFiles(false);

	return true;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
BOOL CWorkspaceDialog::PreTranslateMessage(MSG* pMsg) 
{
	if (!pMsg)
		return false;

	if (!m_hWnd)
		return false;

	if (!m_pBrowserApp)
		return false;

	// check if the browser control wants to handle the message
	CComQIPtr<IOleInPlaceActiveObject> spActiveObject = m_pBrowserApp;
	if (!spActiveObject)
		return false;

	return (spActiveObject->TranslateAccelerator(pMsg) == S_OK);
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CWorkspaceDialog::ShowContextMenu(DWORD dwID, POINT* ppt, IUnknown* pcmdtReserved, IDispatch* pdispReserved)
{
// Pass back S_FALSE for the standard menu, S_OK for no menu
#ifdef _DEBUG //j
	return S_FALSE; 
#else
	return S_OK;
#endif _DEBUG
}

/////////////////////////////////////////////////////////////////////////////
void CWorkspaceDialog::OnOK()
{
	// This catches the Enter key and avoids default dialog processing
}

/////////////////////////////////////////////////////////////////////////////
void CWorkspaceDialog::OnCancel()
{
	// This catches the Escape key and avoids default dialog processing
}

/////////////////////////////////////////////////////////////////////////////
void CWorkspaceDialog::OnClose()
{
	// Is a modal dialog box is up (like the about box), don't destroy the window
	CWnd* pPopup = GetLastActivePopup();
	if (pPopup && (pPopup->m_hWnd != m_hWnd))
		return;

	CDHtmlDialog::OnClose();
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
void CWorkspaceDialog::Close()
{
	if (m_hWnd && ::IsWindow(m_hWnd))
		SendMessage(WM_CLOSE);
}

/////////////////////////////////////////////////////////////////////////////
void CWorkspaceDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout(this);
		dlgAbout.DoModal();
	}
	else
	{
		CDHtmlDialog::OnSysCommand(nID, lParam);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CWorkspaceDialog::OnDropFiles(HDROP hDrop)
{
	char szDroppedFile[MAX_PATH];
	int iFile = -1; // pass in -1 to get the number of files dropped, otherwise the file index
	int nFiles = DragQueryFile(hDrop, iFile, szDroppedFile, sizeof(szDroppedFile));
}
