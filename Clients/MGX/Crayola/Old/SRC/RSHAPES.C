/*****************************************************************************

       Copyright (c) Micrografx, Inc., 1993 - 1994.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
           from Micrografx, 1303 Arapaho Rd, Richardson, Texas 75081.

******************************************************************************
******************************************************************************

                                    rshapes.c

******************************************************************************
******************************************************************************

  Description:  Shapes dialog functions.
 Date Created:  02/11/94
       Author:  Ray

*****************************************************************************/

#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"
#include "rshapes.h"

extern HINSTANCE hInstAstral;

/********************************* Constants ********************************/
int ShapesProc(HWND hWindow, LPARAM lParam, UINT msg);
void ShapesPixelProc(LPTR lpSrc, LPTR lpDst, int dx, int iDepth);
void CheckFinish(HWND hWnd, int idx);

#define MAX_WORD_LEN            20
#define COLOR_OFFSET            200

#define DEF_SHAPE_ON_RED        255
#define DEF_SHAPE_ON_GREEN      0
#define DEF_SHAPE_ON_BLUE       255

#define DEF_LETTER_ON_RED       0
#define DEF_LETTER_ON_GREEN     0
#define DEF_LETTER_ON_BLUE      255

#define DEF_SHAPE_OFF_RED       255
#define DEF_SHAPE_OFF_GREEN     255
#define DEF_SHAPE_OFF_BLUE      255

#define CHAR_START              0x20
#define CHAR_END                0x7F

#define DO_SEQUENTIAL           TRUE        // uncover the letters sequentially or random

/******************************** Private Data ******************************/
HWND hShapesWnd = 0;
int nLetters;
FINDSHAPE FindShapes[MAX_WORD_LEN];
int chTargetIdx;
static HFONT hNormalFont;

void InitShapes();
void ShowControls(HWND hWnd, bool bCover);
void SetShapesFonts(HWND hWnd);
void ShowSeachWord(HWND hWnd);
void ShowAllShapes(HWND hWnd);
void RemoveCodedImageColors();
/****************************** Public Functions ****************************/


BOOL WINPROC EXPORT DlgRoomShapes (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
    BOOL bHandled = TRUE;
    char szTemp[MAX_PATH];
    HWND hImageCtrl;

    switch (wMsg)
    {
        case WM_COMMAND:
            switch (wParam)
            {
            case HS_COVER:
                ShowControls(hWnd, TRUE);
				HideBitmaps (hWnd);
                break;
            case HS_SHOWME:
                ShowAllShapes(hWnd);
                break;
            case HS_COLORME:
                RemoveCodedImageColors();
                GoPaintApp (hWnd,szPreview);
                break;
            case HS_TAB1:
            case HS_TAB2:
            case HS_TAB3:
            case HS_TAB4:
            case HS_TAB5:
            case HS_TAB6:
            case HS_TAB7:
            case HS_TAB8:
                ShowControls(hWnd, FALSE);
				HideBitmaps (hWnd);
                SetDlgItemText(hWnd, HS_BLOCK_TEXT, "");
                wsprintf (szTemp,GetString (IDS_SHAPES, NULL),wParam - HS_TAB1 + 1);
                {
                    BOOL    bLoad;
                    hImageCtrl = GetDlgItem(hWnd, IDC_SHAPES_IMAGE);
                    
                    if (hImageCtrl)
                    {
                        PathCat (szPreview,Control.PouchPath,szTemp);
                        bLoad = AstralImageLoad( NULL, szPreview, TRUE, FALSE);
                        if (bLoad && lpImage)
                        {
                            SetWindowLong(hImageCtrl, GWL_IMAGE,
                                (long) ImgGetBase (lpImage));
                            SetWindowLong(hImageCtrl, GWL_IMAGE_TOOLPROC,
                                (long) ShapesProc);
                            ShowWindow(hImageCtrl, SW_SHOW);
                            InitShapes();
                            ShowSeachWord(hWnd);
                            ImgSetPixelProc(ShapesPixelProc);
                            AstralControlPaint(hWnd, IDC_SHAPES_IMAGE);
                        }
                    }
                        
                } // HS_TABS
				SoundStartID(HS_XTRA_SHAPES, FALSE, 0);
                break;

			case HS_HT_HAT:
				ShowWindow (GetDlgItem(hWnd, HS_HT_HAT), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, IDC_HT_HAT), SW_SHOW);
				bHandled = TRUE;
				break;
			case IDC_HT_HAT:
				ShowWindow (GetDlgItem(hWnd, IDC_HT_HAT), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, HS_HT_HAT), SW_SHOW);
				bHandled = TRUE;
				break;

			case HS_HT_IGLOO:
				ShowWindow (GetDlgItem(hWnd, HS_HT_IGLOO), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, IDC_HT_IGLOO), SW_SHOW);
				bHandled = TRUE;
				break;
			case IDC_HT_IGLOO:
				ShowWindow (GetDlgItem(hWnd, IDC_HT_IGLOO), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, HS_HT_IGLOO), SW_SHOW);
				bHandled = TRUE;
				break;

			case HS_HT_DOG:
				ShowWindow (GetDlgItem(hWnd, HS_HT_DOG), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, IDC_HT_DOG), SW_SHOW);
				bHandled = TRUE;
				break;
			case IDC_HT_DOG:
				ShowWindow (GetDlgItem(hWnd, IDC_HT_DOG), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, HS_HT_DOG), SW_SHOW);
				bHandled = TRUE;
				break;

			case HS_HT_BBEYES:
				ShowWindow (GetDlgItem(hWnd, HS_HT_BBEYES), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, IDC_HT_BBEYES), SW_SHOW);
				bHandled = TRUE;
				break;
			case IDC_HT_BBEYES:
				ShowWindow (GetDlgItem(hWnd, IDC_HT_BBEYES), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, HS_HT_BBEYES), SW_SHOW);
				bHandled = TRUE;
				break;

			case HS_HT_NECKLACE:
				ShowWindow (GetDlgItem(hWnd, HS_HT_NECKLACE), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, IDC_HT_NECKLACE), SW_SHOW);
				bHandled = TRUE;
				break;
			case IDC_HT_NECKLACE:
				ShowWindow (GetDlgItem(hWnd, IDC_HT_NECKLACE), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, HS_HT_NECKLACE), SW_SHOW);
				bHandled = TRUE;
				break;

			case HS_HT_TREE:
				ShowWindow (GetDlgItem(hWnd, HS_HT_TREE), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, IDC_HT_TREE), SW_SHOW);
				bHandled = TRUE;
				break;
			case IDC_HT_TREE:
				ShowWindow (GetDlgItem(hWnd, IDC_HT_TREE), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, HS_HT_TREE), SW_SHOW);
				bHandled = TRUE;
				break;

			case HS_HT_RACOON1:
				ShowWindow (GetDlgItem(hWnd, HS_HT_RACOON1), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, IDC_HT_RACOON1), SW_SHOW);
				bHandled = TRUE;
				break;
			case IDC_HT_RACOON1:
				ShowWindow (GetDlgItem(hWnd, IDC_HT_RACOON1), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, HS_HT_RACOON1), SW_SHOW);
				bHandled = TRUE;
				break;

			case HS_HT_LBEYES:
				ShowWindow (GetDlgItem(hWnd, HS_HT_LBEYES), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, IDC_HT_LBEYES), SW_SHOW);
				bHandled = TRUE;
				break;
			case IDC_HT_LBEYES:
				ShowWindow (GetDlgItem(hWnd, IDC_HT_LBEYES), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, HS_HT_LBEYES), SW_SHOW);
				bHandled = TRUE;
				break;

			case HS_HT_SAW:
				ShowWindow (GetDlgItem(hWnd, HS_HT_SAW), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, IDC_HT_SAW), SW_SHOW);
				bHandled = TRUE;
				break;
			case IDC_HT_SAW:
				ShowWindow (GetDlgItem(hWnd, IDC_HT_SAW), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, HS_HT_SAW), SW_SHOW);
				bHandled = TRUE;
				break;

			case HS_HT_APPLE:
				ShowWindow (GetDlgItem(hWnd, HS_HT_APPLE), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, IDC_HT_APPLE), SW_SHOW);
				bHandled = TRUE;
				break;
			case IDC_HT_APPLE:
				ShowWindow (GetDlgItem(hWnd, IDC_HT_APPLE), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, HS_HT_APPLE), SW_SHOW);
				bHandled = TRUE;
				break;

			case HS_HT_UNICORN:
				ShowWindow (GetDlgItem(hWnd, HS_HT_UNICORN), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, IDC_HT_UNICORN), SW_SHOW);
				bHandled = TRUE;
				break;
			case IDC_HT_UNICORN:
				ShowWindow (GetDlgItem(hWnd, IDC_HT_UNICORN), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, HS_HT_UNICORN), SW_SHOW);
				bHandled = TRUE;
				break;

			case HS_HT_RACOON2:
				ShowWindow (GetDlgItem(hWnd, HS_HT_RACOON2), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, IDC_HT_RACOON2), SW_SHOW);
				bHandled = TRUE;
				break;
			case IDC_HT_RACOON2:
				ShowWindow (GetDlgItem(hWnd, IDC_HT_RACOON2), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, HS_HT_RACOON2), SW_SHOW);
				bHandled = TRUE;
				break;

			case HS_HT_GEYES:
				ShowWindow (GetDlgItem(hWnd, HS_HT_GEYES), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, IDC_HT_GEYES), SW_SHOW);
				bHandled = TRUE;
				break;
			case IDC_HT_GEYES:
				ShowWindow (GetDlgItem(hWnd, IDC_HT_GEYES), SW_HIDE);
				ShowWindow (GetDlgItem(hWnd, HS_HT_GEYES), SW_SHOW);
				bHandled = TRUE;
				break;

			default:
				bHandled = FALSE;
            } // switch (wParam)
            break;
                          
        case WM_INITDIALOG:
            ShowControls(hWnd, TRUE);
			HideBitmaps (hWnd);
            SetShapesFonts(hWnd);
            hShapesWnd = hWnd;
            bHandled = FALSE;           // allow DefRoomProc to be called.
            break;

        case WM_DESTROY:
            ImgSetPixelProc(NULL);      // make sure our proc is no longer used
            if (hNormalFont)
                DeleteObject(hNormalFont);
            bHandled = FALSE;           // allow DefRoomProc to be called
            break;
            
        default:
            bHandled = FALSE;    
    } // switch (wMsg)

    if (!bHandled)
        bHandled = DefRoomProc (hWnd,wMsg,wParam,lParam);
    return bHandled;
}




int ShapesProc(
/************************************************************************/
HWND 	hWindow,
LPARAM 	lParam,
UINT 	msg)
{
    int x, y;
    LPFRAME lpFrame;
    LPTR lpSrcLine;
    WORD wFlag;
    bool bShapeOn = FALSE;
    RGBS Pixel;
    BYTE r, g, b;
	bool bPicked = FALSE;

    x = LOWORD(lParam);
    y = HIWORD(lParam);
    switch (msg)
    {
        case WM_CREATE:	        // mouse down messages
    	case WM_LBUTTONDOWN:
            /*
                Check the color of the pixel at the point where the mouse landed.
                If the pixel is a valid coded shape color then turn the shape "off",
                then redraw the image.  The shape will turn color.
            */
            if (!lpImage) return TRUE;
            lpFrame = ImgGetBaseEditFrame(lpImage);
            if (!lpFrame) return TRUE;
            
            lpSrcLine = FramePointer(lpFrame,x,y,FALSE);
            if (!lpSrcLine) return TRUE;
            FrameGetRGB(lpFrame,lpSrcLine,&Pixel,1);
            r = Pixel.red;
            g = Pixel.green;
            b = Pixel.blue;
                
            if (r != 0 && g >= CHAR_START && g < CHAR_END)
            {
                // Can the letters be picked randomly or to they have to be sequential
                if (DO_SEQUENTIAL && 
                    (FindShapes[chTargetIdx].cLetter != g))
				{
					SoundStartResource("WRONGANSWER2", FALSE, 0);
                    return TRUE;
				}
                    
                // A letter has been picked
                if (r > COLOR_OFFSET)
                    r -= COLOR_OFFSET;
                r -= 1;
                    
                if (r < MAX_WORD_LEN)
                {
                    if (b > COLOR_OFFSET)
                        b -= COLOR_OFFSET;
                    b -= 1;
                    
                    wFlag = 1 << b;
                    bShapeOn = !(FindShapes[r].FindFlags & wFlag);
                    
                    // Has a new letter been turned on
                    if (!bShapeOn)
                    {
						bPicked = TRUE;
						SoundStartResource("GOODANSWER1", FALSE, 0);
                        FindShapes[r].FindFlags &= ~wFlag;      // turn the flag off
                        FindShapes[r].nNumFind--;      // another one bites the dust
                        AstralControlPaint(hShapesWnd, IDC_SHAPES_IMAGE);
                        CheckFinish(hShapesWnd, r);
                    }
                }
            }
            if (!bPicked)
            	SoundStartResource("WRONGANSWER2", FALSE, 0);   
    		break;

    	case WM_SETCURSOR:
    		return TRUE;

        case WM_LBUTTONUP:
    		break;

        case WM_MOUSEMOVE:	// sent when ToolActive is on
    		break;
    }
    return TRUE;
}


void ShapesPixelProc(LPTR lpSrc, LPTR lpDst, int dx, int iDepth)
{
    bool bChanged;
    bool bShapeOn;
    bool bLetter;
    WORD wFlag;
    BYTE r, g, b;

    if ( !iDepth ) iDepth = 1;
    copy( lpSrc, lpDst, dx * iDepth );
    if (iDepth != 3)  return;
    
    while (--dx >= 0)
    {
        bChanged = FALSE;
    	r = *lpSrc++;
    	g = *lpSrc++;
    	b = *lpSrc++;
        bLetter = r < COLOR_OFFSET;
        
        if (r != 0 && g >= CHAR_START && g < CHAR_END)
        {
            if (r > COLOR_OFFSET)
                r -= COLOR_OFFSET;
            r -= 1;
            // This is a coded pixel that contains:
            //  1) green - Letter to be found by user.
            //  2) red   - Sequence that the letter is found in the word.
            //  3) blue  - Number of letters of this value to be located by the user.

            
            if (r < MAX_WORD_LEN)
            {
                if (b > COLOR_OFFSET)
                    b -= COLOR_OFFSET;
                b -= 1;
                
                wFlag = 1 << b;
                bShapeOn = !(FindShapes[r].FindFlags & wFlag);
                
    		    *lpDst++ = bShapeOn ? DEF_SHAPE_ON_RED   : (bLetter ? DEF_LETTER_ON_RED   : DEF_SHAPE_OFF_RED  );
    		    *lpDst++ = bShapeOn ? DEF_SHAPE_ON_GREEN : (bLetter ? DEF_LETTER_ON_GREEN : DEF_SHAPE_OFF_GREEN);
    		    *lpDst++ = bShapeOn ? DEF_SHAPE_ON_BLUE  : (bLetter ? DEF_LETTER_ON_BLUE  : DEF_SHAPE_OFF_BLUE );

                bChanged = TRUE;
            }
        }
            
        if (!bChanged)
            lpDst += 3;
    }
}



void ClearShapes()
{
    int i;
    nLetters = 0;
    
    for (i=0; i<MAX_WORD_LEN; i++)
    {
        FindShapes[i].cLetter   = 0;
        FindShapes[i].nNumFind  = 0;
        FindShapes[i].FindFlags = 0;
    }
    chTargetIdx = 0;
}


void InitShapes()
{
/* Initialize the FindShapes array with the following data:
    Letter      - letter to find by user.
    NumFind     - Number of these letters the user has to find.
    FindFlags   - flags for each one of the letters that has to be found.
*/    

    LPFRAME lpFrame;
    int Width;
    int Height;
    int x,y;
    LPTR lpSrcLine;
    BYTE r, g, b;

    ClearShapes();
    if (!lpImage) return;
    lpFrame = ImgGetBaseEditFrame(lpImage);
    if (!lpFrame) return;
    Width  = FrameXSize (lpFrame);
    Height = FrameYSize (lpFrame);


    for (y=0;y<Height;y += 5)
    {
        lpSrcLine = FramePointer (lpFrame,0,y,FALSE);

        for (x=0;x<Width;x++)
        {
            RGBS Pixel;
            WORD wFlags;
            int num;
            FrameGetRGB (lpFrame,lpSrcLine,&Pixel,1);
            r = Pixel.red;
            g = Pixel.green;
            b = Pixel.blue;
            if (r > COLOR_OFFSET)
            {
                // This is a coded pixel that contains:
                //  1) green - Letter to be found by user.
                //  2) red   - Sequence that the letter is found in the word.
                //  3) blue  - Number of letters of this value to be located by the user.

                r -= (COLOR_OFFSET+1);
                if (r < MAX_WORD_LEN)
                {
                    if (b > COLOR_OFFSET)
                        b -= (COLOR_OFFSET+1);
                    
                    wFlags = FindShapes[r].FindFlags;    
                    wFlags = wFlags | (1 << b);
                    num = FindShapes[r].nNumFind;
                    
                    FindShapes[r].FindFlags = wFlags;
                    FindShapes[r].cLetter   = g;
                    FindShapes[r].nNumFind  = max(b+1, num);
                    nLetters = max(nLetters, r+1);
                }
            }
            lpSrcLine += 3;
        } // Width
    }     // Height
}




void CheckFinish(HWND hWnd, int idx)
{
// Check to see if the game is finished, or at least another letter is completed.
    int i;
    
    if (FindShapes[chTargetIdx].nNumFind  == 0 ||
        FindShapes[chTargetIdx].FindFlags == 0)
    {
        SoundStartResource("GOODANSWER1", FALSE, 0);
        chTargetIdx++;      // now start looking for the next letter
        ShowSeachWord(hWnd);
    }

// Check to see if there are any more to do, or if the game is finished    
    for (i=0; i<nLetters; i++)
        if (FindShapes[i].nNumFind  == 0 ||
            FindShapes[i].FindFlags == 0)
        { }
        else return;
            
// The Game is finished.
    chTargetIdx = nLetters-1;
    ShowSeachWord(hWnd);
    SoundStartResource("GOODANSWER3", FALSE, 0);
    RemoveCodedImageColors();           
    AstralControlPaint(hWnd, IDC_SHAPES_IMAGE);
}



void ShowSeachWord(HWND hWnd)
{
// Show just the letters that have been found and the next one to find.
    STRING szTemp;
    int i;
    
    for (i=0; i<chTargetIdx+1; i++)
        szTemp[i] = FindShapes[i].cLetter;
    szTemp[chTargetIdx+1] = 0;
    
    SetDlgItemText(hWnd, HS_BLOCK_TEXT, szTemp);

// Redraw the background since this control has invisible background    
//  hCtrl = GetDlgItem(hWnd, HS_XTRA_SHAPES);
//  InvalidateRect(hCtrl, NULL, TRUE);
//std. RC text control doesn't need this
}




void ShowControls(HWND hWnd, bool bCover)
{
    HWND hCtrl;
    
    hCtrl = GetDlgItem(hWnd, HS_LOGO_SHAPES);
    ShowWindow(hCtrl, bCover ? SW_SHOW : SW_HIDE);
    EnableWindow(hCtrl, FALSE);

    hCtrl = GetDlgItem(hWnd, IDC_SHAPES_IMAGE);
    ShowWindow(hCtrl, bCover ? SW_HIDE : SW_SHOW);

    hCtrl = GetDlgItem(hWnd, HS_COLORME);
    ShowWindow(hCtrl, bCover ? SW_HIDE : SW_SHOW);

    hCtrl = GetDlgItem(hWnd, HS_SHOWME);
    ShowWindow(hCtrl, bCover ? SW_HIDE : SW_SHOW);

    hCtrl = GetDlgItem(hWnd, HS_XTRA_SHAPES);
    ShowWindow(hCtrl, bCover ? SW_HIDE : SW_SHOW);
    EnableWindow(hCtrl, FALSE);

    hCtrl = GetDlgItem(hWnd, HS_BLOCK_TEXT);
    ShowWindow(hCtrl, bCover ? SW_HIDE : SW_SHOW);
}



void SetShapesFonts(HWND hWnd)
{
// Set the text controls with the proper fonts.
    char szTmp[MAX_PATH];
	if (LoadString(hInstAstral, IDS_FONT_NORMAL, szTmp, sizeof(szTmp)))
	{
	    hNormalFont = CreateFont(-24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  							(PSTR)szTmp);	// Typeface Name
		if (hNormalFont != 0)
			SendDlgItemMessage(hWnd, HS_BLOCK_TEXT, WM_SETFONT, (WPARAM)hNormalFont, 0L);
    }
}


void ShowAllShapes(HWND hWnd)
{
    int i;

    for (i=0; i<MAX_WORD_LEN; i++)
    {
        FindShapes[i].nNumFind  = 0;
        FindShapes[i].FindFlags = 0;
    }
    chTargetIdx = nLetters-1;
    ShowSeachWord(hWnd);
    RemoveCodedImageColors();           
    AstralControlPaint(hWnd, IDC_SHAPES_IMAGE);
}


void RemoveCodedImageColors()
{
    LPFRAME lpFrame;
    int x, y;
    int Width;
    int Height;
    LPTR lpSrcLine;
    BYTE r, g, b;
    
    if (!lpImage) return;
    lpFrame = ImgGetBaseEditFrame(lpImage);
    if (!lpFrame) return;
    Width  = FrameXSize (lpFrame);
    Height = FrameYSize (lpFrame);


    for (y=0;y<Height;y++)
    {
	    AstralClockCursor( y, Height, NO );
        lpSrcLine = FramePointer (lpFrame,0,y,TRUE);

        for (x=0;x<Width;x++)
        {
            RGBS Pixel;
            FrameGetRGB (lpFrame,lpSrcLine,&Pixel,1);
            r = Pixel.red;
            g = Pixel.green;
            b = Pixel.blue;
            if ((r==0   && b==0   && g==0) || 
                (r==255 && b==255 && g==255))
            {
            }
            else
            {
                if (g >= CHAR_START && g < CHAR_END)
                {
                    // This is a coded shape.
                    Pixel.red   = DEF_SHAPE_ON_RED;
                    Pixel.green = DEF_SHAPE_ON_GREEN;
                    Pixel.blue  = DEF_SHAPE_ON_BLUE;
                }
                else
                {
                    Pixel.red   = 255;
                    Pixel.green = 255;
                    Pixel.blue  = 255;
                }
                FrameSetRGB (lpFrame,&Pixel,lpSrcLine,1);
            }
            lpSrcLine += 3;
            
        } // Width
    }     // Height
}


/****************************************************************************/

static void HideBitmaps (HWND hWnd)
{
	ShowWindow (GetDlgItem(hWnd, IDC_HT_HAT), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_HT_HAT), SW_SHOW);

	ShowWindow (GetDlgItem(hWnd, IDC_HT_IGLOO), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_HT_IGLOO), SW_SHOW);

	ShowWindow (GetDlgItem(hWnd, IDC_HT_DOG), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_HT_DOG), SW_SHOW);

	ShowWindow (GetDlgItem(hWnd, IDC_HT_BBEYES), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_HT_BBEYES), SW_SHOW);

	ShowWindow (GetDlgItem(hWnd, IDC_HT_NECKLACE), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_HT_NECKLACE), SW_SHOW);

	ShowWindow (GetDlgItem(hWnd, IDC_HT_TREE), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_HT_TREE), SW_SHOW);

	ShowWindow (GetDlgItem(hWnd, IDC_HT_RACOON1), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_HT_RACOON1), SW_SHOW);

	ShowWindow (GetDlgItem(hWnd, IDC_HT_LBEYES), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_HT_LBEYES), SW_SHOW);

	ShowWindow (GetDlgItem(hWnd, IDC_HT_SAW), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_HT_SAW), SW_SHOW);

	ShowWindow (GetDlgItem(hWnd, IDC_HT_APPLE), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_HT_APPLE), SW_SHOW);

	ShowWindow (GetDlgItem(hWnd, IDC_HT_UNICORN), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_HT_UNICORN), SW_SHOW);

	ShowWindow (GetDlgItem(hWnd, IDC_HT_RACOON2), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_HT_RACOON2), SW_SHOW);

	ShowWindow (GetDlgItem(hWnd, IDC_HT_GEYES), SW_HIDE);
	ShowWindow (GetDlgItem(hWnd, HS_HT_GEYES), SW_SHOW);
}


