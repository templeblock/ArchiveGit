// (c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// routines to handle the image browsers thumbnail viewer
#include "pp.h"    
#include "id.h"
#include "bsrtypes.h"
#include "handlebs.h"
#include "bsrid.h"
#include "thmbsr.h"
#include "bsrutil.h"
#include "cthmmgr.h"
#include "browser.h"
#include "dbfmgr.h"
#include "ctllib.h"

//***************************external vars******************************/
extern LPBSRSTATE lpBsr;


//***************************static prototypes******************************/
LOCAL LPCTHMNAIL FindThumbnailAndFrame(LPFILELOC lpFile, BOOL bAutoThumbs, BOOL *bError);
LOCAL BOOL TryDefaultDBF(LPFILELOC lpFile, LPENTRY lpOutEntry);

//***************************global vars******************************/

//***********************************************************************/
//*****************************Functions***********************************/
//***********************************************************************/

/***********************************************************************/
// 	Extended Browser aware.
/***********************************************************************/
void MeasureThumb(HWND hDlg, LPMEASUREITEMSTRUCT lpMeasure )
/***********************************************************************/
{
	HWND hControl;
	int width, height;
	RECT rArea;
	int yNum;

	hControl = GetDlgItem(hDlg, IDC_BSR_THUMBS);
	GetWindowRect(hControl, &rArea);
	// determine the best fit
	yNum = RectHeight(&rArea)/64;
	yNum = Bound(yNum, 1, 3);
	height = (RectHeight(&rArea)- GetSystemMetrics(SM_CYHSCROLL))/yNum;
	width = height;
	lpMeasure->itemHeight = height;
	lpMeasure->itemWidth = width;

//	int fontH, fontW;
//	HDC hDC;
//	TEXTMETRIC metrics;
//	HFONT hSmallSystemFont, hOldFont;
//	BOOL bGotOldFont = FALSE;
//						   		
//	hDC  = NULL;
//	hSmallSystemFont = NULL;	
//	// try creating replica of font use in thubnails		
//	hControl = THUMBWINDOW(hDlg);
//	if (!hControl)
//		goto ExitNoGood;
//	hDC = GetDC(hControl);
//	if (!hDC)
//		goto ExitNoGood;
//    hSmallSystemFont = CreateFont (GetDeviceCaps(hDC,LOGPIXELSY) * 8 / 72,0,
//        0,0,FW_BOLD,0,0,0,0,0,0,0,FF_SWISS,_T("Helv"));
//	if (!hSmallSystemFont)
//		goto ExitNoGood;	
//	hOldFont = SelectObject(hDC, hSmallSystemFont);
//	bGotOldFont = TRUE;
//	if (!GetTextMetrics(hDC, &metrics))
//		goto ExitNoGood;
//	fontH = metrics.tmHeight+2;	
//	fontW = metrics.tmMaxCharWidth;
//	SelectObject(hDC, hOldFont);
//	DeleteObject(hSmallSystemFont);
//	ReleaseDC(hControl,hDC);
//	width = DEF_THM_X;
//   	width += 7;	//bevels etc.
//	height = width;
//	height += fontH; // text 	
//	lpMeasure->itemHeight = lpBsr->ThumbYSize = height;
//	lpMeasure->itemWidth = lpBsr->ThumbXSize = height;
//	return;
//	
//ExitNoGood:
//		if (bGotOldFont && hDC)
//			SelectObject(hDC, hOldFont);
//		if (hSmallSystemFont)
//			DeleteObject(hSmallSystemFont);
//		if (hDC)
//			ReleaseDC(hControl,hDC);
//		fontH = 20;
//		width = DEF_THM_X;
//		width += 7;	//bevels etc.
//		height = width;
//		height += fontH; // text & text space	
//		lpMeasure->itemHeight = lpBsr->ThumbYSize = height;
//		lpMeasure->itemWidth = lpBsr->ThumbXSize = height;
}

/***********************************************************************/
// 	Extended Browser aware.
/***********************************************************************/
void DrawThumb(HWND hDlg,LPDRAWITEMSTRUCT lpDraw )
/***********************************************************************/
{
	RECT ClientRect, ImageRect, rect, ClipRect;
	HWND hControl;
	HDC hDC;
	BOOL bFocus, bHilighted, bGrayed;
	BOOL bCurs, bMessageEnable, AutoThumbnails;
	int iOldMode;
	COLORREF lOldColor;
	HBRUSH hBrush;
	LPSTR lpString;
	FNAME szFileName;
	FILELOC FileLocation;
	LPCTHMNAIL lpThm;
	LPFRAME lpFrame;
	int	xAmount, yAmount, width, height;
	UINT fTextFlags;
	TEXTMETRIC metrics;
	HFONT hOldFont;
	BOOL bGotFile;
	BOOL bCDThm = FALSE;
	BOOL bError = FALSE;
	
	// Check state (ODS_??) and not action (ODA_??)
	#define OD_GRAYED (ODS_GRAYED | ODS_DISABLED)
	#define OD_SELECTED (ODS_SELECTED | ODS_CHECKED)
	
	if ( lpDraw->itemID == -1 )
		return;
	
	hDC = lpDraw->hDC;
	ClientRect = lpDraw->rcItem;
	hControl = GetDlgItem( hDlg, lpDraw->CtlID );
	bGrayed = (lpDraw->itemState & OD_GRAYED);
	bHilighted = ( (lpDraw->itemState & OD_SELECTED) && !bGrayed );
	bFocus = ( (lpDraw->itemState & ODS_FOCUS) && !bGrayed );
	
	// Check to see if its focus draw only
	if ( lpDraw->itemAction == ODA_FOCUS)
	{
		// Draw the box interior
		if (bHilighted)
			HilightFrame( hDC, &ClientRect, DrawTools.GetBtnGrayBrush(),
					4, YES );
		else
		{
			hBrush = CreateSolidBrush( 
			GetSysColor( bHilighted ? COLOR_HIGHLIGHT : COLOR_BTNFACE ) );
			HilightFrame( hDC, &ClientRect, hBrush, 4, NO );
			DeleteObject( hBrush );
		}
		DrawSculptedBox(hDC, &ClientRect, bFocus, bHilighted, YES);
		return;
	}
	
	// Draw the box interior
	if (bHilighted)
		HilightRect( hDC, &ClientRect, DrawTools.GetBtnGrayBrush() );
	else
	{
		hBrush = CreateSolidBrush( 
			GetSysColor( bHilighted ? COLOR_HIGHLIGHT : COLOR_BTNFACE ) );
		FillRect( hDC, &ClientRect, hBrush );
		DeleteObject( hBrush );
	}
	DrawSculptedBox(hDC, &ClientRect, bFocus, bHilighted, YES);
	InflateRect(&ClientRect, -2, -2);
	
	// Draw the image
	GetTextMetrics(hDC, &metrics);
	ImageRect = ClientRect;
	--ImageRect.right; --ImageRect.bottom;
	yAmount = metrics.tmHeight+2;
	xAmount = 0;
//	xAmount = (2*yAmount)/3;
	ImageRect.top += 1;
	ImageRect.bottom -= yAmount;
//	ImageRect.top += ((2*xAmount) - yAmount);
	ImageRect.left += xAmount;
	ImageRect.right -= xAmount; 
	if (bHilighted)
		OffsetRect(&ImageRect, 2, 2);
	
	// Get the file name
	bCurs = AstralCursorEnable(NO);
	ListBox_GetText( hControl, lpDraw->itemID, (long)((LPSTR)szFileName) );
	bGotFile  = LBEntryToFileLocation(szFileName, &FileLocation);
	if (bGotFile && !EXTENDED)
		lstrcpy(szFileName, filename(FileLocation.szFile));
		
	GetClipBox(hDC, &ClipRect);
	AutoThumbnails = AUTOTHUMBS;
	if (AstralIntersectRect(&ClipRect, &ClipRect, &ImageRect))
	{
//		if (ClipRect.left > ImageRect.left || ClipRect.top > ImageRect.top ||
//			ClipRect.right < ImageRect.right || ClipRect.bottom < ImageRect.bottom)
//			AutoThumbnails = FALSE; 	

		InflateRect(&ImageRect, -1, -1);
		width = RectWidth(&ImageRect);
		height = RectHeight(&ImageRect);
	
		lpFrame = NULL;
		lpThm = NULL;
		if (bGotFile)
		{
			bMessageEnable = MessageEnable(FALSE);
			if (lpThm = FindThumbnailAndFrame(&FileLocation, AutoThumbnails, &bError))
				if (!lpThm->thm.frame && !SAFECANCEL )
				{
					CTHMClose(lpThm);
					lpThm =	GetCDThumb(&FileLocation);
					bCDThm = TRUE;
				}	
			if (!lpThm && !SAFECANCEL && !bCDThm)
				lpThm =	GetCDThumb(&FileLocation);
			MessageEnable(bMessageEnable);
		}
		if (lpThm)
			lpFrame = lpThm->thm.frame;
		if (lpFrame)
		{
			if (width > lpFrame->Xsize)
			{
				xAmount = (width-lpFrame->Xsize)/2;
				ImageRect.left += xAmount;
				ImageRect.right -= ((width-lpFrame->Xsize)-xAmount);
			}
			if (height > lpFrame->Ysize)
			{
				yAmount = (height-lpFrame->Ysize)/2;
				ImageRect.top += yAmount;
				ImageRect.bottom -= ((height-lpFrame->Ysize)-yAmount);
			}
			if ((RectWidth(&ImageRect) != lpFrame->Xsize) ||
				(RectHeight(&ImageRect) != lpFrame->Ysize))
					dbg("Thumb Rect: %ld %ld", (long)RectWidth(&ImageRect), (long)RectHeight(&ImageRect));
			DrawImageControl( hDC, NULL, lpFrame, &ImageRect, 0, 0, NO);
		}
		else if (bError)
		{
			if (AstralStr(IDS_CANNOTREAD, &lpString))
			{
				iOldMode = SetBkMode( hDC, TRANSPARENT );
				lOldColor = SetTextColor(hDC, RGB(255,0,0));
				yAmount = (height-(2*metrics.tmHeight)) / 2;
				ImageRect.top += yAmount;
				DrawText( hDC, lpString, -1, &ImageRect,
					DT_NOPREFIX | DT_WORDBREAK | DT_CENTER);
				SetBkMode( hDC, iOldMode );
				SetTextColor( hDC, lOldColor );
			}
		}
		else // no thumbnail
		{
			lpString = NULL;	
			AstralStr(IDS_NOTHUMB, &lpString);
			if (lpString)
			{
				iOldMode = SetBkMode( hDC, TRANSPARENT );
				lOldColor = SetTextColor(hDC, RGB(255,0,0));
				yAmount = (height-(2*metrics.tmHeight)) / 2;
				ImageRect.top += yAmount;
				DrawText( hDC, lpString, -1, &ImageRect,
					DT_NOPREFIX | DT_WORDBREAK | DT_CENTER);
				SetBkMode( hDC, iOldMode );
				SetTextColor( hDC, lOldColor );
			}
		}
		if (lpThm)
			CTHMClose(lpThm);
		else if (lpFrame)
			FrameClose(lpFrame);
	} // end if AstralIntersectRect
	
	AstralCursorEnable(bCurs);
	
	// Draw the text
	iOldMode = SetBkMode( hDC, TRANSPARENT );
	lOldColor = SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
	
#ifndef WIN32
	if (!EXTENDED)
		AnsiLower(szFileName);
#endif
	ClientRect.top = ClientRect.bottom - (metrics.tmHeight+1);
	hOldFont = (HFONT)SelectObject(hDC, GetStockObject( Window.iLabelFont ) );
	// size the text
	ImageRect = ClientRect;
	DrawText( hDC, szFileName, -1, &ImageRect,	DT_NOPREFIX | DT_SINGLELINE | DT_CALCRECT);
	
	// determine if it should be centered
	fTextFlags = DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER;
	IntersectRect(&rect, &ClientRect, &ImageRect);
	if (!EqualRect(&rect, &ImageRect))
		fTextFlags |= DT_LEFT;
	else
		fTextFlags |= DT_CENTER;
		
	// draw text
	DrawText( hDC, szFileName, -1, &ClientRect, fTextFlags);
	if ( hOldFont )
		SelectObject(hDC, hOldFont);
	SetBkMode( hDC, iOldMode );
	SetTextColor( hDC, lOldColor );
	
	// Use PeekMessage seems to be the best way to allow the user
	// to intervene during display of thumbnails, however it seems
	// to cause some strange behavior that would require more
	// investigation.  I believe it has something to do with closing
	// the dialog box during this response to WM_DRAWITEM
	// this can also cause some of the items to be drawn incorrectly
	// if a selection change occurs between painting of items
	// MSG	msg;
	//while ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
	//	{
	//	if ( !IsDialogMessage((HWND)hDlg, &msg) )
	//		{
	//		TranslateMessage(&msg);
	//		DispatchMessage(&msg);
	//		}
	//	}
}

//***********************************************************************/
//********************************STATIC FUNCTIONS*****************************/
//***********************************************************************/

//***********************************************************************/
// 	Simple routine to find the thumbnial from a file name.
//	Loads the preview frame if any.
//	Returns the thumbnail or null.
//	if there was an error then bError is set to TRUE.
//	Caller is responsilbe for freeing the thumbnail.
/***********************************************************************/
// 	Extended Browser aware.
//***********************************************************************/
LOCAL LPCTHMNAIL FindThumbnailAndFrame(LPFILELOC lpFile, BOOL bAutoThumbs, BOOL *bError)
//***********************************************************************/
{
	LPCTHMNAIL lpThm;
  	ENTRY entry;
  	BOOL bCurs;
	int ret;
	UINT32 time;

  	*bError = FALSE;
	lpThm = NULL;
	if (!lpBsr->OpenState.bInitialized)
		return(FALSE);
	// Try cache first
	// Try the current album or directory dbf
	if (!TryDefaultDBF(lpFile, &entry))
	{
		if(!bAutoThumbs || SAFECANCEL )
			return(NULL);
		// create it
		bCurs = AstralCursorEnable(YES);
		ret = FindOrCreateThumb(lpFile, &entry, YES);
		AstralCursorEnable(bCurs);
		if (ret == BSR_ERROR)
			goto Error;
		// there may now be a dbf for the directory
		if (DIRMODE && lpBsr->szLastDirDBF[0] == -1)
			CLEAR_DIR_DBF_CACHE;
	}
	lpThm = CTHMOpen(&entry);
	// get the thumbnail
	if (!lpThm)
	{
		if (!bAutoThumbs || SAFECANCEL)
			goto Error;
		bCurs = AstralCursorEnable(YES);
		ret = FindOrCreateThumb(lpFile, &entry, YES);
		AstralCursorEnable(bCurs);
		if (ret == BSR_ERROR)
			goto Error;
		if (!(lpThm = CTHMOpen(&entry)))
			goto Error;
	}
	// Get the frame
	if (!CTHMGetFrame(lpThm))
	{
		if(!bAutoThumbs || SAFECANCEL) 
			return(lpThm);
		// create the frame
		bCurs = AstralCursorEnable(YES);
		ret = CTHMReCreateFrame(lpThm, &entry.File, YES);
		AstralCursorEnable(bCurs);
		if (!ret)
			goto Error;
	}
	// be sure its up to date
	if ( bAutoThumbs && lpThm)
	{
		if (VolumeCheck(&entry.File, NULL))
			if (SafeFileStats(entry.File.szFile, NULL, NULL, &time, NULL))
				if (time != lpThm->thm.info.thmtime)
					CTHMReCreateFrame(lpThm, &entry.File, YES);
	}
	
	SAFECANCEL;
	return(lpThm);
	
Error:
	if (lpThm)
		CTHMClose(lpThm);
	*bError = !SAFECANCEL;
	return(NULL);
}

//***********************************************************************/
// 	Tries to find a valid entry from a file name.
//	Uses a stored copy of the current dbf.
//	Returns TRUE if an entry was found.
//***********************************************************************/
LOCAL BOOL TryDefaultDBF(LPFILELOC lpFile, LPENTRY lpOutEntry)
//***********************************************************************/
{
	FILELOC Path;
	LPDBFLL lpDBFLL;
	DBFS dbf;
	BOOL bFound;

	if (!lpBsr->OpenState.bInitialized)
		return(FALSE);	
	// if album mode just find album
	if (ALBMODE)
	{
		lpDBFLL = FindAlbum(lpBsr->OpenState.szAlbum);
		if (!lpDBFLL)
			return(FALSE);
		DBFReOpen(&lpDBFLL->dbf, NO);
		bFound = DBFGetEntry(&lpDBFLL->dbf, lpFile, lpOutEntry) != NULL;
	   	DBFClose(&lpDBFLL->dbf);
		if (!bFound || lpOutEntry->Thumb.szFile[0] == _T('\0'))
			return(FALSE);
		return(TRUE);
	}
	// Must be DIRECTORY MODE or extended mode
	// Setup default dbf if not already
	if (EXTENDED)
	{
		// extended mode -- 
		// possibly different dir every time... but usually the same
		Path = *lpFile;
		stripfile(Path.szFile);
	}
	else
	{		// directory mode
		if (!CurrentDirEx(CURRENTDRIVE, Path.szFile, sizeof(Path.szFile)))
			return(FALSE);
		DriveVolume(CURRENTDRIVE, &Path.volume, YES);
	}
	FixPath(Path.szFile);
	bFound = (lpBsr->szLastDirDBF[0]); 	// is this the first call?
	bFound = (bFound && EQFILELOC(Path, lpBsr->LastPath)); // is the path the same?
	if (bFound && (lpBsr->szLastDirDBF[0] == -1))
		return(FALSE);		// There is none
	if (bFound)		// there is a dbf file 
		if (!DBFOpenByFile(lpBsr->szLastDirDBF, &dbf, NO))
			bFound = FALSE;
	if (!bFound)	
	{
		// dbf needs to be found
		lpBsr->szLastDirDBF[0] = -1;
		lpBsr->LastPath = Path;
		if (!DBFOpenDirectory(&Path, &dbf, NO))
			return(FALSE);	// there is no dbf for the directory
		lstrcpy(lpBsr->szLastDirDBF, dbf.szFile);
	}
	// now we have a dbf
	DBFReOpen(&dbf, NO);
	bFound = (DBFGetEntry(&dbf, lpFile, lpOutEntry) != NULL);
	DBFClose(&dbf);
	if (!bFound || lpOutEntry->Thumb.szFile[0] == _T('\0'))
		return(FALSE);
	return(TRUE);
}

