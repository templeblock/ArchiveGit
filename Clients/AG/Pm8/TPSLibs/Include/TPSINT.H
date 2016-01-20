/******************************************************************************

	Summary:	TPS library integer utilities.

	Author:	Bruce D. Rosenblum
			Turning Point Software
			13 Feb 95

	Client:	Turning Point Software
			One Gateway Center, Suite 800
			Newton, MA 02158
			(617) 332-0202

	Copyright (C) 1995, Turning Point Software. All Rights Reserved.
	
	This file contains a variety of utilities for manipulating shorts, longs,
	and ints.
	
	Notes: 
	
	1)	There are no functions for swapping the bytes of an integer since
	an integer field should never be written to a file under TPS guidelines
	because the sizeof integer is dependent on the specific compiler.
	
	2)	Put this code in a fixed segment on the Macintosh
	
	3)	tintStartup initialized the Random number seed

******************************************************************************/

#ifndef _tpsint_h
#define _tpsint_h

#ifndef	_tpsdefs_h
#include "tpsdefs.h"
#endif	// _tpstypes_h


#define	tintMin(i1, i2)	((i1) < (i2) ? (i1) : (i2))
#define	tintMax(i1, i2)	((i1) > (i2) ? (i1) : (i2))
#define	tintAbs(i)		((i) < 0 ? -(i) : (i))
#define	tintNeg(i)		(-(i))

//
//		Macros and useful defines
#define	tintLOBYTE(w)		((uBYTE)(w))
#define	tintLOWORD(l)		((uWORD)(l))

#if	defined( MAC )

#define	tintHIBYTE(w)		((uBYTE) *(((uBYTE*)&(w))) )
#define	tintHIWORD(l)		((uWORD) *(((uWORD*)&(l))) )

#elif	defined( _WINDOWS ) || defined( DOS )

#define	tintHIBYTE(w)		((uBYTE) (((uBYTE*)&(w)))[1] )
#define	tintHIWORD(l)		((uWORD) (((uWORD*)&(l)))[1] )

#endif


#define	tintMAKEWORD(l,h)	((uWORD)( ((uBYTE)(l)) | (((uWORD)(h)<<8)) ))
#define	tintMAKELONG(l,h)	((uLONG)( ((uWORD)(l)) | (((uLONG)(h)<<16)) ))


#define	tintMAXSWAPBITS	32

/***  prototypes  ***/
#ifdef __cplusplus
extern "C" {
#endif

TPSAPI_DECLP( BOOLEAN )	tintStartup( VOID );
TPSAPI_DECLP( VOID )		tintShutdown( VOID );

TPSAPI_DECLP( uWORD )	tintRandom(uWORD uLimit);
TPSAPI_DECLP( sWORD )	tintMulDiv(sWORD sMul1, sWORD sMul2, sWORD sDiv);
TPSAPI_DECLP( uWORD )	tintUMulDiv(uWORD uMul1, uWORD uMul2, uWORD uDiv);

TPSAPI_DECLP( uWORD )	tintSwapWord(uWORD us);
TPSAPI_DECLP( uLONG )	tintSwapLong(uLONG ul);
TPSAPI_DECLP( VOID )		tintSwapWords(LPUWORD lpuw, uWORD uwCnt);
TPSAPI_DECLP( VOID )		tintSwapLongs(LPULONG lpul, uWORD uwCnt);

TPSAPI_DECLP( float )	tintSwapFloat(float f);
TPSAPI_DECLP( double )	tintSwapDouble(double d);
TPSAPI_DECLP( VOID )		tintSwapFloats(float FAR* lpf, uWORD uwCnt);
TPSAPI_DECLP( VOID )		tintSwapDoubles(double FAR* lpd, uWORD uwCnt);


/*
tintSwapStructs(LPVOID lpv, uWORD uwSize, uWORD uwCnt, uLONG ulSwapBits );

Swap the given array of structures using the given structure encoding.
The structures are all assumed to be uwSize bytes long...
The function swaps uwCnt structures starting at lpv.
Swap information is encoded into a sequence of 2 bit values where the
low 2 bits of the swap code correspond to the first bytes in the
structure.  Code values are interpreted as follows:
	00 -- skip the next two bytes (raw byte data)
	01 -- swap the next two bytes (word data)
	10 -- swap the next four bytes (long data)
	11 -- reserved at this time.  (we may want to extend this API to
	      deal with other size items, such as floating point numbers...)

A sample structure and it's corresponding swap encoding follows...

	typedef struct tagfoobar
	{
		sBYTE	foo1;		// 00
		sBYTE	foo2;		// (included with previous code)
		sWORD	foo3;		// 01
		sLONG	foo4;		// 10
		sWORD	foo5;		// 01
		sLONG	foo6;		// 10
		sWORD	foo7;		// 01
		sBYTE	foo8;		// 00
		sBYTE	foo9;		// (included with previous code)
		sLONG	last;		// 10
	} foobar;
	#define foobar_SWAPBITS	0x8664L

If there are portions of the structure that require swapping
that are beyond the range of the long swap encoding (e.g. a
structure with a 30 character string followed by 8 longs), the
swapping can be split into multiple passes as follows...

using the given example:

	typedef struct tagfoostr
	{
		uBYTE	str[30];	// 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
		sLONG	sl1;		// 10
		sLONG	sl2;		// 10
		sLONG	sl3;     // 10
		sLONG	sl4;     // 10
		sLONG	sl5;     // 10
		sLONG	sl6;     // 10
		sLONG	sl7;     // 10
		sLONG	sl8;     // 10
	} foostr;

the complete encoding string would be:  0x2AAA8 0000000L
It is obvious (to anyone who cares to count digits) that this is
too large to fit into a long, so we need to split it up into
multiple passes.

There are no restrictions on how this is split up, so we choose the
easiest method: combine the 0's into a skip val, followed by the
encoding for the remainder of the structure, as follows...
	#define	foostr_SWAPBITS0	0x0000000L
	#define	foostr_SWAPOFFS0	0
	#define	foostr_SWAPBITS1	0x2AAA8L
	#define	foostr_SWAPOFFS1	(7*4)	// each 2bit code corresponds (in
												// this case) to 2 sequential bytes

	The swap sequence is as follows:
	
	foostr	astruct;
	tintSwapStructs( (LPVOID)(((LPSTR)&astruct)+foostr_SWAPOFFS0),
							sizeof(astruct), 1, foostr_SWAPBITS0 );
	tintSwapStructs( (LPVOID)(((LPSTR)&astruct)+foostr_SWAPOFFS1),
							sizeof(astruct), 1, foostr_SWAPBITS1 );

Note that in this example, since the first swap code contains all
zeros, it is equally valid while also being more optimal to skip
the first SwapStructs call altogether, i.e.

	tintSwapStructs( (LPVOID)(((LPSTR)&astruct)+foostr_SWAPOFFS1),
							sizeof(astruct), 1, foostr_SWAPBITS1 );

is equivilent to the previous example, because the first call
to SwapStructs doesn't actually do anything...

We leave it to you to find the optimal sequences...
*/
TPSAPI_DECLP( VOID )		tintSwapStructs(LPVOID lpv, uWORD uwSize, uWORD uwCnt, uLONG ulSwapBits );


#ifdef __cplusplus
}
#endif

#endif	/* _tpsint_h */

