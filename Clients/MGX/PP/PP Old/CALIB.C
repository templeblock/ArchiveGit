// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
/*®FD1¯®PL1¯®TP0¯®BT0¯*/
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"

extern HANDLE hInstAstral;
extern HWND hWndAstral;

#define IDC_CALCMYK 1

#undef SPACE
#define NCOLUMNS 7
#define NPATCHES 11
#define SPACE   8	 /* 1/8" between patches */
#define BORDER	2	 /* thin black border */ 
#define PATCH   50   /* 1/2" patch size */
#define WIDTH 	((NCOLUMNS*PATCH)+((NCOLUMNS+1)*SPACE)+(2*BORDER))  /* about 4.25 inches */
#define HEIGHT	((NPATCHES*PATCH)+((NPATCHES+1)*SPACE)+(2*BORDER))	 /* about 6.75 inches */

static FIXED fSpaceW, fBorderW, fPatchW;
static FIXED fSpaceH, fBorderH, fPatchH;

/************************************************************************/
void DoMeasure( column, lpRGB, mRect )
/************************************************************************/
int column;
LPRGB lpRGB;
RECT mRect;
{
int x, y, patch;
RECT pRect;

x = FMUL(fSpaceW + (fPatchW/4) + (column * (fPatchW + fSpaceW)), 1);
for (patch = 0; patch < NPATCHES; ++patch)
	{
	y = FMUL(fSpaceH + (fPatchH/4) + (patch * (fPatchH + fSpaceH)), 1);
	pRect.left = mRect.left + x;
	pRect.right = pRect.left + FMUL(fPatchW/2, 1);
	pRect.top = mRect.top + y;
	pRect.bottom = pRect.top + FMUL(fPatchH/2, 1);
	Measure(&pRect, &lpRGB[patch], YES);
	}
}


/************************************************************************/
void MeasureTestStrip()
/************************************************************************/
{
RGBS Krgb[NPATCHES], Crgb[NPATCHES], Mrgb[NPATCHES], Yrgb[NPATCHES];
RGBS Rrgb[NPATCHES], Grgb[NPATCHES], Brgb[NPATCHES];

AutoMeasure(IDC_CALCMYK, 0, Krgb, Crgb, Mrgb, Yrgb, Rrgb, Grgb, Brgb);
}


/************************************************************************/
int AutoMeasure(iMode, iColor, Krgb, Crgb, Mrgb, Yrgb, Rrgb, Grgb, Brgb)
/************************************************************************/
int iMode, iColor;
LPRGB Krgb, Crgb, Mrgb, Yrgb, Rrgb, Grgb, Brgb;
{
int x, y, pixel, i, j;
long sum, rsum, gsum, bsum, count;
RECT mRect, pRect;
LPFRAME lpFrame;
LPTR lpDLine, m;
long color;
RGBS rgb;
LPRGB lpRGB;
long npix, nlin;
LPRGB rgbTbl[NCOLUMNS];

if ( !mask_rect(&mRect) )
	return(-1);
npix = mRect.right - mRect.left + 1;
nlin = mRect.bottom - mRect.top + 1;

fSpaceW = FGET(SPACE*npix, WIDTH);
fSpaceH = FGET(SPACE*nlin, HEIGHT);
fBorderW = FGET(BORDER*npix, WIDTH);
fBorderH = FGET(BORDER*nlin, HEIGHT);
fPatchW = FGET(PATCH*npix, WIDTH);
fPatchH = FGET(PATCH*nlin, HEIGHT);

rgbTbl[0] = Krgb;
rgbTbl[1] = Crgb;
rgbTbl[2] = Mrgb;
rgbTbl[3] = Yrgb;
rgbTbl[4] = Rrgb;
rgbTbl[5] = Grgb;
rgbTbl[6] = Brgb;

for (i = 0; i < NCOLUMNS; ++i)
    {
    lpRGB = rgbTbl[i];
    if (lpRGB)
        {
        DoMeasure(i, lpRGB, mRect);
        if (i == 0) /* black */
            {
            for (j = 0; j < NPATCHES; ++j)
		lpRGB[j].red = RGBtoL( &lpRGB[j] );
            }
        else if (iMode == IDC_CALCMYK)
            {
            for (j = 0; j < NPATCHES; ++j)
                {
                lpRGB[j].red = 255 - lpRGB[j].red;
                lpRGB[j].green = 255 - lpRGB[j].green;
                lpRGB[j].blue = 255 - lpRGB[j].blue;
                }
            }
        }
    }
return(0);
}


/************************************************************************/
long Measure( lpRect, lpRGB, fFileCoords )
/************************************************************************/
LPRECT lpRect;
LPRGB lpRGB;
BOOL fFileCoords;
{
LPTR lpLine;
RGBS rgb;
int x, y;
long rsum, gsum, bsum, count;

if (!fFileCoords)
    {
    Display2File(&lpRect->left, &lpRect->top );
    Display2File(&lpRect->right, &lpRect->bottom);
    }
rsum = gsum = bsum = count = 0;
for (y = lpRect->top; y <= lpRect->bottom; ++y)
	{
	if ( !(lpLine = CachePtr(0, lpRect->left, y, NO)) )
		continue;
	for (x = lpRect->left; x <= lpRect->right; ++x, lpLine += DEPTH)
		{
		frame_getRGB( lpLine, &rgb );
		rsum += rgb.red;
		gsum += rgb.green;
		bsum += rgb.blue;
		++count;
		}
	}

if (count)
    {
    lpRGB->red = rsum / count;
    lpRGB->green = gsum / count;
    lpRGB->blue = bsum / count;
    }
return( count );
}


#ifdef UNUSED
/************************************************************************/
ColorCorrect()
/************************************************************************/
    {
    RGBS Rrgb[CALPOINTS], Grgb[CALPOINTS], Brgb[CALPOINTS];
    double x[CALPOINTS], yRy[CALPOINTS], yRm[CALPOINTS];
    GRAYMAP MYMap;
    int i;
    FILE_SPEC file;
    
    AutoMeasure(IDC_CALCMYK, 0, NULL, NULL, NULL, NULL, Rrgb, NULL, NULL);
    
    for (i = 0; i < CALPOINTS; ++i)
        x[i] = i * 255.0 / 10.0;
    for (i = 0; i < CALPOINTS; ++i)
        {
        if (Rrgb[i].blue >= 255)
            break;
        yRy[i] = (double)Rrgb[i].blue;
        }
    for (; i < CALPOINTS; ++i)
        {
        yRy[i] = f4l(x[i], x, yRy, i);
        }
    
/*    MYMap.Color = LMAP; resetlut( &MYMap, CALPOINTS );
    for (i = 0; i < CALPOINTS; ++i)
        {
        MYMap.PntX[i] = (BYTE)(x[i] + 0.5);
        MYMap.PntY[i] = (BYTE)((yRy[i] * 255.0 / yRy[CALPOINTS-1]) + 0.5);
        }
    makelut(&MYMap);
    if (!DoSaveFileDlg(&file, IDS_SAVECALMAP, IDS_GRAYMAP_FILETYPE,
                       &Ctrl.CalMapDir, NULL))
         return;
    AstralMapSave(IDC_CALBLACK, &MYMap, NULL, NULL, NULL,
        	  NULL, NULL, NULL, NULL, 0, &file);
*/
       	  
    for (i = 0; i < CALPOINTS; ++i)
        {
        if (Rrgb[i].green >= 255)
            break;
        yRm[i] = (double)Rrgb[i].green;
        }
    for (; i < CALPOINTS; ++i)
        {
        yRm[i] = f4l(x[i], x, yRm, i);
        }
/*    MYMap.Color = LMAP; resetlut( &MYMap, CALPOINTS );
    for (i = 0; i < CALPOINTS; ++i)
        {
        MYMap.PntX[i] = (BYTE)(x[i] + 0.5);
        MYMap.PntY[i] = (BYTE)((yRm[i] * 255.0 / yRm[CALPOINTS-1]) + 0.5);
        }
    makelut(&MYMap);
    if (!DoSaveFileDlg(&file, IDS_SAVECALMAP, IDS_GRAYMAP_FILETYPE,
                       &Ctrl.CalMapDir, NULL))
         return;
    AstralMapSave(IDC_CALBLACK, &MYMap, NULL, NULL, NULL,
        	  NULL, NULL, NULL, NULL, 0, &file);
*/        	  
    MYMap.Color = LMAP; resetlut( &MYMap, CALPOINTS );
    for (i = 0; i < CALPOINTS; ++i)
        {
        MYMap.PntX[i] = (BYTE)(x[i] + 0.5);
        MYMap.PntY[i] = (BYTE)(yRy[i] - yRm[i] + 0.5);
        }
    makelut(&MYMap);
    if (!DoSaveFileDlg(&file, IDS_SAVECALMAP, IDS_GRAYMAP_FILETYPE,
                       &Ctrl.CalMapDir, NULL))
         return;
    AstralMapSave(IDC_CALBLACK, &MYMap, NULL, NULL, NULL,
        	  NULL, NULL, NULL, NULL, 0, &file);
    
    }
    
/************************************************************************/
CreateTestStrip()
/************************************************************************/
    {
    LPFRAME lpFrame;
    long color, white, black;
    RGBS rgb;
    LPTR lpData;
    int y, i, patch, depth;
    IMAGEHDL imageData;
    
    depth = 3;
    lpFrame = frame_open(depth, WIDTH, HEIGHT, 100);
    if (!lpFrame)
        {
        error("Error creating frame");
        return;
        }
    frame_set(lpFrame);
    rgb.red = 255;
    rgb.green = 255;
    rgb.blue = 255;
    white = frame_tocache(&rgb);
    rgb.red = 0;
    rgb.green = 0;
    rgb.blue = 0;
    black = frame_tocache(&rgb);
    rgb.red = 255;
    rgb.green = 0;
    rgb.blue = 255;
    color = frame_tocache(&rgb);
/*    for (y = 0; y < lpFrame->Ysize; ++y)
        {
        lpData = frame_ptr(0, 0, y, YES);
        if (!lpData)
            continue;
       frame_setpixels(lpData, lpFrame->Xsize, white);
        }*/
    y = 0;
    for (i = 0; i < BORDER; ++i, ++y)
        {
        lpData = frame_ptr(0, 0, y, YES);
        if (!lpData)
            continue;
        frame_setpixels(lpData, lpFrame->Xsize, black);
        }
        
    for (i = 0; i < SPACE; ++i, ++y)
        {
        lpData = frame_ptr(0, 0, y, YES);
        if (!lpData)
            continue;
        frame_setpixels(lpData, BORDER, black);
        lpData += BORDER * depth;
            
        frame_setpixels(lpData, lpFrame->Xsize-(2*BORDER), white);
        lpData += (lpFrame->Xsize-(2*BORDER)) * depth;

        frame_setpixels(lpData, BORDER, black);
        lpData += BORDER * depth;
        }
    for (patch = 0; patch < NPATCHES; ++patch)
        {
        for (i = 0; i < PATCH; ++i, ++y)
            {
            lpData = frame_ptr(0, 0, y, YES);
            if (!lpData)
                continue;
        
            frame_setpixels(lpData, BORDER, black);
            lpData += BORDER * depth;
            
            frame_setpixels(lpData, SPACE, white);
            lpData += SPACE * depth;
            
            /* black */
            rgb.red = 255-((patch * 255)/10);	 
            rgb.green = 255-((patch * 255)/10);
            rgb.blue = 255-((patch * 255)/10);
            color = frame_tocache(&rgb);
            frame_setpixels(lpData, PATCH, color);
            lpData += PATCH * depth;
            
            frame_setpixels(lpData, SPACE, white);
            lpData += SPACE * depth;
        
            /* cyan */
            rgb.red = 255-((patch * 255)/10);
            rgb.green = 255;
            rgb.blue = 255;
            color = frame_tocache(&rgb);
            frame_setpixels(lpData, PATCH, color);
            lpData += PATCH * depth;
        
            frame_setpixels(lpData, SPACE, white);
            lpData += SPACE * depth;
            
            /* magenta */
            rgb.red = 255;
            rgb.green = 255-((patch * 255)/10);
            rgb.blue = 255;
            color = frame_tocache(&rgb);
            frame_setpixels(lpData, PATCH, color);
            lpData += PATCH * depth;

            frame_setpixels(lpData, SPACE, white);
            lpData += SPACE * depth;
            
            /* yellow */
            rgb.red = 255;
            rgb.green = 255;
            rgb.blue = 255-((patch * 255)/10);
            color = frame_tocache(&rgb);
            frame_setpixels(lpData, PATCH, color);
            lpData += PATCH * depth;

            frame_setpixels(lpData, SPACE, white);
            lpData += SPACE * depth;
            
            /* red */
            rgb.red = 255;
            rgb.green = 255-((patch * 255)/10);
            rgb.blue = 255-((patch * 255)/10);
            color = frame_tocache(&rgb);
            frame_setpixels(lpData, PATCH, color);
            lpData += PATCH * depth;

            frame_setpixels(lpData, SPACE, white);
            lpData += SPACE * depth;
            
            /* green */
            rgb.red = 255-((patch * 255)/10);
            rgb.green = 255;
            rgb.blue = 255-((patch * 255)/10);
            color = frame_tocache(&rgb);
            frame_setpixels(lpData, PATCH, color);
            lpData += PATCH * depth;

            frame_setpixels(lpData, SPACE, white);
            lpData += SPACE * depth;
            
            /* blue */
            rgb.red = 255-((patch * 255)/10);
            rgb.green = 255-((patch * 255)/10);
            rgb.blue = 255;
            color = frame_tocache(&rgb);
            frame_setpixels(lpData, PATCH, color);
            lpData += PATCH * depth;

            frame_setpixels(lpData, SPACE, white);
            lpData += SPACE * depth;
            
            frame_setpixels(lpData, BORDER, black);
            lpData += BORDER * depth;
            }
        for (i = 0; i < SPACE; ++i, ++y)
            {
            lpData = frame_ptr(0, 0, y, YES);
            if (!lpData)
                continue;
            frame_setpixels(lpData, BORDER, black);
            lpData += BORDER * depth;
            
            frame_setpixels(lpData, lpFrame->Xsize-(2*BORDER), white);
            lpData += (lpFrame->Xsize-(2*BORDER)) * depth;

            frame_setpixels(lpData, BORDER, black);
            lpData += BORDER * depth;
            }
        }
    for (i = 0; i < BORDER; ++i, ++y)
        {
        lpData = frame_ptr(0, 0, y, YES);
        if (!lpData)
            continue;
        frame_setpixels(lpData, lpFrame->Xsize, black);
        }
        
    imageData = new_image_init( NULL, depth, WIDTH, HEIGHT, 24, 100, 100, lpFrame, IDC_SAVETIFF);
	NewImageWindow(imageData, NULL, TRUE);
	GrayMenus(NO);
    StatusSetup();
    StatusDisplay();
    }
#endif

#ifdef UNUSED
/***********************************************************************/
BOOL FAR PASCAL DlgPrintCalibProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	hDlg;
unsigned msg;
WORD	wParam;
long	lParam;
{
BOOL	Bool;
int	i, idStep, nStep;
char	szNewName[MAX_FNAME_LEN];
RECT rMask;
RGBS rgb;

switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	if ( !Calibrate.bInit )
		{
		for ( i=0; i<CALPOINTS; i++ )
			{
			Calibrate.CStep[i] = i*10;
			Calibrate.MStep[i] = i*10;
			Calibrate.YStep[i] = i*10;
			Calibrate.KStep[i] = i*10;
			}
		Calibrate.bStretch = NO;
		Calibrate.bInit = YES;
		Calibrate.iColor = IDC_CALIB_K;
		Calibrate.iMode = IDC_CALIB_CMYK;
		}
	for ( i=0; i<CALPOINTS; i++ )
		{
		SetDlgItemInt( hDlg, i+IDC_STEP0C, Calibrate.CStep[i], NO );
		SetDlgItemInt( hDlg, i+IDC_STEP0M, Calibrate.MStep[i], NO );
		SetDlgItemInt( hDlg, i+IDC_STEP0Y, Calibrate.YStep[i], NO );
		SetDlgItemInt( hDlg, i+IDC_STEP0K, Calibrate.KStep[i], NO );
		}
	CheckRadioButton( hDlg, IDC_CALIBSCAN, IDC_CALIBPRINT,
		IDC_CALIBSCAN+Calibrate.bStretch );
	CheckRadioButton( hDlg, IDC_CALIB_C, IDC_CALIB_K,
		Calibrate.iColor );
	CheckRadioButton( hDlg, IDC_CALIB_BLACK, IDC_CALIB_RGB,
		Calibrate.iMode );
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_MEASURE0:
	    case IDC_MEASURE1:
	    case IDC_MEASURE2:
	    case IDC_MEASURE3:
	    case IDC_MEASURE4:
	    case IDC_MEASURE5:
	    case IDC_MEASURE6:
	    case IDC_MEASURE7:
	    case IDC_MEASURE8:
	    case IDC_MEASURE9:
	    case IDC_MEASURE10:
		if ( !mask_rect( &rMask ) )
			{
			Message( IDS_ENOMASK );
			break;
			}
		AstralCursor( IDC_WAIT );
		nStep = ( wParam - IDC_MEASURE0 );
		MeasureMask(&rgb);
//		Print("(rgb)=(%d,%d,%d)", rgb.red, rgb.green, rgb.blue);
		if ( Calibrate.iColor == IDC_CALIB_K )
			{
			Calibrate.KStep[nStep] = TOPERCENT(rgb.red);
			idStep = IDC_STEP0K + nStep;
			SetDlgItemInt( hDlg, idStep, Calibrate.KStep[nStep],NO);
			}
		else
		if ( Calibrate.iColor == IDC_CALIB_C )
			{
			Calibrate.CStep[nStep] = TOPERCENT(rgb.red);
			idStep = IDC_STEP0C + nStep;
			SetDlgItemInt( hDlg, idStep, Calibrate.CStep[nStep],NO);
			}
		else
		if ( Calibrate.iColor == IDC_CALIB_M )
			{
			Calibrate.MStep[nStep] = TOPERCENT(rgb.green);
			idStep = IDC_STEP0M + nStep;
			SetDlgItemInt( hDlg, idStep, Calibrate.MStep[nStep],NO);
			}
		else
		if ( Calibrate.iColor == IDC_CALIB_Y )
			{
			Calibrate.YStep[nStep] = TOPERCENT(rgb.blue);
			idStep = IDC_STEP0Y + nStep;
			SetDlgItemInt( hDlg, idStep, Calibrate.YStep[nStep],NO);
			}
		SendMessage( hWndAstral, WM_COMMAND, IDM_MASKREMOVE, 0L );
		AstralCursor( NULL );
		break;

	    case IDC_STEP0C:
	    case IDC_STEP1C:
	    case IDC_STEP2C:
	    case IDC_STEP3C:
	    case IDC_STEP4C:
	    case IDC_STEP5C:
	    case IDC_STEP6C:
	    case IDC_STEP7C:
	    case IDC_STEP8C:
	    case IDC_STEP9C:
	    case IDC_STEP10C:
		Calibrate.CStep[wParam-IDC_STEP0C] = GetDlgItemInt( hDlg, wParam,
			&Bool, NO );
		break;

	    case IDC_STEP0M:
	    case IDC_STEP1M:
	    case IDC_STEP2M:
	    case IDC_STEP3M:
	    case IDC_STEP4M:
	    case IDC_STEP5M:
	    case IDC_STEP6M:
	    case IDC_STEP7M:
	    case IDC_STEP8M:
	    case IDC_STEP9M:
	    case IDC_STEP10M:
		Calibrate.MStep[wParam-IDC_STEP0M] = GetDlgItemInt( hDlg, wParam,
			&Bool, NO );
		break;

	    case IDC_STEP0Y:
	    case IDC_STEP1Y:
	    case IDC_STEP2Y:
	    case IDC_STEP3Y:
	    case IDC_STEP4Y:
	    case IDC_STEP5Y:
	    case IDC_STEP6Y:
	    case IDC_STEP7Y:
	    case IDC_STEP8Y:
	    case IDC_STEP9Y:
	    case IDC_STEP10Y:
		Calibrate.YStep[wParam-IDC_STEP0Y] = GetDlgItemInt( hDlg, wParam,
			&Bool, NO );
		break;

	    case IDC_STEP0K:
	    case IDC_STEP1K:
	    case IDC_STEP2K:
	    case IDC_STEP3K:
	    case IDC_STEP4K:
	    case IDC_STEP5K:
	    case IDC_STEP6K:
	    case IDC_STEP7K:
	    case IDC_STEP8K:
	    case IDC_STEP9K:
	    case IDC_STEP10K:
		Calibrate.KStep[wParam-IDC_STEP0K] = GetDlgItemInt( hDlg, wParam,
			&Bool, NO );
		break;

	    case IDC_CALIBSCAN:
	    case IDC_CALIBPRINT:
		Calibrate.bStretch = wParam - IDC_CALIBSCAN;
		CheckRadioButton( hDlg, IDC_CALIBSCAN, IDC_CALIBPRINT, wParam );
		break;

	    case IDC_CALIB_C:
	    case IDC_CALIB_M:
	    case IDC_CALIB_Y:
	    case IDC_CALIB_K:
		Calibrate.iColor = wParam;
		CheckRadioButton( hDlg, IDC_CALIB_C, IDC_CALIB_K, wParam );
		break;

	    case IDC_CALIB_BLACK:
	    case IDC_CALIB_CMYK:
	    case IDC_CALIB_RGB:
		Calibrate.iMode = wParam;
		CheckRadioButton( hDlg, IDC_CALIB_BLACK, IDC_CALIB_RGB, wParam);
		break;

	    case IDC_CALIBSAVE:
		/* Make sure the points are in ascending order */
		for ( i=0; i<CALPOINTS-1; i++ )
			{
			if ( Calibrate.KStep[i] > Calibrate.KStep[i+1] )
				break;
			}
		if ( i != CALPOINTS-1 )
			{
			Message( IDS_EMAPASC );
			break;
			}

		/* Clear out the Graymap structure */
		ResetMap( &Calmap, CALPOINTS );

		/* Setup the Graymap structure with the calibration points */
		Calmap.bStretch = Calibrate.bStretch;
		for ( i=0; i<CALPOINTS; i++ )
			{
			Calmap.Pnt[i].x = TOGRAY( i*10 );
			Calmap.Pnt[i].y = TOGRAY( Calibrate.KStep[i] );
			}
		MakeMap( &Calmap );

		if ( DoOpenDlg( hDlg, IDD_CALIBSAVE, 0, szNewName, YES ) )
			SaveMap( &Calmap, NULL, NULL, NULL, szNewName );
		SetFocus( hDlg );
		break;

	    case IDC_CALIBLOAD:
		if ( !DoOpenDlg( hDlg, IDD_CALIBLOAD, 0, szNewName,NO))
			{
			SetFocus( hDlg );
			break;
			}

		LoadMap( &Calmap, 0,0,0, szNewName );
		if ( Calmap.Points != CALPOINTS )
			{ /* Make sure it has the right number of points */
			Message( IDS_EBADCAL, (LPTR)szNewName );
			break;
			}
		Calibrate.bStretch = (Calmap.bStretch == YES);
		for ( i=0; i<CALPOINTS; i++ )
			Calibrate.KStep[i] = TOPERCENT( Calmap.Pnt[i].x );
		SetDlgItemInt( hDlg, IDC_STEP0K, Calibrate.KStep[0], NO );
		SetDlgItemInt( hDlg, IDC_STEP1K, Calibrate.KStep[1], NO );
		SetDlgItemInt( hDlg, IDC_STEP2K, Calibrate.KStep[2], NO );
		SetDlgItemInt( hDlg, IDC_STEP3K, Calibrate.KStep[3], NO );
		SetDlgItemInt( hDlg, IDC_STEP4K, Calibrate.KStep[4], NO );
		SetDlgItemInt( hDlg, IDC_STEP5K, Calibrate.KStep[5], NO );
		SetDlgItemInt( hDlg, IDC_STEP6K, Calibrate.KStep[6], NO );
		SetDlgItemInt( hDlg, IDC_STEP7K, Calibrate.KStep[7], NO );
		SetDlgItemInt( hDlg, IDC_STEP8K, Calibrate.KStep[8], NO );
		SetDlgItemInt( hDlg, IDC_STEP9K, Calibrate.KStep[9], NO );
		SetDlgItemInt( hDlg, IDC_STEP10K, Calibrate.KStep[10], NO );
		CheckRadioButton( hDlg, IDC_CALIBSCAN, IDC_CALIBPRINT,
			IDC_CALIBSCAN+Calibrate.bStretch );
		SetFocus( hDlg );
		break;

	    case IDCANCEL:
		AstralDlgEnd( hDlg, FALSE );
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }
return( TRUE );
}
#endif
