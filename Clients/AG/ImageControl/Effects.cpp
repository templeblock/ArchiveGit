#include "StdAfx.h"
#include "Effects.h"
#include "ColorMap.h"
#include "Utility.h"
#include "MessageBox.h"
#include "Dib.h"

// The following kernel definitions are for convolution filtering. Kernel entries are specified
// with a divisor to get around the requirement for floating point numbers in the low pass filters. 

KERNEL3 KSharpen = {	// Sharpen
{	 -8,-12, -8,
	-12, 90,-12,
	 -8,-12, -8},
	10,				// Divisor
	0				// Bias
};

KERNEL3 KBlur = {	// Blur
{	4,  2,  4,
	2,  0,  2,
	4,  2,  4},
	24,				// Divisor
	0				// Bias
};

KERNEL3 KEdges = {	// Edges
{	 2,  2,  2,
	 2, -4,  2,
	-2, -2, -2},
	1,				// Divisor
	0				// Bias
};

KERNEL3 KEmboss = {	// Emboss
{	-1, -1,  0,
	-1,  0,  1,
	 0,  1,  1},
	1,				// Divisor
	128				// Bias
};

#ifdef NOTUSED
KERNEL3 HP1 = {		// HP filter #1
{	-1, -1, -1,
	-1,  9, -1,
	-1, -1, -1},
    1,				// Divisor
    0				// Bias
};

KERNEL3 HP2 = {		// HP filter #2
{	 0, -1,  0,
	-1,  5, -1,
	 0, -1,  0},
	1,				// Divisor
	0				// Bias
};

KERNEL3 HP3 = {		// HP filter #3
{	 1, -2,  1,
	-2,  5, -2,
	 1, -2,  1},
	1,				// Divisor
	0				// Bias
};

KERNEL3 LP1 = {		// LP filter #1
{	1,  1,  1,
	1,  1,  1,
	1,  1,  1},
	9,				// Divisor
	0				// Bias
};

KERNEL3 LP2 = {		// LP filter #2
{	1,  1,  1,
	1,  2,  1,
	1,  1,  1},
	10,				// Divisor
	0				// Bias
};

KERNEL3 LP3 = {		// LP filter #3
{	1,  2,  1,
	2,  4,  2,
	1,  2,  1},
	16,				// Divisor
	0				// Bias
};

KERNEL3 EdgeNorth = {// North gradient
{	 1,  1,  1,
	 1, -2,  1,
	-1, -1, -1},
	1,				// Divisor
	0				// Bias
};

KERNEL3 EdgeNorthEast = {	// North East gradient
{	 1,  1,  1,
	-1, -2,  1,
	-1, -1,  1},
	1,				// Divisor
	0				// Bias
};

KERNEL3 EdgeEast = {	// East gradient
{	-1,  1,  1,
	-1, -2,  1,
	-1,  1,  1},
	1,				// Divisor
	0				// Bias
};

KERNEL3 EdgeSouthEast = {// South East gradient
{	-1, -1,  1,
	-1, -2,  1,
	 1,  1,  1},
	1,				// Divisor
	0				// Bias
};

KERNEL3 EdgeSouth = {// South gradient
{	-1, -1, -1,
	 1, -2,  1,
	 1,  1,  1},
	1,				// Divisor
	0				// Bias
};

KERNEL3 EdgeSouthWest = {// South West gradient
{	1, -1, -1,
	1, -2, -1,
	1,  1,  1},
	1,				// Divisor
	0				// Bias
};

KERNEL3 EdgeWest = {	// West gradient
{	1,  1, -1,
	1, -2, -1,
	1,  1, -1},
	1,				// Divisor
	0				// Bias
};

KERNEL3 EdgeNorthWest = {// North West gradient
{	1,  1,  1,
	1, -2, -1,
	1, -1, -1},
	1,				// Divisor
	0				// Bias
};

KERNEL3 EdgePREWITT_V = {// 
{	-1,  0,  1, 
	-1,  0,  1, 
	-1,  0,  1};
	1,				// Divisor
	0				// Bias
};

KERNEL3 EdgePREWITT_H = {// 
{	-1, -1, -1, 
	 0,  0,  0, 
	 1,  1,  1};
	1,				// Divisor
	0				// Bias
};

KERNEL3 EdgeSOBEL_V = {// 
{	-1,  0,  1, 
	-2,  0,  2, 
	-1,  0,  1};
	1,				// Divisor
	0				// Bias
};

KERNEL3 EdgeSOBEL_H = {// 
{	-1, -2, -1, 
	 0,  0,  0, 
	 1,  2,  1};
	1,				// Divisor
	0				// Bias
};

KERNEL3 EdgeFREI_CHEN_V = {// 
{	-10, 0, 10, 
	-14, 0, 14, 
	-10, 0, 10};
    10,				// Divisor
    0				// Bias
};

KERNEL3 EdgeFREI_CHEN_H = {// 
 {	-10, -14, -10, 
	  0,   0,   0, 
	 10,  14,  10};
    10,				// Divisor
    0				// Bias
};

// More complex edge algorithm calculates 2 filters as it goes - H & V
// and then computes RGB as follows:
//		R = sqrt(RH*RH + RV*RV) / 1.8;
//		G = sqrt(GH*GH + GV*GV) / 1.8;
//		B = sqrt(BH*BH + BV*BV) / 1.8;

#endif NOTUSED

//////////////////////////////////////////////////////////////////////
bool CEffects::Sharpen(int iStrength, BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib)
{
	return Convolve(KSharpen, iStrength, pSrcDib, pDstDib);
}

//////////////////////////////////////////////////////////////////////
bool CEffects::Blur(int iStrength, BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib)
{
	CDib DibSrc = pSrcDib;
	CDib pDibNew = DibSrc.Blur(iStrength);
	if (!pDibNew)
		return false;

	pDibNew.DuplicateData(pDstDib);
	return true;
//j	return Convolve(KBlur, iStrength, pSrcDib, pDstDib);
}

//////////////////////////////////////////////////////////////////////
bool CEffects::Edges(int iStrength, BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib)
{
	return Convolve(KEdges, iStrength, pSrcDib, pDstDib);
}

//////////////////////////////////////////////////////////////////////
bool CEffects::Emboss(int iStrength, BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib)
{
	return Convolve(KEmboss, iStrength, pSrcDib, pDstDib);
}

//////////////////////////////////////////////////////////////////////
bool CEffects::Convolve(const KERNEL3& Kernel, int iStrength, BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib)
{
	if (!pSrcDib)
		return false;

	// Only supports 24 bit Dib's
	if (pSrcDib->biBitCount != 24)
	{
		CMessageBox::Message(String("Cannot operate on an image that is %d bits deep", pSrcDib->biBitCount));
		return false;
	}

	if (!pSrcDib || !pDstDib)
		return false;

	// Setup the dib pointers
	BYTE* pSrcStart = DibPtrXY(pSrcDib, 0, 0);
	BYTE* pDstStart = DibPtrXY(pDstDib, 0, 0);
	if (!pSrcStart || !pDstStart)
		return false;

	int ySrcIncr = DibWidthBytes(pSrcDib);
	int xCount = DibWidth(pSrcDib);
	int yCount = DibHeight(pSrcDib);

	if (iStrength < 0)
		iStrength = 0;
	if (iStrength > 100)
		iStrength = 100;

	if (!iStrength)
		return DibDuplicateData(pSrcDib, pDstDib);

	// Compensate for edge effects
	WORD xStart = (KERNEL3_SIZE / 2);
	WORD yStart = (KERNEL3_SIZE / 2);
	WORD dx = xCount - (KERNEL3_SIZE - 1);
	WORD dy = yCount - (KERNEL3_SIZE - 1);

	int yOffset = ySrcIncr - (3 * KERNEL3_SIZE);
	int yBigOffset = (ySrcIncr * KERNEL3_SIZE) - 3;
	for (int y = 0; y < dy; y++)
	{
		// Get the pointer to the destination row
		BYTE* pDst = pDstStart + (ySrcIncr * (y + yStart)) + (xStart * 3);

		// Get the pointers to the source row; each successive row is yOffset away
		BYTE* p = pSrcStart + (ySrcIncr * y);

		for (int x = 0; x < dx; x++)
		{
			// Point at first number in kernel
			const int* pElement = Kernel.Element;
			int Element = 0;
			int R = 0;
			int G = 0;
			int B = 0;

			// Row 1
			Element = *pElement++; B += *p++ * Element; G += *p++ * Element; R += *p++ * Element;
			Element = *pElement++; B += *p++ * Element; G += *p++ * Element; R += *p++ * Element;
			Element = *pElement++; B += *p++ * Element; G += *p++ * Element; R += *p++ * Element;
			p += yOffset;

			// Row 2 - Save the center source pixel value
			Element = *pElement++; B += *p++ * Element; G += *p++ * Element; R += *p++ * Element;
			int OB = *p++; int OG = *p++; int OR = *p++;
			Element = *pElement++; B += OB * Element; G += OG * Element; R += OR * Element;
			Element = *pElement++; B += *p++ * Element; G += *p++ * Element; R += *p++ * Element;
			p += yOffset;

			// Row 3
			Element = *pElement++; B += *p++ * Element; G += *p++ * Element; R += *p++ * Element;
			Element = *pElement++; B += *p++ * Element; G += *p++ * Element; R += *p++ * Element;
			Element = *pElement++; B += *p++ * Element; G += *p++ * Element; R += *p++ * Element;
			p += yOffset;

			p -= yBigOffset;

			int iBias = Kernel.Bias;
			if (Kernel.Divisor != 1 || iStrength != 100)
			{
				int iDivisor = Kernel.Divisor;
				int ORex = OR * iDivisor;
				int OGex = OG * iDivisor;
				int OBex = OB * iDivisor;

				iDivisor *= 100;
				R = OR + (((R - ORex) * iStrength) / iDivisor);
				G = OG + (((G - OGex) * iStrength) / iDivisor);
				B = OB + (((B - OBex) * iStrength) / iDivisor);
				iBias = (iBias * iStrength) / 100;
			}

			// Add any bias
			if (iBias)
			{
				R += iBias;
				G += iBias;
				B += iBias;
			}
			
			// Range check the pixel before storing it in output buffer
			R = (R > 255 ? 255 : R); R = (R <   0 ?   0 : R);
			G = (G > 255 ? 255 : G); G = (G <   0 ?   0 : G);
			B = (B > 255 ? 255 : B); B = (B <   0 ?   0 : B);

			// Store the pixel
			*pDst++ = B;
			*pDst++ = G;
			*pDst++ = R;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CEffects::Pixelize(int iStrength, BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib)
{
	const int MIN_BLOCKS = 50;

	if (!pSrcDib)
		return false;

	// Only supports 24 bit Dib's
	if (pSrcDib->biBitCount != 24)
	{
		CMessageBox::Message(String("Cannot operate on an image that is %d bits deep", pSrcDib->biBitCount));
		return false;
	}

	if (!pSrcDib || !pDstDib)
		return false;

	// Setup the dib pointers
	BYTE* pSrcStart = DibPtrXY(pSrcDib, 0, 0);
	BYTE* pDstStart = DibPtrXY(pDstDib, 0, 0);
	if (!pSrcStart || !pDstStart)
		return false;

	int ySrcIncr = DibWidthBytes(pSrcDib);
	int xCount = DibWidth(pSrcDib);
	int yCount = DibHeight(pSrcDib);

	if (iStrength < 0)
		iStrength = 0;
	if (iStrength > 100)
		iStrength = 100;

	if (!iStrength)
		return DibDuplicateData(pSrcDib, pDstDib);

	int nMaxBlockSize = max(xCount, yCount) / MIN_BLOCKS;
	int nBlockSize = (nMaxBlockSize * iStrength) / 100;
	if (nBlockSize < 1)
		nBlockSize = 1;

	for (int y = 0; y < yCount; y++)
	{
		// Get the pointer to the destination row
		BYTE* pDst = pDstStart + (ySrcIncr * y);

		int yTarget = y - (y % nBlockSize) + nBlockSize / 2;
		if (yTarget >= yCount) yTarget = yCount - 1;
		for (int x = 0; x < xCount; x++)
		{
			// Get the pointer to the source pixel
			int xTarget = x - (x % nBlockSize) + nBlockSize / 2;
			if (xTarget >= xCount) xTarget = xCount - 1;
			BYTE* pSrc = DibPtrXY(pSrcDib, xTarget, yTarget);

			// Store the pixel
			*pDst++ = *pSrc++;
			*pDst++ = *pSrc++;
			*pDst++ = *pSrc++;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CEffects::Posterize(int iStrength, BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib)
{
	if (iStrength < 0)
		iStrength = 0;
	if (iStrength > 100)
		iStrength = 100;

	if (!iStrength)
		return DibDuplicateData(pSrcDib, pDstDib);

	int nLevels = (!iStrength ? 256 : (35 - iStrength/3)); // 0-100 to 0,35-2

	CColorMap ColorMap;
	ColorMap.MakePoster(nLevels);
	return ColorMap.ProcessImage(pSrcDib, pDstDib);
}

//////////////////////////////////////////////////////////////////////
bool CEffects::Prism(int iStrength, BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib)
{
	const int MIN_BLOCKS = 50;

	if (!pSrcDib)
		return false;

	// Only supports 24 bit Dib's
	if (pSrcDib->biBitCount != 24)
	{
		CMessageBox::Message(String("Cannot operate on an image that is %d bits deep", pSrcDib->biBitCount));
		return false;
	}

	if (!pSrcDib || !pDstDib)
		return false;

	// Setup the dib pointers
	BYTE* pSrcStart = DibPtrXY(pSrcDib, 0, 0);
	BYTE* pDstStart = DibPtrXY(pDstDib, 0, 0);
	if (!pSrcStart || !pDstStart)
		return false;

	int ySrcIncr = DibWidthBytes(pSrcDib);
	int xCount = DibWidth(pSrcDib);
	int yCount = DibHeight(pSrcDib);

	if (iStrength < 0)
		iStrength = 0;
	if (iStrength > 100)
		iStrength = 100;

	if (!iStrength)
		return DibDuplicateData(pSrcDib, pDstDib);

//j	int iOffset = (iStrength + 4) / 5; // 0, 1-20
//j	int iAngle = (!iStrength ? 0 : (iStrength - 1) % 5); // 0-4
//j	iAngle *= 30; // 0-120
//j	CMatrix Matrix;
//j	Matrix.Rotate(iAngle, iAngle);
//j	CPoint ptOffsetR(iOffset, iOffset);
//j	Matrix.Transform(ptOffsetR);

	int iOffset = iStrength;
	int iOffsetR = iOffset * -1;
	int iOffsetG = iOffset *  0;
	int iOffsetB = iOffset *  1;

	for (int y = 0; y < yCount; y++)
	{
		// Get the pointer to the destination row
		BYTE* pDst = pDstStart + (ySrcIncr * y);

		int yTargetR = (y + iOffsetR);
		int yTargetG = (y + iOffsetG);
		int yTargetB = (y + iOffsetB);
		for (int x = 0; x < xCount; x++)
		{
			// Get the pointer to the source pixel
			int xTargetR = (x + iOffsetR);
			int xTargetG = (x + iOffsetG);
			int xTargetB = (x + iOffsetB);
			BYTE R = (xTargetR < 0 || xTargetR >= xCount || yTargetR < 0 || yTargetR >= yCount ? 255 : *(DibPtrXY(pSrcDib, xTargetR, yTargetR) + 2));
			BYTE G = (xTargetG < 0 || xTargetG >= xCount || yTargetG < 0 || yTargetG >= yCount ? 255 : *(DibPtrXY(pSrcDib, xTargetG, yTargetG) + 1));
			BYTE B = (xTargetB < 0 || xTargetB >= xCount || yTargetB < 0 || yTargetB >= yCount ? 255 : *(DibPtrXY(pSrcDib, xTargetB, yTargetB) + 0));

			// Store the pixel
			*pDst++ = B;
			*pDst++ = G;
			*pDst++ = R;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CEffects::OilPainting(int iStrength, BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib)
{
	if (!pSrcDib)
		return false;

	// Only supports 24 bit Dib's
	if (pSrcDib->biBitCount != 24)
	{
		CMessageBox::Message(String("Cannot operate on an image that is %d bits deep", pSrcDib->biBitCount));
		return false;
	}

	if (!pSrcDib || !pDstDib)
		return false;

	// Setup the dib pointers
	BYTE* pSrcStart = DibPtrXY(pSrcDib, 0, 0);
	BYTE* pDstStart = DibPtrXY(pDstDib, 0, 0);
	if (!pSrcStart || !pDstStart)
		return false;

	int ySrcIncr = DibWidthBytes(pSrcDib);
	int xCount = DibWidth(pSrcDib);
	int yCount = DibHeight(pSrcDib);

	if (iStrength < 0)
		iStrength = 0;
	if (iStrength > 100)
		iStrength = 100;
		
	if (!iStrength)
		return DibDuplicateData(pSrcDib, pDstDib);

	// Convert strength into the number of bits of resolution to use
	 // Strength of 100 = 0 bits of reduction, Strength of 0 = 7 bits of reduction, 
	int nBits = ((iStrength  * 7) + 50) / 100;
	int nLevels = (256>>nBits);

	// Compensate for edge effects
	#define SAMPLE7_SIZE 7
	WORD xStart = (SAMPLE7_SIZE / 2);
	WORD yStart = (SAMPLE7_SIZE / 2);
	WORD dx = xCount - (SAMPLE7_SIZE - 1);
	WORD dy = yCount - (SAMPLE7_SIZE - 1);

	// Setup the count arrays
	int rCount[256];
	int gCount[256];
	int bCount[256];

	// Clear out the count arrays
	int* prCount = rCount;
	int* pgCount = gCount;
	int* pbCount = bCount;
	for (int i = 0; i < nLevels; i++)
	{
		*prCount++ = 0;
		*pgCount++ = 0;
		*pbCount++ = 0;
	}

	int xOffset = (3 * (SAMPLE7_SIZE-1));
	int yOffset = ySrcIncr - (3 * SAMPLE7_SIZE);
	int yOffset1 = yOffset - 3;
	int yBigOffset = (ySrcIncr * SAMPLE7_SIZE);
	for (int y = 0; y < dy; y++)
	{
		// Get the pointer to the destination row
		BYTE* pDst = pDstStart + (ySrcIncr * (y + yStart)) + (xStart * 3);

		// Get the pointers to the source row; each successive row is yOffset away
		BYTE* p = pSrcStart + (ySrcIncr * y);

		// Pre-load the count arrays
		int R = 0;
		int G = 0;
		int B = 0;

		// Row 1
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		p += yOffset;																  
																					  
		// Row 2																	  
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		p += yOffset;																  
																					  
		// Row 3																	  
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		p += yOffset;																  
																					  
		// Row 4																	  
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		p += yOffset;																  
																					  
		// Row 5																	  
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		p += yOffset;																  
																					  
		// Row 6																	  
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		p += yOffset;																  
																					  
		// Row 7																	  
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
		p += yOffset;

		p -= yBigOffset;

		for (int x = 0; x < dx; x++)
		{
			// Find the most popular pixel value
			R = 0;
			G = 0;
			B = 0;
			int irMaxCount = -1;
			int igMaxCount = -1;
			int ibMaxCount = -1;
			prCount = rCount;
			pgCount = gCount;
			pbCount = bCount;
			for (int i = 0; i < nLevels; i++)
			{
				if (irMaxCount < *prCount) { irMaxCount = *prCount; R = i; };
				if (igMaxCount < *pgCount) { igMaxCount = *pgCount; G = i; };
				if (ibMaxCount < *pbCount) { ibMaxCount = *pbCount; B = i; };
				prCount++;
				pgCount++;
				pbCount++;
			}

			// Store the pixel
			*pDst++ = B<<nBits;
			*pDst++ = G<<nBits;
			*pDst++ = R<<nBits;

			// To advance to the next pixel, subtract the first pixels's contribution, then in the last pixel's contribution

			// Row 1
			B = *p++; G = *p++; R = *p++; rCount[R>>nBits]--; gCount[G>>nBits]--; bCount[B>>nBits]--;
			p += xOffset;
			B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
			p += yOffset1;																  

			// Row 2
			B = *p++; G = *p++; R = *p++; rCount[R>>nBits]--; gCount[G>>nBits]--; bCount[B>>nBits]--;
			p += xOffset;
			B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
			p += yOffset1;																  

			// Row 3
			B = *p++; G = *p++; R = *p++; rCount[R>>nBits]--; gCount[G>>nBits]--; bCount[B>>nBits]--;
			p += xOffset;
			B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
			p += yOffset1;																  

			// Row 4
			B = *p++; G = *p++; R = *p++; rCount[R>>nBits]--; gCount[G>>nBits]--; bCount[B>>nBits]--;
			p += xOffset;
			B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
			p += yOffset1;																  

			// Row 5
			B = *p++; G = *p++; R = *p++; rCount[R>>nBits]--; gCount[G>>nBits]--; bCount[B>>nBits]--;
			p += xOffset;
			B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
			p += yOffset1;																  

			// Row 6
			B = *p++; G = *p++; R = *p++; rCount[R>>nBits]--; gCount[G>>nBits]--; bCount[B>>nBits]--;
			p += xOffset;
			B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
			p += yOffset1;																  

			// Row 7
			B = *p++; G = *p++; R = *p++; rCount[R>>nBits]--; gCount[G>>nBits]--; bCount[B>>nBits]--;
			p += xOffset;
			B = *p++; G = *p++; R = *p++; rCount[R>>nBits]++; gCount[G>>nBits]++; bCount[B>>nBits]++;
			p += yOffset;																  

			p -= yBigOffset;
		}

		// Clear out the count arrays
		prCount = rCount;
		pgCount = gCount;
		pbCount = bCount;
		for (int i = 0; i < nLevels; i++)
		{
			*prCount++ = 0;
			*pgCount++ = 0;
			*pbCount++ = 0;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CEffects::Antiqued(int iStrength, BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib)
{
	if (iStrength < 0)
		iStrength = 0;
	if (iStrength > 100)
		iStrength = 100;

	if (!iStrength)
		return DibDuplicateData(pSrcDib, pDstDib);

	int iIntensity = iStrength;

	CColorMap ColorMap;
	ColorMap.MakeSepiaTone(iIntensity);
	return ColorMap.ProcessImage(pSrcDib, pDstDib, true/*bColorize*/);
}

//////////////////////////////////////////////////////////////////////
bool CEffects::Texturize(LPCTSTR pstrType, int iStrength, BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib)
{
	if (pstrType == "PAPER")
	{
	}
	else
	if (pstrType == "OTHER")
	{
	}
	
	return false;
}
