/**************************************************************************
	Dynamic Dialog Boxes
	Dynamic Dialog Box Section Management Module
	Written by Atif Aziz, May 1993
**************************************************************************/

#define STRICT

#include <windows.h>
#include <windowsx.h>
#include <memory.h>
#include "debug.h"
#include "dyndlg.h"

	// Function prototypes

static BOOL			UseDlgSectionCategory(LPDLGSECTION,
					       LPSECTIONCATEGORY);
static BOOL			AdjustDlgSectionLoad(LPDLGSECTION, BOOL fPurgeCategories);
static LPSECTIONCATEGORY	GetDlgSectionCategory(LPCDLGSECTION, int);

// -----------------------------------------------------------------------
//  PURPOSE:
//	Uses a cateogry, thus loading it if was previously discarded or
//	never preloaded.
//
//  RETURN:
//	TRUE  = Success.
//	FALSE = Failure loading the category dialog resource. 
// -----------------------------------------------------------------------

static BOOL UseDlgSectionCategory(lpdlgsec, lpseccat)
LPDLGSECTION		lpdlgsec;
LPSECTIONCATEGORY	lpseccat;
{
  CHECK_PARAMETER(lpdlgsec, FALSE);
  CHECK_PARAMETER(lpseccat, FALSE);

  // If the category was not pre-loaded or had been previously discarded,
  // then re-load it.

  if (!lpseccat->dlgcat.hlocWndList)
  {
    if (!CreateDlgCategoryIndirect(&lpseccat->dlgcat, lpdlgsec->hwndDlg,
      lpdlgsec->nFrameID))
      return FALSE;

    lpdlgsec->nLoaded++;

    // Inform the main dialog box that a section has been loaded. This
    // permits the section to be initialised if it had been previously
    // discarded or deferred its loading by not specifying the
    // DSCF_PRELOAD flag.

    FORWARD_WM_COMMAND(lpdlgsec->hwndDlg, lpdlgsec->nID,
      (HWND) lpseccat->nID, DSN_LOADED, SendMessage);

    // Do any discarding that may be required as a result of using the
    // specified category.

    AdjustDlgSectionLoad(lpdlgsec, FALSE);

  }

  return TRUE;
}

// -----------------------------------------------------------------------
//  PURPOSE:
//	Adjusts the number of categories by discarding or loading to fit
//	within the minimum-maximum thresholds. If fPurgeCategories is
//  TRUE then all categories controls are discarded.
//  RETURN:
//	TRUE  = Success.
//	FALSE = Failure loading the category dialog resource. 
// -----------------------------------------------------------------------

static BOOL AdjustDlgSectionLoad(lpdlgsec, fPurgeCategories)
LPDLGSECTION	lpdlgsec;
BOOL			fPurgeCategories;
{
  int			i;
  LPSECTIONCATEGORY	lpseccatToDiscard;

  CHECK_PARAMETER(lpdlgsec, FALSE);

  // Check if the threshold system is active and if the number of
  // categories loaded is above the maximum limit.

  if (fPurgeCategories ||
  	(lpdlgsec->nMinThreshold > 0 &&
    lpdlgsec->nMaxThreshold >= lpdlgsec->nMinThreshold &&
    lpdlgsec->nLoaded > lpdlgsec->nMaxThreshold) )
  {
    // OK, we need to possibly discard one or more categories.

    lpseccatToDiscard = lpdlgsec->lpseccatList;

    // Loop through all sections.

    for (i = 0; i < lpdlgsec->nCount; i++, lpseccatToDiscard++)
    {
      // Make sure that the section in question is not currently
      // selected, is discardable and is not already discarded.

      if (lpseccatToDiscard->nID != lpdlgsec->nCurrentID &&
	(lpseccatToDiscard->uFlags & DSCF_DISCARDABLE) &&
	lpseccatToDiscard->dlgcat.hlocWndList)
      {
	  // Inform the main dialog box that we are about to discard the
	  // section's dialog box. This permits the user to save the
	  // current of each control.

	  FORWARD_WM_COMMAND(lpdlgsec->hwndDlg, lpdlgsec->nID,
	    (HWND) lpseccatToDiscard->nID, DSN_DISCARDING,
	    SendMessage);

	  // Destroy the section dialog's controls.

	  DestroyDlgCategoryControls(&lpseccatToDiscard->dlgcat);

	  if (!fPurgeCategories && --lpdlgsec->nLoaded == lpdlgsec->nMinThreshold)
	    break;
      }
    }
  }

  return TRUE;
}

// -----------------------------------------------------------------------
//  PURPOSE:
//	Initialises a dialog section structure with default values.
//
//  RETURN:
//	TRUE  = Success.
//	FALSE = Failure.
// -----------------------------------------------------------------------

BOOL InitDlgSection(lpdlgsec, nID, hwndDlg, nFrameID, lpvData)
LPDLGSECTION	lpdlgsec;
int		nID;
HWND		hwndDlg;
int		nFrameID;
LPVOID		lpvData;
{
  CHECK_PARAMETER(lpdlgsec, FALSE);
  CHECK_PARAMETER(IsWindow(hwndDlg), FALSE);
  CHECK_PARAMETER(GetDlgItem(hwndDlg, nFrameID), FALSE);

  lpdlgsec->nID = nID;			// ID of the section
  lpdlgsec->hwndDlg = hwndDlg;		// Dialog box window
  lpdlgsec->nFrameID = nFrameID;	// Alignment frame
  lpdlgsec->nCount = 0;			// Count of categories
  lpdlgsec->nCurrentID = DLGSEC_ERR;	// Currently selected category
  lpdlgsec->lpvData = lpvData;		// User data
  lpdlgsec->nMinThreshold = 0;		// Minimum threshold
  lpdlgsec->nMaxThreshold = 0;		// Maximum threshold
  lpdlgsec->nLoaded = 0;		// Count of categories loaded
  lpdlgsec->lpseccatList = NULL;	// Directory of categories

  return TRUE;
}

// -----------------------------------------------------------------------
//  PURPOSE:
//	Deletes a dialog section by destroying its all categories. 
//
//  RETURN:
//	TRUE  = Success.
//	FALSE = Failure.
// -----------------------------------------------------------------------

#ifdef __BORLANDC__	// GlobalFreePtr produces a 'code has no effect'
#pragma warn -eff	// warning under Borland C++, so suppress the 
#endif			// warning.				

BOOL DeleteDlgSection(lpdlgsec)
LPDLGSECTION	lpdlgsec;
{
  int			i;
  LPSECTIONCATEGORY	lpseccat;

  CHECK_PARAMETER(lpdlgsec, FALSE);

  // Free the list of categories allocated, but destroy each first.

  if (lpdlgsec->lpseccatList)
  {
    for (i = 0, lpseccat = lpdlgsec->lpseccatList;
	 i < lpdlgsec->nCount;
	 i++, lpseccat++)
    {
      DeleteDlgCategory(&lpseccat->dlgcat);
    }

    GlobalFreePtr(lpdlgsec->lpseccatList);
    lpdlgsec->nCount = 0;
  }

  return TRUE;
}

#ifdef __BORLANDC__
#pragma warn +eff	// Re-enable the 'code has no effect' warning.
#endif

// -----------------------------------------------------------------------
//  PURPOSE:
//	Adds a category to the dialog section, pre-loading it if
//	indicated.
//  RETURN:
//	TRUE  = Success.
//	FALSE = Failure.
// -----------------------------------------------------------------------

int AddDlgSectionCategory(lpdlgsec, hinst, lpcszTemplateName, nID,
		          uFlags, lpvData)
LPDLGSECTION	lpdlgsec;
HINSTANCE	hinst;
LPCTSTR		lpcszTemplateName;
int		nID;
UINT		uFlags;
LPVOID		lpvData;
{
  SECTIONCATEGORY	seccat;
  LPSECTIONCATEGORY	lpseccatNewList;

  CHECK_PARAMETER(lpdlgsec, 0);
  CHECK_PARAMETER(hinst, 0);
  CHECK_PARAMETER(lpcszTemplateName, 0);
//  CHECK_PARAMETER(lstrlen(lpcszTemplateName), 0);

  seccat.nID = nID;
  seccat.uFlags = uFlags;
  seccat.dlgcat.hinst = hinst;
  seccat.dlgcat.lpcszTemplateName = lpcszTemplateName;
  seccat.dlgcat.lpvData = lpvData;
  seccat.dlgcat.hEditDS = NULL;

  // Attempt to load the section's dialog box resource.

  if (!LoadDlgCategory(&seccat.dlgcat))
    return 0;

  // Create the actual section dialog controls only if the DSCF_PRELOAD
  // flag was specified. 

  if (uFlags & DSCF_PRELOAD)
    if (!UseDlgSectionCategory(lpdlgsec, &seccat))
      return 0;

  if (!lpdlgsec->lpseccatList)
  {
    // If no memory has been allocated for the dynamic section array,
    // the do so and set the counter to zero.

    lpdlgsec->nCount = 0;
    if (!(lpdlgsec->lpseccatList = (LPSECTIONCATEGORY) GlobalAllocPtr(GHND,
      sizeof(SECTIONCATEGORY))))
      return 0;
  }
  else
  {
    // Expand the section array to accomodate a new entry.

    lpseccatNewList = (LPSECTIONCATEGORY) GlobalReAllocPtr(lpdlgsec->lpseccatList,
      (lpdlgsec->nCount + 1) * sizeof(SECTIONCATEGORY), GMEM_ZEROINIT);

    if (!lpseccatNewList)
      return 0;

    lpdlgsec->lpseccatList = lpseccatNewList;
  }

  // Copy the section dialog structure passed into the section
  // directory entry.

  _fmemcpy(lpdlgsec->lpseccatList + lpdlgsec->nCount, &seccat,
    sizeof(SECTIONCATEGORY));

  // Increase the count of sections in the directory.

  return ++lpdlgsec->nCount;
}

// -----------------------------------------------------------------------
//  PURPOSE:
//	Deletes a category from the dialog section, destroys its
//	controls and releases memory.
//
//  RETURN:
//	Count of categories remaining in the dialog section.
//	A negative value indicates an error.
// -----------------------------------------------------------------------

#ifdef __BORLANDC__	// GlobalFreePtr produces a 'code has no effect'
#pragma warn -eff	// warning under Borland C++, so suppress the 
#endif			// warning.				

int DeleteDlgSectionCategory(lpdlgsec, nID)
LPDLGSECTION	lpdlgsec;
int		nID;
{
  LPSECTIONCATEGORY	lpseccat;

  CHECK_PARAMETER(lpdlgsec, -1);

  // Get the section from the ID.

  lpseccat = GetDlgSectionCategory(lpdlgsec, nID);

  if (lpseccat)
  {
    // If the section being deleted is the currently selected one,
    // then invalidate the current ID marker.

    if (lpdlgsec->nCurrentID == nID)
      lpdlgsec->nCurrentID = DLGSEC_ERR;

    // If the section dialog controls exist, then decrease the loaded
    // sections counter.

    if (lpseccat->dlgcat.hlocWndList)
      lpdlgsec->nLoaded--;

    DeleteDlgCategory(&lpseccat->dlgcat);

    // Move all succeeding array of sections down one and shrink the
    // section directory allocation to the new size.

    lpdlgsec->nCount--;
    _fmemcpy(lpseccat, lpseccat + 1,
      sizeof(SECTIONCATEGORY) * (lpdlgsec->nCount - (lpseccat - lpdlgsec->lpseccatList)));

    if (lpdlgsec->nCount)
      lpdlgsec->lpseccatList = (LPSECTIONCATEGORY) GlobalReAllocPtr(lpdlgsec->lpseccatList,
	(lpdlgsec->nCount) * sizeof(SECTIONCATEGORY), 0);
    else
    {
      GlobalFreePtr(lpdlgsec->lpseccatList);
      lpdlgsec->lpseccatList = NULL;
    }

    return lpdlgsec->nCount;
  }

  return -1;	  // Category not found.
}

#ifdef __BORLANDC__
#pragma warn +eff	// Re-enable the 'code has no effect' warning.
#endif

// -----------------------------------------------------------------------
//  PURPOSE:
//	Selects a category from a dialog section. It hides the controls
//	of the category that was previously selected and then shows
//	those belonging to the new selection.
//
//  RETURN:
//	TRUE  = Success.
//	FALSE = Failure.
// -----------------------------------------------------------------------

BOOL SelectDlgSectionCategory(lpdlgsec, nID)
LPDLGSECTION	lpdlgsec;
int		nID;
{
  RECT			rcFrame;
  RECT			rcUpdate;
  RECT			rcInvalid;
  LPSECTIONCATEGORY	lpseccat;
  BOOL			fResult = TRUE;
  BOOL			fChanged = FALSE;
  BOOL			fVisible;

  CHECK_PARAMETER(lpdlgsec, FALSE);

  // If the requested section is already selected, then do not proceed
  // any further.

  if (lpdlgsec->nCurrentID == nID)
    return TRUE;

  // fVisible, below, typically results to FALSE during WM_INITDIALOG
  // when this function is called but the dialog box hasn't been
  // displayed yet.

  fVisible = IsWindowVisible(lpdlgsec->hwndDlg);

  // Retrieve the current category only if it exists.

  if (lpdlgsec->nCurrentID != DLGSEC_ERR)
  {
    lpseccat = GetDlgSectionCategory(lpdlgsec, lpdlgsec->nCurrentID);

    if (lpseccat)
    {
      if (fVisible)
      {
	// Force a WM_PAINT message, if one is pending, to be processed
	// because it will be suppressed by the next call to the
        // SetWindowRedraw function.

	UpdateWindow(lpdlgsec->hwndDlg);

	// To optimise on screen refresh, tell dialog box not to
	// redraw changes.

	SetWindowRedraw(lpdlgsec->hwndDlg, FALSE);
      }

      ShowDlgCategory(&lpseccat->dlgcat, SDC_HIDE);
      fChanged = TRUE;

      if (fVisible)
	SetWindowRedraw(lpdlgsec->hwndDlg, TRUE);
    }
    else
      fResult = FALSE;
  }

  if (fResult)		// Everything OK so far?
  {
    // Retrieve the new category to be selected and show its controls.

    lpdlgsec->nCurrentID = nID;
    lpseccat = GetDlgSectionCategory(lpdlgsec, nID);

    if (lpseccat && UseDlgSectionCategory(lpdlgsec, lpseccat))
    {
      if (fVisible)
      {
	// Make sure we account for any updates that may have
	// appeared during the notification of discarding and
	// loading categories.

	GetUpdateRect(lpdlgsec->hwndDlg, &rcUpdate, FALSE);
	SetWindowRedraw(lpdlgsec->hwndDlg, FALSE);
      }

      ShowDlgCategory(&lpseccat->dlgcat, SDC_SHOW);
      fChanged = TRUE;

      if (fVisible)
	SetWindowRedraw(lpdlgsec->hwndDlg, TRUE);
    }
    else
	// is caller trying to select a non-existent category in?
	// this is a special call to cause all controls belonging
	// to this section to be destroyed
	if (lpdlgsec->nCurrentID == DLGSEC_ERR)
	{
		AdjustDlgSectionLoad(lpdlgsec, TRUE);
		fResult = TRUE;
	}
	else
	{
      fResult = FALSE;
	}
  }

  // Update only if any changes were made or if the dialog box window
  // is visible.

  if (fVisible && fChanged)
  {
    // Invalidate the section's frame region in the dialog box so that
    // all changes that have taken place are only redrawn once.

    GetWindowRect(GetDlgItem(lpdlgsec->hwndDlg, lpdlgsec->nFrameID),
      &rcFrame);
    MapWindowRect(NULL, lpdlgsec->hwndDlg, &rcFrame);
    UnionRect(&rcInvalid, &rcUpdate, &rcFrame);
    InvalidateRect(lpdlgsec->hwndDlg, &rcInvalid, TRUE);
  }

  return fResult;
}

// -----------------------------------------------------------------------
//  PURPOSE:
//	Sets the minimum and maximum threshold of the dialog section.
//	A value of 0 for maximum and minimum disables the threshold
//	system.
//
//  RETURN:
//	TRUE  = Success.
//	FALSE = Failure.
// -----------------------------------------------------------------------

BOOL SetDlgSectionThreshold(lpdlgsec, nMinThreshold, nMaxThreshold)
LPDLGSECTION	lpdlgsec;
int		nMinThreshold;
int		nMaxThreshold;
{
  CHECK_PARAMETER(lpdlgsec, FALSE);
  CHECK_PARAMETER(nMaxThreshold >= nMinThreshold >= 0, FALSE);

  lpdlgsec->nMinThreshold = nMinThreshold;
  lpdlgsec->nMaxThreshold = nMaxThreshold;

  // Adjust the dialog section load to reflect the new constraints.

  AdjustDlgSectionLoad(lpdlgsec, FALSE);

  return TRUE;
}

// -----------------------------------------------------------------------
//  PURPOSE:
//	Returns a dialog section cateogry given its ID.
//
//  RETURN:
//	A pointer to the section category with the specified ID.
//	Otherwise NULL if no category with the given ID was found.
// -----------------------------------------------------------------------

static LPSECTIONCATEGORY GetDlgSectionCategory(lpdlgsec, nID)
LPCDLGSECTION	lpdlgsec;
int		nID;
{
  int			i;
  LPSECTIONCATEGORY	lpseccat = lpdlgsec->lpseccatList;

  CHECK_PARAMETER(lpdlgsec, NULL);

  for (i = 0; i < lpdlgsec->nCount; i++, lpseccat++)
    if (lpseccat->nID == nID)
      return lpseccat;

  return NULL;	// Not found.
}

// Retrieves the ID of the currently selected category in the dialog
// section.

int GetDlgSectionCurrentID(lpdlgsec)
LPCDLGSECTION	lpdlgsec;
{
  CHECK_PARAMETER(lpdlgsec, DLGSEC_ERR);

  return lpdlgsec->nCurrentID;
}

// Retrieves the number of categories in a dialog section.

int GetDlgSectionCount(lpdlgsec)
LPCDLGSECTION	lpdlgsec;
{
  CHECK_PARAMETER(lpdlgsec, -1);

  return lpdlgsec->nCount;
}

// Retrieves user data associated with a dialog section.

LPVOID GetDlgSectionData(lpdlgsec)
LPCDLGSECTION	lpdlgsec;
{
  CHECK_PARAMETER(lpdlgsec, NULL);

  return lpdlgsec->lpvData;
}

