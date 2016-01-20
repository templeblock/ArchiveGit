// Color Meter Plus.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Color Meter Plus.h"
#include "DialogMain.h"
#include "MainFrm.h"
#include "Color Meter PlusDoc.h"
#include "Color Meter PlusView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SetUpProfile gSetUpProfile;
ReferenceFile MonitorData;


/////////////////////////////////////////////////////////////////////////////
// CColorMeterPlusApp

BEGIN_MESSAGE_MAP(CColorMeterPlusApp, CWinApp)
	//{{AFX_MSG_MAP(CColorMeterPlusApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorMeterPlusApp construction

CColorMeterPlusApp::CColorMeterPlusApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CColorMeterPlusApp object

CColorMeterPlusApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CColorMeterPlusApp initialization

BOOL CColorMeterPlusApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	// Open the the SetUpProfile or default to one if there is no file

		FILE *fin,*fout;

	char FileName[] = "CMPsetup.prf";
	char SysDir[256];
	
	GetSystemDirectory( SysDir,256);
  	strcat (SysDir,"\\Sequel\\");
	strcat (SysDir,FileName);
	fin = fopen(SysDir,"r+b");
	if(fin == NULL)
	{
		gSetUpProfile.whichDisplay = MXYL;
		gSetUpProfile.whichUnits = MENGLISH;
		gSetUpProfile.whichSpeed = MMED;
		strcpy(gSetUpProfile.whichFileName,"");

	}
	else
	{
		fread(&gSetUpProfile,sizeof(gSetUpProfile),1,fin);
		fclose(fin);
		if(gSetUpProfile.whichDisplay == MRGB)
		{
			GetSystemDirectory( SysDir,256);
  			strcat (SysDir,"\\Sequel\\");
			strcat (SysDir,gSetUpProfile.whichFileName);
			fin = fopen(SysDir,"r+b");
			fread(&MonitorData,sizeof(ReferenceFile),1,fin);
			fclose(fin);
		}
	}

	CDialogMain dlg;
	
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		GetSystemDirectory( SysDir,256);
  	strcat (SysDir,"\\Sequel\\");
	strcat (SysDir,FileName);
	fout = fopen(SysDir,"w+b");
	fwrite(&gSetUpProfile,sizeof(gSetUpProfile),1,fout);

		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;

	


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

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
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
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CColorMeterPlusApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CColorMeterPlusApp commands
