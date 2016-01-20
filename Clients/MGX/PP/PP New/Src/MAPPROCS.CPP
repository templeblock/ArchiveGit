//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "visual.h"
#include "mapctl.h"
#include "gradctl.h"
#include "ppafx.h"
#include "svrdoc.h"
#include "svrview.h"
#include "ctxhelp.h"

#define PACKEDLUTSIZE (7L*256L)
#define GRIDSIZE 64
#define PREVIEW_TIMER 1
#define PREVIEW_WAIT 500

/***************************************************************************/
// for visaual contrast brigntness
typedef struct _cbstruct
{
	HWND hDlg;
	MAP *lpMap;
	WORD wChannel;
	WORD wBand;
	LPTR lpLuts;        // packed luts in order of id from IDC_VISUAL_00 to IDC_VISUAL_22
	LPTR lpBuffer;		// for display
}
CBSTRUCT, FAR *LPCBSTRUCT;

// for visual color balance
typedef struct _balancestruct
{
	HWND hDlg;
	LPTR lpLuts;		// packed lusts in order of id from IDC_VISUAL_00 to IDC_VISUAL_22
	LPTR lpBuffer;		// for display
	WORD wBand;
	BOOL bMaintainDensity;
}
BALANCESTRUCT, FAR *LPBALANCESTRUCT;

// Static prototypes
LOCAL MAP *ResetCBChannel( HWND hDlg, WORD wChannel, WORD wBand, int mul = 1);

// Common data used for all mapping
static BOOL bProbing = NO;
static int iBubbles;	// the ballon state before probing
static UINT bPreviewTimer = NO;
static int iCombo[8];		// for channels combo
static int nCombo;			// for channels combo
static WORD wBand;
BOOL IsInPreview = NULL;


// LOCAL prototypes
LOCAL BOOL Map_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Map_OnDestroy(HWND hDlg);
LOCAL void Map_OnTimer(HWND hwnd, UINT id);
LOCAL void Map_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Map_OnLButtonUp(HWND hDlg, int x, int y, UINT keyFlags);
LOCAL void Map_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL int order_points( LPPOINT lpPoints, int nPoints);
LOCAL void Map_ComputePoint( LPMAP lpMap, int i1, int i2, int i3, LFIXED rate );
LOCAL int Map_Get_Color(LPCOLORINFO lpColor, WORD wChannel);
LOCAL VOID Map_DlgSetNewMap(HWND hDlg, LPMAP lpMap);
LOCAL void map_to_mapinfo(LPMAP lpMap, LPMAPINFO lpInfo);
LOCAL void InitPreview(HWND hDlg);
LOCAL void DoPreview(HWND hDlg, BOOL f);

BOOL WINPROC EXPORT DlgMapOptionsProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );

// statics used for Map dialog
static LPMODIFYMAP_PARMS Map_lpParms;
static MAPVALUES Map_Values;

#define MAP_TO_DISPLAY(i) \
	(Mapping.Map_bPercent ? (TOPERCENT(i)) : i)
#define DISPLAY_TO_MAP(i) \
	(Mapping.Map_bPercent ? (TOGRAY(i)) : i)


/***********************************************************************/
BOOL WINPROC EXPORT DlgMapProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch (msg)
	{
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, Map_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_DESTROY, Map_OnDestroy);
		HANDLE_DLGMSG(hDlg, WM_MOUSEMOVE, Map_OnMouseMove);
		HANDLE_DLGMSG(hDlg, WM_LBUTTONUP, Map_OnLButtonUp);
		HANDLE_DLGMSG(hDlg, WM_COMMAND, Map_OnCommand);
		HANDLE_DLGMSG(hDlg, WM_TIMER, Map_OnTimer);

		HANDLE_DLGRET(hDlg, WM_QUERYNEWPALETTE, Dialog_OnQueryNewPalette);

		default:
		return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_MAP);
	}
}


/************************************************************************/
LOCAL BOOL Map_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	HWND hActiveWnd;
	FRMDATATYPE type;
	LPIMAGE lpImage;

	RightPopup( hDlg );
	Map_lpParms = (LPMODIFYMAP_PARMS)lParam;

	// set maps to defaults
	ResetAllMapsEx(NO);

	hActiveWnd = GetActiveDoc();
	if (hActiveWnd)
	{
		lpImage = GetImagePtr(hActiveWnd);
		type = FrameType(ImgGetBaseEditFrame(lpImage));
	}
	else
		type = FDT_RGBCOLOR;

	Map_Values.wChannel = IDC_DO_ALL;
	HandleMapInit(hDlg, &Map_Values, hActiveWnd, type, TRUE);
	return(TRUE);
}

/***************************************************************************/
LOCAL void Map_OnDestroy(HWND hDlg)
/***************************************************************************/
{
	HandleMapDestroy(hDlg);
}

/************************************************************************/
LOCAL void Map_OnTimer(HWND hDlg, UINT id)
/************************************************************************/
{	

	if (!AutoPreview_OnTimer(hDlg, id))
		return;
		
	// got good timer
	UpdateCurrentMap(hDlg, &Map_Values);
	MakeMap( Map_Values.pMap );
	MapPreview(GetActiveDoc());
}

/***********************************************************************/
LOCAL void Map_OnMouseMove(HWND hDlg, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	HandleMapMouseMove(hDlg, x, y, keyFlags, &Map_Values);
}

/***********************************************************************/
LOCAL void Map_OnLButtonUp(HWND hDlg, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	HandleMapLButtonUp(hDlg, x, y, keyFlags, &Map_Values);
}

/************************************************************************/
LOCAL void Map_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
	HWND hActiveWnd;
	LPIMAGE lpImage = NULL;

	if (hActiveWnd = GetActiveDoc())
		lpImage = GetImagePtr(hActiveWnd);

	switch (id)
	{
		case IDOK:
			// get the current map set
			UpdateCurrentMap(hDlg, &Map_Values);
			
			map_to_mapinfo(&MasterMap, &Map_lpParms->MasterInfo);
			map_to_mapinfo(&RedMap, &Map_lpParms->RedInfo);
			map_to_mapinfo(&GreenMap, &Map_lpParms->GreenInfo);
			map_to_mapinfo(&BlueMap, &Map_lpParms->BlueInfo);
			map_to_mapinfo(&BlackMap, &Map_lpParms->BlackInfo);
			
			HandleMapEnd(hDlg, hActiveWnd);
			AstralDlgEnd( hDlg, TRUE );
		break;

		case IDCANCEL:
			HandleMapEnd(hDlg, hActiveWnd);
			AstralDlgEnd( hDlg, FALSE );
		break;

		case IDC_MAPLOAD:
			if ( !AstralDlg( NO, PictPubApp.GetResourceHandle(), hDlg, IDD_MAPLOAD,
		 		DlgMapLoadProc ) )
				break;
			HandleMapLoad(hDlg, &Map_Values, hActiveWnd);
		break;
		
		case IDC_MAPSAVE:
			if ( !AstralDlg( NO, PictPubApp.GetResourceHandle(), hDlg, IDD_MAPSAVE,
		 		DlgMapSaveProc ) )
				break;
		break;

	   	default:
			if (HandleMapCommand(hDlg, id, codeNotify, &Map_Values, hActiveWnd))
				ControlEnable(hDlg, IDOK, Map_Values.fModified);
		break;
	}
}


/***************************************************************************/
static void Map_ComputePoint( LPMAP lpMap, int i1, int i2, int i3, LFIXED rate )
/***************************************************************************/
{
	lpMap->Pnt[i1].x = FMUL( lpMap->Pnt[i2].x, -rate+FUNITY )
					 + FMUL( lpMap->Pnt[i3].x, rate );
	lpMap->Pnt[i1].y = FMUL( lpMap->Pnt[i2].y, -rate+FUNITY )
					 + FMUL( lpMap->Pnt[i3].y, rate );
}


//************************************************************************
//		Draws the owner draw bar buttons for map dialog
//************************************************************************
LOCAL void Map_DrawBar(HWND hDlg, const DRAWITEMSTRUCT FAR * lpDrawItem)
//************************************************************************
{
	int      dx, dy, id;
	BITMAP   Data;
	HBITMAP  hBitmap;
	HDC      hDC, hSrcDC;
	HGDIOBJ  hOldBitmap;


	id = Mapping.Map_bIntensity ? IDC_BITMAP_BRIGHT1:IDC_BITMAP_BRIGHT2;
	hBitmap =LoadBitmap(PictPubApp.GetResourceHandle(),MAKEINTRESOURCE(id));

	if (!hBitmap)
		return;

	hDC = lpDrawItem->hDC;
	if ( !(hSrcDC = CreateCompatibleDC(hDC)) )
		return;

	hOldBitmap = SelectObject( hSrcDC, hBitmap );

	GetObject( hBitmap, sizeof(BITMAP), (LPSTR)&Data );

	dx = RectWidth(&lpDrawItem->rcItem)-1;
	dy = RectHeight(&lpDrawItem->rcItem)-1;

	StretchBlt( hDC, 
		lpDrawItem->rcItem.left, 
		lpDrawItem->rcItem.top, 
		dx, 
		dy, 
		hSrcDC, 
		0, 
		0, 
		Data.bmWidth,
		Data.bmHeight,
		SRCCOPY);
		
	SelectObject( hSrcDC, hOldBitmap );
	DeleteDC( hSrcDC );
	
		
	
}

//************************************************************************
//			Map_DlgSetNewMap
// PARAMETERS:
//					HWND hDlg-		The map dialog.
//					LPMAP lpMap- 	The map to base state on.
// DESCRIPTION:
//			Sets the gamma slider and the curves button based on lpMap.
//			Updates the map control as well.
//************************************************************************
LOCAL VOID Map_DlgSetNewMap(HWND hDlg, LPMAP lpMap)
//************************************************************************
{
	HWND hControl;
	double gamma;
	int iGammaSlide;
	BOOL bCurves;
	
	// because this code gets called for other dialogs via ResetMapChannel()
	// make sure that ancillary controls exist
	
	gamma = lpMap->gamma;
	bCurves = (lpMap->iCurves != 0);

	// gamma
	if (GetDlgItem(hDlg, IDC_MAP_GAMMA)) 
	{
		char szFixed[32];
		if (gamma < 1.0)
			iGammaSlide = (int)(200 - (gamma*9.0-1.0)*100./8.);
		else
			iGammaSlide = (int)(100 - (gamma-1.0)*5);
		SetSlide(hDlg, IDC_MAP_GAMMA, iGammaSlide);
		// set gamma value display
		if (gamma)
		{
			FixedAscii( DBL2FIX((1.0/gamma)), szFixed, 2 );
			SetDlgItemText( hDlg, IDC_MAP_GAMMA_TEXT, szFixed );
		}
	}

	// curves
	if (hControl = GetDlgItem(hDlg, IDC_MAP_CURVES))
	{
		SetWindowWord(hControl, GWW_ICONID, bCurves ? IDC_MAP_BEZIER:IDC_MAP_LINES);
		AstralControlRepaint(hDlg, IDC_MAP_CURVES);
	}

	// map control
	if (hControl = GetDlgItem( hDlg, IDC_MAP ))
	{
		Map_SetGamma(hControl, gamma);
		Map_SetCurveType(hControl, bCurves ? lpMap->iCurves:Mapping.Map_CurveType);
		Map_SetPointList(hControl, (LPPOINT)lpMap->Pnt, lpMap->Points);
		Map_SetStyle(hControl, MS_CURVES, bCurves);
//		bInvGamma = ((lpMap->Reverse & REVERSE_POINTSXY) || lpMap->Negate) &&
//					!((lpMap->Reverse & REVERSE_POINTSXY) && lpMap->Negate);
//		Map_SetStyle(hControl, MS_INVERT_GAMMA, bInvGamma);
	}
}


/***************************************************************************/
MAP *ResetMapChannel( HWND hDlg, WORD wChannel )
/***************************************************************************/
{
	Map_Values.wChannel = wChannel;
	return(ResetMapChannel(hDlg, &Map_Values));
}

/***************************************************************************/
MAP *ResetMapChannel( HWND hDlg, LPMAPVALUES lpValues )
/***************************************************************************/
{
	int i;
	
	lpValues->pMap = ActivateChannel( lpValues->wChannel );
	if (lpValues->wChannel == IDC_DO_ALL)
	{
		// master channel is always reversed in map dialogs
		int ReverseLUTXY = lpValues->pMap->Reverse & REVERSE_LUTXY;
		lpValues->pMap->Reverse = YES;
		lpValues->pMap->Reverse |= ReverseLUTXY;
	}
	
	Map_DlgSetNewMap(hDlg, lpValues->pMap);
	
	// copy lpMap's point to lpValues->ptNumericptNumeric, marking invalid points
	copy((LPTR)lpValues->pMap->Pnt, (LPTR)lpValues->ptNumeric, MAPPOINTS*sizeof(POINT));
	for (i=1; i<MAPPOINTS;i++)
	{
		if (lpValues->ptNumeric[i].x == 0)
			lpValues->ptNumeric[i].x = lpValues->ptNumeric[i].y = INT_MAX;
	}

#define INIT_MAPIN_SPIN(hWnd, id, index) \
		{InitDlgItemSpin(hWnd, id, 0, NO, 0, Mapping.Map_bPercent ? 100:255 ); \
		if (lpValues->ptNumeric[index].x == INT_MAX) SetDlgItemText(hWnd, id, ""); \
		else SetDlgItemSpin( hWnd, id, MAP_TO_DISPLAY(lpValues->ptNumeric[index].x), NO );} \

#define INIT_MAPOUT_SPIN(hWnd, id, index) \
		{InitDlgItemSpin(hWnd, id, 0, NO, 0, Mapping.Map_bPercent ? 100:255  ); \
		if (lpValues->ptNumeric[index].y == INT_MAX) SetDlgItemText(hWnd, id, ""); \
		else SetDlgItemSpin( hWnd, id, MAP_TO_DISPLAY(lpValues->ptNumeric[index].y), NO );} \
					
	if (GetDlgItem(hDlg, IDC_MAPIN))
	{
		SetDlgItemText( hDlg,  IDC_MAPIN,   " " );
		SetDlgItemText( hDlg, IDC_MAPOUT,  " " );

		INIT_MAPIN_SPIN(hDlg, IDC_MAP0IN,   0);
		INIT_MAPOUT_SPIN(hDlg, IDC_MAP0OUT, 0);
		INIT_MAPIN_SPIN(hDlg, IDC_MAP1IN,   1);
		INIT_MAPOUT_SPIN(hDlg, IDC_MAP1OUT, 1);
		INIT_MAPIN_SPIN(hDlg, IDC_MAP2IN,   2);
		INIT_MAPOUT_SPIN(hDlg, IDC_MAP2OUT, 2);
		INIT_MAPIN_SPIN(hDlg, IDC_MAP3IN,   3);
		INIT_MAPOUT_SPIN(hDlg, IDC_MAP3OUT, 3);
		INIT_MAPIN_SPIN(hDlg, IDC_MAP4IN,   4);
		INIT_MAPOUT_SPIN(hDlg, IDC_MAP4OUT, 4);
		INIT_MAPIN_SPIN(hDlg, IDC_MAP5IN,   5);
		INIT_MAPOUT_SPIN(hDlg, IDC_MAP5OUT, 5);
		INIT_MAPIN_SPIN(hDlg, IDC_MAP6IN,   6);
		INIT_MAPOUT_SPIN(hDlg, IDC_MAP6OUT, 6);
		INIT_MAPIN_SPIN(hDlg, IDC_MAP7IN,   7);
		INIT_MAPOUT_SPIN(hDlg, IDC_MAP7OUT, 7);
		INIT_MAPIN_SPIN(hDlg, IDC_MAP8IN,   8);
		INIT_MAPOUT_SPIN(hDlg, IDC_MAP8OUT, 8);
		INIT_MAPIN_SPIN(hDlg, IDC_MAP9IN,   9);
		INIT_MAPOUT_SPIN(hDlg, IDC_MAP9OUT, 9);
		INIT_MAPIN_SPIN(hDlg, IDC_MAP10IN,   10);
		INIT_MAPOUT_SPIN(hDlg, IDC_MAP10OUT, 10);
	}
									   
	return( lpValues->pMap );
}

/***************************************************************************/
LOCAL int Map_Get_Color(LPCOLORINFO lpColor, WORD wChannel)
/***************************************************************************/
{
	switch (wChannel)
	{
		case IDC_DO_RED:		
			return(lpColor->rgb.red);
		case IDC_DO_GREEN:	
			return(lpColor->rgb.green);
		case IDC_DO_BLUE:		
			return(lpColor->rgb.blue);
		case IDC_DO_CYAN:		
			return(lpColor->cmyk.c);
		case IDC_DO_MAGENTA:
			return(lpColor->cmyk.m);
		case IDC_DO_YELLOW:	
			return(lpColor->cmyk.y);
		case IDC_DO_BLACK:	
			return(lpColor->cmyk.k);
		case IDC_DO_ALL:
			return(lpColor->gray);
		default:
			return(lpColor->gray);
	}
	return(0);
}

// LOCAL prototypes
LOCAL BOOL MapOptions_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void MapOptions_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

static BOOL Map_bGrid;
static BOOL Map_bPercent;
static BOOL Map_nPoints;
static BOOL Map_bAuto;
static BOOL Map_CurveType;
 
/***********************************************************************/
// modifies Mapping.Map_bGrid,
//			Mapping.Map_bPercent,
//			Mapping.Map_nPoints.
//			Mapping.Map_bAuto.
/***********************************************************************/
BOOL WINPROC EXPORT DlgMapOptionsProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, MapOptions_OnInitDialog);
	HANDLE_DLGMSG(hDlg, WM_COMMAND, MapOptions_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_MAPLOAD);
	}
}

/************************************************************************/
LOCAL BOOL MapOptions_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	int id;
	
	CenterPopup( hDlg );
	
	Map_bGrid = 	Mapping.Map_bGrid;
	Map_bPercent = 	Mapping.Map_bPercent;
	Map_nPoints = 	Mapping.Map_nPoints;
	Map_bAuto = 	Mapping.Map_bAuto;
	Map_CurveType = 	Mapping.Map_CurveType;
	CheckDlgButton( hDlg, IDC_AUTO, Map_bAuto);
	CheckDlgButton( hDlg, IDC_MAP_GRID, Map_bGrid);
	CheckDlgButton( hDlg, IDC_MAP_PERCENT, Map_bPercent);
	InitDlgItemSpin(hDlg, IDC_MAP_NUM, Mapping.Map_nPoints, NO, 0, 11);
	if (Map_CurveType <= SMOOTH_EXSOFT)
		id = IDC_SMOOTH_EXSOFT;
	else if (Mapping.Map_CurveType <= SMOOTH_SOFT)
		id = IDC_SMOOTH_SOFT;
	else if (Mapping.Map_CurveType <= SMOOTH_NORMAL)
		id = IDC_SMOOTH_NORMAL;
	else if (Mapping.Map_CurveType <= SMOOTH_HARD)
		id = IDC_SMOOTH_HARD;	
	else // SMOOTH_EXHARD
		id = IDC_SMOOTH_EXHARD;
	CheckComboItem( hDlg, IDC_CURVE_SMOOTH, IDC_SMOOTH_FIRST, IDC_SMOOTH_LAST,id);
	
	return(TRUE);
}

/************************************************************************/
LOCAL void MapOptions_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
	int i;
	BOOL bool;
	
	switch (id)
	{
		case IDC_MAPSAVE:
			if ( !AstralDlg( NO, PictPubApp.GetResourceHandle(), hDlg, IDD_MAPSAVE,
		 		DlgMapSaveProc ) )
				break;
		break;
		
		case IDC_MAPLOAD:
			if ( !AstralDlg( NO, PictPubApp.GetResourceHandle(), hDlg, IDD_MAPLOAD,
		 		DlgMapLoadProc ) )
				break;
			Map_Values.pMap = ResetMapChannel( hDlg, Map_Values.wChannel );
		break;
		
		case IDC_MAP_NUM:
			i = GetDlgItemSpin(hDlg, id, &bool, NO);
			if (bool)
				Map_nPoints = Bound(i,2,11);
		break; 
		
		case IDC_MAP_GRID:
			Map_bGrid = !Map_bGrid;
			CheckDlgButton( hDlg, id, Map_bGrid);
		break;
		
		case IDC_MAP_PERCENT:
			Map_bPercent = !Map_bPercent;
			CheckDlgButton( hDlg, id, Map_bPercent);
		break;
	
		case IDC_AUTO:
			Map_bAuto = !Map_bAuto;
			CheckDlgButton( hDlg, id, Map_bAuto);
		break;
		
		case IDC_CURVE_SMOOTH:
			if ( !(i = HandleCombo( hDlg, id, codeNotify )) )
				break;
			switch(i)
			{	
				case IDC_SMOOTH_EXSOFT:
					Map_CurveType = SMOOTH_EXSOFT;
				break;
				case IDC_SMOOTH_SOFT:
					Map_CurveType = SMOOTH_SOFT;
				break;
				case IDC_SMOOTH_NORMAL:
					Map_CurveType = SMOOTH_NORMAL;
				break;
				case IDC_SMOOTH_HARD:
					Map_CurveType = SMOOTH_HARD;
				break;
				case IDC_SMOOTH_EXHARD:
					Map_CurveType = SMOOTH_EXHARD;
				break;
			}
		break;

		case IDOK:
			Mapping.Map_bGrid = Map_bGrid;
			Mapping.Map_bPercent = Map_bPercent;
			Mapping.Map_nPoints = Map_nPoints;
			Mapping.Map_bAuto = Map_bAuto;
			Mapping.Map_CurveType = Map_CurveType;
			AstralDlgEnd( hDlg, TRUE );
		break;

		case IDCANCEL:
			AstralDlgEnd( hDlg, FALSE );
		break;

	   default:
		break;
	}
}


//************************************************************************
//			map_to_mapinfo
// DESCRIPTION:
//			Copies the relevent data from the given MAP structure 
//			into the given MAPINFO structure.
//************************************************************************
LOCAL void map_to_mapinfo(LPMAP lpMap, LPMAPINFO lpInfo)
//************************************************************************
{
	lpInfo->Reverse = lpMap->Reverse; 
	lpInfo->iCurves = lpMap->iCurves; 
	lpInfo->gamma = lpMap->gamma; 
	lpInfo->Points = lpMap->Points; 
	copy((LPTR)&lpMap->Pnt, (LPTR)&lpInfo->Pnt, sizeof(lpInfo->Pnt));
}

// LOCAL prototypes
LOCAL BOOL ContBrit_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void ContBrit_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void ContBrit_OnTimer(HWND hwnd, UINT id);
LOCAL void ContBrit_OnDestroy(HWND hDlg);

static LPCONTBRIT_PARMS ContBrit_lpParms;

/***********************************************************************/
BOOL WINPROC EXPORT DlgContBritProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, ContBrit_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, ContBrit_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_TIMER, ContBrit_OnTimer);
	HANDLE_DLGMSG(hDlg, WM_DESTROY, ContBrit_OnDestroy);

	HANDLE_DLGRET(hDlg, WM_QUERYNEWPALETTE, Dialog_OnQueryNewPalette);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_CONTBRIT_STICK);
	}
}

/************************************************************************/
LOCAL BOOL ContBrit_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
RightPopup( hDlg );
ContBrit_lpParms = (LPCONTBRIT_PARMS)lParam;
ResetAllMaps();
Map_Values.wChannel = IDC_DO_ALL;
//CheckComboItem( hDlg, IDC_CHANNELS, IDC_DO_ALL, IDC_DO_YELLOW,
//	Map_Values.wChannel = IDC_DO_ALL );
//
//if ( FrameDepth(ImgGetBaseEditFrame(lpImage)) == 1 )
//{
//	ControlEnable( hDlg, IDC_DO_RED, NO );
//	ControlEnable( hDlg, IDC_DO_GREEN, NO );
//	ControlEnable( hDlg, IDC_DO_BLUE, NO );
//	ControlEnable( hDlg, IDC_DO_CYAN, NO );
//		ControlEnable( hDlg, IDC_DO_MAGENTA, NO );
//	ControlEnable( hDlg, IDC_DO_YELLOW, NO );
//}
wBand = BAND_ALL;
CheckComboItem( hDlg, IDC_BAND, IDC_BAND_ALL, IDC_BAND_LO,
	wBand + IDC_BAND_ALL );

CheckDlgButton( hDlg, IDC_AUTO, Mapping.ContBrit_bAuto);
ControlEnable(hDlg, IDC_PREVIEW, !Mapping.ContBrit_bAuto);

ControlEnable( hDlg, IDC_CONTRAST, wBand == BAND_ALL);
SetWordBit(GetDlgItem( hDlg, IDC_CONTBRIT ), GWL_STYLE, (WORD)ST_HONLY, wBand != BAND_ALL);

Map_Values.pMap = ResetCBChannel( hDlg, Map_Values.wChannel, wBand );
return(TRUE);
}

/***************************************************************************/
LOCAL void ContBrit_OnDestroy(HWND hDlg)
/***************************************************************************/
{
	AutoPreview_OnDestroy(hDlg);
	Dialog_OnDestroy(hDlg);
}

/************************************************************************/
LOCAL void ContBrit_OnTimer(HWND hDlg, UINT id)
/************************************************************************/
{	
	if (!AutoPreview_OnTimer(hDlg, id))
		return;
		
	// got good timer		   
	MakeMap(Map_Values.pMap);
	MapPreview(GetActiveDoc());
}

/************************************************************************/
LOCAL void ContBrit_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
BOOL Bool;
int i;
int x, y;
HWND hActiveWnd;
LPIMAGE lpImage = NULL;

if (hActiveWnd = GetActiveDoc())
	lpImage = GetImagePtr(hActiveWnd);

switch (id)
	{
//	case IDC_CHANNELS:
//		if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
//			break;
//		Map_Values.wChannel = id;
//		Map_Values.pMap = ResetCBChannel( hDlg, Map_Values.wChannel, wBand );
//	break;


	case IDC_BAND:
		if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
			break;
		// setup new map
		wBand = id - IDC_BAND_ALL;
		ControlEnable( hDlg, IDC_CONTRAST, wBand == BAND_ALL);
		SetWordBit(GetDlgItem( hDlg, IDC_CONTBRIT ), GWL_STYLE, (WORD)ST_HONLY, wBand != BAND_ALL);
		Map_Values.pMap = ResetCBChannel( hDlg, Map_Values.wChannel, wBand );
	break;

	case IDC_AUTO:
		Mapping.ContBrit_bAuto = !Mapping.ContBrit_bAuto;
		CheckDlgButton( hDlg, id, Mapping.ContBrit_bAuto);
		ControlEnable(hDlg, IDC_PREVIEW, !Mapping.ContBrit_bAuto);
		AutoPreview(Map_Values.pMap, Mapping.ContBrit_bAuto, YES, hDlg, hActiveWnd);
	break; 
	
	case IDC_PREVIEW:
	  	AutoPreview_Button(hActiveWnd, Map_Values.pMap);
	break;

	case IDC_RESETALL:
	case IDC_RESET:
		// reset all bands
		ResetMapEx( &MasterMap, MAPPOINTS, -1, YES );
		Map_Values.pMap = ResetCBChannel( hDlg, Map_Values.wChannel, wBand );
		AutoPreview(Map_Values.pMap, Mapping.ContBrit_bAuto, YES, hDlg, hActiveWnd);
	break;

//	case IDC_RESET:
//		// reset this band
//		Map_Values.pMap->Contrast = 0;
//		Map_Values.pMap->Brightness[wBand] = 0;
//		Map_Values.pMap = ResetCBChannel( hDlg, Map_Values.wChannel, wBand );
//		AnimateMap(hActiveWnd, Map_Values.pMap, YES);
//		if (Previewing(hActiveWnd))
//			MapPreview(hActiveWnd);
//	break;

	case IDOK:
		if ( !lpImage )
		{
			AstralDlgEnd( hDlg, TRUE );
			break;
		}
		if (Map_Values.pMap->Reverse)
			InvertMap(Map_Values.pMap);
		ContBrit_lpParms->Contrast = Map_Values.pMap->Contrast;
		for (i = 0; i < NUM_BANDS; ++i)
			ContBrit_lpParms->Brightness[i] = Map_Values.pMap->Brightness[i];
		UnAutoPreview(hDlg, hActiveWnd);
		AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
		if ( !lpImage )
		{
			AstralDlgEnd( hDlg, FALSE );
			break;
		}
		ResetAllMaps();
		UnAutoPreview(hDlg, hActiveWnd);
		AstralDlgEnd( hDlg, FALSE );
	break;

	case IDC_CONTBRIT:
		if ( GetFocus() != GetDlgItem( hDlg, IDC_CONTBRIT ) )
			break;
		Stick_GetValue(GetDlgItem(hDlg, IDC_CONTBRIT), &x, &y);
		Map_Values.pMap->Brightness[wBand] = x;
		Map_Values.pMap->Contrast = y;
		SetDlgItemSpin( hDlg, IDC_CONTRAST,
			Map_Values.pMap->Contrast, YES );
		SetDlgItemSpin( hDlg, IDC_BRIGHTNESS,
			Map_Values.pMap->Brightness[wBand], YES );
		AutoPreview(Map_Values.pMap, Mapping.ContBrit_bAuto, (codeNotify==SCN_STICK_CHANGED), hDlg, hActiveWnd);
	break;

	case IDC_CONTRAST:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		Map_Values.pMap->Contrast = (int)GetDlgItemSpin( hDlg, IDC_CONTRAST,
			&Bool, YES );
		Stick_SetValue(GetDlgItem(hDlg, IDC_CONTBRIT),
					Map_Values.pMap->Brightness[wBand],
					Map_Values.pMap->Contrast);
		AstralControlRepaint( hDlg, IDC_CONTBRIT );
		AutoPreview(Map_Values.pMap, Mapping.ContBrit_bAuto, NO, hDlg, hActiveWnd);
	break;

	case IDC_BRIGHTNESS:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		Map_Values.pMap->Brightness[wBand] = (int)GetDlgItemSpin( hDlg,
			IDC_BRIGHTNESS,	&Bool, YES );
		Stick_SetValue(GetDlgItem(hDlg, IDC_CONTBRIT),
					Map_Values.pMap->Brightness[wBand],
					Map_Values.pMap->Contrast);
		AstralControlRepaint( hDlg, IDC_CONTBRIT );
		AutoPreview(Map_Values.pMap, Mapping.ContBrit_bAuto, NO, hDlg, hActiveWnd);
	break;

   default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL Balance_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Balance_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void Balance_Set_Gradient(HWND hDlg, WORD wChannel);
LOCAL void Balance_OnTimer(HWND hwnd, UINT id);
LOCAL void Balance_OnDestroy(HWND hDlg);

static LPBALANCE_PARMS Balance_lpParms;

/***********************************************************************/
BOOL WINPROC EXPORT DlgBalanceProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Balance_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Balance_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_TIMER, Balance_OnTimer);
	HANDLE_DLGMSG(hDlg, WM_DESTROY, Balance_OnDestroy);

	HANDLE_DLGRET(hDlg, WM_QUERYNEWPALETTE, Dialog_OnQueryNewPalette);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_COLORBAL_STICK);
	}
}

/************************************************************************/
LOCAL BOOL Balance_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
LPIMAGE lpImage;
FRMDATATYPE type;
int			mul;

lpImage = GetActiveImage();

RightPopup( hDlg );
Balance_lpParms = (LPBALANCE_PARMS)lParam;

ResetAllMaps();

type = FrameType(ImgGetBaseEditFrame(lpImage));
Map_Values.wChannel = IDC_DO_ALL;
ChannelsCombo_Setup(hDlg, IDC_CHANNELS, type, Map_Values.wChannel);

wBand = BAND_ALL;
CheckComboItem( hDlg, IDC_BAND, IDC_BAND_ALL, IDC_BAND_LO,
	wBand + IDC_BAND_ALL );

CheckDlgButton( hDlg, IDC_AUTO, Mapping.Balance_bAuto);
ControlEnable(hDlg, IDC_PREVIEW, !Mapping.Balance_bAuto);

ControlEnable( hDlg, IDC_CONTRAST, wBand == BAND_ALL);
SetWordBit(GetDlgItem( hDlg, IDC_CONTBRIT ), GWL_STYLE, (WORD)ST_HONLY, wBand != BAND_ALL);

if ( FrameDepth(ImgGetBaseEditFrame(lpImage)) == 1 )
	{
	ControlEnable( hDlg, IDC_DO_RED, NO );
	ControlEnable( hDlg, IDC_DO_GREEN, NO );
	ControlEnable( hDlg, IDC_DO_BLUE, NO );
	ControlEnable( hDlg, IDC_DO_CYAN, NO );
	ControlEnable( hDlg, IDC_DO_MAGENTA, NO );
	ControlEnable( hDlg, IDC_DO_YELLOW, NO );
	}
Balance_Set_Gradient(hDlg, Map_Values.wChannel);
mul = (Map_Values.wChannel == IDC_DO_ALL) ? 1:-1;
Map_Values.pMap = ResetCBChannel( hDlg, Map_Values.wChannel, wBand, mul);
return(TRUE);
}

/************************************************************6***************/
LOCAL void Balance_OnDestroy(HWND hDlg)
/***************************************************************************/
{
	AutoPreview_OnDestroy(hDlg);
	Dialog_OnDestroy(hDlg);
}

/************************************************************************/
LOCAL void Balance_OnTimer(HWND hDlg, UINT id)
/************************************************************************/
{	
	if (!AutoPreview_OnTimer(hDlg, id))
		return;
		
	// got good timer		   
	MakeMap(Map_Values.pMap);
	MapPreview(GetActiveDoc());
}


/************************************************************************/
LOCAL void Balance_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
BOOL Bool;
int Contrast;
int mul;
int i, x, y;
HWND hActiveWnd;
LPIMAGE lpImage = NULL;

if (hActiveWnd = GetActiveDoc())
	lpImage = GetImagePtr(hActiveWnd);

switch (id)
	{
	case IDC_BAND:
		if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
			break;
		// setup new map
		wBand = id- IDC_BAND_ALL;
		ControlEnable( hDlg, IDC_CONTRAST, wBand == BAND_ALL);
		SetWordBit(GetDlgItem( hDlg, IDC_CONTBRIT ), GWL_STYLE, (WORD)ST_HONLY, wBand != BAND_ALL);
		mul = (Map_Values.wChannel == IDC_DO_ALL) ? 1:-1;
		Map_Values.pMap = ResetCBChannel( hDlg, Map_Values.wChannel, wBand, mul);
	break;
	
	case IDC_CHANNELS:
		if (!(i=ChannelsCombo_Handle(hDlg, id, codeNotify)))
			break;
		Map_Values.wChannel = i;
		Balance_Set_Gradient(hDlg, Map_Values.wChannel);	 
		mul = (Map_Values.wChannel == IDC_DO_ALL) ? 1:-1;
		Map_Values.pMap = ResetCBChannel( hDlg, Map_Values.wChannel, wBand, mul);
	break;

	case IDC_AUTO:
		Mapping.Balance_bAuto = !Mapping.Balance_bAuto;
		CheckDlgButton( hDlg, id, Mapping.Balance_bAuto);
		ControlEnable(hDlg, IDC_PREVIEW, !Mapping.Balance_bAuto);
		AutoPreview(Map_Values.pMap, Mapping.Balance_bAuto, YES, hDlg, hActiveWnd);
	break; 
	
	case IDC_PREVIEW:
	  	AutoPreview_Button(hActiveWnd, Map_Values.pMap);
	break;

	case IDC_RESETALL:
		ResetAllMaps();
		mul = (Map_Values.wChannel == IDC_DO_ALL) ? 1:-1;
		Map_Values.pMap = ResetCBChannel( hDlg, Map_Values.wChannel, wBand, mul);
		AutoPreview(Map_Values.pMap, Mapping.Balance_bAuto, YES, hDlg, hActiveWnd);
	break;

	case IDC_RESET:
		ResetMapEx( Map_Values.pMap, MAPPOINTS, -1, Map_Values.wChannel == IDC_DO_ALL );
		mul = (Map_Values.wChannel == IDC_DO_ALL) ? 1:-1;
		Map_Values.pMap = ResetCBChannel( hDlg, Map_Values.wChannel, wBand, mul);
		AutoPreview(Map_Values.pMap, Mapping.Balance_bAuto, YES, hDlg, hActiveWnd);
	break;

	case IDOK:
		if ( !lpImage )
		{
			AstralDlgEnd( hDlg, TRUE );
			break;
		}
		UndoMapPreview(hActiveWnd/*FALSE*/);

		Balance_lpParms->MasterContrast = MasterMap.Contrast;
		for (i = 0; i < 4; ++i)
			Balance_lpParms->MasterBrightness[i] = MasterMap.Brightness[i];
		Balance_lpParms->fMasterReverse = MasterMap.Reverse;
		Balance_lpParms->RedContrast = RedMap.Contrast;
		for (i = 0; i < 4; ++i)
			Balance_lpParms->RedBrightness[i] = RedMap.Brightness[i];
		Balance_lpParms->fRedReverse = RedMap.Reverse;
		Balance_lpParms->GreenContrast = GreenMap.Contrast;
		for (i = 0; i < 4; ++i)
			Balance_lpParms->GreenBrightness[i] = GreenMap.Brightness[i];
		Balance_lpParms->fGreenReverse = GreenMap.Reverse;
		Balance_lpParms->BlueContrast = BlueMap.Contrast;
		for (i = 0; i < 4; ++i)
			Balance_lpParms->BlueBrightness[i] = BlueMap.Brightness[i];
		Balance_lpParms->fBlueReverse = BlueMap.Reverse;
		Balance_lpParms->BlackContrast = BlackMap.Contrast;
		for (i = 0; i < 4; ++i)
			Balance_lpParms->BlackBrightness[i] = BlackMap.Brightness[i];
		Balance_lpParms->fBlackReverse = BlackMap.Reverse;

//		ApplyMap(IDS_UNDOCOLORBAL);
		ResetAllMaps();
		UnAutoPreview(hDlg, hActiveWnd);
		AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
		if ( !lpImage )
		{
			AstralDlgEnd( hDlg, FALSE );
			break;
		}
		ResetAllMaps();
		UnAutoPreview(hDlg, hActiveWnd);
		AstralDlgEnd( hDlg, FALSE );
	break;

	case IDC_CONTBRIT:
		if ( GetFocus() != GetDlgItem( hDlg, IDC_CONTBRIT ) )
			break;
		Stick_GetValue(GetDlgItem(hDlg, IDC_CONTBRIT), &x, &y);
		mul = (Map_Values.wChannel == IDC_DO_ALL) ? 1:-1;
	   	Map_Values.pMap->Brightness[wBand] = mul*x;
		if (wBand == BAND_ALL)
		{
			Contrast = Map_Values.pMap->Contrast;
			Map_Values.pMap->Contrast = y;
			SetDlgItemSpin( hDlg, IDC_CONTRAST,
				Map_Values.pMap->Contrast, YES );
		}
		else
      {
			Contrast = 0;
		   Stick_SetValue(GetDlgItem(hDlg, IDC_CONTBRIT),
					Map_Values.pMap->Brightness[wBand]*mul,
					Contrast);
      }
		SetDlgItemSpin( hDlg, IDC_BRIGHTNESS,
			mul*Map_Values.pMap->Brightness[wBand], YES );
		AutoPreview(Map_Values.pMap, Mapping.Balance_bAuto, (codeNotify==SCN_STICK_CHANGED), hDlg, hActiveWnd);
	break;

	case IDC_CONTRAST:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		Map_Values.pMap->Contrast = (int)GetDlgItemSpin( hDlg, IDC_CONTRAST,
			&Bool, YES );
		mul = (Map_Values.wChannel == IDC_DO_ALL) ? 1:-1;
		Stick_SetValue(GetDlgItem(hDlg, IDC_CONTBRIT),
					mul*Map_Values.pMap->Brightness[wBand],
					Map_Values.pMap->Contrast);
		AstralControlRepaint( hDlg, IDC_CONTBRIT );
		AutoPreview(Map_Values.pMap, Mapping.Balance_bAuto, NO, hDlg, hActiveWnd);
	break;

	case IDC_BRIGHTNESS:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		mul = (Map_Values.wChannel == IDC_DO_ALL) ? 1:-1;
		Map_Values.pMap->Brightness[wBand] = mul*(int)GetDlgItemSpin( hDlg,
			IDC_BRIGHTNESS,	&Bool, YES );
		if (wBand == BAND_ALL)
			Contrast = Map_Values.pMap->Contrast;
		else
			Contrast = 0;
		Stick_SetValue(GetDlgItem(hDlg, IDC_CONTBRIT),
					Map_Values.pMap->Brightness[wBand]*mul,
					Contrast);
		AstralControlRepaint( hDlg, IDC_CONTBRIT );
		AutoPreview(Map_Values.pMap, Mapping.Balance_bAuto, NO, hDlg, hActiveWnd);
	break;

   default:
	break;
   }
}


//************************************************************************
//		Balance_Set_Gradient.
// DESCRIPTION:
//		sets the given gradient control based on the channel.
//************************************************************************
LOCAL void Balance_Set_Gradient(HWND hDlg, WORD wChannel)
//************************************************************************
{
	HWND hBar;
	RGBS rgb1;
	RGBS rgb2;

	switch (wChannel)
	{
		case IDC_DO_RED:		
			rgb2.red = 0;
			rgb2.green = rgb2.blue = 255;
			rgb1.red = 255;
			rgb1.green = rgb1.blue = 0;
		break;
		case IDC_DO_GREEN:	
			rgb2.green = 0; 
			rgb2.red = rgb2.blue = 255;
			rgb1.green = 255;
			rgb1.red = rgb1.blue = 0;
		break;
		case IDC_DO_BLUE:		
			rgb2.red = rgb2.green = 255;
			rgb2.blue = 0;
			rgb1.red = rgb1.green = 0;
			rgb1.blue = 255;
		break;
		case IDC_DO_CYAN:		
			rgb2.red = 255;
			rgb2.green = rgb2.blue = 0;
			rgb1.red = 0;
			rgb1.green = rgb1.blue = 255;
		break;
		case IDC_DO_MAGENTA:
			rgb2.green = 255;
			rgb2.red = rgb2.blue = 0;
			rgb1.red = rgb1.blue = 255;
			rgb1.green = 0; 
		break;
		case IDC_DO_YELLOW:	
			rgb2.blue = 255;
			rgb2.red = rgb2.green = 0;
			rgb1.red = rgb1.green = 255;
			rgb1.blue = 0;
		break;
		case IDC_DO_BLACK:	
			rgb2.red = rgb2.green = rgb2.blue = 255;
			rgb1.red = rgb1.green = rgb1.blue = 0;
		break;
		case IDC_DO_ALL:
		default:
			rgb2.red = rgb2.green = rgb2.blue = 0;
			rgb1.red = rgb1.green = rgb1.blue = 255;
		break;
	}
	
	if (hBar = GetDlgItem(hDlg, IDC_HISTO_GRAD))
	{
		Gradient_SetRGB(hBar, &rgb2, &rgb1);
		Gradient_SetType(hBar, GT_RGB);																	  
	}
}

// LOCAL prototypes
LOCAL void EnableContrastVisual(HWND hDlg, BOOL bEnable);
LOCAL BOOL ContBritVisual_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void ContBritVisual_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void ContBritVisual_OnDestroy(HWND hDlg);
LOCAL void ContBritVisual_Draw(HDC, LPRECT,int,int,struct _visual FAR *);
LOCAL BOOL ContBritVisual_Move(int,int,struct _visual FAR *);
LOCAL BOOL ContBritVisual_GetNew(int x ,int y, LPINT cont, LPINT brit, LPVISUAL lpVisual);

static VISUAL ContBritVisual_visual;
static CBSTRUCT ContBritVisual_Data;
static LPCONTBRIT_PARMS ContBritVisual_lpParms;

/***********************************************************************/
BOOL WINPROC EXPORT DlgContBritVisualProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, ContBritVisual_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, ContBritVisual_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DESTROY, ContBritVisual_OnDestroy);

	HANDLE_DLGRET(hDlg, WM_QUERYNEWPALETTE, Dialog_OnQueryNewPalette);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_CONTBRIT_VISUAL);
	}
}

/************************************************************************/
LOCAL BOOL ContBritVisual_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
LPIMAGE lpImage;
FRMDATATYPE type;

InitPreview(hDlg);
lpImage = GetActiveImage();
CenterPopup( hDlg );
ContBritVisual_lpParms = (LPCONTBRIT_PARMS)lParam;

ResetAllMaps();

type = FrameType(ImgGetBaseEditFrame(lpImage));
ContBritVisual_Data.wChannel = IDC_DO_ALL;
ChannelsCombo_Setup(hDlg, IDC_CHANNELS, type, ContBritVisual_Data.wChannel);

ContBritVisual_Data.wBand = BAND_ALL;
EnableContrastVisual(hDlg, ContBritVisual_Data.wBand == BAND_ALL);
CheckComboItem( hDlg, IDC_BAND, IDC_BAND_ALL, IDC_BAND_LO,
	ContBritVisual_Data.wBand+IDC_BAND_ALL );

if ( FrameDepth(ImgGetBaseEditFrame(lpImage)) == 1 )
	{
	ControlEnable( hDlg, IDC_DO_RED, NO );
	ControlEnable( hDlg, IDC_DO_GREEN, NO );
	ControlEnable( hDlg, IDC_DO_BLUE, NO );
	ControlEnable( hDlg, IDC_DO_CYAN, NO );
	ControlEnable( hDlg, IDC_DO_MAGENTA, NO );
	ControlEnable( hDlg, IDC_DO_YELLOW, NO );
	}
ContBritVisual_Data.hDlg = hDlg;
ContBritVisual_Data.lpMap = ActivateChannel(ContBritVisual_Data.wChannel);
ContBritVisual_Data.lpBuffer = NULL;
ContBritVisual_Data.lpLuts = Alloc(PACKEDLUTSIZE*9);
if (!ContBritVisual_Data.lpLuts)
	{
	Message(IDS_EMEMALLOC);
	AstralDlgEnd( hDlg, FALSE);
	return(TRUE);
	}
ContBritVisual_visual.lpImage = lpImage;
ContBritVisual_visual.lpOrgFrame = NULL;
ContBritVisual_visual.lpBasisFrame = NULL;
ContBritVisual_visual.lpUser = (LPTR)&ContBritVisual_Data;
ContBritVisual_visual.step = 5;
ContBritVisual_visual.moveType = 1;	// 2-d directional
ContBritVisual_visual.pDrawVisual = ContBritVisual_Draw;
ContBritVisual_visual.pMoveVisual = ContBritVisual_Move;
VisualMap_OnInitDialog(hDlg, hWndFocus, lParam, &ContBritVisual_visual);
return(TRUE);
}

/************************************************************************/
LOCAL void ContBritVisual_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
int i;
HWND hActiveWnd;
LPIMAGE lpImage = NULL;

if (hActiveWnd = GetActiveDoc())
	lpImage = GetImagePtr(hActiveWnd);

switch (id)
	{
	case IDC_CHANNELS:
      DoPreview(hDlg, FALSE);
		if (!(i=ChannelsCombo_Handle(hDlg, id, codeNotify)))
			break;
			
		// clear old map to the center
		MakeMap(ContBritVisual_Data.lpMap );

		// setup new map
		ContBritVisual_Data.wChannel = i;
		ContBritVisual_Data.lpMap = ActivateChannel(ContBritVisual_Data.wChannel );
		SetVisual(hDlg,0,0,&ContBritVisual_visual);
	break;
	
	case IDC_BAND:
      DoPreview(hDlg, FALSE);
		if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
			break;
		// setup new map
		if (ContBritVisual_Data.wBand != id - IDC_BAND_ALL)
		{
			ContBritVisual_Data.wBand = id - IDC_BAND_ALL;
			EnableContrastVisual(hDlg, ContBritVisual_Data.wBand == BAND_ALL);
			SetVisual(hDlg,0,0,&ContBritVisual_visual);
		}
	break;
	
	case IDC_PREVIEW:
		// clear map to the center
      DoPreview(hDlg, !IsInPreview);
		MakeMap(ContBritVisual_Data.lpMap );
		MapPreview(hActiveWnd);
	   break;

	case IDOK:
		if ( lpImage )
		{
			// clear old map to the center
			MakeMap(ContBritVisual_Data.lpMap );
			if (ContBritVisual_Data.lpMap->Reverse)
				InvertMap(ContBritVisual_Data.lpMap);
			ContBritVisual_lpParms->Contrast = ContBritVisual_Data.lpMap->Contrast;
			for (i = 0; i < NUM_BANDS; ++i)
				ContBritVisual_lpParms->Brightness[i] = ContBritVisual_Data.lpMap->Brightness[i];
//			ApplyMap(IDS_UNDOCONTBRIT);
			ResetAllMaps();
			UndoMapPreview(hActiveWnd/*FALSE*/);
			AnimateMap(hActiveWnd, NULL, YES);
		}
		AstralDlgEnd( hDlg, TRUE);
	break;

	case IDCANCEL:
		ResetAllMaps();
		UndoMapPreview(hActiveWnd/*FALSE*/);
		AstralDlgEnd( hDlg, FALSE);
	break;

   default:
      DoPreview(hDlg, FALSE);
		VisualMap_OnCommand(hDlg, id, hControl, codeNotify,
							&ContBritVisual_visual);
	break;
   }
}

/***************************************************************************/
LOCAL void ContBritVisual_OnDestroy(HWND hDlg)
/***************************************************************************/
{
if (ContBritVisual_Data.lpLuts)
	FreeUp(ContBritVisual_Data.lpLuts);
if (ContBritVisual_Data.lpBuffer)
	FreeUp(ContBritVisual_Data.lpBuffer); 
VisualMap_OnDestroy(hDlg, &ContBritVisual_visual);
}

/***************************************************************************/
LOCAL void ContBritVisual_Draw(HDC hDC,LPRECT lpRect, int x,int y,LPVISUAL lpVisual)
/***************************************************************************/
{
	LPCBSTRUCT lpCB;
	int w, h, id;
	RECT SourceRect, Rect;
	POINT DestPoint;
	LPFRAME lpFrame;
	int depth;
	HBRUSH hBrush;
	LPVOID lpCmsXform = NULL;
	
	lpCB = (LPCBSTRUCT)lpVisual->lpUser;
	if (!lpCB)
		return;

	lpFrame = lpVisual->lpBasisFrame;

	// Check if frames setup
	if (!lpVisual->lpOrgFrame || !lpFrame)
		return;

	// setup onetime buffer
	if (!lpCB->lpBuffer)
	{
		depth = FrameDepth(lpFrame);
		if (!depth) depth = 1;
		if (FrameType(lpFrame) == FDT_PALETTECOLOR)
			depth = 3;
		lpCB->lpBuffer = Alloc((long)FrameXSize(lpFrame)*depth);
	}

	// setup rectangles
	w=FrameXSize(lpFrame);
	h=FrameYSize(lpFrame);

	SourceRect.left   =
	SourceRect.top    = 0;
	SourceRect.right  = Min(w,RectWidth( lpRect))-1; 
	SourceRect.bottom = Min(h,RectHeight(lpRect))-1; 

	DestPoint.x = lpRect->left;
	DestPoint.y = lpRect->top;

	// apply map and diplay
	if (id = GetIDFromPos(x, y))
	{
		id -= IDC_VISUAL_00;
		if (lpCB->wBand != BAND_ALL && WITHIN(id,0,8))
		{
			if (y != 0)
			{
				Rect.left = DestPoint.x;
				Rect.top = DestPoint.y;
				Rect.right = Rect.left+RectWidth(&SourceRect)+1;
				Rect.bottom = Rect.top+RectHeight(&SourceRect)+1;
				hBrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE) );
				HilightRect( hDC, &Rect, hBrush );
				DeleteObject( hBrush );
//				MgxGrayArea( hDC, DestPoint.x, DestPoint.y,
//					RectWidth(&SourceRect), RectHeight(&SourceRect));
				return;
			}
		}
		if (WITHIN(id,0,8))
		{
			MapSetPackedLut(lpCB->lpLuts+PACKEDLUTSIZE*id);
		}
		else
		{
			MapSetPackedLut(NULL);
		}
		if (lpVisual->lpImage)
			lpCmsXform = lpVisual->lpImage->m_cmsXform;
		DisplayMappedImage(hDC,lpFrame,&SourceRect,
			DestPoint, lpCB->lpBuffer,0,0, lpCmsXform);
	}
}

/***************************************************************************/
static BOOL ContBritVisual_Move(int x ,int y,LPVISUAL lpVisual)
/***************************************************************************/
{
	LPCBSTRUCT lpCB;
	int newCont, newBrit;
	int cont, brit;
	int i,j, id;

	lpCB = (LPCBSTRUCT)lpVisual->lpUser;
	if (!lpCB)
		return(FALSE);

	if (x == VISUAL_BEFORE)
	{
		ResetAllMaps();
	}

	if (!ContBritVisual_GetNew(x,y,&newCont,&newBrit,lpVisual))
		return(FALSE);

	UndoMapPreview(GetActiveDoc()/*FALSE*/);
	lpCB->lpMap->Contrast = newCont;
	lpCB->lpMap->Brightness[lpCB->wBand] = newBrit;

	// setup luts
	for(j=-1;j<=1;j++) 
	{
		for(i=-1;i<=1;i++) 
		{
			if (id = GetIDFromPos(i, j))
			{
				id -= IDC_VISUAL_00;
				if (WITHIN(id,0,8) &&
					ContBritVisual_GetNew(i,j,&cont,&brit,lpVisual))
				{
					lpCB->lpMap->Contrast = cont;
					lpCB->lpMap->Brightness[lpCB->wBand] = brit;
			  		MakeMap(lpCB->lpMap );

					MapGetPackedLut(lpCB->lpLuts+PACKEDLUTSIZE*id,
						HueMap.Lut,
						SatMap.Lut,
						MasterMap.Lut,
						RedMap.Lut,
						GreenMap.Lut,
						BlueMap.Lut,
						BlackMap.Lut);

					lpCB->lpMap->Contrast = newCont;
					lpCB->lpMap->Brightness[lpCB->wBand] = newBrit;
				}
			}
		}
	}

	SetDlgItemIntExt(lpCB->hDlg, IDC_CONTRAST,   newCont, " %" );
	SetDlgItemIntExt(lpCB->hDlg, IDC_BRIGHTNESS, newBrit, " %" );
	return(TRUE);
}

/***************************************************************************/
static BOOL ContBritVisual_GetNew(int x ,int y, LPINT cont, LPINT brit, LPVISUAL lpVisual)
/***************************************************************************/
{
	LPCBSTRUCT  lpCB;

	lpCB = (LPCBSTRUCT)lpVisual->lpUser;

	if (!lpCB)
		return(FALSE);

	if (x == VISUAL_BEFORE)
	{
		*cont = 0;
		*brit = 0;
	}
	else
	{
		*cont = lpCB->lpMap->Contrast;
		*brit = lpCB->lpMap->Brightness[lpCB->wBand];
		if (lpCB->wBand == BAND_ALL)
			*cont -= lpVisual->step*y;
		*brit += lpVisual->step*x;
	}
	*cont = Bound(*cont, -100, 100);
	*brit = Bound(*brit, -100, 100);

	return(TRUE);
}


/***************************************************************************/
//	 Enables or diables the top and bottom rows of controls
/***************************************************************************/
LOCAL void EnableContrastVisual(HWND hDlg, BOOL bEnable)
/***************************************************************************/
{
	ControlEnable(hDlg, IDC_VISUAL_00, bEnable);
	ControlEnable(hDlg, IDC_VISUAL_01, bEnable);
	ControlEnable(hDlg, IDC_VISUAL_02, bEnable);
	ControlEnable(hDlg, IDC_VISUAL_20, bEnable);
	ControlEnable(hDlg, IDC_VISUAL_21, bEnable);
	ControlEnable(hDlg, IDC_VISUAL_22, bEnable);
}


// LOCAL prototypes
LOCAL BOOL BalanceVisual_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void BalanceVisual_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void BalanceVisual_OnDestroy(HWND hDlg);
LOCAL void BalanceVisual_Draw(HDC, LPRECT,int,int,struct _visual FAR *);
LOCAL BOOL BalanceVisual_Move(int,int,struct _visual FAR *);
LOCAL void BalanceVisual_MakeMaps(LPMAP lpMap, int newBrit, LPBALANCESTRUCT lpBAL, BOOL bPerminant);
LOCAL MAP *BalanceVisual_GetNew(int x ,int y, LPINT newBrit, LPVISUAL lpVisual);
LOCAL void BalanceVisual_PrepareValues();
LOCAL void BalanceVisual_RestoreValues();

static VISUAL BalanceVisual_visual;
static BALANCESTRUCT BalanceVisual_Data;
static LPBALANCE_PARMS BalanceVisual_lpParms;

/***********************************************************************/
BOOL WINPROC EXPORT DlgBalanceVisualProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, BalanceVisual_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, BalanceVisual_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DESTROY, BalanceVisual_OnDestroy);

	HANDLE_DLGRET(hDlg, WM_QUERYNEWPALETTE, Dialog_OnQueryNewPalette);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_COLORBAL_VISUAL);
	}
}

/************************************************************************/
LOCAL BOOL BalanceVisual_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
LPIMAGE lpImage;

InitPreview(hDlg);
lpImage = GetActiveImage();
CenterPopup( hDlg );
BalanceVisual_lpParms = (LPBALANCE_PARMS)lParam;
if (!lpImage)
	{
	AstralDlgEnd( hDlg, FALSE);
	return(TRUE);
	}

BalanceVisual_Data.wBand = BAND_ALL;
CheckComboItem( hDlg, IDC_BAND, IDC_BAND_ALL, IDC_BAND_LO,
	BalanceVisual_Data.wBand + IDC_BAND_ALL );
BalanceVisual_Data.hDlg     = hDlg;
BalanceVisual_Data.lpBuffer = NULL;
BalanceVisual_Data.lpLuts   = Alloc(PACKEDLUTSIZE*9);
BalanceVisual_Data.bMaintainDensity = TRUE;
CheckDlgButton(hDlg, IDC_MAINTAIN, BalanceVisual_Data.bMaintainDensity);

if (!BalanceVisual_Data.lpLuts)
{
	Message(IDS_EMEMALLOC);
	AstralDlgEnd( hDlg, FALSE);
	return(TRUE);
}

BalanceVisual_visual.lpImage = lpImage;
BalanceVisual_visual.lpOrgFrame = NULL;
BalanceVisual_visual.lpBasisFrame = NULL;
BalanceVisual_visual.lpUser = (LPTR)&BalanceVisual_Data;
BalanceVisual_visual.step = 5;
BalanceVisual_visual.moveType = 2; 	// all but center re-done
BalanceVisual_visual.pDrawVisual = BalanceVisual_Draw;
BalanceVisual_visual.pMoveVisual = BalanceVisual_Move;
ResetAllMaps();
return(VisualMap_OnInitDialog(hDlg, hWndFocus, lParam, &BalanceVisual_visual));
}

LOCAL void InitPreview(HWND hDlg)
   {
   int      x, y, dx, dy;
   HWND     hControl, hWndImage;
   LPOBJECT lpBase;
   LPFRAME  lpFrame;
   RECT     r;

   IsInPreview = FALSE;
   hWndImage = GetActiveDoc();
   hControl = GetDlgItem( hDlg, IDC_VIEWFULL );
   GetWindowRect(GetDlgItem( hDlg, IDC_VISUAL_00), &r);
   ScreenToClient(hDlg, (LPPOINT)&r);
   x = r.left;
   y = r.top;

   GetWindowRect(GetDlgItem( hDlg, IDC_VISUAL_22), &r);
   ScreenToClient(hDlg, (LPPOINT)&r.right);
   dx = r.right - x;
   dy = r.bottom - y;
   SetWindowPos(hControl, NULL, x, y, dx, dy, SWP_NOZORDER);

   GetWindowRect(hControl, &r);
   dx = r.right  - r.left;
   dy = r.bottom - r.top;
   ScreenToClient(hDlg, (LPPOINT)&r);
   ScreenToClient(hDlg, (LPPOINT)&r.right);
   if (hWndImage)
      {
      lpBase = ImgGetBase(GetImagePtr(hWndImage));
      lpFrame = ObjGetEditFrame(lpBase);

      // Link the frame to the image control
      SetWindowLong( hControl, GWL_IMAGE, (long)lpBase);
      }
   }

LOCAL void DoPreview(HWND hDlg, BOOL f)
   {
   HWND  HWnd;

   if ((f && !IsInPreview) || (!f && IsInPreview))
      {
	   for (int i =0 ; i < 13; ++i)
         {
         HWnd = GetDlgItem(hDlg, i+IDC_VISUAL_00);
         if (HWnd)
            ShowWindow(HWnd, f? SW_HIDE : SW_SHOW);
         HWnd = GetDlgItem(hDlg, i+IDC_VISUAL_DUMMY1);
         if (HWnd)
            ShowWindow(HWnd, f? SW_HIDE : SW_SHOW);
         HWnd = GetDlgItem(hDlg, i+IDC_CONTICON);
         if (HWnd)
            ShowWindow(HWnd, f? SW_HIDE : SW_SHOW);
         }
      ShowWindow(GetDlgItem(hDlg, IDC_VIEWFULL), f? SW_SHOW : SW_HIDE);
      IsInPreview = f;
      }

	ControlEnable(hDlg, IDC_RESET, !IsInPreview);
	ControlEnable(hDlg, IDC_RESETALL, !IsInPreview);
   }

/************************************************************************/
LOCAL void BalanceVisual_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
int i;
HWND hActiveWnd;
LPIMAGE lpImage = NULL;
//HWND  hWnd;
//int   x,y,dx,dy;

if (hActiveWnd = GetActiveDoc())
	lpImage = GetImagePtr(hActiveWnd);

switch (id)
	{
	case IDC_BAND:
      DoPreview(hDlg, FALSE);
		if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
			break;
		// setup new band
		if (BalanceVisual_Data.wBand  != id - IDC_BAND_ALL)
		{
			BalanceVisual_Data.wBand = id - IDC_BAND_ALL;
			SetVisual(hDlg,0,0,&BalanceVisual_visual);
		}
	break;
	
	case IDC_MAINTAIN:
      DoPreview(hDlg, FALSE);
		BalanceVisual_Data.bMaintainDensity = !BalanceVisual_Data.bMaintainDensity;
		CheckDlgButton(hDlg, IDC_MAINTAIN, BalanceVisual_Data.bMaintainDensity);
		SetVisual(hDlg,0,0,&BalanceVisual_visual);
	break;
	
	case IDC_PREVIEW:
      DoPreview(hDlg, !IsInPreview);
		BalanceVisual_PrepareValues();
		MapPreview(hActiveWnd);
		BalanceVisual_RestoreValues();
	break;

	case IDOK:
		BalanceVisual_PrepareValues();
		BalanceVisual_lpParms->MasterContrast = MasterMap.Contrast;
		for (i = 0; i < 4; ++i)
			BalanceVisual_lpParms->MasterBrightness[i] = MasterMap.Brightness[i];
		BalanceVisual_lpParms->fMasterReverse = MasterMap.Reverse;
		BalanceVisual_lpParms->RedContrast = RedMap.Contrast;
		for (i = 0; i < 4; ++i)
			BalanceVisual_lpParms->RedBrightness[i] = RedMap.Brightness[i];
		BalanceVisual_lpParms->fRedReverse = RedMap.Reverse;
		BalanceVisual_lpParms->GreenContrast = GreenMap.Contrast;
		for (i = 0; i < 4; ++i)
			BalanceVisual_lpParms->GreenBrightness[i] = GreenMap.Brightness[i];
		BalanceVisual_lpParms->fGreenReverse = GreenMap.Reverse;
		BalanceVisual_lpParms->BlueContrast = BlueMap.Contrast;
		for (i = 0; i < 4; ++i)
			BalanceVisual_lpParms->BlueBrightness[i] = BlueMap.Brightness[i];
		BalanceVisual_lpParms->fBlueReverse = BlueMap.Reverse;
		BalanceVisual_lpParms->BlackContrast = BlackMap.Contrast;
		for (i = 0; i < 4; ++i)
			BalanceVisual_lpParms->BlackBrightness[i] = BlackMap.Brightness[i];
		BalanceVisual_lpParms->fBlackReverse = BlackMap.Reverse;
//		ApplyMap(IDS_UNDOCOLORBAL);
		ResetAllMaps();
		UndoMapPreview(hActiveWnd/*FALSE*/);
		AstralDlgEnd( hDlg, TRUE);
	break;
	
	case IDC_RESETALL:
      DoPreview(hDlg, FALSE);
		VisualMap_OnCommand(hDlg, IDC_VISUAL_BEFORE, NULL, 0, &BalanceVisual_visual);
	break;

	case IDC_RESET:
      DoPreview(hDlg, FALSE);
		ResetBandAllMaps(BalanceVisual_Data.wBand);
		SetVisual(hDlg,0,0,&BalanceVisual_visual);
	break;

	case IDCANCEL:
		ResetAllMaps();
		UndoMapPreview(hActiveWnd/*FALSE*/);
		AstralDlgEnd( hDlg, FALSE);
	break;

	default:
      DoPreview(hDlg, FALSE);
		VisualMap_OnCommand(hDlg, id, hControl, codeNotify, &BalanceVisual_visual);
	break;
   }
}

/***************************************************************************/
LOCAL void BalanceVisual_OnDestroy(HWND hDlg)
/***************************************************************************/
{
if (BalanceVisual_Data.lpLuts)
	FreeUp(BalanceVisual_Data.lpLuts);
if (BalanceVisual_Data.lpBuffer)
	FreeUp(BalanceVisual_Data.lpBuffer); 
VisualMap_OnDestroy(hDlg, &BalanceVisual_visual);
}

/***************************************************************************/
static void BalanceVisual_Draw(HDC hDC,LPRECT lpRect, int x,int y,LPVISUAL lpVisual)
/***************************************************************************/
{
	LPBALANCESTRUCT  lpBal;
	int w, h, id;
	RECT SourceRect;
	POINT DestPoint;
	LPFRAME lpFrame;
	int depth;
	LPVOID lpCmsXform = NULL;
	
	lpBal = (LPBALANCESTRUCT)lpVisual->lpUser;

	if (!lpBal)
		return;

	lpFrame = lpVisual->lpBasisFrame;

	// Check if frames setup
	if (!lpVisual->lpOrgFrame || !lpFrame)
		return;

	// setup onetime buffer
	if (!lpBal->lpBuffer)
	{
		depth = FrameDepth(lpFrame);
		if (!depth) depth = 1;
		if (FrameType(lpFrame) == FDT_PALETTECOLOR)
			depth = 3;
		lpBal->lpBuffer = Alloc((long)FrameXSize(lpFrame)*depth);
	}

	// setup rectangles
	w=FrameXSize(lpFrame);
	h=FrameYSize(lpFrame);

	SourceRect.left   =
	SourceRect.top    = 0;
	SourceRect.right  = Min(w,RectWidth( lpRect))-1; 
	SourceRect.bottom = Min(h,RectHeight(lpRect))-1; 

	DestPoint.x = lpRect->left;
	DestPoint.y = lpRect->top;

	// apply map and diplay
	if (id = GetIDFromPos(x, y))
	{
		id -= IDC_VISUAL_00;
		if (WITHIN(id,0,8))
		{
			MapSetPackedLut(lpBal->lpLuts+PACKEDLUTSIZE*id);
		}
		else
		{
			MapSetPackedLut(NULL);
		}
		if (lpVisual->lpImage)
			lpCmsXform = lpVisual->lpImage->m_cmsXform;
		DisplayMappedImage(hDC,lpFrame,&SourceRect, DestPoint,
			lpBal->lpBuffer,0,0, lpCmsXform);
	}
}

/***************************************************************************/
static BOOL BalanceVisual_Move(int x ,int y,LPVISUAL lpVisual)
/***************************************************************************/
{
	LPBALANCESTRUCT  lpBAL;
	int brit;
	int red, green, blue, cyan, magenta, yellow, black, white;
	MAP *lpMap;
	int i,j, id;

	lpBAL = (LPBALANCESTRUCT)lpVisual->lpUser;

	if (!lpBAL)
		return(FALSE);

	UndoMapPreview(GetActiveDoc()/*FALSE*/);

	lpMap = NULL;
	if (x == VISUAL_BEFORE)
		ResetAllMaps();
	else if (lpMap = BalanceVisual_GetNew(x , y, &brit, lpVisual))
		BalanceVisual_MakeMaps(lpMap, brit, lpBAL, YES);

	// take care of after frame
	id = IDC_VISUAL_11 - IDC_VISUAL_00;

	MapGetPackedLut(lpBAL->lpLuts+PACKEDLUTSIZE*id,
		HueMap.Lut,
		SatMap.Lut,
		MasterMap.Lut,
		RedMap.Lut,
		GreenMap.Lut,
		BlueMap.Lut,
		BlackMap.Lut);

	for(j=-1;j<=1;j++) 
	{
		for(i=-1;i<=1;i++) 
		{
			if (id = GetIDFromPos(i, j))
			{
				id -= IDC_VISUAL_00;
				if (WITHIN(id,0,8) &&
					(lpMap = BalanceVisual_GetNew(i , j, &brit, lpVisual)))
				{
					// create maps
					BalanceVisual_MakeMaps(lpMap, brit, lpBAL, NO);

					MapGetPackedLut(lpBAL->lpLuts+PACKEDLUTSIZE*id,
						HueMap.Lut,
						SatMap.Lut,
						MasterMap.Lut,
						RedMap.Lut,
						GreenMap.Lut,
						BlueMap.Lut,
						BlackMap.Lut);
					// reset maps
					BalanceVisual_MakeMaps(lpMap, lpMap->Brightness[lpBAL->wBand], lpBAL, NO);
				}
			}
		}
	}
	
	// display values
	BalanceVisual_PrepareValues();
	lpMap = ActivateChannel(IDC_DO_ALL);
	if (lpMap){	white = lpMap->Brightness[lpBAL->wBand]; black = -white;}

	lpMap = ActivateChannel(IDC_DO_RED);
	if (lpMap){	red = -lpMap->Brightness[lpBAL->wBand]; cyan = -red;}

	lpMap = ActivateChannel(IDC_DO_GREEN);
	if (lpMap){	green = -lpMap->Brightness[lpBAL->wBand]; magenta = -green;}

	lpMap = ActivateChannel(IDC_DO_BLUE);
	if (lpMap){	blue = -lpMap->Brightness[lpBAL->wBand]; yellow = -blue;}

	SetDlgItemIntExt(lpBAL->hDlg, IDC_CORRECT_R, red,     " %" );
	SetDlgItemIntExt(lpBAL->hDlg, IDC_CORRECT_G, green,   " %" );
	SetDlgItemIntExt(lpBAL->hDlg, IDC_CORRECT_B, blue,    " %" );
	SetDlgItemIntExt(lpBAL->hDlg, IDC_CORRECT_C, cyan,    " %" );
	SetDlgItemIntExt(lpBAL->hDlg, IDC_CORRECT_M, magenta, " %" );
	SetDlgItemIntExt(lpBAL->hDlg, IDC_CORRECT_Y, yellow,  " %" );
	SetDlgItemIntExt(lpBAL->hDlg, IDC_CORRECT_K, black,   " %" );
	SetDlgItemIntExt(lpBAL->hDlg, IDC_CORRECT_W, white,   " %" );

	BalanceVisual_RestoreValues();
	return(TRUE);
}


//************************************************************************=
static void BalanceVisual_MakeMaps(LPMAP lpMap, int newBrit, LPBALANCESTRUCT lpBAL, BOOL bPerminant)
//************************************************************************=
{
	int old;
	int red, green, blue;
	int dred, dgreen, dblue;
	BOOL bMaintain;
	
	bMaintain = lpBAL->bMaintainDensity;
	dred = dgreen = dblue = 0;
	red = 	RedMap.Brightness[lpBAL->wBand];
	green = GreenMap.Brightness[lpBAL->wBand];
	blue = 	BlueMap.Brightness[lpBAL->wBand];
	
	if (lpMap == &RedMap)
	{
		dred = newBrit-red;
		if (!bMaintain)
			goto DoIt;
		dgreen = -(dred)/2;
		dblue = -(dred)/2;
	}
	else if (lpMap == &GreenMap)
	{
		dgreen = newBrit-green;
		if (!bMaintain)
			goto DoIt;
		dred = -(dgreen)/2;
		dblue = -(dgreen)/2;
	}
	else if (lpMap == &BlueMap)
	{
		dblue = newBrit-blue;
		if (!bMaintain)
			goto DoIt;
		dred = -(dblue)/2;
		dgreen = -(dblue)/2;
	}
	else
	{	
		old = lpMap->Brightness[lpBAL->wBand];
 		lpMap->Brightness[lpBAL->wBand] = newBrit;
		BalanceVisual_PrepareValues();
		MakeMap(lpMap);
		BalanceVisual_RestoreValues();
		if (!bPerminant)
			lpMap->Brightness[lpBAL->wBand] = old;  
		return;
	}
DoIt:	
	BlueMap.Brightness[lpBAL->wBand] = mbound(blue+dblue, -100,100);
	GreenMap.Brightness[lpBAL->wBand] = mbound(green+dgreen, -100,100);
	RedMap.Brightness[lpBAL->wBand] = mbound(red+dred, -100,100);
	BalanceVisual_PrepareValues();
	// adjust blue
	lpMap = ActivateChannel(IDC_DO_BLUE);
	MakeMap(lpMap);
	// adjust green
	lpMap = ActivateChannel(IDC_DO_GREEN);
	MakeMap(lpMap);
	// adjust red
	lpMap = ActivateChannel(IDC_DO_RED);
	MakeMap(lpMap);
	BalanceVisual_RestoreValues();
	if (!bPerminant)
	{
		BlueMap.Brightness[lpBAL->wBand] = blue;
		GreenMap.Brightness[lpBAL->wBand] = green;
		RedMap.Brightness[lpBAL->wBand] = red;
	}
}

/***************************************************************************\

	Returns the color map associated with the x,y pos.
		sets newBrit to be the new brightness associated with the channel
		returns NULL if  failure or 'After'

\***************************************************************************/

//************************************************************************=
LOCAL MAP *BalanceVisual_GetNew(int x ,int y, LPINT newBrit, LPVISUAL lpVisual)
//************************************************************************=
{
	MAP *lpMap;
	LPBALANCESTRUCT  lpBAL;

	if (x == VISUAL_BEFORE)
		return(NULL);

	if (!WITHIN(x, -1,1))
		return(NULL);

	lpBAL = (LPBALANCESTRUCT)lpVisual->lpUser;

	if (!lpBAL)
		return(FALSE);

	switch (y)
	{
		case -1:
			if (x==-1)
			{
				lpMap = ActivateChannel(IDC_DO_RED);
				*newBrit = lpMap->Brightness[lpBAL->wBand]+(lpVisual->step*-2);
			}  
			else if (x==1)
			{
				lpMap = ActivateChannel(IDC_DO_BLUE);
				*newBrit = lpMap->Brightness[lpBAL->wBand]+(lpVisual->step*2);
			}
			else
			{
				lpMap = ActivateChannel(IDC_DO_ALL);
				*newBrit = lpMap->Brightness[lpBAL->wBand]+lpVisual->step*2;
			} 			
		break;

		case 0:
			if (x==1) 
			{
				lpMap = ActivateChannel(IDC_DO_GREEN);
				*newBrit = lpMap->Brightness[lpBAL->wBand]+(lpVisual->step*-2);
			}				
			else if (x==-1) 
			{
				lpMap = ActivateChannel(IDC_DO_GREEN);
				*newBrit = lpMap->Brightness[lpBAL->wBand]+(lpVisual->step*2);
			}  
			else
				return(NULL);							
		break;

		case 1:
			if (x==-1)
			{
				lpMap = ActivateChannel(IDC_DO_BLUE);
				*newBrit = lpMap->Brightness[lpBAL->wBand]+(lpVisual->step*-2);
			}
			else if (x==1) 
			{
				lpMap = ActivateChannel(IDC_DO_RED);
				*newBrit = lpMap->Brightness[lpBAL->wBand]+(lpVisual->step*2);
			}			
			else 
			{
				lpMap = ActivateChannel(IDC_DO_ALL);
				*newBrit = lpMap->Brightness[lpBAL->wBand]-lpVisual->step*2;
			}
		break;

		default:
			return(NULL);
		break;
	}

	*newBrit = Bound(*newBrit, -100, 100);

	return(lpMap);
}

static int BalVals[4][4];

//************************************************************************=
//	This is a 11th hour workaround to keep more accuracy in the brightness.
//	Ultimately the mapping stuff should use more than 100 levels.
//************************************************************************=
static void BalanceVisual_PrepareValues()
//************************************************************************=
{
	int band;
	int val;
	
	for (band=0; band<4; band++)
	{
	 	val = BalVals[0][band] = MasterMap.Brightness[band];
		MasterMap.Brightness[band] = val/2;
		val = BalVals[1][band] = RedMap.Brightness[band];
		RedMap.Brightness[band] = val/2;
		val = BalVals[2][band] = GreenMap.Brightness[band];
		GreenMap.Brightness[band] = val/2;
		val = BalVals[3][band] = BlueMap.Brightness[band];
		BlueMap.Brightness[band] = val/2;
	}
}

//************************************************************************=
static void BalanceVisual_RestoreValues()
//************************************************************************=
{
	int band;
	
	for (band=0; band<4; band++)
	{
	 	MasterMap.Brightness[band]=	BalVals[0][band]; 
		RedMap.Brightness[band]	  =	BalVals[1][band]; 
		GreenMap.Brightness[band] =	BalVals[2][band]; 
		BlueMap.Brightness[band] =	BalVals[3][band]; 
	}
}

// LOCAL prototypes
LOCAL BOOL Posterize_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Posterize_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL MAP *Posterize_ResetChannel( HWND hDlg, WORD wChannel );
LOCAL void Poster_OnTimer(HWND hwnd, UINT id);
LOCAL void Poster_OnDestroy(HWND hDlg);

static LPPOSTERIZE_PARMS Posterize_lpParms;
BOOL  InPosterizeMode = TRUE;

/***********************************************************************/
BOOL WINPROC EXPORT DlgPosterizeProc( HWND hDlg, UINT msg, WPARAM wParam,
                                 LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Posterize_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Posterize_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_TIMER, Poster_OnTimer);
	HANDLE_DLGMSG(hDlg, WM_DESTROY, Poster_OnDestroy);

	HANDLE_DLGRET(hDlg, WM_QUERYNEWPALETTE, Dialog_OnQueryNewPalette);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, InPosterizeMode ? IDD_POSTERIZE : IDD_THRESHOLD);
	}
}


/************************************************************************/
LOCAL BOOL Posterize_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
LPIMAGE lpImage;
FRMDATATYPE type;

lpImage = GetActiveImage();
RightPopup( hDlg );
Posterize_lpParms = (LPPOSTERIZE_PARMS)lParam;
ResetAllMaps();

if (InPosterizeMode)
   MasterMap.Levels    = 5;
else
   MasterMap.Threshold = 50;
MakeMap(&MasterMap);

type = FrameType(ImgGetBaseEditFrame(lpImage));
Map_Values.wChannel = IDC_DO_ALL;
ChannelsCombo_Setup(hDlg, IDC_CHANNELS, type, Map_Values.wChannel);

CheckDlgButton( hDlg, IDC_AUTO, Mapping.Poster_bAuto);
ControlEnable(hDlg, IDC_PREVIEW, !Mapping.Poster_bAuto);

if ( FrameDepth(ImgGetBaseEditFrame(lpImage)) == 1 )
	{
	ControlEnable( hDlg, IDC_DO_RED, NO );
	ControlEnable( hDlg, IDC_DO_GREEN, NO );
	ControlEnable( hDlg, IDC_DO_BLUE, NO );
	ControlEnable( hDlg, IDC_DO_CYAN, NO );
	ControlEnable( hDlg, IDC_DO_MAGENTA, NO );
	ControlEnable( hDlg, IDC_DO_YELLOW, NO );
	}

Map_Values.pMap = Posterize_ResetChannel( hDlg, Map_Values.wChannel );

if (Mapping.Poster_bAuto)
   AutoPreview(Map_Values.pMap, Mapping.Poster_bAuto, YES, hDlg, GetActiveDoc());

return(TRUE);
}

/************************************************************6***************/
LOCAL void Poster_OnDestroy(HWND hDlg)
/***************************************************************************/
{
	AutoPreview_OnDestroy(hDlg);
	Dialog_OnDestroy(hDlg);
}

/************************************************************************/
LOCAL void Poster_OnTimer(HWND hDlg, UINT id)
/************************************************************************/
{	
	if (!AutoPreview_OnTimer(hDlg, id))
		return;
		
	// got good timer		   
	MakeMap(Map_Values.pMap);
	MapPreview(GetActiveDoc());
}


/************************************************************************/
LOCAL void Posterize_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
int i;
BOOL Bool;
HWND hActiveWnd;
LPIMAGE lpImage = NULL;

if (hActiveWnd = GetActiveDoc())
	lpImage = GetImagePtr(hActiveWnd);

switch (id)
	{
	case IDC_CHANNELS:
		if (!(i=ChannelsCombo_Handle(hDlg, id, codeNotify)))
			break;
		Map_Values.wChannel = i;
		Map_Values.pMap = Posterize_ResetChannel( hDlg, Map_Values.wChannel );
	break;

	case IDC_MAPLEVELS_SCROLL:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		i = HandleSlide( hDlg, id, codeNotify, &Bool );
		if ( Map_Values.pMap->Levels == i )
			break;
		Map_Values.pMap->Levels = i;
		SetDlgItemSpin( hDlg, IDC_MAPLEVELS, Map_Values.pMap->Levels, NO );
		AutoPreview(Map_Values.pMap, Mapping.Poster_bAuto, Bool, hDlg, hActiveWnd);
	break;

	case IDC_MAPTHRESH_SCROLL:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		i = HandleSlide( hDlg, id, codeNotify, &Bool );
		if ( Map_Values.pMap->Threshold == i )
			break;
		Map_Values.pMap->Threshold = i;
		SetDlgItemSpin( hDlg, IDC_MAPTHRESH, Map_Values.pMap->Threshold, NO);
		AutoPreview(Map_Values.pMap, Mapping.Poster_bAuto, Bool, hDlg, hActiveWnd);
	break;

	case IDC_MAPLEVELS:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		i = GetDlgItemSpin( hDlg, IDC_MAPLEVELS, &Bool, NO );
		if ( Map_Values.pMap->Levels == i )
			break;
		Map_Values.pMap->Levels = i;
		SetSlide( hDlg, IDC_MAPLEVELS_SCROLL, Map_Values.pMap->Levels );
		AutoPreview(Map_Values.pMap, Mapping.Poster_bAuto, NO, hDlg, hActiveWnd);
	break;

	case IDC_MAPTHRESH:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		i = GetDlgItemSpin( hDlg, IDC_MAPTHRESH, &Bool, NO );
		if ( Map_Values.pMap->Threshold == i )
			break;
		Map_Values.pMap->Threshold = i;
		SetSlide( hDlg, IDC_MAPTHRESH_SCROLL, Map_Values.pMap->Threshold );
		AutoPreview(Map_Values.pMap, Mapping.Poster_bAuto, NO, hDlg, hActiveWnd);
	break;

	case IDC_AUTO:
		Mapping.Poster_bAuto = !Mapping.Poster_bAuto;
		CheckDlgButton( hDlg, id, Mapping.Poster_bAuto);
		ControlEnable(hDlg, IDC_PREVIEW, !Mapping.Poster_bAuto);
		AutoPreview(Map_Values.pMap, Mapping.Poster_bAuto, YES, hDlg, hActiveWnd);
	break; 
	
	case IDC_PREVIEW:
	  	AutoPreview_Button(hActiveWnd, Map_Values.pMap);
	break;

	case IDC_RESETALL:
		ResetAllMaps();
		Map_Values.pMap = Posterize_ResetChannel( hDlg, Map_Values.wChannel );
		AutoPreview(Map_Values.pMap, Mapping.Poster_bAuto, YES, hDlg, hActiveWnd);
	break;

	case IDC_RESET:
		ResetMapEx( Map_Values.pMap, MAPPOINTS, -1, Map_Values.wChannel == IDC_DO_ALL );
		Map_Values.pMap = Posterize_ResetChannel( hDlg, Map_Values.wChannel );
		AutoPreview(Map_Values.pMap, Mapping.Poster_bAuto, YES, hDlg, hActiveWnd);
	break;

	case IDOK:
		if ( !lpImage )
		{
			AstralDlgEnd( hDlg, TRUE );
			break;
		}
		UndoMapPreview(hActiveWnd/*FALSE*/);
		Posterize_lpParms->MasterInfo.Levels = MasterMap.Levels;
		Posterize_lpParms->MasterInfo.Threshold = MasterMap.Threshold;
		Posterize_lpParms->RedInfo.Levels = RedMap.Levels;
		Posterize_lpParms->RedInfo.Threshold = RedMap.Threshold;
		Posterize_lpParms->GreenInfo.Levels = GreenMap.Levels;
		Posterize_lpParms->GreenInfo.Threshold = GreenMap.Threshold;
		Posterize_lpParms->BlueInfo.Levels = BlueMap.Levels;
		Posterize_lpParms->BlueInfo.Threshold = BlueMap.Threshold;
		Posterize_lpParms->BlackInfo.Levels = BlackMap.Levels;
		Posterize_lpParms->BlackInfo.Threshold = BlackMap.Threshold;
		ResetAllMaps();
		UnAutoPreview(hDlg, hActiveWnd);
		AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
		if ( !lpImage )
		{
			AstralDlgEnd( hDlg, FALSE );
			break;
		}
		ResetAllMaps();
		UnAutoPreview(hDlg, hActiveWnd);
		AstralDlgEnd( hDlg, FALSE );
	break;

   default:
	break;
   }
}

/***************************************************************************/
MAP *Posterize_ResetChannel( HWND hDlg, WORD wChannel )
//************************************************************************=
{
	MAP *lpMap; //since this is a static pointer to a static structure,
	// don't make it a far pointer because the data segment might move

	lpMap = ActivateChannel( wChannel );
	InitDlgItemSpin( hDlg, IDC_MAPLEVELS, lpMap->Levels, NO, 1, 256 );
	InitSlide( hDlg, IDC_MAPLEVELS_SCROLL, lpMap->Levels, 1, 256 );
	InitDlgItemSpin( hDlg, IDC_MAPTHRESH, lpMap->Threshold, NO, 0, 100 );
	InitSlide( hDlg, IDC_MAPTHRESH_SCROLL, lpMap->Threshold, 0, 100 );

	return( lpMap );
}

// LOCAL prototypes
LOCAL BOOL Hue_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Hue_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void Hue_OnDestroy(HWND hDlg);
LOCAL void Hue_OnTimer(HWND hwnd, UINT id);
// LOCAL void Hue_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
// LOCAL void Hue_OnLButtonUp(HWND hDlg, int x, int y, UINT keyFlags);
LOCAL void Hue_ResetControls( HWND hDlg );
LOCAL void Hue_Reset( HWND hDlg, int slot );
LOCAL void Hue_SetPatch( HWND hDlg, ITEMID idPatch, int hue, BOOL bSetText = FALSE);
LOCAL void Hue_Fix_Map();

// statics used for Hue Dialog
static LPHUE_PARMS Hue_lpParms;

static int HueBaseSlotIndex;	// Slot0 maps to HueMap.Pnt[HueBaseSlotIndex+1]
static int HueBase;				// The hue of first slider (in hue coords)
static int HueStart;			// The hue of HueMap.Pnt[1] (in hue coords)
														

#define HUE_TO_SLIDER(hue) \
	(TOANGLE(hue))

//	(TOANGLE(((hue)-HueBase+MAX_HUES) % MAX_HUES))
	
#define SLIDER_TO_HUE(val) \
	(FROMANGLE(val))

//	( ((FROMANGLE(val))+HueBase) % MAX_HUES)
	

// first and last positions reserved to allow smooth wrap at ends
#define GET_HUE_INDEX(slot) \
	(slot+1)
//	(((slot + HueBaseSlotIndex - 1) % (HueMap.Points - 2)) + 1)
	

/***********************************************************************/
BOOL WINPROC EXPORT DlgHueProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch (msg)
	{
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, Hue_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_DESTROY, Hue_OnDestroy);
		HANDLE_DLGMSG(hDlg, WM_TIMER, Hue_OnTimer);
//		HANDLE_DLGMSG(hDlg, WM_MOUSEMOVE, Hue_OnMouseMove);
//		HANDLE_DLGMSG(hDlg, WM_LBUTTONUP, Hue_OnLButtonUp);
		HANDLE_DLGMSG(hDlg, WM_COMMAND, Hue_OnCommand);
	
		HANDLE_DLGRET(hDlg, WM_QUERYNEWPALETTE, Dialog_OnQueryNewPalette);
	
		default:
		return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_HUE);
	}
}

/************************************************************************/
LOCAL BOOL Hue_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	RightPopup( hDlg );
	Hue_lpParms = (LPHUE_PARMS)lParam;
	HueBase = 0;
	HueStart = 0;
	HueBaseSlotIndex = 1;
	CheckDlgButton( hDlg, IDC_AUTO, Mapping.HueMap_bAuto);
    ControlEnable(hDlg, IDC_PREVIEW, !Mapping.HueMap_bAuto);
	ResetMap( &HueMap, HUEMAPPOINTS, NO );
	ResetMap( &SatMap, MAPPOINTS, NO );
	ResetMapEx( &MasterMap, MAPPOINTS, NO, YES);
	Hue_ResetControls( hDlg );
	return(TRUE);
}

/***************************************************************************/
LOCAL void Hue_OnDestroy(HWND hDlg)
/***************************************************************************/
{
	AutoPreview_OnDestroy(hDlg);
	Probe_End(hDlg);
}

/************************************************************************/
LOCAL void Hue_OnTimer(HWND hDlg, UINT id)
/************************************************************************/
{	
	if (!AutoPreview_OnTimer(hDlg, id))
		return;

	// got good timer
	MakeMap(&HueMap);
	MakeMap(&SatMap);
	MakeMap(&MasterMap);
	MapPreview(GetActiveDoc());
}

#ifdef UNUSED
/***********************************************************************/
LOCAL void Hue_OnMouseMove(HWND hDlg, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	COLORINFO Color;
	
	if (Probe_OnMouseMove(hDlg, x, y, keyFlags, &Color))
	{
		// probing with valid value
		Hue_SetPatch( hDlg, IDC_INHUE1, Color.hsl.hue, TRUE);
	}
	else if (bProbing) 
	{
		// probing but no valid value
		Hue_SetPatch( hDlg, IDC_INHUE1, HueMap.Pnt[GET_HUE_INDEX(0)].x, TRUE);
	}
	
}
#endif //unused

#ifdef UNUSED
/***********************************************************************/
LOCAL void Hue_OnLButtonUp(HWND hDlg, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	COLORINFO Color;
	BOOL bGotPoint;
	int i, newhue, slot;
	double fx, dx;
	
	if (!bProbing)
		return;
	bGotPoint = Probe_OnLButtonUp(hDlg,x,y,keyFlags,&Color);
	if (!bGotPoint)
	{
		// unsucessful probe session
		Hue_SetPatch( hDlg, IDC_INHUE1, HueMap.Pnt[GET_HUE_INDEX(0)].x, TRUE);
		return;
	}
	else
	{
		// got a color - move all to it
		// find new smallest hue
		dx = (double)(MAX_HUES-1)/(HueMap.Points-2);
		newhue = (int)(Color.hsl.hue % (int)dx);
		// get the map- for intrpolation
		MakeMap(&HueMap);
		// make new map
		fx = newhue;
		for (i=1; i<HueMap.Points-1; i++)
		{
			x = (int)(fx+.5);
			x = Bound(x,0,MAX_HUES-1);
			HueMap.Pnt[i].x = x;
			HueMap.Pnt[i].y = HueMap.Lut[x];
			fx+=dx;
		}
		// find which point mapped to the given point
		// find new hue base
		for (i=1; i<HueMap.Points-1; i++)
		{
			// add 1 to compensate for error 
			if (HueMap.Pnt[i].x + 1 >= Color.hsl.hue)
			{
				HueBaseSlotIndex = i;
				HueBase = HueMap.Pnt[i].y;
				HueStart = HueMap.Pnt[1].x;
				break;
			}
		}
		Hue_Fix_Map();
		// reset controls
		for (slot=0; slot<HueMap.Points-2; slot++)
		{
			
			i = GET_HUE_INDEX(slot);
			InitSlide( hDlg, IDC_SLIDEHUE1+slot, HUE_TO_SLIDER(HueMap.Pnt[i].y), 0, 360 );
			Hue_SetPatch( hDlg, IDC_INHUE1+slot, HueMap.Pnt[i].x , TRUE );
			Hue_SetPatch( hDlg, IDC_HUE1+slot, HueMap.Pnt[i].y , TRUE);
		}
	}
}
#endif //unused

/************************************************************************/
LOCAL void Hue_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
	BOOL Bool;
	int i, iSlot;
	HWND hActiveWnd;
	LPIMAGE lpImage = NULL;
	
	if (hActiveWnd = GetActiveDoc())
		lpImage = GetImagePtr(hActiveWnd);
	
	switch (id)
	{
		case IDC_SLIDEHUE1:
		case IDC_SLIDEHUE2:
		case IDC_SLIDEHUE3:
		case IDC_SLIDEHUE4:
		case IDC_SLIDEHUE5:
		case IDC_SLIDEHUE6:
		case IDC_SLIDEHUE7:
		case IDC_SLIDEHUE8:
		case IDC_SLIDEHUE9:
		case IDC_SLIDEHUE10:
		case IDC_SLIDEHUE11:
		case IDC_SLIDEHUE12:
			iSlot = id - IDC_SLIDEHUE1;
			if ( iSlot >= HueMap.Points-2 )
				break;
			if ( GetFocus() != GetDlgItem( hDlg, id ) )
				break;
			i = HandleSlide( hDlg, id, codeNotify, &Bool );
			// get the true hue
			i = SLIDER_TO_HUE(i);
			if ( HueMap.Pnt[GET_HUE_INDEX(iSlot)].y == i )
				break;
			HueMap.Pnt[GET_HUE_INDEX(iSlot)].y = i;
			Hue_Fix_Map();
			Hue_SetPatch( hDlg, IDC_HUE1+iSlot, i, TRUE );
			AutoPreview(&HueMap, Mapping.HueMap_bAuto, Bool, hDlg, hActiveWnd);
		break;
	
		case IDC_HUE1:
		case IDC_HUE2:
		case IDC_HUE3:
		case IDC_HUE4:
		case IDC_HUE5:
		case IDC_HUE6:
		case IDC_HUE7:
		case IDC_HUE8:
		case IDC_HUE9:
		case IDC_HUE10:
		case IDC_HUE11:
		case IDC_HUE12:
			iSlot = id - IDC_HUE1;
			if ( iSlot >= HueMap.Points-2 )
				break;
			Hue_Reset( hDlg, iSlot );
			AutoPreview(&HueMap, Mapping.HueMap_bAuto, YES, hDlg, hActiveWnd);
		break;
	
//		case IDC_MAP_PROBE:
//			Probe_Begin(hDlg, codeNotify);
//		break;
	
		case IDC_AUTO:
			Mapping.HueMap_bAuto = !Mapping.HueMap_bAuto;
			CheckDlgButton( hDlg, id, Mapping.HueMap_bAuto);
            ControlEnable(hDlg, IDC_PREVIEW, !Mapping.HueMap_bAuto);
		   	AutoPreview(&HueMap, Mapping.HueMap_bAuto, YES, hDlg, hActiveWnd);
		break;
	
		case IDC_SATURATION:
			if ( GetFocus() != GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			i = (int)GetDlgItemSpin( hDlg, IDC_SATURATION, &Bool, YES );
			if ( SatMap.Brightness[0] != -i )
			{
				SatMap.Brightness[0] = -i;
				SetSlide( hDlg, IDC_SATURATION_SCROLL, i );
				AutoPreview(&SatMap, Mapping.HueMap_bAuto, NO, hDlg, hActiveWnd);
			}
		break;
	
		case IDC_SATURATION_SCROLL:
			if ( GetFocus() != GetDlgItem( hDlg, id ) )
				break;
			i = HandleSlide( hDlg, id, codeNotify, &Bool );
			if ( SatMap.Brightness[0] != -i )
			{
				SatMap.Brightness[0] = -i;
				SetDlgItemSpin( hDlg, IDC_SATURATION, i, YES );
				AutoPreview(&SatMap, Mapping.HueMap_bAuto, Bool, hDlg, hActiveWnd);
			}
		break;
	
		case IDC_LIGHTNESS:
			if ( GetFocus() != GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			i = (int)GetDlgItemSpin( hDlg, IDC_LIGHTNESS, &Bool, YES );
			if ( MasterMap.Brightness[0] != -i )
			{
				MasterMap.Brightness[0] = -i;
				SetSlide( hDlg, IDC_LIGHTNESS_SCROLL, i );
				AutoPreview(&MasterMap, Mapping.HueMap_bAuto, NO, hDlg, hActiveWnd);
			}
		break;
	
		case IDC_LIGHTNESS_SCROLL:
			if ( GetFocus() != GetDlgItem( hDlg, id ) )
				break;
			i = HandleSlide( hDlg, id, codeNotify, &Bool );
			if ( MasterMap.Brightness[0] != -i )
			{
				MasterMap.Brightness[0] = -i;
				SetDlgItemSpin( hDlg, IDC_LIGHTNESS, i, YES );
				AutoPreview(&MasterMap, Mapping.HueMap_bAuto, Bool, hDlg, hActiveWnd);
			}
		break;
	
		case IDC_PREVIEW:
			AutoPreview_Button(hActiveWnd, &HueMap);
		break;
	
		case IDC_RESETALL:
		case IDC_RESET:
			ResetMap( &HueMap, HUEMAPPOINTS, NO );
			ResetMap( &SatMap, MAPPOINTS, NO );
			ResetMapEx( &MasterMap, MAPPOINTS, NO, YES);
			Hue_ResetControls( hDlg );
			MakeMap(&SatMap);
			MakeMap(&MasterMap);
			AutoPreview(&HueMap, Mapping.HueMap_bAuto, YES, hDlg, hActiveWnd);
		break;
	
		case IDOK:
			if ( !lpImage )
			{
				AstralDlgEnd( hDlg, TRUE );
				break;
			}
			UndoMapPreview(hActiveWnd/*FALSE*/);
			map_to_mapinfo(&HueMap, &Hue_lpParms->HueInfo);
			Hue_lpParms->Saturation = SatMap.Brightness[0];
			Hue_lpParms->Brightness = MasterMap.Brightness[0];
			ResetAllMaps();
			AnimateMap(hActiveWnd, NULL, YES);
			AstralDlgEnd( hDlg, TRUE );
		break;
	
		case IDCANCEL:
			if ( !lpImage )
			{
				AstralDlgEnd( hDlg, FALSE );
				break;
			}
			ResetAllMaps();
			AnimateMap(hActiveWnd, NULL, YES);
			UndoMapPreview(hActiveWnd/*FALSE*/);
			AstralDlgEnd( hDlg, FALSE );
		break;
	
	   default:
		break;
	   }
}

//************************************************************************=
void Hue_ResetControls( HWND hDlg )
//************************************************************************=
{
int i;

InitDlgItemSpin( hDlg, IDC_SATURATION, -SatMap.Brightness[0], YES, -100, 100 );
InitSlide( hDlg, IDC_SATURATION_SCROLL, -SatMap.Brightness[0], -100, 100 );
InitDlgItemSpin( hDlg, IDC_LIGHTNESS, MasterMap.Brightness[0], YES, -100, 100 );
InitSlide( hDlg, IDC_LIGHTNESS_SCROLL, MasterMap.Brightness[0], -100, 100 );
for ( i=0; i<HueMap.Points-2; i++ )
	Hue_Reset( hDlg, i );
HueMap.bModified = YES; // to force HSL mapping
}


//************************************************************************=
void Hue_Reset( HWND hDlg, int slot )
//************************************************************************=
{
int hue;
int index;

index = GET_HUE_INDEX(slot);
//hue = FMUL( (index-1), FGET((MAX_HUES-1), HueMap.Points-2 ));
hue = FMUL( (index-1), FGET((MAX_HUES-1), HueMap.Points-3 ));
hue += HueStart;
hue = hue % MAX_HUES;
HueMap.Pnt[index].x = hue;
HueMap.Pnt[index].y = hue;
Hue_Fix_Map();
InitSlide( hDlg, IDC_SLIDEHUE1+slot, HUE_TO_SLIDER(hue), 0, 360 );
Hue_SetPatch( hDlg, IDC_HUE1+slot, hue , TRUE);
Hue_SetPatch( hDlg, IDC_INHUE1+slot, hue, TRUE );
}

//************************************************************************
//		Hue_Fix_Map
// DESCRIPTION:
//		Assures smooth transition at ends of hue map.
//************************************************************************
LOCAL void Hue_Fix_Map()
//************************************************************************
{
	HueMap.Pnt[HueMap.Points-1].x = MAX_HUES-1;
	HueMap.Pnt[HueMap.Points-1].y = HueMap.Pnt[HueMap.Points-2].y;
	HueMap.Pnt[0].x = 0;
	HueMap.Pnt[0].y = HueMap.Pnt[1].y;     

#ifdef UNUSED // the old way
	int dx, y1, y2, dy1, dy2, y;
	
	// compute first point
	dx = MAX_HUES - HueMap.Pnt[HueMap.Points-2].x;
	if ((dx + HueMap.Pnt[1].x) <= 0)
		return; 	// should never happen
	y1 = HueMap.Pnt[HueMap.Points-2].y;
	y2 = HueMap.Pnt[1].y;
	dy1 = (y1-y2+MAX_HUES) % MAX_HUES;
	dy2 = (y2-y1+MAX_HUES) % MAX_HUES;
	if (dy1 < dy2)
		y = y2 + (dy1*(HueMap.Pnt[1].x)/(dx + HueMap.Pnt[1].x));
	else
		y = y1 + (dy2*dx/(dx + HueMap.Pnt[1].x));
	y = y % MAX_HUES;
	HueMap.Pnt[0].x = 0;
	HueMap.Pnt[0].y = y;     
	// compute last point
	dx = MAX_HUES - 1 - HueMap.Pnt[HueMap.Points-2].x;
	if ((dx + HueMap.Pnt[1].x) <= 0)
		return; 	// should never happen
	y1 = HueMap.Pnt[HueMap.Points-2].y;
	y2 = HueMap.Pnt[1].y;
	dy1 = (y1-y2+MAX_HUES) % MAX_HUES;
	dy2 = (y2-y1+MAX_HUES) % MAX_HUES;
	if (dy1 < dy2)
		y = y2 + (dy1*(HueMap.Pnt[1].x)/(dx + HueMap.Pnt[1].x));
	else
		y = y1 + (dy2*dx/(dx + HueMap.Pnt[1].x));
	y = y % MAX_HUES;
	HueMap.Pnt[HueMap.Points-1].x = MAX_HUES-1;
	HueMap.Pnt[HueMap.Points-1].y = y;
#endif //unused
}

/***************************************************************************/
void Hue_SetPatch( HWND hDlg, ITEMID idPatch, int hue, BOOL bSetText)
/***************************************************************************/
{
COLOR Color;
RGBS rgb;
HSLS hsl;

hsl.hue = hue;
hsl.sat = 255;
hsl.lum = 127;
HSLtoRGB( hsl.hue, hsl.sat, hsl.lum, &rgb );
CopyRGB( &rgb, &Color );
SetWindowLong( GetDlgItem( hDlg, idPatch ), GWL_ACTIVE, Color );
AstralControlRepaint( hDlg, idPatch );

if (!bSetText)
	return;
if (WITHIN(idPatch, IDC_HUE1,IDC_HUE11))
	SetDlgItemIntExt(hDlg, (LONG)idPatch-(LONG)IDC_HUE1+(LONG)IDC_MAP0OUT, TOANGLE(hue), "\260");
else if (WITHIN(idPatch, IDC_INHUE1,IDC_INHUE11))
	SetDlgItemIntExt(hDlg, (LONG)idPatch-(LONG)IDC_INHUE1+(LONG)IDC_MAP0IN, TOANGLE(hue), "\260");
}


// LOCAL prototypes
LOCAL BOOL HueShift_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void HueShift_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void HueShift_OnDestroy(HWND hDlg);
LOCAL void HueShift_OnTimer(HWND hwnd, UINT id);
LOCAL void HueShift_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void HueShift_OnLButtonUp(HWND hDlg, int x, int y, UINT keyFlags);
LOCAL void HueShift_SetControls( HWND hDlg, BOOL bUpdateHueVal);
LOCAL void HueShift_Make_Map();

// statics used for Hue Dialog
static LPHUE_PARMS HueShift_lpParms;
static int InHue, OutHue;
static BOOL bColorize;

#define HUESHIFT_DELTA ((OutHue-InHue + MAX_HUES ) % MAX_HUES)

/***********************************************************************/
BOOL WINPROC EXPORT DlgHueShiftProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch (msg)
	{
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, HueShift_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_DESTROY, HueShift_OnDestroy);
		HANDLE_DLGMSG(hDlg, WM_TIMER, HueShift_OnTimer);
		HANDLE_DLGMSG(hDlg, WM_MOUSEMOVE, HueShift_OnMouseMove);
		HANDLE_DLGMSG(hDlg, WM_LBUTTONUP, HueShift_OnLButtonUp);
		HANDLE_DLGMSG(hDlg, WM_COMMAND, HueShift_OnCommand);
	
		HANDLE_DLGRET(hDlg, WM_QUERYNEWPALETTE, Dialog_OnQueryNewPalette);
	
		default:
		return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_HUE_SHIFT);
	}
}

/************************************************************************/
LOCAL BOOL HueShift_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	RightPopup( hDlg );
	HueShift_lpParms = (LPHUE_PARMS)lParam;
	InHue = 0;
	OutHue = 0;
	bColorize = FALSE;
	CheckDlgButton( hDlg, IDC_HUE_COLORIZE, bColorize);
	CheckDlgButton( hDlg, IDC_AUTO, Mapping.HueShift_bAuto);
	ControlEnable(hDlg, IDC_PREVIEW, !Mapping.HueShift_bAuto);
	ResetMap( &HueMap, HUEMAPPOINTS, NO );
	ResetMap( &SatMap, MAPPOINTS, NO );
	ResetMapEx( &MasterMap, MAPPOINTS, NO, YES);
	// Initialize controls
	InitDlgItemSpin( hDlg, IDC_SATURATION, -SatMap.Brightness[0], YES, -100, 100 );
	InitSlide( hDlg, IDC_SATURATION_SCROLL, -SatMap.Brightness[0], -100, 100 );
	InitDlgItemSpin( hDlg, IDC_LIGHTNESS, -MasterMap.Brightness[0], YES, -100, 100 );
	InitSlide( hDlg, IDC_LIGHTNESS_SCROLL, -MasterMap.Brightness[0], -100, 100 );
	InitSlide( hDlg, IDC_SLIDEHUE1, 0, 0, TOANGLE(MAX_HUES-1) );
	InitDlgItemSpin( hDlg, IDC_HUE_VALUE, 0, YES, 0, TOANGLE(MAX_HUES-1) );
	
	// actually set controls
	HueShift_SetControls( hDlg, YES );
	return(TRUE);
}

/***************************************************************************/
LOCAL void HueShift_OnDestroy(HWND hDlg)
/***************************************************************************/
{
	AutoPreview_OnDestroy(hDlg);
	Probe_End(hDlg);
}

/************************************************************************/
LOCAL void HueShift_OnTimer(HWND hDlg, UINT id)
/************************************************************************/
{	
	if (!AutoPreview_OnTimer(hDlg, id))
		return;
		
	// got good timer
	MakeMap(&HueMap);
	MakeMap(&SatMap);
	MakeMap(&MasterMap);
	MapPreview(GetActiveDoc());
}

/***********************************************************************/
LOCAL void HueShift_OnMouseMove(HWND hDlg, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	COLORINFO Color;
	
	if (Probe_OnMouseMove(hDlg, x, y, keyFlags, &Color))
	{
		// probing with valid value
		Hue_SetPatch( hDlg, IDC_INHUE1, Color.hsl.hue);
	}
	else if (bProbing) 
	{
		// probing but no valid value
		Hue_SetPatch( hDlg, IDC_INHUE1, InHue);
	}
	
}

/***********************************************************************/
LOCAL void HueShift_OnLButtonUp(HWND hDlg, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	COLORINFO Color;
	BOOL bGotPoint;
	
	if (!bProbing)
		return;
	bGotPoint = Probe_OnLButtonUp(hDlg,x,y,keyFlags,&Color);
	if (!bGotPoint)
	{
		// unsucessful probe session
		Hue_SetPatch( hDlg, IDC_INHUE1, InHue);
		return;
	}
	else
	{
		OutHue = OutHue + Color.hsl.hue - InHue;
		OutHue = OutHue % MAX_HUES;
		InHue = Color.hsl.hue;
		HueShift_Make_Map();
		// reset controls
		HueShift_SetControls( hDlg, YES );
	}
}


/************************************************************************/
LOCAL void HueShift_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
	BOOL Bool;
	int i, newhue;
	HWND hActiveWnd;
	LPIMAGE lpImage = NULL;
	
	if (hActiveWnd = GetActiveDoc())
		lpImage = GetImagePtr(hActiveWnd);
	
	switch (id)
	{
		case IDC_SLIDEHUE1:
			if ( GetFocus() != GetDlgItem( hDlg, id ) )
				break;
			i = HandleSlide( hDlg, id, codeNotify, &Bool );
			// get the true hue
			newhue = ((InHue + FROMANGLE(i) + MAX_HUES ) % MAX_HUES);
			if ( OutHue != newhue )
			{
				OutHue = newhue;
				HueShift_Make_Map();
				HueShift_SetControls( hDlg, YES );
				AutoPreview(&HueMap, Mapping.HueShift_bAuto, Bool, hDlg, hActiveWnd);
			}
		break;
		
		case IDC_HUE_VALUE:
			if ( GetFocus() != GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			i = (int)GetDlgItemSpin( hDlg, id, &Bool, NO );
			newhue = ((InHue + FROMANGLE(i) + MAX_HUES ) % MAX_HUES);
			if ( OutHue != newhue )
			{
				OutHue = newhue;
				HueShift_Make_Map();
				HueShift_SetControls( hDlg, NO );
				AutoPreview(&HueMap, Mapping.HueShift_bAuto, NO, hDlg, hActiveWnd);
			}
		break;
	
		case IDC_HUE1:
			OutHue = InHue;
			HueShift_Make_Map();
			HueShift_SetControls( hDlg, YES );
			AutoPreview(&HueMap, Mapping.HueShift_bAuto, YES, hDlg, hActiveWnd);
		break;
	
		case IDC_MAP_PROBE:
			Probe_Begin(hDlg, codeNotify);
		break;
	
		case IDC_AUTO:
			Mapping.HueShift_bAuto = !Mapping.HueShift_bAuto;
			CheckDlgButton( hDlg, id, Mapping.HueShift_bAuto);
        	ControlEnable(hDlg, IDC_PREVIEW, !Mapping.HueShift_bAuto);
		   	AutoPreview(&HueMap, Mapping.HueShift_bAuto, YES, hDlg, hActiveWnd);
		break;
		
		case IDC_HUE_COLORIZE:
			bColorize = !bColorize;
			CheckDlgButton( hDlg, id, bColorize);
			HueShift_Make_Map();
		   	AutoPreview(&HueMap, Mapping.HueShift_bAuto, YES, hDlg, hActiveWnd);
		break;
	
		case IDC_SATURATION:
			if ( GetFocus() != GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			i = (int)GetDlgItemSpin( hDlg, IDC_SATURATION, &Bool, YES );
			if ( SatMap.Brightness[0] != -i )
			{
				SatMap.Brightness[0] = -i;
				SetSlide( hDlg, IDC_SATURATION_SCROLL, i );
				AutoPreview(&SatMap, Mapping.HueShift_bAuto, NO, hDlg, hActiveWnd);
			}
		break;
	
		case IDC_SATURATION_SCROLL:
			if ( GetFocus() != GetDlgItem( hDlg, id ) )
				break;
			i = HandleSlide( hDlg, id, codeNotify, &Bool );
			if ( SatMap.Brightness[0] != -i )
			{
				SatMap.Brightness[0] = -i;
				SetDlgItemSpin( hDlg, IDC_SATURATION, i, YES );
				AutoPreview(&SatMap, Mapping.HueShift_bAuto, Bool, hDlg, hActiveWnd);
			}
		break;
	
		case IDC_LIGHTNESS:
			if ( GetFocus() != GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			i = (int)GetDlgItemSpin( hDlg, IDC_LIGHTNESS, &Bool, YES );
			if ( MasterMap.Brightness[0] != -i )
			{
				MasterMap.Brightness[0] = -i;
				SetSlide( hDlg, IDC_LIGHTNESS_SCROLL, i );
				AutoPreview(&MasterMap, Mapping.HueShift_bAuto, NO, hDlg, hActiveWnd);
			}
		break;
	
		case IDC_LIGHTNESS_SCROLL:
			if ( GetFocus() != GetDlgItem( hDlg, id ) )
				break;
			i = HandleSlide( hDlg, id, codeNotify, &Bool);
			if ( MasterMap.Brightness[0] != -i )
			{
				MasterMap.Brightness[0] = -i;
				SetDlgItemSpin( hDlg, IDC_LIGHTNESS, i, YES );
  				AutoPreview(&MasterMap, Mapping.HueShift_bAuto, Bool, hDlg, hActiveWnd);
			}
		break;
	
		case IDC_PREVIEW:
			AutoPreview_Button(hActiveWnd, &HueMap);
		break;
	
		case IDC_RESETALL:
		case IDC_RESET:
			ResetMap( &HueMap, HUEMAPPOINTS, NO );
			ResetMap( &SatMap, MAPPOINTS, NO );
			ResetMapEx( &MasterMap, MAPPOINTS, NO, YES);
			OutHue = InHue;
			MakeMap(&SatMap);
			MakeMap(&MasterMap);
			HueShift_Make_Map();
			HueShift_SetControls( hDlg, YES );
			AutoPreview(&HueMap, Mapping.HueShift_bAuto, YES, hDlg, hActiveWnd);
		break;
	
		case IDOK:
			if ( !lpImage )
			{
				AstralDlgEnd( hDlg, TRUE );
				break;
			}
			UndoMapPreview(hActiveWnd/*FALSE*/);
			HueShift_Make_Map();
			map_to_mapinfo(&HueMap, &HueShift_lpParms->HueInfo);
			HueShift_lpParms->Saturation = SatMap.Brightness[0];
			HueShift_lpParms->Brightness = MasterMap.Brightness[0];
			ResetAllMaps();
			AnimateMap(hActiveWnd, NULL, YES);
			AstralDlgEnd( hDlg, TRUE );
		break;
	
		case IDCANCEL:
			if ( !lpImage )
			{
				AstralDlgEnd( hDlg, FALSE );
				break;
			}
			ResetAllMaps();
			AnimateMap(hActiveWnd, NULL, YES);
			UndoMapPreview(hActiveWnd/*FALSE*/);
			AstralDlgEnd( hDlg, FALSE );
		break;
	
	   default:
		break;
	   }
}

//************************************************************************=
LOCAL void HueShift_SetControls( HWND hDlg, BOOL bUpdateHueVal )
//************************************************************************=
{
	int delta, i;
	BOOL Bool;
	
	i = GetDlgItemSpin(hDlg, IDC_SATURATION, &Bool, YES );
	if (i != -SatMap.Brightness[0] || !Bool)
		SetDlgItemSpin( hDlg, IDC_SATURATION, -SatMap.Brightness[0], YES);
	SetSlide( hDlg, IDC_SATURATION_SCROLL, -SatMap.Brightness[0]);
	i = GetDlgItemSpin(hDlg, IDC_LIGHTNESS, &Bool, YES );
	if (i != -MasterMap.Brightness[0] || !Bool)
		SetDlgItemSpin( hDlg, IDC_LIGHTNESS, -MasterMap.Brightness[0], YES);
	SetSlide( hDlg, IDC_LIGHTNESS_SCROLL, -MasterMap.Brightness[0]);
	delta = HUESHIFT_DELTA;
	SetSlide( hDlg, IDC_SLIDEHUE1, TOANGLE(delta));
	if (bColorize)
		delta = OutHue;
	if (bUpdateHueVal)
	{
		i = GetDlgItemSpin(hDlg, IDC_HUE_VALUE, &Bool, YES );
		if (i != TOANGLE(delta) || !Bool)
			SetDlgItemSpin( hDlg, IDC_HUE_VALUE, TOANGLE(delta), YES);
	}
	Hue_SetPatch( hDlg, IDC_HUE1, OutHue );
	Hue_SetPatch( hDlg, IDC_INHUE1, InHue );
	HueMap.bModified = YES; // to force HSL mapping
}


//************************************************************************
//		HueShift_Make_Map
// DESCRIPTION:
//		Create Hue shift map points.
//************************************************************************
LOCAL void HueShift_Make_Map()
//************************************************************************
{
	int dx;
	
	if (bColorize)
	{
		HueMap.Points = 2;
		HueMap.Pnt[0].x = 0;
		HueMap.Pnt[1].x = 255;
		HueMap.Pnt[0].y = HueMap.Pnt[1].y = OutHue;
		return;
	}
	
	dx = OutHue - InHue;
	// find x=0
	HueMap.Pnt[0].x = 0;
	HueMap.Pnt[0].y = (MAX_HUES+dx) % MAX_HUES;
	// find x=MAX_HUES-1;
	HueMap.Pnt[3].x = MAX_HUES-1;
	HueMap.Pnt[3].y = (MAX_HUES-1+dx) % MAX_HUES;
	// find crossover point
	HueMap.Pnt[1].x = (MAX_HUES-dx) % MAX_HUES;
	HueMap.Pnt[1].y = MAX_HUES-1;
	HueMap.Pnt[2].x = (MAX_HUES-dx) % MAX_HUES;
	HueMap.Pnt[2].y = 0;
	HueMap.Points = 4;
}

// LOCAL prototypes
LOCAL BOOL MapLoad_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void MapLoad_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgMapLoadProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, MapLoad_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, MapLoad_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_MAPLOAD);
	}
}

/************************************************************************/
LOCAL BOOL MapLoad_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CenterPopup( hDlg );
InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.ColorMap, IDN_MAP );
lstrcpy( Names.Saved, Names.ColorMap );
return(TRUE);
}

/************************************************************************/
LOCAL void MapLoad_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
FNAME szFileName;

switch (id)
	{
	case IDOK:
	if ( !LookupExtFile(Names.ColorMap, szFileName, IDN_MAP) )
		break;
	if ( !LoadMap( &MasterMap, &RedMap, &GreenMap, &BlueMap, &BlackMap, szFileName ) )
		break;
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	lstrcpy( Names.ColorMap, Names.Saved );
	AstralDlgEnd( hDlg, FALSE );
	break;

	case IDC_EXTNAMES:
	GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
		Names.ColorMap, IDN_MAP, codeNotify );
	break;

	case IDC_EXTMANAGE:
	PopupMenu( hDlg, id, IDC_FILENAME );
	break;

	case IDC_ADDEXT:
	case IDC_DELETEEXT:
	case IDC_RENAMEEXT:
	ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME,
		Names.ColorMap, IDN_MAP, id, NO );
	break;

   default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL MapSave_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void MapSave_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgMapSaveProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, MapSave_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, MapSave_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_MAPSAVE);
	}
}

/************************************************************************/
LOCAL BOOL MapSave_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CenterPopup( hDlg );
InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.ColorMap, IDN_MAP );
lstrcpy( Names.Saved, Names.ColorMap );
return(TRUE);
}

/************************************************************************/
LOCAL void MapSave_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
FNAME szFileName;

switch (id)
	{
	case IDC_EXTNAMES:
	GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
		Names.ColorMap, IDN_MAP, codeNotify );
	break;

	case IDC_CLIPEXTMANAGE:
	PopupMenu( hDlg, id, IDC_FILENAME );
	break;

	case IDC_ADDEXT:
	case IDC_DELETEEXT:
	case IDC_RENAMEEXT:
	ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.ColorMap,
		IDN_MAP, id, NO );
	break;

	case IDOK:
	if ( LookupExtFile(Names.ColorMap, szFileName, IDN_MAP) )
		{
		if (AstralOKCancel(IDS_OVERWRITEEXTFILE,
			(LPTR)Names.ColorMap) == IDCANCEL)
			break;
		}
	else
		{
		if ( !CreateFileName( Names.ColorMap, IDN_MAP, szFileName ) )
			break;
		}
	if ( !SaveMap( &MasterMap, &RedMap, &GreenMap, &BlueMap, &BlackMap, szFileName ) )
		break;
	if ( !AddExtFile( Names.ColorMap, szFileName, IDN_MAP ) )
		{
		FileDelete( szFileName );
		break;
		}
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	lstrcpy( Names.ColorMap, Names.Saved );
	AstralDlgEnd( hDlg, FALSE );
	break;

   default:
	break;
   }
}

//************************************************************************=
MAP *ActivateChannel( WORD wChannel )
//************************************************************************=
{
	BOOL bReverse;
	int  ReverseLUTXY; // used to save the state of flag
	MAP *lpMap; //since this is a static pointer to a static structure,
	// don't make it a far pointer because the data segment might move

	switch ( wChannel )
	{
		case IDC_CAL_MASTER:
		case IDC_DO_ALL:
			lpMap = &MasterMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = NO;
			lpMap->Reverse = NO;  // master is always reversed
			lpMap->Reverse |= ReverseLUTXY;
		break;

		case IDC_DO_CYAN:
			lpMap = &RedMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = !lpMap->Reverse;
			lpMap->Reverse = YES;
			lpMap->Reverse |= ReverseLUTXY;
		break;

		case IDC_DO_MAGENTA:
			lpMap = &GreenMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = !lpMap->Reverse;
			lpMap->Reverse = YES;
			lpMap->Reverse |= ReverseLUTXY;
		break;

		case IDC_DO_YELLOW:
			lpMap = &BlueMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = !lpMap->Reverse;
			lpMap->Reverse = YES;
			lpMap->Reverse |= ReverseLUTXY;
		break;

		case IDC_DO_RED:
			lpMap = &RedMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = lpMap->Reverse;
			lpMap->Reverse = NO;
			lpMap->Reverse |= ReverseLUTXY;
		break;

		case IDC_DO_GREEN:
			lpMap = &GreenMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = lpMap->Reverse;
			lpMap->Reverse = NO;
			lpMap->Reverse |= ReverseLUTXY;
		break;

		case IDC_DO_BLUE:
			lpMap = &BlueMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = lpMap->Reverse;
			lpMap->Reverse = NO;
			lpMap->Reverse |= ReverseLUTXY;
		break;

		case IDC_DO_BLACK:
			lpMap = &BlackMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = NO;
			lpMap->Reverse = NO;
			lpMap->Reverse |= ReverseLUTXY;
		break;

		case IDC_CAL_BLACK:
			lpMap = &BlackMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = NO;
			lpMap->Reverse = NO;
			lpMap->Reverse |= ReverseLUTXY;
		break;

		case IDC_CAL_CYAN:
			lpMap = &RedMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = NO;
			lpMap->Reverse = NO;
			lpMap->Reverse |= ReverseLUTXY;
		break;

		case IDC_CAL_MAGENTA:
			lpMap = &GreenMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = NO;
			lpMap->Reverse = NO;
			lpMap->Reverse |= ReverseLUTXY;
		break;

		case IDC_CAL_YELLOW:
			lpMap = &BlueMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = NO;
			lpMap->Reverse = NO;
			lpMap->Reverse |= ReverseLUTXY;
		break;

		default:
			lpMap = &MasterMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = NO;
			lpMap->Reverse = NO;
			lpMap->Reverse |= ReverseLUTXY;
		break;
	}

	if ( bReverse )
		InvertMap( lpMap );

	return( lpMap );
}

/***************************************************************************/
MAP *ResetCBChannel( HWND hDlg, WORD wChannel, WORD wBand ,int mul)
/***************************************************************************/
{
	int Contrast;
	
	MAP *lpMap; //since this is a static pointer to a static structure,
	// don't make it a far pointer because the data segment might move

	lpMap = ActivateChannel( wChannel );

	if (wBand == BAND_ALL)
		Contrast = lpMap->Contrast;
	else
		Contrast = 0;
	Stick_SetValue(GetDlgItem(hDlg, IDC_CONTBRIT), lpMap->Brightness[wBand] * mul, 0);

	InitDlgItemSpin( hDlg, IDC_CONTRAST, 0, YES, -100, 100 );
	InitDlgItemSpin( hDlg, IDC_BRIGHTNESS, lpMap->Brightness[wBand] * mul, YES, -100, 100 );

	AstralControlRepaint( hDlg, IDC_CONTBRIT );

	return( lpMap );
}


	
//************************************************************************
//		ChannelsCombo_Setup
// DESCRIPTION:
//		Sets the given combo to be all applicable channels for the given
//		frame type.  Disables the combo for grayscale.
//************************************************************************
VOID ChannelsCombo_Setup(HWND hDlg, int idControl, FRMDATATYPE type, WORD wChannel)
//************************************************************************
{
	switch(type)
	{
		case FDT_PALETTECOLOR:
		case FDT_RGBCOLOR:
			iCombo[0] = IDC_DO_ALL;
			iCombo[1] = IDC_DO_RED;
			iCombo[2] = IDC_DO_GREEN;
			iCombo[3] = IDC_DO_BLUE;
			nCombo = 4;
		break;
		case FDT_CMYKCOLOR:
			iCombo[0] = IDC_DO_ALL;
			iCombo[1] = IDC_DO_CYAN;
			iCombo[2] = IDC_DO_MAGENTA;
			iCombo[3] = IDC_DO_YELLOW;
			iCombo[4] = IDC_DO_BLACK;
			nCombo = 5;
		break;
		case FDT_GRAYSCALE:
		default:
			iCombo[0] = IDC_DO_ALL;
			nCombo = 1; 
		break;
	}
	
	TableCheckComboItem(hDlg, idControl, &iCombo[0], nCombo, wChannel, NO);
	if (type == FDT_GRAYSCALE)
		ControlEnable(hDlg, idControl, NO);
}		


//************************************************************************
//		ChannelsCombo_Handle
// DESCRIPTION:
//		Hanldes combo set up by ChannelsCombo_Setup. 
// RETURNS:
//		Tthe item hit in the combo, or 0 for failure.
//
//************************************************************************
int ChannelsCombo_Handle(HWND hDlg, int idControl, UINT codeNotify)
//************************************************************************
{
	int id;
	
	id = TableHandleCombo( hDlg, &iCombo[0], nCombo, idControl, codeNotify);
	if (!WITHIN(id, IDC_DO_ALL, IDC_DO_BLACK))
		return(0);
	return(id);
}



//************************************************************************
//		ChannelsCombo_IsEnabled
// RETURNS:
//		TRUE if the given channel is available in the channels combo.
//		Must setup with ChannelsCombo_Setup. 
//************************************************************************
BOOL ChannelsCombo_IsEnabled(HWND hDlg, WORD wChannel)
//************************************************************************
{
	int i;
	for (i=0;i<nCombo;i++)
	{
		if(iCombo[i] == wChannel)
			return(TRUE);
	}
	return(FALSE);
}

//************************************************************************
//			order_points
// DESCRIPTION:
//			Puts points in ascending order, removing duplicates.
//			Will clear all 'extra' points (up to MAPPOINTS).
//			Assumes bounded x values.
//			points with x or y == INT_MAX are discarded.
// RETURNS:
//			The number of points in the final map.
//************************************************************************
LOCAL int order_points( LPPOINT lpPoints, int nPoints)
//************************************************************************
{
	int i, j, in, out;
	POINT ptTemp;
	
	if ( !lpPoints)
		return(0);

	// skip any invalid points
	for (in = out = 0; in<nPoints; in++)
	{
		if (lpPoints[in].x != INT_MAX && lpPoints[in].y != INT_MAX)
			lpPoints[out++] = lpPoints[in];
	}
			
	nPoints = out;
	if (nPoints < 1)
		return(0);
	if ( nPoints == 1)
	{
		out = 1;
		goto Clear;
	}
	
	// sort points by x value
	for (i = 1; i < nPoints; ++i)
	{
		ptTemp = lpPoints[i];
		for (j = i-1; j >= 0 && (lpPoints[j].x > ptTemp.x); --j)
			lpPoints[j+1] = lpPoints[j];
		lpPoints[j+1] = ptTemp;
	}
	// remove duplicates
	for(in = out = 1;in<nPoints;in++)
	{
		if (lpPoints[out-1].x == lpPoints[in].x)
		{
			if (lpPoints[in].x >= 255)
			{
				// overwrite duplicate x's with smallest y 
				lpPoints[out-1].y = min(lpPoints[in].y, lpPoints[out-1].y);
			}
			else
			{
				// overwrite duplicate x's with largest y 
				lpPoints[out-1].y = max(lpPoints[in].y, lpPoints[out-1].y);
			}
		}
		else
			lpPoints[out++] = lpPoints[in];
	}	
Clear:
	// clear unused points
	for (i=out;i<MAPPOINTS;i++)
		lpPoints[i].x = 0;
	return(out);	
	
}



//************************************************************************
//		AutoPreview_Button
// PARAMETERS:
//					HWND hPreviewWnd - The window with the image.
// DESCRIPTION:
//			call when preview button hit in mapping dialog w/ auto preview.
//************************************************************************
VOID AutoPreview_Button(HWND hPreviewWnd, LPMAP lpMap)
//************************************************************************
{
	if (lpMap)
		MakeMap(lpMap);
	MapPreview(hPreviewWnd);
}

//************************************************************************
//		AutoPreview
// PARAMETERS:
//					LPMAP lpMap- The map to preiw (must not be NULL).
//					BOOL bAuto- Auto preiew on?
//					BOOL bDoNow- Preview now or set timer. 
//					HWND hDlg - The dialog running the preview.
//					HWND hPreviewWnd - The window with the image.
//
// DESCRIPTION:
//			Handles regular and autopreview.  
//			Call after any action which changes the map.
//************************************************************************
VOID AutoPreview(LPMAP lpMap, BOOL bAuto, BOOL bDoNow, HWND hDlg, HWND hPreviewWnd)
//************************************************************************
{
	if (bAuto & !bDoNow)
	{
	 	// setup timer
		bPreviewTimer = SetTimer(hDlg, PREVIEW_TIMER, PREVIEW_WAIT, NULL);
		return;
	}
	if (bAuto)	
	{
			// auto preview
			if (lpMap)
				MakeMap( lpMap );
			MapPreview(hPreviewWnd);
	}
	else
	{
		// Unpreview
		UndoMapPreview(hPreviewWnd /*NO*/ );
		AnimateMap(hPreviewWnd, lpMap, YES);
	}
	// kill any timer
	if (bPreviewTimer)
	{
		KillTimer(hDlg, PREVIEW_TIMER);
		bPreviewTimer = FALSE;
	}
}


//************************************************************************
//		nAutoPreview	
// PARAMETERS:
//					HWND hDlg - The dialog running the preview.
//					HWND hPreviewWnd - The window with the image.
// DESCRIPTION:
//		Turns off both autopreview and palette animation
//************************************************************************
VOID UnAutoPreview(HWND hDlg, HWND hPreviewWnd)
//************************************************************************
{
	UndoMapPreview(hPreviewWnd /*FALSE*/);
	AnimateMap(hPreviewWnd, NULL, YES);
}


//************************************************************************
//		AutoPreview_OnDestroy
// PARAMETERS:
//					HWND hDlg - The dialog running the preview.
// DESCRIPTION:
//				Call when destroying any dialog which uses autopreviw
//************************************************************************
VOID AutoPreview_OnDestroy(HWND hDlg)
//************************************************************************
{
	if (bPreviewTimer)
	{
		KillTimer(hDlg, PREVIEW_TIMER);
		bPreviewTimer = FALSE;
	}
}



//************************************************************************
//		AutoPreview_OnTimer
// DESCRIPTION:
//			Call in response to WM_TIMER message from any dialog which 
//			uses autopreview stuff.
// RETURNS:
//		TRUE if the caller should preview.
//************************************************************************
BOOL AutoPreview_OnTimer(HWND hDlg, UINT id)
//************************************************************************
{
	
	if (id != PREVIEW_TIMER || !bPreviewTimer)
		return(FALSE);

	// kill the timer
	KillTimer(hDlg, PREVIEW_TIMER);
	bPreviewTimer = FALSE;
	return(TRUE);
}


//************************************************************************
//		Probe_Begin
// DESCRIPTION:
// 		Begins probing probing.  Call in response to probe button hit.  
//		Uses static bProbing.
// RETURNS:
//		TRUE if probing will occure
//************************************************************************
BOOL Probe_Begin(HWND hDlg, UINT codeNotify)
//************************************************************************
{
	// Be sure its not a double click
	if ( codeNotify == 1 || !GetActiveDoc())
		return(FALSE);
	if ( !QuickProbeStart( hDlg ) )
		return(FALSE);
	bProbing = TRUE;
    iBubbles = HelpContext.BubbleHintsOn(HL_BUBBLE_QUERY);
    HelpContext.BubbleHintsOn(FALSE);
	CheckDlgButton(hDlg, IDC_MAP_PROBE, bProbing);
	return(TRUE);
}


//************************************************************************
// RETURNS:
//		TRUE if a probe session is in effect.
//************************************************************************
BOOL Probe_IsProbing()
//************************************************************************
{
	return(bProbing);
}

//************************************************************************
//		Probe_OnMouseMove
// DESCRIPTION:
// 		Handles mouse move for probing.  
//		Uses statics bProbing.
//		Fills lpProbeColor.
// RETURNS:
//		TRUE if valid probe took place
//************************************************************************
BOOL Probe_OnMouseMove(HWND hDlg, int x, int y, UINT keyFlags, LPCOLORINFO lpProbeColor)
//************************************************************************
{
	if (bProbing)
		return(QuickProbeGet( hDlg, x, y, lpProbeColor ) );
	return(FALSE);
}

//************************************************************************
//		Probe_OnLButtonUp
// DESCRIPTION:
// 		Handles mouse up for probing.  
//		Ends probing (regardless of return value).
//		Uses static bProbing.
//		Fills lpProbeColor.
// RETURNS:
//		TRUE if a color was probed. 
//************************************************************************
BOOL Probe_OnLButtonUp(HWND hDlg, int x, int y, UINT keyFlags, LPCOLORINFO lpProbeColor)
//************************************************************************
{
	BOOL bGotPoint;
	
	if (!bProbing)
		return(FALSE);
	bGotPoint = QuickProbeGet( hDlg, x, y, lpProbeColor );
	Probe_End(hDlg);
	return(bGotPoint);
}



//************************************************************************
//		Probe_End
// DESCRIPTION:
// 		Cleans up probing dialog.  
//		Call when destroying dialog which can probe.  
//		Uses static bProbing.
//************************************************************************
void Probe_End(HWND hDlg)
//************************************************************************
{
	if (!bProbing)
		return;

	QuickProbeEnd( hDlg, -1, -1 );
    HelpContext.BubbleHintsOn(iBubbles);
	bProbing = FALSE;
	CheckDlgButton(hDlg, IDC_MAP_PROBE, bProbing);
}

//************************************************************************
//			HandleMapInit
// PARAMETERS:
//					HWND hDlg- The map dialog.
//					LPMAPVALUES lpValues - map values pointer
//					HWND hActiveWnd - active image window for previewing
// DESCRIPTION:
//				Handle WM_INITDIALOG for any mapping dialog
//************************************************************************
void HandleMapInit(HWND hDlg, LPMAPVALUES lpValues, HWND hActiveWnd,
				FRMDATATYPE type, BOOL fUseProbe)
//************************************************************************
{
	int x,y;
	HWND hMap;
	WORD wStyles;

	lpValues->fModified = FALSE;

	MapSetupPanes(hDlg, lpValues, fUseProbe);
	
	if (GetDlgItem(hDlg, IDC_CHANNELS))
		ChannelsCombo_Setup(hDlg, IDC_CHANNELS, type, lpValues->wChannel);
	
	// remove or add percents
	SetAllDlgItemsText(hDlg, IDC_PERCENT, Mapping.Map_bPercent ? (LPSTR)"%":(LPSTR)"");
	
	// initialize the map conrol 
	wStyles = 0;
	if (!Mapping.Map_bIntensity)
		wStyles |= (MS_INVERTX | MS_INVERTY);
	if (Mapping.Map_bGrid)
		x = y = GRIDSIZE;
	else
		x = y = 0;	
	hMap = GetDlgItem( hDlg, IDC_MAP );
	Map_Init(hMap, 
				x, // gridX 
				y, // gridX 
				1.0, // gamma 
				11, // MaxPoints
				wStyles); // styles
		
	InitSlide( hDlg, IDC_MAP_GAMMA, 100 ,1 , 200 );
	// set gamma value display
		// set gamma value display
	AstralSetDlgItemFixed(hDlg, IDC_MAP_GAMMA_TEXT, DBL2FIX(1.0), YES);
	
	MapSetGradients(hDlg, Mapping.Map_bIntensity, lpValues->wChannel);
	
	// ensure that the correct window is showing
	lpValues->bMap_Visual = NO;
	MapActivatePane(hDlg, IDC_MAPTOOL3, lpValues);
	// get current channel
	lpValues->pMap = ResetMapChannel( hDlg, lpValues->wChannel );
	if (GetDlgItem(hDlg, IDC_PREVIEW))
		ControlEnable(hDlg, IDC_PREVIEW, !Mapping.Map_bAuto && hActiveWnd);
	ControlEnable(hDlg, IDOK, lpValues->fModified);
}

//************************************************************************
//			HandleMapCommand
// PARAMETERS:
//					HWND hDlg- The map dialog.
//					ITEMID id- The id of the map control.
//					UINT codeNotify- notification code from map control
//					LPMAPVALUES lpValues - map values pointer
//					HWND hActiveWnd - active image window for previewing
// DESCRIPTION:
//				Handle WM_COMMAND's for any mapping dialog
//************************************************************************
BOOL HandleMapCommand(HWND hDlg, ITEMID id, UINT codeNotify,
						LPMAPVALUES lpValues, HWND hActiveWnd)
//************************************************************************
{
	BOOL fModified;

	switch (id)
	{
	case IDC_CHANNELS:
		fModified = HandleMapChannels(hDlg, id, codeNotify, lpValues, hActiveWnd);
	break;

	case IDC_MAP:
		fModified = HandleMap(hDlg, id, codeNotify, lpValues, hActiveWnd);
	break;

	case IDC_PREVIEW:
		fModified = HandleMapPreview(hDlg, id, codeNotify, lpValues, hActiveWnd);
	break;

	case IDC_MAPTOOL:
		fModified = HandleMapTool(hDlg, id, codeNotify, lpValues, hActiveWnd);
	break;

	case IDC_MAP0IN:
	case IDC_MAP1IN:
	case IDC_MAP2IN:
	case IDC_MAP3IN:
	case IDC_MAP4IN:
	case IDC_MAP5IN:
	case IDC_MAP6IN:
	case IDC_MAP7IN:
	case IDC_MAP8IN:
	case IDC_MAP9IN:
	case IDC_MAP10IN:
		fModified = HandleMapIn(hDlg, id, codeNotify, lpValues, hActiveWnd);
	break;

	case IDC_MAP0OUT:
	case IDC_MAP1OUT:
	case IDC_MAP2OUT:
	case IDC_MAP3OUT:
	case IDC_MAP4OUT:
	case IDC_MAP5OUT:
	case IDC_MAP6OUT:
	case IDC_MAP7OUT:
	case IDC_MAP8OUT:
	case IDC_MAP9OUT:
	case IDC_MAP10OUT:
		fModified = HandleMapOut(hDlg, id, codeNotify, lpValues, hActiveWnd);
	break;

	case IDC_MAP_OPTIONS:
		fModified = HandleMapOptions(hDlg, id, codeNotify, lpValues, hActiveWnd);
	break;

	case IDC_MAP_CURVES:
		fModified = HandleMapCurves(hDlg, id, codeNotify, lpValues, hActiveWnd);
	break;

	case IDC_MAP_GAMMA:
		fModified = HandleMapGamma(hDlg, id, codeNotify, lpValues, hActiveWnd);
	break;

	case IDC_MAP_SHIFT_UP:
	case IDC_MAP_SHIFT_DOWN: 
	case IDC_MAP_SHIFT_LEFT: 
	case IDC_MAP_SHIFT_RIGHT:
		fModified = HandleMapShift(hDlg, id, codeNotify, lpValues, hActiveWnd);
	break;

	case IDC_RESET:
		fModified = HandleReset(hDlg, id, codeNotify, lpValues, hActiveWnd);
	break;

	case IDC_RESETALL:
		fModified = HandleResetAll(hDlg, id, codeNotify, lpValues, hActiveWnd);
	break;

	case IDC_MAP_HBAR:
	case IDC_MAP_VBAR:
		fModified = HandleMapGradient(hDlg, id, codeNotify, lpValues, hActiveWnd);
	break;

	case IDC_MAP_PROBE:
		fModified = HandleMapProbe(hDlg, id, codeNotify, lpValues, hActiveWnd);
	break;

	default:
		return(FALSE);
	}
	if (fModified)
		lpValues->fModified = TRUE;
	return(TRUE);
}

//************************************************************************
//			HandleMap
// PARAMETERS:
//					HWND hDlg- The map dialog.
//					ITEMID id- The id of the map control.
//					UINT codeNotify- notification code from map control
//					LPMAPVALUES lpValues - map values pointer
//					HWND hActiveWnd - active image window for previewing
// DESCRIPTION:
//				Handle WM_COMMAND from map control.
//************************************************************************
BOOL HandleMap(HWND hDlg, ITEMID id, UINT codeNotify,
						LPMAPVALUES lpValues, HWND hActiveWnd)
//************************************************************************
{
	HWND hMap;
	POINT pt;

	hMap = GetDlgItem(hDlg, id);
	if ( codeNotify == MN_POINTCHANGED || codeNotify == MN_POINTCHANGING)
	{
		if (!Map_GetActivePoint(hMap, &pt))
			pt.x = pt.y = -1;
		
		lpValues->pMap->Points = Map_GetPointList(hMap, lpValues->pMap->Pnt, MAPPOINTS); 
		if (hActiveWnd)
		{
			if (codeNotify == MN_POINTCHANGED)
				AutoPreview(lpValues->pMap, Mapping.Map_bAuto, YES, hDlg, hActiveWnd);
			else
				AutoPreview(lpValues->pMap, Mapping.Map_bAuto, NO, hDlg, hActiveWnd);
		}
	}
	else if (codeNotify == MN_BROWSE)
	{
		if (!Map_GetBrowsePoint(hMap, &pt))
			pt.x = pt.y = -1;
	}
	else
		return(FALSE);
	DisplayInOutValues(hDlg, pt.x, pt.y);
	return(TRUE);
}

//************************************************************************
//			HandleMapIn
// PARAMETERS:
//					HWND hDlg- The map dialog.
//					ITEMID id- The id of the map control.
//					UINT codeNotify- notification code from map control
//					LPMAPVALUES lpValues - map values pointer
//					HWND hActiveWnd - active image window for previewing
// DESCRIPTION:
//				Handle WM_COMMAND from map control.
//************************************************************************
BOOL HandleMapIn(HWND hDlg, ITEMID id, UINT codeNotify,
						LPMAPVALUES lpValues, HWND hActiveWnd)
//************************************************************************
{
	int i;
	BOOL Bool;

 	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		return(FALSE);
	if ( codeNotify != EN_CHANGE)
		return(FALSE);
	i = GetDlgItemSpin( hDlg, id, &Bool, NO );
	if (!Bool)
	{
		if (lpValues->ptNumeric[id-IDC_MAP0IN].x == INT_MAX)
			return(FALSE);
		lpValues->ptNumeric[id-IDC_MAP0IN].x = INT_MAX;
	}
	else
		lpValues->ptNumeric[id-IDC_MAP0IN].x = DISPLAY_TO_MAP(i);
	if (lpValues->ptNumeric[id-IDC_MAP0IN].y == INT_MAX)
		return(FALSE);
	UpdateCurrentMap(hDlg, lpValues);
	if (hActiveWnd)
		AutoPreview(lpValues->pMap, Mapping.Map_bAuto, YES, hDlg, hActiveWnd);
	return(TRUE);
}

//************************************************************************
//			HandleMapOut
// PARAMETERS:
//					HWND hDlg- The map dialog.
//					ITEMID id- The id of the map control.
//					UINT codeNotify- notification code from map control
//					LPMAPVALUES lpValues - map values pointer
//					HWND hActiveWnd - active image window for previewing
// DESCRIPTION:
//				Handle WM_COMMAND from map control.
//************************************************************************
BOOL HandleMapOut(HWND hDlg, ITEMID id, UINT codeNotify,
						LPMAPVALUES lpValues, HWND hActiveWnd)
//************************************************************************
{	
	int i;
	BOOL Bool;

	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		return(FALSE);
	if ( codeNotify != EN_CHANGE)
		return(FALSE);
	i = GetDlgItemSpin(hDlg, id, &Bool, NO );
	if (!Bool)
	{
		if (lpValues->ptNumeric[id-IDC_MAP0IN].y == INT_MAX)
			return(FALSE);
		lpValues->ptNumeric[id-IDC_MAP0OUT].y = INT_MAX;
	}
	else
		lpValues->ptNumeric[id-IDC_MAP0OUT].y = DISPLAY_TO_MAP(i);
	if (lpValues->ptNumeric[id-IDC_MAP0OUT].x == INT_MAX)
		return(FALSE);
	UpdateCurrentMap(hDlg, lpValues);
	if (hActiveWnd)
		AutoPreview(lpValues->pMap, Mapping.Map_bAuto, YES, hDlg, hActiveWnd);
	return(TRUE);
}

//************************************************************************
//			HandleMapOptions
// PARAMETERS:
//					HWND hDlg- The map dialog.
//					LPMAPVALUES lpValues - map values pointer
//					HWND hActiveWnd - active image window for previewing
// DESCRIPTION:
//				Handle WM_COMMAND from map options button
//************************************************************************
BOOL HandleMapOptions(HWND hDlg, ITEMID id, UINT codeNotify,
						LPMAPVALUES lpValues, HWND hActiveWnd)
//************************************************************************
{
	HWND hMap;
	int x, y;
	
	if ( !AstralDlg( NO, PictPubApp.GetResourceHandle(), hDlg, IDD_MAP_OPTIONS,
		DlgMapOptionsProc ) )
		return(FALSE);

	// remove or add percents
	SetAllDlgItemsText(hDlg, IDC_PERCENT, Mapping.Map_bPercent ? (LPSTR)"%":(LPSTR)"");

	// set curves, etc
	if (lpValues->pMap->iCurves)
		lpValues->pMap->iCurves = Mapping.Map_CurveType;
		
	Map_DlgSetNewMap(hDlg, lpValues->pMap);
	// turn grid on/off
	if (Mapping.Map_bGrid)
		x = y = GRIDSIZE;
	else
		x = y = 0;
	hMap = GetDlgItem(hDlg, IDC_MAP);
	Map_Init(hMap, x, y, -1.0, -1, -1);
	UpdateCurrentMap(hDlg, lpValues);
	if (hActiveWnd)
	{
		AutoPreview(lpValues->pMap, Mapping.Map_bAuto, YES, hDlg, hActiveWnd);
		ControlEnable(hDlg, IDC_PREVIEW, !Mapping.Map_bAuto);
	}
	return(TRUE);
}

//************************************************************************
//			HandleMapCurves
// PARAMETERS:
//					HWND hDlg- The map dialog.
//					ITEMID id - id of curve button
//					LPMAPVALUES lpValues - map values pointer
//					HWND hActiveWnd - active image window for previewing
// DESCRIPTION:
//				Handle WM_COMMAND from map curves button
//************************************************************************
BOOL HandleMapCurves(HWND hDlg, ITEMID id, UINT codeNotify,
						LPMAPVALUES lpValues, HWND hActiveWnd)
//************************************************************************
{	
	HWND hMap;
	
	hMap = GetDlgItem(hDlg, IDC_MAP);
	Mapping.Map_bCurves = (lpValues->pMap->iCurves == 0);
	lpValues->pMap->iCurves = Mapping.Map_bCurves ? Mapping.Map_CurveType : 0;
	SetWindowWord(GetDlgItem(hDlg, id), GWW_ICONID, lpValues->pMap->iCurves ? IDC_MAP_BEZIER:IDC_MAP_LINES);
	Map_SetCurveType(hMap, Mapping.Map_CurveType);
	Map_SetStyle(hMap, MS_CURVES, lpValues->pMap->iCurves);
	//			MasterMap.iCurves = RedMap.iCurves = Mapping.Map_bCurves;
	//			GreenMap.iCurves = BlueMap.iCurves = Mapping.Map_bCurves;
	//			BlackMap.iCurves = Mapping.Map_bCurves;
	// get the current map set
	UpdateCurrentMap(hDlg, lpValues);
	if (hActiveWnd)
		AutoPreview(lpValues->pMap, Mapping.Map_bAuto, YES, hDlg, hActiveWnd);
	return(TRUE);
}

//************************************************************************
//			HandleMapTool
// PARAMETERS:
//					HWND hDlg- The map dialog.
//					ITEMID id- The id of the map control.
//					UINT codeNotify- notification code from map control
//					LPMAPVALUES lpValues - map values pointer
//					HWND hActiveWnd - active image window for previewing
// DESCRIPTION:
//				Handle WM_COMMAND from map control.
//************************************************************************
BOOL HandleMapTool(HWND hDlg, ITEMID id, UINT codeNotify,
						LPMAPVALUES lpValues, HWND hActiveWnd)
//************************************************************************
{	
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		return(FALSE);

	// no cange? 
	if (id == IDC_MAPTOOLV && !lpValues->bMap_Visual)
		return(FALSE);
	if (id != IDC_MAPTOOLV && lpValues->bMap_Visual)
		return(FALSE);

	if (hActiveWnd)
		UndoMapPreview(hActiveWnd/* NO */);

	// get the current map set
	UpdateCurrentMap(hDlg, lpValues);
	MapActivatePane( hDlg, id, lpValues );
	lpValues->pMap = ResetMapChannel( hDlg, lpValues );
	if (hActiveWnd)
		AutoPreview(lpValues->pMap, Mapping.Map_bAuto, YES, hDlg, hActiveWnd);
	return(FALSE);
}

//************************************************************************
//			HandleResetAll
// PARAMETERS:
//					HWND hDlg- The map dialog.
//					ITEMID id- The id of the map control.
//					UINT codeNotify- notification code from map control
//					LPMAPVALUES lpValues - map values pointer
//					HWND hActiveWnd - active image window for previewing
// DESCRIPTION:
//				Handles IDC_RESETALL
//************************************************************************
BOOL HandleResetAll(HWND hDlg, ITEMID id, UINT codeNotify,
						LPMAPVALUES lpValues, HWND hActiveWnd)
//************************************************************************
{	
	ResetAllMapsEx(-1, -1);

	lpValues->pMap = ResetMapChannel( hDlg, lpValues->wChannel );
	if (hActiveWnd)
		AutoPreview(lpValues->pMap, Mapping.Map_bAuto, YES, hDlg, hActiveWnd);
	return(TRUE);
}

//************************************************************************
//			HandleReset
// PARAMETERS:
//					HWND hDlg- The map dialog.
//					ITEMID id- The id of the map control.
//					UINT codeNotify- notification code from map control
//					LPMAPVALUES lpValues - map values pointer
//					HWND hActiveWnd - active image window for previewing
// DESCRIPTION:
//				Handles IDC_RESET
//************************************************************************
BOOL HandleReset(HWND hDlg, ITEMID id, UINT codeNotify,
						LPMAPVALUES lpValues, HWND hActiveWnd)
//************************************************************************
{	
	ResetMapEx( lpValues->pMap, Mapping.Map_nPoints, -1, lpValues->wChannel == IDC_DO_ALL);
	lpValues->pMap->iCurves = Mapping.Map_bCurves ? Mapping.Map_CurveType:0;
	lpValues->pMap = ResetMapChannel( hDlg, lpValues->wChannel );
	if (hActiveWnd)
		AutoPreview(lpValues->pMap, Mapping.Map_bAuto, YES, hDlg, hActiveWnd);
	return(TRUE);
}

//************************************************************************
//			HandleMapGamma
// PARAMETERS:
//					HWND hDlg- The map dialog.
//					ITEMID id- The id of the map control.
//					UINT codeNotify- notification code from map control
//					LPMAPVALUES lpValues - map values pointer
//					HWND hActiveWnd - active image window for previewing
// DESCRIPTION:
//				Handles IDC_MAP_GAMMA
//************************************************************************
BOOL HandleMapGamma(HWND hDlg, ITEMID id, UINT codeNotify,
						LPMAPVALUES lpValues, HWND hActiveWnd)
//************************************************************************
{	
	double gamma;
	int i;
	HWND hMap;
	POINT pt;
	char szFixed[32];

	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		return(FALSE);
	// get gamma value
	i = HandleSlide( hDlg, id, codeNotify, NULL );
	if (i > 100)
		gamma = (1.0 + (200.0-i)*8.0/100)/9.0;
	else
		gamma = (100.0 - i)/5+1.0;

	// set gamma value display
	FixedAscii( DBL2FIX((1.0/gamma)), szFixed, 2 );
	SetDlgItemText( hDlg, IDC_MAP_GAMMA_TEXT, szFixed );
	// set gamma in map control
	hMap = GetDlgItem(hDlg, IDC_MAP);
	Map_SetGamma(hMap, gamma);
	lpValues->pMap->gamma = gamma;

	if (Map_GetActivePoint(hMap, &pt))
		DisplayInOutValues(hDlg, pt.x, pt.y);

	if (codeNotify == SN_VALUESELECTED)
	{
		// get the current map set
		UpdateCurrentMap(hDlg, lpValues);
		if (hActiveWnd)
			AutoPreview(lpValues->pMap, Mapping.Map_bAuto, YES, hDlg, hActiveWnd);
	}
	else
	{
		if (hActiveWnd)
			AutoPreview(lpValues->pMap, Mapping.Map_bAuto, NO, hDlg, hActiveWnd);
	}
	return(TRUE);
}

//************************************************************************
//			HandleMapShift
// PARAMETERS:
//					HWND hDlg- The map dialog.
//					ITEMID id- The id of the map control.
//					UINT codeNotify- notification code from map control
//					LPMAPVALUES lpValues - map values pointer
//					HWND hActiveWnd - active image window for previewing
// DESCRIPTION:
//				Handles IDC_SHIFT_???
//************************************************************************
BOOL HandleMapShift(HWND hDlg, ITEMID id, UINT codeNotify,
						LPMAPVALUES lpValues, HWND hActiveWnd)
//************************************************************************
{	
	int x, y, num, i;
	HWND hMap;

	// shift entire map
	x = ((id==IDC_MAP_SHIFT_LEFT) ? 5 : ((id==IDC_MAP_SHIFT_RIGHT) ? -5:0));
	y = ((id==IDC_MAP_SHIFT_UP) ? 5 : ((id==IDC_MAP_SHIFT_DOWN) ? -5:0));
	if (!Mapping.Map_bIntensity)
	{
		x = -x;
		y = -y;
	}
	hMap = GetDlgItem( hDlg, IDC_MAP );
	num = Map_GetPointList(hMap, lpValues->pMap->Pnt, MAPPOINTS, NO); 
	lpValues->pMap->Points = num;
	for(i=0;i<num;i++)
	{
		lpValues->pMap->Pnt[i].x += x;
		lpValues->pMap->Pnt[i].y += y;
	}
	Map_SetPointList(hMap, lpValues->pMap->Pnt, num);
	// to ensure bounded values - get 'em again
	lpValues->pMap->Points = Map_GetPointList(hMap, lpValues->pMap->Pnt, MAPPOINTS); 
	if (hActiveWnd)
		AutoPreview(lpValues->pMap, Mapping.Map_bAuto, NO, hDlg, hActiveWnd);
	return(TRUE);
}

//************************************************************************
//			HandleMapPreview
// PARAMETERS:
//					HWND hDlg- The map dialog.
//					ITEMID id- The id of the map control.
//					UINT codeNotify- notification code from map control
//					LPMAPVALUES lpValues - map values pointer
//					HWND hActiveWnd - active image window for previewing
// DESCRIPTION:
//				Handles IDC_PREVIEW_???
//************************************************************************
BOOL HandleMapPreview(HWND hDlg, ITEMID id, UINT codeNotify,
						LPMAPVALUES lpValues, HWND hActiveWnd)
//************************************************************************
{	
	UpdateCurrentMap(hDlg, lpValues);
	AutoPreview_Button(hActiveWnd, lpValues->pMap);
	return(FALSE);
}

//************************************************************************
//			HandleMapLoad
// PARAMETERS:
//					HWND hDlg- The map dialog.
//					LPMAPVALUES lpValues - map values pointer
//					HWND hActiveWnd - active image window for previewing
// DESCRIPTION:
//				Handles reset mapping stuff after a load
//************************************************************************
void HandleMapLoad(HWND hDlg, LPMAPVALUES lpValues, HWND hActiveWnd)
//************************************************************************
{
	lpValues->pMap = ResetMapChannel( hDlg, lpValues->wChannel );
	AutoPreview(lpValues->pMap, Mapping.Map_bAuto, YES, hDlg, hActiveWnd);
	lpValues->fModified = TRUE;
	ControlEnable(hDlg, IDOK, lpValues->fModified);
}

//************************************************************************
//			HandleMapChannels
// PARAMETERS:
//					HWND hDlg- The map dialog.
//					ITEMID id- The id of the map control.
//					UINT codeNotify- notification code from map control
//					LPMAPVALUES lpValues - map values pointer
//					HWND hActiveWnd - active image window for previewing
// DESCRIPTION:
//				Handles IDC_CHANNELS
//************************************************************************
BOOL HandleMapChannels(HWND hDlg, ITEMID id, UINT codeNotify,
						LPMAPVALUES lpValues, HWND hActiveWnd)
//************************************************************************
{	
	int i;

	i=ChannelsCombo_Handle(hDlg, id, codeNotify);
	if (!i)
		return(FALSE);
	UpdateCurrentMap(hDlg, lpValues);
	lpValues->wChannel = i;
	MapSetGradients(hDlg, Mapping.Map_bIntensity, lpValues->wChannel);
	lpValues->pMap = ResetMapChannel( hDlg, lpValues->wChannel );
	return(FALSE);
}

//************************************************************************
//			HandleMapGradient
// PARAMETERS:
//					HWND hDlg- The map dialog.
//					ITEMID id- The id of the map control.
//					UINT codeNotify- notification code from map control
//					LPMAPVALUES lpValues - map values pointer
//					HWND hActiveWnd - active image window for previewing
// DESCRIPTION:
//				Handles IDC_VBAR, IDC_HBAR
//************************************************************************
BOOL HandleMapGradient(HWND hDlg, ITEMID id, UINT codeNotify,
						LPMAPVALUES lpValues, HWND hActiveWnd)
//************************************************************************
{	
	Mapping.Map_bIntensity = !Mapping.Map_bIntensity;

	MapSetGradients(hDlg, Mapping.Map_bIntensity, lpValues->wChannel);
	// set invert syles
	Map_SetStyle(GetDlgItem(hDlg, IDC_MAP), MS_INVERTX | MS_INVERTY, !Mapping.Map_bIntensity);
	return(FALSE);
}

//************************************************************************
//			HandleMapProbe
// PARAMETERS:
//					HWND hDlg- The map dialog.
//					ITEMID id- The id of the map control.
//					UINT codeNotify- notification code from map control
//					LPMAPVALUES lpValues - map values pointer
//					HWND hActiveWnd - active image window for previewing
// DESCRIPTION:
//				Handles IDC_MAP_PROBE
//************************************************************************
BOOL HandleMapProbe(HWND hDlg, ITEMID id, UINT codeNotify,
						LPMAPVALUES lpValues, HWND hActiveWnd)
//************************************************************************
{	
	if (bProbing)
		Probe_End(hDlg);
	else
		Probe_Begin(hDlg, codeNotify);
	return(FALSE);
}

//************************************************************************
//			HandleMapEnd
// PARAMETERS:
//					HWND hDlg- The map dialog.
//					HWND hActiveWnd - active image window for previewing
// DESCRIPTION:
//				Handles IDOK and IDCANCEL
//************************************************************************
void HandleMapEnd(HWND hDlg, HWND hActiveWnd)
//************************************************************************
{	
	
	UnAutoPreview(hDlg, hActiveWnd);
	ResetAllMapsEx(NO);
}

//************************************************************************
//			HandleDestroy
// PARAMETERS:
//					HWND hDlg- The map dialog.
// DESCRIPTION:
//				Handles WM_DESTROY
//************************************************************************
void HandleMapDestroy(HWND hDlg)
//************************************************************************
{	
	AutoPreview_OnDestroy(hDlg);
	Probe_End(hDlg);
}

//************************************************************************
//			HandleMapMouseMove
// PARAMETERS:
//					HWND hDlg- The map dialog.
// DESCRIPTION:
//				Handles WM_MOUSEMOVE
/***********************************************************************/
void HandleMapMouseMove(HWND hDlg, int x, int y, UINT keyFlags,
							LPMAPVALUES lpValues)
/***********************************************************************/
{
	POINT pt;
	COLORINFO Color;
	HWND hMap;
	
	if (Probe_OnMouseMove(hDlg, x, y, keyFlags, &Color))
	{
		hMap = GetDlgItem( hDlg, IDC_MAP);
		pt.x = Map_Get_Color(&Color, lpValues->wChannel);
		pt.y = Map_SetProbe(hMap, pt.x);
		DisplayInOutValues(hDlg, pt.x, pt.y);
	}
	else if (bProbing) // probing but no valid value
	{
		hMap = GetDlgItem( hDlg, IDC_MAP);
	   	Map_ClearProbe(hMap);
	   	DisplayInOutValues(hDlg, -1, -1);
	}
}

//************************************************************************
//			HandleMapLButtonUp
// PARAMETERS:
//					HWND hDlg- The map dialog.
// DESCRIPTION:
//				Handles WM_LBUTTONUP
/***********************************************************************/
void HandleMapLButtonUp(HWND hDlg, int x, int y, UINT keyFlags, LPMAPVALUES lpValues)
/***********************************************************************/
{
	COLORINFO Color;
	POINT pt;
	HWND hMap;
	BOOL bGotPoint;
	
	if (!bProbing)
		return;
	bGotPoint = Probe_OnLButtonUp(hDlg,x,y,keyFlags,&Color);
	hMap = GetDlgItem( hDlg, IDC_MAP);
	if (bGotPoint)
	{
		// got a color - get x & y values
		pt.x = Map_Get_Color(&Color, lpValues->wChannel);
		pt.y = Map_SetProbe(hMap, pt.x);
		Map_ClearProbe(hMap);
		Map_AddPoint(hMap, &pt);
		lpValues->fModified = TRUE;
	}
	else
	{
		// unsucessful probe session
		Map_ClearProbe(hMap);
		if (!Map_GetActivePoint(hMap, &pt))
			pt.x = pt.y = -1;
	}
	// update readouts
	DisplayInOutValues(hDlg, pt.x, pt.y);
}

//************************************************************************
//		UpdateCurrentMap
// DESCRIPTION:
//			Sets lpMap's points based on the Map Control or the Numeric 
//			Combos.
//************************************************************************
VOID UpdateCurrentMap(HWND hDlg, LPMAPVALUES lpValues)
//************************************************************************
{
	HWND hMap;

	if (lpValues->bMap_Visual)
	{
		hMap = GetDlgItem(hDlg, IDC_MAP);
		lpValues->pMap->Points = Map_GetPointList(hMap, lpValues->pMap->Pnt, MAPPOINTS); 
	}
	else
	{
		lpValues->pMap->Points = MAPPOINTS;
		copy((LPTR)lpValues->ptNumeric, (LPTR)lpValues->pMap->Pnt, MAPPOINTS*sizeof(POINT));
		lpValues->pMap->Points = order_points( lpValues->pMap->Pnt, lpValues->pMap->Points);
	}
}

//************************************************************************
//			DisplayInOutValues
// PARAMETERS:
//					HWND hDlg- The map dialog.
//					int x- The x value (map coords) to display, -1 for none.
//					int y- The x value (map coords) to display, -1 for none.
// DESCRIPTION:
//				Displays the input and output values for the mapping dialog.
//************************************************************************
VOID DisplayInOutValues(HWND hDlg, int x, int y)
//************************************************************************
{

	if (x >= 0)
	{
		x = MAP_TO_DISPLAY( x );
		SetDlgItemIntExt( hDlg,
			IDC_MAPIN,  x, Mapping.Map_bPercent ? (LPSTR)"%":(LPSTR)"");
	}
	else
		SetDlgItemText(hDlg,
			IDC_MAPIN, "");
	if (y >= 0)
	{
		y = MAP_TO_DISPLAY( y );
		SetDlgItemIntExt( hDlg,
			IDC_MAPOUT,  y, Mapping.Map_bPercent ? (LPSTR)"%":(LPSTR)"");
	}
	else
		SetDlgItemText(hDlg,
			IDC_MAPOUT, "");
}


//************************************************************************
//		MapSetGradient.
// DESCRIPTION:
//		sets the given gradient control based on the channel.
//************************************************************************
void MapSetGradients(HWND hDlg, BOOL bIntensity, WORD wChannel)
//************************************************************************
{
	HWND hBar;
	RGBS rgb1;
	RGBS rgb2;
	RGBS rgbTemp;

	switch (wChannel)
	{
		case IDC_DO_RED:		
			rgb1.red = rgb1.green = rgb1.blue = 0;
			rgb2.red = 255;
			rgb2.green = rgb2.blue = 0;
		break;
		case IDC_DO_GREEN:	
			rgb1.red = rgb1.green = rgb1.blue = 0;
			rgb2.green = 255;
			rgb2.red = rgb2.blue = 0;
		break;
		case IDC_DO_BLUE:		
			rgb1.red = rgb1.green = rgb1.blue = 0;
			rgb2.red = rgb2.green = 0;
			rgb2.blue = 255;
		break;
		case IDC_CAL_CYAN:
		case IDC_DO_CYAN:		
			rgb2.red = 0;
			rgb2.green = rgb2.blue = 255;
			rgb1.red = rgb1.green = rgb1.blue = 255;
		break;
		case IDC_CAL_MAGENTA:
		case IDC_DO_MAGENTA:
			rgb2.green = 0;
			rgb2.red = rgb2.blue = 255;
			rgb1.red = rgb1.green = rgb1.blue = 255;
		break;
		case IDC_CAL_YELLOW:
		case IDC_DO_YELLOW:	
			rgb2.blue = 0;
			rgb2.red = rgb2.green = 255;
			rgb1.red = rgb1.green = rgb1.blue = 255;
		break;
		case IDC_CAL_BLACK:
		case IDC_DO_BLACK:	
			bIntensity = !bIntensity;
		case IDC_CAL_MASTER:
		case IDC_DO_ALL:
		default:
			rgb1.red = rgb1.green = rgb1.blue = 0;
			rgb2.red = rgb2.green = rgb2.blue = 255;
		break;
	}
	
	if (bIntensity)
	{
		rgbTemp = rgb1;
		rgb1 = rgb2;
		rgb2 = rgbTemp;
	}
		
	if (hBar = GetDlgItem(hDlg, IDC_MAP_HBAR))
	{
		Gradient_SetRGB(hBar, &rgb2, &rgb1);
		Gradient_SetType(hBar, GT_RGB);																	  
	}
	if (hBar = GetDlgItem(hDlg,IDC_MAP_VBAR))
	{
		Gradient_SetRGB(hBar, &rgb1, &rgb2);
		Gradient_SetType(hBar, GT_RGB);																	  
	}
}

//************************************************************************
//		MapSetupPanes
// DESCRIPTION:
//		sets up the panes for visual and numeric map
//************************************************************************
void MapSetupPanes(HWND hDlg, LPMAPVALUES lpValues, BOOL fUseProbe)
//************************************************************************
{
	ITEMID id;

	// Initialise the dialog section.
	InitDlgSection(&lpValues->MapSec, ID_SECTION0, hDlg, IDC_PANE0, NULL);

	// Set threshold so that a category is always discarded when the user
	// selects another.
	SetDlgSectionThreshold(&lpValues->MapSec, 2, 2);

	AddDlgSectionCategory(	&lpValues->MapSec,
							PictPubApp.GetResourceHandle(),
							MAKEINTRESOURCE(IDD_MAPNUMERIC),
							IDD_MAPNUMERIC,
				    		DSCF_PRELOAD,
							NULL);
	if (fUseProbe)
		id = IDD_MAPVISUAL;
	else
		id = IDD_MAPVISUALNOPROBE;
	AddDlgSectionCategory(	&lpValues->MapSec,
							PictPubApp.GetResourceHandle(),
							MAKEINTRESOURCE(id),
							IDD_MAPVISUAL,
				    		DSCF_PRELOAD,
							NULL);
}

/***************************************************************************/
void MapActivatePane( HWND hDlg, ITEMID idTool, LPMAPVALUES lpValues )
/***************************************************************************/
{
	// no change? 
	if (idTool == IDC_MAPTOOLV && !lpValues->bMap_Visual)
		return;
	if (idTool != IDC_MAPTOOLV && lpValues->bMap_Visual)
		return;
	lpValues->bMap_Visual = (idTool != IDC_MAPTOOLV);

	if ( lpValues->bMap_Visual )
		SelectDlgSectionCategory(&lpValues->MapSec, IDD_MAPVISUAL);
	else
		SelectDlgSectionCategory(&lpValues->MapSec, IDD_MAPNUMERIC);

	CheckComboItem( hDlg, IDC_MAPTOOL, IDC_MAPTOOL3, IDC_MAPTOOLV, idTool );
}

