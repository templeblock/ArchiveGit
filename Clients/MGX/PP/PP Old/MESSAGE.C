// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

extern HANDLE hInstAstral;
extern HWND hWndAstral;
extern char szAppName[MAX_STR_LEN];

/***********************************************************************/
int AstralStr( idString, lppString )
/***********************************************************************/
int	idString;
LPPTR	lppString;
{
static LPTR lpBuffer;
#define STRING_SIZE 256

if ( !lpBuffer )
    if ( !(lpBuffer = Alloc((long)STRING_SIZE)) )
	{ // Should never happen
	*lppString = (LPTR)&lpBuffer;
	return( 0 );
	}

/* Pass back a pointer to our allocated string */
*lppString = lpBuffer;
*lpBuffer = '\0';

/* Get the string from the string table that matches the passed ID */
/* Use this routine when you only need a string for short period of time */
if ( !LoadString( hInstAstral, idString, lpBuffer, STRING_SIZE-1 ) )
	return( 0 );

/* Pass back the string length */
return( lstrlen(lpBuffer) );
}

/***********************************************************************/
void dbg( lpFormat, ... )
/***********************************************************************/
LPTR lpFormat;
{
LPTR lpArguments;

lpArguments = (LPTR)&lpFormat + sizeof(lpFormat);
dbgv(lpFormat, lpArguments);
}

/***********************************************************************/
void dbgv( lpFormat, lpArguments )
/***********************************************************************/
LPTR lpFormat;
LPTR lpArguments;
{
char szBuffer[1024], name[128];
static int hFile = 0;

if (lpFormat)
    {
    if (!hFile)
        {
     	lstrcpy(name, Control.ProgHome);
    	lstrcat(name, "DEBUG.TXT");
    	hFile = _lcreat(name, 0);
    	if (hFile < 0)
	    hFile = 0;
    	}
    if (hFile)
        {
    	wvsprintf( szBuffer, lpFormat, lpArguments );
    	lstrcat(szBuffer, "\r\n");
	_lwrite(hFile, szBuffer, strlen(szBuffer));
 	}
    }
else if (hFile)
    {
    lstrcpy(szBuffer, "---------- CLOSED ----------\r\n");
    _lwrite(hFile, szBuffer, strlen(szBuffer));
    _lclose(hFile);
    lstrcpy(name, Control.ProgHome);
    lstrcat(name, "DEBUG.TXT");
    hFile = _lopen(name, OF_WRITE);
    if (hFile < 0)
	hFile = 0;
    else
        _llseek(hFile, 0L, 2);
    }
}

/***********************************************************************/
int Print( lpFormat, ... )
/***********************************************************************/
LPTR lpFormat;
{
char szBuffer[1024];
LPTR lpArguments;
int retc;
HWND hFocusWindow;

/* Put up a modal message box with arguments specified exactly like printf */
if ( Control.InPicwin )
	return( IDCANCEL );
lpArguments = (LPTR)&lpFormat + sizeof(lpFormat);
wvsprintf( szBuffer, lpFormat, lpArguments );
hFocusWindow = GetFocus();
retc = MessageBox( hWndAstral, szBuffer, szAppName, MB_OK | MB_TASKMODAL );
SetFocus( hFocusWindow );
return( retc );
}

/***********************************************************************/
int Message( idStr, ... )
/***********************************************************************/
int idStr;
{
char szBuffer[1024];
LPTR lpArguments;
LPTR lpString;
int retc;
HWND hFocusWindow;

/* Put up a modal message box with arguments specified exactly like printf */
/* The caller passes stringtable ID's, and not hardcoded strings */
/* Used to display error message and warnings */
if ( Control.InPicwin )
	return( IDCANCEL );
if ( !AstralStr( idStr, &lpString ) )
	return( IDCANCEL );
lpArguments = (LPTR)&idStr + sizeof(idStr);
wvsprintf( szBuffer, lpString, lpArguments );
hFocusWindow = GetFocus();
retc = MessageBox( hWndAstral, szBuffer, szAppName,
	MB_ICONEXCLAMATION | MB_OK | MB_TASKMODAL );
SetFocus( hFocusWindow );
return( retc );
}


/***********************************************************************/
void PrintStatus( lpFormat, ... )
/***********************************************************************/
LPTR lpFormat;
{
char szBuffer[1024];
LPTR lpArguments;

/* Put up a modal message box with arguments specified exactly like printf */
if ( Control.InPicwin )
	return;
lpArguments = (LPTR)&lpFormat + sizeof(lpFormat);
wvsprintf( szBuffer, lpFormat, lpArguments );
Status( szBuffer );
}


/***********************************************************************/
void MessageStatus( idStr, ... )
/***********************************************************************/
int idStr;
{
char szBuffer[1024];
LPTR lpArguments;
LPTR lpString;

/* Paint the status bar with a string; arguments specified exactly like printf*/
/* The caller passes stringtable ID's, and not hardcoded strings */
/* Used to display error message and warnings */
if ( Control.InPicwin )
	return;
if ( AstralStr( idStr, &lpString ) )
	{
	lpArguments = (LPTR)&idStr + sizeof(idStr);
	wvsprintf( szBuffer, lpString, lpArguments );
	}
else	wsprintf( szBuffer, "Ready %d", idStr );
Status( szBuffer );
}


/***********************************************************************/
void ColorStatus( lpRGB, lCount )
/***********************************************************************/
LPRGB lpRGB;
long lCount;
{
RGBS rgb;
BOOL bIsGray;

rgb = *lpRGB;
rgb.red   = TOPERCENT( rgb.red );
rgb.green = TOPERCENT( rgb.green );
rgb.blue  = TOPERCENT( rgb.blue );
bIsGray = ( rgb.red == rgb.blue && rgb.blue == rgb.green );
if ( lCount )
	{
	if ( bIsGray )
		MessageStatus( IDS_COLORGRAYCOUNT, lCount, 100-rgb.red );
	else	MessageStatus( IDS_COLORRGBCOUNT, lCount,
			rgb.red, rgb.green, rgb.blue );
	}
else	{
	if ( bIsGray )
		MessageStatus( IDS_COLORGRAY, 100-rgb.red );
	else	MessageStatus( IDS_COLORRGB, rgb.red, rgb.green, rgb.blue );
	}
}


/***********************************************************************/
void StatusOfRectangle( lpRect )
/***********************************************************************/
LPRECT lpRect;
{
STRING szString1, szString2, szString3, szString4;
RECT rFile;

rFile = *lpRect;
if (lpImage)
	{
	Display2File(&rFile.left, &rFile.top);
	Display2File(&rFile.right, &rFile.bottom);
	}
	
PrintStatus( "%ls x %ls inches at (%ls,%ls)",
	Unit2String( Pixels2Inches( RectWidth(&rFile) ),  szString1 ),
	Unit2String( Pixels2Inches( RectHeight(&rFile) ), szString2 ),
	Unit2String( Pixels2Inches( rFile.top ),        szString3 ),
	Unit2String( Pixels2Inches( rFile.left ),       szString4 ) );
}


/***********************************************************************/
void Status( lpString )
/***********************************************************************/
LPTR lpString;
{
int len;
HDC hDC;
HWND hWnd;

if ( StringsEqual( Control.Status, lpString ) )
	return;
if ( !(hWnd = AstralDlgGet( IDD_STATUS )) )
	return;
if ( !(hWnd = GetDlgItem( hWnd, IDC_STATUSTEXT )) )
	return;
if ( !(hDC = GetDC( hWnd )) )
	return;
SelectObject( hDC, Window.hHelv10Font );
if ( Control.Status[0] ) // If the last status string is not null, undraw it
	ColorText( hDC, 10, 2, Control.Status, lstrlen(Control.Status),
		GetSysColor(COLOR_BTNFACE) );
if ( (len = lstrlen(lpString)) >= sizeof(Control.Status) )
	{
	len = sizeof(Control.Status) - 1;
	*(lpString + len) = '\0';
	}
lstrcpy( Control.Status, lpString ); // Copy in the new string
ColorText( hDC, 10, 2, Control.Status, len, GetSysColor(COLOR_WINDOWTEXT) );
ReleaseDC( hWnd, hDC );
}


/***********************************************************************/
int AstralConfirm( idStr, ... )
/***********************************************************************/
int	idStr;
{
char szBuffer[1024];
LPTR lpArguments;
LPTR lpString;
int retc;
HWND hFocusWindow;

/* Put up a modal message box with arguments specified exactly like printf */
/* The caller passes stringtable ID's, and not hardcoded strings */
/* Used to confirm any user operation */
if ( Control.InPicwin )
	return( IDCANCEL );
if ( !AstralStr( idStr, &lpString ) )
	return( IDCANCEL );
lpArguments = (LPTR)&idStr + sizeof(idStr);
wvsprintf( szBuffer, lpString, lpArguments );
hFocusWindow = GetFocus();
retc = MessageBox( hWndAstral, szBuffer, szAppName,
	MB_ICONQUESTION | MB_YESNOCANCEL | MB_TASKMODAL );
SetFocus( hFocusWindow );
return( retc );
}


/***********************************************************************/
int AstralOKCancel( idStr, ... )
/***********************************************************************/
int	idStr;
{
char szBuffer[1024];
LPTR lpArguments;
LPTR lpString;
int retc;
HWND hFocusWindow;

/* Put up a modal message box with arguments specified exactly like printf */
/* The caller passes stringtable ID's, and not hardcoded strings */
/* Used to confirm any user operation */
if ( Control.InPicwin )
	return( IDCANCEL );
if ( !AstralStr( idStr, &lpString ) )
	return( IDCANCEL );
lpArguments = (LPTR)&idStr + sizeof(idStr);
wvsprintf( szBuffer, lpString, lpArguments );
hFocusWindow = GetFocus();
retc = MessageBox( hWndAstral, szBuffer, szAppName,
	MB_ICONQUESTION | MB_OKCANCEL | MB_TASKMODAL );
SetFocus( hFocusWindow );
return( retc );
}

/***********************************************************************/
void StatusBox(idStr, ... )
/***********************************************************************/
int	idStr;
{
char szBuffer[1024];
LPTR lpArguments;
LPTR lpString;
LPTR lpDlgResource;
static HWND hDlg;

if (hDlg)
    {
    DestroyWindow(hDlg);
    hDlg = NULL;
    return;
    }
if ( !AstralStr( idStr, &lpString ) )
	return;
lpArguments = (LPTR)&idStr + sizeof(idStr);
wvsprintf( szBuffer, lpString, lpArguments );

lpDlgResource = MAKEINTRESOURCE(IDD_STATUSBOX);
hDlg = CreateDialog(hInstAstral, lpDlgResource, hWndAstral, DlgStatusBoxProc);
if (hDlg)
    SetDlgItemText(hDlg, IDC_STATUSTEXT, (LPSTR)szBuffer);
}
