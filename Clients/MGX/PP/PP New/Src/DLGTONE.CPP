// (c) Copyright 1995 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

//dlgstone.cpp
#include "pp.h"
#include "id.h"
#include "ppafx.h"
#include "ctlutil.h"
#include "histoctl.h"
#include "tagctl.h"
#include "gradctl.h"
#include "ppmodal.h"
#include "dlgsmap.h"
#include "dlgtone.h"
#include "defaults.h"

#define TO_DISPLAY(val) \
	(m_bPercentages ? ((val)*100L/255):(val))
#define FROM_DISPLAY(val) \
	(m_bPercentages ? ((val)*255L/100):(val))
#define MAX_SPIN (m_bPercentages ? 100:255)




IMPLEMENT_DYNCREATE(CToneBalanceDlg, CPPMapExDlg)
IMPLEMENT_DYNCREATE(CToneBalanceOptionsDlg, CPPModalDlg)

BEGIN_MESSAGE_MAP(CToneBalanceDlg, CPPMapExDlg)
	ON_WM_DESTROY()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_DRAWITEM()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_MAPLOAD, OnMapLoad)
	ON_BN_CLICKED(IDC_MAPSAVE, OnMapSave)
	ON_BN_CLICKED(IDC_MAP_OPTIONS, OnMapOptions)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_BN_CLICKED(IDC_RESETALL, OnResetAll)
	ON_BN_CLICKED(IDC_PREVIEW, OnPreview)
	ON_BN_CLICKED(IDC_QUART_DEFAULTS, OnAutoClip)
END_MESSAGE_MAP()

CToneBalanceDlg::CToneBalanceDlg(CServerView* pView, CWnd* pParent)
:CPPMapExDlg(pView, CToneBalanceDlg::IDD, pParent)
{
}

BOOL CToneBalanceDlg::PreDoModal(LPVOID lpVoid)
{
	m_bAutoPreview = GetDefaultInt(Quarter_bAuto_Str, YES);
	m_bPercentages = GetDefaultInt(Quarter_bPercent_Str, NO);
	m_bUseMidPer   = GetDefaultInt(Quarter_bUseMidPer_Str, YES); 
	m_nHSacrifice  = GetDefaultInt(Quarter_HSacrifice_Str, 5);
	m_nSSacrifice  = GetDefaultInt(Quarter_SSacrifice_Str, 5);
	return CPPMapDlg::PreDoModal(lpVoid);
}

void CToneBalanceDlg::OnCancel()
{
	UnAutoPreview();
	CPPMapDlg::OnCancel();
}

void CToneBalanceDlg::OnOK()
{
	map_to_mapinfo(&m_MasterMap, &m_Parms.MasterInfo);
	map_to_mapinfo(&m_RedMap, &m_Parms.RedInfo);
	map_to_mapinfo(&m_GreenMap, &m_Parms.GreenInfo);
	map_to_mapinfo(&m_BlueMap, &m_Parms.BlueInfo);
	map_to_mapinfo(&m_BlackMap, &m_Parms.BlackInfo);
	SetMapGlobals();
	UnAutoPreview();
	CPPMapDlg::OnOK();
}

void CToneBalanceDlg::OnReset()
{
	reset_channel(m_lpData->wChannel);					// setup channel with defaults
	set_quarter_channel(m_lpData->wChannel);			// setup the actual histogram and controls 
	AutoPreview(m_lpData->lpMap, m_bAutoPreview, YES);
}

void CToneBalanceDlg::OnPreview()
{
	AutoPreview_Button(m_lpData->lpMap);
}

void CToneBalanceDlg::OnResetAll()
{
	for (int i=IDC_DO_ALL; i<IDC_DO_BLACK; i++)				// setup each channel with defaults
	{
		if(ChannelsCombo_IsEnabled(i))
			reset_channel(i);
	}
	set_quarter_channel(m_lpData->wChannel);				// setup the actual histogram and controls 
	AutoPreview(m_lpData->lpMap, m_bAutoPreview, YES);

}



void CToneBalanceDlg::OnTimer( UINT  nIDEvent )
{
	

	if (!AutoPreview_OnTimer(nIDEvent))	   //if timer's no good, return
		return;
	MakeMap(m_lpData->lpMap );
	SetMapGlobals();
	MapPreview(GetViewHwnd());
}

void CToneBalanceDlg::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	::DrawItem( GetSafeHwnd(), lpDrawItemStruct, 0 );
}

void CToneBalanceDlg::OnMouseMove( UINT nFlags, CPoint point )
{
	COLORINFO ProbeColor;
	int i;
	HWND hDlg = GetSafeHwnd();
	

	if (Probe_OnMouseMove(point.x, point.y, nFlags,&ProbeColor))   //if probe is valid...
	{
		switch (m_lpData->wChannel)
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
		change_mark(m_lpData, m_lpData->iProbe, i, INT_MAX);
	}
	else if (Probe_IsProbing())	   //if probe doesn't have a good value ....
		change_mark(m_lpData, m_lpData->iProbe, m_lpData->OriginalValue, INT_MAX);
}


void CToneBalanceDlg::OnLButtonUp( UINT nFlags, CPoint point )
{
	COLORINFO ProbeColor;
	int i;
	HWND hDlg = GetSafeHwnd();
	
	
	if (!Probe_IsProbing())
		return;
	
	if (Probe_OnLButtonUp(point.x, point.y, nFlags,&ProbeColor))	   //if color is good ...
	{
		switch (m_lpData->wChannel)
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
		change_mark(m_lpData, m_lpData->iProbe, i, INT_MAX);
	}
	else // unsucessful probe session ...
		change_mark(m_lpData, m_lpData->iProbe, m_lpData->OriginalValue, INT_MAX);

	CheckDlgButton(IDC_QUART1_PROBE, NO );
	CheckDlgButton(IDC_QUART2_PROBE, NO );
	CheckDlgButton(IDC_QUART3_PROBE, NO );
	AutoPreview(m_lpData->lpMap, m_bAutoPreview, YES);
}


void CToneBalanceDlg::OnAutoClip()
{
	set_sacrifice(m_lpData, m_lpData->wChannel);			// setup channel with sacrifice defaults
	set_quarter_channel(m_lpData->wChannel);				// setup the actual histogram and controls 
	AutoPreview(m_lpData->lpMap, m_bAutoPreview, YES);
}

void CToneBalanceDlg::OnMapOptions()
{
	HWND hDlg = GetSafeHwnd();
	CToneBalanceOptionsDlg dlg(this);
	dlg.DoModal();				

	ControlEnable(hDlg, IDC_PREVIEW, !m_bAutoPreview);
	SetAllDlgItemsText(hDlg, IDC_PERCENT, m_bPercentages ? (LPSTR)"%":(LPSTR)"");
	set_quarter_channel(m_lpData->wChannel);
	AutoPreview(m_lpData->lpMap, m_bAutoPreview, YES);
}

void CToneBalanceDlg::OnMapSave()
{
	CMapSaveDlg dlg(this);
	dlg.DoModal();
}

void CToneBalanceDlg::OnMapLoad()
{
	CMapLoadDlg dlg(this);
	dlg.DoModal();
	handle_tone_load(m_lpData);
}

CToneBalanceDlg::OnInitDialog()
{
	HWND hControl;
	FRMDATATYPE type;
	LPIMAGE lpImage = NULL;
	int i;
	LONG style;
	HWND hDlg = GetSafeHwnd();
	PreDoModal();	
	m_lpData = (LPQUARTERSTRUCT)Alloc(sizeof(QUARTERSTRUCT));
	if (!m_lpData)
		return(FALSE);
	for (i=0;i<3;i++) 	// setup tag sliders
	{
		if (hControl = ::GetDlgItem(hDlg, i+IDC_QUART1))
		{
			style = GetWindowLong(hControl, GWL_EXSTYLE);
			style |= WS_EX_TRANSPARENT;
			SetWindowLong(hControl, GWL_EXSTYLE, style);
		}
	}

	lpImage = GetImage();
	RightPopup( hDlg );
	
	// set channels combo
	type = FrameType(ImgGetBaseEditFrame(lpImage));
	m_lpData->wChannel = IDC_DO_ALL;
	m_MapValues.wChannel = m_lpData->wChannel; //this is not a very good	solution
	ChannelsCombo_Setup(IDC_CHANNELS, type);

	ControlEnable(hDlg, IDC_PREVIEW, !m_bAutoPreview);

	// remove or add percents
	SetAllDlgItemsText(hDlg, IDC_PERCENT, m_bPercentages ? (LPSTR)"%":(LPSTR)"");
	
	m_lpData->lpHistos[0] = NULL;
	ResetAllMaps();
	
	// setup each channel with defaults
	for (i=IDC_DO_ALL; i<=IDC_DO_BLACK;i++)
		reset_channel(i);
	// setup the actual histogram and controls 
	set_quarter_channel(m_lpData->wChannel);
	return CPPMapDlg::OnInitDialog();
}

void CToneBalanceDlg::OnDestroy()
{
	AutoPreview_OnDestroy();
	Probe_End();
	free_histograms(m_lpData->lpHistos);
}

BOOL CToneBalanceDlg::OnCommand( WPARAM wParam, LPARAM lParam )
{
	int id = LOWORD(wParam);					//unpack the message
	WORD codeNotify = HIWORD(wParam);
	HWND hControl = (HWND) lParam;
	HWND hDlg = GetSafeHwnd();

	int i, val;
	BOOL Bool;
	HWND hActiveWnd;
	LPIMAGE lpImage = NULL;
	

	lpImage = GetImage();

	switch (id)
	{
		case IDC_CHANNELS:
			i=ChannelsCombo_Handle(id, codeNotify);
			if (!i)
				break;
			m_lpData->wChannel = i;
			set_quarter_channel(m_lpData->wChannel);
		break;
	
		case IDC_QUART1_PROBE:
		case IDC_QUART2_PROBE:
		case IDC_QUART3_PROBE:
			if (!Probe_Begin(codeNotify))
				break;
			CheckDlgButton(id, YES );
			m_lpData->iProbe = id-IDC_QUART1_PROBE;
			if (m_lpData->iProbe == 0)
				m_lpData->OriginalValue = m_lpData->iShadow;
			else if (m_lpData->iProbe == 1)
				m_lpData->OriginalValue = m_lpData->iMidtone;
			else
				m_lpData->OriginalValue = m_lpData->iHighlight;
			// unpreview
			UnAutoPreview();
		break;

		case IDC_QUART1_VALUE:
		case IDC_QUART2_VALUE:
		case IDC_QUART3_VALUE:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE)
				break;
			val = GetDlgItemSpin(hDlg, id, &Bool, NO);
			if (!Bool)
				break;
			// convert to 'real' location
			if (id == IDC_QUART2_VALUE && m_bUseMidPer)
			  	i = m_lpData->iShadow + (LONG)val*(m_lpData->iHighlight-m_lpData->iShadow)/100;
			else
				i = FROM_DISPLAY(val);
			if (id != IDC_QUART2_VALUE)
				val = INT_MAX;
			change_mark(m_lpData,  id-IDC_QUART1_VALUE, i, val);
			AutoPreview(m_lpData->lpMap, m_bAutoPreview, NO);
		break;
		
		case IDC_MINHIGHLIGHT:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE)
				break;
			i = GetDlgItemSpin(hDlg, id, &Bool, NO);
			if (!Bool)
				break;
			i = FROM_DISPLAY(i);
			m_lpData->lpMap->Pnt[2].y = i;
			m_lpData->lpMap->Pnt[1].y = (m_lpData->lpMap->Pnt[2].y+m_lpData->lpMap->Pnt[0].y)/2;
			AutoPreview(m_lpData->lpMap,m_bAutoPreview, YES);
		break;
		
		case IDC_MAXSHADOW:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE)
				break;
			i = GetDlgItemSpin(hDlg, id, &Bool, NO);
			if (!Bool)
				break;
			i = FROM_DISPLAY(i);
			m_lpData->lpMap->Pnt[0].y = i;
			m_lpData->lpMap->Pnt[1].y = (m_lpData->lpMap->Pnt[2].y+m_lpData->lpMap->Pnt[0].y)/2;
			AutoPreview(m_lpData->lpMap, m_bAutoPreview, YES);
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
			change_mark(m_lpData, i, val, INT_MAX);
			AutoPreview(m_lpData->lpMap, m_bAutoPreview, Bool);
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
			change_mark(m_lpData, i, val, INT_MAX);
			AutoPreview(m_lpData->lpMap, m_bAutoPreview, Bool);
		break;

//		case IDC_PREVIEW:
//	  		AutoPreview_Button(m_lpData->lpMap);
//		break;
		
		
//		case IDC_QUART_DEFAULTS:
//			// setup channel with sacrifice defaults
//			set_sacrifice(m_lpData, m_lpData->wChannel);
//			// setup the actual histogram and controls 
//			set_quarter_channel(m_lpData->wChannel);
//			AutoPreview(m_lpData->lpMap, m_bAutoPreview, YES);
//		break;
	
		


   		default:
		break;
   }
return CWnd::OnCommand(wParam, lParam);
}




//			Sets the given channel with the default vales.
VOID CToneBalanceDlg::reset_channel(WORD wChannel)
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

//			Sets up the given channel, Setting invert flags etc.
// RETURNS:
//			The map for the given channel.
LPMAP CToneBalanceDlg::tone_activate_channel(WORD wChannel )
{
	LPMAP lpMap; 

	switch ( wChannel )
	{
		case IDC_DO_ALL:
			lpMap = &m_MasterMap;
			lpMap->Reverse = YES;
		break;
		case IDC_DO_CYAN:
			lpMap = &m_RedMap;
			lpMap->Reverse = NO;
		break;
		case IDC_DO_MAGENTA:
			lpMap = &m_GreenMap;
			lpMap->Reverse = NO;
		break;
		case IDC_DO_YELLOW:
			lpMap = &m_BlueMap;
			lpMap->Reverse = NO;
		break;
		case IDC_DO_RED:
			lpMap = &m_RedMap;
			lpMap->Reverse = NO;
		break;
		case IDC_DO_GREEN:
			lpMap = &m_GreenMap;
			lpMap->Reverse = NO;
		break;
		case IDC_DO_BLUE:
			lpMap = &m_BlueMap;
			lpMap->Reverse = NO;
		break;
		case IDC_DO_BLACK:
			lpMap = &m_BlackMap;
			lpMap->Reverse = NO;
		break;
		default:
			lpMap = &m_MasterMap;
			lpMap->Reverse = NO;
		break;
	}

	return( lpMap );
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
void CToneBalanceDlg::change_mark(LPQUARTERSTRUCT lpData, int nMark, int val, int inval)
//************************************************************************
{
	HWND hHisto, hTag;
	int i;
	BOOL Bool;
	int Marks[3];
	HWND hDlg = GetSafeHwnd();
	
	// set value and adjust others
	Marks[0] = lpData->iShadow;
	Marks[2] = lpData->iHighlight;
	if (m_bUseMidPer)
		Marks[1] = Marks[0] + (LONG)lpData->MidtonePer*(Marks[2]-Marks[0])/255;
	else
		Marks[1] = lpData->iMidtone;
	if (nMark == 0)
	{
		Marks[0] = lpData->iShadow = val;
		if (Marks[2] < Marks[0])
			Marks[2] = Marks[0];
		if (m_bUseMidPer)
			Marks[1] = Marks[0] + (LONG)lpData->MidtonePer*(Marks[2]-Marks[0])/255;
		else if (Marks[1] < Marks[0])
			Marks[1] = Marks[0];
	}
	else if (nMark == 2)
	{
		Marks[2] = lpData->iHighlight = val;
		if (Marks[0] > Marks[2])
			Marks[0] = Marks[2];
		if (m_bUseMidPer)
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
		if (m_bUseMidPer)
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
	hHisto = ::GetDlgItem(hDlg, IDC_HISTOGRAM);
	hTag = ::GetDlgItem(hDlg, IDC_TAGS);
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
//		set_quarter_channel
// DESCRIPTION:
//		Makes the given channel the current channel.
//		Updates the display.
//************************************************************************
VOID CToneBalanceDlg::set_quarter_channel(WORD wChannel)
//************************************************************************
{
	LPDWORD lpHisto;
	HWND hControl;
	BOOL bInv;
	LONG lMaxEntry;
	int icons[3];
	HWND hDlg = GetSafeHwnd();
	
	
	m_lpData->lpMap = tone_activate_channel( wChannel );
	lpHisto = get_histo(m_lpData->lpHistos, wChannel, &bInv);
	
	// setup the histo control
	lMaxEntry = compute_yscale(lpHisto);
	hControl = ::GetDlgItem(hDlg, IDC_HISTOGRAM);
	Histo_Init(hControl, lpHisto, lMaxEntry, 3, bInv ? HTS_FLIPX:0);
	m_lpData->iShadow = m_lpData->lpMap->Pnt[0].x;
	m_lpData->iMidtone = m_lpData->lpMap->Pnt[1].x;
	m_lpData->iHighlight = m_lpData->lpMap->Pnt[2].x;
	if (!(m_lpData->iHighlight - m_lpData->iShadow))
		m_lpData->MidtonePer = 0;
	else
		m_lpData->MidtonePer = (m_lpData->iMidtone - m_lpData->iShadow)*255/
								(m_lpData->iHighlight - m_lpData->iShadow);
	Histo_SetMark(hControl, 0, m_lpData->lpMap->Pnt[0].x, NO);
	Histo_SetMark(hControl, 1, m_lpData->lpMap->Pnt[1].x, NO);
	Histo_SetMark(hControl, 2, m_lpData->lpMap->Pnt[2].x, NO);
	
	// some people seem to have an update problem here
    ::InvalidateRect (hControl,NULL,FALSE);

	// setup tags
	hControl = ::GetDlgItem(hDlg, IDC_TAGS);
	
	icons[0] = IDC_QUART1;
	icons[1] = IDC_QUART2;
	icons[2] = IDC_QUART3;
	Tag_Init(hControl, 3, 255, &icons[0]);
	
	Tag_SetMark(hControl, 0, m_lpData->lpMap->Pnt[0].x, NO);
	Tag_SetMark(hControl, 1, m_lpData->lpMap->Pnt[1].x, NO);
	Tag_SetMark(hControl, 2, m_lpData->lpMap->Pnt[2].x, NO);
	
	// some people seem to have an update problem here
    ::InvalidateRect (hControl,NULL,FALSE);
	
	set_gradient(IDC_HISTO_GRAD, wChannel);

	// spins
	InitDlgItemSpin(hDlg, IDC_QUART1_VALUE, TO_DISPLAY(m_lpData->lpMap->Pnt[0].x), NO, 0, MAX_SPIN);
	InitDlgItemSpin(hDlg, IDC_QUART3_VALUE, TO_DISPLAY(m_lpData->lpMap->Pnt[2].x), NO, 0, MAX_SPIN);

	if (m_bUseMidPer)
		InitDlgItemSpin(hDlg, IDC_QUART2_VALUE, m_lpData->MidtonePer*100L/255, NO, 0, MAX_SPIN );
	else
 		InitDlgItemSpin(hDlg, IDC_QUART2_VALUE, TO_DISPLAY(m_lpData->lpMap->Pnt[1].x), NO, 0, MAX_SPIN );

	InitDlgItemSpin(hDlg, IDC_MINHIGHLIGHT, TO_DISPLAY(m_lpData->lpMap->Pnt[2].y), NO, 0, MAX_SPIN);
	InitDlgItemSpin(hDlg, IDC_MAXSHADOW, TO_DISPLAY(m_lpData->lpMap->Pnt[0].y), NO, 0, MAX_SPIN);
}

//			Uses Mapping.Quarter_HSacrifice & Mapping.Quarter_SSacrifice
//			to set the highlights midtones and shaddows.
void CToneBalanceDlg::set_sacrifice(LPQUARTERSTRUCT lpData, WORD wChannel)
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
		lSearch = bInv ? m_nHSacrifice:m_nSSacrifice;
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
		lSearch = bInv ? m_nSSacrifice:m_nHSacrifice;
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


void CToneBalanceDlg::handle_tone_load(LPQUARTERSTRUCT lpData)
{
	WORD wChannel;
	LPMAP lpMap;
	BOOL bAllCool;
	HWND hDlg = GetSafeHwnd();
	
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
	set_quarter_channel(lpData->wChannel);
	AutoPreview(lpData->lpMap, m_bAutoPreview, YES);
}
//////////////////////////////// CToneBalanceOptionsDlg ///////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CToneBalanceOptionsDlg, CPPModalDlg)
	ON_BN_CLICKED(IDC_USE_MID_PER, OnMidPercentages)
	ON_BN_CLICKED(IDC_AUTO, OnAutoPreview)
	ON_BN_CLICKED(IDC_MAP_PERCENT, OnPercentages)
//	ON_NOTIFY(UDN_DELTAPOS, IDC_HILIGHTS_SACRIFICED, OnHilightsSacrificed)
END_MESSAGE_MAP()

void CToneBalanceOptionsDlg::OnMidPercentages()
{
	m_bUseMidPer = !m_bUseMidPer;
	CheckDlgButton(IDC_USE_MID_PER, m_bUseMidPer);
}

void CToneBalanceOptionsDlg::OnAutoPreview()
{
	m_bAutoPreview = !m_bAutoPreview;
	CheckDlgButton(IDC_AUTO, m_bAutoPreview);
}

void CToneBalanceOptionsDlg::OnPercentages()
{
	m_bPercentages = !m_bPercentages;
	CheckDlgButton(IDC_MAP_PERCENT, m_bPercentages);
}	

CToneBalanceOptionsDlg::CToneBalanceOptionsDlg(CWnd* pParent)
:CPPModalDlg(CToneBalanceOptionsDlg::IDD, pParent)
{
    m_pParent = (CToneBalanceDlg*) pParent;
}


BOOL CToneBalanceOptionsDlg::PreDoModal(LPVOID lpVoid)
{
	m_bAutoPreview = m_pParent->GetAutoPreviewPref();
	m_bPercentages = m_pParent->GetPercentagesPref();
	m_bUseMidPer   = m_pParent->GetMidtonePercentagesPref();
	m_nHSacrifice  = m_pParent->GetHSacrificePref();
	m_nSSacrifice  = m_pParent->GetSSacrificePref();
	return CPPModalDlg::PreDoModal();
}

LPVOID CToneBalanceOptionsDlg::PostDoModal()
{
	m_pParent->SetAutoPreviewPref(m_bAutoPreview);			 
		PutDefaultInt(Quarter_bAuto_Str, m_bAutoPreview);
	m_pParent->SetPercentagesPref(m_bPercentages);
		PutDefaultInt(Quarter_bPercent_Str, m_bPercentages);
	m_pParent->SetMidtonePercentagesPref(m_bUseMidPer);
		PutDefaultInt(Quarter_bUseMidPer_Str, m_bUseMidPer); 
	m_pParent->SetHSacrificePref(m_nHSacrifice);
		PutDefaultInt(Quarter_HSacrifice_Str, m_nHSacrifice);
	m_pParent->SetSSacrificePref(m_nSSacrifice);
		PutDefaultInt(Quarter_SSacrifice_Str, m_nSSacrifice);
	return CPPModalDlg::PostDoModal();
}


BOOL CToneBalanceOptionsDlg::OnInitDialog()
{
	PreDoModal();
	CheckDlgButton(IDC_USE_MID_PER, m_bUseMidPer);
	CheckDlgButton(IDC_AUTO,m_bAutoPreview);
	CheckDlgButton(IDC_MAP_PERCENT, m_bPercentages);
	InitDlgItemSpin(GetSafeHwnd(), IDC_HILIGHTS_SACRIFICED, m_nHSacrifice, NO, 0, 100);
	InitDlgItemSpin(GetSafeHwnd(), IDC_SHADOWS_SACRIFICED, m_nSSacrifice, NO, 0, 100);
	return CPPModalDlg::OnInitDialog();
}


void CToneBalanceOptionsDlg::OnOK()
{
	PostDoModal();
	CPPModalDlg::OnOK();
}

BOOL CToneBalanceOptionsDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int id = LOWORD(wParam);					//unpack the message
	WORD codeNotify = HIWORD(wParam);
	HWND hControl = (HWND) lParam;
	HWND hDlg = GetSafeHwnd();

	int i;
	BOOL bool;
	
	switch (id)
	{
		
		case IDC_HILIGHTS_SACRIFICED:
			i = GetDlgItemSpin(hDlg, id, &bool, NO);
			if (bool)
				m_nHSacrifice = i;
		break; 
		
		case IDC_SHADOWS_SACRIFICED:
			i = GetDlgItemSpin(hDlg, id, &bool, NO);
			if (bool)
				m_nSSacrifice = i;
		break; 
		
	   default:
		break;
	}
return CWnd::OnCommand(wParam, lParam);
}

//void CToneBalanceOptionsDlg::OnHilightsSacrificed(NMHDR* pNMHDR, LRESULT* pResult)
//{
//	int i;
//	BOOL bool;
//	i = GetDlgItemSpin(GetSafeHwnd(), IDC_HILIGHTS_SACRIFICED, &bool, NO);
//	if (bool)
//		m_nHSacrifice = i;
//}
