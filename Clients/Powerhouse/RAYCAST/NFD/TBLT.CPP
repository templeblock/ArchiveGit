/*********************** ( First 3D Development ) ***********************/
/*	(c) 1994/5  John Lundy															     						*/
/************************************************************************/
// tblt.cpp : transparent and non-transparent blt routines
//

#include "stdafx.h"
#include "fd.h"
#include <wing.h>
#include "utils.h"

/*--------------------------------------------------------------------------
	Copy a bitmap with transparency
*/
void	TransparentBits(char unsigned huge *pDestination,
						char unsigned huge *pSource,
						DWORD dwWidth,
						DWORD dwHeight,
						long dwScanD,
						long dwScanS,
						char unsigned bTranClr )
{
unsigned char	al, ah;
unsigned char	huge *edi, huge *esi;
long	ebx, ecx, edx;

	if (dwWidth == 0) return;
	ecx = dwWidth;
	edx = dwHeight;
	ah = bTranClr;
	esi = pSource;
	edi = pDestination;
	dwScanD -= ecx;
	dwScanS -= ecx;
	ebx = ecx;
	do
	{
		ecx = ebx >> 2;
		if (ecx)
		{
			do
			{
				al = *esi;
				if (al != ah) *edi = al;
				esi++; edi++;
				al = *esi;
				if (al != ah) *edi = al;
				esi++; edi++;
				al = *esi;
				if (al != ah) *edi = al;
				esi++; edi++;
				al = *esi;
				if (al != ah) *edi = al;
				esi++; edi++;
			}
			while (--ecx);
		}
		ecx = ebx & 3;
		if (ecx)
		{
			do
			{
				al = *esi;
				esi++;
				if (al != ah) *edi = al;
				edi++;
			}
			while (--ecx);
		}
        esi += dwScanS;
		edi += dwScanD;
	}
	while (--edx);
}

/*--------------------------------------------------------------------------
	Copy a bitmap without transparency
*/
void	CopyBits(	char unsigned huge *pDestination,
					char unsigned huge *pSource,
					DWORD dwWidth,
					DWORD dwHeight,
					long dwScanD,
					long dwScanS)
{
unsigned char	huge *edi, huge *esi;
long	eax, ebx, ecx, edx;

	if (dwWidth == 0) return;
	ecx = dwWidth;
	edx = dwHeight;
	esi = pSource;
	edi = pDestination;
	dwScanD -= ecx;
	dwScanS -= ecx;
	ebx = ecx;
	do
	{
		ecx = ebx >> 2;
		for ( ; ecx ; ecx--, edi += 4, esi += 4)
			*((unsigned long huge *)edi) = *((unsigned long huge *)esi);
		eax = ebx & 3;
		for ( ; eax ; eax--, edi++, esi++) *edi = *esi;
		esi += dwScanS;
		edi += dwScanD;
	}
	while (--edx);
}

void	TransparentDIBBits(	BITMAPINFO far *pBufferInfo,
							char huge *pBufferBits,
							int nXOriginDest,
							int nYOriginDest,
							BITMAPINFO far *pBitmapInfo,
							char huge *pBitmapBits,
							int nXOriginSrc,
							int nYOriginSrc,
							unsigned char TransparentColor )
{
char	unsigned huge *pSource;
char	unsigned huge *pDestination;
int		Orientation = 1;
int		SourceHeight		=	DibHeight(&pBitmapInfo->bmiHeader);
int		DestinationHeight	=	DibHeight(&pBufferInfo->bmiHeader);
int		Height;
int		nWidthSrc			=	DibWidth(&pBitmapInfo->bmiHeader);
int		nHeightSrc			=	DibHeight(&pBitmapInfo->bmiHeader);
long	SourceScanDelta;
long	DestinationScanDelta;
RECT	SourceRectangle = { nXOriginDest,
							nYOriginDest,
    						nXOriginDest + nWidthSrc,
    						nYOriginDest + nHeightSrc };
RECT	DestinationRectangle;
RECT	ClippedDestination;

if (DestinationHeight < 0)	// check for top-down DIB
{
	Orientation = -1;
	DestinationHeight = -DestinationHeight;
}

DestinationRectangle.top	=	0;
DestinationRectangle.left	=	0;
DestinationRectangle.bottom	=	DestinationHeight;
DestinationRectangle.right	=	DibWidth(&pBufferInfo->bmiHeader);

// intersect the destination rectangle with the destination DIB
if (IntersectRect(&ClippedDestination, &SourceRectangle, &DestinationRectangle))
{
	// default delta scan width in bytes
	DestinationScanDelta = DibWidthBytes(&pBufferInfo->bmiHeader);
	SourceScanDelta = DibWidthBytes(&pBitmapInfo->bmiHeader);

	Height = ClippedDestination.bottom - ClippedDestination.top;

	if (nXOriginDest < 0) { nXOriginSrc += nXOriginDest; nWidthSrc += nXOriginDest; }
	if (nYOriginDest < 0) { nYOriginSrc += nYOriginDest; nHeightSrc += nYOriginDest; }

	// now we'll calculate the starting pointer taking into
	// account we may have a top-down destination
	pSource = (char unsigned huge *)pBitmapBits;
	if (SourceScanDelta < 0) SourceScanDelta = -SourceScanDelta;
	pSource += (((long)(SourceHeight - ((nYOriginSrc >= 0) ? nYOriginSrc : -nYOriginSrc) - Height)) * 
				(long)DibWidthBytes(&pBitmapInfo->bmiHeader)) + 
				(long)((nXOriginSrc >= 0) ? nXOriginSrc : -nXOriginSrc);
	pDestination =	(char unsigned huge *)pBufferBits;
	if (Orientation < 0)
	{
		// top-down
		DestinationScanDelta = -DestinationScanDelta;
		pDestination +=	((long)(ClippedDestination.bottom - 1) *
	        			(long)DibWidthBytes(&pBufferInfo->bmiHeader) +
	        			ClippedDestination.left);
	}
	else
	{
		// bottom-up
		pDestination +=	(((long)DestinationHeight - 
						ClippedDestination.bottom) *
						(long)DibWidthBytes(&pBufferInfo->bmiHeader) +
						ClippedDestination.left);
	}

	TransparentBits(pDestination,
	   				pSource,
	   				ClippedDestination.right - ClippedDestination.left,	//	Width
	   				Height,
					DestinationScanDelta,
					SourceScanDelta,
					TransparentColor);

}
}

void	CopySomeDIBBits(BITMAPINFO far *pBufferInfo,
						char huge *pBufferBits,
						int nXOriginDest,
						int nYOriginDest,
						BITMAPINFO far *pBitmapInfo,
						char huge *pBitmapBits,
						int nXOriginSrc,
						int nYOriginSrc,
						int nWidthSrc,
						int nHeightSrc)
{
char	unsigned huge *pSource;
char	unsigned huge *pDestination;
int		Orientation = 1;
int		SourceHeight		=	DibHeight(&pBitmapInfo->bmiHeader);
int		DestinationHeight	=	DibHeight(&pBufferInfo->bmiHeader);
int		Height;
long	SourceScanDelta;
long	DestinationScanDelta;
RECT	SourceRectangle = { nXOriginDest,
							nYOriginDest,
    						nXOriginDest + nWidthSrc,
    						nYOriginDest + nHeightSrc };
RECT	DestinationRectangle;
RECT	ClippedDestination;

if (DestinationHeight < 0)	// check for top-down DIB
{
	Orientation = -1;
	DestinationHeight = -DestinationHeight;
}

DestinationRectangle.top	=	0;
DestinationRectangle.left	=	0;
DestinationRectangle.bottom	=	DestinationHeight;
DestinationRectangle.right	=	DibWidth(&pBufferInfo->bmiHeader);

// intersect the destination rectangle with the destination DIB
if (IntersectRect(&ClippedDestination, &SourceRectangle, &DestinationRectangle))
{
	// default delta scan width in bytes
	DestinationScanDelta = DibWidthBytes(&pBufferInfo->bmiHeader);
	SourceScanDelta = DibWidthBytes(&pBitmapInfo->bmiHeader);

	Height = ClippedDestination.bottom - ClippedDestination.top;

	if (nXOriginDest < 0) { nXOriginSrc += nXOriginDest; nWidthSrc += nXOriginDest; }
	if (nYOriginDest < 0) { nYOriginSrc += nYOriginDest; nHeightSrc += nYOriginDest; }

	// now we'll calculate the starting pointer taking into
	// account we may have a top-down destination
	pSource = (char unsigned huge *)pBitmapBits;
	if (SourceScanDelta < 0) SourceScanDelta = -SourceScanDelta;
	pSource += (((long)(SourceHeight - ((nYOriginSrc >= 0) ? nYOriginSrc : 0) - Height)) * 
				(long)DibWidthBytes(&pBitmapInfo->bmiHeader)) + 
				(long)((nXOriginSrc >= 0) ? nXOriginSrc : 0);
	pDestination =	(char unsigned huge *)pBufferBits;
	if (Orientation < 0)
	{
		// top-down
		DestinationScanDelta = -DestinationScanDelta;
		pDestination +=	((long)(ClippedDestination.bottom - 1) *
	        			(long)DibWidthBytes(&pBufferInfo->bmiHeader) +
	        			ClippedDestination.left);
	}
	else
	{
		// bottom-up
		pDestination +=	(((long)DestinationHeight - 
						ClippedDestination.bottom) *
						(long)DibWidthBytes(&pBufferInfo->bmiHeader) +
						ClippedDestination.left);
	}

	CopyBits(	pDestination,
	   			pSource,
	   			ClippedDestination.right - ClippedDestination.left,	//	Width
	   			Height,
				DestinationScanDelta,
				SourceScanDelta);

}
}

void	CopyDIBBits(BITMAPINFO far *pBufferInfo,
					char huge *pBufferBits,
					int nXOriginDest,
					int nYOriginDest,
					BITMAPINFO far *pBitmapInfo,
					char huge *pBitmapBits,
					int nXOriginSrc,
					int nYOriginSrc)
{

CopySomeDIBBits((BITMAPINFO far *)pBufferInfo,
				(char huge *)pBufferBits,
				nXOriginDest,
				nYOriginDest,
				(BITMAPINFO far *)pBitmapInfo,
				(char huge *)pBitmapBits,
				nXOriginSrc,
				nYOriginSrc,
				DibWidth(&pBitmapInfo->bmiHeader),
				DibHeight(&pBitmapInfo->bmiHeader));
}
