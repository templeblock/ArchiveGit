/*****************************************************************************

       Copyright (c) Micrografx, Inc., 1993 - 1994.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
           from Micrografx, 1303 Arapaho Rd, Richardson, Texas 75081.

******************************************************************************
******************************************************************************

                                    rwho.c

******************************************************************************
******************************************************************************

  Description:  Who Are You room dialog procedure
 Date Created:  02/03/94
       Author:  Stephen Suggs

*****************************************************************************/

#include <windows.h>
#include <math.h>
#include <ctype.h>
#include <direct.h>		// rmdir()
#include "id.h"
#include "data.h"
#include "routines.h"

#ifndef WIN32
#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif	/* __cplusplus */

int _rmDir (char *);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif // WIN32

extern	HINSTANCE	hInstAstral;
extern	HWND	hWndAstral;

static	HFONT	hDlgFont;
static	int		iUserCount;
static	int		iUserIndex;
static	int		iButtonIndex;
static	BOOL	fButtonHidden;
static	BOOL	fNoUsers;
static	STRING	szNewUser;
static	int	ButtonIds[4] =
{
	IDC_WHO_BUTTON1,
	IDC_WHO_BUTTON2,
	IDC_WHO_BUTTON3,
	IDC_WHO_BUTTON4
};


/****************************************************************************/
BOOL WINPROC EXPORT DlgRoomWhoAreYou (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
	STRING	szTmp;
	WORD	wKey;
	int		iTmp;
    BOOL	bHandled = TRUE;
	int		iPtSize = 30;

	if (wMsg == WM_INITDIALOG)
	{
#ifdef _MAC	
		RECT rEdit;
#endif		
		DefRoomProc (hWnd, wMsg, wParam, lParam);

		if (LoadString (hInstAstral, IDS_FONT_NORMAL, szTmp, sizeof(szTmp)))
		{
			hDlgFont = (HFONT)NULL;
			hDlgFont = CreateFont (-iPtSize,		// Height
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
		  							(PSTR)szTmp);	// Typeface Name

			if (hDlgFont != NULL)
			{
				SendDlgItemMessage (hWnd, IDC_WHO_BUTTON1, WM_SETFONT, (WPARAM)hDlgFont, 0L);
				SendDlgItemMessage (hWnd, IDC_WHO_BUTTON2, WM_SETFONT, (WPARAM)hDlgFont, 0L);
				SendDlgItemMessage (hWnd, IDC_WHO_BUTTON3, WM_SETFONT, (WPARAM)hDlgFont, 0L);
				SendDlgItemMessage (hWnd, IDC_WHO_BUTTON4, WM_SETFONT, (WPARAM)hDlgFont, 0L);
				SendDlgItemMessage (hWnd, IDC_WHO_NEWBUTTON, WM_SETFONT, (WPARAM)hDlgFont, 0L);
				SendDlgItemMessage (hWnd, IDC_WHO_EDIT, WM_SETFONT, (WPARAM)hDlgFont, 0L);
			}
		}
		
		
#ifdef _MAC	
		// Make sure the edit control is exactly the same size as the radio button.
		// Without this hack, it is smaller on the mac.
		
		if (LoadString (hInstAstral, IDS_IM_NEW, szTmp, sizeof(szTmp)))
			SetDlgItemText (hWnd, IDC_WHO_EDIT, szTmp);

		GetClientRect(GetDlgItem(hWnd, IDC_WHO_NEWBUTTON), &rEdit);	
		SetWindowPos(
			GetDlgItem(hWnd, IDC_WHO_EDIT),
			NULL,
			0, 0,
			RectWidth(&rEdit),
			RectHeight(&rEdit),
			SWP_NOMOVE | SWP_NOZORDER);
#endif		
		
		// initially hide the edit control
    	ShowWindow (GetDlgItem(hWnd, IDC_WHO_EDIT), SW_HIDE);

		if (LoadString (hInstAstral, IDS_IM_NEW, szTmp, sizeof(szTmp)))
			SetDlgItemText (hWnd, IDC_WHO_NEWBUTTON, szTmp);

		DoInit (hWnd);

    	ShowWindow (GetDlgItem(hWnd, IDOK), SW_HIDE);
		fButtonHidden = FALSE;
		bHandled = TRUE;
	}
    else if (wMsg == WM_CLOSE)
    {
        AstralDlgEnd (hWnd,TRUE|2);
		bHandled = TRUE;
    }
    else if (wMsg == WM_COMMAND)
    {
		switch (wParam)
		{
			// Go To Room hotspot
			case HS_WHO:
				if (CheckUsername (hWnd))
				{
					DeleteObject (hDlgFont);
        			GoRoom (hInstAstral, RM_ARTROOM, FALSE);
				}
				bHandled = TRUE;
				break;

			// I'm New button
			case IDC_WHO_NEWBUTTON:
#ifndef _MAC	// This problem doesn't exist on the mac			
				if (! fNoUsers)	// hack for Windows focus problem
#endif				
				{
					CheckRadioButton (hWnd, IDC_WHO_BUTTON1,
					  IDC_WHO_NEWBUTTON, IDC_WHO_NEWBUTTON);
    				ShowWindow (GetDlgItem(hWnd, IDC_WHO_NEWBUTTON), SW_HIDE);
    				ShowWindow (GetDlgItem(hWnd, IDC_WHO_EDIT), SW_SHOW);

					if (LoadString (hInstAstral, IDS_YOUR_NAME, szTmp,
				  	sizeof(szTmp)))
						SetDlgItemText (hWnd, IDC_WHO_EDIT, szTmp);

					SetFocus (GetDlgItem (hWnd, IDC_WHO_EDIT));
					SendDlgItemMessage (hWnd, IDC_WHO_EDIT, EM_SETSEL, 0,
				  	  MAKELONG(0, -1) );
				}
				iButtonIndex = 4;
				fNoUsers = FALSE;
				bHandled = TRUE;
				fButtonHidden = TRUE;
				break;

			// Scroll names down
			case HS_WHO_UPARROW:
				if (iUserCount > 4)
				{
					iTmp = iUserIndex;
					if (iUserIndex > 0)
                    {
						iUserIndex--;
                    	ShowWindow (GetDlgItem(hWnd, HS_WHO_DNARROW), SW_SHOW);

						if (iUserIndex == 0)
                    		ShowWindow (GetDlgItem(hWnd, HS_WHO_UPARROW), SW_HIDE);
                    }
                    else
            			ShowWindow (GetDlgItem(hWnd, HS_WHO_UPARROW), SW_HIDE);

					if (iUserIndex != iTmp)
						UpdateButtons (hWnd, iUserIndex, iUserCount);
				}
				bHandled = TRUE;
				break;

			// Scroll names up
			case HS_WHO_DNARROW:
				if (iUserCount > 4)
				{
					iTmp = iUserIndex;
					if (iUserIndex < (iUserCount - 4))
                    {
						iUserIndex++;
                    	ShowWindow (GetDlgItem(hWnd, HS_WHO_UPARROW), SW_SHOW);

						if (iUserIndex +4 >= iUserCount)
                    		ShowWindow (GetDlgItem(hWnd, HS_WHO_DNARROW), SW_HIDE);
                    }
                    else
            			ShowWindow (GetDlgItem(hWnd, HS_WHO_DNARROW), SW_HIDE);

					if (iUserIndex != iTmp)
						UpdateButtons (hWnd, iUserIndex, iUserCount);
				}
				bHandled = TRUE;
				break;

			case IDC_WHO_BUTTON1:
				HandleButton (hWnd, 0);
        		bHandled = TRUE;
				// if they click twice on same control - validate and continue
				if ((iButtonIndex == 0) && (CheckUsername (hWnd)))
				{
					DeleteObject (hDlgFont);
        			GoRoom (hInstAstral, RM_ARTROOM, FALSE);
				}
				iButtonIndex = 0;
				break;

			case IDC_WHO_BUTTON2:
				HandleButton (hWnd, 1);
        		bHandled = TRUE;
				if ((iButtonIndex == 1) && (CheckUsername (hWnd)))
				{
					DeleteObject (hDlgFont);
        			GoRoom (hInstAstral, RM_ARTROOM, FALSE);
				}
				iButtonIndex = 1;
				break;

			case IDC_WHO_BUTTON3:
				HandleButton (hWnd, 2);
        		bHandled = TRUE;
				if ((iButtonIndex == 2) && (CheckUsername (hWnd)))
				{
					DeleteObject (hDlgFont);
        			GoRoom (hInstAstral, RM_ARTROOM, FALSE);
				}
				iButtonIndex = 2;
				break;

			case IDC_WHO_BUTTON4:
				HandleButton (hWnd, 3);
        		bHandled = TRUE;
				if ((iButtonIndex == 3) && (CheckUsername (hWnd)))
				{
					DeleteObject (hDlgFont);
        			GoRoom (hInstAstral, RM_ARTROOM, FALSE);
				}
				iButtonIndex = 3;
				break;

			case IDC_WHO_DELNAME:
				if (Control.CurrentUser[0] != 0)
				{
					if (! (GetNumberIniEntries() ))
					{
						if (GetUserNameByDir (Control.CurrentUser, szTmp))
						{
							DeleteUserName (szTmp);
							lstrcpy (szTmp, Control.WorkPath);
							lstrcat (szTmp, Control.CurrentUser);
#ifdef WIN32
							RemoveDirectory (szTmp);
#else
							_rmDir (szTmp);
#endif							
							clr ((LPTR)Control.CurrentUser, sizeof(Control.CurrentUser));
							DoInit (hWnd);
						}
					}
				}
        		bHandled = TRUE;
				break;

			case IDOK:
				bHandled = TRUE;
				if (CheckUsername (hWnd))
				{
					DeleteObject (hDlgFont);
       				GoRoom (hInstAstral, RM_ARTROOM, FALSE);
				}
				break;

			default:
        		bHandled = FALSE;
				break;
		}
    }
    else if (wMsg == WM_CHAR)
	{
		wKey = (WORD)lParam;
		if (wKey == VK_DELETE)
		{
			GetUserNameString (iUserIndex, szTmp);
		}
	}
    else
        bHandled = FALSE;

    if (! bHandled)
        bHandled = DefRoomProc (hWnd, wMsg, wParam, lParam);

    return bHandled;
}


/****************************************************************************/

static void HandleButton (HWND hWnd, int Index)
{
	STRING	szDir;
	HWND	hControl;

	if (fButtonHidden)
	{
		ShowWindow (GetDlgItem(hWnd, IDC_WHO_EDIT), SW_HIDE);
		ShowWindow (GetDlgItem(hWnd, IDC_WHO_NEWBUTTON), SW_SHOW);
		if ((hControl = GetDlgItem (hWnd, IDC_WHO_NEWBUTTON)))
			InvalidateRect (hControl, NULL, FALSE);
		fButtonHidden = FALSE;
	}
	clr ((LPTR)szNewUser, sizeof(STRING));
	if ( GetUserNameString (iUserIndex + Index, szNewUser) )
	{
		if ( GetUserDir (szNewUser, szDir))
			lstrcpy (Control.CurrentUser, szDir);
	}
	CheckRadioButton (hWnd, IDC_WHO_BUTTON1, IDC_WHO_NEWBUTTON,
	  ButtonIds[Index]);
}


/****************************************************************************/

static void UpdateButtons (HWND hWnd, int iUserIndex, int iUserCount)
{
	STRING	szUser;
	STRING	szDir;
	HWND	hControl;
	int 	idx, iUser;

	iUser = iUserIndex;

	// update the button text
	for (idx = 0 ; idx < 4 ; idx++)
	{
		if (GetUserNameString (iUser++, szUser))
		{
			SetDlgItemText (hWnd, ButtonIds[idx], szUser);
			if ((hControl = GetDlgItem (hWnd, ButtonIds[idx])))
				InvalidateRect (hControl, NULL, FALSE);

		}
		else
			break;
	}
	// the top button is selected by default, so get a users 
	// directory if one exists until they change it
	if ( GetUserNameString (iUserIndex, szUser) )
	{
		if ( GetUserDir (szUser, szDir))
			lstrcpy (Control.CurrentUser, szDir);
	}
}


/****************************************************************************/

static BOOL CheckUsername (HWND hWnd)
{
	STRING	szTmp;
	STRING	szDir;
	STRING	szGuest;
	char	*src;
	char	*dst;
	int		idx, iErr;

	if (fButtonHidden)
	{
		clr ((LPTR)Control.CurrentUser, sizeof(Control.CurrentUser));
		LoadString (hInstAstral, IDS_YOUR_NAME, szTmp, sizeof(szTmp));
		clr ((LPTR)szNewUser, sizeof(STRING));
		GetDlgItemText (hWnd, IDC_WHO_EDIT, szNewUser, sizeof(szNewUser));
		if ( (szNewUser[0] != 0) && (lstrcmp (szNewUser, szTmp)))
		{
			// create a directory name by using the first 8
			// alnum characters
			clr ((LPTR)szDir, sizeof(STRING));
			src = szNewUser;
			dst = szDir;
			idx = 0;
			while (idx < 8)
			{
				if ( isalnum (*src))
				{
					*dst = *src;
					dst++;
					src++;
					idx++;
				}
				else
				{
					src++;
					// if two non-alnum char's together, break;
					if (! isalnum (*src))
						break;
				}
			}
			// save this new entry in the INI file
			SaveUsername (szNewUser, szDir);

			// create this directory if it doesn't exist
			lstrcpy (szTmp, Control.WorkPath);
			lstrcat (szTmp, szDir);
			if ( (iErr = _mkdir (szTmp)) )
			{
				// no error message, dir could already exist
			}

			// save this directory name in the control structure
			lstrcpy (Control.CurrentUser, szDir);
		}
		else
		{
			LoadString (hInstAstral, IDS_GUEST, szGuest, sizeof(szGuest));
			lstrcpy (szTmp, Control.WorkPath);
			lstrcat (szTmp, szGuest);
			_mkdir (szTmp);
			lstrcpy (Control.CurrentUser, szGuest);
			SaveUsername (szGuest, szGuest);
		}
	}
	else if (szNewUser[0] != 0)
	{
		// make sure the directory exists in case some idiot deletes it
		if (GetUserDir (szNewUser, szDir))
		{
			lstrcpy (szTmp, Control.WorkPath);
			lstrcat (szTmp, szDir);
			_mkdir (szTmp);
		}
		return TRUE;
	}
	// sign them in as Guest
	else
	{
		LoadString (hInstAstral, IDS_GUEST, szGuest, sizeof(szGuest));
		lstrcpy (szTmp, Control.WorkPath);
		lstrcat (szTmp, szGuest);
		_mkdir (szTmp);
		lstrcpy (Control.CurrentUser, szGuest);
		SaveUsername (szGuest, szGuest);
	}
	return TRUE;
}


/****************************************************************************/

static void DoInit (HWND hWnd)
{
	STRING	szTmp;
	STRING	szDir;
	int		idx;

	iUserIndex = 0;
	iUserCount = GetNumberOfUsers();
	iButtonIndex = 4;	// set to other than 0 - 3

	// hide unused radio buttons
	switch (iUserCount)
	{
		case 0:
    		ShowWindow (GetDlgItem(hWnd, IDC_WHO_BUTTON1), SW_HIDE);
		case 1:
    		ShowWindow (GetDlgItem(hWnd, IDC_WHO_BUTTON2), SW_HIDE);
		case 2:
    		ShowWindow (GetDlgItem(hWnd, IDC_WHO_BUTTON3), SW_HIDE);
		case 3:
    		ShowWindow (GetDlgItem(hWnd, IDC_WHO_BUTTON4), SW_HIDE);
		default:
			for (idx = 0 ; idx < iUserCount ; idx++)
			{
				if ( GetUserNameString (idx, szTmp) )
				{
					SetDlgItemText (hWnd, ButtonIds[idx], szTmp);
					AstralControlRepaint (hWnd, ButtonIds[idx]);
				}
			}
			break;

	}

    ShowWindow (GetDlgItem(hWnd, HS_WHO_UPARROW), SW_HIDE);
    if (iUserCount <= 4)
    	ShowWindow (GetDlgItem(hWnd, HS_WHO_DNARROW), SW_HIDE);
	else
    	ShowWindow (GetDlgItem(hWnd, HS_WHO_DNARROW), SW_SHOW);

	clr ((LPTR)Control.CurrentUser, sizeof(Control.CurrentUser));
	clr ((LPTR)szNewUser, sizeof(STRING));
	if (GetUserNameString (iUserIndex, szNewUser))
	{
		if (GetUserDir (szNewUser, szDir))
			lstrcpy (Control.CurrentUser, szDir);
	}
	if (iUserCount)
	{
		SetFocus (GetDlgItem (hWnd, IDC_WHO_BUTTON1));
		CheckRadioButton (hWnd, IDC_WHO_BUTTON1, IDC_WHO_NEWBUTTON,
	  	  IDC_WHO_BUTTON1);
		fNoUsers = FALSE;
	}
	else
	{
		SetFocus (GetDlgItem (hWnd, IDOK));
		fNoUsers = TRUE;
	}
}


