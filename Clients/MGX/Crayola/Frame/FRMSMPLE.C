/*=======================================================================*\

	FRMSMPLE.C - Pixel Sampleing routines.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#include <windows.h>
#include "framelib.h"
#include "macros.h"

/************************************************************************/

//static LPTR lpBuffer;
//static int iBufferSize;

void Sample_1( LPTR input, int inskip, LPTR output, int outskip,
                  int count, LFIXED rate )
{
LFIXED 	offset;
WORD 	   bit;
BYTE	   byte, outmask;

offset = (inskip & 7) << 16;
input += (inskip >> 3);
byte = 0;
outmask = 128;
while ( --count >= 0 )
	{
	bit =  WHOLE(offset);
	if ( *(input + (bit >> 3)) & (128 >> (bit & 7)) )
		byte |= outmask;
	if ( !( outmask >>= 1 ) )
		{
		*output++ = byte;
		byte = 0;
		outmask = 128;
		}
	offset += rate;
	}

if ( outmask != 128 )
	*output = byte;

//LineArtToGrayScale( input, lpBuffer, iSize );
//Sample_8( lpBuffer, inskip, output, outskip, count, rate );
//GrayScaleToLineArt( output, output, count );
}

void Sample_1MW( LPTR input, int inskip, LPTR output, int outskip,
                  int count, LFIXED rate, LPTR mask )
{
}

#ifdef C_CODE

void Sample_8( LPTR input, int inskip, LPTR output, int outskip,
                  int count, LFIXED rate )
{
	LFIXED offset;
	LPTR   lpSrc = (LPTR)input;
	LPTR   lpDst = (LPTR)output;

	// Skip inskip pixels at start
	lpSrc += inskip;

	// Skip outskip pixels on output
	offset  = FDIV2(rate);
	offset += ( rate * outskip );

	// only process the number of pixels left
	count -= outskip;

	if (rate != 1L)
	{
		// Begin processing data
		while( --count >= 0 )
		{
			*lpDst++  = lpSrc[WHOLE(offset)];
			offset   += rate;
		}
	}
	else
	{
		copy((LPTR)&lpSrc[WHOLE(offset)], (LPTR)lpDst, count);
	}
}

/************************************************************************/

void Sample_8MW( LPTR input, int inskip, LPTR output, int outskip,
                     int count, LFIXED rate, LPTR mask )
{
	LFIXED offset;
	LPTR   lpSrc = (LPTR)input;
	LPTR   lpDst = (LPTR)output;

	// Skip inskip pixels at start
	lpSrc += inskip;

	// Skip outskip pixels on output
	offset  = FDIV2(rate);
	offset += ( rate * outskip );

	// only process the number of pixels left
	count -= outskip;

	// Begin processing data
	while( --count >= 0 )
	{
		if ( *mask++ )
		{
			*lpDst++ = lpSrc[WHOLE(offset)];
		}
		else
		{
			lpDst++;
		}
		offset += rate;
	}
}

/************************************************************************/

void Sample_24( LPTR input, int inskip, LPTR output, int outskip,
                     int count, LFIXED rate )
{
	LFIXED offset;
	LPRGB  lpSrc = (LPRGB)input;
	LPRGB  lpDst = (LPRGB)output;

	// Skip inskip pixels at start
	lpSrc += inskip;

	// Skip outskip pixels on output
	offset  = FDIV2(rate);
	offset += ( rate * outskip );

	// only process the number of pixels left
	count -= outskip;

	if (rate != 1)
	{
		// Begin processing data
		while( --count >= 0 )
		{
			*lpDst++  = lpSrc[WHOLE(offset)];
			offset   += rate;
		}
	}
	else
	{
		copy((LPTR)&lpSrc[WHOLE(offset)], (LPTR)lpDst, count*3);
	}
}

/************************************************************************/

void Sample_24MW( LPTR input, int inskip, LPTR output, int outskip,
                     int count, LFIXED rate, LPTR mask )
{
	LFIXED offset;
	LPRGB  lpSrc = (LPRGB)input;
	LPRGB  lpDst = (LPRGB)output;

	// Skip inskip pixels at start
	lpSrc += inskip;

	// Skip outskip pixels on output
	offset  = FDIV2(rate);
	offset += ( rate * outskip );

	// only process the number of pixels left
	count -= outskip;

	// Begin processing data
	while( --count >= 0 )
	{
		if ( *mask++ )
		{
			*lpDst++ = lpSrc[WHOLE(offset)];
		}
		else
		{
			lpDst++;
		}
		offset += rate;
	}
}

#endif // C_CODE

/************************************************************************/

void Sample_32( LPTR input, int inskip, LPTR output, int outskip,
                     int count, LFIXED rate )
{
	LFIXED offset;
	LPLONG lpSrc = (LPLONG)input;
	LPLONG lpDst = (LPLONG)output;

	// Skip inskip pixels at start
	lpSrc += inskip;

	// Skip outskip pixels on output
	offset  = FDIV2(rate);
	offset += ( rate * outskip );

	// only process the number of pixels left
	count -= outskip;

	if (rate != 1)
	{
		// Begin processing data
		while( --count >= 0 )
		{
			*lpDst++  = lpSrc[WHOLE(offset)];
			offset   += rate;
		}
	}
	else
	{
		copy((LPTR)&lpSrc[WHOLE(offset)], (LPTR)lpDst, count*4);
	}
}

/************************************************************************/

void Sample_32MW( LPTR input, int inskip, LPTR output, int outskip,
                     int count, LFIXED rate, LPTR mask )
{
	LFIXED offset;
	LPLONG lpSrc = (LPLONG)input;
	LPLONG lpDst = (LPLONG)output;

	// Skip inskip pixels at start
	lpSrc += inskip;

	// Skip outskip pixels on output
	offset  = FDIV2(rate);
	offset += ( rate * outskip );

	// only process the number of pixels left
	count -= outskip;

	// Begin processing data
	while( --count >= 0 )
	{
		if ( *mask++ )
		{
			*lpDst++ = lpSrc[WHOLE(offset)];
		}
		else
		{
			lpDst++;
		}
		offset += rate;
	}
}


