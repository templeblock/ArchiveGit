//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "cpo.h"
#include "ctxhelp.h"

// Static Prototypes
static void PaletteInit(HWND hDlg);
static BOOL PaletteClose(HWND hDlg, BOOL bHaveToClose);
static void PaletteCommand(HWND hDlg, int id, UINT codeNotify);
static void SetPalette(HWND hDlg, LPPALETTE lpPalette, BOOL fNew);
static BOOL CreateNewEntries(LPPALETTE lpHeadPalette, int iPalette, int iAfter, int iEntries, BOOL bDoCPO);
static BOOL DoUndo(HWND hDlg,int iFunction, int iEntry1, int iEntry2, LPCOLORINFO lpColor1, LPCOLORINFO lpColor2, int iCount);
static void SavePalettePreferences( HWND hDlg );
static void LoadPalettePreferences( HWND hDlg );
static void SetPaletteMenus(HWND hDlg);
static void SetSizePalette( HWND hDlg, int iRows, int iCols );
static void GetSizePalette( HWND hDlg, LPINT lpRows, LPINT lpCols );

struct PALETTEDATA
{
	HWND 		hPalDlg;
	// local storage values that need to be saved in pp.ini
	FNAME 		szPalette;	// Active Palette name
	int			iActivePalette;	// Always indicates active palette
	BOOL		bFillMax;	// Fill maximum amount of entries
	int			iFill;		// Number of entries to fill
	int			iNewFill;	// Number of entries for new fill
	BOOL		bFillHSL;	// do an HSL fill instead of RGB
	BOOL		bNewCPO;	// Add a CPO palette
	BOOL		bAddCPO;	// Add a CPO palette
	int			iAddEntries;	// Number of Entries in Add Palette
	BOOL		bSetActive;	// Set ActiveRGB on entry change
	
	// local storage
	LPPALETTE	lpPaletteList;	// Pointer to list of palettes
	int 		iActiveEntry;	// Index of active palette entry
	BOOL		bUntitled;	// Palette opened from a file?
	BOOL		bChanged;	// Has the palette been changed
	int 		iUndoFunction;		// Current function to be undone
	BOOL		bRedo;			// Redo or Undo?
	int			iOldWidth, iOldHeight;
	int			PaletteRows, PaletteCols;
	int			iOldPalWidth, iOldPalHeight;
	int			iOldLabelWidth, iOldLabelHeight;
};

static  BOOL		bVertical;	// Is the scroll bar vertical

typedef PALETTEDATA FAR * LPPALETTEDATA;
#define LockPaletteData(hDlg) ((LPPALETTEDATA)GetWindowLong(hDlg, DWL_USER))
#define UnlockPaletteData(hDlg,lpData) (SetWindowLong(hDlg, DWL_USER, (LONG)lpData))

/***********************************************************************/
static void PaletteInit(HWND hDlg)
/***********************************************************************/
{
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) return;

LPPALETTE lpPalette;
FNAME szFileName;

LoadPalettePreferences( hDlg );
lpData->hPalDlg = hDlg;
lpData->bRedo = lpData->iUndoFunction = 0;
lpData->bUntitled = YES;
lpPalette = NULL;
if (lstrlen(lpData->szPalette))
	{
	if ( LookupExtFile( lpData->szPalette, szFileName, IDN_PALETTE ) &&
		(lpPalette = Palette_Read(szFileName,YES)))
		{
		lpData->iActivePalette = bound(lpData->iActivePalette,
				0,Palette_GetCount(lpPalette)-1);
		SetWindowText(hDlg, lpData->szPalette);
		lpData->bUntitled = NO;
		lpData->bChanged = NO;
		}
	}
lpData->iActiveEntry = 0;
SetPalette(hDlg, lpPalette, YES);

UnlockPaletteData(hDlg,lpData);
}


/***********************************************************************/
BOOL PaletteDlgClose(BOOL bHaveToClose,HWND hThisDlg/* NULL */)
/***********************************************************************/
{
HWND hDlg;

if (!hThisDlg)
{
	if ( !(hDlg = AstralDlgGet(IDD_PALETTE)) )
		return( YES );
}
else
	hDlg = hThisDlg;

LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) return(YES);

SavePalettePreferences( hDlg );

if ( !lpData->lpPaletteList )
	return( YES );

// if a current palette see if user wants to save
if (!PaletteClose(hDlg, bHaveToClose) && !bHaveToClose)
	return(NO);

return(YES);
}

/***********************************************************************/
static BOOL PaletteClose( HWND hDlg, BOOL bHaveToClose )
/***********************************************************************/
{
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) return(YES);

int ret;

if (!lpData->lpPaletteList || !lpData->bChanged)
	return(YES);

if (bHaveToClose)
	{
	if (AstralAffirm( IDS_AREYOUSURE, (LPSTR)lpData->szPalette ))
		SendMessage(hDlg, WM_COMMAND, IDC_PALSAVE, 0L);
	return(YES);
	}
else
	{
	ret = AstralConfirm( IDS_AREYOUSURE, (LPSTR)lpData->szPalette );
	if ( ret == IDNO )
		return(YES);
	else if ( ret == IDYES )
		{
		SendMessage(hDlg, WM_COMMAND, IDC_PALSAVE, 0L);
		return(YES);
		}
	else
		return(NO);
	}

UnlockPaletteData(hDlg,lpData);
}

/***********************************************************************/
static void PaletteCommand(HWND hDlg, int id, UINT codeNotify)
/***********************************************************************/
{
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) return;

LPPALETTE lpPalette;
STRING	szString;
WORD wOption;
int iEntry;
FNAME szFileName;

switch (id)
	{
	case IDC_PALDELETEITEM:
		lpData->bRedo = lpData->iUndoFunction = 0;
		DoUndo(hDlg,id, lpData->iActiveEntry, 0, NULL, NULL, 0);
		if (Palette_DeleteEntry(lpData->lpPaletteList, lpData->iActivePalette, 
			lpData->iActiveEntry))
			lpData->iUndoFunction = IDS_UNDODELETE;
		lpPalette = Palette_Get(lpData->lpPaletteList, NULL, 
			lpData->iActivePalette);
		if (lpPalette && lpData->iActiveEntry >= lpPalette->iColors)
			--lpData->iActiveEntry;
		SetPalette(hDlg, lpPalette, NO);
		lpData->bChanged = lpData->bChanged || (lpData->iUndoFunction != 0);
		break;

	case IDC_PALINSERT:
	case IDC_PALAPPEND:
		InsertPaletteColor(hDlg, lpData->iActiveEntry, GetActiveColorInfo(),
			SHIFT/*Insert After?*/ );
		break;

	case IDC_PALUNDO:
		if (!lpData->iUndoFunction)
			break;
		if (DoUndo(hDlg,id, 0, 0, NULL, NULL, 0))
			{
			if (lpData->iUndoFunction == IDS_UNDOLABEL)
				{
				Palette_GetEntryLabel(lpData->lpPaletteList, 
					lpData->iActivePalette,lpData->iActiveEntry, szString);
				SetDlgItemTextNow(hDlg, IDC_COLORLABEL, szString);
				}
			else
				{
				SetPalette(hDlg, NULL, NO);
				}
			lpData->bRedo = !lpData->bRedo;
			}
		break;

	case IDC_PALNEW:
		{
		if (!PaletteClose(hDlg, NO))
			break;
		if ( !AstralDlgParam( NO, 
			PictPubApp.GetResourceHandle(), hDlg, IDD_PALETTENEW,
			DlgPaletteNewProc,(LPARAM)lpData ) )
			break;
		SetWindowText(hDlg, lpData->szPalette);
		lpData->bUntitled = YES;
		lpData->bChanged = YES;
		}
		break;

	case IDC_PALOPEN:
		if (!PaletteClose(hDlg, NO))
			break;
		if ( !AstralDlgParam( NO, PictPubApp.GetResourceHandle(), hDlg, IDD_PALETTELOAD,
			   DlgPaletteLoadProc, (LPARAM)lpData ) )
			break;
		SetWindowText(hDlg, lpData->szPalette);
		lpData->bUntitled = NO;
		lpData->bChanged = NO;
		break;

    case IDC_PALSAVE:
		if (!lpData->bUntitled)
			{
			lpData->bRedo = lpData->iUndoFunction = 0;
			if (!lpData->lpPaletteList)
				break;
			if ( !LookupExtFile( lpData->szPalette, szFileName,IDN_PALETTE))
				break;
			lstrcpy(lpData->lpPaletteList->szName, lpData->szPalette);
			if (Palette_Write(szFileName, lpData->lpPaletteList))
				lpData->bChanged = NO;
			break;
			}
	// else fall thru...

    case IDC_PALSAVEAS:
		lpData->bRedo = lpData->iUndoFunction = 0;
		if ( !AstralDlgParam( NO, PictPubApp.GetResourceHandle(), hDlg, IDD_PALETTESAVE,
			   DlgPaletteSaveProc, (LPARAM)lpData ) )
			break;
		SetWindowText(hDlg, lpData->szPalette);
		lpData->bChanged = NO;
		break;

	case IDC_PALLABEL:
		if ( !AstralDlgParam( NO, PictPubApp.GetResourceHandle(), hDlg, IDD_PALETTELABEL,
			   DlgLabelProc, (LPARAM)lpData ) )
			break;
		lpData->iUndoFunction = IDS_UNDOLABEL;
		Palette_GetEntryLabel(lpData->lpPaletteList, 
			lpData->iActivePalette, lpData->iActiveEntry,
			szString);
		SetDlgItemTextNow(hDlg, IDC_COLORLABEL, szString);
		lpData->bChanged = lpData->bChanged || (lpData->iUndoFunction != 0);
		break;

	case IDC_PALFILL:
		if ( !AstralDlgParam( NO, PictPubApp.GetResourceHandle(), hDlg, IDD_PALETTEFILL,
			   DlgFillPaletteProc, (LPARAM)lpData ) )
			break;
		lpData->iUndoFunction = IDS_UNDOFILL;
		SetPalette(hDlg, NULL, NO);
		lpData->bChanged = lpData->bChanged || (lpData->iUndoFunction != 0);
		break;

	case IDC_PALETTE:
		wOption = codeNotify;
		iEntry = GetWindowLong(GetDlgItem(hDlg, id), GWL_SELECT);
		if ( iEntry < 0 )
			{
			SetDlgItemTextNow( hDlg, IDC_COLORLABEL, "" );
			break;
			}
		if ( !(lpPalette = Palette_Get(lpData->lpPaletteList, NULL, 
			lpData->iActivePalette)) )
			break;
		if ( wOption != 3 ) // user is browsing over color
			lpData->iActiveEntry = iEntry;
		if ( wOption == 1 )
			{
			lpData->bRedo = lpData->iUndoFunction = 0;
			DoUndo(hDlg,id, lpData->iActiveEntry, 0, NULL, NULL, 0);
			if ( !ColorPicker(&lpPalette->lpColorInfo[lpData->iActiveEntry], 
				NULL) )
				break;
			lpData->iUndoFunction = IDS_UNDOCOLOR;
			SetPalette(hDlg, lpPalette, NO);
			}
		Palette_GetEntryLabel(lpData->lpPaletteList, lpData->iActivePalette, 
			iEntry, szString);
		SetDlgItemTextNow(hDlg, IDC_COLORLABEL, szString);
		ColorStatus(&lpPalette->lpColorInfo[iEntry], 0L, szString, YES);
		if ( lpData->bSetActive )
			{
			if (wOption == 0)
				SetActiveColor(&lpPalette->lpColorInfo[lpData->iActiveEntry],
					lpPalette->lpColorInfo[lpData->iActiveEntry].ColorSpace, 
					FALSE );
			else if (wOption == 2)
				SetAlternateColor(&lpPalette->lpColorInfo[lpData->iActiveEntry],
					lpPalette->lpColorInfo[lpData->iActiveEntry].ColorSpace, 
					FALSE );
			}
		lpData->bChanged = lpData->bChanged || (lpData->iUndoFunction != 0);
		break;
	}

UnlockPaletteData(hDlg,lpData);

}

/***********************************************************************/
void MovePaletteColor(HWND hDlg, int iOldEntry, int iNewEntry)
/***********************************************************************/
{
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) return;

if (iOldEntry == iNewEntry)
	return;

lpData->bRedo = lpData->iUndoFunction = 0;
DoUndo(hDlg,IDC_PALMOVE, iOldEntry, iNewEntry, NULL, NULL, 0);
if (Palette_MoveEntry(lpData->lpPaletteList, lpData->iActivePalette, 
	iOldEntry, iNewEntry))
	{
	lpData->iActiveEntry = iNewEntry;
	lpData->iUndoFunction = IDS_UNDOMOVE;
	}
SetPalette(hDlg, NULL, NO);
lpData->bChanged = lpData->bChanged || (lpData->iUndoFunction != 0);

UnlockPaletteData(hDlg,lpData);

}

/***********************************************************************/
void InsertPaletteColor(HWND hDlg, int iEntry, LPCOLORINFO lpColorInfo, BOOL fInsert)
/***********************************************************************/
{
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) return;

lpData->iActiveEntry = iEntry;
lpData->bRedo = lpData->iUndoFunction = 0;
DoUndo(hDlg,fInsert ? IDC_PALINSERT : IDC_PALAPPEND, lpData->iActiveEntry, 0, 
	lpColorInfo, NULL, 0);
if (fInsert)
	{
	if (Palette_InsertEntry(lpData->lpPaletteList, lpData->iActivePalette,
					   lpData->iActiveEntry, lpColorInfo))
		lpData->iUndoFunction = IDS_UNDOINSERT;
	}
else
	{
	if (Palette_AppendEntry(lpData->lpPaletteList, lpData->iActivePalette,
				lpData->iActiveEntry, lpColorInfo))
		{
		lpData->iUndoFunction = IDS_UNDOAPPEND;
		++lpData->iActiveEntry;
		}
	}
SetPalette(hDlg, NULL, NO);
lpData->bChanged = lpData->bChanged || (lpData->iUndoFunction != 0);

UnlockPaletteData(hDlg,lpData);
}

/***********************************************************************/
static void SetPalette(HWND hDlg, LPPALETTE lpPalette, BOOL fNew)
/***********************************************************************/
{
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) return;

LPPALETTE lpHeadPalette;
//int PaletteRows, PaletteCols;

if ( !lpPalette )
	lpPalette = Palette_Get(lpData->lpPaletteList, NULL, lpData->iActivePalette);
lpHeadPalette = lpPalette;
if (lpData->lpPaletteList && lpData->lpPaletteList != lpHeadPalette)
	Palette_FreeUp(lpData->lpPaletteList);
lpPalette = Palette_Get(lpHeadPalette, NULL, lpData->iActivePalette);
lpData->lpPaletteList = lpHeadPalette;
SendMessage(GetDlgItem(hDlg, IDC_PALETTE), WM_SETPALETTE,
			    (WORD)(fNew ? -1 : lpData->iActiveEntry), (long)lpPalette);
if (fNew)
    {
        // adjust for scroll bars
//        GetSizePalette( hDlg, &PaletteRows, &PaletteCols );
        SetSizePalette( hDlg, lpData->PaletteRows, lpData->PaletteCols );
    }

UnlockPaletteData(hDlg,lpData);
}

// LOCAL prototypes
LOCAL BOOL PaletteDlg_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void PaletteDlg_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
void PaletteDlg_OnClose(HWND hDlg);
LOCAL void PaletteDlg_OnSize(HWND hDlg, UINT state, int cx, int cy);
BOOL PaletteDlg_OnSetCursor(HWND hDlg, HWND hWndCursor, UINT codeHitTest, UINT msg);
LOCAL void PaletteDlg_OnInitMenu(HWND hDlg, HMENU hMenu);
LOCAL void PaletteDlg_OnDestroy(HWND hwnd);

/***********************************************************************/
BOOL WINPROC EXPORT DlgPaletteProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg,WM_INITDIALOG,PaletteDlg_OnInitDialog);  
	HANDLE_DLGMSG(hDlg,WM_COMMAND,PaletteDlg_OnCommand);
	HANDLE_DLGMSG(hDlg,WM_CLOSE, PaletteDlg_OnClose);
	HANDLE_DLGMSG(hDlg,WM_SIZE, PaletteDlg_OnSize);
	HANDLE_DLGRET(hDlg,WM_SETCURSOR, PaletteDlg_OnSetCursor);
	HANDLE_DLGMSG(hDlg,WM_INITMENU, PaletteDlg_OnInitMenu);
	HANDLE_DLGRET(hDlg,WM_QUERYNEWPALETTE, Dialog_OnQueryNewPalette);
	HANDLE_DLGRET(hDlg,WM_PALETTECHANGED, Dialog_OnPaletteChanged);
	HANDLE_DLGMSG(hDlg,WM_MENUSELECT, Astral_OnMenuSelect);
	HANDLE_DLGMSG(hDlg,WM_DESTROY,PaletteDlg_OnDestroy);
	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PALETTE);
	return FALSE;
	}
}

/************************************************************************/
LOCAL BOOL PaletteDlg_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
HWND hControl;
RECT ClientRect, ControlRect;

// Allocate and init the environment Structure and set it into the DWL_USER
LPPALETTEDATA lpData = (LPPALETTEDATA)Alloc(sizeof(PALETTEDATA));
if (!lpData)
	return(FALSE);
clr((LPTR)lpData,sizeof(PALETTEDATA));

GetClientRect( hDlg, &ClientRect );
lpData->iOldWidth = ClientRect.right - ClientRect.left;
lpData->iOldHeight = ClientRect.bottom - ClientRect.top;

if ( hControl = GetDlgItem( hDlg, IDC_COLORLABEL ) )
	{
	GetWindowRect( hControl, &ControlRect );
	lpData->iOldLabelWidth = ControlRect.right - ControlRect.left;
	lpData->iOldLabelHeight = ControlRect.bottom - ControlRect.top;
	}

if ( hControl = GetDlgItem( hDlg, IDC_PALETTE ) )
	{
	GetWindowRect( hControl, &ControlRect );
	lpData->iOldPalWidth = ControlRect.right - ControlRect.left;
	lpData->iOldPalHeight = ControlRect.bottom - ControlRect.top;
	}

UnlockPaletteData(hDlg,lpData);

PaletteInit(hDlg);
RestorePopup( hDlg, "xyPalette", RightPopup );
return(TRUE);
}

LOCAL void PaletteDlg_OnDestroy(HWND hDlg)
{
	LPPALETTEDATA lpData = LockPaletteData(hDlg);
	if (!lpData) 
		return;

	// FreeUp all palettes in list
	Palette_FreeUp(lpData->lpPaletteList);
	lpData->lpPaletteList = NULL;
	FreeUp((LPTR)lpData);
	UnlockPaletteData(hDlg,NULL);
}


/************************************************************************/
LOCAL void PaletteDlg_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) return;

LPPALETTE lpPalette;
FNAME szFileName;

switch (id)
	{
	case IDC_PALUNDO:
	case IDC_PALNEW:
	case IDC_PALOPEN:
	case IDC_PALSAVE:
	case IDC_PALSAVEAS:
	case IDC_PALDELETEITEM:
	case IDC_PALLABEL:
	case IDC_PALFILL:
	case IDC_PALETTE:
	case IDC_PALINSERT:
	case IDC_PALAPPEND:
	PaletteCommand(hDlg, id, codeNotify);
	break;

	case IDC_PALRESET:
	if (!PaletteClose(hDlg, NO))
		break;
	if ( !LookupExtFile( lpData->szPalette, szFileName, IDN_PALETTE ) )
		break;
	if (!(lpPalette = Palette_Read(szFileName,YES)))
		break;
	lpData->bRedo = lpData->iUndoFunction = 0;
	lpData->bChanged = NO;
	lpData->iActivePalette = 0;
	SetPalette( hDlg, lpPalette, NO );
	SetDlgItemTextNow( hDlg, IDC_COLORLABEL, "" );
	break;

	case IDC_PALMERGE:
	if ( !AstralDlgParam( NO, PictPubApp.GetResourceHandle(), hDlg, IDD_PALETTEMERGE,
		DlgPaletteMergeProc, (LPARAM)lpData ) )
		break;
	lpData->bRedo = lpData->iUndoFunction = 0;
	lpData->bChanged = YES;
	break;

	case IDC_PALAUTOSET:
	if ( !(lpData->bSetActive = !lpData->bSetActive) )
		break;
	if ( !(lpPalette = Palette_Get(lpData->lpPaletteList, NULL, 
		lpData->iActivePalette)) )
		break;
	if ( lpData->iActiveEntry >= lpPalette->iColors )
		break;
	SetActiveColor( &lpPalette->lpColorInfo[lpData->iActiveEntry], 
		lpPalette->lpColorInfo[lpData->iActiveEntry].ColorSpace, FALSE );
	break;

	case IDC_PALSETACTIVE:
	lpData->bRedo = lpData->iUndoFunction = 0;

	DoUndo(hDlg,id, lpData->iActiveEntry, 0, NULL, NULL, 0);
	if ( Palette_SetEntryColor(lpData->lpPaletteList,
			lpData->iActivePalette,
			lpData->iActiveEntry,
			GetActiveColorInfo()) )
		lpData->iUndoFunction = IDS_UNDOCOLOR;
	SetPalette( hDlg, NULL, NO );
	lpData->bChanged = lpData->bChanged || (lpData->iUndoFunction != 0);
	break;

	case IDC_PALFIND:
	if ( !AstralDlgParam( NO, PictPubApp.GetResourceHandle(), hDlg, IDD_PALETTEFIND, 
		DlgFindProc, (LPARAM)lpData ) )
		break;
	SendMessage(GetDlgItem(hDlg, IDC_PALETTE), WM_SELECTPALETTE,
		lpData->iActiveEntry, 0L);
	break;

	case IDC_PALOPTIONS:
	if ( !AstralDlgParam(NO, PictPubApp.GetResourceHandle(), hDlg, IDD_PALETTEOPTIONS,
		      DlgSizePaletteProc, (LPARAM)lpData ) )
		break;
	SetPalette( hDlg, NULL, NO );
	lpData->bChanged = YES;
	break;

   default:
	break;
   }

UnlockPaletteData(hDlg,lpData);
}

/************************************************************************/
LOCAL void PaletteDlg_OnClose(HWND hDlg)
/************************************************************************/
{
SavePalettePreferences( hDlg );
AstralDlgEnd(hDlg, FALSE);
UpdateStatusBar( NO, NO, NO, YES );
}

/***********************************************************************/
LOCAL void PaletteDlg_OnSize(HWND hDlg, UINT state, int cx, int cy)
/***********************************************************************/
{
	LPPALETTEDATA lpData = LockPaletteData(hDlg);
	if (!lpData) return;

	HWND hControl;
	int dx, dy;

	if (hControl = GetDlgItem(hDlg, IDC_PALETTE))
	{
		dx = lpData->iOldPalWidth;
		dy = lpData->iOldPalHeight;
		dx += cx - lpData->iOldWidth;
		dy += cy - lpData->iOldHeight;
		SetWindowPos(hControl, NULL, 0, 0, dx, dy,
				SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
		lpData->iOldPalWidth = dx;
		lpData->iOldPalHeight = dy;
	}

	if (hControl = GetDlgItem(hDlg, IDC_COLORLABEL))
	{
		dx = lpData->iOldLabelWidth;
		dy = lpData->iOldLabelHeight;
		dx += cx - lpData->iOldWidth;
		//dy += cy - lpData->iOldHeight;
		SetWindowPos(hControl, NULL, 0, 0, dx, dy,
			SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
		lpData->iOldLabelWidth = dx;
		lpData->iOldLabelHeight = dy;
	}

	lpData->iOldWidth = cx;
	lpData->iOldHeight = cy;

	SetPalette( hDlg, NULL, NO );
	UnlockPaletteData(hDlg,lpData);
}

/************************************************************************/
LOCAL BOOL PaletteDlg_OnSetCursor(HWND hDlg, HWND hWndCursor, UINT codeHitTest,
							UINT msg)
/************************************************************************/
{
    CPoint pt;
    HWND hChild;

    if (hWndCursor == GetDlgItem(hDlg, IDC_PALETTE))
	    return(FALSE);

    // check for disabled control
    GetCursorPos( &pt );
    if( hChild = MyChildWindowFromPoint( hWndCursor, pt ))
    {
        hWndCursor = hChild;
    }
    return( SetupCursor( hWndCursor, codeHitTest, IDD_PALETTE ));
}

/***********************************************************************/
LOCAL void PaletteDlg_OnInitMenu(HWND hDlg, HMENU hMenu)
/***********************************************************************/
{
SetPaletteMenus(hDlg);
}

/***********************************************************************/
static BOOL CreateNewEntries( LPPALETTE lpHeadPalette, int iPalette,
                                 int iAfter, int iEntries, BOOL bDoCPO )
/***********************************************************************/
{

RGBS RGBmap[256];
LPCOLORINFO lpColorInfo;
int	i;
BOOL bRetVal;
LPIMAGE lpImage;

lpImage = GetActiveImage();

if (iEntries > 256)
	iEntries = 256;

lpColorInfo = (LPCOLORINFO)Alloc(iEntries * sizeof(COLORINFO));

if (!lpColorInfo)
{
	return(FALSE);
}

if (bDoCPO && lpImage)
	{
	AstralCursor(IDC_WAIT);
	if (!CreateOptimizedPalette(ImgGetBase(lpImage),
								ImgGetBaseEditFrame(lpImage),
								RGBmap, &iEntries,
								TRUE,
								AstralClockCursor))
		{
		iEntries = 0;
		}
	AstralCursor(NULL);

	if (!iEntries)
	{
		Message(IDS_EMEMALLOC);
		return(FALSE);
	}

	switch(FrameType(ImgGetBaseEditFrame(lpImage)))
		{
		case FDT_LINEART :
		case FDT_GRAYSCALE :
			for (i=0; i<iEntries; i++)
			{
				lpColorInfo[i].rgb = RGBmap[i];
				SetColorInfo( &lpColorInfo[i], &lpColorInfo[i], CS_RGB );
				SetColorInfo( &lpColorInfo[i], &lpColorInfo[i], CS_GRAY );
			}
		break;

		case FDT_PALETTECOLOR:
		case FDT_RGBCOLOR :
			for (i=0; i<iEntries; i++)
			{
				lpColorInfo[i].rgb = RGBmap[i];
				SetColorInfo( &lpColorInfo[i], &lpColorInfo[i], CS_RGB );
			}
		break;

		case FDT_CMYKCOLOR :
			for (i=0; i<iEntries; i++)
			{
				lpColorInfo[i].rgb = RGBmap[i];
				SetColorInfo( &lpColorInfo[i], &lpColorInfo[i], CS_RGB );
				SetColorInfo( &lpColorInfo[i], &lpColorInfo[i], CS_CMYK );
			}
		break;
		}
	}
else
	{
	for (i=0; i<iEntries; i++)
		{
		lpColorInfo[i].rgb.red   = 255;
		lpColorInfo[i].rgb.green = 255;
		lpColorInfo[i].rgb.blue  = 255;
		SetColorInfo( &lpColorInfo[i], &lpColorInfo[i], CS_RGB);
		}
	}

bRetVal = 
	Palette_AddEntries(lpHeadPalette, iPalette, iAfter, iEntries, lpColorInfo);

if (lpColorInfo)
	FreeUp((LPTR)lpColorInfo);

return(bRetVal);
}

/***********************************************************************/
static BOOL DoUndo( HWND hDlg, int iFunction, int iEntry1, int iEntry2,
                     LPCOLORINFO lpColor1, LPCOLORINFO lpColor2, int iCount )
/***********************************************************************/
{
static int SaveUndoFunction;
static COLORINFO SaveUndoColor1, SaveUndoColor2;
static STRING SaveUndoLabel;
static int SaveUndoEntry1;
static int SaveUndoEntry2;
static int SaveColors;
static int SaveCount;
static BOOL SaveFillHSL;
int UndoFunction;
COLORINFO UndoColor1, UndoColor2;
STRING UndoLabel;
int UndoEntry1;
int UndoEntry2;
int UndoColors;
int UndoCount;
LPPALETTE lpPalette;
BOOL UndoFillHSL;

LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) 
	return FALSE;

lpPalette = Palette_Get(lpData->lpPaletteList, NULL, lpData->iActivePalette);
if (!lpPalette)
{
	UnlockPaletteData(hDlg,lpData);
	return(FALSE);
}

if (iFunction != IDC_PALUNDO)
	{
	SaveUndoFunction = iFunction;
	SaveUndoEntry1 = iEntry1;
	SaveUndoEntry2 = iEntry2;
	switch (iFunction)
		{
		case IDC_PALDELETEITEM:
			Palette_GetEntryColor(lpData->lpPaletteList,
				lpData->iActivePalette,
				iEntry1,
				&SaveUndoColor1);
			Palette_GetEntryLabel(lpData->lpPaletteList,
				lpData->iActivePalette,
				iEntry1,
				SaveUndoLabel);
			break;

		case IDC_PALINSERT:
		case IDC_PALAPPEND:
			SaveUndoColor1 = *lpColor1;
			break;
		
		case IDC_PALUNDOFILL:
		case IDC_PALFILL:
			SaveUndoColor1 = *lpColor1;
			SaveUndoColor2 = *lpColor2;
			SaveCount = iCount;
			SaveColors = lpPalette->iColors;
			SaveFillHSL = lpData->bFillHSL;
			break;
	
		case IDC_PALLABEL:
			Palette_GetEntryLabel(lpData->lpPaletteList,
				lpData->iActivePalette,
				iEntry1,
				SaveUndoLabel);
			break;

		case IDC_PALSETACTIVE:
		case IDC_PALETTE:
			Palette_GetEntryColor(lpData->lpPaletteList,
				lpData->iActivePalette,
				iEntry1,
				&SaveUndoColor1);
			break;
		default:
			break;
		}
	}
else
	{
	UndoFunction = SaveUndoFunction;
	UndoEntry1 = SaveUndoEntry1;
	UndoEntry2 = SaveUndoEntry2;
	UndoColor1 = SaveUndoColor1;
	UndoColor2 = SaveUndoColor2;
	UndoColors = SaveColors;
	UndoCount = SaveCount;
	UndoFillHSL = SaveFillHSL;
	lstrcpy(UndoLabel, SaveUndoLabel);
	switch (UndoFunction)
		{
		case IDC_PALDELETEITEM:
			DoUndo(hDlg,IDC_PALINSERT, UndoEntry1, 0, &UndoColor1, NULL, 0);
			if (Palette_InsertEntry(lpData->lpPaletteList,
					lpData->iActivePalette,
					UndoEntry1, &UndoColor1))
				{
				if (lstrlen(UndoLabel))
				      Palette_SetEntryLabel(lpData->lpPaletteList,
						lpData->iActivePalette,
						UndoEntry1, UndoLabel);
				UnlockPaletteData(hDlg,lpData);
				return(TRUE);
				}
			break;
	
		case IDC_PALINSERT:
			DoUndo(hDlg,IDC_PALDELETEITEM, UndoEntry1, 0, NULL, NULL, 0);
			if (Palette_DeleteEntry(lpData->lpPaletteList,
					lpData->iActivePalette,
					UndoEntry1))
				{
				UnlockPaletteData(hDlg,lpData);
				return(TRUE);
				}
			break;

		case IDC_PALMOVE:
			DoUndo(hDlg,IDC_PALMOVE, UndoEntry2, UndoEntry1,
				NULL, NULL, 0);
			if (Palette_MoveEntry(lpData->lpPaletteList, lpData->iActivePalette,
					UndoEntry2, UndoEntry1))
				{
				if (UndoEntry2 == lpData->iActiveEntry)
					lpData->iActiveEntry = UndoEntry1;
				UnlockPaletteData(hDlg,lpData);
				return(TRUE);
				}
			break;

		case IDC_PALAPPEND:
			DoUndo(hDlg,IDC_PALDELETEITEM, UndoEntry1+1, 0, NULL, NULL, 0);
			if (Palette_DeleteEntry(lpData->lpPaletteList,
					lpData->iActivePalette,
					UndoEntry1+1))
				{
				UnlockPaletteData(hDlg,lpData);
				return(TRUE);
				}
			break;

		case IDC_PALFILL:
			UndoCount = lpPalette->iColors - UndoColors;
			DoUndo(hDlg,IDC_PALUNDOFILL, UndoEntry1, 0, &UndoColor1,
				&UndoColor2, UndoCount);
			if (Palette_DeleteEntries(lpData->lpPaletteList,
					lpData->iActivePalette, UndoEntry1+1,
					UndoCount))
				{
				UnlockPaletteData(hDlg,lpData);
				return(TRUE);
				}
			break;
		
		case IDC_PALUNDOFILL:
			DoUndo(hDlg,IDC_PALFILL, UndoEntry1, 0, &UndoColor1, &UndoColor2, UndoCount);
			if (Palette_FillEntries(lpData->lpPaletteList,
					lpData->iActivePalette, UndoEntry1,
					&UndoColor1, &UndoColor2, UndoCount,
					UndoFillHSL))
				{		
				UnlockPaletteData(hDlg,lpData);
				return(TRUE);
				}
			break;

		case IDC_PALLABEL:
			DoUndo(hDlg,UndoFunction, UndoEntry1, 0, NULL, NULL, 0);
			if (Palette_SetEntryLabel(lpData->lpPaletteList,
				lpData->iActivePalette,
				UndoEntry1, UndoLabel))
				{
				UnlockPaletteData(hDlg,lpData);
				return(TRUE);
				}
			break;

		case IDC_PALSETACTIVE:
		case IDC_PALETTE:
			DoUndo(hDlg,UndoFunction, UndoEntry1, 0, NULL, NULL, 0);
			if (Palette_SetEntryColor(lpData->lpPaletteList,
				lpData->iActivePalette,
				UndoEntry1, &UndoColor1))
				{
				UnlockPaletteData(hDlg,lpData);
				return(TRUE);
				}
			break;

		default:
			break;
		}
	}

UnlockPaletteData(hDlg,lpData);
return(FALSE);
}

/***********************************************************************/
static void SavePalettePreferences( HWND hDlg )
/***********************************************************************/
{
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) 
	return;

FNAME szFileName;
int PaletteRows, PaletteCols;

if (lstrlen(lpData->szPalette) &&
    LookupExtFile( lpData->szPalette, szFileName, IDN_PALETTE ))
	{
	PutDefStr(szPalette, lpData->szPalette);
	PutDefInt(iActivePalette, lpData->iActivePalette);
	}
PutDefInt(bSetActive, lpData->bSetActive);
PutDefInt(bFillMax, lpData->bFillMax);
PutDefInt(iFill, lpData->iFill);
PutDefInt(bFillHSL, lpData->bFillHSL);
PutDefInt(iNewFill, lpData->iNewFill);
PutDefInt(bNewCPO, lpData->bNewCPO);
if ( hDlg )
	{
	GetSizePalette( hDlg, &PaletteRows, &PaletteCols );
	PutDefInt(PaletteRows, PaletteRows);
	PutDefInt(PaletteCols, PaletteCols);
	}
UnlockPaletteData(hDlg,lpData);
}

/***********************************************************************/
static void LoadPalettePreferences( HWND hDlg )
/***********************************************************************/
{
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) 
	return;

GetDefaultString("szPalette","Default",lpData->szPalette,sizeof(lpData->szPalette));
lpData->iActivePalette = GetDefaultInt("iActivePalette", 0);
lpData->bSetActive = GetDefaultInt("bSetActive", YES);
lpData->bFillMax = GetDefaultInt("bFillMax", NO);
lpData->iFill = GetDefaultInt("iFill", 16);
lpData->bFillHSL = GetDefaultInt("bFillHSL", NO);
lpData->iNewFill = GetDefaultInt("iNewFill", 16);
lpData->bNewCPO = GetDefaultInt("bNewCPO", NO);
if ( hDlg )
	{
	lpData->PaletteRows = GetDefaultInt("PaletteRows", 5);
   lpData->PaletteCols = GetDefaultInt("PaletteCols", 5);
	}
UnlockPaletteData(hDlg,lpData);
}

// LOCAL prototypes
LOCAL BOOL PaletteNew_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void PaletteNew_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void PaletteNew_OnDestroy(HWND hwnd);

/***********************************************************************/
BOOL WINPROC EXPORT DlgPaletteNewProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg,WM_INITDIALOG,PaletteNew_OnInitDialog);  
	HANDLE_DLGMSG(hDlg,WM_COMMAND,PaletteNew_OnCommand);
	HANDLE_DLGMSG(hDlg,WM_DESTROY,PaletteNew_OnDestroy);
	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PALETTENEW);
	}
}

/************************************************************************/
LOCAL BOOL PaletteNew_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
SetWindowLong(hDlg, DWL_USER, (LONG)lParam);
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) 
	return FALSE;

HWND hWnd;

hWnd = GetActiveDoc();
CenterPopup( hDlg );
CheckDlgButton(hDlg, IDC_PALIMAGEFILL, hWnd ? lpData->bNewCPO : NO);
ControlEnable(hDlg, IDC_PALIMAGEFILL, hWnd != NULL);
InitDlgItemSpin(hDlg, IDC_PALETTEFILL, lpData->iNewFill, NO, 0, 256 );

UnlockPaletteData(hDlg,lpData);
return(TRUE);
}

/************************************************************************/
LOCAL void PaletteNew_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) 
	return;

BOOL Bool;
LPPALETTE lpPalette;
LPSTR lpString;
RGBS rgb;

switch (id)
	{
	case IDC_PALIMAGEFILL:
	lpData->bNewCPO = !lpData->bNewCPO;
	CheckDlgButton(hDlg, id, lpData->bNewCPO);
	break;

	case IDC_PALETTEFILL:
	lpData->iNewFill = GetDlgItemSpin(hDlg, IDC_PALETTEFILL, &Bool, NO );
	break;

	case IDOK:
	AstralStr(IDS_UNTITLED, &lpString);
	lstrcpy(lpData->szPalette, lpString);
	if (!(lpPalette = Palette_New(lpData->szPalette)))
		{
		Message(IDS_EMEMALLOC);
		break;
		}
	if (lpData->iNewFill)
		{
		rgb.red = rgb.green = rgb.blue = 255;
		if (!CreateNewEntries(lpPalette, 0, 0, lpData->iNewFill,
			 lpData->bNewCPO))
			{
	    		Message(IDS_EMEMALLOC);
			Palette_FreeUp(lpPalette);
			break;
			}
		}
	lpData->bRedo = lpData->iUndoFunction = 0;
	lpData->iActivePalette = 0;
   SetPalette( lpData->hPalDlg, lpPalette, NO );
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	AstralDlgEnd( hDlg, FALSE );
	break;

   default:
	break;
   }
UnlockPaletteData(hDlg,lpData);
}

void PaletteNew_OnDestroy(HWND hwnd)
{
	// do nawting
}

// LOCAL prototypes
LOCAL BOOL PaletteLoad_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void PaletteLoad_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void PaletteLoad_OnDestroy(HWND hwnd);

/***********************************************************************/
BOOL WINPROC EXPORT DlgPaletteLoadProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg,WM_INITDIALOG,PaletteLoad_OnInitDialog);  
	HANDLE_DLGMSG(hDlg,WM_COMMAND,PaletteLoad_OnCommand);
	HANDLE_DLGMSG(hDlg,WM_DESTROY,PaletteLoad_OnDestroy);
	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PALETTELOAD);
	}
}

/************************************************************************/
LOCAL BOOL PaletteLoad_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
SetWindowLong(hDlg, DWL_USER, (LONG)lParam);
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) 
	return FALSE;

CenterPopup( hDlg );
lstrcpy( Names.Saved, lpData->szPalette );
InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, lpData->szPalette, IDN_PALETTE );

UnlockPaletteData(hDlg,lpData);

return(TRUE);
}

/************************************************************************/
LOCAL void PaletteLoad_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) 
	return;

FNAME szFileName;
LPPALETTE lpPalette;
HWND hWnd;

switch (id)
	{
	case IDOK:
	if ( !LookupExtFile( lpData->szPalette, szFileName, IDN_PALETTE ) )
		break;
   if (!(hWnd = lpData->hPalDlg))
		break;
	if (!(lpPalette = Palette_Read(szFileName,YES)))
		break;
	lpData->bRedo = lpData->iUndoFunction = 0;
	lpData->iActivePalette = 0;
	lpData->iActiveEntry = 0;
	SetPalette(hWnd, lpPalette, YES);
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	lstrcpy( lpData->szPalette, Names.Saved );
	AstralDlgEnd( hDlg, FALSE );
	break;

	case IDC_EXTNAMES:
	GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
		lpData->szPalette, IDN_PALETTE, codeNotify );
	break;

	case IDC_EXTMANAGE:
	PopupMenu( hDlg, id, IDC_FILENAME );
	break;

	case IDC_ADDEXT:
	case IDC_DELETEEXT:
	case IDC_RENAMEEXT:
	ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, lpData->szPalette,
		IDN_PALETTE, id, NO );
	break;

   default:
	break;
   }
UnlockPaletteData(hDlg,lpData);
}

void PaletteLoad_OnDestroy(HWND hwnd)
{
	// do nawting
}

// LOCAL prototypes
LOCAL BOOL PaletteMerge_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void PaletteMerge_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void PaletteMerge_OnDestroy(HWND hwnd);

/***********************************************************************/
BOOL WINPROC EXPORT DlgPaletteMergeProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg,WM_INITDIALOG,PaletteMerge_OnInitDialog);  
	HANDLE_DLGMSG(hDlg,WM_COMMAND,PaletteMerge_OnCommand);
	HANDLE_DLGMSG(hDlg,WM_DESTROY,PaletteMerge_OnDestroy);
	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PALETTEMERGE);
	}
}

/************************************************************************/
LOCAL BOOL PaletteMerge_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
SetWindowLong(hDlg, DWL_USER, (LONG)lParam);
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) 
	return FALSE;

CenterPopup( hDlg );
lstrcpy( Names.Saved, lpData->szPalette );
InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, lpData->szPalette, IDN_PALETTE );

UnlockPaletteData(hDlg,lpData);
return(TRUE);
}

/************************************************************************/
LOCAL void PaletteMerge_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) 
	return ;

FNAME szFileName;
LPPALETTE lpPalette;
HWND hWnd;

switch (id)
	{
	case IDOK:
	if ( !LookupExtFile( lpData->szPalette, szFileName, IDN_PALETTE ) )
		break;
	lstrcpy( lpData->szPalette, Names.Saved );
	if (!(lpPalette = Palette_Read(szFileName,YES)))
		break;
	Palette_Link(lpData->lpPaletteList, lpPalette);
	if ( !(lpPalette = Palette_Combine(lpData->lpPaletteList)) )
		{
		Message (IDS_EMEMALLOC);
		break;
		}
	Palette_FreeUp(lpData->lpPaletteList);
	lpData->lpPaletteList = lpPalette;
//	if (!(hWnd = AstralDlgGet(IDD_PALETTE)))
if (!(hWnd = lpData->hPalDlg))
		break;
	SetPalette(hWnd, lpData->lpPaletteList, YES);
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	lstrcpy( lpData->szPalette, Names.Saved );
	AstralDlgEnd( hDlg, FALSE );
	break;

	case IDC_EXTNAMES:
	GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
		lpData->szPalette, IDN_PALETTE, codeNotify );
	break;

	case IDC_EXTMANAGE:
	PopupMenu( hDlg, id, IDC_FILENAME );
	break;

	case IDC_ADDEXT:
	case IDC_DELETEEXT:
	case IDC_RENAMEEXT:
	ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, lpData->szPalette,
		IDN_PALETTE, id, NO );
	break;

   default:
	break;
   }
UnlockPaletteData(hDlg,lpData);
}

void PaletteMerge_OnDestroy(HWND hwnd)
{
	// do nawting
}

// LOCAL prototypes
LOCAL BOOL PaletteSave_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void PaletteSave_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void PaletteSave_OnDestroy(HWND hwnd);

/***********************************************************************/
BOOL WINPROC EXPORT DlgPaletteSaveProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg,WM_INITDIALOG,PaletteSave_OnInitDialog);  
	HANDLE_DLGMSG(hDlg,WM_COMMAND,PaletteSave_OnCommand);
	HANDLE_DLGMSG(hDlg,WM_DESTROY,PaletteMerge_OnDestroy);
	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PALETTESAVE);
	}
}

/************************************************************************/
LOCAL BOOL PaletteSave_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
SetWindowLong(hDlg, DWL_USER, (LONG)lParam);
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) 
	return FALSE;

CenterPopup( hDlg );
lstrcpy( Names.Saved, lpData->szPalette );
InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, lpData->szPalette, IDN_PALETTE );

UnlockPaletteData(hDlg,lpData);
return(TRUE);
}

/************************************************************************/
LOCAL void PaletteSave_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) 
	return;

FNAME szFileName;

switch (id)
	{
	case IDC_EXTNAMES:
	GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
		lpData->szPalette, IDN_PALETTE, codeNotify );
	break;

	case IDC_EXTMANAGE:
	PopupMenu( hDlg, id, IDC_FILENAME );
	break;

	case IDC_ADDEXT:
	case IDC_DELETEEXT:
	case IDC_RENAMEEXT:
	ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, lpData->szPalette,
		IDN_PALETTE, id, NO );
	break;

	case IDOK:
	if (!lpData->lpPaletteList)
		break;
	if ( LookupExtFile(lpData->szPalette, szFileName, IDN_PALETTE) )
		{
		if (AstralOKCancel(IDS_OVERWRITEEXTFILE,
		    (LPSTR)lpData->szPalette) == IDCANCEL)
			break;
		}
	else
		{
		if ( !CreateFileName( lpData->szPalette, IDN_PALETTE, szFileName) )
			break;
		}
	lstrcpy(lpData->lpPaletteList->szName, lpData->szPalette);
	AstralCursor(IDC_WAIT);
	if (!Palette_Write(szFileName, lpData->lpPaletteList))
		{
		AstralCursor(NULL);
		break;
		}
	AstralCursor(NULL);
	if (!AddExtFile(lpData->szPalette, szFileName, IDN_PALETTE))
		{
		FileDelete(szFileName);
		break;
		}
	lpData->bRedo = lpData->iUndoFunction = 0;
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	lstrcpy( lpData->szPalette, Names.Saved );
	AstralDlgEnd( hDlg, FALSE );
	break;

   default:
	break;
   }
UnlockPaletteData(hDlg,lpData);
}

void PaletteSave_OnDestroy(HWND hwnd)
{
	// do nawting
}

// LOCAL prototypes
LOCAL BOOL Label_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Label_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void Label_OnDestroy(HWND hwnd);

/***********************************************************************/
BOOL WINPROC EXPORT DlgLabelProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg,WM_INITDIALOG,Label_OnInitDialog);  
	HANDLE_DLGMSG(hDlg,WM_COMMAND,Label_OnCommand);
	HANDLE_DLGMSG(hDlg,WM_DESTROY,Label_OnDestroy);
	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PALETTELABEL);
	}
}

/************************************************************************/
LOCAL BOOL Label_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
SetWindowLong(hDlg, DWL_USER, (LONG)lParam);
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) 
	return FALSE;

FNAME szLabel;

CenterPopup( hDlg );
Palette_GetEntryLabel(lpData->lpPaletteList, lpData->iActivePalette,
		lpData->iActiveEntry, szLabel);
Edit_LimitText(GetDlgItem(hDlg, IDC_LABELNAME), sizeof(STRING)-1);
SetDlgItemText(hDlg, IDC_LABELNAME, szLabel);

UnlockPaletteData(hDlg,lpData);
return(TRUE);
}

/************************************************************************/
LOCAL void Label_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
STRING szLabel;

LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) 
	return ;


switch (id)
	{
	case IDOK:
	GetDlgItemText(hDlg, IDC_LABELNAME, szLabel, MAX_FNAME_LEN);
	lpData->bRedo = lpData->iUndoFunction = 0;
	DoUndo(hDlg,IDC_PALLABEL, lpData->iActiveEntry, 0, NULL, NULL, 0);
	if (Palette_SetEntryLabel(lpData->lpPaletteList, lpData->iActivePalette, 
		lpData->iActiveEntry, szLabel))
		{
		AstralDlgEnd( hDlg, TRUE );
		break;
		}
	AstralDlgEnd( hDlg, FALSE );
	break;

	case IDCANCEL:
	AstralDlgEnd( hDlg, FALSE );
	break;

   default:
	break;
   }

UnlockPaletteData(hDlg,lpData);
}

void Label_OnDestroy(HWND hwnd)
{
	// do nawting
}

// LOCAL prototypes
LOCAL BOOL Find_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Find_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void Find_OnDestroy(HWND hwnd);

/***********************************************************************/
BOOL WINPROC EXPORT DlgFindProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg,WM_INITDIALOG, Find_OnInitDialog);  
	HANDLE_DLGMSG(hDlg,WM_COMMAND, Find_OnCommand);
	HANDLE_DLGRET(hDlg,WM_DESTROY, Find_OnDestroy);
	HANDLE_DLGRET(hDlg,WM_QUERYNEWPALETTE, Dialog_OnQueryNewPalette);
	HANDLE_DLGRET(hDlg,WM_PALETTECHANGED, Dialog_OnPaletteChanged);
	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PALETTEFIND);
	}
}

/************************************************************************/
LOCAL BOOL Find_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
SetWindowLong(hDlg, DWL_USER, (LONG)lParam);

CenterPopup( hDlg );
SetDlgItemText(hDlg, IDC_COLORNAME, "");
SetDlgItemText(hDlg, IDC_FINDCOLOR, "");
ShowControl(hDlg, IDC_FINDPATCH, IDC_FINDPATCH, NO, NO);
ControlEnable(hDlg, IDC_PREVIOUS, NO);
ControlEnable(hDlg, IDC_NEXT, NO);
ControlEnable(hDlg, IDOK, NO);
FORWARD_WM_COMMAND(hDlg, IDC_FINDCOLOR, GetDlgItem(hDlg,IDC_FINDCOLOR),
					EN_CHANGE, SendMessage);
return(TRUE);
}

/************************************************************************/
LOCAL void Find_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) 
	return ;

BOOL bNext, bPrev;
FNAME szLabel;
int iSelect;
static int iNum, iWhich;
HWND hFocusWnd, hPrevWnd, hNextWnd;
COLORINFO ColorInfo;
COLOR Color;

switch (id)
	{
	case IDC_FINDCOLOR:
	if ( codeNotify != EN_CHANGE )
		break;
	iWhich = 0;
	GetDlgItemText(hDlg, IDC_FINDCOLOR, szLabel, MAX_FNAME_LEN);
	lstrcat( szLabel, "*" );
	iNum = Palette_FindEntryLabel(lpData->lpPaletteList, 
		lpData->iActivePalette, &iSelect, szLabel, -1);
	if ( !iNum )
		szLabel[0] = '\0';
	SetDlgItemTextNow(hDlg, IDC_COLORNAME, szLabel);
	Palette_GetEntryColor(lpData->lpPaletteList, lpData->iActivePalette, iSelect,
		&ColorInfo);
	ShowControl(hDlg, IDC_FINDPATCH, IDC_FINDPATCH, iNum != 0, NO);
	if (iNum)
		{
		CopyRGB(&ColorInfo.rgb, &Color);
		SetWindowLong( GetDlgItem(hDlg, IDC_FINDPATCH), GWL_ACTIVE, Color );
		AstralControlRepaint( hDlg, IDC_FINDPATCH );
		}
	SetDlgItemInt(hDlg, IDC_COLORSFOUND, iNum, TRUE);
	PostMessage(hDlg, WM_COMMAND, IDC_FINDENABLE, MAKELONG(0, 0));
	break;
	
	case IDC_FINDENABLE:
	bPrev = iWhich > 0;
	bNext = iWhich < (iNum-1);
	hNextWnd = GetDlgItem(hDlg, IDC_NEXT);
	hPrevWnd = GetDlgItem(hDlg, IDC_PREVIOUS);
	hFocusWnd = GetFocus();

	if (hFocusWnd == hPrevWnd && !bPrev && bNext)
		FORWARD_WM_NEXTDLGCTL(hDlg, hNextWnd, TRUE, PostMessage);
	else if (hFocusWnd == hNextWnd && !bNext && bPrev)
		FORWARD_WM_NEXTDLGCTL(hDlg, hPrevWnd, TRUE, PostMessage);

	ControlEnable(hDlg, IDC_PREVIOUS, bPrev);
	ControlEnable(hDlg, IDC_NEXT, bNext);
	ControlEnable(hDlg, IDOK, iNum != 0);
	break;

	case IDC_NEXT:
	case IDC_PREVIOUS:
	if (id == IDC_NEXT)
			++iWhich;
	else	--iWhich;
	GetDlgItemText(hDlg, IDC_FINDCOLOR, szLabel, MAX_FNAME_LEN);
	lstrcat( szLabel, "*" );
	Palette_FindEntryLabel(lpData->lpPaletteList, lpData->iActivePalette, 
		&iSelect, szLabel, iWhich);
	SetDlgItemTextNow(hDlg, IDC_COLORNAME, szLabel);
	Palette_GetEntryColor(lpData->lpPaletteList, lpData->iActivePalette, iSelect,
		&ColorInfo);

	CopyRGB(&ColorInfo.rgb, &Color);
	SetWindowLong( GetDlgItem(hDlg, IDC_FINDPATCH), GWL_ACTIVE, Color );
	AstralControlRepaint( hDlg, IDC_FINDPATCH );

	PostMessage(hDlg, WM_COMMAND, IDC_FINDENABLE, MAKELONG(id, 0));
	break;

	case IDOK:
	GetDlgItemText(hDlg, IDC_FINDCOLOR, szLabel, MAX_FNAME_LEN);
	lstrcat( szLabel, "*" );
	if (Palette_FindEntryLabel(lpData->lpPaletteList, lpData->iActivePalette, 
		&iSelect, szLabel, iWhich))
		{
		lpData->iActiveEntry = iSelect;
		AstralDlgEnd( hDlg, TRUE );
		break;
		}
	AstralDlgEnd( hDlg, FALSE );
	break;

	case IDCANCEL:
	AstralDlgEnd( hDlg, FALSE );
	break;

   default:
	break;
   }
UnlockPaletteData(hDlg,lpData);
}

void Find_OnDestroy(HWND hwnd)
{
	// do nawting
}

// LOCAL prototypes
LOCAL BOOL SizePalette_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void SizePalette_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void SizePalette_OnDestroy(HWND hwnd);

static int SizePalette_iRows, SizePalette_iCols;

/***********************************************************************/
BOOL WINPROC EXPORT DlgSizePaletteProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg,WM_INITDIALOG, SizePalette_OnInitDialog);  
	HANDLE_DLGMSG(hDlg,WM_COMMAND, SizePalette_OnCommand);
	HANDLE_DLGMSG(hDlg,WM_DESTROY, SizePalette_OnDestroy);
	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PALETTEOPTIONS);
	}
}

/************************************************************************/
LOCAL BOOL SizePalette_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
SetWindowLong(hDlg, DWL_USER, (LONG)lParam);
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) 
	return FALSE;

CenterPopup( hDlg );
CheckRadioButton(hDlg, IDC_PALHORZ, IDC_PALVERT, IDC_PALHORZ + bVertical );
GetSizePalette( lpData->hPalDlg, &SizePalette_iRows, &SizePalette_iCols );
InitDlgItemSpin(hDlg, IDC_PALROWS, SizePalette_iRows, NO, 1, 99 );
InitDlgItemSpin(hDlg, IDC_PALCOLUMNS, SizePalette_iCols, NO, 1, 99 );

UnlockPaletteData(hDlg,lpData);
return(TRUE);
}

/************************************************************************/
LOCAL void SizePalette_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) 
	return;

BOOL Bool;
int iOrigRows, iOrigCols;

switch (id)
	{
	case IDC_PALROWS:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	SizePalette_iRows = GetDlgItemSpin( hDlg, id, &Bool, NO );
	break;

	case IDC_PALCOLUMNS:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	SizePalette_iCols = GetDlgItemSpin( hDlg, id, &Bool, NO );
	break;

	case IDC_PALHORZ:
	case IDC_PALVERT:
	CheckRadioButton(hDlg, IDC_PALHORZ, IDC_PALVERT, id );
	bVertical = id - IDC_PALHORZ;
	break;

	case IDOK:
   GetSizePalette( lpData->hPalDlg, &iOrigRows, &iOrigCols );
	if ( iOrigRows != SizePalette_iRows || iOrigCols != SizePalette_iCols )
      SetSizePalette( lpData->hPalDlg, SizePalette_iRows, SizePalette_iCols );
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	AstralDlgEnd( hDlg, FALSE );
	break;

   default:
	break;
   }
UnlockPaletteData(hDlg,lpData);
}

void SizePalette_OnDestroy(HWND hwnd)
{
	// do nawting
}

// LOCAL prototypes
LOCAL BOOL FillPalette_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void FillPalette_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void FillPalette_OnDestroy(HWND hwnd);

static COLORINFO Color1, Color2;
static int i, iStart;
static LPPALETTE lpPalette;

/***********************************************************************/
BOOL WINPROC EXPORT DlgFillPaletteProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg,WM_INITDIALOG,FillPalette_OnInitDialog);  
	HANDLE_DLGMSG(hDlg,WM_COMMAND,FillPalette_OnCommand);
	HANDLE_DLGMSG(hDlg,WM_DESTROY,FillPalette_OnDestroy);
	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PALETTEFILL);
	}
}

/************************************************************************/
LOCAL BOOL FillPalette_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
SetWindowLong(hDlg, DWL_USER, (LONG)lParam);
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) 
	return FALSE;

COLOR Color;

CenterPopup( hDlg );
InitDlgItemSpin(hDlg, IDC_PALETTEFILL, lpData->iFill, NO, 1, 256 );
CheckDlgButton(hDlg, IDC_FILLMAX, lpData->bFillMax);
ControlEnable(hDlg, IDC_PALETTEFILL, !lpData->bFillMax);
CheckRadioButton(hDlg, IDC_FILLRGB, IDC_FILLHSL,
		lpData->bFillHSL ? IDC_FILLHSL : IDC_FILLRGB);
lpPalette = Palette_Get(lpData->lpPaletteList, NULL, lpData->iActivePalette);
if ( !lpPalette || lpPalette->iColors <= 1 )
	{
	AstralDlgEnd(hDlg, FALSE);
	return(TRUE);
	}
if ( (lpData->iActiveEntry+1) < lpPalette->iColors )
	iStart = lpData->iActiveEntry;
else if ((lpData->iActiveEntry-1) >= 0)
	iStart = lpData->iActiveEntry - 1;
else
	{
	AstralDlgEnd(hDlg, FALSE);
	return(TRUE);
	}
Color1 = lpPalette->lpColorInfo[iStart];
CopyRGB( &Color1.rgb, &Color );
SetWindowLong( GetDlgItem( hDlg, IDC_PALETTEFILL_COLOR1 ), GWL_ACTIVE,
	Color );
Color2 = lpPalette->lpColorInfo[iStart+1];
CopyRGB( &Color2.rgb, &Color );
SetWindowLong( GetDlgItem( hDlg, IDC_PALETTEFILL_COLOR2 ), GWL_ACTIVE,
	Color );

UnlockPaletteData(hDlg,lpData);
return(TRUE);
}

/************************************************************************/
LOCAL void FillPalette_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) 
	return ;

BOOL Bool;
COLOR Color;

switch (id)
	{
	case IDC_FILLRGB:
	lpData->bFillHSL = NO;
	CheckRadioButton(hDlg, IDC_FILLRGB, IDC_FILLHSL,
		lpData->bFillHSL ? IDC_FILLHSL : IDC_FILLRGB);
	break;

	case IDC_FILLHSL:
	lpData->bFillHSL = YES;
	CheckRadioButton(hDlg, IDC_FILLRGB, IDC_FILLHSL,
		lpData->bFillHSL ? IDC_FILLHSL : IDC_FILLRGB);
	break;

	case IDC_FILLMAX:
	lpData->bFillMax = !lpData->bFillMax;
	CheckDlgButton(hDlg, id, lpData->bFillMax);
	ControlEnable(hDlg, IDC_PALETTEFILL, !lpData->bFillMax);
	break;

	case IDC_PALETTEFILL_COLOR1:
	if ( !ColorPicker( &Color1, NULL ) )
		break;
	CopyRGB( &Color1.rgb, &Color );
	SetWindowLong( GetDlgItem( hDlg, id ), GWL_ACTIVE, Color );
	AstralControlRepaint( hDlg, id );
	break;

	case IDC_PALETTEFILL_COLOR2:
	if ( !ColorPicker( &Color2, NULL ) )
		break;
	CopyRGB( &Color2.rgb, &Color );
	SetWindowLong( GetDlgItem( hDlg, id ), GWL_ACTIVE, Color );
	AstralControlRepaint( hDlg, id );
	break;

	case IDC_PALETTEFILL:
	lpData->iFill = GetDlgItemSpin(hDlg, IDC_PALETTEFILL, &Bool, NO );
	break;

	case IDOK:
	if (lpData->bFillMax)
		i = 0;
	else
		i = bound(lpData->iFill, 0, 256);

	lpData->bRedo = lpData->iUndoFunction = 0;
	DoUndo(hDlg,IDC_PALFILL, lpData->iActiveEntry, 0, &Color1, &Color2, 0);
	if (Palette_FillEntries(lpData->lpPaletteList, lpData->iActivePalette, 
		lpData->iActiveEntry, &Color1, &Color2, i, lpData->bFillHSL))
		{
		AstralDlgEnd(hDlg, TRUE);
		break;
		}
	AstralDlgEnd(hDlg, FALSE);
	break;
	
	case IDCANCEL:
	AstralDlgEnd(hDlg, FALSE);
	break;

   default:
	break;
   }
UnlockPaletteData(hDlg,lpData);
}

void FillPalette_OnDestroy(HWND hwnd)
{
	// do nawting
}



/***********************************************************************/
static void SetPaletteMenus( HWND hDlg )
/***********************************************************************/
{
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) 
	return ;

HMENU	hMenu;
int	Enable;
LPPALETTE lpPalette;
STRING	szFormat, szString;
LPSTR	lpString;

if ( !(hMenu = GetMenu(hDlg)) )
{
	UnlockPaletteData(hDlg,lpData);
	return;
}

lpPalette = Palette_Get(lpData->lpPaletteList, NULL, lpData->iActivePalette);
Enable = (lpPalette == NULL) ? MF_GRAYED : MF_ENABLED;
EnableMenuItem(hMenu, IDC_PALMERGE, Enable);
EnableMenuItem(hMenu, IDC_PALSAVE, Enable);
EnableMenuItem(hMenu, IDC_PALSAVEAS, Enable);
EnableMenuItem(hMenu, IDC_PALINSERT, Enable);
EnableMenuItem(hMenu, IDC_PALAPPEND, Enable);
if (lpPalette)
	{
	EnableMenuItem(hMenu, IDC_PALRESET,
			lpData->bUntitled ? MF_GRAYED : MF_ENABLED);
	EnableMenuItem(hMenu, IDC_PALFILL,
 			(lpPalette->iColors > 1) ? MF_ENABLED : MF_GRAYED);
	EnableMenuItem(hMenu, IDC_PALDELETEITEM,
 			(lpPalette->iColors > 0) ? MF_ENABLED : MF_GRAYED);
	EnableMenuItem(hMenu, IDC_PALSETACTIVE,
 			(lpPalette->iColors > 0) ? MF_ENABLED : MF_GRAYED);
	EnableMenuItem(hMenu, IDC_PALLABEL,
 			(lpPalette->iColors > 0) ? MF_ENABLED : MF_GRAYED);
	EnableMenuItem(hMenu, IDC_PALFIND,
 			(lpPalette->iColors > 0) ? MF_ENABLED : MF_GRAYED);
	EnableMenuItem(hMenu, IDC_PALRESET, lpData->bChanged ? MF_ENABLED : MF_GRAYED);
	}
else
	{
	EnableMenuItem(hMenu, IDC_PALRESET, Enable);
	EnableMenuItem(hMenu, IDC_PALFILL, Enable);
	EnableMenuItem(hMenu, IDC_PALDELETEITEM, Enable);
	EnableMenuItem(hMenu, IDC_PALSETACTIVE, Enable);
	EnableMenuItem(hMenu, IDC_PALLABEL, Enable);
	EnableMenuItem(hMenu, IDC_PALFIND, Enable);
	EnableMenuItem(hMenu, IDC_PALRESET, Enable);
	}
szFormat[0] = '\0';
if (!lpData->iUndoFunction || !lpData->bRedo)
	{
	if (AstralStr(IDS_UNDO, &lpString))
		lstrcpy(szFormat, lpString);
	}
else
	{
	if (AstralStr(IDS_REDO, &lpString))
		lstrcpy(szFormat, lpString);
	}
szString[0] = '\0';
if (!lpData->iUndoFunction)
	lstrcpy(szString, szFormat);
else
	{
	if (AstralStr(lpData->iUndoFunction, &lpString))
		{
		lstrcpy(szString, szFormat);
		lstrcat(szString, lpString);
		}
	else
		wsprintf(szString, szFormat, (LPSTR)"");
	}
ModifyMenu( hMenu, IDC_PALUNDO, MF_BYCOMMAND | MF_STRING,
		IDC_PALUNDO, (LPSTR)szString);
EnableMenuItem(hMenu, IDC_PALUNDO,
		((lpData->iUndoFunction && lpPalette) ? MF_ENABLED : MF_GRAYED));
if ( lpData->bSetActive )
		Enable = MF_CHECKED;
else	Enable = MF_UNCHECKED;
CheckMenuItem( hMenu, IDC_PALAUTOSET, Enable|MF_BYCOMMAND );

UnlockPaletteData(hDlg,lpData);
}


/***********************************************************************/
BOOL PaletteVertical( void )
/***********************************************************************/
{
return (bVertical);
}


/***********************************************************************/
static void SetSizePalette( HWND hDlg, int iRows, int iCols )
/***********************************************************************/
{
LPPALETTEDATA lpData = LockPaletteData(hDlg);
if (!lpData) 
	return ;

HWND hControl;
RECT ControlRect, ClientRect, WindowRect;
int iWidth, iHeight, dx, dy;

if ( !hDlg )
	return;

if ( !(hControl = GetDlgItem( hDlg, IDC_PALETTE ) ))
	return;

GetWindowRect( hDlg, &WindowRect );
GetClientRect( hDlg, &ClientRect );
GetWindowRect( hControl, &ControlRect );

GetChipSize( &dx, &dy );
iWidth = (iCols * dx) + 6;
iWidth += ( RectWidth(&WindowRect) - RectWidth(&ControlRect) );
iHeight = (iRows * dy) + 6;
iHeight += ( RectHeight(&WindowRect) - RectHeight(&ControlRect) );
// check for scroll bars
if (lpData->lpPaletteList)
    if (lpData->lpPaletteList[0].iColors > iRows*iCols)
    {
        if (bVertical)
            iWidth+=GetSystemMetrics(SM_CXVSCROLL);
        else
            iHeight+=GetSystemMetrics(SM_CYHSCROLL);
    }

SetWindowPos( hDlg, NULL, 0, 0, iWidth, iHeight,
	SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE );

lpData->PaletteCols = iCols;
lpData->PaletteRows = iRows;
UnlockPaletteData(hDlg,lpData);
}


/***********************************************************************/
static void GetSizePalette( HWND hDlg, LPINT lpRows, LPINT lpCols )
/***********************************************************************/
{
RECT           ClientRect;
int            iRowIncr, iColIncr, iStart;
LPPALETTEDATA  lpData;

if ( !hDlg )
	return;
lpData = LockPaletteData(hDlg);
if (!lpData) 
	return ;

if ( !(hDlg = GetDlgItem( hDlg, IDC_PALETTE )) )
	return;
GetChipLayout( hDlg, &ClientRect, lpRows, lpCols, &iRowIncr, &iColIncr,
	&iStart );
lpData->PaletteCols = *lpCols;
lpData->PaletteRows = *lpRows;
UnlockPaletteData(hDlg,lpData);
}
