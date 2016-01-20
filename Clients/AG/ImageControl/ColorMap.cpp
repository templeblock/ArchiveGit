#include "StdAfx.h"
#include "ColorMap.h"
#include "HSL.h"
#include "Utility.h"
#include "MessageBox.h"
#include "Dib.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////
CColorMap::CColorMap()
{
	::ZeroMemory(m_R, sizeof(m_R));
	::ZeroMemory(m_G, sizeof(m_R));
	::ZeroMemory(m_B, sizeof(m_R));
	m_bUsingHSL = false;
}

//////////////////////////////////////////////////////////////////////
CColorMap::~CColorMap()
{
}

//////////////////////////////////////////////////////////////////////
void CColorMap::MakePoster(int nLevels)
{
	m_bUsingHSL = false;

	double fBreakIncr = 256.0 / nLevels;
	double fValueIncr = 256.0 / (nLevels-1);
	double fNewValue = 0;
	double fNextBreak = fBreakIncr;

	for (int i=0; i<256; i++)
	{
		if (i >= dtoi(fNextBreak))
		{
			fNewValue += fValueIncr;
			fNextBreak += fBreakIncr;
		}

		int iNewValue = bound(dtoi(fNewValue), 0, 255);
		m_R[i] = iNewValue;
		m_G[i] = iNewValue;
		m_B[i] = iNewValue;
	}
}

//////////////////////////////////////////////////////////////////////
void CColorMap::MakeStretch(int iMinIndex, int iMaxIndex)
{
	m_bUsingHSL = false;

	int iDynamicRange = iMaxIndex - iMinIndex;
	for (int i=0; i<256; i++)
	{
		int iNewValue = 0;
		if (i <= iMinIndex)
			iNewValue = 0;
		else
		if (i >= iMaxIndex)
			iNewValue = 255;
		else
			iNewValue = (255 * (i - iMinIndex)) / iDynamicRange;

		iNewValue = bound(iNewValue, 0, 255);
		m_R[i] = iNewValue;
		m_G[i] = iNewValue;
		m_B[i] = iNewValue;
	}
}

//////////////////////////////////////////////////////////////////////
void CColorMap::MakeContrastBrightness(int iContrast, int iBrightness)
{
	m_bUsingHSL = false;

	iContrast = (iContrast * 5) - 250; // Convert 0 thru 100 to -250 thru +250
	iBrightness = (iBrightness * 5) - 250; // Convert 0 thru 100 to -250 thru +250

	double fAngle = (3.14/4) * (1 - (double)iContrast/250); // Convert -250 thru +250 to pi/2 to 0
	double fTanAngle = tan(fAngle);
	if (fTanAngle == 0.0)
		fTanAngle = 0.001;

	for (int i=0; i<256; i++)
	{
		int iNewValue = (int)((i - 128) / fTanAngle) + 128;
		iNewValue += iBrightness;

		iNewValue = bound(iNewValue, 0, 255);
		m_R[i] = iNewValue;
		m_G[i] = iNewValue;
		m_B[i] = iNewValue;
	}
}

//////////////////////////////////////////////////////////////////////
void CColorMap::MakeHueSaturation(int iHueAmount, int iHue, int iSaturation)
{
	m_bUsingHSL = true;

	iHue = (iHue * 5) / 2; // Convert 0 thru 100 to 0 thru +250
	iSaturation = (iSaturation * 5) - 250; // Convert 0 thru 100 to -250 thru +250

	for (int i=0; i<256; i++)
	{
		 // Compute the shortest distance to the target hue
		int iHueDistance = iHue - i;
		if (iHueDistance <= -128)
			iHueDistance += 255;
		else
		if (iHueDistance >=  128)
			iHueDistance -= 255;

		int iNewHue = i + ((iHueDistance * iHueAmount) / 100);
		int iLimit = HMAX;
		if (iNewHue < 0)		iNewHue += iLimit;
		if (iNewHue > iLimit)	iNewHue -= iLimit;

		int iNewSaturation = i + iSaturation;
		iNewSaturation = bound(iNewSaturation, 0, 255);

		m_R[i] = iNewHue;
		m_G[i] = iNewSaturation;
		m_B[i] = i;
	}
}

//////////////////////////////////////////////////////////////////////
void CColorMap::MakeGamma(int iRGamma, int iGGamma, int iBGamma)
{
	m_bUsingHSL = false;

	double rGamma = (double)iRGamma * .02; // Convert 0 thru 100 to 0.0 thru 2.0; 50 is 1.0
	double gGamma = (double)iGGamma * .02;
	double bGamma = (double)iBGamma * .02;
	rGamma = 1 / rGamma;
	gGamma = 1 / gGamma;
	bGamma = 1 / bGamma;

	for (int i=0; i<256; i++)
	{
		double f = (double)i / 255.0;
		m_R[i] = (BYTE)min(255, dtoi(255.0 * pow(f, rGamma)));
		m_G[i] = (BYTE)min(255, dtoi(255.0 * pow(f, gGamma)));
		m_B[i] = (BYTE)min(255, dtoi(255.0 * pow(f, bGamma)));
	}
}

//////////////////////////////////////////////////////////////////////
void CColorMap::MakeSepiaTone(int iIntensity)
{
	int iHueAmount = 100;
	int iHue = (30 * 100) / 360; // 30 degrees
	int iSaturation = 50 + ((25 * iIntensity) / 100); // 50-75

	MakeHueSaturation(iHueAmount, iHue, iSaturation);
}

//////////////////////////////////////////////////////////////////////
bool CColorMap::ProcessImage(BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib, bool bColorize)
{
	if (!pSrcDib || !pDstDib)
		return false;

	// Only supports 24 bit Dib's
	if (pSrcDib->biBitCount != 24)
	{
		CMessageBox::Message(String("Cannot operate on an image that is %d bits deep", pSrcDib->biBitCount));
		return false;
	}

	// Setup the values that we use to walk through  the destination dib
	int xDst = 0;
	int yDst = 0;
	int yDstIncr = (int)DibWidthBytes(pDstDib);

	// Setup the dib pointers
	BYTE* pSrcStart = DibPtrXY(pSrcDib, 0, 0);
	BYTE* pDstStart = DibPtrXY(pDstDib, xDst, yDst);
	if (!pSrcStart || !pDstStart)
		return false;

	// Walk through the source dib and move pixels to the destination dib
	int ySrcIncr = DibWidthBytes(pSrcDib);
	int yCount = DibHeight(pSrcDib);
	while (--yCount >= 0)
	{
		BYTE* pDst = pDstStart;
		pDstStart += yDstIncr;
		BYTE* pSrc = pSrcStart;
		pSrcStart += ySrcIncr;

		int xCount = DibWidth(pSrcDib);
		int B = 0;
		int G = 0;
		int R = 0;
		while (--xCount >= 0)
		{
			if (m_bUsingHSL)
			{
				B = *pSrc++;
				G = *pSrc++;
				R = *pSrc++;
				
				if (bColorize)
				{
					BYTE Gray = (BYTE)dtoi((0.299 * R) + (0.587 * G) + (0.114 * B));
					B = Gray;
					G = Gray;
					R = Gray;
				}

				HSL hsl;
				RGBtoHSL(R, G, B, &hsl);
				hsl.hue = m_R[hsl.hue];
				hsl.sat = m_G[hsl.sat];
				hsl.lum = m_B[hsl.lum];
				RGBS rgb;
				HSLtoRGB(hsl.hue, hsl.sat, hsl.lum, &rgb);
				R = rgb.red;
				G = rgb.green;
				B = rgb.blue;
			}
			else
			{
				B = m_B[*pSrc++];
				G = m_G[*pSrc++];
				R = m_R[*pSrc++];
			}

			*pDst++ = B;
			*pDst++ = G;
			*pDst++ = R;
		}
	}

	return true;
}
