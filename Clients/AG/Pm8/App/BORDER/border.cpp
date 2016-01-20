// border.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "border.h"

#include "MainFrm.h"
#include "Borddoc.h"
#include "Bordview.h"
#include "ifaced.h"
#include "beobjd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

const DEFAULT_MAIN_SZ = 400;
const FUDGE_W = 100;
const FUDGE_H = 50;

/////////////////////////////////////////////////////////////////////////////
// CBorderApp

BEGIN_MESSAGE_MAP(CBorderApp, CWinApp)
	//{{AFX_MSG_MAP(CBorderApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBorderApp construction

CBorderApp::CBorderApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CBorderApp object

CBorderApp theBorderApp;

/////////////////////////////////////////////////////////////////////////////
// CBorderApp initialization

BOOL CBorderApp::InitInstance() 
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	CWinApp::InitApplication();	// Init doc manager

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Mindscape"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CBorddoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CBordview));
	AddDocTemplate(pDocTemplate);
	
	return CWinApp::InitInstance();
}

int CBorderApp::InitApp(CBEObjectD* pBEObject, HWND hParent)
// Set up the document, and windows
{
	// Create a new document
	m_hParent = hParent;
	m_nCmdShow = SW_HIDE;	// Initally invisible
	OnFileNew();

	// Store the BE object & callback interface pointers in the document
	(GetDocument())->SetCallbackInterface(m_pBECallbackInterface);
	(GetDocument())->SetCurrBEObj(pBEObject);

	// Disable the parent window
	::EnableWindow(m_hParent, FALSE);

	// Resize & show window
	ResizeBEWindow(pBEObject->GetXSize(), pBEObject->GetYSize());
	m_pMainWnd->CenterWindow();

	// Set title
	CString szTitle;
	szTitle.LoadString(IDS_BORDER_EDITOR);
	m_pMainWnd->SetWindowText(szTitle);

	// If the border object is empty, select all the regions for the user.
	if (pBEObject != NULL)
	{
		if (pBEObject->IsEmpty())
		{
			pBEObject->SelectAllRegions(TRUE);
		}
	}

	return (0);
}

int CBorderApp::ExitApp()
// Enable parent window & end messaging
{	
	// Enable the parent window
	::EnableWindow(m_hParent, TRUE);

	// Bring parent to the top
	CWnd::FromHandle(m_hParent)->SetForegroundWindow();

	// Terminate the modal loop
	m_pMainWnd->EndModalLoop(0);

	return (0);
}

int CBorderApp::ExitInstance() 
{
	// Release the interface
	m_pBECallbackInterface->Release();

	// Clean up
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// CBorderApp methods

CBorddoc* CBorderApp::GetDocument()
// Retrieve the current document
{
	POSITION pos;
	CDocTemplate *pDocTemplate;

	// Find the current document and send the interface to it
	pos = GetFirstDocTemplatePosition();
	pDocTemplate = GetNextDocTemplate(pos);
	pos = pDocTemplate->GetFirstDocPosition();

	return ((CBorddoc*)pDocTemplate->GetNextDoc(pos));
}

void CBorderApp::ShowWindow(int nCmdShow)
{
	m_pMainWnd->ShowWindow(nCmdShow);
	m_pMainWnd->UpdateWindow();
}

void CBorderApp::SetCallbacks(CBECallbackInterface* pBECallbackInterface)
{
	m_pBECallbackInterface = pBECallbackInterface;
}

void CBorderApp::ResizeBEWindow(double dXSize, double dYSize)
// Set the new size of the window to have the same aspect
// ratio as the border object
{
	// If the window hasn't been created yet, save sizing for later
	if (m_pMainWnd != NULL)
	{
		// Get the main window bounds
		CRect rctMainBounds;
		m_pMainWnd->GetWindowRect(&rctMainBounds);

		// Get the view's client rectangle bounds
		CRect rctClientBounds;
		CBorddoc *pDoc = GetDocument();
		POSITION pos = pDoc->GetFirstViewPosition();
		CView* pView = pDoc->GetNextView(pos);
		pView->GetClientRect(&rctClientBounds);
		
		// Compute the difference in size between the two retangles
		CSize szBoundsDiff = rctMainBounds.Size() - rctClientBounds.Size();

		// Determine the desired default dimensions
		int nDefaultWidth;
		int nDefaultHeight;

		if (dXSize < dYSize)
		{
			nDefaultWidth = DEFAULT_MAIN_SZ;
			nDefaultHeight = (int)(nDefaultWidth * dYSize / dXSize);
		}
		else
		{
			nDefaultHeight = DEFAULT_MAIN_SZ;
			nDefaultWidth = (int)(nDefaultHeight * dXSize / dYSize);
		}

		// Change the window's dimensions, putting back the difference
		m_pMainWnd->MoveWindow(
			rctMainBounds.TopLeft().x,
			rctMainBounds.TopLeft().y,
			nDefaultWidth + szBoundsDiff.cx + FUDGE_W,
			nDefaultHeight + szBoundsDiff.cy,
			FALSE);

	}
}

int CBorderApp::CreateBEInterface(CBEInterface*& pBEInterface)
{
	// Create the object
	pBEInterface = new CBEInterfaceD;
	return (pBEInterface == NULL);		// return 0 on success
}

HPALETTE CBorderApp::GetPalette(void)
{
	HPALETTE hResult = NULL;
	CBECallbackInterface* pCallbackInterface = GetCallbacks();
	if (pCallbackInterface != NULL)
	{
		hResult = pCallbackInterface->GetPalette();
	}
	return hResult;
}

int CBorderApp::DoModal()
// Pump messages to the window in a modal fashion
{
	((CMainFrame*)m_pMainWnd)->RunModalLoop(MLF_SHOWONIDLE);
	
	// Destroy window
	CloseAllDocuments(TRUE);

	return (0);
}

/////////////////////////////////////////////////////////////////////////////
// exported functions
extern "C"
{
__declspec(dllexport)
int Initialize(CBECallbackInterface* pBECallbackInterface, CBEInterface*& pBEInterface)
{
	// MUST have in all exports
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Store the interface objects
	theBorderApp.SetCallbacks(pBECallbackInterface);

	// Create the border Editor interface object
	if (theBorderApp.CreateBEInterface(pBEInterface))
	{
		return (-1);	// error
	}
		
	// Create
	return (0);
}
}


