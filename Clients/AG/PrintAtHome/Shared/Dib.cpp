#include "stdafx.h"
#include "Dib.h"
#include "Utility.h"
#include "MessageBox.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

// There are 8 possible orientations after any number of 90 degree rotations and X or Y flips:
//
//	0:Normal	1:FlipX		2:FlipY		3:FlipXY	4:CCWFlipXY	5:CCWFlipY	6:CCWFlipX	7:CCW
//													  (CW)		  (CWFlipX)	  (CWFlipY)	  (CWFlipXY)
//
//	XXXX		XXXX		X			   X		XXXXX		XXXXX		    X		X		
//	X			   X		XXX			 XXX		  X X		X X			  X X		X X		
//	XXX			 XXX		X			   X		  X X		X X			  X X		X X		
//	X			   X		XXXX		XXXX		    X		X			XXXXX		XXXXX

//////////////////////////////////////////////////////////////////////
void CDib::OrientChangeState(BYTE& iState, int iRotateDirection, bool bFlipX, bool bFlipY)
{
							// 0  1  2  3  4  5  6  7
	const BYTE MapCW[8]		= {4, 6, 5, 7, 3, 1, 2, 0};
	const BYTE MapCCW[8]	= {7, 5, 6, 4, 0, 2, 1, 3};
	const BYTE MapFlipX[8]	= {1, 0, 3, 2, 5, 4, 7, 6};
	const BYTE MapFlipY[8]	= {2, 3, 0, 1, 6, 7, 4, 5};

	if (iRotateDirection > 0) // CW
		iState = MapCW[iState];
	if (iRotateDirection < 0) // CCW
		iState = MapCCW[iState];
	if (bFlipX)
		iState = MapFlipX[iState];
	if (bFlipY)
		iState = MapFlipY[iState];
}

//////////////////////////////////////////////////////////////////////
bool CDib::OrientDecode(BYTE iState, int& iRotateDirection, bool& bFlipX, bool& bFlipY)
{
	if (iState < 1 || iState > 7)
		return false;

	// 0:Normal, 1:FlipX, 2:FlipY, 3:FlipXY, 4:CCW-FlipXY, 5:CCW-FlipY, 6:CCW-FlipX, 7:CCW
	iRotateDirection = (iState >= 4 ? -1 : 0);
	bFlipX = (iState == 1 || iState == 3 || iState == 4 || iState == 6 ? true : false);
	bFlipY = (iState == 2 || iState == 3 || iState == 4 || iState == 5 ? true : false);
	return true;
}

//////////////////////////////////////////////////////////////////////
BITMAPINFOHEADER* CDib::OrientRestore(const BITMAPINFOHEADER* pSrcDib, BYTE iState)
{
	int iRotateDirection = 0;
	bool bFlipX = false;
	bool bFlipY = false;
	bool bOK = OrientDecode(iState, iRotateDirection, bFlipX, bFlipY);
	if (!bOK)
		return NULL;

	return Orient(pSrcDib, iRotateDirection, bFlipX, bFlipY);
}

//////////////////////////////////////////////////////////////////////
BITMAPINFOHEADER* CDib::Orient(const BITMAPINFOHEADER* pSrcDib, int iRotateDirection, bool bFlipX, bool bFlipY, BYTE* pSrcBits)
{
	if (!pSrcDib)
		return NULL;

	// Only supports 8 and 24 bit Dib's
	if (pSrcDib->biBitCount != 8 && pSrcDib->biBitCount != 24)
	{
		CMessageBox::Message(String("Failed to rotate bitmap that is %d bits deep", pSrcDib->biBitCount));
		return NULL;
	}

	BITMAPINFOHEADER Dib = *pSrcDib;

	// If rotating, swap the width and height
	if (iRotateDirection)
	{
		SWAP(Dib.biWidth, Dib.biHeight);
		Dib.biSizeImage = DibWidthBytes(&Dib) * DibHeight(&Dib);
	}

	// Allocate memory for a new dib
	BITMAPINFOHEADER* pDstDib = (BITMAPINFOHEADER*)malloc(DibSize(&Dib));
	if (!pDstDib)
	{
		CMessageBox::Message("Failed to allocate memory during image rotation");
		return NULL;
	}

	// Copy the dib header, and the palette
	*pDstDib = Dib;
	if (DibNumColors(pDstDib))
		memcpy((void*)DibColors(pDstDib), (void*)DibColors(pSrcDib), DibPaletteSize(pSrcDib));

	// To make walking through the source dib simpler,
	// convert CW rotations into CCW rotations with an X and Y flip
	bool bCCW = (iRotateDirection < 0);
	if (iRotateDirection > 0) // bCW
	{
		bCCW = true;
		bFlipX = !bFlipX;
		bFlipY = !bFlipY;
	}

	// CCW rotations require an X flip
	if (bCCW)
		bFlipX = !bFlipX;

	int iDepth = DibDepth(pDstDib);

	// Setup the values that we use to walk through  the destination dib
	int xDst = (!bFlipX ? 0 : DibWidth(pDstDib) - 1);
	int yDst = (!bFlipY ? 0 : DibHeight(pDstDib) - 1);
	int xDstIncr = (!bFlipX ? iDepth : -iDepth);
	int yDstIncr = (!bFlipY ? (int)DibWidthBytes(pDstDib) : -(int)DibWidthBytes(pDstDib));

	// If doing a CCW rotation, swap the destination increments
	if (bCCW)
		SWAP(xDstIncr, yDstIncr);

	// Setup the dib pointers
	BYTE* pSrcStart = DibPtrXY(pSrcDib, 0, 0, pSrcBits);
	BYTE* pDstStart = DibPtrXY(pDstDib, xDst, yDst);
	if (!pSrcStart || !pDstStart)
	{
		free(pDstDib);
		return NULL;
	}

	// Walk through the source dib and move pixels to the destination dib
	int xSrcIncr = iDepth;
	int ySrcIncr = DibWidthBytes(pSrcDib);
	int yCount = DibHeight(pSrcDib);
	while (--yCount >= 0)
	{
		BYTE* pDst = pDstStart;
		pDstStart += yDstIncr;
		BYTE* pSrc = pSrcStart;
		pSrcStart += ySrcIncr;

		int xCount = DibWidth(pSrcDib);
		while (--xCount >= 0)
		{
			*(pDst) = *(pSrc);
			if (iDepth == 3)
			{
				*(pDst+1) = *(pSrc+1);
				*(pDst+2) = *(pSrc+2);
			}
			pSrc += xSrcIncr;
			pDst += xDstIncr;
		}
	}

	return pDstDib;
}
