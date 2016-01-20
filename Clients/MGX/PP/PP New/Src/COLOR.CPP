//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"
#include "color.h"
#include "macros.h"

//**FIX
//static BOOL bOldWay = FALSE;

typedef struct _sepdesc
{
	BYTE		UCRlut[LUTSIZE];
	MAP			BGLimitedMap;
	MAP			ClipBlack;
	int 		TakeMfromR;
	int 		TakeYfromR;
	int 		TakeYfromG;
	int 		TakeCfromG;
	int 		TakeCfromB;
	int 		TakeMfromB;
	
	INT16 		TakeMfromRLut[2*LUTSIZE];
	INT16 		TakeYfromRLut[2*LUTSIZE];
	INT16 		TakeYfromGLut[2*LUTSIZE];
	INT16 		TakeCfromGLut[2*LUTSIZE];
	INT16 		TakeCfromBLut[2*LUTSIZE];
	INT16 		TakeMfromBLut[2*LUTSIZE];
	
	int			TakeMax;
	int			maxC;
	int			maxM;
	int			maxY;
	int			InkMode;
	int			SatBoost;
	BOOL		DoInkCorrect;
	BOOL		DoInkClipping;
	BOOL		DoCalibration;
	BOOL		DoCalOnConvert;
	BOOL		DoBlackAndUCR;
	BOOL		DoSatBoost;
	BYTE		CMYKLuts[4][LUTSIZE];
	BYTE		CMYKInverseLuts[4][LUTSIZE];
	BYTE		GrayLut[LUTSIZE];
	BYTE		SatLut[LUTSIZE];
	BYTE		SatInverseLut[LUTSIZE];
} SEPDESC, FAR * LPSEPDESC;

static LPSEPDESC lpSepDesc = NULL;

LOCAL void ClrFrameRGBtoCMYK(LPRGB lpRGB, LPCMYK lpCMYK, int iCount);
LOCAL void ClrFrameCMYKtoRGB(LPCMYK lpCMYK, LPRGB lpRGB, int iCount);
LOCAL BOOL CreateRGBCalLut(LPCALIBRATE lpCalibrate, int nChannel, LPTR lpLut,	LPTR lpInverseLut);
LOCAL BOOL CreateGrayCalLut(LPCALIBRATE lpCalibrate, LPTR lpLut, LPTR lpInverseLut);
LOCAL void CreateGainMap(LPMAP lpMap, int DotGain);
LOCAL void CreateClipMap(LPMAP lpMap, int Hilight, int Shadow);
LOCAL void CreateSatLut(int SatBoost, LPTR lpLut, LPTR lpInverseLut);
LOCAL void MapSat(LPRGB lpRGB, LPTR lpSatMap);
LOCAL void make_TakeFromLut(LPINT16 TakefromLut, int TakefromVal);

//***********************************************************************
BOOL InitPrintStyle(LPSEPARATE lpSep, LPCALIBRATE lpCalibrate)
//***********************************************************************
{
	LFIXED rate;
	int v, TotalInk, total, i, maxC, maxM, maxY, nChannel;
	LPPOINT lpPoints;
	int BlackLimit;
	RGBS rgb;
	CMYKS cmyk; 
	BOOL fClip;

	if (!lpSepDesc)
		lpSepDesc = (LPSEPDESC)Alloc(sizeof(SEPDESC));
	if (!lpSepDesc)
	{
		Message(IDS_EMEMALLOC);
		return(FALSE);
	}
	clr((LPTR)lpSepDesc, sizeof(SEPDESC));

	lpSepDesc->DoBlackAndUCR = lpSep->DoBlackAndUCR;
	lpSepDesc->DoInkCorrect = lpSep->DoInkCorrect;
	if ( !Control.Retail )
	{
		lpSepDesc->DoInkCorrect = NO; // Don't allow it
		lpSepDesc->DoBlackAndUCR = NO; // Only allow the default
	}

	if (lpSepDesc->DoBlackAndUCR)
	{
		// copy black map into limit map so we can modify limit map
		lpSepDesc->BGLimitedMap = lpSep->BGMap;
		BlackLimit = lpSep->BlackLimit;
		if (BlackLimit < 100)
		{
			// scale y's of points to BlackLimit
			lpPoints = lpSepDesc->BGLimitedMap.Pnt;
			for (i = 0; i < lpSepDesc->BGLimitedMap.Points; ++i)
				lpPoints[i].y = ((lpPoints[i].y * BlackLimit) + 50) / 100;
			MakeMap(&lpSepDesc->BGLimitedMap);
		}

		// Make the UCRlut for faster processing
		rate = FGET( lpSep->UCR, 100 );
		for ( v=0; v<LUTSIZE; v++ )
			lpSepDesc->UCRlut[v] = FMUL( v, rate );
	}
	
	if (lpSepDesc->DoInkCorrect)
	{
		lpSepDesc->TakeMfromR = 100 - bound( 100+lpSep->MfromR, 0, 100 );
		lpSepDesc->TakeYfromR = 100 - bound( 100-lpSep->MfromR, 0, 100 );
		lpSepDesc->TakeYfromG = 100 - bound( 100+lpSep->YfromG, 0, 100 );
		lpSepDesc->TakeCfromG = 100 - bound( 100-lpSep->YfromG, 0, 100 );
		lpSepDesc->TakeCfromB = 100 - bound( 100+lpSep->CfromB, 0, 100 );
		lpSepDesc->TakeMfromB = 100 - bound( 100-lpSep->CfromB, 0, 100 );

		// this crap is so that we can handle all possible ink correction
		// settings even though 99.99% of the time the first one will be used

		lpSepDesc->TakeMax = max(lpSepDesc->TakeMfromR, lpSepDesc->TakeYfromR);
		lpSepDesc->TakeMax = max(lpSepDesc->TakeMax, lpSepDesc->TakeYfromG);
		lpSepDesc->TakeMax = max(lpSepDesc->TakeMax, lpSepDesc->TakeCfromG);
		lpSepDesc->TakeMax = max(lpSepDesc->TakeMax, lpSepDesc->TakeCfromB);
		lpSepDesc->TakeMax = max(lpSepDesc->TakeMax, lpSepDesc->TakeMfromB);

		if (lpSepDesc->TakeMax)
		{
			// check for normal settings first
			if (!lpSepDesc->TakeMfromR && !lpSepDesc->TakeCfromG && !lpSepDesc->TakeCfromB)
				lpSepDesc->InkMode = 0;
			else
			// check for the opposite of normal
			if (!lpSepDesc->TakeYfromR && !lpSepDesc->TakeYfromG && !lpSepDesc->TakeMfromB)
				lpSepDesc->InkMode = 1;
			else
			// all other combinations
			if (!lpSepDesc->TakeYfromR && !lpSepDesc->TakeCfromG && !lpSepDesc->TakeCfromB)
				lpSepDesc->InkMode = 2;
			else
			if (!lpSepDesc->TakeMfromR && !lpSepDesc->TakeYfromG && !lpSepDesc->TakeCfromB)
				lpSepDesc->InkMode = 3;
			else
			if (!lpSepDesc->TakeMfromR && !lpSepDesc->TakeCfromG && !lpSepDesc->TakeMfromB)
				lpSepDesc->InkMode = 4;
			else
			if (!lpSepDesc->TakeYfromR && !lpSepDesc->TakeYfromG && !lpSepDesc->TakeCfromB)
				lpSepDesc->InkMode = 5;
			else
			if (!lpSepDesc->TakeMfromR && !lpSepDesc->TakeYfromG && !lpSepDesc->TakeMfromB)
				lpSepDesc->InkMode = 6;
			//else
			if (!lpSepDesc->TakeYfromR && !lpSepDesc->TakeCfromG && !lpSepDesc->TakeMfromB)
				lpSepDesc->InkMode = 7;
				
			// setup TakeFromLuts
			make_TakeFromLut(lpSepDesc->TakeMfromRLut, lpSepDesc->TakeMfromR);
			make_TakeFromLut(lpSepDesc->TakeYfromRLut, lpSepDesc->TakeYfromR);
			make_TakeFromLut(lpSepDesc->TakeYfromGLut, lpSepDesc->TakeYfromG);
			make_TakeFromLut(lpSepDesc->TakeCfromGLut, lpSepDesc->TakeCfromG);
			make_TakeFromLut(lpSepDesc->TakeCfromBLut, lpSepDesc->TakeCfromB);
			make_TakeFromLut(lpSepDesc->TakeMfromBLut, lpSepDesc->TakeMfromB);
		}
		else
			lpSepDesc->DoInkCorrect = NO;
	}


	// set max values for CMY based on total ink setting
	lpSepDesc->DoCalOnConvert = FALSE;
	lpSepDesc->maxC = lpSepDesc->maxM = lpSepDesc->maxY = 255;
	if (lpSep->TotalInk < 400 && lpSep->DoBlackAndUCR)
	{
		// get values for CMYK at black with no max values
		rgb.red = rgb.green = rgb.blue = 0;
		ClrFrameRGBtoCMYK(&rgb, &cmyk, 1);

		// see if we need to do any ink limiting
		TotalInk = (((long)lpSep->TotalInk * 255L)+50L)/100L;
		total = cmyk.c + cmyk.m + cmyk.y + cmyk.k;
		if (total > TotalInk)
		{
			// subtract 1/3 of the difference from each channel
			total = (total - TotalInk) / 3;
			maxC = cmyk.c - total;
			maxM = cmyk.m - total;
			maxY = cmyk.y - total;
			// find point where clipping of all three channels starts
			for (i = 1; i < 256; ++i)
			{
				// get values for CMYK at black with no max values
				rgb.red = rgb.green = rgb.blue = 255-i;
				ClrFrameRGBtoCMYK(&rgb, &cmyk, 1);
				fClip = TRUE;
				if (cmyk.c <= maxC)
					fClip = FALSE;
				if (cmyk.m <= maxM)
					fClip = FALSE;
				if (cmyk.y <= maxY)
					fClip = FALSE;
				if (fClip)
					break;
			}	
			if (fClip)
			{
				ResetMap(&lpSepDesc->ClipBlack, 3, 0);
				lpSepDesc->ClipBlack.Pnt[0].x = 0;
				lpSepDesc->ClipBlack.Pnt[0].y = 0;
				lpSepDesc->ClipBlack.Pnt[1].x = i;
				lpSepDesc->ClipBlack.Pnt[1].y = cmyk.k;
				lpSepDesc->ClipBlack.Pnt[2].x = 255;
				lpSepDesc->ClipBlack.Pnt[2].y = lpSepDesc->BGLimitedMap.Lut[255];
				MakeMap(&lpSepDesc->ClipBlack);
				lpSepDesc->maxC = maxC;
				lpSepDesc->maxM = maxM;
				lpSepDesc->maxY = maxY;
			}
		}

	}
	lpSepDesc->DoInkClipping = 	lpSepDesc->maxC < 255 ||
								lpSepDesc->maxM < 255 ||
								lpSepDesc->maxY < 255;

	lpSepDesc->DoCalibration = DoCalibrate(lpCalibrate);
	lpSepDesc->DoCalOnConvert = lpSepDesc->DoCalibration &&
								lpCalibrate->DoCalOnConvert;

	if (lpSepDesc->DoCalibration)
	{
		for (nChannel = 0; nChannel < 4; ++nChannel)
		{
			if (!CreateCMYKCalLut(lpCalibrate, nChannel,
								lpSepDesc->CMYKLuts[nChannel],
								lpSepDesc->CMYKInverseLuts[nChannel]))
			{
				lpSepDesc->DoCalibration = FALSE;
				lpSepDesc->DoCalOnConvert = FALSE;
				break;
			}
		}
 		if (!CreateGrayCalLut(lpCalibrate, lpSepDesc->GrayLut, NULL))
		{
			lpSepDesc->DoCalibration = FALSE;
		}
	}

	lpSepDesc->DoSatBoost = lpSep->SatBoost > 0;
	if (lpSepDesc->DoSatBoost)
		CreateSatLut(lpSep->SatBoost, lpSepDesc->SatLut, lpSepDesc->SatInverseLut);

	SetRGBToCMYKProc( ClrFrameRGBtoCMYK, ClrFrameCMYKtoRGB);
	return(TRUE);
}

/***********************************************************************/
BOOL DoCalibrate(LPCALIBRATE lpCalibrate)
/***********************************************************************/
{
	return(	lpCalibrate->DoCalibration ||
			lpCalibrate->DotGain ||
			lpCalibrate->Hilight > 0 ||
			lpCalibrate->Shadow < 255);
}

// Notes to remember:
//		Calibration maps are always CMYK
//		Balance maps are Master,Red,Green,Blue,Black maps
//		Dot Gain maps create work on CMYK

/***********************************************************************/
BOOL CreateCMYKCalLut(
	LPCALIBRATE lpCalibrate,
	int nChannel,				// 0=CYAN, 1=MAGENTA, 2=YELLOW, 3=BLACK
	LPTR lpLut,		
	LPTR lpInverseLut)
/***********************************************************************/
{
	LPFLOAT lpPnts, lpCalPnts, lpTmpPnts;
	MAP TempMap;

	lpPnts = (LPFLOAT)Alloc(512*sizeof(FLOAT));
	if (!lpPnts)
	{
		Message(IDS_EMEMALLOC);
		return(FALSE);
	}
	lpCalPnts = &lpPnts[0];
	lpTmpPnts = &lpPnts[256];

	// start with a ramp
	ResetMap(&TempMap, 2, NO);
	MakeFloatMap(&TempMap, lpCalPnts);

	// do dot clipping
	if (lpCalibrate->Hilight > 0 || lpCalibrate->Shadow < 255)
	{
		// create the dot clipping map
		CreateClipMap(&TempMap, lpCalibrate->Hilight, lpCalibrate->Shadow);
		// create a map that maps Lut indices to output values
		MakeFloatMap(&TempMap, lpTmpPnts);
		// lookup current map in dot clip map
		MapCombine(lpCalPnts, lpTmpPnts);
	}

	// do dot gain adjustment
	if (lpCalibrate->DotGain)
	{
		// create the dot gain map
		CreateGainMap(&TempMap, lpCalibrate->DotGain);
		// create a map that maps Lut indices to output values
		MakeFloatMap(&TempMap, lpTmpPnts);
		// lookup current map in dot gain map
		MapCombine(lpCalPnts, lpTmpPnts);
	}

	// do calibation adjustments
	if (lpCalibrate->DoCalibration)
	{
		// get master map
		MakeFloatMap(&lpCalibrate->CalMap[CAL_MASTER], lpTmpPnts);
		// lookup current map in master cal map
		MapCombine(lpCalPnts, lpTmpPnts);

		// create high-precision calibration map
		MakeFloatMap(&lpCalibrate->CalMap[nChannel+1], lpTmpPnts);
		// lookup current map in calibration map
		MapCombine(lpCalPnts, lpTmpPnts);
	}

	MapCreateLut(lpCalPnts, lpLut);
	if (lpInverseLut)
	{
		MapReflectXY(lpCalPnts);
		MapCreateLut(lpCalPnts, lpInverseLut);
	}
	FreeUp(lpPnts);
	return(TRUE);
}

/***********************************************************************/
LOCAL BOOL CreateRGBCalLut(
	LPCALIBRATE lpCalibrate,
	int nChannel,				// 0=RED, 1=GREEN, 2=BLUE
	LPTR lpLut,		
	LPTR lpInverseLut)
/***********************************************************************/
{
	LPFLOAT lpPnts, lpCalPnts, lpTmpPnts;
	MAP TempMap;

	lpPnts = (LPFLOAT)Alloc(512*sizeof(FLOAT));
	if (!lpPnts)
	{
		Message(IDS_EMEMALLOC);
		return(FALSE);
	}
	lpCalPnts = &lpPnts[0];
	lpTmpPnts = &lpPnts[256];

	// start with a ramp
	ResetMap(&TempMap, 2, NO);
	MakeFloatMap(&TempMap, lpCalPnts);

	// do dot clipping
	if (lpCalibrate->Hilight > 0 || lpCalibrate->Shadow < 255)
	{
		// create the dot clipping map
		CreateClipMap(&TempMap, lpCalibrate->Hilight, lpCalibrate->Shadow);
		// make work on RGB
		ReverseMap(&TempMap, FALSE);
		// create a map that maps Lut indices to output values
		MakeFloatMap(&TempMap, lpTmpPnts);
		// lookup current map in dot clip map
		MapCombine(lpCalPnts, lpTmpPnts);
	}

	// do dot gain adjustment
	if (lpCalibrate->DotGain)
	{
		CreateGainMap(&TempMap, lpCalibrate->DotGain);
		// make work on RGB
		ReverseMap(&TempMap, FALSE);
		// create a map that maps Lut indices to output values
		MakeFloatMap(&TempMap, lpTmpPnts);
		// lookup current map in dot gain map
		MapCombine(lpCalPnts, lpTmpPnts);
	}

	// do calibration
	if (lpCalibrate->DoCalibration)
	{
		// make maps work on RGB
		ReverseMap(&lpCalibrate->CalMap[CAL_MASTER], FALSE);
		// get master map
		MakeFloatMap(&lpCalibrate->CalMap[CAL_MASTER], lpTmpPnts);
		// lookup current map in master cal map
		MapCombine(lpCalPnts, lpTmpPnts);
		// make maps work on CMYK
		ReverseMap(&lpCalibrate->CalMap[CAL_MASTER], FALSE);

		// get calibration map
		// make maps work on RGB
		ReverseMap(&lpCalibrate->CalMap[nChannel+1], FALSE);
		// create high-precision map
		MakeFloatMap(&lpCalibrate->CalMap[nChannel+1], lpTmpPnts);
		// make map work on CMYK
		ReverseMap(&lpCalibrate->CalMap[nChannel+1], FALSE);
		// lookup current map in calibration map
		MapCombine(lpCalPnts, lpTmpPnts);
	}

	MapCreateLut(lpCalPnts, lpLut);
	if (lpInverseLut)
	{
		MapReflectXY(lpCalPnts);
		MapCreateLut(lpCalPnts, lpInverseLut);
	}
	FreeUp(lpPnts);
	return(TRUE);
}

/***********************************************************************/
LOCAL BOOL CreateGrayCalLut(
	LPCALIBRATE lpCalibrate,
	LPTR lpLut,		
	LPTR lpInverseLut)
/***********************************************************************/
{
	LPFLOAT lpPnts, lpCalPnts, lpTmpPnts;
	MAP TempMap;

	lpPnts = (LPFLOAT)Alloc(512*sizeof(FLOAT));
	if (!lpPnts)
	{
		Message(IDS_EMEMALLOC);
		return(FALSE);
	}
	lpCalPnts = &lpPnts[0];
	lpTmpPnts = &lpPnts[256];

	// start with a ramp
	ResetMap(&TempMap, 2, NO);
	MakeFloatMap(&TempMap, lpCalPnts);

	// do dot clipping
	if (lpCalibrate->Hilight > 0 || lpCalibrate->Shadow < 255)
	{
		// create the dot clipping map
		CreateClipMap(&TempMap, lpCalibrate->Hilight, lpCalibrate->Shadow);
		// make work on Gray
		ReverseMap(&TempMap, FALSE);
		// create a map that maps Lut indices to output values
		MakeFloatMap(&TempMap, lpTmpPnts);
		// lookup current map in dot clip map
		MapCombine(lpCalPnts, lpTmpPnts);
	}

	// do dot gain adjustment
	if (lpCalibrate->DotGain)
	{
		CreateGainMap(&TempMap, lpCalibrate->DotGain);
		// make work on Gray
		ReverseMap(&TempMap, FALSE);
		// create a map that maps Lut indices to output values
		MakeFloatMap(&TempMap, lpTmpPnts);
		// lookup current map into dot gain map
		MapCombine(lpCalPnts, lpTmpPnts);
	}

	// do calibration
	if (lpCalibrate->DoCalibration)
	{
		// make maps work on RGB
		ReverseMap(&lpCalibrate->CalMap[CAL_MASTER], FALSE);
		// get master map
		MakeFloatMap(&lpCalibrate->CalMap[CAL_MASTER], lpTmpPnts);
		// lookup current map in master cal map
		MapCombine(lpCalPnts, lpTmpPnts);
		// make maps work on CMYK
		ReverseMap(&lpCalibrate->CalMap[CAL_MASTER], FALSE);

		// get calibration map
		// make maps work on Gray
		ReverseMap(&lpCalibrate->CalMap[CAL_BLACK], FALSE);
		// create high-precision map
		MakeFloatMap(&lpCalibrate->CalMap[CAL_BLACK], lpTmpPnts);
		// make map work on K
		ReverseMap(&lpCalibrate->CalMap[CAL_BLACK], FALSE);
		// lookup current map in calibration map
		MapCombine(lpCalPnts, lpTmpPnts);
	}

	MapCreateLut(lpCalPnts, lpLut);
	if (lpInverseLut)
	{
		MapReflectXY(lpCalPnts);
		MapCreateLut(lpCalPnts, lpInverseLut);
	}
	FreeUp(lpPnts);
	return(TRUE);
}

/***********************************************************************/
LOCAL void CreateGainMap(LPMAP lpMap, int DotGain)
/***********************************************************************/
{
	ResetMap(lpMap, 3, REVERSE_LUTXY);
	lpMap->iCurves = SMOOTH_NORMAL;
	lpMap->Pnt[1].y += ((DotGain * 255) + 50) / 100;
	if (lpMap->Pnt[1].y > 255)
		lpMap->Pnt[1].y = 255;
}

/***********************************************************************/
LOCAL void CreateClipMap(LPMAP lpMap, int Hilight, int Shadow)
/***********************************************************************/
{
	ResetMap(lpMap, 2, NO);
	lpMap->Pnt[0].y = Hilight;
	lpMap->Pnt[1].y = Shadow;
}

/***********************************************************************/
LOCAL void CreateSatLut(int SatBoost, LPTR lpLut, LPTR lpInverseLut)
/***********************************************************************/
{
	MAP SatMap;

	SatBoost = TOGRAY(SatBoost);
	ResetMap(&SatMap, 3, NO);
	SatMap.iCurves = SMOOTH_NORMAL;
	SatMap.Pnt[1].y += SatBoost;
	if (SatMap.Pnt[1].y > 255)
		SatMap.Pnt[1].y = 255;
	MakeMap(&SatMap);
	copy(SatMap.Lut, lpLut, LUTSIZE);
	if (lpInverseLut)
	{
		ReverseMap(&SatMap, TRUE);
		copy(SatMap.Lut, lpInverseLut, LUTSIZE);
	}
}

/***********************************************************************/

LOCAL void ClrFrameRGBtoCMYK(LPRGB lpRGB, LPCMYK lpCMYK, int iCount)
{
	register int c, m, y, k;
	int maxC, maxM, maxY;
	int oldc, oldm, oldy;
	int gray, gcr, bump, limit;
	BOOL fClip;
	LPTR lpCLut, lpMLut, lpYLut, lpKLut, lpSatLut;
	HSLS hsl;
	RGBS rgb;


	if (!lpSepDesc)
		return;

	lpCLut = lpSatLut = NULL;
	if (lpSepDesc->DoCalOnConvert)
	{
		lpCLut = lpSepDesc->CMYKLuts[0];
		lpMLut = lpSepDesc->CMYKLuts[1];
		lpYLut = lpSepDesc->CMYKLuts[2];
		lpKLut = lpSepDesc->CMYKLuts[3];
	}
	if (lpSepDesc->DoInkClipping)
	{
		maxC = lpSepDesc->maxC;
		maxM = lpSepDesc->maxM;
		maxY = lpSepDesc->maxY;
	}
	
	if (lpSepDesc->DoSatBoost)
		lpSatLut = lpSepDesc->SatLut;

	while ( --iCount >= 0 )
	{
		// RGB to CMY conversion
		if (lpSatLut)
		{
//				RGBtoHSL(lpRGB->red, lpRGB->green, lpRGB->blue, &hsl);
//				hsl.sat = lpSatLut[hsl.sat];
//				HSLtoRGB(hsl.hue, hsl.sat, hsl.lum, &rgb);
				rgb = *lpRGB;
				MapSat(&rgb, lpSatLut);

			c = 255 - rgb.red;
			m = 255 - rgb.green;
			y = 255 - rgb.blue;
		}
		else
		{
			c = 255 - lpRGB->red;
			m = 255 - lpRGB->green;
			y = 255 - lpRGB->blue;
		}
		k = 0;


		lpRGB++;

		if ( lpSepDesc->DoBlackAndUCR )
		{ // Black generation
			gray = min( c, min( m, y ) );
			if (gray)
			{
				k = lpSepDesc->BGLimitedMap.Lut[gray];

				if ( k > gray ) k = gray;

				// Under Color removal
				if ( gcr = lpSepDesc->UCRlut[ k ] )
				{
					c -= gcr; // no bounding necessary if k is bound above
					m -= gcr;
					y -= gcr;
				}
			}
		}
		else
			gray = 0;

		if ( lpSepDesc->DoInkCorrect)
		{ // Ink Correction
			if (lpSepDesc->InkMode == 0)
			{
				if (lpSepDesc->TakeMfromB && (c > 0))
					m -= lpSepDesc->TakeMfromBLut[c];
				if (lpSepDesc->TakeYfromG && (c > 0))
					y -= lpSepDesc->TakeYfromGLut[c];
				if (lpSepDesc->TakeYfromR && (m > 0))
					y -= lpSepDesc->TakeYfromRLut[m];
			}
			else
			if (lpSepDesc->InkMode == 1)
			{
				if (lpSepDesc->TakeMfromR && (y > 0))
					m -= lpSepDesc->TakeMfromRLut[y];
				if (lpSepDesc->TakeCfromB && (m > 0))
					c -= lpSepDesc->TakeCfromBLut[m];
				if (lpSepDesc->TakeCfromG && (y > 0))
					c -= lpSepDesc->TakeCfromGLut[y];
			}
			else
			if (lpSepDesc->InkMode == 2)
			{
				if (lpSepDesc->TakeMfromB && (c > 0))
					m -= lpSepDesc->TakeMfromBLut[c];
				if (lpSepDesc->TakeYfromG && (c > 0))
					y -= lpSepDesc->TakeYfromGLut[c];
				if (lpSepDesc->TakeMfromR && (y > 0))
					m -= lpSepDesc->TakeMfromRLut[y];
			}
			else
			if (lpSepDesc->InkMode == 3)
			{
				if (lpSepDesc->TakeMfromB == lpSepDesc->TakeMax)
				{
					if (lpSepDesc->TakeMfromB && (c > 0))
						m -= lpSepDesc->TakeMfromBLut[c];
					if (lpSepDesc->TakeYfromR && (m > 0))
						y -= lpSepDesc->TakeYfromRLut[m];
					if (lpSepDesc->TakeCfromG && (y > 0))
						c -= lpSepDesc->TakeCfromGLut[y];
				}
				else
				if (lpSepDesc->TakeYfromR == lpSepDesc->TakeMax)
				{
					if (lpSepDesc->TakeYfromR && (m > 0))
						y -= lpSepDesc->TakeYfromRLut[m];
					if (lpSepDesc->TakeMfromB && (c > 0))
						m -= lpSepDesc->TakeMfromBLut[c];
					if (lpSepDesc->TakeCfromG && (y > 0))
						c -= lpSepDesc->TakeCfromGLut[y];
				}
				else
				//if (lpSepDesc->TakeCfromG == lpSepDesc->TakeMax)
				{
					if (lpSepDesc->TakeCfromG && (y > 0))
						c -= lpSepDesc->TakeCfromGLut[y];
					if (lpSepDesc->TakeMfromB && (c > 0))
						m -= lpSepDesc->TakeMfromBLut[c];
					if (lpSepDesc->TakeYfromR && (m > 0))
						y -= lpSepDesc->TakeYfromRLut[m];
				}
			}
			else
			if (lpSepDesc->InkMode == 4)
			{
				if (lpSepDesc->TakeCfromB && (m > 0))
					c -= lpSepDesc->TakeCfromBLut[m];
				if (lpSepDesc->TakeYfromG && (c > 0))
					y -= lpSepDesc->TakeYfromGLut[c];
				if (lpSepDesc->TakeYfromR && (m > 0))
					y -= lpSepDesc->TakeYfromRLut[m];
			}
			else
			if (lpSepDesc->InkMode == 5)
			{
				if (lpSepDesc->TakeCfromG && (y > 0))
					c -= lpSepDesc->TakeCfromGLut[y];
				if (lpSepDesc->TakeMfromB && (c > 0))
					m -= lpSepDesc->TakeMfromBLut[c];
				if (lpSepDesc->TakeMfromR && (y > 0))
					m -= lpSepDesc->TakeMfromRLut[y];
			}
			else
			if (lpSepDesc->InkMode == 6)
			{
				if (lpSepDesc->TakeCfromB && (m > 0))
					c -= lpSepDesc->TakeCfromBLut[m];
				if (lpSepDesc->TakeYfromR && (m > 0))
					y -= lpSepDesc->TakeYfromRLut[m];
				if (lpSepDesc->TakeCfromG && (y > 0))
					c -= lpSepDesc->TakeCfromGLut[y];
			}
			else
			//if (lpSepDesc->InkMode == 7)
			{
				if (lpSepDesc->TakeCfromB == lpSepDesc->TakeMax)
				{
					if (lpSepDesc->TakeCfromB && (m > 0))
						c -= lpSepDesc->TakeCfromBLut[m];
					if (lpSepDesc->TakeMfromR && (y > 0))
						m -= lpSepDesc->TakeMfromRLut[y];
					if (lpSepDesc->TakeYfromG && (c > 0))
						y -= lpSepDesc->TakeYfromGLut[c];
				}
				else
				if (lpSepDesc->TakeMfromR == lpSepDesc->TakeMax)
				{
					if (lpSepDesc->TakeMfromR && (y > 0))
						m -= lpSepDesc->TakeMfromRLut[y];
					if (lpSepDesc->TakeCfromB && (m > 0))
						c -= lpSepDesc->TakeCfromBLut[m];
					if (lpSepDesc->TakeYfromG && (c > 0))
						y -= lpSepDesc->TakeYfromGLut[c];
				}
				else
				//if (lpSepDesc->TakeYfromG == lpSepDesc->TakeMax)
				{
					if (lpSepDesc->TakeYfromG && (c > 0))
						y -= lpSepDesc->TakeYfromGLut[c];
					if (lpSepDesc->TakeMfromR && (y > 0))
						m -= lpSepDesc->TakeMfromRLut[y];
					if (lpSepDesc->TakeCfromB && (m > 0))
						c -= lpSepDesc->TakeCfromBLut[m];
				}
			}

		}
		if (gray && lpSepDesc->DoInkClipping)
		{
			fClip = TRUE;
			if (c > maxC)
			{
				oldc = c;
				c = maxC;
			}
			else
				fClip = FALSE;
			if (m > maxM)
			{
				oldm = m;
				m = maxM;
			}
			else
				fClip = FALSE;
			if (y > maxY)
			{
				oldy = y;
				y = maxY;
			}
			else
				fClip = FALSE;
			// clipped all three channels, see if we
			// have room to add back some gray
			if (fClip)
			{
				// get amount clipped for each channel
				oldc -= c;
				oldm -= m;
				oldy -= y;
				// get K bump by getting min delta to add gray back in
				bump = min( oldc, min( oldm, oldy ) );
				// get curve limit, it is either the old K value
				// or the clip limit (which is a line from where
				// clipping began to the max black)
				limit = max(lpSepDesc->ClipBlack.Lut[gray], k);
				// bump k and check against limit
				k += bump;
				if (k > limit)
					k = limit;
			}
		}
		if (c < 0)
			c = 0;
		if (m < 0)
			m = 0;
		if (y < 0)
			y = 0;
		if (lpCLut)
		{
			c = lpCLut[c];
			m = lpMLut[m];
			y = lpYLut[y];
			k = lpKLut[k];
		}

		lpCMYK->c = c;
		lpCMYK->m = m;
		lpCMYK->y = y;
		lpCMYK->k = k;

		lpCMYK++;
	}
	
}


/***********************************************************************/

LOCAL void OldClrFrameCMYKtoRGB(LPCMYK lpCMYK, LPRGB lpRGB, int iCount)
{
	register int c, m, y, k;
	int gcr, newk, gray;
	LPTR lpCLut, lpMLut, lpYLut, lpKLut, lpSatLut;
	HSLS hsl;

	if (!lpSepDesc)
		return;

	lpCLut = lpSatLut = NULL;
	if (lpSepDesc->DoCalOnConvert)
	{
		lpCLut = lpSepDesc->CMYKInverseLuts[0];
		lpMLut = lpSepDesc->CMYKInverseLuts[1];
		lpYLut = lpSepDesc->CMYKInverseLuts[2];
		lpKLut = lpSepDesc->CMYKInverseLuts[3];
	}
	if (lpSepDesc->DoSatBoost)
		lpSatLut = lpSepDesc->SatInverseLut;
	while ( --iCount >= 0 )
	{
		// RGB to CMY conversion
		if (lpCLut)
		{
			c = lpCLut[lpCMYK->c];
			m = lpMLut[lpCMYK->m];
			y = lpYLut[lpCMYK->y];
			k = lpKLut[lpCMYK->k];
		}
		else
		{
			c = lpCMYK->c;
			m = lpCMYK->m;
			y = lpCMYK->y;
			k = lpCMYK->k;
		}

		lpCMYK++;

		if ( lpSepDesc->DoInkCorrect)
		{ // Ink Correction
			if (lpSepDesc->InkMode == 0)
			{
				if (lpSepDesc->TakeYfromR && (m > 0))
					y += ((lpSepDesc->TakeYfromR * m) + 50) / 100;
				if (lpSepDesc->TakeYfromG && (c > 0))
					y += ((lpSepDesc->TakeYfromG * c) + 50) / 100;
				if (lpSepDesc->TakeMfromB && (c > 0))
					m += ((lpSepDesc->TakeMfromB * c) + 50) / 100;
			}
			else
			if (lpSepDesc->InkMode == 1)
			{
				if (lpSepDesc->TakeCfromG && (y > 0))
					c += ((lpSepDesc->TakeCfromG * y) + 50) / 100;
				if (lpSepDesc->TakeCfromB && (m > 0))
					c += ((lpSepDesc->TakeCfromB * m) + 50) / 100;
				if (lpSepDesc->TakeMfromR && (y > 0))
					m += ((lpSepDesc->TakeMfromR * y) + 50) / 100;
			}
			else
			if (lpSepDesc->InkMode == 2)
			{
				if (lpSepDesc->TakeMfromR && (y > 0))
					m += ((lpSepDesc->TakeMfromR * y) + 50) / 100;
				if (lpSepDesc->TakeYfromG && (c > 0))
					y += ((lpSepDesc->TakeYfromG * c) + 50) / 100;
				if (lpSepDesc->TakeMfromB && (c > 0))
					m += ((lpSepDesc->TakeMfromB * c) + 50) / 100;
			}
			else
			if (lpSepDesc->InkMode == 3)
			{
				if (lpSepDesc->TakeMfromB == lpSepDesc->TakeMax)
				{
					if (lpSepDesc->TakeCfromG && (y > 0))
						c += ((lpSepDesc->TakeCfromG * y) + 50) / 100;
					if (lpSepDesc->TakeYfromR && (m > 0))
						y += ((lpSepDesc->TakeYfromR * m) + 50) / 100;
					if (lpSepDesc->TakeMfromB && (c > 0))
						m += ((lpSepDesc->TakeMfromB * c) + 50) / 100;
				}
				else
				if (lpSepDesc->TakeYfromR == lpSepDesc->TakeMax)
				{
					if (lpSepDesc->TakeCfromG && (y > 0))
						c += ((lpSepDesc->TakeCfromG * y) + 50) / 100;
					if (lpSepDesc->TakeMfromB && (c > 0))
						m += ((lpSepDesc->TakeMfromB * c) + 50) / 100;
					if (lpSepDesc->TakeYfromR && (m > 0))
						y += ((lpSepDesc->TakeYfromR * m) + 50) / 100;
				}
				else
				//if (lpSepDesc->TakeCfromG == lpSepDesc->TakeMax)
				{
					if (lpSepDesc->TakeYfromR && (m > 0))
						y += ((lpSepDesc->TakeYfromR * m) + 50) / 100;
					if (lpSepDesc->TakeMfromB && (c > 0))
						m += ((lpSepDesc->TakeMfromB * c) + 50) / 100;
					if (lpSepDesc->TakeCfromG && (y > 0))
						c += ((lpSepDesc->TakeCfromG * y) + 50) / 100;
				}
			}
			else
			if (lpSepDesc->InkMode == 4)
			{
				if (lpSepDesc->TakeYfromR && (m > 0))
					y += ((lpSepDesc->TakeYfromR * m) + 50) / 100;
				if (lpSepDesc->TakeYfromG && (c > 0))
					y += ((lpSepDesc->TakeYfromG * c) + 50) / 100;
				if (lpSepDesc->TakeCfromB && (m > 0))
					c += ((lpSepDesc->TakeCfromB * m) + 50) / 100;
			}
			else
			if (lpSepDesc->InkMode == 5)
			{
				if (lpSepDesc->TakeMfromR && (y > 0))
					m += ((lpSepDesc->TakeMfromR * y) + 50) / 100;
				if (lpSepDesc->TakeMfromB && (c > 0))
					m += ((lpSepDesc->TakeMfromB * c) + 50) / 100;
				if (lpSepDesc->TakeCfromG && (y > 0))
					c += ((lpSepDesc->TakeCfromG * y) + 50) / 100;
			}
			else
			if (lpSepDesc->InkMode == 6)
			{
				if (lpSepDesc->TakeCfromG && (y > 0))
					c += ((lpSepDesc->TakeCfromG * y) + 50) / 100;
				if (lpSepDesc->TakeYfromR && (m > 0))
					y += ((lpSepDesc->TakeYfromR * m) + 50) / 100;
				if (lpSepDesc->TakeCfromB && (m > 0))
					c += ((lpSepDesc->TakeCfromB * m) + 50) / 100;
			}
			else
			//if (lpSepDesc->InkMode == 7)
			{
				if (lpSepDesc->TakeCfromB == lpSepDesc->TakeMax)
				{
					if (lpSepDesc->TakeYfromG && (c > 0))
						y += ((lpSepDesc->TakeYfromG * c) + 50) / 100;
					if (lpSepDesc->TakeMfromR && (y > 0))
						m += ((lpSepDesc->TakeMfromR * y) + 50) / 100;
					if (lpSepDesc->TakeCfromB && (m > 0))
						c += ((lpSepDesc->TakeCfromB * m) + 50) / 100;
				}
				else
				if (lpSepDesc->TakeMfromR == lpSepDesc->TakeMax)
				{
					if (lpSepDesc->TakeYfromG && (c > 0))
						y += ((lpSepDesc->TakeYfromG * c) + 50) / 100;
					if (lpSepDesc->TakeCfromB && (m > 0))
						c += ((lpSepDesc->TakeCfromB * m) + 50) / 100;
					if (lpSepDesc->TakeMfromR && (y > 0))
						m += ((lpSepDesc->TakeMfromR * y) + 50) / 100;
				}
				else
				//if (lpSepDesc->TakeYfromG == lpSepDesc->TakeMax)
				{
					if (lpSepDesc->TakeCfromB && (m > 0))
						c += ((lpSepDesc->TakeCfromB * m) + 50) / 100;
					if (lpSepDesc->TakeMfromR && (y > 0))
						m += ((lpSepDesc->TakeMfromR * y) + 50) / 100;
					if (lpSepDesc->TakeYfromG && (c > 0))
						y += ((lpSepDesc->TakeYfromG * c) + 50) / 100;
				}
			}

		}

		if ( k &&
			lpSepDesc->DoBlackAndUCR )
		{ 
			// Under Color removal
			if ( gcr = lpSepDesc->UCRlut[ k ] )
			{
				c += gcr; 
				m += gcr;
				y += gcr;
			}
			// adjust for k not from UCR
			gray = min( c, min( m, y ) );
			newk = lpSepDesc->BGLimitedMap.Lut[gray];
			k -= newk;
			k = max(k,0);
		}
		
		// add extra k back in
		c += k;
		m += k;
		y += k;
		
		if (c > 255)
			c = 255;
		if (m > 255)
			m = 255;
		if (y > 255)
			y = 255;
		lpRGB->red = 255 - c;
		lpRGB->green = 255 - m;
		lpRGB->blue = 255 - y;
		if (lpSatLut)
		{
			RGBtoHSL(lpRGB->red, lpRGB->green, lpRGB->blue, &hsl);
			hsl.sat = lpSatLut[hsl.sat];
			HSLtoRGB(hsl.hue, hsl.sat, hsl.lum, lpRGB);
		}

		lpRGB++;
	}

}

/***********************************************************************/

LOCAL void ClrFrameCMYKtoRGB(LPCMYK lpCMYK, LPRGB lpRGB, int iCount)
{
	register int c, m, y, k;
	int gcr, newk, gray;
	LPTR lpCLut, lpMLut, lpYLut, lpKLut, lpSatLut;
	LPINT16 TakeMfromRLut, TakeYfromRLut, TakeYfromGLut, TakeCfromGLut, TakeCfromBLut, TakeMfromBLut;

	if (!lpSepDesc)
		return;

	// **FIX
//	if (CONTROL)
//		bOldWay = FALSE;
//	if (SHIFT || bOldWay)
//	{
//		bOldWay = TRUE;
//		OldClrFrameCMYKtoRGB(lpCMYK, lpRGB, iCount);
//		return;
//	}

	lpCLut = lpSatLut = NULL;
	if (lpSepDesc->DoCalOnConvert)
	{
		lpCLut = lpSepDesc->CMYKInverseLuts[0];
		lpMLut = lpSepDesc->CMYKInverseLuts[1];
		lpYLut = lpSepDesc->CMYKInverseLuts[2];
		lpKLut = lpSepDesc->CMYKInverseLuts[3];
	}
	if (lpSepDesc->DoSatBoost)
		lpSatLut = lpSepDesc->SatInverseLut;
	if ( lpSepDesc->DoInkCorrect)
	{
		TakeMfromRLut = lpSepDesc->TakeMfromRLut;
		TakeYfromRLut = lpSepDesc->TakeYfromRLut;
		TakeYfromGLut = lpSepDesc->TakeYfromGLut;
		TakeCfromGLut = lpSepDesc->TakeCfromGLut;
		TakeCfromBLut = lpSepDesc->TakeCfromBLut;
		TakeMfromBLut = lpSepDesc->TakeMfromBLut;
	}
	
	while ( --iCount >= 0 )
	{
		// RGB to CMY conversion
		if (lpCLut)
		{
			c = lpCLut[lpCMYK->c];
			m = lpMLut[lpCMYK->m];
			y = lpYLut[lpCMYK->y];
			k = lpKLut[lpCMYK->k];
		}
		else
		{
			c = lpCMYK->c;
			m = lpCMYK->m;
			y = lpCMYK->y;
			k = lpCMYK->k;
		}

		lpCMYK++;

		if ( lpSepDesc->DoInkCorrect)
		{ // Ink Correction
			if (lpSepDesc->InkMode == 0)
			{	 
				if (lpSepDesc->TakeYfromR && (m > 0))
					y += TakeYfromRLut[m];
				if (lpSepDesc->TakeYfromG && (c > 0))
					y += TakeYfromGLut[c];
				if (lpSepDesc->TakeMfromB && (c > 0))
					m += TakeMfromBLut[c];
			}
			else
			if (lpSepDesc->InkMode == 1)
			{
				if (lpSepDesc->TakeCfromG && (y > 0))
					c += TakeCfromGLut[y];
				if (lpSepDesc->TakeCfromB && (m > 0))
					c += TakeCfromBLut[m];
				if (lpSepDesc->TakeMfromR && (y > 0))
					m += TakeMfromRLut[y];
			}
			else
			if (lpSepDesc->InkMode == 2)
			{
				if (lpSepDesc->TakeMfromR && (y > 0))
					m += TakeMfromRLut[y];
				if (lpSepDesc->TakeYfromG && (c > 0))
					y += TakeYfromGLut[c];
				if (lpSepDesc->TakeMfromB && (c > 0))
					m += TakeMfromBLut[c];
			}
			else
			if (lpSepDesc->InkMode == 3)
			{
				if (lpSepDesc->TakeMfromB == lpSepDesc->TakeMax)
				{
					if (lpSepDesc->TakeCfromG && (y > 0))
						c += TakeCfromGLut[y];
					if (lpSepDesc->TakeYfromR && (m > 0))
						y += TakeYfromRLut[m];
					if (lpSepDesc->TakeMfromB && (c > 0))
						m += TakeMfromBLut[c];
				}
				else
				if (lpSepDesc->TakeYfromR == lpSepDesc->TakeMax)
				{
					if (lpSepDesc->TakeCfromG && (y > 0))
						c += TakeCfromGLut[y];
					if (lpSepDesc->TakeMfromB && (c > 0))
						m += TakeMfromBLut[c];
					if (lpSepDesc->TakeYfromR && (m > 0))
						y += TakeYfromRLut[m];
				}
				else
				//if (lpSepDesc->TakeCfromG == lpSepDesc->TakeMax)
				{
					if (lpSepDesc->TakeYfromR && (m > 0))
						y += TakeYfromRLut[m];
					if (lpSepDesc->TakeMfromB && (c > 0))
						m += TakeMfromBLut[c];
					if (lpSepDesc->TakeCfromG && (y > 0))
						c += TakeCfromGLut[y];
				}
			}
			else
			if (lpSepDesc->InkMode == 4)
			{
				if (lpSepDesc->TakeYfromR && (m > 0))
					y += TakeYfromRLut[m];
				if (lpSepDesc->TakeYfromG && (c > 0))
					y += TakeYfromGLut[c];
				if (lpSepDesc->TakeCfromB && (m > 0))
					c += TakeCfromBLut[m];
			}
			else
			if (lpSepDesc->InkMode == 5)
			{
				if (lpSepDesc->TakeMfromR && (y > 0))
					m += TakeMfromRLut[y];
				if (lpSepDesc->TakeMfromB && (c > 0))
					m += TakeMfromBLut[c];
				if (lpSepDesc->TakeCfromG && (y > 0))
					c += TakeCfromGLut[y];
			}
			else
			if (lpSepDesc->InkMode == 6)
			{
				if (lpSepDesc->TakeCfromG && (y > 0))
					c += TakeCfromGLut[y];
				if (lpSepDesc->TakeYfromR && (m > 0))
					y += TakeYfromRLut[m];
				if (lpSepDesc->TakeCfromB && (m > 0))
					c += TakeCfromBLut[m];
			}
			else
			//if (lpSepDesc->InkMode == 7)
			{
				if (lpSepDesc->TakeCfromB == lpSepDesc->TakeMax)
				{
					if (lpSepDesc->TakeYfromG && (c > 0))
						y += TakeYfromGLut[c];
					if (lpSepDesc->TakeMfromR && (y > 0))
						m += TakeMfromRLut[y];
					if (lpSepDesc->TakeCfromB && (m > 0))
						c += TakeCfromBLut[m];
				}
				else
				if (lpSepDesc->TakeMfromR == lpSepDesc->TakeMax)
				{
					if (lpSepDesc->TakeYfromG && (c > 0))
						y += TakeYfromGLut[c];
					if (lpSepDesc->TakeCfromB && (m > 0))
						c += TakeCfromBLut[m];
					if (lpSepDesc->TakeMfromR && (y > 0))
						m += TakeMfromRLut[y];
				}
				else
				//if (lpSepDesc->TakeYfromG == lpSepDesc->TakeMax)
				{
					if (lpSepDesc->TakeCfromB && (m > 0))
						c += TakeCfromBLut[m];
					if (lpSepDesc->TakeMfromR && (y > 0))
						m += TakeMfromRLut[y];
					if (lpSepDesc->TakeYfromG && (c > 0))
						y += TakeYfromGLut[c];
				}
			}

		}

		if ( k && lpSepDesc->DoBlackAndUCR )
		{ 
			// Under Color removal
			if ( gcr = lpSepDesc->UCRlut[ k ] )
			{
				c += gcr; 
				m += gcr;
				y += gcr;
			}
			// adjust for k not from UCR
			gray = min( c, min( m, y ) );
			newk = lpSepDesc->BGLimitedMap.Lut[gray];
			k -= newk;
			k = max(k,0);
		}
		
		// add extra k back in
		if (k)
		{
			c += k;
			m += k;
			y += k;
		}
		
		if (c > 255)
			c = 255;
		if (m > 255)
			m = 255;
		if (y > 255)
			y = 255;
		lpRGB->red = 255 - c;
		lpRGB->green = 255 - m;
		lpRGB->blue = 255 - y;
		if (lpSatLut)
		{
			MapSat(lpRGB, lpSatLut);
		}

		lpRGB++;
	}

}

// Used for GDI printing
/***********************************************************************/
VOID CorrectRGB( LPRGB lpRGB, int iCount )
/***********************************************************************/
{
	CMYKS cmy;
	BOOL DoCalOnConvert, DoBlackAndUCR; 

	if (!lpSepDesc)
		return;

	if ( !lpSepDesc->DoInkCorrect && !lpSepDesc->DoCalibration )
		return;

	// turn on the cal on convert flag and off the black and ucr flag
	DoCalOnConvert = lpSepDesc->DoCalOnConvert;
	DoBlackAndUCR = lpSepDesc->DoBlackAndUCR;
	lpSepDesc->DoCalOnConvert = TRUE;
	lpSepDesc->DoBlackAndUCR = FALSE;
	while ( --iCount >= 0 )
		{
		// RGB to CMY conversion
		ClrFrameRGBtoCMYK(lpRGB, &cmy, 1);
		lpRGB->red   = 255 - cmy.c;
		lpRGB->green = 255 - cmy.m;
		lpRGB->blue  = 255 - cmy.y;
		lpRGB++;
		}
	// restore the flags
	lpSepDesc->DoCalOnConvert = DoCalOnConvert;
	lpSepDesc->DoBlackAndUCR = DoBlackAndUCR;
}

/***********************************************************************/
VOID CorrectRGB( LPTR lpRed, LPTR lpGreen, LPTR lpBlue, int iCount )
/***********************************************************************/
{
	CMYKS cmy;
	RGBS rgb;
	BOOL DoCalOnConvert, DoBlackAndUCR; 

	if (!lpSepDesc)
		return;

	if ( !lpSepDesc->DoInkCorrect && !lpSepDesc->DoCalibration )
		return;

	// turn on the cal on convert flag and off the black and ucr flag
	DoCalOnConvert = lpSepDesc->DoCalOnConvert;
	DoBlackAndUCR = lpSepDesc->DoBlackAndUCR;
	lpSepDesc->DoCalOnConvert = TRUE;
	lpSepDesc->DoBlackAndUCR = FALSE;
	while ( --iCount >= 0 )
		{
		// RGB to CMY conversion
		rgb.red = *lpRed;
		rgb.green = *lpGreen;
		rgb.blue = *lpBlue;
		ClrFrameRGBtoCMYK(&rgb, &cmy, 1);
		*lpRed++   = 255 - cmy.c;
		*lpGreen++ = 255 - cmy.m;
		*lpBlue++  = 255 - cmy.y;
		}
	// restore the flags
	lpSepDesc->DoCalOnConvert = DoCalOnConvert;
	lpSepDesc->DoBlackAndUCR = DoBlackAndUCR;
}

// Used for both Postscript and GDI printing
/***********************************************************************/
VOID CorrectGray( LPTR lpGray, int iCount )
/***********************************************************************/
{
	LPTR lpLut;

	if ( !lpSepDesc || !lpSepDesc->DoCalibration )
		return;

	lpLut = lpSepDesc->GrayLut;
	while ( --iCount >= 0 )
	{
		*lpGray++ = lpLut[*lpGray];
	}
}

/***********************************************************************/
void CalibrateCMYK(LPTR lpC, LPTR lpM, LPTR lpY, LPTR lpK, int iCount)
/***********************************************************************/
{
	LPTR lpCLut, lpMLut, lpYLut, lpKLut;

	if ( !lpSepDesc || !lpSepDesc->DoCalibration )
		return;

	lpCLut = lpSepDesc->CMYKLuts[0];
	lpMLut = lpSepDesc->CMYKLuts[1];
	lpYLut = lpSepDesc->CMYKLuts[2];
	lpKLut = lpSepDesc->CMYKLuts[3];
	while ( --iCount >= 0 )
	{
		*lpC++ = lpCLut[*lpC];
		*lpM++ = lpMLut[*lpM];
		*lpY++ = lpYLut[*lpY];
		*lpK++ = lpKLut[*lpK];
	}	
}

/***********************************************************************/
void CalibrateCMYKChannel(LPTR lp, int iPlaneOffset, int iCount)
/***********************************************************************/
{
	LPTR lpLut;

	if ( !lpSepDesc || !lpSepDesc->DoCalibration )
		return;

	lpLut = lpSepDesc->CMYKLuts[iPlaneOffset];
	while ( --iCount >= 0 )
	{
		*lp++ = lpLut[*lp];
	}	
}

// Used for both GDI printing and DCS
// for ClrRGBtoCMYK & ClrCMYKtoRGB use InitPrintStyle
/***********************************************************************/
VOID ClrRGBtoCMYK( LPRGB lpRGB, LPTR lpCyan, LPTR lpMagenta, LPTR lpYellow,
                     LPTR lpBlack, int iCount)
/***********************************************************************/
{
CMYKS cmyk;

while ( --iCount >= 0 )
	{
	// RGB to CMY conversion
	ClrFrameRGBtoCMYK(lpRGB, &cmyk, 1);
	lpRGB++;

	*lpCyan++    = cmyk.c;
	*lpMagenta++ = cmyk.m;
	*lpYellow++  = cmyk.y;
	*lpBlack++   = cmyk.k;
	}
}

// Used for both GDI printing and DCS
// for ClrGraytoCMYK use InitPrintStyle
/***********************************************************************/
VOID ClrGraytoCMYK( LPTR lpGray, LPTR lpCyan, LPTR lpMagenta, LPTR lpYellow,
                     LPTR lpBlack, int iCount)
/***********************************************************************/
{
CMYKS cmyk;
RGBS rgb;

while ( --iCount >= 0 )
	{
	// setup gray as rgb
	rgb.red = rgb.green = rgb.blue = *lpGray++;
	// RGB to CMY conversion
	ClrFrameRGBtoCMYK(&rgb, &cmyk, 1);

	*lpCyan++    = cmyk.c;
	*lpMagenta++ = cmyk.m;
	*lpYellow++  = cmyk.y;
	*lpBlack++   = cmyk.k;
	}
}



#ifdef UNUSED
// ClrCMYKtoRGB use InitPrintStyle
/***********************************************************************/
VOID ClrCMYKtoRGB( LPTR lpCyan, LPTR lpMagenta, LPTR lpYellow, LPTR lpBlack,
                     LPRGB lpRGB, int iCount )
/***********************************************************************/
{
CMYKS cmyk;

while (--iCount >= 0)
	{
	cmyk.c = *lpCyan++;
	cmyk.m = *lpMagenta++;
	cmyk.y = *lpYellow++;
	cmyk.k = *lpBlack++;

	// RGB to CMY conversion
	ClrFrameCMYKtoRGB(&cmyk, lpRGB, 1);

	lpRGB++;
	}
}
#endif


/***********************************************************************/
LOCAL void make_TakeFromLut(LPINT16 TakefromLut, int TakefromVal)
/***********************************************************************/
{
	int val = TakefromVal;
	int sum;
	int i;
	
	sum = -val;
	for (i = 0; i<512;i++)
		*TakefromLut++ =  (INT16)(((sum+=val) + 50) / 100);
}

#define RGBMAX 255
#define HSLMAX 255
#define MAX_HUES 252 /* must be divisible by 6 */
#define SMAX 255
#define LMAX 255
#define DEGREE60  ((1*MAX_HUES)/6)
#define DEGREE120 ((2*MAX_HUES)/6)
#define DEGREE180 ((3*MAX_HUES)/6)
#define DEGREE240 ((4*MAX_HUES)/6)
#define DEGREE300 ((5*MAX_HUES)/6)
#define DEGREE360 ((6*MAX_HUES)/6)
#define UNDEFINED 0

/***********************************************************************/
LOCAL void MapSat(LPRGB lpRGB, LPTR lpSatMap)
/***********************************************************************/
{
	BYTE cMax,cMin;      /* max and min RGB values */
	WORD sum, dif;
	BYTE L,S, Sold;
	BYTE R,G,B; /* RGB component values */
	int  n, n1, n2; /* calculated magic numbers (really!) */

	R = lpRGB->red;
	G = lpRGB->green;
	B = lpRGB->blue;

	/* calculate lightness */
	cMax = max( max( R, G ), B );
	cMin = min( min( R, G ), B );
	sum = cMax + cMin;
	dif = cMax - cMin;
	L = sum >> 1; // Actually L = ((sum/2) * HSLMAX) / RGBMAX;

	if (!dif )
	{ 
		/* r=g=b --> starting as achromatic .. do nothing*/
		return;
	}
	
	/* chromatic case */
	/* saturation */
	if ( sum > RGBMAX )
		sum = 2*RGBMAX - sum;
	Sold = ( dif * HSLMAX ) / sum; // Actually, S = 1 - (cMin/L)

	S = lpSatMap[Sold];
	if (S == Sold)
		return;
	if (!S)
	{
		/* r=g=b --> go to achromatic*/
		lpRGB->red   = L;
		lpRGB->green = L;
		lpRGB->blue  = L;
		return;
	}
	
	/* set up magic numbers */
	n1 = n2 = L;
	if ( L > LMAX/2 )
		L = 255 - L; // make L no bigger than 50%
	n = ((WORD)L * S) / SMAX; // n is never bigger that L (or 50%)
	n1 -= n; if ( n1 == 1 ) n1--;
	n2 += n; if ( n2 == 254 ) n2++;

	/* hue */
	if (R == cMax)  
	{
		if (B>G)
		{
			/* 300-359 degrees */
			B = n1 + (((long)(n2-n1)*((int)B - G))/(int)dif);
			G = n1;
		}
		else
		{
			/* 0-59 degrees */
			G = n1 + (((long)(n2-n1)*((int)G - B))/(int)dif);
			B = n1;
		}
		R = n2;
	}
	else if (G == cMax)
	{
		if (R>B)
		{
			/* 60-119 degrees */
			R = n1 + (((long)(n2-n1)*((int)R - B))/(int)dif);
			B = n1;
		}
		else
		{
			/* 120-179 degrees */
			B = n1 + (((long)(n2-n1)*((int)B - R))/(int)dif);
			R = n1;
		}
		G = n2;
	}
	else // if (B == cMax)
	{
		if (G>R)
		{
			/* 180-239 degrees */
			G = n1 + (((long)(n2-n1)*((int)G - R))/(int)dif);
			R = n1;
		}
		else
		{
			/* 240-299 degrees */
			R = n1 + (((long)(n2-n1)*((int)R - G))/(int)dif);
			G = n1;
		}
		B = n2;
	}

	// Actually, R = (RGBMAX * R) / HSLMAX, etc.
	lpRGB->red   = R;
	lpRGB->green = G;
	lpRGB->blue  = B;
}

