/******************************************************************************

   Header Name: tpsrect.h
   Description: TPS Rectangle and Point utility
   Version:     v0.01
   Author:      Michael D. Houle
   Date:        2 February 1995
   Copyright:   (c) Turning Point Software, Inc., 1995.  All Rights Reserved.

   Revision History:

   Date     Who      What
	2/2/95	MDH		Initial Version

	NOTES:

	TODO:
******************************************************************************/

#ifndef	_tpsrect_h
#define	_tpsrect_h

//
//		Includes and dependancies
#ifndef	_TPSDEFS_H_
#include "tpsdefs.h"
#endif	//	_TPSDEFS_H_

//
//		Defines, Flags...


//
//		Structures, Typedefs...

MacCode( typedef Rect	NRECT; )			//	Native rectangle
WinCode( typedef RECT	NRECT; )			//   "        "
MacCode( typedef Point	NPT; )			//	Native point
WinCode( typedef POINT	NPT; )			//   "      "

typedef	NRECT			TRECT;				//	TPS rectangle is actually native rectangle (just don't tell anyone)

typedef	TRECT*		PTRECT;						// ptrc
typedef	TRECT NEAR*	NPTRECT;						// nptrc
typedef	TRECT FAR*	LPTRECT;						//	lptrc


#if	defined( MAC ) || defined( DOS )

typedef	struct	tagTPT
{
	sWORD		y;
	sWORD		x;
} TPT;													//	tpt

#elif	defined( _WINDOWS )

typedef	POINT	TPT;

#endif

typedef	TPT*			PTPT;							//	ptpt
typedef	TPT NEAR*	NPTPT;						// nptpt;
typedef	TPT FAR*		LPTPT;						//	lptpt;

//	define size of x or y component of a point
#if	defined(WIN32)
typedef	sLONG	TSIZE;
#else
typedef	sWORD	TSIZE;
#endif	/* defined(WIN32)) */

//
//		Defines, Macros...

#define	trcMaketrc( ptrc, swL, swT, swR, swB )	{ (ptrc)->left = swL; (ptrc)->top = swT; (ptrc)->right = swR; (ptrc)->bottom = swB; }
#define	tptMaketpt( ptpt, swX, swY )				{ (ptpt)->y = swY; (ptpt)->x = swX; }

#define	trcWidth( ptrc )								( (TSIZE)((ptrc)->right - (ptrc)->left) )
#define	trcHeight( ptrc )								( (TSIZE)((ptrc)->bottom - (ptrc)->top) )
#define	trcTopLeft( ptrc, tpt )						( tpt = ((TPT*)ptrc)[0])//{ tpt.x=ptrc->left;tpt.y=ptrc->top; }
#define	trcBotRight( ptrc, tpt )					( tpt = ((TPT*)ptrc)[1])//{ tpt.x=ptrc->right;tpt.y=ptrc->bottom; }
#define	trcWidthHeight( ptrc, ptpt )				Maketpt( ptpt, trcWidth(trc), trcHeight(trc) )

#if	!defined(WIN32)
#define	trcSetEmpty( ptrc )							{ ((PULONG)(ptrc))[0] = 0L, ((PULONG)(ptrc))[1] = 0L; }
#define	tptIsEqual( tpt1, tpt2 )					( *((PULONG)&(tpt1)) == *((PULONG)&(tpt2)) )
#else
#define	trcSetEmpty( ptrc )							{ trcMaketrc(ptrc, 0, 0, 0, 0) }
#define	tptIsEqual( tpt1, tpt2 )					( (((tpt1)->y) == ((tpt2)->y)) && (((tpt1)->x) == ((tpt2)->x)) )
#endif	/* !defined(WIN32) */

#define	nrcTotrc( nrc, trc )							{ trc = *((TRECT*)&nrc); }
#define	trcTonrc( trc, nrc )							{ nrc = *((NRECT*)&trc); }

#if	1
	#define	nptTotpt( npt, tpt )							(tpt = *((TPT*)&npt))
	#define	tptTonpt( tpt, npt )							(npt = *((NPT*)&tpt))
#else
	#ifdef	MAC

	#define	nptTotpt( npt, tpt )							{ (tpt).x=(npt).h,(tpt).y=(npt).v; }
	#define	tptTonpt( tpt, npt )							{ (npt).h=(tpt).x,(npt).v=(tpt).y; }

	#else

	#define	nptTotpt( npt, tpt )							{ (tpt).x=(npt).x,(tpt).y=(npt).y; }
	#define	tptTonpt( tpt, npt )							{ (npt).x=(tpt).x,(npt).y=(tpt).y; }

	#endif
#endif

//
//		Prototypes
#ifdef	__cplusplus
extern "C" {
#endif	//	_cplusplus


TPSAPI_DECLP( VOID )		trcOffset( LPTRECT ptrc, TSIZE swX, TSIZE swY );
TPSAPI_DECLP( VOID )		trcInset( LPTRECT ptrc, TSIZE swX, TSIZE swY );

TPSAPI_DECLP( BOOLEAN )	trcIntersect( LPTRECT ptrc1, LPTRECT ptrc2, LPTRECT ptrcDst );
TPSAPI_DECLP( VOID )		trcUnion( LPTRECT ptrc1, LPTRECT ptrc2, LPTRECT ptrcDst );

TPSAPI_DECLP( VOID )		trcPintpt( LPTRECT ptrc, PTPT ptpt );
TPSAPI_DECLP( VOID )		trcPintrc( LPTRECT ptrc1, LPTRECT ptrc2 );

TPSAPI_DECLP( VOID )		trcCentertpt( LPTRECT ptrc, PTPT ptpt );
TPSAPI_DECLP( VOID )		trcCentertrc( LPTRECT ptrc1, LPTRECT ptrc2 );

TPSAPI_DECLP( BOOLEAN )	trcIsEmpty( LPTRECT ptrc );
TPSAPI_DECLP( BOOLEAN )	trcIsEqual( LPTRECT ptrc1, LPTRECT ptrc2 );
TPSAPI_DECLP( BOOLEAN )	trcIsIntersect( LPTRECT ptrcSrc1, LPTRECT ptrcSrc2 );
TPSAPI_DECLP( BOOLEAN ) trcIntrc( LPTRECT ptrc1, LPTRECT ptrc2 );

TPSAPI_DECLP( VOID )		tptAdd( TPT tpt1, TPT tpt2, PTPT ptptDst );
TPSAPI_DECLP( VOID )		tptDelta( TPT tpt1, TPT tpt2, PTPT ptptDst );
TPSAPI_DECLP( BOOLEAN )	tptIntrc( LPTRECT ptrc, TPT tpt );


#ifdef	__cplusplus
}
#endif	//	_cplusplus

#endif	//	_tpsrect_h


