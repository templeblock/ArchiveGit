// (c) Copyright 1995 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//-------------------------------------------------------------------
//                          dlgsmap.cpp 
//-------------------------------------------------------------------

#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "mapctl.h"
#include "gradctl.h"
#include "ctlutil.h"
#include "histoctl.h"
#include "tagctl.h"
#include "ctxhelp.h"
#include "ppmodal.h"
#include "svrview.h"		
#include "visual.h"
#include "dlgsmap.h"


#define MAP_TO_DISPLAY(i) \
	(m_bPercentages ? (TOPERCENT(i)) : i)

#define DISPLAY_TO_MAP(i) \
	(m_bPercentages ? (TOGRAY(i)) : i)

#define INIT_MAPIN_SPIN(hWnd, id, index) \
		{InitDlgItemSpin(hWnd, id, 0, NO, 0, m_bPercentages ? 100:255 ); \
		if (m_MapValues.ptNumeric[index].x == INT_MAX) SetDlgItemText(id, ""); \
		else SetDlgItemSpin( hWnd, id, MAP_TO_DISPLAY(m_MapValues.ptNumeric[index].x), NO );} \

#define INIT_MAPOUT_SPIN(hWnd, id, index) \
		{InitDlgItemSpin(hWnd, id, 0, NO, 0, m_bPercentages ? 100:255  ); \
		if (m_MapValues.ptNumeric[index].y == INT_MAX) SetDlgItemText(id, ""); \
		else SetDlgItemSpin( hWnd, id, MAP_TO_DISPLAY(m_MapValues.ptNumeric[index].y), NO );} \

#define GET_HISTO(hDlg) \
	(hDlg ? (LPHISTOSTRUCT)GetWindowLong(hDlg, DWL_USER) : NULL)

#define INC_HISTO(lpHisto, i) \
			{if (!++lpHisto[i]) lpHisto[i]--;}

//BOOL IsInPreview;
__declspec( dllimport ) LPKCMSEVALPROC	lpKCMSEvalProc;



#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(CPPMapDlg, CPPViewModalDlg)
IMPLEMENT_DYNAMIC(CPPMapExDlg, CPPMapDlg)
IMPLEMENT_DYNAMIC(CPPVisualMapDlg, CPPMapDlg)
IMPLEMENT_DYNAMIC(CModifyOptionsDlg, CPPModalDlg)
IMPLEMENT_DYNAMIC(CMapSaveDlg, CPPModalDlg)
IMPLEMENT_DYNAMIC(CMapLoadDlg, CPPModalDlg)
IMPLEMENT_DYNAMIC(CModifyColorMapsDlg, CPPMapDlg)
IMPLEMENT_DYNAMIC(CContrastBrightnessVisualDlg, CPPVisualMapDlg)
IMPLEMENT_DYNAMIC(CContrastBrightnessJoystickDlg, CPPMapDlg)
IMPLEMENT_DYNAMIC(CColorBalanceJoystickDlg, CPPMapDlg)
IMPLEMENT_DYNAMIC(CColorBalanceVisualDlg, CPPVisualMapDlg)
IMPLEMENT_DYNAMIC(CThresholdDlg, CPPMapDlg)
IMPLEMENT_DYNAMIC(CPosterizeDlg, CPPMapDlg)
IMPLEMENT_DYNAMIC(CHueShiftDlg, CPPMapDlg)
IMPLEMENT_DYNAMIC(CHueMapDlg, CPPMapDlg)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////  CPPMapDlg  ///////////////////////////////////////////////////////////
/////////////////////////// Virtual Base Class for All Mapping Dialogs ///////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////



BOOL CPPMapDlg::PreDoModal(LPVOID lpVoid)
{
	m_bProbing = NO;
	m_bPreviewTimer = NO;
	m_bIsInPreview = NO;
	return CPPViewModalDlg::PreDoModal(lpVoid);
}

void CPPMapDlg::OnOK()
{
	CPPViewModalDlg::OnOK();
}

void CPPMapDlg::OnCancel()
{
	CPPViewModalDlg::OnCancel();
}

LPVOID CPPMapDlg::PostDoModal()
{
	return CPPViewModalDlg::PostDoModal();
}

CPPMapDlg::CPPMapDlg (CServerView *pView, UINT nIDTemplate, 
		CWnd* pParentWnd) : CPPViewModalDlg (pView, nIDTemplate, pParentWnd)
{
	m_fMyLuts = FALSE;
}

BOOL CPPMapDlg::OnInitDialog()
{
	return CPPViewModalDlg::OnInitDialog();
}




// CPPMapDlg:UnAutoPreview: Turns off both autopreview and palette animation 
VOID CPPMapDlg::UnAutoPreview()
{
	UndoMapPreview();
	SetMapGlobals();
	AnimateMap(NULL, YES);
}

//CPPMapDlg::ChannelsCombo_Setup:  Sets up the given combo for the given frame type
VOID CPPMapDlg::ChannelsCombo_Setup(int idControl, FRMDATATYPE type)
{
	switch(type)
	{
		case FDT_PALETTECOLOR:
		case FDT_RGBCOLOR:
			m_iCombo[0] = IDC_DO_ALL;
			m_iCombo[1] = IDC_DO_RED;
			m_iCombo[2] = IDC_DO_GREEN;
			m_iCombo[3] = IDC_DO_BLUE;
			m_nCombo = 4;
		break;
		case FDT_CMYKCOLOR:
			m_iCombo[0] = IDC_DO_ALL;
			m_iCombo[1] = IDC_DO_CYAN;
			m_iCombo[2] = IDC_DO_MAGENTA;
			m_iCombo[3] = IDC_DO_YELLOW;
			m_iCombo[4] = IDC_DO_BLACK;
			m_nCombo = 5;
		break;
		case FDT_GRAYSCALE:
		default:
			m_iCombo[0] = IDC_DO_ALL;
			m_nCombo = 1; 
		break;
	}
	
	TableCheckComboItem(GetSafeHwnd(), idControl, &m_iCombo[0], m_nCombo, m_MapValues.wChannel, NO);
	if (type == FDT_GRAYSCALE)
		ControlEnable(GetSafeHwnd(), idControl, NO);
}		




void CPPMapDlg::Hue_SetPatch(ITEMID idPatch, int hue, BOOL bSetText)
{
	COLOR Color;
	RGBS rgb;
	HSLS hsl;
	HWND hDlg = GetSafeHwnd();

	hsl.hue = hue;
	hsl.sat = 255;
	hsl.lum = 127;
	HSLtoRGB( hsl.hue, hsl.sat, hsl.lum, &rgb );
	CopyRGB( &rgb, &Color );
	SetWindowLong( ::GetDlgItem( hDlg, idPatch ), GWL_ACTIVE, Color );
	AstralControlRepaint( hDlg, idPatch );

	if (!bSetText)
		return;
	if (WITHIN(idPatch, IDC_HUE1,IDC_HUE11))
		SetDlgItemIntExt(hDlg, (LONG)idPatch-(LONG)IDC_HUE1+(LONG)IDC_MAP0OUT, TOANGLE(hue), "\260");
	else if (WITHIN(idPatch, IDC_INHUE1,IDC_INHUE11))
		SetDlgItemIntExt(hDlg, (LONG)idPatch-(LONG)IDC_INHUE1+(LONG)IDC_MAP0IN, TOANGLE(hue), "\260");
}

//				Handle WM_COMMAND from map options button
BOOL CPPMapDlg::HandleMapOptions(ITEMID id, UINT codeNotify)
{
	HWND hMap;
	int x, y;
	HWND hDlg = GetSafeHwnd();
	
	CModifyOptionsDlg dlg(this);
	if(dlg.DoModal()==IDOK)
	{
		PutDefaultInt(MODIFYCOLORMAPS_AUTOPREVIEW, m_bAutoPreview);
		PutDefaultInt(MODIFYCOLORMAPS_POINTS, m_nPoints);
		PutDefaultInt(MODIFYCOLORMAPS_GRID, m_bGrid);
		PutDefaultInt(MODIFYCOLORMAPS_PERCENTAGES, m_bPercentages);
		PutDefaultInt(MODIFYCOLORMAPS_INTENSITY, m_bIntensity);
		PutDefaultInt(MODIFYCOLORMAPS_CURVES, m_bCurves);
	}				

	    INIT_MAPIN_SPIN(hDlg, IDC_MAP0IN, 0);
    INIT_MAPIN_SPIN(hDlg, IDC_MAP1IN, 1);
    INIT_MAPIN_SPIN(hDlg, IDC_MAP2IN, 2);
    INIT_MAPIN_SPIN(hDlg, IDC_MAP3IN, 3);
    INIT_MAPIN_SPIN(hDlg, IDC_MAP4IN, 4);
    INIT_MAPIN_SPIN(hDlg, IDC_MAP5IN, 5);
    INIT_MAPIN_SPIN(hDlg, IDC_MAP6IN, 6);
    INIT_MAPIN_SPIN(hDlg, IDC_MAP7IN, 7);
    INIT_MAPIN_SPIN(hDlg, IDC_MAP8IN, 8);
    INIT_MAPIN_SPIN(hDlg, IDC_MAP9IN, 9);
    INIT_MAPIN_SPIN(hDlg, IDC_MAP10IN,10);
    INIT_MAPOUT_SPIN(hDlg, IDC_MAP0OUT, 0);
    INIT_MAPOUT_SPIN(hDlg, IDC_MAP1OUT, 1);
    INIT_MAPOUT_SPIN(hDlg, IDC_MAP2OUT, 2);
    INIT_MAPOUT_SPIN(hDlg, IDC_MAP3OUT, 3);
    INIT_MAPOUT_SPIN(hDlg, IDC_MAP4OUT, 4);
    INIT_MAPOUT_SPIN(hDlg, IDC_MAP5OUT, 5);
    INIT_MAPOUT_SPIN(hDlg, IDC_MAP6OUT, 6);
    INIT_MAPOUT_SPIN(hDlg, IDC_MAP7OUT, 7);
    INIT_MAPOUT_SPIN(hDlg, IDC_MAP8OUT, 8);
    INIT_MAPOUT_SPIN(hDlg, IDC_MAP9OUT, 9);
    INIT_MAPOUT_SPIN(hDlg, IDC_MAP10OUT,10);

	// remove or add percents
	SetAllDlgItemsText(hDlg, IDC_PERCENT, m_bPercentages ? (LPSTR)"%":(LPSTR)"");

	// set curves, etc
	if (m_MapValues.pMap->iCurves)
		m_MapValues.pMap->iCurves = m_nCurveType;
		
	Map_DlgSetNewMap(m_MapValues.pMap);
	// turn grid on/off
	if (m_bGrid)
		x = y = GRIDSIZE;
	else
		x = y = 0;
	hMap = ::GetDlgItem(hDlg, IDC_MAP);
//	Map_Init(hMap, x, y, -1.0, -1, -1);

//let the kludge begin.....
	Map_Init(hMap, x, y, m_MapValues.pMap->gamma, 11, 0);
	SetWindowWord(::GetDlgItem(hDlg, id), GWW_ICONID, m_MapValues.pMap->iCurves ? IDC_MAP_BEZIER:IDC_MAP_LINES);
	Map_SetStyle(hMap, MS_CURVES, m_MapValues.pMap->iCurves);
//end kludge

	UpdateCurrentMap();
	SetMapGlobals();
	AutoPreview(m_MapValues.pMap, m_bAutoPreview, YES);
	ControlEnable(hDlg, IDC_PREVIEW, !m_bAutoPreview);
	return(TRUE);
}



////////////////// CPPMapDlg::HandleResetAll   Handles IDC_RESETALL                     //////////////////////////
////////////////// 		ITEMID         id     	    ID of the map control               //////////////////////////
////////////////// 		UINT           codeNotify   notification code from map control	//////////////////////////
////////////////// 		LPMAPVALUES    lpValues     map values pointer					//////////////////////////



BOOL CPPMapDlg::HandleResetAll()
{	
	ResetAllMapsEx(-1, -1);
	m_MapValues.pMap = ResetMapChannel(m_MapValues.wChannel );

	SetMapGlobals();
	AutoPreview(m_MapValues.pMap, m_bAutoPreview, YES);
	return(TRUE);
}


////////////////// CPPMapDlg::map_to_mapinfo  Copies selected data from*(lpMap) into *(lpInfo)				//////
//////////////////		LPMAP		   lpMap		pointer to map structure from which data will be copied //////
//////////////////		LPMAPINFO	   lpInfo		pointer to structure that will receive the copied data  //////
	
void CPPMapDlg::map_to_mapinfo(LPMAP lpMap, LPMAPINFO lpInfo)
{
	lpInfo->Reverse = lpMap->Reverse; 
	lpInfo->iCurves = lpMap->iCurves; 
	lpInfo->gamma = lpMap->gamma; 
	lpInfo->Points = lpMap->Points; 
	copy((LPTR)&lpMap->Pnt, (LPTR)&lpInfo->Pnt, sizeof(lpInfo->Pnt));
}


////////////////// CPPMapDlg::HandleMapCommand	Handles WM_COMMAND messages for any mapping dialog ///////////////
////////////////// 		ITEMID			id			ID of the map control						   ///////////////
//////////////////		UINT			codeNotify	control notification code					   ///////////////
//////////////////		LPMAPVALUES 	lpValues 	map values pointer							   ///////////////


BOOL CPPMapDlg::HandleMapCommand(ITEMID id, UINT codeNotify)
{
	BOOL fModified;
	HWND hDlg = GetSafeHwnd();

	switch (id)
	{
	case IDC_CHANNELS:
		fModified = HandleMapChannels(id, codeNotify);
	break;

	case IDC_MAP:
		fModified = HandleMap(id, codeNotify);
	break;

	case IDC_PREVIEW:
		fModified = HandleMapPreview();
	break;

	case IDC_MAPTOOL:
		fModified = HandleMapTool(id, codeNotify);
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
		fModified = HandleMapIn(id, codeNotify);
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
		fModified = HandleMapOut(id, codeNotify);
	break;

	case IDC_MAP_OPTIONS:
		fModified = HandleMapOptions(id, codeNotify);
	break;

	case IDC_MAP_CURVES:
		fModified = HandleMapCurves(id);
	break;

	case IDC_MAP_GAMMA:
		fModified = HandleMapGamma(id, codeNotify);
	break;

	case IDC_MAP_SHIFT_UP:
	case IDC_MAP_SHIFT_DOWN: 
	case IDC_MAP_SHIFT_LEFT: 
	case IDC_MAP_SHIFT_RIGHT:
		fModified = HandleMapShift(id);
	break;

	case IDC_RESET:
		fModified = HandleReset();
	break;

	case IDC_RESETALL:
		fModified = HandleResetAll();
	break;

	case IDC_MAP_HBAR:
	case IDC_MAP_VBAR:
		fModified = HandleMapGradient();
	break;

	case IDC_MAP_PROBE:
		fModified = HandleMapProbe(codeNotify);
	break;

	default:
		return(FALSE);
	}
	if (fModified)
		m_MapValues.fModified = TRUE;
	return(TRUE);
}


//////////////// CPPMapDlg::Probe_Begin  Called in response to probe button hit.  //////////////////////////////////


BOOL CPPMapDlg::Probe_Begin(UINT codeNotify)
{
	// Be sure its not a double click
	if ( codeNotify == 1 || !GetViewHwnd())
		return(FALSE);
	if ( !QuickProbeStart( GetSafeHwnd() ) )
		return(FALSE);
	m_bProbing = TRUE;
    m_iBubbles = HelpContext.BubbleHintsOn(HL_BUBBLE_QUERY);
    HelpContext.BubbleHintsOn(FALSE);
	CheckDlgButton(IDC_MAP_PROBE, m_bProbing);
	return(TRUE);
}

void CPPMapDlg::HandleMapLoad( )
{
	m_MapValues.pMap = ResetMapChannel( m_MapValues.wChannel );
	AutoPreview(m_MapValues.pMap, m_bAutoPreview, YES);
	m_MapValues.fModified = TRUE;
	ControlEnable(GetSafeHwnd(), IDOK, m_MapValues.fModified);
}

////////////	 CPPMapDlg::HandleMapProbe	Handles IDC_MAP_PROBE


BOOL CPPMapDlg::HandleMapProbe(UINT codeNotify)
{	
	if (m_bProbing)
		Probe_End();
	else
		Probe_Begin(codeNotify);
	return(FALSE);
}


//				Handles IDC_VBAR, IDC_HBAR


BOOL CPPMapDlg::HandleMapGradient()
{	
	m_bIntensity = !m_bIntensity;

	MapSetGradients();
	// set invert syles
	Map_SetStyle(::GetDlgItem(GetSafeHwnd(), IDC_MAP), MS_INVERTX | MS_INVERTY, !m_bIntensity);
	return(FALSE);
}

//					ITEMID id- The id of the map control.
//					UINT codeNotify- notification code from map control
//				Handle WM_COMMAND from map control.


BOOL CPPMapDlg::HandleMap( ITEMID id, UINT codeNotify)
{
	HWND hDlg = GetSafeHwnd();
	HWND hMap;
	POINT pt;

	hMap = ::GetDlgItem(hDlg, id);
	if ( codeNotify == MN_POINTCHANGED || codeNotify == MN_POINTCHANGING)
	{
		if (!Map_GetActivePoint(hMap, &pt))
			pt.x = pt.y = -1;
		
		m_MapValues.pMap->Points = Map_GetPointList(hMap, m_MapValues.pMap->Pnt, MAPPOINTS); 
		if (GetViewHwnd())
		{
			if (codeNotify == MN_POINTCHANGED)
				{
				SetMapGlobals();
				AutoPreview(m_MapValues.pMap, m_bAutoPreview, YES);
				}
			else
				{
				SetMapGlobals();
				AutoPreview(m_MapValues.pMap, m_bAutoPreview, NO);
				}
		}
	}
	else if (codeNotify == MN_BROWSE)
	{
		if (!Map_GetBrowsePoint(hMap, &pt))
			pt.x = pt.y = -1;
	}
	else
		return(FALSE);
	DisplayInOutValues(pt.x, pt.y);
	return(TRUE);
}

//					ITEMID id- The id of the map control.
//					UINT codeNotify- notification code from map control
//				Handle WM_COMMAND from map control.

BOOL CPPMapDlg::HandleMapTool(ITEMID id, UINT codeNotify)
{	
	HWND hDlg = GetSafeHwnd();
	
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		return(FALSE);

	// no cange? 
	if (id == IDC_MAPTOOLV && !m_MapValues.bMap_Visual)
		return(FALSE);
	if (id != IDC_MAPTOOLV && m_MapValues.bMap_Visual)
		return(FALSE);
	
	UndoMapPreview();

	// get the current map set
	UpdateCurrentMap();
	MapActivatePane(id);
	m_MapValues.pMap = ResetMapChannel();
	SetMapGlobals();
	AutoPreview(m_MapValues.pMap, m_bAutoPreview, YES);
	return(FALSE);
}

void CPPMapDlg::MapActivatePane(ITEMID idTool)
{
	// no change? 
	if (idTool == IDC_MAPTOOLV && !m_MapValues.bMap_Visual)
		return;
	if (idTool != IDC_MAPTOOLV && m_MapValues.bMap_Visual)
		return;
	m_MapValues.bMap_Visual = (idTool != IDC_MAPTOOLV);

	if ( m_MapValues.bMap_Visual )
		SelectDlgSectionCategory(&m_MapValues.MapSec, IDD_MAPVISUAL);
	else
    {
		SelectDlgSectionCategory(&m_MapValues.MapSec, IDD_MAPNUMERIC);
    
	    HWND hDlg = GetSafeHwnd();
		SetSpinBuddy(hDlg, IDC_MAP0IN);
		SetSpinBuddy(hDlg, IDC_MAP1IN);
		SetSpinBuddy(hDlg, IDC_MAP2IN);
		SetSpinBuddy(hDlg, IDC_MAP3IN);
		SetSpinBuddy(hDlg, IDC_MAP4IN);
		SetSpinBuddy(hDlg, IDC_MAP5IN);
		SetSpinBuddy(hDlg, IDC_MAP6IN);
		SetSpinBuddy(hDlg, IDC_MAP7IN);
		SetSpinBuddy(hDlg, IDC_MAP8IN);
		SetSpinBuddy(hDlg, IDC_MAP9IN);
		SetSpinBuddy(hDlg, IDC_MAP10IN);
		SetSpinBuddy(hDlg, IDC_MAP0OUT);
		SetSpinBuddy(hDlg, IDC_MAP1OUT);
		SetSpinBuddy(hDlg, IDC_MAP2OUT);
		SetSpinBuddy(hDlg, IDC_MAP3OUT);
		SetSpinBuddy(hDlg, IDC_MAP4OUT);
		SetSpinBuddy(hDlg, IDC_MAP5OUT);
		SetSpinBuddy(hDlg, IDC_MAP6OUT);
		SetSpinBuddy(hDlg, IDC_MAP7OUT);
		SetSpinBuddy(hDlg, IDC_MAP8OUT);
		SetSpinBuddy(hDlg, IDC_MAP9OUT);
		SetSpinBuddy(hDlg, IDC_MAP10OUT);

        INIT_MAPIN_SPIN(hDlg, IDC_MAP0IN, 0);
        INIT_MAPIN_SPIN(hDlg, IDC_MAP1IN, 1);
        INIT_MAPIN_SPIN(hDlg, IDC_MAP2IN, 2);
        INIT_MAPIN_SPIN(hDlg, IDC_MAP3IN, 3);
        INIT_MAPIN_SPIN(hDlg, IDC_MAP4IN, 4);
        INIT_MAPIN_SPIN(hDlg, IDC_MAP5IN, 5);
        INIT_MAPIN_SPIN(hDlg, IDC_MAP6IN, 6);
        INIT_MAPIN_SPIN(hDlg, IDC_MAP7IN, 7);
        INIT_MAPIN_SPIN(hDlg, IDC_MAP8IN, 8);
        INIT_MAPIN_SPIN(hDlg, IDC_MAP9IN, 9);
        INIT_MAPIN_SPIN(hDlg, IDC_MAP10IN,10);
        
        INIT_MAPOUT_SPIN(hDlg, IDC_MAP0OUT, 0);
        INIT_MAPOUT_SPIN(hDlg, IDC_MAP1OUT, 1);
        INIT_MAPOUT_SPIN(hDlg, IDC_MAP2OUT, 2);
        INIT_MAPOUT_SPIN(hDlg, IDC_MAP3OUT, 3);
        INIT_MAPOUT_SPIN(hDlg, IDC_MAP4OUT, 4);
        INIT_MAPOUT_SPIN(hDlg, IDC_MAP5OUT, 5);
        INIT_MAPOUT_SPIN(hDlg, IDC_MAP6OUT, 6);
        INIT_MAPOUT_SPIN(hDlg, IDC_MAP7OUT, 7);
        INIT_MAPOUT_SPIN(hDlg, IDC_MAP8OUT, 8);
        INIT_MAPOUT_SPIN(hDlg, IDC_MAP9OUT, 9);
        INIT_MAPOUT_SPIN(hDlg, IDC_MAP10OUT,10);
		
		SetAllDlgItemsText(hDlg, IDC_PERCENT, m_bPercentages ? (LPSTR)"%":(LPSTR)"");

    }

	CheckComboItem( GetSafeHwnd(), IDC_MAPTOOL, IDC_MAPTOOL3, IDC_MAPTOOLV, idTool );
}



//					ITEMID id- The id of the map control.
//					UINT codeNotify- notification code from map control
//				Handle WM_COMMAND from map control.


BOOL CPPMapDlg::HandleMapIn(ITEMID id, UINT codeNotify)
{
	HWND hDlg = GetSafeHwnd();
	
	int i;
	BOOL Bool;

 	if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
		return(FALSE);
	if ( codeNotify != EN_CHANGE)
		return(FALSE);
	i = GetDlgItemSpin( hDlg, id, &Bool, NO );
	if (!Bool)
	{
		if (m_MapValues.ptNumeric[id-IDC_MAP0IN].x == INT_MAX)
			return(FALSE);
		m_MapValues.ptNumeric[id-IDC_MAP0IN].x = INT_MAX;
	}
	else
		m_MapValues.ptNumeric[id-IDC_MAP0IN].x = DISPLAY_TO_MAP(i);
	if (m_MapValues.ptNumeric[id-IDC_MAP0IN].y == INT_MAX)
		return(FALSE);
	UpdateCurrentMap();
	SetMapGlobals();
	AutoPreview(m_MapValues.pMap, m_bAutoPreview, YES);
	return(TRUE);			  
}

//					ITEMID id- The id of the map control.
//					UINT codeNotify- notification code from map control
//				Handle WM_COMMAND from map control.


BOOL CPPMapDlg::HandleMapOut(ITEMID id, UINT codeNotify)
{	
	HWND hDlg = GetSafeHwnd();

	int i;
	BOOL Bool;

	if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
		return(FALSE);
	if ( codeNotify != EN_CHANGE)
		return(FALSE);
	i = GetDlgItemSpin(hDlg, id, &Bool, NO );
	if (!Bool)
	{
		if (m_MapValues.ptNumeric[id-IDC_MAP0IN].y == INT_MAX)
			return(FALSE);
		m_MapValues.ptNumeric[id-IDC_MAP0OUT].y = INT_MAX;
	}
	else
		m_MapValues.ptNumeric[id-IDC_MAP0OUT].y = DISPLAY_TO_MAP(i);
	if (m_MapValues.ptNumeric[id-IDC_MAP0OUT].x == INT_MAX)
		return(FALSE);
	UpdateCurrentMap();
	SetMapGlobals();
	AutoPreview(m_MapValues.pMap, m_bAutoPreview, YES);
	return(TRUE);
}

//					ITEMID id- The id of the map control.
//				Handles IDC_SHIFT_???

BOOL CPPMapDlg::HandleMapShift(ITEMID id)
{	
	int x, y, num, i;
	HWND hMap;
	HWND hDlg = GetSafeHwnd();

	// shift entire map
	x = ((id==IDC_MAP_SHIFT_LEFT) ? 5 : ((id==IDC_MAP_SHIFT_RIGHT) ? -5:0));
	y = ((id==IDC_MAP_SHIFT_UP) ? 5 : ((id==IDC_MAP_SHIFT_DOWN) ? -5:0));
	if (!m_bIntensity)
	{
		x = -x;
		y = -y;
	}
	hMap = ::GetDlgItem( hDlg, IDC_MAP );
	num = Map_GetPointList(hMap, m_MapValues.pMap->Pnt, MAPPOINTS, NO); 
	m_MapValues.pMap->Points = num;
	for(i=0;i<num;i++)
	{
		m_MapValues.pMap->Pnt[i].x += x;
		m_MapValues.pMap->Pnt[i].y += y;
	}
	Map_SetPointList(hMap, m_MapValues.pMap->Pnt, num);
	// to ensure bounded values - get 'em again
	m_MapValues.pMap->Points = Map_GetPointList(hMap, m_MapValues.pMap->Pnt, MAPPOINTS); 
	SetMapGlobals();
	AutoPreview(m_MapValues.pMap, m_bAutoPreview, NO);
	return(TRUE);
}


//				Handles IDC_PREVIEW_???

BOOL CPPMapDlg::HandleMapPreview()
{	
	UpdateCurrentMap();
	AutoPreview_Button(m_MapValues.pMap);
	return(FALSE);
}

//					LPMAPVALUES lpValues - map values pointer
//				Handles IDC_RESET

BOOL CPPMapDlg::HandleReset()
{	
	ResetMapEx( m_MapValues.pMap, m_nPoints, -1, m_MapValues.wChannel == IDC_DO_ALL);
	m_MapValues.pMap->iCurves = m_bCurves ? m_nCurveType:0;
	m_MapValues.pMap = ResetMapChannel(m_MapValues.wChannel );
	if (GetViewHwnd())
		{
		SetMapGlobals();
		AutoPreview(m_MapValues.pMap, m_bAutoPreview, YES);
		}
	return(TRUE);
}



//					ITEMID id- The id of the map control.
//					UINT codeNotify- notification code from map control
//					LPMAPVALUES lpValues - map values pointer
//				Handles IDC_CHANNELS


//DIRTY
BOOL CPPMapDlg::HandleMapChannels(ITEMID id, UINT codeNotify)
{	
	
	int i;

	i=ChannelsCombo_Handle(id, codeNotify);
	if (!i)
		return(FALSE);
	UpdateCurrentMap();
	m_MapValues.wChannel = i;
	MapSetGradients();
	m_MapValues.pMap = ResetMapChannel(m_MapValues.wChannel );
	return(FALSE);
}

//		sets the given gradient control based on the channel.
void CPPMapDlg::MapSetGradients()
{
	HWND hBar;
	RGBS rgb1;
	RGBS rgb2;
	RGBS rgbTemp;

	switch (m_MapValues.wChannel)
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
			m_bIntensity = !m_bIntensity;
		case IDC_CAL_MASTER:
		case IDC_DO_ALL:
		default:
			rgb1.red = rgb1.green = rgb1.blue = 0;
			rgb2.red = rgb2.green = rgb2.blue = 255;
		break;
	}
	
	if (m_bIntensity)
	{
		rgbTemp = rgb1;
		rgb1 = rgb2;
		rgb2 = rgbTemp;
	}
		
	if (hBar = ::GetDlgItem(GetSafeHwnd(), IDC_MAP_HBAR))
	{
		Gradient_SetRGB(hBar, &rgb2, &rgb1);
		Gradient_SetType(hBar, GT_RGB);																	  
	}
	if (hBar = ::GetDlgItem(GetSafeHwnd(),IDC_MAP_VBAR))
	{
		Gradient_SetRGB(hBar, &rgb1, &rgb2);
		Gradient_SetType(hBar, GT_RGB);																	  
	}
}

MAP* CPPMapDlg::ResetMapChannel(WORD wChannel )
{
	m_MapValues.wChannel = wChannel;
	return(ResetMapChannel());
}

MAP* CPPMapDlg::ResetMapChannel()
{
	
	HWND hDlg = GetSafeHwnd();

	int i;
	
	m_MapValues.pMap = ActivateChannel( m_MapValues.wChannel );
	if (m_MapValues.wChannel == IDC_DO_ALL)
	{
		// master channel is always reversed in map dialogs
		int ReverseLUTXY = m_MapValues.pMap->Reverse & REVERSE_LUTXY;
		m_MapValues.pMap->Reverse = YES;
		m_MapValues.pMap->Reverse |= ReverseLUTXY;
	}
	
	Map_DlgSetNewMap(m_MapValues.pMap);
	
	// copy lpMap's point to lpValues->ptNumericptNumeric, marking invalid points
	copy((LPTR)m_MapValues.pMap->Pnt, (LPTR)m_MapValues.ptNumeric, MAPPOINTS*sizeof(POINT));
	for (i=1; i<MAPPOINTS;i++)
	{
		if (m_MapValues.ptNumeric[i].x == 0)
			m_MapValues.ptNumeric[i].x = m_MapValues.ptNumeric[i].y = INT_MAX;
	}

					
	if (::GetDlgItem(hDlg, IDC_MAPIN))
	{
		SetDlgItemText(IDC_MAPIN, " ");
		SetDlgItemText(IDC_MAPOUT, " ");

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
									   
	return( m_MapValues.pMap );
}



//		Hanldes combo set up by ChannelsCombo_Setup. 
// RETURNS:
//		Tthe item hit in the combo, or 0 for failure.

//CLEAN
int CPPMapDlg::ChannelsCombo_Handle(int idControl, UINT codeNotify)
{
	int id;
	
	id = TableHandleCombo( GetSafeHwnd(), &m_iCombo[0], m_nCombo, idControl, codeNotify);
	if (!WITHIN(id, IDC_DO_ALL, IDC_DO_BLACK))
		return(0);
	return(id);
}

//			Sets lpMap's points based on the Map Control or the Numeric 
//			Combos.

VOID CPPMapDlg::UpdateCurrentMap()
{
	HWND hMap;

	if (m_MapValues.bMap_Visual)
	{
		hMap = ::GetDlgItem(GetSafeHwnd(), IDC_MAP);
		m_MapValues.pMap->Points = Map_GetPointList(hMap, m_MapValues.pMap->Pnt, MAPPOINTS); 
	}
	else
	{
		m_MapValues.pMap->Points = MAPPOINTS;
		copy((LPTR)m_MapValues.ptNumeric, (LPTR)m_MapValues.pMap->Pnt, MAPPOINTS*sizeof(POINT));
		m_MapValues.pMap->Points = order_points( m_MapValues.pMap->Pnt, m_MapValues.pMap->Points);
	}
}

//			Puts points in ascending order, removing duplicates.
//			Will clear all 'extra' points (up to MAPPOINTS).
//			Assumes bounded x values.
//			points with x or y == INT_MAX are discarded.

//CLEAN
int CPPMapDlg::order_points( LPPOINT lpPoints, int nPoints)
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
//					LPMAPVALUES lpValues - map values pointer
//				Handle WM_INITDIALOG for any mapping dialog

//DIRTY
void CPPMapDlg::HandleMapInit(FRMDATATYPE type, BOOL fUseProbe)
{
	HWND hDlg = GetSafeHwnd();
	int x,y;
	HWND hMap;
	WORD wStyles;

	m_MapValues.fModified = FALSE;

	MapSetupPanes(fUseProbe);
	
	if (::GetDlgItem(hDlg, IDC_CHANNELS))
		ChannelsCombo_Setup(IDC_CHANNELS, type);
	
	// remove or add percents
	SetAllDlgItemsText(hDlg, IDC_PERCENT, m_bPercentages ? (LPSTR)"%":(LPSTR)"");
	
	// initialize the map conrol 
	wStyles = 0;
	if (!m_bIntensity)
		wStyles |= (MS_INVERTX | MS_INVERTY);
	if (m_bGrid)
		x = y = GRIDSIZE;
	else
		x = y = 0;	
	hMap = ::GetDlgItem( hDlg, IDC_MAP );
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
	
	MapSetGradients();
	
	// ensure that the correct window is showing
	m_MapValues.bMap_Visual = NO;
	MapActivatePane(IDC_MAPTOOL3);
	// get current channel
	m_MapValues.pMap = ResetMapChannel(m_MapValues.wChannel );
	if (::GetDlgItem(hDlg, IDC_PREVIEW))
		ControlEnable(hDlg, IDC_PREVIEW, !m_bAutoPreview&& GetViewHwnd());
	ControlEnable(hDlg, IDOK, m_MapValues.fModified);
}

//CLEAN
void CPPMapDlg::MapSetupPanes(BOOL fUseProbe)
{
	ITEMID id;

	// Initialise the dialog section.
	InitDlgSection(&m_MapValues.MapSec, ID_SECTION0, GetSafeHwnd(), IDC_PANE0, NULL);

	// Set threshold so that a category is always discarded when the user
	// selects another.
	SetDlgSectionThreshold(&m_MapValues.MapSec, 2, 2);

	AddDlgSectionCategory(	&m_MapValues.MapSec,
							PictPubApp.GetResourceHandle(),
							MAKEINTRESOURCE(IDD_MAPNUMERIC),
							IDD_MAPNUMERIC,
				    		DSCF_PRELOAD,
							NULL);
	if (fUseProbe)
		id = IDD_MAPVISUAL;
	else
		id = IDD_MAPVISUALNOPROBE;
	AddDlgSectionCategory(	&m_MapValues.MapSec,
							PictPubApp.GetResourceHandle(),
							MAKEINTRESOURCE(id),
							IDD_MAPVISUAL,
				    		DSCF_PRELOAD,
							NULL);
}


//CLEAN
void CPPMapDlg::HandleMapEnd()
{	
	UnAutoPreview();
	ResetAllMapsEx(NO);
}

//CPPMapDlg::AutoPreview_Button: Called when preview button hit in mapping dialog w/ auto preview.
VOID CPPMapDlg::AutoPreview_Button(LPMAP lpMap)
{
	if (lpMap)
		MakeMap(lpMap);
	SetMapGlobals();
	MapPreview();
}

//CPPMapDlg::HandleMapMouseMove: Handles WM_MOUSEMOVE
void CPPMapDlg::HandleMapMouseMove(int x, int y, UINT keyFlags)
{
	POINT pt;
	COLORINFO Color;
	HWND hMap;
	HWND hDlg = GetSafeHwnd();

	if (Probe_OnMouseMove(x, y, keyFlags, &Color))
	{
		hMap = ::GetDlgItem( hDlg, IDC_MAP);
		pt.x = Map_Get_Color(&Color);
		pt.y = Map_SetProbe(hMap, pt.x);
		DisplayInOutValues(pt.x, pt.y);
	}
	else if (m_bProbing) // probing but no valid value
	{
		hMap = ::GetDlgItem( hDlg, IDC_MAP);
	   	Map_ClearProbe(hMap);
	   	DisplayInOutValues(-1, -1);
	}
}

// 		Handles mouse move for probing.  
//		Fills lpProbeColor.
// RETURNS:
//		TRUE if valid probe took place

BOOL CPPMapDlg::Probe_OnMouseMove(int x, int y, UINT keyFlags, LPCOLORINFO lpProbeColor)
{
	if (m_bProbing)
		return(QuickProbeGet( GetSafeHwnd(), x, y, lpProbeColor ) );
	return(FALSE);
}


int CPPMapDlg::Map_Get_Color(LPCOLORINFO lpColor)
{
	switch (m_MapValues.wChannel)
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





//CPPMapDlg::HandleMapCurves: Handles WM_COMMAND from map curves button
BOOL CPPMapDlg::HandleMapCurves(ITEMID id)
{	
	HWND hDlg = GetSafeHwnd();
	HWND hMap = ::GetDlgItem(hDlg, IDC_MAP);
	
	m_bCurves = (m_MapValues.pMap->iCurves == 0);
	m_MapValues.pMap->iCurves = m_bCurves ? m_nCurveType : 0;
	SetWindowWord(::GetDlgItem(hDlg, id), GWW_ICONID, m_MapValues.pMap->iCurves ? IDC_MAP_BEZIER:IDC_MAP_LINES);
	Map_SetCurveType(hMap, m_nCurveType);
	Map_SetStyle(hMap, MS_CURVES, m_MapValues.pMap->iCurves);
	// get the current map set
	UpdateCurrentMap();
	SetMapGlobals();
	AutoPreview(m_MapValues.pMap, m_bAutoPreview, YES);
	return(TRUE);
}

//CPPMapDlg::HandleMapGamma: Handles IDC_MAP_GAMMA
BOOL CPPMapDlg::HandleMapGamma(ITEMID id, UINT codeNotify)
{	
	HWND hDlg = GetSafeHwnd();
	double gamma;
	int i;
	HWND hMap;
	POINT pt;
	char szFixed[32];

	if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
		return(FALSE);
	// get gamma value
	i = HandleSlide( hDlg, id, codeNotify, NULL );
	if (i > 100)
		gamma = (1.0 + (200.0-i)*8.0/100)/9.0;
	else
		gamma = (100.0 - i)/5+1.0;

	// set gamma value display
	FixedAscii( DBL2FIX((1.0/gamma)), szFixed, 2 );
	SetDlgItemText(IDC_MAP_GAMMA_TEXT, szFixed );
	// set gamma in map control
	hMap = ::GetDlgItem(hDlg, IDC_MAP);
	Map_SetGamma(hMap, gamma);
	m_MapValues.pMap->gamma = gamma;

	if (Map_GetActivePoint(hMap, &pt))
		DisplayInOutValues(pt.x, pt.y);

	if (codeNotify == SN_VALUESELECTED)
	{
		// get the current map set
		UpdateCurrentMap();
		SetMapGlobals();
		AutoPreview(m_MapValues.pMap, m_bAutoPreview, YES);
	}
	else
	{
			SetMapGlobals();
			AutoPreview(m_MapValues.pMap, m_bAutoPreview, NO);
	}
	return(TRUE);
}

// PARAMETERS:
//					LPMAP lpMap- The map to preiw (must not be NULL).
//					BOOL bAuto- Auto preiew on?
//					BOOL bDoNow- Preview now or set timer. 
//			Handles regular and autopreview.  
//			Call after any action which changes the map.

//CLEAN
VOID CPPMapDlg::AutoPreview(LPMAP lpMap, BOOL bAuto, BOOL bDoNow)
{
	if (bAuto & !bDoNow)
	{
	 	// setup timer
		m_bPreviewTimer = ::SetTimer(GetSafeHwnd(), PREVIEW_TIMER, PREVIEW_WAIT, NULL);
		return;
	}
	if (bAuto)	
	{
			// auto preview
			if (lpMap)
				MakeMap( lpMap );
			SetMapGlobals();
			MapPreview();
	}
	else
	{
		// Unpreview
		UndoMapPreview();
		SetMapGlobals();
		AnimateMap(lpMap, YES);
	}
	// kill any timer
	if (m_bPreviewTimer)
	{
		KillTimer(PREVIEW_TIMER);
		m_bPreviewTimer = FALSE;
	}
}

//				Call when destroying any dialog which uses autopreviw

VOID CPPMapDlg::AutoPreview_OnDestroy()
{
	if (m_bPreviewTimer)
	{
		KillTimer(PREVIEW_TIMER);
		m_bPreviewTimer = FALSE;
	}
}



//			Call in response to WM_TIMER message from any dialog which 
//			uses autopreview stuff.
// RETURNS:
//		TRUE if the caller should preview.

BOOL CPPMapDlg::AutoPreview_OnTimer(UINT id)
{
	
	if (id != PREVIEW_TIMER || !m_bPreviewTimer)
		return(FALSE);

	// kill the timer
	KillTimer(PREVIEW_TIMER);
	m_bPreviewTimer = FALSE;
	return(TRUE);
}


//					LPMAP lpMap- 	The map to base state on.
// DESCRIPTION:
//			Sets the gamma slider and the curves button based on lpMap.
//			Updates the map control as well.

 VOID CPPMapDlg::Map_DlgSetNewMap(LPMAP lpMap)
{
	HWND hControl;
	double gamma;
	int iGammaSlide;
	BOOL bCurves;
	HWND hDlg = GetSafeHwnd();
	
	// because this code gets called for other dialogs via ResetMapChannel()
	// make sure that ancillary controls exist
	
	gamma = lpMap->gamma;
	bCurves = (lpMap->iCurves != 0);

	// gamma
	if (::GetDlgItem(hDlg, IDC_MAP_GAMMA)) 
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
			SetDlgItemText(IDC_MAP_GAMMA_TEXT, szFixed );
		}
	}

	// curves
	if (hControl = ::GetDlgItem(hDlg, IDC_MAP_CURVES))
	{
		SetWindowWord(hControl, GWW_ICONID, m_bCurves ? IDC_MAP_BEZIER:IDC_MAP_LINES);
		AstralControlRepaint(hDlg, IDC_MAP_CURVES);
	}

	// map control
	if (hControl = ::GetDlgItem( hDlg, IDC_MAP ))
	{
		Map_SetGamma(hControl, gamma);
		Map_SetCurveType(hControl, m_bCurves ? lpMap->iCurves:m_nCurveType);
		Map_SetPointList(hControl, (LPPOINT)lpMap->Pnt, lpMap->Points);
		Map_SetStyle(hControl, MS_CURVES, bCurves);
	}
}

// DESCRIPTION:
//				Handles WM_DESTROY

void CPPMapDlg::HandleMapDestroy()
{	
	AutoPreview_OnDestroy();
	Probe_End();
}

// 		Cleans up probing dialog.  
//		Call when destroying dialog which can probe.  

void CPPMapDlg::Probe_End()
{
	if (!m_bProbing)
		return;

	QuickProbeEnd( GetSafeHwnd(), -1, -1 );
    HelpContext.BubbleHintsOn(m_iBubbles);
	m_bProbing = FALSE;
	CheckDlgButton(IDC_MAP_PROBE, m_bProbing);
}

//				Handles WM_LBUTTONUP

void CPPMapDlg::HandleMapLButtonUp(int x, int y, UINT keyFlags)
{
	COLORINFO Color;
	POINT pt;
	HWND hMap;
	BOOL bGotPoint;
	
	if (!m_bProbing)
		return;
	bGotPoint = Probe_OnLButtonUp(x,y,keyFlags,&Color);
	hMap = ::GetDlgItem( GetSafeHwnd(), IDC_MAP);
	if (bGotPoint)
	{
		// got a color - get x & y values
		pt.x = Map_Get_Color(&Color);
		pt.y = Map_SetProbe(hMap, pt.x);
		Map_ClearProbe(hMap);
		Map_AddPoint(hMap, &pt);
		m_MapValues.fModified = TRUE;
	}
	else
	{
		// unsucessful probe session
		Map_ClearProbe(hMap);
		if (!Map_GetActivePoint(hMap, &pt))
			pt.x = pt.y = -1;
	}
	// update readouts
	DisplayInOutValues(pt.x, pt.y);
}

// 		Handles mouse up for probing.  
//		Ends probing (regardless of return value).
//		Fills lpProbeColor.
// RETURNS:
//		TRUE if a color was probed. 

BOOL CPPMapDlg::Probe_OnLButtonUp(int x, int y, UINT keyFlags, LPCOLORINFO lpProbeColor)
{
	BOOL bGotPoint;
	
	if (!m_bProbing)
		return(FALSE);
	bGotPoint = QuickProbeGet( GetSafeHwnd(), x, y, lpProbeColor );
	Probe_End();
	return(bGotPoint);
}

MAP* CPPMapDlg::ResetCBChannel(int mul)
{

	int Contrast;
	HWND hDlg = GetSafeHwnd();

	MAP *lpMap; //since this is a static pointer to a static structure,
	// don't make it a far pointer because the data segment might move

	lpMap = ActivateChannel(m_MapValues.wChannel);

	if (m_wBand == BAND_ALL)
		Contrast = lpMap->Contrast;
	else
		Contrast = 0;
	Stick_SetValue(::GetDlgItem(hDlg, IDC_CONTBRIT), lpMap->Brightness[m_wBand] * mul, 0);

	InitDlgItemSpin( hDlg, IDC_CONTRAST, 0, YES, -100, 100 );
	InitDlgItemSpin( hDlg, IDC_BRIGHTNESS, lpMap->Brightness[m_wBand] * mul, YES, -100, 100 );

	AstralControlRepaint( hDlg, IDC_CONTBRIT );

	return( lpMap );
}

void CPPMapDlg::InitPreview()
   {
   int      x, y, dx, dy;
   HWND     hControl;
   LPOBJECT lpBase;
   LPFRAME  lpFrame;
   RECT     r;
   HWND hDlg = GetSafeHwnd();

   m_bIsInPreview = FALSE;
   hControl = ::GetDlgItem( hDlg, IDC_VIEWFULL );
   ::GetWindowRect(::GetDlgItem( hDlg, IDC_VISUAL_00), &r);
   ScreenToClient((LPPOINT)&r);
   x = r.left;
   y = r.top;

   ::GetWindowRect(::GetDlgItem( hDlg, IDC_VISUAL_22), &r);
   ScreenToClient((LPPOINT)&r.right);
   dx = r.right - x;
   dy = r.bottom - y;
   ::SetWindowPos(hControl, NULL, x, y, dx, dy, SWP_NOZORDER);

   ::GetWindowRect(hControl, &r);
   dx = r.right  - r.left;
   dy = r.bottom - r.top;
   ScreenToClient((LPPOINT)&r);
   ScreenToClient((LPPOINT)&r.right);
   lpBase = ImgGetBase(GetImage());
   lpFrame = ObjGetEditFrame(lpBase);
   // Link the frame to the image control
   SetWindowLong( hControl, GWL_IMAGE, (long)lpBase);
   }

void CPPMapDlg::DoPreview(BOOL f)
   {
   HWND  HWnd;
   HWND hDlg = GetSafeHwnd();
   if ((f && !m_bIsInPreview) || (!f && m_bIsInPreview))
      {
	   for (int i =0 ; i < 13; ++i)
         {
         HWnd = ::GetDlgItem(hDlg, i+IDC_VISUAL_00);
         if (HWnd)
            ::ShowWindow(HWnd, f? SW_HIDE : SW_SHOW);
         HWnd = ::GetDlgItem(hDlg, i+IDC_VISUAL_DUMMY1);
         if (HWnd)
            ::ShowWindow(HWnd, f? SW_HIDE : SW_SHOW);
         HWnd = ::GetDlgItem(hDlg, i+IDC_CONTICON);
         if (HWnd)
            ::ShowWindow(HWnd, f? SW_HIDE : SW_SHOW);
         }
      ::ShowWindow(::GetDlgItem(hDlg, IDC_VIEWFULL), f? SW_SHOW : SW_HIDE);
      m_bIsInPreview = f;
      }

	ControlEnable(hDlg, IDC_RESET, !m_bIsInPreview);
	ControlEnable(hDlg, IDC_RESETALL, !m_bIsInPreview);
   }

///////////////////////////////////////////CPPMapExDlg/////////////////////////////////////////////////////
CPPMapExDlg::CPPMapExDlg (CServerView *pView, UINT nIDTemplate, 
		CWnd* pParentWnd) : CPPMapDlg (pView, nIDTemplate, pParentWnd)
{

}

BOOL CPPMapExDlg::OnInitDialog()
{
	return CPPMapDlg::OnInitDialog();
}


void CPPMapExDlg::set_gradient(int id, WORD wChannel)
{
	HWND hGrad;
	RGBS rgb1;
	RGBS rgb2;
	HWND hDlg = GetSafeHwnd();

	hGrad = ::GetDlgItem(hDlg, id);
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
LPDWORD  CPPMapExDlg::get_histo(LPDWORD FAR *lpHistos, WORD wChannel, LPINT lpbReverse, LPINT lpNum)
//************************************************************************
{
	int i, num;
	LPDWORD lpHisto;
	LPIMAGE lpImage = NULL;
	BOOL bReverse;
	
	if (!lpHistos[0])
	{
		lpImage =  GetImage();
	
		if ( !(lpHisto = (LPDWORD)Alloc( 256 * 5 * sizeof(DWORD) )) )
			return(NULL);
		clr( (LPTR)lpHisto, 256 * 5 * sizeof(DWORD) );
		for (i=0;i<5;i++)
		{
			lpHistos[i] = lpHisto;
			lpHisto += 256;
		}
		m_lpGetHistos[0] = lpHistos[0];
		m_lpGetHistos[1] = lpHistos[1];
		m_lpGetHistos[2] = lpHistos[2];
		m_lpGetHistos[3] = lpHistos[3];
		m_lpGetHistos[4] = lpHistos[4];
		ReadEngineSelObj(NULL);
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

VOID CPPMapExDlg::free_histograms(LPDWORD FAR *lpHistos)
{
	if(lpHistos[0])
		FreeUp((LPTR)lpHistos[0]);
	lpHistos[0] = NULL;
}

//		compute_yscale
// RETURNS:
//			The 'largest' value for histogram
long CPPMapExDlg::compute_yscale(LPDWORD lpHisto)
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
//	Calls the ReadEngine on all selected objects.
//	The read engine allows the lpReadDataProc to look at all the data,
//	without modifiying it.
BOOL CPPMapExDlg::ReadEngineSelObj(LPRECT lpEditRect)
{
RECT rEdit, rClip;
LPOBJECT lpObject;
int nNumPasses = 0;
LPIMAGE lpImage = GetImage();

if ( !lpImage )
	return( FALSE );

ImgGetMaskRect( lpImage, &rClip );
if ( lpEditRect )
	{
	if ( !AstralIntersectRect( &rClip, &rClip, lpEditRect ) )
		return( FALSE );
	}

// Count the number of passes
lpObject = NULL;
while ( lpObject = ImgGetSelObject( lpImage, lpObject ) )
	++nNumPasses;

// Process each object
ProgressBegin( nNumPasses, 0 );
lpObject = NULL;
while ( lpObject = ImgGetSelObject( lpImage, lpObject ) )
	{
	if ( !AstralIntersectRect( &rEdit, &rClip, &lpObject->rObject ) )
		continue;
	ReadProcessObj(lpObject, FALSE, &rEdit);
	}
ProgressEnd();

return( TRUE );
}


//************************************************************************
//	The read engine allows the lpReadDataProc to look at all the data, 
//		without modifiying it.
BOOL CPPMapExDlg::ReadProcessObj(LPOBJECT lpObject, BOOL fReadAlpha, LPRECT lpRect)
{
RECT rObject;
int y, dx;
LPTR lpSrc;
LPPIXMAP lpPixmap;
FRMTYPEINFO TypeInfo;
LPIMAGE lpImage = GetImage();

if ( !lpRect )
	return( FALSE );

if (fReadAlpha)
		lpPixmap = &lpObject->lpAlpha->Pixmap;
else	lpPixmap = &lpObject->Pixmap;

ImgGetTypeInfo(lpImage, &TypeInfo);
rObject = lpObject->rObject;

dx = RectWidth(lpRect);
ProgressBegin(1,0);
for ( y = lpRect->top; y <= lpRect->bottom; ++y )
	{
	AstralClockCursor( y-lpRect->top, RectHeight(lpRect), NO );
	if ( lpSrc = PixmapPtr( lpPixmap, PMT_EDIT,
		lpRect->left - rObject.left, y - rObject.top, YES, dx ) )
			MapLineHisto( y, lpRect->left, lpRect->right, lpSrc, TypeInfo );
	}
ProgressEnd();

return( TRUE );
}

void CPPMapExDlg::MapLineHisto( int y, int x1, int x2, LPTR lpSrc, FRMTYPEINFO TypeInfo )
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
			lpHisto = m_lpGetHistos[0];
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
				INC_HISTO(m_lpGetHistos[1], rgb.red);
				// green
				INC_HISTO(m_lpGetHistos[2], rgb.green);
				// blue
				INC_HISTO(m_lpGetHistos[3], rgb.blue);
				// master
				i = TOLUM(rgb.red, rgb.green, rgb.blue);
				INC_HISTO(m_lpGetHistos[0], i);
			}
		break;

		case FDT_RGBCOLOR :
			while ( dx-- > 0 )
			{
				// red
				INC_HISTO(m_lpGetHistos[1], lpSrc[0]);
				// green
				INC_HISTO(m_lpGetHistos[2], lpSrc[1]);
				// blue
				INC_HISTO(m_lpGetHistos[3], lpSrc[2]);
				// master
				i = TOLUM(lpSrc[0], lpSrc[1], lpSrc[2]);
				INC_HISTO(m_lpGetHistos[0], i);

				lpSrc += 3;
			}
		break;

		case FDT_CMYKCOLOR :
			while ( dx-- > 0 )
			{
				// cyan
				INC_HISTO(m_lpGetHistos[1], lpSrc[0]);
				// magenta
				INC_HISTO(m_lpGetHistos[2], lpSrc[1]);
				// yellow
				INC_HISTO(m_lpGetHistos[3], lpSrc[2]);
				// black
				INC_HISTO(m_lpGetHistos[4], lpSrc[3]);
			
				// master
				i=CMYKtoL(LPCMYK(lpSrc));
				INC_HISTO(m_lpGetHistos[0], i);

				lpSrc += 4;
			}
		break;
	}
}

///////////////////////////////////////////CModifyColorMapsDlg/////////////////////////////////////////////
CModifyColorMapsDlg::CModifyColorMapsDlg(CServerView* pView, CWnd* pParent)
:CPPMapDlg(pView, CModifyColorMapsDlg::IDD, pParent)
{
	
}

BOOL CModifyColorMapsDlg::PreDoModal(LPVOID lpVoid)
{
	m_bAutoPreview = GetDefaultInt(MODIFYCOLORMAPS_AUTOPREVIEW, YES);
	m_nPoints = GetDefaultInt(MODIFYCOLORMAPS_POINTS, 3);
	m_bGrid = GetDefaultInt(MODIFYCOLORMAPS_GRID, YES);
	m_bPercentages = GetDefaultInt(MODIFYCOLORMAPS_PERCENTAGES, YES);
	m_bIntensity = GetDefaultInt(MODIFYCOLORMAPS_INTENSITY, NO);
	m_bCurves = GetDefaultInt(MODIFYCOLORMAPS_CURVES, YES);
	m_nCurveType = GetDefaultInt(MODIFYCOLORMAPS_CURVETYPE, SMOOTH_NORMAL);
	m_bNumericEdit = GetDefaultInt(MODIFYCOLORMAPS_NUMERICEDIT, NO);

	return CPPMapDlg::PreDoModal(lpVoid);

}

BOOL CModifyColorMapsDlg::OnInitDialog()
{
	FRMDATATYPE type;
	LPIMAGE lpImage = GetImage();
	HWND hDlg = GetSafeHwnd();

	PreDoModal();
	RightPopup(hDlg);
	

	// set maps to defaults
	ResetAllMapsEx(NO);

	if (lpImage)
		type = FrameType(ImgGetBaseEditFrame(lpImage));
	else
		type = FDT_RGBCOLOR;

	m_MapValues.wChannel = IDC_DO_ALL;
	HandleMapInit(type, TRUE);
	return CPPMapDlg::OnInitDialog();
}

void CModifyColorMapsDlg::OnOK()
{
	UpdateCurrentMap();
	map_to_mapinfo(&m_MasterMap, &m_Parms.MasterInfo);
	map_to_mapinfo(&m_RedMap, &m_Parms.RedInfo);
	map_to_mapinfo(&m_GreenMap, &m_Parms.GreenInfo);
	map_to_mapinfo(&m_BlueMap, &m_Parms.BlueInfo);
	map_to_mapinfo(&m_BlackMap, &m_Parms.BlackInfo);
			
	HandleMapEnd();
	PostDoModal();
	CPPMapDlg::OnOK();

}

void CModifyColorMapsDlg::OnCancel()
{
	HandleMapEnd();
	CPPMapDlg::OnCancel();
}

void CModifyColorMapsDlg::OnLoadMap()
{
	CMapLoadDlg dlg(this);
	dlg.DoModal();
	HandleMapLoad();
}

void CModifyColorMapsDlg::OnSaveMap()
{
	CMapSaveDlg dlg(this);
	dlg.DoModal();

}




LPVOID CModifyColorMapsDlg::PostDoModal()
{
	PutDefaultInt(MODIFYCOLORMAPS_AUTOPREVIEW, m_bAutoPreview);
	PutDefaultInt(MODIFYCOLORMAPS_POINTS, m_nPoints);
	PutDefaultInt(MODIFYCOLORMAPS_GRID, m_bGrid);
	PutDefaultInt(MODIFYCOLORMAPS_PERCENTAGES, m_bPercentages);
	PutDefaultInt(MODIFYCOLORMAPS_INTENSITY, m_bIntensity);
	PutDefaultInt(MODIFYCOLORMAPS_CURVES, m_bCurves);
//	PutDefaultInt(MODIFYCOLORMAPS_CURVETYPE, m_nCurveType);

	return CPPMapDlg::PostDoModal();
}



BOOL CModifyColorMapsDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{

	if (HandleMapCommand(LOWORD(wParam), HIWORD(wParam)))
		ControlEnable(GetSafeHwnd(), IDOK, m_MapValues.fModified);
	
 return CWnd::OnCommand(wParam, lParam);
}


BEGIN_MESSAGE_MAP(CModifyColorMapsDlg, CPPMapDlg)
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_MAPLOAD, OnLoadMap)
	ON_BN_CLICKED(IDC_MAPSAVE, OnSaveMap)
END_MESSAGE_MAP()


void CModifyColorMapsDlg::OnDestroy()
{
	HandleMapDestroy();

}

void CModifyColorMapsDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	HandleMapMouseMove(point.x, point.y, nFlags);

}

void CModifyColorMapsDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	HandleMapLButtonUp(point.x, point.y, nFlags);
}

BOOL CModifyColorMapsDlg::OnQueryNewPalette()
{
	return(Dialog_OnQueryNewPalette(GetSafeHwnd()));
}

void CModifyColorMapsDlg::OnTimer(UINT nIDEvent)
{
	if (!AutoPreview_OnTimer(nIDEvent))
		return;
		
	// got good timer
	UpdateCurrentMap();
	MakeMap( m_MapValues.pMap);
	SetMapGlobals();
	MapPreview();

}



//////////////////////////////////////CModifyColorMapsDlg Methods/////////////////////////////////////////


////////////////////////////////////////CModifyOptionsDlg///////////////////////////////////////

CModifyOptionsDlg::CModifyOptionsDlg(CWnd* pParent)
:CPPModalDlg(CModifyOptionsDlg::IDD, pParent)
{
    m_pParent = (CModifyColorMapsDlg*) pParent;
}

BOOL CModifyOptionsDlg::PreDoModal(LPVOID lpVoid)
{
	m_bAutoPreview = m_pParent->GetAutoPreviewPref();
	m_nPoints = m_pParent->GetPointsPref();
	m_bGrid  = m_pParent->GetGridPref();
	m_bPercentages = m_pParent->GetPercentagesPref();
	m_nCurveType = m_pParent->GetCurveTypePref();

	return CPPModalDlg::PreDoModal();
}

BOOL CModifyOptionsDlg::OnInitDialog()
{
	HWND hDlg = GetSafeHwnd();
	PreDoModal();
	int id;

	CenterWindow();
	CheckDlgButton(IDC_AUTO, m_bAutoPreview);
	CheckDlgButton(IDC_MAP_GRID, m_bGrid);
	CheckDlgButton(IDC_MAP_PERCENT, m_bPercentages);
	InitDlgItemSpin(hDlg, IDC_MAP_NUM, m_nPoints, NO, 0, 11);
	if (m_nCurveType <= SMOOTH_EXSOFT)
		id = IDC_SMOOTH_EXSOFT;
	else if (m_nCurveType <= SMOOTH_SOFT)
		id = IDC_SMOOTH_SOFT;
	else if (m_nCurveType <= SMOOTH_NORMAL)
		id = IDC_SMOOTH_NORMAL;
	else if (m_nCurveType <= SMOOTH_HARD)
		id = IDC_SMOOTH_HARD;	
	else // SMOOTH_EXHARD
		id = IDC_SMOOTH_EXHARD;
	CheckComboItem( hDlg, IDC_CURVE_SMOOTH, IDC_SMOOTH_FIRST, IDC_SMOOTH_LAST,id);
	
	return CPPModalDlg::OnInitDialog();
}

LPVOID CModifyOptionsDlg::PostDoModal()
{
	CModifyColorMapsDlg* pParent = (CModifyColorMapsDlg*)GetParent();
	m_pParent->SetAutoPreviewPref(m_bAutoPreview);
	m_pParent->SetPercentagesPref(m_bPercentages);
	m_pParent->SetGridPref(m_bGrid);
	m_pParent->SetCurveTypePref(m_nCurveType);
	m_pParent->SetPointsPref(m_nPoints);

	return CPPModalDlg::PostDoModal();
}

void CModifyOptionsDlg::OnOK()
{
    PostDoModal();
    CPPModalDlg::OnOK();
}

BOOL CModifyOptionsDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
 	int id = LOWORD(wParam);
	WORD codeNotify = HIWORD(wParam);
	HWND hDlg = GetSafeHwnd(); 
	int i;
//	BOOL bool;
	
	switch (id)
	{
		
//		case IDC_MAP_NUM:
//			i = GetDlgItemSpin(hDlg, id, &bool, NO);
//			if (bool)
//				m_nPoints = Bound(i,2,11);
		break; 
		
		case IDC_MAP_GRID:
			m_bGrid = !m_bGrid;
			CheckDlgButton(id, m_bGrid);
		break;
		
		case IDC_MAP_PERCENT:
			m_bPercentages = !m_bPercentages;
			CheckDlgButton(id, m_bPercentages);
		break;
	
		case IDC_AUTO:
			m_bAutoPreview = !m_bAutoPreview;
			CheckDlgButton(id, m_bAutoPreview);
		break;
		
		case IDC_CURVE_SMOOTH:
			if ( !(i = HandleCombo( hDlg, id, codeNotify )) )
				break;
			switch(i)
			{	
				case IDC_SMOOTH_EXSOFT:
					m_nCurveType = SMOOTH_EXSOFT;
				break;
				case IDC_SMOOTH_SOFT:
					m_nCurveType = SMOOTH_SOFT;
				break;
				case IDC_SMOOTH_NORMAL:
					m_nCurveType = SMOOTH_NORMAL;
				break;
				case IDC_SMOOTH_HARD:
					m_nCurveType = SMOOTH_HARD;
				break;
				case IDC_SMOOTH_EXHARD:
					m_nCurveType = SMOOTH_EXHARD;
				break;
			}
		break;
	   default:
		break;
	}
    return CWnd::OnCommand(wParam, lParam);
}

BEGIN_MESSAGE_MAP(CModifyOptionsDlg, CPPModalDlg)
	ON_CONTROL(1024, IDC_MAP_NUM, OnControlMapNum)
END_MESSAGE_MAP()

void CModifyOptionsDlg::OnControlMapNum()
{	
	int i;
	BOOL bool;
	i = GetDlgItemSpin(GetSafeHwnd(), IDC_MAP_NUM, &bool, NO);
	if (bool)
		m_nPoints = Bound(i,2,11);
}

//////////////////////////////////////////CMapLoadDlg////////////////////////////////////////
CMapLoadDlg::CMapLoadDlg(CWnd* pParent)
:CPPModalDlg(CMapLoadDlg::IDD, pParent)
{
	m_pParent = (CPPMapDlg*) pParent;
}


BOOL CMapLoadDlg::OnInitDialog()
{
CenterWindow();
InitExtName(GetSafeHwnd(), IDC_EXTNAMES, IDC_FILENAME, Names.ColorMap, IDN_MAP );
lstrcpy( Names.Saved, Names.ColorMap );
return CPPModalDlg::OnInitDialog();
}

void CMapLoadDlg::OnOK()
{
	if ( !LookupExtFile(Names.ColorMap, m_szFileName, IDN_MAP) )
		 CPPModalDlg::OnOK();
	if ( !LoadMap( &m_pParent->m_MasterMap,
				   &m_pParent->m_RedMap,
				   &m_pParent->m_GreenMap,
				   &m_pParent->m_BlueMap,
				   &m_pParent->m_BlackMap,
				   m_szFileName ) )
		 CPPModalDlg::OnOK();
	m_pParent->SetMapGlobals();
	PostDoModal();
	CPPModalDlg::OnOK();

}

void CMapLoadDlg::OnCancel()
{
	lstrcpy( Names.ColorMap, Names.Saved );
	CPPModalDlg::OnCancel();

}

LPVOID CMapLoadDlg::PostDoModal()
{

return CPPModalDlg::PostDoModal();

}
BOOL CMapLoadDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{	HWND hDlg = GetSafeHwnd();
	int id = LOWORD(wParam);
	WORD codeNotify = HIWORD(wParam);
	HWND hControl = (HWND) lParam;
	
					

switch (id)
	{
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

return CWnd::OnCommand(wParam, lParam);}


/////////////////////////////////////////CMapSaveDlg/////////////////////////////////////////
CMapSaveDlg::CMapSaveDlg(CWnd* pParent)
:CPPModalDlg(CMapSaveDlg::IDD, pParent)
{
	m_pParent = (CPPMapDlg*) pParent;
}



BOOL CMapSaveDlg::OnInitDialog()
{

	CenterWindow();
	InitExtName( GetSafeHwnd(), IDC_EXTNAMES, IDC_FILENAME, Names.ColorMap, IDN_MAP );
	lstrcpy( Names.Saved, Names.ColorMap );
	return CPPModalDlg::OnInitDialog();
}

void CMapSaveDlg::OnOK()
{
	if ( LookupExtFile(Names.ColorMap, m_szFileName, IDN_MAP) )
		{
		if (AstralOKCancel(IDS_OVERWRITEEXTFILE,
			(LPTR)Names.ColorMap) == IDCANCEL)
			CPPModalDlg::OnCancel();
		}
	else
		{
		if ( !CreateFileName( Names.ColorMap, IDN_MAP, m_szFileName ) )
			CPPModalDlg::OnOK();
		}
	if ( !SaveMap( &m_pParent->m_MasterMap,
				   &m_pParent->m_RedMap,
				   &m_pParent->m_GreenMap,
				   &m_pParent->m_BlueMap,
				   &m_pParent->m_BlackMap,
				   m_szFileName ) )
		CPPModalDlg::OnOK();
	if ( !AddExtFile( Names.ColorMap, m_szFileName, IDN_MAP ) )
		{
		DWORD dwError = GetLastError();
		FileDelete( m_szFileName );
		CPPModalDlg::OnOK();
		}
	PostDoModal();
	CPPModalDlg::OnOK();
}

void CMapSaveDlg::OnCancel()
{
	lstrcpy( Names.ColorMap, Names.Saved );
	CPPModalDlg::OnCancel();
}

LPVOID CMapSaveDlg::PostDoModal()
{

return CPPModalDlg::PostDoModal();

}

BOOL CMapSaveDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	HWND hDlg = GetSafeHwnd();
	int id = LOWORD(wParam);
	WORD codeNotify = HIWORD(wParam);
	HWND hControl = (HWND) lParam;



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

   default:
	break;
   }
return CWnd::OnCommand(wParam, lParam);}

/////////////////////////////////////CContrastBrightnessJoystickDlg//////////////////////////////////////
CContrastBrightnessJoystickDlg::CContrastBrightnessJoystickDlg(CServerView* pView, CWnd* pParent)
:CPPMapDlg(pView, CContrastBrightnessJoystickDlg::IDD, pParent)
{

}

BOOL CContrastBrightnessJoystickDlg::PreDoModal(LPVOID lpVoid)
{
	m_bAutoPreview = GetDefaultInt(CONTRASTBRIGHTNESSJOYSTICK_AUTOPREVIEW, YES);
	return CPPMapDlg::PreDoModal(lpVoid);

}

BOOL CContrastBrightnessJoystickDlg::OnInitDialog()
{
PreDoModal();
HWND hDlg = GetSafeHwnd();

RightPopup( hDlg );
ResetAllMaps();
m_MapValues.wChannel = IDC_DO_ALL;
m_wBand = BAND_ALL;
CheckComboItem( hDlg, IDC_BAND, IDC_BAND_ALL, IDC_BAND_LO,
	m_wBand + IDC_BAND_ALL );

CheckDlgButton(IDC_AUTO, m_bAutoPreview);
ControlEnable(hDlg, IDC_PREVIEW, !m_bAutoPreview);

ControlEnable( hDlg, IDC_CONTRAST, m_wBand == BAND_ALL);
//SetWordBit(::GetDlgItem( hDlg, IDC_CONTBRIT ), GWL_STYLE, (WORD)ST_HONLY, m_wBand != BAND_ALL);

SetWindowLong(::GetDlgItem(hDlg, IDC_CONTBRIT), GWL_USERDATA,
				 m_wBand==BAND_ALL ? 0L : (LONG)ST_HONLY);


m_MapValues.pMap = ResetCBChannel();
return CPPMapDlg::OnInitDialog();


}

void CContrastBrightnessJoystickDlg::OnOK()
{
	if (m_MapValues.pMap->Reverse)
		InvertMap(m_MapValues.pMap);
	m_Parms.Contrast = m_MapValues.pMap->Contrast;
	for (int i = 0; i < NUM_BANDS; ++i)
		m_Parms.Brightness[i] = m_MapValues.pMap->Brightness[i];
	UnAutoPreview();
	PostDoModal();
	CPPMapDlg::OnOK();
}

void CContrastBrightnessJoystickDlg::OnCancel()
{
	ResetAllMaps();
	UnAutoPreview();
	CPPMapDlg::OnCancel();
}

LPVOID CContrastBrightnessJoystickDlg::PostDoModal()
{
	 PutDefaultInt(CONTRASTBRIGHTNESSJOYSTICK_AUTOPREVIEW, m_bAutoPreview);
	 return CPPMapDlg::PostDoModal();
}

BOOL CContrastBrightnessJoystickDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	BOOL Bool;
	int x, y;
	LPIMAGE lpImage = GetImage();
	int id = LOWORD(wParam);
	WORD codeNotify = HIWORD(wParam);
	HWND hControl = (HWND) lParam;
	HWND hDlg = GetSafeHwnd();


switch (id)
	{
	case IDC_BAND:
		if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
			break;
		// setup new map
		m_wBand = id - IDC_BAND_ALL;
		ControlEnable( hDlg, IDC_CONTRAST, m_wBand == BAND_ALL);
//		SetWordBit(::GetDlgItem( hDlg, IDC_CONTBRIT ), GWL_STYLE, (WORD)ST_HONLY, m_wBand != BAND_ALL);
		SetWindowLong(::GetDlgItem(hDlg, IDC_CONTBRIT), GWL_USERDATA,
				 m_wBand==BAND_ALL ? 0L : (LONG)ST_HONLY);
		m_MapValues.pMap = ResetCBChannel();
	break;

	case IDC_AUTO:
		m_bAutoPreview = !m_bAutoPreview;
		CheckDlgButton(id, m_bAutoPreview);
		ControlEnable(hDlg, IDC_PREVIEW, !m_bAutoPreview);
		SetMapGlobals();
		AutoPreview(m_MapValues.pMap, m_bAutoPreview, YES);
	break; 
	
	case IDC_PREVIEW:
	  	AutoPreview_Button(m_MapValues.pMap);
	break;

	case IDC_RESETALL:
	case IDC_RESET:
		// reset all bands
		ResetMapEx( &m_MasterMap, MAPPOINTS, -1, YES );
		m_MapValues.pMap = ResetCBChannel();
		SetMapGlobals();
		AutoPreview(m_MapValues.pMap, m_bAutoPreview, YES);
	break;


	case IDC_CONTBRIT:
		if ( ::GetFocus() != ::GetDlgItem( hDlg, IDC_CONTBRIT ) )
			break;
		Stick_GetValue(::GetDlgItem(hDlg, IDC_CONTBRIT), &x, &y);
		m_MapValues.pMap->Brightness[m_wBand] = x;
		m_MapValues.pMap->Contrast = y;
		SetDlgItemSpin( hDlg, IDC_CONTRAST,
			m_MapValues.pMap->Contrast, YES );
		SetDlgItemSpin( hDlg, IDC_BRIGHTNESS,
			m_MapValues.pMap->Brightness[m_wBand], YES );
		SetMapGlobals();
		AutoPreview(m_MapValues.pMap, m_bAutoPreview, (codeNotify==SCN_STICK_CHANGED));
	break;

	case IDC_CONTRAST:
		if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		m_MapValues.pMap->Contrast = (int)GetDlgItemSpin( hDlg, IDC_CONTRAST,
			&Bool, YES );
		Stick_SetValue(::GetDlgItem(hDlg, IDC_CONTBRIT),
					m_MapValues.pMap->Brightness[m_wBand],
					m_MapValues.pMap->Contrast);
		AstralControlRepaint( hDlg, IDC_CONTBRIT );
		SetMapGlobals();	
		AutoPreview(m_MapValues.pMap, m_bAutoPreview, NO);
	break;

	case IDC_BRIGHTNESS:
		if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		m_MapValues.pMap->Brightness[m_wBand] = (int)GetDlgItemSpin( hDlg,
			IDC_BRIGHTNESS,	&Bool, YES );
		Stick_SetValue(::GetDlgItem(hDlg, IDC_CONTBRIT),
					m_MapValues.pMap->Brightness[m_wBand],
					m_MapValues.pMap->Contrast);
		AstralControlRepaint( hDlg, IDC_CONTBRIT );
		SetMapGlobals();
		AutoPreview(m_MapValues.pMap, m_bAutoPreview, NO);
	break;

   default:
	break;
   }
return CWnd::OnCommand(wParam, lParam);}

BEGIN_MESSAGE_MAP(CContrastBrightnessJoystickDlg, CPPMapDlg)
	ON_WM_DESTROY()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CContrastBrightnessJoystickDlg::OnDestroy()
{
	AutoPreview_OnDestroy();
	Dialog_OnDestroy(GetSafeHwnd());

}

BOOL CContrastBrightnessJoystickDlg::OnQueryNewPalette()
{
	return Dialog_OnQueryNewPalette(GetSafeHwnd());
}

void CContrastBrightnessJoystickDlg::OnTimer(UINT nIDEvent)
{
	if (!AutoPreview_OnTimer(nIDEvent))
		return;
		
	// got good timer		   
	MakeMap(m_MapValues.pMap);
	SetMapGlobals();
	MapPreview();

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  CContrastBrightnessVisualDlg  /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CContrastBrightnessVisualDlg::CContrastBrightnessVisualDlg(CServerView* pView, UINT nIDTemplate, CWnd* pParent)
:CPPVisualMapDlg(pView, nIDTemplate, pParent) 
{
	m_Visual.SetDlgPtr(this);
}

BOOL CContrastBrightnessVisualDlg::PreDoModal(LPVOID lpVoid)
{
return CPPMapDlg::PreDoModal(lpVoid);
}

BOOL CContrastBrightnessVisualDlg::OnInitDialog()
{
PreDoModal();
HWND hDlg = GetSafeHwnd();

LPIMAGE lpImage;
FRMDATATYPE type;

InitPreview();
lpImage = GetImage();
CenterWindow();


ResetAllMaps();

type = FrameType(ImgGetBaseEditFrame(lpImage));
m_Data.wChannel = IDC_DO_ALL;
ChannelsCombo_Setup(IDC_CHANNELS, type);

m_Data.wBand = BAND_ALL;
EnableContrastVisual(m_Data.wBand == BAND_ALL);
CheckComboItem( hDlg, IDC_BAND, IDC_BAND_ALL, IDC_BAND_LO,
	m_Data.wBand+IDC_BAND_ALL );

if ( FrameDepth(ImgGetBaseEditFrame(lpImage)) == 1 )
	{
	ControlEnable( hDlg, IDC_DO_RED, NO );
	ControlEnable( hDlg, IDC_DO_GREEN, NO );
	ControlEnable( hDlg, IDC_DO_BLUE, NO );
	ControlEnable( hDlg, IDC_DO_CYAN, NO );
	ControlEnable( hDlg, IDC_DO_MAGENTA, NO );
	ControlEnable( hDlg, IDC_DO_YELLOW, NO );
	}
m_Data.hDlg = hDlg;
m_Data.lpMap = ActivateChannel(m_Data.wChannel);
m_Data.lpBuffer = NULL;
m_Data.lpLuts = Alloc(PACKEDLUTSIZE*9);
if (!m_Data.lpLuts)
	{
	Message(IDS_EMEMALLOC);
	AstralDlgEnd( hDlg, FALSE);
	return TRUE;
	}

m_Visual.m_lpImage = lpImage;
m_Visual.m_lpOrgFrame = NULL;
m_Visual.m_lpBasisFrame = NULL;
m_Visual.m_lpUser = (LPTR)&m_Data;
m_Visual.m_nStep = 5;
m_Visual.m_wMoveType = 1;	// 2-d directional

return CPPVisualMapDlg::OnInitDialog();

}

void CContrastBrightnessVisualDlg::OnOK()
{
	// clear old map to the center
	MakeMap(m_Data.lpMap );
	if (m_Data.lpMap->Reverse)
		InvertMap(m_Data.lpMap);
	m_Parms.Contrast = m_Data.lpMap->Contrast;
	for (int i = 0; i < NUM_BANDS; ++i)
		m_Parms.Brightness[i] = m_Data.lpMap->Brightness[i];
	ResetAllMaps();
	UndoMapPreview();
	SetMapGlobals();
	AnimateMap(NULL, YES);
	PostDoModal();
	CPPVisualMapDlg::OnOK();

}


LPVOID CContrastBrightnessVisualDlg::PostDoModal()
{
	return CPPVisualMapDlg::PostDoModal();
}

BOOL CContrastBrightnessVisualDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int i;
	LPIMAGE lpImage = GetImage();
	HWND hDlg = GetSafeHwnd();
	int id = LOWORD(wParam);
	WORD codeNotify = HIWORD(wParam);
	HWND hControl = (HWND) lParam;

switch (id)
	{
	case IDC_CHANNELS:
      DoPreview(FALSE);
		if (!(i=ChannelsCombo_Handle(id, codeNotify)))
			break;
			
		// clear old map to the center
		MakeMap(m_Data.lpMap );

		// setup new map
		m_Data.wChannel = i;
		m_Data.lpMap = ActivateChannel(m_Data.wChannel );
		SetVisual(0,0);
	break;
	
	case IDC_BAND:
      DoPreview(FALSE);
		if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
			break;
		// setup new map
		if (m_Data.wBand != id - IDC_BAND_ALL)
		{
			m_Data.wBand = id - IDC_BAND_ALL;
			EnableContrastVisual(m_Data.wBand == BAND_ALL);
			SetVisual(0,0);										  
		}
	break;
	
	case IDC_PREVIEW:
		// clear map to the center
        DoPreview(!m_bIsInPreview);
		SetMapGlobals();
		MakeMap(m_Data.lpMap );
		SetMapGlobals();
		MapPreview();
	   break;



   default:
      DoPreview(FALSE);
		CPPVisualMapDlg::OnCommandEx(wParam, lParam);
	break;
   }

return CWnd::OnCommand(wParam, lParam);}

BEGIN_MESSAGE_MAP(CContrastBrightnessVisualDlg, CPPVisualMapDlg)
	ON_WM_DESTROY()
	ON_WM_QUERYNEWPALETTE()
//	ON_CONTROL(3, IDC_BAND, OnControlBand)
END_MESSAGE_MAP()

//void CContrastBrightnessVisualDlg::OnControlBand()
//{
//int id;
//      DoPreview(FALSE);
//		if ( !(id = HandleCombo( GetSafeHwnd(), IDC_BAND, 3)) )
//			return;
//		// setup new map
//		if (m_Data.wBand != id - IDC_BAND_ALL)
//		{
//			m_Data.wBand = id - IDC_BAND_ALL;
//			EnableContrastVisual(m_Data.wBand == BAND_ALL);
//			SetVisual(0,0);										  
//		}
//} 

void CContrastBrightnessVisualDlg::OnDestroy()
{
if (m_Data.lpLuts)
	FreeUp(m_Data.lpLuts);
if (m_Data.lpBuffer)
	FreeUp(m_Data.lpBuffer); 
CPPVisualMapDlg::OnDestroy();

}

BOOL CContrastBrightnessVisualDlg::OnQueryNewPalette()
{
return Dialog_OnQueryNewPalette(GetSafeHwnd());
}
///////////////////////////////////CContrastBrightnessVisualDlg Methods///////////////////////////////////
void CContrastBrightnessVisual::DrawVisual(HDC hDC,LPRECT lpRect, int x,int y)
{
	LPCBSTRUCT lpCB;
	int w, h, id;
	RECT SourceRect, Rect;
	POINT DestPoint;
	LPFRAME lpFrame;
	int depth;
	HBRUSH hBrush;
	LPVOID lpCmsXform = NULL;
	
	lpCB = (LPCBSTRUCT)m_lpUser;
	if (!lpCB)
		return;

	lpFrame = m_lpBasisFrame;

	// Check if frames setup
	if (!m_lpOrgFrame || !lpFrame)
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

	SourceRect.left   =	0;
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
				return;
			}
		}
		if (WITHIN(id,0,8))
		{
			m_pDlg->MapSetPackedLut(lpCB->lpLuts+PACKEDLUTSIZE*id);
		}
		else
		{
			m_pDlg->MapSetPackedLut(NULL);
		}
		if (m_lpImage)
			lpCmsXform = m_lpImage->m_cmsXform;
		m_pDlg->SetMapGlobals();
		m_pDlg->DisplayMappedImage(hDC,lpFrame,&SourceRect,
			DestPoint, lpCB->lpBuffer,0,0, lpCmsXform);
	}
}

BOOL CContrastBrightnessVisual::MoveVisual(int x ,int y)
{
	LPCBSTRUCT lpCB;
	int newCont, newBrit;
	int cont, brit;
	int i,j, id;

	lpCB = (LPCBSTRUCT)m_lpUser;
	if (!lpCB)
		return(FALSE);

	if (x == VISUAL_BEFORE)
	{
		m_pDlg->ResetAllMaps();
	}

	if (!m_pDlg->ContBritVisual_GetNew(x,y,&newCont,&newBrit))
		return(FALSE);

	m_pDlg->UndoMapPreview();
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
					m_pDlg->ContBritVisual_GetNew(i,j,&cont,&brit))
				{
					lpCB->lpMap->Contrast = cont;
					lpCB->lpMap->Brightness[lpCB->wBand] = brit;
			  		MakeMap(lpCB->lpMap );

					m_pDlg->MapGetPackedLut(lpCB->lpLuts+PACKEDLUTSIZE*id,
						m_pDlg->m_HueMap.Lut,
						m_pDlg->m_SatMap.Lut,
						m_pDlg->m_MasterMap.Lut,
						m_pDlg->m_RedMap.Lut,
						m_pDlg->m_GreenMap.Lut,
						m_pDlg->m_BlueMap.Lut,
						m_pDlg->m_BlackMap.Lut);

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


BOOL CContrastBrightnessVisualDlg::ContBritVisual_GetNew(int x ,int y, LPINT cont, LPINT brit)
{
	LPCBSTRUCT  lpCB;

	lpCB = (LPCBSTRUCT)m_Visual.m_lpUser;

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
			*cont -= m_Visual.m_nStep*y;
		*brit += m_Visual.m_nStep*x;
	}
	*cont = Bound(*cont, -100, 100);
	*brit = Bound(*brit, -100, 100);

	return(TRUE);
}


//	 Enables or diables the top and bottom rows of controls
void CContrastBrightnessVisualDlg::EnableContrastVisual(BOOL bEnable)
{
	HWND hDlg = GetSafeHwnd();
	ControlEnable(hDlg, IDC_VISUAL_00, bEnable);
	ControlEnable(hDlg, IDC_VISUAL_01, bEnable);
	ControlEnable(hDlg, IDC_VISUAL_02, bEnable);
	ControlEnable(hDlg, IDC_VISUAL_20, bEnable);
	ControlEnable(hDlg, IDC_VISUAL_21, bEnable);
	ControlEnable(hDlg, IDC_VISUAL_22, bEnable);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////  CColorBalanceJoystickDlg  //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CColorBalanceJoystickDlg::CColorBalanceJoystickDlg(CServerView* pView, CWnd* pParent)
:CPPMapDlg(pView, CColorBalanceJoystickDlg::IDD, pParent)
{

}

BOOL CColorBalanceJoystickDlg::PreDoModal(LPVOID lpVoid)
{
m_bAutoPreview = GetDefaultInt(COLORBALANCEJOYSTICK_AUTOPREVIEW, YES);
return CPPMapDlg::PreDoModal(lpVoid);

}

BOOL CColorBalanceJoystickDlg::OnInitDialog()
{
FRMDATATYPE type;
int			mul;
HWND hDlg = GetSafeHwnd();
LPIMAGE lpImage = GetImage();

RightPopup( hDlg );

ResetAllMaps();

type = FrameType(ImgGetBaseEditFrame(lpImage));
m_MapValues.wChannel = IDC_DO_ALL;
ChannelsCombo_Setup(IDC_CHANNELS, type);

m_wBand = BAND_ALL;
CheckComboItem( hDlg, IDC_BAND, IDC_BAND_ALL, IDC_BAND_LO,
	m_wBand + IDC_BAND_ALL );

CheckDlgButton(IDC_AUTO, m_bAutoPreview);
ControlEnable(hDlg, IDC_PREVIEW, !m_bAutoPreview);

ControlEnable( hDlg, IDC_CONTRAST, m_wBand == BAND_ALL);
//SetWordBit(::GetDlgItem( hDlg, IDC_CONTBRIT ), GWL_STYLE, (WORD)ST_HONLY, m_wBand != BAND_ALL);
SetWindowLong(::GetDlgItem(hDlg, IDC_CONTBRIT), GWL_USERDATA,
				 m_wBand==BAND_ALL ? 0L : (LONG)ST_HONLY);

if ( FrameDepth(ImgGetBaseEditFrame(lpImage)) == 1 )
	{
	ControlEnable( hDlg, IDC_DO_RED, NO );
	ControlEnable( hDlg, IDC_DO_GREEN, NO );
	ControlEnable( hDlg, IDC_DO_BLUE, NO );
	ControlEnable( hDlg, IDC_DO_CYAN, NO );
	ControlEnable( hDlg, IDC_DO_MAGENTA, NO );
	ControlEnable( hDlg, IDC_DO_YELLOW, NO );
	}
Balance_Set_Gradient();
mul = (m_MapValues.wChannel == IDC_DO_ALL) ? 1:-1;
m_MapValues.pMap = ResetCBChannel(mul);
return CPPMapDlg::OnInitDialog();

}

void CColorBalanceJoystickDlg::OnOK()
{
	int i;
	UndoMapPreview();

	m_Parms.MasterContrast = m_MasterMap.Contrast;
	for (i = 0; i < 4; ++i)
		m_Parms.MasterBrightness[i] = m_MasterMap.Brightness[i];
	m_Parms.fMasterReverse = m_MasterMap.Reverse;
	m_Parms.RedContrast = m_RedMap.Contrast;
	for (i = 0; i < 4; ++i)
		m_Parms.RedBrightness[i] = m_RedMap.Brightness[i];
	m_Parms.fRedReverse = m_RedMap.Reverse;
	m_Parms.GreenContrast = m_GreenMap.Contrast;
	for (i = 0; i < 4; ++i)
		m_Parms.GreenBrightness[i] = m_GreenMap.Brightness[i];
	m_Parms.fGreenReverse = m_GreenMap.Reverse;
	m_Parms.BlueContrast = m_BlueMap.Contrast;
	for (i = 0; i < 4; ++i)
		m_Parms.BlueBrightness[i] = m_BlueMap.Brightness[i];
	m_Parms.fBlueReverse = m_BlueMap.Reverse;
	m_Parms.BlackContrast = m_BlackMap.Contrast;
	for (i = 0; i < 4; ++i)
		m_Parms.BlackBrightness[i] = m_BlackMap.Brightness[i];
	m_Parms.fBlackReverse = m_BlackMap.Reverse;
	ResetAllMaps();
	UnAutoPreview();
	PostDoModal();
	CPPMapDlg::OnOK();
	
}

void CColorBalanceJoystickDlg::OnCancel()
{
	ResetAllMaps();
	UnAutoPreview();
	CPPMapDlg::OnCancel();
}

LPVOID CColorBalanceJoystickDlg::PostDoModal()
{
	PutDefaultInt(COLORBALANCEJOYSTICK_AUTOPREVIEW, m_bAutoPreview);
	return CPPMapDlg::PostDoModal();
}

BOOL CColorBalanceJoystickDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	BOOL Bool;
	int Contrast;
	int mul;
	int i, x, y;

	HWND hDlg = GetSafeHwnd();
	LPIMAGE lpImage = GetImage();
	int id = LOWORD(wParam);
	WORD codeNotify = HIWORD(wParam);
	HWND hControl = (HWND) lParam;




switch (id)
	{
	case IDC_BAND:
		if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
			break;
		// setup new map
		m_wBand = id- IDC_BAND_ALL;
		ControlEnable( hDlg, IDC_CONTRAST, m_wBand == BAND_ALL);
//		SetWordBit(::GetDlgItem( hDlg, IDC_CONTBRIT ), GWL_STYLE, (WORD)ST_HONLY, m_wBand != BAND_ALL);
		SetWindowLong(::GetDlgItem(hDlg, IDC_CONTBRIT), GWL_USERDATA,
				 m_wBand==BAND_ALL ? 0L : (LONG)ST_HONLY);

		mul = (m_MapValues.wChannel == IDC_DO_ALL) ? 1:-1;
		m_MapValues.pMap = ResetCBChannel(mul);
	break;
	
	case IDC_CHANNELS:
		if (!(i=ChannelsCombo_Handle(id, codeNotify)))
			break;
		m_MapValues.wChannel = i;
		Balance_Set_Gradient();	 
		mul = (m_MapValues.wChannel == IDC_DO_ALL) ? 1:-1;
		m_MapValues.pMap = ResetCBChannel(mul);
	break;

	case IDC_AUTO:
		m_bAutoPreview = !m_bAutoPreview;
		CheckDlgButton(id, m_bAutoPreview);
		ControlEnable(hDlg, IDC_PREVIEW, !m_bAutoPreview);
		SetMapGlobals();
		AutoPreview(m_MapValues.pMap, m_bAutoPreview, YES);
	break; 
	
	case IDC_PREVIEW:
	  	AutoPreview_Button(m_MapValues.pMap);
	break;

	case IDC_RESETALL:
		ResetAllMaps();
		mul = (m_MapValues.wChannel == IDC_DO_ALL) ? 1:-1;
		m_MapValues.pMap = ResetCBChannel(mul);
		SetMapGlobals();
		AutoPreview(m_MapValues.pMap, m_bAutoPreview, YES);
	break;

	case IDC_RESET:
		ResetMapEx( m_MapValues.pMap, MAPPOINTS, -1, m_MapValues.wChannel == IDC_DO_ALL );
		mul = (m_MapValues.wChannel == IDC_DO_ALL) ? 1:-1;
		m_MapValues.pMap = ResetCBChannel(mul);
		SetMapGlobals();
		AutoPreview(m_MapValues.pMap, Mapping.Balance_bAuto, YES);
	break;

	case IDC_CONTBRIT:
		if ( ::GetFocus() != ::GetDlgItem( hDlg, IDC_CONTBRIT ) )
			break;
		Stick_GetValue(::GetDlgItem(hDlg, IDC_CONTBRIT), &x, &y);
		mul = (m_MapValues.wChannel == IDC_DO_ALL) ? 1:-1;
	   	m_MapValues.pMap->Brightness[m_wBand] = mul*x;
		if (m_wBand == BAND_ALL)
		{
			Contrast = m_MapValues.pMap->Contrast;
			m_MapValues.pMap->Contrast = y;
			SetDlgItemSpin( hDlg, IDC_CONTRAST,
				m_MapValues.pMap->Contrast, YES );
		}
		else
      {
			Contrast = 0;
		   Stick_SetValue(::GetDlgItem(hDlg, IDC_CONTBRIT),
					m_MapValues.pMap->Brightness[m_wBand]*mul,
					Contrast);
      }
		SetDlgItemSpin( hDlg, IDC_BRIGHTNESS,
			mul*m_MapValues.pMap->Brightness[m_wBand], YES );
		SetMapGlobals();
		AutoPreview(m_MapValues.pMap, m_bAutoPreview, (codeNotify==SCN_STICK_CHANGED));
	break;

	case IDC_CONTRAST:
		if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		m_MapValues.pMap->Contrast = (int)GetDlgItemSpin( hDlg, IDC_CONTRAST,
			&Bool, YES );
		mul = (m_MapValues.wChannel == IDC_DO_ALL) ? 1:-1;
		Stick_SetValue(::GetDlgItem(hDlg, IDC_CONTBRIT),
					mul*m_MapValues.pMap->Brightness[m_wBand],
					m_MapValues.pMap->Contrast);
		AstralControlRepaint( hDlg, IDC_CONTBRIT );
		SetMapGlobals();
		AutoPreview(m_MapValues.pMap, m_bAutoPreview, NO);
	break;

	case IDC_BRIGHTNESS:
		if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		mul = (m_MapValues.wChannel == IDC_DO_ALL) ? 1:-1;
		m_MapValues.pMap->Brightness[m_wBand] = mul*(int)GetDlgItemSpin( hDlg,
			IDC_BRIGHTNESS,	&Bool, YES );
		if (m_wBand == BAND_ALL)
			Contrast = m_MapValues.pMap->Contrast;
		else
			Contrast = 0;
		Stick_SetValue(::GetDlgItem(hDlg, IDC_CONTBRIT),
					m_MapValues.pMap->Brightness[m_wBand]*mul,
					Contrast);
		AstralControlRepaint( hDlg, IDC_CONTBRIT );
		SetMapGlobals();
		AutoPreview(m_MapValues.pMap, m_bAutoPreview, NO);
	break;

   default:
	break;
   }
return CWnd::OnCommand(wParam, lParam);;}


BEGIN_MESSAGE_MAP(CColorBalanceJoystickDlg, CPPMapDlg)
	ON_WM_DESTROY()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CColorBalanceJoystickDlg::OnDestroy()
{
	AutoPreview_OnDestroy();
	Dialog_OnDestroy(GetSafeHwnd());

}

BOOL CColorBalanceJoystickDlg::OnQueryNewPalette()
{
return Dialog_OnQueryNewPalette(GetSafeHwnd());
}

void CColorBalanceJoystickDlg::OnTimer(UINT nIDEvent)
{
	if (!AutoPreview_OnTimer(nIDEvent))
		return;
		
	// got good timer		   
	MakeMap(m_MapValues.pMap);
	SetMapGlobals();
	MapPreview();

}

///////////////////////////////////CColorBalanceJoystickDlg Methods///////////////////////////////////////
//		sets the given gradient control based on the channel.
void CColorBalanceJoystickDlg::Balance_Set_Gradient()
{
	HWND hBar;
	RGBS rgb1;
	RGBS rgb2;

	switch (m_MapValues.wChannel)
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
	
	if (hBar = ::GetDlgItem(GetSafeHwnd(), IDC_HISTO_GRAD))
	{
		Gradient_SetRGB(hBar, &rgb2, &rgb1);
		Gradient_SetType(hBar, GT_RGB);																	  
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////  CColorBalanceVisualDlg  ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CColorBalanceVisualDlg::CColorBalanceVisualDlg(CServerView* pView, UINT nIDTemplate, CWnd* pParent)
:CPPVisualMapDlg(pView, nIDTemplate, pParent) 
{
	m_Visual.SetDlgPtr(this); 
}

BOOL CColorBalanceVisualDlg::PreDoModal(LPVOID lpVoid)
{
return CPPMapDlg::PreDoModal(lpVoid);
}

BOOL CColorBalanceVisualDlg::OnInitDialog()
{
PreDoModal();
HWND hDlg = GetSafeHwnd();
LPIMAGE lpImage = GetImage();

InitPreview();
CenterWindow();

m_Data.wBand = BAND_ALL;
CheckComboItem( hDlg, IDC_BAND, IDC_BAND_ALL, IDC_BAND_LO,
	m_Data.wBand + IDC_BAND_ALL );
m_Data.hDlg     = hDlg;
m_Data.lpBuffer = NULL;
m_Data.lpLuts   = Alloc(PACKEDLUTSIZE*9);
m_Data.bMaintainDensity = TRUE;
CheckDlgButton(IDC_MAINTAIN, m_Data.bMaintainDensity);

if (!m_Data.lpLuts)
{
	Message(IDS_EMEMALLOC);
	AstralDlgEnd( hDlg, FALSE);
	return(TRUE);
}

m_Visual.m_lpImage = lpImage;
m_Visual.m_lpOrgFrame = NULL;
m_Visual.m_lpBasisFrame = NULL;
m_Visual.m_lpUser = (LPTR)&m_Data;
m_Visual.m_nStep = 5;
m_Visual.m_wMoveType = 2; 	// all but center re-done
//m_Visual.pDrawVisual = DrawVisualEx;
//m_Visual.pMoveVisual = MoveVisualEx;
//m_pMoveVisual = this;
//m_pDrawVisual = this;
ResetAllMaps();

return CPPVisualMapDlg::OnInitDialog();
}

void CColorBalanceVisualDlg::OnOK()
{
		int i;
		BalanceVisual_PrepareValues();
		m_Parms.MasterContrast = m_MasterMap.Contrast;
		for (i = 0; i < 4; ++i)
			m_Parms.MasterBrightness[i] = m_MasterMap.Brightness[i];
		m_Parms.fMasterReverse = m_MasterMap.Reverse;
		m_Parms.RedContrast = m_RedMap.Contrast;
		for (i = 0; i < 4; ++i)
			m_Parms.RedBrightness[i] = m_RedMap.Brightness[i];
		m_Parms.fRedReverse = m_RedMap.Reverse;
		m_Parms.GreenContrast = m_GreenMap.Contrast;
		for (i = 0; i < 4; ++i)
			m_Parms.GreenBrightness[i] = m_GreenMap.Brightness[i];
		m_Parms.fGreenReverse = m_GreenMap.Reverse;
		m_Parms.BlueContrast = m_BlueMap.Contrast;
		for (i = 0; i < 4; ++i)
			m_Parms.BlueBrightness[i] = m_BlueMap.Brightness[i];
		m_Parms.fBlueReverse = m_BlueMap.Reverse;
		m_Parms.BlackContrast = m_BlackMap.Contrast;
		for (i = 0; i < 4; ++i)
			m_Parms.BlackBrightness[i] = m_BlackMap.Brightness[i];
		m_Parms.fBlackReverse = m_BlackMap.Reverse;
		ResetAllMaps();
		UndoMapPreview();
		PostDoModal();
		CPPVisualMapDlg::OnOK();

}


LPVOID CColorBalanceVisualDlg::PostDoModal()
{
	return CPPMapDlg::PostDoModal();
}

BOOL CColorBalanceVisualDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int id = LOWORD(wParam);
	WORD codeNotify = HIWORD(wParam);
	HWND hControl = (HWND) lParam;
	LPIMAGE lpImage = GetImage();
	HWND hDlg = GetSafeHwnd();

switch (id)
	{
	case IDC_BAND:
      DoPreview(FALSE);
		if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
			break;
		// setup new band
		if (m_Data.wBand  != id - IDC_BAND_ALL)
		{
			m_Data.wBand = id - IDC_BAND_ALL;
			SetVisual(0,0);
		}
	break;
	
	case IDC_MAINTAIN:
      DoPreview(FALSE);
		m_Data.bMaintainDensity = !m_Data.bMaintainDensity;
		CheckDlgButton(IDC_MAINTAIN, m_Data.bMaintainDensity);
		SetVisual(0,0);
	break;
	
	case IDC_PREVIEW:
      DoPreview(!m_bIsInPreview);
		BalanceVisual_PrepareValues();
		SetMapGlobals();
		MapPreview();
		BalanceVisual_RestoreValues();
	break;

	case IDC_RESETALL:
      DoPreview(FALSE);
		CPPVisualMapDlg::OnCommandEx(MAKEWPARAM(IDC_VISUAL_BEFORE,NULL), 0);
	break;

	case IDC_RESET:
      DoPreview(FALSE);
		ResetBandAllMaps(m_Data.wBand);
		SetVisual(0,0);
	break;


	default:
      DoPreview(FALSE);
		CPPVisualMapDlg::OnCommandEx(wParam, lParam);
	break;
   }
return CWnd::OnCommand(wParam, lParam);}

BEGIN_MESSAGE_MAP(CColorBalanceVisualDlg, CPPMapDlg)
	ON_WM_DESTROY()
	ON_WM_QUERYNEWPALETTE()
END_MESSAGE_MAP()

void CColorBalanceVisualDlg::OnDestroy()
{
 if (m_Data.lpLuts)
	FreeUp(m_Data.lpLuts);
if (m_Data.lpBuffer)
	FreeUp(m_Data.lpBuffer); 
CPPVisualMapDlg::OnDestroy();
}

BOOL CColorBalanceVisualDlg::OnQueryNewPalette()
{
	return Dialog_OnQueryNewPalette(GetSafeHwnd());
}

/////////////////////////////////////////CColorBalanceVisual Methods/////////////////////////////////////

void CColorBalanceVisual::DrawVisual(HDC hDC,LPRECT lpRect, int x,int y)
{
	LPBALANCESTRUCT  lpBal;
	int w, h, id;
	RECT SourceRect;
	POINT DestPoint;
	LPFRAME lpFrame;
	int depth;
	LPVOID lpCmsXform = NULL;
	
	lpBal = (LPBALANCESTRUCT)m_lpUser;

	if (!lpBal)
		return;

	lpFrame = m_lpBasisFrame;

	// Check if frames setup
	if (!m_lpOrgFrame || !lpFrame)
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

	SourceRect.left   =	0;
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
			m_pDlg->MapSetPackedLut(lpBal->lpLuts+PACKEDLUTSIZE*id);
		}
		else
		{
			m_pDlg->MapSetPackedLut(NULL);
		}
		if (m_pDlg->m_Visual.m_lpImage)
			lpCmsXform = m_pDlg->m_Visual.m_lpImage->m_cmsXform;
			m_pDlg->SetMapGlobals();
		m_pDlg->DisplayMappedImage(hDC,lpFrame,&SourceRect, DestPoint,
			lpBal->lpBuffer,0,0, lpCmsXform);
	}
}

BOOL CColorBalanceVisual::MoveVisual(int x ,int y)
{
	LPBALANCESTRUCT  lpBAL;
	int brit;
	int red, green, blue, cyan, magenta, yellow, black, white;
	MAP *lpMap;
	int i,j, id;

	lpBAL = (LPBALANCESTRUCT)m_lpUser;

	if (!lpBAL)
		return(FALSE);

	m_pDlg->UndoMapPreview();

	lpMap = NULL;
	if (x == VISUAL_BEFORE)
		m_pDlg->ResetAllMaps();
	else if (lpMap = m_pDlg->BalanceVisual_GetNew(x , y, &brit))
		m_pDlg->BalanceVisual_MakeMaps(lpMap, brit, lpBAL, YES);

	// take care of after frame
	id = IDC_VISUAL_11 - IDC_VISUAL_00;

	m_pDlg->MapGetPackedLut(lpBAL->lpLuts+PACKEDLUTSIZE*id,
		m_pDlg->m_HueMap.Lut,
		m_pDlg->m_SatMap.Lut,
		m_pDlg->m_MasterMap.Lut,
		m_pDlg->m_RedMap.Lut,
		m_pDlg->m_GreenMap.Lut,
		m_pDlg->m_BlueMap.Lut,
		m_pDlg->m_BlackMap.Lut);

	for(j=-1;j<=1;j++) 
	{
		for(i=-1;i<=1;i++) 
		{
			if (id = GetIDFromPos(i, j))
			{
				id -= IDC_VISUAL_00;
				if (WITHIN(id,0,8) &&
					(lpMap = m_pDlg->BalanceVisual_GetNew(i , j, &brit)))
				{
					// create maps
					m_pDlg->BalanceVisual_MakeMaps(lpMap, brit, lpBAL, NO);

					m_pDlg->MapGetPackedLut(lpBAL->lpLuts+PACKEDLUTSIZE*id,
						m_pDlg->m_HueMap.Lut,
						m_pDlg->m_SatMap.Lut,
						m_pDlg->m_MasterMap.Lut,
						m_pDlg->m_RedMap.Lut,
						m_pDlg->m_GreenMap.Lut,
						m_pDlg->m_BlueMap.Lut,
						m_pDlg->m_BlackMap.Lut);
					// reset maps
					m_pDlg->BalanceVisual_MakeMaps(lpMap, lpMap->Brightness[lpBAL->wBand], lpBAL, NO);
				}
			}
		}
	}
	
	// display values
	m_pDlg->BalanceVisual_PrepareValues();
	lpMap = m_pDlg->ActivateChannel(IDC_DO_ALL);
	if (lpMap){	white = lpMap->Brightness[lpBAL->wBand]; black = -white;}

	lpMap = m_pDlg->ActivateChannel(IDC_DO_RED);
	if (lpMap){	red = -lpMap->Brightness[lpBAL->wBand]; cyan = -red;}

	lpMap = m_pDlg->ActivateChannel(IDC_DO_GREEN);
	if (lpMap){	green = -lpMap->Brightness[lpBAL->wBand]; magenta = -green;}

	lpMap = m_pDlg->ActivateChannel(IDC_DO_BLUE);
	if (lpMap){	blue = -lpMap->Brightness[lpBAL->wBand]; yellow = -blue;}

	SetDlgItemIntExt(lpBAL->hDlg, IDC_CORRECT_R, red,     " %" );
	SetDlgItemIntExt(lpBAL->hDlg, IDC_CORRECT_G, green,   " %" );
	SetDlgItemIntExt(lpBAL->hDlg, IDC_CORRECT_B, blue,    " %" );
	SetDlgItemIntExt(lpBAL->hDlg, IDC_CORRECT_C, cyan,    " %" );
	SetDlgItemIntExt(lpBAL->hDlg, IDC_CORRECT_M, magenta, " %" );
	SetDlgItemIntExt(lpBAL->hDlg, IDC_CORRECT_Y, yellow,  " %" );
	SetDlgItemIntExt(lpBAL->hDlg, IDC_CORRECT_K, black,   " %" );
	SetDlgItemIntExt(lpBAL->hDlg, IDC_CORRECT_W, white,   " %" );

	m_pDlg->BalanceVisual_RestoreValues();
	return(TRUE);
}



void CColorBalanceVisualDlg::BalanceVisual_MakeMaps(LPMAP lpMap, int newBrit, LPBALANCESTRUCT lpBAL, BOOL bPerminant)
{
	int old;
	int red, green, blue;
	int dred, dgreen, dblue;
	BOOL bMaintain;
	
	bMaintain = lpBAL->bMaintainDensity;
	dred = dgreen = dblue = 0;
	red = 	m_RedMap.Brightness[lpBAL->wBand];
	green = m_GreenMap.Brightness[lpBAL->wBand];
	blue = 	m_BlueMap.Brightness[lpBAL->wBand];
	
	if (lpMap == &m_RedMap)
	{
		dred = newBrit-red;
		if (!bMaintain)
			goto DoIt;
		dgreen = -(dred)/2;
		dblue = -(dred)/2;
	}
	else if (lpMap == &m_GreenMap)
	{
		dgreen = newBrit-green;
		if (!bMaintain)
			goto DoIt;
		dred = -(dgreen)/2;
		dblue = -(dgreen)/2;
	}
	else if (lpMap == &m_BlueMap)
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
	m_BlueMap.Brightness[lpBAL->wBand] = mbound(blue+dblue, -100,100);
	m_GreenMap.Brightness[lpBAL->wBand] = mbound(green+dgreen, -100,100);
	m_RedMap.Brightness[lpBAL->wBand] = mbound(red+dred, -100,100);
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
		m_BlueMap.Brightness[lpBAL->wBand] = blue;
		m_GreenMap.Brightness[lpBAL->wBand] = green;
		m_RedMap.Brightness[lpBAL->wBand] = red;
	}
}



//	Returns the color map associated with the x,y pos.
//		sets newBrit to be the new brightness associated with the channel
//		returns NULL if  failure or 'After'
MAP* CColorBalanceVisualDlg::BalanceVisual_GetNew(int x ,int y, LPINT newBrit)
{
	MAP *lpMap;
	LPBALANCESTRUCT  lpBAL;

	if (x == VISUAL_BEFORE)
		return(NULL);

	if (!WITHIN(x, -1,1))
		return(NULL);

	lpBAL = (LPBALANCESTRUCT)m_Visual.m_lpUser;

	if (!lpBAL)
		return(FALSE);

	switch (y)
	{
		case -1:
			if (x==-1)
			{
				lpMap = ActivateChannel(IDC_DO_RED);
				*newBrit = lpMap->Brightness[lpBAL->wBand]+(m_Visual.m_nStep*-2);
			}  
			else if (x==1)
			{
				lpMap = ActivateChannel(IDC_DO_BLUE);
				*newBrit = lpMap->Brightness[lpBAL->wBand]+(m_Visual.m_nStep*2);
			}
			else
			{
				lpMap = ActivateChannel(IDC_DO_ALL);
				*newBrit = lpMap->Brightness[lpBAL->wBand]+m_Visual.m_nStep*2;
			} 			
		break;

		case 0:
			if (x==1) 
			{
				lpMap = ActivateChannel(IDC_DO_GREEN);
				*newBrit = lpMap->Brightness[lpBAL->wBand]+(m_Visual.m_nStep*-2);
			}				
			else if (x==-1) 
			{
				lpMap = ActivateChannel(IDC_DO_GREEN);
				*newBrit = lpMap->Brightness[lpBAL->wBand]+(m_Visual.m_nStep*2);
			}  
			else
				return(NULL);							
		break;

		case 1:
			if (x==-1)
			{
				lpMap = ActivateChannel(IDC_DO_BLUE);
				*newBrit = lpMap->Brightness[lpBAL->wBand]+(m_Visual.m_nStep*-2);
			}
			else if (x==1) 
			{
				lpMap = ActivateChannel(IDC_DO_RED);
				*newBrit = lpMap->Brightness[lpBAL->wBand]+(m_Visual.m_nStep*2);
			}			
			else 
			{
				lpMap = ActivateChannel(IDC_DO_ALL);
				*newBrit = lpMap->Brightness[lpBAL->wBand]-m_Visual.m_nStep*2;
			}
		break;

		default:
			return(NULL);
		break;
	}

	*newBrit = Bound(*newBrit, -100, 100);

	return(lpMap);
}




//	This is a 11th hour workaround to keep more accuracy in the brightness.
//	Ultimately the mapping stuff should use more than 100 levels.
void CColorBalanceVisualDlg::BalanceVisual_PrepareValues()
{
	int band;
	int val;
	
	for (band=0; band<4; band++)
	{
	 	val = m_BalVals[0][band] = m_MasterMap.Brightness[band];
		m_MasterMap.Brightness[band] = val/2;
		val = m_BalVals[1][band] = m_RedMap.Brightness[band];
		m_RedMap.Brightness[band] = val/2;
		val = m_BalVals[2][band] = m_GreenMap.Brightness[band];
		m_GreenMap.Brightness[band] = val/2;
		val = m_BalVals[3][band] = m_BlueMap.Brightness[band];
		m_BlueMap.Brightness[band] = val/2;
	}
}


void CColorBalanceVisualDlg::BalanceVisual_RestoreValues()
{
	int band;
	
	for (band=0; band<4; band++)
	{
	 	m_MasterMap.Brightness[band]=	m_BalVals[0][band]; 
		m_RedMap.Brightness[band]	  =	m_BalVals[1][band]; 
		m_GreenMap.Brightness[band] =	m_BalVals[2][band]; 
		m_BlueMap.Brightness[band] =	m_BalVals[3][band]; 
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////  CPosterizeDlg  ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
CPosterizeDlg::CPosterizeDlg(CServerView* pView, CWnd* pParent)
:CPPMapDlg(pView, CPosterizeDlg::IDD, pParent)
{

}

BOOL CPosterizeDlg::PreDoModal(LPVOID lpVoid)
{
	m_bAutoPreview = GetDefaultInt(POSTERIZE_AUTOPREVIEW, YES);
	m_MasterMap.Levels    = 5;
	return CPPMapDlg::PreDoModal(lpVoid);

}

BOOL CPosterizeDlg::OnInitDialog()
{
PreDoModal();

FRMDATATYPE type;
HWND hDlg = GetSafeHwnd();

RightPopup( hDlg );
ResetAllMaps();
MakeMap(&m_MasterMap);

type = FrameType(ImgGetBaseEditFrame(GetImage()));
m_MapValues.wChannel = IDC_DO_ALL;
ChannelsCombo_Setup(IDC_CHANNELS, type);

CheckDlgButton(IDC_AUTO, m_bAutoPreview);
ControlEnable(hDlg, IDC_PREVIEW, !m_bAutoPreview);

if ( FrameDepth(ImgGetBaseEditFrame(GetImage())) == 1 )
	{
	ControlEnable( hDlg, IDC_DO_RED, NO );
	ControlEnable( hDlg, IDC_DO_GREEN, NO );
	ControlEnable( hDlg, IDC_DO_BLUE, NO );
	ControlEnable( hDlg, IDC_DO_CYAN, NO );
	ControlEnable( hDlg, IDC_DO_MAGENTA, NO );
	ControlEnable( hDlg, IDC_DO_YELLOW, NO );
	}

m_MapValues.pMap = Posterize_ResetChannel(m_MapValues.wChannel );

if (m_bAutoPreview)
   {
   SetMapGlobals();
   AutoPreview(m_MapValues.pMap, m_bAutoPreview, YES);
   }


return CPPMapDlg::OnInitDialog();

}

void CPosterizeDlg::OnOK()
{
	UndoMapPreview();
	m_Parms.MasterInfo.Levels = m_MasterMap.Levels;
	m_Parms.MasterInfo.Threshold = m_MasterMap.Threshold;
	m_Parms.RedInfo.Levels = m_RedMap.Levels;
	m_Parms.RedInfo.Threshold = m_RedMap.Threshold;
	m_Parms.GreenInfo.Levels = m_GreenMap.Levels;
	m_Parms.GreenInfo.Threshold = m_GreenMap.Threshold;
	m_Parms.BlueInfo.Levels = m_BlueMap.Levels;
	m_Parms.BlueInfo.Threshold = m_BlueMap.Threshold;
	m_Parms.BlackInfo.Levels = m_BlackMap.Levels;
	m_Parms.BlackInfo.Threshold = m_BlackMap.Threshold;
	ResetAllMaps();
	UnAutoPreview();
	PostDoModal();
	CPPMapDlg::OnOK();
}

void CPosterizeDlg::OnCancel()
{
	ResetAllMaps();
	UnAutoPreview();
	CPPMapDlg::OnCancel();
}

LPVOID CPosterizeDlg::PostDoModal()
{
	PutDefaultInt(POSTERIZE_AUTOPREVIEW, m_bAutoPreview);
	return CPPMapDlg::PostDoModal();
}

BOOL CPosterizeDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int i;
	BOOL Bool;
	LPIMAGE lpImage = GetImage();

	int id = LOWORD(wParam);
	WORD codeNotify = HIWORD(wParam);
	HWND hControl = (HWND) lParam;
	HWND hDlg = GetSafeHwnd();


switch (id)
	{
	case IDC_CHANNELS:
		if (!(i=ChannelsCombo_Handle(id, codeNotify)))
			break;
		m_MapValues.wChannel = i;
		m_MapValues.pMap = Posterize_ResetChannel(m_MapValues.wChannel );
	break;

	case IDC_MAPLEVELS_SCROLL:
		if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
			break;
		i = HandleSlide( hDlg, id, codeNotify, &Bool );
		if ( m_MapValues.pMap->Levels == i )
			break;
		m_MapValues.pMap->Levels = i;
		SetDlgItemSpin( hDlg, IDC_MAPLEVELS, m_MapValues.pMap->Levels, NO );
		SetMapGlobals();
		AutoPreview(m_MapValues.pMap, m_bAutoPreview, Bool);
	break;


	case IDC_MAPLEVELS:
		if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		i = GetDlgItemSpin( hDlg, IDC_MAPLEVELS, &Bool, NO );
		if ( m_MapValues.pMap->Levels == i )
			break;
		m_MapValues.pMap->Levels = i;
		SetSlide( hDlg, IDC_MAPLEVELS_SCROLL, m_MapValues.pMap->Levels );
		SetMapGlobals();	
		AutoPreview(m_MapValues.pMap, m_bAutoPreview, NO);
	break;

	case IDC_AUTO:
		m_bAutoPreview = !m_bAutoPreview;
		CheckDlgButton(id, m_bAutoPreview);
		ControlEnable(hDlg, IDC_PREVIEW, !m_bAutoPreview);
		SetMapGlobals();	
		AutoPreview(m_MapValues.pMap, m_bAutoPreview, YES);
	break; 
	
	case IDC_PREVIEW:
	  	AutoPreview_Button(m_MapValues.pMap);
	break;

	case IDC_RESETALL:
		ResetAllMaps();
		m_MapValues.pMap = Posterize_ResetChannel(m_MapValues.wChannel );
		SetMapGlobals();
		AutoPreview(m_MapValues.pMap, m_bAutoPreview, YES);
	break;

	case IDC_RESET:
		ResetMapEx( m_MapValues.pMap, MAPPOINTS, -1, m_MapValues.wChannel == IDC_DO_ALL );
		m_MapValues.pMap = Posterize_ResetChannel(m_MapValues.wChannel );
		SetMapGlobals();
		AutoPreview(m_MapValues.pMap, m_bAutoPreview, YES);
	break;



   default:
	break;
   }
return CWnd::OnCommand(wParam, lParam);}

BEGIN_MESSAGE_MAP(CPosterizeDlg, CPPMapDlg)
	ON_WM_DESTROY()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CPosterizeDlg::OnDestroy()
{
	AutoPreview_OnDestroy();
	Dialog_OnDestroy(GetSafeHwnd());

}

BOOL CPosterizeDlg::OnQueryNewPalette()
{
	return Dialog_OnQueryNewPalette(GetSafeHwnd());
}

void CPosterizeDlg::OnTimer(UINT nIDEvent)
{
	if (!AutoPreview_OnTimer(nIDEvent))
		return;
		
	// got good timer		   
	MakeMap(m_MapValues.pMap);
	SetMapGlobals();
	MapPreview();

}

/////////////////////////////////////////CPosterizeDlg Methods///////////////////////////////////////////

MAP* CPosterizeDlg::Posterize_ResetChannel(WORD wChannel )
{
	MAP *lpMap; //since this is a static pointer to a static structure,
	// don't make it a far pointer because the data segment might move

	HWND hDlg = GetSafeHwnd();
	lpMap = ActivateChannel( wChannel );
	InitDlgItemSpin( hDlg, IDC_MAPLEVELS, lpMap->Levels, NO, 1, 256 );
	InitSlide( hDlg, IDC_MAPLEVELS_SCROLL, lpMap->Levels, 1, 256 );
	InitDlgItemSpin( hDlg, IDC_MAPTHRESH, lpMap->Threshold, NO, 0, 100 );
	InitSlide( hDlg, IDC_MAPTHRESH_SCROLL, lpMap->Threshold, 0, 100 );

	return( lpMap );
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////  CThresholdDlg  /////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CThresholdDlg::CThresholdDlg(CServerView* pView, CWnd* pParent)
:CPPMapDlg(pView, CThresholdDlg::IDD, pParent)
{

}


BOOL CThresholdDlg::PreDoModal(LPVOID lpVoid)
{
	m_bAutoPreview = GetDefaultInt(THRESHOLD_AUTOPREVIEW, YES);
   	m_MasterMap.Threshold = 50;
	return CPPMapDlg::PreDoModal(lpVoid);

}

BOOL CThresholdDlg::OnInitDialog()
{
FRMDATATYPE type;
HWND hDlg = GetSafeHwnd();

RightPopup( hDlg );
ResetAllMaps();
MakeMap(&m_MasterMap);

type = FrameType(ImgGetBaseEditFrame(GetImage()));
m_MapValues.wChannel = IDC_DO_ALL;
ChannelsCombo_Setup(IDC_CHANNELS, type);

CheckDlgButton(IDC_AUTO, m_bAutoPreview);
ControlEnable(hDlg, IDC_PREVIEW, !m_bAutoPreview);

if ( FrameDepth(ImgGetBaseEditFrame(GetImage())) == 1 )
	{
	ControlEnable( hDlg, IDC_DO_RED, NO );
	ControlEnable( hDlg, IDC_DO_GREEN, NO );
	ControlEnable( hDlg, IDC_DO_BLUE, NO );
	ControlEnable( hDlg, IDC_DO_CYAN, NO );
	ControlEnable( hDlg, IDC_DO_MAGENTA, NO );
	ControlEnable( hDlg, IDC_DO_YELLOW, NO );
	}

m_MapValues.pMap = Threshold_ResetChannel(m_MapValues.wChannel );

if (m_bAutoPreview)
   {
   SetMapGlobals();
   AutoPreview(m_MapValues.pMap, m_bAutoPreview, YES);
   }

return CPPMapDlg::OnInitDialog();


}

void CThresholdDlg::OnOK()
{
	UndoMapPreview();
	m_Parms.MasterInfo.Levels = m_MasterMap.Levels;
	m_Parms.MasterInfo.Threshold = m_MasterMap.Threshold;
	m_Parms.RedInfo.Levels = m_RedMap.Levels;
	m_Parms.RedInfo.Threshold = m_RedMap.Threshold;
	m_Parms.GreenInfo.Levels = m_GreenMap.Levels;
	m_Parms.GreenInfo.Threshold = m_GreenMap.Threshold;
	m_Parms.BlueInfo.Levels = m_BlueMap.Levels;
	m_Parms.BlueInfo.Threshold = m_BlueMap.Threshold;
	m_Parms.BlackInfo.Levels = m_BlackMap.Levels;
	m_Parms.BlackInfo.Threshold = m_BlackMap.Threshold;
	ResetAllMaps();
	UnAutoPreview();
	PostDoModal();
	CPPMapDlg::OnOK();

}

void CThresholdDlg::OnCancel()
{
	ResetAllMaps();
	UnAutoPreview();
	CPPMapDlg::OnCancel();

}

LPVOID CThresholdDlg::PostDoModal()
{
	PutDefaultInt(THRESHOLD_AUTOPREVIEW, m_bAutoPreview);
	return CPPMapDlg::PostDoModal();
}

BOOL CThresholdDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int i;
	BOOL Bool;
	LPIMAGE lpImage = GetImage();

	int id = LOWORD(wParam);
	WORD codeNotify = HIWORD(wParam);
	HWND hControl = (HWND) lParam;
	HWND hDlg = GetSafeHwnd();



switch (id)
	{
	case IDC_CHANNELS:
		if (!(i=ChannelsCombo_Handle(id, codeNotify)))
			break;
		m_MapValues.wChannel = i;
		m_MapValues.pMap = Threshold_ResetChannel(m_MapValues.wChannel );
	break;


	case IDC_MAPTHRESH_SCROLL:
		if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
			break;
		i = HandleSlide( hDlg, id, codeNotify, &Bool );
		if ( m_MapValues.pMap->Threshold == i )
			break;
		m_MapValues.pMap->Threshold = i;
		SetDlgItemSpin( hDlg, IDC_MAPTHRESH, m_MapValues.pMap->Threshold, NO);
		SetMapGlobals();
		AutoPreview(m_MapValues.pMap, m_bAutoPreview, Bool);
	break;


	case IDC_MAPTHRESH:
		if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		i = GetDlgItemSpin( hDlg, IDC_MAPTHRESH, &Bool, NO );
		if ( m_MapValues.pMap->Threshold == i )
			break;
		m_MapValues.pMap->Threshold = i;
		SetSlide( hDlg, IDC_MAPTHRESH_SCROLL, m_MapValues.pMap->Threshold );
		SetMapGlobals();
		AutoPreview(m_MapValues.pMap, m_bAutoPreview, NO);
	break;

	case IDC_AUTO:
		m_bAutoPreview = !m_bAutoPreview;
		CheckDlgButton(id, m_bAutoPreview);
		ControlEnable(hDlg, IDC_PREVIEW, !m_bAutoPreview);
		SetMapGlobals();
		AutoPreview(m_MapValues.pMap, m_bAutoPreview, YES);
	break; 
	
	case IDC_PREVIEW:
	  	AutoPreview_Button(m_MapValues.pMap);
	break;

	case IDC_RESETALL:
		ResetAllMaps();
		m_MapValues.pMap = Threshold_ResetChannel(m_MapValues.wChannel );
		SetMapGlobals();
		AutoPreview(m_MapValues.pMap, m_bAutoPreview, YES);
	break;

	case IDC_RESET:
		ResetMapEx( m_MapValues.pMap, MAPPOINTS, -1, m_MapValues.wChannel == IDC_DO_ALL );
		m_MapValues.pMap = Threshold_ResetChannel(m_MapValues.wChannel );
		SetMapGlobals();
		AutoPreview(m_MapValues.pMap, m_bAutoPreview, YES);
	break;



   default:
	break;
   }
return CWnd::OnCommand(wParam, lParam);}

BEGIN_MESSAGE_MAP(CThresholdDlg, CPPMapDlg)
	ON_WM_DESTROY()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CThresholdDlg::OnDestroy()
{
	AutoPreview_OnDestroy();
	Dialog_OnDestroy(GetSafeHwnd());

}

BOOL CThresholdDlg::OnQueryNewPalette()
{
	return Dialog_OnQueryNewPalette(GetSafeHwnd());
}

void CThresholdDlg::OnTimer(UINT nIDEvent)
{
	if (!AutoPreview_OnTimer(nIDEvent))
		return;
		
	// got good timer		   
	MakeMap(m_MapValues.pMap);
	SetMapGlobals();
	MapPreview();

}

//////////////////////////////////////// CThresholdDlg Methods //////////////////////////////////////////
MAP* CThresholdDlg::Threshold_ResetChannel(WORD wChannel )
{
	MAP *lpMap; //since this is a static pointer to a static structure,
	// don't make it a far pointer because the data segment might move

	HWND hDlg = GetSafeHwnd();
	lpMap = ActivateChannel( wChannel );
	InitDlgItemSpin( hDlg, IDC_MAPLEVELS, lpMap->Levels, NO, 1, 256 );
	InitSlide( hDlg, IDC_MAPLEVELS_SCROLL, lpMap->Levels, 1, 256 );
	InitDlgItemSpin( hDlg, IDC_MAPTHRESH, lpMap->Threshold, NO, 0, 100 );
	InitSlide( hDlg, IDC_MAPTHRESH_SCROLL, lpMap->Threshold, 0, 100 );

	return( lpMap );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////  CHueShiftDlg  //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CHueShiftDlg::CHueShiftDlg(CServerView* pView, CWnd* pParent)
:CPPMapDlg(pView, CHueShiftDlg::IDD, pParent)
{

}

BOOL CHueShiftDlg::PreDoModal(LPVOID lpVoid)
{
	m_nInHue = 0;
	m_nOutHue = 0;
	m_bAutoPreview = GetDefaultInt(HUESHIFT_AUTOPREVIEW, YES);
	m_bColorize = GetDefaultInt(HUESHIFT_COLORIZE, NO);
	return CPPMapDlg::PreDoModal(lpVoid);
}

BOOL CHueShiftDlg::OnInitDialog()
{
	PreDoModal();
	HWND hDlg = GetSafeHwnd();
	RightPopup( hDlg );
	CheckDlgButton(IDC_HUE_COLORIZE, m_bColorize);
	CheckDlgButton(IDC_AUTO, m_bAutoPreview);
	ControlEnable(hDlg, IDC_PREVIEW, !m_bAutoPreview);
	ResetMap( &m_HueMap, HUEMAPPOINTS, NO );
	ResetMap( &m_SatMap, MAPPOINTS, NO );
	ResetMapEx( &m_MasterMap, MAPPOINTS, NO, YES);
	// Initialize controls
	InitDlgItemSpin( hDlg, IDC_SATURATION, -m_SatMap.Brightness[0], YES, -100, 100 );
	InitSlide( hDlg, IDC_SATURATION_SCROLL, -m_SatMap.Brightness[0], -100, 100 );
	InitDlgItemSpin( hDlg, IDC_LIGHTNESS, -m_MasterMap.Brightness[0], YES, -100, 100 );
	InitSlide( hDlg, IDC_LIGHTNESS_SCROLL, -m_MasterMap.Brightness[0], -100, 100 );
	InitSlide( hDlg, IDC_SLIDEHUE1, 0, 0, TOANGLE(MAX_HUES-1) );
	InitDlgItemSpin( hDlg, IDC_HUE_VALUE, 0, YES, 0, TOANGLE(MAX_HUES-1) );
	
	// actually set controls
	HueShift_SetControls(YES );
	return CPPMapDlg::OnInitDialog();

}

void CHueShiftDlg::OnOK()
{
	UndoMapPreview();
	HueShift_Make_Map();
	map_to_mapinfo(&m_HueMap, &m_Parms.HueInfo);
	m_Parms.Saturation = m_SatMap.Brightness[0];
	m_Parms.Brightness = m_MasterMap.Brightness[0];
	ResetAllMaps();
	SetMapGlobals();
	AnimateMap(NULL, YES);
	PostDoModal();
	CPPMapDlg::OnOK();
}

void CHueShiftDlg::OnCancel()
{
	ResetAllMaps();
	SetMapGlobals();
	AnimateMap(NULL, YES);
	UndoMapPreview();
	CPPMapDlg::OnCancel();
}

LPVOID CHueShiftDlg::PostDoModal()
{
	PutDefaultInt(HUESHIFT_AUTOPREVIEW, m_bAutoPreview);
	PutDefaultInt(HUESHIFT_COLORIZE, m_bColorize);
	return CPPMapDlg::PostDoModal();
}

BOOL CHueShiftDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	BOOL Bool;
	int i, newhue;
	LPIMAGE lpImage = GetImage();
	HWND hDlg = GetSafeHwnd();

	int id = LOWORD(wParam);
	WORD codeNotify = HIWORD(wParam);
	HWND hControl = (HWND) lParam;

	
	switch (id)
	{
		case IDC_SLIDEHUE1:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			i = HandleSlide( hDlg, id, codeNotify, &Bool );
			// get the true hue
			newhue = ((m_nInHue + FROMANGLE(i) + MAX_HUES ) % MAX_HUES);
			if ( m_nOutHue != newhue )
			{
				m_nOutHue = newhue;
				HueShift_Make_Map();
				HueShift_SetControls(YES );
				SetMapGlobals();
				AutoPreview(&m_HueMap, m_bAutoPreview, Bool);
			}
		break;
		
		case IDC_HUE_VALUE:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			i = (int)GetDlgItemSpin( hDlg, id, &Bool, NO );
			newhue = ((m_nInHue + FROMANGLE(i) + MAX_HUES ) % MAX_HUES);
			if ( m_nOutHue != newhue )
			{
				m_nOutHue = newhue;
				HueShift_Make_Map();
				HueShift_SetControls(NO );
				SetMapGlobals();
				AutoPreview(&m_HueMap, m_bAutoPreview, NO);
			}
		break;
	
		case IDC_HUE1:
			m_nOutHue = m_nInHue;
			HueShift_Make_Map();
			HueShift_SetControls(YES );
			SetMapGlobals();
			AutoPreview(&m_HueMap, m_bAutoPreview, YES);
		break;
	
		case IDC_MAP_PROBE:
			Probe_Begin(codeNotify);
		break;
	
		case IDC_AUTO:
			m_bAutoPreview= !m_bAutoPreview;
			CheckDlgButton(id, m_bAutoPreview);
        	ControlEnable(hDlg, IDC_PREVIEW, !m_bAutoPreview);
		 	SetMapGlobals();  
		   	AutoPreview(&m_HueMap, m_bAutoPreview, YES);
		break;
		
		case IDC_HUE_COLORIZE:
			m_bColorize = !m_bColorize;
			CheckDlgButton(id, m_bColorize);
			HueShift_Make_Map();
		 	SetMapGlobals();
		   	AutoPreview(&m_HueMap, m_bAutoPreview, YES);
		break;
	
		case IDC_SATURATION:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			i = (int)GetDlgItemSpin( hDlg, IDC_SATURATION, &Bool, YES );
			if ( m_SatMap.Brightness[0] != -i )
			{
				m_SatMap.Brightness[0] = -i;
				SetSlide( hDlg, IDC_SATURATION_SCROLL, i );
		   		SetMapGlobals();
				AutoPreview(&m_SatMap, m_bAutoPreview, NO);
			}
		break;
	
		case IDC_SATURATION_SCROLL:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			i = HandleSlide( hDlg, id, codeNotify, &Bool );
			if ( m_SatMap.Brightness[0] != -i )
			{
				m_SatMap.Brightness[0] = -i;
				SetDlgItemSpin( hDlg, IDC_SATURATION, i, YES );
				SetMapGlobals();
				AutoPreview(&m_SatMap, m_bAutoPreview, Bool);
			}
		break;
	
		case IDC_LIGHTNESS:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			i = (int)GetDlgItemSpin( hDlg, IDC_LIGHTNESS, &Bool, YES );
			if ( m_MasterMap.Brightness[0] != -i )
			{
				m_MasterMap.Brightness[0] = -i;
				SetSlide( hDlg, IDC_LIGHTNESS_SCROLL, i );
				SetMapGlobals();
				AutoPreview(&m_MasterMap, m_bAutoPreview, NO);
			}
		break;
	
		case IDC_LIGHTNESS_SCROLL:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			i = HandleSlide( hDlg, id, codeNotify, &Bool);
			if ( m_MasterMap.Brightness[0] != -i )
			{
				m_MasterMap.Brightness[0] = -i;
				SetDlgItemSpin( hDlg, IDC_LIGHTNESS, i, YES );
  				SetMapGlobals();
  				AutoPreview(&m_MasterMap, m_bAutoPreview, Bool);
			}
		break;
	
		case IDC_PREVIEW:
			AutoPreview_Button(&m_HueMap);
		break;
	
		case IDC_RESETALL:
		case IDC_RESET:
			ResetMap( &m_HueMap, HUEMAPPOINTS, NO );
			ResetMap( &m_SatMap, MAPPOINTS, NO );
			ResetMapEx( &m_MasterMap, MAPPOINTS, NO, YES);
			m_nOutHue = m_nInHue;
			MakeMap(&m_SatMap);
			MakeMap(&m_MasterMap);
			HueShift_Make_Map();
			HueShift_SetControls(YES );
			SetMapGlobals();
			AutoPreview(&m_HueMap, m_bAutoPreview, YES);
		break;
	
		
	   default:
		break;
	   }
return CWnd::OnCommand(wParam, lParam);}

BEGIN_MESSAGE_MAP(CHueShiftDlg, CPPMapDlg)
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CHueShiftDlg::OnDestroy()
{
	AutoPreview_OnDestroy();
	Probe_End();

}

void CHueShiftDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	COLORINFO Color;
	
	if (Probe_OnMouseMove(point.x, point.y, nFlags, &Color))
	{
		// probing with valid value
		Hue_SetPatch(IDC_INHUE1, Color.hsl.hue);
	}
	else if (m_bProbing) 
	{
		// probing but no valid value
		Hue_SetPatch(IDC_INHUE1, m_nInHue);
	}
	

}

void CHueShiftDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	COLORINFO Color;
	BOOL bGotPoint;
	
	if (!m_bProbing)
		return;
	bGotPoint = Probe_OnLButtonUp(point.x, point.y, nFlags,&Color);
	if (!bGotPoint)
	{
		// unsucessful probe session
		Hue_SetPatch(IDC_INHUE1, m_nInHue);
		return;
	}
	else
	{
		m_nOutHue = m_nOutHue + Color.hsl.hue - m_nInHue;
		m_nOutHue = m_nOutHue % MAX_HUES;
		m_nInHue = Color.hsl.hue;
		HueShift_Make_Map();
		// reset controls
		HueShift_SetControls(YES );
	}

}

BOOL CHueShiftDlg::OnQueryNewPalette()
{
	return Dialog_OnQueryNewPalette(GetSafeHwnd());
}

void CHueShiftDlg::OnTimer(UINT nIDEvent)
{
	if (!AutoPreview_OnTimer(nIDEvent))
		return;
		
	// got good timer
	MakeMap(&m_HueMap);
	MakeMap(&m_SatMap);
	MakeMap(&m_MasterMap);
	SetMapGlobals();
	MapPreview();

}

////////////////////////////////////////CHueShiftDlg Methods/////////////////////////////////////////////

void CHueShiftDlg::HueShift_SetControls(BOOL bUpdateHueVal )
{
	int delta, i;
	BOOL Bool;

	HWND hDlg = GetSafeHwnd();
	
	i = GetDlgItemSpin(hDlg, IDC_SATURATION, &Bool, YES );
	if (i != -m_SatMap.Brightness[0] || !Bool)
		SetDlgItemSpin( hDlg, IDC_SATURATION, -m_SatMap.Brightness[0], YES);
	SetSlide( hDlg, IDC_SATURATION_SCROLL, -m_SatMap.Brightness[0]);
	i = GetDlgItemSpin(hDlg, IDC_LIGHTNESS, &Bool, YES );
	if (i != -m_MasterMap.Brightness[0] || !Bool)
		SetDlgItemSpin( hDlg, IDC_LIGHTNESS, -m_MasterMap.Brightness[0], YES);
	SetSlide( hDlg, IDC_LIGHTNESS_SCROLL, -m_MasterMap.Brightness[0]);
	delta = (m_nOutHue-m_nInHue + MAX_HUES ) % MAX_HUES;   //was HUESHIFT_DELTA
	SetSlide( hDlg, IDC_SLIDEHUE1, TOANGLE(delta));
	if (m_bColorize)
		delta = m_nOutHue;
	if (bUpdateHueVal)
	{
		i = GetDlgItemSpin(hDlg, IDC_HUE_VALUE, &Bool, YES );
		if (i != TOANGLE(delta) || !Bool)
			SetDlgItemSpin( hDlg, IDC_HUE_VALUE, TOANGLE(delta), YES);
	}
	Hue_SetPatch(IDC_HUE1, m_nOutHue );
	Hue_SetPatch(IDC_INHUE1, m_nInHue );
	m_HueMap.bModified = YES; // to force HSL mapping
}


void CHueShiftDlg::HueShift_Make_Map()

{
	int dx;
	
	if (m_bColorize)
	{
		m_HueMap.Points = 2;
		m_HueMap.Pnt[0].x = 0;
		m_HueMap.Pnt[1].x = 255;
		m_HueMap.Pnt[0].y = m_HueMap.Pnt[1].y = m_nOutHue;
		return;
	}
	
	dx = m_nOutHue - m_nInHue;
	// find x=0
	m_HueMap.Pnt[0].x = 0;
	m_HueMap.Pnt[0].y = (MAX_HUES+dx) % MAX_HUES;
	// find x=MAX_HUES-1;
	m_HueMap.Pnt[3].x = MAX_HUES-1;
	m_HueMap.Pnt[3].y = (MAX_HUES-1+dx) % MAX_HUES;
	// find crossover point
	m_HueMap.Pnt[1].x = (MAX_HUES-dx) % MAX_HUES;
	m_HueMap.Pnt[1].y = MAX_HUES-1;
	m_HueMap.Pnt[2].x = (MAX_HUES-dx) % MAX_HUES;
	m_HueMap.Pnt[2].y = 0;
	m_HueMap.Points = 4;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////  CHueMapDlg  ///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#define HUE_TO_SLIDER(hue) \
	(TOANGLE(hue))
#define SLIDER_TO_HUE(val) \
	(FROMANGLE(val))
#define GET_HUE_INDEX(slot) \
	(slot+1)


CHueMapDlg::CHueMapDlg(CServerView* pView, CWnd* pParent)
:CPPMapDlg(pView, CHueMapDlg::IDD, pParent)
{

}

BOOL CHueMapDlg::PreDoModal(LPVOID lpVoid)
{
	m_bAutoPreview = GetDefaultInt(HUEMAP_AUTOPREVIEW, YES);
	m_HueStart = 0;
	return CPPMapDlg::PreDoModal(lpVoid);

}

BOOL CHueMapDlg::OnInitDialog()
{
	PreDoModal();
	HWND hDlg = GetSafeHwnd();

	RightPopup( hDlg );
	CheckDlgButton(IDC_AUTO, m_bAutoPreview);
    ControlEnable(hDlg, IDC_PREVIEW, !m_bAutoPreview);
	ResetMap( &m_HueMap, HUEMAPPOINTS, NO );
	ResetMap( &m_SatMap, MAPPOINTS, NO );
	ResetMapEx( &m_MasterMap, MAPPOINTS, NO, YES);
	SetMapGlobals();
	Hue_ResetControls();
	return CPPMapDlg::OnInitDialog();

}

void CHueMapDlg::OnOK()
{
	if ( !GetImage() )
		CPPMapDlg::OnOK();
	
	UndoMapPreview();
	map_to_mapinfo(&m_HueMap, &m_Parms.HueInfo);
	m_Parms.Saturation = m_SatMap.Brightness[0];
	m_Parms.Brightness = m_MasterMap.Brightness[0];
	ResetAllMaps();
	SetMapGlobals();
	AnimateMap(NULL, YES);
	PostDoModal();
	CPPMapDlg::OnOK();

}

void CHueMapDlg::OnCancel()
{
	ResetAllMaps();
	SetMapGlobals();
	AnimateMap(NULL, YES);
	UndoMapPreview();
	CPPMapDlg::OnCancel();

}

LPVOID CHueMapDlg::PostDoModal()
{
	PutDefaultInt(HUEMAP_AUTOPREVIEW, m_bAutoPreview);
	return CPPMapDlg::PostDoModal();
}

BOOL CHueMapDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int id = LOWORD(wParam);
	WORD codeNotify = HIWORD(wParam);
	HWND hControl = (HWND) lParam;
	HWND hDlg = GetSafeHwnd();
	
	BOOL Bool;
	int i, iSlot;
	LPIMAGE lpImage = GetImage();
		
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
			if ( iSlot >= m_HueMap.Points-2 )
				break;
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			i = HandleSlide( hDlg, id, codeNotify, &Bool );
			// get the true hue
			i = SLIDER_TO_HUE(i);
			if ( m_HueMap.Pnt[GET_HUE_INDEX(iSlot)].y == i )
				break;
			m_HueMap.Pnt[GET_HUE_INDEX(iSlot)].y = i;
			Hue_Fix_Map();
			Hue_SetPatch(IDC_HUE1+iSlot, i, TRUE );
			SetMapGlobals();
			AutoPreview(&m_HueMap, m_bAutoPreview, Bool);
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
			if ( iSlot >= m_HueMap.Points-2 )
				break;
			Hue_Reset(iSlot );
			SetMapGlobals();
			AutoPreview(&m_HueMap, m_bAutoPreview, YES);
		break;
	
		case IDC_AUTO:
			m_bAutoPreview= !m_bAutoPreview;
			CheckDlgButton(id, m_bAutoPreview);
            ControlEnable(hDlg, IDC_PREVIEW, !m_bAutoPreview);
			SetMapGlobals();
		   	AutoPreview(&m_HueMap, m_bAutoPreview, YES);
		break;
	
		case IDC_SATURATION:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			i = (int)GetDlgItemSpin( hDlg, IDC_SATURATION, &Bool, YES );
			if ( m_SatMap.Brightness[0] != -i )
			{
				m_SatMap.Brightness[0] = -i;
				SetSlide( hDlg, IDC_SATURATION_SCROLL, i );
				SetMapGlobals();
				AutoPreview(&m_SatMap, m_bAutoPreview, NO);
			}
		break;
	
		case IDC_SATURATION_SCROLL:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			i = HandleSlide( hDlg, id, codeNotify, &Bool );
			if ( m_SatMap.Brightness[0] != -i )
			{
				m_SatMap.Brightness[0] = -i;
				SetDlgItemSpin( hDlg, IDC_SATURATION, i, YES );
				SetMapGlobals();
				AutoPreview(&m_SatMap, m_bAutoPreview, Bool);
			}
		break;
	
		case IDC_LIGHTNESS:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			i = (int)GetDlgItemSpin( hDlg, IDC_LIGHTNESS, &Bool, YES );
			if ( m_MasterMap.Brightness[0] != -i )
			{
				m_MasterMap.Brightness[0] = -i;
				SetSlide( hDlg, IDC_LIGHTNESS_SCROLL, i );
				SetMapGlobals();
				AutoPreview(&m_MasterMap, m_bAutoPreview, NO);
			}
		break;
	
		case IDC_LIGHTNESS_SCROLL:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			i = HandleSlide( hDlg, id, codeNotify, &Bool );
			if ( m_MasterMap.Brightness[0] != -i )
			{
				m_MasterMap.Brightness[0] = -i;
				SetDlgItemSpin( hDlg, IDC_LIGHTNESS, i, YES );
				SetMapGlobals();
				AutoPreview(&m_MasterMap, m_bAutoPreview, Bool);
			}
		break;
	
		case IDC_PREVIEW:
			AutoPreview_Button(&m_HueMap);
		break;
	
		case IDC_RESETALL:
		case IDC_RESET:
			ResetMap( &m_HueMap, HUEMAPPOINTS, NO );
			ResetMap( &m_SatMap, MAPPOINTS, NO );
			ResetMapEx( &m_MasterMap, MAPPOINTS, NO, YES);
			Hue_ResetControls();
			MakeMap(&m_SatMap);
			MakeMap(&m_MasterMap);
			SetMapGlobals();
			AutoPreview(&m_HueMap, m_bAutoPreview, YES);
		break;
	
	
	   default:
		break;
	   }
return CWnd::OnCommand(wParam, lParam);}

BEGIN_MESSAGE_MAP(CHueMapDlg, CPPMapDlg)
	ON_WM_DESTROY()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CHueMapDlg::OnDestroy()
{
	AutoPreview_OnDestroy();
	Probe_End();

}



BOOL CHueMapDlg::OnQueryNewPalette()
{
	return Dialog_OnQueryNewPalette(GetSafeHwnd());
}

void CHueMapDlg::OnTimer(UINT nIDEvent)
{
	if (!AutoPreview_OnTimer(nIDEvent))
		return;

	// got good timer
	MakeMap(&m_HueMap);
	MakeMap(&m_SatMap);
	MakeMap(&m_MasterMap);
	SetMapGlobals();
	MapPreview();

}

/////////////////////////////////////////CHueMapDlg Methods////////////////////////////////////////////

//CLEAN
void CHueMapDlg::Hue_ResetControls()
{
int i;
HWND hDlg = GetSafeHwnd();

InitDlgItemSpin( hDlg, IDC_SATURATION, -m_SatMap.Brightness[0], YES, -100, 100 );
InitSlide( hDlg, IDC_SATURATION_SCROLL, -m_SatMap.Brightness[0], -100, 100 );
InitDlgItemSpin( hDlg, IDC_LIGHTNESS, m_MasterMap.Brightness[0], YES, -100, 100 );
InitSlide( hDlg, IDC_LIGHTNESS_SCROLL, m_MasterMap.Brightness[0], -100, 100 );
for ( i=0; i<m_HueMap.Points-2; i++ )
	Hue_Reset(i );
m_HueMap.bModified = YES; // to force HSL mapping
}


//CLEAN
void CHueMapDlg::Hue_Reset(int slot )
{
int hue;
int index;
HWND hDlg = GetSafeHwnd();

index = GET_HUE_INDEX(slot);
hue = FMUL( (index-1), FGET((MAX_HUES-1), m_HueMap.Points-3 ));
hue += m_HueStart;
hue = hue % MAX_HUES;
m_HueMap.Pnt[index].x = hue;
m_HueMap.Pnt[index].y = hue;
Hue_Fix_Map();
InitSlide( hDlg, IDC_SLIDEHUE1+slot, HUE_TO_SLIDER(hue), 0, 360 );
Hue_SetPatch(IDC_HUE1+slot, hue , TRUE);
Hue_SetPatch(IDC_INHUE1+slot, hue, TRUE );
}


//CLEAN
void CHueMapDlg::Hue_Fix_Map()
{
	m_HueMap.Pnt[m_HueMap.Points-1].x = MAX_HUES-1;
	m_HueMap.Pnt[m_HueMap.Points-1].y = m_HueMap.Pnt[m_HueMap.Points-2].y;
	m_HueMap.Pnt[0].x = 0;
	m_HueMap.Pnt[0].y = m_HueMap.Pnt[1].y;     

}






//	Call when the calling dlg wants to change the posion of the visual
//		map control. Only those image which need to be re-calculated 
//		will be.
BOOL CPPVisualMapDlg::MoveVisual(int x, int y)
{
	CVisual* lpVisual = GetVisualPtr();
	HWND hDlg = GetSafeHwnd();
	int i;
	if (x == VISUAL_BEFORE || !WITHIN(x,-1,1) || !WITHIN(y, -1, 1))
		return(SetVisual(x, y));
	switch(lpVisual->m_wMoveType)
	{
		case 1:
			return(Move2DDirectional(x, y));
		case 2:
			return(MoveCenterOnly(x, y));
	}
	// assume update all 
	AstralCursor(IDC_WAIT);
	if (!(lpVisual->MoveVisual)(x,y))
	{
		AstralCursor(NULL);
		return(FALSE);
	}
	for (i=IDC_VISUAL_00; i<=IDC_VISUAL_22; i++)
		UpdateVisual(i);
	AstralCursor(NULL);
	return(TRUE);
	
}


//	Call when the calling dlg wants to set/reset the visual
//		map control. Will redraw all image previews.
BOOL CPPVisualMapDlg::SetVisual(int x, int y)
{
	CVisual* lpVisual = GetVisualPtr();
	int i;
	
	AstralCursor(IDC_WAIT);
	if ((lpVisual->MoveVisual)(x,y)==NULL)
	{
		AstralCursor(NULL);
		return(FALSE);
	}
	for (i=IDC_VISUAL_BEFORE; i<=IDC_VISUAL_22; i++)
		UpdateVisual(i);
	AstralCursor(NULL);
	return(TRUE);
}




//	Returns true if sucessful
BOOL CPPVisualMapDlg::GetPosFromID(ITEMID id, LPINT lpx, LPINT lpy)   
{

	switch (id)
	{
		case IDC_VISUAL_BEFORE:
			*lpx = *lpy = VISUAL_BEFORE;
		break;
		case IDC_VISUAL_00:	
		case IDC_VISUAL_01:	
		case IDC_VISUAL_02:
			*lpx = id-IDC_VISUAL_01;
			*lpy = -1;
		break;
		case IDC_VISUAL_10:	
		case IDC_VISUAL_11:	
		case IDC_VISUAL_12:
			*lpx = id-IDC_VISUAL_11;
			*lpy = 0;
		break;
		case IDC_VISUAL_20:	
		case IDC_VISUAL_21:	
		case IDC_VISUAL_22:
			*lpx = id-IDC_VISUAL_21;
			*lpy = 1;
		break;
		default:
			return(FALSE);
	}
	return(TRUE);
}		


//	Returns 0 if failure
int CPPVisualMapDlg::GetIDFromPos(int x, int y)
{
	if (x==VISUAL_BEFORE)
		return(IDC_VISUAL_BEFORE);
	if (x<-1 || x>1)
		return(0);
	switch (y)
	{
		break;
		case -1:	
			return(IDC_VISUAL_01+x);
		case 0:	
			return(IDC_VISUAL_11+x);
		case 1:	
			return(IDC_VISUAL_21+x);
		default:
			return(0);
	}
	return(0);
}		



//	Sets up the extra frames 
void CPPVisualMapDlg::SetupFrames()
{
	LFIXED 	rate, line;
	LPTR	lpSrc, lpDst;
	int 	yread;
	int 	x,y,depth, lin, pix, id, width;
	HWND 	hControl;
	LONG 	size;
	BOOL 	fUseImage;
	FRMTYPEINFO TypeInfo;
	LPTR lpLineBuf;
	LPIMAGE lpImage;
	HWND hDlg = GetSafeHwnd();
	CVisual* lpVisual = GetVisualPtr();
	lpImage = GetImage();

	// set lpVisual for all windows
	for (id = IDC_VISUAL_BEFORE; id <= IDC_VISUAL_22; id++)
		if (hControl=::GetDlgItem(hDlg, id))
			SetWindowLong( hControl, GWL_LPVISUAL,(LONG)lpVisual);
	// there may be 2 center panes 
	if (hControl = GetExtraWindow(hDlg,IDC_VISUAL_11,2))
		SetWindowLong( hControl, GWL_LPVISUAL,(LONG)lpVisual);
		
	// Check if frames setup
	fUseImage = (lpVisual->m_lpImage != NULL);
	if (lpVisual->m_lpOrgFrame && !fUseImage)
	{
		lpVisual->m_lpBasisFrame=NULL;
		return;
	}
	if (fUseImage)
		lpVisual->m_lpOrgFrame = ImgGetBaseEditFrame(lpVisual->m_lpImage);
	size = 0;
	for (id = IDC_VISUAL_BEFORE; id <= IDC_VISUAL_22; id++)
		if (hControl=::GetDlgItem(hDlg, id))
			if (size = ::SendMessage(hControl, WM_IMAGESPACE, 0, 0L))
				break;
	if (!size)
		return;
	pix = Max(LOWORD(size) - 10, 1);
	lin = Max(HIWORD(size) - 10, 1);
	
	TypeInfo = FrameTypeInfo(lpVisual->m_lpOrgFrame);
	depth = FrameDepth(lpVisual->m_lpOrgFrame);
	width = FrameXSize(lpVisual->m_lpOrgFrame);
	if (!lpVisual->m_lpBasisFrame)
	{
		// Create basis frame
		x = FrameXSize(lpVisual->m_lpOrgFrame);
		y = FrameYSize(lpVisual->m_lpOrgFrame);
		ScaleToFit(&pix, &lin, x,y);
		lin = Max(1, lin);
		pix = Max(1, pix);	
		rate = FGET(x, pix);
		// get new frame 
		lpVisual->m_lpBasisFrame = FrameOpen(TypeInfo, 
				   		pix, 
				   		lin,
				   		FrameResolution(lpVisual->m_lpOrgFrame));
		if (!lpVisual->m_lpBasisFrame)
			return;
		// setup buffers for frame sampling
		lpLineBuf=NULL;
		if (fUseImage)
			if (!(lpSrc=lpLineBuf=Alloc(depth*width)))
				fUseImage = FALSE;
		// sample frame
		line = 0L;
		yread = 0;
		for (y=0;y<lin;y++)
		{
			yread = WHOLE(line);
			lpDst = FramePointer(lpVisual->m_lpBasisFrame, 0,y,YES);
			if (fUseImage)
				ImgGetLine(lpImage,NULL,0,yread,width,lpSrc);
			else
				lpSrc = FramePointer(lpVisual->m_lpOrgFrame, 0,yread,NO);
			if (!lpSrc || !lpDst)
			{
				FrameClose(lpVisual->m_lpBasisFrame);
				lpVisual->m_lpBasisFrame=NULL;
				return;
			}
			FrameSample(lpVisual->m_lpBasisFrame, lpSrc, 0, lpDst, 0, pix, rate);
			line += rate; 
		} 	// for y
		// free up buffers 
		if (lpLineBuf) FreeUp(lpLineBuf);
	} // if !lpBasisFrame
	// make copy for lpWorkFrame
	lpVisual->m_lpWorkFrame = FrameOpen(
					TypeInfo, 
					pix, 
					lin,
				   	FrameResolution(lpVisual->m_lpOrgFrame));
}


//	Call when the calling dlg wants to change the posion of the visual
//		map control. Only those image which need to be re-calculated 
//		will be. Moves images like a 2-d joystick.
BOOL CPPVisualMapDlg::Move2DDirectional(int x, int y)
{
	HWND hDlg = GetSafeHwnd();
	CVisual* lpVisual = GetVisualPtr();
	int id,dx, dy,xpos, ypos, xstart;
	
	if (x == VISUAL_BEFORE || !WITHIN(x,-1,1) || !WITHIN(y, -1, 1))
		return(SetVisual(x, y));
	AstralCursor(IDC_WAIT);
	if ((lpVisual->MoveVisual)(x,y)==NULL)
	{
		AstralCursor(NULL);
		return(FALSE);
	}
	if (!x && !y)
	{
		AstralCursor(NULL);
		return(TRUE);
	}
	xstart = xpos = x ? -x:-1;
	ypos = y ? -y:-1;
	dx = -xpos;
	dy = -ypos;
	for (;WITHIN(ypos,-1,1); ypos+=dy)
		for (xpos = xstart;WITHIN(xpos,-1,1); xpos+=dx)
		{
			if (!(id = GetIDFromPos(xpos,ypos)))
				continue;
			// draw the new window
			UpdateVisual(id);
		}  // for x pos	
	AstralCursor(NULL);
	return(TRUE);
}

//	Call when the calling dlg wants to change the posion of the visual
//		map control. The selected image will become the center image.
//		All other images will be re-calculated.

BOOL CPPVisualMapDlg::MoveCenterOnly(int x, int y)
{
	int id, idDone;
	HWND hDlg = GetSafeHwnd();
	CVisual* lpVisual = GetVisualPtr();
	
	if (x == VISUAL_BEFORE || !WITHIN(x,-1,1) || !WITHIN(y, -1, 1))
		return(SetVisual(x, y));
	AstralCursor(IDC_WAIT);
	if ((lpVisual->MoveVisual)(x,y)==NULL)
	{
		AstralCursor(NULL);
		return(FALSE);
	}
	// swap selected with center
	idDone = 0;
	for (id=IDC_VISUAL_00; id<=IDC_VISUAL_22; id++)
		UpdateVisual(id);
	AstralCursor(NULL);
	return(TRUE);
	
}

//	Gets the n'th (if any) child window with the given id.
//	Returns NULL for failure.

HWND CPPVisualMapDlg::GetExtraWindow(HWND hParent, int id, int n)
{
	HWND hWnd;
	
	hWnd = ::GetDlgItem(hParent, id);
	n--;
	while (n)
	{
		hWnd = ::GetNextWindow(hWnd, GW_HWNDNEXT);
		if (!hWnd)
			break;
		if (::GetDlgCtrlID(hWnd) == id)
			n--;
	}
	return(hWnd);
}

//	Updates the visual element with given id  in the dlg.
void CPPVisualMapDlg::UpdateVisual(int id)
{
	HWND hWnd;
	int n;
	
	n = 1;
	if (id == IDC_VISUAL_11)
		n =2;
	while (n)
	{
		hWnd = GetExtraWindow(GetSafeHwnd(),id,n--);
		if (hWnd)
		{
			::InvalidateRect(hWnd, NULL, YES);
			::UpdateWindow(hWnd);
		}
	}
}


CPPVisualMapDlg::CPPVisualMapDlg (CServerView *pView, UINT nIDTemplate, 
		CWnd* pParentWnd) : CPPMapDlg (pView, nIDTemplate, pParentWnd)
{

}

BEGIN_MESSAGE_MAP(CPPVisualMapDlg, CPPMapDlg)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


BOOL CPPVisualMapDlg::OnInitDialog()
{
HWND hDlg = GetSafeHwnd();
CVisual* lpVisual = GetVisualPtr();

lpVisual->m_lpBasisFrame=NULL;
SetupFrames();
SetDlgItemIntExt(hDlg,IDC_VSTEP_TEXT,lpVisual->m_nStep," %");
InitSlide(hDlg, IDC_VSTEP, lpVisual->m_nStep, 1, 25);
SetVisual(0, 0);
return CPPMapDlg::OnInitDialog();
}

BOOL CPPVisualMapDlg::OnCommandEx(WPARAM wParam, LPARAM lParam)
{
	int x, y;
	HWND hDlg = GetSafeHwnd();
	int id = LOWORD(wParam);
	WORD codeNotify = HIWORD(wParam);
	HWND hControl = (HWND) lParam;
	CVisual* lpVisual = GetVisualPtr();
	SetMapGlobals();

switch (id)
	{
	case IDC_VSTEP:
   	lpVisual->m_nStep = HandleSlide (hDlg,id,codeNotify,NULL);
   	SetDlgItemIntExt(hDlg,IDC_VSTEP_TEXT,lpVisual->m_nStep," %");
   	SetSlide (hDlg,id,lpVisual->m_nStep);
		if (codeNotify == SN_VALUESELECTED)
			SetVisual(0, 0);
	break;
	case IDC_RESET:
		id = IDC_VISUAL_BEFORE;
	// Fall through.
	case IDC_VISUAL_BEFORE:
	case IDC_VISUAL_00:	
	case IDC_VISUAL_01:	
	case IDC_VISUAL_02:
	case IDC_VISUAL_10:	
	case IDC_VISUAL_11:	
	case IDC_VISUAL_12:
	case IDC_VISUAL_20:	
	case IDC_VISUAL_21:	
	case IDC_VISUAL_22:
		if (GetPosFromID(id, &x,&y))
			//(lpVisual->MoveVisual)(x,y);
			  MoveVisual(x,y);
	break;
   default:
	break;
   }
return TRUE;}

void CPPVisualMapDlg::OnDestroy()
{
CVisual* lpVisual = GetVisualPtr();
if (lpVisual->m_lpBasisFrame)
	{
	FrameClose(lpVisual->m_lpBasisFrame);
	lpVisual->m_lpBasisFrame = NULL;
	}
if (lpVisual->m_lpWorkFrame)
	{
	FrameClose(lpVisual->m_lpWorkFrame);
	lpVisual->m_lpWorkFrame = NULL;
	}
}

void CPPVisualMapDlg::OnCancel()
{
	ResetAllMaps();
	UndoMapPreview();
	CPPMapDlg::OnCancel();
}

void CPPVisualMapDlg::OnOK()
{
CPPMapDlg::OnOK();
}



//BOOL CPPMapDlg::Previewing()
//{
//	return(TRUE);
////	return(GetImageHook(GetViewHwnd()) == MapPreviewProcEx);
//}



/************************************************************************/

//	If bapplyMToK = TRUE then the Master map will be applied to the K Map
// 	If bapplyMToK = -1 then the state wont change.
//  Currently bapplyMToK is only hooked up in the preview stuff.
void CPPMapDlg::ResetAllMaps(BOOL bapplyMToK)
{
	if (bapplyMToK >=0)
		m_bApplyMToK = bapplyMToK;
	ResetMapEx( &m_MasterMap, MAPPOINTS, NO, YES);
	ResetMap( &m_RedMap,    MAPPOINTS, NO ); // sometimes Cyan
	ResetMap( &m_GreenMap,  MAPPOINTS, NO ); // sometimes Magenta
	ResetMap( &m_BlueMap,   MAPPOINTS, NO ); // sometimes Yellow
	ResetMap( &m_BlackMap,   MAPPOINTS, NO );
	ResetMap( &m_HueMap,    HUEMAPPOINTS, NO );
	ResetMap( &m_SatMap,    MAPPOINTS, NO );
	SetMapGlobals();
}

/************************************************************************/
//	If bapplyMToK = TRUE then the Master map will be applied to the K Map
// 	If bapplyMToK = -1 then the state wont change.
//  Currently bapplyMToK is only hooked up in the preview stuff.
/************************************************************************/
void CPPMapDlg::ResetAllMapsEx(int type, BOOL bapplyMToK)
{
	if (bapplyMToK >=0)
		m_bApplyMToK = bapplyMToK;
	ResetMapEx( &m_MasterMap, m_nPoints, type, YES );
	m_MasterMap.iCurves = m_bCurves ? m_nCurveType:0;
	MakeMap( &m_MasterMap);
	ResetMap( &m_RedMap, m_nPoints, type );
	m_RedMap.iCurves = m_bCurves ? m_nCurveType:0;
	MakeMap( &m_RedMap);
	ResetMap( &m_GreenMap, m_nPoints, type );
	m_GreenMap.iCurves = m_bCurves ? m_nCurveType:0;
	MakeMap( &m_GreenMap);
	ResetMap( &m_BlueMap, m_nPoints, type );
	m_BlueMap.iCurves = m_bCurves ? m_nCurveType:0;
	MakeMap( &m_BlueMap);
	ResetMap( &m_BlackMap, m_nPoints, type );
	m_BlackMap.iCurves = m_bCurves ? m_nCurveType:0;
	MakeMap( &m_BlackMap);
	ResetMap( &m_HueMap,    HUEMAPPOINTS, type );
	ResetMap( &m_SatMap,    MAPPOINTS, type );
	SetMapGlobals();
}

void CPPMapDlg::ResetBandAllMaps(WORD wBand)
{
	m_MasterMap.Brightness[wBand] = 0;
	m_RedMap.Brightness[wBand] = 0;
	m_GreenMap.Brightness[wBand] = 0;
	m_BlueMap.Brightness[wBand] = 0;
	m_BlackMap.Brightness[wBand] = 0;
	m_HueMap.Brightness[wBand] = 0;
	m_SatMap.Brightness[wBand] = 0;
	MakeMap(&m_MasterMap);
	m_MasterMap.bModified = NO;
	MakeMap(&m_BlueMap);
	m_BlueMap.bModified = NO;
	MakeMap(&m_BlackMap);
	m_BlackMap.bModified = NO;
	MakeMap(&m_GreenMap);
	m_GreenMap.bModified = NO;
	MakeMap(&m_RedMap);
	m_RedMap.bModified = NO;
	MakeMap(&m_HueMap);
	m_HueMap.bModified = NO;
	MakeMap(&m_SatMap);
	m_SatMap.bModified = NO;
	
}


//************************************************************************=
MAP* CPPMapDlg::ActivateChannel( WORD wChannel )
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
			lpMap = &m_MasterMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = NO;
			lpMap->Reverse = NO;  // master is always reversed
			lpMap->Reverse |= ReverseLUTXY;
		break;

		case IDC_DO_CYAN:
			lpMap = &m_RedMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = !lpMap->Reverse;
			lpMap->Reverse = YES;
			lpMap->Reverse |= ReverseLUTXY;
		break;

		case IDC_DO_MAGENTA:
			lpMap = &m_GreenMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = !lpMap->Reverse;
			lpMap->Reverse = YES;
			lpMap->Reverse |= ReverseLUTXY;
		break;

		case IDC_DO_YELLOW:
			lpMap = &m_BlueMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = !lpMap->Reverse;
			lpMap->Reverse = YES;
			lpMap->Reverse |= ReverseLUTXY;
		break;

		case IDC_DO_RED:
			lpMap = &m_RedMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = lpMap->Reverse;
			lpMap->Reverse = NO;
			lpMap->Reverse |= ReverseLUTXY;
		break;

		case IDC_DO_GREEN:
			lpMap = &m_GreenMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = lpMap->Reverse;
			lpMap->Reverse = NO;
			lpMap->Reverse |= ReverseLUTXY;
		break;

		case IDC_DO_BLUE:
			lpMap = &m_BlueMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = lpMap->Reverse;
			lpMap->Reverse = NO;
			lpMap->Reverse |= ReverseLUTXY;
		break;

		case IDC_DO_BLACK:
			lpMap = &m_BlackMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = NO;
			lpMap->Reverse = NO;
			lpMap->Reverse |= ReverseLUTXY;
		break;

		case IDC_CAL_BLACK:
			lpMap = &m_BlackMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = NO;
			lpMap->Reverse = NO;
			lpMap->Reverse |= ReverseLUTXY;
		break;

		case IDC_CAL_CYAN:
			lpMap = &m_RedMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = NO;
			lpMap->Reverse = NO;
			lpMap->Reverse |= ReverseLUTXY;
		break;

		case IDC_CAL_MAGENTA:
			lpMap = &m_GreenMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = NO;
			lpMap->Reverse = NO;
			lpMap->Reverse |= ReverseLUTXY;
		break;

		case IDC_CAL_YELLOW:
			lpMap = &m_BlueMap;
			ReverseLUTXY = lpMap->Reverse & REVERSE_LUTXY;
			lpMap->Reverse &= ~REVERSE_LUTXY;
			bReverse = NO;
			lpMap->Reverse = NO;
			lpMap->Reverse |= ReverseLUTXY;
		break;

		default:
			lpMap = &m_MasterMap;
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

void CPPMapDlg::SetMapGlobals()
{
//	HueMap = m_HueMap;
//	SatMap = m_SatMap;
//	MasterMap = m_MasterMap;
//	RedMap = m_RedMap;
//	GreenMap = m_GreenMap;
//	BlueMap = m_BlueMap;
//	BlackMap = m_BlackMap;
//	IsInPreview = m_bIsInPreview;
return;
}

//					int x- The x value (map coords) to display, -1 for none.
//					int y- The x value (map coords) to display, -1 for none.
// DESCRIPTION:
//				Displays the input and output values for the mapping dialog.
//************************************************************************
VOID CPPMapDlg::DisplayInOutValues(int x, int y)
//************************************************************************
{
	HWND hDlg = GetSafeHwnd();
	if (x >= 0)
	{
		x = MAP_TO_DISPLAY( x );
		SetDlgItemIntExt( hDlg,
			IDC_MAPIN,  x, m_bPercentages ? (LPSTR)"%":(LPSTR)"");
	}
	else
		SetDlgItemText(IDC_MAPIN, "");
	if (y >= 0)
	{
		y = MAP_TO_DISPLAY( y );
		SetDlgItemIntExt( hDlg,
			IDC_MAPOUT,  y, m_bPercentages ? (LPSTR)"%":(LPSTR)"");
	}
	else
		SetDlgItemText(IDC_MAPOUT, "");
}

BOOL CPPMapDlg::Probe_IsProbing()
{
	return(m_bProbing);
}

// RETURNS:
//		TRUE if the given channel is available in the channels combo.
//		Must setup with ChannelsCombo_Setup. 
BOOL CPPMapDlg::ChannelsCombo_IsEnabled(WORD wChannel)
{
	int i;
	for (i=0;i<m_nCombo;i++)
	{
		if(m_iCombo[i] == wChannel)
			return(TRUE);
	}
	return(FALSE);
}

/************************************************************************\
	Same as setup luts, exept it gets the packed lut from lpLuts.
	if lpLuts = NULL, then clears luts
\************************************************************************/

void CPPMapDlg::MapSetPackedLut(LPTR lpLuts)
{
	if (m_lpHLut && m_fMyLuts)		 
	{
		FreeUp(m_lpHLut);
		m_lpHLut = NULL;
	}
	m_lpHLut = lpLuts;
	if (!m_lpHLut)
		return;
	m_fMyLuts = FALSE;	
	m_lpSLut = m_lpHLut + 256;
	m_lpMLut = m_lpSLut + 256;
	m_lpRLut = m_lpMLut + 256;
	m_lpGLut = m_lpRLut + 256;
	m_lpBLut = m_lpGLut + 256;
	m_lpKLut = m_lpBLut + 256;
}

BOOL CPPMapDlg::MapSetupLuts( LPTR HLut, LPTR SLut, LPTR MLut, LPTR RLut, LPTR GLut, LPTR BLut, LPTR KLut )
{
	if (!HLut)
	{
		if (m_lpHLut && m_fMyLuts)
		{
			FreeUp(m_lpHLut);
			m_lpHLut = NULL;
		}
		return(TRUE);
	}
	else
	{
		if (!m_lpHLut || !m_fMyLuts)
		{
			m_lpHLut = Alloc(256L*7L);
			if (!m_lpHLut)
				return(FALSE);
			m_fMyLuts = TRUE;
			m_lpSLut = m_lpHLut + 256;
			m_lpMLut = m_lpSLut + 256;
			m_lpRLut = m_lpMLut + 256;
			m_lpGLut = m_lpRLut + 256;
			m_lpBLut = m_lpGLut + 256;
			m_lpKLut = m_lpBLut + 256;
		}
		copy(HLut, m_lpHLut, 256);
		copy(SLut, m_lpSLut, 256);
		copy(MLut, m_lpMLut, 256);
		copy(RLut, m_lpRLut, 256);
		copy(GLut, m_lpGLut, 256);
		copy(BLut, m_lpBLut, 256);
		copy(KLut, m_lpKLut, 256);
		return(TRUE);
	}
}

LPTR CPPMapDlg::MapGetPackedLut(LPTR lpDst,LPTR HLut,LPTR SLut,LPTR MLut,LPTR RLut,LPTR GLut,LPTR BLut, LPTR KLut)
{
	LPTR lpTmpHLut, lpTmpSLut, lpTmpMLut, lpTmpRLut, lpTmpGLut, lpTmpBLut, lpTmpKLut;
 	
 	lpTmpHLut = lpDst;
 	lpTmpSLut = lpTmpHLut + 256;
 	lpTmpMLut = lpTmpSLut + 256;
 	lpTmpRLut = lpTmpMLut + 256;
 	lpTmpGLut = lpTmpRLut + 256;
 	lpTmpBLut = lpTmpGLut + 256;
 	lpTmpKLut = lpTmpBLut + 256;
	copy(HLut, lpTmpHLut, 256);
	copy(SLut, lpTmpSLut, 256);
	copy(MLut, lpTmpMLut, 256);
	copy(RLut, lpTmpRLut, 256);
	copy(GLut, lpTmpGLut, 256);
	copy(BLut, lpTmpBLut, 256);
	copy(KLut, lpTmpKLut, 256);
	return(lpTmpHLut);
}

void CPPMapDlg::Map8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int dx)
{
	LPPROCESSPROC lpProcessProc;

	if (lpMsk)
		copy(lpDst, lpSrc, dx);
	else
		lpSrc = lpDst;

	// do actual mapping
	if (!m_HueMap.bModified && !m_SatMap.bModified && m_lpHLut)
		MapLine8by2(m_lpMLut, m_lpGLut, lpSrc, dx);
	else if (m_lpHLut)
		MapLine8(m_lpMLut, lpSrc, dx);
	if (lpMsk)
	{
		lpProcessProc = GetProcessProc(MM_NORMAL, FDT_GRAYSCALE);
		(*lpProcessProc)(lpDst, lpSrc, lpMsk, dx);
	}
}

/************************************************************************\
 	Displays the given portion of the frame at the given location.
 	DOES NO DOWNSIZING
	Applys the given luts to the image when displaying.
 	lpOutBuffer should be NULL or should be large enough for one full line;
 	Should setup Luts before calling
\************************************************************************/

int CPPMapDlg::DisplayMappedImage(
									HDC     hDC,
									LPFRAME lpFrame,
									LPRECT  lpSourceRect,
									POINT   destPoint,
									LPTR    lpOutBuffer,
									int     xDiva,
									int     yDiva,
									LPVOID  lpCmsXform)
{

	int y, count, depth;
	RECT rSource, rDest;
	LPTR lpLine;
	BLTSESSION BltSession;  
	FRMTYPEINFO TypeInfo, DisplayTypeInfo;
	CFrameTypeConvert TypeConvert;
	
	rSource      = *lpSourceRect;
	rDest.left   = destPoint.x;
	rDest.top    = destPoint.y;
	rDest.right  = rDest.left+(rSource.right -rSource.left);
	rDest.bottom = rDest.top +(rSource.bottom-rSource.top);
	count = RectWidth(&rDest);

	depth = FrameDepth(lpFrame);
	if (depth == 0) depth = 1;
	FrameGetTypeInfo(lpFrame, &TypeInfo);
	if (TypeInfo.DataType == FDT_PALETTECOLOR)
		{
		depth = 3;
		FrameSetTypeInfo(&DisplayTypeInfo, FDT_RGBCOLOR, NULL);
		if (!TypeConvert.Init(TypeInfo, DisplayTypeInfo, count))
			return(FALSE);
		}
	else
		DisplayTypeInfo = TypeInfo;

	if (!lpOutBuffer)
		if (!(lpOutBuffer = Alloc((long)RectWidth( &rSource )* depth)) )
			return(FALSE);
	
	StartSuperBlt(
		&BltSession,
		hDC,
		NULL,
		lpBltScreen,
		&rDest,
		DisplayTypeInfo,
		PRLINES,
		xDiva,
		yDiva,
		YES,
		lpCmsXform,
		lpKCMSEvalProc );

	for ( y=rSource.top; y<=rSource.bottom; y++ )
	{
		if (lpLine = FramePointer(lpFrame, rSource.left, y, NO))
		{
			switch(TypeInfo.DataType)
			{
				case FDT_LINEART:
				case FDT_GRAYSCALE:
					copy(lpLine,lpOutBuffer,count);  
					Map8(lpOutBuffer,NULL,NULL,count);
				break;

				case FDT_PALETTECOLOR:
					TypeConvert.ConvertData(lpLine, lpOutBuffer, y, count);
					Map24(lpOutBuffer,NULL,NULL,count);
				break;

				case FDT_RGBCOLOR:
					copy(lpLine,lpOutBuffer,count*depth);  
					Map24(lpOutBuffer,NULL,NULL,count);
				break;

				case FDT_CMYKCOLOR:
					copy(lpLine,lpOutBuffer,count*depth);  
					Map32(lpOutBuffer,NULL,NULL,count);
				break;

				default:
					map_proc(
						y,
						rSource.left,
						rSource.right,
						lpLine,
						lpOutBuffer,
						depth, NULL);
				break;
			}
		}
		SuperBlt( &BltSession, lpOutBuffer );
	}

	SuperBlt( &BltSession, NULL );

	return( TRUE );
}

void CPPMapDlg::Map24(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int dx)
{
	LPPROCESSPROC lpProcessProc;

	if (lpMsk)
		copy(lpDst, lpSrc, dx*3);
	else
		lpSrc = lpDst;

	// do actual mapping
	if (!m_HueMap.bModified && !m_SatMap.bModified && m_lpHLut)
		MapLine24(m_lpMLut,m_lpRLut,m_lpGLut,m_lpBLut,lpSrc,dx);
	else if (m_lpHLut)
		MapLine24HSL(m_lpHLut,m_lpSLut,m_lpMLut, lpSrc, dx);

	if (lpMsk)
	{
		lpProcessProc = GetProcessProc(MM_NORMAL, FDT_RGBCOLOR);
		(*lpProcessProc)(lpDst, lpSrc, lpMsk, dx);
	}
}

void CPPMapDlg::Map32(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int dx)
{
	LPPROCESSPROC lpProcessProc;

	if (lpMsk)
		copy(lpDst, lpSrc, dx*4);
	else
		lpSrc = lpDst;
	
	// do actual mapping
	if (!m_HueMap.bModified && !m_SatMap.bModified && m_lpHLut)
	{
	 	if(m_bApplyMToK)
			MapLine32MToK(m_lpMLut,m_lpRLut,m_lpGLut,m_lpBLut, m_lpKLut, lpSrc,dx);
		else
			MapLine32(m_lpMLut,m_lpRLut,m_lpGLut,m_lpBLut, m_lpKLut, lpSrc,dx);
	}
	else if (m_lpHLut)
		MapLine32HSL(m_lpHLut,m_lpSLut,m_lpMLut,lpSrc,dx);
	
	if (lpMsk)
	{
		lpProcessProc = GetProcessProc(MM_NORMAL, FDT_CMYKCOLOR);
		(*lpProcessProc)(lpDst, lpSrc, lpMsk, dx);
	}
}

BOOL CPPMapDlg::Previewing()
{
	return(GetImageHook(GetSafeHwnd()) == this);
}

void CPPMapDlg::UndoMapPreview()
{
	RECT rMask;
	LPIMAGE lpImage = GetImage();
	FRMTYPEINFO TypeInfo;
	HWND hWnd = GetViewHwnd();


    if (!hWnd || !lpImage)
        return;

	if (Previewing())
		return;

	ImgGetTypeInfo(lpImage, &TypeInfo);

	// reset the images colormap to its original state
	if ( TypeInfo.DataType == FDT_PALETTECOLOR)
		copy((LPTR)m_PreviewRGBmap, (LPTR)TypeInfo.ColorMap->RGBData, sizeof(m_PreviewRGBmap));

	MapSetupLuts(NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	SetImageHook(hWnd, NULL);

	if (ImgGetMask(lpImage))
		ImgGetMaskRect(lpImage, &rMask);
	else
		ImgGetSelObjectRect(lpImage, &rMask, NO);

	File2DispRect(hWnd, &rMask, &rMask);
	::InvalidateRect(hWnd, &rMask, FALSE);
}

void CPPMapDlg::MapPreview()
{
	RECT rMask;
	LPIMAGE lpImage = GetImage();
	FRMTYPEINFO TypeInfo;
	HWND hWnd = GetViewHwnd();

	ImgGetTypeInfo(lpImage, &TypeInfo);

	// palette color gets treated special for preview
	// we really just remap the palette, we use PreviewRGBmap to
	// save the original colormap of the image
	if ( TypeInfo.DataType == FDT_PALETTECOLOR)
	{
		if (!Previewing())
			copy((LPTR)TypeInfo.ColorMap->RGBData, (LPTR)m_PreviewRGBmap, sizeof(m_PreviewRGBmap));
		else
			copy((LPTR)m_PreviewRGBmap, (LPTR)TypeInfo.ColorMap->RGBData, sizeof(m_PreviewRGBmap));
	}

//	SetImageHook(hWnd, &MapPreviewHook);
	SetImageHook(hWnd, this);
	AnimateMap(NULL, YES); // turn off palette animation

	MapSetupLuts(m_HueMap.Lut, m_SatMap.Lut, m_MasterMap.Lut, m_RedMap.Lut, m_GreenMap.Lut, m_BlueMap.Lut, m_BlackMap.Lut);

	// map the palette colored images colormap
	if (TypeInfo.DataType == FDT_PALETTECOLOR)
	{
		Map24((LPTR)TypeInfo.ColorMap->RGBData, 
			  (LPTR)TypeInfo.ColorMap->RGBData,
			   NULL, TypeInfo.ColorMap->NumEntries);
	}

	if (ImgGetMask(lpImage))
		ImgGetMaskRect(lpImage, &rMask);
	else
		ImgGetSelObjectRect(lpImage, &rMask, NO);

	File2DispRect(hWnd, &rMask, &rMask);
	::InvalidateRect(hWnd, &rMask, FALSE);
}
BOOL CPPMapDlg::AnimateMap(LPMAP lpMap, BOOL fDoGamma)
{
	int i;
	LPPALETTEENTRY lpEntry, lpNewEntry;
	PALETTEENTRY TempEntries[256+1];
	BOOL bPreview;
	LPIMAGE lpImage = GetImage();

	if ( lpMap )
		MakeMap( lpMap );
	
	if ( !lpImage->hPal )
		return( NO );

	if ( lpBltScreen->BitMapInfo.bmiHeader.biBitCount != 8 )
		return( NO );

	if (lpMap) 
	{
		if ( !(bPreview = Previewing()) )
			MapSetupLuts(m_HueMap.Lut, m_SatMap.Lut, m_MasterMap.Lut, m_RedMap.Lut, m_GreenMap.Lut,
						 m_BlueMap.Lut, m_BlackMap.Lut);
		lpEntry = lpImage->Palette;
		lpNewEntry = TempEntries;

		for ( i=0; i<lpImage->nPaletteEntries; i++ )
		{
			*lpNewEntry = *lpEntry;
			if ( !bPreview )
				MapRGB(
					&lpNewEntry->peRed,
					&lpNewEntry->peGreen,
					&lpNewEntry->peBlue );
			lpEntry++;
			lpNewEntry++;
		}
		lpNewEntry = TempEntries;
	}
	else
	{
		// revert to no animation
		lpNewEntry = lpImage->Palette;
	}
	
	if (fDoGamma)
	{
		GammaAdjustPalette(
			lpBltScreen, lpImage->hPal,	lpImage->nPaletteEntries, lpNewEntry);
	}
	else
	{
		AnimatePalette(
			lpImage->hPal, 0, lpImage->nPaletteEntries, lpNewEntry );
	}

	return( YES );
}

void CPPMapDlg::MapGray( LPTR lpGray )
{
	if (!m_lpHLut)
		return;

	if ( m_HueMap.bModified || m_SatMap.bModified )
	{
		*lpGray = m_lpMLut[ *lpGray ];
	}
	else
	{
		*lpGray = m_lpGLut[ 255-m_lpMLut[255-*lpGray] ];
	}
}

void CPPMapDlg::MapRGB( LPTR lpRed, LPTR lpGreen, LPTR lpBlue )
{
	RGBS rgb;
	HSLS hsl;

	if (!m_lpHLut)
		return;

	if ( m_HueMap.bModified || m_SatMap.bModified )
	{
		RGBtoHSL( *lpRed, *lpGreen, *lpBlue, &hsl );
		hsl.hue =  m_lpHLut[ hsl.hue ];
		hsl.sat =  m_lpSLut[ hsl.sat ];
		hsl.lum =  m_lpMLut[ hsl.lum ];
		HSLtoRGB( hsl.hue, hsl.sat, hsl.lum, &rgb );
		*lpRed   = rgb.red;
		*lpGreen = rgb.green;
		*lpBlue  = rgb.blue;
	}
	else
	{
		*lpRed   = m_lpRLut[ 255-m_lpMLut[255-*lpRed]   ];
		*lpGreen = m_lpGLut[ 255-m_lpMLut[255-*lpGreen] ];
		*lpBlue  = m_lpBLut[ 255-m_lpMLut[255-*lpBlue]  ];
	}
}


void CPPMapDlg::MapCMYK( LPTR lpC, LPTR lpM, LPTR lpY, LPTR lpK )
{
	CMYKS cmyk;
	HSLS  hsl;

	if (!m_lpHLut)
		return;

	if ( m_HueMap.bModified || m_SatMap.bModified )
	{
		CMYKtoHSL( *lpC, *lpM, *lpY, *lpK, &hsl );
		hsl.hue =  m_lpHLut[ hsl.hue ];
		hsl.sat =  m_lpSLut[ hsl.sat ];
		hsl.lum =  m_lpMLut[ hsl.lum ];
		HSLtoCMYK( hsl.hue, hsl.sat, hsl.lum, &cmyk );
		*lpC = cmyk.c;
		*lpM = cmyk.m;
		*lpY = cmyk.y;
		*lpK = cmyk.k;
	}
	else
	{
		*lpC = 255 - m_lpRLut[ 255-m_lpMLut[*lpC] ];
		*lpM = 255 - m_lpGLut[ 255-m_lpMLut[*lpM] ];
		*lpY = 255 - m_lpBLut[ 255-m_lpMLut[*lpY] ];
		*lpK = m_lpMLut[ *lpK ];
	}
}

void CPPMapDlg::map_proc(int y, int left, int right, LPTR lpSrc, LPTR lpDst, int depth, LPVOID lpParms  )
{
	int iCount = right - left + 1;

	switch(depth)
	{
		case 0 :
		case 1 :
			copy (lpSrc, lpDst, iCount);
			Map8(lpDst, NULL, NULL, iCount);
		break;

		case 3 :
			copy (lpSrc, lpDst, iCount*3);
			Map24(lpDst, NULL, NULL, iCount);
		break;

		case 4 :
			copy (lpSrc, lpDst, iCount*4);
			Map32(lpDst, NULL, NULL, iCount);
		break;
	}
}

void CPPMapDlg::ImageHookProc(
	int    x,
	int    y,
	int    dx,
	FRMTYPEINFO TypeInfo,
	LPTR   lpSrc,
	LPTR   lpDst,
	LPTR   lpMsk,
	LPMASK lpMask)
{
	mload( x, y, dx, 1, lpMsk, lpDst, TypeInfo, lpMask );

	switch ( TypeInfo.DataType )
	{
		case FDT_LINEART:
		case FDT_GRAYSCALE:
			Map8( lpDst, lpSrc, lpMsk, dx );
		break;

		case FDT_PALETTECOLOR:
			return;
		break;

		case FDT_RGBCOLOR:
			Map24( lpDst, lpSrc, lpMsk, dx );
		break;

		case FDT_CMYKCOLOR:
			Map32( lpDst, lpSrc, lpMsk, dx );
		break;

		default:
			return;
		break;
	}

}

