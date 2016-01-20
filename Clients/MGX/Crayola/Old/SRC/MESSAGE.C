//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

extern BOOL fAppActive;
extern HINSTANCE hInstAstral;
extern HWND hWndAstral;
extern STRING szAppName;

static BOOL bMessageEnable = YES;

/***********************************************************************/
void FrameError(int idMsg)
/***********************************************************************/
{
int err;
LPSTR lpFormat;
char szBuffer[512];

err = FrameGetError();
switch (err)
	{
	case ERR_MAXIMAGEHEIGHT:
		if (!AstralStr(IDS_EMAXIMAGEHEIGHT, &lpFormat))
			return;
		lstrcpy(szBuffer, lpFormat);
	break;		
	case ERR_MAXIMAGEWIDTH:
		if (!AstralStr(IDS_EMAXIMAGEWIDTH, &lpFormat))
			return;
		lstrcpy(szBuffer, lpFormat);
	break;
	case ERR_IMAGEOVERHEAD:
		if (!AstralStr(IDS_EIMAGEOVERHEAD, &lpFormat))
			return;
		lstrcpy(szBuffer, lpFormat);
	break;
	case ERR_MALLOC:
		if (!AstralStr(IDS_EMALLOC, &lpFormat))
			return;
		lstrcpy(szBuffer, lpFormat);
	break;
	case ERR_LINEPOOLALLOC:
		if (!AstralStr(IDS_ELINEPOOLALLOC, &lpFormat))
			return;
		lstrcpy(szBuffer, lpFormat);
	break;
	case ERR_DISKCACHE:
		if (!AstralStr(IDS_EDISKCACHE, &lpFormat))
			return;
 	   	wsprintf( szBuffer, lpFormat, (LPSTR)Control.RamDisk,
			DiskSpace(Control.RamDisk) );
	break;
	case ERR_SCROPEN:
		if (!AstralStr(IDS_ESCROPEN, &lpFormat))
			return;
 	   	wsprintf( szBuffer, lpFormat, (LPSTR)Control.RamDisk );
	break;
	case ERR_SCRWRITE:
		if (!AstralStr(IDS_ESCRWRITE, &lpFormat))
			return;
 	   	wsprintf( szBuffer, lpFormat, (LPSTR)Control.RamDisk );
	break;
	case ERR_SCRREAD:
		if (!AstralStr(IDS_ESCRREAD, &lpFormat))
			return;
 	   	wsprintf( szBuffer, lpFormat, (LPSTR)Control.RamDisk );
	break;
	default:
		return;
	}
Message(idMsg, (LPSTR)szBuffer);
}

/***********************************************************************/
BOOL MessageEnable(BOOL bEnable)
/***********************************************************************/
{
BOOL	bOldEnable;

bOldEnable = bMessageEnable;
bMessageEnable = bEnable;
return(bOldEnable);
}


/***********************************************************************/
void dbg( LPSTR lpFormat, ... )
/***********************************************************************/
{
LPTR lpArguments;

lpArguments = (LPTR)&lpFormat + sizeof(lpFormat);
dbgv(lpFormat, lpArguments);
}

/***********************************************************************/
void dbgv(
/***********************************************************************/
LPSTR 	lpFormat,
LPTR 	lpArguments)
{
char szBuffer[512];
static int hFile, debug;
static BOOL bReopen;

if ( !debug ) // First time in...
	debug = GetDefaultInt( "debug", -1 );
if ( debug <= 0 )
	return;

if ( lpFormat )
	{
	if ( !hFile )
		{
	 	lstrcpy( szBuffer, Control.ProgHome );
		lstrcat( szBuffer, "DEBUG.TXT" );
		if ( bReopen )
			{
			if ( (hFile = _lopen( szBuffer, OF_WRITE )) < 0 )
				hFile = 0;
			else	_llseek( hFile, 0L, 2 );
			}
		else	{
			if ( (hFile = _lcreat( szBuffer, 0 )) < 0 )
				hFile = 0;
			}
		}
	if ( hFile )
		{
 	   	wvsprintf( szBuffer, lpFormat, (LPSTR)lpArguments );
    		lstrcat(szBuffer, "\r\n");
		_lwrite( hFile, szBuffer, lstrlen(szBuffer) );
		if (debug > 1)
			OutputDebugString(szBuffer);
		}
	}
else
if ( hFile )
	{
	lstrcpy( szBuffer, "---------- CLOSED ----------\r\n" );
	_lwrite( hFile, szBuffer, lstrlen(szBuffer) );
	_lclose( hFile );
	bReopen = YES;
	hFile = 0;
	}
}


/***********************************************************************/
int Print( LPSTR lpFormat, ... )
/***********************************************************************/
{
char szBuffer[512];
LPTR lpArguments;
int retc;
HWND hFocusWindow;

if (!bMessageEnable)
	return( IDCANCEL );

/* Put up a modal message box with arguments specified exactly like printf */
lpArguments = (LPTR)&lpFormat + sizeof(lpFormat);
wvsprintf( szBuffer, lpFormat, (LPSTR)lpArguments );
hFocusWindow = GetFocus();
retc = MessageBox( MessageParent(), szBuffer, szAppName, MB_OK | MB_TASKMODAL );
SetFocus( hFocusWindow );
return( retc );
}

/***********************************************************************/
int Message( int idStr, ... )
/***********************************************************************/
{
char szBuffer[512];
LPTR lpArguments;
LPSTR lpString;
int retc;
HWND hFocusWindow;

if (!bMessageEnable)
	return(IDCANCEL);
/* Put up a modal message box with arguments specified exactly like printf */
/* The caller passes stringtable ID's, and not hardcoded strings */
/* Used to display error message and warnings */
if ( !AstralStr( idStr, &lpString ) )
	return( IDCANCEL );
lpArguments = (LPTR)&idStr + sizeof(idStr);
wvsprintf( szBuffer, lpString, (LPSTR)lpArguments );
hFocusWindow = GetFocus();
retc = MessageBox( MessageParent(), szBuffer, szAppName,
	MB_ICONEXCLAMATION | MB_OK | MB_TASKMODAL );
SetFocus( hFocusWindow );
return( retc );
}


/***********************************************************************/
int MessageEx( WORD wStyle, int idStr, ... )
/***********************************************************************/
{
char szBuffer[512];
LPTR lpArguments;
LPSTR lpString;
int retc;
HWND hFocusWindow;

if (!bMessageEnable)
	return(IDCANCEL);
/* Put up a modal message box with arguments specified exactly like printf */
/* The caller passes stringtable ID's, and not hardcoded strings */
/* Used to display error message and warnings */
if ( !AstralStr( idStr, &lpString ) )
	return( IDCANCEL );
lpArguments = (LPTR)&idStr + sizeof(idStr);
wvsprintf( szBuffer, lpString, (LPSTR)lpArguments );
hFocusWindow = GetFocus();
retc = MessageBox( MessageParent(), szBuffer, szAppName,
	(wStyle ? wStyle : MB_ICONEXCLAMATION | MB_OK | MB_TASKMODAL) );
SetFocus( hFocusWindow );
return( retc );
}


/***********************************************************************/
void PrintStatus( LPSTR lpFormat, ... )
/***********************************************************************/
{
    char szBuffer[256];
    LPTR lpArguments;

// Put up a modal message box with arguments specified exactly like printf.

    lpArguments = (LPTR)&lpFormat + sizeof lpFormat;
    wvsprintf (szBuffer,lpFormat,(LPSTR)lpArguments);
    Status (0,szBuffer);
}


/***********************************************************************/
WORD MessageStatus( int idStr, ... )
/***********************************************************************/
{
    return NULL;
}

/***********************************************************************/
WORD MessageStatusEx( int idStr, ... )
/***********************************************************************/
{
    BOOL bShow = Control.Hints && fAppActive;
    char szBuffer[256];
    LPTR lpArguments;
    LPSTR lpString;

// Used to paint the hint bubble with a string.
// Arguments specified exactly like printf.
// Caller passes stringtable ID's, not hardcoded strings.

    *szBuffer = 0;
    switch (idStr)
    {
    case IDS_READY:
        idStr = 0;
        break;
    case HS_HORN:
        idStr = SoundIsOn() ? IDS_SOUND_ON : IDS_SOUND_OFF;
        bShow = fAppActive;
        break;
    case HS_TOUR:
        idStr = Control.Hints ? IDS_TOUR_GUIDE_ON : IDS_TOUR_GUIDE_OFF;
        bShow = fAppActive;
        break;
    case HS_BACKSHOW:
        idStr = (idPreviousRoom == RM_SLIDE) ? HS_BACKSHOW : HS_BACKART;
        break;
    }
	if ( AnimateIsOn() )
	{
		bShow = NO;
	}
    else if (idStr && AstralStr (idStr,&lpString))
    {
        lpArguments = (LPTR)&idStr + sizeof idStr;
        wvsprintf (szBuffer,lpString,(LPSTR)lpArguments);
    }
//SMS - this didn't work when set to Audio hints only since there is no
//way to throttle starting the sounds, they started like a machine gun....
//#ifdef CDBUILD
//	if (bShow)
//	{
//		if (Control.bBubbleHints)
//		{
//	    	if (Status (0,bShow ? szBuffer : NULL))
//			{
//				if (Control.bAudioHints)
//        			SoundStartResource ((LPSTR)MAKEINTRESOURCE(idStr), FALSE, 0);
//			}
//		}
//		else if (Control.bAudioHints)
//        	SoundStartResource ((LPSTR)MAKEINTRESOURCE(idStr), FALSE, 0);
//		else
//			Status ((HWND)-1, NULL);
//	}
//	else
//		Status ((HWND)-1, NULL);
//#else
    if (Status (0,bShow ? szBuffer : NULL))
        SoundStartResource ((LPSTR) MAKEINTRESOURCE (idStr),FALSE,0);
//#endif
    return NULL;
}


/***********************************************************************/
void StatusOfRectangle(
/***********************************************************************/
LPRECT lpRect,
LFIXED fScale)
{
STRING szUnit, szString1, szString2, szString3, szString4;
LPSTR lpUnit;
RECT rFile;
int Resolution;

OrderRect( lpRect, &rFile );
if ( fScale )
	{
	Resolution = 1000;
	ScaleRect( &rFile, fScale );
	}
else
if (lpImage)
	Resolution = FrameResolution( ImgGetBaseEditFrame(lpImage) );
if ( AstralStr( Control.Units-IDC_PREF_UNITINCHES+IDS_INCHES, &lpUnit ) )
	{
	lstrcpy( szUnit, lpUnit );
	MessageStatus( IDS_RECT_STATUS,
		Unit2String( FGET( RectWidth(&rFile), Resolution ), szString1 ),
		Unit2String( FGET( RectHeight(&rFile), Resolution ), szString2),
		(LPSTR)szUnit,
		Unit2String( FGET( rFile.left, Resolution ), szString3 ),
		Unit2String( FGET( rFile.top, Resolution ), szString4 ) );
	}
}


/***********************************************************************/
BOOL Status( HWND hWnd, LPSTR lpString )
/***********************************************************************/
{
    static HBITMAP hBubble,hMask,hSaveBits;
    BOOL bBubble = FALSE;

    if (hWnd == (HWND)-1)
    {
        // remove bubble from screen, then clean up
        Status (0,NULL);
        if (hBubble)
        {
            DeleteObject (hBubble);
            hBubble = 0;
        }
        if (hMask)
        {
            DeleteObject (hMask);
            hMask = 0;
        }
        if (hSaveBits)
        {
            DeleteObject (hSaveBits);
            hSaveBits = 0;
        }
    }
    else
    {
        static char szPrevString[256];
        static RECT rBubble,rText;
        static POINT ptBubble,ptLimit;
        HBITMAP hBitmap;
        HDC hDC,hMemDC;

        hDC = GetDC (HWND_DESKTOP);
        if (lpString && *lpString)
        {
            if (!hBubble)
            {
                BITMAP bmBubble;

                // load bubble bitmap
                hBubble = DibResource2Bitmap (hInstAstral,IDC_STATUSBUBBLE);
                GetObject (hBubble,sizeof bmBubble,(LPVOID)&bmBubble);

                // get bubble rect
                rBubble.left   = 0;
                rBubble.right  = bmBubble.bmWidth;
                rBubble.top    = 0;
                rBubble.bottom = bmBubble.bmHeight;

                // get text rect
                rText = rBubble;
                InflateRect (&rText,-50,-50);
                OffsetRect (&rText,8,0);

                // get screen limits
                ptLimit.x = ptLimit.y = 0;
                MapWindowPoints (AstralDlgGet (IDD_MAIN),HWND_DESKTOP,
                    &ptLimit,1);
                ptLimit.x += 640;
                ptLimit.y += 480;
            }
            if (hBubble)
            {
                if (!hSaveBits)
                {
                    #define XFUDGE 40
                    #define YFUDGE 20

                    // get bubble position
                    GetCursorPos (&ptBubble);
                    if ((ptBubble.x + rBubble.right + XFUDGE) > ptLimit.x)
                        ptBubble.x -= rBubble.right + XFUDGE;
                    else
                        ptBubble.x += XFUDGE;
                    if ((ptBubble.y + rBubble.bottom + YFUDGE) > ptLimit.y)
                        ptBubble.y -= rBubble.bottom + YFUDGE;
                    else
                        ptBubble.y += YFUDGE;

                    // save background
                    hMemDC = CreateCompatibleDC (hDC);
                    hSaveBits = CreateCompatibleBitmap (hDC,
                        rBubble.right,rBubble.bottom);
                    hBitmap = (HBITMAP)SelectObject (hMemDC,hSaveBits);
                    BitBlt (hMemDC,0,0,rBubble.right,rBubble.bottom,
                        hDC,ptBubble.x,ptBubble.y,SRCCOPY);
                    SelectObject (hMemDC,hBitmap);
                    DeleteDC (hMemDC);
                }
                if (!*szPrevString || lstrcmp (lpString,szPrevString) != 0)
                {
                    HBITMAP hTemp;
                    RECT rTemp;

                    // draw bubble on screen
                    hTemp = DrawTransDIB (hDC,hBubble,hMask,
                        ptBubble.x + rBubble.right / 2,
                        ptBubble.y + rBubble.bottom / 2);
                    if (!hMask)
                        hMask = hTemp;

                    // draw text in bubble
                    rTemp = rText;
                    OffsetRect (&rTemp,ptBubble.x,ptBubble.y);
                    DrawText (hDC,lpString,-1,&rTemp,DT_CENTER | DT_WORDBREAK);
                    lstrcpy (szPrevString,lpString);

                    // indicate that bubble was actually drawn, not skipped
                    bBubble = TRUE;
                }
			}
        }
        else
        {
            if (hSaveBits)
            {
                // Restore background
                hMemDC = CreateCompatibleDC (hDC);
                hBitmap = (HBITMAP)SelectObject (hMemDC,hSaveBits);
                BitBlt (hDC,ptBubble.x,ptBubble.y,
                    rBubble.right,rBubble.bottom,hMemDC,0,0,SRCCOPY);
                SelectObject (hMemDC,hBitmap);
                DeleteObject (hSaveBits);
                DeleteDC (hMemDC);
                hSaveBits = 0;
            }
            *szPrevString = 0;
        }
        ReleaseDC (HWND_DESKTOP,hDC);
    }
    return bBubble;
}


/***********************************************************************/
int AstralConfirm( int idStr, ... )
/***********************************************************************/
{
char szBuffer[512];
LPTR lpArguments;
LPSTR lpString;
int retc;
HWND hFocusWindow;

#ifndef NOTCRAYOLA
	return( IDNO );
#endif

/* Put up a modal message box with arguments specified exactly like printf */
/* The caller passes stringtable ID's, and not hardcoded strings */
/* Used to confirm any user operation */
if ( !AstralStr( idStr, &lpString ) )
	return( IDCANCEL );
lpArguments = (LPTR)&idStr + sizeof(idStr);
wvsprintf( szBuffer, lpString, (LPSTR)lpArguments );
hFocusWindow = GetFocus();
retc = MessageBox( MessageParent(), szBuffer, szAppName,
	MB_ICONQUESTION | MB_YESNOCANCEL | MB_TASKMODAL );
SetFocus( hFocusWindow );
return( retc );
}


/***********************************************************************/
int AstralOKCancel( int idStr, ... )
/***********************************************************************/
{
char szBuffer[512];
LPTR lpArguments;
LPSTR lpString;
int retc;
HWND hFocusWindow;

/* Put up a modal message box with arguments specified exactly like printf */
/* The caller passes stringtable ID's, and not hardcoded strings */
/* Used to confirm any user operation */
if ( !AstralStr( idStr, &lpString ) )
	return( IDCANCEL );
lpArguments = (LPTR)&idStr + sizeof(idStr);
wvsprintf( szBuffer, lpString, (LPSTR)lpArguments );
hFocusWindow = GetFocus();
retc = MessageBox( MessageParent(), szBuffer, szAppName,
	MB_ICONQUESTION | MB_OKCANCEL | MB_TASKMODAL );
SetFocus( hFocusWindow );
return( retc );
}


/***********************************************************************/
BOOL AstralAffirm( int idStr, ... )
/***********************************************************************/
{
char szBuffer[512];
LPTR lpArguments;
LPSTR lpString;
int retc;
HWND hFocusWindow;

/* Put up a modal message box with arguments specified exactly like printf */
/* The caller passes stringtable ID's, and not hardcoded strings */
/* Used to confirm any user operation */
if ( !AstralStr( idStr, &lpString ) )
	return( NO );
lpArguments = (LPTR)&idStr + sizeof(idStr);
wvsprintf( szBuffer, lpString, (LPSTR)lpArguments );
hFocusWindow = GetFocus();
retc = MessageBox( MessageParent(), szBuffer, szAppName,
	MB_ICONQUESTION | MB_YESNO | MB_TASKMODAL );
SetFocus( hFocusWindow );
return( retc == IDYES );
}


/***********************************************************************/
void ReadOutF( int idStr, int iNum, ... )
/***********************************************************************/
{
char szNum1[32], szNum2[32];
LPTR lpArguments;
LPLFIXED lpNum;

if (iNum < 1 || iNum > 2)
	return;
lpArguments = (LPTR)&iNum + sizeof(iNum);
lpNum = (LPLFIXED)lpArguments;
szNum1[0] = szNum2[0] = '\0';
FixedAscii(*lpNum, (LPSTR)szNum1, 1);
++lpNum;
if (iNum == 2)
	FixedAscii(*lpNum, (LPSTR)szNum2, 1);
MessageStatus(idStr, (LPSTR)szNum1, (LPSTR)szNum2);
}


/***********************************************************************/
HWND MessageParent()
/***********************************************************************/
{
return( NULL ); // To force TASK MODAL
//if ( !(hWnd = GetLastActivePopup( hWndAstral )) )
//	hWnd = hWndAstral;
//return( hWnd );
}

