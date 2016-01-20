// ****************************************************************************
//
//  File Name:			ATMFont.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the RATMFont class
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
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/ATMFont.cpp                              $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"ATMFont.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#ifndef	IMAGER
#include "ApplicationGlobals.h"
#else
#include "GlobalBuffer.h"
#endif

#include "CharacterInfo.h"
#include "Path.h"

#ifdef MAC
	// define Close Path routine descriptor
	typedef	UniversalProcPtr	closePathHookPI;
	#define	NewClosePathProc(userRoutine)	\
			(closePathHookPI) NewRoutineDescriptor((ProcPtr) (userRoutine), closePathHookProcInfo, GetCurrentISA())
	// define Line To routine descriptor
	typedef	UniversalProcPtr	lineToHookPI;
	#define	NewLineToProc(userRoutine)	\
			(lineToHookPI) NewRoutineDescriptor((ProcPtr) (userRoutine), lineToHookProcInfo, GetCurrentISA())
	// define Move To routine descriptor
	typedef	UniversalProcPtr	moveToHookPI;
	#define	NewMoveToProc(userRoutine)	\
			(moveToHookPI) NewRoutineDescriptor((ProcPtr) (userRoutine), moveToHookProcInfo, GetCurrentISA())
	// define Curve To routine descriptor
	typedef	UniversalProcPtr	curveToHookPI;
	#define	NewCurveToProc(userRoutine)	\
			(curveToHookPI) NewRoutineDescriptor((ProcPtr) (userRoutine), curveToHookProcInfo, GetCurrentISA())
#endif

//	local defs 
#define	FAILURE										0
#define	SUCCESS										1

struct	YBufferHeader
{
	uLONG	ulNumRecords;
	uLONG	ulMaxRecords;
} ;

// callback prototypes 
#ifdef _WINDOWS
		 BOOL ATMCALLBACK 	atmMoveToCallback( LPATMFixedPoint lpFixPnt, DWORD userData );
		 BOOL ATMCALLBACK 	atmLineToCallback( LPATMFixedPoint lpFixPnt, DWORD userData );
		 BOOL ATMCALLBACK 	atmCurveToCallback( LPATMFixedPoint pt1, LPATMFixedPoint pt2, LPATMFixedPoint pt3, DWORD userData );
		 BOOL ATMCALLBACK 	atmClosePathCallback( DWORD userData );
#endif
#ifdef MAC
		 short 	atmMoveToCallback( uLONG userData, ATMFixedPoint* pt );
		 short 	atmLineToCallback( uLONG userData, ATMFixedPoint* pt );
		 short 	atmCurveToCallback( uLONG userData, ATMFixedPoint* pt1, ATMFixedPoint* pt2, ATMFixedPoint* pt3 );
		 short 	atmClosePathCallback( uLONG userData );
#endif


// ****************************************************************************
//
//  Function Name:	RATMFont::RATMFont( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
RATMFont::RATMFont( const YFontInfo& fontInfo, BOOLEAN fInitialize )
	: RFont( fontInfo, fInitialize )
{
#ifdef MAC
	m_MoveToUPP = NewMoveToProc( atmMoveToCallback );
	m_LineToUPP = NewLineToProc( atmLineToCallback );
	m_CurveToUPP = NewCurveToProc( atmCurveToCallback );
	m_ClosePathUPP = NewClosePathProc( atmClosePathCallback );
#endif
}

// ****************************************************************************
//
//  Function Name:	RATMFont::~RATMFont( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RATMFont::~RATMFont( )
{
#ifdef MAC
	DisposeRoutineDescriptor( m_MoveToUPP );
	DisposeRoutineDescriptor( m_LineToUPP );
	DisposeRoutineDescriptor( m_CurveToUPP );
	DisposeRoutineDescriptor( m_ClosePathUPP );
#endif
}

// ****************************************************************************
//
//  Function Name:	RATMFont::GetGlyphPath( )
//
//  Description:		Return the path that constitues the given character data
//
//  Returns:			Return the path data for the character
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
RPath*	RATMFont::GetGlyphPath( const RCharacter& character )
{
RPath*						path = NULL;
uBYTE*						pGlobalBuffer;
YBufferHeader*				pBufferHeader;
YATMSegmentInfoRecord*	pSegments;
EPathOperatorArray		pathOperators;
RIntPointArray				pathPoints;
RIntPoint					point;
int							nr;

RDataObjectLocker	locker( m_pPaths );

	// check cache
	if ( (path = m_pPaths->GetCharacterPath( character )) != NULL )
		return path;

	// get the character outline data out of the font
	pGlobalBuffer						= ::GetGlobalBuffer( );
	//
	//	Setup the buffer header
	pBufferHeader						= reinterpret_cast<YBufferHeader*>( pGlobalBuffer );
	pBufferHeader->ulNumRecords	= 0;
	pBufferHeader->ulMaxRecords	= (::GetGlobalBufferSize( pGlobalBuffer ) - sizeof(YBufferHeader) ) / sizeof(YATMSegmentInfoRecord);
	//
	//	Extract the character outline
	if ( !ExtractCharacterOutline( (int)character, (uLONG)pBufferHeader ) )
	{
		//	Find out the number of records used in the character
		uLONG		ulRecordCount = pBufferHeader->ulNumRecords;
		::ReleaseGlobalBuffer( pGlobalBuffer );

		//
		//	If number or records is greater than 0, we overflowed the buffer, so realloc it and try again
		if (ulRecordCount > 0)
		{
			//		Reallocate
			uLONG	ulBufferSize	= (ulRecordCount * sizeof(YATMSegmentInfoRecord)) + sizeof(YBufferHeader);
			pGlobalBuffer			= ::GetGlobalBuffer( ulBufferSize );
			//
			//		Setup the buffer header
			pBufferHeader						= reinterpret_cast<YBufferHeader*>( pGlobalBuffer );
			pBufferHeader->ulNumRecords	= 0;
			pBufferHeader->ulMaxRecords	= (::GetGlobalBufferSize( pGlobalBuffer ) - sizeof(YBufferHeader) ) / sizeof(YATMSegmentInfoRecord);
			//
			//		Lets try to get it again
			if ( !ExtractCharacterOutline( (int)character, (uLONG)pGlobalBuffer ) )
			{
				TpsAssertAlways( "ExtractCharacterOutline needed bigger buffer and then failed?" );
				ulRecordCount = 0;
			}
			else
			{
				TpsAssert( pBufferHeader->ulNumRecords == ulRecordCount, "ExtractCharacterOutline was called twice and had different counts?" );
				ulRecordCount = pBufferHeader->ulNumRecords;
			}
		}

		if ( ulRecordCount == 0)
		{
			::ReleaseGlobalBuffer( pGlobalBuffer );
			return NULL;
		}
	}

	// load the extracted data into path operator and point arrays
	nr				= pBufferHeader->ulNumRecords;
	pSegments	= (YATMSegmentInfoRecord *)( pBufferHeader + 1 );
	try
	{
		for ( int i = 0; i < nr; i++ )
		{
			switch ( pSegments->opCode )
			{
				case MOVE_TO :
					pathOperators.InsertAtEnd( kMoveTo );
					point.m_x = pSegments->pt1.x / 65536;
					point.m_y = pSegments->pt1.y / 65536;
					pathPoints.InsertAtEnd( point );
					break;
				case LINE_TO :
					pathOperators.InsertAtEnd( kLine );
					pathOperators.InsertAtEnd( (EPathOperator)1 );
					point.m_x = pSegments->pt1.x / 65536;
					point.m_y = pSegments->pt1.y / 65536;
					pathPoints.InsertAtEnd( point );
					break;
				case BEZIER_TO :
					pathOperators.InsertAtEnd( kBezier );
					pathOperators.InsertAtEnd( (EPathOperator)1 );
					point.m_x = pSegments->pt1.x / 65536;
					point.m_y = pSegments->pt1.y / 65536;
					pathPoints.InsertAtEnd( point );
					point.m_x = pSegments->pt2.x / 65536;
					point.m_y = pSegments->pt2.y / 65536;
					pathPoints.InsertAtEnd( point );
					point.m_x = pSegments->pt3.x / 65536;
					point.m_y = pSegments->pt3.y / 65536;
					pathPoints.InsertAtEnd( point );
					break;
				case CLOSE_PATH :
					pathOperators.InsertAtEnd( kClose );
					break;
				default :
					TpsAssertAlways( "Invalid segment operator." );
					break;
			}
			pSegments++;
		}
		pathOperators.InsertAtEnd( kEnd );
	}
	catch( ... )
	{
		pathOperators.Empty();
		pathPoints.Empty();
	}
	::ReleaseGlobalBuffer( pGlobalBuffer );

	// create a path object
	try
	{
		if ( pathOperators.Count() != 0 && pathPoints.Count() != 0 )
		{
			if ( pathPoints.Count() > 512 )
				path = new RPath( 512 );
			else
				path = new RPath;
			if ( path->Define( &(*pathOperators.Start()), &(*pathPoints.Start()) ) )
				m_pPaths->SetCharacterPath( character, path );
			else
			{
				delete path;
				path = NULL;
			}
		}
	}
	catch( ... )
	{
		delete path;
		throw;
	}

	// cleanup
	pathOperators.Empty();
	pathPoints.Empty();

	return path;
}


// ****************************************************************************
//
//  Function Name:	RATMFont::ExtractCharacterOutline( )
//
//  Description:		
//
//  Returns:			Boolean indicating successful completion
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
BOOLEAN	RATMFont::ExtractCharacterOutline( int character, uLONG cookie ) const
{
short						result = ATM_MEMORY;
ATMFixedMatrix			matrix;	
const YFontInfo		fontInfo = GetInfo();
#ifdef	_WINDOWS
HDC						hdc;
#endif
#ifdef	MAC
CGrafPtr					pCPort = (CGrafPtr)::NewPtr( sizeof( CGrafPort ) );
GrafPtr					holdPort;
#endif

	matrix.a = (ATMFixed)(fontInfo.height * 65536.0);				// point size
	matrix.b = matrix.c = (ATMFixed)0;									// no skew 
	matrix.d = matrix.a;
	matrix.tx = matrix.ty = (ATMFixed)0;								// at origin
#ifdef	_WINDOWS

	hdc			= ::GetDC( HWND_DESKTOP );
	if (!hdc)
		return FALSE;

	if ( hdc )
	{
		try
		{
		HFONT				hFontOld;
		RGDIFont			font( GetInfo() );
		
		result = static_cast<short>( ATMSelectObject( hdc, font.GetFont(), ATM_SELECT|ATM_USEEXACTWIDTH, (LPHANDLE)&hFontOld ) );
		if ( result == ATM_SELECTED )
			{
			result = static_cast<short>( ATMGetOutline( hdc, static_cast<char>( character ), &matrix, atmMoveToCallback, 
																	  atmLineToCallback, 
																	  atmCurveToCallback, 
																	  atmClosePathCallback, (DWORD)cookie ) );

			::SelectObject( hdc, hFontOld );
			}
		}
		catch( ... )
		{
			result = ATM_MEMORY;
		}
	}

	// Clean up
	if( hdc )
		::ReleaseDC( HWND_DESKTOP, hdc );
	
#endif
#ifdef	MAC
	::GetPort( &holdPort );   
	if ( pCPort != NULL )
	{
		::OpenCPort( pCPort );
		::SetPort( (GrafPtr)pCPort );
		::SetOutlinePreferred( true );				
		::TextFont( RFont::GetMacFontFamilyId( (const LPSZ)fontInfo.sbName ) );
		::TextFace( RFont::GetMacStyleBits( fontInfo.attributes ) );
#ifdef	powerc
		result = getOutlineATM( (short)character, &matrix, (Ptr)cookie, (short(*)())m_MoveToUPP, 
																							 (short(*)())m_LineToUPP, 
																							 (short(*)())m_CurveToUPP, 
																							 (short(*)())m_ClosePathUPP );
#else
		result = ATM_NO_VALID_FONT;
#endif	// power mac
		::CloseCPort( pCPort );
		::DisposePtr( (Ptr)pCPort );
	}
	::SetPort( holdPort );													// cleanup
#endif

	return static_cast<BOOLEAN>( result == ATM_NOERR );
}


#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RATMFont::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RATMFont::Validate( ) const
{
	RFont::Validate( );
	TpsAssertIsObject( RATMFont, this );
}

#endif	//	TPSDEBUG


// ****************************************************************************
//
//  Function Name:	atmMoveToCallback( )
//
//  Description:		Move to callback rountine used by getATMOutline/ATMGetOutline
//
//  Returns:			short/boolean indicating successful completion
//
//  Exceptions:		None
//
// ****************************************************************************
//
#ifdef _WINDOWS
	BOOL ATMCALLBACK atmMoveToCallback( LPATMFixedPoint pt, DWORD userData )
#endif
#ifdef MAC
	short atmMoveToCallback( uLONG userData, ATMFixedPoint* pt )
#endif
{
YBufferHeader*							pHeader	= reinterpret_cast<YBufferHeader*>( userData );
RATMFont::YATMSegmentInfoRecord*	rec		= ((RATMFont::YATMSegmentInfoRecord*)(pHeader+1));
	
	TpsAssert( ( pHeader != NULL ), "Nil buffer pointer." )

	if ( pHeader->ulNumRecords < pHeader->ulMaxRecords )
	{
		rec			+= pHeader->ulNumRecords;
		rec->opCode	= MOVE_TO;
		rec->pt1		= *pt;
	}
	++( pHeader->ulNumRecords );

	return SUCCESS;
}

// ****************************************************************************
//
//  Function Name:	atmLineToCallback( )
//
//  Description:		Line to callback rountine used by getATMOutline/ATMGetOutline
//
//  Returns:			short/boolean indicating successful completion
//
//  Exceptions:		None
//
// ****************************************************************************
//
#ifdef _WINDOWS
	BOOL ATMCALLBACK atmLineToCallback( LPATMFixedPoint pt, DWORD userData )
#endif
#ifdef MAC
	short atmLineToCallback( uLONG userData, ATMFixedPoint* pt )
#endif
{
YBufferHeader*							pHeader	= reinterpret_cast<YBufferHeader*>( userData );
RATMFont::YATMSegmentInfoRecord*	rec		= ((RATMFont::YATMSegmentInfoRecord*)(pHeader+1));
	
	TpsAssert( ( pHeader != NULL ), "Nil buffer pointer." )

	if ( pHeader->ulNumRecords < pHeader->ulMaxRecords )
	{
		rec			+= pHeader->ulNumRecords;
		rec->opCode	= LINE_TO;
		rec->pt1		= *pt;
	}
	++( pHeader->ulNumRecords );

	return SUCCESS;
}

// ****************************************************************************
//
//  Function Name:	atmCurveToCallback( )
//
//  Description:		Curve to callback rountine used by getATMOutline/ATMGetOutline
//
//  Returns:			short/boolean indicating successful completion
//
//  Exceptions:		None
//
// ****************************************************************************
//
#ifdef _WINDOWS
	BOOL ATMCALLBACK atmCurveToCallback( LPATMFixedPoint pt1, LPATMFixedPoint pt2, LPATMFixedPoint pt3, DWORD userData )
#endif
#ifdef MAC
	short atmCurveToCallback( uLONG userData, ATMFixedPoint* pt1, ATMFixedPoint* pt2, ATMFixedPoint* pt3 )
#endif
{
YBufferHeader*							pHeader	= reinterpret_cast<YBufferHeader*>( userData );
RATMFont::YATMSegmentInfoRecord*	rec		= ((RATMFont::YATMSegmentInfoRecord*)(pHeader+1));
	
	TpsAssert( ( pHeader != NULL ), "Nil buffer pointer." )

	if ( pHeader->ulNumRecords < pHeader->ulMaxRecords )
	{
		rec			+= pHeader->ulNumRecords;
		rec->opCode = BEZIER_TO;
		rec->pt1 = *pt1;
		rec->pt2 = *pt2;
		rec->pt3 = *pt3;
	}
	++( pHeader->ulNumRecords );

	return SUCCESS;
}

// ****************************************************************************
//
//  Function Name:	atmClosePathCallback( )
//
//  Description:		Close path callback rountine used by getATMOutline/ATMGetOutline
//
//  Returns:			short/boolean indicating successful completion
//
//  Exceptions:		None
//
// ****************************************************************************
//
#ifdef _WINDOWS
	BOOL ATMCALLBACK atmClosePathCallback( DWORD userData )
#endif
#ifdef MAC
	short atmClosePathCallback( uLONG userData )
#endif
{
YBufferHeader*							pHeader	= reinterpret_cast<YBufferHeader*>( userData );
RATMFont::YATMSegmentInfoRecord*	rec		= ((RATMFont::YATMSegmentInfoRecord*)(pHeader+1));
	
	TpsAssert( ( pHeader != NULL ), "Nil buffer pointer." )

	if ( pHeader->ulNumRecords < pHeader->ulMaxRecords )
	{
		rec			+= pHeader->ulNumRecords;
		rec->opCode = CLOSE_PATH;
	}
	++( pHeader->ulNumRecords );

	return SUCCESS;
}

