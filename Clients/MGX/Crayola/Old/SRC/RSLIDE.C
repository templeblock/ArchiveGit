/*****************************************************************************

       Copyright (c) Micrografx, Inc., 1993 - 1994.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
           from Micrografx, 1303 Arapaho Rd, Richardson, Texas 75081.

******************************************************************************
******************************************************************************

                                    rslide.c

******************************************************************************
******************************************************************************

  Description:  Slideshow Room, SlidePlay Room and Effects dialog procedures
 Date Created:  02/07/94
       Author:  Stephen Suggs

*****************************************************************************/

#include <windows.h>
#include <math.h>
#include <ctype.h>
#include "id.h"
#include "data.h"
#include "routines.h"

#define	MAX_SLIDES	24
#define	NUM_IMAGES	8
#define OFFSET_INC	8
#define TIMER_ID	3

extern HINSTANCE hInstAstral;
extern HWND   hWndAstral;

// DlgRoomSlidePlay stuff
static	UINT	uTimer;
static	BOOL	bAnimationOn;
static	BOOL	bAnimationStarted;
static	BOOL	bStopped;
static	int		SlideCount;
static	int		SlideIndex;
static	int		iSlideSet;
static	UINT	uDelay;

// DlgRoomSlideshow stuff
int		SlideImageOffset;	// global so we can come back into the same
							// slideshow screen from the gallery
static	int		iCount;
static	int		ImageIdx;
static	int		ImageIDs[NUM_IMAGES] =
{
	IDC_SLIDE_IMAGE1,
	IDC_SLIDE_IMAGE2,
	IDC_SLIDE_IMAGE3,
	IDC_SLIDE_IMAGE4,
	IDC_SLIDE_IMAGE5,
	IDC_SLIDE_IMAGE6,
	IDC_SLIDE_IMAGE7,
	IDC_SLIDE_IMAGE8
};

static char szSlideNames[MAX_SLIDES][MAX_STR_LEN];

/****************************************************************************/
BOOL WINPROC EXPORT DlgRoomSlideshow (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
	HWND	hImageControl;
	STRING	szFileName;
	STRING	szNumber;
	STRING	szBuff;
	STRING	szScratch;
	int		idx, iFrame, imageidx;
	LPFRAME lpFrame;
	WORD    DataType;
	LPSTR	src;
	LPSTR	dst;
	BOOL	bHandled = TRUE;

	switch (wMsg)
	{
	case WM_INITDIALOG:
		DefRoomProc (hWnd, wMsg, wParam, lParam);
		bHandled = TRUE;
		ImageIdx = 0;

		if (idPreviousRoom != RM_GALLERY)
			SlideImageOffset = 0;

		for (idx = 0 ; idx < MAX_SLIDES ; idx++)
			clr ((LPTR)szSlideNames[idx], MAX_STR_LEN);

		ShowWindow (GetDlgItem(hWnd, HS_SLIDE_PREVPAGE), SW_HIDE);

		if (! (iCount = GetNumberSlides ()))
		{
			ShowWindow (GetDlgItem(hWnd, HS_SLIDE_PLAYMOVIE), SW_HIDE);
			break;
		}

		if (iCount > NUM_IMAGES)
			iCount = NUM_IMAGES;

		clr ((LPTR)szScratch, sizeof(STRING));
		clr ((LPTR)szNumber, sizeof(STRING));
		// load up all of the INI strings into the correct slot
		for (idx = 0 ; idx < MAX_SLIDES ; idx++)
		{
			if (GetSlideFileIniEntry ((LPSTR)&szBuff, idx))
			{
				lstrcpy (szScratch, szBuff);
				src = lstrchr (szScratch, '.');
				dst = szNumber;
				*src = 0;
				src--;
				while (isdigit (*src))
				{
					src--;
				}
				src++;
				while (isdigit (*src))
				{
					*dst = *src;
					dst++;
					src++;
				}
				*dst = 0;
				iFrame = atoi (szNumber);
				if (iFrame < MAX_SLIDES)
				{
					lstrcpy (szSlideNames[iFrame], szBuff);
				}
			}
		}

		imageidx = 0;
		for (idx = SlideImageOffset ; idx < NUM_IMAGES + SlideImageOffset ; idx++)
		{
			if (szSlideNames[idx][0] != 0)
			{
				hImageControl = GetDlgItem (hWnd, ImageIDs[imageidx]);
				GetThumbFilename (szFileName, idx);
            	lpFrame = tifrdr (szFileName, -1, (LPINT)&DataType, YES);
				if (lpFrame && hImageControl)
				{
					SetWindowLong (hImageControl, GWL_IMAGE, (long)lpFrame);
				}
			}
			imageidx++;
		}
		break;

    case WM_COMMAND:
		switch (wParam)
		{
		case HS_BACKART:		// back to the Art Room
			FreeImages (hWnd);
			bHandled = FALSE;
			break;

		case HS_SLIDE_PLAYMOVIE:	// bring up the slide player room
			bHandled = TRUE;
			FreeImages (hWnd);
			if ( GetNumberSlides() )
				GoRoom (hInstAstral, RM_SLIDEPLAY, FALSE);
			break;

		case HS_IMAGE1_DEL:
		case HS_IMAGE2_DEL:
		case HS_IMAGE3_DEL:
		case HS_IMAGE4_DEL:
		case HS_IMAGE5_DEL:
		case HS_IMAGE6_DEL:
		case HS_IMAGE7_DEL:
		case HS_IMAGE8_DEL:
			if (DeleteSlideImage (hWnd, wParam -HS_IMAGE1_DEL +
			  SlideImageOffset))
			{
				UpdateImages (hWnd);
			}
        	bHandled = TRUE;
			break;

		case HS_IMAGE1_INS:
		case HS_IMAGE2_INS:
		case HS_IMAGE3_INS:
		case HS_IMAGE4_INS:
		case HS_IMAGE5_INS:
		case HS_IMAGE6_INS:
		case HS_IMAGE7_INS:
		case HS_IMAGE8_INS:
			if (InsertSlideImage (hWnd, wParam -HS_IMAGE1_INS +
			  SlideImageOffset))
			{
				UpdateImages (hWnd);
			}
        	bHandled = TRUE;
			break;

		case IDC_SLIDE_IMAGE1:
		case IDC_SLIDE_IMAGE2:
		case IDC_SLIDE_IMAGE3:
		case IDC_SLIDE_IMAGE4:
		case IDC_SLIDE_IMAGE5:
		case IDC_SLIDE_IMAGE6:
		case IDC_SLIDE_IMAGE7:
		case IDC_SLIDE_IMAGE8:
        	if (ProcessImage (hWnd, wParam -IDC_SLIDE_IMAGE1 +
			  SlideImageOffset))
			{
				FreeImages (hWnd);
            	GoRoom (hInstAstral, RM_GALLERY, FALSE);
			}
        	bHandled = TRUE;
			break;

		case HS_IMAGE1_EFF:
		case HS_IMAGE2_EFF:
		case HS_IMAGE3_EFF:
		case HS_IMAGE4_EFF:
		case HS_IMAGE5_EFF:
		case HS_IMAGE6_EFF:
		case HS_IMAGE7_EFF:
		case HS_IMAGE8_EFF:
			wsprintf (szNumber, "%d", wParam -HS_IMAGE1_EFF +SlideImageOffset);
			SaveSlideNumber (szNumber);
			AstralDlg (NO|2, hInstAstral, hWnd, IDD_SLIDE_EFFSETUP,
			  (DLGPROC)DlgSlideEffProc);
        	bHandled = TRUE;
			break;

		case HS_SLIDE_NEXTPAGE:
			if (SlideImageOffset < (OFFSET_INC *2))
			{
				SlideImageOffset += OFFSET_INC;
				UpdateImages (hWnd);
			}
			if (SlideImageOffset > OFFSET_INC)
				ShowWindow (GetDlgItem(hWnd, HS_SLIDE_NEXTPAGE), SW_HIDE);
			ShowWindow (GetDlgItem(hWnd, HS_SLIDE_PREVPAGE), SW_SHOW);
			bHandled = TRUE;
			break;

		case HS_SLIDE_PREVPAGE:
			if (SlideImageOffset > (OFFSET_INC -1))
			{
				SlideImageOffset -= OFFSET_INC;
				UpdateImages (hWnd);
			}
			if (SlideImageOffset <= (OFFSET_INC -1))
				ShowWindow (GetDlgItem(hWnd, HS_SLIDE_PREVPAGE), SW_HIDE);
			else if (SlideImageOffset < (OFFSET_INC *2))
				ShowWindow (GetDlgItem(hWnd, HS_SLIDE_NEXTPAGE), SW_SHOW);
			bHandled = TRUE;
			break;

		default:
        	bHandled = FALSE;
			break;
		}

    default:
        bHandled = FALSE;
		break;
	}
    if (! bHandled)
        bHandled = DefRoomProc (hWnd,wMsg,wParam,lParam);
    return bHandled;
}


/****************************************************************************/

BOOL WINPROC EXPORT DlgRoomSlidePlay (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
	HWND	hImageControl;
	BOOL	bHandled = TRUE;
	STRING	szFileName;
	STRING	szDelay;

	switch (wMsg)
	{
	case WM_INITDIALOG:
		DefRoomProc (hWnd, wMsg, wParam, lParam);
		bHandled = TRUE;
		SlideIndex = 0;
		bAnimationOn = NO;
		bAnimationStarted = FALSE;
		bStopped = FALSE;

		SlideCount = GetNumberSlides();
		if (SlideCount)
		{
			// get the first .art file and display it
			clr ((LPTR)szFileName, sizeof(STRING));
			while (SlideIndex < MAX_SLIDES)
			{
				if (GetSlideArtFileName (SlideIndex, (LPSTR)&szFileName))
					break;
				else
					SlideIndex++;
			}
			if (szFileName[0] != 0)
			{
				hImageControl = GetDlgItem(hWnd, IDC_SLIDEPLAY_IMAGE);
				if (AstralImageLoad (IDN_ART, szFileName, MAYBE, NO))
				{
					if (hImageControl && (lpImage != NULL))
					{
						SetWindowLong (hImageControl, GWL_IMAGE,
					  	  (long)ImgGetBase(lpImage));
					}
				}
			}
			if (SlideCount > 1)
			{
				GetImageDelay (SlideIndex, szDelay);
				uDelay = (UINT)atoi (szDelay) * 1000;
				SetTimer (hWnd, uTimer = TIMER_ID, uDelay, NULL);
			}
			SlideIndex++;
			if (SlideIndex >= MAX_SLIDES -1)
				SlideIndex = 0;
		}
		// hide the start button - only start or stop will be shown
		ShowWindow (GetDlgItem(hWnd, IDC_SLIDEPLAY_START), SW_HIDE);
		SoundStartResource( "projector", YES, NULL );
		break;

	case WM_PAINT:
		DefRoomProc (hWnd, wMsg, wParam, lParam);
		if (! bAnimationStarted)
		{
			SlidePlayAnimations (hWnd);
			bAnimationStarted = TRUE;
		}
        bHandled = TRUE;
		break;

    case WM_COMMAND:
		switch (wParam)
		{
		case HS_SLIDEPLAY_QUIT:
        	bHandled = TRUE;
			if (uTimer)
				KillTimer (hWnd, uTimer);
			uTimer = 0;
//			SlideStopAnimations (hWnd);
			if (bAnimationOn)
			{
				StopAnimation();
				SoundStopAndFree();
				bAnimationOn = NO;
			}
			GoRoom (hInstAstral, RM_SLIDE, FALSE);
			break;

		case IDC_SLIDEPLAY_START:
		case IDC_SLIDEPLAY_STOP:
			if (bStopped)
			{
				ShowWindow (GetDlgItem(hWnd, IDC_SLIDEPLAY_START), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, IDC_SLIDEPLAY_STOP), SW_SHOW);
				GetImageDelay (SlideIndex, szDelay);
				uDelay = (UINT)atoi (szDelay) * 1000;
				SetTimer (hWnd, uTimer = TIMER_ID, uDelay, NULL);
				SoundStartResource( "projector", YES, NULL );
			}
			else
			{
				// pause the slideshow and show the Play button
				KillTimer (hWnd, uTimer);
				uTimer = 0;
				ShowWindow (GetDlgItem(hWnd, IDC_SLIDEPLAY_STOP), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, IDC_SLIDEPLAY_START), SW_SHOW);
				SoundStop();
			}
			bStopped = !bStopped;
        	bHandled = TRUE;
			break;
		default:
        	bHandled = FALSE;
			break;
		}

	case WM_TIMER:
        bHandled = TRUE;
		if (uTimer)
		{
			KillTimer (hWnd, uTimer);
			uTimer = 0;
			clr ((LPTR)szFileName, sizeof(STRING));
			while (SlideIndex < MAX_SLIDES)
			{
				if (GetSlideArtFileName (SlideIndex, (LPSTR)&szFileName))
					break;
				else
					SlideIndex++;
			}
			if (szFileName[0] != 0)
			{
				hImageControl = GetDlgItem(hWnd, IDC_SLIDEPLAY_IMAGE);
				if (AstralImageLoad (IDN_ART, szFileName, MAYBE, NO))
				{
					if (hImageControl && (lpImage != NULL))
					{
						SetWindowLong (hImageControl, GWL_IMAGE,
				  		  (long)ImgGetBase(lpImage));
					}
					// turn off previous animations
					if (bAnimationOn)
					{
						StopAnimation();
						SoundStopAndFree();
						bAnimationOn = NO;
					}
					AstralControlPaint (hWnd, IDC_SLIDEPLAY_IMAGE);
					SoundStartResource( "projector", YES, NULL );
					SlidePlayAnimations (hWnd);
				}
			}
			GetImageDelay (SlideIndex, szDelay);
			uDelay = (UINT)atoi (szDelay) * 1000;
			SetTimer (hWnd, uTimer = TIMER_ID, uDelay, NULL);
			SlideIndex++;
			if (SlideIndex > MAX_SLIDES -1)
				SlideIndex = 0;
		}
        bHandled = TRUE;
		break;

    default:
        bHandled = FALSE;
		break;
	}
    if (! bHandled)
        bHandled = DefRoomProc (hWnd,wMsg,wParam,lParam);
    return bHandled;
}


/************************************************************************/
// Dialog procedure for the Slideshow Effects Setup

BOOL WINPROC EXPORT DlgSlideEffProc (HWND hDlg,UINT msg,WPARAM wParam,LPARAM lParam)
{
	STRING	szNumber;
	STRING	szBuf;
	STRING	szTmp;
	int		iNumber;

	switch (msg)
	{
		case WM_INITDIALOG:
		SET_CLASS_HBRBACKGROUND(hDlg, ( HBRUSH )GetStockObject(LTGRAY_BRUSH));
		CenterPopup (hDlg);
		SendDlgItemMessage (hDlg, IDC_EFFSETUP_EDIT, EM_LIMITTEXT, 2, 0L);
		// Index to the image control
		GetSlideNumber (szTmp);
		iNumber = atoi (szTmp);
		// Get current setting if one, will get back default setting if not
		GetImageDelay (iNumber, szTmp);
		SetDlgItemText (hDlg, IDC_EFFSETUP_EDIT, szTmp);
		break;

    	case WM_CTLCOLOR:
		return ( (BOOL)SetControlColors ((HDC)wParam, hDlg, (HWND)LOWORD(lParam),
		  HIWORD(lParam)));

		case WM_COMMAND:
		switch (wParam)
		{
			case IDOK:
			GetDlgItemText (hDlg, IDC_EFFSETUP_EDIT, szBuf, sizeof(szBuf));
			iNumber = atoi (szBuf);
			if (iNumber < 1 || iNumber > 60)
			{
				if (iNumber < 1)
				{
					wsprintf (szTmp, "%d", 1);
					SetDlgItemText (hDlg, IDC_EFFSETUP_EDIT, szTmp);
				}
				else if (iNumber > 60)
				{
					wsprintf (szTmp, "%d", 60);
					SetDlgItemText (hDlg, IDC_EFFSETUP_EDIT, szTmp);
				}
				return (FALSE);
			}
			// Get the index to the image control
			GetSlideNumber (szNumber);
			// Save this in the CGALLERY.INI file, SlideshowDelay section
			SaveImageDelay (szNumber, szBuf);
			AstralDlgEnd (hDlg, TRUE|2);
			break;

			case IDCANCEL:
			AstralDlgEnd (hDlg, FALSE|2);
			break;

			default:
			return (FALSE);
		}
		break;

		default:
		return (FALSE);
		break;
	}
	return (TRUE);
}


/****************************************************************************/

BOOL ProcessImage (HWND hWnd, int Index)
{
	STRING   szNumber;

	clr ((LPTR)szNumber, sizeof(STRING));
	if (Index <= MAX_SLIDES -1)
	{
		// save an index to the image
		// rgallery.c uses this to name the slideshow thumbnail
		wsprintf (szNumber, "%d", Index);
		if (SaveSlideNumber (szNumber))
			return TRUE;
	}
	return FALSE;
}


/****************************************************************************/

void FreeImages (HWND hWnd)
{
	int		idx;
	LPFRAME lpFrame;
	HWND	hImageControl;

	for (idx = 0 ; idx < NUM_IMAGES ; idx++)
	{
		hImageControl = GetDlgItem(hWnd, ImageIDs[idx]);
		if ((lpFrame = (LPFRAME)GetWindowLong (hImageControl,
	  	  GWL_IMAGE)) != 0L)
		{
			FrameClose (lpFrame);
		 	SetWindowLong (hImageControl, GWL_IMAGE, 0L);
		}
	}
}


/************************************************************************/
static void UpdateImages (HWND hWnd)
{
	int		idx;
	WORD    DataType;
	HWND	hImageControl;
	LPFRAME	lpFrame;
	RECT	rect;
	STRING	szFileName;
	int		imageidx = 0;

	FreeImages (hWnd);

	for (idx = SlideImageOffset ; idx < NUM_IMAGES + SlideImageOffset ; idx++)
	{
		hImageControl = GetDlgItem (hWnd, ImageIDs[imageidx]);
		if (szSlideNames[idx][0] != 0)
		{
			GetThumbFilename (szFileName, idx);
            lpFrame = tifrdr (szFileName, -1, (LPINT)&DataType, YES);
			if (lpFrame && hImageControl)
			{
				SetWindowLong (hImageControl, GWL_IMAGE, (long)lpFrame);
        		ShowWindow (hImageControl, SW_SHOW);
        		InvalidateRect (hImageControl, NULL, TRUE);
			}
		}
		else
		{
			GetClientRect (hImageControl, &rect);
			MapWindowPoints (hImageControl, hWnd, (LPPOINT)&rect, 2);
			InvalidateRect (hWnd, &rect, TRUE);
		}
		imageidx++;
	}
}


/****************************************************************************/
static void GetThumbFilename (LPSTR szFileName, int idx)
{
	STRING	szBuff;
	LPSTR	src;

	lstrcpy (szBuff, szSlideNames[idx]);
	src = lstrchr (szBuff, '=');
	*src = 0;
	lstrcpy (szFileName, szBuff);
}


/****************************************************************************/
// Delete current image and shuffle others up
BOOL DeleteSlideImage (HWND hWnd, int Index)
{
	int		idx;

	if (Index == MAX_SLIDES -1)
	{
		if (szSlideNames[Index][0] != 0)
		{
			MyDeleteFile ((LPSTR)szSlideNames[Index]);
			clr ((LPTR)szSlideNames[Index], MAX_STR_LEN);
			UpdateSlideINI();
			return TRUE;
		}
		else
			return FALSE;
	}

	for (idx = Index ; idx < (MAX_SLIDES -1) ; idx++)
	{
		if (szSlideNames[idx+1][0] != 0)
		{
			// Rename the thumbnail files
			MyRenameFile ((LPSTR)szSlideNames[idx +1], idx);

			// Shift the INI strings for these entries
			lstrcpy (szSlideNames[idx], szSlideNames[idx+1]);
			clr ((LPTR)szSlideNames[idx +1], MAX_STR_LEN);
		}
		else
		{
			clr ((LPTR)szSlideNames[idx], MAX_STR_LEN);
		}
	}
	// clear last INI entry
	if (szSlideNames[MAX_SLIDES -1][0] != 0)
	{
		MyDeleteFile ((LPSTR)szSlideNames[MAX_SLIDES -1]);
		clr ((LPTR)szSlideNames[MAX_SLIDES -1], MAX_STR_LEN);
	}
	UpdateSlideINI();
	return TRUE;
}


/****************************************************************************/
// Insert an empty image and shuffle others down
BOOL InsertSlideImage (HWND hWnd, int Index)
{
	int		idx, iStart;

	if (Index >= (MAX_SLIDES - 1))
	{
		return FALSE;
	}

	for (idx = MAX_SLIDES -1 ; idx > Index ; idx--)
	{
		if (szSlideNames[idx][0] == 0)
			break;
	}
	if (idx == Index)
		return FALSE;

	iStart = idx;

	for (idx = iStart ; idx > Index ; idx--)
	{
		if (szSlideNames[idx-1][0] != 0)
		{
			MyRenameFile ((LPSTR)szSlideNames[idx -1], idx);

			// Shift the INI strings for these entries
			lstrcpy (szSlideNames[idx], szSlideNames[idx -1]);
			clr ((LPTR)szSlideNames[idx -1], MAX_STR_LEN);
		}
		else
		{
			clr ((LPTR)szSlideNames[idx], MAX_STR_LEN);
		}
	}
	// clear the entry they clicked on
	clr ((LPTR)szSlideNames[Index], MAX_STR_LEN);
	UpdateSlideINI();
	return TRUE;
}


/****************************************************************************/

BOOL UpdateSlideINI (void)
{
	STRING	szIniName;
	STRING	szSection;
	STRING	szNum;
	STRING	szIndex;
	FNAME	szPath;
	int		idx;
	LPSTR	src;
	LPSTR	dst;
  
	if (! LoadString (hInstAstral, IDS_GALLERY_INIFILE, szIniName, sizeof(STRING)) )
		return FALSE;
	if (! LoadString (hInstAstral, IDS_SLIDE_FILE_SECTION, szSection, sizeof(STRING)) )
		return FALSE;
//	if (! LoadString (hInstAstral, IDS_GALLERY_EXT, szExtension, sizeof(STRING)) )
//		return FALSE;

	lstrcpy (szPath, Control.WorkPath);
  
	// if a user is signed in, append the name to the path
	if (Control.CurrentUser[0] != 0)
	{
		lstrcat (szPath, Control.CurrentUser);
		lstrcat (szPath, "\\");
	}
	// this is the full pathname to this user's CGALLERY.INI file
	lstrcat (szPath, szIniName);
	// wipe out the entire section so it can be re-created
	WritePrivateProfileString (szSection, NULL, NULL, szPath);

	clr ((LPTR)szIndex, MAX_STR_LEN);
	clr ((LPTR)szNum, MAX_STR_LEN);

	for (idx = 0 ; idx < MAX_SLIDES ; idx++)
    {
		if (szSlideNames[idx][0] != 0)
		{
			// save the string
			dst = szNum;
			src = lstrchr (szSlideNames[idx], '=');
			src++;
			while (*src != 0)
			{
				*dst = *src;
				dst++;
				src++;
			}
			*dst = 0;

			// update the index (to the image control)
			src = lstrchr (szSlideNames[idx], '.');
			*src = 0;
			*(src +1) = 0;
			src--;
			while (isdigit (*src))
			{
				*src = 0;
				src--;
			}
			wsprintf (szIndex, "%d", idx);
			lstrcat (szSlideNames[idx], szIndex);
			lstrcat (szSlideNames[idx], ".TIF");
			WritePrivateProfileString (szSection, szSlideNames[idx], szNum, szPath);
			lstrcat (szSlideNames[idx], "=");
			lstrcat (szSlideNames[idx], szNum);
		}
	}
	return TRUE;
}


/****************************************************************************/
// Delete a Thumbnail using an INI string passed in
BOOL MyDeleteFile (LPSTR pFile)
{
	STRING	szBuff;
	LPSTR	ptr;

	lstrcpy (szBuff, pFile);
	ptr = lstrchr (szBuff, '=');
	*ptr = 0;
	FileDelete (szBuff);
	return TRUE;
}


/****************************************************************************/
// Uses the index passed in to form the new name for the Thumbnail
// deleting the new one if it already exists
BOOL MyRenameFile (LPSTR pOldName, int iNewIndex)
{
	STRING	szOldName;
	STRING	szNewName;
	STRING	szIndex;
	LPSTR	ptr;

	if (pOldName != NULL)
	{
		lstrcpy (szOldName, pOldName);
		ptr = lstrchr (szOldName, '=');
		*ptr = 0;
		lstrcpy (szNewName, szOldName);
		ptr = lstrchr (szNewName, '.');
		*ptr = 0;
		ptr--;
		while (isdigit (*ptr))
		{
			*ptr = 0;
			ptr--;
		}
		wsprintf (szIndex, "%d", iNewIndex);
		lstrcat (szNewName, szIndex);
		lstrcat (szNewName, ".TIF");
		FileDelete (szNewName);
		rename (szOldName, szNewName);
	}
	return TRUE;
}


/************************************************************************/
static BOOL MyCopyFiles (LPSTR lpInFile, LPSTR lpOutFile)
{
	int i, ifp, ofp;
	char Buffer[2048];
	int	nTotal, nCount;

	nTotal = FileSize (lpInFile) / 2048;
	lstrcpy (Buffer, lpInFile);
	if ((ifp = _lopen (Buffer, OF_READ)) < 0)
		return (FALSE);
	lstrcpy (Buffer, lpOutFile);
	if ((ofp = _lcreat (Buffer, 0)) < 0)
	{
		_lclose (ifp);
		return (FALSE);
	}

	i = sizeof (Buffer);
	nCount = 0;
	while (1)
	{
		if ((i = _lread (ifp, Buffer, i)) <= 0)
		{
			_lclose (ifp);
			_lclose (ofp);
			return (i == 0);
		}
		if ( _lwrite (ofp, Buffer, i) != i)
		{
			_lclose (ifp);
			_lclose (ofp);
			lstrcpy (Buffer, lpOutFile);
			unlink (Buffer);
			return (FALSE);
		}
		++nCount;
	}
	return TRUE;
}


/************************************************************************/
static void SlidePlayAnimations (HWND hWnd)
{
    LPOBJECT lpObject;

	if (lpImage && ImageHasAnimations() )
	{
/*		
		lpObject = ImgGetBase (lpImage);
		while (lpObject)
		{
			if (lpObject->ObjectDataID == OBJECT_DATA_STOCK_ANIMATION)
			{
   				lpObject->fHidden = TRUE;
			}
			lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO);
		}
		AstralControlPaint (hWnd, IDC_SLIDEPLAY_IMAGE);
*/		 
		StartAnimation (GetDlgItem (hWnd, IDC_SLIDEPLAY_IMAGE));
		bAnimationOn = YES;
	}
}


/************************************************************************/
static void SlideStopAnimations (HWND hWnd)
{
    LPOBJECT lpObject;

	if (bAnimationOn && lpImage != NULL)
	{
		StopAnimation();
/*		
		lpObject = ImgGetBase (lpImage);
		while (lpObject)
		{
			if (lpObject->ObjectDataID == OBJECT_DATA_STOCK_ANIMATION)
			{
   				lpObject->fHidden = FALSE;
			}
			lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO);
		}
		AstralControlPaint (hWnd, IDC_SLIDEPLAY_IMAGE);
*/		 
	}
	SoundStopAndFree();
	bAnimationOn = NO;
}


/************************************************************************/
static BOOL ImageHasAnimations (void)
{
    LPOBJECT lpObject;

	if (lpImage)
	{
		lpObject = ImgGetBase (lpImage);
		while (lpObject)
		{
			if (lpObject->ObjectDataID == OBJECT_DATA_STOCK_ANIMATION)
			{
   				return TRUE;
			}
			lpObject = ImgGetNextObject (lpImage, lpObject, YES, NO);
		}
	}
	return FALSE;
}



