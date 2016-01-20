/*****************************************************************************

       Copyright (c) Micrografx, Inc., 1993 - 1994.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
           from Micrografx, 1303 Arapaho Rd, Richardson, Texas 75081.

******************************************************************************
******************************************************************************

                                    rpuzzle.c

******************************************************************************
******************************************************************************

  Description:  Puzzle dialog functions.
 Date Created:  01/11/94
       Author:  Ray

*****************************************************************************/

#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <math.h>
#include "id.h"
#include "data.h"
#include "routines.h"

extern HINSTANCE hInstAstral;

//static int iTempRand;


/********************************* Constants ********************************/
#define MAX_STICKERS_IN_DLL 16
/******************************** Private Data ******************************/
int iHardness = 1;       // Easy=1, Tough=2, Mind Bender=3
HWND hPuzzleWnd = 0;
int iCategory, iTotalCategories;
HFONT hItalicFont;
HFONT hNormalFont;

FNAME szGuess;
FNAME szGuessDisplay;
int iPieces;
int iGuessesLeft;
BOOL bFinished;

void LoadPuzzleImage(HWND hWnd);
void LoadBackgroundImage(HWND hWnd);
int PuzzleProc( HWND hWindow, LPARAM lParam, UINT msg );
void ShowAllObjects(HWND hWnd, BOOL bShow);
void SetPuzzleFonts(HWND hWnd);
void PrepareGuesses(HWND hWnd);
void AnnounceWin(HWND hWnd, BOOL bWin);
void ProcessKey(HWND hWnd, UINT key);
char ToUpper(char ch);
void AdjustGuesses(HWND hWnd, int inc);
void UpdateTextControl(HWND hWnd, int ID);
void ResizePuzzleFrame(LPFRAME lpNewFrame);
int  GetNextValidStampString(HINSTANCE hStamps, int iString, LPSTR lpString);
/****************************** Public Functions ****************************/


BOOL WINPROC EXPORT DlgRoomPuzzle (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
    BOOL bHandled = TRUE;

    switch (wMsg)
    {
        case WM_COMMAND:
            switch (wParam)
            {
                case HS_COVER:
                    // There is no cover in this room, but this keeps
                    // DefRoomProc from disabling ShowMe.
                    break;
                    
                case HS_BACKPUZZLE:
				case IDCANCEL:
                    GoRoom (hInstAstral, RM_PUZZLESTART, FALSE);
                    break;                    
                case HS_PUZZLE_NEWGAME:
                    ShowAllObjects(hWnd, TRUE);
                    LoadBackgroundImage(hWnd);
                    PrepareGuesses(hWnd);
                    break;                    
                case HS_SHOWME:
                case HS_SHOWMEPUZZLE:
                    bFinished = TRUE;
                    SetDlgItemText(hWnd, IDC_PUZZLE_NAME, szGuess);
                    ShowAllObjects(hWnd, FALSE);
                    break;                    
                default:
                    bHandled = FALSE;
            } // switch (wParam)
            break;
            
        case WM_KEYDOWN:
            ProcessKey(hWnd, wParam);
            break;

        case WM_INITDIALOG:
            hPuzzleWnd = hWnd;
            SetPuzzleFonts(hWnd);
            LoadPuzzleImage(hWnd);
            LoadBackgroundImage(hWnd);
            PrepareGuesses(hWnd);
            SetFocus(hWnd);
            bHandled = FALSE;           // allow DefRoomProc to be called.
            break;

        case WM_DESTROY:
            if (hItalicFont)
                DeleteObject(hItalicFont);
            if (hNormalFont)
                DeleteObject(hNormalFont);
            bHandled = FALSE;   // allow DefRoomProc to be called
			hPuzzleWnd = 0;
            break;
            
        default:
            bHandled = FALSE;    
    } // switch (wMsg)

    if (!bHandled)
        bHandled = DefRoomProc (hWnd,wMsg,wParam,lParam);

// ----------This crap hangs the app-----------
//    if (wMsg != WM_KILLFOCUS  && 
//        wMsg != WM_CLOSE      &&
//        wMsg != WM_SYSKEYDOWN &&
//        wMsg != WM_SYSKEYUP   &&
//        wMsg != WM_SYSCHAR    &&
//        wMsg != WM_DESTROY    &&
//        wMsg != WM_NCDESTROY)
//        SetFocus(hWnd);

    return bHandled;
}


BOOL PuzzleRoomMsg(LPMSG lpMsg)
{
// When the puzzle room is active, it wants ALL key strokes.
// Check to see if this is a key stroke and the puzzle screen is active.

	if (hPuzzleWnd && lpMsg)
	{
		if (lpMsg->message == WM_KEYDOWN)
		{
			SendMessage(hPuzzleWnd, lpMsg->message, lpMsg->wParam, lpMsg->lParam);
			return TRUE;
		}
	}
	return FALSE;
}




int PuzzleProc(
/************************************************************************/
HWND 	hWindow,
LPARAM 	lParam,
UINT 	msg)
{
    int         x, y;
    LPOBJECT    lpObject;
    HWND        hImageCtrl;
    RECT        rUpdate;

    x = LOWORD(lParam);
    y = HIWORD(lParam);
    switch (msg)
    {
        case WM_CREATE:	// The first mouse down message
    	case WM_LBUTTONDOWN:
            lpObject = ImgFindObject(lpImage, x, y, TRUE, TRUE);
    		if (lpObject && !lpObject->fHidden)
    		{
#ifndef _MAC
                SoundStartResource("ICONS", FALSE, 0);
#endif
                lpObject->fHidden = TRUE;
                rUpdate = lpObject->rObject;
                InflateRect(&rUpdate, 2, 2);
                hImageCtrl = GetDlgItem(hPuzzleWnd, IDC_PUZZLE_IMAGE);
                InvalidateRect(hImageCtrl, &rUpdate, FALSE);
                UpdateWindow(hImageCtrl);
                AdjustGuesses(hPuzzleWnd, -1);
            }
    		break;

    	case WM_SETCURSOR:
    		return TRUE;

        case WM_LBUTTONUP:
    		break;

        case WM_MOUSEMOVE:	// sent when ToolActive is on
    		break;

        case WM_KEYDOWN:
            ProcessKey (hPuzzleWnd, (UINT)lParam);
            break;
    }
    return TRUE;
}



void LoadPuzzleImage(HWND hWnd)
{
    char szTemp[MAX_PATH];
    char szFname[MAX_PATH];
    BOOL bLoad;
    HWND hImageCtrl = GetDlgItem(hWnd, IDC_PUZZLE_IMAGE);
    if (hImageCtrl)
    {
        lstrcpy(szTemp, GetString (IDS_PUZZLE, NULL));
        wsprintf(szFname, szTemp, iHardness);
        PathCat(szPreview,Control.PouchPath,szFname);
        bLoad = AstralImageLoad(0, szPreview, TRUE, FALSE);
        if (bLoad && lpImage)
        {
            // set in the image and tool proc
            SetWindowLong(hImageCtrl, GWL_IMAGE,
                (long) ImgGetBase (lpImage));
            SetWindowLong(hImageCtrl, GWL_IMAGE_TOOLPROC,
                (long) PuzzleProc);
            ShowWindow(hImageCtrl, SW_SHOW);
            AstralControlPaint(hWnd, IDC_PUZZLE_IMAGE);
        }
    }
}


void LoadBackgroundImage(HWND hWnd)
{
    HINSTANCE hStamps = 0;
    HWND hImageCtrl = GetDlgItem(hWnd, IDC_PUZZLE_IMAGE);
    if (hImageCtrl)
    {
        // Choose a random sticker dll based upon iCategory.    
	    iTotalCategories = GetExtNameCount(IDN_STAMP);
        iCategory = rand();
        iCategory %= iTotalCategories;
	    hStamps = OptionLoadLibrary(hWnd, 0, 0,
		    IDN_STAMP, iCategory, iTotalCategories);
        if (hStamps > (HINSTANCE)HINSTANCE_ERROR)
        {
            LPFRAME lpNewFrame;
            // Choose a random sticker based on iCategory
            iCategory = rand();
            iCategory %= MAX_STICKERS_IN_DLL;
            iCategory++;
            iCategory = GetNextValidStampString(hStamps, iCategory, szGuess);
            lpNewFrame = TiffResource2Frame(hStamps,
                (LPSTR)MAKEINTRESOURCE(iCategory));
            if (lpNewFrame)
                ResizePuzzleFrame(lpNewFrame);
            FreeLibrary(hStamps);
        }
        hStamps = 0;
    }
}


void ResizePuzzleFrame(LPFRAME lpNewFrame)
{
    LPFRAME     lpOldFrame;
    LPFRAME     lpPrevFrame;
    double      fXMult;
    double      fYMult;
    double      fMult;
    int         iXNew;
    int         iYNew;
    int         iXOld;
    int         iYOld;

    lpOldFrame = ImgGetBaseEditFrame(lpImage);
    iXNew = FrameXSize(lpNewFrame);
    iYNew = FrameYSize(lpNewFrame);
    iXOld = FrameXSize(lpOldFrame);
    iYOld = FrameYSize(lpOldFrame);

    fXMult = (double)iXOld / (double)iXNew;
    fYMult = (double)iYOld / (double)iYNew;
        
    if (fXMult < fYMult)
         fMult = fXMult;
    else fMult = fYMult;
    iXNew = (int)((double)iXNew * fMult);
    iYNew = (int)((double)iYNew * fMult);
    
    lpPrevFrame = lpNewFrame;
    lpNewFrame = SizeFrame(
        lpNewFrame, 
        iXNew, 
        iYNew, 
        FrameResolution(lpOldFrame),
		FALSE);
    FrameClose(lpPrevFrame);

    iXNew = (FrameXSize(lpOldFrame) - FrameXSize(lpNewFrame)) / 2;
    iYNew = (FrameYSize(lpOldFrame) - FrameYSize(lpNewFrame)) / 2;
    iXNew = max(0, iXNew);
    iYNew = max(0, iYNew);
    lpPrevFrame = lpNewFrame;
    lpNewFrame = ExpandFrame(lpNewFrame, 
                    FrameXSize(lpOldFrame), /*pix*/ 
                    FrameYSize(lpOldFrame), /*lin*/ 
                    iXNew,                  /*left*/ 
                    iYNew,                  /*top*/ 
					FrameGetBackground(lpOldFrame));

    ImgReplaceBaseEditFrame(lpImage, lpNewFrame);
    FrameClose(lpPrevFrame);
}



void PrepareGuesses(HWND hWnd)
{
    int i;
    bFinished = FALSE;
    iPieces = ImgCountObjects(lpImage);
    iGuessesLeft = iPieces * 60 / 100;
	switch (iPieces)
		{
		case 10+1:
			iGuessesLeft = 15;
			break;
		case 15+1:
			iGuessesLeft = 12;
			break;
		case 20+1:
		default:
			iGuessesLeft = 10;
			break;
		}

    SetDlgItemInt(hWnd, IDC_PUZZLE_CHANGES, iGuessesLeft, FALSE);
    lstrcpy( szGuessDisplay, szGuess );
    for (i=0; i<lstrlen(szGuessDisplay); i++)
		if ( szGuessDisplay[i] > ' ' )
	        szGuessDisplay[i] = '*';
    SetDlgItemText(hWnd, IDC_PUZZLE_NAME, szGuessDisplay);
}



void AnnounceWin(HWND hWnd, BOOL bWin)
{
    bFinished = TRUE;
    if (bWin)
        SoundStartResource("GOODANSWER3", FALSE, 0);
}




void ShowAllObjects(HWND hWnd, BOOL bShow)
{
    LPOBJECT lpObject = ImgGetNextObject(lpImage, NULL, TRUE, FALSE);
    while (lpObject)
    {
        lpObject->fHidden = !bShow;
        lpObject = ImgGetNextObject(lpImage, lpObject, TRUE, FALSE);
    }
    AstralControlPaint(hWnd, IDC_PUZZLE_IMAGE);
}




void SetPuzzleFonts(HWND hWnd)
{
// Set the text controls with the proper fonts.
    char szTmp[MAX_PATH];
	if (LoadString(hInstAstral, IDS_FONT_ITALIC, szTmp, sizeof(szTmp)))
	{
	    hItalicFont = CreateFont(-26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  							(PSTR)szTmp);	// Typeface Name
		if (hItalicFont != 0)
			SendDlgItemMessage(hWnd, IDC_PUZZLE_CHANGES, WM_SETFONT, (WPARAM)hItalicFont, 0L);
    }

	if (LoadString(hInstAstral, IDS_FONT_NORMAL, szTmp, sizeof(szTmp)))
	{
	    hNormalFont = CreateFont(-22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  							(PSTR)szTmp);	// Typeface Name
		if (hNormalFont != 0)
			SendDlgItemMessage(hWnd, IDC_PUZZLE_NAME, WM_SETFONT, (WPARAM)hNormalFont, 0L);
    }
}


void ProcessKey(HWND hWnd, UINT key)
{
    int i;
    BOOL bFound = FALSE;
    char ch = LOBYTE(key);
    if (bFinished) return;

// Check for any matches in the word    
    for (i=0; i<lstrlen(szGuess); i++)
    {
        if (ch == ToUpper(szGuess[i]) && ch != ToUpper(szGuessDisplay[i]))
        {
            szGuessDisplay[i] = szGuess[i];
            bFound = TRUE;
        }
    }

// A character was found    
    if (bFound)
    {
        SetDlgItemText(hWnd, IDC_PUZZLE_NAME, szGuessDisplay);
        if (lstrcmpi(szGuessDisplay, szGuess) == 0)
        {
            AnnounceWin(hWnd, TRUE); 
            PostMessage(hWnd, WM_COMMAND, HS_SHOWME, 0L);
        }
        else
            SoundStartResource("GOODANSWER1", FALSE, 0);
    }
    else
    {
        AdjustGuesses(hWnd, -1);
        SoundStartResource("WRONGANSWER2", FALSE, 0);
    }    
}

char ToUpper(char ch)
{
    if (ch > 0x60 && ch < 0x7B)
        ch -= 0x20;
    return ch;
}


void AdjustGuesses(HWND hWnd, int inc)
{
    iGuessesLeft += inc;
    SetDlgItemInt(hWnd, IDC_PUZZLE_CHANGES, iGuessesLeft, FALSE);
    if (iGuessesLeft <= 0)
    {
        AnnounceWin(hWnd, FALSE);
        PostMessage(hWnd, WM_COMMAND, HS_SHOWME, 0L);
    }
}

void UpdateTextControl(HWND hWnd, int ID)
{
    HWND hCtrl;
    HWND hParent;
    RECT r;
    POINT pt1, pt2;
    hCtrl = GetDlgItem(hWnd, ID);
    if (hCtrl)
    {
        GetWindowRect(hCtrl, &r);
        pt1.x = r.left;
        pt1.y = r.top;
        pt2.x = r.right;
        pt2.y = r.bottom;
        ScreenToClient(hWnd, &pt1);
        ScreenToClient(hWnd, &pt2);
        SetRect(&r, pt1.x, pt1.y, pt2.x, pt2.y);
        InvalidateRect(hWnd, &r, TRUE);
        InvalidateRect(hCtrl, NULL, TRUE);
//InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hCtrl);
        UpdateWindow(hPuzzleWnd);

        hParent = GetParent (hWnd);
        InvalidateRect(hParent, NULL, TRUE);
        UpdateWindow(hParent);
    }
}



//---------------------------------------------------------------------------------

BOOL WINPROC EXPORT DlgRoomPuzzleStart (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
    BOOL bHandled = TRUE;

    switch (wMsg)
    {
        case WM_COMMAND:
            switch (wParam)
            {
                case HS_PUZZLE_EASY:
				case IDOK:
                    iHardness = 1;
                    if (CheckRegistration (TRUE))
                        GoRoom (hInstAstral, RM_PUZZLE, FALSE);
                    break;                    
                case HS_PUZZLE_TOUGH:
                    iHardness = 2;
                    if (CheckRegistration (TRUE))
                        GoRoom (hInstAstral, RM_PUZZLE, FALSE);
                    break;                    
                case HS_PUZZLE_HARD:
                    iHardness = 3;
                    if (CheckRegistration (TRUE))
                        GoRoom (hInstAstral, RM_PUZZLE, FALSE);
                    break;                    
                default:
                    bHandled = FALSE;
            }
            break;
        default:
            bHandled = FALSE;    
    }

    if (!bHandled)
        bHandled = DefRoomProc (hWnd,wMsg,wParam,lParam);
    return bHandled;
}


int GetNextValidStampString(HINSTANCE hStamps, int iString, LPSTR lpString)
{
// Loop over all the strings in the DLL looking for one that is not
// empty.  When it is found, return the ID of the string resource.
    int     iStart;

    iStart = iString;
    do
    {
        if ( LoadString(hStamps, iString + (iHardness == 3 ? 50:0),
			lpString, sizeof(STRING)) && *lpString )
            	return iString;
        if (++iString > MAX_STICKERS_IN_DLL )
            iString = 1;
    } while (iStart != iString);
    return iString;
}
/****************************************************************************/

