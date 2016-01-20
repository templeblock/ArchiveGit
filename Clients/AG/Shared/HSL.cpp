#include <stdafx.h>
#include "HSL.h"

//////////////////////////////////////////////////////////////////////
BYTE RGBtoL(RGBS* pRGB)
{
	// Calculate lightness; Actually, L = ((cMax+cMin)/2 * HSLMAX) / RGBMAX;
	BYTE r = pRGB->red;
	BYTE g = pRGB->green;
	BYTE b = pRGB->blue;
	return TOLUM(r, g, b);
}

//////////////////////////////////////////////////////////////////////
void RGBtoHSL(BYTE R, BYTE G, BYTE B, HSL* pHSL)
{
	// calculate lightness
	BYTE cMax = max(max(R, G), B);
	BYTE cMin = min(min(R, G), B);
	WORD sum = cMax + cMin;
	WORD dif = cMax - cMin;
	int H = 0;
	BYTE S = 0;
	BYTE L = sum >> 1; // Actually L = ((sum/2) * HSLMAX) / RGBMAX;

	if (dif)
	{ // chromatic case
		// saturation
		if (sum > RGBMAX)
			sum = 2*RGBMAX - sum;
		S = (dif * HSLMAX) / sum; // Actually, S = 1 - (cMin/L)

		// hue
		if (R == cMax)	H = ((DEGREE60 * (G - B)) / (int)dif);
		else
		if (G == cMax)	H = ((DEGREE60 * (B - R)) / (int)dif) + DEGREE120;
		else
		if (B == cMax)	H = ((DEGREE60 * (R - G)) / (int)dif) + DEGREE240;

		if (H < 0)		H += DEGREE360;
		if (H > DEGREE360)	H -= DEGREE360;
	}
	else
	{ // r=g=b --> achromatic case
		S = 0;			// saturation
		H = UNDEFINED;	// hue
	}

	pHSL->hue = H;
	pHSL->sat = S;
	pHSL->lum = L;
}

//////////////////////////////////////////////////////////////////////
void HSLtoRGB(BYTE hue, BYTE sat, BYTE lum, RGBS* pRGB)
{
	if (!sat)
	{ // achromatic case
		pRGB->red = pRGB->green = pRGB->blue = lum; // Actually = (RGBMAX * lum) / LMAX
		return;
	}

	// chromatic case
	// range check: note values passed add/subtract thirds of range
	if (hue > DEGREE360)
		hue -= DEGREE360;

	// set up magic numbers
	int n = ((WORD)lum * sat) / SMAX;
	int n1, n2;	// calculated magic numbers (really!)
	if (lum <= LMAX/2)
	{
		n1 = (int)lum - n;
		n2 = (int)lum + n;
	}
	else
	{
		n1 = (int)lum + n - sat;
		n2 = (int)lum - n + sat;
	}

	BYTE R,G,B;	// RGB component values
	switch (hue/DEGREE60)
	{
		case 0: // 0-59 degrees
		R = n2;
		G = n1 + (((n2-n1)*hue)/DEGREE60);
		B = n1;
		break;

		case 1: // 60-119 degrees
		R = n1 + (((n2-n1)*(DEGREE120-hue))/DEGREE60);
		G = n2;
		B = n1;
		break;

		case 2: // 120-179 degrees
		R = n1;
		G = n2;
		B = n1 + (((n2-n1)*(hue-DEGREE120))/DEGREE60);
		break;

		case 3: // 180-239 degrees
		R = n1;
		G = n1 + (((n2-n1)*(DEGREE240-hue))/DEGREE60);
		B = n2;
		break;

		case 4: // 240-299 degrees
		R = n1 + (((n2-n1)*(hue-DEGREE240))/DEGREE60);
		G = n1;
		B = n2;
		break;

		default: // 300-359 degrees
		R = n2;
		G = n1;
		B = n1 + (((n2-n1)*(DEGREE360-hue))/DEGREE60);
		break;
	}

	// Actually, R = (RGBMAX * R) / HSLMAX, etc.
	pRGB->red	= R;
	pRGB->green	= G;
	pRGB->blue	= B;
}
