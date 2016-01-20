/****************************************************************************
 *
 *  $Header: /Development/TPSLibs/Source/WINDLG.C 1     11/06/96 7:22a Mike_houle $
 *
 *  Module Name: WINDLG.C
 *  Description: Dialog routines for TPS debug library (Windows).
 *  Version:     v0.00
 *  Author:      
 *  Date:        
 *
 *  Copyright (c) 1994, 1995 by Turning Point Software, Inc.  All Rights Reserved.
 *
 *  $Log: /Development/TPSLibs/Source/WINDLG.C $
 * 
 * 1     11/06/96 7:22a Mike_houle
 * 
 * 2     11/01/96 6:42a Mike_houle
 * Set the messagebox to be TaskModal
 * 
 * 2     3/13/96 3:22p Mike_houle
 * added FileName, LineNo and buffer to the MessageBox version of the
 * Assert
    
    16    2/28/95 5:49p Bob
    cleaned up use of global variables
    fixed potential crash of mouse/keyboard input
    no longer using SYM file for Win stack crawl; now strictly CodeView
    CodeView info is read into a global handle (should be fixed so we only
    save info we need)
    
    15    2/22/95 10:19a Mgd
    windows requires packed structures
    
    14    2/03/95 5:50p Bob
    Added windows stack crawl from CodeView information
    
    13    1/30/95 12:53p Bob
    stack trace stuff
    
    12    1/27/95 3:00p Bob
    moved some stuff around
    
    11    1/26/95 8:07p Bob
    add stack crawl display
    
    10    1/26/95 7:24p Bob
    Added actual stack-crawl code for Win. Now we just have to call it...
    
    9     1/25/95 5:01p Bob
    fixed file headers
 *
 *
 ****************************************************************************
 */

#if defined(_WINDOWS)

#include "tpsdefs.h"
#include "stackcra.h"
#include "dbgdlg.h"
#include "dbgglbl.h"

#include <windowsx.h>
#include <string.h>

//
// Windows requires packed structures...

#define	MESSAGE_BUFFER_SIZE		1024
//	This is on the stack to not be overwritten if the message box causes more Assertions
char		_szMessageBoxBuffer[MESSAGE_BUFFER_SIZE];
BOOLEAN	fInDoAssertDialog = FALSE;	//	cheep way to guard against multiple entrancy 

#pragma pack(1)

typedef struct DialogHeader
{
   DWORD	dtStyle;
   BYTE	dtItemCount;
   WORD	dtX;
   WORD	dtY;
   WORD	dtCX;
   WORD	dtCY;

	/* The following 5 header items are variable strings so not part of DLGHDR */
	/*   char dtResourceName[]; */
	/*   char dtClassName[];    */
	/*   char dtCaptionText[];  */
	/*   WORD dtPointSize;      */
	/*   char dtFaceName[];     */

} DLGHDR, FAR* LPDLGHDR;


typedef struct DialogItem
{
   WORD	dtilX;
   WORD	dtilY;
   WORD	dtilCX;

   WORD	dtilCY;
   WORD	dtilID;
   DWORD	dtilStyle;
   BYTE  dtilControlClass;
} DLGITEM, FAR* LPDLGITEM;


typedef struct DialogValues {
	LPSZ		lpszTest;
	LPSZ		lpszReason;
	LPSZ		lpszFilename;
	uLONG		ulLineNo;
	uLONG		ulCount;
	LPVOID	lpvData;
	uLONG		ulFormat;
	HGLOBAL	hStackData;
} DLGVALS, FAR* LPDLGVALS;

//
// restore default packing
#pragma pack()



#define	DEFCTL_STATICTEXT			0
#define	DEFCTL_LISTBOX				1
#define	DEFCTL_TABLISTBOX			2
#define	DEFCTL_CHECKBOX			3
#define	DEFCTL_RADIOBUTTON		4
#define	DEFCTL_PUSHBUTTON			5
#define	DEFCTL_DEFPUSHBUTTON		6
#define	DEFCTL_STATICRTEXT		7


#define	DLGID_FILENAME			2000
#define	DLGID_FILENAMETEXT	2001
#define	DLGID_LINENO			2002
#define	DLGID_LINENOTEXT		2003
#define	DLGID_REASON			2004
#define	DLGID_REASONTEXT		2005
#define	DLGID_MEMORY			2006
#define	DLGID_MEMORYTEXT		2007
#define	DLGID_STACKCRAWL		2008
#define	DLGID_STACKCRAWLLB	2009
#define	DLGID_DISABLEASSERT	2010
#define	DLGID_DISPLAYASHEX	2011
#define	DLGID_DEBUGGER			2012
#define	DLGID_TERMINATE		2013
#define	DLGID_IGNORE			2014


#define	ConvertNumberToString(lVal, pStr, cnt)	\
			if (g_tdb_ulFlags & kDbgHexDump)	\
				_NumToHexStr(lVal, pStr, cnt);\
			else										\
				_NumToDecStr(lVal, pStr);


BOOL FAR PASCAL EXPORT _tdb_AssertDlgProc (HANDLE, UINT, WORD, LONG);


static VOID 	NEAR PASCAL	_SetMemoryString( HWND hDlg, LPDLGVALS lpdlgVals );
static VOID 	NEAR PASCAL	_FormatBytes(sBYTE FAR *pData, long count, sBYTE FAR *msg);
static VOID 	NEAR PASCAL	_FormatWords(sBYTE FAR *pData, long count, sBYTE FAR *msg);
static VOID 	NEAR PASCAL	_FormatLongs(sBYTE FAR *pData, long count, sBYTE FAR *msg);
static VOID 	NEAR PASCAL	_FormatCStr(sBYTE FAR *pData, sBYTE FAR *msg);
static VOID 	NEAR PASCAL	_FormatPStr(sBYTE FAR *pData, sBYTE FAR *msg);
static VOID 	NEAR PASCAL	_NumToHexStr(long number, sBYTE FAR *str, sWORD cnt);
static VOID 	NEAR PASCAL	_NumToDecStr(long number, sBYTE FAR *str);
static BOOLEAN NEAR PASCAL	_BuildDialog( VOID );
static BOOLEAN NEAR PASCAL	_CreateDialogHeader( LONG lStyle,
															 int iX, int iY, int icX, int icY,
															 LPSTR lpszResource,
															 LPSTR lpszClass,
															 LPSTR lpszCaption,
															 WORD  dwPointSize,
															 LPSTR lpszFaceName);
static BOOLEAN NEAR PASCAL	_CreateDialogItem( int iCtrlID,
														  LONG lStyle, BYTE bClass, 
														  int iX, int iY, int icX, int icY,
														  LPSTR lpszText,
														  BYTE bExtraBytes );
static VOID 	NEAR PASCAL	_SetDefaultStyle(int iStyle, LPDLGITEM lpDlgItem );
static VOID 	NEAR PASCAL	_CenterDlg(HWND hDlg);


//
//	Type definitions
//
typedef	char	SZMSGBUF[256];



//
//	Local Variables and Data
//

static HANDLE	_hDlgTemplate	= NULL; /* Handle to current dialog template memory   */

static char READONLY	 _szEmpty[]				= "" ;
static char READONLY	 _szSpace[]				= " " ;
static char READONLY	 _szCRLF[]				= "\n" ;
static char READONLY	 _szUnknown[]			= "Unknown" ;
static char READONLY	 _szAssertFailed[]	= "Assertion Failed: " ;
static char READONLY	 _szMemoryMessage[]	= "The value at 0x00000000 is : " ;
static char READONLY	 _szSerifName[]		= "MS Serif" ;
static char READONLY	 _szSansSerifName[]	= "MS Sans Serif";
static char READONLY  _szFile[]				= "File: ";
static char READONLY  _szLineNo[]			= "Line No: ";

#define	MYPOINTSIZE		8
#define	MYFACENAME		_szSansSerifName

#define	kAddressOffset	13	//	the number of bytes from beginning of memory text to 0x00000 string...

/*--------------------------------------------------------------------------
 * Function: _tdb_InitWinAssertDlg
 * Expects:  DEBUGPARMS FAR * lpParms
 *           uLONG            ulFlags
 * Returns:  BOOLEAN          bSuccess
 * Purpose:  Initialize Assert code.
 *--------------------------------------------------------------------------
 */
TDBAPI( BOOLEAN ) _tdb_InitAssertDlg( DEBUGPARMS FAR * lpParms, uLONG ulFlags )
{
	g_tdb_DebugFlags = ulFlags;


	if( lpParms )
		{
		g_tdb_hinstApp = lpParms->hinstApp;
		g_tdb_hwndApp  = lpParms->hwndApp;
		}

	//	We can't run if we do not have an app instance.
	if ( !g_tdb_hinstApp )
		{
		tdb_DebugBeep();
		DebugMsg( "_tdb_InitAssertDlg: missing lpParms->hinstApp\n" );
		return FALSE;
		}

	//
	// BOB950228 -- Note that g_tdb_hwndApp may be NULL.  In particular,
	// the call to DialogBoxIndirectParam() (see next routine) will
	// accept NULL for hwndOwner, but causes nasty problems if we give
	// it the window from GetDesktopWindow().
	//
	return _BuildDialog();
}


/*--------------------------------------------------------------------------
 * Function: _tdb_DoAssertDlg
 * Expects:  char FAR *  lpszTest
 *           char FAR *  lpszReason
 *           char FAR *  lpszFilename
 *           uLONG       ulLineNo
 *           uLONG       ulCount
 *           void FAR *  lpvData
 *           uLONG       ulFormat
 * Returns:  int         iUserSelection
 * Purpose:  Setup and run the Macintosh Assertion Dialog
 * History:  10/28/94  MGD - Initial Revision
 *--------------------------------------------------------------------------
 */
TDBAPI( int ) _tdb_DoAssertDlg( LPSZ lpszTest, LPSZ lpszReason, LPSZ lpszFilename, uLONG ulLineNo, uLONG ulCount, LPVOID lpvData, uLONG ulFormat )
{
	int iUserSelection = ID_OK;

	//	In order to prevent possible nesting of DoAssertDlg. Only enter this section
	//		if we are not currently displaying an Assert.  In a true pre-emptive multi-threaded app
	//		there is a small chance that we may execute the test and be paged out before setting
	//		the boolean, but it is a small enough change that I am not going to worry about it for now.
	if ( !fInDoAssertDialog )
	{
		fInDoAssertDialog = TRUE;
		if (_hDlgTemplate)
			{
			// Hey, Dude!
			DLGVALS	dlgVals;
			FARPROC	lpDlgTest	= MakeProcInstance( (FARPROC) _tdb_AssertDlgProc, g_tdb_hinstApp );
			dlgVals.lpszTest		= lpszTest? lpszTest : _szEmpty;
			dlgVals.lpszReason	= lpszReason? lpszReason : _szEmpty;
			dlgVals.lpszFilename	= lpszFilename? lpszFilename : _szUnknown;
			dlgVals.ulLineNo		= ulLineNo;
			dlgVals.ulCount		= ulCount;
			dlgVals.lpvData		= lpvData;
			dlgVals.ulFormat		= ulFormat;
			dlgVals.hStackData	= g_tdb_hStackData;
			iUserSelection			= DialogBoxIndirectParam( g_tdb_hinstApp, _hDlgTemplate, g_tdb_hwndApp, lpDlgTest, (LPARAM)&dlgVals);
			FreeProcInstance(lpDlgTest);
		}
		else
		{
			int			iret;
			int			iApproxMessageSize;	//	To count the potential size of the message to make sure I
														//	don't overflow the szMessageBufferSize
			sBYTE FAR*	szBuf;

			iApproxMessageSize	= wsprintf( _szMessageBoxBuffer, "%s%s\n%s%ld\n\n", _szFile, lpszFilename, _szLineNo, ulLineNo );
			szBuf						= _szMessageBoxBuffer + iApproxMessageSize;
			iApproxMessageSize	+= _fstrlen( lpszReason );
			iApproxMessageSize	+= 256;	//	256 is an unofficial size for the amount of data that will
												//	be visible in an Assert as extra data

			//	If there is any data to add and a rough calculation implies that the data will fit
			//	in the buffer, include it. Otherwise, don't include it
			if ((ulCount > 0) && (iApproxMessageSize<MESSAGE_BUFFER_SIZE))
			{
				switch (ulFormat)
				{
					case 'b':
					case 'B':
						_FormatBytes((sBYTE FAR*)lpvData, ulCount, szBuf);
						break;
			
					case 'w':
					case 'W':
						_FormatWords((sBYTE FAR*)lpvData, ulCount, szBuf);
						break;
			
					case 'l':
					case 'L':
						_FormatLongs((sBYTE FAR*)lpvData, ulCount, szBuf);
						break;
			
					case 's':
					case 'S':
						_FormatCStr((sBYTE FAR*)lpvData,  szBuf);
						break;
			
					case 'p':
					case 'P':
						_FormatPStr((sBYTE FAR*)lpvData, szBuf);
						break;
				}
				_fstrcat( _szMessageBoxBuffer, _szCRLF );
				_fstrcat( _szMessageBoxBuffer, _szCRLF );
			}

			//	Add the Reason to the buffer
			_fstrcat( _szMessageBoxBuffer, lpszReason );
			iret = MessageBox(NULL, _szMessageBoxBuffer, ((lpszTest)? lpszTest : _szAssertFailed), MB_YESNOCANCEL | MB_TASKMODAL);

			switch (iret)
			{
				case IDYES:
					iUserSelection = ID_OK;
					break;
				case IDNO:
					iUserSelection = ID_TERMINATE;
					break;
				default:
					iUserSelection = ID_DEBUGGER;
					break;
			}
		}
		fInDoAssertDialog = FALSE;
	}

	return iUserSelection;
}


//
//	Shut down debug library.
//
TDBAPI( VOID ) _tdb_DeinitAssertDlg( VOID )
{
	if (_hDlgTemplate)
		GlobalFree(_hDlgTemplate);
	_hDlgTemplate = NULL;
}


//--------------------------------------------------------------------------
//		Local Support Functions.
//--------------------------------------------------------------------------

/*--------------------------------------------------------------------------
 * Function: _tdb_AssertDlgProc
 * Expects:  HWND        hDlg      see windows documentation for parameter list
 *           UINT        message;
 *           WORD        wParam;
 *           LONG        lParam;
 * Returns:  BOOL        bProcessed
 * Purpose:  Process the dialog box commands
 * History:  11/4/94     Initial Revision (mdh)
 *--------------------------------------------------------------------------
 */
BOOL FAR PASCAL EXPORT _tdb_AssertDlgProc ( HWND hDlg, UINT message, WORD wParam, LONG lParam )
{
	char						tmp[200];
	BOOLEAN					bVal;
	static	LPDLGVALS	lpdlgVals = NULL;

   switch(message)
   	{

      case WM_INITDIALOG:

			_CenterDlg(hDlg);

			lpdlgVals	= (DLGVALS*)lParam;

			//	Fill in the Static Text Variables here...
			lstrcpy( tmp, _szAssertFailed );
			lstrcat( tmp, lpdlgVals->lpszTest );
			SetWindowText(hDlg, tmp );
			SetWindowText( GetDlgItem(hDlg, DLGID_FILENAMETEXT), lpdlgVals->lpszFilename );
			SetWindowText( GetDlgItem(hDlg, DLGID_REASONTEXT), lpdlgVals->lpszReason );
			wsprintf(tmp, "%ld", lpdlgVals->ulLineNo);
			SetWindowText( GetDlgItem(hDlg, DLGID_LINENOTEXT), tmp );

			//	Setup the Check Boxes.
			CheckDlgButton(hDlg, DLGID_DISABLEASSERT, 0);	//	checkbox cannot be checked.
			CheckDlgButton(hDlg, DLGID_DISPLAYASHEX, (g_tdb_ulFlags & kDbgHexDump)? 1 : 0);
			_SetMemoryString( hDlg, lpdlgVals );

#ifndef	WIN32

			//	Fill in the StackCrawl listbox...
			if (lpdlgVals->hStackData)
			{
				LPSTR	lpStackData	= GlobalLock(lpdlgVals->hStackData);
				int	count			= 0;
				DWORD	dwIndex;
				while (*lpStackData)
				{
					// Skip over current routine.
					if (count > 0)
					{
						dwIndex = SendDlgItemMessage( hDlg, DLGID_STACKCRAWLLB, 
													LB_ADDSTRING, 0, (LPARAM)lpStackData );
					}
					lpStackData += lstrlen(lpStackData) + 1;
					count++;
				}
				GlobalUnlock(lpdlgVals->hStackData);
			}

#endif


         return(TRUE);

      case WM_COMMAND:

         switch( wParam )
         	{

				case	DLGID_DISABLEASSERT:
					bVal = (BOOLEAN) IsDlgButtonChecked( hDlg, wParam );
					CheckDlgButton(hDlg, wParam, (bVal)? 0 : 1);
					tdb_ToggleDebugFlags( kDbgAssertDisabled );
					return ( TRUE );
					break;

				case	DLGID_DISPLAYASHEX:
					bVal = (BOOLEAN) IsDlgButtonChecked( hDlg, wParam );
					CheckDlgButton(hDlg, wParam, (bVal)? 0 : 1);
					tdb_ToggleDebugFlags( kDbgHexDump );
					_SetMemoryString( hDlg, lpdlgVals );
					InvalidateRect( GetDlgItem(hDlg, wParam), NULL, FALSE );
  					return ( TRUE );
               break;

				case	IDYES:
            case	DLGID_IGNORE:
					EndDialog( hDlg, ID_OK );
					return (TRUE );

				case	IDNO:
				case	DLGID_TERMINATE:
					EndDialog( hDlg, ID_TERMINATE );
               return( TRUE );

				case	IDCANCEL:
				case	DLGID_DEBUGGER:
					EndDialog( hDlg, ID_DEBUGGER );
					return ( TRUE );

            default:
                return( TRUE );

         	}  /* end wParam switch */
         break;   /* end wm_command */

   	}  /* end message switch */

   return( FALSE );
}


/*--------------------------------------------------------------------------
 * Function: _SetMemoryString
 * Expects:  HWND        hDlg
 *           LPDLGVALS   lpDlgVals;
 * Returns:  nothing
 * Purpose:  To Set the static string for the 'Memory display' of the assert
 *           dialog box.
 * History:  11/7/94     Initial Revision (mdh)
 *--------------------------------------------------------------------------
 */
static VOID NEAR PASCAL	_SetMemoryString( HWND hDlg, LPDLGVALS lpdlgVals )
{
	char		szTmp[32];
	SZMSGBUF	szBuf;

	if (lpdlgVals->ulCount > 0)
		{
		_NumToHexStr((long)lpdlgVals->lpvData, szTmp, 8);

		_fstrcpy(szBuf, _szMemoryMessage);
		_fmemmove(&(szBuf[kAddressOffset]), szTmp, _fstrlen(szTmp));

		switch (lpdlgVals->ulFormat)
			{
			case 'b':
			case 'B':
				_FormatBytes(((sBYTE FAR*)lpdlgVals->lpvData), lpdlgVals->ulCount, (sBYTE FAR*)szBuf);
				break;
	
			case 'w':
			case 'W':
				_FormatWords(((sBYTE FAR*)lpdlgVals->lpvData), lpdlgVals->ulCount, (sBYTE FAR*)szBuf);
				break;
	
			case 'l':
			case 'L':
				_FormatLongs(((sBYTE FAR*)lpdlgVals->lpvData), lpdlgVals->ulCount, (sBYTE FAR*)szBuf);
				break;
	
			case 's':
			case 'S':
				_FormatCStr(((sBYTE FAR*)lpdlgVals->lpvData),  szBuf);
				break;
	
			case 'p':
			case 'P':
				_FormatPStr(((sBYTE FAR*)lpdlgVals->lpvData), szBuf);
				break;
			}
		}
	else
		*szBuf = 0;		//	Null terminate the string.
	
	SetWindowText( GetDlgItem( hDlg, DLGID_MEMORYTEXT ), szBuf );
}	


//==================================================================================
//	_FormatBytes(void FAR *pdata, long count)
//
//==================================================================================
static	VOID NEAR PASCAL	_FormatBytes(sBYTE FAR *pData, long count, sBYTE FAR *msg)
{
	uBYTE	temp[32];
	short	i;
	
	for (i = 0 ; i < count; ++i)
	{
		ConvertNumberToString(*((sBYTE *)pData), (sBYTE *)temp, 2);
		
		if (_fstrlen(msg) + _fstrlen(temp) + 1 < sizeof(SZMSGBUF))
		{
			_fstrcat(msg, temp);
			_fstrcat(msg, _szSpace);
			pData += sizeof(sBYTE);
		}
	}
}

//==================================================================================
//	_FormatWords(void FAR *pdata, long count)
//
//==================================================================================
static	VOID NEAR PASCAL	_FormatWords(sBYTE FAR *pData, long count, sBYTE FAR *msg)
{
	uBYTE	temp[32];
	short	i;
	
	for (i = 0 ; i < count; ++i)
	{
		ConvertNumberToString(*((sWORD *)pData), (sBYTE *)temp, 4);

		if (_fstrlen(msg) + _fstrlen(temp) + 1 < sizeof(SZMSGBUF))
		{
			_fstrcat(msg, temp);
			_fstrcat(msg, _szSpace);
			pData += sizeof(sWORD);
		}
	}
}

//==================================================================================
//	_FormatLongs(void FAR *pdata, long count)
//
//==================================================================================
static	VOID NEAR PASCAL	_FormatLongs(sBYTE FAR *pData, long count, sBYTE FAR *msg)
{
	uBYTE	temp[32];
	short	i;
	
	for (i = 0 ; i < count; ++i)
	{
		ConvertNumberToString(*((long *)pData), (sBYTE *)temp, 8);
		
		if (_fstrlen(msg) + _fstrlen(temp) + 1 < sizeof(SZMSGBUF))
		{
			_fstrcat(msg, temp);
			_fstrcat(msg, _szSpace);
			pData += sizeof(sLONG);
		}
	}
}

//==================================================================================
//	_FormatCStr(void FAR *pdata, long count)
//
//==================================================================================
static	VOID NEAR PASCAL	_FormatCStr(sBYTE FAR *pData, sBYTE FAR *msg)
{
	int iMsgLen		= _fstrlen(msg);
	int iDataLen	= _fstrlen(pData);
	if ( (iMsgLen + iDataLen + 1) < sizeof(SZMSGBUF))
		iDataLen = sizeof(SZMSGBUF) - iMsgLen - 1;
	if (iDataLen > 0)
		_fstrncat(msg, pData, iDataLen);
}

//==================================================================================
//	_FormatPStr(void FAR *pdata, long count)
//
//==================================================================================
static	VOID NEAR PASCAL	_FormatPStr(sBYTE FAR *pData, sBYTE FAR *msg)
{
	int	iMsgLen	= _fstrlen(msg);
	int	iDataLen	= (uBYTE)pData[0];
	if ((iMsgLen + iDataLen)  < sizeof(SZMSGBUF))
		iDataLen = sizeof(SZMSGBUF) - (uBYTE)iMsgLen;
		
	if (iDataLen > 0)
		_fmemmove(&(msg[iMsgLen]), pData + 1, iDataLen);
	iMsgLen += (uBYTE)pData[0];
	msg[iMsgLen] = 0;		//	make sure to null terminate the string
}


/*--------------------------------------------------------------------------
 * Function: _NumtoHexStr
 * Expects:  long        number
 *           sBYTE*      str
 *           sWORD       cnt
 * Returns:  nothing
 * Purpose:  To put the ascii equivalent of the hexidecimal values
 *           refered to be number.  The cnt is to determine how many
 *           nibbles are actually significant.
 * History:  11/7/94     Initial Revision (mdh)
 *--------------------------------------------------------------------------
 */
static	VOID NEAR PASCAL	_NumToHexStr(long number, sBYTE FAR *str, sWORD cnt)
{
	int	i = 0;
	uBYTE	mask;

	//	adjust number to account for the bits not desired.
	if (cnt < 8)
		number <<= 8;
	if (cnt < 6)
		number <<= 8;
	if (cnt < 4)
		number <<= 8;

	//	first insert the '0x' designator into the string
	str[i++] = '0';
	str[i++] = 'x';
	cnt += 2;		//	add 2 to the count because I added the '0x' identifier


	do
	{
		mask = (uBYTE)((number & 0xF0000000) >> 28);
		if (mask < 10)
			str[i++] = (sBYTE)(mask + '0');
		else
			str[i++] =  (sBYTE)((mask - 10) + 'A');
		number <<= 4;
	}
	while (i < cnt);
	
	str[i] = 0;	//	null terminate the string
}



/*--------------------------------------------------------------------------
 * Function: _NumtoDecStr
 * Expects:  long        number
 *           sBYTE*      str
 * Returns:  nothing
 * Purpose:  To put the ascii equivalent of the decimal values

 *           refered to be number.  The cnt is to determine how many
 *           nibbles are actually significant.
 * History:  11/7/94     Initial Revision (mdh)
 *--------------------------------------------------------------------------
 */
static	VOID NEAR PASCAL	_NumToDecStr(long number, sBYTE FAR *str)
{
	uBYTE	c;
	int	count;
	int	i;
	int	j;
	sLONG	sign;
	sLONG	n = (sLONG)number;
	
	//	first, build the string backwards and then reverse it...
	if ((sign = n) < 0)
		n = -n;

	i=0;
	str[i++] = 0;		//	null terminate the string at the head (it is backwards)
	do 
	{
		str[i++] = (uBYTE)(n % 10) + '0';
	} while ((n /= 10) > 0);
	
	if (sign < 0)
		str[i++] = (uBYTE)'-';

	count = i;
	for (i = 0, j = count-1; i < j; i++, j--)
	{
		c = str[i];
		str[i] = str[j];
		str[j] = c;
	}	
}


//----------------------------------------------------------------
//		Below are the function used to create a dialog template in
//		memory.  The structure of a dialog template is described

//		above in the structures or in Vol 4, chapter 7 of the
//		Microsoft programmers reference manual.
//
//		The code was copied from a developers CD and rewritten to have
//		error checking and be correct.
//----------------------------------------------------------------

static WORD	_wOffset;      /* Current memory offset (updated by CDH & CDI) */

/*--------------------------------------------------------------------------
 * Function: _BuildDialog
 * Expects:  nothing
 * Returns:  BOOLEAN     bSucessful
 * Purpose:  Create a dialog template for the Assertion dialog
 * History:  11/4/94     Initial Revision (mdh)
 *--------------------------------------------------------------------------
 */
static	BOOLEAN NEAR PASCAL	_BuildDialog( VOID )
{
#ifdef	WIN32

	_hDlgTemplate = NULL;
	return TRUE;

#else

	BOOLEAN	bResult;
	int		x, y, w, h;
	LONG		lStyle;

	x = y = 0;
	w = 320;
	h = 260;

	lStyle = WS_CAPTION | WS_DLGFRAME  | WS_GROUP | WS_POPUP;	/* window style */
	lStyle |= DS_SETFONT;			// Bob's hack

	/* Build the Dialog header */
	bResult = _CreateDialogHeader(
						lStyle,			/* window style */
						x, y, w, h,		/* coordinates of Dialog box.*/
						_szEmpty,		/* menu        */
						_szEmpty,      /* class name  */
						_szEmpty,		/* Caption bar */
						(lStyle & DS_SETFONT) ? MYPOINTSIZE : 0,			/* point size  */
						(lStyle & DS_SETFONT) ? MYFACENAME : _szEmpty	/* face name   */
						);

	if (bResult == FALSE)
		return FALSE;

	/* Each call to CreateDialogItem is an item to be placed inside the dialog box */
	/* This next comment line is the value that is being passed */
	/*    id, style, class, x, y, cx, cy, text, extrabytes      */

//    LTEXT           "FILE NAME:",-1,10,9,40,12,NOT WS_GROUP	
//    LTEXT           "File Name goes here",-1,55,10,200,12,NOT WS_GROUP	
//    LTEXT           "Line#: ",-1,260,10,20,12,NOT WS_GROUP	
//    LTEXT           "99999",-1,285,10,22,12,NOT WS_GROUP	
//    LTEXT           "REASON:",-1,10,25,40,12,NOT WS_GROUP	
//    LTEXT           "Reason goes here",-1,55,25,254,24,NOT WS_GROUP
//    LTEXT           "MEMORY:",-1,10,55,40,12,NOT WS_GROUP	
//    LTEXT           "Memory dump goes here",-1,55,55,185,48,NOT WS_GROUP
//    LTEXT           "STACK:",-1,10,105,38,12,NOT WS_GROUP
//    LISTBOX         1006,10,120,300,103,LBS_NOINTEGRALHEIGHT | WS_VSCROLL
//    CONTROL         "Disable &Asserts",1007,"Button",BS_AUTOCHECKBOX | 
//                    WS_TABSTOP,245,75,65,12
//    CONTROL         "Display as &Hex",1008,"Button",BS_AUTOCHECKBOX | 
//                    WS_TABSTOP,245,55,61,12
//    DEFPUSHBUTTON   "&Ignore",1000,55,235,50,14
//    PUSHBUTTON      "&Terminate App",1001,122,235,70,14
//    PUSHBUTTON      "&Debugger",1002,209,235,50,14

	_CreateDialogItem( DLGID_FILENAME,      DEFCTL_STATICTEXT,   0,  10, 	9, 	40, 	12, 	"FILE NAME:",       0 );
	_CreateDialogItem( DLGID_FILENAMETEXT,  DEFCTL_STATICTEXT,   0,  55, 	10, 	200, 	12, 	"",                 0 );
	_CreateDialogItem( DLGID_LINENO,        DEFCTL_STATICTEXT,   0,  260, 	10, 	20, 	12, 	"Line#:",           0 );
	_CreateDialogItem( DLGID_LINENOTEXT,    DEFCTL_STATICTEXT,   0,  285, 	10, 	22, 	12, 	"",                 0 );
	_CreateDialogItem( DLGID_REASON,        DEFCTL_STATICTEXT,   0,  10, 	25, 	40, 	12, 	"REASON:",          0 );
	_CreateDialogItem( DLGID_REASONTEXT,    DEFCTL_STATICTEXT,   0,  55, 	25, 	254, 	24, 	"",                 0 );
	_CreateDialogItem( DLGID_MEMORY,        DEFCTL_STATICTEXT,   0,  10, 	55, 	40, 	12, 	"MEMORY:",          0 );
	_CreateDialogItem( DLGID_MEMORYTEXT,    DEFCTL_STATICTEXT,   0,  55, 	55, 	185, 	48, 	"",                 0 );
	_CreateDialogItem( DLGID_STACKCRAWL,    DEFCTL_STATICTEXT,   0,  10, 	105, 	38, 	12, 	"STACK:",           0 );
   _CreateDialogItem( DLGID_STACKCRAWLLB,  DEFCTL_TABLISTBOX,   0,  10, 	120, 	300, 	103,	"",                 0 );
   _CreateDialogItem( DLGID_DISPLAYASHEX,  DEFCTL_CHECKBOX,     0,  245, 	55, 	61, 	12,  "Display As &Hex",   0 );
   _CreateDialogItem( DLGID_DISABLEASSERT, DEFCTL_CHECKBOX,     0,  245, 	75, 	65, 	12,  "Disable &Asserts",  0 );
   _CreateDialogItem( DLGID_IGNORE,        DEFCTL_DEFPUSHBUTTON,0,  55,   	235, 	50,  	16,  "&Ignore",           0 );
   _CreateDialogItem( DLGID_TERMINATE,     DEFCTL_PUSHBUTTON,   0,  122,  	235, 	70,  	16,  "&Terminate",        0 );
   _CreateDialogItem( DLGID_DEBUGGER,      DEFCTL_PUSHBUTTON,   0,  209,  	235, 	50,  	16,  "&Debugger",         0 );

   /* Get the handle to the new dialog table */
	return (_hDlgTemplate != NULL);

#endif
}



/*--------------------------------------------------------------------------
 *
 * Create Dialog Header 
 *
 *  This routine allocates a piece of global memory
 *  and then fills in the dialog header structure and saves the
 *  information in global memory.
 *		LONG  lStyle;			Dialog box Style
 *		int   iX; 				Dialog box top left column
 *		int   iY; 				Dialog box top row 
 *		int   icX;				Dialog box width
 *		int   icY;				Dialog box height
 *		LPSTR lpszResource;	Dialog box resource string
 *		LPSTR lpszClass;		Dialog box class string
 *		LPSTR lpszCaption;	Dialog box caption
 *
 *--------------------------------------------------------------------------
 */
static	BOOLEAN NEAR PASCAL	_CreateDialogHeader( LONG lStyle,
																int iX, int iY, int icX, int icY,
																LPSTR lpszResource,
																LPSTR lpszClass,
																LPSTR lpszCaption,
																WORD  dwPointSize,
																LPSTR lpszFaceName)
{

	uWORD		uwResourceLength,		/* Length of resource string */
				uwClassLength,			/* Length of class string */
				uwCaptionLength,		/* Length of caption String */
				uwPointSizeLength,	/* Length of pointsize field */
				uwFaceNameLength;		/* Length of facename string */
	DWORD		dwMemLength;			/* Dialog header memory allocation length */
	DLGHDR	DlgHdr;					/* Dialog header structure */
	LPSTR		lpHdrData;				/*	Long Pointer to the header data */
	LPSTR		lpDlgHdr;				/* Long pointer to dialog header structure */
	int		i;							/* Loop index */

   /* Initialize memory offset */
	_wOffset = 0;  /* set memory offset to ZERO */

   /* Determine string lengths (including terminating null ) */
   uwResourceLength 	= _fstrlen( lpszResource ) + 1;
   uwClassLength    	= _fstrlen( lpszClass ) + 1;
   uwCaptionLength  	= _fstrlen( lpszCaption ) + 1;
	uwPointSizeLength	= (lStyle & DS_SETFONT) ? sizeof( WORD ) : 0;
	uwFaceNameLength	= (lStyle & DS_SETFONT) ? _fstrlen( lpszFaceName ) + 1 : 0;

   /* Determine length of memory to allocate for dialog header */
   dwMemLength = (DWORD)( sizeof( DLGHDR )
									+ uwResourceLength
									+ uwClassLength
									+ uwCaptionLength 
									+ uwPointSizeLength
									+ uwFaceNameLength
								);

   /* Allocate dialog template memory for dialog header and obtain handle */
	_hDlgTemplate = GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, dwMemLength );

   /* Lock allocated memory for modification */
   if (_hDlgTemplate && ((lpHdrData = GlobalLock(_hDlgTemplate)) != NULL) )
		{
		/* Set dialog header structure data to passed in parameters */
		DlgHdr.dtStyle     = lStyle;
		DlgHdr.dtItemCount = 0;      /* set to zero */
		DlgHdr.dtX         = iX;
		DlgHdr.dtY         = iY;
		DlgHdr.dtCX        = icX;
		DlgHdr.dtCY        = icY;

      /* Get pointer to dialog header structure */
      lpDlgHdr = (LPSTR)&DlgHdr;

      /* Copy dialog header structure to allocated memory */
      for ( i = 0; i < sizeof( DLGHDR ); i++ )
         *lpHdrData++ = *lpDlgHdr++;

      /* Copy resource string to allocated memory */
      while ( ( *lpHdrData++ = *lpszResource++ ) );

      /* Copy class string to allocated memory */
      while ( ( *lpHdrData++ = *lpszClass++ ) );

      /* Copy caption string to allocated memory */
      while ( ( *lpHdrData++ = *lpszCaption++ ) );

		if (lStyle & DS_SETFONT)
		{
      	/* Copy point size to allocated memory */
			*(LPWORD)lpHdrData = dwPointSize;
			lpHdrData += sizeof(WORD);

      	/* Copy facename string to allocated memory */
      	while ( ( *lpHdrData++ = *lpszFaceName++ ) );
		}

      /* Adjust memory offset past memory allocated for dialog header */
      _wOffset += (WORD)dwMemLength;

      /* Unlock allocated memory */
      GlobalUnlock( _hDlgTemplate );
   	}
   else
   	{
      GlobalFree( _hDlgTemplate ); /* free allocated memory */
		_hDlgTemplate = NULL;
	   }

	return (BOOLEAN)(_hDlgTemplate != NULL); 	//	Return TRUE if we have a template.
}

/*--------------------------------------------------------------------------
 *
 * Create Dialog Item 
 *  
 *  This routine fills in the dialog item structure and 
 *  saves the information in global memory after resizing it.
 *		int    iCtrlID;		Control ID
 *		LONG   lStyle;			Control style
 *		BYTE   bClass;			Control class
 *		int    iX;				Control top left column
 *		int    iY;				Control top row
 *		int    icX;				Control width
 *		int    icY;				Control height
 *		LPSTR  lpszText;		Control text
 *		BYTE   bExtraBytes;	Control extra bytes count
 *
 *--------------------------------------------------------------------------
 */

static	BOOLEAN NEAR PASCAL	_CreateDialogItem( int iCtrlID,
															LONG lStyle, BYTE bClass, 
															int iX, int iY, int icX, int icY,
															LPSTR lpszText,
															BYTE bExtraBytes )
{
	DLGITEM	dlgItem;
	LPDLGHDR	lpDlgHdr;
	LPSTR    lpszCtrlData;  /* Long pointer to locked dialog template memory  */
	LPSTR    lpszDlgItem;   /* Long pointer to dialog control structure       */
	int      i;           /* Loop index                                     */
	WORD     wTextLength;  /* Length of control text string                  */
	DWORD    dwMemLength;
	HANDLE   hCurDlgTemp;

	//	If dialog template is NULL - return FALSE;
	if (!_hDlgTemplate)
		return FALSE;

	dlgItem.dtilX  = iX;
	dlgItem.dtilY  = iY;
	dlgItem.dtilCX = icX;
	dlgItem.dtilCY = icY;
	dlgItem.dtilID = iCtrlID;

	if (bClass == (BYTE)0)
  		_SetDefaultStyle( (int)lStyle, &dlgItem );
	else
		{
      dlgItem.dtilControlClass	= bClass; /* default, may change in next function */
      dlgItem.dtilStyle				= lStyle;
		}

	wTextLength	= _fstrlen( lpszText ) + 1;
	dwMemLength	= (DWORD)(_wOffset + sizeof(DLGITEM) + wTextLength + sizeof(BYTE) );
	hCurDlgTemp	= GlobalReAlloc( _hDlgTemplate, dwMemLength, GMEM_MOVEABLE);

	if (hCurDlgTemp == NULL)
  		{ 
		GlobalFree( _hDlgTemplate );
		_hDlgTemplate = NULL;
		return FALSE;
		}
  _hDlgTemplate = hCurDlgTemp;

   /* Adjust pointer to reallocated memory bypassing existing data */
   if ( (lpszCtrlData = GlobalLock(hCurDlgTemp)) == NULL)
   {
     GlobalFree( _hDlgTemplate );
	  _hDlgTemplate = NULL;
	  return FALSE;
   }

	lpDlgHdr			= (LPDLGHDR)lpszCtrlData;
   lpszCtrlData	+= _wOffset;
   
   /* Get pointer to dialog control structure */
	lpszDlgItem = (LPSTR)&dlgItem;

   /* Copy dialog control structure to allocated memory */
	for ( i=0; i<sizeof( DLGITEM ); i++ )
		*lpszCtrlData++ = *lpszDlgItem++;

   /* Copy control text string to allocated memory */
	while ( ( *lpszCtrlData++ = *lpszText++ ) );

   /* Copy extra byte count to allocated memory */
	*lpszCtrlData = bExtraBytes;

   /* Adjust memory offset past memory reallocated for dialog control */
	_wOffset += (WORD)( sizeof( dlgItem ) + wTextLength + sizeof(BYTE));

	/*	Increment the number of items in the dialog header */
	++(lpDlgHdr->dtItemCount);

   /* Unlock reallocated memory */
	GlobalUnlock( _hDlgTemplate );

	return( TRUE );  /* return successful */
}


/* ------------- List of predefined dialog items ------------------- */

#define BUTTONCLASS    0x80 /* 128 */
#define EDITCLASS      0x81 /* 129 */
#define STATICCLASS    0x82 /* 130 */
#define LISTBOXCLASS   0x83 /* 131 */
#define SCROLLBARCLASS 0x84 /* 132 */
#define COMBOBOXCLASS  0x85 /* 133 */

#define	STATICSTYLE				(SS_LEFT | WS_CHILD | WS_VISIBLE)
#define	STATICRSTYLE			(SS_RIGHT | WS_CHILD | WS_VISIBLE)
#define	LISTBOXSTYLE			(WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL)
#define	TABLISTBOXSTYLE	(LISTBOXSTYLE | LBS_USETABSTOPS)
#define	CHECKBOXSTYLE			(BS_CHECKBOX | WS_CHILD | WS_VISIBLE | WS_TABSTOP)
#define	RADIOBUTTONSTYLE		(BS_RADIOBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP)
#define	DEFPUSHBUTTONSTYLE	(BS_DEFPUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP)
#define	PUSHBUTTONSTYLE		(BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP)

/*--------------------------------------------------------------------------
 *
 * Function: _SetDefaultStyle
 * Expects:  int         iStyle
 *           LPDLGITEM   lpDlgItem
 * Returns:  BOOLEAN     bSucessful
 * Purpose:  Create a dialog template for the Assertion dialog
 * History:  11/4/94     Initial Revision (mdh)
 *
 *--------------------------------------------------------------------------
 */
static	VOID NEAR PASCAL	_SetDefaultStyle(int iStyle, LPDLGITEM lpDlgItem )
{
	/* if a iStyle is given then dtilControlClass will take on the default value. */ 
	switch ( iStyle )
		{
		case DEFCTL_STATICTEXT:
			lpDlgItem->dtilControlClass = STATICCLASS;
			lpDlgItem->dtilStyle = STATICSTYLE;

			break;

		case DEFCTL_STATICRTEXT:
			lpDlgItem->dtilControlClass = STATICCLASS;
			lpDlgItem->dtilStyle = STATICRSTYLE;
			break;

		case DEFCTL_LISTBOX:
			lpDlgItem->dtilControlClass = LISTBOXCLASS ;
			lpDlgItem->dtilStyle = LISTBOXSTYLE;
			break;

		case DEFCTL_TABLISTBOX:
			lpDlgItem->dtilControlClass = LISTBOXCLASS ;
			lpDlgItem->dtilStyle = TABLISTBOXSTYLE;
			break;

		case DEFCTL_CHECKBOX:
			lpDlgItem->dtilControlClass = BUTTONCLASS ;
			lpDlgItem->dtilStyle = CHECKBOXSTYLE;
			break;
		
		case DEFCTL_RADIOBUTTON:
			lpDlgItem->dtilControlClass = BUTTONCLASS ;
			lpDlgItem->dtilStyle = RADIOBUTTONSTYLE;
			break;

		case DEFCTL_DEFPUSHBUTTON:
			lpDlgItem->dtilControlClass = BUTTONCLASS ;
			lpDlgItem->dtilStyle = DEFPUSHBUTTONSTYLE;
			break;

		case DEFCTL_PUSHBUTTON:
			lpDlgItem->dtilControlClass = BUTTONCLASS ;
			lpDlgItem->dtilStyle = PUSHBUTTONSTYLE;
			break;

		default:
			DebugMsg( "Unknown dialog style\n" );
			break;

		} /* end switch */
}


/*--------------------------------------------------------------------------
 * Function: _CenterDlg
 *
 * Purpose:	Center a dialog box (or other window!) on screen.
 *
 * Expects:
 *		HWND	hwnd
 *
 * Returns: 
 *		nothing
 *
 *--------------------------------------------------------------------------
 */
static	VOID NEAR PASCAL	_CenterDlg(HWND hDlg)
{
	RECT	rcDlg, rcDesktop;
	int	x, y, w, h;

	GetWindowRect(GetDesktopWindow(), &rcDesktop);			
	GetWindowRect(hDlg, &rcDlg);

	w = rcDlg.right - rcDlg.left;
	h = rcDlg.bottom - rcDlg.top;
	x = rcDesktop.left + (rcDesktop.right - w) / 2;
	y = rcDesktop.top + (rcDesktop.bottom - h) / 2;

	MoveWindow(	hDlg, x, y, w, h, TRUE );

} // CenterDialog


#endif /* _WINDOWS */





