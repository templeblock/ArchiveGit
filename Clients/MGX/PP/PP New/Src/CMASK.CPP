//®PL1¯®FD1¯®BT0¯®TP0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"

// Static prototypes
static void SetupColorMask( BYTE Mask, LPCOLORINFO lpColor1, LPCOLORINFO lpColor2, BOOL On );
static void ShowShieldPreview( void );
static void ShowShieldReset( void );

static RECT rPreview;
static int idLastTool;

class CShieldFloatHook : public CHookHandler
{
public:
    CShieldFloatHook() : CHookHandler() {};
    virtual void ImageHookProc(int x, int y, int dx, FRMTYPEINFO TypeInfo, LPTR lpSrc,
            LPTR lpDst, LPTR lpMsk, LPMASK lpMask);
    virtual void DisplayHookProc(HWND hWnd, HDC hDC, LPRECT lpRect) { /*no implementation*/ };
protected:
private:
};
CShieldFloatHook ShieldFloatHook;


/***********************************************************************/
void SetupShield()
/***********************************************************************/
{
	int i, range;
	BOOL on;
	COLORINFO Color, Color1, Color2;
	HWND hWnd;

	for (i = 0; i < 8; ++i)
	{
		Color = ColorMask.MaskColor[i];
		Color1 = Color2 = Color;

		range = ColorMask.MaskRange[i];
		range = TOGRAY(range);

		Color1.rgb.red   = bound((int)Color.rgb.red   - range, 0, 255);
		Color1.rgb.green = bound((int)Color.rgb.green - range, 0, 255);
		Color1.rgb.blue  = bound((int)Color.rgb.blue  - range, 0, 255);

		Color2.rgb.red   = bound((int)Color.rgb.red   + range, 0, 255);
		Color2.rgb.green = bound((int)Color.rgb.green + range, 0, 255);
		Color2.rgb.blue  = bound((int)Color.rgb.blue  + range, 0, 255);

		Color1.cmyk.c = bound((int)Color.cmyk.c - range, 0, 255);
		Color1.cmyk.m = bound((int)Color.cmyk.m - range, 0, 255);
		Color1.cmyk.y = bound((int)Color.cmyk.y - range, 0, 255);
		Color1.cmyk.k = bound((int)Color.cmyk.k - range, 0, 255);

		Color2.cmyk.c = bound((int)Color.cmyk.c + range, 0, 255);
		Color2.cmyk.m = bound((int)Color.cmyk.m + range, 0, 255);
		Color2.cmyk.y = bound((int)Color.cmyk.y + range, 0, 255);
		Color2.cmyk.k = bound((int)Color.cmyk.k + range, 0, 255);

		on = ColorMask.Mask & (1<<i);

		SetupColorMask((BYTE)(1<<i), &Color1, &Color2, on); 
	}

	if ( hWnd = AstralDlgGet(IDD_SHIELDFLOAT) )
		ControlEnable( hWnd, IDC_PREVIEW, ColorMask.Mask );
	if ( Tool.hRibbon )
		SendMessage( Tool.hRibbon, WM_SHIELDCHANGE, 0, 0L );
}

/************************************************************************/
static void SetupColorMask( BYTE Mask, LPCOLORINFO lpColor1, LPCOLORINFO lpColor2, BOOL On )
/***********************************************************************/
{
	int min, max, i;

	min = min(lpColor1->rgb.red, lpColor2->rgb.red);
	max = max(lpColor1->rgb.red, lpColor2->rgb.red);
	for (i = 0; i < 256; ++i)
	{
		if (i >= min && i <= max && On)
			ColorMask.RMask[i] |= Mask;
		else
			ColorMask.RMask[i] &= ~Mask;
	}

	min = min(lpColor1->rgb.green, lpColor2->rgb.green);
	max = max(lpColor1->rgb.green, lpColor2->rgb.green);
	for (i = 0; i < 256; ++i)
	{
		if (i >= min && i <= max && On)
			ColorMask.GMask[i] |= Mask;
		else
			ColorMask.GMask[i] &= ~Mask;
	}

	min = min(lpColor1->rgb.blue, lpColor2->rgb.blue);
	max = max(lpColor1->rgb.blue, lpColor2->rgb.blue);
	for (i = 0; i < 256; ++i)
	{
		if (i >= min && i <= max && On)
			ColorMask.BMask[i] |= Mask;
		else
			ColorMask.BMask[i] &= ~Mask;
	}

	min = min(lpColor1->cmyk.c, lpColor2->cmyk.c);
	max = max(lpColor1->cmyk.c, lpColor2->cmyk.c);
	for (i = 0; i < 256; ++i)
	{
		if (i >= min && i <= max && On)
			ColorMask.CMask[i] |= Mask;
		else
			ColorMask.CMask[i] &= ~Mask;
	}

	min = min(lpColor1->cmyk.m, lpColor2->cmyk.m);
	max = max(lpColor1->cmyk.m, lpColor2->cmyk.m);
	for (i = 0; i < 256; ++i)
	{
		if (i >= min && i <= max && On)
			ColorMask.MMask[i] |= Mask;
		else
			ColorMask.MMask[i] &= ~Mask;
	}

	min = min(lpColor1->cmyk.y, lpColor2->cmyk.y);
	max = max(lpColor1->cmyk.y, lpColor2->cmyk.y);
	for (i = 0; i < 256; ++i)
	{
		if (i >= min && i <= max && On)
			ColorMask.YMask[i] |= Mask;
		else
			ColorMask.YMask[i] &= ~Mask;
	}

	min = min(lpColor1->cmyk.k, lpColor2->cmyk.k);
	max = max(lpColor1->cmyk.k, lpColor2->cmyk.k);
	for (i = 0; i < 256; ++i)
	{
		if (i >= min && i <= max && On)
			ColorMask.KMask[i] |= Mask;
		else
			ColorMask.KMask[i] &= ~Mask;
	}
}

/************************************************************************/
void UpdateColorMask( LPCOLORINFO lpColor )
/***********************************************************************/
{
	COLORINFO Color, Color1, Color2;
	int  i, range;
	HWND hWnd;
	long lColor;
	BOOL on;

	i = ColorMask.MaskShield;
	ColorMask.MaskColor[i] = *lpColor;
	Color = Color1 = Color2 = ColorMask.MaskColor[i];
	range = TOGRAY(ColorMask.MaskRange[i]);

	Color1.rgb.red   = bound((int)Color.rgb.red   - range, 0, 255);
	Color1.rgb.green = bound((int)Color.rgb.green - range, 0, 255);
	Color1.rgb.blue  = bound((int)Color.rgb.blue  - range, 0, 255);

	Color2.rgb.red   = bound((int)Color.rgb.red   + range, 0, 255);
	Color2.rgb.green = bound((int)Color.rgb.green + range, 0, 255);
	Color2.rgb.blue  = bound((int)Color.rgb.blue  + range, 0, 255);

	Color1.cmyk.c = bound((int)Color.cmyk.c - range, 0, 255);
	Color1.cmyk.m = bound((int)Color.cmyk.m - range, 0, 255);
	Color1.cmyk.y = bound((int)Color.cmyk.y - range, 0, 255);
	Color1.cmyk.k = bound((int)Color.cmyk.k - range, 0, 255);

	Color2.cmyk.c = bound((int)Color.cmyk.c + range, 0, 255);
	Color2.cmyk.m = bound((int)Color.cmyk.m + range, 0, 255);
	Color2.cmyk.y = bound((int)Color.cmyk.y + range, 0, 255);
	Color2.cmyk.k = bound((int)Color.cmyk.k + range, 0, 255);

	on = ColorMask.Mask & (1<<i);

	SetupColorMask((BYTE)(1<<i), &Color1, &Color2, on);

	if ( hWnd = AstralDlgGet(IDD_SHIELDFLOAT) )
	{
		CopyRGB( &ColorMask.MaskColor[i].rgb, &lColor );
		SetWindowLong( GetDlgItem( hWnd, IDC_MASKCOLOR1+i ), GWL_ACTIVE, lColor );
		InvalidateRect( GetDlgItem( hWnd, IDC_MASKCOLOR1+i ), NULL, FALSE );
	}
}

/***********************************************************************/
void ShowShieldProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/***********************************************************************/
{
	static WORD idLastTool;

	switch (msg)
	{
		case WM_CREATE:
			idLastTool = Option;
			ShowShieldPreview();
			Tool.bActive = YES;
		break;

		case WM_LBUTTONDBLCLK:
		case WM_DESTROY:	// The cancel operation message
			ShowShieldReset();
			Tool.bActive = NO;
			ActivateTool(idLastTool);
		break;

		default:
		break;
	}
}

/************************************************************************/
static void ShowShieldPreview()
/************************************************************************/
{
	RECT rMask;
	HWND hWnd;
	HWND hActiveWnd;
	LPIMAGE lpImage;


	hActiveWnd = GetActiveDoc();
	if (!hActiveWnd)
		return;
	lpImage = GetImagePtr(hActiveWnd);

	if (ImgGetMask(lpImage))
		ImgGetMaskRect(lpImage, &rMask);
	else
		ImgGetSelObjectRect(lpImage, &rMask, NO);

	File2DispRect(GetActiveDoc(), &rMask, &rMask);

	if (Tool.id == IDC_PREVIEW)
		AstralUnionRect(&rMask, &rMask, &rPreview);

	ImgGetMaskRect(lpImage, &rPreview);
	File2DispRect(hActiveWnd, &rPreview, &rPreview);
	SetImageHook(hActiveWnd, &ShieldFloatHook);
	AstralCursor( IDC_WAIT );
	InvalidateRect(hActiveWnd, &rMask, FALSE);
	AstralUpdateWindow(hActiveWnd);

	if ( hWnd = AstralDlgGet(IDD_SHIELDFLOAT) )
		ControlEnable( hWnd, IDC_RESET, YES );

	AstralCursor( NULL );
}

/************************************************************************/
static void ShowShieldReset()
/************************************************************************/
{
	HWND hWnd;
	HWND hActiveWnd;


	hActiveWnd = GetActiveDoc();
	if (!hActiveWnd)
		return;

	SetImageHook(hActiveWnd, NULL);
	AstralCursor( IDC_WAIT);
	InvalidateRect(hActiveWnd, &rPreview, FALSE);
	AstralUpdateWindow(hActiveWnd);
	AstralCursor( NULL );

	if ( hWnd = AstralDlgGet(IDD_SHIELDFLOAT) )
		ControlEnable( hWnd, IDC_RESET, NO );
}

/************************************************************************/
void CShieldFloatHook::ImageHookProc(
	int    x,
	int    y,
	int    dx,
	FRMTYPEINFO TypeInfo,
	LPTR   lpSrc,
	LPTR   lpDst,
	LPTR   lpMsk,
	LPMASK lpMask)
/************************************************************************/
{
	long color;
	int depth;

	if (!ColorMask.Mask || !ColorMask.On)
		return;

	if (!lpMsk)
		return;

	mload(x, y, dx, 1, lpMsk, lpDst, TypeInfo, lpMask);
	setifset(lpMsk, dx, 255);

	switch (TypeInfo.DataType)
	{
		case FDT_RGBCOLOR:
			depth = 3;
		break;
	
		case FDT_CMYKCOLOR:
			depth = 4;
		break;

		default:
		case FDT_LINEART:
		case FDT_GRAYSCALE:
		case FDT_PALETTECOLOR:
			depth = 1;
		break;

	}

	color = GetFrameColor( GetActiveColorInfo(), TypeInfo );

	while (dx-- > 0)
	{
		if (*lpMsk++)
			FrameSetPixelEx( TypeInfo.DataType, color, lpDst, 1 );
		lpDst += depth;
	}
}


