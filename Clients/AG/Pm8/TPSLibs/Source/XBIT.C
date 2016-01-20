/* ===========================================================================
   Module   :   XBIT.c


   Abstract :   Cross Platform Bitmap Routines for DIB formatted files
   Status   :   Under Development
  
   Copyright Ñ (c) 1995, Turning Point Software.  All Rights Reserved.
  
   Modification History :
		10-January-95	E. VanHelene	Initial revision.
  
   Note:
		These routines were originally coded in macbit.c.  They are being
		moved to allow greater flexibility amoungst platforms.

   To Do:

   =========================================================================== */

#include "TPSBIT.H"		//	includes tpstypes....

ASSERTNAME

//
// If there are any problems with accessing longs, such as boundary
// or alignment conditions, then the code wrapped in #if CAN_COPY_DWORDS
// blocks must be adjusted to account for those cases.  Otherwise we must
// leave CAN_COPY_DWORDS set to FALSE...
//
// On the 68000, DWORDS must start on an even address.  Note that this means both
// source and destination!!!
//
// On x86 machines, DWORDS cannot cross a 64k boundary...
//
// On all machines, it is likely that the optimal usages is DWORD aligned
// blocks, but it is not likely that getting to that state would be
// simple...

//
//		On Windows and for a 68000 build (not 68030 or above) builds,
//			do not move in DWORD.
#if	defined( BUILD_68K )
#define	CAN_COPY_DWORDS	FALSE
#else	
#define	CAN_COPY_DWORDS	TRUE
#endif

//
// Local support...
static void	NEAR		tbitencode( HTBIT htbmp, HPSBYTE hpDst, BOOLEAN bTrans, uBYTE ubTransColor, PULONG pulDstLen );
static void NEAR		tbitdecode( HTBIT htbmp, HPSBYTE hpSrc, BOOLEAN bTrans, uBYTE ubTransColor );
static BOOLEAN NEAR	tbithittest( LPTPT lppt, HPSBYTE hpSrc, LPTRECT lptrcSrc, BOOLEAN bTrans, uBYTE ubTransColor );
static uLONG NEAR		rle_pack( HPSBYTE* srcPtr, HPSBYTE* dstPtr, register uLONG srcBytes );
static uLONG NEAR		rle_packtransparent( HPSBYTE* ppSrc, HPSBYTE* ppDst, register uLONG ulSrcLen, register uBYTE ubTrans );
static BOOLEAN NEAR	rle_hittest( HPSBYTE* ppSrc, uWORD uwPoint, uLONG ulDstLen, BOOLEAN bTrans, uBYTE ubTrans );

static void NEAR	rle_unpack( HPSBYTE* srcPtr, HPSBYTE* dstPtr, register uLONG originalLength );

static void NEAR	rle_unpacktransparent( HPSBYTE pOrigSrc, HPSBYTE pOrigDst, uWORD uwDstOfs, uLONG ulDstLen, register uBYTE ubTrans, register int iHgt, uLONG ulDstLineAdj, PULONG puLineTbl );

/*--------------------------------------------------------------------------
 * Function: tbitCompress
 * Expects:  HTBIT       htbmp
 *           LPVOID      lpDst
 *           PULONG      pulDstLen
 * Returns:  nothing
 * Purpose:  Compress the data in the input bitmap, storing the compressed
 *				 data beginning at the pointer pointed to by "dst". The output argument "len"
 *				 will contain the compressed length of the bitmap.
 *				 This routine uses the local utility "rle_pack" to compress the data a row at a time.
 *				 packed data is preceeded by a line table uLONG[ bmpheight+1 ]
 * History:  02/09/95  MGD - Initial Revision
 *--------------------------------------------------------------------------
 */
TPSAPI_DECLP(void) tbitCompress( HTBIT htbmp, LPVOID lpDst, PULONG pulDstLen )
{
	tbitencode( htbmp, (HPSBYTE)lpDst, FALSE, 0, pulDstLen );
}


/*--------------------------------------------------------------------------
 * Function: tbitCompressTransparent
 * Expects:  HTBIT       htbmp
 *           LPVOID      lpDst
 *           uBYTE       ubTransColor
 *           PULONG      pulDstLen
 * Returns:  nothing
 * Purpose:  Compress the data in the input bitmap, storing the compressed
 *				 data beginning at the pointer pointed to by "dst". The output argument "len"
 *				 will contain the compressed length of the bitmap.
 *				 This routine uses the local utility "rle_pack" to compress the data a row at a time.
 *				 packed data is preceeded by a line table uLONG[ bmpheight+1 ]
 *           the transparent color is forced to always be in a packed run...
 * History:  02/09/95  MGD - Initial Revision
 *--------------------------------------------------------------------------
 */
TPSAPI_DECLP(void) tbitCompressTransparent( HTBIT htbmp, LPVOID lpDst, uBYTE ubTransColor, PULONG pulDstLen )
{
	tbitencode( htbmp, (HPSBYTE)lpDst, TRUE, ubTransColor, pulDstLen );
}


/*--------------------------------------------------------------------------
 * Function: tbitUncompress
 * Expects:  HTBIT       htbmp
 *           LPVOID      lpSrc
 * Returns:  nothing
 * Purpose:  Expand the data pointed to by "lpSrc" into the input bitmap.
 *				 This routine uses the local utility "rle_unpack" to expand the
 *           data one row at a time.
 * History:  02/09/95  MGD - Initial Revision
 *--------------------------------------------------------------------------
 */
TPSAPI_DECLP(void) tbitUncompress( HTBIT htbmp, LPVOID lpSrc )
{
	tbitdecode( htbmp, (HPSBYTE)lpSrc, FALSE, 0 );
}


/*--------------------------------------------------------------------------
 * Function: tbitUncompressTransparent
 * Expects:  HTBIT       htbmp
 *           LPVOID      lpSrc
 *           uBYTE       ubTransColor
 * Returns:  nothing
 * Purpose:  Expand the data pointed to by "lpSrc" into the input bitmap.
 *				 This routine uses the local utility "rle_unpack" to expand the
 *           data one row at a time.  bytes in the source stream that
 *           are transparent are NOT copied to the dst...
 * History:  02/09/95  MGD - Initial Revision
 *--------------------------------------------------------------------------
 */
TPSAPI_DECLP(void) tbitUncompressTransparent( HTBIT htbmp, LPVOID lpSrc, uBYTE ubTransColor )
{
	tbitdecode( htbmp, (HPSBYTE)lpSrc, TRUE, ubTransColor );
}


/*--------------------------------------------------------------------------
 * Function: tbitUncompressHitTest
 * Expects:  TPT         pt
 *           LPVOID      lpSrc
 *           LPTRECT     lptrcSrc
 * Returns:  BOOLEAN bHit
 * Purpose:  
 * History:  02/16/95  MGD - Initial Revision
 *--------------------------------------------------------------------------
 */
TPSAPI_DECLP(BOOLEAN) tbitUncompressHitTest( LPTPT lppt, LPVOID lpSrc, LPTRECT lptrcSrc )
{
	return tbithittest( lppt, (HPSBYTE)lpSrc, lptrcSrc, FALSE, 0 );
}


/*--------------------------------------------------------------------------
 * Function: tbitUncompressTransHitTest
 * Expects:  TPT         pt
 *           LPVOID      lpSrc
 *           LPTRECT     lptrcSrc
 *           uBYTE       ubTransColor
 * Returns:  BOOLEAN bHit
 * Purpose:  
 * History:  02/16/95  MGD - Initial Revision
 *--------------------------------------------------------------------------
 */
TPSAPI_DECLP(BOOLEAN) tbitUncompressTransHitTest( LPTPT lppt, LPVOID lpSrc, LPTRECT lptrcSrc, uBYTE ubTransColor )
{
	return tbithittest( lppt, (HPSBYTE)lpSrc, lptrcSrc, TRUE, ubTransColor );
}


/*--------------------------------------------------------------------------
 * Function: tbitUncompressTransToBitmap
 * Expects:  HTBIT       htbmpDst      bitmap to uncompress into
 *           LPTRECT     lptrcDst		rect within dst to uncompress into
 *           LPVOID      lpSrc			=> compressed tbit
 *           LPTRECT     lptrcSrc		=> actual location and size of tbit
 *           uBYTE       ubTranColor	index of transparent color in src
 * Returns:  BOOLEAN bIntersects
 * Purpose:  
 * History:  02/10/95  MGD - Initial Revision
 *--------------------------------------------------------------------------
 */
TPSAPI_DECLP(BOOLEAN) tbitUncompressTransToBitmap( HTBIT htbmpDst, LPTRECT lprcDst, LPVOID lpSrc, LPTRECT lprcSrc, uBYTE ubTranColor )
{
	uLONG		ulRowBytes;
	register	HPULONG	hpulLineTbl;
	register HPSBYTE	hpsbDst;
	register	HPSBYTE	hpsbSrc;
	TRECT		trcVis;
	TRECT		trcDst;
	TRECT		trcTmp;
	register int 	swWid;
	register int	uwHgt;
	uWORD		uwSrcOfs;
	uWORD		uwDstW;

	//
	// sanity checks...
	TpsAssert( htbmpDst!=NULL,					"tbitUncompressTransToBitmap: NULL Destination!" );
	TpsAssert( lprcDst!=NULL,					"tbitUncompressTransToBitmap: NULL Destination Rect!" );
	TpsAssert( lpSrc!=NULL,						"tbitUncompressTransToBitmap: NULL Source!" );
	TpsAssert( lprcSrc!=NULL,					"tbitUncompressTransToBitmap: NULL Source Rect!" );
	TpsAssert( tbitGetDepth(htbmpDst)==8,	"tbitUncompressTransToBitmap: Detination bitmap must be 8 bit!" );

	tbitGetRect( htbmpDst, &trcDst );
	uwDstW	= (uWORD)tbitGetBytesPerRow( htbmpDst );
	//
	// get visible portion of src.  this will make:
	//		trcVis.top-trcSrc.top	= 1st line in src to uncompress
	//		trcHeight(&trcVis)		= # lines from src to uncompress
	//		trcVis.left-trcSrc.left	= 1st byte in src line to uncompress
	// 	trcWidth(&trcVis)			= # bytes from src line to uncompress
	if( !trcIntersect(lprcDst,&trcDst,&trcTmp) )
		{
		//
		// dst rect is outside of dst bitmap!
		return FALSE;
		}
	if( !trcIntersect(lprcSrc,&trcTmp,&trcVis) )
		{
		//
		// no intersection between src and dst...
		return FALSE;
		}

	swWid	= trcWidth( &trcVis );
	uwHgt	= trcHeight( &trcVis );
	
	hpulLineTbl	= ((HPULONG)lpSrc) + (sWORD)(trcVis.top - lprcSrc->top);
	//
	// offsets in line table are from start of DATA, not start of BITMAP
	hpsbSrc		= (HPSBYTE)lpSrc;

	ulRowBytes	= (uLONG)uwDstW;
	hpsbDst		= ((HPSBYTE)tbitGetBaseAddr(htbmpDst)) + (((uLONG)trcVis.top) * ulRowBytes) + trcVis.left;
	uwSrcOfs		= (uWORD)(trcVis.left - lprcSrc->left);

#if 0
	++uwHgt;
	while( --uwHgt )
		{
register HPSBYTE	hptmps;
register HPSBYTE	hptmpd;
		hptmps = (hpsbSrc + *hpulLineTbl);
		hptmpd = hpsbDst;
		rle_unpacktransparent( hptmps, hptmpd, uwSrcOfs, swWid, ubTranColor, 1, 0, NULL );//uwHgt, ulRowBytes, hpulLineTbl );
		++hpulLineTbl;
		hpsbDst	+= ulRowBytes;
		}
#else
	rle_unpacktransparent( hpsbSrc, hpsbDst, uwSrcOfs, swWid, ubTranColor, uwHgt, ulRowBytes, hpulLineTbl );
#endif

	return TRUE;
}


/*--------------------------------------------------------------------------
 * Function: tbitencode
 * Expects:  HTBIT       htbmp
 *           HPSBYTE     hpDst
 *           BOOLEAN     bTrans
 *           uBYTE       ubTransColor
 *           PULONG      pulDstLen
 * Returns:  nothing
 * Purpose:  
 * History:  02/09/95  MGD - Initial Revision
 *--------------------------------------------------------------------------
 */
static void NEAR tbitencode( HTBIT htbmp, HPSBYTE hpDst, BOOLEAN bTrans, uBYTE ubTransColor, PULONG pulDstLen )
{
	int		iRows;
	uLONG		ulRowBytes;
	HPULONG	hpulLineTbl;
	TRECT		rcBounds;
	HPSBYTE	hpsbSrc;
	HPSBYTE	hpsbDst;

	TpsAssert(htbmp != NULL,				"tbitencode: NULL source bitmap" );
	TpsAssert(hpDst != NULL,				"tbitencode: NULL destination buffer" );
	TpsAssert(pulDstLen != NULL,			"tbitencode: NULL destination length" );
	TpsAssert(tbitGetDepth(htbmp)==8,	"tbitencode: source bitmap must be 8 bit" );

	//
	// get the size of the bitmap...
	tbitGetRect( htbmp, &rcBounds );
	//
	// get the size of a row of data...
	ulRowBytes	= (uLONG)tbitGetBytesPerRow( htbmp );
	//
	// get a pointer to the raw bitmap data...
	hpsbSrc		= (HPSBYTE)tbitGetBaseAddr(htbmp);
	//
	// get the number of rows in the bitmap
	iRows			= (rcBounds.bottom - rcBounds.top);
	//
	// use something with a size for dst!
	hpsbDst		= hpDst;
	//
	// alloc space for line table
	hpulLineTbl	= (HPULONG)hpsbDst;
	hpsbDst		+= (sizeof(uLONG) * (iRows+1));
	//
	// init packed length...
	//		initial packed length is size of lineStart information
	*pulDstLen	= (uLONG)((iRows+1) * sizeof(uLONG));
	
	if( bTrans )
		{
		while( iRows-- )
			{
			*hpulLineTbl	= *pulDstLen;
			++hpulLineTbl;
			(*pulDstLen)	+= rle_packtransparent( &hpsbSrc, &hpsbDst, ulRowBytes, ubTransColor );
			}
		}
	else
		{
		while( iRows-- )
			{
			*hpulLineTbl	= *pulDstLen;
			++hpulLineTbl;
			(*pulDstLen)	+= rle_pack( &hpsbSrc, &hpsbDst, ulRowBytes );
			}
		}
	//
	// round out the line table...
	*hpulLineTbl 	= *pulDstLen;

	//
	//		Add the size of the line table so the caller knows the EXACT size...
//	*pulDstLen		+= (uLONG)(sizeof(uLONG) * (rcBounds.bottom - rcBounds.top));
}


/*--------------------------------------------------------------------------
 * Function: tbitdecode
 * Expects:  HTBIT       htbmp
 *           HPSBYTE     hpSrc
 *           BOOLEAN     bTrans
 *           uBYTE       ubTransColor
 * Returns:  nothing
 * Purpose:  Expand the data pointed to by "lpSrc" into the input bitmap.
 *				 This routine uses the local utility "rle_unpack" to expand the
 *           data one row at a time.
 * History:  02/09/95  MGD - Initial Revision
 *--------------------------------------------------------------------------
 */
static void NEAR tbitdecode( HTBIT htbmp, HPSBYTE hpSrc, BOOLEAN bTrans, uBYTE ubTransColor )
{
	int		iRows;
	uLONG		ulRowBytes;
	TRECT		rcBounds;
	HPSBYTE	hpsbDst;
	HPSBYTE	hpsbSrc;
	
	TpsAssert( htbmp != NULL,				"tbitdecode: NULL destination bitmap" );
	TpsAssert( hpSrc != NULL,				"tbitdecode: NULL source buffer" );
	TpsAssert( tbitGetDepth(htbmp)==8,	"tbitdecode: destination bitmap must be 8 bit" );

	tbitGetRect(htbmp, &rcBounds);

	hpsbSrc		= hpSrc;
	hpsbDst		= (HPSBYTE)tbitGetBaseAddr(htbmp);
	ulRowBytes	= (uLONG)tbitGetBytesPerRow(htbmp);
	iRows			= (rcBounds.bottom - rcBounds.top);

	//
	// skip over linetable...
	hpsbSrc += (sizeof(uLONG)*(iRows+1));

	if( bTrans )
		{
//		while( iRows-- )
			rle_unpacktransparent( hpsbSrc, hpsbDst, 0, ulRowBytes, ubTransColor, iRows, 0, NULL );
		}
	else
		{
		while( iRows-- )
			rle_unpack( &hpsbSrc, &hpsbDst, ulRowBytes );
		}
}


/*--------------------------------------------------------------------------
 * Function: tbithittest
 * Expects:  LPTPT       lppt
 *           HPSBYTE     hpSrc
 *           LPTRECT     lptrcSrc
 *           BOOLEAN     bTrans
 *           uBYTE       ubTransColor
 * Returns:  BOOLEAN bHit
 * Purpose:  
 * History:  02/16/95  MGD - Initial Revision
 *--------------------------------------------------------------------------
 */
static BOOLEAN NEAR tbithittest( LPTPT lppt, HPSBYTE hpSrc, LPTRECT lptrcSrc, BOOLEAN bTrans, uBYTE ubTransColor )
{
	HPSBYTE	hpsbSrc;
	HPULONG	hpulStart;
	uWORD		uwRowBytes;
	
	TpsAssert( hpSrc != NULL,	"tbithittest: NULL source buffer" );

	hpsbSrc		= hpSrc;
	hpulStart	= (HPULONG)hpSrc;
	uwRowBytes	= (uWORD)trcWidth(lptrcSrc);

	//
	// First, see if it's within the source rectangle...
	if( !tptIntrc(lptrcSrc,*lppt) )
		{
		//
		// Nope, impossible to hit...
		return (BOOLEAN)FALSE;
		}

	//
	// oh well...
	// Guess we have to actually look...

	hpsbSrc += hpulStart[ lppt->y - lptrcSrc->top ];
	return rle_hittest( &hpsbSrc, (uWORD)(lppt->x-lptrcSrc->left), (uLONG)uwRowBytes, bTrans, ubTransColor );
}


#ifdef	MAC
#pragma mark -
#endif	//	MAC

//REVIEW	The Pack and the Unpack routines can be optimized to verify
//		that the values can move DWORD values at all times.  This means
//		that on non-68000 machines, moving longs and only stop if
//		the memory would cross a 64K segment (windows).

//	utility routines in support of tbitCompress() and tbitUncompress().

/*--------------------------------------------------------------------------
 * Function: rle_pack
 * Expects:  PSBYTE*     ppSrc
 *           PSBYTE*     ppDst
 *           uLONG       ulSrcLen
 * Returns:  uLONG ulDstLen
 * Purpose:  Runs of 3 or more matching bytes are grouped with a count byte, 
 *				 followed by the counted character. The counts are
 *				 0 based, i.e. 
 *				 -4 -3 -2 -1 0 1 2 3 4   count byte in compressed stream 
 *				   5  4  3  2 1 2 3 4 5   actual count
 *				  for the count bytes, negative numbers indicate packed data, positive and 0
 *					represent non-repeating data.
 *
 *				  for example, the string AAAAAAAAAABABABABABA
 *				  would be translated to  (in text representation) -9A9BABABABABA			
 *
 *				 For calculating the destination buffer size, use the macro CALCWORSTPACKCASE(x)
 *				 where x is source size in bytes.
 * History:  02/09/95  MGD - Initial Revision
 *--------------------------------------------------------------------------
 */
static uLONG NEAR rle_pack( HPSBYTE* ppSrc, HPSBYTE* ppDst, register uLONG ulSrcLen )
{
	uLONG		packLen;
	HPSBYTE	endSrcPtr;
	sBYTE		lastByte;
	sBYTE		count = 0;
	sBYTE		noPackCount =0;
	HPSBYTE	destCountPtr = NULL;
	BOOLEAN	flag = FALSE;
	register HPSBYTE _ppSrc = *ppSrc;
	register HPSBYTE _ppDst = *ppDst;

	endSrcPtr = _ppSrc + ulSrcLen;					// create sentinel pointer
	
	while(_ppSrc < endSrcPtr){
		lastByte = *_ppSrc++;
		
		if (!destCountPtr){
			destCountPtr = _ppDst++;				// set up pointer for destination count byte
			noPackCount = 0;
		}

		while(_ppSrc < endSrcPtr){
			if  (lastByte == *_ppSrc){ 			// count matches
				count--;
				_ppSrc++;
			} else 
				break;
			if (count == -127){						// exceeded count, go to pack
				break;
			}
		}
	
		if (count <= -2){							// PACK
			if (flag){								// coming from NOPACK, need to set up destCountPtr.
				flag = FALSE;
				destCountPtr = _ppDst++;			// set up pointer for destination count byte
				noPackCount = 0;	
			}
			*destCountPtr = count;					// put the count byte and the char
			*_ppDst++ = lastByte;
			destCountPtr = NULL;
		} else {
			flag = TRUE;
			_ppSrc += (sLONG)(count - 1);			// note, plus a neg num, i.e, back up
			
			TpsAssert(_ppSrc < endSrcPtr,"RLEPack ppSrc > endSrcPtr");
			TpsAssert(noPackCount != -1, "RLEPack: count == -1");

			*destCountPtr = noPackCount;			// set the byte count
			*_ppDst++ = *_ppSrc++;				// copy the data
		
			if (_ppSrc >= endSrcPtr)				// if end, leave
				goto LEnd;
			
			if (noPackCount == 127){				// if wrap, set destCountPtr NULL to reset
				destCountPtr = NULL;				
				flag = FALSE;							// reset is done, no need to do it on pack
			} else 
				noPackCount++;	
					
		} 
		count = 0;
	}


LEnd:
	*ppSrc	= _ppSrc;
	packLen	= (uLONG)(_ppDst - *ppDst);
	*ppDst	= _ppDst;
	return packLen;
}

/*--------------------------------------------------------------------------
 * Function: rle_packtransparent
 * Expects:  HPSBYTE*    ppSrc
 *           HPSBYTE*    ppDst
 *           uLONG       ulSrcLen
 *           uBYTE       ubTrans
 * Returns:  uLONG ulDestLen
 * Purpose:  see rle_pack.  the only exception is that bytes in the source
 *           that resolve to ubTrans are always forced into a packed run.
 * History:  02/09/95  MGD - Initial Revision
 *--------------------------------------------------------------------------
 */
static uLONG NEAR rle_packtransparent( HPSBYTE* ppSrc, HPSBYTE* ppDst, register uLONG ulSrcLen, register uBYTE ubTrans )
{
	uLONG					ulDstLen;
	HPSBYTE				pSrcEnd;
	uBYTE					ubLast;
	sBYTE					sbCnt			= 0;
	sBYTE					sbRawCnt		= 0;
	HPSBYTE				psbDstCnt	= NULL;
	BOOLEAN				bRawData		= FALSE;
	register HPSBYTE	pSrc			= *ppSrc;
	register HPSBYTE	pDst			= *ppDst;

	//
	// Create sentinel pointer
	pSrcEnd	= pSrc + ulSrcLen;

	//
	// Loop until all of source has been packed...
	while( pSrc < pSrcEnd )
		{
		//
		// get byte from src...
		ubLast = (uBYTE)*pSrc++;

		//
		// if we don't have a place for the count in the
		// destination buffer, allocate it now...
		if( !psbDstCnt )
			{
			psbDstCnt	= pDst++;
			sbRawCnt		= 0;
			}

		//
		// count duplicates...
		while( pSrc<pSrcEnd && ubLast==(uBYTE)*pSrc )
			{
			++pSrc;
			if( --sbCnt == -127 )
				{
				//
				// can only pack 128 occurrences...
				break;
				}
			}

		//
		// if we have 3 or more occurrences, or we're forcing a particular value
		// to be packed, and this is that value, then pack it...
		if( sbCnt <= -2 || ubLast==ubTrans )
			{
			//
			// we have a run...
			// if we were previously stuffing raw data, we need
			// to start a run at the current position.  Otherwise
			// we're already setup...
			if( bRawData )
				{
				bRawData		= FALSE;
				psbDstCnt	= pDst++;
				sbRawCnt		= 0;
				}
			//
			// store the run length
			*psbDstCnt	= sbCnt;
			//
			// store the repeated byte
			*pDst++	= (sBYTE)ubLast;
			//
			// reset for next run
			psbDstCnt	= NULL;
			}
		else
			{
			//
			// not enough duplicates to pack, so start a raw run...
			// flag that we're dealing with raw data...
			bRawData	= TRUE;
			//
			// back up to beginning of run.  remember that sbCnt is negative, and
			// its absolute value is 1 less than the run length...
			pSrc += (((sWORD)sbCnt) - 1);
			//
			// Sanity check...
			TpsAssert( pSrc < pSrcEnd, "RLEPack ppSrc > pSrcEnd" );
			TpsAssert( sbRawCnt != -1, "RLEPack: sbCnt == -1" );
			//
			// stuff the raw-run length
			*psbDstCnt	= sbRawCnt;
			//
			// copy one byte of the raw data...
			*pDst++	= *pSrc++;				// copy the data
			//
			// if we're about to overflow, then prepare for a new
			// run, otherwise count the raw byte and continue...
			if( sbRawCnt == 127 )
				{
				psbDstCnt	= NULL;
				bRawData		= FALSE;
				}
			else
				++sbRawCnt;
			}
		//
		// reset count for next run...
		sbCnt = 0;
		}
	//
	// done packing.  Store the ending pointers and return the length
	// of the packed data...
	ulDstLen		= (uLONG)(pDst - *ppDst);
	*ppSrc		= pSrc;
	*ppDst		= pDst;
	return ulDstLen;
}


/*--------------------------------------------------------------------------
 * Function: rle_unpack
 * Expects:  HPSBYTE*    ppSrc       => packed data pointer
 *           HPSBYTE*    ppDst       => unpack buffer pointer
 *           uLONG       ulDstLen    number of bytes unpacked bytes to keep
 * Returns:  nothing
 * Purpose:  
 * History:  02/09/95  MGD - Initial Revision
 *--------------------------------------------------------------------------
 */
static void NEAR rle_unpack(HPSBYTE *ppSrc,HPSBYTE *ppDst, register uLONG ulDstLen)
{
	register HPSBYTE	_ppSrc	= *ppSrc;
	register HPSBYTE	_ppDst	= *ppDst;
	HPSBYTE				dstEnd	= _ppDst + ulDstLen;
	register sWORD		count;

	TpsAssert(ulDstLen > 0,"RLEUnPack: bad destLen");
	
	while (_ppDst < dstEnd)		// while line not expanded
	{
		count = *_ppSrc++;			// get count for this section
		if (count < 0)				// if negative, then packed bytes
		{
			//
			// c must be UNSIGNED to prevent sign extension!!!
			register uBYTE c;
			c = *_ppSrc++;			// get byte pattern for this run

			// To ensure the best compatibility, we start by aligning the
			// destination to a uLONG boundary.  This allows us to then attempt
			// to stuff the bytes using a packed long...
			/*
			 * BLOCK
			 */
			{
			//
			// Adjust to DWORD boundary...
			sWORD	swAdjust;
			if( (swAdjust=(sWORD)((uLONG)_ppDst)&(sizeof(uLONG)-1)) != 0)
				{
				while( swAdjust++ < sizeof(uLONG) && count<=0 )
					{
					*_ppDst++ = c;
					++count;
					}
				}
			}
			/*
			 * END BLOCK
			 */
			if(count < -(sWORD)(sizeof(uLONG) * 2))	//	-8
			{						// at least two full longs to make it worth while
				register uLONG lwChar;
						 uLONG ulToStuff;

						((uBYTE*)&ulToStuff)[0] = c;		//	smear the character across all 4 bytes of the long
						((uBYTE*)&ulToStuff)[1] = c;		//		the use of ulToStuff is so the compilier will
						((uBYTE*)&ulToStuff)[2] = c;		//		have a better chance of believing me when I say I want
						((uBYTE*)&ulToStuff)[3] = c;		//		ulStuff to be a register (ulToStuff cannot be a register
						lwChar = ulToStuff;					//		since I am taking the address of it...

				while (count < -(sWORD)(sizeof(uLONG)))	//	-4
				{					// while we can, move longs, not bytes
					*(HPULONG)_ppDst = lwChar;
					_ppDst += sizeof(uLONG);
					count += sizeof(uLONG);
					TpsAssert(_ppDst <= dstEnd,"RLEUnpack: ppDst > destEnd");
				}

			}
			while(count <= 0)		// move remainder as bytes
			{
				*_ppDst++ = (sBYTE)c;
				++count;
				TpsAssert(_ppDst <= dstEnd,"RLEUnpack: ppDst > destEnd");
			}
		} 
		else						// unpacked bytes
		{
//	WARNING section will fail on 68000 - only use for non-68000 products
#if CAN_COPY_DWORDS
			while(count > sizeof(uLONG))
			{						// if 4 or more bytes, move longs
				*(HPULONG)_ppDst = *(HPULONG)_ppSrc;
				_ppSrc += sizeof(uLONG);
				_ppDst += sizeof(uLONG);
				count -= sizeof(uLONG);
				TpsAssert(_ppDst <= dstEnd,"RLEUnpack: ppDst > destEnd");
			}
#endif	//	CAN_COPY_DWORDS
			while(count >=0)		// move remainder as bytes
			{
				*_ppDst++ = *_ppSrc++;
				--count;
				TpsAssert(_ppDst <= dstEnd,"RLEUnpack: ppDst > destEnd");
			}
		}
	}
	// update pointers for return
	*ppSrc = _ppSrc;
	*ppDst = _ppDst;
}


/*--------------------------------------------------------------------------
 * Function: rle_unpacktransparent
 * Expects:  HPSBYTE*    ppSrc         => packed data pointer
 *           HPSBYTE*    ppDst         => unpack buffer pointer
 *           uWORD       uwDstOfs      first unpacked byte in src to keep
 *           uLONG       ulDstLen       number of bytes unpacked bytes to keep
 *           uBYTE       ubTrans       value of transparent byte
 *           uWORD       uwHgt         number of lines to unpack
 *           uLONG       ulDstLineAdj  Amount to adjust dest between lines (0 for no adjustment)
 *           PULONG      puLineTbl     Amount to adjust source between lines (NULL for no adjustment)
 * Returns:  nothing
 * Purpose:  
 * History:  02/09/95  MGD - Initial Revision
 *--------------------------------------------------------------------------
 */
static void NEAR rle_unpacktransparent( HPSBYTE pOrigSrc, HPSBYTE pOrigDst, uWORD uwOrigDstOfs, uLONG ulOrigDstLen, register uBYTE ubTrans, register int iHgt, uLONG ulDstLineAdj, PULONG puLineTbl )
{
	register HPSBYTE	pSrc			= pOrigSrc;
	register HPSBYTE	pDst			= pOrigDst;
	register HPSBYTE	pDstEnd;
	register uWORD		uwDstOfs;
	register uLONG		ulDstLen;
	register sWORD		swRunLength;

	//
	// Sanity check...
	TpsAssert(ulOrigDstLen > 0,"RLEUnPack: bad destLen");

	++iHgt;
	while (--iHgt)
		{
		//	Setup for a scanline pass
		ulDstLen	= ulOrigDstLen;
		uwDstOfs	= uwOrigDstOfs;
		pDstEnd		= pDst	+ ulDstLen;

		//	If there is a LineTbl adjustment, adjust the source here...
		if (puLineTbl)
			pSrc	= pOrigSrc + *puLineTbl++;

		//
		// Loop until we've unpacked the requested number of bytes...
		while( pDst < pDstEnd )
			{
			//
			// get length of run...
			swRunLength	= (sWORD)(*pSrc++);
			if( swRunLength<0  || (!swRunLength && (*pSrc)==ubTrans) )
				{
				//
				// it's a packed run...
				// grab the packed byte and fill...
				// the byte must be UNSIGNED to prevent sign extension!
				register uBYTE	ubVal	= (uBYTE)*pSrc++;
	
				// Compute the actual runlength.
				swRunLength = -(swRunLength-1);
	
				//
				// Allow for the dst offset...
				if( uwDstOfs )
					{
					//
					// we still need to skip some...
					if( swRunLength <= (sWORD)uwDstOfs )
						{
						//
						// run is shorter than or equal to the offset, so skip run and continue...
						uwDstOfs	-= swRunLength;
						continue;
						}
					//
					// shorten up the run...
					swRunLength -= (sWORD)uwDstOfs;
					uwDstOfs		= 0;
					}
	
				if( ubVal == ubTrans )
					{
					//
					// transparent, so just skip over the dst bytes...
					//
					// skip over swRunLength bytes in dest...
					pDst		+= swRunLength;
					ulDstLen	-= swRunLength;
					}// is transparent
				else
					{
					//
					// not transparent, so stuff the dst bytes...
					// make sure we don't go nutty on the dst!
					if( swRunLength > (sWORD)ulDstLen )
						swRunLength = (sWORD)ulDstLen;
					ulDstLen -= swRunLength;
					//
					// set the bits...
					//
					// In order to be independant of the other TPS libraries, we do
					// our own memset here...
					// To ensure the best compatibility, we start by aligning the
					// destination to a uLONG boundary.  This allows us to then attempt
					// to stuff the bytes using a packed long...
					/*
					 * BLOCK
					 */
					{
					//
					// Adjust to DWORD boundary...
					sWORD	swAdjust;
					if( (swAdjust=(sWORD)((uLONG)pDst)&(sizeof(uLONG)-1)) != 0)
						{
						while( swAdjust++ < sizeof(uLONG) && swRunLength>0 )
							{
							*pDst++ = ubVal;
							--swRunLength;
							}
						}
					}
					/*
					 * END BLOCK
					 */
					//
					// see if it's worthwhile to stuff dwords...  We only
					// do it if the run is at least as long as two dwords...
					if( swRunLength > (sWORD)(sizeof(uLONG)<<1) )
						{
								 uLONG ulToStuff;
						register uLONG ulStuff;

						((uBYTE*)&ulToStuff)[0] = ubVal;		//	smear the character across all 4 bytes of the long
						((uBYTE*)&ulToStuff)[1] = ubVal;		//		the use of ulToStuff is so the compilier will
						((uBYTE*)&ulToStuff)[2] = ubVal;		//		have a better chance of believing me when I say I want
						((uBYTE*)&ulToStuff)[3] = ubVal;		//		ulStuff to be a register (ulToStuff cannot be a register
						ulStuff = ulToStuff;					//		since I am taking the address of it...

						while( swRunLength > (sWORD)sizeof(uLONG) )
							{
							(*(HPULONG)pDst)	= ulStuff;
							pDst					+= sizeof(uLONG);
							swRunLength			-= sizeof(uLONG);
							//
							// sanity check...
							TpsAssert( pDst<=pDstEnd, "RLEUnpackTransparent: Destination Overflow" );
							}
						}
					//
					// stuff the remainder of the packed run...
					while( --swRunLength >= 0 )
						*pDst++ = ubVal;
					//
					// sanity check...
					TpsAssert( pDst<=pDstEnd, "RLEUnpackTransparent: Destination Overflow" );
					}// not transparent
				}// if packed run
			else
				{
				//
				// run of raw data...
	
				//
				//	Adjust to Real swRunLength;
				++swRunLength;
	
				//
				// Allow for the dst offset...
				if( uwDstOfs )
					{
					//
					// we still need to skip some...
					if( swRunLength <= (sWORD)uwDstOfs )
						{
						//
						// run is shorter than or equal to the offset, so skip run and continue...
						uwDstOfs	-= swRunLength;
						pSrc		+= swRunLength;
						continue;
						}
					//
					// shorten up the run...
					swRunLength -= (sWORD)uwDstOfs;
					pSrc			+= uwDstOfs;
					uwDstOfs		= 0;
					}
				//
				// make sure we don't go nutty on the dst...
				if( swRunLength > (sWORD)ulDstLen )
					swRunLength = (sWORD)ulDstLen;
				ulDstLen -= swRunLength;
	
				//
				// copy the bits...
				//
				// In order to be independant of the other TPS libraries, we do
				// our own memcpy here...
				// NOTE: the 68000 can't deal with longs starting on odd addresses,
				//       so unless someone wants to write the adjustment code, we'll
				//       bail the fastunpack for 68000's and fall through to the code
				//       handling the remainder...
				//  The author of the adjustment code should note that it's possible
				//  to have src and dst starting on different alignments, which could
				//  potentially preclude the adjustment altogether...
				
	#if CAN_COPY_DWORDS
	
	#if	defined( _WINDOWS )
				if( (((uWORD)((uLONG)pDst+swRunLength)) > ((uWORD)(uLONG)pDst))
					&& (((uWORD)((uLONG)pSrc+swRunLength)) > ((uWORD)(uLONG)pSrc)) )
				{
					register LPUBYTE	lpuSrc = (LPUBYTE)pSrc;
					register LPUBYTE	lpuDst = (LPUBYTE)pDst;
				#define	SRCPTR	lpuSrc
				#define	DSTPTR	lpuDst
	#else
				#define	SRCPTR	pSrc
				#define	DSTPTR	pDst
	#endif
					while( swRunLength >= (sWORD)(sizeof(uLONG)) )
						{
						*((LPULONG)DSTPTR) = *((LPULONG)SRCPTR);
						DSTPTR		+= sizeof(uLONG);
						SRCPTR		+= sizeof(uLONG);
						swRunLength	-= sizeof(uLONG);
						}
	#ifdef	_WINDOWS
					pSrc	= (HPSBYTE)lpuSrc;
					pDst	= (HPSBYTE)lpuDst;
				}
	#endif
				#undef	SRCPTR
				#undef	DSTPTR
	
	#endif // CAN_COPY_DWORDS
					//
					// Copy the remainder...
					while( --swRunLength >= 0 )
						*pDst++ = *pSrc++;
				//
				// sanity check...
				TpsAssert( pDst<=pDstEnd, "RLEUnpackTransparent: Destination Overflow" );
				}
			}

			//	If there is a dest adjustment, adjust it here.
			if ( ulDstLineAdj )
			{
				pOrigDst	+= ulDstLineAdj;
				pDst		= pOrigDst;
			}
		}
}


/*--------------------------------------------------------------------------
 * Function: rle_hittest
 * Expects:  HPSBYTE*    ppSrc       => packed data pointer for line
 *           uWORD       uwPoint     offset into line for test...
 *           uLONG       ulDstLen    length of unpacked line (bytes)
 *           BOOLEAN     bTrans      TRUE if respect transparency
 *           uBYTE       ubTrans     value of transparent byte
 * Returns:  BOOLEAN     bHit        TRUE if there's a hit...
 * Purpose:  
 * History:  02/09/95  MGD - Initial Revision
 *--------------------------------------------------------------------------
 */
static BOOLEAN NEAR rle_hittest( HPSBYTE* ppSrc, uWORD uwPoint, uLONG ulDstLen, BOOLEAN bTrans, uBYTE ubTrans )
{
	register HPSBYTE	pSrc			= *ppSrc;
	register uWORD		uwUnpacked	= 0;
	register sWORD		swRunLength;

	//
	// Sanity check...
	TpsAssert(ulDstLen > 0,"RLEUnPack: bad destLen");

	//
	// Impossible to hit if beyond end...
	if( (uLONG)uwPoint >= ulDstLen )
		return (BOOLEAN)FALSE;
	//
	// if not respecting transparency, then it's simple.  Just check to see if
	// the given point offset is within the given length...
	if( !bTrans )
		return (BOOLEAN)((uLONG)uwPoint < ulDstLen);
	//
	// Loop until we've hit or passed the test point...
	while( uwUnpacked <= uwPoint )
		{
		//
		// get length of run...
		swRunLength	= (sWORD)(*pSrc++);
		if( swRunLength<0  || (!swRunLength && bTrans && (*pSrc)==ubTrans) )
			{
			//
			// it's a packed run...
			// grab the packed byte and fill...
			// the byte must be UNSIGNED to prevent sign extension!
			register uBYTE	ubVal	= (uBYTE)*pSrc++;
			uwUnpacked += (uWORD)-(swRunLength-1);
			if( uwPoint < uwUnpacked )
				{
				//
				// within this run.
				// if the run is transparent and we respect that, then there's no
				// hit.  Otherwise there's a hit...
				if( bTrans && ubVal==ubTrans )
					return (BOOLEAN)FALSE;
				return (BOOLEAN)TRUE;
				}
			}// if packed run
		else
			{
			//
			// run of raw data...
			uwUnpacked += (uWORD)(swRunLength+1);
			if( uwPoint < uwUnpacked )
				return (BOOLEAN)TRUE;
			pSrc += (swRunLength+1);
			}
		}
	//
	// update the source pointer...
	*ppSrc = pSrc;
	//
	// no hit...
	return (BOOLEAN)FALSE;
}
