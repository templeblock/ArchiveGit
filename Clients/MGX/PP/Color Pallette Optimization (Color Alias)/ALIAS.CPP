// alias.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "alias.h"
#include "aliasdlg.h"
#include "tgardr.h"
#include "tgawrt.h"
#include "palio.h"
#include "pal.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

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
LPSTR FixPath( LPSTR lpPath )
//************************************************************************
{
int length;

if ( !(length = lstrlen(lpPath)) )
	return( lpPath );
while ( length && (lpPath[length-1] == '\n' || lpPath[length-1] == '\r') )
	lpPath[--length] = '\0';
if ( !length )
	return( lpPath );
if ( lpPath[length-1] != '\\' )
	lstrcat( lpPath, "\\" );
return( lpPath );
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

int ExtractNumber(LPCSTR lpString)
{
	char szNum[25];
	int i = 0;
	int num;

	lstrcpy(szNum, lpString);
	if (szNum[i] && !isdigit(szNum[i]))
		++i;
	if (szNum[i])
	{
		if (sscanf(&szNum[i], "%d", &num) == 1)
			return(num);
	}
	return(-1);
}

LPSTR ExtractPrefix(LPCSTR lpString, LPSTR lpPrefix)
{
	char szNum[25];
	int i = 0;

	lstrcpy(szNum, lpString);
	if (szNum[i] && !isdigit(szNum[i]))
	{
		lpPrefix[i] = szNum[i];
		++i;
	}
	lpPrefix[i] = '\0';
	return(lpPrefix);
}

/////////////////////////////////////////////////////////////////////////////
// CAliasApp

BEGIN_MESSAGE_MAP(CAliasApp, CWinApp)
	//{{AFX_MSG_MAP(CAliasApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAliasApp construction

CAliasApp::CAliasApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAliasApp object

CAliasApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAliasApp initialization

#ifdef GARBAGE
class CMyFileDialog : public CFileDialog
{
public:
	CMyFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL) :
	CFileDialog(bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		lpszDefExt,
		lpszFileName,
		dwFlags,
		lpszFilter,
		pParentWnd) {}
	~CMyFileDialog() {}

	BOOL OnInitDialog();
	void OnLBSelChangedNotify(UINT nIDBox, UINT iCurSel, UINT nCode);
};


BOOL CMyFileDialog::OnInitDialog()
{
	return(CFileDialog::OnInitDialog());
}

void CMyFileDialog::OnLBSelChangedNotify(UINT nIDBox, UINT iCurSel, UINT nCode)
{
	HWND hDlg = GetSafeHwnd();
	switch (nCode)
	{
		case CD_LBSELCHANGE:
		char buf[256];
		char buf2[256];

		((CListBox *)GetDlgItem(nIDBox))->GetText(iCurSel, buf2);

		//ListBox_GetText(::GetDlgItem(hDlg, nIDBox), iCurSel, buf2);
		wsprintf(buf, "nIDBox = %d iCurSel = %d string = %s\n", nIDBox, iCurSel, buf2);
		OutputDebugString(buf);
		break;
		
		case CD_LBSELSUB:
		break;
		
		case CD_LBSELADD:
		break;
		
		case CD_LBSELNOITEMS:
		break;
		
		default:
		break; 
	}
}
#endif

BOOL CAliasApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	BOOL bError = FALSE;

	if (!m_Quantize.Init())
		return(FALSE);

	Enable3dControls();
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	static char BASED_CODE szFilter[] = "Targa Files|*.tga||";

	while (!bError)
	{
		char szSceneDir[MAX_PATH];
		char szGrayDir[MAX_PATH];
		char szColorDir[MAX_PATH];
		char szCompDir[MAX_PATH];
		char szWildCard[MAX_PATH];
		char szMsg[MAX_PATH];
		WIN32_FIND_DATA FindData;

		CFileDialog bgdlg(TRUE,
				"tga",
				"*.tga",
				OFN_HIDEREADONLY,
				szFilter,
				NULL);

		//fgdlg.m_ofn.Flags &= ~OFN_EXPLORER;
		bgdlg.m_ofn.lpstrTitle = "Select Scene Background File";

		int nResponse = bgdlg.DoModal();
		if (nResponse != IDOK)
		{
			DWORD dwErr = CommDlgExtendedError();
			break;

		}
		m_statusDlg.Create(IDD_STATUS);

		CString szBackFile = bgdlg.GetPathName();
		lstrcpy(szSceneDir, szBackFile);
		StripFile(szSceneDir);

		lstrcpy(szGrayDir, szSceneDir);
		lstrcat(szGrayDir, "GRAY\\");
		lstrcpy(szColorDir, szSceneDir);
		lstrcat(szColorDir, "COLOR\\");
		lstrcpy(szCompDir, szSceneDir);
		lstrcat(szCompDir, "COMP");
#ifndef CPO
		CreateDirectory(szCompDir, NULL);
#endif
		lstrcat(szCompDir, "\\");

		lstrcpy(szWildCard, szColorDir);
		lstrcat(szWildCard, "*.*");

		m_Quantize.Reset();

		// first count the number of files to be processed
		m_nTotal = m_nFiles = 0;
		HANDLE hFind = FindFirstFile(szWildCard, &FindData);
		if (!hFind)
		{
			wsprintf(szMsg, "No subdirectories found in %s", szGrayDir);
			MessageBox(NULL, szMsg, "Error", MB_ICONSTOP|MB_OK);
			continue;
		}
		while (hFind != INVALID_HANDLE_VALUE)
		{
			if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
				FindData.cFileName[0] != '.')
			{
				char szGrayAnimDir[MAX_PATH];
				char szColorAnimDir[MAX_PATH];
				char szCompAnimDir[MAX_PATH];
				lstrcpy(szGrayAnimDir, szGrayDir);
				lstrcat(szGrayAnimDir, FindData.cFileName);
				FixPath(szGrayAnimDir);
				lstrcpy(szColorAnimDir, szColorDir);
				lstrcat(szColorAnimDir, FindData.cFileName);
				FixPath(szColorAnimDir);
				lstrcpy(szCompAnimDir, szCompDir);
				lstrcat(szCompAnimDir, FindData.cFileName);
#ifndef CPO
				CreateDirectory(szCompAnimDir, NULL);
#endif
				FixPath(szCompAnimDir);
				if (!ProcessFiles(szGrayAnimDir, szColorAnimDir, szCompAnimDir, TRUE))
				{
					FindClose(hFind);
					bError = TRUE;
					break;
				}
			}
			if (!FindNextFile(hFind, &FindData))
			{
				FindClose(hFind);
				break;
			}
		}
		// now actually process all the files
		hFind = FindFirstFile(szWildCard, &FindData);
		if (!hFind)
		{
			wsprintf(szMsg, "No subdirectories found in %s", szGrayDir);
			MessageBox(NULL, szMsg, "Error", MB_ICONSTOP|MB_OK);
			continue;
		}
		while (hFind != INVALID_HANDLE_VALUE)
		{
			if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
				FindData.cFileName[0] != '.')
			{
				char szGrayAnimDir[MAX_PATH];
				char szColorAnimDir[MAX_PATH];
				char szCompAnimDir[MAX_PATH];
				lstrcpy(szGrayAnimDir, szGrayDir);
				lstrcat(szGrayAnimDir, FindData.cFileName);
				FixPath(szGrayAnimDir);
				lstrcpy(szColorAnimDir, szColorDir);
				lstrcat(szColorAnimDir, FindData.cFileName);
				FixPath(szColorAnimDir);
				lstrcpy(szCompAnimDir, szCompDir);
				lstrcat(szCompAnimDir, FindData.cFileName);
#ifndef CPO
				CreateDirectory(szCompAnimDir, NULL);
#endif
				FixPath(szCompAnimDir);
				if (!ProcessFiles(szGrayAnimDir, szColorAnimDir, szCompAnimDir))
				{
					FindClose(hFind);
					bError = TRUE;
					break;
				}
			}
			if (!FindNextFile(hFind, &FindData))
			{
				FindClose(hFind);
				break;
			}
		}

		FNAME szPath, szFileName;
		RGBQUAD StaticMap[256];
		RGBQUAD InitialMap[256];
		RGBQUAD PaletteMap[256];

		ZeroMemory(StaticMap, sizeof(StaticMap));
		ZeroMemory(PaletteMap, sizeof(PaletteMap));
		GetModuleFileName(m_hInstance, szPath, sizeof(szPath));
		StripFile(szPath);

		lstrcpy(szFileName, szPath);
		lstrcat(szFileName, "static.pal");
		int nStaticEntries = LoadPalette(szFileName, &StaticMap[10]);

		lstrcpy(szFileName, szPath);
		lstrcat(szFileName, "initial.pal");
		int nInitialEntries = LoadPalette(szFileName, InitialMap);
		int nEntries = m_Quantize.CreatePalette(PaletteMap, 236-nStaticEntries, InitialMap, nInitialEntries);
		if (nEntries)
		{
			char szPalFile[MAX_PATH];

			#ifdef GARBAGE
		 	lstrcpy(szPalFile, szSceneDir);
		 	lstrcat(szPalFile, "backgrnd.pal");
			SavePalette(szPalFile, PaletteMap, 256);
			PaletteMap[nEntries].rgbRed = m_rgbTrans.red;
			PaletteMap[nEntries].rgbGreen = m_rgbTrans.green;
			PaletteMap[nEntries].rgbBlue = m_rgbTrans.blue;
		 	lstrcpy(szPalFile, szSceneDir);
		 	lstrcat(szPalFile, "foregrnd.pal");
			SavePalette(szPalFile, PaletteMap, 256);
			#endif

			for (int i = 0; i < nEntries; ++i)
				StaticMap[i+nStaticEntries+10] = PaletteMap[i];
			CreateIdentityMap(StaticMap, 256);

		 	lstrcpy(szPalFile, szSceneDir);
		 	lstrcat(szPalFile, "final.pal");
			SavePalette(szPalFile, StaticMap, 256);
			nStaticEntries += 10;
			for (i = 0; i < nStaticEntries; ++i)
			{
#ifndef CPO
				if (StaticMap[i].rgbRed != m_rgbTrans.red ||
					StaticMap[i].rgbGreen != m_rgbTrans.green ||
					StaticMap[i].rgbBlue != m_rgbTrans.blue)
#endif
				{
					StaticMap[i].rgbRed =
					StaticMap[i].rgbGreen =
					StaticMap[i].rgbBlue = 0;
				}
			}
			for (i = 246; i < 256; ++i)
			{
#ifndef CPO
				if (StaticMap[i].rgbRed != m_rgbTrans.red ||
					StaticMap[i].rgbGreen != m_rgbTrans.green ||
					StaticMap[i].rgbBlue != m_rgbTrans.blue)
#endif
				{
					StaticMap[i].rgbRed =
					StaticMap[i].rgbGreen =
					StaticMap[i].rgbBlue = 0;
				}
			}
		 	lstrcpy(szPalFile, szSceneDir);
		 	lstrcat(szPalFile, "premiere.pal");
			SavePalette(szPalFile, StaticMap, 256);
		}
		m_statusDlg.DestroyWindow();
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

BOOL CAliasApp::AntiAliasDib(PDIB pForeDib, PDIB pCompDib)
{
#ifndef CPO
	if (pForeDib->GetHeight() != pCompDib->GetHeight() ||
		pForeDib->GetWidth() != pCompDib->GetWidth())
	{
		MessageBox(NULL, "Bitmaps are different sizes", "Error", MB_ICONSTOP|MB_OK);
		return(FALSE);
	}
	if (pForeDib->GetBitCount() != 24 ||
		pCompDib->GetBitCount() != 24)
	{
		MessageBox(NULL, "Bitmaps must both be 24-bits", "Error", MB_ICONSTOP|MB_OK);
		return(FALSE);
	}

	RGBS rgb;
	BOOL fColor;

	LPTR lpFore = pForeDib->GetPtr();
	if (FALSE)
		rgb.red = rgb.green = rgb.blue = 148;
	else
		rgb = *((LPRGB)pForeDib->GetXY(0, 0));
	BYTE r = rgb.red;
	BYTE g = rgb.green;
	BYTE b = rgb.blue;
	fColor = ((r == 255 && g == 0 && b == 0) ||	// red
		(r == 0 && g == 255 && b == 0) ||	// green
		(r == 0 && g == 0 && b == 255) ||	// blue
		(r == 0 && g == 255 && b == 255) ||	// cyan
		(r == 255 && g == 0 && b == 255) ||	// magenta
		(r == 255 && g == 255 && b == 0));	// yellow
	
	if (!fColor)
	{
		m_rgbTrans.red = 255;
		m_rgbTrans.green = 0;
		m_rgbTrans.blue = 255;
	}
	else
	{
		m_rgbTrans = rgb;
	}
#endif
	LPTR lpComp = pCompDib->GetPtr();
	UINT uWidthBytes = pCompDib->GetWidthBytes();
	int iHeight = abs(pCompDib->GetHeight());
	while (--iHeight >= 0)
	{
		int iWidth = pCompDib->GetWidth();
		LPRGB lpC = (LPRGB)lpComp;
		m_Quantize.CountFrequencies((LPRGBTRIPLE)lpComp, iWidth);

		#ifndef CPO
		LPRGB lpF = (LPRGB)lpFore;
		while (--iWidth >= 0)
		{
			if (lpF->red != r || lpF->green != g || lpF->blue != b)
			{
				*lpF++ = *lpC++;
			}
			else
			{
				if (!fColor)
				{
					lpF->red = 255;
					lpF->green = 0;
					lpF->blue = 255;
				}
				++lpF;
				++lpC;
			}
		}
		lpFore += uWidthBytes;
		#endif
		lpComp += uWidthBytes;
	}

	return(TRUE);
}

CMyStatusDlg::CMyStatusDlg()
{
	m_fAbort = FALSE;
}

void CMyStatusDlg::OnCancel()
{
	if (MessageBox("Are you sure you want to cancel?", "Cancel Confirmation", MB_ICONSTOP|MB_YESNO) == IDYES)
		m_fAbort = TRUE;
}

BOOL CMyStatusDlg::AbortTest()
{
	MSG msg;

	while ( !m_fAbort && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
	{
		if ( !IsDialogMessage(&msg) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	/* fAbortPrint is TRUE (return is FALSE) if the user has aborted */
	return( m_fAbort );
}

BOOL CAliasApp::CreateAntiAliasedFile(LPCSTR lpForeFile, LPCSTR lpCompFile, LPCSTR lpDstFile, CWnd *pStatusCtrl)
{
	PDIB pForeDib, pCompDib;
	BOOL fRet = FALSE;
	char szStatus[512];

	if (m_statusDlg.AbortTest())
		return(FALSE);

#ifndef CPO
	wsprintf(szStatus, "Reading gray file: '%s'", lpForeFile);
	pStatusCtrl->SetWindowText(szStatus);
	if (!(pForeDib = ReadTarga(lpForeFile)))
		return(FALSE);
	if (m_statusDlg.AbortTest())
		return(FALSE);
#endif

	wsprintf(szStatus, "Reading color file: '%s'", lpCompFile);
	pStatusCtrl->SetWindowText(szStatus);
	if (!(pCompDib = ReadTarga(lpCompFile)))
	{

#ifndef CPO
		delete pForeDib;
#endif
		return(FALSE);
	}
	if (m_statusDlg.AbortTest())
		return(FALSE);
	#ifdef CPO
	pStatusCtrl->SetWindowText("Counting Colors");
	#else
	pStatusCtrl->SetWindowText("Performing Anti-Aliasing");
	#endif
	if (AntiAliasDib(pForeDib, pCompDib))
	{
		if (m_statusDlg.AbortTest())
			return(FALSE);
		#ifndef CPO
		wsprintf(szStatus, "Writing output file: '%s'", lpDstFile);
		pStatusCtrl->SetWindowText(szStatus);
		fRet = WriteTarga(pForeDib, lpDstFile);
		#else
		fRet = TRUE;
		#endif
	}
#ifndef CPO
	delete pForeDib;
#endif
	delete pCompDib;
		
	return(fRet);
}

BOOL CAliasApp::ProcessFiles(LPSTR lpGrayDir, LPSTR lpColorDir, LPSTR lpCompDir, BOOL fCountFiles)
{
	WIN32_FIND_DATA FindData;
	CWnd *pStatusCtrl;
	BOOL fRet = FALSE;
	char szWildCard[MAX_PATH];
	char szGrayFile[MAX_PATH];
	char szColorFile[MAX_PATH];
	char szCompFile[MAX_PATH];
	HANDLE hFind;

	lstrcpy(szWildCard, lpColorDir);
	lstrcat(szWildCard, "*.tga");

	if (fCountFiles)
	{
		hFind = FindFirstFile(szWildCard, &FindData);
		while (hFind != INVALID_HANDLE_VALUE)
		{
			if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
				++m_nTotal;
			if (!FindNextFile(hFind, &FindData))
				break;
		}
		if (hFind != INVALID_HANDLE_VALUE)
			FindClose(hFind);
		return(TRUE);
	}
	
	pStatusCtrl = m_statusDlg.GetDlgItem(IDC_STATUS);
	hFind = FindFirstFile(szWildCard, &FindData);
	while (hFind != INVALID_HANDLE_VALUE)
	{
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (!FindNextFile(hFind, &FindData))
				break;
			continue;
		}
		m_statusDlg.SetDlgItemInt(IDC_PERCENT, (m_nFiles * 100) / m_nTotal);

		lstrcpy(szGrayFile, lpGrayDir);
		lstrcat(szGrayFile, FindData.cFileName);
		lstrcpy(szColorFile, lpColorDir);
		lstrcat(szColorFile, FindData.cFileName);
		lstrcpy(szCompFile, lpCompDir);
		lstrcat(szCompFile, FindData.cFileName);

		// break out if we encounter an error
		if (!CreateAntiAliasedFile(szGrayFile, szColorFile, szCompFile, pStatusCtrl))
			break;

		++m_nFiles;
		if (!FindNextFile(hFind, &FindData))
			break;
	}
	m_statusDlg.SetDlgItemInt(IDC_PERCENT, (m_nFiles * 100) / m_nTotal);

	if (hFind != INVALID_HANDLE_VALUE)
		FindClose(hFind);

	return(!m_statusDlg.m_fAbort);
}

