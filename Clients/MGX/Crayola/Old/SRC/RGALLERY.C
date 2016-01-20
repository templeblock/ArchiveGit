/*****************************************************************************

       Copyright (c) Micrografx, Inc., 1993 - 1994.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
           from Micrografx, 1303 Arapaho Rd, Richardson, Texas 75081.

******************************************************************************
******************************************************************************

                                  rgallery.c

******************************************************************************
******************************************************************************

  Description:  Gallery Room dialog functions.
 Date Created:  01/11/94
       Author:  Stephen Suggs

*****************************************************************************/

#include <windows.h>
#include <math.h>
#include <ctype.h>		// isdigit()
#include "id.h"
#include "data.h"
#include "routines.h"

extern HINSTANCE hInstAstral;
extern HWND   hWndAstral;

#define MAX_TEXTCONTROLS	20

static	int		ImageCnt;
static	int		ImageIdx;
static	int		ImageOffset;
static	int		LastImage;
static	BOOL	bAnimate;
static	BOOL	bSlideActive;
static	BOOL	bDeleted;
static	FNAME	szFileName;
static	STRING	szOldTitle;
static	STRING	szNewTitle;
static	HFONT	hDlgFont;
static	PAINTSTRUCT	sPaint;

static	int		HotSpotIDs[MAX_TEXTCONTROLS] =
{
	HS_GALLERY_TEXT1,
	HS_GALLERY_TEXT2,
	HS_GALLERY_TEXT3,
	HS_GALLERY_TEXT4,
	HS_GALLERY_TEXT5,
	HS_GALLERY_TEXT6,
	HS_GALLERY_TEXT7,
	HS_GALLERY_TEXT8,
	HS_GALLERY_TEXT9,
	HS_GALLERY_TEXT10,
	HS_GALLERY_TEXT11,
	HS_GALLERY_TEXT12,
	HS_GALLERY_TEXT13,
	HS_GALLERY_TEXT14,
	HS_GALLERY_TEXT15,
	HS_GALLERY_TEXT16,
	HS_GALLERY_TEXT17,
	HS_GALLERY_TEXT18,
	HS_GALLERY_TEXT19,
	HS_GALLERY_TEXT20
};

/********************************* Constants ********************************/

#define TITLE_LENGTH	50
#define THUMB_HEIGHT	90
#define THUMB_WIDTH		120

/****************************** Public Functions ****************************/

BOOL WINPROC EXPORT DlgRoomGallery (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
	STRING	szNewName;
	STRING	szNumber;
	STRING	szPicIndex;
	HWND	hImageControl;
	BOOL	bStatus;
	int		type, idx, count;
	char  * src;
	char  * dst;
	char  * dst2;
	LPFRAME	lpFrame;
	RECT	rect;
	HDC		hDC;
	BOOL	bHandled = TRUE;
    LPOBJECT lpObject;

	switch (wMsg)
	{
	case WM_INITDIALOG:
		DefRoomProc (hWnd, wMsg, wParam, lParam);

		clr ((LPTR)szFileName, sizeof(FNAME));
		ImageCnt = GetNumberIniEntries();
		clr ((LPTR)szOldTitle, sizeof(STRING));
		clr ((LPTR)szNewTitle, sizeof(STRING));

		hDlgFont = (HFONT)NULL;
		hDlgFont = CreateFont (-12,				// Height
								0,				// Width
								0,				// Escapement
								0,				// Orientation
								0,				// Weight
								0,				// Italic
								0,				// Underline
								0,				// Strikeout
								0,				// Charset
								0,				// OutputPrecision
								0,				// ClipPrecision
								0,				// Quality
								0,				// PitchAndFamily
								(LPCSTR)"Arial Bold");	// Typeface Name

		if (hDlgFont != NULL)
			SendDlgItemMessage (hWnd, IDC_GALLERY_EDIT, WM_SETFONT, (WPARAM)hDlgFont, 0L);


		SendDlgItemMessage (hWnd, IDC_GALLERY_EDIT, EM_LIMITTEXT,
		  TITLE_LENGTH, 0L);

		// Start at last picture saved
		ImageIdx = ImageCnt - 1;
		if (ImageCnt > MAX_TEXTCONTROLS)
		{
			ImageOffset = ImageCnt - MAX_TEXTCONTROLS;
			LastImage = ImageOffset + MAX_TEXTCONTROLS -1;
		}
		else
		{
			ImageOffset = 0;
		}
		if ((ImageIdx >= 0) &&
		  (GetFileNameByIndex ((LPSTR)&szFileName, ImageIdx)) )
		{
			if (AstralImageLoad (IDN_ART, szFileName, MAYBE, NO))
			{
				hImageControl = GetDlgItem(hWnd, IDC_GALLERY_IMAGE);
				GetFilenameTitle (szFileName, szOldTitle);
				SetDlgItemText (hWnd, IDC_GALLERY_EDIT, szOldTitle);
				lpImage->fUntitled = TRUE;
				if (hImageControl && (lpImage != NULL))
				{
					SetWindowLong (hImageControl, GWL_IMAGE,
					  (long)ImgGetBase(lpImage));
				}
				AstralControlPaint (hWnd, IDC_GALLERY_IMAGE);
			}
		}
		// called from Art room
		if (idPreviousRoom != RM_SLIDE)
		{
			bSlideActive = FALSE;
			ShowWindow (GetDlgItem(hWnd, HS_GALLERY_SLIDES), SW_HIDE);
			ShowWindow (GetDlgItem(hWnd, HS_GALLERY_GOPAINT), SW_SHOW);
			ShowWindow (GetDlgItem(hWnd, HS_GALLERY_TRASH), SW_SHOW);
			CheckForAnimations (hWnd);
		}
		// if called from Slideshow
		else
		{
			bSlideActive = TRUE;
			ShowWindow (GetDlgItem(hWnd, HS_GALLERY_GOPAINT), SW_HIDE);
			ShowWindow (GetDlgItem(hWnd, IDC_GALLERY_SW1), SW_HIDE);
			ShowWindow (GetDlgItem(hWnd, HS_GALLERY_TRASH), SW_HIDE);
			EnableWindow (GetDlgItem(hWnd, IDC_GALLERY_EDIT), FALSE);
		}
		// if none or only 1 image, hide the Before/Next buttons
		if (ImageCnt <= 1)
		{
			ShowWindow (GetDlgItem(hWnd, HS_GALLERY_BEFORE), SW_HIDE);
			ShowWindow (GetDlgItem(hWnd, HS_GALLERY_NEXT), SW_HIDE);
		}
		else
		{
			ShowWindow (GetDlgItem(hWnd, HS_GALLERY_BEFORE), SW_SHOW);
			ShowWindow (GetDlgItem(hWnd, HS_GALLERY_NEXT), SW_SHOW);
		}
		// disable un-needed hot spots
		if (ImageCnt < MAX_TEXTCONTROLS)
		{
			for (idx = ImageIdx +1 ; idx < MAX_TEXTCONTROLS ; idx++)
			{
				ShowWindow (GetDlgItem(hWnd, HotSpotIDs[idx]), SW_HIDE);
			}
		}

		bHandled = TRUE;
		bAnimate = FALSE;
		bDeleted = FALSE;
		break;

	case WM_CLOSE:
	AstralDlgEnd( hWnd,TRUE|2 );
	bHandled = TRUE;
	break;

	case WM_PAINT:
		PaintRoomBitmap (hWnd);
		// Start at last picture saved
		hDC = GetDC (hWnd);
		if (hDC != NULL)
		{
			SetBkColor (hDC, RGB(0,0,0));
			SetTextColor (hDC, RGB(255,255,255));
			if (ImageCnt > MAX_TEXTCONTROLS)
				count = MAX_TEXTCONTROLS + ImageOffset -1;
			else
				count = ImageCnt -1;
			for (idx = ImageOffset ; idx <= count; idx++)
			{
				if (idx == ImageIdx)
				{
					SetTextColor (hDC, RGB(255,0,0));
				}
				else
				{
					SetTextColor (hDC, RGB(255,255,255));
				}
				clr ((LPTR)szNumber, sizeof(STRING));
				wsprintf (szNumber, "%2d", idx +1);
				TextOut (hDC, 0, (((idx-ImageOffset)*20)+50),
				  (LPSTR)szNumber, 2);
			}
			// this SelectObject is needed for the edit control caret size
			SelectObject (hDC, hDlgFont);
			ReleaseDC (hWnd, hDC);
		}
		bHandled = TRUE;
		break;

	case WM_COMMAND:
		switch (wParam)
		{
		//-------------------------------------------------
		// Animate button
		//-------------------------------------------------
		case IDC_GALLERY_SW1:
		if ((ImageCnt) && (szFileName[0] != 0))
		{
			bAnimate = !bAnimate;
			CheckDlgButton (hWnd, IDC_GALLERY_SW1, bAnimate);
			if (bAnimate)
			{
//				lpObject = ImgGetBase (lpImage);
//				while (lpObject)
//				{
//					if (lpObject->ObjectDataID == OBJECT_DATA_STOCK_ANIMATION)
//					{
//   						lpObject->fHidden = TRUE;
//					}
//					lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO);
//				}
//				AstralControlPaint (hWnd, IDC_GALLERY_IMAGE); 
				StartAnimation (GetDlgItem(hWnd, IDC_GALLERY_IMAGE));
			}
			else
			{
				StopAnimation();
//				lpObject = ImgGetBase (lpImage);
//				while (lpObject)
//				{
//					if (lpObject->ObjectDataID == OBJECT_DATA_STOCK_ANIMATION)
//					{
//   						lpObject->fHidden = FALSE;
//					}
//					lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO);
//				}
//				AstralControlPaint (hWnd, IDC_GALLERY_IMAGE);
				SoundStopAndFree(); 
			}
		}
		bHandled = TRUE;
		break;

		//-------------------------------------------------
		// Paint button
		//-------------------------------------------------
		case HS_GALLERY_GOPAINT:
		DeleteObject (hDlgFont);
		if (bAnimate)
			StopAnimation();

		if ((ImageCnt) && (szFileName[0] != 0))
		{
			// make sure title is saved to INI file
			GetDlgItemText (hWnd, IDC_GALLERY_EDIT, szNewTitle,
			  sizeof(szNewTitle));
			GetFilenameTitle (szFileName, szOldTitle);
			if (lstrcmp (szOldTitle, szNewTitle))
			{
				SetFilenameTitle (szFileName, szNewTitle);
			}

			if (lpImage != NULL)
				lpImage->fUntitled = NO;
			GoPaintApp (hWnd, szFileName);
		}

		bHandled = TRUE;
		break;

		//-------------------------------------------------
		// Before button (previous)
		//-------------------------------------------------
//		case HS_GALLERY_BEFORE:
		case HS_GALLERY_NEXT:
		ImageIdx++;
		if (ImageCnt > MAX_TEXTCONTROLS)
		{
			if (ImageIdx > LastImage)
			{
				ImageOffset++;
				if (ImageOffset > ImageCnt - MAX_TEXTCONTROLS)
				{
					ImageOffset = 0;
				}
				LastImage = ImageOffset + MAX_TEXTCONTROLS -1;
			}
		}
		if (ImageIdx >= ImageCnt)
		{
			ImageIdx = 0;
		}
		if (ImageCnt > 1)
		{
			if (bAnimate)
			{
				StopAnimation();
				bAnimate = !bAnimate;
				CheckDlgButton (hWnd, IDC_GALLERY_SW1, bAnimate);
			}

			UpdateGalleryImage (hWnd);
			UpdateTextControls (hWnd);
		}
		bHandled = TRUE;
		break;

		//-------------------------------------------------
		// Next button
		//-------------------------------------------------
//		case HS_GALLERY_NEXT:
		case HS_GALLERY_BEFORE:
		ImageIdx--;
		if (ImageCnt > MAX_TEXTCONTROLS)
		{
			if (ImageIdx < ImageOffset)
			{
				ImageOffset--;
				if (ImageOffset < 0)
				{
					ImageOffset = ImageCnt - MAX_TEXTCONTROLS;
				}
				LastImage = ImageOffset + MAX_TEXTCONTROLS -1;
			}
		}
		if (ImageIdx < 0)
		{
			ImageIdx = ImageCnt -1;
		}
		if (ImageCnt > 1)
		{
			if (bAnimate)
			{
				StopAnimation();
				bAnimate = !bAnimate;
				CheckDlgButton (hWnd, IDC_GALLERY_SW1, bAnimate);
			}

			UpdateGalleryImage (hWnd);
			UpdateTextControls (hWnd);
		}
		bHandled = TRUE;
		break;

		//-------------------------------------------------
		// Slideshow button
		//-------------------------------------------------
		case HS_GALLERY_SLIDES:
		bHandled = TRUE;
		if (bSlideActive && ImageCnt && szFileName[0] != 0)
		{
			clr ((LPTR)szPicIndex, sizeof(STRING));
			clr ((LPTR)szNumber, sizeof(STRING));
			GetSlideNumber (szNumber);

			clr ((LPTR)szNewName, sizeof(STRING));
			dst = szNewName;
			dst2 = szPicIndex;
			src = szFileName;
			while ( *(src) != '.')
			{
				*dst = *src;
				dst++;
				src++;
			}
			dst--;
			while (isdigit (*dst))
			{
				*dst = 0;
				dst--;
			}

			src--;
			while (isdigit (*src))
				src--;
			src++;
			while (isdigit (*src))
			{
				*dst2 = *src;
				dst2++;
				src++;
			}
			lstrcat (szNewName, szNumber);
			lstrcat (szNewName, ".TIF");

			if ( !(bStatus = AstralImageSave (IDN_TIFF, IDC_SAVE8BITCOLOR,
			  szNewName )))
				break;

			// size the thumbnail down
			rect.left = rect.top = 0;
			rect.right = THUMB_WIDTH;
			rect.bottom = THUMB_HEIGHT;
			lpFrame = ReadTiffData (-1, szNewName, 3, &type,
			  FALSE, &rect, FALSE, NULL, NULL, NULL);
			if (lpFrame)
				AstralFrameSave (szNewName, lpFrame, NULL, IDC_SAVE8BITCOLOR,
				  IDN_TIFF, FALSE);

			SaveToSlideshow (szNewName, szPicIndex);
			DeleteObject (hDlgFont);
            GoRoom (hInstAstral, RM_SLIDE, FALSE);
		}
   		break;

		//-------------------------------------------------
		// Trash button (delete picture)
		//-------------------------------------------------
		case HS_GALLERY_TRASH:
		bHandled = TRUE;

		if (bAnimate)
		{
			StopAnimation();
			bAnimate = !bAnimate;
			CheckDlgButton (hWnd, IDC_GALLERY_SW1, bAnimate);
		}

		if (ImageCnt && szFileName[0] != 0)
		{
			if (AstralAffirm (IDS_CONFIRMDELETE) )
			{
				DeleteFileAndIniEntry (szFileName);
				CloseImage (NO, NULL);
				clr ((LPTR)szFileName, sizeof(FNAME));
				bDeleted = TRUE;
				ImageCnt--;
				ImageOffset = 0;
				if (! ImageCnt)
				{
					// if last picture deleted - go back to art room
					GoRoom (hInstAstral, RM_ARTROOM, FALSE);
					break;
				}
				if (ImageCnt == 1)
				{
					ImageIdx = 0;
					hImageControl = GetDlgItem(hWnd, IDC_GALLERY_IMAGE);
					if (GetFileNameByIndex ((LPSTR)&szFileName, ImageIdx))
					{
						if (AstralImageLoad (IDN_ART, szFileName, MAYBE, NO))
						{
							GetFilenameTitle (szFileName, szOldTitle);
							SetDlgItemText (hWnd, IDC_GALLERY_EDIT, szOldTitle);
							lpImage->fUntitled = TRUE;
							if (hImageControl && (lpImage != NULL))
							{
  								SetWindowLong (hImageControl, GWL_IMAGE,
				  	  			(long)ImgGetBase(lpImage));
							}
							AstralControlPaint (hWnd, IDC_GALLERY_IMAGE);
						}
					}
					break;
				}
				ImageIdx = 0;
				UpdateGalleryImage (hWnd);
				UpdateTextControls (hWnd);
			}
			else
				break;
		}
   		break;

		//-------------------------------------------------
		// Back to the art room
		//-------------------------------------------------
		case HS_BACKSHOW:
			DeleteObject (hDlgFont);
			if (bAnimate)
				StopAnimation();

			// before going, see if the title was changed
			if (szFileName[0] != 0)
			{
				clr ((LPTR)szNewTitle, sizeof(STRING));
				clr ((LPTR)szOldTitle, sizeof(STRING));
				GetDlgItemText (hWnd, IDC_GALLERY_EDIT, szNewTitle,
				  sizeof(szNewTitle));
				GetFilenameTitle (szFileName, szOldTitle);
				if (lstrcmp (szOldTitle, szNewTitle))
				{
					SetFilenameTitle (szFileName, szNewTitle);
				}
			}
			GoRoom (hInstAstral, bSlideActive ? RM_SLIDE : RM_ARTROOM, FALSE);
			bHandled = TRUE;
			break;

		case HS_GALLERY_TEXT1:
		case HS_GALLERY_TEXT2:
		case HS_GALLERY_TEXT3:
		case HS_GALLERY_TEXT4:
		case HS_GALLERY_TEXT5:
		case HS_GALLERY_TEXT6:
		case HS_GALLERY_TEXT7:
		case HS_GALLERY_TEXT8:
		case HS_GALLERY_TEXT9:
		case HS_GALLERY_TEXT10:
		case HS_GALLERY_TEXT11:
		case HS_GALLERY_TEXT12:
		case HS_GALLERY_TEXT13:
		case HS_GALLERY_TEXT14:
		case HS_GALLERY_TEXT15:
		case HS_GALLERY_TEXT16:
		case HS_GALLERY_TEXT17:
		case HS_GALLERY_TEXT18:
		case HS_GALLERY_TEXT19:
		case HS_GALLERY_TEXT20:
			if (wParam - HS_GALLERY_TEXT1 < ImageCnt)
			{
				ImageIdx = wParam - HS_GALLERY_TEXT1 + ImageOffset;
				if (ImageCnt > 1)
				{
					UpdateGalleryImage (hWnd);
					UpdateTextControls (hWnd);
				}
			}
			bHandled = TRUE;
			break;

		default:
			bHandled = FALSE;
			break;
		}
		break;

	default:
		bHandled = FALSE;
		break;
	}

	if (! bHandled)
		bHandled = DefRoomProc (hWnd, wMsg, wParam, lParam);
	return bHandled;
}

/****************************************************************************/

static BOOL UpdateGalleryImage (HWND hWnd)
{
	HWND	hImageControl;

	// before loading new image, see if old title was changed
	if (szFileName[0] != 0)
	{
		GetDlgItemText (hWnd, IDC_GALLERY_EDIT, szNewTitle, sizeof(szNewTitle));
		GetFilenameTitle (szFileName, szOldTitle);
		if (lstrcmp (szOldTitle, szNewTitle))
		{
			SetFilenameTitle (szFileName, szNewTitle);
		}
	}
	hImageControl = GetDlgItem(hWnd, IDC_GALLERY_IMAGE);
	if (GetFileNameByIndex ((LPSTR)&szFileName, ImageIdx))
	{
		if (AstralImageLoad (IDN_ART, szFileName, MAYBE, NO))
		{
			GetFilenameTitle (szFileName, szOldTitle);
			SetDlgItemText (hWnd, IDC_GALLERY_EDIT, szOldTitle);
			lpImage->fUntitled = TRUE;
			if (hImageControl && (lpImage != NULL))
			{
				SetWindowLong (hImageControl, GWL_IMAGE,
  			  	  (long)ImgGetBase(lpImage));
			}
			if (! bSlideActive)
				CheckForAnimations (hWnd);
			AstralControlPaint (hWnd, IDC_GALLERY_IMAGE);
		}
	}
	return TRUE;
}

/****************************************************************************/
// Update the index numbers
static BOOL UpdateTextControls (HWND hWnd)
{
	HDC		hDC;
	STRING	szNumber;
	int		idx, count;
	RECT	Rect;
	HBRUSH	hBrush, hOldBrush;

	hDC = GetDC (hWnd);
	if (hDC != NULL)
	{
		hBrush = CreateSolidBrush (RGB(0,0,0));
		hOldBrush = (HBRUSH)SelectObject (hDC, hBrush);
		SetBkColor (hDC, RGB(0,0,0));

		Rect.left = 0;
		Rect.right = 15;

		if (ImageCnt > MAX_TEXTCONTROLS)
			count = MAX_TEXTCONTROLS + ImageOffset -1;
		else
			count = ImageCnt -1;
		for (idx = ImageOffset ; idx <= count ; idx++)
		{
			Rect.top = (((idx-ImageOffset)*20)+50);
			Rect.bottom = Rect.top + 15;
			Rectangle (hDC, Rect.left, Rect.top, Rect.right, Rect.bottom);

			if (idx == ImageIdx)
			{
				// set to red for current image
				SetTextColor (hDC, RGB(255,0,0));
			}
			else
			{
				// set to white for all others
				SetTextColor (hDC, RGB(255,255,255));
			}
			clr ((LPTR)szNumber, sizeof(STRING));
			wsprintf (szNumber, "%2d", idx +1);
			TextOut (hDC, 0, Rect.top, (LPSTR)szNumber, 2);
		}
		if ((ImageCnt < MAX_TEXTCONTROLS) && bDeleted)
		{
			// hide the hotspot
			ShowWindow (GetDlgItem(hWnd, HotSpotIDs[idx-ImageOffset]), SW_HIDE);
			// clear the old number
			Rect.top = (((idx-ImageOffset)*20)+50);
			Rect.bottom = Rect.top + 15;
			Rectangle (hDC, Rect.left, Rect.top, Rect.right, Rect.bottom);
		}
		SelectObject (hDC, hOldBrush);
		DeleteObject (hBrush);
		ReleaseDC (hWnd, hDC);
	}
	bDeleted = FALSE;
	return TRUE;
}


/****************************************************************************/
// See if this image has any animations and hide or show the Animation button
static void CheckForAnimations (HWND hWnd)
{
    LPOBJECT lpObject;
	if (lpImage)
	{
		lpObject = ImgGetBase (lpImage);
		while (lpObject)
		{
			if (lpObject->ObjectDataID == OBJECT_DATA_STOCK_ANIMATION)
			{
				ShowWindow (GetDlgItem (hWnd, IDC_GALLERY_SW1), SW_SHOW);
   				return;
			}
			lpObject = ImgGetNextObject (lpImage, lpObject, YES, NO);
		}
	}
	ShowWindow (GetDlgItem(hWnd, IDC_GALLERY_SW1), SW_HIDE);
}

