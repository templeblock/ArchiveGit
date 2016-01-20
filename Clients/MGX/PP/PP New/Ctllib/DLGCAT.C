/**************************************************************************
	Dynamic Dialog Boxes
	Dynamic Dialog Box Category Management Module
	Written by Atif Aziz, May 1993
**************************************************************************/

#define STRICT

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>
#include "debug.h"
#include "dlgcat.h"

	// Constants

#define CCH_MAX_CLASSNAME       80
#define CCH_MAX_TITLE           256

	// Dialog item template variable information

typedef struct tagDLGITEMTEMPLATEVARINFO        // ditvi
{
	LPTSTR          lpszClassName;
	int             cchClassName;
	LPTSTR          lpszTitle;
	int             cchTitle;
	LPVOID          lpvData;
}
DLGITEMTEMPLATEVARINFO;

TYPEDEF_POINTERS(DLGITEMTEMPLATEVARINFO, DLGITEMTEMPLATEVARINFO)

	// Function prototypes

static LPTSTR                   CopyResourceString(LPTSTR, LPRSTR, int);
static LPDLGITEMTEMPLATE        GetDlgItemTemplateVarInfo(LPDLGITEMTEMPLATE,
					LPDLGITEMTEMPLATEVARINFO);

/************************************************************************/
static BOOL StringsEqual( LPTSTR pszStr1, LPTSTR pszStr2 )
/************************************************************************/
{
    int	i = lstrcmpi( pszStr1, pszStr2 );
    return( i ? FALSE : TRUE );
}

#define BYTEPTRPLUS(lpv, n)             (((LPTSTR) (lpv)) + (n))
#define AfterDlgResString(lpcsz)        ((lpcsz) + (_rcslen(lpcsz)) + 1)

	// Function implementations specific to the target environment

#ifdef WIN32

  #define GetDlgItemTemplateExtStyle(lpdit)     (lpdit)->dwExtendedStyle

  static LPVOID AlignToNextDlgItem(LPVOID);

#else

  // Win16 dialog item templates do not contain an extended style field.

  #define GetDlgItemTemplateExtStyle(lpdit)     0L

  // We don't need to align resources on DWORD boundaries in Win16,
  // therefore just define this function as a dumb stub.

  #define AlignToNextDlgItem(lpvResItem)   (lpvResItem)

#endif

#ifdef WIN32

typedef enum        // current view mode of image
    {
    WV_NT,
    WV_WIN32S,
    WV_WINDOWS95
    } WIN_VERSION;

WIN_VERSION GetWinVersion()
{
DWORD		dwVersion = GetVersion();
WIN_VERSION	wv;
    if (dwVersion < 0x80000000L)
        wv = WV_NT;
    else if (LOBYTE(LOWORD(dwVersion)) < 4)
        wv = WV_WIN32S;
    else
        wv = WV_WINDOWS95;
return(wv);
}

int MylstrlenW(LPCWSTR	lpcrsz)
	{
	int		i = 0;
	if (GetWinVersion() != WV_NT)
		{
		LPWORD	lp = (LPWORD)lpcrsz;
		while (*lp++)
			i++;
		}
	else
		i = lstrlenW(lpcrsz);
	return(i);
	}
// -----------------------------------------------------------------------
//  PURPOSE:
//      This is a helper function for internal use by this module. It
//      double-word aligns the lpvResItem pointer. This is used for
//      resource headers and data items which are DWORD aligned in Win32.
//
//  RETURN:
//      Returns a double-word aligned pointer to the the next resource
//      item.
//  COMMENTS:
//      This function is defined for Win32 only. Its Win16 counterpart
//      is a dumb macro stub returning lpvResItem.
// -----------------------------------------------------------------------

static LPVOID AlignToNextDlgItem(lpvResItem)
LPVOID  lpvResItem;
{
  ULONG ul;

  CHECK_PARAMETER(lpvResItem, NULL);

  ul = (ULONG) lpvResItem;

  ul += 3;
  ul >>= 2;             // Truncate the 2 least significant bits to 
  ul <<= 2;             // achieve the double-word alignment
  return (LPVOID) ul;
}

#endif

// -----------------------------------------------------------------------
//  PURPOSE:
//      This is a helper function for internal use by this module. It
//      copies and converts a resource string (LPRSTR) into a standard
//	LPTSTR string. In Win16 and Win32 Unicode, this function simply
//	calls lstrcpyn because the resource strings are compatible with
//	LPTSTR strings. In Win32 ANSI, however, resource string are
//	two-byte Unicode character arrays and LPTSTR string are
//	single-byte ANSI character arrays. In this case, the
//	WideCharToMultiByte function is used for conversion.
//
//  RETURN:
//      Returns a pointer to lpszDest.
// -----------------------------------------------------------------------

static LPTSTR CopyResourceString(lpszDest, lprszRes, cchDest)
LPTSTR	lpszDest;
LPRSTR	lprszRes;
int	cchDest;
{
  CHECK_PARAMETER(lpszDest, NULL);
  CHECK_PARAMETER(lprszRes, NULL);
  CHECK_PARAMETER(cchDest > 0, NULL);

#if defined(WIN32) && !defined(UNICODE)

  // In Win32 ANSI, translate the double-byte character string lprszRes
  // into the single-byte character string lpszDest.

  {
    BOOL  fDefCharUsed;

    WideCharToMultiByte(CP_ACP, 0, lprszRes, -1, lpszDest, cchDest, NULL,
      &fDefCharUsed);
  }

#else   

  // In Win16 and Win32 Unicode, we simply copy the string.

  lstrcpyn(lpszDest, lprszRes, cchDest);

#endif  

  return lpszDest;
} 

// -----------------------------------------------------------------------
//  PURPOSE:
//      This is a helper function for internal use by this module. It
//      presumes that lpdit points to a dialog box item template in an
//      array of dialog box item templates. It begins by skipping the
//      fixed-length portion of the lpdit dialog box item and then
//      retrieves the variable-length portions into their corresponding
//      fields in lpditvi.
//
//  RETURN:
//      Returns a pointer to the beginning of the next dialog item
//	template.
//
//  COMMENTS:
//      The LPTSTR fields of lpditvi can be NULL if the information is
//      not required. 
// -----------------------------------------------------------------------

static LPDLGITEMTEMPLATE GetDlgItemTemplateVarInfo(lpdit, lpditvi)
LPDLGITEMTEMPLATE               lpdit;
LPDLGITEMTEMPLATEVARINFO        lpditvi;                
{
  TCHAR                         szNum[15];
  WORD                          wCtlDataSize;
  int                           iClass;
  BOOL                          fAtomicClass = FALSE;
  LPDLGITEMCLASSTEMPLATE       lpvdict;
  LPDLGITEMTITLETEMPLATE       lpvditt;
  LPDLGITEMDATATEMPLATE        lpvdidt;
  LPDLGITEMTEMPLATE             lpditNext;
  static LPCTSTR                lpszWinCtlClass[] =
				{
					_T("Button"),
					_T("Edit"),
					_T("Static"),
					_T("Listbox"),
					_T("Scrollbar"),
					_T("Combobox")
				};

  CHECK_PARAMETER(lpdit, NULL);
  CHECK_PARAMETER(lpditvi, NULL);

  // Skip the fixed part of the dialog box item template.

  lpvdict = (LPDLGITEMCLASSTEMPLATE) (lpdit + 1);

  // Set the control class name ...

#ifdef WIN32

  // In Win32, the first element of the class name indicates whether we
  // are dealing with a class name identified by an atom (ordinal) or a
  // Unicode string.

  if (lpvdict->alt.rchScheme == RCH_ORDINAL_SCHEME)
  {
    iClass = lpvdict->alt.rchAtom & 0x000f;
    fAtomicClass = TRUE;
  }

#else 

  // In Win16, if the most-significant bit of the first element is set,
  // then it indicates a class name identified by an ordinal value,
  // otherwise it is a null-terminated string.

  if (lpvdict->alt.rchAtom & 0x80)
  {
    iClass = lpvdict->alt.rchAtom & 0x0f;
    fAtomicClass = TRUE;
  }

#endif

  // Is the class name being represented by an atom?

  if (fAtomicClass)
  {
    // Is the class name asked for?

    if (lpditvi->lpszClassName)
      lstrcpyn(lpditvi->lpszClassName, lpszWinCtlClass[iClass],
	lpditvi->cchClassName);

    lpvditt = (LPDLGITEMTITLETEMPLATE) BYTEPTRPLUS(lpvdict,
      sizeof(DLGITEMCLASSTEMPLATE));
  }
  else
  {
    // Is the class name asked for?

    if (lpditvi->lpszClassName)
      CopyResourceString(lpditvi->lpszClassName, lpvdict->szName,
	lpditvi->cchClassName);

    lpvditt = (LPDLGITEMTITLETEMPLATE) AfterDlgResString(lpvdict->szName);
  }

  // Set the control's title.

  if (lpvditt->alt.rchScheme == RCH_ORDINAL_SCHEME)
  {
    if (lpditvi->lpszTitle)
    {
      wsprintf(szNum, _T("#%d"), lpvditt->alt.wOrd);
      lstrcpyn(lpditvi->lpszTitle, szNum, lpditvi->cchTitle);
    }

    lpvdidt = (LPDLGITEMDATATEMPLATE) BYTEPTRPLUS(lpvditt,
      sizeof(DLGITEMTITLETEMPLATE));
  }
  else
  {
    if (lpditvi->lpszTitle)
      CopyResourceString(lpditvi->lpszTitle, lpvditt->szTitle,
	lpditvi->cchTitle);

    lpvdidt = (LPDLGITEMDATATEMPLATE) AfterDlgResString(lpvditt->szTitle);
  }

  // Get the control-specific data only if it's size is not zero.

  wCtlDataSize = (WORD) lpvdidt->rchSize;

  lpditvi->lpvData = (LPVOID) (wCtlDataSize ? lpvdidt->bData : NULL);

  // Get the next dialog item's location.

  lpditNext = (LPDLGITEMTEMPLATE) BYTEPTRPLUS(lpvdidt,
    wCtlDataSize + sizeof(lpvdidt->rchSize));

  return (LPDLGITEMTEMPLATE) AlignToNextDlgItem(lpditNext);
}

// -----------------------------------------------------------------------
//  PURPOSE:
//      Loads a category dialog box resource, but does not create any of
//      its controls. The dialog box resource is retained until the
//      category is destroyed.
//
//  RETURN:
//      TRUE  = Success.
//      FALSE = Could not find or load dialog resource.
// 
//  COMMENTS:
//	This function also fills the lpditList field of the lpdlgcat
//	structure with a pointer to the first dialog item template.
//	This makes it easier for other functions to quickly skip
//	the dialog box header.
// -----------------------------------------------------------------------

BOOL LoadDlgCategory(lpdlgcat)
LPDLGCATEGORY   lpdlgcat;
{
  LPDLGTEMPLATE         lpdt;
  LPDLGMENUTEMPLATE	lpvdmt;
  LPDLGCLASSTEMPLATE	lpvdct;
  LPDLGFONTTEMPLATE	lpvdft;
  LPRSTR                lprszTitle;
  HRSRC                 hrsrc;

  CHECK_PARAMETER(lpdlgcat, FALSE);

  hrsrc = FindResource(lpdlgcat->hinst, lpdlgcat->lpcszTemplateName,
    RT_DIALOG);

  if (hrsrc)
  {
    if ((lpdlgcat->hglbTemplate = LoadResource(lpdlgcat->hinst, hrsrc)) != NULL)
    {
      lpdt = (LPDLGTEMPLATE) LockResource(lpdlgcat->hglbTemplate);
      if (lpdt)
      {
	lpdlgcat->hlocWndList = NULL;
	lpdlgcat->nItemCount = lpdt->cdit;

	// The next few lines skip the information in the dialog box header
	// as this is of no interest. What we need is the dialog item
	// templates that come after.

	lpvdmt = (LPDLGMENUTEMPLATE) (lpdt + 1);

	// If there is a menu name associated, then skip it.

	// Is the menu resource ordinal?

	if (lpvdmt->alt.rchScheme == RCH_ORDINAL_SCHEME)
        {
	  lpvdct = (LPDLGCLASSTEMPLATE) BYTEPTRPLUS(lpvdmt,
	    sizeof(DLGMENUTEMPLATE));
        }
	else
	  lpvdct = (LPDLGCLASSTEMPLATE) AfterDlgResString(lpvdmt->szName);

	// If there is a class name associated, then skip it.

#ifdef WIN32
	  // Under Windows NT, the first character of the class name is
	  // 0xffff when specifying an atom that represents the registered
	  // window class. Otherwise the class name is a Unicode string.  

	if (lpvdct->alt.rchScheme == RCH_ORDINAL_SCHEME)
        {
	  lprszTitle = (LPRSTR) BYTEPTRPLUS(lpvdct,
	    sizeof(DLGCLASSTEMPLATE));
	}
	else
	  // Leak to below...
#endif
	lprszTitle = AfterDlgResString(lpvdct->szName);

	// Skip the dialog caption.

	lpvdft = (LPDLGFONTTEMPLATE) AfterDlgResString(lprszTitle);

	// Skip the dialog box point size and face name only if the
	// DS_SETFONT style was specified. 

	if (lpdt->style & DS_SETFONT)
	{
	  int   nSize;

	  nSize = sizeof(lpvdft->wSize) + (_rcslen(lpvdft->szName) + 1) * sizeof(RCHAR);

	  // Note a pointer to the first dialog item template that appears
          // after the dialog font information. 

	  lpdlgcat->lpditList = (LPDLGITEMTEMPLATE) BYTEPTRPLUS(lpvdft, nSize);
	  lpdlgcat->lpditList = (LPDLGITEMTEMPLATE) AlignToNextDlgItem(lpdlgcat->lpditList);
	}
	else
        {
	  // Note a pointer to the first dialog item template.

	  lpdlgcat->lpditList = (LPDLGITEMTEMPLATE) lpvdft;
        }

	return TRUE;
      }
      else
	FreeResource(lpdlgcat->hglbTemplate);
    }
  }
  return FALSE;
}

// -----------------------------------------------------------------------
//  PURPOSE:
//      Releases the category's dialog box resource and destroys the
//      associated controls if any had been created.
//
//  RETURN:
//      TRUE  = Success.
//      FALSE = Could not free the dialog resource.
// -----------------------------------------------------------------------

BOOL DeleteDlgCategory(lpdlgcat)
LPDLGCATEGORY   lpdlgcat;
{
  BOOL  fResult;

  CHECK_PARAMETER(lpdlgcat, FALSE);

  // Release the dialog box resource.

  UnlockResource(lpdlgcat->hglbTemplate);
  fResult = !FreeResource(lpdlgcat->hglbTemplate);

  // Destroy the category's dialog box controls.

  DestroyDlgCategoryControls(lpdlgcat);

  // Reset related structure values.

  lpdlgcat->hglbTemplate = NULL;
  lpdlgcat->nItemCount = 0;
  lpdlgcat->lpditList = NULL;
  if (lpdlgcat->hEditDS)
  	{
	GlobalFree(lpdlgcat->hEditDS);
	lpdlgcat->hEditDS = NULL;
	}

  return fResult;
}

// -----------------------------------------------------------------------
//  PURPOSE:
//      Destroys the controls of a category and frees all associated 
//      memory.
//
//  RETURN:
//      TRUE  = Success.
//      FALSE = Failure.
// -----------------------------------------------------------------------

BOOL DestroyDlgCategoryControls(lpdlgcat)
LPDLGCATEGORY   lpdlgcat;
{
  int                   i;
  NPHWND                nphwnd;

  CHECK_PARAMETER(lpdlgcat, FALSE);

  // Only destroy category control if they had been created.

  if (lpdlgcat->hlocWndList)
  {
    if (!(nphwnd = (NPHWND) LocalLock(lpdlgcat->hlocWndList)))
      return FALSE;

    for (i = 0; i < lpdlgcat->nItemCount; i++, nphwnd++)
      DestroyWindow(*nphwnd);

    LocalUnlock(lpdlgcat->hlocWndList);

#ifdef __BORLANDC__
#pragma warn -eff
#endif
    LocalFree(lpdlgcat->hlocWndList);
#ifdef __BORLANDC__
#pragma warn +eff
#endif

    lpdlgcat->hlocWndList = NULL;
  }

  return TRUE;
}

// -----------------------------------------------------------------------
//  PURPOSE:
//      Renders a category's controls into a dialog box. The nFrameID
//      identifies a control in the dialog box that is used as the
//      category's frame---meaning that the frame control's window
//      position is used to position the category's controls.
//
//  RETURN:
//      TRUE  = Success.
//      FALSE = Failure.
//
//  COMMENTS:
//      The category's controls inherit the same font as the dialog box.
//	The category's dialog box resource must have been previously
//	loaded via LoadDlgCategory.
// -----------------------------------------------------------------------

BOOL CreateDlgCategoryIndirect(lpdlgcat, hwndDlg, nFrameID)
LPDLGCATEGORY   lpdlgcat;
HWND            hwndDlg;
int             nFrameID;
{
  int                           i;
  RECT                          rcCtl;
  POINT                         pt;
  LPDLGITEMTEMPLATE             lpdit;
  LPDLGITEMTEMPLATE             lpditNext;
  HFONT                         hfont;
  NPHWND                        nphwnd;
  HWND                          hwndInsertAfter;
  TCHAR                         szClassName[CCH_MAX_CLASSNAME + 1];
  TCHAR                         szTitle[CCH_MAX_TITLE + 1];
  DLGITEMTEMPLATEVARINFO        ditvi;
  HGLOBAL						hEditMem = NULL;
  HINSTANCE						hInst;

  CHECK_PARAMETER(lpdlgcat, FALSE);
  CHECK_PARAMETER(IsWindow(hwndDlg), FALSE);
  CHECK_PARAMETER(GetDlgItem(hwndDlg, nFrameID), FALSE);

  ASSERT(lpdlgcat->lpditList);

  // Get the frame window which is where we will begin inserting
  // the category's controls.

  hwndInsertAfter = GetDlgItem(hwndDlg, nFrameID);
  if (!hwndInsertAfter)
    return FALSE;

  // Get the frame window's rectangle and map it into the dialog's
  // coordinates so that category controls can be aligned to it.

  GetWindowRect(hwndInsertAfter, &rcCtl);
  MapWindowRect(NULL, hwndDlg, &rcCtl);
  pt.x = rcCtl.left;
  pt.y = rcCtl.top;

  // Allocate memory to hold the window handles of each control.

  lpdlgcat->hlocWndList = LocalAlloc(LHND,
    sizeof(HWND) * lpdlgcat->nItemCount);

  if (!lpdlgcat->hlocWndList)
    return FALSE;

  if (!(nphwnd = (NPHWND) LocalLock(lpdlgcat->hlocWndList)))
    return FALSE;

  hfont = GetWindowFont(hwndDlg);

  // Setup buffers to receive the variable-length information from a
  // dialog item's template.

  ditvi.lpszClassName = szClassName;
  ditvi.cchClassName = CCH_MAX_CLASSNAME;
  ditvi.lpszTitle = szTitle;
  ditvi.cchTitle = CCH_MAX_TITLE;

  // Loop through each item's template and create the control.

  for (i = 0, lpdit = lpdlgcat->lpditList; i < lpdlgcat->nItemCount;
    i++, nphwnd++, lpdit = lpditNext)
  {
    // Get the variable-length information in the dialog box item template.

    lpditNext = GetDlgItemTemplateVarInfo(lpdit, &ditvi);

    // Map the control's rectangle and create its window.

    rcCtl.left = lpdit->x;
    rcCtl.top = lpdit->y;
    rcCtl.right = lpdit->cx;
    rcCtl.bottom = lpdit->cy;

    // Control coordinates are in dialog box units, so convert these
    // into pixel units because that is what CreateWindowEX expects
    // them  to be in.

    MapDialogRect(hwndDlg, &rcCtl);
#ifndef WIN32
	if (StringsEqual(ditvi.lpszClassName, _T("edit")))
		{
		if (!lpdlgcat->hEditDS)
			{
			lpdlgcat->hEditDS = GlobalAlloc( GHND|GMEM_SHARE, 256L );
			if (!lpdlgcat->hEditDS)
				{
			    LocalUnlock(lpdlgcat->hlocWndList);
			    return FALSE;
				}
			}
		hInst = (HINSTANCE)lpdlgcat->hEditDS;
		}
	else
#endif
		hInst = GetWindowInstance(hwndDlg);

    *nphwnd = CreateWindowEx(GetDlgItemTemplateExtStyle(lpdit),
      ditvi.lpszClassName, ditvi.lpszTitle,
      lpdit->style & ~(WS_VISIBLE), // Force invisible
      rcCtl.left + pt.x, rcCtl.top + pt.y, rcCtl.right, rcCtl.bottom,
      hwndDlg, (HMENU) lpdit->id, hInst, ditvi.lpvData);

    if (!*nphwnd)     
    {
      LocalUnlock(lpdlgcat->hlocWndList);
      return FALSE;
    }

    // Correct the z-order of the newly creatred control.

    SetWindowPos(*nphwnd, hwndInsertAfter, 0, 0, 0, 0,
      SWP_NOMOVE | SWP_NOSIZE);

    // The next control will be inserted after this new one.

    hwndInsertAfter = *nphwnd;

    if (hfont)
      SetWindowFont(*nphwnd, hfont, FALSE);
  }

  LocalUnlock(lpdlgcat->hlocWndList);

  return TRUE;
}

// -----------------------------------------------------------------------
//  PURPOSE:
//      Shows the controls of a dialog category. To work around a bug in
//      Windows, this function removes the text of controls with
//      mnemonics when hiding them, and then restores it from the
//      dialog resource template when showing them.
//
//  RETURN:
//      TRUE  = Success.
//      FALSE = Failure.
// -----------------------------------------------------------------------

BOOL ShowDlgCategory(lpdlgcat, fShow)
LPDLGCATEGORY   lpdlgcat;
BOOL            fShow;
{
  int                           i;
  UINT                          dwCtlCode;
  NPHWND                        nphwnd;
  LPDLGITEMTEMPLATE             lpdit;
  DLGITEMTEMPLATEVARINFO        ditvi;
  TCHAR                         szTitle[CCH_MAX_TITLE + 1];
  BOOL                          fCtlText;

  CHECK_PARAMETER(lpdlgcat, FALSE);

  ASSERT(lpdlgcat->hlocWndList);

  if (!(nphwnd = (NPHWND) LocalLock(lpdlgcat->hlocWndList)))
    return FALSE;

  // Setup to receive the title of a dialog item from its variable-length
  // information.

  memset(&ditvi, 0, sizeof(ditvi));

  ditvi.lpszTitle = szTitle;
  ditvi.cchTitle = CCH_MAX_TITLE;

  lpdit = lpdlgcat->lpditList;

  for (i = 0; i < lpdlgcat->nItemCount; i++, nphwnd++)
  {
    fCtlText = TRUE;

    dwCtlCode = (UINT) SendMessage(*nphwnd, WM_GETDLGCODE, 0, 0);

    if (dwCtlCode & DLGC_WANTCHARS)
      fCtlText = FALSE;

    if ((dwCtlCode & DLGC_STATIC) &&
      (GetWindowStyle(*nphwnd) & SS_NOPREFIX))
      fCtlText = FALSE;

    if (fShow)
    {
      // Get the title from the variable-length information in the dialog
      // box item template.

      lpdit = GetDlgItemTemplateVarInfo(lpdit, &ditvi);

      if (fCtlText)
		SetWindowText(*nphwnd, ditvi.lpszTitle);

      // Show window after restoring the title to avoid unnecessary painting
      // or screen flicker.

      ShowWindow(*nphwnd, SW_SHOW);
    }
    else
    {
      // Hide window before resetting the title to avoid unnecessary painting
      // or screen flicker.

      ShowWindow(*nphwnd, SW_HIDE);


      if (fCtlText)
		SetWindowText(*nphwnd, _T(""));
    }
  }

  LocalUnlock(lpdlgcat->hlocWndList);

  return TRUE;
}

