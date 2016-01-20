#include "ctlutil.h"
#include "gradctl.h"
#include "gradctl.rh"
#include "framelib.h"
#include "macros.h"


// one long for structure pointer
#define GWL_GRADSTRUCT	(GWW_START+0)			

// Number of extra memory for window words
#define GRADIENT_EXTRA	(GWW_START+4)

// struct allocated for every map control
typedef struct _mapctl
{	
	RGBS rgb1;
	RGBS rgb2;
	HSLS hsl1;
	HSLS hsl2;
	int type;			// GT_HSL, GT_RGB
	BOOL bDirection;	// direction
}GRADCTL, FAR * LPGRADCTL;


// helper macro
#define GET_LPGRADCTL(hGrad) \
	(hGrad ? (LPGRADCTL)GetWindowLong(hGrad,GWL_GRADSTRUCT) : NULL)


//************************************************************************
// 	LOCALS 
//************************************************************************
LOCAL void gradient_draw(LPGRADCTL lpgrad, HDC hDC, LPRECT lpRect, DWORD dwStyles);


//************************************************************************
//			Gradient_SetRGB
//	PARAMETERS:
//				
//					HWND hGrad- the window of control to set.
//					LPRGB lpRGBFrom- The 1st color. (NULL for no change)
//					LPRGB lpRGBTo- The 2nd color. (NULL for no change)
//	DESCRIPTION: 
//			Sets 1st & 2nd color.
//			Redraws control.
//************************************************************************
void Gradient_SetRGB(HWND hGrad, LPRGB lpRGBFrom, LPRGB lpRGBTo)
//************************************************************************
{
	LPGRADCTL lpGrad;
	
	if (!(lpGrad = GET_LPGRADCTL(hGrad)))
		return;
	

	if (lpRGBFrom)
	{
		lpGrad->rgb1 = *lpRGBFrom;
		RGBtoHSL(lpGrad->rgb1.red, lpGrad->rgb1.green, lpGrad->rgb1.blue, &lpGrad->hsl1);
	}
	if (lpRGBTo)
	{
		lpGrad->rgb2 = *lpRGBTo;
		RGBtoHSL(lpGrad->rgb2.red, lpGrad->rgb2.green, lpGrad->rgb2.blue, &lpGrad->hsl2);
	}
    
    InvalidateRect (hGrad,NULL,FALSE);
    UpdateWindow(hGrad); 
}

//************************************************************************
//			Gradient_SetHSL
//	PARAMETERS:
//				
//					HWND hGrad- the window of control to set.
//					LPHSL lpHSLFrom- The 1st color. (NULL for no change)
//					LPHSL lpHSLTo- The 2nd color. (NULL for no change)
//	DESCRIPTION: 
//			Sets 1st & 2nd color.
//			Redraws control.
//************************************************************************
void Gradient_SetHSL(HWND hGrad, LPHSL lpHSLFrom, LPHSL lpHSLTo)
//************************************************************************
{
	LPGRADCTL lpGrad;
	
	if (!(lpGrad = GET_LPGRADCTL(hGrad)))
		return;
													   
	if (lpHSLFrom)
	{
		lpGrad->hsl1 = *lpHSLFrom;
		HSLtoRGB( lpHSLFrom->hue, lpHSLFrom->sat, lpHSLFrom->lum, &lpGrad->rgb1 );
	}
	if (lpHSLTo)
	{
		lpGrad->hsl2 = *lpHSLTo;
		HSLtoRGB( lpHSLTo->hue, lpHSLTo->sat, lpHSLTo->lum, &lpGrad->rgb2 );
	}
	
    InvalidateRect (hGrad,NULL,FALSE);
    UpdateWindow(hGrad); 
}

//************************************************************************
//		Gradient_SetType
//	PARAMETERS:
//					HWND hGrad- the window of control to set.
//					int type- GT_HSL or GT_RGB.
//	 				BOOL bDirection- Sets direction (ie l2r vs r2l)
//	DESCRIPTION: 
//			Sets type and direction.
//			Set bDirection = FASLE for l2r or t2b.
//			Redraws control.
//************************************************************************
void Gradient_SetType(HWND hGrad, int type, BOOL bDirection)
{
	LPGRADCTL lpGrad;
	
	if (!(lpGrad = GET_LPGRADCTL(hGrad)))
		return;
		
	lpGrad->type = type;
	lpGrad->bDirection = bDirection;
		
    InvalidateRect (hGrad,NULL,FALSE);
    UpdateWindow(hGrad); 
}																	  


//************************************************************************
BOOL Gradient_RegisterClass (HINSTANCE hInstance, UINT ClassScope)
//************************************************************************
{
	WNDCLASS WndClass;

	WndClass.hIcon		= NULL;
	WndClass.lpszMenuName	= NULL;
	WndClass.lpszClassName	= (LPTSTR)_T("gradient");
	WndClass.hCursor	= NULL;
	WndClass.hbrBackground	= NULL;
	WndClass.hInstance	= hInstance;
	WndClass.style		= CS_DBLCLKS | ClassScope;
	WndClass.lpfnWndProc	= Gradient_Control;
	WndClass.cbClsExtra	= 0;
	WndClass.cbWndExtra	= GRADIENT_EXTRA;

	return(RegisterClass( &WndClass ));
}

BOOL Gradient_UnRegisterClass (HINSTANCE hInstance)
{
    return (UnregisterClass ((LPTSTR)_T("gradient"), hInstance));
}


/***********************************************************************/
/***********************************************************************/
// GRADIENT CONTROL
/***********************************************************************/
/***********************************************************************/

LOCAL void Gradient_OnPaint(HWND hWindow);
LOCAL BOOL Grad_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct);
LOCAL void Grad_OnDestroy(HWND hWindow);
LOCAL void Grad_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);

/***********************************************************************/
LONG WINPROC EXPORT Gradient_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch ( message )
	{
	HANDLE_MSG(hWindow, WM_GETDLGCODE, Control_OnGetDlgCode);
	HANDLE_MSG(hWindow, WM_PALETTECHANGED, Control_OnPaletteChanged);
	HANDLE_MSG(hWindow, WM_ERASEBKGND, Control_OnEraseBkgnd);

	HANDLE_MSG(hWindow, WM_PAINT, Gradient_OnPaint);
	HANDLE_MSG(hWindow, WM_CREATE, Grad_OnCreate);
	HANDLE_MSG(hWindow, WM_DESTROY, Grad_OnDestroy);
	HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Grad_OnLButtonDown);

	default:
	return Control_DefProc( hWindow, message, wParam, lParam );
	}
}


/***********************************************************************/
LOCAL BOOL Grad_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{
	LPGRADCTL lpgrad;

	lpgrad = (LPGRADCTL)Alloc(sizeof(GRADCTL));
	if (lpgrad)
	{
		if (GetWindowLong(hWindow, GWL_STYLE) & GS_VERT)
		{
			lpgrad->rgb1.red = lpgrad->rgb1.green = lpgrad->rgb1.blue = 0;
			lpgrad->rgb2.red = lpgrad->rgb2.green = lpgrad->rgb2.blue = 255;
		}
		else
		{
			lpgrad->rgb1.red = lpgrad->rgb1.green = lpgrad->rgb1.blue = 255;
			lpgrad->rgb2.red = lpgrad->rgb2.green = lpgrad->rgb2.blue = 0;
		}
		lpgrad->type = GT_RGB;			// GT_HSL, GT_RGB
		lpgrad->bDirection = FALSE;		// direction
	}
	SetWindowLong( hWindow, GWL_GRADSTRUCT, (long)lpgrad);
	return(TRUE);
}

/***********************************************************************/
LOCAL void Grad_OnDestroy(HWND hWindow)
/***********************************************************************/
{
	LPGRADCTL lpgrad;

	if (!(lpgrad = GET_LPGRADCTL(hWindow)))
		return;
	FreeUp((LPTR)lpgrad);
		SetWindowLong( hWindow, GWL_GRADSTRUCT, (long)NULL);
	Control_OnDestroy(hWindow);
}


/***********************************************************************/
LOCAL void Grad_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	// just notify parent
	FORWARD_WM_COMMAND( GetParent(hWindow), 
				GetDlgCtrlID(hWindow), 
				hWindow,
				GN_LBUTTONDOWN, 
				SendMessage );
	
}


/***********************************************************************/
LOCAL void Gradient_OnPaint(HWND hWindow)
/***********************************************************************/
{
	RECT ClientRect;
	PAINTSTRUCT ps;
	HDC hDC;
	DWORD dwStyles;
	LPGRADCTL lpgrad;
	
	if (!(lpgrad = GET_LPGRADCTL(hWindow)))
		return;

	hDC = BeginPaint( hWindow, &ps );
	GetClientRect( hWindow, &ClientRect );
	dwStyles = GetWindowLong(hWindow, GWL_STYLE);

	// Draw the box frame
	FrameRect( hDC, &ClientRect, (HBRUSH)GetStockObject(BLACK_BRUSH) );
	InflateRect( &ClientRect, -1, -1 );

	// Draw the gradient
	gradient_draw(lpgrad, hDC, &ClientRect, dwStyles);
	EndPaint( hWindow, &ps );
}

/***********************************************************************/
LOCAL void gradient_draw(LPGRADCTL lpgrad, HDC hDC, LPRECT lpRect, DWORD dwStyles)
/***********************************************************************/
{
	int x, y, delta, lines;
	long color;
	LFIXED fScale, fOffset;
	LPRGB lpRGBline, lpRGBWork;
	RECT rRepair;
	BLTSESSION BltSession;
	FRMTYPEINFO TypeInfo;
	RGBS rgb1, rgb2;
	HSLS hsl1, hsl2, hslWork;
	BOOL bVert;
	
	bVert = ((dwStyles & GS_VERT) != 0);
	// setup direction
	if (lpgrad->bDirection)
	{
		rgb1 = lpgrad->rgb2;
		hsl1 = lpgrad->hsl2;
		rgb2 = lpgrad->rgb1;
		hsl2 = lpgrad->hsl1;
	}
	else
	{
		rgb2 = lpgrad->rgb2;
		hsl2 = lpgrad->hsl2;
		rgb1 = lpgrad->rgb1;
		hsl1 = lpgrad->hsl1;
	}

	// get rect
	rRepair = *lpRect;
	rRepair.right--;
	rRepair.bottom--;

	if ( !(lpRGBline = (LPRGB)Alloc((long)RectWidth(&rRepair)*3L)) )
		return;

	// setup superblit session
	FrameSetTypeInfo(&TypeInfo, FDT_RGBCOLOR, NULL);
	StartSuperBlt( 
		&BltSession,
		hDC,
		NULL,
		Ctl_GetBltScreen(),
		&rRepair,
		TypeInfo,
		10,
		0,
		0,
		YES,
		NULL,
		NULL);


	// prepare to loop
	lpRGBWork = lpRGBline;

	if (bVert)
	{
		delta = RectHeight( lpRect ) - 1;
		fScale  = FGET( 255, delta);
		lines = RectHeight( &rRepair );
		fOffset = (fScale * rRepair.top);
	}
	else
	{
		delta = RectWidth( lpRect ) - 1;
		fScale  = FGET( 255, delta);
		lines = RectWidth( &rRepair );
		fOffset = (fScale * rRepair.left);
	}

	for ( x=0; x<lines; x++)
	{
		delta = WHOLE(fOffset);
		fOffset += fScale;

		// get color
		switch ( lpgrad->type )
		{
			case GT_HSL:
				hslWork.hue = ((long)hsl1.hue*(255-delta)+(long)hsl2.hue*delta)/255;
				hslWork.sat = ((long)hsl1.sat*(255-delta)+(long)hsl2.sat*delta)/255;
				hslWork.lum = ((long)hsl1.lum*(255-delta)+(long)hsl2.lum*delta)/255;
				HSLtoRGB( hslWork.hue, hslWork.sat, hslWork.lum, lpRGBWork );
			break;

			case GT_RGB :
			default:
				lpRGBWork->red = ((long)rgb1.red*(255-delta)+(long)rgb2.red*delta)/255;
				lpRGBWork->green = ((long)rgb1.green*(255-delta)+(long)rgb2.green*delta)/255;
				lpRGBWork->blue = ((long)rgb1.blue*(255-delta)+(long)rgb2.blue*delta)/255;
			break;
		}
		
		if (bVert)
		{
			// draw line of single color now
			CopyRGB(lpRGBline, &color);
			set24(lpRGBline, RectWidth(&rRepair), color);
			SuperBlt( &BltSession, (LPTR)(lpRGBline) );
			lpRGBWork = lpRGBline;
		}
		else
		 	lpRGBWork++;
	
	}

	if (!bVert)
	{
		// draw lines of the gradient
		for ( y=0; y<=(rRepair.bottom-rRepair.top); y++ )
			SuperBlt( &BltSession, (LPTR)(lpRGBline) );
	}

	SuperBlt( &BltSession, NULL );
	FreeUp( (LPTR)lpRGBline );
}
