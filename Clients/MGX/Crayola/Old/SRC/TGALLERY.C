//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include <dos.h>
#include <ctype.h>		// isdigit(), toupper()
#include <direct.h>		// chdrive(), getdrive()
#include <string.h>		// strchr()
#include <stdlib.h>		// atoi()
#include "id.h"
#include "data.h"
#include "routines.h"

#ifdef _MAC
#define CURRENTDRIVE 2
#define CHDRIVE(drive) 'C'
#else
#define CURRENTDRIVE (_getdrive()-1)
#define CHDRIVE(drive) (_chdrive((drive)+1) )
#endif

extern	HINSTANCE	hInstAstral;
extern	HWND	hWndAstral;

static	BOOL	bSaving;
static	char	szFloppies[6];
static	FNAME	szFileSpec;
static	int		iStartDrive;
static	int		iFloppyIndex;


/***********************************************************************/
BOOL WINPROC EXPORT DlgGalleryProc(
/***********************************************************************/
HWND 	hDlg,
UINT 	msg,
WPARAM 	wParam,
LPARAM 	lParam)
{
	FNAME	szTitleSave;
	BOOL	bUntitledSave;

switch (msg)
    {
    case WM_INITDIALOG:
	SetControlFont (hDlg, IDC_GALLERY_T1);
	SetControlFont (hDlg, IDC_GALLERY_T2);
	SetControlFont (hDlg, IDC_GALLERY_T3);
	SetControlFont (hDlg, IDC_GALLERY_T4);
	RibbonInit( hDlg );
	// fall thru...

	case WM_CONTROLENABLE:
	case WM_DOCACTIVATED:
	break;

    case WM_SETCURSOR:
	return( SetupCursor( wParam, lParam, IDD_FILL ) );

    case WM_ERASEBKGND:
	break; // handle ERASEBKGND and do nothing; PAINT covers everything

    case WM_PAINT:
	LayoutPaint( hDlg );
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, NO|2);
	break;

    case WM_MEASUREITEM:
    case WM_DRAWITEM:
	return( OwnerDraw( hDlg, msg, lParam, NO ) );

    case WM_CTLCOLOR:
	return( (BOOL)SetControlColors( (HDC)wParam, hDlg, (HWND)LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_COMMAND:
	switch (wParam)
	    {
		case IDC_GALLERY_T1:	// Save to Disk
		SendMessage( hWndAstral, WM_COMMAND, IDM_SAVESPECIAL, 0L );
		break;

		case IDC_GALLERY_T2:	// Print
		SendMessage( hWndAstral, WM_COMMAND, IDM_PRINT, 0L );
		break;

		case IDC_GALLERY_T3:	// To Floppy
		bSaving = TRUE;
		AstralDlg (NO|2, hInstAstral, hDlg, IDD_SAVEFLOPPY, (DLGPROC)DlgFloppyProc);
		if (szFileSpec[0] != 0)
		{
			bUntitledSave = lpImage->fUntitled;
			lpImage->fUntitled = FALSE;
			lstrcpy (szTitleSave, lpImage->CurFile);
			lstrcpy (lpImage->CurFile, szFileSpec);
			SendMessage (hWndAstral, WM_COMMAND, IDM_SAVESPECIAL, 0L);
			lstrcpy (lpImage->CurFile, szTitleSave);
			lpImage->fUntitled = bUntitledSave;
		}
		break;

		case IDC_GALLERY_T4:	// From Floppy
		bSaving = FALSE;
		AstralDlg (NO|2, hInstAstral, hDlg, IDD_LOADFLOPPY, (DLGPROC)DlgFloppyProc);
		if (szFileSpec[0] != 0)
		{
			AstralImageLoad (IDN_ART, szFileSpec, MAYBE, NO);
		}
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }

return( TRUE );
}


/************************************************************************/
int GalleryProc(
/************************************************************************/
HWND 	hWindow,
LPARAM 	lParam,
UINT 	msg)
{
return( ToyProc( hWindow, lParam, msg ) );
}


/************************************************************************/
BOOL WINPROC EXPORT DlgFloppyProc (HWND hWnd, WORD msg, WORD wParam, LONG lParam)
/************************************************************************/
{
	int		iNum;
	WORD	wSel;
	DWORD	dwResult;
	FNAME	szFileName;
	STRING	szExt;
	STRING	szPref;
	STRING	szTitle;
	STRING	szText;
	char	szNum[8];

	switch (msg)
	{
		case WM_INITDIALOG:
		SET_CLASS_HBRBACKGROUND(hWnd, ( HBRUSH )GetStockObject(LTGRAY_BRUSH));
		CenterPopup (hWnd);
		if (bSaving)
		{
			LoadString (hInstAstral, IDS_SAVEFLOPPY, szPref, sizeof(szPref));
			SetWindowText (hWnd, szPref);
			ShowWindow (GetDlgItem(hWnd, IDC_CHOOSEFILE), SW_HIDE);
		}
		else
		{
			LoadString (hInstAstral, IDS_LOADFLOPPY, szPref, sizeof(szPref));
			SetWindowText (hWnd, szPref);
			EnableWindow (GetDlgItem (hWnd, IDOK), FALSE);
		}
		LoadString (hInstAstral, IDS_SELFLOPPY, szPref, sizeof(szPref));
		SetDlgItemText (hWnd, IDC_FLOPPY_TEXT, szPref);

		clr ((LPTR)szFloppies, sizeof(szFloppies));
		if ( (GetFloppyDrives (szFloppies)) <= 1)
		{
			// if only one floppy, hide second button
			ShowWindow (GetDlgItem(hWnd, IDC_FLOPPY2), SW_HIDE);
		}
		else
		{
			SetDlgItemText (hWnd, IDC_FLOPPY2, &szFloppies[3]);
		}
		SetDlgItemText (hWnd, IDC_FLOPPY1, szFloppies);
		iStartDrive = CURRENTDRIVE;
		break;

    	case WM_CTLCOLOR:
		return( (BOOL)SetControlColors( (HDC)wParam, hWnd, (HWND)LOWORD(lParam),
		  HIWORD(lParam) ) );

		case WM_COMMAND:
		switch (wParam)
		{
			case IDC_FLOPPY1:
			case IDC_FLOPPY2:
			iFloppyIndex = wParam - IDC_FLOPPY1;
			if (iFloppyIndex)
				iFloppyIndex += 2;

			// format the search spec.
			lstrcpy (szFileSpec, &szFloppies[iFloppyIndex]);
			lstrcat (szFileSpec, "\\");
			LoadString (hInstAstral, IDS_GALLERY_PREFIX, szPref, sizeof(szPref));
			lstrcat (szFileSpec, szPref);
			LoadString (hInstAstral, IDN_ART, szExt, sizeof(szExt));
			lstrcat (szFileSpec, szExt);

			if (bSaving)
			{
				if (! FloppySpace (iFloppyIndex))
				{
					LoadString (hInstAstral, IDS_SAVEFLOPPY, szTitle,
					  sizeof(szTitle));
					LoadString (hInstAstral, IDS_ENOTSPACE, szText,
					  sizeof(szText));
					MessageBox (hWnd, szText, szTitle,
					  MB_OK | MB_ICONEXCLAMATION);
					break;
				}
				if (! DlgDirList (hWnd, szFileSpec, IDC_CHOOSEFILE, 0,
				  DDL_READWRITE))
				{
					// no files found
					iNum = 1;
				}
				else
				{
					// find last picture number used in listbox
					iNum = FindLastNumber (hWnd);
					iNum++;
				}
				// format a pathname/filename and save the picture
				wsprintf (szNum, "%d", iNum);
				lstrcpy (szFileSpec, &szFloppies[iFloppyIndex]);
				lstrcat (szFileSpec, "\\");
				lstrcat (szFileSpec, szPref);
				lstrcat (szFileSpec, szNum);
				LoadString (hInstAstral, IDS_GALLERY_EXT, szExt, sizeof(szExt));
				lstrcat (szFileSpec, szExt);
				CHDRIVE (iStartDrive);
				AstralDlgEnd (hWnd, NO|2);
			}
			else
			{
				DlgDirList (hWnd, szFileSpec, IDC_CHOOSEFILE, 0,
				  DDL_READWRITE);
				dwResult = SendDlgItemMessage (hWnd, IDC_CHOOSEFILE,
				  LB_GETCOUNT, 0, 0L);
				if (dwResult == (DWORD)0)
				{
					LoadString (hInstAstral, IDS_LOADFLOPPY, szTitle,
					  sizeof(szTitle));
					LoadString (hInstAstral, IDS_NOPICTURES, szText,
					  sizeof(szText));
					MessageBox (hWnd, szText, szTitle,
					  MB_OK | MB_ICONEXCLAMATION);
					break;
				}
				else if (dwResult == (DWORD)1)
				{
					clr ((LPTR)szFileName, sizeof(FNAME));
					SendDlgItemMessage (hWnd, IDC_CHOOSEFILE, LB_GETTEXT,
			  	  	  (WPARAM)0, (LPARAM)((LPSTR)szFileName));
					if (szFileName[0] != 0)
					{
						lstrcpy (szFileSpec, &szFloppies[iFloppyIndex]);
						lstrcat (szFileSpec, "\\");
						lstrcat (szFileSpec, szFileName);
					}
					else
						clr ((LPTR)szFileSpec, sizeof(STRING));

					CHDRIVE (iStartDrive);
					AstralDlgEnd (hWnd, NO|2);
				}
			}
			break;

			case IDOK:
			if ((dwResult = SendDlgItemMessage (hWnd, IDC_CHOOSEFILE,
			  LB_GETCURSEL, 0, 0L)) != LB_ERR)
			{
				wSel = (WPARAM)dwResult;
				clr ((LPTR)szFileName, sizeof(FNAME));
				SendDlgItemMessage (hWnd, IDC_CHOOSEFILE, LB_GETTEXT,
			  	  (WPARAM)wSel, (LPARAM)((LPSTR)szFileName));
				if (szFileName[0] != 0)
				{
					lstrcpy (szFileSpec, &szFloppies[iFloppyIndex]);
					lstrcat (szFileSpec, "\\");
					lstrcat (szFileSpec, szFileName);
					CHDRIVE (iStartDrive);
					AstralDlgEnd (hWnd, NO|2);
				}
				else
					clr ((LPTR)szFileSpec, sizeof(STRING));
			}
			break;

			case IDCANCEL:
			clr ((LPTR)szFileSpec, sizeof(STRING));
			CHDRIVE (iStartDrive);
			AstralDlgEnd (hWnd, NO|2);
			break;

			case IDC_CHOOSEFILE:
			if ( HIWORD(lParam) == LBN_DBLCLK )
				SendMessage (hWnd, WM_COMMAND, IDOK, 1L);
			else if ((SendDlgItemMessage (hWnd, IDC_CHOOSEFILE,
			  LB_GETCURSEL, 0, 0L)) != LB_ERR)
			{
				EnableWindow (GetDlgItem (hWnd, IDOK), TRUE);
			}
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


/************************************************************************/
// Get the letters of the first two removable drives and put them into
// the passed in string in the form: a:0b:0 (two zero terminated strings)
static int GetFloppyDrives (LPSTR lpString)
{
#ifdef WIN32
	return 0;
#else
	int i, drive;
	WORD wType;
	int count = 0;
	int idx = 0;

	drive = CURRENTDRIVE;
	for (i = 0 ; i <= 26 ; i++)
	{
		if (! (GetDriveType (i)) )
			continue;
		wType = GetExtendedDriveType (i);
		if (i != drive && (wType == DRIVE_REMOVABLE))
		{
			lpString[idx++] = toupper('a' + i);
			lpString[idx++] = ':';
			lpString[idx++] = 0;
			count++;
			if (count >= 2)
				return count;
		}
	}
	return count;
#endif	
}


/************************************************************************/
static int FindLastNumber (HWND hWnd)
{
	DWORD	dwResult;
	STRING	szBuf;
	char	*src;
	int		iTmp;
	int		iRet = 0;
	int		iIdx = 0;

	clr ((LPTR)szBuf, sizeof(STRING));
	dwResult = SendDlgItemMessage (hWnd, IDC_CHOOSEFILE, LB_GETCOUNT, 0, 0L);

	if (dwResult)
	{
		for (iIdx = 0 ; iIdx < (int)dwResult ; iIdx++)
		{
			SendDlgItemMessage (hWnd, IDC_CHOOSEFILE, LB_GETTEXT, (WPARAM)iIdx,
		  	  (LPARAM)((LPSTR)szBuf));
			if (szBuf[0] != 0)
			{
				src = strchr (szBuf, '.');
				*src = 0;
				src--;
				while (isdigit (*src))
				{
				src--;
				}
				src++;
				iTmp = atoi (src);
				if (iTmp > iRet)
					iRet = iTmp;
			}
		}
	}
	return iRet;
}


/************************************************************************/
#define MIN_SPACE 50000

BOOL FloppySpace (int iFloppyIndex)
{
#ifdef WIN32
#else
	BYTE	Device;
	DWORD 	lSpace = 0;
	struct	diskfree_t DiskFree;

	Device = ((BYTE)szFloppies[iFloppyIndex] - 'A') +1;

	if (Device < 0 || Device > 26)
		return (FALSE);
	if ( _dos_getdiskfree (Device, &DiskFree) )
		return (FALSE);
	lSpace = (DWORD)DiskFree.avail_clusters * DiskFree.sectors_per_cluster * 
		DiskFree.bytes_per_sector;
	if (lSpace < MIN_SPACE)
		return (FALSE);

#endif
	return (TRUE);
}


