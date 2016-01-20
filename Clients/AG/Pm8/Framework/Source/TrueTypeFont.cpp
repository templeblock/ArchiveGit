// ****************************************************************************
//
//  File Name:			TrueTypeFont.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the RTrueTypeFont class
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
//  $Logfile:: /PM8/Framework/Source/TrueTypeFont.cpp                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"TrueTypeFont.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#ifndef	IMAGER
#include "ApplicationGlobals.h"
#else
#include "GlobalBuffer.h"
#endif

#include "CharacterInfo.h"
#include "Path.h"

#ifdef	_WINDOWS
#include	"GDIObjects.h"
#endif	//	_WINDOWS

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::RTrueTypeFont( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
RTrueTypeFont::RTrueTypeFont( const YFontInfo& fontInfo, BOOLEAN fInitialize )
	: RFont( fontInfo, fInitialize ),
	m_hSfnt( NULL )
{
	NULL;
}											 

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::~RTrueTypeFont( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RTrueTypeFont::~RTrueTypeFont( )
{
#ifdef MAC
	::HPurge( m_hSfnt );
#endif
}

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::GetGlyphPath( )
//
//  Description:		Return the path that constitues the given character data
//
//  Returns:			Return the path data for the character
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
RPath*	RTrueTypeFont::GetGlyphPath( const RCharacter& character )
{
RPath*						path = NULL;
uBYTE*						pGlobalBuffer;
YTTSegmentInfoRecord*	pSegments;
EPathOperatorArray		pathOperators;
RIntPointArray				pathPoints;
RIntPoint					point;
int							nr;

RDataObjectLocker	locker( m_pPaths );
	// check cache
	if ( (path = m_pPaths->GetCharacterPath( character )) != NULL )
		return path;

	// get the character outline data out of the font
	pGlobalBuffer = ::GetGlobalBuffer( );
	*(uLONG *)pGlobalBuffer = 0;
	if ( !ExtractCharacterOutline( (int)character, (uLONG)pGlobalBuffer ) )
	{
		::ReleaseGlobalBuffer( pGlobalBuffer );
		return NULL;
	}

	// load the extracted data into path operator and point arrays
	nr = *(uLONG *)pGlobalBuffer;
	TpsAssert( ( ( nr * sizeof( YTTSegmentInfoRecord ) ) < GetGlobalBufferSize() ), "Global Buffer overflow." );
	pSegments = (YTTSegmentInfoRecord *)( pGlobalBuffer + sizeof( uLONG ) );
	try
	{
		for ( int i = 0; i < nr; i++ )
		{
			switch ( pSegments->opCode )
			{
				case MOVE_TO :
					pathOperators.InsertAtEnd( kMoveTo );
					point.m_x = pSegments->pt1.m_x;
					point.m_y = pSegments->pt1.m_y;
					pathPoints.InsertAtEnd( point );
					break;
				case LINE_TO :
					pathOperators.InsertAtEnd( kLine );
					pathOperators.InsertAtEnd( (EPathOperator)1 );
					point.m_x = pSegments->pt1.m_x;
					point.m_y = pSegments->pt1.m_y;
					pathPoints.InsertAtEnd( point );
					break;
				case BEZIER_TO :
					pathOperators.InsertAtEnd( kBezier );
					pathOperators.InsertAtEnd( (EPathOperator)1 );
					point.m_x = pSegments->pt1.m_x;
					point.m_y = pSegments->pt1.m_y;
					pathPoints.InsertAtEnd( point );
					point.m_x = pSegments->pt2.m_x;
					point.m_y = pSegments->pt2.m_y;
					pathPoints.InsertAtEnd( point );
					point.m_x = pSegments->pt3.m_x;
					point.m_y = pSegments->pt3.m_y;
					pathPoints.InsertAtEnd( point );
					break;
				case QUADRATIC_TO :
					pathOperators.InsertAtEnd( kQuadraticSpline );
					pathOperators.InsertAtEnd( (EPathOperator)1 );
					point.m_x = pSegments->pt1.m_x;
					point.m_y = pSegments->pt1.m_y;
					pathPoints.InsertAtEnd( point );
					point.m_x = pSegments->pt2.m_x;
					point.m_y = pSegments->pt2.m_y;
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


#ifdef	_WINDOWS
// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::ExtractCharacterOutline( )
//
//  Description:		
//
//  Returns:			Boolean indicating successful completion
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
BOOLEAN	RTrueTypeFont::ExtractCharacterOutline( int character, uLONG cookie )
{
HDC							hdc			= NULL;
uLONG							ulSegCount	= 0;
BOOLEAN						fSuccess		= FALSE;
LPUBYTE						lpubRawData	= NULL;

uLONG*						pulSegCount	= (uLONG*)( cookie );
YTTSegmentInfoRecord*	pSegments	= (YTTSegmentInfoRecord *)( cookie + sizeof(uLONG) );

	//	Do not need a printer to get the character outlines
	hdc			= ::GetDC( HWND_DESKTOP );
	if (!hdc)
		return FALSE;

RGDIFont			font( GetInfo() );
RGDIFont			oldFont( (HFONT)::SelectObject( hdc, font.GetFont() ) );

	try
	{
	int							i;
	int							structSize;
	MAT2							mat;
	uLONG							ulDataSize;
	uLONG							ulPolySize;
	LPUBYTE						pData;
	POINTFX*						pFxPoint;
	GLYPHMETRICS				glyphMetrics;
	TTPOLYCURVE*				pPolyCurve;
	TTPOLYGONHEADER*			pPolyHeader;

		//	Retrieve the size of the outline data
		memset( &mat, 0, sizeof(mat) );
		mat.eM11.value	= 1;
		mat.eM22.value = 1;
#ifdef	TPSDEBUG
		::SetLastError( 0 );
#endif	//	TPSDEBUG
		ulDataSize	= ::GetGlyphOutline( hdc, (UINT)character, GGO_NATIVE, &glyphMetrics, 0, NULL, &mat );
		if ( ulDataSize == GDI_ERROR )
		{
#ifdef	TPSDEBUG
			DWORD	lastError	= ::GetLastError( );
			LOGFONT	lf;
			::GetObject( font.GetFont(), sizeof(lf), &lf );
			if ( lastError == 0 )
				lastError = 1;
#endif	//	TPSDEBUG
			goto weFailed;
		}
		else if ( ulDataSize == 0 )
			goto weFailed;
		lpubRawData	= new uBYTE[ulDataSize];
		if ( ::GetGlyphOutline( hdc, (UINT)character, GGO_NATIVE, &glyphMetrics, ulDataSize, lpubRawData, &mat ) == GDI_ERROR )
			goto weFailed;

		pData			= lpubRawData;

		while ( pData < (lpubRawData + ulDataSize) )
		{
			pPolyHeader	= (TTPOLYGONHEADER*)pData;
			TpsAssert( pPolyHeader->dwType == TT_POLYGON_TYPE, "Outline type not TT_POLYGON_TYPE, not understood" );
			if ( pPolyHeader->dwType != TT_POLYGON_TYPE )
				goto weFailed;

			pSegments->opCode		= MOVE_TO;
			pSegments->pt1.m_x	= pPolyHeader->pfxStart.x.value;
			pSegments->pt1.m_y	= pPolyHeader->pfxStart.y.value;
			++pSegments;
			++ulSegCount;

			pData			= pData + sizeof(TTPOLYGONHEADER);
			ulPolySize	= pPolyHeader->cb - sizeof(TTPOLYGONHEADER);
			while( ulPolySize > 0 )
			{
				pPolyCurve = (TTPOLYCURVE*)pData;
				switch (pPolyCurve->wType)
				{
					case TT_PRIM_LINE :
						pFxPoint	= pPolyCurve->apfx;
						for (i = 0; i < pPolyCurve->cpfx; ++i, ++pFxPoint )
							{					
							pSegments->opCode		= LINE_TO;
							pSegments->pt1.m_x	= pFxPoint->x.value;
							pSegments->pt1.m_y	= pFxPoint->y.value;
							++pSegments;
							++ulSegCount;
							}
						break;
					case TT_PRIM_QSPLINE :
						pFxPoint	= pPolyCurve->apfx;
						for (i = 0; i < pPolyCurve->cpfx; NULL )
							{
							pSegments->opCode		= QUADRATIC_TO;
							pSegments->pt1.m_x	= pFxPoint[i].x.value;
							pSegments->pt1.m_y	= pFxPoint[i].y.value;
							++i;
							//
							//	If i == (pPolyCurve->cpfx-1) this is last point of polycurve
							//		Otherwise, endpoint is midpoint of this point and next point.
							if ( i == (pPolyCurve->cpfx-1) )
								{
								pSegments->pt2.m_x	= pFxPoint[i].x.value;
								pSegments->pt2.m_y	= pFxPoint[i].y.value;
								++i;
								}
							else
								{
								pSegments->pt2.m_x	= (pSegments->pt1.m_x + pFxPoint[i].x.value) / 2;
								pSegments->pt2.m_y	= (pSegments->pt1.m_y + pFxPoint[i].y.value) / 2;
								}

							++pSegments;
							++ulSegCount;
							}
						break;
					default :
						TpsAssertAlways( "Unknown PolyCurve Type" );
						break;
				}

			//	Advance the pData pointer and subtract the ulDataSize
			structSize	= sizeof(TTPOLYCURVE) + ((pPolyCurve->cpfx - 1)*sizeof(POINTFX));
			pData			+= structSize;
			TpsAssert( ulDataSize >= (uLONG)structSize, "Size of structure exceeds size of remaining data available" );
			ulPolySize	-= structSize;
			}

			//	End this path
			pSegments->opCode = CLOSE_PATH;
			++pSegments;
			++ulSegCount;
			fSuccess	= TRUE;

		}	//	End of while (pData < (RawData+ulDataSize))
	}
	catch( ... )
	{
		fSuccess = FALSE;
	}

weFailed:
	::SelectObject( hdc, oldFont.GetFont( ) );
	delete[] lpubRawData;
	::ReleaseDC( HWND_DESKTOP, hdc );

	*pulSegCount	= ulSegCount;
	return fSuccess;
}
#endif

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RTrueTypeFont::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RTrueTypeFont::Validate( ) const
{
	RFont::Validate( );
	TpsAssertIsObject( RTrueTypeFont, this );
}

#endif	//	TPSDEBUG
