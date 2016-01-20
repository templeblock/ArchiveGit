/* ===========================================================================
   Module   :	tpsBit.h
   Project  :	TPS Tool Library
   Abstract :	Off-screen bitmap subsystem primitives.
   Status   :	Under Development
  
   Copyright © 1994, Turning Point Software.  All Rights Reserved.
  
   Modification History :
  	0.0.0	08-August-94		R. Hood		Initial revision.

   To Do:
  
  
   =========================================================================== */

#ifndef _TPSBIT_H_
#define _TPSBIT_H_

#ifndef	_tpsmem_h
#include	"tpsMem.h"
#endif	// _tpsmem_h

#ifndef	_tpsrect_h
#include	"tpsRect.h"
#endif	//	_tpsrect_h

/***  define TPS error/return codes  ***/
#define	TERR_TBITSECTION					8000
#define	TBIT_ERRBASE						1
#define	TBIT_ERRGENERIC					1
#define	TBIT_ERRBADPARAMETER				2
#define	TBIT_ERROUTOFPORTS				3
#define	TBIT_ERRTOOMANYBITMAPS			4
#define	TBIT_ERRNOTLOCKED					5
#define	TBIT_ERRBITMAPBUSY				6
#define	TBIT_ERRBADBITMAP					7

/***  define color codes  ***/
#ifdef MAC

enum
{
   TBIT_Black = 0,    
   TBIT_White,    
   TBIT_Red,
   TBIT_Green,
   TBIT_Blue
};

#endif // MAC

/***  define blit modes  ***/
enum
{
   TBIT_SrcCopy = 0,    
   TBIT_SrcOr,
   TBIT_SrcXor,
   TBIT_SrcBic,
   TBIT_NotSrcCopy,
   TBIT_NotSrcOr,
   TBIT_NotSrcXor,
   TBIT_NotSrcBic
};

/***  define edge-finding modes (for tbitFindEdge()) ***/
enum
{
	TBIT_kLeftEdge		= 0x01,
	TBIT_kRightEdge	= 0x02,
	TBIT_kTopEdge		= 0x04,
	TBIT_kBottomEdge	= 0x08,
	TBIT_kAllEdges 	= 0x0F		//	all of the above OR'ed together
};

#ifdef	MAC
//	useful background color constants
#define kBitBackBlack 0x00
#define kBitBackWhite 0xFF
#endif	// MAC

/***  define bitmap flags  ***/
#define  TBIT_ZeroInit               0x0001  
#define  TBIT_Virtual                0x0002  
#define  TBIT_Purgeable              0x0004  
#define  TBIT_LongAlignRows          0x0010
#define  TBIT_16ByteAlignRows        0x0020
#define  TBIT_DefaultColorTable      0x0040

#define	TBIT_InvalidPortID			-1

/***  type defs  ***/
typedef  HTMEM          HTBIT;                          // off-screen bitmap subsystem transparant handle

/***  prototypes  ***/
#ifdef __cplusplus
extern "C" {
#endif

TPSAPI_DECLP( VOID )		SetIdentityFlag (BOOLEAN bCreateIdentity); 
TPSAPI_DECLP( BOOLEAN )		tbitStartup (uWORD NrPorts, uWORD depth, HNATIVE hColor); 
TPSAPI_DECLP( VOID )			tbitShutdown (void);
TPSAPI_DECLP( BOOLEAN )		tbitStarted ( ); 
TPSAPI_DECLP( VOID )			tbitSetMainScreen (LPVOID screen);
TPSAPI_DECLP( VOID )			tbitSetTransparentColor (sWORD color);
TPSAPI_DECLP( HTBIT )		tbitAlloc (uWORD depth, LPTRECT lptrc, uWORD flags);
TPSAPI_DECLP( VOID )			tbitFree (HTBIT hTBit);
TPSAPI_DECLP( BOOLEAN )		tbitLock (HTBIT hTBit);
TPSAPI_DECLP( VOID )			tbitUnlock (HTBIT hTBit);
TPSAPI_DECLP( uWORD )		tbitGetBytesPerRow (HTBIT hTBit);
TPSAPI_DECLP( LPVOID )		tbitGetBaseAddr (HTBIT hTBit);
TPSAPI_DECLP( HNATIVE )		tbitGetImageHandle (HTBIT hTBit);
TPSAPI_DECLP( VOID )			tbitGetRect (HTBIT hTBit, LPTRECT lptrc);
TPSAPI_DECLP( uWORD )		tbitGetDepth (HTBIT hTBit);
TPSAPI_DECLP( VOID )			tbitGetDevice (HTBIT hTBit, LPVOID lpDevice);
TPSAPI_DECLP( BOOLEAN )		tbitBlitToBitmap (HTBIT hSrc, HTBIT hDest, LPTRECT lptrcSrc, LPTRECT lptrcDest, uWORD mode, BOOLEAN fTransparent);
TPSAPI_DECLP( BOOLEAN )		tbitBlitToDevice (HTBIT htbSrc, uLONG ulDst, LPTRECT lptrcSrc, LPTRECT lptrcDst, uWORD mode, BOOLEAN fTransparent);
TPSAPI_DECLP( BOOLEAN )		tbitBlitFromDevice (HTBIT hDest, uLONG ulSrc, LPTRECT lptrcSrc, LPTRECT lptrcDest, uWORD mode, BOOLEAN fTransparent);
TPSAPI_DECLP( BOOLEAN )		tbitBlitToScreen (HTBIT hSrc, LPTRECT lptrcSrc, LPTRECT lptrcDest, uWORD mode, BOOLEAN fTransparent);
TPSAPI_DECLP( BOOLEAN )		tbitBlitFromScreen (HTBIT hDest, LPTRECT lptrcSrc, LPTRECT lptrcDest, uWORD mode, BOOLEAN fTransparent);
TPSAPI_DECLP( BOOLEAN )		tbitScrollRect (HTBIT hTBit, LPTRECT lptrcSrc, TPT tptdelta);
TPSAPI_DECLP( HTMEM )		tbitFlatten (HTBIT hTBit);
TPSAPI_DECLP( HTBIT )		tbitUnFlatten (HTMEM hTMem);
TPSAPI_DECLP( void )	 		tbitFindEdge (HTBIT hTBit, LPTRECT lptrcMask, uBYTE edge_mask, uBYTE back_color); 
TPSAPI_DECLP( BOOLEAN )		tbitIsRectInBitmap (HTBIT hTBit, LPTRECT lptrcBit);
TPSAPI_DECLP( uLONG )  		tbitCalcRowBytes (uLONG uWidth, uLONG uBitDepth, uWORD uFlags);
TPSAPI_DECLP( HNATIVE )		tbitGetScreenPalette ();
TPSAPI_DECLP( HNATIVE )		tbitGetPrinterPalette ();
TPSAPI_DECLP( HTMEM )		tbitGetColorTable ( uWORD* puwNumEntries );

TPSAPI_DECLP( void )			tbitCompress(HTBIT bitmap, LPVOID dst, PULONG pulLen);
TPSAPI_DECLP( void )			tbitUncompress(HTBIT bitmap, LPVOID src);
TPSAPI_DECLP( void )			tbitCompressTransparent( HTBIT htbmp, LPVOID lpDst, uBYTE ubTransColor, PULONG pulDstLen );
TPSAPI_DECLP( void )			tbitUncompressTransparent( HTBIT htbmp, LPVOID lpSrc, uBYTE ubTransColor );
TPSAPI_DECLP( BOOLEAN )		tbitUncompressTransToBitmap( HTBIT htDst, LPTRECT lprcDst, LPVOID lpSrc, LPTRECT lprcSrc, uBYTE ubTranColor );
TPSAPI_DECLP( BOOLEAN )		tbitUncompressHitTest( LPTPT lppt, LPVOID lpSrc, LPTRECT lptrcSrc );
TPSAPI_DECLP( BOOLEAN )		tbitUncompressTransHitTest( LPTPT lppt, LPVOID lpSrc, LPTRECT lptrcSrc, uBYTE ubTransColor );

//	following routine for test purposes only
TPSAPI_DECLP( void )			tbitTestAPI(void);

#ifdef __cplusplus
}
#endif


#endif // _TPSBIT_H_ 


