#include <windows.h>
#include <commdlg.h>
#include <mmsystem.h>
#include <stdlib.h> // needed for srand
#include <memory.h> // needed for _fmemset
#include <time.h>
#include <dos.h>
#include <sys\stat.h> // needed for stat
#include <direct.h> // needed for chdir()
#ifndef WIN32
#include <stdio.h> 	  // needed for remove	
#endif
#include "proto.h"
#include "commonid.h"
#include "wing.h"

/************************************************************************/
BOOL FileFind( LPSTR lpPathName, LPSTR lpFileName, LPSTR lpExpFileName )
/************************************************************************/
{
	if ( !lpFileName || !lpExpFileName )
		return( NO );
	lstrcpy( lpExpFileName, lpFileName );

	if ( !lpPathName )
		lpPathName = lpFileName;

	STRING szFilter;
	lstrcpy( szFilter, FileName(lpFileName) );
	lstrcat( szFilter, " (*" );
	lstrcat( szFilter, Extension(lpFileName) );
	lstrcat( szFilter, ")|*" );
	lstrcat( szFilter, Extension(lpFileName) );
	lstrcat( szFilter, "|All Files (*.*)|*.*|" );
	int iLength = lstrlen( szFilter );

	char  chReplace;	/* string separator for szFilter */
	chReplace = szFilter[iLength - 1]; // retrieve wildcard character
	for ( int i = 0; szFilter[i] != '\0'; i++ )
	{
		if (szFilter[i] == chReplace)
		   szFilter[i] = '\0';
	}

	OPENFILENAME ofn;
	_fmemset(&ofn, 0, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = GetFocus();
	ofn.lpstrFilter = szFilter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = lpExpFileName;
	ofn.nMaxFile = sizeof(FNAME);
	ofn.lpstrInitialDir = lpPathName;
	ofn.lpstrTitle = "Find File"; // title of the dialog - "Open" by default
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	return( GetOpenFileName(&ofn) );
}


/************************************************************************/
BOOL FileExistsExpand( LPSTR lpFileName, LPSTR lpExpFileName )
/************************************************************************/
{
	if ( !lpFileName )
		return( NO );

	#ifndef WIN32
	UINT errMode = SetErrorMode(SEM_FAILCRITICALERRORS);
	#endif
	FNAME szVolume;
	GetVolumeID( lpFileName, szVolume );
	OFSTRUCT of;
	HFILE hFile;
	if ( (hFile = OpenFile( lpFileName, &of, OF_READ )) != HFILE_ERROR )
	{
		_lclose( hFile );
		if ( lpExpFileName )
			lstrcpy( lpExpFileName, of.szPathName );
		#ifndef WIN32
		SetErrorMode(errMode);
		#endif
		return( TRUE );
	}

	#ifndef WIN32
	SetErrorMode(errMode);
	#endif
	return( NO );
}

/************************************************************************/
static UINT ExtractDiskNumber( LPSTR lpFileName )
/************************************************************************/
{
	STRING szString;
	lstrcpy( szString, FileName(lpFileName) );
	StripExt( szString );
	char c = szString[ lstrlen(szString)-1 ];
	if ( c >= '0' && c <= '9' )
		return( c - '0' );
	return( 0 );
}

/************************************************************************/
BOOL FindContent( LPSTR lpFileName, LPSTR lpExpFileName )
/************************************************************************/
{
	FNAME szFileName;

	if ( !lpFileName )
		return( FALSE );

	// 1.) Try the file name as passed in
	lstrcpy( szFileName, lpFileName );
	if ( FileExistsExpand( szFileName, lpExpFileName ) )
		return( TRUE );

	// 2.) Try looking in the program home path
	GetModuleFileName( GetApp()->GetInstance(), szFileName, sizeof(szFileName) );
	StripFile( szFileName );
	lstrcat( szFileName, FileName(lpFileName) );
	if ( FileExistsExpand( szFileName, lpExpFileName ) )
		return( TRUE );

	FNAME szIni;
	szIni[0] = '\0';
	LoadString( GetApp()->GetInstance(), IDS_INI, szIni, sizeof(szIni) );

	while ( 1 )
	{
		// 3a.) Try looking in the last "Content" path where we found something
		int iLastContent = GetPrivateProfileInt( "Options", "iLastContent", 0, szIni );
		LPSTR lpContent = ( !iLastContent ? "Content" : "Content1" );
		if ( GetPrivateProfileString( "Options", lpContent, "", szFileName, sizeof(szFileName), szIni ) )
		{
			StripFile( szFileName );
			lstrcat( szFileName, FileName(lpFileName) );
			if ( FileExistsExpand( szFileName, lpExpFileName ) )
				return( TRUE );
		}

		// 3b.) Try looking in the other "Content1" path
		iLastContent = !iLastContent;
		lpContent = ( !iLastContent ? "Content" : "Content1" );
		if ( GetPrivateProfileString( "Options", lpContent, "", szFileName, sizeof(szFileName), szIni ) )
		{
			StripFile( szFileName );
			lstrcat( szFileName, FileName(lpFileName) );
			if ( FileExistsExpand( szFileName, lpExpFileName ) )
			{
				wsprintf( szFileName, "%d", iLastContent );
				WritePrivateProfileString( "Options", "iLastContent", szFileName, szIni );
				return( TRUE );
			}
		}

		// 3c.) Prompt to insert the CDROM disk...
		UINT wDisk, uRet;
		if ( wDisk = ExtractDiskNumber( lpFileName ) )
			uRet = PrintOKCancel( "Please insert CDROM game disk %d.", wDisk );
		else
			uRet = PrintOKCancel( "Please insert CDROM game disk." );
		if ( uRet == IDCANCEL )
			break;
		Delay( 5000L ); // wait in case the CD is not up to speed
	}

	// See if the user wants to browse
	while ( 1 )
	{
		UINT uRet = PrintOKCancel( "Looking for file '%s'.\nPress 'OK' to browse for it.\nPress 'Cancel' to exit.", lpFileName );
		if ( uRet == IDCANCEL )
		{
			SendMessage( GetApp()->GetMainWnd(), WM_CLOSE, 0, 0L );
			return( FALSE );
		}
	
		// 4b.) Still can't find it, so give them the dialog box to find the content
		GetPrivateProfileString( "Options", "Content", lpFileName, szFileName, sizeof(szFileName), szIni );
		if ( FileFind( szFileName/*lpDir*/, lpFileName/*lpFile*/, lpExpFileName ) )
		{
			lstrcpy( szFileName, lpExpFileName );
			StripFile( szFileName );
			int iContent = GetPrivateProfileInt( "Options", "iContent", 0, szIni );
			if ( iContent )
				WritePrivateProfileString( "Options", "Content", szFileName, szIni );
			else
				WritePrivateProfileString( "Options", "Content1", szFileName, szIni );
			wsprintf( szFileName, "%d", !iContent );
			WritePrivateProfileString( "Options", "iContent", szFileName, szIni );
			return( TRUE );
		}
	}

	return( FALSE );
}

//	Volume Checking Code
//		BOOL bMultiVolume = lstrcmpi( szFileName, szExpFileName );
//		if ( bMultiVolume )
//		{ // Perform a volume check
//			STRING szVolume;
//			GetVolumeID( szExpFileName, szVolume );
//			wDiskNew = ExtractDiskNumber( szExpFileName );
//			if ( wDisk) == wDiskNew )
//				break;
//	
//			wsprintf( szString, "Please insert disk %d.", wDisk );
//			Print( szString );
//			goto Repeat;
//		}

/************************************************************************/
LPTSTR GetVolumeID(LPTSTR lpPath, LPTSTR lpVolume)
/************************************************************************/
{
	FNAME szRoot;
	szRoot[0] = *lpPath;
	szRoot[1] = ':';
	szRoot[2] = '\\';
	szRoot[3] = '\0';

#ifdef WIN32
	DWORD dwSerialNumber, dwComponentLength, dwFlags;
	if ( !GetVolumeInformation( szRoot, lpVolume, 12, &dwSerialNumber, 
			&dwComponentLength, &dwFlags, NULL, 0 ) )
		*lpVolume = '\0'; 
	return( lpVolume );
#else
	lstrcat( szRoot, "*.*" );

	// get volume name for this disk
	struct find_t afile;
	int retc = _dos_findfirst( szRoot, _A_VOLID, &afile );
	if ( retc )
		*lpVolume = '\0';
	else
	{
		afile.name[11] = '\0';
		lstrcpy( lpVolume, afile.name );
	}
	return( lpVolume );
#endif
}

/************************************************************************/
BOOL GetVolumeSerialNumber(LPTSTR lpPath, LPDWORD lpSerialNumber)
/************************************************************************/
{
	FNAME szRoot;
	szRoot[0] = '\0';
	FileExistsExpand( lpPath, szRoot );
	szRoot[3] = '\0';;

#ifdef WIN32
	DWORD dwComponentLength, dwFlags;
	if ( GetVolumeInformation( szRoot, NULL, 0, lpSerialNumber, 
			&dwComponentLength, &dwFlags, NULL, 0 ) )
		return( TRUE );

	*lpSerialNumber = '\0';
	return( FALSE );
#else
	lstrcat( szRoot, "*.*" );

	// get volume name for this disk
	struct find_t afile;
	int retc = _dos_findfirst( szRoot, _A_VOLID, &afile );
	if ( retc )
	{
		*lpSerialNumber = '\0';
		return( FALSE );
	}
	else
	{
		*lpSerialNumber = MAKELONG( afile.wr_date, afile.wr_time );
		return( TRUE );
	}
#endif
}

/***********************************************************************/
void PrintWindowCoordinates( HWND hWindow )
/***********************************************************************/
{
#ifdef _DEBUG
	RECT ClientRect, ScreenRect;

	GetWindowRect( hWindow, &ScreenRect );
	GetClientRect( hWindow, &ClientRect );
	MapWindowPoints( hWindow, GetParent(hWindow), (LPPOINT)&ClientRect, 2 );
	Print( "From UL Screen (%d,%d),\nFrom UL Parent (%d,%d),\nSize (%d,%d)",
			ScreenRect.left, ScreenRect.top,
			ClientRect.left, ClientRect.top,
			ClientRect.right - ClientRect.left,
			ClientRect.bottom - ClientRect.top );
#endif
}

/***********************************************************************/
HWND FindClassDescendent(HWND hParent, LPCTSTR lpClassName)
/***********************************************************************/ 
{
	HWND hNext = GetWindow(hParent, GW_CHILD);
	while (hNext)
	{	
		STRING szClassName;
		
		GetClassName(hNext, szClassName, sizeof(szClassName));
		if (lstrcmpi(lpClassName, szClassName) == 0)
			return(hNext);
		HWND hWnd = FindClassDescendent(hNext, lpClassName);
		if (hWnd)
			return(hWnd);
		hNext = GetWindow(hNext, GW_HWNDNEXT);
	}
	return(NULL);
}

/***********************************************************************/
POINT GetPrivateProfilePoint(LPSTR szSection, LPSTR szKey, POINT ptDefault, LPCTSTR szFile)
/***********************************************************************/
{
	char buf[16];
	LPSTR lp1, lp2;
	POINT pt;
	
	if ( !GetPrivateProfileString(szSection, szKey, "", buf, sizeof(buf), szFile) )
		return ptDefault;
	lp1 = buf;
	if ( !(lp2 = FindString(lp1, ",")) )
		return ptDefault;
	lp2++;
	pt.x = latoi(lp1);
	pt.y = latoi(lp2);
	return pt;
}

/***********************************************************************/
BOOL GetPrivateProfileRect(LPSTR szSection, LPSTR szKey, LPRECT pRect, LPCTSTR szFile)
/***********************************************************************/
{
	char buf[32];
	LPSTR lp1, lp2, lp3, lp4;
	
	if ( !GetPrivateProfileString(szSection, szKey, "", buf, sizeof(buf), szFile) )
		return FALSE;
	lp1 = buf;
	if ( !(lp2 = FindString(lp1, ",")) )
		return FALSE;
	lp2++;
	if ( !(lp3 = FindString(lp2, ",")) )
		return FALSE;
	lp3++;
	if ( !(lp4 = FindString(lp3, ",")) )
		return FALSE;
	lp4++;
	pRect->left	  = latoi(lp1);
	pRect->top	  = latoi(lp2);
	pRect->right  = latoi(lp3);
	pRect->bottom = latoi(lp4);
	return TRUE;
}

/***********************************************************************/
LONG GetPrivateProfileLong(LPCTSTR szSection, LPCTSTR szKey, LONG lDefault, LPCTSTR szFile)
/***********************************************************************/
{
	char szBuf[32], szDefault[32];

	wsprintf(szDefault, "%ld", lDefault);
	GetPrivateProfileString(szSection, szKey, szDefault, szBuf, sizeof(szBuf), szFile);
	return(latol(szBuf));
}

/***********************************************************************/
BOOL WritePrivateProfileLong(LPCTSTR szSection, LPCTSTR  lpKeyName, long lValue, LPCTSTR  lpFileName)
/***********************************************************************/
{
	char szBuf[32];

	wsprintf(szBuf, "%ld", lValue);
	return(WritePrivateProfileString(szSection, lpKeyName, szBuf, lpFileName));
}

/***********************************************************************/
void AsciiRGB( LPTSTR pszAscii, LPRGBTRIPLE pRGB )
/***********************************************************************/
{
	LPTSTR psz, pszStr = pszAscii;
	WORD wChar;

	// replace commas with NULL terminators
	while( *pszStr )
	{
		if( IsDBCSLeadByte(( BYTE )*pszStr ))
			wChar = *(( LPWORD )pszStr );
		else
			wChar = *pszStr;

		if( wChar == ',' )
		{
			psz = pszStr;
			pszStr = MyCharNext( pszStr );
			*psz = '\0';
		}
		else
			pszStr = MyCharNext( pszStr );
	}

	pRGB->rgbtRed= latoi( pszAscii );
	pszAscii += (lstrlen( pszAscii ) + 1 );

	pRGB->rgbtGreen = latoi( pszAscii );
	pszAscii += (lstrlen( pszAscii ) + 1 );

	pRGB->rgbtBlue = latoi( pszAscii );
}

//************************************************************************
HBITMAP DrawTransBitmap( HDC hDCscreen, HBITMAP hBMobject, HBITMAP hBMmask, int cx, int cy )
//************************************************************************
{
BITMAP bm;
HDC hDCcompose, hDCobject, hDCmask;
HBITMAP hBMcompose;
#define SRCNAND 0x00220326

if ( !hDCscreen )
	return( NULL );
if ( !hBMobject )
	return( NULL );
GetObject( hBMobject, sizeof(BITMAP), &bm );

hBMcompose = NULL;
hDCcompose = NULL;
hDCmask	   = NULL;
hDCobject  = NULL;

// Setup the compose DC and bitmap
if ( !(hDCcompose = CreateCompatibleDC( NULL )) )
	goto ErrorExit;
if ( !(hBMcompose = CreateCompatibleBitmap( hDCscreen, bm.bmWidth, bm.bmHeight)) )
	goto ErrorExit;
cx -= bm.bmWidth/2;
cy -= bm.bmHeight/2;
SelectObject( hDCcompose, hBMcompose );
BitBlt( hDCcompose, 0, 0, bm.bmWidth, bm.bmHeight, hDCscreen, cx, cy, SRCCOPY );

// Setup the object DC and bitmap
if ( !(hDCobject = CreateCompatibleDC( NULL )) )
	goto ErrorExit;
SelectObject( hDCobject, hBMobject );

// Setup the mask DC and bitmap
if ( !(hDCmask = CreateCompatibleDC( NULL )) )
	goto ErrorExit;
if ( hBMmask )
	{
	SelectObject( hDCmask, hBMmask );
	BitBlt( hDCobject, 0, 0, bm.bmWidth, bm.bmHeight, hDCmask, 0, 0, SRCNAND );
	}
else
	{
	if ( !(hBMmask = MakeBitmapMono( hDCmask, hDCobject, hBMobject, &bm )) )
		goto ErrorExit;
	}

BitBlt( hDCcompose, 0, 0, bm.bmWidth, bm.bmHeight, hDCmask, 0, 0, SRCAND );
BitBlt( hDCcompose, 0, 0, bm.bmWidth, bm.bmHeight, hDCobject, 0, 0, SRCPAINT );

// Cleanup the mask
DeleteDC( hDCmask );

// Cleanup the object
DeleteDC( hDCobject );

BitBlt( hDCscreen, cx, cy, bm.bmWidth, bm.bmHeight, hDCcompose, 0, 0, SRCCOPY );

// Cleanup the compose
DeleteDC( hDCcompose );
DeleteObject( hBMcompose );
return( hBMmask );

ErrorExit:
if ( hDCmask )		DeleteDC( hDCmask );
if ( hDCobject )	DeleteDC( hDCobject );
if ( hDCcompose )	DeleteDC( hDCcompose );
if ( hBMcompose )	DeleteObject( hBMcompose );
return( hBMmask );
}


//************************************************************************
HBITMAP MakeBitmapMono( HDC hDCmask, HDC hDCobject, HBITMAP hBMobject, LPBITMAP lpBM )
//************************************************************************
{
HBITMAP hBMmask;
BITMAP	bm;
HPALETTE hPal, hOldPal;
COLORREF cOldColor, cColor;

if ( !hDCmask || !hDCobject || !lpBM )
	return( NULL );
bm = *lpBM;

if ( !(hBMmask = CreateBitmap( bm.bmWidth, bm.bmHeight, 1, 1, NULL )) )
	return( NULL );

SelectObject( hDCmask, hBMmask );
if ( hPal = CopySystemPalette() )
	{
	hOldPal = SelectPalette( hDCobject, hPal, FALSE/*bProtectPhysicalPal*/ );
	RealizePalette( hDCobject );
	cColor = GetPixel( hDCobject, 0, 0 ) | 0x02000000; // make PALETTERGB()
	}
else
	cColor = GetPixel( hDCobject, 0, 0 );
cOldColor = SetBkColor( hDCobject, cColor );
BitBlt( hDCmask, 0, 0, bm.bmWidth, bm.bmHeight, hDCobject, 0, 0, SRCCOPY );
BitBlt( hDCobject, 0, 0, bm.bmWidth, bm.bmHeight, hDCmask, 0, 0, SRCNAND );
SetBkColor( hDCobject, cOldColor );
if ( hPal )
	{
	SelectPalette( hDCobject, hOldPal, FALSE/*bProtectPhysicalPal*/ );
	RealizePalette( hDCobject );
	DeleteObject( hPal );
	}
return( hBMmask );
}

//***********************************************************************
BOOL IsOurWindow(HWND	hWnd)
//***********************************************************************
{
#ifdef WIN32
	return TRUE;
#else
	return(GetCurrentTask() == GetWindowTask(hWnd));
#endif
}


/***********************************************************************/
BOOL StyleOn( HWND hWnd, DWORD	dwFlag)
/***********************************************************************/
{
if ( !hWnd )
	return( NO );
dwFlag &= GetWindowLong( hWnd, GWL_STYLE );
return( dwFlag != 0 );
}

/************************************************************************/
void SendMessageToChildren(
/************************************************************************/
HWND	hParent,
WORD	msg,
WORD	wParam,
long	lParam)
{
HWND	hChild;

hChild = GetWindow(hParent, GW_CHILD);
while (hChild)
	{
	SendMessage(hChild, msg, wParam, lParam);
	SendMessageToChildren(hChild, msg, wParam, lParam);
	hChild = GetWindow(hChild, GW_HWNDNEXT);
	}
}



/***********************************************************************/
void ResizeControl( HWND hControl, int dx, int dy, int x, int y )
/***********************************************************************/
{
RECT Rect;
int flag;
POINT pt;

GetClientRect( hControl, &Rect );
if ( dx == RectWidth(&Rect) && dy == RectHeight(&Rect) )
	return;

// Convert control's coordinates to be relative to parent
if ( (x == -1) && (y == -1) )
	{ // no new origin was passed
	pt.x = Rect.left;
	pt.y = Rect.right;
	MapWindowPoints( hControl, GetParent(hControl), &pt, 1 );
	flag = SWP_NOMOVE;
	}
else
	{ // a new origin was passed
	pt.x = x;
	pt.y = y;
	flag = 0;
	}

// Center it, if desired
//pt.x = ((Rect.left + Rect.right - dx - 1) / 2);
//pt.y = ((Rect.top + Rect.bottom - dy - 1) / 2);
//flag = 0;

flag |= ( IsWindowVisible(hControl) ? 0: SWP_NOREDRAW );
SetWindowPos( hControl, NULL, pt.x, pt.y, dx, dy, flag | SWP_NOZORDER | SWP_NOACTIVATE );
}


/***********************************************************************/
void CenterWindow( HWND hWnd )
/***********************************************************************/
{
	RECT ClientRect, WindowRect;
	int dx, dy;

	// Move the window to be centered within it's parent client area
	GetClientRect( GetParent(hWnd), &ClientRect );
	GetWindowRect( hWnd, &WindowRect );
	dx = RectWidth(&ClientRect) - RectWidth(&WindowRect);
	dx /= 2;
	dy = RectHeight(&ClientRect) - RectHeight(&WindowRect);
	dy /= 2;
	UINT flag = ( IsWindowVisible(hWnd) ? 0: SWP_NOREDRAW );
	SetWindowPos( hWnd, NULL, dx, dy, 0, 0,
		flag | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
}

/***********************************************************************/
int Print( LPSTR lpFormat, ... )
/***********************************************************************/
{
// Put up a modal message box with arguments specified exactly like printf
LPSTR lpArguments = (LPSTR)&lpFormat + sizeof(lpFormat);
char szBuffer[512];
wvsprintf( szBuffer, lpFormat, (LPSTR)lpArguments );
HWND hFocusWindow = GetFocus();
// Load app name resource
STRING szTitle;
LoadString( GetApp()->GetInstance(), IDS_TITLE, szTitle, sizeof(szTitle) );
int retc = MessageBox( NULL/*MessageParent*/, szBuffer, szTitle, MB_OK | MB_TASKMODAL );
SetFocus( hFocusWindow );
return( retc );
}

/***********************************************************************/
int PrintOKCancel( LPSTR lpFormat, ... )
/***********************************************************************/
{
// Put up a modal message box with arguments specified exactly like printf
LPSTR lpArguments = (LPSTR)&lpFormat + sizeof(lpFormat);
char szBuffer[512];
wvsprintf( szBuffer, lpFormat, (LPSTR)lpArguments );
HWND hFocusWindow = GetFocus();
// Load app name resource
STRING szTitle;
LoadString( GetApp()->GetInstance(), IDS_TITLE, szTitle, sizeof(szTitle) );
int retc = MessageBox( NULL/*MessageParent*/, szBuffer, szTitle, MB_OKCANCEL | MB_TASKMODAL );
SetFocus( hFocusWindow );
return( retc );
}

/***********************************************************************/
void Debug( LPSTR lpFormat, ... )
/***********************************************************************/
{
#ifdef _DEBUG
	// Put up a modal message box with arguments specified exactly like printf
	LPSTR lpArguments = (LPSTR)&lpFormat + sizeof(lpFormat);
	char szBuffer[512];
	wvsprintf( szBuffer, lpFormat, (LPSTR)lpArguments );
	lstrcat( szBuffer, "\n" );
	OutputDebugString( szBuffer );
#endif
}


// The reason for this is that we want to type PIXEL coordinates into the RC
// file.  Windows converts these to pixels, we convert them back to the
// values typed into the RC statement.

// Reposition all the controls in a dialog box to use 1:1 pixel mapping.
// All units specified in the dialog template in the resource file will
// be interpreted as PIXELS not DIALOG UNITS.

// Repositon the dialog before the controls so that if the controls
// try to position themselves (like centering) it will work. (ted)

/***********************************************************************/
void RepositionDialogControls( HWND hWnd )
/***********************************************************************/
{
int flag;
HWND hParent, hChild;
RECT rect;
DWORD dwUnits;
WORD wBaseWidth, wBaseHeight;

if ( !hWnd )
	return;
	
dwUnits = GetDialogBaseUnits();
wBaseWidth	= LOWORD(dwUnits);
wBaseHeight = HIWORD(dwUnits);
	
if ( hParent = GetParent(hWnd) )
	{
	GetClientRect( hWnd, &rect );
	MapWindowPoints( hWnd, hParent, (LPPOINT)&rect, 2 );
	SetRect(&rect,
		(rect.left	 * 4) / wBaseWidth,
		(rect.top	 * 8) / wBaseHeight,
		(rect.right	 * 4) / wBaseWidth,
		(rect.bottom * 8) / wBaseHeight);
	flag = ( IsWindowVisible(hWnd) ? 0: SWP_NOREDRAW );
	SetWindowPos(hWnd,
		NULL,
		rect.left,
		rect.top,
		rect.right	- rect.left,
		rect.bottom - rect.top,
		flag | SWP_NOZORDER | SWP_NOACTIVATE );
	}

hChild = GetWindow(hWnd, GW_CHILD);
while (hChild)
	{
	GetClientRect( hChild, &rect );
	MapWindowPoints( hChild, hWnd, (LPPOINT)&rect, 2 );
	SetRect(&rect,
		(rect.left	 * 4) / wBaseWidth,
		(rect.top	 * 8) / wBaseHeight,
		(rect.right	 * 4) / wBaseWidth,
		(rect.bottom * 8) / wBaseHeight);
	flag = ( IsWindowVisible(hChild) ? 0: SWP_NOREDRAW );
	SetWindowPos(hChild,
		NULL,
		rect.left,
		rect.top,
		rect.right	- rect.left,
		rect.bottom - rect.top,
		flag | SWP_NOZORDER | SWP_NOACTIVATE );
		
	// Go on to the next child
	hChild = GetWindow(hChild, GW_HWNDNEXT);
	}
}

//************************************************************************
LPSTR StripFile( LPSTR lpPath )
//************************************************************************
{
LPSTR lp;

if ( lp = FileName( lpPath ) )
	*lp = '\0';
return( lpPath );
}

//************************************************************************
LPSTR StripExt( LPSTR lpPath )
//************************************************************************
{
LPSTR lp;

if ( lp = Extension( lpPath ) )
	*lp = '\0';
return( lpPath );
}


//************************************************************************
LPSTR FileName( LPSTR lpPath )
//************************************************************************
{
LPSTR lp;

lp = lpPath;
lp += lstrlen( lpPath );
while( *lp != '\\' && *lp != ':' )
	{
	if ( (--lp) < lpPath )
		return( lpPath );
	}
lp++;
return( lp ); // the file name
}

//************************************************************************
LPSTR Extension( LPSTR lpPath )
//************************************************************************
{
LPSTR lp;

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
void Delay( long lTicks )
//************************************************************************
{
DWORD dwBaseTime;

/* Pause for a few milliseconds */
dwBaseTime = timeGetTime() + lTicks;
while ( timeGetTime() < dwBaseTime )
	;
}


//************************************************************************
WORD GetCycleNumber( WORD wCount )
//************************************************************************
{
static long lFake = -1;

if ( !wCount )
	return( 0 );

return( (WORD)(++lFake) % wCount );
}


//************************************************************************
WORD GetRandomNumber( WORD wCount )
//************************************************************************
{
static BOOL bInit;

// Seed the random number generator with current time so that
// the numbers will be different every time we run
if ( !bInit )
	{ // Can't do this every time; too much of a pattern
	srand( (unsigned int)time( NULL ) );
	bInit = YES;
	}

if ( !wCount )
	return( 0 );

return( (WORD)rand() % wCount );
}


//************************************************************************
BOOL GetString( ITEMID idString, LPSTR lpString )
//************************************************************************
{
*lpString = '\0';
return ( LoadString( GetApp()->GetInstance(), idString, lpString, sizeof(STRING)-1 ) );
}

/***********************************************************************/
LFIXED fget( long numer, long denom )
/***********************************************************************/
{
LFIXED val;
char flag;

if ( numer < 0L )
	{
	numer = -numer;
	flag = 1;
	}
else
	flag = 0;

if ( denom < 0L )
	{
	denom = -denom;
	flag = !flag;
	}

while ( numer > 0xFFFFL )
	{
	numer++; numer >>= 1;
	denom++; denom >>= 1;
	}

if ( !denom )
	return(0);

val = ((unsigned long)numer << 16) / denom; /* always round it down */

if ( flag )
	return( -val );
else
	return( val );
}


/***********************************************************************/
long fmul( long a, LFIXED b )
/***********************************************************************/
{
DWORD t;
BYTE negative;

if ( negative = (a<0) )
	a = -a;

if ( b<0 )
	{
	b = -b;
	negative ^= 1;
	}

t = ( (ULONG)LOWORD(a) * LOWORD(b) ) + HALF;
t = HIWORD(t);

if ( HIWORD(a) )
	{
	t += ( (ULONG)HIWORD(a) * HIWORD(b) ) << 16;
	t += ( (ULONG)HIWORD(a) * LOWORD(b) );
	}

if ( HIWORD(b) )
	t += ( (ULONG)LOWORD(a) * HIWORD(b) );

if ( negative )
	return( -((long)t) );
else
	return( t );
}

//***********************************************************************
int latoi( LPSTR p )
//***********************************************************************
{
BYTE ch;
int len;
WORD mult, result;
BOOL bNeg;
LPSTR p1;

if (*p == '-')
	{
	bNeg = TRUE;
	p++;
	}
else
	bNeg = FALSE;

p1 = p;
len = 0;
while (((ch = *p1) >= '0') && ch <= '9')
	{
	len++;
	p1++;
	if ( len == 1 )
		mult = 1;
	else
		mult *= 10;
	}

result = 0;
while ( --len >= 0 )
	{
	result += ((*p++ - '0') * mult);
	mult /= 10;
	}

return (bNeg ? -(int)result : result);
}

//***********************************************************************
long latol( LPSTR p, LPINT lpError )
//***********************************************************************
{
BYTE ch;
int len;
DWORD mult, result;
BOOL bNeg;
LPSTR p1;

if ( !p )
	{
	if ( lpError )
		*lpError = YES;
	return( 0 );
	}

if ( *p == '-' )
	{
	bNeg = TRUE;
	p++;
	}
else
	bNeg = FALSE;

p1 = p;
len = 0;
while ( ((ch = *p1) >= '0') && ch <= '9' )
	{
	len++;
	p1++;
	if ( len == 1 )
		mult = 1;
	else
		mult *= 10;
	}

result = 0;
while ( --len >= 0 )
	{
	result += ((*p++ - '0') * mult);
	mult /= 10;
	}

if ( lpError ) // did it break out on a NULL character?
	*lpError = ( ch == '\0' ? NO : YES );
return( bNeg ? -(long)result : result );
}

//***********************************************************************
HINSTANCE PHLoadLibrary( LPCTSTR lpszLibFile )
//***********************************************************************
{
	HINSTANCE hInst;

	#ifdef WIN32
	if ( (hInst = LoadLibrary(lpszLibFile)) == NULL)
		return( NULL );
	#else
	if ( (hInst = LoadLibrary(lpszLibFile)) <= HINSTANCE_ERROR)
		return( NULL );
	#endif
	return(hInst);
}

//************************************************************************
LPSTR FindString( LPSTR lpString, LPSTR lpSearchString )
//************************************************************************
{
LPSTR lp, lpStart, lpSearch;
char c, c1;

if ( !(lp = lpString) )
	return( NULL );
if ( !(lpSearch = lpSearchString) )
	return( NULL );

lpStart = NULL;
while ( c = *lp )
	{
	if ( c >= 'a' && c <= 'z' ) // Convert to upper case
		c -= ('a'-'A');
	c1 = *lpSearch;
	if ( c1 >= 'a' && c1 <= 'z' ) // Convert to upper case
		c1 -= ('a'-'A');
	if ( c == c1 )
		{ // If a character match...
		lpSearch++;
		if ( !lpStart )
			lpStart = lp;
		}
	else	{ // If not a character match...
		lpSearch = lpSearchString;
		lpStart = NULL;
		}
	if ( !*lpSearch ) // If at the end of the search string...
		break;
	lp++;
	}

return( lpStart );
}


//***********************************************************************
LPLONG ExtractSwitches( LPSTR lpString )
//***********************************************************************
{
BYTE c, cLastSwitch;
DWORD dwSwitches;
BOOL bSwitchPending, bValuePending;
LPLONG lpSwitches;

if ( !(lpSwitches = (LPLONG)AllocX( 26 * sizeof(long), GMEM_ZEROINIT )) )
	return( NULL );

dwSwitches = 0;
cLastSwitch = 26;
bSwitchPending = bValuePending = NO;

while ( c = *lpString )
	{
	if ( c == '/' )
		{
		bSwitchPending = YES;
		*lpString++ = '\0'; // Pass back the string without the switches
		}
	else
	if ( c == '=' )
		{
		bValuePending = YES;
		*lpString++ = '\0'; // Pass back the string without the switches
		}
	else
		{
		if ( bSwitchPending )
			{
			if ( c >= 'a' && c <= 'z' ) c -= 'a';
			else
			if ( c >= 'A' && c <= 'Z' ) c -= 'A';
			else						c = 26;
			if ( c < 26 )
				{
				cLastSwitch = c;
				lpSwitches[cLastSwitch] = 1L;
				}
			bSwitchPending = NO;
			}
		else
		if ( bValuePending )
			{
			if ( cLastSwitch < 26 )
				lpSwitches[cLastSwitch] = latol( lpString );
			bValuePending = NO;
			}

		lpString++;
		}
	}

// Pass back the switches, and the string with the switches removed
return( lpSwitches );
}


//***********************************************************************
long GetSwitchValue( BYTE c, LPLONG lpSwitches )
//***********************************************************************
{
if ( !lpSwitches )
	return( 0 );
if ( c >= 'a' && c <= 'z' ) return( lpSwitches[c-'a'] );
if ( c >= 'A' && c <= 'Z' ) return( lpSwitches[c-'A'] );
return( 0 );
}

//***********************************************************************
BOOL GetStringParm(LPCTSTR lpString, int nIndex, BYTE cSeparator, LPTSTR lpReturn)
//***********************************************************************
{
	BOOL fRet = FALSE;
	int i = 0;

	while (i < nIndex && *lpString)
	{
		if (*lpString == cSeparator)
			++i;
		++lpString;
	}
	while (*lpString)
	{
		if (*lpString == cSeparator)
			break;
		if ( *lpString > ' ' )
		{
			*lpReturn++ = *lpString;
			fRet = TRUE;
		}
		++lpString;
	}
	*lpReturn = '\0';
	return(fRet);
}

//************************************************************************
void ShowDlgItem( HWND hWnd, ITEMID idItem, BOOL bShow)
//************************************************************************
{
	HWND hControl = GetDlgItem(hWnd, idItem);
	if (hControl)
	{
		ShowWindow(hControl, bShow ? SW_SHOW : SW_HIDE);
	}
}

//************************************************************************
void EnableDlgItem( HWND hWnd, ITEMID idItem, BOOL bEnable)
//************************************************************************
{
	HWND hControl = GetDlgItem(hWnd, idItem);
	if (hControl)
	{
		EnableWindow(hControl, bEnable);
	}
}

//************************************************************************
void ScaleRect( LPRECT lpRect, int iScale )
//************************************************************************
{
	if ( !lpRect || !iScale )
		return;

	if ( iScale < 0 )
	{
		lpRect->left   /= -iScale;
		lpRect->top	   /= -iScale;
		lpRect->right  /= -iScale;
		lpRect->bottom /= -iScale;
	}
	else
	{
		lpRect->left   *= iScale;
		lpRect->top	   *= iScale;
		lpRect->right  *= iScale;
		lpRect->bottom *= iScale;
	}
}

//************************************************************************
PDIB TextToDib (LPSTR lpString, LPSTR lpFaceName, int iPtSize, BOOL bItalic, COLORREF TextColor, HPALETTE hPal)
//************************************************************************
{
	HDC hWinGDC;
	if ( !(hWinGDC = WinGCreateDC()) )
		return(NULL);

	//	Get WinG to recommend the fastest DIB format
	//	Orientation = ( m_pdibStage->m_bmiHeader.biHeight < 0 ? -1 : 1 ); 
	CDib dib;
	if ( !WinGRecommendDIBFormat( dib.GetInfo() ) )
	{ // set it up ourselves
		dib.SetPlanes(1);
		dib.SetSizeImage(0);
		dib.SetColorsUsed(0);
		dib.SetColorsImportant(0);
		dib.SetWidth(1);
		dib.SetHeight(1);
	}

	// make sure it's 8bpp
	dib.SetBitCount(8);
	dib.SetCompression(BI_RGB);
	dib.FixHeader();

	// Create the font to use
	LOGFONT lfFont;
	memset(&lfFont, 0, sizeof(lfFont));
	lstrcpy(lfFont.lfFaceName, lpFaceName);
	lfFont.lfHeight = - iPtSize;
	lfFont.lfWeight = FW_NORMAL;
	lfFont.lfItalic = bItalic;
	lfFont.lfUnderline = 0;

	HFONT hFont;
	if ( !(hFont = CreateFontIndirect(&lfFont)) )
	{
		DeleteDC(hWinGDC);
		return (NULL);
	}

	HFONT hOldFont = (HFONT)SelectObject(hWinGDC, hFont);
	SetTextColor(hWinGDC, TextColor);

	RECT rText;
	SetRect(&rText, 0, 0, 0, 0);
	// Let DrawText figure out the min. size of the rectangle for the text
	int iActualHeight = DrawText (hWinGDC, lpString, lstrlen (lpString), & rText,
								DT_CALCRECT | DT_NOPREFIX);

	// Increase the rectangle size for a margin
	rText.right += 10;
	rText.bottom += 10;
	int iWidth = rText.right - rText.left;
	int iHeight = rText.bottom - rText.top;

	// Create a new 8-bit WinGBitmap with the new size
	// while maintaining the same y orientation
	int Orientation = ( dib.GetHeight() < 0 ? -1 : 1 ); 
	dib.SetHeight(iHeight * Orientation);
	dib.SetWidth(iWidth);
	dib.SetSizeImage(0);
	dib.FixHeader();

	PALETTEENTRY pe[256];
	int nEntries = GetPaletteEntries(hPal, 0, 256, pe);
	LPRGBQUAD lpRGB = dib.GetColors();
	for (int i = 0; i < nEntries; ++i)
	{
		lpRGB[i].rgbRed = pe[i].peRed;
		lpRGB[i].rgbGreen = pe[i].peGreen;
		lpRGB[i].rgbBlue = pe[i].peBlue;
		lpRGB[i].rgbReserved = 0;
	}
	dib.SetColorsUsed(nEntries);
 
	// Create a WinG Bitmap, store the data pointer, then select away
	HBITMAP hbmWinG;
	LPTR lp;
	if ( !(hbmWinG = WinGCreateBitmap( hWinGDC, (LPBITMAPINFO)dib.GetInfo(), (LPPVOID)&lp )) )
	{
		DeleteDC(hWinGDC);
		DeleteObject(hFont);
		return(NULL);
	}
	dib.SetPtr(lp);

	// Select it in and delete the old one
	HBITMAP hbmOld = (HBITMAP)SelectObject( hWinGDC, hbmWinG );

	FillRect(hWinGDC, &rText, (HBRUSH)GetStockObject(WHITE_BRUSH));
	// Render the text into the bitmap
	DrawText(hWinGDC, lpString, lstrlen (lpString), & rText,
				DT_CENTER | DT_VCENTER | DT_NOPREFIX);

	// Housecleaning
	SelectObject(hWinGDC, hbmOld);
	SelectObject(hWinGDC, hOldFont);
	DeleteObject(hFont);

	// create a dib to return
	PDIB pDib;
	LPTR lpBits;
	if ( lpBits = Alloc(dib.GetSizeImage()) )
		pDib = new CDib(&dib, lpBits, TRUE);
	dib.SetPtr(NULL);

	DeleteObject(hbmWinG);
	DeleteDC(hWinGDC);
	return pDib;
}

// Indicates whether Win32s version 1.1 is installed
//************************************************************************
BOOL FAR PASCAL IsWin32sLoaded( void )
//************************************************************************
{
HINSTANCE hWin32sys;
typedef struct {
    BYTE bMajor;
    BYTE bMinor;
    WORD wBuildNumber;
    BOOL fDebug;
} WIN32SINFO, far * LPWIN32SINFO;
WIN32SINFO Info;
typedef BOOL (CALLBACK* LPFNWIN32SINFO)(LPWIN32SINFO);
LPFNWIN32SINFO pfnInfo;

if ( !(hWin32sys = PHLoadLibrary( "W32SYS.DLL" )) )
	return( (int)hWin32sys );

if ( !(pfnInfo = (LPFNWIN32SINFO)GetProcAddress(hWin32sys, "GETWIN32SINFO")) )
{
	FreeLibrary( hWin32sys );
	return( FALSE );
}

Info.bMajor = Info.bMinor = 0;
Info.wBuildNumber = 0;
Info.fDebug = 0;
if( (*pfnInfo)((LPWIN32SINFO) &Info) )
{
	FreeLibrary( hWin32sys );
	return( FALSE );
}

FreeLibrary( hWin32sys );
return( Info.bMajor == 1 && Info.bMinor == 1 );
}

//************************************************************************
void HourGlass( BOOL bHourGlass )
//************************************************************************
{
	static HCURSOR hCursor;
	static WORD wWaiting;
	
	if ( bHourGlass )
	{
		if ( !wWaiting )
		{
			hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
			ShowCursor( TRUE );
		}
		wWaiting++;
	}

	else
	{
		if ( !--wWaiting )
		{
			ShowCursor( FALSE );
			SetCursor( hCursor );
		}
	}
}

//************************************************************************
void FlushMouseMessages( void )
//************************************************************************
{
	MSG msg, lastMsg;

	lastMsg.message = 0;
	while ( PeekMessage( &msg, GetApp()->GetMainWnd(), WM_LBUTTONDOWN, WM_MOUSELAST, PM_REMOVE ) )
	{
		if ( msg.message == WM_LBUTTONUP )
			lastMsg = msg; // preserve one WM_LBUTTONUP message
	}
	if ( lastMsg.message )
		PostMessage( lastMsg.hwnd, lastMsg.message, lastMsg.wParam, lastMsg.lParam );
}

#ifdef UNUSED
/***********************************************************************/
BOOL RegKeyValue( DWORD hRootKey, LPSTR lpSubKey, LPSTR lpKey, LPSTR lpValue )
/***********************************************************************/
{
	if ( !lpSubKey )
		return( NO );
	
	#ifdef WIN32
		DWORD hKey;
		RegCreateKey( hRootKey, lpSubKey, &hKey );
		return( !RegSetValueEx( hKey, lpKey, NULL, REG_SZ, (CONST BYTE*)lpValue, lstrlen(lpValue)+1 ) );
	#else
		STRING sz;
		lstrcpy( szSubKey, lpSubKey );
		if ( lpKey )
			lstrcat( szSubKey, lpKey );
		return( !RegSetValue( hRootKey, szSubKey, REG_SZ, lpValue, lstrlen(lpValue)+1 ) );
	#endif
}


/***********************************************************************/
BOOL RegKeyExists( DWORD hRootKey, LPSTR lpSubKey )
/***********************************************************************/
{
	if ( !lpSubKey )
		return( NO );
	
	STRING szValue;
	long lLength = sizeof(szValue);
	return( !RegQueryValue( hRootKey, lpSubKey, szValue, &lLength ) );
}
#endif

// Special purpose timer for video draw proc.  Someday we could
// make this more generic by having a separate set of calls
// to do the timeBeginPeriod() and timeEndPeriod().
// Also we could bag all the globals, except for lpTimerProc.
#define TIMER_RESOLUTION 33 // 1/30 of a second

static FARPROC lpTimerProc = NULL;
static UINT uTimerRes;
static UINT uTimerEvent;

/***********************************************************************/
void CALLBACK TimerProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
/***********************************************************************/
{
	HWND hWnd = (HWND)dwUser;
	FORWARD_WM_TIMER(hWnd, uID, PostMessage);
}

/***********************************************************************/
UINT SetMMTimer( HWND hWnd, UINT uTimeout )
/***********************************************************************/
{
	TIMECAPS tc;

	uTimerEvent = TIME_PERIODIC;
	uTimerRes = uTimeout/*TIMER_RESOLUTION*/;
	if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) == TIMERR_NOERROR)
		uTimerRes = min(max(tc.wPeriodMin, uTimerRes), tc.wPeriodMax);
	timeBeginPeriod( uTimerRes );

	if (!lpTimerProc)
		lpTimerProc = MakeProcInstance((FARPROC)TimerProc, GetApp()->GetInstance());
	UINT idNewTimer = timeSetEvent( uTimeout, uTimerRes, (LPTIMECALLBACK)lpTimerProc, (DWORD)(hWnd), uTimerEvent );

	return(idNewTimer);
}

/***********************************************************************/
BOOL KillMMTimer( HWND hWnd, UINT idTimer )
/***********************************************************************/
{
	BOOL bError = TRUE;
	if (uTimerEvent == TIME_PERIODIC)
		bError = timeKillEvent( idTimer );
	timeEndPeriod( uTimerRes );
	return( bError != TIMERR_NOERROR );
}

// < 0	lpFile1 earlier date than lpFile2
// = 0	lpFile1 same date lpFile2
// > 0	lpFile1 later date than lpFile2
/***********************************************************************/
BOOL CompareFileTimes( LPCTSTR lpFile1, LPCTSTR lpFile2, LPINT lpResult )
/***********************************************************************/
{
	FNAME szFile1, szFile2;
	struct _stat stat1, stat2;

	lstrcpy(szFile1, lpFile1);
	if (_stat(szFile1, &stat1))
		return(FALSE);
	lstrcpy(szFile2, lpFile2);
	if (_stat(szFile2, &stat2))
		return(FALSE);
	if (stat1.st_mtime == stat2.st_mtime)
		*lpResult = 0;
	else
	if (stat1.st_mtime < stat2.st_mtime)
		*lpResult = -1;
	else
		*lpResult = 1;
	return(TRUE);
}

/************************************************************************/
BOOL FileDelete( LPCTSTR lpFileName )
/************************************************************************/
{
#ifdef WIN32
	return(DeleteFile(lpFileName));
#else
	FNAME szFile;

	lstrcpy( szFile, lpFileName );
	return (remove(szFile) == 0);
#endif
}

