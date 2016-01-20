// palmerge.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "palmerge.h"
#include "palmedlg.h"
#include "dib.h"
#include "pal.h"
#include "palio.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPalmergeApp

BEGIN_MESSAGE_MAP(CPalmergeApp, CWinApp)
	//{{AFX_MSG_MAP(CPalmergeApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPalmergeApp construction

CPalmergeApp::CPalmergeApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPalmergeApp object

CPalmergeApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPalmergeApp initialization

//************************************************************************
LPCSTR FileName( LPCSTR lpPath )
//************************************************************************
{
	LPCSTR lp;

	lp = lpPath;
	lp += lstrlen( lpPath );
	while( *lp != '\\' && *lp != ':' )
	{
		if ( (--lp) < lpPath )
		    return( lpPath + lstrlen(lpPath) ); // null string
	}
	lp++;
	return( lp ); // the file name
}

//************************************************************************
LPCSTR Extension( LPCSTR lpPath )
//************************************************************************
{
	LPCSTR lp;

	lp = lpPath;
	lp += lstrlen( lpPath );
	while( *lp != '.' )
	{
		if ( (--lp) < lpPath )
		    return( lpPath + lstrlen(lpPath) ); // null string
	}
	return( lp ); // the extension, including the '.'
}

//************************************************************************
LPSTR StripFile( LPSTR lpPath )
//************************************************************************
{
	LPSTR lp;

	if ( lp = (LPSTR)FileName( lpPath ) )
		*lp = '\0';
	return( lpPath );
}

BOOL CPalmergeApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	RGBQUAD StaticMap[256];
	char szPath[MAX_PATH];
	int nStaticEntries;
	char szString[80];
	int i;

	Enable3dControls();
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	static char BASED_CODE szFilter[] = "Windows Bitmap Files|*.bmp||";

	ZeroMemory(StaticMap, sizeof(StaticMap));
	GetModuleFileName(m_hInstance, szPath, sizeof(szPath));
	StripFile(szPath);
	lstrcat(szPath, "static.pal");

	nStaticEntries = LoadPalette(szPath, StaticMap);
	if (!nStaticEntries)
	{
		wsprintf(szString, "Error opening file '%s'", szPath);
		m_pMainWnd->MessageBox(szString, "Fatal Error", MB_OK);
		return(FALSE);
	}

	while (TRUE)
	{
		char szFileName[MAX_PATH];

		CFileDialog open(TRUE,
				"bmp",
				"*.bmp",
				OFN_HIDEREADONLY,
				szFilter,
				NULL);

		//fgdlg.m_ofn.Flags &= ~OFN_EXPLORER;
		open.m_ofn.lpstrTitle = "Select Bitmap File";

		int nResponse = open.DoModal();
		if (nResponse != IDOK)
		{
			DWORD dwErr = CommDlgExtendedError();
			break;

		}
		CString szOpenFile = open.GetPathName();
		lstrcpy(szFileName, szOpenFile);

		PDIB pDib = CDib::LoadDibFromFile(szFileName, NULL, FALSE, TRUE);
		if (!pDib)
		{

			wsprintf(szString, "Error opening file '%s'", szFileName);
			nResponse = m_pMainWnd->MessageBox(szString, "Error", MB_OKCANCEL);
			if (nResponse != IDOK)
				break;
		}

		CFileDialog save(FALSE,
				"bmp",
				"*.bmp",
				OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
				szFilter,
				NULL);


		//dstdlg.m_ofn.Flags &= ~OFN_EXPLORER;
		save.m_ofn.lpstrTitle = "Specify Output File Name";

		nResponse = save.DoModal();
		if (nResponse != IDOK)
		{
			DWORD dwErr = CommDlgExtendedError();
			continue;
		}
		CString szSaveFile = save.GetPathName();
		lstrcpy(szFileName, szSaveFile);

		LPRGBQUAD lpMap = pDib->GetColors();
		// copy static entries into dib colormap
		for (i = 0; i < nStaticEntries; ++i)
			lpMap[i+10] = StaticMap[i];
		CreateIdentityMap(lpMap, 256);
		pDib->SetColorsUsed(256);
		if (!pDib->WriteFile(szFileName))
		{
			wsprintf(szString, "Error writing file '%s'", szFileName);
			nResponse = m_pMainWnd->MessageBox(szString, "Error", MB_OKCANCEL);
			if (nResponse != IDOK)
			{
				delete pDib;
				break;
			}
		}
		delete pDib;
	}
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
