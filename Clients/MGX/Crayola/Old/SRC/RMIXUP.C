/*****************************************************************************

       Copyright (c) Micrografx, Inc., 1993 - 1994.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
           from Micrografx, 1303 Arapaho Rd, Richardson, Texas 75081.

******************************************************************************
******************************************************************************

                                   rmixup.c

******************************************************************************
******************************************************************************

  Description:  Room dialog functions.
 Date Created:  01/11/94
       Author:  Ray

*****************************************************************************/

#include <windows.h>
#include <math.h>
#include "id.h"
#include "data.h"
#include "routines.h"

extern HINSTANCE hInstAstral;
extern HWND   hWndAstral;

HINSTANCE hMixLib = NULL;
int iObjectA = 1;
int iObjectB = 1;
int iObjectC = 1;
bool bObjectsAdded = FALSE;
UINT LastTab = HS_TAB1;
bool bVertOrientation = TRUE;

#define MAX_IMAGES      10
#define NUM_SECTIONS    3
// Objects currently on the screen.
LPOBJECT LastObject[NUM_SECTIONS] =
{
    NULL,
    NULL,
    NULL
};

POINT ptAOffsets[MAX_IMAGES];
POINT ptBOffsets[MAX_IMAGES];
POINT ptCOffsets[MAX_IMAGES];

/********************************* Constants ********************************/

#define MIXUP_LEFT    20
#define MIXUP_RIGHT   506
#define MIXUP_TOP     24
#define MIXUP_BOTTOM  386

#define MIX_OFFS_RES_TYPE       "OFFSETS"
#define MIX_TIFF_RES_FORMAT     "TIF_%c%i"  // TIF_a1, TIF_a2, TIF_a3 ...
#define MIX_OFFS_RES_FORMAT     "OFF_%c"    // OFF_a, OFF_b, OFF_c
#define STR_MIXUP_ORIENT    1000
#define STR_MIXUP_ORDER     1001

/****************************** Local Functions ****************************/

LPFRAME LoadImageFrame(int iSection, int iImage);
LPOBJECT AddObjectToImage(int section, int image);
void RemoveLastObject(int iSection);
void AddAllObjects();
void RepositionSectionA();
void RepositionSectionB();
void RepositionSectionC();
void RepositionToCenter(int iSection);
void IncrementSection(HWND hWnd, int iSection, int* iCounter);
void SetObjectOrder();
void UnlinkAllObjects();
void LinkAllObjects();
void LoadOffsets(LPPOINT lpPoint, int iSection);
static void CenterAllObjects();


/****************************** Public Functions ****************************/

BOOL WINPROC EXPORT DlgRoomMixup (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
    BOOL bHandled = TRUE;
    char szTemp[MAX_PATH];
    HWND hImageCtrl, hCtl;

    switch(wMsg)
    {
        case WM_COMMAND:
        {
            switch (wParam)
            {
            case HS_MIXUP_FHEAD:
            case HS_MIXUP_HEAD:
            case HS_MIXUP_FRONT:
                IncrementSection(hWnd, 1, &iObjectA);
                break;
            case HS_MIXUP_FTOP:
            case HS_MIXUP_BODY:
            case HS_MIXUP_MIDDLE:
                IncrementSection(hWnd, 2, &iObjectB);
                break;
            case HS_MIXUP_FBOTTOM:
            case HS_MIXUP_LEGS:
            case HS_MIXUP_REAR:
                IncrementSection(hWnd, 3, &iObjectC);
                break;
                
            case HS_COVER:
				ShowCoverAnimatedButtons(hWnd, TRUE);
                hImageCtrl = GetDlgItem(hWnd, IDC_MIXUP_IMAGE);
                ShowWindow(hImageCtrl, SW_HIDE);
	            if (idCurrentRoom == RM_FASHION)
	            {
	                hCtl = GetDlgItem (hWnd,HS_LOGO_FASHION);
	                ShowWindow (hCtl,SW_SHOW);
	                hCtl = GetDlgItem (hWnd,HS_MIXUP_FHEAD);
	                ShowWindow (hCtl,SW_HIDE);
	                hCtl = GetDlgItem (hWnd,HS_MIXUP_FTOP);
	                ShowWindow (hCtl,SW_HIDE);
	                hCtl = GetDlgItem (hWnd,HS_MIXUP_FBOTTOM);
	                ShowWindow (hCtl,SW_HIDE);
	            }
	            else if (idCurrentRoom == RM_VEHICLE)
	            {
	                hCtl = GetDlgItem (hWnd,HS_LOGO_VEHICLE);
	                ShowWindow (hCtl,SW_SHOW);
	                hCtl = GetDlgItem (hWnd,HS_MIXUP_FRONT);
	                ShowWindow (hCtl,SW_HIDE);
	                hCtl = GetDlgItem (hWnd,HS_MIXUP_MIDDLE);
	                ShowWindow (hCtl,SW_HIDE);
	                hCtl = GetDlgItem (hWnd,HS_MIXUP_REAR);
	                ShowWindow (hCtl,SW_HIDE);
	            }
	            else if (idCurrentRoom == RM_MONSTER)
	            {
	                hCtl = GetDlgItem (hWnd,HS_LOGO_MONSTER);
	                ShowWindow (hCtl,SW_SHOW);
	                hCtl = GetDlgItem (hWnd,HS_MIXUP_HEAD);
	                ShowWindow (hCtl,SW_HIDE);
	                hCtl = GetDlgItem (hWnd,HS_MIXUP_BODY);
	                ShowWindow (hCtl,SW_HIDE);
	                hCtl = GetDlgItem (hWnd,HS_MIXUP_LEGS);
	                ShowWindow (hCtl,SW_HIDE);
	            }
	            bHandled = FALSE;
                break;
			case IDC_COVER_ANIMATE5:
				MessageBox(NULL, "Ray and Steve were here!", "CodeHeads", MB_OK);
				break;	
            case HS_TAB1:
            case HS_TAB2:
            case HS_TAB3:
            case HS_TAB4:
            case HS_TAB5:
            case HS_TAB6:
            case HS_TAB7:
            case HS_TAB8:
				ShowCoverAnimatedButtons(hWnd, FALSE);
                LastTab = wParam;
                switch ((DWORD)idCurrentRoom)
                {
                // Studio coloring books:
                case RM_FASHION:
	                hCtl = GetDlgItem (hWnd,HS_LOGO_FASHION);
	                ShowWindow (hCtl,SW_HIDE);
	                hCtl = GetDlgItem (hWnd,HS_MIXUP_FHEAD);
	                ShowWindow (hCtl,SW_SHOW);
	                hCtl = GetDlgItem (hWnd,HS_MIXUP_FTOP);
	                ShowWindow (hCtl,SW_SHOW);
	                hCtl = GetDlgItem (hWnd,HS_MIXUP_FBOTTOM);
	                ShowWindow (hCtl,SW_SHOW);
                    wsprintf (szTemp,GetString (IDS_FASHION, NULL),wParam - HS_TAB1 + 1);
                    break;
                case RM_VEHICLE:
	                hCtl = GetDlgItem (hWnd,HS_LOGO_VEHICLE);
	                ShowWindow (hCtl,SW_HIDE);
	                hCtl = GetDlgItem (hWnd,HS_MIXUP_FRONT);
	                ShowWindow (hCtl,SW_SHOW);
	                hCtl = GetDlgItem (hWnd,HS_MIXUP_MIDDLE);
	                ShowWindow (hCtl,SW_SHOW);
	                hCtl = GetDlgItem (hWnd,HS_MIXUP_REAR);
	                ShowWindow (hCtl,SW_SHOW);
                    wsprintf (szTemp,GetString (IDS_VEHICLE, NULL),wParam - HS_TAB1 + 1);
                    break;

                // Adventure coloring books:
                case RM_MONSTER:
	                hCtl = GetDlgItem (hWnd,HS_LOGO_MONSTER);
	                ShowWindow (hCtl,SW_HIDE);
	                hCtl = GetDlgItem (hWnd,HS_MIXUP_HEAD);
	                ShowWindow (hCtl,SW_SHOW);
	                hCtl = GetDlgItem (hWnd,HS_MIXUP_BODY);
	                ShowWindow (hCtl,SW_SHOW);
	                hCtl = GetDlgItem (hWnd,HS_MIXUP_LEGS);
	                ShowWindow (hCtl,SW_SHOW);
                    wsprintf (szTemp,GetString (IDS_MONSTER, NULL),wParam - HS_TAB1 + 1);
                    break;
                }
                {
                    BOOL    bLoad;
                    hImageCtrl = GetDlgItem(hWnd, IDC_MIXUP_IMAGE);
                    
                    if (bObjectsAdded)
                        UnlinkAllObjects();         // remove them from this image
                    
                    if (hImageCtrl)
                    {
                        PathCat (szPreview,Control.PouchPath,szTemp);
                        bLoad = AstralImageLoad( NULL, szPreview, TRUE, FALSE);
                        if (bLoad && lpImage)
                        {
                            SetWindowLong(hImageCtrl, GWL_IMAGE,
                                (long) ImgGetBase (lpImage));
                            if (!bObjectsAdded)
                                 AddAllObjects();
                            else LinkAllObjects();
                            ShowWindow(hImageCtrl, SW_SHOW);
                            AstralControlPaint(hWnd, IDC_MIXUP_IMAGE);
                        }
                    }

	            bHandled = FALSE;
                } // HS_TABS
                break;
                
            case HS_COLORME:
                GoPaintApp (hWnd,szPreview);
                break;
            default:
                bHandled = FALSE;
            }
            break;
        }
        
        case WM_INITDIALOG:
        {
            STRING sz;
            bObjectsAdded = FALSE;
            switch ((DWORD)idCurrentRoom)
            {
                case RM_FASHION:
                    lstrcpy(szTemp, GetString (IDS_FASHION_LIB, NULL));
                    break;
                case RM_VEHICLE:
                    lstrcpy(szTemp, GetString (IDS_VEHICLE_LIB, NULL));
                    break;
                case RM_MONSTER:
                    lstrcpy(szTemp, GetString (IDS_MIXUP_LIB, NULL));
                    break;
            }
            
            PathCat(szPreview,Control.PouchPath,szTemp);
            hMixLib = LoadLibrary(szPreview);
            if (hMixLib < (HINSTANCE)HINSTANCE_ERROR)
                hMixLib = 0;
                
            if (!LoadOSString(hMixLib, STR_MIXUP_ORIENT, sz, sizeof(sz)-1))
                 bVertOrientation = TRUE;
            else bVertOrientation = Ascii2Int(sz);
            
            LoadOffsets(ptAOffsets, 1);
            LoadOffsets(ptBOffsets, 2);
            LoadOffsets(ptCOffsets, 3);

//            PostMessage(hWnd, WM_COMMAND, LastTab, 0);
            bHandled = FALSE;   // allow DefRoomProc to be called
            break;
        }
        case WM_DESTROY:
        {
            if (hMixLib)
                FreeLibrary(hMixLib);
            hMixLib = NULL;
            
            if (bObjectsAdded)
                ImgCombineObjects(lpImage, TRUE, FALSE, TRUE);

            bHandled = FALSE;   // allow DefRoomProc to be called
            break;
        }
        default:
            bHandled = FALSE;
    } // switch (wMsg)
    
    if (!bHandled)
        bHandled = DefRoomProc (hWnd,wMsg,wParam,lParam);
    return bHandled;
}


LPOBJECT AddObjectToImage(int iSection, int iImage)
{
LPFRAME  lpImageFrame, lpMaskFrame;
LPMASK   lpMask;
int      xData;
int      yData;
RECT     rObject; 
RECT     rDisplay;
LPOBJECT lpObject;
char     szRes[10];

    if (!hMixLib) return(NULL);
    
    wsprintf(szRes, MIX_TIFF_RES_FORMAT, iSection-1+'A', iImage);
    if ( !(lpImageFrame = TiffResource2Frame(hMixLib, szRes)))
    	return( NULL );
            
    if ( !(lpMaskFrame = MaskCreateAlphaFrame( lpImageFrame)))
    	{
    	FrameClose( lpImageFrame );
    	return( NULL );
    	}
    if ( !(lpMask = MaskCreate( lpMaskFrame,0,0,ON,YES)))
    	{
    	FrameClose( lpImageFrame );
    	FrameClose( lpMaskFrame );
    	return( NULL );
    	}

    xData = FrameXSize(lpImageFrame);
    yData = FrameYSize(lpImageFrame);
    rDisplay = lpImage->lpDisplay->FileRect;
    rObject.left = ( rDisplay.left + rDisplay.right - xData ) / 2;
    rObject.top  = ( rDisplay.top + rDisplay.bottom - yData ) / 2;
    if (rObject.left < rDisplay.left)
        rObject.left = rDisplay.left;
    if (rObject.top < rDisplay.top)
        rObject.top = rDisplay.top;
    rObject.right = rObject.left + xData - 1;
    rObject.bottom = rObject.top + yData - 1;

    if ( !(lpObject = ObjCreateFromFrame( ST_TEMPORARY,
    		lpImageFrame,lpMask,&rObject, Control.NoUndo)) )
    	{
    	FrameClose( lpImageFrame );
    	MaskClose( lpMask );
    	return( NULL );
    	}

    LastObject[iSection-1] = lpObject;
    ImgAddNewObject( lpImage, lpObject );
    return( lpObject );
}


LPFRAME LoadImageFrame(int iSection, int iImage)
{
    LPFRAME  lpImageFrame;
    char     szRes[10];

    if (!hMixLib) return(NULL);
    
    wsprintf(szRes, MIX_TIFF_RES_FORMAT, iSection-1+'A', iImage);
    lpImageFrame = TiffResource2Frame(hMixLib, szRes);
    return lpImageFrame;
}


void RemoveLastObject(int iSection)
{
// Remove a single section from the image.
    LPOBJECT lpObject = LastObject[iSection-1];
    if (!lpObject) return;
    ImgDeleteObject(lpImage, lpObject);
    LastObject[iSection-1] = NULL;
}


void AddAllObjects()
{
// Place all the section objects on the image
    if (!AddObjectToImage(1, iObjectA))
    {
        iObjectA = 1;       // start over at 1
        AddObjectToImage(1, iObjectA);
    }

    if (!AddObjectToImage(2, iObjectB))
    {
        iObjectB = 1;       // start over at 1
        AddObjectToImage(2, iObjectB);
    }

    if (!AddObjectToImage(3, iObjectC))
    {
        iObjectC = 1;       // start over at 1
        AddObjectToImage(3, iObjectC);
    }

    RepositionSectionA();
    RepositionSectionB();
    RepositionSectionC();
    SetObjectOrder();
    
    bObjectsAdded = TRUE;
}





void RepositionSectionA()
{
	RECT r;
	int xPos, yPos;
	
	xPos = ptAOffsets[iObjectA-1].x;
	yPos = ptAOffsets[iObjectA-1].y;
	r = LastObject[0]->rObject;
	SetRect(&r, xPos, yPos, xPos + RectWidth(&r), yPos + RectHeight(&r));
	LastObject[0]->rObject = r;
}

void RepositionSectionB()
{
	RECT r;
	int xPos, yPos;
	
	xPos = ptBOffsets[iObjectB-1].x;
	yPos = ptBOffsets[iObjectB-1].y;
	r = LastObject[1]->rObject;
	SetRect(&r, xPos, yPos, xPos + RectWidth(&r), yPos + RectHeight(&r));
	LastObject[1]->rObject = r;
}

void RepositionSectionC()
{
	RECT r;
	int xPos, yPos;
	
	xPos = ptCOffsets[iObjectC-1].x;
	yPos = ptCOffsets[iObjectC-1].y;
	r = LastObject[2]->rObject;
	SetRect(&r, xPos, yPos, xPos + RectWidth(&r), yPos + RectHeight(&r));
	LastObject[2]->rObject = r;
}


void RepositionToCenter(int iSection)
{
    RECT rObj;
    RECT rDisplay;
    int  iWidth;
    int  iHeight;

    if (!LastObject[iSection-1]) return;
    if (!lpImage) return;

// Get size    
    rObj = LastObject[iSection-1]->rObject;
    iWidth  = rObj.right  - rObj.left;     
    iHeight = rObj.bottom - rObj.top;     
    rDisplay = lpImage->lpDisplay->FileRect;

// Move to center
    rObj.left   = (rDisplay.left + rDisplay.right  - iWidth ) / 2;
    rObj.top    = (rDisplay.top  + rDisplay.bottom - iHeight) / 2;
    rObj.right  = rObj.left + iWidth;
    rObj.bottom = rObj.top  + iHeight;
    
    LastObject[iSection-1]->rObject = rObj;
}



void IncrementSection(HWND hWnd, int iSection, int* iCounter)
{
// Step to the next available object for this section.
// For example, step through all the heads.
// The section is 1..NUM_SECTIONS, the counter is the image to step through.
    int iImage = *iCounter;
    iImage++;
    RemoveLastObject(iSection);
    if (!AddObjectToImage(iSection, iImage))
    {
        iImage = 1;       // start over at 1
        AddObjectToImage(iSection, iImage);
    }
    *iCounter = iImage;
    switch (iSection)
    {
        case 1: RepositionSectionA();   break;
        case 2: RepositionSectionB();   break;
        case 3: RepositionSectionC();   break;
    }
    SetObjectOrder();
    AstralControlPaint(hWnd, IDC_MIXUP_IMAGE);
}



void SetObjectOrder()
{
// Rearange the objects so that they overlap in the correct order.
    UnlinkAllObjects();
    LinkAllObjects();
}


void UnlinkAllObjects()
{
// Remove all the objects from this image  Do not delete the objects.
    int i;
    if (!lpImage) return;
    for (i=0; i<NUM_SECTIONS; i++)
    {
        if (LastObject[i])
            ObjUnlinkObject(&lpImage->ObjList, (LPPRIMOBJECT)LastObject[i]);
    } 
}


void LinkAllObjects()
{
// Add all the existing objects back into the image.
    int 	i;
	STRING	sz;
	int		iSection = -1;
	
    if (!hMixLib) return;
    if (!lpImage) return;
	
    for (i=0; i<NUM_SECTIONS; i++)
    {
		if (!LoadOSString(hMixLib, STR_MIXUP_ORDER+i, sz, sizeof(sz)-1))
			iSection++;
		else iSection = sz[0] - 'A';
		iSection = max(iSection, 0);
		iSection = min(iSection, NUM_SECTIONS-1);

        if (LastObject[iSection])
            ImgAddNewObject(lpImage, LastObject[iSection]);
    }
}


void LoadOffsets(LPPOINT lpPoint, int iSection)
{
#ifdef WIN32
    LPPOINTS    lpPoints;	// short points
#else
    LPPOINT     lpPoints;
#endif
    int         i;
    short       nPoints;
    char        szRes[10];
    HGLOBAL     hGlobal;
    LPSTR       lpGlobal;
    
    wsprintf(szRes, MIX_OFFS_RES_FORMAT, iSection-1+'A');

#ifdef _MAC
	hGlobal = (HGLOBAL)LoadMacResource(hMixLib, szRes, MIX_OFFS_RES_TYPE);
	if (!hGlobal) return;
	lpGlobal = *((LPSTR *)hGlobal);	// dereference the Mac loacked handle
	if (!lpGlobal)
	{
		ReleaseMacResource((HANDLE)hGlobal);
		return;
	}	
    if (nPoints = *(short far *)lpGlobal)
    {
        lpPoints = (LPPOINTS)(lpGlobal+2);
		// Swap these, the Mac POINT structure is backwards from Windows.
		for (i=0; i<nPoints; i++)
		{
			lpPoint[i].x = lpPoints[i].y;
			lpPoint[i].y = lpPoints[i].x;
		}
    }
	ReleaseMacResource(hGlobal);
	
#else
	// Windows version
		
    if ( !(hGlobal = (HGLOBAL)FindResource(hMixLib, szRes, MIX_OFFS_RES_TYPE)) )
		return;
    if ( !(hGlobal = LoadResource(hMixLib, (HRSRC)hGlobal)) )
		return;
	if ( !(lpGlobal = (LPSTR)LockResource( hGlobal )) )
	{
		FreeResource( hGlobal );
		return;
	}
    if ( nPoints = *(short far *)lpGlobal )
    {
#ifdef WIN32
        lpPoints = (LPPOINTS)(lpGlobal+2);
#else
        lpPoints = (LPPOINT)(lpGlobal+2);
#endif
        for (i=0; i<nPoints; i++)
		{
            lpPoint[i].x = lpPoints[i].x;
            lpPoint[i].y = lpPoints[i].y;
		}
    }
	UnlockResource( hGlobal );
    FreeResource( hGlobal );
#endif // _MAC
}


/****************************************************************************/

