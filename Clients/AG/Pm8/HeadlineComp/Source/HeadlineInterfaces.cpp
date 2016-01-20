// ****************************************************************************
//
//  File Name:			HeadlineInterfaces.cpp
//
//  Project:			Renaissance Headline Component
//
//  Author:				R. Hood
//
//  Description:		Definition of the RHeadlineGraphic class
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
//  $Logfile:: /PM8/HeadlineComp/Source/HeadlineInterfaces.cpp                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "HeadlineIncludes.h"

ASSERTNAME

#include "CharacterInfo.h"
#include "HeadlineGraphic.h"
#include "HeadlinesCanned.h"
#include "WarpPath.h"

#ifndef	IMAGER
	#include "ApplicationGlobals.h"
#else
	#include "GlobalBuffer.h"
#endif

#ifndef	IMAGER
	#include "HeadlineCompResource.h"
#endif

// constances
const		RCharacter			kLineFeed( 0x0A );				// REVEIW RAH needs to be internationalized
const		RCharacter			kCarrageReturn( 0x0D );
const		RCharacter			kSpace( 0x20 );
const		int					kPathArraySize	= 512;

// macros
//#ifdef MAC
#if 0
inline	double	_hypot( double dy, double dx )
{
	return tanh( dy / dx );
}
#endif  // MAC


// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetDefaultHeadlineSize( )
//
// Description:		Returns default headline size in logical units
//
// Returns:				RIntSize
//
// Exceptions:			None
//
// ****************************************************************************
//
RIntSize  RHeadlineGraphic::GetDefaultHeadlineSize( BOOLEAN fHorizontal )
{
	if ( fHorizontal )
		return RIntSize( ::InchesToLogicalUnits( 5 ), ::InchesToLogicalUnits( 2 ) );
	return RIntSize( ::InchesToLogicalUnits( 2 ), ::InchesToLogicalUnits( 5 ) );
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::ApplyOutlineEffect( )
//
// Description:		Setup outline effect
//
// Returns:				Nothing.
//
// Exceptions:			None
//
// ****************************************************************************
//
void RHeadlineGraphic::ApplyOutlineEffect( EOutlineEffects type ) 
{
	//	switch moved above if statement below to help fix bug 2035
	//	problem was that some callers might not set line weight to hairline
	//	when setting blur effect.
	switch ( type )
	{
		case kSimpleStroke :
		case kBlurredOutline :
		case kShadowedOutline :
		case kEmbossedOutline :
		case kDebossedOutline :
		case kSoftEmbossedOutline :
		case kSoftDebossedOutline :
			if ( m_LineWeight != kHairLine )
				Redefine();
			m_LineWeight = kHairLine;
			break;
		default :
			break;
	}
	if ( m_OutlineEffect == type )
		return;
	m_OutlineEffect = type;
	if ( m_FrameEffect != kFrameNoFrame )
	{
		m_FrameEffect		= kFrameNoFrame;
		m_FrameFillDesc	= RColor();		// Transparent
	}
	Redefine();
	m_BaseStringBoundingBoxes.Empty();
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::ApplyProjectionEffect( )
//
// Description:		Setup projection (3D) effect
//
// Returns:				Nothing.
//
// Exceptions:			None
//
// ****************************************************************************
//
void RHeadlineGraphic::ApplyProjectionEffect( EProjectionEffects type,
															 YPointCount nrStages,
															 YPercentage depth,
															 const RIntPoint& vanishing )
{
	TpsAssert( ( depth >= 0 && depth <= 1.0 ), "Bad projection data." );
	if ( m_ProjectionEffect == type && m_NrProjectionStages == nrStages &&
		  m_ProjectionDepth == depth && m_VanishingPoint == vanishing )
		return;
	m_ProjectionEffect = type;
	m_NrProjectionStages = nrStages;
	m_ProjectionDepth = depth;
	m_VanishingPoint = vanishing;	
	m_VanishingLength = m_VanishingPoint.Distance();
	if ( m_VanishingLength < 0 )
		m_VanishingLength = -m_VanishingLength;
	if ( m_VanishingPoint.m_y == 0 && m_VanishingPoint.m_x == 0 )
		m_ProjectionAngle = 0;
	else
		m_ProjectionAngle = ::RadiansToDegrees( atan2( (YRealDimension)m_VanishingPoint.m_y, (YRealDimension)m_VanishingPoint.m_x ) );
	switch ( type )
	{
		case kNoProjection :
			m_NrProjectionStages = 0;
			break;
		case kSimpleStack :
		case kPseudoPerspectiveStack :
			TpsAssert( ( m_NrProjectionStages > 0 ), "Bad projection data." );
			break;
		case kSpinStack :
			if ( m_ProjectionAngle < 0 )
				m_ProjectionAngle = m_ProjectionAngle + 360;
			m_ProjectionAngle = m_ProjectionAngle * m_SpinDirection;
			TpsAssert( ( m_NrProjectionStages > 0 ), "Bad projection data." );
			break;
		case kSolidExtrusion :
		case kIlluminateExtrusion :
		case kPseudoPerspectiveExtrusion :
			m_NrProjectionStages = 1;
			break;
		default :
			TpsAssertAlways( "Unrecognized projection effect type." );
			break;
	}

	if ( m_FrameEffect != kFrameNoFrame )
	{
		m_FrameEffect		= kFrameNoFrame;
		m_FrameFillDesc	= RColor();		// Transparent
	}
	if ( m_OutlineEffect == kEmbossedOutline || m_OutlineEffect == kSoftEmbossedOutline || 
		  m_OutlineEffect == kDebossedOutline || m_OutlineEffect == kSoftDebossedOutline )
	{
		if (kNoProjection != type)
			m_OutlineEffect = kSimpleStroke;
	}

	Redefine();
	m_BaseStringBoundingBoxes.Empty();
	m_ProjectionTransforms.Empty();
	TpsAssert( ( m_ProjectionDepth >= 0 ), "Bad projection data." );
	TpsAssert( ( m_VanishingLength >= 0 ), "Bad projection data." );
	TpsAssert( ( m_ProjectionAngle > -360 && m_ProjectionAngle < 360 ), "Bad projection data." );
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::ApplyShadowEffect( )
//
// Description:		Setup shadow effect
//
// Returns:				Nothing.
//
// Exceptions:			None
//
// ****************************************************************************
//
void RHeadlineGraphic::ApplyShadowEffect( EShadowEffects type,
														YPercentage depth,
														const RIntPoint& shadowing )
{
	// 06/01/98 Lance - Shadows no longer supported in headline
	// graphic, as they are no supported at the component level.
	if (kNoShadow != type)
		type = kNoShadow;	

	if ( m_ShadowEffect   == type &&
		  m_ShadowDepth    == depth && 
		  m_ShadowingPoint == shadowing )
		return;
	m_ShadowEffect = type;
	m_ShadowDepth = depth;
	m_ShadowingPoint = shadowing;	
	m_ShadowingLength = m_ShadowingPoint.Distance();
	if ( m_ShadowingLength < 0 )
		m_ShadowingLength = -m_ShadowingLength;
	if ( m_ShadowingPoint.m_y == 0 && m_ShadowingPoint.m_x == 0 )
		m_ShadowAngle = 0;
	else
		m_ShadowAngle = ::RadiansToDegrees( atan2( (YRealDimension)m_ShadowingPoint.m_y, (YRealDimension)m_ShadowingPoint.m_x ) );
	switch ( type )
	{
		case kNoShadow :
		case kDropShadow :
		case kSoftDropShadow :
			m_ShadowSlope = 0;
			break;
		case kCastShadow :
		case kSoftCastShadow :
			if ( m_ShadowAngle == 90 || m_ShadowAngle == -270 ||
				  m_ShadowAngle == -90 || m_ShadowAngle == 270 )
				m_ShadowSlope = 0.0;
			else if ( m_ShadowAngle == 0 || m_ShadowAngle == 360 || m_ShadowAngle == -360 )
				m_ShadowSlope = 1000.0;
			else if (  m_ShadowAngle == 180 || m_ShadowAngle == -180 )
				m_ShadowSlope = -1000.0;
			else 
				m_ShadowSlope = ( 1.0 / tan( ::DegreesToRadians( m_ShadowAngle ) ) );
			if ( m_ShadowSlope > 1000 )
				m_ShadowSlope = 1000.0;
			else if (  m_ShadowAngle < -1000 )
				m_ShadowSlope = -1000.0;
			break;
		default :
			TpsAssertAlways( "Unrecognized shadow effect type." );
			break;
	}

#if 0
	if ( m_FrameEffect != kFrameNoFrame )
	{
		m_FrameEffect		= kFrameNoFrame;
		m_FrameFillDesc	= RColor();			// Transparent
	}
#endif

	if ( m_OutlineEffect == kEmbossedOutline || m_OutlineEffect == kSoftEmbossedOutline || 
		  m_OutlineEffect == kDebossedOutline || m_OutlineEffect == kSoftDebossedOutline )
	{
		if (kNoShadow != type)
			m_OutlineEffect = kSimpleStroke;
	}
	Redefine();
	m_BaseStringBoundingBoxes.Empty();
	m_ProjectionTransforms.Empty();
	TpsAssert( ( m_ShadowDepth >= 0 ), "Bad shadow data." );
	TpsAssert( ( m_ShadowingLength >= 0 ), "Bad shadow data." );
	TpsAssert( ( m_ShadowAngle > -360 && m_ShadowAngle < 360 ), "Bad shadow data." );
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::ApplyFrameEffect( )
//
// Description:		Setup frame/background effect
//
// Returns:				Nothing.
//
// Exceptions:			None
//
// ****************************************************************************
//
void RHeadlineGraphic::ApplyFrameEffect( EFramedEffects type )
{
	if ( m_FrameEffect == type )
		return;
	m_FrameEffect = type;
	switch ( type )
	{
		case kFrameNoFrame :
			m_FrameFillDesc = RColor();		// Transparent;
			break;
		case kSilhouette :
			m_OutlineEffect = kSimpleFill;
			m_OutlineFillDesc = RColor(); 	// Transparent;
			break;
		case kEmboss :
			m_OutlineEffect = kEmbossedOutline;
			m_FrameFillDesc = m_OutlineFillDesc;
			break;
		case kDeboss :
			m_OutlineEffect = kDebossedOutline;
			m_FrameFillDesc = m_OutlineFillDesc;
			break;
		case kSoftEmboss :
			m_OutlineEffect = kSoftEmbossedOutline;
			m_FrameFillDesc = m_OutlineFillDesc;
			break;
		case kSoftDeboss :
			m_OutlineEffect = kSoftDebossedOutline;
			m_FrameFillDesc = m_OutlineFillDesc;
			break;
		case kSimpleFrame :
			break;
		default :
			TpsAssertAlways( "Unrecognized frame effect type." );
			break;
	}
	if ( m_FrameEffect == kEmboss || m_FrameEffect == kSoftEmboss || 
		  m_FrameEffect == kDeboss || m_FrameEffect == kSoftDeboss )
	{
		m_ProjectionEffect = kNoProjection;
		m_ShadowEffect = kNoShadow;
		Redefine();
		m_BaseStringBoundingBoxes.Empty();
		m_ProjectionTransforms.Empty();
	}
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::SetFont( )
//
// Description:		Sepecify font face/family/style of headline, size is irrelivant
//							Extract character glyph data if text is defined.
//							Extract and aply character metrics if is text defined.
//
// Returns:				Nothing.
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::SetFont( RFont*	pFont ) 
{
	TpsAssert( ( pFont != NULL ), "Bad font." );
	m_pFont = pFont;
	Redefine();
	m_FontPointSize = ::LogicalUnitToPrinterPoint( m_pFont->GetHeight() );
	TpsAssert( ( ::Round( m_FontPointSize ) == 72 ), "Unnatural font point size." );
	m_Compensation = static_cast< uWORD >( m_pFont->GetCompensationAmount() );
	m_CompensationDelta = 1.0;
	m_BaseStringBoundingBoxes.Empty();
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::SetText( )
//
// Description:		Set the text string.
//
// Returns:				Boolean indicating successfull completion.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
BOOLEAN RHeadlineGraphic::SetText( const RMBCString string ) 
{
RMBCString*				pstz = NULL;

	// clear out the old text data
	m_NrLines = 0;
	for ( RMBCStringArrayIterator sz = m_LineTextList.Start(); sz != m_LineTextList.End(); ++sz )
	{
		pstz = *sz;
		delete pstz;
	}
	m_LineTextList.Empty();

	// validate that the string is not empty
	if ( string.GetStringLength() == 0 )
		m_OriginalText = RMBCString( " " );
	else
		m_OriginalText = string;

	// reinitialize the graphic
	Redefine();
	m_BaseStringBoundingBoxes.Empty();
	return TRUE;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::ParseTextLines( )
//
// Description:		Create line info data structures.
//
// Returns:				Nothing.
//
// Exceptions:			None
//
// Notes:				Input string includes imbedded returns to indicate multiple lines.
//
// To Do:				Needs to be modified for unicode.
//
// ****************************************************************************
//
void RHeadlineGraphic::ParseTextLines( const RMBCString string ) 
{
RMBCString*					pstz = NULL;
int							count = 0;
LPSZ							line, ptr;
RMBCStringIterator		cz;
RMBCStringArrayIterator sz;
RCharacter					character, next, last;
uBYTE*						pGlobalBuffer = NULL;

	// clear out the old text data
	m_NrLines = 0;
	for ( sz = m_LineTextList.Start(); sz != m_LineTextList.End(); ++sz )
	{
		pstz = *sz;
		delete pstz;
	}
	m_LineTextList.Empty();
	pstz = NULL;

	// validate that the string is not empty
	try
	{
		if ( string.GetStringLength() == 0 )
		{
			pstz = new RMBCString( kSpace );
			m_LineTextList.InsertAtEnd( pstz );
			pstz = NULL;
			m_NrLines++;
			m_OriginalText = RMBCString( kSpace );
			return;
		}
	}
	catch ( ... )
	{
		delete pstz;
		throw;
	}
	m_OriginalText = string;

	// parse the input line
	pGlobalBuffer = ::GetGlobalBuffer();
	cz = string.Start();
	line = ptr = (LPSZ)pGlobalBuffer;
	*ptr = 0;									// paranoia
	try
	{
		// handle the special case of follow path
		if ( m_DistortEffect == kFollowPath /*|| IsVertical()*/ )
		{
			cz = string.Start();
			while ( cz != string.End() )
			{
				character = *cz;
				if ( character == kCarrageReturn || character == kLineFeed )
				{
					if ( last != kSpace )
					{
						last = kSpace;
						*ptr++ = static_cast<uBYTE>( kSpace );
					}
				}
				else if ( character.IsPrint() || ((int)character >= 0x80) )		//	must not be return or tab or some such character
				{
					if ( character.IsMultiByte() )
					{
						last = character;
						*((uWORD*)ptr)	= static_cast<uWORD>( (int)character );
						ptr += sizeof(uWORD);
					}
					else
					{
						last = character;
						*ptr++ = static_cast<uBYTE>( (int)character );
					}
				}
				++cz;
			}
			*ptr = 0;
			if ( last == kSpace )
				if ( ::strlen( line ) > 1 )
					ptr--;
			*ptr = 0;
			if ( ::strlen( line ) != 0 )
			{
				pstz = new RMBCString( line );
				m_LineTextList.InsertAtEnd( pstz );
				pstz = NULL;
				m_NrLines++;
			}
		}
		else
		{
			// divide the input string into lines
			character = kCarrageReturn;
			next = *cz;
			while ( cz != string.End() )
			{
				if ( ( count + 2 ) > kMaxHeadlineCharacters )
					break;
				character = next;
				++cz;
				if ( cz == string.End() )
					next = kSpace;
				else
					next = *cz;
				if ( character == kCarrageReturn || character == kLineFeed )
				{
					*ptr = 0;
					if ( ::strlen ( line ) == 0 )
						pstz = new RMBCString( kSpace );
					else
						pstz = new RMBCString( line );
					m_LineTextList.InsertAtEnd( pstz );
					pstz = NULL;
					m_NrLines++;
					ptr = line;
					if ( m_DistortGraphic )
					{
						while ( next == kCarrageReturn || next == kLineFeed )
						{
							++cz;
							if ( cz == string.End() )
								next = kSpace;
							else
								next = *cz;
						}
					}
					else
					{
						if ( ( character == kCarrageReturn && next == kLineFeed ) ||
							  ( next == kCarrageReturn && character == kLineFeed ) )
						{
							++cz;
							if ( cz == string.End() )
								next = kSpace;
							else
								next = *cz;
						}
					}
					if ( ( m_NrLines + 2 ) > kMaxHeadlineLines )
						break;
				}
			//	REVIEW MDH - This last check for character > 80 is not necessarily international but it may be
			//		since Windows does not support IsPrint for mbcs other than IsAscii
				else if ( character.IsPrint() || ((int)character >= 0x80) )		//	must not be return or tab or some such character
				{
					if ( character.IsMultiByte() )
					{
						*((uWORD*)ptr)	= static_cast<uWORD>( (int)character );
						ptr += sizeof(uWORD);
					}
					else
						*ptr++ = static_cast<uBYTE>( (int)character );
				}
				count++;
			}
			*ptr = 0;
			if ( ::strlen ( line ) != 0 )
			{
				pstz = new RMBCString( line );
				m_LineTextList.InsertAtEnd( pstz );
				pstz = NULL;
				m_NrLines++;
			}
		}
	}
	catch ( ... )
	{
		delete pstz;
		::ReleaseGlobalBuffer( pGlobalBuffer );
		throw;
	}
	::ReleaseGlobalBuffer( pGlobalBuffer );
	// make sure there are any characters in the headline 
	count = 0;
	sz = m_LineTextList.Start();
	for ( int k = 0; k < m_NrLines; k++ )
	{
		count += (*sz)->GetStringLength();
		++sz;
	}
	m_TextDefined = static_cast<BOOLEAN>( count != 0 );
	// reverse stings for right to left of bottom to top escapement
	if ( m_Escape == kRight2LeftTop2Bottom || m_Escape == kRight2LeftBottom2Top ||
		  m_Escape == kBottom2TopLeft2Right || m_Escape == kBottom2TopRight2Left )
		for ( sz = m_LineTextList.Start(); sz != m_LineTextList.End(); ++sz )
			(*sz)->Reverse();
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::EnableLineLeading( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::EnableLineLeading( BOOLEAN fEnable ) 
{
	if ( m_fUseLeading && !fEnable )
		m_fUseLeading = FALSE;
	else if ( !m_fUseLeading && fEnable )
		m_fUseLeading = TRUE;
	else
		return;
	Redefine();
	m_BaseStringBoundingBoxes.Empty();						// REVEIW RAH here???
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::EnableCharKerning( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::EnableCharKerning( BOOLEAN fEnable ) 
{
	if ( m_fUseKerning && !fEnable )
		m_fUseKerning = FALSE;
	else if ( !m_fUseKerning && fEnable )
		m_fUseKerning = TRUE;
	else
		return;
//	m_Compensate = kRegular;								// kerning and compensation are mutually exclusive
	Redefine();
	m_BaseStringBoundingBoxes.Empty();
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::ApplyTextEscapement( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::ApplyTextEscapement( ETextEscapement type ) 
{
	if ( m_Escape == type )
		return;
	m_Escape = type;
	// reinitialize internal headline structures
	m_fIsHorizontal = !( m_Escape == kTop2BottomLeft2Right || m_Escape == kTop2BottomRight2Left || m_Escape == kBottom2TopLeft2Right || m_Escape == kBottom2TopRight2Left );
	if ( IsVertical() )
	{
		if ( m_DistortEffect == kFollowPath )
		{
			(void)ApplyNonDistort( RHeadlineGraphic::GetDefaultHeadlineSize( IsHorizontal() ) );
			m_fUseKerning = FALSE;					
			m_Compensate = kRegular;
		}
	}
	if ( m_DistortEffect == kWarpToEnvelop )
	{
		if ( ( IsHorizontal() && IsVerticalWarpId( m_WarpShapeOrPathId ) ) ||
				( IsVertical() && IsHorizontalWarpId( m_WarpShapeOrPathId ) ) )
		{
			(void)ApplyNonDistort( RHeadlineGraphic::GetDefaultHeadlineSize( IsHorizontal() ) );
		}
	}
	Redefine();
	m_BaseStringBoundingBoxes.Empty();
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::ApplyTextJustification( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::ApplyTextJustification( ETextJustification type ) 
{
	if ( m_Justification == type )
		return;
	m_Justification = type;
	if ( m_DistortEffect == kFollowPath )
		if ( m_Justification == kFullJust )				// full justification not allowed with follow path
			m_Justification = kCenterJust;
	Redefine();				 
	m_BaseStringBoundingBoxes.Empty();
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::ApplyLineWeight( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::ApplyLineWeight( ELineWeight type ) 
{
	if ( m_LineWeight == type )
		return;
	m_LineWeight = type;
	Redefine();				 
	m_BaseStringBoundingBoxes.Empty();
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::ApplyVerticalPlacement( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::ApplyVerticalPlacement( EVerticalPlacement type ) 
{
	if ( m_Verticality == type )
		return;
	m_Verticality = type;
	Redefine();				 
	m_BaseStringBoundingBoxes.Empty();
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::ApplyCharCompensation( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::ApplyCharCompensation( ETextCompensation type ) 
{
	if ( m_Compensate == type )
		return;
	m_Compensate = type;
//	m_fUseKerning = FALSE;								// kerning and compensation are mutually exclusive
	Redefine();
	m_BaseStringBoundingBoxes.Empty();
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::AdjustCharCompensation( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::AdjustCharCompensation( YPercentage scale ) 
{
	TpsAssert( ( m_pFont != NULL ), "Bad font." );
	TpsAssert( ( scale >= 0 ), "Bad compensation adjustment." );

	m_Compensation = static_cast< uWORD >( m_pFont->GetCompensationAmount() * scale );
	m_CompensationDelta = scale;

	if ( m_Compensate != kRegular )
	{
		Redefine();
		m_BaseStringBoundingBoxes.Empty();
	}
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::AdjustLineScale( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::AdjustLineScale( uWORD line1, uWORD line2, uWORD line3 ) 
{
	m_LineScaleProportions[0] = line1;
	m_LineScaleProportions[1] = line2;
	m_LineScaleProportions[2] = line3;
	Redefine();
}


// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::ApplyNonDistort( )
//
// Description:		
//
// Returns:				Boolean indicating successfull completion.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
BOOLEAN RHeadlineGraphic::ApplyNonDistort( const RIntSize& size ) 
{
	delete m_pPathTop;
	delete m_pPathBottom;
	delete m_pPathEnvelope;
	m_pPathTop = m_pPathBottom = m_pPathEnvelope = NULL;
	m_PathRect = RIntRect( size );
	m_BoundingSize = size;
	m_DistortEffect = kNonDistort;
	m_WarpShapeOrPathId = kNoWarpShape;
	Redefine();
	m_DistortGraphic = FALSE;
	// create a composite outline shape
	m_pPathEnvelope = new RPath;
	if ( !m_pPathEnvelope->Define( m_PathRect ) )
	{
		delete m_pPathEnvelope;
		m_pPathEnvelope = NULL;
	}
	return TRUE;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::ApplyShearEffect( )
//
// Description:		
//
// Returns:				Nothing.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::ApplyShearEffect( YAngle shear ) 
{
YAngle			angle;

	delete m_pPathTop;
	delete m_pPathBottom;
	delete m_pPathEnvelope;
	m_pPathTop = m_pPathBottom = m_pPathEnvelope = NULL;
	m_PathRect = RIntRect( m_BoundingSize );
	m_DistortEffect = kShear;
	m_ShearAngle = shear;
	m_ShearRotaion = 0.0;
	Redefine();
	m_DistortGraphic = FALSE;
	// calc slope of shear angle
	TpsAssert( ( m_ShearAngle > -90 && m_ShearAngle < 90 ), "Bad shear data." );
	TpsAssert( ( m_ShearRotaion > -90 && m_ShearRotaion < 90 ), "Bad shear data." );
	if ( m_ShearAngle == 0 )
		m_ShearSlope = 0.0;
	else
	{
		angle = ::DegreesToRadians( ( m_ShearAngle > 0 ) ? ( 90 - m_ShearAngle ) : ( 90 + m_ShearAngle ) );
		m_ShearSlope = ( 1.0 / tan( angle ) ) * ( ( m_ShearAngle > 0 ) ? 1.0 : -1.0 );
	}
	// create a composite outline shape
	if ( m_PathRect.IsEmpty() ) 
		m_PathRect = RIntRect( GetDefaultHeadlineSize( IsHorizontal() ) );
	m_pPathEnvelope = new RPath;
	if ( !m_pPathEnvelope->Define( m_PathRect ) )
	{
		delete m_pPathEnvelope;
		m_pPathEnvelope = NULL;
	}
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::ApplyAltShearEffect( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::ApplyAltShearEffect( YAngle shear, YAngle rotate ) 
{
YAngle			angle;

	delete m_pPathTop;
	delete m_pPathBottom;
	delete m_pPathEnvelope;
	m_pPathTop = m_pPathBottom = m_pPathEnvelope = NULL;
	m_PathRect = RIntRect( m_BoundingSize );
	m_DistortEffect = kAltShearRotate;
	m_ShearAngle = shear;
	m_ShearRotaion = rotate;
	Redefine();
	m_DistortGraphic = FALSE;
	// calc slope of shear angle
	TpsAssert( ( m_ShearAngle > -90 && m_ShearAngle < 90 ), "Bad shear data." );
	TpsAssert( ( m_ShearRotaion > -90 && m_ShearRotaion < 90 ), "Bad shear data." );
	if ( m_ShearAngle == 0 )
		m_ShearSlope = 0.0;
	else
	{
		angle = ::DegreesToRadians( ( m_ShearAngle > 0 ) ? ( 90 - m_ShearAngle ) : ( 90 + m_ShearAngle ) );
		m_ShearSlope = ( 1.0 / tan( angle ) ) * ( ( m_ShearAngle > 0 ) ? 1.0 : -1.0 );
	}
	// create a composite outline shape
	if ( m_PathRect.IsEmpty() ) 
		m_PathRect = RIntRect( GetDefaultHeadlineSize( IsHorizontal() ) );
	m_pPathEnvelope = new RPath;
	if ( !m_pPathEnvelope->Define( m_PathRect ) )
	{
		delete m_pPathEnvelope;
		m_pPathEnvelope = NULL;
	}
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::ApplyAltShearEffect( )
//
// Description:		
//
// Returns:				Nothing.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::ApplyAltShearEffect( YAngle shear ) 
{
YAngle			angle;

	delete m_pPathTop;
	delete m_pPathBottom;
	delete m_pPathEnvelope;
	m_pPathTop = m_pPathBottom = m_pPathEnvelope = NULL;
	m_PathRect = RIntRect( m_BoundingSize );
	m_DistortEffect = kAltShear;
	m_ShearAngle = shear;
	m_ShearRotaion = 0.0;
	Redefine();
	m_DistortGraphic = FALSE;
	// calc slope of shear angle
	TpsAssert( ( m_ShearAngle > -90 && m_ShearAngle < 90 ), "Bad shear data." );
	TpsAssert( ( m_ShearRotaion > -90 && m_ShearRotaion < 90 ), "Bad shear data." );
	if ( m_ShearAngle == 0 )
		m_ShearSlope = 0.0;
	else
	{
		angle = ::DegreesToRadians( ( m_ShearAngle > 0 ) ? ( 90 - m_ShearAngle ) : ( 90 + m_ShearAngle ) );
		m_ShearSlope = ( 1.0 / tan( angle ) ) * ( ( m_ShearAngle > 0 ) ? 1.0 : -1.0 );
	}
	// create a composite outline shape
	if ( m_PathRect.IsEmpty() ) 
		m_PathRect = RIntRect( GetDefaultHeadlineSize( IsHorizontal() ) );
	m_pPathEnvelope = new RPath;
	if ( !m_pPathEnvelope->Define( m_PathRect ) )
	{
		delete m_pPathEnvelope;
		m_pPathEnvelope = NULL;
	}
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::ApplyAlternateBaselineEffect( )
//
// Description:		
//
// Returns:				Nothing.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::ApplyAlternateBaselineEffect( YPercentage scale ) 
{
	delete m_pPathTop;
	delete m_pPathBottom;
	delete m_pPathEnvelope;
	m_pPathTop = m_pPathBottom = m_pPathEnvelope = NULL;
	m_PathRect = RIntRect( m_BoundingSize );
	m_DistortEffect = kAltBaseline;
	m_BaselineAlternate = scale,
	Redefine();
	TpsAssert( ( m_BaselineAlternate >= 0 && m_BaselineAlternate <= 1 ), "Bad baseline alternate data." );
	m_DistortGraphic = FALSE;
	// create a composite outline shape
	if ( m_PathRect.IsEmpty() ) 
		m_PathRect = RIntRect( GetDefaultHeadlineSize( IsHorizontal() ) );
	m_pPathEnvelope = new RPath;
	if ( !m_pPathEnvelope->Define( m_PathRect ) )
	{
		delete m_pPathEnvelope;
		m_pPathEnvelope = NULL;
	}
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::ApplyDistort( )
//
// Description:		
//
// Returns:				Boolean indicating successfull completion.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
BOOLEAN RHeadlineGraphic::ApplyDistort( BOOLEAN fEnable ) 
{
	if ( m_DistortEffect == kFollowPath )
		return static_cast<BOOLEAN>( !fEnable );				// follow path can only be nondistort
	else if ( m_DistortEffect == kWarpToEnvelop )
		return fEnable;												// warp to envelop can only be distort
	else if ( fEnable )
	{
		if ( !m_DistortGraphic )
		{
			delete m_pPathTop;
			delete m_pPathBottom;
			delete m_pPathEnvelope;
			m_pPathTop = m_pPathBottom = m_pPathEnvelope = NULL;
			m_PathRect = RIntRect( GetDefaultHeadlineSize( IsHorizontal() ) );
			if ( m_DistortEffect == kFollowPath )
				m_DistortEffect = kNonDistort;
			Redefine();
			ApplyVerticalPlacement( kPin2Middle );
			m_DistortGraphic = TRUE;
			// create a composite outline shape
			m_pPathEnvelope = new RPath;
			if ( !m_pPathEnvelope->Define( m_PathRect ) )
			{
				delete m_pPathEnvelope;
				m_pPathEnvelope = NULL;
			}
		}
	}
	else if ( m_DistortGraphic )
	{
		Redefine();
		m_DistortGraphic = FALSE;
	}
	return TRUE;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::NotDisplayEmptyEnvelop( )
//
// Description:		
//
// Returns:				Nothing.
//
// Exceptions:			None
//
// ****************************************************************************
//
void RHeadlineGraphic::NotDisplayEmptyEnvelop( BOOLEAN fEnable ) 
{
	m_NotDisplayEmptyEnvelop = fEnable;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::ApplyPathToWarp( )
//
// Description:		
//
// Returns:				Boolean indicating successfull completion.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
BOOLEAN RHeadlineGraphic::ApplyPathToWarp( sWORD warpId ) 
{
BOOLEAN					success = FALSE;
sWORD						swWarpResId	= warpId;
uWORD						orientation, placement;
RIntPointArray			pArray( kPathArraySize );
RIntPointArray			p1Array( kPathArraySize );
RIntPointArray			p2Array( kPathArraySize );
EPathOperatorArray	oArray( kPathArraySize );
EPathOperatorArray	o1Array( kPathArraySize );
EPathOperatorArray	o2Array( kPathArraySize );
RIntPoint				ptStart, ptEnd;

#ifndef	IMAGER
	swWarpResId	+= WARP_PATH_START_VALUE;
#endif

	if ( warpId == kNoWarpShape )
	{
		success = ApplyNonDistort( RHeadlineGraphic::GetDefaultHeadlineSize( IsHorizontal() ) );
		m_WarpShapeOrPathId = warpId;
		return success;	 
	}
	else if ( warpId > kNoWarpShape && warpId < kWarpShape1 )
	{
		success = ApplyNonDistort( RHeadlineGraphic::GetDefaultHeadlineSize( IsHorizontal() ) );
		if ( success )
			success = ApplyDistort( TRUE );
		m_WarpShapeOrPathId = warpId;
		return success;	 
	}
	else if ( warpId < kWarpShapeLast )
	{
		success = LoadWarpPathData( swWarpResId, &pArray, &oArray, &p1Array, &o1Array, &p2Array, &o2Array, &orientation, &placement );
		if ( success )
			success = CreatePathToWarp( o1Array, p1Array, o2Array, p2Array );
	}

	if ( success )
	{
		m_DistortEffect = kWarpToEnvelop;
		m_WarpShapeOrPathId = warpId;
		// validations
#ifdef	TPSDEBUG
		TpsAssert( ( m_pPathTop != NULL ), "Bad warp path." );
		TpsAssert( ( m_pPathBottom != NULL ), "Bad warp path." );
		TpsAssert( ( m_pPathTop->GetOutlineCount() == 1 ), "Bad warp path." );
		TpsAssert( ( m_pPathBottom->GetOutlineCount() == 1 ), "Bad warp path." );
		if ( warpId != kNoWarpShape )
			if ( IsHorizontalWarpId( warpId ) )
			{
				TpsAssert( ( orientation == 1 ), "Bad warp path data." );
				TpsAssert( IsHorizontal(), "Bad warp path data." );
			}
			else if ( IsVerticalWarpId( warpId ) )
			{
				TpsAssert( ( orientation == 2 ), "Bad warp path data." );
				TpsAssert( IsVertical(), "Bad warp path data." );
			}
		   else
			{
				TpsAssertAlways( "Bad warp path data." );
			}
#endif
		// distortion paths must go in a positive direction
		ptStart = m_pPathTop->GetStartPoint();
		ptEnd = m_pPathTop->GetEndPoint();
		if ( IsHorizontalWarpId( warpId ) )		 
			if ( ptStart.m_x > ptEnd.m_x )
				m_pPathTop->Reverse();
		if ( IsVerticalWarpId( warpId ) )	 
			if ( ptStart.m_y > ptEnd.m_y )
				m_pPathTop->Reverse();
		ptStart = m_pPathBottom->GetStartPoint();
		ptEnd = m_pPathBottom->GetEndPoint();
		if ( IsHorizontalWarpId( warpId ) )		
			if ( ptStart.m_x > ptEnd.m_x )
				m_pPathBottom->Reverse();
		if ( IsVerticalWarpId( warpId ) )		
			if ( ptStart.m_y > ptEnd.m_y )
				m_pPathBottom->Reverse();
#if	0
#ifdef	TPSDEBUG
		if ( warpId != kNoWarpShape )
			if ( IsHorizontal() )
			{
				ptStart = m_pPathTop->GetStartPoint();
				ptEnd = m_pPathTop->GetEndPoint();
				TpsAssert( ( ptStart.m_x < 10 ), "Top warp path does not start at zero." );
				TpsAssert( ( ptEnd.m_x > 7190 ), "Top warp path not wide enough." );
				ptStart = m_pPathBottom->GetStartPoint();
				ptEnd = m_pPathBottom->GetEndPoint();
				TpsAssert( ( ptStart.m_x < 10 ), "Bottom warp path does not start at zero." );
				TpsAssert( ( ptEnd.m_x > 7190 ), "Bottom warp path not wide enough." );
			}
			else if ( IsVertical() )
			{
				ptStart = m_pPathTop->GetStartPoint();
				ptEnd = m_pPathTop->GetEndPoint();
				TpsAssert( ( ptStart.m_y < 10 ), "Top warp path does not start at zero." );
				TpsAssert( ( ptEnd.m_y > 7190 ), "Top warp path not tall enough." );
				ptStart = m_pPathBottom->GetStartPoint();
				ptEnd = m_pPathBottom->GetEndPoint();
				TpsAssert( ( ptStart.m_y < 10 ), "Bottom warp path does not start at zero." );
				TpsAssert( ( ptEnd.m_y > 7190 ), "Bottom warp path not tall enough." );
			}
#endif
#endif
		// apply placement
		switch ( placement ) 
		{
		   case 2 :
				ApplyVerticalPlacement( kPin2Top );
			  	break;
		   case 3 :
				ApplyVerticalPlacement( kPin2Bottom );
			  	break;
		   case 1 :
				ApplyVerticalPlacement( kPin2Middle );
			  	break;
		   case 4 :
				ApplyVerticalPlacement( kPin2Left );
			  	break;
		   case 5 :
				ApplyVerticalPlacement( kPin2Right );	
			  	break;
		   default:
				TpsAssertAlways( "Bad warp path data." );
			  	break;
		}
		// create a composite outline shape
		delete m_pPathEnvelope;
		m_pPathEnvelope = new RPath;
		if ( !m_pPathEnvelope->Define( oArray, pArray ) )
		{
			delete m_pPathEnvelope;
			m_pPathEnvelope = NULL;
		}
		// reinitalize the graphic
		m_DistortGraphic = TRUE;
		Redefine();
	}

	return success;	 
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CreatePathToWarp( )
//
// Description:		
//
// Returns:				Boolean indicating successfull completion.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
BOOLEAN RHeadlineGraphic::CreatePathToWarp( const RIntPoint* pVertices, 
														  YPointCount nrVertices, 
														  const RIntPoint* pVertices2, 
													  	  YPointCount nrVertices2 ) 
{
RPath*		path1 = NULL;
RPath*		path2 = NULL;
BOOLEAN		defined = FALSE;

	try
	{
		path1 = new RPath;
		path2 = new RPath;
		if ( path1->Define( pVertices, &nrVertices, 1, FALSE ) && 
			  path2->Define( pVertices2, &nrVertices2, 1, FALSE ) )
		{
			delete m_pPathTop;
			delete m_pPathBottom;
			m_pPathTop = path1;
			m_pPathBottom = path2;
			path1 = path2 = NULL;
			defined = TRUE;
		}
	}
	catch ( ... )
	{
		delete path1;
		delete path2;
		throw;
	}
	delete path1;
	delete path2;
	return defined;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CreatePathToWarp( )
//
// Description:		
//
// Returns:				Boolean indicating successfull completion.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
BOOLEAN RHeadlineGraphic::CreatePathToWarp( EPathOperator* pOperators, 
														  const RIntPoint* pVertices, 
														  EPathOperator* pOperators2, 
														  const RIntPoint* pVertices2 ) 
{
RPath*						path1 = NULL;
RPath*						path2 = NULL;
BOOLEAN						defined = FALSE;

	try
	{
		path1 = new RPath;
		path2 = new RPath;
		if ( path1->Define( pOperators, pVertices ) && 
			  path2->Define( pOperators2, pVertices2 ) )
		{
			delete m_pPathTop;
			delete m_pPathBottom;
			m_pPathTop = path1;
			m_pPathBottom = path2;
			path1 = path2 = NULL;
			defined = TRUE;
		}
	}
	catch ( ... )
	{
		delete path1;
		delete path2;
		throw;
	}
	delete path1;
	delete path2;
	return defined;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::ApplyPathToFollow( )
//
// Description:		
//
// Returns:				Boolean indicating successfull completion.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
BOOLEAN RHeadlineGraphic::ApplyPathToFollow( sWORD warpId, YPercentage maxScale ) 
{
BOOLEAN					success = FALSE;
sWORD						swWarpResId	= warpId;
RIntPointArray			pArray( kPathArraySize );

#ifndef	IMAGER
	swWarpResId	+= WARP_PATH_START_VALUE;
#endif

	switch ( warpId ) 
	{
	   case kNoWarpShape:
			success = ApplyNonDistort( RHeadlineGraphic::GetDefaultHeadlineSize( IsHorizontal() ) );
		  	break;
	   case kWarpPath00:
			success = ApplyNonDistort( RHeadlineGraphic::GetDefaultHeadlineSize( IsHorizontal() ) );
			if ( success )
				success = ApplyDistort( FALSE );
		  	break;
	   case kWarpPath10 :
	   case kWarpPath20 :
	   case kWarpPath30 :
	   case kWarpPath40 :
	   case kWarpPath50 :
	   case kWarpPath60 :
	   case kWarpPath70 :
	   case kWarpPath80 :
	   case kWarpPath90 :
	   case kWarpPath100 :
	   case kWarpPath110 :
	   case kWarpPath120 :
	   case kWarpPath130 :
			if ( !LoadFollowPathData( swWarpResId, &pArray ) )
				return FALSE;
			success = CreatePathToFollow( pArray, pArray.Count(), TRUE );
			if ( success )
				m_WarpShapeOrPathId = warpId;
		  	break;
	   default:
			success = FALSE;
		  	break;
	}

	if ( success && warpId != kNoWarpShape )
	{
		TpsAssert( ( m_pPathTop == NULL ), "Bad follow path." );
		TpsAssert( ( m_pPathBottom != NULL ), "Bad follow path." );
		TpsAssert( ( m_pPathBottom->GetOutlineCount() == 1 ), "Bad follow path." );
		TpsAssert( ( maxScale > 0 && maxScale <= 1 ), "Bad follow path max scale factor." );
		AdjustLineScale( 1, 1, 1 );
		ApplyVerticalPlacement( kPin2Middle );
		ApplyTextEscapement( kLeft2RightTop2Bottom );
		m_MaxPathHeightScale = maxScale;
		m_DistortGraphic = FALSE;
		Redefine();
	}

	return success;	 
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CreatePathToFollow( )
//
// Description:		
//
// Returns:				Boolean indicating successfull completion.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
BOOLEAN RHeadlineGraphic::CreatePathToFollow( const RIntPoint* pPoints, YPointCount nrPoints, BOOLEAN fDefineEnvelop ) 
{
RPath*				path = NULL;
RIntPoint* 			points = NULL;
BOOLEAN				defined = FALSE;
RIntPoint			ptStart, ptEnd;
YPointCount 		nr = nrPoints;

	if ( nrPoints < 2 )
		return FALSE;
	try
	{
		path = new RPath;
		if ( ( nrPoints & 0x00001 ) == 0 )
		{
			defined = path->Define( pPoints, &nr, 1, FALSE );
		}
		else
		{
			defined = path->DefineFromArcs( pPoints, &nr, 1 );
		}
		if ( defined )
		{
			m_PathRadius = 0;
			m_PathType = kPathStraight;
			m_PathAngle = 0;
			if ( ( nrPoints & 0x00001 ) != 0 )
			{
				RRealPoint	pt0 = pPoints[0];
				RRealPoint	pt1 = pPoints[1];
				RRealPoint	pt2 = pPoints[2];
				RRealPoint	pt3;
				YAngle		ang;
				if ( ComputeArcMetrics( pt0, pt1, pt2, pt3, ang ) )
				{
					m_PathRadius = pt0.Distance( pt3 );
					m_PathAngle = ::RadiansToDegrees( ang );		// REVEIW RAH ang is wrong!!!
					if ( nrPoints > 3 )
						m_PathType = kPathCurved;
					else if ( m_PathAngle > 0 )
						m_PathType = kPathConcave;
					else if ( m_PathAngle < 0 )
						m_PathType = kPathConvex;
				}
			}
			delete m_pPathTop;
			delete m_pPathBottom;
			m_pPathTop = NULL;
			m_pPathBottom = path;
			m_DistortEffect = kFollowPath;
			if ( m_Justification == kFullJust )				// full justification not allowed with follow path
				m_Justification = kCenterJust;
			path = NULL;
			defined = TRUE;
			// follow paths must go in a positive direction
			ptStart = m_pPathBottom->GetStartPoint();
			ptEnd = m_pPathBottom->GetEndPoint();
			if ( ptStart.m_x > ptEnd.m_x )
			{
				m_pPathBottom->Reverse();
				if ( m_PathType == kPathConcave )
					m_PathType = kPathConvex;
				else if ( m_PathType == kPathConvex )
					m_PathType = kPathConcave;
				m_PathAngle = -m_PathAngle;
			}
			// create a composite outline shape
			if ( fDefineEnvelop )
			{
			RIntRect				rcBounds( GetDefaultHeadlineSize( TRUE ) );
			const RIntPoint*	p1 = pPoints;
			RIntPoint* 			p2 = new RIntPoint[ ( nrPoints * 2 ) + 4 ];
			YIntDimension 		yDelta = 0;
			BOOLEAN				result;
			int 					i;
				points = p2;
				for ( i = 0; i < nrPoints; i++ )
				{
					if ( p1->m_y > yDelta )
						yDelta = p1->m_y;
					*p2 = *p1;
					p1++;
					p2++;
				}
				yDelta = rcBounds.Height() - yDelta;
				p1--;
				*p2 = *p1;
				p2->m_y += ( yDelta / 2 );
				p2++;
				for ( i = 0; i < nrPoints; i++ )
				{
					*p2 = *p1;
					p2->m_y += yDelta;
					p1--;
					p2++;
				}
				*p2 = *pPoints;
				p2->m_y += ( yDelta / 2 );
				p2++;
				*p2 = *pPoints;
				delete m_pPathEnvelope;
				m_pPathEnvelope = new RPath;
				nr = ( nrPoints * 2 ) + 3;
				if ( ( nrPoints & 0x00001 ) == 0 )
					result = m_pPathEnvelope->Define( points, &nr, 1, TRUE );
				else
					result = m_pPathEnvelope->DefineFromArcs( points, &nr, 1 );
				if ( !result )
				{
					delete m_pPathEnvelope;
					m_pPathEnvelope = NULL;
				}
			}
		}
	}
	catch ( ... )
	{
		delete [] points;
		delete path;
		throw;
	}
	delete [] points;
	delete path;
	return defined;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::SetWindingDirection( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::SetWindingDirection( YIntDimension direction ) 
{
	m_SpinDirection = direction;
	TpsAssert( ( m_SpinDirection == 1 || m_SpinDirection == 0 || m_SpinDirection == -1 ), "Bad projection data." );
	if ( m_ProjectionEffect == kSpinStack )
	{
		Redefine();
		m_BaseStringBoundingBoxes.Empty();
		m_ProjectionTransforms.Empty();
	}
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::SetOutlineStroke( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::SetOutlineStroke( ) 
{
	UnimplementedCode();
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::SetOutlineStroke( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::SetOutlineStroke( const RSolidColor& color ) 
{
	m_OutlineStrokeColor = color;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::SetOutlineFill( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::SetOutlineFill( EOutlineEffects effect, const RHeadlineColor& fill ) 
{
	ApplyOutlineEffect( effect );
	m_OutlineFillDesc	= fill;
	if( m_FrameEffect == kEmboss || m_FrameEffect == kSoftEmboss || 
		 m_FrameEffect == kDeboss || m_FrameEffect == kSoftDeboss )
		{
		m_FrameFillDesc = fill;
		}
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::SetOutlineHilite( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::SetOutlineHilite( const RSolidColor& color ) 
{
	m_OutlineHiliteColor = color;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::SetOutlineShadow( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::SetOutlineShadow( const RSolidColor& color ) 
{
	m_OutlineShadowColor = color;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::SetProjectionFill( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::SetProjectionFill( EProjectionEffects /*effect*/, const RHeadlineColor& fill ) 
{
	TpsAssert( !fill.IsTransparent(), "Bad projection fill type." );
	TpsAssert( !fill.IsTranslucent(), "Bad projection fill type." );
	TpsAssert( !fill.IsBitmap(), "Bad projection fill type." );
	m_ProjectionFillDesc = fill;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::SetShadowFill( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::SetShadowFill( EShadowEffects /*effect*/, const RHeadlineColor& fill ) 
{
	TpsAssert( !fill.IsTransparent(), "Bad shadow fill type." );
	TpsAssert( !fill.IsTranslucent(), "Bad shadow fill type." );
	TpsAssert( !fill.IsBitmap(), "Bad shadow fill type." );
	m_ShadowFillDesc = fill;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::SetFrameFill( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::SetFrameFill( EFramedEffects effect, const RHeadlineColor& fill ) 
{
	ApplyFrameEffect( effect );
	m_FrameFillDesc = fill;
	if ( m_FrameEffect == kEmboss || m_FrameEffect == kSoftEmboss || 
		  m_FrameEffect == kDeboss || m_FrameEffect == kSoftDeboss )
	{
		m_OutlineFillDesc = fill;
		YAngle	angle	= -m_OutlineFillDesc.GetAngle();
//		angle	=	kPI - angle;
		m_OutlineFillDesc.SetAngle( angle );
	}
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::SetFrameHiliteColor( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::SetFrameHilite( EFramedEffects effect, const RSolidColor& color ) 
{
	if ( effect == kEmboss || effect == kSoftEmboss || 
		  effect == kDeboss || effect == kSoftDeboss )
	{
		SetOutlineHilite( color );
	}
	else
	{
		NULL;	//	There will be something here eventually
	}
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::SetFrameShadowColor( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::SetFrameShadow( EFramedEffects effect, const RSolidColor& color ) 
{
	if ( effect == kEmboss || effect == kSoftEmboss || 
		  effect == kDeboss || effect == kSoftDeboss )
	{
		SetOutlineShadow( color );
	}
	else
	{
		NULL; 	//	There will be something here eventually
	}
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::SetBlur2Background( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::SetBlur2Background( const RSolidColor& color ) 
{
	m_BackgroundBlur2Color = color;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::SetShadow2Background( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::SetShadow2Background( const RSolidColor& color ) 
{
	m_BackgroundShadow2Color = color;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::SetTrimWhiteSpace( )
//
// Description:		Sets the m_TrimWhiteSpace flag
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RHeadlineGraphic::SetTrimWhiteSpace( BOOLEAN fTrimWhiteSpace )
{
	m_TrimWhiteSpace = fTrimWhiteSpace;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetFont( )
//
// Description:		Return the font associated with this headline
//
// Returns:				m_pFont
//
// Exceptions:			None
//
//
// ****************************************************************************
//
RFont* RHeadlineGraphic::GetFont( ) const
{
	return m_pFont;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetText( )
//
// Description:		Get the text string.
//
// Returns:				m_OriginalText
//
// Exceptions:			None
//
// ****************************************************************************
//
RMBCString RHeadlineGraphic::GetText( ) const
{
	return m_OriginalText;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetCharCompensation( )
//
// Description:		Return value of m_Compensate
//
// Returns:				m_Compensate
//
// Exceptions:			Nothing
//
// ****************************************************************************
//
ETextCompensation	RHeadlineGraphic::GetCharCompensation( ) const
{
	return m_Compensate;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetCharCompensation( )
//
// Description:		Return value of m_CompensationDelta
//
// Returns:				m_CompensationDelta
//
// Exceptions:			Nothing
//
// ****************************************************************************
//
YPercentage	RHeadlineGraphic::GetCompensationAdjustment( ) const
{
	return m_CompensationDelta;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetTextEscapement( )
//
// Description:		return value of m_Escape
//
// Returns:				m_Escape
//
// Exceptions:			Nothing
//
// ****************************************************************************
//
ETextEscapement RHeadlineGraphic::GetTextEscapement( ) const
{
	return m_Escape;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetTextJustification( )
//
// Description:		Return m_Justification
//
// Returns:				m_Justification
//
// Exceptions:			Nothing
//
// ****************************************************************************
//
ETextJustification RHeadlineGraphic::GetTextJustification( ) const
{
	return m_Justification;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetLineWeight( )
//
// Description:		Return m_LineWeight
//
// Returns:				m_LineWeight
//
// Exceptions:			Nothing
//
// ****************************************************************************
//
ELineWeight RHeadlineGraphic::GetLineWeight( ) const
{
	return m_LineWeight;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetLineScales( )
//
// Description:		Return the current line scale proportions
//
// Returns:				Nothing
//
// Exceptions:			Nothing
//
// ****************************************************************************
//
void RHeadlineGraphic::GetLineScales( uWORD& line1, uWORD& line2, uWORD& line3 ) const
{
	line1	= m_LineScaleProportions[0];
	line2 = m_LineScaleProportions[1];
	line3 = m_LineScaleProportions[2];
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetOutlineEffect( )
//
// Description:		return value of m_OutlineEffect
//
// Returns:				EOutlineEffects
//
// Exceptions:			Nothing
//
// ****************************************************************************
//
EOutlineEffects RHeadlineGraphic::GetOutlineEffect( ) const
{
	return m_OutlineEffect;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetDistortEffect( )
//
// Description:		return value of m_DistortEffect
//
// Returns:				EDistortEffects
//
// Exceptions:			Nothing
//
// ****************************************************************************
//
EDistortEffects RHeadlineGraphic::GetDistortEffect( ) const
{
	return m_DistortEffect;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetProjectionEffect( )
//
// Description:		return value of m_ProjectionEffect
//
// Returns:				EProjectionEffects
//
// Exceptions:			Nothing
//
// ****************************************************************************
//
EProjectionEffects RHeadlineGraphic::GetProjectionEffect( ) const
{
	return m_ProjectionEffect;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetShadowEffect( )
//
// Description:		return Shadow Effect Value
//
// Returns:				m_ShadowEffect
//
// Exceptions:			Nothing
//
// ****************************************************************************
//
EShadowEffects RHeadlineGraphic::GetShadowEffect( ) const
{
	return m_ShadowEffect;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetFrameEffect( )
//
// Description:		return the Frame Effect
//
// Returns:				m_FrameEffect
//
// Exceptions:			Nothing
//
// ****************************************************************************
//
EFramedEffects RHeadlineGraphic::GetFrameEffect( ) const
{
	return m_FrameEffect;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetPathToWarpOrFollowId( )
//
// Description:		return the warp path Id
//
// Returns:				m_WarpShapeOrPathId
//
// Exceptions:			Nothing
//
// ****************************************************************************
//
uWORD RHeadlineGraphic::GetPathToWarpId( ) const
{
	if ( m_DistortEffect == kWarpToEnvelop )
		return m_WarpShapeOrPathId;
	return kNoWarpShape;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetPathToFollow( )
//
// Description:		return the follow path Id
//
// Returns:				m_WarpShapeOrPathId
//
// Exceptions:			Nothing
//
// ****************************************************************************
//
uWORD RHeadlineGraphic::GetPathToFollowId( ) const
{
	if ( m_DistortEffect == kFollowPath )
		return m_WarpShapeOrPathId;
	return kNoWarpShape;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetVerticalPlacement( )
//
// Description:		return m_Verticality
//
// Returns:				EVerticalPlacement
//
// Exceptions:			None
//
//
// ****************************************************************************
//
EVerticalPlacement RHeadlineGraphic::GetVerticalPlacement( ) const
{
	return m_Verticality;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetOutlineStrokeColor( )
//
// Description:		
//
// Returns:				RSolidColor
//
// Exceptions:			None
//
//
// ****************************************************************************
//
RSolidColor RHeadlineGraphic::GetOutlineStrokeColor( ) const
{
	return m_OutlineStrokeColor;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetOutlineFill( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::GetOutlineFill( RHeadlineColor& fill ) const
{
	fill = m_OutlineFillDesc;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetOutlineHiliteColor( )
//
// Description:		
//
// Returns:				RSolidColor
//
// Exceptions:			None
//
//
// ****************************************************************************
//
RSolidColor RHeadlineGraphic::GetOutlineHiliteColor( ) const
{
	return m_OutlineHiliteColor;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetOutlineShadowColor( )
//
// Description:		
//
// Returns:				RSolidColor
//
// Exceptions:			None
//
//
// ****************************************************************************
//
RSolidColor RHeadlineGraphic::GetOutlineShadowColor( ) const
{
	return m_OutlineShadowColor;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetFrameFill( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::GetFrameFill( RHeadlineColor& fill ) const
{
	fill = m_FrameFillDesc;
}


// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetFrameHiliteColor( )
//
// Description:		
//
// Returns:				RSolidColor
//
// Exceptions:			None
//
//
// ****************************************************************************
//
RSolidColor RHeadlineGraphic::GetFrameHiliteColor( ) const
{
	return m_OutlineHiliteColor;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetFrameShadowColor( )
//
// Description:		
//
// Returns:				RSolidColor
//
// Exceptions:			None
//
//
// ****************************************************************************
//
RSolidColor RHeadlineGraphic::GetFrameShadowColor( ) const
{
	return m_OutlineShadowColor;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetBlur2BackgroundColor( )
//
// Description:		
//
// Returns:				RSolidColor
//
// Exceptions:			None
//
//
// ****************************************************************************
//
RSolidColor RHeadlineGraphic::GetBlur2BackgroundColor( ) const
{
	return m_BackgroundBlur2Color;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetShadow2BackgroundColor( )
//
// Description:		
//
// Returns:				RSolidColor
//
// Exceptions:			None
//
//
// ****************************************************************************
//
RSolidColor RHeadlineGraphic::GetShadow2BackgroundColor( ) const
{
	return m_BackgroundShadow2Color;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetShearAngle( )
//
// Description:		Return the Shear Angle
//
// Returns:				m_ShearAngle
//
// Exceptions:			None
//
//
// ****************************************************************************
//
YAngle RHeadlineGraphic::GetShearAngle( ) const
{
	return m_ShearAngle;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetShearRotation( )
//
// Description:		Return the Rotation amount for the shear
//
// Returns:				m_ShearRotation
//
// Exceptions:			None
//
//
// ****************************************************************************
//
YAngle RHeadlineGraphic::GetShearRotation( ) const
{
	return m_ShearRotaion;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetBaselineAlternate( )
//
// Description:		Return the Alternate amount for the baseline
//
// Returns:				m_BaselineAlternate
//
// Exceptions:			None
//
//
// ****************************************************************************
//
YPercentage RHeadlineGraphic::GetBaselineAlternate( ) const
{
	return m_BaselineAlternate;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetNumStages( )
//
// Description:		Return the Number of Stages
//
// Returns:				m_NrProjectionStages
//
// Exceptions:			None
//
//
// ****************************************************************************
//
YPointCount RHeadlineGraphic::GetNumStages( ) const
{
	return m_NrProjectionStages;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetProjectionDepth( )
//
// Description:		Return the Projection Depth
//
// Returns:				m_ProjectionDepth
//
// Exceptions:			None
//
//
// ****************************************************************************
//
YPercentage RHeadlineGraphic::GetProjectionDepth( ) const
{
	return m_ProjectionDepth;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetVanishingPoint( )
//
// Description:		Return the Vanishing Point
//
// Returns:				m_VanishingPoint
//
// Exceptions:			None
//
//
// ****************************************************************************
//
RIntPoint RHeadlineGraphic::GetVanishingPoint( ) const
{
	return m_VanishingPoint;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetShadowDepth( )
//
// Description:		Return the Shadow Depth
//
// Returns:				m_ShadowDepth
//
// Exceptions:			None
//
//
// ****************************************************************************
//
YPercentage RHeadlineGraphic::GetShadowDepth( ) const
{
	return m_ShadowDepth;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetMaxPathHeightScale( )
//
// Description:		Return the max percentage of follow path scale
//
// Returns:				m_MaxPathHeightScale
//
// Exceptions:			None
//
//
// ****************************************************************************
//
YPercentage RHeadlineGraphic::GetMaxPathHeightScale( ) const
{
	return m_MaxPathHeightScale;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetNumLines( )
//
// Description:		Return the number of text lines in the headline graphic
//
// Returns:				m_NrLines
//
// Exceptions:			None
//
//
// ****************************************************************************
//
uWORD RHeadlineGraphic::GetNumLines( ) const
{
	return m_NrLines;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetShadowingPoint( )
//
// Description:		Return the Shadowing Point
//
// Returns:				m_ShadowingPoint
//
// Exceptions:			None
//
//
// ****************************************************************************
//
RIntPoint RHeadlineGraphic::GetShadowingPoint( ) const
{
	return m_ShadowingPoint;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetLightSourcePoint( )
//
// Description:		Return the Light Source Point
//
// Returns:				m_LightSourcePoint
//
// Exceptions:			None
//
//
// ****************************************************************************
//
RIntPoint RHeadlineGraphic::GetLightSourcePoint( ) const
{
	return m_LightSourcePoint;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetProjectionFill( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::GetProjectionFill( RHeadlineColor& fill ) const
{
	fill = m_ProjectionFillDesc;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetShadowFill( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::GetShadowFill( RHeadlineColor& fill ) const
{
	fill = m_ShadowFillDesc;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetWindingDirection( )
//
// Description:		Return the winding direction
//
// Returns:				m_SpinDirection
//
// Exceptions:			None
//
//
// ****************************************************************************
//
YIntDimension RHeadlineGraphic::GetWindingDirection( ) const
{
	return m_SpinDirection;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::IsHorizontalWarpId( )
//
// Description:		
//
// Returns:				Boolean
//
// Exceptions:			Nothing
//
// ****************************************************************************
//
BOOLEAN RHeadlineGraphic::IsHorizontalWarpId( uWORD warpId )
{
	switch ( warpId ) 
	{
	   case kWarpShape1 :
	   case kWarpShape2 :
	   case kWarpShape3 :
	   case kWarpShape4 :
	   case kWarpShape5 :
	   case kWarpShape6 :
	   case kWarpShape7 :
	   case kWarpShape8 :
	   case kWarpShape9 :
	   case kWarpShape10 :
	   case kWarpShape11 :
	   case kWarpShape12 :
	   case kWarpShape13 :
	   case kWarpShape14 :
	   case kWarpShape110 :
	   case kWarpShape120 :
	   case kWarpShape130 :
	   case kWarpShape140 :
	   case kWarpShape150 :
	   case kWarpShape160 :
	   case kWarpShape170 :
	   case kWarpShape180 :
	   case kWarpShape210 :
	   case kWarpShape220 :
	   case kWarpShape230 :
	   case kWarpShape240 :
	   case kWarpShape250 :
	   case kWarpShape260 :
	   case kWarpShape270 :
	   case kWarpShape280 :
	   case kWarpShape310 :
	   case kWarpShape320 :
	   case kWarpShape330 :
	   case kWarpShape340 :
	   case kWarpShape350 :
	   case kWarpShape360 :
	   case kWarpShape370 :
	   case kWarpShape380 :
	   case kWarpShape410 :
	   case kWarpShape420 :
	   case kWarpShape430 :
	   case kWarpShape440 :
	   case kWarpShape450 :
	   case kWarpShape460 :
	   case kWarpShape470 :
	   case kWarpShape480 :
	   case kWarpShape510 :
	   case kWarpShape520 :
	   case kWarpShape530 :
	   case kWarpShape540 :
	   case kWarpShape550 :
	   case kWarpShape560 :
	   case kWarpShape570 :
	   case kWarpShape580 :
	   case kWarpShape610 :
	   case kWarpShape620 :
	   case kWarpShape630 :
	   case kWarpShape640 :
	   case kWarpShape650 :
	   case kWarpShape660 :
	   case kWarpShape670 :
	   case kWarpShape680 :
	   case kWarpShape710 :
	   case kWarpShape720 :
	   case kWarpShape730 :
			return TRUE;
		  	break;
	   default:
		  	break;
	}
	return FALSE;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::IsVerticalWarpId( )
//
// Description:		
//
// Returns:				Boolean
//
// Exceptions:			Nothing
//
// ****************************************************************************
//
BOOLEAN RHeadlineGraphic::IsVerticalWarpId( uWORD warpId )
{
	switch ( warpId ) 
	{
		case kWarpShape810 :
		case kWarpShape820 :
		case kWarpShape830 :
		case kWarpShape840 :
		case kWarpShape850 :
		case kWarpShape860 :
		case kWarpShape910 :
		case kWarpShape920 :
	 	case kWarpShape930 :
		case kWarpShape940 :
		case kWarpShape950 :
		case kWarpShape960 :
		case kWarpShape970 :
	   case kWarpShape1010 :
		case kWarpShape1020 :
		case kWarpShape1030 :
		case kWarpShape1040 :
		case kWarpShape1050 :
		case kWarpShape1060 :
			return TRUE;
		  	break;
	   default:
		  	break;
	}
	return FALSE;
}

#if 0
// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::LoadGradientFillDesc( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::LoadGradientFillDesc( RHeadlineGradientFillDesc& gradientDesc, 
															const RSolidColor& color1, 
															const RSolidColor& color2, 
															EMajorCompassDirections direction ) 
{
	gradientDesc.SetGradientType( kLinear );
	gradientDesc.SetHilitePositionPercent( 0.0 );			// not used in linear gradients
	gradientDesc.SetHiliteAngle( 0.0 );							// not used in linear gradients
	switch ( direction )
	{
		case kNorth :
			gradientDesc.SetStartPercent( RRealPoint( 0.5, 0.0 ) );
			gradientDesc.SetEndPercent( RRealPoint( 0.5, 1.0 ) );
			break;
		case kNorthEast :
			gradientDesc.SetStartPercent( RRealPoint( 0.0, 0.0 ) );
			gradientDesc.SetEndPercent( RRealPoint( 1.0, 1.0 ) );
			break;
		case kEast :
			gradientDesc.SetStartPercent( RRealPoint( 0.0, 0.5 ) );
			gradientDesc.SetEndPercent( RRealPoint( 1.0, 0.5 ) );
			break;
		case kSouthEast :
			gradientDesc.SetStartPercent( RRealPoint( 0.0, 1.0 ) );
			gradientDesc.SetEndPercent( RRealPoint( 1.0, 0.0 ) );
			break;
		case kSouth :
			gradientDesc.SetStartPercent( RRealPoint( 0.5, 1.0 ) );
			gradientDesc.SetEndPercent( RRealPoint( 0.5, 0.0 ) );
			break;
		case kSouthWest :
			gradientDesc.SetStartPercent( RRealPoint( 1.0, 1.0 ) );
			gradientDesc.SetEndPercent( RRealPoint( 0.0, 0.0 ) );
			break;
		case kWest :
			gradientDesc.SetStartPercent( RRealPoint( 1.0, 0.5 ) );
			gradientDesc.SetEndPercent( RRealPoint( 0.0, 0.5 ) );
			break;
		case kNorthWest :
			gradientDesc.SetStartPercent( RRealPoint( 1.0, 0.0 ) );
			gradientDesc.SetEndPercent( RRealPoint( 0.0, 1.0 ) );
			break;
		default :
			TpsAssertAlways( "Unrecognized direction type." );
			break;
	}
	YGradientRampPointArray	rampPtz;
	rampPtz.InsertAtEnd( RGradientRampPoint( color1, YPercentage( 0.5 ), YPercentage( 0.0 ) ) );
	rampPtz.InsertAtEnd( RGradientRampPoint( color2, YPercentage( 0.0 ), YPercentage( 1.0 ) ) );
	gradientDesc.SetRampPoints( rampPtz );
}
#endif

#if 0
// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::SetFillDesc( )
//
// Description:		Fill color desc assignment
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void RHeadlineGraphic::SetFillDesc( const RHeadlineColor& fill, RHeadlineColor& filled ) 
{
	if ( filled.fillType == kBitmap && filled.hBitmap != NULL )
		::tbitFree( filled.hBitmap );
	filled = fill;
	if ( filled.fillType != kBitmap )
		filled.hBitmap = NULL;
	switch ( fill.fillType )
	{
		case kBitmap :
		case kTranslucent :
		case kTransparent :
			filled.fillColor = RSolidColor( kBlack );
			break;
		case kBlend :
			LoadGradientFillDesc( filled.gradient, fill.fillColor, fill.blendColor, fill.direction );
			filled.fillColor = fill.fillColor;
			break;
		case kColor :
			filled.fillColor = fill.fillColor;
			break;
		case kGradient :
			{
				const YGradientRampPointArray&	rampPtz = filled.gradient.GetRampPoints();
				YGradientRampPointArrayIterator	iterator = rampPtz.Start();
				TpsAssert( rampPtz.Count() >= 2, "Illegal gradient with only one ramp point." );
				filled.fillColor = (*iterator).m_color;
			}
			break;
		default :
			TpsAssertAlways( "Unrecognized fill type." );
			break;
	}
}
#endif
