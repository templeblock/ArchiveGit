//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "svrdoc.h"
#include "svrview.h"
#include "ipframe.h"
#include "id.h"
#include "ctxhelp.h"

extern STRING szAppName;
extern BOOL fAppActive;

// for YesToAll dialogs
typedef struct _ytastruct
{
	LPSTR lpDialogText;
	LPSTR lpDialogAltText;
	BOOL bYesToAllMultiple;
} YTA, FAR *LPYTA;

static CFile *pLogFile = NULL;

static BOOL bMessageEnable = YES;
CStatusForum	StatusForum;

CStatusForum::CStatusForum()
{
	Status = "";
}

CStatusMonitor::CStatusMonitor()
{
	y = 0;
	m_hWnd = NULL;
}

CStatusMonitor::~CStatusMonitor()
{
}

void CStatusMonitor::Init(HWND hWnd)
{
	if (hWnd)
	{
		ArriveAt(StatusForum);
		m_hWnd = hWnd;
		Assimilate(&StatusForum);
	}
	else
	{
		// only depart from forum if we ever joined
		if (m_hWnd)
		{
			m_hWnd = NULL;
			DepartFrom(StatusForum);
		}
	}
}

void CStatusMonitor::Assimilate (CForum * pForum)
{
	if (pForum == &StatusForum)
	{
		HDC 		hDC;
		RECT 		rWnd;
		LPTSTR		lp;
		CString		StatusLine = StatusForum.GetStatus();

		if (hDC = GetDC(m_hWnd))
		{
			GetClientRect(m_hWnd, &rWnd);
			IntersectClipRect(hDC, rWnd.left, rWnd.top, rWnd.right-X_STATUS, rWnd.bottom);
			SelectObject( hDC, GetStockObject( Window.iLabelFont ) );
			if ( !y )
			{
				TEXTMETRIC tm;
				GetTextMetrics( hDC, &tm );
				y = ( RectHeight(&rWnd) - (tm.tmHeight + tm.tmInternalLeading) ) / 2;
			}
				
			rWnd.left += X_STATUS;
			rWnd.top  += y;
			rWnd.right -= X_STATUS;
			rWnd.bottom -= y;
			HBRUSH	hbr = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
			FillRect(hDC, &rWnd, hbr);
			DeleteObject(hbr);
			lp = StatusLine.GetBuffer(256);
			ColorText(hDC, X_STATUS, y, lp, lstrlen(lp), GetSysColor(COLOR_WINDOWTEXT));
			StatusLine.ReleaseBuffer();
			ReleaseDC(m_hWnd, hDC);
		}
	}
}

/***********************************************************************/
void SetMessageLogging(CFile *pFile)
/***********************************************************************/
{
	pLogFile = pFile;
}

/***********************************************************************/
void LogMessage(CFile *pFile, LPSTR lpMessage)
/***********************************************************************/
{
	if (pFile)
	{
		TRY
		{
			int len;
		
			len = lstrlen(lpMessage);
			lpMessage[len] = '\r';
			++len;
			lpMessage[len] = '\n';
			++len;
			pFile->Write(lpMessage, len);
		}
		END_TRY
	}
}

/***********************************************************************/
void FrameError(int idMsg)
/***********************************************************************/
{
int err;
char szBuffer[512];
char szFormat[256];

err = FrameGetError();
switch (err)
	{
	case ERR_MAXIMAGEHEIGHT:
		if (!AstralStrEx(IDS_EMAXIMAGEHEIGHT, szBuffer, sizeof(szBuffer)))
			return;
	break;		
	case ERR_MAXIMAGEWIDTH:
		if (!AstralStrEx(IDS_EMAXIMAGEWIDTH, szBuffer, sizeof(szBuffer)))
			return;
	break;
	case ERR_IMAGEOVERHEAD:
		if (!AstralStrEx(IDS_EIMAGEOVERHEAD, szBuffer, sizeof(szBuffer)))
			return;
	break;
	case ERR_MALLOC:
		if (!AstralStrEx(IDS_EMALLOC, szBuffer, sizeof(szBuffer)))
			return;
	break;
	case ERR_LINEPOOLALLOC:
		if (!AstralStrEx(IDS_ELINEPOOLALLOC, szBuffer, sizeof(szBuffer)))
			return;
	break;
	case ERR_DISKCACHE:
		if (!AstralStrEx(IDS_EDISKCACHE, szFormat, sizeof(szFormat)))
			return;
 	   	wsprintf( szBuffer, szFormat, (LPSTR)Control.RamDisk,
			DiskSpace(Control.RamDisk) );
	break;
	case ERR_SCROPEN:
		if (!AstralStrEx(IDS_ESCROPEN, szFormat, sizeof(szFormat)))
			return;
 	   	wsprintf( szBuffer, szFormat, (LPSTR)Control.RamDisk );
	break;
	case ERR_SCRWRITE:
		if (!AstralStrEx(IDS_ESCRWRITE, szFormat, sizeof(szFormat)))
			return;
 	   	wsprintf( szBuffer, szFormat, (LPSTR)Control.RamDisk );
	break;
	case ERR_SCRREAD:
		if (!AstralStrEx(IDS_ESCRREAD, szFormat, sizeof(szFormat)))
			return;
 	   	wsprintf( szBuffer, szFormat, (LPSTR)Control.RamDisk );
	break;
	default:
		return;
	}

Message(idMsg, (LPSTR)szBuffer);
}

/***********************************************************************/
BOOL MessageEnable( BOOL bEnable )
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
va_list  lpArguments;
char     szBuffer[512];
static   int debug;
static   FILE_HANDLE hFile;
static   BOOL bReopen;

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
			if ( (hFile = FileOpen( szBuffer, FO_WRITE )) == FILE_HANDLE_INVALID )
				hFile = 0;
			else
				FileSeek( hFile, 0L, 2 );
			}
		else	{
			if ( (hFile = FileOpen( szBuffer, FO_CREATE|FO_WRITE )) == FILE_HANDLE_INVALID )
				hFile = 0;
			}
		}
	if ( hFile )
		{
         va_start( lpArguments, lpFormat );
 	   	wvsprintf( szBuffer, lpFormat, lpArguments );
         va_end( lpArguments );
    		lstrcat(szBuffer, "\r\n");
		FileWrite( hFile, szBuffer, lstrlen(szBuffer) );
		if (debug > 1)
			OutputDebugString(szBuffer);
		}
	}
else
if ( hFile )
	{
	lstrcpy( szBuffer, "---------- CLOSED ----------\r\n" );
	FileWrite( hFile, szBuffer, lstrlen(szBuffer) );
	FileClose( hFile );
	bReopen = YES;
	hFile = 0;
	}
}


/***********************************************************************/
int Print( LPSTR lpFormat, ... )
/***********************************************************************/
{
char     szBuffer[512];
va_list  lpArguments;
int      retc;

if (!bMessageEnable)
	return( IDCANCEL );

/* Put up a modal message box with arguments specified exactly like printf */

va_start( lpArguments, lpFormat );
wvsprintf( szBuffer, lpFormat, lpArguments );
va_end( lpArguments );

retc =  PPMessageBox( szBuffer, MB_OK | MB_TASKMODAL );
return( retc );
}

/***********************************************************************/
int Message( ITEMID idStr, ... )
/***********************************************************************/
{
char     szBuffer[512];
char     szFormat[256];
va_list  lpArguments;
int      retc;

if (!bMessageEnable)
	return(IDCANCEL);
/* Put up a modal message box with arguments specified exactly like printf */
/* The caller passes stringtable ID's, and not hardcoded strings */
/* Used to display error message and warnings */
if ( !AstralStrEx( idStr, szFormat, sizeof(szFormat) ) )
	return( IDCANCEL );

va_start( lpArguments, idStr );
wvsprintf( szBuffer, szFormat, lpArguments );
va_end( lpArguments );

if (pLogFile)
	{
	LogMessage(pLogFile, szBuffer);
	retc = IDOK;
	}
else
	retc = PPMessageBox( szBuffer, MB_ICONEXCLAMATION | MB_OK | MB_TASKMODAL);
return( retc );
}


/***********************************************************************/
void PrintStatus( LPSTR lpFormat, ... )
/***********************************************************************/
{
char     szBuffer[256];
va_list  lpArguments;

va_start( lpArguments, lpFormat );
wvsprintf( szBuffer, lpFormat, lpArguments );
va_end( lpArguments );

Status( szBuffer );
}

/***********************************************************************/
WORD MessageStatus( ITEMID idStr, ... )
/***********************************************************************/
{
va_list  lpArguments;
BOOL     fHasHelp;
char     szBuffer[512];
char     szFormat[256];

// Paint the status bar with a string; arguments specified exactly like printf
// The caller passes stringtable ID's, and not hardcoded strings
// Used to display error message and warnings
// Return the id if it has help (starts with a space)
szBuffer[0] = '\0';
if ( idStr && AstralStrEx( idStr, szFormat, sizeof(szFormat) ) )
	{
   va_start( lpArguments, idStr );
	wvsprintf( szBuffer, szFormat, lpArguments );
   va_end( lpArguments );
	}
else
	{
	AstralStrEx( IDS_READY, szBuffer, sizeof(szBuffer) ) ;
	}
if ( fHasHelp = (szBuffer[0] == ' ') )
	{
	if ( AstralStrEx( IDS_HASHELP, szFormat, sizeof(szFormat) ) )
		lstrcat( szBuffer, szFormat );
	}
if ( !fHasHelp && !Control.Hints )
	return( 0 );

Status( szBuffer );
return( fHasHelp ? idStr : 0 );
}

/***********************************************************************/
WORD MessageHint( ITEMID idStr, ... )
/***********************************************************************/
{
    va_list     lpArguments;
    BOOL        fHasHelp;
    char        szBuffer[512];
    char        szFormat[256];
    const char *Ptr;

    // Paint the status bar with a hint; arguments specified exactly like printf
    // The caller passes stringtable ID's, and not hardcoded strings
    // Used to display error message and warnings
    // Return the id if it has help (starts with a space)
    szBuffer[0] = '\0';

    if ( idStr && AstralHintEx( idStr, szFormat, sizeof( szFormat )))
    {
        va_start( lpArguments, idStr );
	    wvsprintf( szBuffer, szFormat, lpArguments );
        va_end( lpArguments );
    }
    else
    {
	    AstralHintEx( IDS_READY, szBuffer, sizeof(szBuffer)) ;
    }
    if ( fHasHelp = (szBuffer[0] == ' '))
    {
	    if ( AstralHintEx( IDS_HASHELP, szFormat, sizeof(szFormat)))
		    lstrcat( szBuffer, szFormat );
    }
    if ( !fHasHelp && !Control.Hints )
	    return( 0 );

    // Is there an explicit title?
    if( Ptr = strchr( szBuffer, HINTBREAK ))
        lstrcpy( szBuffer, Ptr + 1 );
    else
    if( Ptr = strchr( szBuffer, TITLEBREAK ))
        lstrcpy( szBuffer, Ptr + 1 );

    Status( szBuffer );
    return( fHasHelp ? idStr : 0 );
}

/***********************************************************************/
void StatusOfRectangle( LPRECT lpRect, LFIXED fScale, int Resolution )
/***********************************************************************/
{
STRING szUnit, szString1, szString2, szString3, szString4;
RECT rFile;

OrderRect( lpRect, &rFile );
if ( fScale != 0 )
	{
	ScaleRect( &rFile, fScale );
	}
if ( AstralStrEx( Control.Units-IDC_PREF_UNITINCHES+IDS_INCHES, szUnit, sizeof(szUnit) ) )
	{
	MessageStatus( IDS_RECT_STATUS,
		Unit2String( FGET( RectWidth(&rFile), Resolution ), szString1 ),
		Unit2String( FGET( RectHeight(&rFile), Resolution ), szString2),
		(LPSTR)szUnit,
		Unit2String( FGET( rFile.left, Resolution ), szString3 ),
		Unit2String( FGET( rFile.top, Resolution ), szString4 ) );
	}
}


/***********************************************************************/
void Status( LPSTR lpString )
/***********************************************************************/
{
	if (GetCurrentThreadId() != PictPubApp.GetMainThreadId())
		PictPubApp.m_pMainWnd->SendMessage(WM_STATUSMSG, (WPARAM)lpString);
	else
		CServerDoc::ProcessStatus(PictPubApp.GetActiveView(),lpString);
}


/***********************************************************************/
int AstralConfirm( ITEMID idStr, ... )
/***********************************************************************/
{
char     szBuffer[512];
char     szFormat[256];
va_list  lpArguments;
int      retc;

/* Put up a modal message box with arguments specified exactly like printf */
/* The caller passes stringtable ID's, and not hardcoded strings */
/* Used to confirm any user operation */
if ( !AstralStrEx( idStr, szFormat, sizeof(szFormat) ) )
	return( IDCANCEL );
//lpArguments = (LPTR)&idStr + sizeof(idStr);

va_start( lpArguments, idStr );
wvsprintf( szBuffer, szFormat, lpArguments );
va_end( lpArguments );

if (pLogFile)
	{
	LogMessage(pLogFile, szBuffer);
	retc = IDYES;
	}
else
	{
	retc = PPMessageBox( szBuffer, MB_ICONQUESTION | MB_YESNOCANCEL | MB_TASKMODAL );
	}
return( retc );
}


/***********************************************************************/
int AstralOKCancel( ITEMID idStr, ... )
/***********************************************************************/
{
char     szBuffer[512];
char     szFormat[256];
va_list  lpArguments;
int      retc;

/* Put up a modal message box with arguments specified exactly like printf */
/* The caller passes stringtable ID's, and not hardcoded strings */
/* Used to confirm any user operation */
if ( !AstralStrEx( idStr, szFormat, sizeof(szFormat) ) )
	return( IDCANCEL );
//lpArguments = (LPTR)&idStr + sizeof(idStr);

va_start( lpArguments, idStr );
wvsprintf( szBuffer, szFormat, lpArguments );
va_end( lpArguments );

if (pLogFile)
	{
	LogMessage(pLogFile, szBuffer);
	retc = IDOK;
	}
else
	{
	retc = PPMessageBox( szBuffer, MB_ICONQUESTION | MB_OKCANCEL | MB_TASKMODAL );
	}
return( retc );
}


/***********************************************************************/
BOOL AstralAffirm( ITEMID idStr, ... )
/***********************************************************************/
{
char     szBuffer[512];
char     szFormat[256];
va_list  lpArguments;
int      retc;

/* Put up a modal message box with arguments specified exactly like printf */
/* The caller passes stringtable ID's, and not hardcoded strings */
/* Used to confirm any user operation */
if ( !AstralStrEx( idStr, szFormat, sizeof(szFormat) ) )
	return( NO );
//lpArguments = (LPTR)&idStr + sizeof(idStr);

va_start( lpArguments, idStr );
wvsprintf( szBuffer, szFormat, lpArguments );
va_end( lpArguments );

if (pLogFile)
	{
	LogMessage(pLogFile, szBuffer);
	retc = IDYES;
	}
else
	{
	retc = PPMessageBox( szBuffer, MB_ICONQUESTION | MB_YESNO | MB_TASKMODAL );
	}
return( retc == IDYES );
}


/***********************************************************************/
void ReadOutF( ITEMID idStr, int iNum, ... )
/***********************************************************************/
{
char szNum1[32], szNum2[32];
va_list  lpArguments;
LFIXED value;

if (iNum < 1 || iNum > 2)
	return;

szNum1[0] = szNum2[0] = '\0';

va_start( lpArguments, iNum );
value = va_arg(lpArguments, LFIXED);
FixedAscii(value, (LPSTR)szNum1, 1);
if (iNum == 2)
	{
	value = va_arg(lpArguments, LFIXED);
	FixedAscii(value, (LPSTR)szNum2, 1);
	}
va_end( lpArguments );
MessageStatus(idStr, (LPSTR)szNum1, (LPSTR)szNum2);
}


/***********************************************************************/
HWND MessageParent()
/***********************************************************************/
{
	if (GetCurrentThreadId() != PictPubApp.GetMainThreadId())
	{
		return((HWND)PictPubApp.m_pMainWnd->SendMessage(WM_GETMESSAGEPARENT));
	}
	else
	{
		CWnd *pParent;

		// get active view to determine what window to use for parent
		CServerView *pView = PictPubApp.GetActiveView();
		if (pView)
			pParent = pView->GetParentFrame();
		else // always use this is now view
			pParent = AfxGetMainWnd();

		if (pParent == NULL)
			return(NULL);
		ASSERT_VALID(pParent);
		if (pParent->m_hWnd == NULL)
			return(NULL);

		return (::GetLastActivePopup(pParent->GetTopLevelParent()->m_hWnd));
	}
}

LOCAL BOOL YesToAll_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam);
LOCAL void YesToAll_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);

//************************************************************************/
//	Handle a dialog with the options, Yes, Yes To All, No  & Cancel.
//  If there is a IDC_BSR_TEXT then fills with lpDialogText
//  If there is a IDC_BSR_ALT_TEXT then fills with lpDialogAltText
//************************************************************************/
BOOL WINPROC DlgYesToAllProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//************************************************************************/
{
	switch (msg)
	{						    
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, YesToAll_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, YesToAll_OnCommand);
	    default:
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, 0L);
	}  
}	

//************************************************************************
LOCAL BOOL YesToAll_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************
{
	LPYTA lpYTA;
	
	lpYTA = (LPYTA)lParam;
	if (!lpYTA)
		return(1);
	CenterPopup(hDlg);
	if (lpYTA->lpDialogText)
		SetDlgItemText(hDlg, IDC_BSR_TEXT, lpYTA->lpDialogText);
	if (lpYTA->lpDialogAltText)
		SetDlgItemText(hDlg, IDC_BSR_ALT_TEXT, lpYTA->lpDialogAltText);
	if (!lpYTA->bYesToAllMultiple)
		ControlEnable(hDlg, IDC_YESTOALL, lpYTA->bYesToAllMultiple);
	return(0);
}

//************************************************************************
LOCAL void YesToAll_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
//************************************************************************
{
	switch(id)
	{
		case IDOK:
		case IDCANCEL:
		case IDC_BSR_NO:
		case IDC_YESTOALL:
			AstralDlgEnd(hDlg, id);
		break;
		default:
		break;
	}  // switch command
}

//***********************************************************************
// 	Puts Up the given String in the given dialog.
//	Returns IDOK, IDCANCEL, ID_NO, ID_YESTOALL.
//***********************************************************************
int AstralYesToAllString(HWND hParent, int idDialog, LPSTR lpString, BOOL bMultiple)
//***********************************************************************
{
	int retc;
	YTA yta;
	
	yta.lpDialogText = lpString;
	yta.lpDialogAltText = NULL;
	yta.bYesToAllMultiple = bMultiple;
	retc = (int)AstralDlgParam(NO, PictPubApp.GetResourceHandle(),hParent,idDialog,DlgYesToAllProc, (LONG)&yta);
	return( retc );
}

//***********************************************************************
// 	Puts Up the given Strings in the given dialog.
//	Returns IDOK, IDCANCEL, ID_NO, ID_YESTOALL.
//***********************************************************************
int AstralYesToAllStrings(HWND hParent, int idDialog, LPSTR lpString, LPSTR lpAltString, BOOL bMultiple)
//***********************************************************************
{
	int retc;
	YTA yta;
	
	yta.lpDialogText = lpString;
	yta.lpDialogAltText = lpAltString;
	yta.bYesToAllMultiple = bMultiple;
	retc = (int)AstralDlgParam(NO, PictPubApp.GetResourceHandle(),hParent,idDialog,DlgYesToAllProc, (LONG)&yta);
	return( retc );
}


//***********************************************************************
// 	Puts Up the given String in the given dialog.
//	Arguments specified exactly like printf
// 	if idStr = 0 then no string is used.
//	Returns IDOK, IDCANCEL, ID_NO, ID_YESTOALL.
//***********************************************************************
int AstralYesNoYesToAll(HWND hParent, BOOL bMultiple, int idDialog, int idStr, ... )
//***********************************************************************
{
	char     szBuffer[512];
	va_list  lpArguments;
	LPSTR     lpString;
	int      retc;
	YTA 	yta;
	
	yta.lpDialogAltText = NULL;
	yta.bYesToAllMultiple = bMultiple;
	if (idStr)
	{
		if ( !AstralStr( idStr, &lpString ) )
			return( IDCANCEL );
      va_start( lpArguments, idStr );
		wvsprintf( szBuffer, lpString, lpArguments );
      va_end( lpArguments );
		yta.lpDialogText = *&szBuffer;
	}
	else
		yta.lpDialogText = NULL;
	retc = (int)AstralDlgParam(NO, PictPubApp.GetResourceHandle(),hParent,idDialog,DlgYesToAllProc, (LONG)&yta);
	return( retc );
}

//***********************************************************************
// 	Puts Up the given String in the given dialog.
//	Arguments specified exactly like printf
// 	if idStr = 0 then no string is used.
//	Returns IDYES, IDNO, IDRETRY.
//***********************************************************************
int AstralYesNoRetry(HWND hParent, int idDialog, int idStr, ... )
//***********************************************************************
{
	char     szBuffer[512];
	va_list  lpArguments;
	LPSTR     lpString;
	int      retc;
	
	if (idStr)
	{
		if ( !AstralStr( idStr, &lpString ) )
			return( IDCANCEL );
		va_start( lpArguments, idStr );
		wvsprintf( szBuffer, lpString, lpArguments );
      	va_end( lpArguments );
	}
	else
		szBuffer[0] = '\0';

	retc = (int)AstralDlgParam(NO, PictPubApp.GetResourceHandle(),hParent,idDialog,DlgYesNoRetryProc, (LONG)(LPSTR)szBuffer);
	return( retc );
}


LOCAL BOOL YesNoRetry_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam);
LOCAL void YesNoRetry_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);

//************************************************************************/
BOOL WINPROC DlgYesNoRetryProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//************************************************************************/
{
	switch (msg)
	{						    
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, YesNoRetry_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, YesNoRetry_OnCommand);
	    default:
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, 0L);
	}  
}	

//************************************************************************
LOCAL BOOL YesNoRetry_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************
{
	
	CenterPopup(hDlg);
	if (lParam)
		SetDlgItemText(hDlg, IDC_YESNORETRY_TEXT, (LPSTR)lParam);
	return(0);
}

//************************************************************************
LOCAL void YesNoRetry_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
//************************************************************************
{
	switch(id)
	{
		case IDYES:
		case IDNO:
		case IDRETRY:
			AstralDlgEnd(hDlg, id);
		break;
		default:
		break;
	}  // switch command
}

//************************************************************************
int PPMessageBox(LPCTSTR lpszText, UINT nType)
//************************************************************************
{
	if (GetCurrentThreadId() != PictPubApp.GetMainThreadId())
	{
		return((int)PictPubApp.m_pMainWnd->SendMessage(WM_MESSAGEBOX,
					(WPARAM)lpszText, (LPARAM)nType));
	}
	else
	{
		#ifdef USE_AFXMESSAGEBOX
		int retc = AfxMessageBox(lpszText, nType);
		#else
		HWND hFocusWindow = GetFocus();
		HWND hWndParent = MessageParent();
		EnableOverlappedWindow( hWndParent, NO );
		int retc =  ::MessageBox( hWndParent, lpszText, szAppName, nType );
		EnableOverlappedWindow( hWndParent, YES );
		SetFocus( hFocusWindow );
		#endif
		return(retc);
	}
}

