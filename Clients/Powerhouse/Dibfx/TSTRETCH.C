/*************************************************************************
	tstretch.c

	Transparent Stretch DIB function

	This file is Copyright (c) 1994 Microsoft Corporation
*/

#ifndef _INC_WINDOWS
#include <windows.h>
#include <windowsx.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include "dib.h"
#include "dibfx.h"

/*************************************************************************
	The transparent stretch engine from tstret32.asm
*/

extern void FAR PASCAL DoDibTransparentStretch(
	PDIB pdibDst,
	LPVOID lpDst,
	int DstX,
	int DstY,
	int DstXE,
	int DstYE,
	PDIB pdibSrc,
	LPVOID lpSrc,
	int SrcX,
	int SrcY,
	int SrcXE,
	int SrcYE,
	BYTE Transparent);

/*************************************************************************
	The wrapper for DibTransparentStretch

	This does analytical clipping, inverts the Y coordinates,
	and calls down to the assembly level stretch routine.
*/

BOOL FAR PASCAL DibTransparentStretch(
	PDIB pdibDst,
	LPVOID lpDst,
	int DstX,
	int DstY,
	int DstXE,
	int DstYE,
	PDIB pdibSrc,
	LPVOID lpSrc,
	int SrcX,
	int SrcY,
	int SrcXE,
	int SrcYE,
	char unsigned Transparent)
{
	int XMax, YMax;
	Fixed fClipRatio;
	int iTemp;

	assert(pdibDst && lpDst);
	assert(pdibSrc && lpSrc);

	//*** Currently, this only works with 8-bit dibs
	assert(pdibDst->biBitCount == 8); 
	assert(pdibSrc->biBitCount == 8);

	//*** This does NOT mirror
	assert (DstXE >= 0 && DstYE >= 0);
	assert (SrcXE >= 0 && SrcYE >= 0);

	XMax = (int)pdibDst->biWidth - 1;
	// bill@mediastation.com:  I changed the following line.
	// In the version I was given, the abs was not here.
	// As a result, this did not handle top-down DIBS.
	// The rest of the routine works fine.
	YMax = abs((int)pdibDst->biHeight) - 1;
	///End WWW Change.

	//*** Trivial reject
	if (DstX > XMax || DstY > YMax ||
		(DstX + DstXE) < 0 || (DstY + DstYE) < 0)
		return TRUE;

	/// bill@mediastation.com: I duplicated the following line
	// of code.  This is changing the starting position
	// of the blit from a 'standard' Windows top-down
	// coordinate system to a 'standard' DIB bottom-up
	// coordinate system.  Unfortunately, the original
	// source only changed the destination.  This meant
	// that it painted from the wrong spot in the source
	// DIB.  So, I changed the source Y spot as well.
	DstY = YMax + 1 - DstY - DstYE;
	SrcY = abs ((int)pdibSrc->biHeight) - SrcY - SrcYE;

	//*** Clip the left edge
	if (DstX < 0)
	{
		fClipRatio = FixedDivide( IntToFixed(-DstX),
			IntToFixed(DstXE) );

		//*** Remember, DstX is negative!
		DstXE = DstXE + DstX;
		DstX = 0;

		iTemp = FixedToInt( FixedMultiply( fClipRatio, IntToFixed(SrcXE) ) );
		SrcX = SrcX + iTemp;
		SrcXE = SrcXE - iTemp;
	}

	//*** Clip the top edge
	if (DstY < 0)
	{
		fClipRatio = FixedDivide( IntToFixed(-DstY),
			IntToFixed(DstYE) );

		//*** Remember, DstY is negative!
		DstYE = DstYE + DstY;
		DstY = 0;

		iTemp = FixedToInt( FixedMultiply( fClipRatio, IntToFixed(SrcYE) ) );
		SrcY = SrcY + iTemp;
		SrcYE = SrcYE - iTemp;
	}

	
	//
	// bill@mediastation.com
	// XMax (and later YMax) are being used as 0 based when
	// they are in fact the width (and height) in pixels of
	// the dib and are 1 based.
	//
	//*** Clip the X extent
	if ((DstX + DstXE) > (XMax + 1))
	{
		fClipRatio = FixedDivide( IntToFixed(XMax - DstX),
			IntToFixed(DstXE) );

		DstXE = XMax - DstX + 1;
		SrcXE = FixedToInt( FixedMultiply(fClipRatio, IntToFixed(SrcXE)) );

		//*** Make sure the round-off is correct
		if ((DstX + DstXE) < XMax)
		  ++DstXE;
	}

	//
	// bill@mediastation.com
	// As above, YMax was being used as a 0 based value
	// instead of the 1 based that it is.
	//
	//*** Clip the Y extent
	if ((DstY + DstYE) > (YMax + 1))
	{
		fClipRatio = FixedDivide( IntToFixed(YMax - DstY),
			IntToFixed(DstYE) );

		DstYE = YMax - DstY + 1;
		SrcYE = FixedToInt( FixedMultiply(fClipRatio, IntToFixed(SrcYE)) );

		//*** Make sure the round-off is correct
		if ((DstY + DstYE) < YMax)
		  ++DstYE;
	}

	DoDibTransparentStretch(
		pdibDst, lpDst, DstX, DstY, DstXE, DstYE,
		pdibSrc, lpSrc, SrcX, SrcY, SrcXE, SrcYE,
		Transparent);

	return TRUE;
}

