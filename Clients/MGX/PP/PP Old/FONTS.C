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

/***********************************************************************/
int FAR PASCAL EnumFunc(lpLogFont, lpTextMetric, FontType, lpData)
// PURPOSE: Initializes window data and registers window class
/***********************************************************************/
LPLOGFONT lpLogFont;
LPTEXTMETRIC lpTextMetric;
short FontType;
LPTR lpData;
{
switch (LOWORD(lpData))
	{
	case 0:
		if ( Text.FontCount >= MAX_FONTS )
			return( 0 );
		lstrcpy( Text.FontList[Text.FontCount],
			lpLogFont->lfFaceName );
		Text.CharSet[Text.FontCount] = lpLogFont->lfCharSet;
		Text.PitchAndFamily[Text.FontCount] = lpLogFont->lfPitchAndFamily;
		return( ++Text.FontCount );
	case 1:
		if ( Text.SizeCount >= MAX_FONT_SIZES )
			return( 0 );
		Text.SizeList[Text.SizeCount] = lpLogFont->lfHeight;
		return( ++Text.SizeCount );
	}

return( 0 );
}


/***********************************************************************/
void GetFonts(hWnd)
/***********************************************************************/
HWND hWnd;
{
HDC hDC;
FARPROC lpEnumFunc;

Text.FontCount = 0;
Text.SizeCount = 0;
hDC = GetDC(hWnd);
lpEnumFunc = MakeProcInstance(EnumFunc, hInstAstral);
EnumFonts(hDC, NULL, lpEnumFunc, (LPTR)0L);
FreeProcInstance(lpEnumFunc);
ReleaseDC(hWnd, hDC);
SortFonts();
}

/***********************************************************************/
void SortFonts()
/***********************************************************************/
{
int i, j, n;
char tempFace[32];
BYTE tempPitchAndFamily;
BYTE tempCharSet;

n = Text.FontCount;
for (i = 1; i < n; ++i)
	{
	lstrcpy(tempFace, Text.FontList[i]);
	tempPitchAndFamily = Text.PitchAndFamily[i];
	tempCharSet = Text.CharSet[i];
	for (j = i-1; j >= 0 && lstrcmp(Text.FontList[j], tempFace) > 0; --j)
		{
		lstrcpy(Text.FontList[j+1], Text.FontList[j]);
		Text.PitchAndFamily[j+1] = Text.PitchAndFamily[j];
		Text.CharSet[j+1] = Text.CharSet[j];
		}
	lstrcpy(Text.FontList[j+1], tempFace);
	Text.PitchAndFamily[j+1] = tempPitchAndFamily;
	Text.CharSet[j+1] = tempCharSet;
	}
}

/***********************************************************************/
void GetSizes(hWnd, CurrentFont)
/***********************************************************************/
HWND hWnd;
int CurrentFont;
{
HDC hDC;
FARPROC lpEnumFunc;

Text.SizeCount = 0;
hDC = GetDC(hWnd);
lpEnumFunc = MakeProcInstance(EnumFunc, hInstAstral);
EnumFonts(hDC, Text.FontList[CurrentFont], lpEnumFunc, (LPTR)1L);
FreeProcInstance(lpEnumFunc);
ReleaseDC(hWnd, hDC);
}


/***********************************************************************/
short GetStringExtent(hDC, lpString, hFont)
// PURPOSE: get the string extent
/***********************************************************************/
HDC hDC;
LPTR lpString;
HFONT hFont;
{
HFONT hOldFont;
DWORD dwExtent;

if ( !(hOldFont = SelectObject(hDC, hFont)) )
	return( 0 );
dwExtent = GetTextExtent(hDC, lpString, lstrlen(lpString));
SelectObject(hDC, hOldFont);
return( LOWORD(dwExtent) );
}


/***********************************************************************/
WORD StringOut(hDC, x, y, lpString, hFont)
// PURPOSE: Sends string to application's window
/***********************************************************************/
HDC hDC;
int x, y;
LPTR lpString;
HFONT hFont;
{
HFONT hOldFont;
DWORD dwExtent;

if ( !(hOldFont = SelectObject(hDC, hFont)) )
	return( 0 );
dwExtent = GetTextExtent(hDC, lpString, lstrlen(lpString));
TextOut(hDC, x, y, lpString, lstrlen(lpString));
SelectObject(hDC, hOldFont);
return( LOWORD(dwExtent) );
}

#ifdef UNUSED

HANDLE hInst;
HFONT hOFont, hFFont, hVFont, hSFont, hDFont, hMFont, hFont;
int hFile;
char line[4][64];
char FontNameList[32][128];			  /* list of added fonts  */
int nFontIndex = 0;				  /* position in Text.FontList */
int nLineSpace;

TEXTMETRIC TextMetric;
LOGFONT LogFont;
FARPROC lpCFontDlg;
POINT ptCurrent = {0, 0};
short nBkMode = OPAQUE;
DWORD rgbBkColor = RGB(255, 255, 255);
DWORD rgbTextColor = RGB(0, 0, 0);
DWORD rgbColor;
short nAlignLCR = TA_LEFT;
short nAlignTBB = TA_TOP; 
WORD wPaint = 0;
BYTE CharSet[MAXFONT];
BYTE PitchAndFamily[MAXFONT];
int CurrentFont = 0;
int CurrentSize = 0;
FARPROC lpSelectFont;
FARPROC lpEnumFunc;
WORD wPrevVAlign = IDM_ALIGNBASE;
WORD wPrevHAlign = IDM_ALIGNLEFT;
WORD wPrevFont = IDM_SYSTEM;
char AppName[] = "ShowFont Sample Application   Font: ";
char WindowTitle[80];
char str[255];
char DefPath[128];
char DefExt[] = ".fon";
char DefSpec[13];
char FontFileName[128];

/****************************************************************************

FUNCTION: ShowSampleString(HWND)

PURPOSE: Show sample string in current font

****************************************************************************/

void ShowSampleString(hWnd)
HWND hWnd;
{
HFONT hItalicFont;
HFONT hBoldFont;
HFONT hUnderlineFont;
HFONT hStrikeOutFont;
HDC hDC;
short X, tmpX;
short Y;
short nAlign;

GetObject(hFont, sizeof(LOGFONT), (LPSTR) &LogFont);
LogFont.lfItalic = TRUE;
hItalicFont = CreateFontIndirect(&LogFont);
LogFont.lfItalic = FALSE;
LogFont.lfUnderline = TRUE;
hUnderlineFont = CreateFontIndirect(&LogFont);
LogFont.lfUnderline = FALSE;
LogFont.lfStrikeOut = TRUE;
hStrikeOutFont = CreateFontIndirect(&LogFont);
LogFont.lfStrikeOut = FALSE;
LogFont.lfWeight = FW_BOLD;
hBoldFont = CreateFontIndirect(&LogFont);

hDC=GetDC(hWnd);
X=ptCurrent.x;
Y=ptCurrent.y;
nAlign =  nAlignLCR | nAlignTBB;		   /* GetTextAlign(hDC); */
if ((nAlign & TA_CENTER) == TA_CENTER)
	{
	tmpX = X;
	nAlignLCR = TA_LEFT;
	SetTextAlign(hDC, nAlignLCR | nAlignTBB);
	X += StringOut(hDC, X, Y, ", and ", hFont);
	X += StringOut(hDC, X, Y, "strikeout", hStrikeOutFont);
	X += StringOut(hDC, X, Y, " in a single line.", hFont);
	X = tmpX;
	nAlignLCR = TA_RIGHT;
	SetTextAlign(hDC, nAlignLCR | nAlignTBB);
	X -= StringOut(hDC, X, Y, "underline", hUnderlineFont);
	X -= StringOut(hDC, X, Y, ", ", hFont);
	X -= StringOut(hDC, X, Y, "italic", hItalicFont);
	X -= StringOut(hDC, X, Y, ", ", hFont);
	X -= StringOut(hDC, X, Y, "bold", hBoldFont);
	X -= StringOut(hDC, X, Y, "You can use ", hFont);
	nAlignLCR = TA_CENTER;
	}
else
if ((nAlign & TA_CENTER) == TA_RIGHT)
	{
	X -= StringOut(hDC, X, Y, " in a single line.", hFont);
	X -= StringOut(hDC, X, Y, "strikeout", hStrikeOutFont);
	X -= StringOut(hDC, X, Y, ", and ", hFont);
	X -= StringOut(hDC, X, Y, "underline", hUnderlineFont);
	X -= StringOut(hDC, X, Y, ", ", hFont);
	X -= StringOut(hDC, X, Y, "italic", hItalicFont);
	X -= StringOut(hDC, X, Y, ", ", hFont);
	X -= StringOut(hDC, X, Y, "bold", hBoldFont);
	X -= StringOut(hDC, X, Y, "You can use ", hFont);
	}
else	{
	X += StringOut(hDC, X, Y, "You can use ", hFont);
	X += StringOut(hDC, X, Y, "bold", hBoldFont);
	X += StringOut(hDC, X, Y, ", ", hFont);
	X += StringOut(hDC, X, Y, "italic", hItalicFont);
	X += StringOut(hDC, X, Y, ", ", hFont);
	X += StringOut(hDC, X, Y, "underline", hUnderlineFont);
	X += StringOut(hDC, X, Y, ", and ", hFont);
	X += StringOut(hDC, X, Y, "strikeout", hStrikeOutFont);
	X += StringOut(hDC, X, Y, " in a single line.", hFont);
	}
ReleaseDC(hWnd, hDC);

DeleteObject(hItalicFont);
DeleteObject(hUnderlineFont);
DeleteObject(hStrikeOutFont);
DeleteObject(hBoldFont);
}

/****************************************************************************

FUNCTION: ShowCharacterSet(HDC, HFONT)

PURPOSE: display character set using current font

****************************************************************************/

void ShowCharacterSet(hDC, hFont)
HDC hDC;
HFONT hFont;
{
HFONT hOldFont;
TEXTMETRIC TextMetric;
int LineSpace;
short X;
short Y;

if (!(hOldFont = SelectObject(hDC, hFont)))
	return;
GetTextMetrics(hDC, &TextMetric);
nLineSpace = (TextMetric.tmHeight + TextMetric.tmExternalLeading)*2;
X = ptCurrent.x;
Y = ptCurrent.y;
TextOut(hDC, X, Y, line[0], 64);
TextOut(hDC, X, Y += nLineSpace, line[1], 64);
TextOut(hDC, X, Y += nLineSpace, line[2], 64);
TextOut(hDC, X, Y += nLineSpace, line[3], 64);
SelectObject(hDC, hOldFont);
}


/***********************************************************************/
long FAR PASCAL ShowFontWndProc(hWnd, message, wParam, lParam)
/***********************************************************************/
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
FARPROC lpProcAbout, lpAddDlg, lpRemoveDlg;
HDC hDC;
PAINTSTRUCT ps;
HFONT hOldFont;
int i;
short Y;
char buf[80];

switch(message) {
	case WM_CREATE:
		GetFonts(hWnd);
		hMFont = CreateFont(
		10,					/* height	   */
		10,				/* width		*/
		0,					/* escapement	*/
		0,					/* orientation	*/
		FW_NORMAL,				/* weight	   */
		FALSE,				   /* italic	   */
		FALSE,				   /* underline	*/
		FALSE,				   /* strikeout	*/
		OEM_CHARSET,				 /* charset	  */
		OUT_DEFAULT_PRECIS,			/* out precision	*/
		CLIP_DEFAULT_PRECIS,			 /* clip precision   */
		DEFAULT_QUALITY,			 /* quality	  */
		FIXED_PITCH | FF_MODERN,		 /* pitch and family */
		"Courier");				/* typeface	 */
		hOFont = GetStockObject(OEM_FIXED_FONT);
		hFFont = GetStockObject(ANSI_FIXED_FONT);
		hVFont = GetStockObject(ANSI_VAR_FONT);
		hSFont = GetStockObject(SYSTEM_FONT);
		hDFont = GetStockObject(DEVICE_DEFAULT_FONT);
		hFont = hSFont;
		GetObject(hFont, sizeof(LOGFONT), (LPSTR) &LogFont);
		strcpy(WindowTitle, AppName);
		strcat(WindowTitle, "SYSTEM");
		SetWindowText(hWnd, (LPSTR) WindowTitle);
		break;

	case WM_COMMAND:
		if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
		switch (wParam) {

		/* File menu */

		case IDM_ADDFONT:
			/* Tell Windows to add the font resource */
			AddFontResource((LPSTR) FontFileName);
			/* Let all applications know there is a new font */
			SendMessage((HWND) 0xFFFF, WM_FONTCHANGE, NULL,
				(LONG) NULL);
			/* Copy the name selected to the list of fonts added */
			strcpy(&FontNameList[nFontIndex++][0], FontFileName);
			break;

		case IDM_DELFONT:
			RemoveFontResource((LPSTR) FontFileName);
			SendMessage((HWND) 0xFFFF, WM_FONTCHANGE, NULL,
				(LONG) NULL);
			strcpy(&FontNameList[i][0],
				&FontNameList[--nFontIndex][0]);
			break;

		case IDM_SELECTFONT:
			DeleteObject(hMFont);
			hMFont = CreateFont(
				Text.SizeList[CurrentSize],
				0,
				0,
				0,
				FW_NORMAL,
				FALSE,
				FALSE,
				FALSE,
				CharSet[CurrentFont],
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				PitchAndFamily[CurrentFont],
				Text.FontList[CurrentFont]);
			hFont = hMFont;
			break;

		case IDM_CFONT:
			GetObject(hMFont, sizeof(LOGFONT), (LPSTR) &CLogFont);
			DeleteObject(hMFont);
			hMFont = CreateFontIndirect(&CLogFont);
			hFont = hMFont;
			break;
		}
		break;

	case WM_FONTCHANGE:
		GetFonts(hWnd);
		break;

	case WM_DESTROY:
		/* Remove any fonts that were added */
		for (i = 0; i < nFontIndex; i++)
			RemoveFontResource((LPSTR) &FontNameList[i][0]);
		/* Notify any other apps know the fonts have been deleted */
		SendMessage((HWND) 0xFFFF, WM_FONTCHANGE, NULL, (LONG) NULL);
		PostQuitMessage(0);
		break;

	default:
		return (DefWindowProc(hWnd, message, wParam, lParam));
}
return (0L);
}


/****************************************************************************

FUNCTION: Metric(HWND, unsigned, WORD, LONG)

PURPOSE: Modeless dialog box to display metric font information

****************************************************************************/

BOOL FAR PASCAL Metric(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
switch (message) {
	case WM_INITDIALOG:
		SetDlgItemInt(hDlg, IDMB_HEIGHT, TextMetric.tmHeight, FALSE);
		SetDlgItemInt(hDlg, IDMB_ASCENT, TextMetric.tmAscent, FALSE);
		SetDlgItemInt(hDlg, IDMB_DESCENT, TextMetric.tmDescent, FALSE);
		SetDlgItemInt(hDlg, IDMB_INTERNALLEADING,
		TextMetric.tmInternalLeading, FALSE);
		SetDlgItemInt(hDlg, IDMB_EXTERNALLEADING,
		TextMetric.tmExternalLeading, FALSE);
		SetDlgItemInt(hDlg, IDMB_AVECHARWIDTH, TextMetric.tmAveCharWidth,
		FALSE);
		SetDlgItemInt(hDlg, IDMB_MAXCHARWIDTH, TextMetric.tmMaxCharWidth,
		FALSE);
		SetDlgItemInt(hDlg, IDMB_WEIGHT, TextMetric.tmWeight, FALSE);
		SetDlgItemInt(hDlg, IDMB_ITALIC, TextMetric.tmItalic, FALSE);
		SetDlgItemInt(hDlg, IDMB_UNDERLINED, TextMetric.tmUnderlined,
		FALSE);
		SetDlgItemInt(hDlg, IDMB_STRUCKOUT, TextMetric.tmStruckOut, FALSE);
		SetDlgItemInt(hDlg, IDMB_FIRSTCHAR, TextMetric.tmFirstChar, FALSE);
		SetDlgItemInt(hDlg, IDMB_LASTCHAR, TextMetric.tmLastChar, FALSE);
		SetDlgItemInt(hDlg, IDMB_DEFAULTCHAR, TextMetric.tmDefaultChar,
		FALSE);
		SetDlgItemInt(hDlg, IDMB_BREAKCHAR, TextMetric.tmBreakChar, FALSE);
		SetDlgItemInt(hDlg, IDMB_PITCHANDFAMILY,
		TextMetric.tmPitchAndFamily, FALSE);
		SetDlgItemInt(hDlg, IDMB_CHARSET, TextMetric.tmCharSet, FALSE);
		SetDlgItemInt(hDlg, IDMB_OVERHANG, TextMetric.tmOverhang, FALSE);
		SetDlgItemInt(hDlg, IDMB_DIGITIZEDASPECTX,
		TextMetric.tmDigitizedAspectX, FALSE);
		SetDlgItemInt(hDlg, IDMB_DIGITIZEDASPECTY,
		TextMetric.tmDigitizedAspectY, FALSE);
		return (TRUE);

	case WM_CLOSE:
		DestroyWindow(hDlg);
		break;
}
return (FALSE);
}

/****************************************************************************

FUNCTION: Log(HWND, unsigned, WORD, LONG)

PURPOSE: Displays logical font information

****************************************************************************/

BOOL FAR PASCAL Log(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{

switch (message) {
	case WM_INITDIALOG:
		SetDlgItemInt(hDlg, IDMI_HEIGHT, LogFont.lfHeight, FALSE);
		SetDlgItemInt(hDlg, IDMI_WIDTH, LogFont.lfWidth, FALSE);
		SetDlgItemInt(hDlg, IDMI_ESCAPEMENT, LogFont.lfEscapement, FALSE);
		SetDlgItemInt(hDlg, IDMI_ORIENTATION, LogFont.lfOrientation, FALSE);
		SetDlgItemInt(hDlg, IDMI_WEIGHT, LogFont.lfWeight, FALSE);
		SetDlgItemInt(hDlg, IDMI_ITALIC, LogFont.lfItalic, FALSE);
		SetDlgItemInt(hDlg, IDMI_UNDERLINED, LogFont.lfUnderline, FALSE);
		SetDlgItemInt(hDlg, IDMI_STRIKEOUT, LogFont.lfStrikeOut, FALSE);
		SetDlgItemInt(hDlg, IDMI_CHARSET, LogFont.lfCharSet, FALSE);
		SetDlgItemInt(hDlg, IDMI_OUTPRECISION, LogFont.lfOutPrecision,
		FALSE);
		SetDlgItemInt(hDlg, IDMI_CLIPPRECISION, LogFont.lfClipPrecision,
		FALSE);
		SetDlgItemInt(hDlg, IDMI_QUALITY, LogFont.lfQuality, FALSE);
		SetDlgItemInt(hDlg, IDMI_PITCHANDFAMILY,
		LogFont.lfPitchAndFamily, FALSE);
		return (TRUE);

	case WM_CLOSE:
		DestroyWindow(hDlg);
		break;

}
return (FALSE);
}

/****************************************************************************

FUNCTION: AddDlg(HWND, unsigned, WORD, LONG)

PURPOSE: Used to add a font

COMMENTS:

	This dialog box displays all the availble font files on the currently
	selected directory, and lets the user select a font to add.

****************************************************************************/

BOOL FAR PASCAL AddDlg(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
switch (message) {
	case WM_INITDIALOG:
		SetWindowText(hDlg, (LPSTR) "Add Font Resource");
		strcpy(DefSpec, "*.fon");
//		UpdateListBox(hDlg);
		SetDlgItemText(hDlg, ID_EDIT, DefSpec);
		SendDlgItemMessage(hDlg, ID_EDIT, EM_SETSEL, NULL,
		MAKELONG(0, 0x7fff));
		SetFocus(GetDlgItem(hDlg, ID_EDIT));
		return (FALSE);

	case WM_COMMAND:
		if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
		switch (wParam) {
		case ID_LISTBOX:

			switch (HIWORD(lParam)) {
			case LBN_SELCHANGE:
				/* If item is a directory name, append "*.*" */
				if (DlgDirSelect(hDlg, str, ID_LISTBOX)) 
				strcat(str, DefSpec);

				SetDlgItemText(hDlg, ID_EDIT, str);
				SendDlgItemMessage(hDlg,
				ID_EDIT,
				EM_SETSEL,
				NULL,
				MAKELONG(0, 0x7fff));
				break;

			case LBN_DBLCLK:
				goto CheckSelection;
				break;

			}				/* end of ID_LISTBOX case */
			return (TRUE);

		case IDOK:
CheckSelection:
			/* Get the filename from the edit control */

			GetDlgItemText(hDlg, ID_EDIT, str, 128);
			GetDlgItemText(hDlg, ID_PATH, DefPath, 128);

			/* Check for wildcard.  If found, use the string as a new
			 * search path.
			 */

			if (strchr(str, '*') ||
			strchr(str, '?')) {

			/* Separate filename from path.  The path is stored in
			 * str which is discarded if null, else it is used for a new
			 * search path.
			 */

//			SeparateFile(hDlg, (LPSTR) str, (LPSTR) DefSpec,
//			  (LPSTR) str);
			if (str[0])
				strcpy(DefPath, str);

//			UpdateListBox(hDlg);
			return (TRUE);
			}

			/* Ignore it if no filename specified */

			if (!str[0]) {
			MessageBox(hDlg, "No filename specified.",
				NULL, MB_OK | MB_ICONQUESTION);
			return (TRUE);
			}

			/* Append the default extension if needed */

			strcpy(FontFileName, DefPath);
			strcat(FontFileName, str);
//			AddExt(FontFileName, DefExt);
			EndDialog(hDlg, TRUE);
			return (TRUE);

		case IDCANCEL:

			/* Let the caller know the user cancelled */

			EndDialog(hDlg, FALSE);
			return (TRUE);
		}
		break;
}
return FALSE;
}

/****************************************************************************

FUNCTION: RemoveDlg(HANDLE)

PURPOSE: Used to remove a font

COMMENTS:

	This dialog box displays all fonts which have been added to the system,
	and lets the user select which font to delete.

****************************************************************************/

BOOL FAR PASCAL RemoveDlg(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
WORD index;
int i;

switch (message) {
	case WM_INITDIALOG:
		SetWindowText(hDlg, (LPSTR) "Remove Font Resource");
		for (i = 0; i < nFontIndex; i++)
		SendDlgItemMessage(hDlg,
			ID_LISTBOX,
			LB_ADDSTRING,
			NULL,
			(LONG)(char far *) &FontNameList[i][0]);

		SetFocus(GetDlgItem(hDlg, ID_EDIT));
		return (FALSE);

	case WM_COMMAND:
		if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
		switch (wParam) {
		case ID_LISTBOX:

			switch (HIWORD(lParam)) {
			case LBN_SELCHANGE:
				index = SendDlgItemMessage(hDlg,
				ID_LISTBOX,
				LB_GETCURSEL,	 /* get index command   */
				NULL,
				(LONG) NULL);
				SendDlgItemMessage(hDlg,
				ID_LISTBOX,
				LB_GETTEXT,	   /* copy string command */
				index,
				(LONG) (LPSTR) FontFileName);
				SetDlgItemText(hDlg, ID_EDIT, FontFileName);
				break;

			case LBN_DBLCLK:
				GetDlgItemText(hDlg, ID_EDIT, FontFileName, 128);
				EndDialog(hDlg, TRUE);
				return (TRUE);
			}
			return (TRUE);

		case IDOK:

			/* Get the filename from the edit control */

			GetDlgItemText(hDlg, ID_EDIT, FontFileName, 128);

			/* Ignore it if no filename specified */

			if (!FontFileName[0]) {
			MessageBox(hDlg, "No filename specified.",
				NULL, MB_OK | MB_ICONQUESTION);
			return (TRUE);
			}

			EndDialog(hDlg, TRUE);
			return (TRUE);

		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			return (TRUE);
		}
		break;
}
return FALSE;
}
#endif
