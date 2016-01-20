//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"

// LOCAL prototypes
LOCAL BOOL ShieldFloat_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void ShieldFloat_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void ShieldFloat_OnClose(HWND hDlg);
LOCAL void ShieldFloat_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void ShieldFloat_OnMouseMove(HWND hDlg, int x, int y, UINT keyFlags);

static BOOL bPreview, bProbePending;

void TurnOffShieldPreview()
   {
	if ( bPreview )
		DeactivateTool();
	bPreview = NO;
   }

/***********************************************************************/
BOOL WINPROC EXPORT DlgShieldFloatProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, ShieldFloat_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, ShieldFloat_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_LBUTTONUP, ShieldFloat_OnLButtonUp);
	HANDLE_DLGMSG(hDlg, WM_MOUSEMOVE, ShieldFloat_OnMouseMove);

	HANDLE_DLGMSG(hDlg, WM_CLOSE, ShieldFloat_OnClose);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_SHIELDFLOAT);
	}
}

/************************************************************************/
LOCAL BOOL ShieldFloat_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
int i;
BOOL active;
long Color;

RestorePopup( hDlg, "xyShield", RightPopup );
for (i=0; i<NUM_COLORMASKS; i++)
	{
	InitDlgItemSpin( hDlg, IDC_MASKRANGE1 + i,
		ColorMask.MaskRange[i], NO, 0, 100 );
	active = ColorMask.Mask & (1 << i);
	CheckDlgButton( hDlg, IDC_MASKACTIVE1 + i, active );
	CopyRGB(&ColorMask.MaskColor[i].rgb, &Color);
	SetWindowLong( GetDlgItem( hDlg, IDC_MASKCOLOR1+i ), GWL_ACTIVE,
		Color );
	}
CheckComboItem( hDlg, IDC_SHIELDSELECT, IDC_MASKINCLUDE,IDC_MASKEXCLUDE,
	ColorMask.Include ? IDC_MASKINCLUDE : IDC_MASKEXCLUDE);
CheckDlgButton( hDlg, IDC_SHIELD_IFHIDDEN, !ColorMask.IfHidden );
SetupShield();
ColorMask.On = YES;
ControlEnable( hDlg, IDC_PREVIEW, ColorMask.Mask );
ControlEnable( hDlg, IDC_RESET, NO );
return(TRUE);
}

/************************************************************************/
LOCAL void ShieldFloat_OnClose(HWND hDlg)
/************************************************************************/
{
TurnOffShieldPreview();
QuickProbeEnd( hDlg, -1, -1 );
ColorMask.On = !ColorMask.IfHidden;
AstralDlgEnd( hDlg, FALSE );
UpdateStatusBar( NO, NO, NO, YES );
if ( Tool.hRibbon )
	SendMessage( Tool.hRibbon, WM_SHIELDCHANGE, 0, 0L );
//	SaveToolPreferences( IDC_SHIELD );
}

/***********************************************************************/
LOCAL void ShieldFloat_OnLButtonUp(HWND hDlg, int x, int y, UINT keyFlags)
/***********************************************************************/
{
QuickProbeEnd( hDlg, -1, -1 );
}

/***********************************************************************/
LOCAL void ShieldFloat_OnMouseMove(HWND hDlg, int x, int y, UINT keyFlags)
/***********************************************************************/
{
COLORINFO ProbeColor;

if ( bProbePending )
	{
	bProbePending = NO;
	if ( !QuickProbeStart( hDlg ) )
		return;
	}
if ( QuickProbeGet( hDlg, x, y, &ProbeColor ) )
	UpdateColorMask( &ProbeColor );
}

/************************************************************************/
LOCAL void ShieldFloat_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
int i;
BOOL Bool, active;
BYTE mask;
long Color;
ITEMID idLastTool;
HWND hActiveWnd;
char Buff[10];

hActiveWnd = GetActiveDoc();
switch (id)
	{
   case IDC_MAP_PROBE:
      GetWindowText(hControl, Buff, 10);
      id = atoi(Buff);
      if (id < 1 || id > 8)
         break;
      id += IDC_MASKCOLOR1 - 1;
	   ShowWindow(GetDlgItem(hDlg, id), SW_SHOW);
      SetFocus(GetDlgItem(hDlg, id));
	   ShowWindow(hControl, SW_HIDE);
	case IDC_MASKCOLOR1:
	case IDC_MASKCOLOR2:
	case IDC_MASKCOLOR3:
	case IDC_MASKCOLOR4:
	case IDC_MASKCOLOR5:
	case IDC_MASKCOLOR6:
	case IDC_MASKCOLOR7:
	case IDC_MASKCOLOR8:
	ColorMask.MaskShield = i = id - IDC_MASKCOLOR1;
	if ( codeNotify != 1 /*Not A DoubleClick*/ )
		{
		if ( hActiveWnd )
			bProbePending = YES;
		}
   else
      {
	   bProbePending = NO;
	   if ( !ColorPicker( &ColorMask.MaskColor[i], NULL ) )
		   break;
	   CopyRGB( &ColorMask.MaskColor[i].rgb, &Color );
	   SetWindowLong( GetDlgItem( hDlg, id ), GWL_ACTIVE, Color );
	   AstralControlRepaint( hDlg, IDC_MASKCOLOR1 + i );
      }
	mask = 1 << (id - IDC_MASKCOLOR1);
   if (ColorMask.Mask & mask)
      {
	   SetupShield();
      break;
      }
   id = id - IDC_MASKCOLOR1 + IDC_MASKACTIVE1;

	case IDC_MASKACTIVE1:
	case IDC_MASKACTIVE2:
	case IDC_MASKACTIVE3:
	case IDC_MASKACTIVE4:
	case IDC_MASKACTIVE5:
	case IDC_MASKACTIVE6:
	case IDC_MASKACTIVE7:
	case IDC_MASKACTIVE8:
	mask = 1 << (id - IDC_MASKACTIVE1);
	ColorMask.Mask ^= mask;
	active = ColorMask.Mask & mask;
	CheckDlgButton( hDlg, id, active );
	SetupShield();
	break;

	case IDC_MASKRANGE1:
	case IDC_MASKRANGE2:
	case IDC_MASKRANGE3:
	case IDC_MASKRANGE4:
	case IDC_MASKRANGE5:
	case IDC_MASKRANGE6:
	case IDC_MASKRANGE7:
	case IDC_MASKRANGE8:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	ColorMask.MaskRange[id-IDC_MASKRANGE1] =
		GetDlgItemSpin( hDlg, id, &Bool, NO );
	SetupShield();
		break;

	case IDC_SHIELD_IFHIDDEN:
	ColorMask.IfHidden = !ColorMask.IfHidden;
	CheckDlgButton( hDlg, id, !ColorMask.IfHidden );
	break;

	case IDC_SHIELDSELECT:
//	case IDC_MASKINCLUDE:
//	case IDC_MASKEXCLUDE:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	ColorMask.Include = ( id == IDC_MASKINCLUDE );
	if ( Tool.hRibbon )
		SendMessage( Tool.hRibbon, WM_SHIELDCHANGE, 0, 0L );
	break;

	case IDC_PREVIEW:
	if (!hActiveWnd)
		break;
	idLastTool = ActivateTool(id);
	CreateProc( hActiveWnd, 0, 0, idLastTool );
	bPreview = YES;
	break;

	case IDC_RESET:
   TurnOffShieldPreview();
	break;

   default:
	break;
   }
}



