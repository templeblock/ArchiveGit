/*****************************************************************************

       Copyright (c) Micrografx, Inc., 1993 - 1994.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
           from Micrografx, 1303 Arapaho Rd, Richardson, Texas 75081.

******************************************************************************
******************************************************************************

                                    raddon.c

******************************************************************************
******************************************************************************

  Description:  Add-On room dialog procedure
 Date Created:  05/04/94
       Author:  Stephen Suggs

*****************************************************************************/

#include <windows.h>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#include "id.h"
#include "data.h"
#include "routines.h"
#include <addon.h>

extern	HINSTANCE	hInstAstral;
extern	HWND		hWndAstral;
extern	LPIMAGE		lpImage;
extern	HBITMAP		hRoomBitmap;

#define RC_TOY_NAME			2
#define RC_TOY_X			3
#define RC_TOY_Y			4
#define FIRST_CONTROL_ID	3000

#define MAX_ADDONS			30
#define PRIVATE_INI "CRAYOLA.INI"

HINSTANCE	hCurrentDLL = NULL;

//***************************************************************************
// Local variables
//***************************************************************************
static	DLGPROC	dlgProc;
static	DLGPROC	DlgAddonProc;

//***************************************************************************

typedef	void (FAR PASCAL *LPFNSETUPADDON)(HINSTANCE, LPIMAGE FAR *, HWND,
			  HINSTANCE, FARPROC, APP FAR *, HBITMAP FAR *);

//***************************************************************************

BOOL WINPROC EXPORT DlgRoomAddOn (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    BOOL	bHandled = TRUE;
	char	szName[10];
	char	szTmp[20];
	int		idx, tx, ty;
	HWND	hCtrl;
	HWND	hWndAddon;
	HINSTANCE hInstTmp;

	char szLibPath[MAX_FNAME_LEN];
	int	 iEntry = 1;

    switch (wMsg)
    {
    case WM_INITDIALOG:
		// call before creating the controls
		DefRoomProc (hWnd, wMsg, wParam, lParam);
		for (idx = 0 ; idx < MAX_ADDONS ; idx++)
		{
			// get each addon directory name from the INI file
			if ( GetDLLPath (idx + 1, szLibPath) )
			{
				if ((hInstTmp =
				  LoadLibrary (szLibPath)) > (HINSTANCE)HINSTANCE_ERROR)
				{
					LoadString (hInstTmp, RC_TOY_X, szTmp,
					  sizeof(szTmp));
					tx = atoi (szTmp);

					LoadString (hInstTmp, RC_TOY_Y, szTmp,
					  sizeof(szTmp));
					ty = atoi (szTmp);

					// get the animated button name
					if ((LoadString (hInstTmp,
					  RC_TOY_NAME, szName, sizeof(szName) )) != 0)
					{
						hCtrl = CreateWindow (
								"toy",
								szName,
								WS_VISIBLE | WS_CHILD,
								tx,
								ty,
								0,
								0,
								hWnd,
								(HMENU)(FIRST_CONTROL_ID + idx),
								hInstAstral,
								NULL);
					}
					FreeLibrary (hInstTmp);
					hInstTmp = NULL;
				}
			}
			else
				break;
		}
		bHandled = TRUE;
		break;

    case WM_DESTROY:
		idx = 0;
		while ( (hCtrl = GetDlgItem (hWnd, (FIRST_CONTROL_ID + idx))) != NULL)
		{
			DestroyWindow (hCtrl);
			idx++;
		}

		bHandled = FALSE;
		break;

    case WM_COMMAND:
        switch (wParam)
        {
		case HS_BACKART:
			if (hCurrentDLL != NULL)
			{
				FreeLibrary (hCurrentDLL);
				hCurrentDLL = NULL;
			}
            GoRoom (hInstAstral, RM_ARTROOM, FALSE);
			break;

        default:
			// check for controls created on the fly
			if (wParam >= FIRST_CONTROL_ID  &&
			  wParam < (FIRST_CONTROL_ID + MAX_ADDONS))
			{
				if ( GetDLLPath ((wParam - FIRST_CONTROL_ID + 1), szLibPath) )
				{
					if (hCurrentDLL != NULL)
					{
						FreeLibrary (hCurrentDLL);
						hCurrentDLL = NULL;
					}

					if ((hCurrentDLL =
				  	  LoadLibrary (szLibPath)) > (HINSTANCE)HINSTANCE_ERROR)
					{
						InitDLL (hCurrentDLL);
						GoRoom (hCurrentDLL, 400, TRUE);
					}
				}
				bHandled = TRUE;
			}
			bHandled = FALSE;
		}
		break;

	default:
		bHandled = FALSE;
	}

	if (! bHandled)
		bHandled = DefRoomProc (hWnd, wMsg, wParam, lParam);

	return bHandled;
}

//***************************************************************************

BOOL GetDLLPath  (int Index, char * szPath)
{
	char szBuff[MAX_STR_LEN];
	char szEntry[5];
	char szAddOn[50];
	char szIniPath[MAX_FNAME_LEN];
	char szLibPath[MAX_FNAME_LEN];

	memset (szLibPath, 0, sizeof(szLibPath));
	memset (szIniPath, 0, sizeof(szIniPath));

//[AddOns] section moved to crayola.ini
//	lstrcpy (szIniPath, Control.ProgHome);
//	lstrcat (szIniPath, "files\\");
//	lstrcat (szIniPath, "studio.ini");

	wsprintf (szEntry, "%d", Index);
	memset (szBuff, 0, sizeof(szBuff));
	GetPrivateProfileString ("AddOns", szEntry, "", szBuff,
	  sizeof(szBuff), PRIVATE_INI);
	if (szBuff[0] != 0)
	{
		lstrcpy (szLibPath, Control.WorkPath);
//		lstrcat (szLibPath, "files\\");
		lstrcat (szLibPath, szBuff);
		lstrcat (szLibPath, "\\");
		lstrcat (szLibPath, szBuff);
		lstrcat (szLibPath, ".DLL");

		lstrcpy (szPath, szLibPath);
		return TRUE;
	}
	return FALSE;
}

//***************************************************************************

FARPROC CALLBACK EXPORT GetAppFunction (int FunctionID)
{
	FARPROC lpfn;

	switch (FunctionID)
	{
		case A_IMAGE_LOAD:
			lpfn = (FARPROC)AstralImageLoad;		break;

		case A_DLGEND:
			lpfn = (FARPROC)AstralDlgEnd;			break;

		case T_TIFRDR:
			lpfn = (FARPROC)tifrdr;					break;

		case F_FTOBMP:
			lpfn = (FARPROC)FrameToBitmap;			break;

		case F_FDEPTH:
			lpfn = (FARPROC)DLLFrameDepth;			break;

		case F_FGETRGB:
			lpfn = (FARPROC)DLLFrameGetRGB;			break;

		case F_FPOINTER:
			lpfn = (FARPROC)DLLFramePointer;		break;

		case F_FXSIZE:
			lpfn = (FARPROC)DLLFrameXSize;			break;

		case F_FYSIZE:
			lpfn = (FARPROC)DLLFrameYSize;			break;

		case F_FCLOSE:
			lpfn = (FARPROC)DLLFrameClose;			break;

		case I_GETBASE:
			lpfn = (FARPROC)ImgGetBase;				break;

		case I_GETBASEEDITF:
			lpfn = (FARPROC)ImgGetBaseEditFrame;	break;

		case I_ADDNEWOBJ:
			lpfn = (FARPROC)ImgAddNewObject;		break;

		case I_DELETEOBJS:
			lpfn = (FARPROC)ImgDeleteObjects;		break;

		case I_COMBINEOBJ:
			lpfn = (FARPROC)ImgCombineObjects;		break;

		case I_GETNEXTOBJ:
			lpfn = (FARPROC)ImgGetNextObject;		break;

		case I_SETPIXELPROC:
			lpfn = (FARPROC)ImgSetPixelProc;		break;

		case I_CHANGESEL:
			lpfn = (FARPROC)ImgChangeSelection;		break;

		case I_ADJUSTLSO:
			lpfn = (FARPROC)ImgAdjustLayerSelObj;	break;

		case I_FINDOBJ:
			lpfn = (FARPROC)ImgFindObject;			break;

		case AN_ANIMATEPROC:
			lpfn = (FARPROC)AnimateProc;			break;

		case R_GOROOM:
			lpfn = (FARPROC)GoRoom;					break;

		case R_GOPAINT:
			lpfn = (FARPROC)GoPaintApp;				break;

		case M_MASKCLOSE:
			lpfn = (FARPROC)MaskClose;				break;

		case M_MASKCREATE:
			lpfn = (FARPROC)MaskCreate;				break;

		case M_MASKCREATEALPHAF:
			lpfn = (FARPROC)MaskCreateAlphaFrame;	break;

		case O_CREATEFF:
			lpfn = (FARPROC)ObjCreateFromFrame;		break;

		case O_OBJUNLINK:
			lpfn = (FARPROC)ObjUnlinkObject;		break;

		case P_PIXMAPFRAME:
			lpfn = (FARPROC)PixmapFrame;			break;

		case F_ATOWINRECT:
			lpfn = (FARPROC)DLLAstralToWindowsRect;	break;

		case A_STARTANI:
			lpfn = (FARPROC)StartAnimation;			break;

		case A_STOPANI:
			lpfn = (FARPROC)StopAnimation;			break;

		case S_SOUNDSTARTRES:
			lpfn = (FARPROC)SoundStartResource;		break;

		case S_SOUNDSTARTFILE:
			lpfn = (FARPROC)SoundStartFile;			break;

		case S_SOUNDSTOP:
			lpfn = (FARPROC)SoundStop;				break;

		case S_SOUNDSTOPFREE:
			lpfn = (FARPROC)SoundStopAndFree;		break;

		case S_MIXSOUNDSTARTRES:
			lpfn = (FARPROC)mixSoundStartResource;	break;

		case S_MIXSOUNDSTARTFILE:
			lpfn = (FARPROC)mixSoundStartFile;		break;

		case S_MIXSOUNDSTOPALL:
			lpfn = (FARPROC)mixSoundStopAll;		break;

		case F_FSETRGB:
			lpfn = (FARPROC)DLLFrameSetRGB;			break;

		case T_TIFFRES2FRAME:
			lpfn = (FARPROC)TiffResource2Frame;		break;

		case I_DELETEOBJ:
			lpfn = (FARPROC)ImgDeleteObject;		break;

		case S_SOUNDSTARTID:
			lpfn = (FARPROC)SoundStartID;			break;

		case AN_ANIMATEDIBID:
			lpfn = (FARPROC)AnimateDibID;			break;

		case I_IMGREPBASEEDITF:
			lpfn = (FARPROC)ImgReplaceBaseEditFrame;	break;

		case I_IMGCOUNTOBJS:
			lpfn = (FARPROC)ImgCountObjects;		break;

		case F_SIZEFRAME:
			lpfn = (FARPROC)SizeFrame;				break;

		case F_EXPANDFRAME:
			lpfn = (FARPROC)ExpandFrame;			break;

		case F_FRAMERES:
			lpfn = (FARPROC)DLLFrameResolution;		break;

		case F_FRAMEGETBACKGND:
			lpfn = (FARPROC)DLLFrameGetBackground;	break;

		default:
			lpfn = NULL;
			break;
	}
	return lpfn;
}

//***************************************************************************
void InitDLL (HINSTANCE hInstance)
{
	LPFNSETUPADDON lpfnSetupAddon;

	lpfnSetupAddon = (LPFNSETUPADDON)GetProcAddress (hInstance,
	  MAKEINTRESOURCE(3) );
	if (lpfnSetupAddon != NULL)
	{
		(*lpfnSetupAddon)(hInstance, &lpImage, hWndAstral, hInstAstral,
		  (FARPROC)GetAppFunction, &Control, &hRoomBitmap);
	}
}


//***************************************************************************
// exported wrappers for frame and misc. application functions
//***************************************************************************

int CALLBACK EXPORT DLLFrameDepth (LPFRAME lpFrame)
{
	return (FrameDepth (lpFrame));
}

void CALLBACK EXPORT DLLFrameGetRGB (LPFRAME lpFrame, LPTR lpSrc,
  LPRGB lpDstRGB, int iPixelCount)
{
	FrameGetRGB (lpFrame, lpSrc, lpDstRGB, iPixelCount);
}

void CALLBACK EXPORT DLLFrameSetRGB (LPFRAME lpFrame, LPRGB lpRGBPixel,
  LPTR lpDst, int iPixelCount)
{
	FrameSetRGB (lpFrame, lpRGBPixel, lpDst, iPixelCount);
}

LPTR FAR CALLBACK EXPORT DLLFramePointer (LPFRAME lpFrame, int x, int y,
  BOOL bModify)
{
	return (FramePointer (lpFrame, x, y, bModify));
}

int CALLBACK EXPORT DLLFrameXSize (LPFRAME lpFrame)
{
	return (FrameXSize (lpFrame));
}

int CALLBACK EXPORT DLLFrameYSize (LPFRAME lpFrame)
{
	return (FrameYSize (lpFrame));
}

void CALLBACK EXPORT DLLFrameClose (LPFRAME lpFrame)
{
	FrameClose (lpFrame);
}

LPRECT CALLBACK EXPORT DLLAstralToWindowsRect (LPRECT lpDest)
{
	return (AstralToWindowsRect (lpDest));
}

int CALLBACK EXPORT DLLFrameResolution (LPFRAME lpFrame)
{
	return (FrameResolution (lpFrame));
}

BYTE CALLBACK EXPORT DLLFrameGetBackground (LPFRAME lpFrame)
{
	return (FrameGetBackground (lpFrame));
}

