//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "pp.h"
#include "id.h"
#include "ppafx.h"
#include "ctlutil.h"
#include "histoctl.h"
#include "tagctl.h"
#include "gradctl.h"


// number of tags in histo dialog
#define HISTOMARKS 	2

#define TO_DISPLAY(val) \
	(Mapping.Quarter_bPercent ? ((val)*100L/255):(val))

#define FROM_DISPLAY(val) \
	(Mapping.Quarter_bPercent ? ((val)*255L/100):(val))

#define MAX_SPIN (Mapping.Quarter_bPercent ? 100:255)

typedef struct _quarterstruct {
	LPDWORD lpHistos[5];		// histograms for each channel
	int iProbe;				// probing which marker
	int OriginalValue;  		// value of marker before probing
	WORD wChannel;			// channel currently editing
	LPMAP lpMap;				// Map for current channel
	int iHighlight;
	int iShadow;
	int iMidtone;		// actual value
	int MidtonePer;		// 'percent'(0-255) as range between iShadow & iMidtone 
}QUARTERSTRUCT, FAR * LPQUARTERSTRUCT;


// for histogram dialog
typedef struct _histostruct {
	LPDWORD lpHistos[5];		// histograms for each channel
	LONG lTotals[5];			// totals for each histogram
	BOOL bProbe;				// probing which marker
	int OriginalValue;  		// value of marker before probing
	WORD wChannel;				// channel currently examining
}HISTOSTRUCT, FAR * LPHISTOSTRUCT;

// Static prototypes
BOOL WINPROC EXPORT DlgQuarterOptionsProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
LOCAL void change_mark(HWND hDlg, LPQUARTERSTRUCT lpData, int nMark, int val, int inval);
LOCAL VOID reset_channel(WORD wChannel);
LOCAL VOID set_quarter_channel(HWND  hDlg, WORD wChannel);
LOCAL LPDWORD  get_histo(LPDWORD FAR *lpHistos, WORD wChannel, LPINT lpbReverse, LPINT lpNum = NULL);
LOCAL VOID free_hitograms(LPDWORD FAR *lpHistos);
LOCAL void set_sacrifice(HWND hDlg, LPQUARTERSTRUCT lpData, WORD wChannel);
LOCAL LPMAP tone_activate_channel(WORD wChannel );
LOCAL void MapLineHistoProc( int y, int x1, int x2, LPTR lpSrc, FRMTYPEINFO TypeInfo );
LOCAL void map_to_mapinfo(LPMAP lpMap, LPMAPINFO lpInfo);
LOCAL long compute_yscale(LPDWORD lpHisto);
LOCAL VOID set_histo_channel(HWND hDlg, WORD wChannel);
LOCAL void histo_change_mark(HWND hDlg, LPHISTOSTRUCT lpData, int nMark, int val);
LOCAL void histo_set_HMSvalues(HWND hDlg, LPHISTOSTRUCT lpData);
LOCAL void histo_browse(HWND hDlg, int x);
LOCAL void set_gradient(HWND hDlg, int id, WORD wChannel);
LOCAL void handle_tone_load(HWND hDlg,LPQUARTERSTRUCT lpData,HWND hActiveWnd);

static LPDWORD lpGetHistos[5];		// histograms for MapLineHistoProc

#define GET_QUARTER(hDlg) \
	(hDlg ? (LPQUARTERSTRUCT)GetWindowLong(hDlg,DWL_USER) : NULL)
	
// LOCAL prototypes
LOCAL BOOL Quarter_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Quarter_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void Quarter_OnDestroy(HWND hDlg);
LOCAL void Quarter_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Quarter_OnMouseMove(HWND hDlg, int x, int y, UINT keyFlags);
LOCAL void Quarter_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR *lpDrawItem);
LOCAL void Quarter_OnTimer(HWND hwnd, UINT id);

static LPTONE_PARMS Quarter_lpParms;

/***********************************************************************/
BOOL WINPROC EXPORT DlgQuarterProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch (msg)
	{
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, Quarter_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_DESTROY, Quarter_OnDestroy);
		HANDLE_DLGMSG(hDlg, WM_COMMAND, Quarter_OnCommand);
		HANDLE_DLGMSG(hDlg, WM_LBUTTONUP, Quarter_OnLButtonUp);
		HANDLE_DLGMSG(hDlg, WM_MOUSEMOVE, Quarter_OnMouseMove);
		HANDLE_DLGMSG(hDlg, WM_DRAWITEM, Quarter_OnDrawItem);
		HANDLE_DLGMSG(hDlg, WM_TIMER, Quarter_OnTimer);

		default:
		return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_QUARTERTONE);
	}
}

/************************************************************************/
LOCAL BOOL Quarter_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	HWND hActiveWnd, hControl;
	FRMDATATYPE type;
	LPIMAGE lpImage = NULL;
	int i;
	LONG style;
	LPQUARTERSTRUCT lpData;
	
	lpData = (LPQUARTERSTRUCT)Alloc(sizeof(QUARTERSTRUCT));
	if (!lpData)
		return(FALSE);
	SetWindowLong(hDlg, DWL_USER, (LONG)lpData);

	// setup tag sliders
	for (i=0;i<3;i++)
	{
		if (hControl = GetDlgItem(hDlg, i+IDC_QUART1))
		{
			style = GetWindowLong(hControl, GWL_EXSTYLE);
			style |= WS_EX_TRANSPARENT;
			SetWindowLong(hControl, GWL_EXSTYLE, style);
		}
	}
	
	if (hActiveWnd = GetActiveDoc())
		lpImage = GetImagePtr(hActiveWnd);
	RightPopup( hDlg );
	
	// set channels combo
	type = FrameType(ImgGetBaseEditFrame(lpImage));
	lpData->wChannel = IDC_DO_ALL;
	ChannelsCombo_Setup(hDlg, IDC_CHANNELS, type, lpData->wChannel);

	ControlEnable(hDlg, IDC_PREVIEW, !Mapping.Quarter_bAuto);

	// remove or add percents
	SetAllDlgItemsText(hDlg, IDC_PERCENT, Mapping.Quarter_bPercent ? (LPSTR)"%":(LPSTR)"");
	
	Quarter_lpParms = (LPTONE_PARMS)lParam;
	lpData->lpHistos[0] = NULL;
	ResetAllMaps();
	
	// setup each channel with defaults
	for (i=IDC_DO_ALL; i<=IDC_DO_BLACK;i++)
		reset_channel(i);
	// setup the actual histogram and controls 
	set_quarter_channel(hDlg, lpData->wChannel);
	return(TRUE);
}

/***************************************************************************/
LOCAL void Quarter_OnDestroy(HWND hDlg)
/***************************************************************************/
{
	LPQUARTERSTRUCT lpData;
	
	if (!(lpData = GET_QUARTER(hDlg)))
		return;

	AutoPreview_OnDestroy(hDlg);
	Probe_End(hDlg);
	free_hitograms(lpData->lpHistos);
	// will free up structure
	Dialog_OnDestroy(hDlg);
}

/************************************************************************/
LOCAL void Quarter_OnTimer(HWND hDlg, UINT id)
/************************************************************************/
{	
	LPQUARTERSTRUCT lpData;
	
	if (!(lpData = GET_QUARTER(hDlg)))
		return;

	if (!AutoPreview_OnTimer(hDlg, id))
		return;
		
	// got good timer		   
	MakeMap(lpData->lpMap );
	MapPreview(GetActiveDoc());
}

/************************************************************************/
LOCAL void Quarter_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
	int i, val;
	BOOL Bool;
	HWND hActiveWnd;
	LPIMAGE lpImage = NULL;
	LPQUARTERSTRUCT lpData;
	
	if (!(lpData = GET_QUARTER(hDlg)))
		return;

	if (hActiveWnd = GetActiveDoc())
		lpImage = GetImagePtr(hActiveWnd);

	switch (id)
	{
		case IDC_CHANNELS:
			i=ChannelsCombo_Handle(hDlg, id, codeNotify);
			if (!i)
				break;
			lpData->wChannel = i;
			set_quarter_channel(hDlg, lpData->wChannel);
		break;
	
		case IDC_MAPLOAD:
			if ( !AstralDlg( NO, PictPubApp.GetResourceHandle(), hDlg, IDD_MAPLOAD,
		 		DlgMapLoadProc ) )
				break;
			handle_tone_load(hDlg, lpData, hActiveWnd);
		break;
		
		case IDC_MAPSAVE:
			if ( !AstralDlg( NO, PictPubApp.GetResourceHandle(), hDlg, IDD_MAPSAVE,
		 		DlgMapSaveProc ) )
				break;
		break;
	
		case IDC_QUART1_PROBE:
		case IDC_QUART2_PROBE:
		case IDC_QUART3_PROBE:
			if (!Probe_Begin(hDlg, codeNotify))
				break;
			CheckDlgButton( hDlg, id, YES );
			lpData->iProbe = id-IDC_QUART1_PROBE;
			if (lpData->iProbe == 0)
				lpData->OriginalValue = lpData->iShadow;
			else if (lpData->iProbe == 1)
				lpData->OriginalValue = lpData->iMidtone;
			else
				lpData->OriginalValue = lpData->iHighlight;
			// unpreview
			UnAutoPreview(hDlg, hActiveWnd);
		break;

		case IDC_QUART1_VALUE:
		case IDC_QUART2_VALUE:
		case IDC_QUART3_VALUE:
			if ( GetFocus() != GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE)
				break;
			val = GetDlgItemSpin(hDlg, id, &Bool, NO);
			if (!Bool)
				break;
			// convert to 'real' location
			if (id == IDC_QUART2_VALUE && Mapping.Quarter_bUseMidPer)
			  	i = lpData->iShadow + (LONG)val*(lpData->iHighlight-lpData->iShadow)/100;
			else
				i = FROM_DISPLAY(val);
			if (id != IDC_QUART2_VALUE)
				val = INT_MAX;
			change_mark(hDlg, lpData,  id-IDC_QUART1_VALUE, i, val);
			AutoPreview(lpData->lpMap, Mapping.Quarter_bAuto, NO, hDlg, hActiveWnd);
		break;
		
		case IDC_MINHIGHLIGHT:
			if ( GetFocus() != GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE)
				break;
			i = GetDlgItemSpin(hDlg, id, &Bool, NO);
			if (!Bool)
				break;
			i = FROM_DISPLAY(i);
			lpData->lpMap->Pnt[2].y = i;
			lpData->lpMap->Pnt[1].y = (lpData->lpMap->Pnt[2].y+lpData->lpMap->Pnt[0].y)/2;
			AutoPreview(lpData->lpMap, Mapping.Quarter_bAuto, YES, hDlg, hActiveWnd);
		break;
		
		case IDC_MAXSHADOW:
			if ( GetFocus() != GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE)
				break;
			i = GetDlgItemSpin(hDlg, id, &Bool, NO);
			if (!Bool)
				break;
			i = FROM_DISPLAY(i);
			lpData->lpMap->Pnt[0].y = i;
			lpData->lpMap->Pnt[1].y = (lpData->lpMap->Pnt[2].y+lpData->lpMap->Pnt[0].y)/2;
			AutoPreview(lpData->lpMap, Mapping.Quarter_bAuto, YES, hDlg, hActiveWnd);
		break;
		
		case IDC_HISTOGRAM:
			// value changed
			if (WITHIN(codeNotify, HTN_CHANGED_FIRST, HTN_CHANGED_LAST))
			{
				i = codeNotify-HTN_CHANGED_FIRST;
				Bool = TRUE;
			}
			else if (WITHIN(codeNotify, HTN_CHANGING_FIRST, HTN_CHANGING_LAST))
			{
				i = codeNotify-HTN_CHANGING_FIRST;
				Bool = FALSE;
			}
			else
				break;
			val = Histo_GetMark(hControl, i);
			change_mark(hDlg, lpData, i, val, INT_MAX);
			AutoPreview(lpData->lpMap, Mapping.Quarter_bAuto, Bool, hDlg, hActiveWnd);
		break;
		
		case IDC_TAGS:
			// value changed
			if (WITHIN(codeNotify, TN_CHANGED_FIRST, TN_CHANGED_LAST))
			{
				i = codeNotify-TN_CHANGED_FIRST;
				Bool = TRUE;
			}
			else if (WITHIN(codeNotify, TN_CHANGING_FIRST, TN_CHANGING_LAST))
			{
				i = codeNotify-TN_CHANGING_FIRST;
				Bool = FALSE;
			}
			else
				break;
			val = Tag_GetMark(hControl, i);
			change_mark(hDlg, lpData, i, val, INT_MAX);
			AutoPreview(lpData->lpMap, Mapping.Quarter_bAuto, Bool, hDlg, hActiveWnd);
		break;

		case IDC_PREVIEW:
	  		AutoPreview_Button(hActiveWnd, lpData->lpMap);
		break;
		
		case IDC_MAP_OPTIONS:
			if ( !AstralDlg( NO, AfxGetInstanceHandle(), hDlg, IDD_QUARTER_OPTIONS,
		 		DlgQuarterOptionsProc ) )
				break;
			
			// Enable or Disable preview button		
			ControlEnable(hDlg, IDC_PREVIEW, !Mapping.Quarter_bAuto);
		
			// remove or add percents
			SetAllDlgItemsText(hDlg, IDC_PERCENT, Mapping.Quarter_bPercent ? (LPSTR)"%":(LPSTR)"");
			
			// setup the actual histogram and controls 
			set_quarter_channel(hDlg, lpData->wChannel);
			AutoPreview(lpData->lpMap, Mapping.Quarter_bAuto, YES, hDlg, hActiveWnd);
		break;
		
		case IDC_QUART_DEFAULTS:
			// setup channel with sacrifice defaults
			set_sacrifice(hDlg, lpData, lpData->wChannel);
			// setup the actual histogram and controls 
			set_quarter_channel(hDlg, lpData->wChannel);
			AutoPreview(lpData->lpMap, Mapping.Quarter_bAuto, YES, hDlg, hActiveWnd);
		break;
	
		case IDC_RESET:
			// setup channel with defaults
			reset_channel(lpData->wChannel);
			// setup the actual histogram and controls 
			set_quarter_channel(hDlg, lpData->wChannel);
			AutoPreview(lpData->lpMap, Mapping.Quarter_bAuto, YES, hDlg, hActiveWnd);
		break;
		
		case IDC_RESETALL:
			// setup each channel with defaults
			for (i=IDC_DO_ALL; i<IDC_DO_BLACK; i++)
			{
				if(ChannelsCombo_IsEnabled(hDlg, i))
					reset_channel(i);
			}
			// setup the actual histogram and controls 
			set_quarter_channel(hDlg, lpData->wChannel);
			AutoPreview(lpData->lpMap, Mapping.Quarter_bAuto, YES, hDlg, hActiveWnd);
		break;

		case IDOK:
			map_to_mapinfo(&MasterMap, &Quarter_lpParms->MasterInfo);
			map_to_mapinfo(&RedMap, &Quarter_lpParms->RedInfo);
			map_to_mapinfo(&GreenMap, &Quarter_lpParms->GreenInfo);
			map_to_mapinfo(&BlueMap, &Quarter_lpParms->BlueInfo);
			map_to_mapinfo(&BlackMap, &Quarter_lpParms->BlackInfo);
		case IDCANCEL:
			// unpreview
			UnAutoPreview(hDlg, hActiveWnd);
			AstralDlgEnd( hDlg, id == IDOK );
		break;

   		default:
		break;
   }
}

/***********************************************************************/
LOCAL void Quarter_OnMouseMove(HWND hDlg, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	COLORINFO ProbeColor;
	int i;
	LPQUARTERSTRUCT lpData;
	
	if (!(lpData = GET_QUARTER(hDlg)))
		return;

	if (Probe_OnMouseMove(hDlg,x,y,keyFlags,&ProbeColor))
	{
		// get good probe
		switch (lpData->wChannel)
		{
			case IDC_DO_RED:		
				i = ProbeColor.rgb.red;
			break;
			case IDC_DO_GREEN:	
				i = ProbeColor.rgb.green;
			break;
			case IDC_DO_BLUE:		
				i = ProbeColor.rgb.blue;
			break;
			case IDC_DO_CYAN:		
				i = ProbeColor.cmyk.c;
			break;
			case IDC_DO_MAGENTA:
				i = ProbeColor.cmyk.m;
			break;
			case IDC_DO_YELLOW:	
				i = ProbeColor.cmyk.y;
			break;
			case IDC_DO_BLACK:	
				i = ProbeColor.cmyk.k;
			break;
			case IDC_DO_ALL:
				i = ProbeColor.gray;
			break;
			default:
				i = ProbeColor.gray;
			break;
		}
		change_mark(hDlg, lpData, lpData->iProbe, i, INT_MAX);
	}
	else if (Probe_IsProbing())
	{
		// probing but no valid value
		change_mark(hDlg, lpData, lpData->iProbe, lpData->OriginalValue, INT_MAX);
	}
}

/***********************************************************************/
LOCAL void Quarter_OnLButtonUp(HWND hDlg, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	COLORINFO ProbeColor;
	int i;
	LPQUARTERSTRUCT lpData;
	
	if (!(lpData = GET_QUARTER(hDlg)))
		return;
	
	if (!Probe_IsProbing())
		return;
	
	if (Probe_OnLButtonUp(hDlg,x,y,keyFlags,&ProbeColor))
	{
	
		// got a color
		switch (lpData->wChannel)
		{
			case IDC_DO_RED:		
				i = ProbeColor.rgb.red;
			break;
			case IDC_DO_GREEN:	
				i = ProbeColor.rgb.green;
			break;
			case IDC_DO_BLUE:		
				i = ProbeColor.rgb.blue;
			break;
			case IDC_DO_CYAN:		
				i = ProbeColor.cmyk.c;
			break;
			case IDC_DO_MAGENTA:
				i = ProbeColor.cmyk.m;
			break;
			case IDC_DO_YELLOW:	
				i = ProbeColor.cmyk.y;
			break;
			case IDC_DO_BLACK:	
				i = ProbeColor.cmyk.k;
			break;
			case IDC_DO_ALL:
				i = ProbeColor.gray;
			break;
			default:
				i = ProbeColor.gray;
			break;
		}
		change_mark(hDlg, lpData, lpData->iProbe, i, INT_MAX);
	}
	else
	{
		// unsucessful probe session
		change_mark(hDlg, lpData, lpData->iProbe, lpData->OriginalValue, INT_MAX);
	}
	// update buttons
	CheckDlgButton( hDlg, IDC_QUART1_PROBE, NO );
	CheckDlgButton( hDlg, IDC_QUART2_PROBE, NO );
	CheckDlgButton( hDlg, IDC_QUART3_PROBE, NO );
	// autopreview
	AutoPreview(lpData->lpMap, Mapping.Quarter_bAuto, YES, hDlg, GetActiveDoc());
}


//************************************************************************
LOCAL void Quarter_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR *lpDrawItem)
//************************************************************************
{
			DrawItem( hDlg, lpDrawItem, 0 );
}

// LOCAL prototypes
LOCAL BOOL QuarterOptions_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void QuarterOptions_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

static BOOL Quarter_HSacrifice;
static BOOL Quarter_SSacrifice;
static BOOL Quarter_bPercent;
static BOOL Quarter_bUseMidPer;
static BOOL Quarter_bAuto;
 
/***********************************************************************/
// modifies Mapping.Quarter_bGrid,
//			Mapping.Quarter_bPercent,
//			Mapping.Quarter_nPoints.
//			Mapping.Quarter_bUseMidPer.
//			Mapping.Quarter_bAuto.
/***********************************************************************/
BOOL WINPROC EXPORT DlgQuarterOptionsProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch (msg)
	{
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, QuarterOptions_OnInitDialog);
		HANDLE_DLGMSG(hDlg, WM_COMMAND, QuarterOptions_OnCommand);

		default:
		return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_MAPLOAD);
	}
}

/************************************************************************/
LOCAL BOOL QuarterOptions_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	CenterPopup( hDlg );
	
	Quarter_HSacrifice = 	Mapping.Quarter_HSacrifice;
	Quarter_SSacrifice = 	Mapping.Quarter_SSacrifice;
	Quarter_bPercent = 	Mapping.Quarter_bPercent;
	Quarter_bAuto = 	Mapping.Quarter_bAuto;
	Quarter_bUseMidPer = 	Mapping.Quarter_bUseMidPer;
	CheckDlgButton( hDlg, IDC_USE_MID_PER, Quarter_bUseMidPer);
	CheckDlgButton( hDlg, IDC_AUTO, Quarter_bAuto);
	CheckDlgButton( hDlg, IDC_MAP_PERCENT, Quarter_bPercent);
	InitDlgItemSpin(hDlg, IDC_HILIGHTS_SACRIFICED, Quarter_HSacrifice, NO, 0, 100);
	InitDlgItemSpin(hDlg, IDC_SHADOWS_SACRIFICED, Quarter_SSacrifice, NO, 0, 100);
	return(TRUE);
}

/************************************************************************/
LOCAL void QuarterOptions_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
	int i;
	BOOL bool;
	
	switch (id)
	{
		
		case IDC_HILIGHTS_SACRIFICED:
			i = GetDlgItemSpin(hDlg, id, &bool, NO);
			if (bool)
				Quarter_HSacrifice = i;
		break; 
		
		case IDC_SHADOWS_SACRIFICED:
			i = GetDlgItemSpin(hDlg, id, &bool, NO);
			if (bool)
				Quarter_SSacrifice = i;
		break; 
		
		case IDC_MAP_PERCENT:
			Quarter_bPercent = !Quarter_bPercent;
			CheckDlgButton( hDlg, id, Quarter_bPercent);
		break;
	
		case IDC_AUTO:
			Quarter_bAuto = !Quarter_bAuto;
			CheckDlgButton( hDlg, id, Quarter_bAuto);
		break;

		case IDC_USE_MID_PER:
			Quarter_bUseMidPer = !Quarter_bUseMidPer;
			CheckDlgButton( hDlg, id, Quarter_bUseMidPer);
		break;
	
		case IDOK:
			Mapping.Quarter_bPercent = Quarter_bPercent;
			Mapping.Quarter_bAuto = Quarter_bAuto;
			Mapping.Quarter_bUseMidPer = Quarter_bUseMidPer;
			Mapping.Quarter_HSacrifice = Quarter_HSacrifice;
			Mapping.Quarter_SSacrifice = Quarter_SSacrifice;
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
LOCAL void handle_tone_load(HWND hDlg,LPQUARTERSTRUCT lpData,HWND hActiveWnd)
//************************************************************************
{
	WORD wChannel;
	LPMAP lpMap;
	BOOL bAllCool;
	
	bAllCool = TRUE;
	for(wChannel = IDC_DO_ALL; wChannel<=IDC_DO_BLACK;wChannel++)
	{
		lpMap = tone_activate_channel( wChannel );
		if (lpMap->Points == 3)
			continue;
		bAllCool = FALSE;
		if (lpMap->Points > 3)
		{
			lpMap->Pnt[1] = lpMap->Pnt[lpMap->Points/2];
			lpMap->Pnt[2] = lpMap->Pnt[2];
		}
		else if (lpMap->Points == 2)
		{
			lpMap->Pnt[2] = lpMap->Pnt[1];
			lpMap->Pnt[1].x = (lpMap->Pnt[0].x + lpMap->Pnt[2].x)/2;
			lpMap->Pnt[1].y = (lpMap->Pnt[0].y + lpMap->Pnt[2].y)/2;
		}
		else	// must be 1 point, ignore it
		{
			lpMap->Pnt[0].x = 0;
			lpMap->Pnt[0].y = 0;
			lpMap->Pnt[1].x = 127;
			lpMap->Pnt[1].y = 127;
			lpMap->Pnt[2].x = 255;
			lpMap->Pnt[2].y = 255;
		}
		lpMap->Points = 3;
		MakeMap(lpMap);
	}
	if (!bAllCool)
		Message(IDS_TONE_BADMAP);
	set_quarter_channel(hDlg, lpData->wChannel);
	AutoPreview(lpData->lpMap, Mapping.Quarter_bAuto, YES, hDlg, hActiveWnd);
}


//************************************************************************
//		change_mark
// PARAMETERS:
//					int nMark- which mark (0 for highlights etc)
//					int val- New value.
//					int inval- the 'readout' value of the given mark (INT_MAX for none)
//							Currently used only for midtone
// DESCRIPTION:
//				Sets the mark, updates other marks to keep them valid.
//				Sets and redraws all appropriate controls.
//				Updates the map.
//************************************************************************
LOCAL void change_mark(HWND hDlg, LPQUARTERSTRUCT lpData, int nMark, int val, int inval)
//************************************************************************
{
	HWND hHisto, hTag;
	int i;
	BOOL Bool;
	int Marks[3];
	
	// set value and adjust others
	Marks[0] = lpData->iShadow;
	Marks[2] = lpData->iHighlight;
	if (Mapping.Quarter_bUseMidPer)
		Marks[1] = Marks[0] + (LONG)lpData->MidtonePer*(Marks[2]-Marks[0])/255;
	else
		Marks[1] = lpData->iMidtone;
	if (nMark == 0)
	{
		Marks[0] = lpData->iShadow = val;
		if (Marks[2] < Marks[0])
			Marks[2] = Marks[0];
		if (Mapping.Quarter_bUseMidPer)
			Marks[1] = Marks[0] + (LONG)lpData->MidtonePer*(Marks[2]-Marks[0])/255;
		else if (Marks[1] < Marks[0])
			Marks[1] = Marks[0];
	}
	else if (nMark == 2)
	{
		Marks[2] = lpData->iHighlight = val;
		if (Marks[0] > Marks[2])
			Marks[0] = Marks[2];
		if (Mapping.Quarter_bUseMidPer)
			Marks[1] = Marks[0] + (LONG)lpData->MidtonePer*(Marks[2]-Marks[0])/255;
		else if (Marks[1] > Marks[2])
			Marks[1] = Marks[2];
	}
	else if (nMark == 1)
	{
		Marks[1] = lpData->iMidtone = Bound(val, Marks[0], Marks[2]);
		if (Marks[2]-Marks[0])
			lpData->MidtonePer = (lpData->iMidtone - Marks[0])*255L/(Marks[2]-Marks[0]);
	}
	else
		return;
	
	// update anything that needs it
	// spins
	i = GetDlgItemSpin(hDlg, IDC_QUART1_VALUE, &Bool, NO);
	i = FROM_DISPLAY(i);
	if (!Bool || i != Marks[0])
		InitDlgItemSpin(hDlg, IDC_QUART1_VALUE, TO_DISPLAY(Marks[0]), NO, 0, MAX_SPIN );
	if (inval != INT_MAX  && nMark==1)
	{
	}
	else
	{
		// update percent
		if (Mapping.Quarter_bUseMidPer)
		{
			i = GetDlgItemSpin(hDlg, IDC_QUART2_VALUE, &Bool, NO);
			if (!Bool || (i*255L/100) != lpData->MidtonePer)
				InitDlgItemSpin(hDlg, IDC_QUART2_VALUE, lpData->MidtonePer*100L/255, NO, 0, MAX_SPIN );
		}
		else
		{
			i = GetDlgItemSpin(hDlg, IDC_QUART2_VALUE, &Bool, NO);
			i = FROM_DISPLAY(i);
			if (!Bool || i != Marks[1])
				InitDlgItemSpin(hDlg, IDC_QUART2_VALUE, TO_DISPLAY(Marks[1]), NO, 0, MAX_SPIN );
		}
	}

	i = GetDlgItemSpin(hDlg, IDC_QUART3_VALUE, &Bool, NO);
	i = FROM_DISPLAY(i);
	if (!Bool || i != Marks[2])
		InitDlgItemSpin(hDlg, IDC_QUART3_VALUE, TO_DISPLAY(Marks[2]), NO, 0, MAX_SPIN );
	
	// histogram
	hHisto = GetDlgItem(hDlg, IDC_HISTOGRAM);
	hTag = GetDlgItem(hDlg, IDC_TAGS);
	for (i=0;i<3;i++)
	{
		if (Marks[i] != Histo_GetMark(hHisto,i))
			Histo_SetMark(hHisto,i, Marks[i], NO);
		if (Marks[i] != Tag_GetMark(hTag,i))
			Tag_SetMark(hTag,i, Marks[i], NO);
	}
	
	lpData->lpMap->Pnt[0].x = Marks[0];
	lpData->lpMap->Pnt[1].x = Marks[1];
	lpData->lpMap->Pnt[2].x = Marks[2];
}	

//************************************************************************
//			reset_channel
// PARAMETERS:
//				LPQUARTERSTRUCT lpData- The data fro the control.
//				WORD wChannel- IDC_DO_ALL etc.
// DESCRIPTION:
//			Sets the given channel with the default vales.
//************************************************************************
LOCAL VOID reset_channel(WORD wChannel)
//************************************************************************
{
	LPMAP lpMap;
	                       
	// get the map & set its invert flags
	lpMap = tone_activate_channel(wChannel );
	
	// setup the map
	ResetMap( lpMap, 3, NO );
	
	lpMap->Pnt[0].x = 0;
	lpMap->Pnt[0].y = 0;
	lpMap->Pnt[1].x = 127;
	lpMap->Pnt[1].y = 127;
	lpMap->Pnt[2].x = 255;
	lpMap->Pnt[2].y = 255;
}



//************************************************************************
//			set_sacrifice
// DESCRIPTION:
//			Uses Mapping.Quarter_HSacrifice & Mapping.Quarter_SSacrifice
//			to set the highlights midtones and shaddows.
//************************************************************************
LOCAL void set_sacrifice(HWND hDlg, LPQUARTERSTRUCT lpData, WORD wChannel)
//************************************************************************
{
	LPMAP lpMap;
	LPDWORD lpHisto;
	BOOL bInv;
	LONG lSum, lSearch, lTotal;
	int hi, lo, i;
	
	// get the map & set its invert flags
	lpMap = tone_activate_channel( wChannel );
	lpHisto = get_histo(lpData->lpHistos, wChannel, &bInv);
	// determine initail values
	hi = 170;
	lo = 85;
	if (lpHisto)
	{
		// Find the total of all entries to scale
		lTotal = 0;
		for ( i=0; i<256; i++ )
			lTotal += lpHisto[i];

		// find the sacrafice
		lSearch = bInv ? Mapping.Quarter_HSacrifice:Mapping.Quarter_SSacrifice;
		lSearch = FMUL( lTotal, FGET( lSearch, 100 ) );
		lSum = -1;
		for ( i=0; i<255; i++ )
		{
			lSum += lpHisto[i];
			if ( lSum >= lSearch )
				break;
		}
		lo = i;
		
		// find the sacrafice
		lSearch = bInv ? Mapping.Quarter_SSacrifice:Mapping.Quarter_HSacrifice;
		lSearch = FMUL( lTotal, FGET( lSearch, 100 ) );
		lSum = -1;
		for ( i=255; i>0; i-- )
		{
			lSum += lpHisto[i];
			if ( lSum >= lSearch )
				break;
		}
		hi = i;
	}
	
	if (bInv)
	{
		hi = 255-hi;
		lo = 255-lo;
	}
	i = lo;
	lo = min(lo,hi);
	hi = max(i,hi);
	
	// setup the map
	if (wChannel == IDC_DO_ALL)
		ResetMapEx( lpMap, 3, NO, YES);
	else
		ResetMap( lpMap, 3, NO );
	lpMap->Pnt[0].x = lo;
	lpMap->Pnt[1].x = lo + (hi-lo)/2;
	lpMap->Pnt[2].x = hi;
}

//************************************************************************
//		set_quarter_channel
// DESCRIPTION:
//		Makes the given channel the current channel.
//		Updates the display.
//************************************************************************
LOCAL VOID set_quarter_channel(HWND  hDlg, WORD wChannel)
//************************************************************************
{
	LPDWORD lpHisto;
	HWND hControl;
	BOOL bInv;
	LONG lMaxEntry;
	int icons[3];
	LPQUARTERSTRUCT lpData;
	
	if (!(lpData = GET_QUARTER(hDlg)))
		return;
	
	lpData->lpMap = tone_activate_channel( wChannel );
	lpHisto = get_histo(lpData->lpHistos, wChannel, &bInv);
	
	// setup the histo control
	lMaxEntry = compute_yscale(lpHisto);
	hControl = GetDlgItem(hDlg, IDC_HISTOGRAM);
	Histo_Init(hControl, lpHisto, lMaxEntry, 3, bInv ? HTS_FLIPX:0);
	lpData->iShadow = lpData->lpMap->Pnt[0].x;
	lpData->iMidtone = lpData->lpMap->Pnt[1].x;
	lpData->iHighlight = lpData->lpMap->Pnt[2].x;
	if (!(lpData->iHighlight - lpData->iShadow))
		lpData->MidtonePer = 0;
	else
		lpData->MidtonePer = (lpData->iMidtone - lpData->iShadow)*255/
								(lpData->iHighlight - lpData->iShadow);
	Histo_SetMark(hControl, 0, lpData->lpMap->Pnt[0].x, NO);
	Histo_SetMark(hControl, 1, lpData->lpMap->Pnt[1].x, NO);
	Histo_SetMark(hControl, 2, lpData->lpMap->Pnt[2].x, NO);
	
	// some people seem to have an update problem here
    InvalidateRect (hControl,NULL,FALSE);

	// setup tags
	hControl = GetDlgItem(hDlg, IDC_TAGS);
	
	icons[0] = IDC_QUART1;
	icons[1] = IDC_QUART2;
	icons[2] = IDC_QUART3;
	Tag_Init(hControl, 3, 255, &icons[0]);
	
	Tag_SetMark(hControl, 0, lpData->lpMap->Pnt[0].x, NO);
	Tag_SetMark(hControl, 1, lpData->lpMap->Pnt[1].x, NO);
	Tag_SetMark(hControl, 2, lpData->lpMap->Pnt[2].x, NO);
	
	// some people seem to have an update problem here
    InvalidateRect (hControl,NULL,FALSE);
	
	set_gradient(hDlg, IDC_HISTO_GRAD, wChannel);

	// spins
	InitDlgItemSpin(hDlg, IDC_QUART1_VALUE, TO_DISPLAY(lpData->lpMap->Pnt[0].x), NO, 0, MAX_SPIN);
	InitDlgItemSpin(hDlg, IDC_QUART3_VALUE, TO_DISPLAY(lpData->lpMap->Pnt[2].x), NO, 0, MAX_SPIN);

	if (Mapping.Quarter_bUseMidPer)
		InitDlgItemSpin(hDlg, IDC_QUART2_VALUE, lpData->MidtonePer*100L/255, NO, 0, MAX_SPIN );
	else
 		InitDlgItemSpin(hDlg, IDC_QUART2_VALUE, TO_DISPLAY(lpData->lpMap->Pnt[1].x), NO, 0, MAX_SPIN );

	InitDlgItemSpin(hDlg, IDC_MINHIGHLIGHT, TO_DISPLAY(lpData->lpMap->Pnt[2].y), NO, 0, MAX_SPIN);
	InitDlgItemSpin(hDlg, IDC_MAXSHADOW, TO_DISPLAY(lpData->lpMap->Pnt[0].y), NO, 0, MAX_SPIN);
}



//************************************************************************
//			get_histo
// PARAMETERS:
//			WORD wChannel- The channel to get.
//			LPINT lpbReverse- Filled with Reverse state channel.
//			LPINT lpNum- filled with the index of the histo.
// DESCRIPTION:
//			Gets/creates the given histogram.
//			Sets lpbReverse=TRUE if the histogram should be reversed.
// RETURNS:
//		The histogram for the given channel.
//************************************************************************
LOCAL LPDWORD  get_histo(LPDWORD FAR *lpHistos, WORD wChannel, LPINT lpbReverse, LPINT lpNum)
//************************************************************************
{
	int i, num;
	HWND hActiveWnd;
	LPDWORD lpHisto;
	LPIMAGE lpImage = NULL;
	BOOL bReverse;
	
	if (!lpHistos[0])
	{
	
		if (hActiveWnd = GetActiveDoc())
			lpImage =  GetImagePtr(hActiveWnd);
	
		if ( !(lpHisto = (LPDWORD)Alloc( 256 * 5 * sizeof(DWORD) )) )
			return(NULL);
		clr( (LPTR)lpHisto, 256 * 5 * sizeof(DWORD) );
		for (i=0;i<5;i++)
		{
			lpHistos[i] = lpHisto;
			lpHisto += 256;
		}
		lpGetHistos[0] = lpHistos[0];
		lpGetHistos[1] = lpHistos[1];
		lpGetHistos[2] = lpHistos[2];
		lpGetHistos[3] = lpHistos[3];
		lpGetHistos[4] = lpHistos[4];
		ReadEngineSelObj( lpImage, NULL, MapLineHistoProc );
	}
	
	switch (wChannel)
	{
		case IDC_DO_RED:		
			num = 1;
			bReverse = NO;
		break;
		case IDC_DO_GREEN:	
			num = 2;
			bReverse = NO;
		break;
		case IDC_DO_BLUE:		
			num = 3;
			bReverse = NO;
		break;
		case IDC_DO_CYAN:		
			num = 1;
			bReverse = YES;
		break;
		case IDC_DO_MAGENTA:
			num = 2;
			bReverse = YES;
		break;
		case IDC_DO_YELLOW:	
			num = 3;
			bReverse = YES;
		break;
		case IDC_DO_BLACK:	
			num = 4;
			bReverse = NO;
		break;
		case IDC_DO_ALL:
		default:
			num = 0;
			bReverse = NO;
		break;
	}

	lpHisto = lpHistos[num];
	
	if (lpNum)
		*lpNum = num;
	if (lpbReverse)
		*lpbReverse = bReverse;
	return(lpHisto);
}


//************************************************************************
LOCAL VOID free_hitograms(LPDWORD FAR *lpHistos)
//************************************************************************
{
	if(lpHistos[0])
		FreeUp((LPTR)lpHistos[0]);
	lpHistos[0] = NULL;
}

#define INC_HISTO(lpHisto, i) \
			{if (!++lpHisto[i]) lpHisto[i]--;}


//***********************************************************************
LOCAL void MapLineHistoProc( int y, int x1, int x2, LPTR lpSrc, FRMTYPEINFO TypeInfo )
//***********************************************************************
{
	int i, dx;
	LPDWORD lpHisto;
	RGBS rgb;
	LPRGB lpRGBmap;

	dx = x2 - x1 + 1;

	switch(TypeInfo.DataType)
	{
		case FDT_LINEART :
		case FDT_GRAYSCALE :
			lpHisto = lpGetHistos[0];
			while ( dx-- > 0 )
			{
				i = *lpSrc++;
				INC_HISTO(lpHisto, i);
			}
		break;

		case FDT_PALETTECOLOR :
			lpRGBmap = TypeInfo.ColorMap->RGBData;
			while ( dx-- > 0 )
			{
				rgb = lpRGBmap[*lpSrc++];
				// red
				INC_HISTO(lpGetHistos[1], rgb.red);
				// green
				INC_HISTO(lpGetHistos[2], rgb.green);
				// blue
				INC_HISTO(lpGetHistos[3], rgb.blue);
				// master
				i = TOLUM(rgb.red, rgb.green, rgb.blue);
				INC_HISTO(lpGetHistos[0], i);
			}
		break;

		case FDT_RGBCOLOR :
			while ( dx-- > 0 )
			{
				// red
				INC_HISTO(lpGetHistos[1], lpSrc[0]);
				// green
				INC_HISTO(lpGetHistos[2], lpSrc[1]);
				// blue
				INC_HISTO(lpGetHistos[3], lpSrc[2]);
				// master
				i = TOLUM(lpSrc[0], lpSrc[1], lpSrc[2]);
				INC_HISTO(lpGetHistos[0], i);

				lpSrc += 3;
			}
		break;

		case FDT_CMYKCOLOR :
			while ( dx-- > 0 )
			{
				// cyan
				INC_HISTO(lpGetHistos[1], lpSrc[0]);
				// magenta
				INC_HISTO(lpGetHistos[2], lpSrc[1]);
				// yellow
				INC_HISTO(lpGetHistos[3], lpSrc[2]);
				// black
				INC_HISTO(lpGetHistos[4], lpSrc[3]);
			
				// master
				i=CMYKtoL(LPCMYK(lpSrc));
				INC_HISTO(lpGetHistos[0], i);

				lpSrc += 4;
			}
		break;
	}
}



//************************************************************************
//		tone_activate_channel
// DESCRIPTION:
//			Sets up the given channel, Setting invert flags etc.
// RETURNS:
//			The map for the given channel.
//************************************************************************
LOCAL LPMAP tone_activate_channel(WORD wChannel )
//************************************************************************
{
	LPMAP lpMap; 

	switch ( wChannel )
	{
		case IDC_DO_ALL:
			lpMap = &MasterMap;
			lpMap->Reverse = YES;
		break;
		case IDC_DO_CYAN:
			lpMap = &RedMap;
			lpMap->Reverse = NO;
		break;
		case IDC_DO_MAGENTA:
			lpMap = &GreenMap;
			lpMap->Reverse = NO;
		break;
		case IDC_DO_YELLOW:
			lpMap = &BlueMap;
			lpMap->Reverse = NO;
		break;
		case IDC_DO_RED:
			lpMap = &RedMap;
			lpMap->Reverse = NO;
		break;
		case IDC_DO_GREEN:
			lpMap = &GreenMap;
			lpMap->Reverse = NO;
		break;
		case IDC_DO_BLUE:
			lpMap = &BlueMap;
			lpMap->Reverse = NO;
		break;
		case IDC_DO_BLACK:
			lpMap = &BlackMap;
			lpMap->Reverse = NO;
		break;
		default:
			lpMap = &MasterMap;
			lpMap->Reverse = NO;
		break;
	}

	return( lpMap );
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


//************************************************************************
//		compute_yscale
// RETURNS:
//			The 'largest' value for histogram
//************************************************************************
LOCAL long compute_yscale(LPDWORD lpHisto)
//************************************************************************
{
	long lMaxEntry;
	int i, j;
	long large, small1, temp;
	double sum;
	
	DWORD Data[256];
	
	copy((LPTR)lpHisto, (LPTR)Data, 256*sizeof(DWORD));
	
	// sort data
	for (i = 1; i < 256; ++i)
	{
		temp = Data[i];
		for (j = i-1; j >= 0 && (Data[j] > temp); --j)
			Data[j+1] = Data[j];
		Data[j+1] = temp;
	}
	
	sum = 0.0;
	for (i=0;i<256;i++)
		sum += (double)lpHisto[i];
	
	// rule out elements below .1%
	small1 = (LONG)Bound((sum/1000),0.0,LONG_MAX);
	for (i=0;i<256;i++)
		if (Data[i] > small1)
			break;
	small1 = Data[Bound((i+10), 0, 255)];
	
	// rule out elements above 20%
	large = (LONG)Bound((sum*2.0/10),0.0,LONG_MAX);
	for (i=255;i>=0;i--)
		if (Data[i] < large)
			break;
	large = Data[Bound((i-5), 0, 255)];

	// find average
	lMaxEntry = large + small1;
	return(lMaxEntry);
}


//************************************************************************
//		set_gradient.
// DESCRIPTION:
//		sets the given gradient control based on the channel.
//************************************************************************
LOCAL void set_gradient(HWND hDlg, int id, WORD wChannel)
//************************************************************************
{
	HWND hGrad;
	RGBS rgb1;
	RGBS rgb2;

	hGrad = GetDlgItem(hDlg, id);
	if (!hGrad)
		return;
	
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
		case IDC_DO_CYAN:		
			rgb1.red = 0;
			rgb1.green = rgb1.blue = 255;
			rgb2.red = rgb2.green = rgb2.blue = 255;
		break;
		case IDC_DO_MAGENTA:
			rgb1.green = 0;
			rgb1.red = rgb1.blue = 255;
			rgb2.red = rgb2.green = rgb2.blue = 255;
		break;
		case IDC_DO_YELLOW:	
			rgb1.blue = 0;
			rgb1.red = rgb1.green = 255;
			rgb2.red = rgb2.green = rgb2.blue = 255;
		break;
		case IDC_DO_BLACK:	
			rgb1.red = rgb1.green = rgb1.blue = 0;
			rgb2.red = rgb2.green = rgb2.blue = 255;
		break;
		case IDC_DO_ALL:
		default:
			rgb1.red = rgb1.green = rgb1.blue = 0;
			rgb2.red = rgb2.green = rgb2.blue = 255;
		break;
	}
	Gradient_SetRGB(hGrad, &rgb1, &rgb2);
	Gradient_SetType(hGrad, GT_RGB);																	  
}

	
#define GET_HISTO(hDlg) \
	(hDlg ? (LPHISTOSTRUCT)GetWindowLong(hDlg, DWL_USER) : NULL)

// LOCAL prototypes
LOCAL BOOL Histogram_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Histogram_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void Histogram_OnDestroy(HWND hDlg);


/***********************************************************************/
BOOL WINPROC EXPORT DlgHistogramProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch (msg)
	{
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, Histogram_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_DESTROY, Histogram_OnDestroy);
		HANDLE_DLGMSG(hDlg, WM_COMMAND, Histogram_OnCommand);

		default:
		return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_HISTOGRAM);
	}
}

/************************************************************************/
LOCAL BOOL Histogram_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	HWND hActiveWnd;
	HWND hTags;
	FRMDATATYPE type;
	LPIMAGE lpImage = NULL;
	LPHISTOSTRUCT lpData;
	int icons[HISTOMARKS];
	
	lpData = (LPHISTOSTRUCT)Alloc(sizeof(HISTOSTRUCT));
	if (!lpData)
		return(FALSE);
	SetWindowLong(hDlg, DWL_USER, (LONG)lpData);

	if (hActiveWnd = GetActiveDoc())
		lpImage = GetImagePtr(hActiveWnd);
	RightPopup( hDlg );
	
	// set channels combo
	type = FrameType(ImgGetBaseEditFrame(lpImage));
	lpData->wChannel = IDC_DO_ALL;
	ChannelsCombo_Setup(hDlg, IDC_CHANNELS, type, lpData->wChannel);

	// remove or add percents
//	SetAllDlgItemsText(hDlg, IDC_PERCENT, Mapping.Histogram_bPercent ? (LPSTR)"%":(LPSTR)"");
	
	// setup tags
	hTags = GetDlgItem(hDlg, IDC_TAGS);
	icons[0] = IDC_HISTOTAG1;
	icons[1] = IDC_HISTOTAG2;
	Tag_Init(hTags, HISTOMARKS, 255, &icons[0]);
	Tag_SetMark(hTags, 0, 85, NO);
	Tag_SetMark(hTags, 1, 170, NO);
	
	lpData->lpHistos[0] = NULL;
	
	// setup the actual histogram and controls 
	set_histo_channel(hDlg, lpData->wChannel);
	return(TRUE);
}

/***************************************************************************/
LOCAL void Histogram_OnDestroy(HWND hDlg)
/***************************************************************************/
{
	LPHISTOSTRUCT lpData;
	
	if (!(lpData = GET_HISTO(hDlg)))
		return;
//	Probe_End(hDlg);
	free_hitograms(lpData->lpHistos);
	// will free up structure
	Dialog_OnDestroy(hDlg);
}

/************************************************************************/
LOCAL void Histogram_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
	int i, val;
	LPHISTOSTRUCT lpData;
	
	if (!(lpData = GET_HISTO(hDlg)))
		return;

	switch (id)
	{
		case IDC_CHANNELS:
			i=ChannelsCombo_Handle(hDlg, id, codeNotify);
			if (!i)
				break;
			lpData->wChannel = i;
			set_histo_channel(hDlg, lpData->wChannel);
		break;
	
		case IDC_QUART1_PROBE:
//			if (!Probe_Begin(hDlg, codeNotify))
//				break;
//			CheckDlgButton( hDlg, id, YES );
//			lpData->iProbe = id-IDC_QUART1_PROBE;
//			lpData->OriginalValue = lpData->iMark;
		break;

		case IDC_HISTOGRAM:
			// value changed
			if (WITHIN(codeNotify, HTN_CHANGED_FIRST, HTN_CHANGED_LAST))
				i = codeNotify-HTN_CHANGED_FIRST;
			else if (WITHIN(codeNotify, HTN_CHANGING_FIRST, HTN_CHANGING_LAST))
				i = codeNotify-HTN_CHANGING_FIRST;
			else if (codeNotify = HTN_BROWSE)
			{
				val = Histo_GetBrowse(hControl);
				histo_browse(hDlg, val);
				break;
			}	
			else
				break;
			val = Histo_GetMark(hControl, i);
			histo_browse(hDlg, val);
			histo_change_mark(hDlg, lpData, i, val);
		break;

		case IDC_TAGS:
			// value changed
			if (WITHIN(codeNotify, TN_CHANGED_FIRST, TN_CHANGED_LAST))
				i = codeNotify-TN_CHANGED_FIRST;
			else if (WITHIN(codeNotify, TN_CHANGING_FIRST, TN_CHANGING_LAST))
				i = codeNotify-TN_CHANGING_FIRST;
			else
				break;
			val = Tag_GetMark(hControl, i);
			histo_browse(hDlg, val);
			histo_change_mark(hDlg, lpData, i, val);
		break;

		case IDOK:
		case IDCANCEL:
			// unpreview
			AstralDlgEnd( hDlg, id == IDOK );
		break;

   		default:
		break;
   }
}

//************************************************************************
//		set_histo_channel
// DESCRIPTION:
//		Makes the given channel the current channel.
//		Updates the display.
//************************************************************************
LOCAL VOID set_histo_channel(HWND hDlg, WORD wChannel)
//************************************************************************
{
	HWND hHisto, hTags;
	BOOL bInv, bMakeTotals;
	LPDWORD lpHisto;
	int i, j, iHisto;
	LONG sum;
	LONG lMaxEntry;
	LPDWORD lpdw;
	char szNum[40];
	LPHISTOSTRUCT lpData;
	
	if (!(lpData = GET_HISTO(hDlg)))
		return;
	bMakeTotals = (lpData->lpHistos[0] == NULL);
	lpHisto = get_histo(lpData->lpHistos, wChannel, &bInv, &iHisto);
	if (!lpHisto)
		return;
	
	// need to compute totals?
	if (bMakeTotals)
	{	
    	for (j=0;j<5;j++)
		{
			lpdw = lpData->lpHistos[j];
			sum = 0L;
			for (i=0;i<256;i++)
				sum += *lpdw++;
			lpData->lTotals[j] = sum;
		}
	}
	
	// setup the histo control
	lMaxEntry = compute_yscale(lpHisto);
	hHisto = GetDlgItem(hDlg, IDC_HISTOGRAM);
	Histo_Init(hHisto, lpHisto, lMaxEntry, HISTOMARKS, bInv ? HTS_FLIPX:0);
	// reset marks based on tag
	hTags = GetDlgItem(hDlg, IDC_TAGS);
	i = Tag_GetMark(hTags,0);
	Histo_SetMark(hHisto, 0, i, NO);
	i = Tag_GetMark(hTags,1);
	Histo_SetMark(hHisto, 1, i, NO);
	
	// setup the gradient
	set_gradient(hDlg, IDC_HISTO_GRAD, wChannel);
	
	// setup readouts
	SetDlgItemText(hDlg, IDC_HISTO_TOTAL, ltoa(lpData->lTotals[iHisto],szNum,10));
	SetDlgItemText(hDlg, IDC_HISTO_POS, (LPSTR)"");
	SetDlgItemText(hDlg, IDC_HISTO_VALUE, (LPSTR)"");
	histo_set_HMSvalues(hDlg, lpData);
}



//************************************************************************
//		histo_change_mark
// PARAMETERS:
//				int nMark-
//				int val-
// DESCRIPTION:
//		Coordinates the tag control and the histo control for the 
//		histogram dialog.  
//		Updates readouts for H,M&S
//************************************************************************
LOCAL void histo_change_mark(HWND hDlg, LPHISTOSTRUCT lpData, int nMark, int val)
//************************************************************************
{

	HWND hHisto, hTag;
	int i;
	int Marks[HISTOMARKS];
	
	hHisto = GetDlgItem(hDlg, IDC_HISTOGRAM);
	
	// get the marks
	for (i=0;i<HISTOMARKS;i++)
		Marks[i] = Histo_GetMark(hHisto, i);
	
	// set value and adjust others
	if (nMark == 0)
	{
		Marks[0] = val;
		if (Marks[1] < Marks[0])
			Marks[1] = Marks[0];
	}
	else if (nMark == 1)
	{
		Marks[1] = val;
		if (Marks[1] < Marks[0])
			Marks[0] = Marks[1];
	}
	
	// update anything that needs it
	hTag = GetDlgItem(hDlg, IDC_TAGS);
	for (i=0;i<HISTOMARKS;i++)
	{
		if (Marks[i] != Histo_GetMark(hHisto,i))
			Histo_SetMark(hHisto,i, Marks[i], NO);
		if (Marks[i] != Tag_GetMark(hTag,i))
			Tag_SetMark(hTag,i, Marks[i], NO);
	}
	
	// set HMS readouts
	histo_set_HMSvalues(hDlg, lpData);
}


//************************************************************************
// DESCRIPTION:
//		Sets the HMS readouts for the histo dialog based on the histogram control!
//************************************************************************
LOCAL void histo_set_HMSvalues(HWND hDlg, LPHISTOSTRUCT lpData)
//************************************************************************
{
	HWND hHisto;
	LPDWORD lpHisto;
	BOOL bInv;
	LONG lTotal;
	int iHisto;
	int i,j, start, end;
	int marks[HISTOMARKS];
	LONG sums[HISTOMARKS+1];
	double percents[HISTOMARKS+1];
	LONG sum;
	int add;
	char szDouble[30];

	lpHisto = get_histo(lpData->lpHistos, lpData->wChannel, &bInv, &iHisto);
	if (!lpHisto)
		return;
	lTotal = lpData->lTotals[iHisto];
	
	hHisto = GetDlgItem(hDlg, IDC_HISTOGRAM);
	
	// get the marks
	for (i=0;i<HISTOMARKS;i++)
	{
		marks[i] = Histo_GetMark(hHisto, i);
	}
	
	// get the three sums
	end = bInv ? 255:0;
	add = bInv ? -1:1;
	for (i=0;i<=HISTOMARKS;i++)
	{
		start = end;
		if (i >= HISTOMARKS)
			end = bInv ? 0:255;
		else
			end = bInv ? (255-marks[i]):marks[i];
		sum = 0L;
		for (j=start;j*add<end*add;j += add)
			sum += lpHisto[j];
		sums[i] = sum;
	}
	// scale if needed
	if (lTotal > 0x100000)
	{
		lTotal >>= 8L;
		for (i=0;i<=HISTOMARKS;i++)
			sums[i] >>= 8L;
	}
	// get percentages
	for (i=0;i<=HISTOMARKS;i++)
		percents[i] = (sums[i]*100.0 + (double)lTotal/2000)/lTotal;
	// write out percentages
	sprintf(szDouble, "%.1f", percents[2]);
	lstrcat(szDouble, (LPSTR)"%");
	SetDlgItemText(hDlg,IDC_HISTO_HI  ,szDouble);
	sprintf(szDouble, "%.1f", percents[1]);
	lstrcat(szDouble, (LPSTR)"%");
	SetDlgItemText(hDlg,IDC_HISTO_MID ,szDouble);
	sprintf(szDouble, "%.1f", percents[0]);
	lstrcat(szDouble, (LPSTR)"%");
	SetDlgItemText(hDlg,IDC_HISTO_SHAD,szDouble);
}



//************************************************************************
//		histo_browse
// DESCRIPTION:
//		Called when point is browsed in histo dialog
//************************************************************************
LOCAL void histo_browse(HWND hDlg, int x)
//************************************************************************
{
	BOOL bInv;
	LPDWORD lpHisto;
	LPHISTOSTRUCT lpData;
	
	if (!WITHIN(x,0,255))
	{
		SetDlgItemText(hDlg, IDC_HISTO_POS, (LPSTR)"");
		SetDlgItemText(hDlg, IDC_HISTO_VALUE, (LPSTR)"");
		return;
	}
	
	if (!(lpData = GET_HISTO(hDlg)))
		return;

	lpHisto = get_histo(lpData->lpHistos, lpData->wChannel, &bInv);
	if (!lpHisto)
		return;
		
	if (Control.DisplayPixelValue)
		SetDlgItemInt(hDlg, IDC_HISTO_POS, x, FALSE);
	else
		SetDlgItemIntExt(hDlg, IDC_HISTO_POS, x*100L/255, "%");
		
	if (bInv)
		x = 255-x;
	SetDlgItemInt(hDlg, IDC_HISTO_VALUE, lpHisto[x], FALSE);
}

