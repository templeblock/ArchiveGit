/******************************************************************************

   Module Name: tpsint.c
   Description: TPS Integer Utility Manager module.
   Version:     v0.01
   Author:      Michael D. Houle
   Date:        22 February 1995
   Copyright:   (c) Turning Point Software, Inc., 1995.  All Rights Reserved.

   Revision History:

   Date     Who      What
   ----     ---      ----
   2/22/95   MDH      Initial version.

   Notes:

******************************************************************************/

//
// INCLUDE FILES
#include "tpsint.h"

#if	defined( _WINDOWS ) || defined( DOS )
#include <stdlib.h>	//	for rand and srand

#endif


ASSERTNAME

//
// SYMBOLIC CONSTANTS


//
// TYPEDEFS


//
// MACROS

#define	MAX_SWAP_STRUCT_SIZE		32


//
// GLOBAL VARIABLES


sLONG		_tpsIntStartupCount = 0;


//
//	LOCAL FUNCTIONS


//
// FUNCTION PROTOYPES
/*
		tintStartup

			Initialize the Integer Utility Functions Library

			Will internally seed the random value function

		OnEntry:
		OnExit:
			Nothing

		Returns:
			TRUE if sucessful
*/
TPSAPI_DECLP( BOOLEAN )	tintStartup( VOID )
{
	// Only do if the startup count is 0
	if( !_tpsIntStartupCount )
	{
		//	Seed the random number generator
	#if defined( MAC )
		GetDateTime((unsigned long*)&qd.randSeed);
	#elif defined( _WINDOWS )
		srand((int)GetTickCount());
	#elif defined( DOS )
		srand((int)1);		//	not very random huh..
	#endif	//	defined( MAC or _WINDOWS or DOS )
	}

	// Increment the startup count
	_tpsIntStartupCount++;

	return TRUE;
}


/*
		tintShutdown

			Shutdown the Integer Utility library

		OnEntry:
		OnExit:
		Returns:
			Nothing
*/
TPSAPI_DECLP( VOID )		tintShutdown( VOID )
{
	TpsAssert( _tpsIntStartupCount > 0, "Integer Library not yet started...");
	_tpsIntStartupCount--;
}


/*
		tintRandom

				Return a random number in the range of 0<=x<uLimit

		OnEntry:
			uLimit	- the number that no random value will be 
							greater than or equal to...

		OnExit:
			Nothing

		Returns:
			a random number (x) such that  0 <= x < uLimit
*/
TPSAPI_DECLP( uWORD )	tintRandom(uWORD uLimit)
{
	int		iRet;

	TpsAssert( _tpsIntStartupCount > 0, "The Integer Utility Functions have not been initialized");

#if	defined( MAC )
	iRet	= Random();
#elif	defined( _WINDOWS )
	iRet	= rand();
#elif	defined( DOS )
	iRet	= rand();
#endif	//	defined( MAC or _WINDOWS or DOS )

	iRet	= iRet % uLimit;
	return (uWORD)iRet;
}


/*
		tintMulDiv

			Return the computation of sMul1 * sMul2 / sDiv.
			However, use long values to guard against roundoff...

		OnEntry:
			sMul1		- the original number
			sMul2		- the Multiplier
			sDiv		- the Divisor

		OnExit:
			Nothing

		Returns:
			the value
*/
TPSAPI_DECLP( sWORD )	tintMulDiv(sWORD sMul1, sWORD sMul2, sWORD sDiv)
{
	//	Always perform multiplcation before division to guard against roundoff
	sLONG	slVal	= (sLONG)sMul1 * (sLONG)sMul2;
	slVal	= slVal / (sLONG)sDiv;
	return (sWORD)slVal;
}

/*
		tintUMulDiv

			Return the computation of sMul1 * sMul2 / sDiv.
			However, use long values to guard against roundoff...

		OnEntry:
			sMul1		- the original number
			sMul2		- the Multiplier
			sDiv		- the Divisor

		OnExit:
			Nothing

		Returns:
			the value
*/
TPSAPI_DECLP( uWORD )	tintUMulDiv(uWORD uMul1, uWORD uMul2, uWORD uDiv)
{
	//	Always perform multiplcation before division to guard against roundoff
	uLONG	ulVal	= (uLONG)uMul1 * (uLONG)uMul2;
	ulVal	= ulVal / (uLONG)uDiv;
	return (uWORD)ulVal;
}


/*
		tintSwapWord

			Swap the lo/hi bytes of the given value

		OnEntry:
			uw			- the value to swap
		OnExit:
			Nothing
		returns:
			the Swapped value
*/
TPSAPI_DECLP( uWORD )	tintSwapWord(uWORD uw)
{
	if( !uw ) return uw;
	return tintMAKEWORD( tintHIBYTE(uw), tintLOBYTE(uw) );
}


/*
		tintSwapLong

			Swap the lo/hi words of the given value

		OnEntry:
			uw			- the value to swap
		OnExit:
			Nothing
		returns:
			the Swapped value
*/
TPSAPI_DECLP( uLONG )	tintSwapLong(uLONG ul)
{
	if( !ul ) return ul;
	return tintMAKELONG( tintSwapWord(tintHIWORD(ul)), tintSwapWord(tintLOWORD(ul)) );
}


/*
		tintSwapWords

			Swap the lo/hi bytes of the given array

		OnEntry:
			uw			- the value to swap
		OnExit:
			Nothing
		returns:
			the Swapped value
*/
TPSAPI_DECLP( VOID )		tintSwapWords(LPUWORD lpuw, register uWORD uwCnt)
{
	while (uwCnt-- > 0)
	{
		*lpuw	= tintSwapWord( *lpuw );
		++lpuw;
	}
}


/*
		tintSwapLongs

			Swap the lo/hi words of the given array

		OnEntry:
			uw			- the value to swap
		OnExit:
			Nothing
		returns:
			the Swapped value
*/
TPSAPI_DECLP( VOID )		tintSwapLongs(LPULONG lpul, uWORD uwCnt)
{
	while (uwCnt-- > 0)
	{
		*lpul	= tintSwapLong( *lpul );
		++lpul;
	}
}


/*
		tintSwapFloat

			Swap the float as if it was a long

		OnEntry:
			f			- the value to swap
		OnExit:
			Nothing
		returns:
			the Swapped value
*/
TPSAPI_DECLP( float )	tintSwapFloat( float f )
{
	float		fdst;
	long*		pldst;

	if( f == 0.0 )
		return f;

	TpsAssert( sizeof(float)==4, "The size of this float is not 4, this code will not work" );
//	UntestedCode();	//	not tested yet...

	pldst	= (long*)&fdst;

	//	Swap low 4-bits as a long into the high 4-bits
	*pldst	= tintSwapLong( *(LPULONG)&f );
	return fdst;
}


/*
		tintSwapDouble

			Swap the high 4-bits as a long with the low 4-bits as a long.

		OnEntry:
			d			- the value to swap
		OnExit:
			Nothing
		returns:
			the Swapped value
*/
TPSAPI_DECLP( double )	tintSwapDouble( double d )
{
	double	ddst;
	LPULONG	pldst;
	LPULONG	plsrc;

	if( d == 0.0 )
		return d;

	TpsAssert( sizeof(double)==8, "The size of this double is not 8, this code will not work" );
	UntestedCode();	//	not tested yet...

	plsrc	= (LPULONG)&d;
	pldst	= (LPULONG)&ddst;

	//	Swap low 4-bits as a long into the high 4-bits
	pldst[0]	= tintSwapLong( plsrc[1] );
	pldst[1]	= tintSwapLong( plsrc[0] );
	return ddst;
}


/*
		tintSwapFloats

			Swap the float values of the given array

		OnEntry:
			lpf			- the array to swap
			uwCnt			- the number of entries in the array
		OnExit:
			Nothing
		returns:
			Nothing
*/
TPSAPI_DECLP( VOID )		tintSwapFloats( float FAR* lpf, register uWORD uwCnt )
{
	while (uwCnt-- > 0)
	{
		*lpf	= tintSwapFloat( *lpf );
		++lpf;
	}
}


/*
		tintSwapDoubles

			Swap the double values of the given array

		OnEntry:
			lpd		- the array to swap
			uwCnt		- the number of values in the array
		OnExit:
			Nothing
		returns:
			Nothing
*/
TPSAPI_DECLP( VOID )		tintSwapDoubles( double FAR* lpd, uWORD uwCnt )
{
	while (uwCnt-- > 0)
	{
		*lpd	= tintSwapDouble( *lpd );
		++lpd;
	}
}


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
TPSAPI_DECLP( VOID )		tintSwapStructs(LPVOID lpv, uWORD uwSize, uWORD uwCnt, uLONG ulSwapBits )
{
	register uLONG			ulTempBits;
	register LPSBYTE		lpDst;			//	destination (verify it does not cross a segment)
				HPSBYTE		hpv;

	hpv	= (HPSBYTE) lpv;
	while (uwCnt-- > 0)
	{
		//	get a far pointer so that the internal pointer adjustments will NOT
		//	be done with HUGE mathematics...
		lpDst	= (LPSBYTE)hpv;			//	get starting point and assert it will be valid for the structure
		TpsAssert( (lpDst < lpDst+uwSize), "Structure crosses a Segment boundry");

		ulTempBits	= ulSwapBits;
		while (ulTempBits != 0)
		{
			//	Switch on the low 2 bits of the long
			//		to determine the swap pattern
			switch (ulTempBits & 0x03L)
			{
				case 1:
					*(uWORD*)lpDst	= tintSwapWord( *((uWORD*)lpDst) );
						//	allow fall through
				case 0:
					lpDst				+= sizeof(uWORD);	//	adjust destination for size swaped (or ignored)
					ulTempBits		>>= 2;				//	shift bit fields
					break;

				case 2:
					*(PULONG)lpDst	= tintSwapLong( *((PULONG)lpDst) );
					lpDst				+= sizeof(uLONG);	//	adjust destination for size swapped
					ulTempBits		>>= 2;				//	shift bit fields
					break;

				case 3:
					TpsAssertAlways("Invalid SwapBit Code found in ulSwapBits - 0x3");
					ulTempBits		= 0;	//	end the loop NOW..
					break;
			}
		}

		//
		//		Adjust by full size of struct using HUGE pointer to 
		//	make sure segment crossing is correct on Windows
		hpv	+= uwSize;
	}
}
