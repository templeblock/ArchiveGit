// ****************************************************************************
//
//  File Name:			Psd3SingleGraphic.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RPsd3SingleGraphic class
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
//  $Logfile:: /PM8/GraphicComp/Source/Psd3SingleGraphic.cpp                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"GraphicIncludes.h"

ASSERTNAME

#ifdef	MAC
#include "tpsint.h"
#endif

#include "Psd3SingleGraphic.h"
#include "lzCompression.h"
#include	"GradientFill.h"
#include "Psd3GraphicFileFormat.h"

// Types found in the PSD 3 graphic file
typedef uBYTE YPsd3Opcode;
typedef short YPsd3PointCount;
typedef uBYTE YPsd3LineWidth;
typedef uBYTE YPsd3LineStyle;
typedef uBYTE YPsd3Color[3];
typedef short YPsd3FreeformOperatorArraySize;
typedef uBYTE YPsd3FreeformOperatorRepCount;

// Psd 3 Opcodes
const YPsd3Opcode kPsd3SetLineWidth		= 12;
const YPsd3Opcode kPsd3SetLineStyle		= 13;
const YPsd3Opcode kPsd3SetLineColor		= 15;
const YPsd3Opcode kPsd3SetFillColor		= 14;
const YPsd3Opcode kPsd3Polyline			= 1;
const YPsd3Opcode kPsd3Box					= 2;
const YPsd3Opcode kPsd3Polygon			= 8;
const YPsd3Opcode kPsd3Freeform			= 11;
const YPsd3Opcode kPsd3EndPacket			= 16;

// Psd 3 Freeform operators
typedef char YPsd3FreeformOperator;
const YPsd3FreeformOperator kFreeformMoveTo	= 'M';
const YPsd3FreeformOperator kFreeformClose	= 'C';
const YPsd3FreeformOperator kFreeformLine		= 'L';
const YPsd3FreeformOperator kFreeformBezier	= 'B';
const YPsd3FreeformOperator kFreeformEnd		= 0;

// Various bitmasks
const uBYTE kFilledMask			= 0x01;
const uBYTE kStrokedMask		= 0x02;
const uBYTE kPackTypeMask		= 0x0C;

//
// Psd 3 extensions
//
const uBYTE kGradFilledMask	= 0x10;

// gradient data
// opcodes
const YPsd3Opcode	kPsdExtSetGradColor	= 17;
const YPsd3Opcode kPsdExtGradDescTable	= 18;
const YPsd3Opcode kPsdExtGradGeomTable	= 19;

// gradient type
const short	kPsdExtGradLinear	= 0;
const short	kPsdExtGradRadial	= 1;	// our "circular burst"...


// ****************************************************************************
//
// Function Name:		RPsd3SingleGraphic::RPsd3SingleGraphic( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RPsd3SingleGraphic::RPsd3SingleGraphic( )
:	m_CurrentPosition( 0, 0 )
{
	m_fContainsGradientData = FALSE;
}

// ****************************************************************************
//
// Function Name:		RPsd3SingleGraphic::RPsd3SingleGraphic()
//
// Description:		Constructor
//
// Returns:				
//
// Exceptions:			
//
// ****************************************************************************
RPsd3SingleGraphic::RPsd3SingleGraphic(const RPsd3SingleGraphic &rhs)
:	RSingleGraphic(rhs)	
{	
	m_uPsd3GraphicDataSize = rhs.m_uPsd3GraphicDataSize;
	m_fPsd3Monochrome = rhs.m_fPsd3Monochrome;

	// Copy the compressed graphic data
	m_pPsd3GraphicData = new uBYTE[rhs.m_uPsd3GraphicDataSize];
	memcpy(m_pPsd3GraphicData, rhs.m_pPsd3GraphicData, rhs.m_uPsd3GraphicDataSize);
}	

// ****************************************************************************
//
// Function Name:		RPsd3SingleGraphic::~RPsd3SingleGraphic( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RPsd3SingleGraphic::~RPsd3SingleGraphic( )
{
}

// ****************************************************************************
//
// Function Name:		RPsd3SingleGraphic::ValidateGraphic( )
//
// Description:		Runs a series of checks to determine if the given graphic
//							is a valid Psd3 Graphic.
//
// Returns:				TRUE if it is a valid graphic, FALSE otherwise.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RPsd3SingleGraphic::ValidateGraphic(const uBYTE* pGraphicData)
	{
	RPsd3GraphicHeader* pGraphicHeader = (RPsd3GraphicHeader*)pGraphicData;

	// Validate the graphic header
	if( !pGraphicHeader->m_Size			||
		 !pGraphicHeader->m_UnpackedSize	||
		 !pGraphicHeader->m_xSize			||
		 !pGraphicHeader->m_ySize			||
		 !pGraphicHeader->m_ElementCount	||
		 ( !pGraphicHeader->m_PackedTableSize && pGraphicHeader->m_Compression ) )

		 return FALSE;

	// Check the upper limit of some fields. These are arbitrary limits; in theory there is no
	// restriction on the size of the graphic, in practice we should never see anything remotely
	// this size.
	const int kMaxGraphicSize = 0xFFFFFF;
	const int kMaxGraphicHeightWidth = 8000;
	const int kMaxElementCount = 10000;
	const int kMaxPackedTableSize = 1000;

	TpsAssert( pGraphicHeader->m_Size < kMaxGraphicSize, "Invalid graphic size." );
	TpsAssert( pGraphicHeader->m_UnpackedSize < kMaxGraphicSize, "Invalid graphic unpacked size." );
	TpsAssert( pGraphicHeader->m_xSize < kMaxGraphicHeightWidth, "Invalid graphic width." );
	TpsAssert( pGraphicHeader->m_ySize < kMaxGraphicHeightWidth, "Invalid graphic height." );
	TpsAssert( pGraphicHeader->m_ElementCount < kMaxElementCount, "Invalid graphic element count." );
	TpsAssert( pGraphicHeader->m_PackedTableSize < kMaxPackedTableSize, "Invalid graphic packed table size." );

	TpsAssert( ((pGraphicHeader->m_Compression >= 3) && (pGraphicHeader->m_Compression <= 7)) ||
						pGraphicHeader->m_Compression == 0,
				"Invalid Single Graphic compression scheme, value is not 0 or between 3 and 7.");

	return TRUE;
	}

// ****************************************************************************
//
// Function Name:		RPsd3SingleGraphic::Initialize( )
//
// Description:		Convertes the given compressed PSD 3.0 graphic data into
//							our internal format.
//
// Returns:				TRUE if the graphic was parsed successfuly.
//							FALSE if there was a parsing error; the graphic is not a
//							PSD 3 format graphic.
//
// Exceptions:			Memory exception
//
// ****************************************************************************
//
BOOLEAN RPsd3SingleGraphic::Initialize(const uBYTE* pGraphicData, BOOLEAN fAdjustLineWidth, BOOLEAN fMonochrome)
{	
	m_fContainsGradientData = FALSE;

	// Get a pointer to the header information
	RPsd3GraphicHeader* pGraphicHeader = (RPsd3GraphicHeader*)pGraphicData;

	// byte swap data on mac
#ifdef MAC
	tintSwapStructs( pGraphicHeader, sizeof(RPsd3GraphicHeader), 1, 0x000000A54AL );
#endif

	// Validate the graphic										
	if (!ValidateGraphic(pGraphicData)) return FALSE;

	// Get the graphic size
	m_GraphicSize.m_dx = pGraphicHeader->m_xSize;
	m_GraphicSize.m_dy = pGraphicHeader->m_ySize;

	// Save the monochrome flag
	m_fMonochrome = fMonochrome;

	// Packed table entry structure definition
	struct RPsd3GraphicPackedTableEntry
		{
		long		m_PackedSize;
		long		m_UnpackedSize;
		};

	// Get a pointer to the packed table
	RPsd3GraphicPackedTableEntry* pPackedTable = (RPsd3GraphicPackedTableEntry*)( pGraphicData + sizeof( RPsd3GraphicHeader ) );

	// Get a pointer to the graphic elements
	const uBYTE* pGraphicElements = pGraphicData + sizeof( RPsd3GraphicHeader ) + pGraphicHeader->m_PackedTableSize + pGraphicHeader->m_ClassSize;

	uBYTE* pBuffer;

	// if the graphic is compressed, uncompress it
	if( pGraphicHeader->m_Compression )
		{
		// Sum up the total packed and unpacked sizes
		uLONG ulTotalUnpackedSize = 0;
		uLONG ulTotalPackedSize = 0;

		for( long i = 0; i < (long)( pGraphicHeader->m_PackedTableSize / sizeof( RPsd3GraphicPackedTableEntry ) ); i++ )
			{
			RPsd3GraphicPackedTableEntry	*pPackedEntry = &pPackedTable[ i ];
#ifdef MAC
			tintSwapLongs( (uLONG *)pPackedEntry, 2 );
#endif
			ulTotalPackedSize += pPackedEntry->m_PackedSize;
			ulTotalUnpackedSize += pPackedEntry->m_UnpackedSize;
			}

		// Allocate a buffer for the unpacked data
		pBuffer = new uBYTE[ ulTotalUnpackedSize ];

		// Unpack the data
		uLONG ulBytesOut = LZunpack( pGraphicElements, pBuffer, ulTotalUnpackedSize, pGraphicHeader->m_Compression);

		TpsAssert( ulBytesOut == ulTotalUnpackedSize, "Bytes out from LZunpack != to expected size" );
		}
	else
		// The graphic was not compressed, we can parse it directly
		pBuffer = (uBYTE*)pGraphicElements;
	
	// Set m_fFirstBox to FALSE so we can recognize the first box element when we see it
	m_fFirstBox = FALSE;
	m_rFirstBox = RRealRect(0.0, 0.0, 0.0, 0.0);
	
	// Convert the elements into out internal format
	BOOLEAN fReturn = ConvertGraphicElements( pBuffer, pGraphicHeader->m_ElementCount );

	// Clean up
	if( pGraphicHeader->m_Compression )
		delete [] pBuffer;

	// Allocate a copy of the original points stream in the transformed points stream
	m_TransformedIntPointsStream.Empty();
//NOTE: Points are copied in ApplyTransform()	::Append( m_TransformedIntPointsStream, m_OriginalPointsStream.Start(), m_OriginalPointsStream.End() );
	m_TransformedRealPointsStream = m_OriginalPointsStream;

	// Set the adjust line width member
	m_fAdjustLineWidth = fAdjustLineWidth;

	return fReturn;
	}

// ****************************************************************************
//
// Function Name:			RPsd3BorderGraphic::ContainsGradientData()
//
// Description:			Returns TRUE if this graphic contains gradient data
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
BOOLEAN RPsd3SingleGraphic::ContainsGradientData()
{
	return m_fContainsGradientData;
}

// ****************************************************************************
//
// Function Name:			RPsd3SingleGraphic::SetupFillMethod()
//
// Description:			
//
// Returns:					
//
// Exceptions:				
//
// ****************************************************************************
void RPsd3SingleGraphic::SetupFillMethod( YFillMethod& fillMethod, BOOLEAN& fLastWasGradient, const RSolidColor& rgb, YGradientIndex nidxGradient )
{
	BOOLEAN	fSetGradient	= FALSE;

	switch( fillMethod )
		{
		case kStroked:
			//
			// we don't care about the fill color...
			break;
		case kFilled:
		case kFilledAndStroked:
		case kStrokedAndFilled:
			//
			// fill with solid color...
			if( fLastWasGradient )
				{
				//
				// set the current solid color...
				m_OperationsStream.InsertAtEnd( kSetFillColor );
				m_ColorsStream.InsertAtEnd( RColor(rgb) );
				fLastWasGradient	= FALSE;
				}
			break;
		case kGradFilled:
			fillMethod		= kFilled;
			fSetGradient	= TRUE;
			break;
		case kGradFilledAndStroked:
			fillMethod		= kFilledAndStroked;
			fSetGradient	= TRUE;
			break;
		case kStrokedAndGradFilled:
			fillMethod		= kStrokedAndFilled;
			fSetGradient	= TRUE;
			break;
		}
	if( fSetGradient && !fLastWasGradient )
		{
		//
		// set the current gradient color...
		m_OperationsStream.InsertAtEnd( kSetGradient );
		m_GradientIndexStream.InsertAtEnd( nidxGradient );
		fLastWasGradient	= TRUE;
		}
}

// ****************************************************************************
//
// Function Name:		RPsd3SingleGraphic::ConvertGraphicElements( )
//
// Description:		Converts the given PSD 3.0 graphic element stream into
//							our internal format. The operation and point buffers must
//							have already been allocated.
//
//							Our internal format mirrors the PSD 3.0 format fairly
//							closly. The major difference is that the points are stored
//							in a seperate array. This makes performing transforms on
//							the points extremly easy.
//
// Returns:				TRUE if the graphic was parsed successfuly.
//							FALSE if there was a parsing error; the graphic is not a
//							PSD 3 format graphic.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RPsd3SingleGraphic::ConvertGraphicElements( const uBYTE* pGraphicElements, short elementCount )
	{
	TpsAssert( pGraphicElements, "NULL graphic element stream." );
	TpsAssert( ( m_OperationsStream.Count() == 0 ), "Nonzero operation stream." );
	TpsAssert( ( m_PointCountsStream.Count() == 0 ), "Nonzero point count array." );
	TpsAssert( ( m_PathOperatorsStream.Count() == 0 ), "Nonzero path operators stream." );
	TpsAssert( ( m_OriginalPointsStream.Count() == 0 ), "Nonzero point array." );
	TpsAssert( ( m_TransformedRealPointsStream.Count() == 0 ), "Nonzero point array." );

	// locals
	RIntPoint 					ptTemp;
	YPenWidth					penWidth;
	YPenStyle 					penStyle;
	YPsd3PointCount			pointCount;
	YFillMethod 				fillMethod;
	YPointPackType 			pointPackType;
	YPathOperatorRepCount	repCount;

	RSolidColor					rgbCurSolidFill;
	RSolidColor					rgbCurSolidStroke;
	YGradientIndex				idxCurGradientFill = 0;
	BOOLEAN						fLastFillWasGradient	= FALSE;

	// Loop through the elements
	for( short element = 0; element < elementCount; element++ )
		{
		// Get the next op-code
		YPsd3Opcode opcode = *( (YPsd3Opcode*&)pGraphicElements )++;

		switch( opcode )
			{
			case kPsd3EndPacket :
				// End of packet. Add a byte. Dont know why, its not documented.
				pGraphicElements++;
				break;

			case kPsd3SetLineWidth :
				// Set line width is an opcode followed by a linewidth
				penWidth = *( (YPsd3LineWidth*&)pGraphicElements )++;
				m_OperationsStream.InsertAtEnd( kSetLineWidth );
				m_PenWidthsStream.InsertAtEnd( penWidth );
				break;

			case kPsd3SetLineStyle :
				// Set line style is an opcode followed by a linestyle
				penStyle = *( (YPsd3LineStyle*&)pGraphicElements )++;
				m_OperationsStream.InsertAtEnd( kSetLineStyle );
				m_PenStylesStream.InsertAtEnd( penStyle );
				break;

			case kPsd3SetLineColor :
				// Set line color is an opcode followed by a color
				rgbCurSolidStroke	= RSolidColor( *pGraphicElements, *( pGraphicElements + 1 ), *( pGraphicElements + 2 ) );
				pGraphicElements += sizeof( YPsd3Color );
				m_OperationsStream.InsertAtEnd( kSetLineColor );
				m_ColorsStream.InsertAtEnd( RColor(rgbCurSolidStroke) );
				break;

			case kPsd3SetFillColor :
				// Set fill color is an opcode followed by a color
				rgbCurSolidFill	= RSolidColor( *pGraphicElements, *( pGraphicElements + 1 ), *( pGraphicElements + 2 ) );
				pGraphicElements += sizeof( YPsd3Color );
				m_OperationsStream.InsertAtEnd( kSetFillColor );
				m_ColorsStream.InsertAtEnd( RColor(rgbCurSolidFill) );
				fLastFillWasGradient	= FALSE;
				break;

			case kPsd3Polyline :
				{
				// Get the packing type and point count
				pointPackType = GetPointPackType( *pGraphicElements );
				pGraphicElements++;
				pointCount = *( (YPsd3PointCount*&)pGraphicElements )++;
#ifdef MAC
				pointCount = tintSwapWord( pointCount );
#endif

				// Polyline is an opcode followed by a point count
				m_OperationsStream.InsertAtEnd( kPolyline );
				m_PointCountsStream.InsertAtEnd( pointCount );

				// Unpack the points
				UnpackPoints( pGraphicElements, pointPackType, pointCount );
				break;
				}

			case kPsd3Polygon :
				{
				// Get the packing type, fill method, and point count
				fillMethod = GetFillMethod( *pGraphicElements );
				pointPackType = GetPointPackType( *pGraphicElements );
				pGraphicElements++;
				pointCount = *( (YPsd3PointCount*&)pGraphicElements )++;
#ifdef MAC
				pointCount = tintSwapWord( pointCount );
#endif
				//
				// Make sure the correct fill is set...
				SetupFillMethod( fillMethod, fLastFillWasGradient, rgbCurSolidFill, idxCurGradientFill );

				// Polyline is an opcode followed by a fill method followed by a point count
				m_OperationsStream.InsertAtEnd( kPolygon );
				m_FillMethodsStream.InsertAtEnd( fillMethod );
				m_PointCountsStream.InsertAtEnd( pointCount );

				// Unpack the points
				UnpackPoints( pGraphicElements, pointPackType, pointCount );
				break;
				}

			case kPsd3Box :
				{
				// Get the fill method
				fillMethod = GetFillMethod( *pGraphicElements );
				pGraphicElements++;
				
				//
				// Make sure the correct fill is set...
				SetupFillMethod( fillMethod, fLastFillWasGradient, rgbCurSolidFill, idxCurGradientFill );

				// Convert the box into a polygon of five points so that it can be easily rotated.
				// We use 5 points instead of 4, because we must explicitly close the polygon.
				m_OperationsStream.InsertAtEnd( kPolygon );
				m_FillMethodsStream.InsertAtEnd( fillMethod );
				m_PointCountsStream.InsertAtEnd( 5 );

				// Insert the points

				// Top Left
				ptTemp.m_x = MacCode( (short)tintSwapWord )( *( ( (short*)pGraphicElements ) + 0 ) );
				ptTemp.m_y = MacCode( (short)tintSwapWord )( *( ( (short*)pGraphicElements ) + 1 ) );
				m_OriginalPointsStream.InsertAtEnd( RRealPoint( ptTemp ) );
				if (!m_fFirstBox) m_rFirstBox.m_Top = ptTemp.m_y;

				// Bottom Left
				ptTemp.m_x = MacCode( (short)tintSwapWord )( *( ( (short*)pGraphicElements ) + 0 ) );
				ptTemp.m_y = MacCode( (short)tintSwapWord )( *( ( (short*)pGraphicElements ) + 3 ) );
				m_OriginalPointsStream.InsertAtEnd( RRealPoint( ptTemp ) );
				if (!m_fFirstBox) m_rFirstBox.m_Left = ptTemp.m_x;

				// Bottom Right
				ptTemp.m_x = MacCode( (short)tintSwapWord )( *( ( (short*)pGraphicElements ) + 2 ) );
				ptTemp.m_y = MacCode( (short)tintSwapWord )( *( ( (short*)pGraphicElements ) + 3 ) );
				m_OriginalPointsStream.InsertAtEnd( RRealPoint( ptTemp ) );
				if (!m_fFirstBox) m_rFirstBox.m_Bottom = ptTemp.m_y;

				// Top Right
				ptTemp.m_x = MacCode( (short)tintSwapWord )( *( ( (short*)pGraphicElements ) + 2 ) );
				ptTemp.m_y = MacCode( (short)tintSwapWord )( *( ( (short*)pGraphicElements ) + 1 ) );
				m_OriginalPointsStream.InsertAtEnd( RRealPoint( ptTemp ) );
				if (!m_fFirstBox) m_rFirstBox.m_Right = ptTemp.m_x;

				// Top Left again
				ptTemp.m_x = MacCode( (short)tintSwapWord )( *( ( (short*)pGraphicElements ) + 0 ) );
				ptTemp.m_y = MacCode( (short)tintSwapWord )( *( ( (short*)pGraphicElements ) + 1 ) );
				m_OriginalPointsStream.InsertAtEnd( RRealPoint( ptTemp ) );

				// Box updates the current position with the second point (bottom right)
				m_CurrentPosition.m_x = MacCode( (short)tintSwapWord )( *( ( (short*)pGraphicElements ) + 2 ) );
				m_CurrentPosition.m_y = MacCode( (short)tintSwapWord )( *( ( (short*)pGraphicElements ) + 3 ) );
				pGraphicElements += sizeof( short ) * 4;

				if (!m_fFirstBox) m_fFirstBox = TRUE;

				break;
				}

			case kPsd3Freeform :
				{
				// Get the packing type, fill method, operator array length, and point count
				fillMethod = GetFillMethod( *pGraphicElements );
				pointPackType = GetPointPackType( *pGraphicElements );
				pGraphicElements++;
				pGraphicElements += sizeof( YPsd3FreeformOperatorArraySize );
				pointCount = *( (YPsd3PointCount*&)pGraphicElements )++;
#ifdef MAC
				pointCount = tintSwapWord( pointCount );
#endif
				//
				// Make sure the correct fill is set...
				SetupFillMethod( fillMethod, fLastFillWasGradient, rgbCurSolidFill, idxCurGradientFill );

				// Path is an opcode, fill method, operator array size, and point count
				m_OperationsStream.InsertAtEnd( kPath );
				m_FillMethodsStream.InsertAtEnd( fillMethod );
				m_PointCountsStream.InsertAtEnd( pointCount );

				// Got through the operators
				YPsd3FreeformOperator freeformOperator;
				// Make sure the first operator in the list is 'M' - MoveTo
				TpsAssert( kFreeformMoveTo == (*( (YPsd3FreeformOperator*&)pGraphicElements )), "First operator in kPsd3Freeform must be 'M'." );
				while( ( freeformOperator = *( (YPsd3FreeformOperator*&)pGraphicElements )++ ) != kFreeformEnd )
					{
					switch( freeformOperator )
						{
						case kFreeformMoveTo :
							m_PathOperatorsStream.InsertAtEnd( kMoveTo );
							break;

						case kFreeformClose :
							m_PathOperatorsStream.InsertAtEnd( kClose );
							break;

						case kFreeformLine :
							repCount = *( (YPsd3FreeformOperatorRepCount*&)pGraphicElements )++;
							m_PathOperatorsStream.InsertAtEnd( kLine );
							m_PathOperatorsStream.InsertAtEnd( (EPathOperator)repCount );
							break;

						case kFreeformBezier :
							repCount = *( (YPsd3FreeformOperatorRepCount*&)pGraphicElements )++;
							m_PathOperatorsStream.InsertAtEnd( kBezier );
							m_PathOperatorsStream.InsertAtEnd( (EPathOperator)repCount );
							break;

						default :
							return FALSE;
						}
					}

				// Include the path close operator
				m_PathOperatorsStream.InsertAtEnd( kEnd );

				// Now unpack the points
				UnpackPoints( pGraphicElements, pointPackType, pointCount );
				break;
				}

			//
			// Psd 3 Extensions
			//
			case kPsdExtSetGradColor:
				{
				// Indicate that this graphic contains gradient data
				m_fContainsGradientData = TRUE;

				//
				// Set the gradient color
				// Skip over the pad byte
				pGraphicElements++;
				//
				// Get the index into the gradient geometry table
				idxCurGradientFill = YGradientIndex(MacCode( tintSwapWord )( *((short*&)pGraphicElements)++ ));
				//
				// Set up the operator and index streams and continue...
				m_OperationsStream.InsertAtEnd( kSetGradient );
				m_GradientIndexStream.InsertAtEnd( idxCurGradientFill );
				fLastFillWasGradient	= TRUE;
				break;
				}

			case kPsdExtGradDescTable:
				{
				// Indicate that this graphic contains gradient data
				m_fContainsGradientData = TRUE;

				//
				// Get the number of gradient descriptions (ramps) in the table...
				short	nTableEntries	= MacCode( (short)tintSwapWord )( *((short*&)pGraphicElements)++ );
				TpsAssert( nTableEntries > 0, "Invalid number of gradient description table entries" );
				//
				// Now load each description...
				while( nTableEntries-- )
					{
					//
					// Get the type of this description (linear or radial)
					short	radialFlag	= MacCode( (short)tintSwapWord )( *((short*&)pGraphicElements)++ );
					TpsAssert( radialFlag==kPsdExtGradRadial || radialFlag==kPsdExtGradLinear, "Invalid gradient type in gradient description" );

					//
					// Get the number of color stops for this description
					short	nColorStops	= MacCode( (short)tintSwapWord )( *((short*&)pGraphicElements)++ );
					//
					// Create an empty color ramp...
					RGradientRamp	Ramp;
					switch( radialFlag )
						{
						case kPsdExtGradLinear:
							Ramp.SetType( kLinear );
							break;
						case kPsdExtGradRadial:
							Ramp.SetType( kCircularBurst );
							break;
						}
					//
					// Read the color stops...
					while( nColorStops-- )
						{
						//
						// Read the color code (R G B)
						RSolidColor	rgb( *pGraphicElements, *(pGraphicElements + 1), *(pGraphicElements + 2) );
						pGraphicElements += 3 * sizeof(uBYTE);
						//
						// Skip over the pad byte...
						TpsAssert( (*pGraphicElements)==uBYTE(0), "Pad byte is NOT zero!" );
						++pGraphicElements;
						//
						// Read the midpoint percent
						uBYTE	ubPctMidPt	= *pGraphicElements++;
						TpsAssert( (ubPctMidPt >= 13) && (ubPctMidPt <= 87), "Midpoint percentage is NOT between 13 and 87." );
						//
						// Read the ramppoint percent
						uBYTE	ubPctRampPt	= *pGraphicElements++;
						TpsAssert( (ubPctMidPt >= 13) && (ubPctMidPt <= 87), "Ramppoint percentage is NOT between 0 and 100." );
						//
						// Add the color stop to the ramp...
						Ramp.Add( RGradientRampPoint(rgb,YPercentage(ubPctMidPt/100.0),YPercentage(ubPctRampPt/100.0)) );
						}// for all color stops
					//
					// Add the new ramp to the ramp stream...
					m_GradientRampStream.InsertAtEnd( Ramp );
					}// for all gradient descriptions (ramps)
				break;
				}
			case kPsdExtGradGeomTable:
				{
				// Indicate that this graphic contains gradient data
				m_fContainsGradientData = TRUE;

				//
				// Get the number of geometry definitions in the table...
				short	nTableEntries	= MacCode( (short)tintSwapWord )( *((short*&)pGraphicElements)++ );
				TpsAssert( nTableEntries > 0, "Invalid number of gradient geometry table entries" );
				//
				// Now load each geometry...
				while( nTableEntries-- )
					{
					//
					// Create the gradient...
					RGradientFill	Gradient;
					//
					// Get the gradient description for this geometry...
					short				nidxDesc	= MacCode( (short)tintSwapWord )( *((short*&)pGraphicElements)++ );
					TpsAssert( nidxDesc < m_GradientRampStream.Count(), "GradientFill references non-existent ramp" );
					RGradientRamp*	pRamp		= &(m_GradientRampStream.Start()[nidxDesc]);
					Gradient.SetRamp( pRamp );
					Gradient.SetType( pRamp->GetType() );	// don't forget to copy the type!
					//
					// Skip over the hilite X and Y offsets (they're redundant)...
					((short*&)pGraphicElements)++;	// skip X
					((short*&)pGraphicElements)++;	// skip Y
					//
					// Get the hilite vector angle...
					short	nangHilite		= MacCode( (short)tintSwapWord )( *((short*&)pGraphicElements)++ );
					//
					// Get the hilite vector length percentage...
					float	dpctlenHilite	= MacCode( tintSwapFloat )( *((float*&)pGraphicElements)++ );
					//
					// Skip over the action selector (it has an unknown effect, as per BobGo)
					((short*&)pGraphicElements)++;
					//
					// Get the gradient vector origin...
					short	nGradientX		= MacCode( (short)tintSwapWord )( *((short*&)pGraphicElements)++ );
					short	nGradientY		= MacCode( (short)tintSwapWord )( *((short*&)pGraphicElements)++ );
					//
					// Get the gradient vector angle...
					short	nangGradient	= MacCode( (short)tintSwapWord )( *((short*&)pGraphicElements)++ );
					//
					// Get the gradient vector length...
					short	nlenGradient	= MacCode( (short)tintSwapWord )( *((short*&)pGraphicElements)++ );
					//
					// Get the gradient's transformation matrix...
					float	a					= MacCode( tintSwapFloat )( *((float*&)pGraphicElements)++ );
					float	b					= MacCode( tintSwapFloat )( *((float*&)pGraphicElements)++ );
					float	c					= MacCode( tintSwapFloat )( *((float*&)pGraphicElements)++ );
					float	d					= MacCode( tintSwapFloat )( *((float*&)pGraphicElements)++ );
					float	tx					= MacCode( tintSwapFloat )( *((float*&)pGraphicElements)++ );
					float	ty					= MacCode( tintSwapFloat )( *((float*&)pGraphicElements)++ );
					R2dTransform	xform( a, b, c, d, tx, ty );
					Gradient.SetBaseTransform( xform );
					//
					// Set the origin, angle, and length of the gradient...
					RRealPoint		ptOrigin		= RRealPoint( YRealCoordinate(nGradientX), YRealCoordinate(nGradientY) );
					YRealDimension	lenVector	= YRealDimension(nlenGradient);
					YAngle			angVector	= YAngle( nangGradient*kPI/-180.0 );
					YAngle			angHilite		= YAngle( nangHilite*kPI/-180.0 );
					YPercentage		pctHilitePos	= YPercentage( dpctlenHilite );

					Gradient.SetGradientOrigin( ptOrigin );
					Gradient.SetGradientAngle( angVector );
					Gradient.SetGradientLength( lenVector );
					//
					// Set the hilite angle and position...
					Gradient.SetHiliteAngle( angHilite );
					Gradient.SetHilitePosition( pctHilitePos );
					//
					// Add the completed gradient geometry to the graphic's data...
					m_GradientFillStream.InsertAtEnd( Gradient );
					}
				break;
				}

			default :
				return FALSE;
			}
		}
	// Add the EndGraphic opcode
	m_OperationsStream.InsertAtEnd( kEndGraphic );

	return TRUE;
	}

// ****************************************************************************
//
// Function Name:		RPsd3SingleGraphic::UnpackPoints( )
//
// Description:		Unpacks the points in the given graphic element stream
//							into the point stream array
//
// Returns:				None
//
// Exceptions:			None
//
// ****************************************************************************
//
void RPsd3SingleGraphic::UnpackPoints( uBYTE const*& pGraphicElements, YPointPackType pointPackType, YPointCount pointCount )
	{
	YIntCoordinate x;
	YIntCoordinate y;

	if( pointPackType == kShort )
		{
		// Absolute points packed as two shorts
		for( YPointCount i = 0; i < pointCount; i++ )
			{
			x = MacCode( (short)tintSwapWord )( *( (short*&)pGraphicElements )++ );
			y = MacCode( (short)tintSwapWord )( *( (short*&)pGraphicElements )++ );
			m_CurrentPosition.m_x = x;
			m_CurrentPosition.m_y = y;
			m_OriginalPointsStream.InsertAtEnd( RRealPoint( m_CurrentPosition ) );
			}
		}
	else if( pointPackType == kByte )
		{
		// Relative offsets packed as two bytes. If the first byte of an offset is -128, then
		// the offset is really an absolute point stored as two shorts

		for( YPointCount i = 0; i < pointCount; i++ )
			{
			if( *(sBYTE*)pGraphicElements == -128 )
				{
				// short absolute points
				pGraphicElements++;
				x = MacCode( (short)tintSwapWord )( *( (short*&)pGraphicElements )++ );
				y = MacCode( (short)tintSwapWord )( *( (short*&)pGraphicElements )++ );
				}
			else
				{
				// byte relative points
				x = m_CurrentPosition.m_x - *( (sBYTE*&)pGraphicElements )++;
				y = m_CurrentPosition.m_y - *( (sBYTE*&)pGraphicElements )++;
				}
			m_CurrentPosition.m_x = x;
			m_CurrentPosition.m_y = y;
			m_OriginalPointsStream.InsertAtEnd( RRealPoint( m_CurrentPosition ) );
			}
		}
	else if( pointPackType == kNibble )
		{
		// Relative offsets packed as two nibbles. If the byte is -16, then the offset is really
		// an absolute point stored as two shorts. If the byte is -1, then the offset is really
		// a relative offset stored as two bytes.

		for( YPointCount i = 0; i < pointCount; i++ )
			{
			if( *(sBYTE*)pGraphicElements == -16 )
				{
				// short absolute points
				pGraphicElements++;
				x = MacCode( (short)tintSwapWord )( *( (short*&)pGraphicElements )++ );
				y = MacCode( (short)tintSwapWord )( *( (short*&)pGraphicElements )++ );
				}
				
			else if( *(sBYTE*)pGraphicElements == -1 )
				{
				// byte relative points
				pGraphicElements++;
				x = m_CurrentPosition.m_x - *( (sBYTE*&)pGraphicElements )++;
				y = m_CurrentPosition.m_y - *( (sBYTE*&)pGraphicElements )++;
				}
			
			else
				{
				// nibble relative points
				x = m_CurrentPosition.m_x - ( ( ( *(sBYTE*)pGraphicElements >> 4 ) & 0x0F ) - 7 );
				y = m_CurrentPosition.m_y - ( ( ( *(sBYTE*)pGraphicElements++ ) & 0x0F ) - 7 );
				}

			m_CurrentPosition.m_x = x;
			m_CurrentPosition.m_y = y;
			m_OriginalPointsStream.InsertAtEnd( RRealPoint( m_CurrentPosition ) );
			}
		}
	}

// ****************************************************************************
//
// Function Name:		RPsd3SingleGraphic::GetFillMethod( )
//
// Description:		Converts a PSD 3 graphic element info byte into a
//							YFillMethod.
//
// Returns:				The fill method
//
// Exceptions:			None
//
// ****************************************************************************
//
YFillMethod RPsd3SingleGraphic::GetFillMethod( uBYTE info ) const
	{
	uBYTE ubStroked		= static_cast<uBYTE>( info & kStrokedMask );
	uBYTE ubFilled			= static_cast<uBYTE>( info & kFilledMask );
	uBYTE	ubGradFilled	= static_cast<uBYTE>( info & kGradFilledMask );

	if( ubStroked )
		{
		if( ubGradFilled )
			{
			TpsAssert( !ubFilled, "Object is both filled and gradient filled!" );
			return kGradFilledAndStroked;
			}
		if( ubFilled )
			return kFilledAndStroked;
		return kStroked;
		}
	if( ubGradFilled )
		{
		TpsAssert( !ubFilled, "Object is both filled and gradient filled!" );
		return kGradFilled;
		}

//	The following assert was removed because timepieces will trigger this.
//	Historically (meaning prior to Renaissance), the bounding box with the
//	rectangle information for timepieces to render either a digital time
//	or the rotated hands was encoded as a DS_BOX with neither fill nor
//	stroke. While we would like to leave this assert to handle data of other
//	cases that should not exist, it was decided to remove this to support
//	the timepiece information encoding. - BDR 4/23/97
//	TpsAssert( ubFilled != 0, "Object has unknown fill method" );
	return kFilled;
	}

// ****************************************************************************
//
// Function Name:		RPsd3SingleGraphic::GetPointPackType( )
//
// Description:		Converts a PSD 3 graphic element info byte into a
//							YPointPackType.
//
// Returns:				The point pack type
//
// Exceptions:			None
//
// ****************************************************************************
//
YPointPackType RPsd3SingleGraphic::GetPointPackType( uBYTE info ) const
	{
	return static_cast<YPointPackType>( info & kPackTypeMask );
	}
