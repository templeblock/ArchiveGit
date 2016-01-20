#include <windows.h>
#include <string.h>
#include <stdlib.h>
#define MAIN
#include <mmsystem.h>
#include "proto.h"
#include "main.h"
#include "mainid.h"
#include "commonid.h"
#include "memory.h"
#include "tgardr.h"

static LPMAINSCENE lpScene;
static BOOL bConvertDroppedFiles;
static BOOL bReadOnly;
static BOOL bSilent;
static BOOL bCompress;
static FNAME szDestDir;
static HPALETTE hStandardPal;

/************************************************************************/
BOOL CMainScene::OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{ // Parcel out init's to each scene that needs it
	DWORD dwStyle = GetWindowStyle(hWnd);
	DragAcceptFiles(hWnd, YES);
	
	CScene::OnInitDialog(hWnd, hWndFocus, lParam);
 	switch (m_nSceneNo)
	{
		case 1:
			break;

		default:
			break;
	}
   	return( FALSE );
}

//************************************************************************
void SetDestDir(LPSTR szDestPath, LPSTR szFileName)
//************************************************************************
{
	int length = _fstrlen(szFileName) - 1;

	// Remove the filename from the fully qualified file name
	_fstrcpy(szDestPath, szFileName);
	do  {
		char c = szDestPath[length];	
		if (c == '\\')  {
			szDestPath[length] = '\0';
			break;
		}	
			
		--length;	
	}		
	while (length >= 0);
}


//************************************************************************
void AppendFileName(LPSTR szDestPath, LPSTR szPath, LPSTR szFileName)
//************************************************************************
{
	LPSTR pNameOnly;
	
	int length = _fstrlen(szFileName);		
	pNameOnly = szFileName + length;		// Point to end of string;

	for (int i=0; i < length; i++)  {
		if (*(pNameOnly) == '\\')  {
			pNameOnly++;
			break;
		}
		else
			--pNameOnly;	
	}

	// Check for a \ at the end, append if necessary
	length = _fstrlen(szPath);
	if (szPath[length-1] != '\\')
		_fstrcat(szPath, "\\");
			
	_fstrcpy(szDestPath, szPath);
	_fstrcat(szDestPath, pNameOnly);
}



/***********************************************************************/
void CMainScene::OnDropFiles(HWND hWnd, HDROP hDrop)
/***********************************************************************/
{
	int iCount = DragQueryFile( hDrop, -1, NULL, NULL );
	SetWindowPos(hWnd, HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	
	for ( int i=0; i<iCount; i++ )
	{
		FNAME szFileName;
		DragQueryFile( hDrop, i, szFileName, sizeof(FNAME) );
		
		if ( !bConvertDroppedFiles )  {
			SetDestDir(szDestDir, szFileName);		
			SetDlgItemText(hWnd, IDC_DEST_DIR, szDestDir);
			
			DoProcessBGFile( hWnd, szFileName );
		}	
		else
		{
			if ( !hStandardPal )
			{
				Print( "First drop a single palette file or bitmap file first to load the conversion palette" );
				break;
			}
			else
				DoProcessFile( szFileName );
		}
		
		if ( GetAsyncKeyState(VK_ESCAPE))
			break;
	}

	DragFinish( hDrop );
}

//************************************************************************
HPALETTE CMainScene::LoadPaletteFromDib( LPSTR lpFileName )
//************************************************************************
{
	if ( !lpFileName )
		return(FALSE);

	HourGlass( YES );
	PDIB pDib;
	if ( FindString(lpFileName, ".tga") )
		pDib = ReadTarga(lpFileName);
	else
		pDib = CDib::LoadDibFromFile(lpFileName, NULL, FALSE, TRUE, FALSE);
	HourGlass( NO );

	if (!pDib)
	{
		Print("Error opening bitmap '%ls'", lpFileName);
		return(FALSE);
	}

	HourGlass( YES );
	HPALETTE hPal = CreateIdentityPalette( pDib->GetColors(), pDib->GetNumColors(), NO/*bMakeIdentity*/);
	HourGlass( NO );

	delete pDib;

	if ( !hPal )
	{
		Print("Error creating palette from bitmap '%ls'", lpFileName);
		return(FALSE);
	}

	return( hPal );
}

/***********************************************************************/
void CMainScene::DoProcessBGFile( HWND hWnd, LPSTR lpFileName )
/***********************************************************************/
{
	if ( hStandardPal )
	{
		DeletePalette( hStandardPal );
		hStandardPal = NULL;
		InvalidateRect( hWnd, NULL, TRUE );
		UpdateWindow( hWnd );
	}
		
	if ( FindString(lpFileName, ".pal") )
	{
		hStandardPal = LoadPalette(lpFileName);
		Print( "Error loading palette %ls", lpFileName );
	}
	else
		hStandardPal = LoadPaletteFromDib(lpFileName);

	if ( !hStandardPal )
		return;

	InvalidateRect( hWnd, NULL, TRUE );
	UpdateWindow( hWnd );
}

//************************************************************************
void CMainScene::DoProcessFile( LPSTR lpFileName )
//************************************************************************
{
	if ( !lpFileName )
		return;

	HourGlass( YES );
	PDIB pDib;
	if ( FindString(lpFileName, ".tga") )
		pDib = ReadTarga(lpFileName);
	else
		pDib = CDib::LoadDibFromFile(lpFileName, NULL, FALSE, TRUE, FALSE);
	HourGlass( NO );

	if (!pDib)
	{
		Print("Error opening bitmap '%ls'", lpFileName);
		return;
	}

	if ( !bReadOnly && !bSilent )
		if ( PrintOKCancel( "Convert bitmap '%ls'?", lpFileName ) == IDCANCEL )
			return;

	FNAME szDestName;
	GetDlgItemText(m_hWnd, IDC_DEST_DIR, szDestDir, sizeof(szDestDir));
	AppendFileName(szDestName, szDestDir, lpFileName);
	
    int iColors;
	GetObject(hStandardPal, 2, (LPSTR)&iColors);

	BYTE cRemapped[256];
    _fmemset( cRemapped, 0, sizeof(cRemapped) );

	HourGlass( YES );
	int iChanged = pDib->MapToPalette( hStandardPal, cRemapped );
	HourGlass( NO );

	int iRemapped  = 0;
	for ( int i=0; i<256; i++ )
		if ( cRemapped[i] )
			iRemapped++;

	if ( bReadOnly || !bSilent )
	{
		LPSTR lpString = ( bReadOnly ? "checked" : "converted" );
		Print( "'%ls' %ls:\n%d color table change(s), %d actually used", (LPSTR)szDestName, lpString, iChanged, iRemapped );
	}

	if ( !bReadOnly )
	{
		LPSTR lp;
		if ( lp = FindString(szDestName, ".tga") )
		{
			lp[0] = '.';
			lp[1] = 'b';
			lp[2] = 'm';
			lp[3] = 'p';
			iChanged = YES;
		}
		if ( iChanged || (bCompress != (pDib->GetCompression() == BI_RLE8)) )
		{
			HourGlass( YES );
			pDib->SetColorsUsed(iColors);
			
			PDIB pNewDib;
			if (bCompress && (pDib->GetCompression() != BI_RLE8))
				pNewDib = pDib->Convert( pDib->GetBitCount(), BI_RLE8 );

			if (!bCompress && (pDib->GetCompression() == BI_RLE8))
				pNewDib = pDib->Convert( pDib->GetBitCount(), BI_RGB );
				
			if ( pNewDib )
			{
				delete pDib;
				pDib = pNewDib;
			}
			
			if ( !pDib->WriteFile(szDestName) )
				Print( "Error writing bitmap '%ls'", szDestName );
			HourGlass( NO );
		}
	}

	delete pDib;
}

/***********************************************************************/
BOOL CMainScene::OnEraseBkgnd(HWND hWnd, HDC hDC)
/***********************************************************************/
{
	HBRUSH hBrush = CreateHatchBrush(HS_FDIAGONAL, RGB(128,128,128));
	HBRUSH hOldBrush = SelectBrush(hDC, hBrush);
	RECT rect;
	GetClientRect( hWnd, &rect );
	Rectangle(hDC, rect.left, rect.top, rect.right, rect.bottom);
	SelectObject(hDC, hOldBrush);
    DeleteBrush( hBrush );
	return( TRUE );
}

//************************************************************************
void CMainScene::OnPaint( HWND hWnd )
//************************************************************************
{
	HDC hDC;
	PAINTSTRUCT ps;
	if ( !(hDC = BeginPaint( hWnd, &ps )) )
		return;
	RECT rect;
	GetClientRect( hWnd, &rect );
	InflateRect( &rect, -5, -5 );
	rect.left += 160;
	Paint( hDC, &rect );
	EndPaint( hWnd, &ps );
}

//************************************************************************
void CMainScene::Paint( HDC hDC, LPRECT lpRect )
//************************************************************************
{
	if ( !hStandardPal )
		return;

    int iColors;
	GetObject(hStandardPal, 2, (LPSTR)&iColors);

	HPALETTE hOldPal = SelectPalette(hDC, hStandardPal, FALSE/*bProtectPhysicalPal*/ );
	RealizePalette( hDC );
	int right, bottom;
	int iWidth  = lpRect->right - lpRect->left;
	int iHeight = lpRect->bottom - lpRect->top;
	int top = 0;
	for (int j=0; j<16 && iColors; j++, top=bottom)
	{
		bottom = ((j+1) * iHeight / 16 + 1);
		int left = 0;
		for (int i=0; i<16 && iColors; i++, left=right)
		{
			right = ((i+1) * iWidth / 16 + 1);
			HBRUSH hBrush = CreateSolidBrush(PALETTEINDEX(j * 16 + i));
			HBRUSH hOldBrush = SelectBrush(hDC, hBrush);
			Rectangle(hDC, lpRect->left + left - 1,
						   lpRect->top + top - 1,
						   lpRect->left + right,
						   lpRect->top + bottom);
			SelectObject(hDC, hOldBrush);
		    DeleteBrush( hBrush );
			iColors--;
		}
	}

	SelectPalette(hDC, hOldPal, TRUE/*bProtectPhysicalPal*/);
}

//************************************************************************
void CMainScene::OnShiftDown( int nCount )
//************************************************************************
{
	if ( !hStandardPal )
		return;

	if ( nCount <= 0 )
		return;

	if ( nCount > 256 )
		nCount = 256;

	PALETTEENTRY Entry[256];
	int nColors = GetPaletteEntries( hStandardPal, 0, 256, Entry );
	if ( !nColors )
		return;
	int nNewColors = min( nColors + nCount, 256 );
	int nCopyColors = nNewColors - nCount;
	hmemcpy( Entry + nCount, Entry, nCopyColors * sizeof(PALETTEENTRY) );
    _fmemset( Entry, 0, nCount * sizeof(PALETTEENTRY) );
	ResizePalette( hStandardPal, nNewColors );
	SetPaletteEntries( hStandardPal, 0, nNewColors, Entry );
}

//************************************************************************
void CMainScene::OnShiftUp( int nCount )
//************************************************************************
{
	if ( !hStandardPal )
		return;

	if ( nCount <= 0 )
		return;

	if ( nCount > 256 )
		nCount = 256;

	PALETTEENTRY Entry[256];
	int nColors = GetPaletteEntries( hStandardPal, 0, 256, Entry );
	if ( !nColors )
		return;
	int nNewColors = max( nColors - nCount, 0 );
	hmemcpy( Entry, Entry + nCount, nNewColors * sizeof(PALETTEENTRY) );
	ResizePalette( hStandardPal, nNewColors );
	SetPaletteEntries( hStandardPal, 0, nNewColors, Entry );
}

//************************************************************************
void CMainScene::OnTrimTail( int nCount )
//************************************************************************
{
	if ( !hStandardPal )
		return;

	int nColors;
	GetObject(hStandardPal, 2, (LPSTR)&nColors);
	if ( !nColors )
		return;
	int nNewColors = max( nColors - nCount, 0 );
	ResizePalette(hStandardPal, nNewColors);
}

//************************************************************************
void CMainScene::OnAddSystemColors()
//************************************************************************
{
	if ( !hStandardPal )
		return;

	MakeIdentityPalette( hStandardPal );
}

//************************************************************************
void CMainScene::OnAddReservedColors()
//************************************************************************
{
	if ( !hStandardPal )
		return;

	static PALETTEENTRY Entry[10] = {
	//	red,	green,	blue,	flags
		255,	205,	  0,	PC_NOCOLLAPSE,
		255,	255,	128,	PC_NOCOLLAPSE,
		230,	154,	  0,	PC_NOCOLLAPSE,
		128,	128,	 64,	PC_NOCOLLAPSE,
		128,	 64,	  0,	PC_NOCOLLAPSE,
		255,	128,	 64,	PC_NOCOLLAPSE,
		  0,	  0,	  0,	PC_NOCOLLAPSE,//128,	128,	128,	PC_NOCOLLAPSE,
		  0,	  0,	  0,	PC_NOCOLLAPSE,
		  0,	  0,	  0,	PC_NOCOLLAPSE,
		  0,	  0,	  0,	PC_NOCOLLAPSE,
	};
	
	SetPaletteEntries(hStandardPal, 10, 10, Entry);
}

//************************************************************************
void CMainScene::OnAddTextColors()
//************************************************************************
{
	if ( !hStandardPal )
		return;

	static PALETTEENTRY Entry[3] = {
	//	red,	green,	blue,	flags
		255,	  0,	255,	PC_NOCOLLAPSE,
		255,	205,	  0,	PC_NOCOLLAPSE,
		255,	255,	128,	PC_NOCOLLAPSE,
	};
	
	SetPaletteEntries(hStandardPal, 20, 3, Entry);
}

//************************************************************************
void CMainScene::OnAddMagenta()
//************************************************************************
{
	if ( !hStandardPal )
		return;

	static PALETTEENTRY Entry[1] = {
	//	red,	green,	blue,	flags
		255,	  0,	255,	PC_NOCOLLAPSE,
	};
	
	SetPaletteEntries(hStandardPal, 253, 1, Entry);
}

//************************************************************************
void CMainScene::OnClrSystemColors()
//************************************************************************
{
	if ( !hStandardPal )
		return;

	PALETTEENTRY Entry[10];
	int len = sizeof(Entry);
    _fmemset( Entry, 0, len );
	SetPaletteEntries(hStandardPal,   0, 10, Entry);
	SetPaletteEntries(hStandardPal, 246, 10, Entry);
}

//************************************************************************
void CMainScene::OnClrReservedColors()
//************************************************************************
{
	if ( !hStandardPal )
		return;

	PALETTEENTRY Entry[10];
	int len = sizeof(Entry);
    _fmemset( Entry, 0, len );
	SetPaletteEntries(hStandardPal, 10, 10, Entry);
}

//************************************************************************
void CMainScene::OnCommand(HWND hWnd, int id, HWND loWord, UINT hiWord)
//************************************************************************
{ // Parcel out commands to each scene
	switch (m_nSceneNo)
	{
		case 1:
		{
			switch ( id )
			{
				case IDC_SHIFTDOWN:
				{
					OnShiftDown(1);
					InvalidateRect( hWnd, NULL, TRUE );
					UpdateWindow( hWnd );
					break;
				}
				case IDC_SHIFTDOWN20:
				{
					OnShiftDown(20);
					InvalidateRect( hWnd, NULL, TRUE );
					UpdateWindow( hWnd );
					break;
				}
				case IDC_SHIFTUP:
				{
					OnShiftUp(1);
					InvalidateRect( hWnd, NULL, TRUE );
					UpdateWindow( hWnd );
					break;
				}
				case IDC_TRIMTAIL:
				{
					OnTrimTail(1);
					InvalidateRect( hWnd, NULL, TRUE );
					UpdateWindow( hWnd );
					break;
				}
				case IDC_ADDSYSCOL:
				{
					OnAddSystemColors();
					InvalidateRect( hWnd, NULL, TRUE );
					UpdateWindow( hWnd );
					break;
				}
				case IDC_CLRSYSCOL:
				{
					OnClrSystemColors();
					InvalidateRect( hWnd, NULL, TRUE );
					UpdateWindow( hWnd );
					break;
				}
				case IDC_ADDRESCOL:
				{
					OnAddReservedColors();
					InvalidateRect( hWnd, NULL, TRUE );
					UpdateWindow( hWnd );
					break;
				}
				case IDC_CLRRESCOL:
				{
					OnClrReservedColors();
					InvalidateRect( hWnd, NULL, TRUE );
					UpdateWindow( hWnd );
					break;
				}
				case IDC_ADDTXTCOL:
				{
					OnAddTextColors();
					InvalidateRect( hWnd, NULL, TRUE );
					UpdateWindow( hWnd );
					break;
				}
				case IDC_ADDMAGENTA:
				{
					OnAddMagenta();
					InvalidateRect( hWnd, NULL, TRUE );
					UpdateWindow( hWnd );
					break;
				}
				case IDC_CONVERT:
				{
					bConvertDroppedFiles = !bConvertDroppedFiles;
					CheckDlgButton(hWnd, id, bConvertDroppedFiles);
					break;
				}
				case IDC_READONLY:
				{
					bReadOnly = !bReadOnly;
					CheckDlgButton(hWnd, id, bReadOnly);
					break;
				}
				case IDC_SILENT:
				{
					bSilent = !bSilent;
					CheckDlgButton(hWnd, id, bSilent);
					break;
				}
				case IDC_COMPRESS:
				{
					bCompress = !bCompress;
					CheckDlgButton(hWnd, id, bCompress);
					break;
				}
			}
			break;
		}
	}
}

//***********************************************************************
void CMainScene::OnClose(HWND hWnd)
//***********************************************************************
{ // Parcel out the close to each scene
	CScene::OnClose( hWnd );
	switch (m_nSceneNo)
	{
		case 1:
		default:
			break;
	}
}

//***********************************************************************
BOOL CMainApp::CreateMainWindow()
//***********************************************************************
{
	// Size the application window based on the screen size
	int iWidth = GetSystemMetrics( SM_CXSCREEN );
	int i = GetSystemMetrics( SM_CYCAPTION );
	if ( iWidth > 640 ) iWidth = 640;
	int iHeight = GetSystemMetrics( SM_CYSCREEN );
	if ( iHeight > 480+i ) iHeight = 480+i;

	// Load app name resource
	STRING szTitle;
	LoadString( GetInstance(), IDS_TITLE, szTitle, sizeof(szTitle) );

	// Create the application window
	if ( !(m_hMainWnd = CreateWindow(
		m_szClass,			// lpClassName
		szTitle,			// lpWindowName
		WS_POPUP |			// with WS_OVERLAPPED you can't make window bigger than screen
		WS_MAXIMIZE |
		WS_THICKFRAME |
		WS_CLIPCHILDREN |
		WS_CAPTION |
		WS_SYSMENU |
		WS_MINIMIZEBOX |
		WS_MAXIMIZEBOX,		// dwStyle
		0, 0,				// position of window
		iWidth, iHeight,	// size of window
		NULL,				// hWindowParent (null for tiled)
		NULL,				// hMenu (null = class menu)
		GetInstance(),			// hInstance
		NULL				// lpParam
		)) )
			return FALSE;

	// Make window visible according to the way the app is activated
	ShowWindow( m_hMainWnd, m_nCmdShow );
	if ( m_nCmdShow != SW_HIDE )
		UpdateWindow( m_hMainWnd );

	return(TRUE);
}

//***********************************************************************
BOOL CMainApp::InitInstance()
//***********************************************************************
{
	if (!CPHApp::InitInstance())
		return(FALSE);

    return TRUE;
}

//***********************************************************************
BOOL CMainApp::GotoScene( HWND hWndPreviousScene, int iScene )
//***********************************************************************
{ // Standard fare for most titles
	if ( !iScene )
		{
		PostMessage( GetMainWnd(), WM_CLOSE, 0, 0L );
		return( TRUE );
		}

    if ( hWndPreviousScene )
        SendMessage( hWndPreviousScene, WM_CLOSE, 0, 0L );
          
    lpScene = new CMainScene();
    if (!lpScene)
		return(FALSE);
	if (!lpScene->Create(YES, App.GetInstance(), App.GetMainWnd(), iScene))
		return(FALSE);
    return( TRUE );
}

//***********************************************************************
void CMainApp::OnPaletteChanged(HWND hWnd, HWND hWndCausedBy)
/***********************************************************************/
{ // Some window caused (or is causing) the palette to change
	if ( hWndCausedBy != hWnd )
        OnQueryNewPalette(hWnd);
}

// Note: Windows actually ignores the return value.
//***********************************************************************
BOOL CMainApp::OnQueryNewPalette(HWND hWnd)
//***********************************************************************
{
	// We are going active or the system palette has changed
	// so realize our palette.
	if (hStandardPal)
	{
		HDC hDC = GetDC(NULL);
		HPALETTE hOldPal = SelectPalette(hDC, hStandardPal, FALSE/*bProtectPhysicalPal*/);
		UINT u = RealizePalette(hDC);
		if (u) // Some colors changed so we need to do a repaint.
		{
			InvalidateRect(hWnd, NULL, TRUE); // Repaint the lot.
			UpdateWindow( hWnd );
		}
		SelectPalette(hDC, hOldPal, TRUE/*bProtectPhysicalPal*/);
		ReleaseDC(NULL, hDC);
	}

	return(TRUE); // Windows ignores this.
}
