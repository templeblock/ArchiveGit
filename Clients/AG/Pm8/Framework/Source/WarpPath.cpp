// ****************************************************************************
//
//  File Name:			WarpPath.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				M. Houle
//
//  Description:		Functions to load and parse warp path, text block, or follow path
//							data.
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/WarpPath.cpp                             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "WarpPath.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#ifndef	IMAGER
	#include "ApplicationGlobals.h"
#else
	#include "GlobalBuffer.h"
#endif

#include "ResourceManager.h"

// type defs 
typedef struct 
{
	uBYTE							opCode; 
	uBYTE							path; 
	uWORD							padding;									// long align structure
	sLONG							pt1x;
	sLONG							pt1y;
	sLONG							pt2x;
	sLONG							pt2y;
	sLONG							pt3x;
	sLONG							pt3y;
} WarpSegmentRecord;
typedef struct 
{
	sLONG							pt1x;
	sLONG							pt1y;
} PointSegmentRecord;
#define	warpSegmentSwapBits	0x0AAA4
#define	pathSegmentSwapBits	0x0AAA4

const	uBYTE		kMoveToOpCode	= 'M';
const uBYTE		kmoveToOpCode	= 'm';
const uBYTE		kLineToOpCode	= 'L';
const	uBYTE		klineToOpCode	= 'l';
const	uBYTE		kCloseOpCode	= 'C';
const	uBYTE		kcloseOpCode	= 'c';
const	uBYTE		kBezierOpCode	= 'B';
const	uBYTE		kbezierOpCode	= 'b';
const	uBYTE		kEndOpCode		= 'E';
const	uBYTE		kendOpCode		= 'e';

// ****************************************************************************
//
//  Function Name:	LoadWarpPathData( )
//
//  Description:		Load the warp path by ID into the given arrays.  If any of the
//							arrays are NULL, it will not be filled in.
//							However, it is required that the PointArray and OperatorArrays
//							be passed in pairs.
//
//  Returns:			Boolean indicating successfull completion
//
//  Exceptions:		None
//
// ****************************************************************************
//
FrameworkLinkage BOOLEAN	LoadWarpPathData( sWORD swWarpId, RIntPointArray* pPArray,  EPathOperatorArray* pOArray,
																				 RIntPointArray* pP1Array, EPathOperatorArray* pO1Array,
																				 RIntPointArray* pP2Array, EPathOperatorArray* pO2Array,
																				 uWORD* pOrientation, uWORD* pPlacement )
{
	BOOLEAN	fSuccess	= TRUE;
	//
	//		First, validate ALL of the assumptions
	TpsAssert( ((pPArray==NULL)&&(pOArray==NULL)) || ((pPArray!=NULL)&&(pOArray!=NULL)), "Primary point array and Operator array are not paired" );
	TpsAssert( ((pP1Array==NULL)&&(pO1Array==NULL)) || ((pP1Array!=NULL)&&(pO1Array!=NULL)), "P1 point array and Operator array are not paired" );
	TpsAssert( ((pP2Array==NULL)&&(pO2Array==NULL)) || ((pP2Array!=NULL)&&(pO2Array!=NULL)), "P2 point array and Operator array are not paired" );
	TpsAssert( (pPArray||pP1Array||pP2Array), "No point array passed in..." );

	//
	//	Clean up any existing data in the arrays...
	if (pPArray)		pPArray->Empty();
	if (pOArray)		pOArray->Empty();
	if (pP1Array)		pP1Array->Empty();
	if (pO1Array)		pO1Array->Empty();
	if (pP2Array)		pP2Array->Empty();
	if (pO2Array)		pO2Array->Empty();

	//	Try to load the resource data..
	HNATIVE	hWarp;
#ifdef _WINDOWS
	YResourceType		trt;
	SetResourceType(trt, 'W','A','R','P');
	YResourceId			trid	= static_cast<YResourceId>( swWarpId );
	hWarp = GetResourceManager().GetResource( trt, trid );
#endif
#ifdef MAC 
	hWarp = ::Get1Resource( 'Warp', swWarpId );
#endif 
	if ( hWarp == NULL ) 
		return FALSE;

	try
	{
		uBYTE*					ubPtr;
		uWORD						uwNumSegments, uwOrientation, uwPlacement, uwPadding;
		uWORD						uwPath;
		RIntPoint				pt;
		RIntPoint				ptStart, ptLast;
		WarpSegmentRecord		segment;
		WarpSegmentRecord*	pSegments;

		tmemLockNativeResource( hWarp );

		ubPtr = (uBYTE *)tmemDeRefNativeResource( hWarp );
		uwOrientation = *(uWORD *)ubPtr;
		WinCode( uwOrientation = tintSwapWord( uwOrientation ); )
		TpsAssert( ( uwOrientation == 1 || uwOrientation == 2 ), "Bad orientation value." );
		ubPtr += sizeof( uWORD );
		uwPlacement = *(uWORD *)ubPtr;
		WinCode( uwPlacement = tintSwapWord( uwPlacement ); )
		TpsAssert( ( uwPlacement == 1 || uwPlacement == 2 || uwPlacement == 3 || uwPlacement == 4 || uwPlacement == 5 ), "Bad placement value." );
		ubPtr += sizeof( uWORD );
		uwPadding = *(uWORD *)ubPtr;
		TpsAssert( ( uwPadding == 0 ), "Nonzero padding value." );
		ubPtr += sizeof( uWORD );
		uwPadding = *(uWORD *)ubPtr;
		TpsAssert( ( uwPadding == 0 ), "Nonzero padding value." );
		ubPtr += sizeof( uWORD );
		uwNumSegments	= *(uWORD *)ubPtr;
		WinCode( uwNumSegments = tintSwapWord( uwNumSegments ); )
		ubPtr += sizeof( uWORD );
		pSegments = (WarpSegmentRecord *)( (uBYTE *)ubPtr );
		TpsAssert( ( pSegments->opCode == kMoveToOpCode || pSegments->opCode == kmoveToOpCode ), "Paths must start with a moveto." );

		BOOLEAN	fDone	= FALSE;
		for ( int i = 0; (i < uwNumSegments) && !fDone; ++i )
		{
			//	Copy the segment (can't modify resource data - it is read only)
			segment	= *pSegments;
			WinCode( tintSwapStructs( &segment, sizeof( segment ), 1, warpSegmentSwapBits ); )

			//
			//		Parse the data...
			switch ( segment.opCode ) 
			{
				case kMoveToOpCode:
				case kmoveToOpCode:
					//	Calculate MoveTo Point and store into proper arrays
					pt.m_x = ::InchesToLogicalUnits( ( (YRealCoordinate)segment.pt1x / 65536.0 ) );
					pt.m_y = ::InchesToLogicalUnits( ( (YRealCoordinate)segment.pt1y / 65536.0 ) );
					if (pOArray)	pOArray->InsertAtEnd( kMoveTo );
					if (pPArray)	pPArray->InsertAtEnd( pt );

					if ( (segment.path==1) && pP1Array )
					{
						pO1Array->InsertAtEnd( kMoveTo );
						pP1Array->InsertAtEnd( pt );
					}
					else if ( (segment.path==2) && pP2Array )
					{
						pO2Array->InsertAtEnd( kMoveTo );
						pP2Array->InsertAtEnd( pt );
					}
					uwPath = segment.path;
					ptStart = ptLast = pt;
			  		break;

				case kLineToOpCode:
				case klineToOpCode:
					//	If starting a new path, insert a move to
					if ( (segment.path==1) && (uwPath!=1) && pP1Array )
					{
						pO1Array->InsertAtEnd( kMoveTo );
						pP1Array->InsertAtEnd( pt );
					}
					else if ( (segment.path==2) && (uwPath!=2) && pP2Array )
					{
						pO2Array->InsertAtEnd( kMoveTo );
						pP2Array->InsertAtEnd( pt );
					}

					//	Calculate the LineTo Point and store into proper arrays
					pt.m_x = ::InchesToLogicalUnits( ( (YRealCoordinate)segment.pt1x / 65536.0 ) );
					pt.m_y = ::InchesToLogicalUnits( ( (YRealCoordinate)segment.pt1y / 65536.0 ) );
					if (pOArray)	pOArray->InsertAtEnd( kLine ), pOArray->InsertAtEnd( (EPathOperator)1 );
					if (pPArray)	pPArray->InsertAtEnd( pt );

					if ( (segment.path==1) && pP1Array )
					{
						pO1Array->InsertAtEnd( kLine ), pO1Array->InsertAtEnd( (EPathOperator)1 );
						pP1Array->InsertAtEnd( pt );
					}
					else if ( (segment.path==2) && pP2Array )
					{
						pO2Array->InsertAtEnd( kLine ), pO2Array->InsertAtEnd( (EPathOperator)1 );
						pP2Array->InsertAtEnd( pt );
					}
					uwPath = segment.path;
					ptLast = pt;
			  		break;

				case kCloseOpCode:
				case kcloseOpCode:
					if ( ptLast != ptStart )
					{
						if (pOArray)	pOArray->InsertAtEnd( kLine ), pOArray->InsertAtEnd( (EPathOperator)1 );
						if (pPArray)	pPArray->InsertAtEnd( ptStart );
					}
					if (pOArray)	pOArray->InsertAtEnd( kClose );
					uwPath = segment.path;
			  		break;

				case kBezierOpCode:
				case kbezierOpCode:
					//	If starting a new path, insert a move to
					if ( (segment.path==1) && (uwPath!=1) && pP1Array )
					{
						pO1Array->InsertAtEnd( kMoveTo );
						pP1Array->InsertAtEnd( pt );
					}
					else if ( (segment.path==2) && (uwPath!=2) && pP2Array )
					{
						pO2Array->InsertAtEnd( kMoveTo );
						pP2Array->InsertAtEnd( pt );
					}

					//	Insert the operators
					if (pOArray)	pOArray->InsertAtEnd( kBezier ), pOArray->InsertAtEnd( (EPathOperator)1 );
					if ( (segment.path==1) && pO1Array)
						pO1Array->InsertAtEnd( kBezier ), pO1Array->InsertAtEnd( (EPathOperator)1 );
					if ( (segment.path==2) && pO2Array)
						pO2Array->InsertAtEnd( kBezier ), pO2Array->InsertAtEnd( (EPathOperator)1 );

					//	Insert the points
					pt.m_x = ::InchesToLogicalUnits( ( (YRealCoordinate)segment.pt1x / 65536.0 ) );
					pt.m_y = ::InchesToLogicalUnits( ( (YRealCoordinate)segment.pt1y / 65536.0 ) );
					if (pPArray)	pPArray->InsertAtEnd( pt );
					if ( (segment.path==1) && pP1Array )
						pP1Array->InsertAtEnd( pt );
					else if ( (segment.path==2) && pP2Array )
						pP2Array->InsertAtEnd( pt );

					pt.m_x = ::InchesToLogicalUnits( ( (YRealCoordinate)segment.pt2x / 65536.0 ) );
					pt.m_y = ::InchesToLogicalUnits( ( (YRealCoordinate)segment.pt2y / 65536.0 ) );
					if (pPArray)	pPArray->InsertAtEnd( pt );
					if ( (segment.path==1) && pP1Array )
						pP1Array->InsertAtEnd( pt );
					else if ( (segment.path==2) && pP2Array )
						pP2Array->InsertAtEnd( pt );

					pt.m_x = ::InchesToLogicalUnits( ( (YRealCoordinate)segment.pt3x / 65536.0 ) );
					pt.m_y = ::InchesToLogicalUnits( ( (YRealCoordinate)segment.pt3y / 65536.0 ) );
					if (pPArray)	pPArray->InsertAtEnd( pt );
					if ( (segment.path==1) && pP1Array )
						pP1Array->InsertAtEnd( pt );
					else if ( (segment.path==2) && pP2Array )
						pP2Array->InsertAtEnd( pt );

					uwPath = segment.path;
					ptLast = pt;
			  		break;

				case kEndOpCode:
				case kendOpCode:
					//	End all of the operator arrays
					fDone = TRUE;
					TpsAssert( i == (uwNumSegments-1), "EndOpCode found, but not all segments parsed" );
			  		break;

				default:
					TpsAssertAlways( "Invalid warp segment operator." );
			  		break;
			}

			pSegments++;
		}

		if ( pOrientation != NULL )
			*pOrientation = uwOrientation;
		if ( pPlacement != NULL )
			*pPlacement = uwPlacement;
	}
	catch( ... )
	{
		if (pPArray)		pPArray->Empty();
		if (pOArray)		pOArray->Empty();
		if (pP1Array)		pP1Array->Empty();
		if (pO1Array)		pO1Array->Empty();
		if (pP2Array)		pP2Array->Empty();
		if (pO2Array)		pO2Array->Empty();
  		fSuccess = FALSE;
	}


	//
	//		If we succeeded. force the end into the operator arrays
	if (fSuccess)
	{
		if (pOArray)	pOArray->InsertAtEnd( kEnd );
		if (pO1Array)	pO1Array->InsertAtEnd( kEnd );
		if (pO2Array)	pO2Array->InsertAtEnd( kEnd );
	}
	
	// cleanup resources
	tmemUnlockNativeResource( hWarp );
#ifdef _WINDOWS
	 GetResourceManager().FreeResource( hWarp );
#endif
#ifdef MAC 
	tmemPurgeNativeResource( hWarp );
#endif 

	return fSuccess;
}

// ****************************************************************************
//
//  Function Name:	LoadFollowPathData( )
//
//  Description:		Load the follow path by ID into the given arrays.  If any of the
//							arrays are NULL, it will not be filled in.
//							However, it is required that the PointArray and OperatorArrays
//							be passed in pairs.
//
//  Returns:			Boolean indicating successfull completion
//
//  Exceptions:		None
//
// ****************************************************************************
//
FrameworkLinkage BOOLEAN	LoadFollowPathData( sWORD swWarpId, RIntPointArray* pPArray )
{
	BOOLEAN	fSuccess	= TRUE;
	//
	//		First, validate ALL of the assumptions
	TpsAssert( ( pPArray != NULL ), "No point array passed in..." );

	//
	//	Clean up any existing data in the arrays...
	if (pPArray)		pPArray->Empty();

	//	Try to load the resource data..
	HNATIVE	hPath;
#ifdef _WINDOWS
	YResourceType		trt;
	SetResourceType(trt, 'A','R','C','S');
	YResourceId			trid	= static_cast<YResourceId>( swWarpId );
	hPath = GetResourceManager().GetResource( trt, trid );
#endif
#ifdef MAC 
	hPath = ::Get1Resource( 'Arcs', swWarpId );
#endif 
	if ( hPath == NULL ) 
		return FALSE;

	try
	{
		uBYTE*					ubPtr;
		uWORD						uwNumSegments, uwPadding;
		RIntPoint				pt;
		PointSegmentRecord	segment;
		PointSegmentRecord*	pSegments;

		tmemLockNativeResource( hPath );

		ubPtr = (uBYTE *)tmemDeRefNativeResource( hPath );
		uwPadding = *(uWORD *)ubPtr;
		TpsAssert( ( uwPadding == 0 ), "Padding nonzero" );
		ubPtr += sizeof( uWORD );
		uwNumSegments = *(uWORD *)ubPtr;
		WinCode( uwNumSegments = tintSwapWord( uwNumSegments ); )
		ubPtr += sizeof( uWORD );
		pSegments = (PointSegmentRecord *)( (uBYTE *)ubPtr );

		BOOLEAN	fDone				= FALSE;
		for ( int i = 0; (i < uwNumSegments) && !fDone; ++i )
		{
			//	Copy the segment (can't modify resource data - it is read only)
			segment	= *pSegments;
			WinCode( segment.pt1x = tintSwapLong( segment.pt1x ); )
			WinCode( segment.pt1y = tintSwapLong( segment.pt1y ); )

			//
			//		Parse the data...
			//	Calculate ArcTo Point and store into proper arrays
			pt.m_x = ::InchesToLogicalUnits( ( (YRealCoordinate)segment.pt1x / 65536.0 ) );
			pt.m_y = ::InchesToLogicalUnits( ( (YRealCoordinate)segment.pt1y / 65536.0 ) );
			if (pPArray)	pPArray->InsertAtEnd( pt );

			pSegments++;
		}
	}
	catch( ... )
	{
		if (pPArray)		pPArray->Empty();
  		fSuccess = FALSE;
	}

	// cleanup resources
	tmemUnlockNativeResource( hPath );
#ifdef _WINDOWS
	 GetResourceManager().FreeResource( hPath );
#endif
#ifdef MAC 
	tmemPurgeNativeResource( hPath );
#endif 

	return fSuccess;
}

