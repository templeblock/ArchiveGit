/****************************************************************************
Contains:	HiRes region definition.
****************************************************************************/

#pragma once

#include "sctypes.h"

#define sliver_growSize 64

#define HRgnSize( n )	((long)(sizeof(HRgn)+((n)*sizeof(Sliver))))

struct Sliver {
	MicroPoint	fSLx1,
				fSLx2;		// horizontal extants of sliver
	MicroPoint	fSLTop; 	// top of sliver


	void		SetSliver( MicroPoint a, MicroPoint b, MicroPoint c )
					{ fSLx1 = a, fSLx2 = b, fSLTop = c; }
};

struct HEdge;

class HRgn {
public:
	void		SetBounds( void );
	void		UpdateBounds( MicroPoint, MicroPoint, MicroPoint );
	void		UpdateRealBounds( void );
	void		AddSliver( MicroPoint, MicroPoint, MicroPoint );
	BOOL		IsBorder( int, int );

	void		CorpSliver( Sliver**, MicroPoint, MicroPoint, MicroPoint );
	
	void		InsertSliver( MicroPoint, MicroPoint, MicroPoint, 
							  Sliver**, int, int );

	void		ScanEdges( HEdge*, long, int );


	MicroPoint	FirstLinePos( MicroPoint	firstLinePos,
							  MicroPoint	leading );

	void		SectRect( scXRect&, MicroPoint, MicroPoint, MicroPoint);

	long		fVersion;
	scXRect 	fOrigBounds;	// the original bounds of the region
	scXRect 	fMaxBounds; 	// max bounds of region
	scMuPoint	fOrg;			// locations of slivers are with
								// respect to this point
	MicroPoint	fVertInterval;	// vertical size of slivers
	int 		fNumSlivers;
	int 		fMaxSlivers;	// space exists for this many slivers
	
	scMemHandle fSlivers;
};

#if SCDEBUG > 1
	HRgnHandle	NewHRgnDebug( MicroPoint, const char*, int );
	#define NewHRgn( mp )		NewHRgnDebug( (mp), __FILE__, __LINE__ )
#else
	HRgnHandle	NewHRgn( MicroPoint );
#endif

void		DisposeHRgn( HRgnHandle );
BOOL		EmptyHRgn( HRgnHandle );
BOOL		EqualHRgn( const HRgnHandle, const HRgnHandle );
BOOL		PtInHRgn( const HRgnHandle, const scMuPoint& );
void		RectHRgn( HRgnHandle, const scXRect& );
void		PolyHRgn( HRgnHandle, const scVertex* );
void		CopyHRgn( HRgnHandle, const HRgnHandle );
void		SectHRgn( const HRgnHandle, const HRgnHandle, HRgnHandle );
void		UnionHRgn( const HRgnHandle, const HRgnHandle, HRgnHandle );
void		DiffHRgn( const HRgnHandle, const HRgnHandle, HRgnHandle );
void		XorHRgn( const HRgnHandle, const HRgnHandle, HRgnHandle );
void		TranslateHRgn( HRgnHandle, MicroPoint, MicroPoint );
void		InsetHRgn( HRgnHandle, MicroPoint, MicroPoint, BOOL );
void		SetEmptyHRgn( HRgnHandle );
MicroPoint	RGNSliverSize( const HRgnHandle );
BOOL		RectInHRgn ( const HRgnHandle, const scXRect& );

long		RGNExternalSize( const HRgnHandle, long );
void		RGNGetExtents( const HRgnHandle, scXRect& );
MicroPoint	RGNMaxDepth( const HRgnHandle );
BOOL		RGNtoFile( APPCtxPtr, IOFuncPtr, HRgnHandle, int );
HRgnHandle	RGNfromFile( APPCtxPtr, IOFuncPtr, int );

#if SCDEBUG > 0
void		RGNInvertSlivers( const HRgnHandle, 
							  APPDrwCtx, 
							  HiliteFuncPtr, 
							  const scSize&,
							  int vertflow );
#endif
