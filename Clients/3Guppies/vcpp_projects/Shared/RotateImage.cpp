#include "stdafx.h"
#include "RotateImage.h"
#include "Dib.h"
#include "Utility.h"
#include <math.h>

#define PIdiv180 0.0174532925199432957692369076848861

/////////////////////////////////////////////////////////////////////////////
void RotateImage(const BITMAPINFOHEADER* pDibSrc, const BITMAPINFOHEADER* pDibDst, double fAngle, double fScale)
{
	CDib DibSrc = pDibSrc;
	CDib DibDst = pDibDst;

	int dxSrc = DibSrc.Width();
	int dySrc = DibSrc.Height();
	int dxDst = DibDst.Width();
	int dyDst = DibDst.Height();

	int lxSrc = dxSrc - 1; // Last X
	int lySrc = dySrc - 1; // Last Y

	double fAngleRadian = fAngle * PIdiv180;	
	double duCol = sin(fAngleRadian) * (1.0 / fScale);
	double dvCol = cos(fAngleRadian) * (1.0 / fScale);
	double duRow = dvCol;
	double dvRow = -duCol;

	double fcxSrc = dxSrc / 2;
	double fcySrc = dySrc / 2;
	double fcxDst = dxDst / 2;
	double fcyDst = dyDst / 2;

	double uRow = fcxSrc - (fcxDst * dvCol + fcyDst * duCol);
	double vRow = fcySrc - (fcxDst * dvRow + fcyDst * duRow);

	int iDepth = DibDst.Depth();
	for (int y = 0; y < dyDst; y++)
	{
		double u = uRow;
		double v = vRow;

		BYTE* pDst = DibDst.PtrXYExact(0, y);
		for (int x = 0; x < dxDst ; x++)
		{ 
			int sx = dtoi(u);
			int sy = dtoi(v);

			bool bInside = (sx >= 0 && sx <= lxSrc && sy >= 0 && sy <= lySrc);
			if (!bInside)
			{ // Allow a 2 pixel edge to avoid seeing any mismatch with the clip rectangle
				bool bInsideEdge = (sx >= -2 && sx <= lxSrc+3 && sy >= -2 && sy <= lySrc+3);
				if (bInsideEdge)
				{
					if (sx >= -2      && sx < 0)		sx = 0;
					if (sx <= lxSrc+3 && sx > lxSrc)	sx = lxSrc;
					if (sy >= -2      && sy < 0)		sy = 0;
					if (sy <= lySrc+3 && sy > lySrc)	sy = lySrc;
					bInside = (sx >= 0 && sx <= lxSrc && sy >= 0 && sy <= lySrc);
				}
			}

			if (bInside)
			{
				BYTE* pSrc = DibSrc.PtrXYExact(sx, sy);
				*pDst++ = *pSrc++;
				if (iDepth == 3)
				{
					*pDst++ = *pSrc++;
					*pDst++ = *pSrc++;
				}
			}
			else
			{
				*pDst++ = 255;
				if (iDepth == 3)
				{
					*pDst++ = 255;
					*pDst++ = 255;
				}
			}

			u += duRow;
			v += dvRow;
		}

		uRow += duCol;
		vRow += dvCol;
	}
}
