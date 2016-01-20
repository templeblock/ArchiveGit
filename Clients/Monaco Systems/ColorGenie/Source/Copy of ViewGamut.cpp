// ViewGamut.cpp : implementation file
//

#include "stdafx.h"
#include "ColorGenie.h"
#include "ViewGamut.h"
#include "Qd3dExtras.h"
#include "Message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewGamut dialog

// Message Map
BEGIN_MESSAGE_MAP(CViewGamut, CDialog)
	//{{AFX_MSG_MAP(CViewGamut)
	ON_WM_INITMENU()
	ON_COMMAND(IDM_VIEWGAMUT_ACTIVE, OnActive)
	ON_COMMAND(IDM_VIEWGAMUT_BADGE, OnBadge)
	ON_COMMAND(IDM_VIEWGAMUT_CAMERABUTTON, OnCamerabutton)
	ON_COMMAND(IDM_VIEWGAMUT_CLEAR, OnClear)
	ON_COMMAND(IDM_VIEWGAMUT_CONTROLSTRIP, OnControlstrip)
	ON_COMMAND(IDM_VIEWGAMUT_COPY, OnCopy)
	ON_COMMAND(IDM_VIEWGAMUT_CUT, OnCut)
	ON_COMMAND(IDM_VIEWGAMUT_DOLLYBUTTON, OnDollybutton)
	ON_COMMAND(IDM_VIEWGAMUT_DRAGANDDROP, OnDragAndDrop)
	ON_COMMAND(IDM_VIEWGAMUT_DRAWFRAME, OnDrawframe)
	ON_COMMAND(IDM_VIEWGAMUT_EXIT, OnExit)
	ON_COMMAND(IDM_VIEWGAMUT_OPEN, OnOpen)
	ON_COMMAND(IDM_VIEWGAMUT_ORBITBUTTON, OnOrbitbutton)
	ON_COMMAND(IDM_VIEWGAMUT_PASTE, OnPaste)
	ON_COMMAND(IDM_VIEWGAMUT_RESETBUTTON, OnResetbutton)
	ON_COMMAND(IDM_VIEWGAMUT_SAVEAS, OnSaveas)
	ON_COMMAND(IDM_VIEWGAMUT_TEXTMODE, OnTextmode)
	ON_COMMAND(IDM_VIEWGAMUT_TRUCKBUTTON, OnTruckbutton)
	ON_COMMAND(IDM_VIEWGAMUT_UNDO, OnUndo)
	ON_COMMAND(IDM_VIEWGAMUT_ZOOMBUTTON, OnZoombutton)
	ON_WM_SETFOCUS()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Event Map
BEGIN_EVENTSINK_MAP(CViewGamut, CDialog)
    //{{AFX_EVENTSINK_MAP(CViewGamut)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
//CViewGamut::CViewGamut(CProfileDoc* pDoc, int iDataType, CWnd* pParent /*=NULL*/)
CViewGamut::CViewGamut(long data, int iDataType, const char* description, CWnd* pParent /*=NULL*/)
	: CDialog(CViewGamut::IDD, pParent)
{
	m_description = description;
	m_data = (McoHandle)data;
	//{{AFX_DATA_INIT(CViewGamut)
	m_pDoc = NULL;
	m_iDataType = iDataType; // 0=destination data, 1=source data
	m_pViewer = NULL;
	m_ppMyself = NULL;
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
CViewGamut::~CViewGamut()
{
	if (m_pViewer)
		Q3WinViewerDispose(m_pViewer);

	if (m_ppMyself)
		*m_ppMyself = NULL;
}


/////////////////////////////////////////////////////////////////////////////
void CViewGamut::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewGamut)
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CViewGamut message handlers


/////////////////////////////////////////////////////////////////////////////
BOOL CViewGamut::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Initialize stuff
	CenterWindow();
	ShowWindow(SW_NORMAL);
	UpdateWindow();

	if (!m_description.IsEmpty())
	{ // Update the window's title bar
		CString szTitle;
		GetWindowText(szTitle);
		SetWindowText(szTitle + " - " + m_description);
	}

	SetQd3dErrorHandler();
		
	RECT rect;
	GetClientRect(&rect);

	m_pViewer = Q3WinViewerNew(m_hWnd, &rect, (unsigned long)kQ3ViewerDefault);
	if (m_pViewer)
	{
		TQ3ColorARGB color = {1.0, 0.0, 0.0, 0.0};
		Q3WinViewerSetBackgroundColor(m_pViewer, &color);
		Q3WinViewerDraw(m_pViewer);

		BeginWaitCursor();
		//McoStatus status = m_Qd3dGamut.Init(pFileName, m_hWnd);
		McoStatus status = m_Qd3dGamut.InitFromDoc(m_data, m_hWnd);
		m_Qd3dGamut._deleteflag = TRUE;
		TQ3Status s = Q3WinViewerUseGroup(m_pViewer, m_Qd3dGamut.Document.fModel);
		Q3WinViewerDraw(m_pViewer);
		EndWaitCursor();

	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
BOOL CViewGamut::DoModeless(CWnd* pParent, CViewGamut** ppMyself)
{
	m_ppMyself = ppMyself;
	return Create(IDD, pParent);
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::PostNcDestroy() 
{
	delete this;
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnOK() 
{
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnCancel() 
{
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CViewGamut::OpenModelFile(void)
{
	if (!m_pViewer)
		return FALSE;

	char pathNameChars[255];
	char* pathName = &(pathNameChars[0]);
	if (BrowseForPathName(pathName, TRUE) != kQ3Success)
		return FALSE;

	HANDLE hFile = CreateFile(pathName, GENERIC_READ, FILE_SHARE_READ, 
							   NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile <= 0)
		Message("CreateFile failed.\nYikes!");
	else
	{
		TQ3Status aStatus = Q3WinViewerUseFile(m_pViewer, hFile);
		CloseHandle(hFile);
	}

	::InvalidateRect(m_hWnd, NULL, FALSE);	

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::SaveModelFile(void)
{
	if (!m_pViewer)
		return;

	char pathNameChars[255];
	char* pathName = &(pathNameChars[0]);
	if (BrowseForPathName(pathName, FALSE) != kQ3Success)
		return;
	
	HANDLE hFile = CreateFile(
		pathName,	// pointer to name of the file 
		GENERIC_WRITE,	// access (read-write) mode 
		0,	// share mode 
		NULL,	// pointer to security descriptor 
		CREATE_NEW,	// how to create 
		FILE_ATTRIBUTE_NORMAL,	// file attributes 
		NULL 	// handle to file with attributes to copy  
	);

	if (hFile <= 0)
	{
		Message("CreateFile failed during save.");
		return;
	}

	if (Q3WinViewerWriteFile(m_pViewer, hFile) == kQ3Failure)
	{
		Message("Q3WinViewerWriteFile failed during save.");
	}

	CloseHandle( hFile );
}


static char ExtFilter[] = "QuickDraw 3D Metafiles\0*.3dmf;*.3dm;*.q3d;*.qd3d\0All Files\0*.*\0\0";

/////////////////////////////////////////////////////////////////////////////
// put up common dialog; fOpen == TRUE for Open, FALSE for Save As.
BOOL CViewGamut::BrowseForPathName(char *inPathName, BOOLEAN fOpen) 
{
	inPathName[0] = 0;

	OPENFILENAME aFileName;
	aFileName.lStructSize = sizeof(OPENFILENAME);
	aFileName.hwndOwner = m_hWnd;
	aFileName.hInstance = AfxGetInstanceHandle();
	aFileName.lpstrFilter = ExtFilter;
    aFileName.lpstrCustomFilter = NULL; 
    aFileName.nMaxCustFilter = 0L; 
    aFileName.nFilterIndex = 0; 
    aFileName.lpstrFile = inPathName; 
    aFileName.nMaxFile = 255; 
    aFileName.lpstrFileTitle = NULL;	
    aFileName.nMaxFileTitle = 0; 
    aFileName.lpstrInitialDir = NULL;  
    aFileName.lpstrTitle = NULL;	
    aFileName.Flags = OFN_EXPLORER + OFN_LONGNAMES + OFN_PATHMUSTEXIST; 
	if( fOpen )
		aFileName.Flags += OFN_FILEMUSTEXIST;
    aFileName.nFileOffset = 0; 
    aFileName.nFileExtension = 0; 
    aFileName.lpstrDefExt = NULL; 
    aFileName.lCustData = 0; 
    aFileName.lpfnHook = NULL; 
    aFileName.lpTemplateName = NULL; 

	BOOL aStatus = TRUE;
	if (fOpen )
		if (GetOpenFileName((LPOPENFILENAME)&aFileName))
			aStatus = TRUE;
		else
			aStatus = FALSE;
	else //saving
		if (GetSaveFileName((LPOPENFILENAME)&aFileName))
			aStatus = TRUE;
		else
			aStatus = FALSE;

	return aStatus;
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnSetFocus(CWnd* pOldWnd) 
{
	CDialog::OnSetFocus(pOldWnd);
	
	if (m_pViewer)
		::SetFocus(Q3WinViewerGetWindow(m_pViewer));
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnSysColorChange() 
{
	CDialog::OnSysColorChange();
	
	if (m_pViewer)
		::SendMessage(Q3WinViewerGetWindow(m_pViewer), WM_SYSCOLORCHANGE, 0 , 0);
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	RECT rect;
	rect.top = 0;
	rect.left = 0;
	rect.right = cx;
	rect.bottom = cy;

	if (m_pViewer)
		Q3WinViewerSetBounds(m_pViewer, &rect);
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnInitMenu(CMenu* pMenu) 
{
	if (!pMenu)
		return;

	CDialog::OnInitMenu(pMenu);
	
	InitFlagMenu();

	if (!m_pViewer)
		return;

	BOOL bHasUndo = (Q3WinViewerGetState(m_pViewer) & kQ3ViewerHasUndo);
	if (bHasUndo)
	{
		unsigned long actualSize;
		if (Q3WinViewerGetUndoString(m_pViewer, NULL, 0, &actualSize) == kQ3False)
		{
			Message("Q3WinViewerGetUndoString failed in WM_INITMENU");
			return;
		}

		char* string;
		if (!(string = (char*)malloc(actualSize)))
		{
			Message("malloc failed in WM_INITMENU");
			return;
		}

		if (Q3WinViewerGetUndoString(m_pViewer, string, actualSize, &actualSize) == kQ3False)
		{
			Message("Q3WinViewerGetUndoString failed in WM_INITMENU");
			free(string);
			return;
		}

		if (!pMenu->ModifyMenu(IDM_VIEWGAMUT_UNDO, MF_BYCOMMAND | MF_STRING, IDM_VIEWGAMUT_UNDO, string))
		{
			Message("ModifyMenu failed in WM_INITMENU");
			free(string);
			return;
		}
		
		free(string);

		pMenu->EnableMenuItem(IDM_VIEWGAMUT_UNDO, MF_ENABLED);
	}
	else
	{
		if (!pMenu->ModifyMenu(IDM_VIEWGAMUT_UNDO, MF_BYCOMMAND | MF_STRING, IDM_VIEWGAMUT_UNDO, "Undo"))
		{
			Message("ModifyMenu failed in WM_INITMENU");
			return;
		}

		pMenu->EnableMenuItem(IDM_VIEWGAMUT_UNDO, MF_DISABLED | MF_GRAYED);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::DoFlagCommand(unsigned long flag)
{
	if (!m_pViewer)
		return;

	unsigned long flags = Q3WinViewerGetFlags(m_pViewer);

	// Toggle the flag
	TQ3Status status = Q3WinViewerSetFlags(m_pViewer, flags ^ flag);

	RECT aRect;
	::GetClientRect(m_hWnd, (LPRECT)&aRect);
	::InvalidateRect(m_hWnd, &aRect, FALSE);	
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::InitFlagMenu(void)
{
	if (!m_pViewer)
		return;

	unsigned long flags = Q3WinViewerGetFlags(m_pViewer);
	HMENU hMenu = ::GetMenu( m_hWnd );

	if (flags & kQ3ViewerControllerVisible) 
		CheckMenuItem( hMenu, IDM_VIEWGAMUT_CONTROLSTRIP, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, IDM_VIEWGAMUT_CONTROLSTRIP, MF_BYCOMMAND | MF_UNCHECKED );
	
	if (flags & kQ3ViewerButtonCamera) 
		CheckMenuItem( hMenu, IDM_VIEWGAMUT_CAMERABUTTON, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, IDM_VIEWGAMUT_CAMERABUTTON, MF_BYCOMMAND | MF_UNCHECKED );

	if (flags & kQ3ViewerButtonTruck) 
		CheckMenuItem( hMenu, IDM_VIEWGAMUT_TRUCKBUTTON, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, IDM_VIEWGAMUT_TRUCKBUTTON, MF_BYCOMMAND | MF_UNCHECKED );

	if (flags & kQ3ViewerButtonOrbit) 
		CheckMenuItem( hMenu, IDM_VIEWGAMUT_ORBITBUTTON, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, IDM_VIEWGAMUT_ORBITBUTTON, MF_BYCOMMAND | MF_UNCHECKED );

	if (flags & kQ3ViewerButtonDolly) 
		CheckMenuItem( hMenu, IDM_VIEWGAMUT_DOLLYBUTTON, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, IDM_VIEWGAMUT_DOLLYBUTTON, MF_BYCOMMAND | MF_UNCHECKED );

	if (flags & kQ3ViewerButtonZoom)
		CheckMenuItem( hMenu, IDM_VIEWGAMUT_ZOOMBUTTON, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, IDM_VIEWGAMUT_ZOOMBUTTON, MF_BYCOMMAND | MF_UNCHECKED );

	if (flags & kQ3ViewerButtonReset) 
		CheckMenuItem( hMenu, IDM_VIEWGAMUT_RESETBUTTON, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, IDM_VIEWGAMUT_RESETBUTTON, MF_BYCOMMAND | MF_UNCHECKED );
	
	if (flags & kQ3ViewerShowBadge) 
		CheckMenuItem( hMenu, IDM_VIEWGAMUT_BADGE, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, IDM_VIEWGAMUT_BADGE, MF_BYCOMMAND | MF_UNCHECKED );

	if (flags & kQ3ViewerOutputTextMode) 
		CheckMenuItem( hMenu, IDM_VIEWGAMUT_TEXTMODE, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, IDM_VIEWGAMUT_TEXTMODE, MF_BYCOMMAND | MF_UNCHECKED );

	if (flags & kQ3ViewerDraggingInOff) 
		CheckMenuItem( hMenu, IDM_VIEWGAMUT_DRAGANDDROP, MF_BYCOMMAND | MF_UNCHECKED );
	else
		CheckMenuItem( hMenu, IDM_VIEWGAMUT_DRAGANDDROP, MF_BYCOMMAND | MF_CHECKED );

	if (flags & kQ3ViewerActive) 
		CheckMenuItem( hMenu, IDM_VIEWGAMUT_ACTIVE, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, IDM_VIEWGAMUT_ACTIVE, MF_BYCOMMAND | MF_UNCHECKED );

	flags = GetWindowLong(Q3WinViewerGetWindow(m_pViewer), GWL_STYLE);
	if (flags & WS_BORDER) 
		CheckMenuItem( hMenu, IDM_VIEWGAMUT_DRAWFRAME, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, IDM_VIEWGAMUT_DRAWFRAME, MF_BYCOMMAND | MF_UNCHECKED );
	
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnActive() 
{
	DoFlagCommand(kQ3ViewerActive);
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnBadge() 
{
	DoFlagCommand(kQ3ViewerShowBadge);
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnCamerabutton() 
{
	DoFlagCommand(kQ3ViewerButtonCamera);
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnClear() 
{
	if (!m_pViewer)
		return;

	if (Q3WinViewerClear(m_pViewer) == kQ3Failure)
		Message("Q3WinViewerClear failed");
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnControlstrip() 
{
	DoFlagCommand(kQ3ViewerControllerVisible);
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnCopy() 
{
	if (!m_pViewer)
		return;

	if (Q3WinViewerCopy(m_pViewer) == kQ3Failure)
		Message("Q3WinViewerCopy failed");
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnCut() 
{
	if (!m_pViewer)
		return;

	if (Q3WinViewerCut(m_pViewer) == kQ3Failure)
		Message("Q3WinViewerCut failed");
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnDollybutton() 
{
	DoFlagCommand(kQ3ViewerButtonDolly);
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnDragAndDrop() 
{
	DoFlagCommand(kQ3ViewerDraggingInOff);
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnDrawframe() 
{
	if (!m_pViewer)
		return;

	unsigned long flags = GetWindowLong(Q3WinViewerGetWindow(m_pViewer), GWL_STYLE);
	
	SetLastError(0);
	flags = SetWindowLong(Q3WinViewerGetWindow(m_pViewer), GWL_STYLE, flags ^ WS_BORDER);

	if (flags == 0 && (GetLastError() != 0))
		Message("Setting the WS_BORDER flags failed");
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnExit() 
{
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnOpen() 
{
	OpenModelFile();
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnOrbitbutton() 
{
	DoFlagCommand(kQ3ViewerButtonOrbit);
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnPaste() 
{
	if (!m_pViewer)
		return;

	if (Q3WinViewerPaste(m_pViewer) == kQ3Failure)
		Message("Q3WinViewerPaste failed");
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnResetbutton() 
{
	DoFlagCommand(kQ3ViewerButtonReset);
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnSaveas() 
{
	SaveModelFile();
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnTextmode() 
{
	DoFlagCommand(kQ3ViewerOutputTextMode);
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnTruckbutton() 
{
	DoFlagCommand(kQ3ViewerButtonTruck);
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnUndo() 
{
	if (!m_pViewer)
		return;

	if (Q3WinViewerUndo(m_pViewer) == kQ3Failure)
		Message("Q3WinViewerUndo failed");
	else
		Q3WinViewerDrawContent(m_pViewer);
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut::OnZoombutton() 
{
	DoFlagCommand(kQ3ViewerButtonZoom);
}
