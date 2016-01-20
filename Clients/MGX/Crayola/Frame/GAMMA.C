// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include <math.h>
#include "superlib.h"
#include "macros.h"

/************************************************************************/

void BuildGammaLut( LFIXED Gamma, LPTR GammaLut )
{
	MAP GammaMap;

	CreateGammaMap( &GammaMap, Gamma );

	copy( GammaMap.Lut, GammaLut, LUTSIZE );
}

/************************************************************************/

void CreateGammaMap( LPMAP lpMap, LFIXED fGamma )
{
	double dGamma;
	int i, j;
	double pow(double, double);

	ResetMap( lpMap, MAPPOINTS, NO );

	if (fGamma != 0)
		dGamma = 1.0 / DOUBLE(fGamma);
	else
		dGamma = 1.0;

	for (i = 0; i < LUTSIZE; i++)
	{
		j = (int)(
				pow((double)(i)/(double)(LUTSIZE),dGamma)*
				(double)(LUTSIZE)
				+0.5);

		if (j > 255) j = 255;
		if (j <   0) j = 0;

		lpMap->Lut[i] = j;
	}
}

/************************************************************************/

