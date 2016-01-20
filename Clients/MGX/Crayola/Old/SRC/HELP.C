//®PL1¯®FD1¯®BT0¯®TP0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

extern HWND hWndAstral;
extern HINSTANCE hInstAstral;

static int idItemHelp;
static HOOKPROC lpKBHook;
static HHOOK hKBHook, hMsgHook;
static BOOL fKBHook, fMsgHook;
static LPSTR GetColorString(LPCOLORINFO lpColor, COLORSPACE ColorSpace,
					LPSTR lpColorString);

/***********************************************************************/
void Help(
/***********************************************************************/
int 	idTopic,
LPARAM 	lParam)
{
FNAME szFileName;
LPSTR lpFileName;

if ( !AstralStr( IDS_HELPFILE, &lpFileName ) )
	return;

lstrcpy( szFileName, Control.ProgHome );
lstrcat( szFileName, lpFileName );
WinHelp( hWndAstral, szFileName, idTopic, lParam );
}


/***********************************************************************/
int HintLine( int id )
/***********************************************************************/
{
	return idItemHelp = MessageStatusEx (id,(LPSTR)"",(LPSTR)"");
}

/***********************************************************************/
int HintLineSpecial( int id, int idOffset )
/***********************************************************************/
{
MessageStatus( id+idOffset, (LPSTR)"", (LPSTR)"" );
return( idItemHelp = id );
}

/***********************************************************************/
void ColorStatus(
/***********************************************************************/
LPCOLORINFO lpColor,
long 	lCount,
LPSTR 	lpLabel,
BOOL 	bDoubleClick)
{
STRING szLabel, szColor;
LPSTR   lp;

if (lCount)
	{
	GetColorString(lpColor, lpColor->ColorSpace, szColor);
	MessageStatus( IDS_COLORCOUNT, lCount, (LPSTR)szColor);
	}
else
	{
	if (lpLabel && lstrlen(lpLabel) && AstralStr(IDS_COLORLABEL, &lp))
		wsprintf(szLabel, lp, lpLabel);
	else
		szLabel[0] = '\0';
	GetColorString(lpColor, lpColor->ColorSpace, szColor);
	MessageStatus(bDoubleClick ? IDS_COLOR : IDS_COLOR2 ,
				(LPSTR)szLabel, (LPSTR)szColor);
	}
}

/***********************************************************************/
int Color2Status(
/***********************************************************************/
LPCOLORINFO lpColor,
BOOL fActive)
{
STRING szColor;

GetColorString(lpColor, lpColor->ColorSpace, szColor);
MessageStatus( fActive ? IDS_ACTIVECOLOR : IDS_ALTERNATECOLOR, (LPSTR)szColor);

return( idItemHelp = IDC_ACTIVECOLOR );
}

/************************************************************************/
void StartF1Help()
/************************************************************************/
{
#ifdef WIN32
DWORD process = 0;
#else
HTASK process = GetCurrentTask();
#endif

if ( fKBHook )
	return;
fKBHook = YES;
lpKBHook = (HOOKPROC)MakeProcInstance( (FARPROC)KeyboardHookProc, hInstAstral );
hKBHook = SetWindowsHookEx( WH_KEYBOARD, lpKBHook, hInstAstral, process);
}

/************************************************************************/
void EndF1Help()
/************************************************************************/
{
if ( !fKBHook )
	return;
fKBHook = NO;
UnhookWindowsHookEx( hKBHook );
FreeProcInstance( (FARPROC)lpKBHook );
}


static BOOL bHelpDisabled;
/************************************************************************/
void EnableF1Help( BOOL Bool )
/************************************************************************/
{
bHelpDisabled = !Bool;
}


/***********************************************************************/
DWORD CALLBACK EXPORT KeyboardHookProc(
/***********************************************************************/
/*  Process the F1 key and ask for context sensitive help on the last id to receive a hint */
int 	nCode,
WPARAM 	wParam,
LPARAM 	lParam)
{
HWND hWnd;
int id;
#define STATE_MASK 0xC0000000L
#define UP_GOING_DOWN 0x00000000L
#define DOWN_GOING_UP 0xC0000000L

if ( nCode < 0 ||
     bHelpDisabled ||
     wParam != VK_F1 ||
     (lParam & STATE_MASK) != UP_GOING_DOWN )
	return( CallNextHookEx( hKBHook, nCode, wParam, lParam ) );

if ( LBUTTON )
	PostMessage ( hWndAstral, WM_KEYDOWN, VK_ESCAPE, 0L );

if ( idItemHelp <= 0 )
	{ // No help topic 
	if ( hWnd = GetLastActivePopup( hWndAstral ) )
		{ // Found a modal dialog up
		if ( !(id = GET_WINDOW_ID( hWnd )) )
			if ( !(id = AstralDlgGetID( hWnd )) );
				id = AstralDlgGetID( NULL );
		idItemHelp = MessageStatus( id, (LPSTR)"", (LPSTR)"" );
		}
	}

if ( idItemHelp > 0 )
		Help( HELP_CONTEXT, (long)idItemHelp );
else	Help( HELP_CONTEXT, (long)Tool.id );

return( TRUE );
}


/***********************************************************************/
DWORD CALLBACK EXPORT MessageHookProc(
/***********************************************************************/
/*  Process messages while in context sensitive help mode */
int 	nCode,
WPARAM 	wParam,
LPMSG 	lpMsg)
{
BOOL bShift;

if ( nCode < 0 )
	return( CallNextHookEx( hMsgHook, nCode, wParam, (LPARAM)lpMsg ) );

bShift = SHIFT;
if ( lpMsg->message == WM_COMMAND && !bShift )
	return( TRUE ); // ignore it
if ( lpMsg->message == WM_LBUTTONDOWN && !bShift )
	return( TRUE ); // ignore it
if ( lpMsg->message == WM_LBUTTONUP || 
   ( lpMsg->message == WM_KEYUP && lpMsg->wParam == VK_RETURN ) )
	{
	if ( idItemHelp > 0 )
		Help( HELP_CONTEXT, (long)idItemHelp );
	return( TRUE );
	}

return( CallNextHookEx( hMsgHook, nCode, wParam, (LPARAM)lpMsg ) );
}

/***********************************************************************/
static LPSTR GetColorString(LPCOLORINFO lpColor, COLORSPACE ColorSpace,
					LPSTR lpColorString)
/***********************************************************************/
{
STRING szFormat, szPercent;
LPSTR lpString;

*lpColorString = 0;
if (!Control.DisplayPixelValue && AstralStr(IDS_PERCENT, &lpString))
	lstrcpy(szPercent, lpString);
else
	szPercent[0] = '\0';
switch (ColorSpace)
	{
	case CS_GRAY:
	if (!AstralStr(IDS_GRAYVALUE, &lpString))
		break;
	lstrcpy(szFormat, lpString);
	wsprintf(lpColorString, szFormat,
		GRAY2DISP(lpColor->gray),
		(LPSTR)szPercent);
	break;

	case CS_RGB:
	if (!AstralStr(IDS_RGBVALUE, &lpString))
		break;
	lstrcpy(szFormat, lpString);
	wsprintf(lpColorString, szFormat,
		PIX2DISP(lpColor->rgb.red),
		PIX2DISP(lpColor->rgb.green),
		PIX2DISP(lpColor->rgb.blue),
		(LPSTR)szPercent);
	break;

	case CS_CMYK:
	if (!AstralStr(IDS_CMYKVALUE, &lpString))
		break;
	lstrcpy(szFormat, lpString);
	wsprintf(lpColorString, szFormat,
		PIX2DISP(lpColor->cmyk.c),
		PIX2DISP(lpColor->cmyk.m),
		PIX2DISP(lpColor->cmyk.y),
		PIX2DISP(lpColor->cmyk.k),
		(LPSTR)szPercent);
	break;

	case CS_HSL:
	if (!AstralStr(IDS_HSLVALUE, &lpString))
		break;
	lstrcpy(szFormat, lpString);
	wsprintf(lpColorString, szFormat,
		TOANGLE(lpColor->hsl.hue),
		TOPERCENT(lpColor->hsl.sat),
		TOPERCENT(lpColor->hsl.lum));
	break;

	default:
	break;
	}
return(lpColorString);
}
