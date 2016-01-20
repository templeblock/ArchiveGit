//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "pp.h"
#include "ppafx.h"
#include "bsrutil.h"
#include <time.h>
#include "id.h"

// LOCAL prototypes
LOCAL BOOL Align_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Align_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void ObjAlign( LPRECT bRect, LPRECT oRect, LPALIGNOBJECTS_PARMS lpAlignParms,
						LFIXED vSpace, LFIXED hSpace, LPINT xOffset, LPINT yOffset,
						LPLFIXED fx, LPLFIXED fy );
LOCAL void AlignIcons( HWND hDlg, LPALIGNOBJECTS_PARMS lpAlignParms );
LOCAL BOOL Pos_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Pos_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void InitSpin( HWND hDlg, LFIXED fSetX, LFIXED fSetY,int flag);
LOCAL BOOL IsPrevGroup( WORD wGroupID, LPWORD wGroupIDs );
LOCAL void SetGroupList( WORD wGroupID, LPWORD wGroupIDs );
LOCAL void InitGroupList( LPWORD wGroupIDs );
BOOL IsGroupLocked(WORD wGroupID);

#define	PARMS_INDEX	0
/***********************************************************************/
BOOL WINPROC EXPORT DlgAlignProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Align_OnInitDialog);
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Align_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_ALIGNOBJECT);
	}
}

#define  ALIGN_NUM_OBJECTS 3
static   POINT InitPos[ALIGN_NUM_OBJECTS];
/************************************************************************/
LOCAL BOOL Align_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
int                  i, nCount;
LPRECT					pRect;
LPOBJECT					lpObject;
LPIMAGE					lpImage;
HWND  					hWnd, hWndChild;
RECT					   gRect;
LPALIGNOBJECTS_PARMS	lpAlignParms;
int                  arObjects[] = { IDC_ALIGN_SQUARE, IDC_ALIGN_TRIANGLE,
                                    IDC_ALIGN_CIRCLE };

lpAlignParms = ( LPALIGNOBJECTS_PARMS )lParam;
lpAlignParms->HorzAlign = HA_NONE;
lpAlignParms->VertAlign = VA_NONE;
lpAlignParms->RectAlign = RA_OBJECT;
lpAlignParms->bPreview  = FALSE;

if( !( lpImage = GetActiveImage() ) )
	return( FALSE );

if( !( ImgGetMask( lpImage ) ) )
	ControlEnable( hDlg, IDC_ALIGN_MASK, NO );
if( !( ImgMultipleObjects( lpImage ) ) )
	ControlEnable( hDlg, IDC_ALIGN_OBJECT, NO );
	
CheckComboItem( hDlg, IDC_ALIGN_COMBO, IDC_ALIGN_IMAGE,	IDC_ALIGN_MASK,
	IDC_ALIGN_IMAGE );

SendDlgItemMessage( hDlg, IDC_ALIGN_COMBO, CB_SETITEMDATA, PARMS_INDEX,
	lParam );

nCount = 0;

if( !( lpObject = ImgGetBase( lpImage ) )	)
	return( FALSE );
while ( lpObject = ImgGetSelObject(lpImage, lpObject) )
	++nCount;
if( !( pRect = lpAlignParms->lpRect = ( LPRECT )Alloc( sizeof( RECT ) * nCount ) ) )
	return( FALSE );
while ( lpObject = ImgGetSelObject(lpImage, lpObject) )
	*(pRect)++ = lpObject->rObject;

hWnd = GetDlgItem( hDlg, IDC_ALIGN_GRID );
GetClientRect( hWnd, &gRect );

for( i = 0; i < ALIGN_NUM_OBJECTS; i++ )
   {
   InitPos[i].x = gRect.left + ( ( gRect.right / ( ALIGN_NUM_OBJECTS + 1 ) ) * ( i + 1 ) ) - 16;
   InitPos[i].y = gRect.top + ( ( gRect.bottom / ( ALIGN_NUM_OBJECTS + 1 ) ) * ( i + 1 ) ) - 16;
   hWndChild = CreateWindowEx( 
       WS_EX_TRANSPARENT, 
       "icon",
	   "", 
       WS_CHILD | WS_VISIBLE | WS_NOSHADOWS | WS_NOTENABLED | WS_NOBGND,
	   InitPos[i].x, 
       InitPos[i].y, 
       32, 
       32,
	   hWnd, 
       ( HMENU )arObjects[i], 
       PictPubApp.GetResourceHandle(),
       NULL );
   ShowWindow( hWndChild, SW_SHOWNORMAL );
   UpdateWindow( hWndChild );
   }

CenterPopup( hDlg );
return(TRUE);
}

void DoReset(HWND hDlg, LPALIGNOBJECTS_PARMS lpAlignParms)
{
RECT		               uRect;
LPIMAGE                 lpImage = GetActiveImage();
LPRECT						pRect;
LPOBJECT						lpObject;

if(!lpImage)
   return;

HWND                    hWnd = GetDlgItem(hDlg, IDC_ALIGN_GRID);
int                     x,y,i, wind;
RECT		               cRect;
for( i = 0, wind = GW_CHILD; i < ALIGN_NUM_OBJECTS; i++, wind = GW_HWNDNEXT )
   {
	hWnd = GetWindow( hWnd, wind );
	GetClientRect( hWnd, &cRect );
	cRect.right -= 1;
	cRect.bottom -= 1;
	ShowWindow( hWnd, SW_HIDE );
   x = InitPos[i].x - cRect.left;
   y = InitPos[i].y - cRect.top;
	OffsetRect( &cRect, x, y);
	MoveWindow( hWnd, cRect.left, cRect.top, RectWidth( &cRect ),
		RectHeight( &cRect ), TRUE );
	UpdateWindow( GetDlgItem( hDlg, IDC_ALIGN_GRID ) );
	ShowWindow( hWnd, SW_SHOWNA );
   }

lpAlignParms->HorzAlign = HA_NONE;
lpAlignParms->VertAlign = VA_NONE;
if( lpAlignParms->bPreview )
	{
	pRect = lpAlignParms->lpRect;
	if( !( lpObject = ImgGetBase(lpImage)))
		return;
	while ( lpObject = ImgGetSelObject( lpImage, lpObject) )
      {
      AstralUnionRect(&uRect, &uRect, &lpObject->rObject);
		lpObject->rObject = *pRect++;
      }
	AlignObjects( GetActiveImage(), lpAlignParms );
	}
EnableWindow( GetDlgItem( hDlg, IDC_PREVIEW ), TRUE );
lpAlignParms->bPreview  = FALSE;
UpdateImage(lpImage, &uRect, YES);
}

/************************************************************************/
LOCAL void Align_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
LPRECT						pRect;
LPALIGNOBJECTS_PARMS		lpAlignParms;
int                  	nType;
LPOBJECT						lpObject;
LPIMAGE						lpImage;

lpAlignParms = ( LPALIGNOBJECTS_PARMS )SendDlgItemMessage( hDlg,
	IDC_ALIGN_COMBO, CB_GETITEMDATA,	PARMS_INDEX, 0 );
switch (id)
	{
	case IDC_ALIGN_RIGHT:
      if (lpAlignParms->bPreview)
         DoReset(hDlg, lpAlignParms);
		lpAlignParms->HorzAlign = HA_RIGHT;
		AlignIcons( hDlg, lpAlignParms );
	break;

	case IDC_ALIGN_CENTER:
      if (lpAlignParms->bPreview)
         DoReset(hDlg, lpAlignParms);
		lpAlignParms->HorzAlign = HA_CENTER;
		AlignIcons( hDlg, lpAlignParms );
	break;

	case IDC_ALIGN_LEFT:
      if (lpAlignParms->bPreview)
         DoReset(hDlg, lpAlignParms);
		lpAlignParms->HorzAlign = HA_LEFT;
		AlignIcons( hDlg, lpAlignParms );
	break;

	case IDC_ALIGN_HSPACE:
      if (lpAlignParms->bPreview)
         DoReset(hDlg, lpAlignParms);
		lpAlignParms->HorzAlign = HA_EQUAL;
		AlignIcons( hDlg, lpAlignParms );
	break;
	
	
	case IDC_ALIGN_UP:
      if (lpAlignParms->bPreview)
         DoReset(hDlg, lpAlignParms);
		lpAlignParms->VertAlign = VA_TOP;
		AlignIcons( hDlg, lpAlignParms );
	break;

	case IDC_ALIGN_MID:
      if (lpAlignParms->bPreview)
         DoReset(hDlg, lpAlignParms);
		lpAlignParms->VertAlign = VA_MID;
		AlignIcons( hDlg, lpAlignParms );
	break;

	case IDC_ALIGN_DOWN:
      if (lpAlignParms->bPreview)
         DoReset(hDlg, lpAlignParms);
		lpAlignParms->VertAlign = VA_BOTTOM;
		AlignIcons( hDlg, lpAlignParms );
	break;
	
	case IDC_ALIGN_VSPACE:
      if (lpAlignParms->bPreview)
         DoReset(hDlg, lpAlignParms);
		lpAlignParms->VertAlign = VA_EQUAL;
		AlignIcons( hDlg, lpAlignParms );
	break;
	
	case IDC_PREVIEW:
      nType = ( int )SendDlgItemMessage( hDlg, IDC_ALIGN_COMBO,
         CB_GETCURSEL, 0, 0 );
      if( nType == 0 )
         lpAlignParms->RectAlign = RA_IMAGE;
      else if( nType == 1 )
         lpAlignParms->RectAlign = RA_OBJECT;
      else
         lpAlignParms->RectAlign = RA_MASK;
		lpAlignParms->bPreview  = TRUE;
		AlignObjects( GetActiveImage(), lpAlignParms );
		EnableWindow( GetDlgItem( hDlg, IDC_PREVIEW ), FALSE );
	break;
	
	case IDOK:
		ALIGNOBJECTS_PARMS	AlignParms;

		if( lpAlignParms->bPreview )
			{
			AlignParms = *lpAlignParms;
			lpAlignParms->HorzAlign = HA_NONE;
			lpAlignParms->VertAlign = VA_NONE;
			pRect = lpAlignParms->lpRect;
			if( !( lpImage = GetActiveImage() ) )
				break;
			if( !( lpObject = ImgGetBase( lpImage ) ) )
				break;
			while ( lpObject = ImgGetSelObject( lpImage, lpObject) )
				lpObject->rObject = *pRect++;
			AlignObjects( GetActiveImage(), lpAlignParms );
			*lpAlignParms = AlignParms;
			}
      nType = ( int )SendDlgItemMessage( hDlg, IDC_ALIGN_COMBO,
         CB_GETCURSEL, 0, 0 );
      if( nType == 0 )
         lpAlignParms->RectAlign = RA_IMAGE;
      else if( nType == 1 )
         lpAlignParms->RectAlign = RA_OBJECT;
      else
         lpAlignParms->RectAlign = RA_MASK;
		lpAlignParms->bPreview  = FALSE;
		AstralDlgEnd( hDlg, TRUE );
	break;
	
	case IDC_RESET:
      DoReset(hDlg, lpAlignParms);
	break;
	
	case IDCANCEL:
      DoReset(hDlg, lpAlignParms);
		AstralDlgEnd( hDlg, FALSE );
	break;
	
   default:
	break;
   }
}

#define ICON_HEIGHT 	32
#define ICON_WIDTH	32

/************************************************************************/
void AlignIcons( HWND hDlg, LPALIGNOBJECTS_PARMS lpAlignParms )
/************************************************************************/
{
RECT		bRect, cRect;
int		xOffset, yOffset, i, wind;
LFIXED	vSpace, hSpace, fx, fy;
HWND		hWnd;

GetClientRect( hWnd = GetDlgItem( hDlg, IDC_ALIGN_GRID ), &bRect );

vSpace = FGET( RectHeight( &bRect ) - ( ALIGN_NUM_OBJECTS * ICON_HEIGHT ),
	ALIGN_NUM_OBJECTS - 1 );
hSpace = FGET( RectWidth( &bRect ) - ( ALIGN_NUM_OBJECTS * ICON_WIDTH ),
	ALIGN_NUM_OBJECTS - 1 );

fx = fy = 0;

for( i = 0, wind = GW_CHILD; i < ALIGN_NUM_OBJECTS; i++, wind = GW_HWNDNEXT )
	{	
	hWnd = GetWindow( hWnd, wind );
	GetClientRect( hWnd, &cRect );
	cRect.right -= 1;
	cRect.bottom -= 1;
	MapWindowPoints( hWnd, GetDlgItem( hDlg, IDC_ALIGN_GRID ),
		( LPPOINT )&cRect, 2 );
	ShowWindow( hWnd, SW_HIDE );
	ObjAlign( &bRect, &cRect, lpAlignParms, vSpace, hSpace,
		&xOffset, &yOffset, &fx, &fy );
	OffsetRect( &cRect, xOffset, yOffset );
	MoveWindow( hWnd, cRect.left, cRect.top, RectWidth( &cRect ),
		RectHeight( &cRect ), TRUE );
	UpdateWindow( GetDlgItem( hDlg, IDC_ALIGN_GRID ) );
	ShowWindow( hWnd, SW_SHOWNA );
	}
}

#define NUM_GROUPS 128

/************************************************************************/
BOOL AlignObjects( LPIMAGE lpImage, LPALIGNOBJECTS_PARMS lpAlignParms )
/************************************************************************/
{
RECT		bRect, gRect, uRect;
LPOBJECT	lpObject, lpGroupObject;
int		xOffset, yOffset, gXOffset, gYOffset, nWidth, nHeight, nCount;
LFIXED	hSpace, vSpace, fx, fy;
WORD		wGroupIDs[NUM_GROUPS];

if( !( lpImage ) )
	return( FALSE );

if( !( lpObject = ImgGetBase( lpImage ) )	)
   return( FALSE );

if( !( lpAlignParms->bPreview ) )
	{
	if( !( ImgEditInit( lpImage, ET_SELOBJECTS, UT_OBJECTRECT, lpObject ) ) )
		return( FALSE );
	}

if( lpAlignParms->RectAlign == RA_IMAGE )
	{	
	ImgGetImageRect( lpImage, &bRect );
	}
else
	{	
	if( lpAlignParms->RectAlign == RA_MASK )
  		ImgGetMaskRect( lpImage, &bRect );
	else
		ImgGetSelObjectRect( lpImage, &bRect, YES );
	}

ImgGetSelObjectRect( lpImage, &uRect, YES );


nWidth = RectWidth( &bRect );
nHeight = RectHeight( &bRect );

nCount = 0;

InitGroupList( wGroupIDs );

while ( lpObject = ImgGetSelObject(lpImage, lpObject) )
   {
	if( lpObject->wGroupID )
		{
		if( !( IsPrevGroup( lpObject->wGroupID, wGroupIDs ) ) &&
			!( IsGroupLocked( lpObject->wGroupID ) ) )
			{
			SetGroupList( lpObject->wGroupID, wGroupIDs );
			lpImage->GetGroupRect( lpObject, &gRect );
			nWidth -= RectWidth( &gRect );
			nHeight -= RectHeight( &gRect );
			++nCount;
			}
		}
	else if (!lpObject->bLocked)
		{
		nWidth -= RectWidth( &lpObject->rObject );
		nHeight -= RectHeight( &lpObject->rObject );
		++nCount;
		}
	}

if( nCount == 0 )
	return( FALSE );

hSpace = FGET( nWidth, nCount - 1 );
vSpace = FGET( nHeight, nCount - 1 );

fx = fy = 0;

InitGroupList( wGroupIDs );

lpObject = ImgGetBase( lpImage );

while ( lpObject = ImgGetSelObject(lpImage, lpObject) )
   {
	if( lpObject->wGroupID )
		{
		if( !( IsPrevGroup( lpObject->wGroupID, wGroupIDs ) ) &&
			!( IsGroupLocked( lpObject->wGroupID ) ) )
			{
			SetGroupList( lpObject->wGroupID, wGroupIDs );
			lpImage->GetGroupRect( lpObject, &gRect );
			ObjAlign( &bRect, &gRect, lpAlignParms, vSpace, hSpace,
				&gXOffset, &gYOffset, &fx, &fy );
			lpGroupObject = ImgGetBase( lpImage );
			while( lpGroupObject = ImgGetSelObject( lpImage, lpGroupObject ) )
				{
				if( lpObject->wGroupID == lpGroupObject->wGroupID )
					{
					if( !( lpAlignParms->bPreview ) )
						ImgEditedObject( lpImage, lpGroupObject, IDS_UNDOOBJALIGN,
							NULL );
					OffsetRect( &lpGroupObject->rObject, gXOffset, gYOffset );
					}
				}
			}
		}
	else if (!lpObject->bLocked)
		{
		ObjAlign( &bRect, &lpObject->rObject, lpAlignParms, vSpace, hSpace,
			&xOffset, &yOffset, &fx, &fy ); 
		if( !( lpAlignParms->bPreview ) )
			ImgEditedObject( lpImage, lpObject, IDS_UNDOOBJALIGN, NULL );
		OffsetRect( &lpObject->rObject, xOffset, yOffset );
		}
   }
ImgGetSelObjectRect( lpImage, &bRect, YES );
AstralUnionRect( &uRect, &uRect, &bRect );
UpdateImage( lpImage, &uRect, YES );
return( TRUE );
}

/************************************************************************/
void ObjAlign( LPRECT bRect, LPRECT oRect, LPALIGNOBJECTS_PARMS lpAlignParms,
						LFIXED vSpace, LFIXED hSpace, LPINT xOffset, LPINT yOffset,
						LPLFIXED fx, LPLFIXED fy )
/************************************************************************/
{

switch( lpAlignParms->HorzAlign )
	{
	case HA_RIGHT:
		*xOffset = ( bRect->right - RectWidth( oRect ) ) -
			oRect->left;
	break;

	case HA_LEFT:
		*xOffset = bRect->left - oRect->left;
	break;

	case HA_CENTER:
		*xOffset = bRect->left + FINT( FGET( RectWidth( bRect ), 2 ) -
			FGET( RectWidth( oRect ), 2 ) ) -
			oRect->left;
	break;

	case HA_EQUAL:
		if( *fx == 0 )
			{
			*xOffset = bRect->left - oRect->left;
			*fx = TOFIXED( RectWidth( oRect ) + bRect->left )
				+ hSpace;
			}
		else
			{
			*xOffset = FINT( *fx ) - oRect->left;
			*fx += TOFIXED( RectWidth( oRect ) )
				+ hSpace;
			}
	break;

	case HA_NONE:
		*xOffset = 0;
	break;
	}

switch( lpAlignParms->VertAlign )
	{
	case VA_TOP:
		*yOffset = bRect->top - oRect->top;
	break;

	case VA_BOTTOM:
		*yOffset = ( bRect->bottom - RectHeight( oRect ) ) -
			oRect->top;
	break;

	case VA_MID:
		*yOffset = bRect->top + FINT( FGET( RectHeight( bRect ), 2 ) -
			FGET( RectHeight( oRect ), 2 ) ) -
			oRect->top;
	break;

	case VA_EQUAL:
		if( *fy == 0 )
			{
			*yOffset = bRect->top - oRect->top;
			*fy = TOFIXED( RectHeight( oRect ) + bRect->top )
				+ vSpace;
			}
		else
			{
			*yOffset = FINT( *fy ) - oRect->top;
			*fy += TOFIXED( RectHeight( oRect ) )
				+ vSpace;
			}
	break;

	case VA_NONE:
		*yOffset = 0;
	break;
	}
}

/************************************************************************/
BOOL IsPrevGroup( WORD wGroupID, LPWORD wGroupIDs )
/************************************************************************/
{
int	i;

for( i = 0; i < NUM_GROUPS; i++ )
	{
	if( wGroupID == wGroupIDs[i] )
		return( TRUE );
	}
return( FALSE );
}

/************************************************************************/
void InitGroupList( LPWORD wGroupIDs )
/************************************************************************/
{
int		i;

for( i = 0; i < NUM_GROUPS; i++ )
	wGroupIDs[i] = 0;

}

/************************************************************************/
BOOL IsGroupLocked( WORD wGroupID )
/************************************************************************/
{
LPOBJECT	lpObject;
LPIMAGE	lpImage;

lpImage = GetActiveImage();
lpObject = ImgGetBase( lpImage );

while( lpObject = ImgGetSelObject( lpImage, lpObject ) )
	{
	if( lpObject->wGroupID == wGroupID && lpObject->bLocked )
		return( TRUE );
	}
return( FALSE );

}

/************************************************************************/
void SetGroupList( WORD wGroupID, LPWORD wGroupIDs )
/************************************************************************/
{
int	i = 0;

while( wGroupIDs[i] != 0 && i < NUM_GROUPS )
	i++;
if( i < NUM_GROUPS )
	wGroupIDs[i] = wGroupID;
}

LPPOSOBJECTS_PARMS	lpPosParms;
/***********************************************************************/
BOOL WINPROC EXPORT DlgPosProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Pos_OnInitDialog);
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Pos_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_POSOBJECT);
	}
}

/************************************************************************/
LOCAL BOOL Pos_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
RECT     rSel;
LPIMAGE  lpImage;
LPFRAME	lpFrame;
LFIXED   XPos, YPos;
int 		res;

if(!(lpImage = GetActiveImage()))
	return(FALSE);

lpFrame = ImgGetBaseEditFrame( lpImage );
res = FrameResolution( lpFrame );
ImgGetSelObjectRect(lpImage, &rSel, YES);
XPos = FGET(rSel.left, res); 
YPos = FGET(rSel.top, res); 

CheckComboItem( hDlg, IDC_PREF_UNITS, IDC_PREF_UNITINCHES,
	IDC_PREF_UNITPIXELS,	IDC_PREF_UNITINCHES );

lpPosParms = (LPPOSOBJECTS_PARMS)lParam;

Edit.SaveUnits = Control.Units;

SetUnitResolution( 0 );

InitSpin( hDlg, XPos, YPos,3);

CenterPopup( hDlg );
return(TRUE);
}

/************************************************************************/
LOCAL void Pos_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
int						idUnits;
BOOL						Bool;
LFIXED					fx, fy;
LPIMAGE              lpImage;
int                  res;

switch (id)
	{
	case IDC_PREF_UNITS:
		if ( !(idUnits = HandleCombo( hDlg, id, codeNotify )) )
			break;
		fx = GetDlgItemSpinFixed( hDlg, IDC_POS_X, &Bool, YES);
		fy = GetDlgItemSpinFixed( hDlg, IDC_POS_Y, &Bool, YES);
		Control.Units = idUnits;
		SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
		InitSpin( hDlg, fx, fy,3);
	break;

   case IDC_POS_X:
	if (codeNotify == EN_KILLFOCUS)
		{
		fx = GetDlgItemSpinFixed( hDlg, IDC_POS_X, &Bool, YES);
		InitSpin( hDlg, fx, 0,1);
		}
	else
		{
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		GetDlgItemSpinFixed( hDlg, IDC_POS_X, &Bool, YES );
      }
   break;

   case IDC_POS_Y:
	if (codeNotify == EN_KILLFOCUS)
		{
		fy = GetDlgItemSpinFixed( hDlg, IDC_POS_Y, &Bool, YES);
		InitSpin( hDlg, 0, fy,2);
		}
	else
		{
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		GetDlgItemSpinFixed( hDlg, IDC_POS_Y, &Bool, YES );
      }
   break;

	case IDOK:
      if(lpImage = GetActiveImage())
         {
		   fx = GetDlgItemSpinFixed( hDlg, IDC_POS_X, &Bool, YES );
		   fy = GetDlgItemSpinFixed( hDlg, IDC_POS_Y, &Bool, YES );

	       ImgGetInfoEx(lpImage, NULL, NULL, NULL, NULL, &res);
		   Control.Units = Edit.SaveUnits;
		   // get pixels
		   lpPosParms->iBaseRes = res;
		   lpPosParms->XOffset = FMUL(res,fx);
		   lpPosParms->YOffset = FMUL(res,fy);
		   AstralDlgEnd( hDlg, TRUE );
         }
	   break;
	
	case IDCANCEL:
		Control.Units = Edit.SaveUnits;
		AstralDlgEnd( hDlg, FALSE );
	break;
	
   default:
	break;
   }
}

/************************************************************************/
BOOL PosObjects( LPIMAGE lpImage, LPPOSOBJECTS_PARMS lpPosParms )
/************************************************************************/
{
RECT		bRect, uRect;
LPOBJECT	lpObject;
int      res;
int      x,y;

if( !( lpImage ) )
	return( FALSE );

if( !( lpObject = ImgGetBase( lpImage ) )	)
   return( FALSE );

ImgEditInit( lpImage, ET_SELOBJECTS, UT_OBJECTRECT, lpObject );

ImgGetInfoEx(lpImage, NULL, NULL, NULL, NULL, &res);
x = lpPosParms->XOffset;
y = lpPosParms->YOffset;
ResConvertUL(lpPosParms->iBaseRes, res, &x, &y);

ImgGetSelObjectRect( lpImage, &bRect, YES );
x -= bRect.left;
y -= bRect.top;

while ( lpObject = ImgGetSelObject(lpImage, lpObject) )
   {
	if( lpObject->wGroupID )
		{
		if( !( IsGroupLocked( lpObject->wGroupID ) ) )
			{
  			ImgEditedObject( lpImage, lpObject, IDS_UNDOOBJPOS, NULL );
			OffsetRect( &lpObject->rObject, x, y);
			}
		}
	else if (!lpObject->bLocked)
		{
  		ImgEditedObject( lpImage, lpObject, IDS_UNDOOBJPOS, NULL );
		OffsetRect( &lpObject->rObject, x, y);
		}

   }
ImgGetSelObjectRect( lpImage, &uRect, YES );
AstralUnionRect( &uRect, &uRect, &bRect );
UpdateImage( lpImage, &uRect, YES );
return( TRUE );
}

/************************************************************************/
void InitSpin( HWND hDlg, LFIXED fSetX, LFIXED fSetY, int flag)
/************************************************************************/
{
LPIMAGE	lpImage;
LPFRAME	lpFrame;
LFIXED	fMin, fMax;
RECT		iRect;
int 		res;

if( !( lpImage = GetActiveImage() ) )
	return;
lpFrame = ImgGetBaseEditFrame( lpImage );

ImgGetImageRect( lpImage, &iRect );

res = FrameResolution( lpFrame );

if (flag & 1)
   {
   fMax = FGET( iRect.right, res );
   fMin = FGET( -iRect.right, res );
   InitDlgItemSpinFixed( hDlg, IDC_POS_X, fSetX, YES, fMin, fMax );
   }

if (flag & 2)
   {
   fMax = FGET( iRect.bottom, res );
   fMin = FGET( -iRect.bottom, res );
   InitDlgItemSpinFixed( hDlg, IDC_POS_Y, fSetY, YES, fMin, fMax );
   }
SetUnitLabels(hDlg);
}

BOOL UnGroupObjects(LPIMAGE lpImage, LPCMD_PARMS lpCmdParms)
   {
   LPOBJECT lpObject = NULL;

   if( !( lpImage ) )
	   return( FALSE );

   if( !( lpObject = ImgGetBase( lpImage ) )	)
      return( FALSE );

   ImgEditInit(lpImage, ET_SELOBJECTS, UT_GROUPOBJECTS, lpObject );
   while (lpObject = ImgGetSelObject(lpImage, lpObject))
      {
	   ImgEditedObject( lpImage, lpObject, IDS_UNDOUNGROUPOBJ, NULL );
      lpObject->wGroupID = 0;
      }
   return(TRUE);
   }

extern WORD ImgGetNextGroupID(LPIMAGE lpImage); // image.c
BOOL GroupObjects(LPIMAGE lpImage, LPCMD_PARMS lpCmdParms)
   {
   WORD wGroupID;
   LPOBJECT lpObject = NULL;

   if( !( lpImage ) )
	   return( FALSE );

   if( !( lpObject = ImgGetBase( lpImage ) )	)
      return( FALSE );

   ImgEditInit( lpImage, ET_SELOBJECTS, UT_GROUPOBJECTS, lpObject );
   wGroupID = ImgGetNextGroupID(lpImage);
   while (lpObject = ImgGetSelObject(lpImage, lpObject))
      {
	   ImgEditedObject( lpImage, lpObject, IDS_UNDOGROUPOBJ, NULL );
      lpObject->wGroupID = wGroupID;
      }
   return(TRUE);
   }

/************************************************************************/
BOOL LockObjects( LPIMAGE lpImage, LPCMD_PARMS lpCmdParms )
/************************************************************************/
{
LPOBJECT	lpObject;

if( !( lpImage ) )
	return( FALSE );

if( !( lpObject = ImgGetBase( lpImage ) )	)
   return( FALSE );

ImgEditInit( lpImage, ET_SELOBJECTS, UT_LOCKOBJECTS, lpObject );

while ( lpObject = ImgGetSelObject(lpImage, lpObject) )
   {
	ImgEditedObject( lpImage, lpObject, IDS_UNDOOBJLOCK, NULL );
	lpObject->bLocked = YES;
	}
return( TRUE );
}

/************************************************************************/
BOOL UnLockObjects( LPIMAGE lpImage, LPCMD_PARMS lpCmdParms )
/************************************************************************/
{
LPOBJECT	lpObject;

if( !( lpImage ) )
	return( FALSE );

if( !( lpObject = ImgGetBase( lpImage ) )	)
   return( FALSE );

ImgEditInit( lpImage, ET_SELOBJECTS, UT_LOCKOBJECTS, lpObject );

while ( lpObject = ImgGetSelObject(lpImage, lpObject) )
   {
	ImgEditedObject( lpImage, lpObject, IDS_UNDOOBJUNLOCK, NULL );
	lpObject->bLocked = NO;
	}
return( TRUE );
}

/************************************************************************/
BOOL CombineObjObjects( LPIMAGE lpImage, LPCMD_PARMS lpCmdParms )
/************************************************************************/
{
RECT		rCombine,r;
LPOBJECT	lpBase, lpObj;
LPALPHA	lpAlpha, lpMask;
LPFRAME	lpDFrame, lpSFrame;
LPTR		lpDF, lpDM, *lpSM, *lpSF;
int		x, y, yy, depth;
int      N, H, W, i;
double  *lpAlphaTable, *AiXOi;
double   red,blue,green,k;
POINT    pt;
double   Oi, Ai, Mi, Mc;
LPOBJECT *lpObject;
FRMTYPEINFO TypeInfo;
BOOL	fCanDoOpacity;
FRMDATATYPE MaskType = FDT_GRAYSCALE;

if (!(lpImage))
	return( FALSE );

ImgGetTypeInfo(lpImage, &TypeInfo);
if (TypeInfo.DataType == FDT_LINEART && Mask.OneBitMask)
   MaskType = FDT_LINEART;
fCanDoOpacity = CANDOTRANSPARENCY(TypeInfo.DataType);

lpObj = lpBase = ImgGetBase(lpImage);
if (!(depth = FrameDepth(ObjGetEditFrame(lpBase))))
   depth = 1; // never worry about lineart
N = ImgCountSelObjects(lpImage, NULL);
ImgGetSelObjectRect(lpImage, &rCombine, YES);
H = RectHeight(&rCombine);
W = RectWidth (&rCombine);

lpObject = NULL;
AiXOi = lpAlphaTable = NULL;

lpAlphaTable = (double *)Alloc((N+1) * sizeof(double));
AiXOi        = (double *)Alloc(N * sizeof(double));
lpSM         = (LPTR *)Alloc(N * sizeof(LPTR));
lpSF         = (LPTR *)Alloc(N * sizeof(LPTR));
lpObject     = (LPOBJECT *)Alloc(N * sizeof(LPOBJECT));
lpAlpha = MaskCreate(NULL, W, H, YES, Control.NoUndo, MaskType);
lpDFrame = ImgGetBaseEditFrame(lpImage);
lpDFrame = FrameOpen(FrameType(lpDFrame), W, H, FrameResolution(lpDFrame));

if (!lpAlphaTable || !lpObject || !AiXOi || !lpAlpha || !lpDFrame || !lpSM || !lpSF)
    {
    if (lpAlphaTable)
      FreeUp(lpAlphaTable);
    if (lpObject)
      FreeUp(lpObject);
    if (AiXOi)
      FreeUp(AiXOi);
    if (lpDFrame)
      FreeUp(lpDFrame);
    if (lpSF)
      FreeUp(lpSF);
    if (lpSM)
      FreeUp(lpSM);
      
    Message(IDS_EMEMALLOC);
    return(FALSE);
    }

for (i=0; i < N; i++)
   lpObj = lpObject[i] = ImgGetSelObject(lpImage, lpObj);

ImgEditInit(lpImage, ET_OBJECT, UT_DELETEOBJECTS, lpBase);

ProgressBegin(1, PROGRESS_ID(IDS_UNDOOBJCOMBOBJS));
for (y = 0; y <= H; y++)
   {
 	AstralClockCursor(y, H, NO);

   lpDF = FramePointer(lpDFrame, 0, y, YES);
   lpDM = PixmapPtr(&lpAlpha->Pixmap, PMT_EDIT, 0, y, YES);
   pt.y = y + rCombine.top;

   for (i=0; i < N; i++)
      {
      yy = pt.y - lpObject[i]->rObject.top;
      lpSFrame = ObjGetEditFrame(lpObject[i]);
      lpSF[i] = FramePointer(lpSFrame, 0, yy, NO);
      lpMask   = ObjGetAlpha(lpObject[i]);
      lpSM[i] = PixmapPtr(&lpMask->Pixmap, PMT_EDIT, 0, yy, NO);
      }
   
	for (x = 0; x < W; x++)
      {
      lpAlphaTable[N] = 1.0;
      pt.x = x + rCombine.left;

      for (i=N-1; i >= 0; i--)
         {
         r = lpObject[i]->rObject;
         r.bottom++;
         r.right++;
         if (PtInRect(&r, pt))
            {
			   if (fCanDoOpacity)
	           	Oi = lpObject[i]->Opacity / 255.0;
            else
					Oi = 1;
			   if (fCanDoOpacity)
	            Ai = *lpSM[i]++ / 255.0;
			   else if (*lpSM[i] < 128)
               {
               Ai = 0;
				   lpSM[i]++;
               }
			   else
			      {
				   Ai = 1;
				   lpSM[i]++;
			      }
            AiXOi[i] = Oi * Ai;
            lpAlphaTable[i] = lpAlphaTable[i+1] * (1.0 - AiXOi[i]);
            }
         else
            {
            AiXOi[i] = 1.0;
            lpAlphaTable[i] = lpAlphaTable[i+1];
            }
         }
      Mc = 1.0 - lpAlphaTable[0];
      lpDM[x] = (BYTE)(Mc * 255);

      red = blue = green = k = 0;
      for (i=0; i < N; i++)
         {
         r = lpObject[i]->rObject;
         r.bottom++;
         r.right++;
         if (PtInRect(&r, pt))
            {
            Mi = AiXOi[i] * lpAlphaTable[i+1];
            switch(depth)
               {
               case 1:
                  red     += *lpSF[i]++ * Mi;
                  break;
               case 3:
                  red     += *lpSF[i]++ * Mi;
                  green   += *lpSF[i]++ * Mi;
                  blue    += *lpSF[i]++ * Mi;
                  break;
               case 4:
                  red     += *lpSF[i]++ * Mi;
                  green   += *lpSF[i]++ * Mi;
                  blue    += *lpSF[i]++ * Mi;
                  k       += *lpSF[i]++ * Mi;
                  break;
               }
            }
         }

      if (Mc == 0.0)
         Mc = 1.0;
      switch(depth)
         {
         case 1:
            *lpDF++ = (BYTE)(red / Mc);
            break;
         case 3:
            *lpDF++ = (BYTE)(red   / Mc);
            *lpDF++ = (BYTE)(green / Mc);
            *lpDF++ = (BYTE)(blue  / Mc);
            break;
         case 4:
            *lpDF++ = (BYTE)(red   / Mc);
            *lpDF++ = (BYTE)(green / Mc);
            *lpDF++ = (BYTE)(blue  / Mc);
            *lpDF++ = (BYTE)(k     / Mc); 
            break;
         }
      }
   }

lpObj = ObjCreateFromFrame(ST_PERMANENT, lpDFrame, lpAlpha, &rCombine,
	Control.NoUndo );
lpObj->fSelected = YES;
lpObj->fUndoDeleted = YES;
ImgAddNewObject(lpImage, lpObj);
for (i=0; i < N; i++)
   lpObject[i]->fDeleted = YES;
ImgEditedObject(lpImage, lpBase, IDS_UNDOOBJCOMBOBJS, NULL);
UpdateImage(lpImage, &rCombine, YES);

FreeUp(lpSM);
FreeUp(lpSF);
FreeUp(AiXOi);
FreeUp(lpAlphaTable);
FreeUp(lpObject);
ProgressEnd();
return( TRUE );
}
