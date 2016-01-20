//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "maskutil.h"
#include "id.h"

#define SAMPLE_OFFSET   20
#define ALIGN_OFFSET    8
#define LUM_STEPS       5
#define LUM_OFFSET      20
#define LUM_RANGE       3
#define THRESHOLD       127

#define UNKNOWN_EDGE	0
#define LEFT_EDGE 	    1
#define RIGHT_EDGE	    2
#define TOP_EDGE	    3
#define BOTTOM_EDGE	    4

#define INFLATE_RECT	    10
#define MAX_STITCH_POINTS   2

#define INRECT(rect, x, y) \
	( (x) >= rect.left && (x) <= rect.right && \
	  (y) >= rect.top && (y) <= rect.bottom )

typedef struct _StitchPoints {
	int		iSlot;
	HWND	hWnd;
	FNAME	CurFile;
	FNAME	CacheFileName;
	int	iGrabbed;
	BOOL	fIsPoint[MAX_STITCH_POINTS];
	POINT	Pt[MAX_STITCH_POINTS];
} STITCHPOINTS;
typedef STITCHPOINTS far *LPSTITCHPOINTS;

class CStitchDrawPointsHook : public CHookHandler
{
public:
    CStitchDrawPointsHook() : CHookHandler() {};
    virtual void ImageHookProc(int x, int y, int dx, FRMTYPEINFO TypeInfo, LPTR lpSrc,
            LPTR lpDst, LPTR lpMsk, LPMASK lpMask) { /*no implementation*/ };
    virtual void DisplayHookProc(HWND hWnd, HDC hDC, LPRECT lpRect); 
protected:
private:
};
CStitchDrawPointsHook StitchDrawPointsHook;


LOCAL LPFRAME GetFrame(HWND hWnd);
LOCAL BOOL GetFileRect(HWND hWnd, LPRECT lpRect);
static STITCHPOINTS Curr, Last;

/***********************************************************************/
LOCAL LPFRAME GetFrame(HWND hWnd)
/***********************************************************************/
{
LPIMAGE lpImage;

if (!IsDoc(hWnd))
	return(NULL);
lpImage = (LPIMAGE)GetImagePtr ( hWnd );
return(ImgGetBaseEditFrame(lpImage));
}

/***********************************************************************/
LOCAL BOOL GetFileRect(HWND hWnd, LPRECT lpRect)
/***********************************************************************/
{
LPDISPLAY lpDisplay;

if (!IsDoc(hWnd))
	return(FALSE);
lpDisplay = GetDisplayPtr(hWnd);
*lpRect = lpDisplay->FileRect;
return(TRUE);
}

// LOCAL prototypes
LOCAL BOOL Stitch_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Stitch_OnDocActivated(HWND hDlg);
LOCAL void Stitch_OnDocChanged(HWND hDlg);
LOCAL void Stitch_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgStitchProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Stitch_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Stitch_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DOCACTIVATED, Stitch_OnDocActivated);
	HANDLE_DLGMSG(hDlg, WM_DOCCHANGED, Stitch_OnDocChanged);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_STITCH);
	}
}

/************************************************************************/
LOCAL BOOL Stitch_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
StitchValidate();
StitchDisplaySetup(YES);
StitchControlEnable(hDlg);
return(TRUE);
}

/************************************************************************/
LOCAL void Stitch_OnDocActivated(HWND hDlg)
/************************************************************************/
{
    StitchControlEnable(hDlg);
}

static BOOL bNoWindows = FALSE;
/************************************************************************/
LOCAL void Stitch_OnDocChanged(HWND hDlg)
/************************************************************************/
{
    HWND hActiveWnd = GetActiveDoc();
    if( hActiveWnd == NULL )
    {
        bNoWindows = TRUE;
        ActivateTool( Tool.idLast );
        return;
    }
    StitchValidate();
    StitchControlEnable( hDlg );
}

/************************************************************************/
LOCAL void Stitch_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
int	iSlot, iPoint;
HWND hActiveWnd;

hActiveWnd = GetActiveDoc();

switch (id)
	{
#ifndef ID_MAGIC
	case IDC_STITCH:
	if ( !SaveToolPreferences( id ) )
		break;
	break;
#endif

	case IDC_STITCHPOINT1:
	case IDC_STITCHPOINT2:
	if ( !hActiveWnd )
		break;
	StitchInit(hActiveWnd, -1);
	if (Curr.hWnd && Curr.fIsPoint[id-IDC_STITCHPOINT1])
		ViewXY(Curr.hWnd, Curr.Pt[id-IDC_STITCHPOINT1].x,
		Curr.Pt[id-IDC_STITCHPOINT1].y);
	break;

	case IDC_STITCHPOINT1C:
	case IDC_STITCHPOINT2C:
	case IDC_STITCHPOINT1S:
	case IDC_STITCHPOINT2S:
	if (id == IDC_STITCHPOINT1C || id == IDC_STITCHPOINT2C)
		{
		iSlot = id - IDC_STITCHPOINT1C;
		iPoint = 0;
		}
	else
		{
		iSlot = id - IDC_STITCHPOINT1S;
		iPoint = 1;
		}
	StitchPointButton(iSlot, iPoint);
	break;

 	case IDC_STITCHALIGN:
	if (StitchReady())
		DoStitchAutoAlign();
	break;

	case IDC_DOSTITCH:
	AstralDlg( NO, PictPubApp.GetResourceHandle(), hDlg, IDD_STITCHCONFIRM, DlgDoStitchProc );
	break;

   default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL DoStitch_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void DoStitch_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void DoStitch_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);

static int iBrightness;

/***********************************************************************/
BOOL WINPROC EXPORT DlgDoStitchProc( HWND hDlg, UINT msg, WPARAM wParam,
                                 LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, DoStitch_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, DoStitch_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_LBUTTONDBLCLK, DoStitch_OnLButtonDblClk);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_STITCHCONFIRM);
	}
}

/************************************************************************/
LOCAL BOOL DoStitch_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
LPSTR lpString;
STRING szTransform, szAnchor;
FNAME szName;

CenterPopup( hDlg );

Stitch.iBrightness = DoStitchGetLumDiff();
iBrightness = Stitch.iBrightness;
iBrightness = iBrightness < 0 ? -TOPERCENT(-iBrightness) :
				 TOPERCENT(iBrightness);
InitSlide( hDlg, IDC_STITCHBRIGHTNESS_SCROLL, iBrightness,
		-100, 100 );
SetDlgItemInt(hDlg, IDC_STITCHBRIGHTNESS, iBrightness, YES);
InitSlide( hDlg, IDC_STITCHPRESSURE_SCROLL, TOPERCENT(Stitch.Pressure),
		0, 100 );
SetDlgItemInt(hDlg, IDC_STITCHPRESSURE, TOPERCENT(Stitch.Pressure), NO);

CheckDlgButton( hDlg, IDC_AUTOBRIGHTNESS, Stitch.AutoBrightness );

// smartstitch can't be done on line art and palette colors
LPIMAGE lpImage;
FRMDATATYPE Type;
lpImage = GetActiveImage();
ImgGetInfo( lpImage, NULL, NULL, NULL, &Type );
CheckDlgButton( hDlg, IDC_SMARTSTITCH, 
                      CANDOTRANSPARENCY( Type ) && Stitch.SmartStitch );

CheckDlgButton( hDlg, IDC_STITCHBLEND, Stitch.AutoBlend );

if (AstralStr(IDS_STITCHTRANSFORM, &lpString))
	lstrcpy(szTransform, lpString);
if (AstralStr(IDS_STITCHANCHOR, &lpString))
	lstrcpy(szAnchor, lpString);
SetDlgItemText(hDlg, IDC_STITCHTRANS1,
	Stitch.iTransform == 0 ? szTransform : szAnchor );
SetDlgItemText(hDlg, IDC_STITCHTRANS2,
	Stitch.iTransform == 1 ? szTransform : szAnchor );
CheckRadioButton(hDlg, IDC_STITCHTRANS1, IDC_STITCHTRANS2,
	IDC_STITCHTRANS1 + Stitch.iTransform);
szName[0] = '\0';
if (Curr.hWnd && (Curr.fIsPoint[0] || Curr.fIsPoint[1]))
	lstrcpy(szName, stripdir(Curr.CurFile));
SetDlgItemText(hDlg, IDC_STITCHIMAGE1+Curr.iSlot, szName);
szName[0] = '\0';
if (Last.hWnd && (Last.fIsPoint[0] || Last.fIsPoint[1]))
	lstrcpy(szName, stripdir(Last.CurFile));
SetDlgItemText(hDlg, IDC_STITCHIMAGE1+Last.iSlot, szName);

ControlEnable(hDlg, IDC_STITCHPRESSURE_SCROLL, Stitch.AutoBlend);
ControlEnable(hDlg, IDC_STITCHPRESSURE, Stitch.AutoBlend);
ControlEnable(hDlg, IDC_STITCHBRIGHTNESS_SCROLL,
	Stitch.AutoBrightness );
ControlEnable(hDlg, IDC_STITCHBRIGHTNESS,
	Stitch.AutoBrightness );

ControlEnable( hDlg, IDC_SMARTSTITCH, CANDOTRANSPARENCY( Type ));

return(FALSE);
}

/************************************************************************/
LOCAL void DoStitch_OnLButtonDblClk(HWND hDlg, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/************************************************************************/
{
Stitch.iBrightness = DoStitchGetLumDiff();
iBrightness = Stitch.iBrightness;
iBrightness = iBrightness < 0 ? -TOPERCENT(-iBrightness) :
								 TOPERCENT(iBrightness);
SetSlide( hDlg, IDC_STITCHBRIGHTNESS_SCROLL, iBrightness );
SetDlgItemSpin( hDlg, IDC_STITCHBRIGHTNESS, iBrightness, YES );
}

/************************************************************************/
LOCAL void DoStitch_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
int i;
LPSTR lpString;
STRING szTransform, szAnchor;
LPIMAGE lpImage;
FRMDATATYPE Type;

lpImage = GetActiveImage();
ImgGetInfo( lpImage, NULL, NULL, NULL, &Type );

switch (id)
	{
	case IDC_STITCHBRIGHTNESS_SCROLL:
	i = HandleSlide( hDlg, id, codeNotify, NULL );
	if ( iBrightness == i )
		break;
	iBrightness = i;
	Stitch.iBrightness = i < 0 ? -TOGRAY(-i) : TOGRAY(i);
	SetDlgItemInt( hDlg, IDC_STITCHBRIGHTNESS, iBrightness, YES );
	break;

	case IDC_STITCHPRESSURE_SCROLL:
	i = HandleSlide( hDlg, id, codeNotify, NULL );
	i = TOGRAY(i);
	if ( Stitch.Pressure == i )
		break;
	Stitch.Pressure = i;
	SetDlgItemInt( hDlg, IDC_STITCHPRESSURE, TOPERCENT(Stitch.Pressure),
				 NO );
	break;

	case IDC_AUTOBRIGHTNESS:
	Stitch.AutoBrightness = !Stitch.AutoBrightness;
	CheckDlgButton( hDlg, id, Stitch.AutoBrightness );
	ControlEnable(hDlg, IDC_STITCHBRIGHTNESS_SCROLL, Stitch.AutoBrightness);
	ControlEnable(hDlg, IDC_STITCHBRIGHTNESS, Stitch.AutoBrightness );
	break;

	case IDC_SMARTSTITCH:
	Stitch.SmartStitch = !Stitch.SmartStitch;
    CheckDlgButton( hDlg, id, CANDOTRANSPARENCY( Type ) && Stitch.SmartStitch );
	break;

	case IDC_STITCHBLEND:
	Stitch.AutoBlend = !Stitch.AutoBlend;
	CheckDlgButton( hDlg, id, Stitch.AutoBlend );
	ControlEnable(hDlg, IDC_STITCHPRESSURE, Stitch.AutoBlend);
	ControlEnable(hDlg, IDC_STITCHPRESSURE_SCROLL, Stitch.AutoBlend);
	break;

	case IDC_STITCHTRANS1:
	case IDC_STITCHTRANS2:
	Stitch.iTransform = id - IDC_STITCHTRANS1;
	if (AstralStr(IDS_STITCHTRANSFORM, &lpString))
		lstrcpy(szTransform, lpString);
	if (AstralStr(IDS_STITCHANCHOR, &lpString))
		lstrcpy(szAnchor, lpString);
	SetDlgItemTextNow(hDlg, IDC_STITCHTRANS1,
		Stitch.iTransform == 0 ? szTransform : szAnchor );
	SetDlgItemTextNow(hDlg, IDC_STITCHTRANS2,
		Stitch.iTransform == 1 ? szTransform : szAnchor );
	CheckRadioButton(hDlg, IDC_STITCHTRANS1, IDC_STITCHTRANS2,
		IDC_STITCHTRANS1 + Stitch.iTransform);
	Stitch.iBrightness = -Stitch.iBrightness;
	iBrightness = -iBrightness;
	SetSlide( hDlg, IDC_STITCHBRIGHTNESS_SCROLL, iBrightness );
	SetDlgItemSpin( hDlg, IDC_STITCHBRIGHTNESS, iBrightness, YES );
	break;

	case IDOK:
	if ( StitchReady() )
		{
		DoStitch();
		}
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	AstralDlgEnd( hDlg, FALSE );
	break;

   default:
	break;
   }
}

static HCURSOR hCursor;
/************************************************************************/
void StitchProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
    static BOOL bFirst;

    switch (msg)
    {
	    case WM_CREATE:	// The first mouse down message
        StitchInit(hWindow, -1);
	    StitchControlEnable(NULL);
	    bFirst = YES;
	    hCursor = NULL;
	    Tool.bActive = YES;
	    break;

	    case WM_ACTIVATE:
        if( bNoWindows )    // if no windows, reset flag and bail out
        {
            bNoWindows = FALSE; 
            break;
        }
	    if (!Option)
		    StitchDisplaySetup(NO);
	    break;

	    case WM_LBUTTONDOWN:
	    StitchMouseDown(hWindow, x, y, bFirst);
	    if (Curr.iGrabbed >= 0)
		    hCursor = SetCursor(Window.hNullCursor);
	    bFirst = NO;
	    StitchControlEnable(NULL);
	    break;

	    case WM_LBUTTONUP:
	    if (hCursor)
	    {
		    SetCursor(hCursor);
		    hCursor = NULL;
	    }
	    StitchMouseUp(hWindow, x, y);
	    Tool.bActive = NO;
	    break;

	    case WM_MOUSEMOVE:	// sent when ToolActive is on
	    StitchMouseMove(hWindow, x, y);
	    break;

	    case WM_DESTROY:	// The cancel operation message
	    /* fall through to ending function */

	    case WM_LBUTTONDBLCLK:
	    if (hCursor)
	    {
		    SetCursor(hCursor);
		    hCursor = NULL;
	    }
	    StitchDone(hWindow);
	    Tool.bActive = NO;
	    break;
    }
}

/************************************************************************/
void StitchControlEnable( HWND hDlg )
/************************************************************************/
{
FNAME	szName;
LPSTITCHPOINTS lpPt;

if ( !hDlg )
	if( !(hDlg = AstralDlgGet( IDD_STITCH )))
		return;

HWND hActiveWnd = GetActiveDoc();
if( hActiveWnd == NULL )
	lpPt = NULL;
else
if (hActiveWnd == Curr.hWnd)
	lpPt = &Curr;
else if (hActiveWnd == Last.hWnd)
	lpPt = &Last;
else
	lpPt = NULL;

ControlEnable( hDlg, IDC_STITCHPOINT1, lpPt && lpPt->fIsPoint[0]);
ControlEnable( hDlg, IDC_STITCHPOINT2, lpPt && lpPt->fIsPoint[1]);

if (Curr.hWnd /*&& (Curr.fIsPoint[0] || Curr.fIsPoint[1])*/)
		lstrcpy( szName, stripdir(Curr.CurFile) );
else	lstrcpy( szName, "*********" );
SetDlgItemText(hDlg, IDC_STITCHIMAGE1+Curr.iSlot, szName);

if (Last.hWnd /*&& (Last.fIsPoint[0] || Last.fIsPoint[1])*/)
		lstrcpy( szName, stripdir(Last.CurFile) );
else	lstrcpy( szName, "*********" );
SetDlgItemText(hDlg, IDC_STITCHIMAGE1+Last.iSlot, szName);

//Fix the Stitch status icon
CheckDlgButton( hDlg, IDC_STITCHPOINT1C + Curr.iSlot,
	Curr.hWnd && Curr.fIsPoint[0] );
CheckDlgButton( hDlg, IDC_STITCHPOINT1S + Curr.iSlot,
	Curr.hWnd && Curr.fIsPoint[1] );
CheckDlgButton( hDlg, IDC_STITCHPOINT1C + Last.iSlot,
	Last.hWnd && Last.fIsPoint[0] );
CheckDlgButton( hDlg, IDC_STITCHPOINT1S + Last.iSlot,
	Last.hWnd && Last.fIsPoint[1] );
ControlEnable( hDlg, IDC_STITCHPOINT1C + Curr.iSlot,
	Curr.hWnd || (hActiveWnd != Last.hWnd));
ControlEnable( hDlg, IDC_STITCHPOINT1S + Curr.iSlot,
	Curr.hWnd || (hActiveWnd != Last.hWnd));
ControlEnable( hDlg, IDC_STITCHPOINT1C + Last.iSlot,
	Last.hWnd || (hActiveWnd != Curr.hWnd));
ControlEnable( hDlg, IDC_STITCHPOINT1S + Last.iSlot,
	Last.hWnd || (hActiveWnd != Curr.hWnd));

ControlEnable(hDlg, IDC_STITCHALIGN, StitchReady());
ControlEnable(hDlg, IDC_DOSTITCH, StitchReady());
}

/************************************************************************/
void StitchInit( HWND hWindow, int iSlot )
/************************************************************************/
{
STITCHPOINTS temp;
LPIMAGE lpTheImage, lpCurrImage, lpLastImage;
int	i;

// Validate any window handles we may already have
StitchValidate();

// Erase all points we currently have displayed
StitchDisplaySetup(NO);

lpTheImage = (LPIMAGE)GetImagePtr ( hWindow );
// if the window is not one we already have, see if it's the same image
if (hWindow != Curr.hWnd && hWindow != Last.hWnd)
	{
	if (Curr.hWnd)
		{
        lpCurrImage = (LPIMAGE)GetImagePtr ( Curr.hWnd );
		if (lpTheImage == lpCurrImage)
			Curr.hWnd = hWindow;
		}
	if (!Curr.hWnd && Last.hWnd)
		{
        lpLastImage = (LPIMAGE)GetImagePtr ( Last.hWnd );
		if (lpTheImage == lpLastImage)
			Last.hWnd = hWindow;
		}
	}

if (!Curr.hWnd && !Last.hWnd)
	{
	if (iSlot >= 0)
		Curr.iSlot = iSlot;
	else
		Curr.iSlot = 0;
	Last.iSlot = iSlot == 1 ? 0 : 1;
	}

// if window was worked with before, swap with current
if (hWindow == Last.hWnd)
	{
	temp = Curr;
	Curr = Last;
	Last = temp;
	}
// if window is not current, make current last and this current
else if (hWindow != Curr.hWnd)
	{
	if (Curr.hWnd && Last.hWnd) // we had two and now we have a new one
		{
		Last.hWnd = NULL; // throw out last one, make user start again
		}
	else if (Curr.hWnd)
		{
		temp = Curr;
		Curr = Last;
		Last = temp;
		}
	Curr.hWnd = hWindow;
	lstrcpy(Curr.CurFile, lpTheImage->CurFile);
	lstrcpy(Curr.CacheFileName, FrameName(ImgGetBaseEditFrame(lpTheImage)));
	for (i = 0; i < MAX_STITCH_POINTS; ++i)
		{
		Curr.Pt[i].x = -1;
		Curr.Pt[i].y = -1;
		Curr.fIsPoint[i] = NO;
		}
	}
if (!Last.hWnd)
	{
	if (iSlot >= 0)
		Curr.iSlot = iSlot;
	else
		Curr.iSlot = 0;
	Last.iSlot = iSlot == 1 ? 0 : 1;
	}
	
StitchDisplaySetup(YES);
Curr.iGrabbed = -1;
}

/************************************************************************/
void StitchDone( HWND hWindow )
/************************************************************************/
{
    Curr.iGrabbed = -1;
}

/************************************************************************/
void StitchValidate()
/************************************************************************/
{
    LPIMAGE lpCurrImage, lpLastImage;

    // Validate any window handles we may already have
    if (Last.hWnd)
	{
 	    if (!IsDoc(Last.hWnd))
		    Last.hWnd = NULL;
	    else
	    {
            lpLastImage = (LPIMAGE)GetImagePtr ( Last.hWnd );
		    if (lstrcmp(FrameName(ImgGetBaseEditFrame(lpLastImage)),
					    Last.CacheFileName) != 0)
			    Last.hWnd = NULL;
        }
	}
    if (Curr.hWnd)
	{
 	    if (!IsDoc(Curr.hWnd))
		    Curr.hWnd = NULL;
	    else
	    {
            lpCurrImage = (LPIMAGE)GetImagePtr ( Curr.hWnd );
		    if (lstrcmp(FrameName(ImgGetBaseEditFrame(lpCurrImage)), Curr.CacheFileName) != 0)
			    Curr.hWnd = NULL;
	    }
	}
    if(( !Last.hWnd && !Curr.hWnd ) || ( Curr.iSlot == Last.iSlot ))
    {
 	    Curr.iSlot = 0;
	    Last.iSlot = 1;
    }
}

/************************************************************************/
BOOL StitchReady()
/************************************************************************/
{
if (!GetActiveDoc())
	return(NO);
if (!IsDoc(Curr.hWnd) || !IsDoc(Last.hWnd))
	return(NO);
if (!Curr.fIsPoint[0] || !Curr.fIsPoint[1] ||
	!Last.fIsPoint[0] || !Last.fIsPoint[1])
	return(NO);
return(YES);
}

/************************************************************************/
void StitchDisplaySetup( BOOL fEnable )
/************************************************************************/
{
StitchDrawPoints(Curr.hWnd, 0, fEnable, -1);
StitchDrawPoints(Last.hWnd, 0, fEnable, -1);
if (Curr.hWnd && IsDoc(Curr.hWnd))
	SetDisplayHook(Curr.hWnd, fEnable ? &StitchDrawPointsHook :
					NULL);
if (Last.hWnd && IsDoc(Last.hWnd))
	SetDisplayHook(Last.hWnd, fEnable ? &StitchDrawPointsHook :
					NULL);
}

/************************************************************************/
void StitchMouseDown( HWND hWindow, int x, int y, BOOL bFirst )
/************************************************************************/
{
int	i, nPoints;
RECT rect;
long lD0, lD1, lDx, lDy;

// see if we are on top of any of our markers
Curr.iGrabbed = -1;
for (i = 0; i < MAX_STITCH_POINTS; ++i)
	{
	if (!Curr.fIsPoint[i])
		continue;
	rect.left = rect.right = Curr.Pt[i].x;
	rect.top = rect.bottom = Curr.Pt[i].y;
	File2DispRect(hWindow, &rect, &rect);
	InflateRect(&rect, INFLATE_RECT, INFLATE_RECT);
	if (x >= rect.left && x <= rect.right &&
		y >= rect.top && y <= rect.bottom)
		{
		Curr.iGrabbed = i;
		StitchDrawPoints(hWindow, Window.hDC, OFF, Curr.iGrabbed);
		Display2File(hWindow, &x, &y);
		Curr.Pt[Curr.iGrabbed].x = x;
		Curr.Pt[Curr.iGrabbed].y = y;
		StitchDrawPoints(hWindow, Window.hDC, ON, Curr.iGrabbed);
		return;
		}
	}

nPoints = 0;
for (i = 0; i < MAX_STITCH_POINTS; ++i)
	if (Curr.fIsPoint[i])
		++nPoints;

if (/*bFirst && */nPoints == MAX_STITCH_POINTS)
	return;

// reset the slots if no points are currently being displayed
if (!Last.hWnd)
	{
	Curr.iSlot = 0;
	Last.iSlot = 1;
	}

Display2File(hWindow, &x, &y);
if (nPoints == MAX_STITCH_POINTS)
	{
	lDx = x - Curr.Pt[0].x;
	lDy = y - Curr.Pt[0].y;
	lD0 = (lDx*lDx) + (lDy*lDy);
	lDx = x - Curr.Pt[1].x;
	lDy = y - Curr.Pt[1].y;
	lD1 = (lDx*lDx) + (lDy*lDy);
	if (lD0 < lD1)
		Curr.iGrabbed = 0;
	else
		Curr.iGrabbed = 1;
	StitchDrawPoints(hWindow, Window.hDC, OFF, Curr.iGrabbed);
	}
else if (!Curr.fIsPoint[0])
	Curr.iGrabbed = 0;
else
	Curr.iGrabbed = 1;
Curr.Pt[Curr.iGrabbed].x = x;
Curr.Pt[Curr.iGrabbed].y = y;
Curr.fIsPoint[Curr.iGrabbed] = YES;
StitchDrawPoints(hWindow, Window.hDC, ON, Curr.iGrabbed);
}

/************************************************************************/
void StitchMouseUp( HWND hWindow, int x, int y )
/************************************************************************/
{
Curr.iGrabbed = -1;
}

/************************************************************************/
void StitchMouseMove( HWND hWindow, int x, int y )
/************************************************************************/
{
RECT	ClientRect;

if (Curr.iGrabbed >= 0)
	{
	ClientRect = GetDisplayPtr(hWindow)->DispRect;
	x = bound( x, ClientRect.left, ClientRect.right );
	y = bound( y, ClientRect.top, ClientRect.bottom );

	StitchDrawPoints(hWindow, Window.hDC, OFF, Curr.iGrabbed);
	Display2File(hWindow, &x, &y);
	Curr.Pt[Curr.iGrabbed].x = x;
	Curr.Pt[Curr.iGrabbed].y = y;
	StitchDrawPoints(hWindow, Window.hDC, ON, Curr.iGrabbed);
	}
}

/************************************************************************/
void CStitchDrawPointsHook::DisplayHookProc(HWND hWnd, HDC hDC, LPRECT lpRect )
/************************************************************************/
{
StitchDrawPoints(hWnd, hDC, ON, -1);
}

/************************************************************************/
void StitchDrawPoints( HWND hWnd, HDC hInDC, BOOL on, int index )
/************************************************************************/
{
HDC	hDC;
RECT rect, irect;
STITCHPOINTS temp;
int pt, width, height, dx, dy;

if (!hWnd)
	return;

if (hWnd == Curr.hWnd)
	temp = Curr;
else if (hWnd == Last.hWnd)
	temp = Last;
else
	return;

if (hInDC)
	hDC = hInDC;
else
	hDC = GetDC(hWnd);
for (pt = 0; pt < MAX_STITCH_POINTS; ++pt)
	{
	if (!temp.fIsPoint[pt])
		continue;
	if (pt == index || index < 0)
		{
		rect.left = rect.right = temp.Pt[pt].x;
		rect.top = rect.bottom = temp.Pt[pt].y;
		File2DispRectEx(hWnd, &rect, &rect);
		Drect(hDC, &rect, on);
		InflateRect(&rect, 1, 1);
		irect = rect;
		InflateRect(&irect, INFLATE_RECT-2, INFLATE_RECT-2);
		if (RectWidth(&irect) & 1)
			irect.right++;
		if (RectHeight(&irect) & 1)
			irect.bottom++;
		if (pt == 0)
			{
			width = RectWidth(&irect) / 2;
			height = RectHeight(&irect) / 2;
			dx = width - (((width * 100)+70) / 141) - 1;
			dy = height - (((height * 100)+70) / 141) - 1;
			InflateRect(&irect, -dx, -dy);
			}
		Dline(hDC,rect.left,rect.top,
			irect.left,irect.top,on);
		Dline(hDC,rect.right,rect.top,
			irect.right,irect.top,on);
		Dline(hDC,rect.right,rect.bottom,
			irect.right,irect.bottom,on);
		Dline(hDC,rect.left,rect.bottom,
			irect.left,irect.bottom,on);
//			InflateRect(&irect, 1, 1);
		if (pt == 0)
			{
			InflateRect(&irect, dx, dy);
			Dellipse(hDC, &irect, on);
			}
		else
			Drect(hDC, &irect, on);
		}
	}
if (!hInDC)
	ReleaseDC(hWnd, hDC);
}

/************************************************************************/
BOOL DoStitch()
/************************************************************************/
{
    LPFRAME lpCurrFrame, lpLastFrame, lpNewFrame;
    int	FileType, DataType;
    LPIMAGE lpImage;
    FRMDATATYPE FrameDataType;

    if (!StitchReady())
	    return(FALSE);
    if (!(lpLastFrame = GetFrame(Last.hWnd)))
	    return(FALSE);
    if (!(lpCurrFrame = GetFrame(Curr.hWnd)))
	    return(FALSE);

    lpImage = GetActiveImage();
    FileType = lpImage->FileType;
    DataType = lpImage->DataType;
	ImgGetInfo(lpImage, NULL, NULL, NULL, &FrameDataType);

    if (Last.iSlot == Stitch.iTransform)
    {
	    lpNewFrame = StitchImages(lpCurrFrame, Curr.Pt[0], Curr.Pt[1],
			    lpLastFrame, Last.Pt[0], Last.Pt[1],
			    Stitch.AutoBrightness ? Stitch.iBrightness : 0,
			    Stitch.AutoBlend ? Stitch.Pressure : 0,
			    CANDOTRANSPARENCY( FrameDataType ) && Stitch.SmartStitch);
    }
    else
    {
	    lpNewFrame = StitchImages(lpLastFrame, Last.Pt[0], Last.Pt[1],
			    lpCurrFrame, Curr.Pt[0], Curr.Pt[1],
			    Stitch.AutoBrightness ? Stitch.iBrightness : 0,
			    Stitch.AutoBlend ? Stitch.Pressure : 0,
			    CANDOTRANSPARENCY( FrameDataType ) && Stitch.SmartStitch);
    }
    if (lpNewFrame)
    {
	    // Deactivate the stitching tool
	    DeactivateTool();

	    // Create the new image window
        LPIMAGE lpNewImage = CreateImage(NULL, lpNewFrame, NULL, 
        NULL, FileType, DataType, IMG_DOCUMENT,NULL);
        if (lpNewImage)
        {
            if (PictPubApp.OpenDocumentFile((LPSTR)lpNewImage->CurFile, lpNewImage))         
            {
                lpNewImage->fChanged = TRUE;
                lpNewImage->fUntitled = YES;
				PictPubApp.DocumentChanged(lpNewImage);
            }
            else
                DestroyImage(lpNewImage);
        }
    }
    return(lpNewFrame != NULL);
}

/************************************************************************/
LPFRAME StitchImages( LPFRAME lpFrame1, POINT p1_1, POINT p2_1,
                      LPFRAME lpFrame2, POINT p1_2, POINT p2_2,
                      BOOL iAdjustLum, int BlendPressure,
                      BOOL fSmartStitch )
/************************************************************************/
// Underneath frame
// point 1 on frame 1 (corresponds to p1_2)
// point 2 on frame 1 (corresponds to p2_2)
// On top frame - this one gets transformed
// point 1 on frame 2 (corresponds to p1_1)
// point 2 on frame 2 (corresponds to p2_1)
// Brightness Adjustment
// 0 - no blending (IDC_PRESSLIGHT...IDC_PRESSHEAVY)
// User interpolation
{
	TFORM	tform;
	int	iWhichEdge;
	int	lum = 0;
	LPFRAME	lpNewFrame = NULL;
	
	ProgressBegin(BlendPressure ? 2 : 1, PROGRESS_ID(IDS_UNDOSTITCH));
	// make sure all input parameters are valid for stitching
	if (!StitchValidatePoints(lpFrame1, &p1_1, &p2_1,
		lpFrame2, &p1_2, &p2_2, &iWhichEdge, NO))
		goto Exit;	
	// Create the transform for stitching lpFrame2 to lpFrame1
	StitchCreateTransform(&tform, p1_1, p2_1, p1_2, p2_2);
	
	// Stitch the frames together
	if (!(lpNewFrame = StitchFrames(&tform, lpFrame1, lpFrame2, iAdjustLum,
		fSmartStitch)))
		goto Exit;	
	
	// Blend the seam if desired
	if (BlendPressure)
		StitchBlend(&tform, lpNewFrame, lpFrame2, iWhichEdge, BlendPressure);
	Exit:
	ProgressEnd();
	return(lpNewFrame);
}

/************************************************************************/
int DoStitchGetLumDiff()
/************************************************************************/
{
LPFRAME lpCurrFrame, lpLastFrame;
POINT	p1_1, p2_1, p1_2, p2_2;
int	iWhichEdge, lum;

if (!GetActiveDoc())
	return(0);
if (!Curr.fIsPoint[0] || !Curr.fIsPoint[1] ||
	!Last.fIsPoint[0] || !Last.fIsPoint[1])
	return(0);
if (!(lpCurrFrame = GetFrame(Curr.hWnd)))
	return(0);
if (!(lpLastFrame = GetFrame(Last.hWnd)))
	return(0);

p1_1 = Curr.Pt[0];
p2_1 = Curr.Pt[1];
p1_2 = Last.Pt[0];
p2_2 = Last.Pt[1];
if (Last.iSlot == Stitch.iTransform)
	{
	if (StitchValidatePoints(lpCurrFrame, &p1_1, &p2_1,
		lpLastFrame, &p1_2, &p2_2, &iWhichEdge, NO))
		lum = StitchGetLumDiff(lpCurrFrame, p1_1, p2_1,
				lpLastFrame, p1_2, p2_2);
	else
		lum = 0;
	}
else
	{
	if (StitchValidatePoints(lpLastFrame, &p1_2, &p2_2,
		lpCurrFrame, &p1_1, &p2_1, &iWhichEdge, NO))
		lum = StitchGetLumDiff(lpLastFrame, p1_2, p2_2,
				lpCurrFrame, p1_1, p2_1);
	else
		lum = 0;
	}
return(lum);
}

/************************************************************************/
void StitchPointButton( int iSlot, int iPoint )
/************************************************************************/
{
HWND hActiveWnd;

hActiveWnd = GetActiveDoc();
if (Last.hWnd && Last.iSlot == iSlot)
	StitchInit(Last.hWnd, iSlot);
else if (hActiveWnd && (!Curr.hWnd || Curr.iSlot != iSlot))
	StitchInit(hActiveWnd, iSlot);
if (Curr.hWnd && Curr.iSlot == iSlot)
	{
	if (Curr.fIsPoint[iPoint])
		{
		StitchDrawPoints(Curr.hWnd, NULL, OFF, iPoint);
		Curr.fIsPoint[iPoint] = NO;
		}
	else
		{
		if (Curr.Pt[iPoint].x >= 0)
			{
			Curr.fIsPoint[iPoint] = YES;
			StitchDrawPoints(Curr.hWnd, NULL, ON, iPoint);
			}
		else
			{
			DoStitchMovePoint(iPoint);
			}
		}
	StitchControlEnable(NULL);
	}
}

/************************************************************************/
void DoStitchMovePoint( int iPoint )
/************************************************************************/
{
LPSTITCHPOINTS lpThisPt, lpOtherPt;
int	iOtherPoint, y, midy;
RECT	rFile, rOther;
HWND hActiveWnd;

hActiveWnd = GetActiveDoc();
if (!hActiveWnd)
	return;
if (hActiveWnd == Curr.hWnd)
	{
	lpThisPt = &Curr;
	lpOtherPt = &Last;
	}
else if (hActiveWnd == Last.hWnd)
	{
	lpThisPt = &Last;
	lpOtherPt = &Curr;
	}
else
	{
	return;
	}

GetFileRect(hActiveWnd, &rFile);
if (!lpThisPt->fIsPoint[iPoint] ||
	!INRECT(rFile, lpThisPt->Pt[iPoint].x, lpThisPt->Pt[iPoint].y))
	{
	iOtherPoint = (iPoint == 0 ? 1 : 0);
	if (lpThisPt->fIsPoint[iOtherPoint] &&
		INRECT(rFile, lpThisPt->Pt[iOtherPoint].x,
		lpThisPt->Pt[iOtherPoint].y))
		{
		midy = ((long)rFile.top+(long)rFile.bottom)/2L;
		if (lpThisPt->Pt[iOtherPoint].y < midy)
			y = rFile.top + ((4L*RectHeight(&rFile))/5L);
		else
			y = rFile.top + ((1L*RectHeight(&rFile))/5L);
		}
	else if (lpOtherPt->hWnd && lpOtherPt->fIsPoint[iPoint] &&
		GetFileRect(lpOtherPt->hWnd, &rOther))
		{
		midy = ((long)rOther.top+(long)rOther.bottom)/2L;
		if (lpOtherPt->Pt[iPoint].y < midy)
			y = rFile.top + ((1L*RectHeight(&rFile))/5L);
		else
			y = rFile.top + ((4L*RectHeight(&rFile))/5L);
		}
	else
		{
		if (iPoint == 0)
			y = rFile.top + ((1L*RectHeight(&rFile))/5L);
		else
			y = rFile.top + ((4L*RectHeight(&rFile))/5L);
		}
	StitchDrawPoints(lpThisPt->hWnd, NULL, OFF, iPoint);
	lpThisPt->Pt[iPoint].x = ((long)rFile.left+(long)rFile.right)/2L;
	lpThisPt->Pt[iPoint].y = y;
	lpThisPt->fIsPoint[iPoint] = YES;
	StitchDrawPoints(lpThisPt->hWnd, NULL, ON, iPoint);
	}
}

/************************************************************************/
void DoStitchAutoAlign()
/************************************************************************/
{
LPFRAME lpCurrFrame, lpLastFrame;
BOOL fCurrent;
HWND hActiveWnd;

hActiveWnd = GetActiveDoc();
if (!hActiveWnd)
	return;
if (hActiveWnd != Curr.hWnd && hActiveWnd != Last.hWnd)
	return;
if (!Curr.fIsPoint[0] || !Curr.fIsPoint[1] ||
	!Last.fIsPoint[0] || !Last.fIsPoint[1])
	return;
fCurrent = hActiveWnd == Curr.hWnd;
if (!(lpCurrFrame = GetFrame(Curr.hWnd)))
	return;
if (!(lpLastFrame = GetFrame(Last.hWnd)))
	return;

AstralCursor(IDC_WAIT);
if (fCurrent)
	{
	StitchDrawPoints(Curr.hWnd, 0, OFF, -1);
	StitchAutoAlign(lpLastFrame, &Last.Pt[0], &Last.Pt[1],
			lpCurrFrame, &Curr.Pt[0], &Curr.Pt[1]);
	StitchDrawPoints(Curr.hWnd, 0, ON, -1);
	}
else
	{
	StitchDrawPoints(Last.hWnd, 0, OFF, -1);
	StitchAutoAlign(lpCurrFrame, &Curr.Pt[0], &Curr.Pt[1],
			lpLastFrame, &Last.Pt[0], &Last.Pt[1]);
	StitchDrawPoints(Last.hWnd, 0, ON, -1);
	}
AstralCursor(NULL);
}

/************************************************************************/
BOOL StitchAutoAlign( LPFRAME lpFrame1, LPPOINT lpP1_1, LPPOINT lpP2_1,
			               LPFRAME lpFrame2, LPPOINT lpP1_2, LPPOINT lpP2_2 )
/************************************************************************/
{
int	iWhichEdge;

// make sure all input parameters are valid for stitching
if (!StitchValidatePoints(lpFrame1, lpP1_1, lpP2_1,
	lpFrame2, lpP1_2, lpP2_2, &iWhichEdge, YES))
	return(FALSE);

// Adjust input points to places where we "think" they
// correspond to each other better. (i.e. - move p1_2 so that
// p1_1 and p1_2 are the same spots in the two frames)
StitchAdjustPoints(lpFrame1, lpP1_1, lpP2_1,
	lpFrame2, lpP1_2, lpP2_2, iWhichEdge);

return(TRUE);
}

/************************************************************************/
BOOL StitchValidatePoints( LPFRAME lpFrame1, LPPOINT lpP1_1, LPPOINT lpP2_1,
                           LPFRAME lpFrame2, LPPOINT lpP1_2, LPPOINT lpP2_2,
                           LPINT lpWhichEdge, BOOL fNoSwap )
/************************************************************************/
{
int	closest1, closest2;
int 	top1, bottom1, left1, right1;
int 	top2, bottom2, left2, right2;
int 	dx1, dx2, dy1, dy2, dx, dy;
POINT	temp;

// make sure we have some frames
if (!lpFrame1 || !lpFrame2)
	return(FALSE);

// proper depth not set for lineart
if( lpFrame1->Depth < 1 )
    lpFrame1->Depth = 1;
if( lpFrame2->Depth < 1 )
    lpFrame2->Depth = 1;

if (lpFrame1->Depth != lpFrame2->Depth)
	{
	Message(IDS_STITCHDEPTH);
	return(FALSE);
	}

// find out which edge the two points on lpFrame1 are closest to
top1 = max(lpP1_1->y, lpP2_1->y);
bottom1 = max(FrameYSize(lpFrame1)-lpP1_1->y-1, FrameYSize(lpFrame1)-lpP2_1->y-1);
left1 = max(lpP1_1->x, lpP2_1->x);
right1 = max(FrameXSize(lpFrame1)-lpP1_1->x-1, FrameXSize(lpFrame1)-lpP2_1->x-1);
closest1 = min(top1, min(bottom1, min(left1, right1)));

// find out which edge the two points on lpFrame2 are closest to
top2 = max(lpP1_2->y, lpP2_2->y);
bottom2 = max(FrameYSize(lpFrame2)-lpP1_2->y-1, FrameYSize(lpFrame2)-lpP2_2->y-1);
left2 = max(lpP1_2->x, lpP2_2->x);
right2 = max(FrameXSize(lpFrame2)-lpP1_2->x-1, FrameXSize(lpFrame2)-lpP2_2->x-1);
closest2 = min(top2, min(bottom2, min(left2, right2)));

// stitching left and right sides?
if ((closest1 == right1 && closest2 == left2) ||
	 (closest2 == right2 && closest1 == left1))
	{
	if (closest2 == right2)
		*lpWhichEdge = RIGHT_EDGE;
	else
		*lpWhichEdge = LEFT_EDGE;
	}
// stitching top and bottom sides?
else if ((closest1 == bottom1 && closest2 == top2) ||
	(closest2 == bottom2 && closest1 == top1))
	{
	if (closest2 == bottom2)
		*lpWhichEdge = BOTTOM_EDGE;
	else
		*lpWhichEdge = TOP_EDGE;
	}
else
	{
	*lpWhichEdge = UNKNOWN_EDGE;
	}

dx = abs(lpP1_1->x - lpP2_1->x);
dy = abs(lpP1_1->y - lpP2_1->y);
if (dy >= dx)
	{
	// make sure we have corresponding points
	if ((lpP1_1->y < lpP2_1->y && lpP1_2->y > lpP2_2->y) ||
		(lpP1_1->y > lpP2_1->y && lpP1_2->y < lpP2_2->y))
		{
		temp = *lpP1_2;
		*lpP1_2 = *lpP2_2;
		*lpP2_2 = temp;
		}
	// find out which point is closest to the edge for transforming
	// because that will transform most of the image data
	dy1 = min(lpP1_2->y, FrameYSize(lpFrame2)-lpP1_2->y-1);
	dy2 = min(lpP2_2->y, FrameYSize(lpFrame2)-lpP2_2->y-1);
	if (dy2 < dy1 && !fNoSwap)
		{ // P1 is always the origin for transforms
		temp = *lpP1_1;
		*lpP1_1 = *lpP2_1;
		*lpP2_1 = temp;
		temp = *lpP1_2;
		*lpP1_2 = *lpP2_2;
		*lpP2_2 = temp;
		}
	}
else
	{
	// make sure we have corresponding points
	if ((lpP1_1->x < lpP2_1->x && lpP1_2->x > lpP2_2->x) ||
		(lpP1_1->x > lpP2_1->x && lpP1_2->x < lpP2_2->x))
		{
		temp = *lpP1_2;
		*lpP1_2 = *lpP2_2;
		*lpP2_2 = temp;
		}
	// find out which point is closest to the edge for transforming
	// because that will transform most of the image data
	dx1 = min(lpP1_2->x, FrameXSize(lpFrame2)-lpP1_2->x-1);
	dx2 = min(lpP2_2->x, FrameXSize(lpFrame2)-lpP2_2->x-1);
	if (dx2 < dx1 && !fNoSwap)
		{ // P1 is always the origin for transforms
		temp = *lpP1_1;
		*lpP1_1 = *lpP2_1;
		*lpP2_1 = temp;
		temp = *lpP1_2;
		*lpP1_2 = *lpP2_2;
		*lpP2_2 = temp;
		}
	}
return(TRUE);
}

/************************************************************************/
BOOL StitchAdjustPoints( LPFRAME lpFrame1, LPPOINT lpP1_1, LPPOINT lpP2_1,
                         LPFRAME lpFrame2, LPPOINT lpP1_2, LPPOINT lpP2_2,
                         int iEdge )
/************************************************************************/
{
if (StitchAdjustPoint(lpFrame1, lpP1_1, lpFrame2, lpP1_2, iEdge))
	return(StitchAdjustPoint(lpFrame1, lpP2_1, lpFrame2, lpP2_2, iEdge));
return(FALSE);
}

/************************************************************************/
BOOL StitchAdjustPoint( LPFRAME lpFrame1, LPPOINT lpP_1, LPFRAME lpFrame2,
                        LPPOINT lpP_2, int iEdge )
/************************************************************************/
{
RECT	rect1, rect2, rect3;
int	x, y, width1, width2, height1, height2, depth, dx, dy;
LPTR	lp1, lp2;
long	lSame, lMaxSame;
POINT	NewP2;

if (!StitchCreateRects(lpFrame1, &rect1, *lpP_1,
	lpFrame2, &rect2, *lpP_2, SAMPLE_OFFSET, iEdge))
	return(FALSE);
dx = lpP_1->x - rect1.left;
dy = lpP_1->y - rect1.top;
rect3 = rect2;
InflateRect(&rect3, ALIGN_OFFSET, ALIGN_OFFSET);
BoundRect(&rect3, 0, 0, FrameXSize(lpFrame2)-1, FrameYSize(lpFrame2)-1);

lp1 = LineBuffer[0];
lp2 = LineBuffer[1];
FindEdges( lpFrame1, &rect1, lp1 );
FindEdges( lpFrame2, &rect3, lp2 );

width1 = RectWidth(&rect1);
height1 = RectHeight(&rect1);
width2 = RectWidth(&rect3);
height2 = RectHeight(&rect3);
depth = FrameDepth(lpFrame2);
if (depth == 0) depth = 1;

StitchConvert(lp1, width1, height1, depth, THRESHOLD);
StitchConvert(lp2, width2, height2, depth, THRESHOLD);

lSame = StitchAmountSame(lp1, width1, lp1, width1, height1);
if (lSame <= 0 || lSame >= ((long)(width1-2)*(long)(height1-2)))
	{
//	Message(IDS_STITCHALIGN);
	return(FALSE);
	}
rect3.right -= (width1-1);
rect3.bottom -= (height1-1);
lp2 += ((rect2.top-rect3.top)*width2)+(rect2.left-rect3.left);
lMaxSame = StitchAmountSame(lp1, width1, lp2, width2, height1);
NewP2.x = rect2.left;
NewP2.y = rect2.top;
for (y = rect3.top; y <= rect3.bottom; ++y)
	{
	lp2 = LineBuffer[1] + ((y-rect3.top)*width2);
	for (x = rect3.left; x <= rect3.right; ++x, ++lp2)
		{
		lSame = StitchAmountSame(lp1, width1, lp2, width2, height1);
		if (lSame > lMaxSame)
			{
			lMaxSame = lSame;
			NewP2.x = x;
			NewP2.y = y;
			}
		}
	}

if (lMaxSame > 0)
	{
//	Print("before adjust - P1 = %d,%d P2 = %d,%d",
//		lpP_1->x, lpP_1->y, lpP_2->x, lpP_2->y);
	lpP_1->x = rect1.left + dx;
	lpP_1->y = rect1.top + dy;
	lpP_2->x = NewP2.x + dx;
	lpP_2->y = NewP2.y + dy;
//	Print("after adjust - P1 = %d,%d P2 = %d,%d",
//		lpP_1->x, lpP_1->y, lpP_2->x, lpP_2->y);
	}
return(TRUE);
}

/************************************************************************/
long StitchAmountSame( LPTR lp1Start, int width1, LPTR lp2Start, int width2,
                        int height )
/************************************************************************/
{
BYTE pix1, pix2;
int	iCount, width;
long lSame;
LPTR lp1, lp2;

width = min(width1, width2) - 2;
height -= 2;
lp1Start += (width1+1);
lp2Start += (width2+1);
lSame = 0;
while (--height >= 0)
	{
	lp1 = lp1Start;  	// don't use first pixel on a line
	lp2 = lp2Start;  	// ditto
	iCount = width;		// take off for last and first on a line
	while (--iCount >= 0)
		{
		pix1 = *lp1++;
		pix2 = *lp2++;
		if (pix1 && pix2)
			++lSame;
		else if (pix1 != pix2)
			--lSame;
		}
	lp1Start += width1;
	lp2Start += width2;
	}
return(lSame);
}

/************************************************************************/
void StitchConvert( LPTR lpIn, int width, int height, int depth,
                     int threshold )
/************************************************************************/
{
long pixel;
int iCount;
LPTR lpOut;

lpOut = lpIn;
iCount = width * height;

	switch( depth )
	{
		case 0:
		case 1:
			while (--iCount)
			{
				if (*lpIn++ > threshold)
					*lpOut++ = FALSE;
				else
					*lpOut++ = TRUE;
			}
		break;

		case 2:
			while (--iCount)
			{
				*((LPWORD)&pixel) = *((LPWORD)lpIn);

				if (TOLUM( MaxiR(pixel), MaxiG(pixel), MaxiB(pixel)) > threshold)
					*lpOut++ = FALSE;
				else
					*lpOut++ = TRUE;

				lpIn += 2;
			}
		break;

		case 3:
			while (--iCount)
			{
				if (TOLUM( lpIn[0], lpIn[1], lpIn[2]) > threshold)
					*lpOut++ = FALSE;
				else
					*lpOut++ = TRUE;

				lpIn += 3;
			}
		break;
	}
}

/************************************************************************/
int StitchGetLumDiff( LPFRAME lpFrame1, POINT p1_1, POINT p2_1,
			             LPFRAME lpFrame2, POINT p1_2, POINT p2_2 )
/************************************************************************/
{
int	i, min, max, times, dx1, dy1, dx2, dy2;
LFIXED x1rate, y1rate, x2rate, y2rate;
POINT p_1, p_2;
int	lums[LUM_STEPS];
long lTotal;

//dbg("p1_1 = %d %d p2_1 = %d %d", p1_1.x, p1_1.y, p2_1.x, p2_1.y);
//dbg("p1_2 = %d %d p2_2 = %d %d", p1_2.x, p1_2.y, p2_2.x, p2_2.y);
dx1 = p2_1.x - p1_1.x;
dy1 = p2_1.y - p1_1.y;
dx2 = p2_2.x - p1_2.x;
dy2 = p2_2.y - p1_2.y;

x1rate = FGET(dx1, LUM_STEPS-1);
y1rate = FGET(dy1, LUM_STEPS-1);
x2rate = FGET(dx2, LUM_STEPS-1);
y2rate = FGET(dy2, LUM_STEPS-1);

for (i = 0; i < LUM_STEPS; ++i)
	{
	p_1.x = p1_1.x + FMUL(i, x1rate);
	p_1.y = p1_1.y + FMUL(i, y1rate);
	p_2.x = p1_2.x + FMUL(i, x2rate);
	p_2.y = p1_2.y + FMUL(i, y2rate);
	lums[i] = StitchGetPointLumDiff(lpFrame1, p_1, lpFrame2, p_2);
//	dbg("p_1 = %d %d p_2 = %d %d lum = %d", p_1.x, p_1.y, p_2.x, p_2.y, lums[i]);
	}

// drop the min and maximum values found
min = 0;
max = 0;
for (i = 1; i < LUM_STEPS; ++i)
	{
	if (lums[i] < lums[min])
		min = i;
	if (lums[i] > lums[max])
		max = i;
	}

// average all but min and max values
times = 0;
lTotal = 0;
for (i = 0; i < LUM_STEPS; ++i)
	{
	if (lums[i] != lums[min] && lums[i] != lums[max])
		{
		lTotal += lums[i];
		++times;
		}
	}
if (!times)
	{
	for (i = 0; i < LUM_STEPS; ++i)
		{
		lTotal += lums[i];
		++times;
		}
	}
if (lTotal < 0)
	lTotal -= times/2;
else
	lTotal += times/2;
//dbg("average - times = %d lum = %d", times, (int)(lTotal/times));
return((int)(lTotal/times));
}

/************************************************************************/
void StitchCreateTransform( LPTFORM lpTForm, POINT p1_1, POINT p2_1,
                              POINT p1_2, POINT p2_2 )
/************************************************************************/
{
int	dx, dy, inc, count;
POINT	orig_p1_2, orig_p2_2;
LFIXED	angle, angle1, angle2;
LFIXED	fScaleY;

// initialize transform
TInit(lpTForm);

// Translate lpFrame2 onto lpFrame1 - p1_2 on top of p1_1
dx = p1_1.x - p1_2.x;
dy = p1_1.y - p1_2.y;
TMove(lpTForm, dx, dy);

// Transform points on lpFrame2 to lpFrame1
orig_p1_2 = p1_2;
orig_p2_2 = p2_2;
Transformer(lpTForm, &p1_2, (LPINT)&p1_2.x, (LPINT)&p1_2.y); // this line can be removed
Transformer(lpTForm, &p2_2, (LPINT)&p2_2.x, (LPINT)&p2_2.y);

// compute angle to rotate lpFrame2
angle1 = TrackGetAngle(p1_1, p2_1.x, p2_1.y);
angle2 = TrackGetAngle(p1_1, p2_2.x, p2_2.y);
angle = angle2 - angle1;
TMove(lpTForm, -p1_1.x, -p1_1.y);
TRotate(lpTForm, angle, angle);
TMove(lpTForm, p1_1.x, p1_1.y);

// find out where p2_2 now lands on new image
Transformer(lpTForm, &orig_p1_2, (LPINT)&p1_2.x, (LPINT)&p1_2.y);
Transformer(lpTForm, &orig_p2_2, (LPINT)&p2_2.x, (LPINT)&p2_2.y);

// transform y of p2_2 to land on top of y of p2_1
fScaleY = FGET(p2_1.y-p1_1.y, p2_2.y-p1_1.y);
if (fScaleY > 0L)
	{ // scale y of p2_2 to land on top of y of p2_1
	TMove(lpTForm, -p1_1.x, -p1_1.y);
	TScale(lpTForm, 0L, fScaleY);
	TMove(lpTForm, p1_1.x, p1_1.y);
	}

// find out where p2_2 now lands on new image
Transformer(lpTForm, &orig_p1_2, (LPINT)&p1_2.x, (LPINT)&p1_2.y);
Transformer(lpTForm, &orig_p2_2, (LPINT)&p2_2.x, (LPINT)&p2_2.y);

// shear x of p2_2 to land on top of x of p2_1
dx = p2_1.x - p2_2.x;
if (dx)
	{
	dy = p2_2.y - p1_2.y;
	if (dy < 0) // top side shear?
		{
		dy = -dy; // make to be opposite of bottom side shear
		dx = -dx;
		}
	inc = dy < 0 ? -1 : 1;
	count = dx/inc;
	while (--count >= 0)
		{
		TShearX(lpTForm, dy, inc);
		dx -= inc;
		}
	TShearX(lpTForm, dy, dx);

	Transformer(lpTForm, &orig_p1_2, (LPINT)&p1_2.x, (LPINT)&p1_2.y);
	dx = p1_1.x - p1_2.x;
	dy = p1_1.y - p1_2.y;
	TMove(lpTForm, dx, dy);
	}
// find out where p2_2 now lands on new image
Transformer(lpTForm, &orig_p1_2, (LPINT)&p1_2.x, (LPINT)&p1_2.y);
Transformer(lpTForm, &orig_p2_2, (LPINT)&p2_2.x, (LPINT)&p2_2.y);
dx = dy * dy;
dy = dx * dx;
dx += dy;
}

/************************************************************************/
int StitchGetPointLumDiff( LPFRAME lpFrame1, POINT p1, LPFRAME lpFrame2,
                           POINT p2 )
/************************************************************************/
{
RECT rect1, rect2;
int	y, iCount, depth;
LPTR lp;
long lRSum, lGSum, lBSum, lSum;
long lCount1, lRSum1, lGSum1, lBSum1;
long lCount2, lRSum2, lGSum2, lBSum2;
RGBS rgb;

if (!StitchCreateRects(lpFrame1, &rect1, p1, lpFrame2, &rect2, p2, LUM_OFFSET, UNKNOWN_EDGE))
	return(0);

lCount1 = 0;
lRSum1 = lGSum1 = lBSum1 = 0;
if ( !(depth = FrameDepth(lpFrame1)) )
	depth = 1;

for (y = rect1.top; y <= rect1.bottom; ++y)
	{
	if (!(lp = FramePointer(lpFrame1, rect1.left, y, YES )))
		continue;
	iCount = rect1.right - rect1.left + 1;
	lCount1 += iCount;
	while (--iCount >= 0)
		{
		frame_getRGB(lp, depth, &rgb);
		lRSum1 += rgb.red;
		lGSum1 += rgb.green;
		lBSum1 += rgb.blue;
		lp += depth;
		}
	}
lCount2 = 0;
lRSum2 = lGSum2 = lBSum2 = 0;
if ( !(depth = FrameDepth( lpFrame2 )) )
	depth = 1;
for (y = rect2.top; y <= rect2.bottom; ++y)
	{
	if (!(lp = FramePointer(lpFrame2, rect2.left, y, YES)))
		continue;
	iCount = rect2.right - rect2.left + 1;
	lCount2 += iCount;
	while (--iCount >= 0)
		{
		frame_getRGB(lp, depth, &rgb);
		lRSum2 += rgb.red;
		lGSum2 += rgb.green;
		lBSum2 += rgb.blue;
		lp += depth;
		}
	}
if (lCount1)
	{
	lRSum1 = (lRSum1 + (lCount1/2))/lCount1;
	lGSum1 = (lGSum1 + (lCount1/2))/lCount1;
	lBSum1 = (lBSum1 + (lCount1/2))/lCount1;
	}
if (lCount2)
	{
	lRSum2 = (lRSum2 + (lCount2/2))/lCount2;
	lGSum2 = (lGSum2 + (lCount2/2))/lCount2;
	lBSum2 = (lBSum2 + (lCount2/2))/lCount2;
	}
lRSum = lRSum1 - lRSum2;
lGSum = lGSum1 - lGSum2;
lBSum = lBSum1 - lBSum2;
lSum = lRSum + lGSum + lBSum;
if (lSum < 0)
	lSum = (lSum-1) / 3;
else
	lSum = (lSum+1) / 3;
return((int)lSum);
}

/************************************************************************/
LPFRAME StitchFrames( LPTFORM lpTForm, LPFRAME lpFrame1, LPFRAME lpFrame2,
                        int iAdjustLum, BOOL fSmartStitch )
/************************************************************************/
{
RECT	rect, rect1, rect2;
TFORM	inverted_tform;
int	dx, dy, nx, ny, right, bottom, depth;
LFIXED fnx, fny, fnegone, fright, fbottom;
LPFRAME	lpNewFrame;
WORD	wStart1, wStart2, Xsize, Ysize;
RGBS	whiteRGB, rgb;
long	white, pixel;
POINT	p;
LPTR	lp, lp2;
int lum;

lum = iAdjustLum;
//dbg("lum = %d", lum);
// get rectangle lpFrame1 occupies
rect1.left   = 0;
rect1.top	 = 0;
rect1.bottom = FrameYSize(lpFrame1) - 1;
rect1.right  = FrameXSize(lpFrame1) - 1;

// get rectangle lpFrame2 transformed occupies
rect2.left   = 0;
rect2.top	 = 0;
rect2.bottom = FrameYSize(lpFrame2) - 1;
rect2.right  = FrameXSize(lpFrame2) - 1;
TransformRect(lpTForm, &rect2, &rect2);

// determine new image rectangle
if (rect2.left < rect1.left) // does transformed rect go off of
	{			 // left side of lpFrame1
	dx = rect1.left - rect2.left;
	OffsetRect(&rect1, dx, 0);
	OffsetRect(&rect2, dx, 0);
	TMove(lpTForm, dx, 0);
	}
if (rect2.top < rect1.top) // does transformed rect go off of
	{ // top side of lpFrame1
	dy = rect1.top - rect2.top;
	OffsetRect(&rect1, 0, dy);
	OffsetRect(&rect2, 0, dy);
	TMove(lpTForm, 0, dy);
	}
AstralUnionRect(&rect, &rect1, &rect2);

// get size of new image
Xsize = RectWidth(&rect);
Ysize = RectHeight(&rect);
depth = lpFrame1->Depth;

// create new image frame
ProgressBegin(1,0);
lpNewFrame = FrameOpen( FrameTypeInfo(lpFrame1), Xsize, Ysize, FrameResolution(lpFrame1));
if (!lpNewFrame)
{
	ProgressEnd();
	return(NULL);
}

// invert tform to transform from new frame back to lpFrame2
inverted_tform = *lpTForm;
TInvert(&inverted_tform);

// get boundary extents for lpFrame2
right  = FrameXSize(lpFrame2)-1;
bottom = FrameYSize(lpFrame2)-1;
if (fSmartStitch)
	{
	fright = FGET(right + 1, 1);
	fbottom = FGET(bottom + 1, 1);
	fnegone = FGET(-1, 1);
	}

// get number of bytes to advance in till lpFrame1 and lpFrame2 starts
wStart1 = (WORD)rect1.left*depth;
wStart2 = (WORD)rect2.left*depth;
whiteRGB.red = 255; whiteRGB.green = 255; whiteRGB.blue = 255;
FrameRGB2Pixel(lpFrame1, &whiteRGB, &white);

// copy frame 1 into new frame
for (p.y = 0; p.y < Ysize; ++p.y)
	{
	if (AstralClockCursor(p.y, Ysize, YES))
		{
		FrameClose(lpNewFrame);
		ProgressEnd();
		return(NULL);
		}
	lp = FramePointer( lpNewFrame, 0, p.y, YES);
	if (!lp)
		continue;
	FrameSetPixel( lpNewFrame, white, lp, Xsize);
	if (p.y >= rect1.top && p.y <= rect1.bottom)
		{
		FrameRead( lpFrame1, 0, p.y-rect1.top, FrameXSize(lpFrame1), 
			lp+wStart1, FrameXSize(lpFrame1));
		}
	if (p.y >= rect2.top && p.y <= rect2.bottom)
		{
		lp += wStart2;
		for (p.x = rect2.left; p.x <= rect2.right; ++p.x, lp += depth)
			{
			if (fSmartStitch)
				{
				FTransformer(&inverted_tform, &p, &fnx, &fny);
				if (fnx <= fnegone || fny <= fnegone ||
				fnx >= fright || fny >= fbottom)
					continue;
				pixel = average_pixel(lpFrame2, fnx, fny, depth,
							0, 0, bottom, right, FALSE, white);
				if (lum)
					{
					FramePixel2RGB( lpFrame2, &pixel, &rgb);
					rgb.red = bound((int)rgb.red+lum,
							0, 255);
					rgb.green = bound((int)rgb.green+lum,
							0, 255);
					rgb.blue = bound((int)rgb.blue+lum,
							0, 255);
					frame_putRGB(lp, depth, &rgb);
					}
				else	frame_putpixel(lp, depth, pixel);
				}
			else
				{
				Transformer(&inverted_tform, &p, &nx, &ny);
				if (nx < 0 || nx > right ||
					ny < 0 || ny > bottom)
					continue;
				lp2 = FramePointer(lpFrame2, nx, ny, YES);
				if (!lp2)
					continue;
				if (depth == 1)
					{
					*lp = bound((int)*lp2+lum, 0, 255);
					}
				else
					{
					frame_getRGB(lp2, depth, &rgb);
					rgb.red = bound((int)rgb.red+lum,
							0, 255);
					rgb.green = bound((int)rgb.green+lum,
							0, 255);
					rgb.blue = bound((int)rgb.blue+lum,
							0, 255);
					frame_putRGB(lp, depth, &rgb);
					}
				}
			}
		}
	}
ProgressEnd();
return(lpNewFrame);
}

/************************************************************************/
BOOL StitchBlend( LPTFORM lpTForm, LPFRAME lpNewFrame, LPFRAME lpFrame,
                  int iWhichEdge, int BlendPressure )
/************************************************************************/
{
	RECT rArea;
	CPoly Poly;
	LPMASK lpMask;
   FRMTYPEINFO TypeInfo;
   FRMDATATYPE FrameType = FDT_GRAYSCALE;

   FrameGetTypeInfo(lpNewFrame, &TypeInfo);
   if (TypeInfo.DataType == FDT_LINEART && Mask.OneBitMask)
      FrameType = FDT_LINEART;
	
	rArea.left = 0;
	rArea.top = 0;
	rArea.right = FrameXSize(lpFrame)-1;
	rArea.bottom = FrameYSize(lpFrame)-1;
	
	// Do the blend
	// This should link to lpNewFrame and not the current image
	if (!(lpMask = MaskCreate(NULL, FrameXSize(lpNewFrame),
									FrameYSize(lpNewFrame),
									OFF, YES, FrameType)))
	{
		Message(IDS_EMEMALLOC);
		return(FALSE);
	}
	MakeRectPoly(&Poly, &rArea, YES);
	PolyTForm(&Poly, lpTForm);
	if (!MaskAddPoly(lpMask, &Poly, SHAPE_ADD,NO /*bAntialias*/))
	{
		MaskClose(lpMask);
		Message(IDS_EMEMALLOC);
		return(FALSE);
	}
	DoBlend(lpNewFrame, lpMask, BlendPressure);
	MaskClose(lpMask);
	return(TRUE);
}

/************************************************************************/
BOOL StitchCreateRects( LPFRAME lpFrame1, LPRECT lpRect1, POINT p1,
                        LPFRAME lpFrame2, LPRECT lpRect2, POINT p2,
                        int offset, int iEdge )
/************************************************************************/
{
int	small_offset, big_offset, half_offset;

small_offset = offset/4;
big_offset = offset - small_offset;
half_offset = offset/2;

// if iEdge is non-zero we create rectangle so it will go
// more towards the overlap of the 2 frames
// iEdge is which edge of lpFrame2 is getting stitched
if (iEdge == LEFT_EDGE || iEdge == RIGHT_EDGE)
	{
	if (iEdge == LEFT_EDGE)
		{
		lpRect1->left = p1.x - small_offset;
		lpRect1->right = p1.x + big_offset;
		lpRect2->left = p2.x - small_offset;
		lpRect2->right = p2.x + big_offset;
		}
	else
		{
		lpRect1->left = p1.x - big_offset;
		lpRect1->right = p1.x + small_offset;
		lpRect2->left = p2.x - big_offset;
		lpRect2->right = p2.x + small_offset;
		}
	lpRect1->top = p1.y - half_offset;
	lpRect1->bottom = p1.y + half_offset;
	lpRect2->top = p2.y - half_offset;
	lpRect2->bottom = p2.y + half_offset;
	}
else if (iEdge == TOP_EDGE || iEdge == BOTTOM_EDGE)
	{
	if (iEdge == TOP_EDGE)
		{
		lpRect1->top = p1.y - small_offset;
		lpRect1->bottom = p1.y + big_offset;
		lpRect2->top = p2.y - small_offset;
		lpRect2->bottom = p2.y + big_offset;
		}
	else
		{
		lpRect1->top = p1.y - big_offset;
		lpRect1->bottom = p1.y + small_offset;
		lpRect2->top = p2.y - big_offset;
		lpRect2->bottom = p2.y + small_offset;
		}
	lpRect1->left = p1.x - half_offset;
	lpRect1->right = p1.x + half_offset;
	lpRect2->left = p2.x - half_offset;
	lpRect2->right = p2.x + half_offset;
	}
else
	{
	lpRect1->left = p1.x - half_offset;
	lpRect1->right = p1.x + half_offset;
	lpRect1->top = p1.y - half_offset;
	lpRect1->bottom = p1.y + half_offset;
	lpRect2->left = p2.x - half_offset;
	lpRect2->right = p2.x + half_offset;
	lpRect2->top = p2.y - half_offset;
	lpRect2->bottom = p2.y + half_offset;
	}

if (lpRect1->left < 0)
	{
	lpRect2->left -= lpRect1->left;
	lpRect1->left -= lpRect1->left;
	}
if (lpRect1->right >= FrameXSize(lpFrame1))
	{
	lpRect2->right -= (lpRect1->right-FrameXSize(lpFrame1)+1);
	lpRect1->right -= (lpRect1->right-FrameXSize(lpFrame1)+1);
	}
if (lpRect1->top < 0)
	{
	lpRect2->top -= lpRect1->top;
	lpRect1->top -= lpRect1->top;
	}
if (lpRect1->bottom >= FrameYSize(lpFrame1))
	{
	lpRect2->bottom -= (lpRect1->bottom-FrameYSize(lpFrame1)+1);
	lpRect1->bottom -= (lpRect1->bottom-FrameYSize(lpFrame1)+1);
	}
if (lpRect2->left < 0)
	{
	lpRect1->left -= lpRect2->left;
	lpRect2->left -= lpRect2->left;
	}
if (lpRect2->right >= lpFrame2->Xsize)
	{
	lpRect1->right -= (lpRect2->right-FrameXSize(lpFrame2)+1);
	lpRect2->right -= (lpRect2->right-FrameXSize(lpFrame2)+1);
	}
if (lpRect2->top < 0)
	{
	lpRect1->top -= lpRect2->top;
	lpRect2->top -= lpRect2->top;
	}
if (lpRect2->bottom >= FrameYSize(lpFrame2))
	{
	lpRect1->bottom -= (lpRect2->bottom-FrameYSize(lpFrame2)+1);
	lpRect2->bottom -= (lpRect2->bottom-FrameYSize(lpFrame2)+1);
	}
return(RectWidth(lpRect1) > 0 && RectHeight(lpRect1) > 0 &&
	   RectWidth(lpRect2) > 0 && RectHeight(lpRect2) > 0);
}


