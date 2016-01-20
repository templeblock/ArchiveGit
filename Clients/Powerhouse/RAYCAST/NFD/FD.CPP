/*********************** ( First 3D Development ) ***********************/
/*	(c) 1994/5  John Lundy															     						*/
/************************************************************************/
// fd.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "fd.h"

#include "mainfrm.h"
#include "fddoc.h"
#include "fdview.h"
#include "fdstatus.h"
#include "fdmap.h"
#include "ack\ack3d.h"
#include "ack\ackeng.h"
#include "ack\ackext.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern int				ViewLowRes, ViewUpDown;
extern CFdView		*OurView;
extern ACKENG			*ae;
extern CFdStatus	*fds;
extern CFdMap			*fdm;

/////////////////////////////////////////////////////////////////////////////
// CFdApp

BEGIN_MESSAGE_MAP(CFdApp, CWinApp)
	//{{AFX_MSG_MAP(CFdApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
	// Standard file based document commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFdApp construction

CFdApp::CFdApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_pszProfileName = "WinAdv.INI";
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFdApp object

CFdApp NEAR theApp;

/////////////////////////////////////////////////////////////////////////////
// CFdApp initialization

BOOL CFdApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	//	Load standard INI file options (including MRU)
	LoadStdProfileSettings();

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	AddDocTemplate(new CSingleDocTemplate(IDR_MAINFRAME,
			RUNTIME_CLASS(CFdDoc),
			RUNTIME_CLASS(CMainFrame),     // main SDI frame window
			RUNTIME_CLASS(CFdView)));

	RegisterShellFileTypes();	//	Register file types with File Manager
	EnableShellOpen();			//	Allow File Manager to run us on double click of our file type

	// create a new (empty) document
	OnFileNew();

	//	set dialog background color
	SetDialogBkColor(RGB(192,192,192),RGB(255,255,255));

	if (m_lpCmdLine[0] != '\0')
	{
		// TODO: add command line processing here
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_RBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CFdApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CFdApp commands

BOOL CFdApp::OnIdle(long iCount)
{
	OurView->PostMessage(WM_USER_IDLE, 0, iCount);
	return CWinApp::OnIdle(iCount);
}


void CFdApp::OnFileNew()
{
	// TODO: Add your command handler code here
	CWinApp::OnFileNew();
//CDocTemplate	*pTemplate = (CDocTemplate*)m_templateList.GetHead();
//CDocument		*pDocument = pTemplate->OpenDocumentFile(NULL);
//POSITION		pos = pDocument->GetFirstViewPosition();
//	OurView = (CFdView *)pDocument->GetNextView(pos);
}

void CFdApp::OnFileOpen()
{
	// TODO: Add your command handler code here
	CWinApp::OnFileOpen();
}

int CFdApp::Run()
{
	TRACE("Entering Run()...\n");
#ifdef	_PROFILER
	#pragma message("...Profiler code is included...")
	ProfSetup(1024,0);
	ProfStart();
#endif
	// Acquire and dispatch messages until a WM_QUIT message is received.
	for (; ;)
	{
		LONG lIdleCount = 0;
		while (!::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE) &&
			OnIdle(lIdleCount++))
		{
			// more work to do
		}

		// either we have a message, or OnIdle returned false

		if (!PumpMessage())
			break;
	}
#ifdef	_PROFILER
	#pragma message("...Profiler code is included...")
	ProfFinish();
#endif
	return ExitInstance();

}

BOOL PASCAL _AfxFullPath(LPSTR lpszPathOut, LPCSTR lpszFileIn);

CDocument* CFdApp::OpenDocumentFile(LPCSTR lpszFileName)
{
	// find the highest confidence
	POSITION pos = m_templateList.GetHeadPosition();
	CDocTemplate::Confidence bestMatch = CDocTemplate::noAttempt;
	CDocTemplate* pBestTemplate = NULL;
	CDocument* pOpenDocument = NULL;

	char szPath[_MAX_PATH];
	_AfxFullPath(szPath, lpszFileName);

	while (pos)
	{
		CDocTemplate* pTemplate = (CDocTemplate*)m_templateList.GetNext(pos);
		ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CDocTemplate)));

		CDocTemplate::Confidence match;
		ASSERT(pOpenDocument == NULL);
		match = pTemplate->MatchDocType(szPath, pOpenDocument);
		if (match > bestMatch)
		{
			bestMatch = match;
			pBestTemplate = pTemplate;
		}
		if (match == CDocTemplate::yesAlreadyOpen) break;	// stop here
	}

	if (pBestTemplate == NULL)
{
		TRACE0("Error: can't open document\n");
		return FALSE;
	}

	return pBestTemplate->OpenDocumentFile(szPath);
}

void CAboutDlg::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
int		i,tmp,chk;
	if (IDOK == AfxMessageBox("THIS IS CHEATING!\n\nAre you sure?", MB_OKCANCEL|MB_ICONSTOP) == IDOK)
	{
		ViewUpDown = 32 << (8 - ViewLowRes);
		ae->ObjList[PLAYER_OBJECT].Health = 16777215;	//	Max out health (almost GOD mode)
		ae->ObjList[PLAYER_OBJECT].Dead = 0;
		ae->Keys = KEY_RED|KEY_GREEN|KEY_BLUE;				//	All keys
		for (i=0 ; i < MaxWeaponsInUse ; i++)					//	All weapons with max ammo
		{
			ae->WeaponsCarried[i].Possessed = 1;
			*ae->WeaponsCarried[i].Ammo = Weapons[i].MaxAmmo;
		}
		tmp = ~DOOR_LOCKED_MASK;
		for (i=0 ; i < GRID_ARRAY ; i++)		//	Unlock all doors
		{
			chk = ae->xGrid[i] & 0xFF;
			if (chk)
			{
				if (chk >= DOOR_XCODE)
				{
					if (ae->xGrid[i] & tmp) ae->xGrid[i] &= tmp;
				}
				ae->xGrid[i] |= WALL_WAS_SEEN;
			}
			chk = ae->yGrid[i] & 0xFF;
			if (chk)
			{
				if (chk >= DOOR_YCODE)
				{
					if (ae->yGrid[i] & tmp) ae->yGrid[i] &= tmp;
				}
				ae->yGrid[i] |= WALL_WAS_SEEN;
			}
		}
		for (i=0 ; i < ae->MaxObjects ; i++)
		{
			ae->ObjList[i].Flags |= OF_WAS_SEEN;
		}
		if (fds)	//	Update status window
		{
			fds->TextHealth();
			fds->TextAmmo();
			fds->TextMsg("C H E A T I N G");
			fds->DrawKeys();
		}
		if (fdm) fdm->InvalidateRect(NULL,FALSE);
	}
	CDialog::OnRButtonDblClk(nFlags, point);
}
