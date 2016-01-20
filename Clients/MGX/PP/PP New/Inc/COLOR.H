// (c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// for color.c

#ifndef _COLOR_H_
#define _COLOR_H_

// Source file: color.c
BOOL InitPrintStyle(LPSEPARATE lpSep, LPCALIBRATE lpCal);

LPFRAME ConvertLongColor( LPFRAME lpFrame );
void ConvertLongColor24( LPFRAME lpFrame, LPFRAME lpNewFrame );
void SetEachThirdValue( LPTR lpIn, LPTR lpOut, int iCount );
void GetEachThirdValue( LPTR lpIn, LPTR lpOut, int iCount );
LPFRAME ConvertWideColor( LPFRAME lpFrame );
void UnshuffleRGB( LPRGB lpRGB, LPTR lpRed, LPTR lpGreen, LPTR lpBlue, int iCount );
VOID CorrectRGB( LPRGB lpRGB, int iCount );
VOID CorrectRGB( LPTR lpRed, LPTR lpGreen, LPTR lpBlue, int iCount );
VOID CorrectGray( LPTR lpGray, int iCount );
BOOL CreateCMYKCalLut(LPCALIBRATE lpCalibrate, int nChannel, LPTR lpLut,	LPTR lpInverseLut);

VOID ClrRGBtoCMYK( LPRGB lpRGB, LPTR lpCyan, LPTR lpMagenta, LPTR lpYellow, LPTR lpBlack, int iCount );
VOID ClrGraytoCMYK( LPTR lpGray, LPTR lpCyan, LPTR lpMagenta, LPTR lpYellow, LPTR lpBlack, int iCount );
BOOL DoCalibrate(LPCALIBRATE lpCalibrate);
void CalibrateCMYK(LPTR lpC, LPTR lpM, LPTR lpY, LPTR lpK, int iCount);
void CalibrateCMYKChannel(LPTR lp, int iPlaneOffset, int iCount);
#endif
