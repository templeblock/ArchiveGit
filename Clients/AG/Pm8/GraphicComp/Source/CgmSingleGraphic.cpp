// ****************************************************************************
//
// $Header: /PM8/GraphicComp/Source/CgmSingleGraphic.cpp 1     3/03/99 6:18p Gbeddow $
//
//  Project:			Graphic Component.
//
//  Author:				Richard Grenfell
//
//  Description:		Declaration of the RCgmSingleGraphic class
//							The CGM graphics class is a vector graphic stored in CGM
//							raw data format.
//
//  Portability:		Platform independent
//
//  Copyright (C) 1999 The Learning Company. All Rights Reserved.
//
//  $Log: /PM8/GraphicComp/Source/CgmSingleGraphic.cpp $
// 
// 1     3/03/99 6:18p Gbeddow
// 
// 1     2/26/99 9:12p Rgrenfel
// Support for CGM graphic data.
//
// ****************************************************************************

#include	"GraphicIncludes.h"

ASSERTNAME

#ifdef	MAC
#include "tpsint.h"
#endif

#include "CgmSingleGraphic.h"
#include "DrawingSurface.h"

// Remove or add an x to turn off or on the debug defines.
#define DEBUG_CGMx
#define DUMP_CGMx
#define	SHOW_COLORSx

#define	WIDTH_SCALE			4

#ifdef DUMP_CGM
// Dump values to allow for validation of input.
char * class_names[] =
{
	"Delimiter",
	"Metafile Descriptor",
	"Picture Descriptor",
	"Control",
	"Graphical Primitive",
	"Attribute",
	"Escape",
	"External",
	"Reserved"
};

/*
// Strings for class ids.
*/
char * delimiter_names[] =
{
	"No-op",
	"Begin Metafile",
	"End Metafile",
	"Begin Picture",
	"Begin Picture Body",
	"End Picture"
};

char * metafile_names[] =
{
	"0",
	"Version",
	"Description",
	"VDC type",
	"Integer precision",
	"Real precision",
	"Index precision",
	"Color precision",
	"Color index precision",
	"Maximum color index",
 	"Color value extent",
	"Metafile element list",
	"Metafile defaults replacement",
	"Font list",
	"Character set list",
	"Character coding announcer"
};

char * picture_names[] =
{
	"0",
	"Scaling mode",
	"Color selection mode",
	"Line width spec.",
	"Marker size spec. mode",
	"Edge width spec. mode",
	"VDC extent",
	"Background color"
};
char * control_names[] =
{
	"0",
	"VDC integer precision",
	"VDC real precision",
	"Auxiliary color",
	"Transparency",
	"Clip rectangle",
	"Clip indicator"
};

char * primitive_names[] =
{
	"0",
	"Polyline",
	"Disjoint polyline",
	"Polymarker",
	"Text",
	"Restricted text",
	"Append text",
	"Polygon",
	"Polygon set",
	"Cell array",
	"Generalized drawing primitive",
	"Rectangle",
	"Circle",
	"Circular arc 3 point",
	"Circular arc 3 point close",
	"Circular arc center",
	"Circular arc center close",
	"Ellipse",
	"Elliptical arc",
	"Elliptical arc close"
};

char * attribute_names[] =
{
	"0",
	"Line bundle index",
	"Line type",
	"Line width",
	"Line color",
	"Marker bundle index",
	"Marker type",
	"Marker size",
	"Marker color",
	"Text bundle index",
	"Text font index",
	"Text precision",
	"Character expansion factor",
	"Character spacing",
	"Text color",
	"Character height",
	"Character orientation",
	"Text path",
	"Text alignment",
	"Character set index",
	"Alternate character set index",
	"Fill bundle index",
	"Interior style",
	"Fill color",
	"Hatch index",
	"Pattern index",
	"Edge bundle index",
	"Edge type",
	"Edge width",
	"Edge color",
	"Edge visibility",
	"Fill reference point",
	"Pattern table",
	"Pattern size",
	"Color table",
	"Aspect source flags"
};

char * escape_names[] =
{
	"0",
	"Escape"
};

char * external_names[] =
{
	"0",
	"Message",
	"Application data"
};

/*
// info about the classes.
*/

struct
{
	SHORT	last_id;
	CHAR	**names;
} info[] =
	{
		{ CGM_DELIMITER_ELEMENTS, delimiter_names },
		{ CGM_METAFILE_ELEMENTS, metafile_names },
		{ CGM_PICTURE_ELEMENTS, picture_names },
		{ CGM_CONTROL_ELEMENTS, control_names },
		{ CGM_PRIMITIVE_ELEMENTS, primitive_names },
		{ CGM_ATTRIBUTE_ELEMENTS, attribute_names },
		{ CGM_ESCAPE_ELEMENTS, escape_names },
		{ CGM_EXTERNAL_ELEMENTS, external_names },
	};
#endif


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::RCGMSingleGraphic( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RCGMSingleGraphic::RCGMSingleGraphic( )
:	m_CurrentPosition( 0, 0 )
{
	m_nFillMode = WINDING;
	m_pColorPalette = NULL;
	m_lBufferDataRead = 0l;
	m_bReverseYTransform = FALSE;
	m_bReverseXTransform = FALSE;
}

// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::RCGMSingleGraphic()
//
// Description:		Copy Constructor
//
// Returns:				
//
// Exceptions:			
//
// ****************************************************************************
RCGMSingleGraphic::RCGMSingleGraphic(const RCGMSingleGraphic &rhs)
:	RSingleGraphic(rhs)	
{	
	m_uPsd3GraphicDataSize = rhs.m_uPsd3GraphicDataSize;
	m_fPsd3Monochrome = rhs.m_fPsd3Monochrome;

	// Copy the compressed graphic data
	m_pPsd3GraphicData = new uBYTE[rhs.m_uPsd3GraphicDataSize];
	memcpy(m_pPsd3GraphicData, rhs.m_pPsd3GraphicData, rhs.m_uPsd3GraphicDataSize);

	m_nFillMode = WINDING;
	m_pColorPalette = NULL;
	m_lBufferDataRead = 0l;
	m_bReverseYTransform = FALSE;
	m_bReverseXTransform = FALSE;
}	

// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::~RCGMSingleGraphic( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RCGMSingleGraphic::~RCGMSingleGraphic( )
{
}

// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::ValidateGraphic( )
//
// Description:		Runs a series of checks to determine if the given graphic
//							is a valid CGM Graphic.
//
// Returns:				TRUE if it is a valid graphic, FALSE otherwise.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::ValidateGraphic(const uBYTE* pGraphicData)
{
	// CGM data is validated as it is processed, it doesn't have a clear cut
	// header block which can be validated before processing.
	if (pGraphicData)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::Initialize( )
//
// Description:		Convertes the given compressed CGM graphic data into
//							our internal format.
//
// Returns:				TRUE if the graphic was parsed successfuly.
//							FALSE if there was a parsing error; the graphic is not a
//							CGM format graphic.
//
// Exceptions:			Memory exception
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::Initialize(const uBYTE* pGraphicData, BOOLEAN fAdjustLineWidth, BOOLEAN fMonochrome)
{	
#ifdef DUMP_CGM
	TRACE( "Initializing the CGM Component Graphic:\n" );
#endif
	// Get a pointer to the header information
//	RPsd3GraphicHeader* pGraphicHeader = (RPsd3GraphicHeader*)pGraphicData;

	// Validate the graphic										
	if (!ValidateGraphic(pGraphicData))
		return FALSE;

	m_ptMetafileMin.m_x = m_ptMetafileMin.m_y =
	m_sizeExtendedMin.m_dx = m_sizeExtendedMin.m_dy = 0x7FFF;
	m_ptMetafileMax.m_x = m_ptMetafileMax.m_y =
	m_sizeExtendedMax.m_dx = m_sizeExtendedMax.m_dy = -0x7FFF;

	m_bTooBig = FALSE;

	// Get the graphic size
//	m_GraphicSize.m_dx = pGraphicHeader->m_xSize;
//	m_GraphicSize.m_dy = pGraphicHeader->m_ySize;

	// Save the monochrome flag
	m_fMonochrome = fMonochrome;

	// Set m_fFirstBox to FALSE so we can recognize the first box element when we see it
	m_fFirstBox = FALSE;
	m_rFirstBox = RRealRect(0.0, 0.0, 0.0, 0.0);
	
	// Convert the elements into out internal format
	BOOLEAN fReturn = ConvertGraphicElements( pGraphicData );

	if ( (m_sizeExtent.m_dx == 0) && (m_sizeExtent.m_dy == 0) )
	{
		ASSERT( 0 );
		return FALSE;
	}

   if ((long)m_ptMetafileMax.m_x - (long)m_ptMetafileMin.m_x > 32767
         || (long)m_ptMetafileMax.m_y - (long)m_ptMetafileMin.m_y > 32767
         || (long)m_sizeExtendedMax.m_dx - (long)m_sizeExtendedMin.m_dx > 32767
         || (long)m_sizeExtendedMax.m_dy - (long)m_sizeExtendedMin.m_dy > 32767)
   {
      m_bTooBig = TRUE;
   }

	if (m_bTooBig)
	{
		m_ptMetafileMin.m_x = m_ptMetafileMin.m_y =
		m_sizeExtendedMin.m_dx = m_sizeExtendedMin.m_dy = 0x7FFF;
		m_ptMetafileMax.m_x = m_ptMetafileMax.m_y =
		m_sizeExtendedMax.m_dx = m_sizeExtendedMax.m_dy = -0x7FFF;
	}

	if ((m_ptMetafileMin.m_x > m_ptMetafileMax.m_x) ||
		 (m_ptMetafileMin.m_y > m_ptMetafileMax.m_y) )
	{
		m_sizeExtendedMax.m_dx = m_sizeExtendedMin.m_dx =
		m_sizeExtendedMax.m_dy = m_sizeExtendedMin.m_dy = 0;
	}
	else
	{
		m_sizeExtent.m_dx = m_ptMetafileMax.m_x - m_ptMetafileMin.m_x;
		m_sizeExtent.m_dy = m_ptMetafileMax.m_y - m_ptMetafileMin.m_y;

		m_sizeExtendedMin.m_dx -= m_ptMetafileMin.m_x;
		m_sizeExtendedMax.m_dx -= m_ptMetafileMax.m_x;
		m_sizeExtendedMin.m_dy -= m_ptMetafileMin.m_y;
		m_sizeExtendedMax.m_dy -= m_ptMetafileMax.m_y;
	}

	m_GraphicSize.m_dx = m_sizeExtent.m_dx + 1;	// The size is one larger than the extent
	m_GraphicSize.m_dy = m_sizeExtent.m_dy + 1;	// since it was constructed from a subtraction.

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
// Function Name:		RCGMSingleGraphic::ConvertGraphicElements( )
//
// Description:		Converts the given CGM graphic data stream into
//							our internal format. The operation and point buffers must
//							have already been allocated.
//
//							Our internal format mirrors the CGM format fairly
//							closly. The major difference is that the points are stored
//							in a seperate array. This makes performing transforms on
//							the points extremly easy.
//
// Returns:				TRUE if the graphic was parsed successfuly.
//							FALSE if there was a parsing error; the graphic is not a
//							CGM format graphic.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::ConvertGraphicElements( const BYTE* pGraphicElements )
{
	TpsAssert( pGraphicElements, "NULL graphic stream." );
	TpsAssert( ( m_OperationsStream.Count() == 0 ), "Nonzero operation stream." );
	TpsAssert( ( m_PointCountsStream.Count() == 0 ), "Nonzero point count array." );
	TpsAssert( ( m_PathOperatorsStream.Count() == 0 ), "Nonzero path operators stream." );
	TpsAssert( ( m_OriginalPointsStream.Count() == 0 ), "Nonzero point array." );
	TpsAssert( ( m_TransformedRealPointsStream.Count() == 0 ), "Nonzero point array." );

	// Ensure we are starting from a known point.
	SetDefaults();

	// Initialize the buffer
	ASSERT( pGraphicElements == m_pPsd3GraphicData );
	m_pInputBuffer = pGraphicElements;
	m_lBufferDataRead = 0l;

	bool bParsing = TRUE;
	SHORT nClass;
	SHORT nID;
	SHORT nLength;

	// Loop through the elements
	while( bParsing )
	{
		// Align to a word boundary if necessary!
		EnsureDataOnWordBoundary();

		// Determine the current command!
		if (!ReadCommand( &nClass, &nID, &nLength ))
		{
			// We failed to read the command, so break out of the loop.
			bParsing = FALSE;
			break;
		}

		if (nClass > CGM_RESERVED)
		{
	#ifdef DUMP_CGM
			TRACE("Class: %s (%d),", (LPCSTR)class_names[CGM_RESERVED], nClass);
	#endif
			nClass = CGM_RESERVED;
		}
		else
		{
	#ifdef DUMP_CGM
			TRACE("Class: %s,", (LPCSTR)class_names[nClass]);
			if (nID >= info[nClass].last_id)
			{
				TRACE(" id: %d, ", nID);
			}
			else
			{
				TRACE(" id: %s, ", (LPCSTR)(info[nClass].names[nID]));
			}
	#endif
		}

		if (nLength == 0x1f)
		{
			nLength = 0;

	#ifdef DUMP_CGM
			TRACE("(LONG)");
	#endif

			SHORT nLengthTemp;
			do
			{
				// Read the full sized length value.
				if (!ReadShortData(&nLengthTemp, 2))
				{
					break;
				}

				if (nLengthTemp < 0)
				{
				/* High bit is set. Extension! */
	#ifdef DUMP_CGM
					TRACE("(EXTENSION)");
	#endif
					SeekRelative( (LONG)(nLengthTemp & (SHORT)0x7FFF) );
				}
				nLength += nLengthTemp & (SHORT)0x7FFF;
			} while (nLengthTemp < 0);
		}
	#ifdef DUMP_CGM
		TRACE("Length: %d\n", nLength);
	#endif

		// Store our current position so we can put it back later.
		LONG lCurrentBufferPosition = GetPosition();

		if ((nClass >= CGM_DELIMITER) && (nClass < CGM_RESERVED))
		{
			// Perform the correct action based upon the class of call specified.
			if ( !ProcessCommand( nClass, nID, nLength ) )
			{
				// Stopped processing for some reason.
				bParsing = FALSE;
				break;
			}
		}
		else
		{
	#ifdef DUMP_CGM
			TRACE( "Unkown command ID hit\n" );
	#endif
		}

		// Put ourselves back to our original location plus our length of movement.
		SeekAbsolute( lCurrentBufferPosition + nLength );
	}

	// Add the EndGraphic opcode
	m_OperationsStream.InsertAtEnd( kEndGraphic );

	return TRUE;
}


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::ProcessCommand()
//
// Description:		Processes the command specified and puts it into the the
//							RSingleGraphic data processing structures.
//
// Returns:				TRUE if the command could be successfully processed.
//							FALSE if the command was invalid.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::ProcessCommand( SHORT nClass, SHORT nID, SHORT nLength )
{
	BOOLEAN bOK = TRUE;

	// If we need to handle Parsing modes (size verses render) then this should
	// become in if statement around two seperate switch statements, one for
	// each type of pass, each calling the appropriate methods.

	// Process the command for the current class.
	switch (nClass)
	{
		case CGM_DELIMITER:
			bOK = DrawDelimiter(nID);
			break;
		case CGM_METAFILE:
			bOK = DrawMetafile(nID);
			break;
		case CGM_PICTURE:
			bOK = DrawPicture(nID);
			break;
		case CGM_CONTROL:
			bOK = DrawControl(nID);
			break;
		case CGM_PRIMITIVE:
			bOK = DrawPrimitive(nID, nLength);
			break;
		case CGM_ATTRIBUTE:
			bOK = DrawAttribute(nID, nLength);
			break;
		case CGM_ESCAPE:
		case CGM_EXTERNAL:
		case CGM_RESERVED:
			// Ignore the command.
			bOK = TRUE;
			break;
	}
	return bOK;
}


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::DrawDelimiter()
//
// Description:		Processes the delimiter command specified.  This detects
//							if the graphic is a nFX graphic and sets the fill mode to
//							winding if it is.
//
// Returns:				TRUE if the command could be successfully processed.
//							FALSE if the command was invalid.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::DrawDelimiter(SHORT nID)
{
#ifdef DUMP_CGM
	TRACE("Processing delimiter");
#endif
	switch (nID)
	{
		case CGM_BEGIN_METAFILE:
		{
			UCHAR uLength = 0;

			ReadData(&uLength, sizeof(uLength));

			if (uLength != 255 && uLength >= 3)
			{
				BYTE pBuffer[3];

				// Read the graphic title.
				ReadData(pBuffer, sizeof(pBuffer));
				if (pBuffer[0] == 'n'
					 && pBuffer[1] == 'F'
					 && pBuffer[2] == 'X')
				{
					// nFX CGM file. Use WINDING mode.
					m_nFillMode = WINDING;
	#ifdef DUMP_CGM
					TRACE(" - nFX graphic!");
	#endif
				}
			}
			break;
		}
		default:
			break;
	}

#ifdef DUMP_CGM
	TRACE("\n");
#endif
	return TRUE;
}


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::DrawMetafile()
//
// Description:		Loads the metafile description objects.  Precision
//							definitions, color extents, etc.
//
// Returns:				TRUE if the command could be successfully processed.
//							FALSE if the command was invalid or we ran out of data.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::DrawMetafile(SHORT nID)
{
	switch (nID)
	{
		case CGM_VDC_TYPE:	// Virtual device coordinates type.
		{
			SHORT nType;

			ReadShortData(&nType);
			m_nVdcType = nType;

#ifdef DUMP_CGM
			TRACE("Vdc Type [%d]\n", m_nVdcType);
#endif

			if (nType != 0)
			{
				return FALSE;				/* Stop now! */
			}
			break;
		}
		case CGM_INTEGER_PRECISION:
		{
			if ( !ReadPrecisionData(&m_nIntegerPrecision) )
			{
				return FALSE;
			}
#ifdef DUMP_CGM
			TRACE("Integer Precision [%d]\n", m_nIntegerPrecision);
#endif
			break;
		}
		case CGM_INDEX_PRECISION:
		{
			if ( !ReadPrecisionData(&m_nIndexPrecision) )
			{
				return FALSE;
			}
#ifdef DUMP_CGM
			TRACE("Index Precision [%d]\n", m_nIndexPrecision);
#endif
			break;
		}
		case CGM_COLOR_PRECISION:
		{
			if ( !ReadPrecisionData(&m_nColorPrecision) )
			{
				return FALSE;
			}
#ifdef DUMP_CGM
			TRACE("Color Precision [%d]\n", m_nColorPrecision);
#endif
			break;
		}
		case CGM_COLOR_INDEX:
		{
			if ( !ReadPrecisionData(&m_nColorIndexPrescision) )
			{
				return FALSE;
			}
#ifdef DUMP_CGM
			TRACE("Color Index Precision [%d]\n", m_nColorIndexPrescision);
#endif
			break;
		}
		case CGM_MAXIMUM_COLOR_INDEX:
		{
			if ( !ReadShortData(&m_nMaxColorIndex, m_nColorIndexPrescision) )
			{
				return FALSE;
			}
#ifdef DUMP_CGM
			TRACE("Maximum Color Index [%d]\n", m_nMaxColorIndex);
#endif
			break;
		}
		case CGM_COLOR_VALUE_EXTENT:
		{
			SCgmColor ColorBase, ColorCap;

			// Read the two values.
			if ( !ReadColorTriple(&ColorBase) )
			{
				return FALSE;
			}
			if ( !ReadColorTriple(&ColorCap) )
			{
				return FALSE;
			}

			// Convert to origin/extent from color/color.
			m_ColorExtent.m_nRed = ColorCap.m_nRed - (m_ColorOrigin.m_nRed = ColorBase.m_nRed);
			m_ColorExtent.m_nGreen = ColorCap.m_nGreen - (m_ColorOrigin.m_nGreen = ColorBase.m_nGreen);
			m_ColorExtent.m_nBlue = ColorCap.m_nBlue - (m_ColorOrigin.m_nBlue = ColorBase.m_nBlue);

			// Set flags about whether we need to scale or not.
			m_bColorScale[COLOR_SCALE_RED] = (m_ColorOrigin.m_nRed != 0 || m_ColorExtent.m_nRed != 255);
			m_bColorScale[COLOR_SCALE_GREEN] = (m_ColorOrigin.m_nGreen != 0 || m_ColorExtent.m_nGreen != 255);
			m_bColorScale[COLOR_SCALE_BLUE] = (m_ColorOrigin.m_nBlue != 0 || m_ColorExtent.m_nBlue != 255);

#ifdef DUMP_CGM
			TRACE("Color Extents : Origin [%d,%d,%d], Extent [%d, %d, %d]\n",
					m_ColorOrigin.m_nRed, m_ColorOrigin.m_nGreen, m_ColorOrigin.m_nBlue,
					m_ColorExtent.m_nRed, m_ColorExtent.m_nGreen, m_ColorExtent.m_nBlue);
#endif
			break;
		}
		default:
		{
			break;
		}
	}
	return TRUE;
}


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::DrawPicture()
//
// Description:		Processes Picture definitions such as edge and line widths,
//							background colors and the virtual device coordinate extents.
//
// Returns:				TRUE if the command could be successfully processed.
//							FALSE if the command was invalid.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::DrawPicture(SHORT nID)
{
	switch (nID)
	{
		case CGM_COLOR_SELECTION_MODE:
		{
			SHORT nColorSelectionMode;
			if ( !ReadShortData(&nColorSelectionMode) )
			{
				return FALSE;
			}
			m_eColorSelectionMode = (COLOR_SELECTION_MODE)nColorSelectionMode;
#ifdef DUMP_CGM
			TRACE( "Color Selection Mode [%s]\n", (nColorSelectionMode) ? "Direct" : "Indexed" );
#endif
			break;
		}
		case CGM_LINE_WIDTH_MODE:
		{
			if ( !ReadShortData(&m_nLineWidthMode) )
			{
				return FALSE;
			}
#ifdef DUMP_CGM
			TRACE( "Line Width Mode [%d]\n", m_nLineWidthMode );
#endif
			break;
		}
		case CGM_EDGE_WIDTH_MODE:
		{
			if ( !ReadShortData(&m_nEdgeWidthMode) )
			{
				return FALSE;
			}
#ifdef DUMP_CGM
			TRACE( "Edge Width Mode [%d]\n", m_nEdgeWidthMode );
#endif
			break;
		}
		case CGM_MARKER_SIZE_MODE:
		{
			if ( !ReadShortData(&m_nMarkerSizeMode) )
			{
				return FALSE;
			}
#ifdef DUMP_CGM
			TRACE( "Marker size mode [%d]\n", m_nMarkerSizeMode );
#endif
			break;
		}
		case CGM_VDC_EXTENT:
		{
			RIntPoint ptExtentBase, ptExtentCap;

#ifdef FIX_CGMS
			LONG lBufferPos = GetPosition();
#endif

			if (!ReadPoint(&ptExtentBase) ||
				 !ReadPoint(&ptExtentCap) )
			{
				return FALSE;
			}

			// Handle sizing information.
			m_sizeOrigin.m_dx = ptExtentBase.m_x;
			m_sizeOrigin.m_dy = ptExtentBase.m_y;
			m_sizeExtent.m_dx = ptExtentCap.m_x - ptExtentBase.m_x;
			m_sizeExtent.m_dy = ptExtentCap.m_y - ptExtentBase.m_y;
			if (m_sizeExtent.m_dx < 0)
			{
				m_sizeExtent.m_dx = -m_sizeExtent.m_dx;
			}
			if (m_sizeExtent.m_dy < 0)
			{
				m_sizeExtent.m_dy = -m_sizeExtent.m_dy;
			}

//			printf("extent: %d, %d to %d, %d\n", ptExtentBase, ptExtentCap);

			if (ptExtentBase.m_y < ptExtentCap.m_y)
			{
				m_bReverseYTransform = !m_bReverseYTransform;
			}
			else
			{
				YIntCoordinate tmp = ptExtentBase.m_y;
				ptExtentBase.m_y = ptExtentCap.m_y;
				ptExtentCap.m_y = tmp;
			}

			if (ptExtentBase.m_x > ptExtentCap.m_x)
			{
				YIntCoordinate tmp = ptExtentBase.m_x;
				ptExtentBase.m_x = ptExtentCap.m_x;
				ptExtentCap.m_x = tmp;
				m_bReverseXTransform = !m_bReverseXTransform;
			}

#ifdef DUMP_CGM
			TRACE("Extent: Base [%d,%d], Cap [%d,%d] (rev: [%d,%d]); min: [%d,%d]; max: [%d,%d]\n",
					ptExtentBase.m_x, ptExtentBase.m_y, ptExtentCap.m_x, ptExtentCap.m_y,
					m_bReverseXTransform, m_bReverseYTransform,
					m_ptMetafileMin.m_x, m_ptMetafileMin.m_y,
					m_ptMetafileMax.m_x, m_ptMetafileMax.m_y );
#endif

			// Use the discovered size if we have them.

			if (m_ptMetafileMin.m_x < m_ptMetafileMax.m_x
					&& m_ptMetafileMin.m_y < m_ptMetafileMax.m_y)
			{
				// Use the discovered bounds.
//				TRACE("use discovered bounds...\n");

#ifdef FIX_CGMS
				LONG lEndPos = GetPosition();

				ASSERT(m_nVdcIntegerPrecision == 2);

				RIntPoint vdcp0, vdcp1;
				if (m_bReverseXTransform)
				{
					vdcp0.m_x = m_ptMetafileMax.m_x;
					vdcp1.m_x = m_ptMetafileMin.m_x;
				}
				else
				{
					vdcp0.m_x = m_ptMetafileMin.m_x;
					vdcp1.m_x = m_ptMetafileMax.m_x;
				}
				if (!m_bReverseYTransform)
				{
					vdcp0.m_y = m_ptMetafileMax.m_y;
					vdcp1.m_y = m_ptMetafileMin.m_y;
				}
				else
				{
					vdcp0.m_y = m_ptMetafileMin.m_y;
					vdcp1.m_y = m_ptMetafileMax.m_y;
				}

				// We want to write!
				
				TRACE("Fix the CGM!\n");
				SeekAbsolute(lBufferPos);
				// Shorts are written reversed.
				WriteData(((char*)&vdcp0.m_x)+1, 1);
				WriteData(((char*)&vdcp0.m_x)+0, 1);
				WriteData(((char*)&vdcp0.m_y)+1, 1);
				WriteData(((char*)&vdcp0.m_y)+0, 1);

				WriteData(((char*)&vdcp1.m_x)+1, 1);
				WriteData(((char*)&vdcp1.m_x)+0, 1);
				WriteData(((char*)&vdcp1.m_y)+1, 1);
				WriteData(((char*)&vdcp1.m_y)+0, 1);
#endif
				ptExtentBase.m_x = m_ptMetafileMin.m_x;
				ptExtentBase.m_y = m_ptMetafileMin.m_y;
				ptExtentCap.m_x = m_ptMetafileMax.m_x;
				ptExtentCap.m_y = m_ptMetafileMax.m_y;
			}
			else
			{
				LONG delta;

//				TRACE("use actual extent...\n");

				if ((delta = (USHORT)ptExtentCap.m_x - (USHORT)ptExtentBase.m_x) > 32767)
				{
					ptExtentCap.m_x = (SHORT)((ptExtentCap.m_x * (YIntCoordinate)32767) / (YIntCoordinate)delta);
					ptExtentBase.m_x = (SHORT)((ptExtentBase.m_x * (YIntCoordinate)32767L) / (YIntCoordinate)delta);
				}

				if ((delta = (USHORT)ptExtentCap.m_y - (USHORT)ptExtentBase.m_y) > 32767)
				{
					ptExtentCap.m_y = (SHORT)((ptExtentCap.m_y * (YIntCoordinate)32767) / (YIntCoordinate)delta);
					ptExtentBase.m_y = (SHORT)((ptExtentBase.m_y * (YIntCoordinate)32767L) / (YIntCoordinate)delta);
				}
			}

			// The source extents are basically the same as used by windows, this
			// is the logical coodinate space used by the picture data in the CGM.
		   m_ptSourceExtent.m_x = ptExtentCap.m_x - (m_ptSourceOrigin.m_x = ptExtentBase.m_x);
   		m_ptSourceExtent.m_y = ptExtentCap.m_y - (m_ptSourceOrigin.m_y = ptExtentBase.m_y);

#ifdef DUMP_CGM
			TRACE( "Final : Source Origin [%d,%d], Source Extent [%d,%d]\n",
					m_ptSourceOrigin.m_x, m_ptSourceOrigin.m_y,
					m_ptSourceExtent.m_x, m_ptSourceExtent.m_y );
#endif
//			TRACE("used VDC extent: %d, %d to %d, %dn", ptExtentBase, ptExtentCap);

			break;
		}
		case CGM_BACKGROUND_COLOR:
		{
			SCgmColor Color;

			if ( !ReadColorTriple(&Color) )
			{
				return FALSE;
			}

			if (m_pColorPalette != NULL)
			{
				m_pColorPalette[0] = Color;
			}

			ScaleColor(Color, &m_BackgroundColor);
#ifdef DUMP_CGM
			TRACE( "Background Color : Input [%d,%d,%d] : Scaled [%d,%d,%d]\n",
					Color.m_nRed, Color.m_nGreen, Color.m_nBlue,
					m_BackgroundColor.m_nRed, m_BackgroundColor.m_nGreen, m_BackgroundColor.m_nBlue );
#endif
			break;
		}
		default:
		{
			break;
		}
	}
	return TRUE;
}


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::DrawControl()
//
// Description:		Reads the control data.  Mostly for reading the VDC precision value.
//
// Returns:				TRUE if the command could be successfully processed.
//							FALSE if the command was invalid.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::DrawControl(SHORT nID)
{
	switch (nID)
	{
		case CGM_VDC_INTEGER_PRECISION:
		{
			if ( !ReadPrecisionData(&m_nVdcIntegerPrecision) )
			{
				return FALSE;
			}
#ifdef DUMP_CGM
			TRACE("VDC Integer Precision [%d]\n", m_nVdcIntegerPrecision);
#endif
			break;
		}
		default:
		{
			break;
		}
	}
	return TRUE;
}



// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::DrawPrimitive()
//
// Description:		Reads the data for a primitive and translates it into the
//							base RSingleGraphic data format.
//
// Returns:				TRUE if the command could be successfully processed.
//							FALSE if the command was invalid.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::DrawPrimitive(SHORT nID, SHORT nLength)
{
	// Reset in preparation for drawing a primitive
	switch (nID)
	{
		case CGM_RECTANGLE:
		{
			ProcessRectangle();
			break;
		}
		case CGM_POLYLINE:
		{
			// Compute the number of points.

			int nNumPoints = nLength / (m_nVdcIntegerPrecision*2);
			if (nNumPoints == 0)
			{
				// Fairly degenerate.
				break;
			}

			if (!ProcessPolyline(nNumPoints))
			{
				return FALSE;
			}
			break;
		}
		case CGM_POLYGON:
		{
			int nNumPoints = nLength / (m_nVdcIntegerPrecision*2);
			if (nNumPoints == 0)
			{
				// Fairly degenerate.
				break;
			}

			if (!ProcessPolygon(nNumPoints))
			{
				return FALSE;
			}
			break;
		}
		case CGM_ELLIPSE:
		{
			if (!ProcessEllipse())
			{
				return FALSE;
			}
			break;
		}
		case CGM_CIRCLE:
		{
			if (!ProcessCircle())
			{
				return FALSE;
			}
			break;
		}
		case CGM_CIRC_ARC_3PT:
		case CGM_CIRC_ARC_3PT_CLOSE:
		{
			if (!Process3PtCircularArc(nID))
			{
				return FALSE;
			}
			break;
		}
		case CGM_CIRC_ARC_CENTER:
		case CGM_CIRC_ARC_CENTER_CLOSE:
		{
			if (!ProcessCenteredCircularArc(nID))
			{
				return FALSE;
			}

			break;
		}
		case CGM_ELLI_ARC:
		case CGM_ELLI_ARC_CLOSE:
		{
			if (!ProcessEllipticalArc(nID))
			{
				return FALSE;
			}

			break;
		}
#if 0
		case CGM_DISJOINT_POLYLINE:
#endif
		case CGM_POLYMARKER:
		{
			// Compute how many points we have.
			int nNumPoints = nLength / (m_nVdcIntegerPrecision*2);

			// Draw the poly marker.
			if (!ProcessPolymarker((SHORT)nNumPoints))
			{
				return FALSE;
			}

			break;
		}
		default:
		{
			break;
		}
	}
	return TRUE;
}



// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::DrawAttribute()
//
// Description:		Sets up the drawing attributes.  Color, fill method, line
//							width and the like.
//
// Returns:				None
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::DrawAttribute(SHORT nID, SHORT nLength)
{
	switch (nID)
	{
		case CGM_LINE_TYPE:
		{
			if ( !ReadShortData(&m_nLineType, m_nIndexPrecision) )
			{
				return FALSE;
			}
#ifdef DUMP_CGM
			TRACE( "Line Type [%d]\n", m_nLineType );
#endif
			break;
		}
		case CGM_LINE_WIDTH:
		{
			if ( !ReadShortData(&m_nLineWidth, m_nVdcIntegerPrecision) )
			{
				return FALSE;
			}
#ifdef DUMP_CGM
			TRACE( "Line Width [%d]\n", m_nLineWidth );
#endif
			break;
		}
		case CGM_LINE_COLOR:
		{
			if (!ReadColorWithScale(&m_LineColor))
			{
				return FALSE;
			}
#ifdef DUMP_CGM
			TRACE( "Line Color [%d,%d,%d]\n", m_LineColor.m_nRed, m_LineColor.m_nGreen, m_LineColor.m_nBlue );
#endif
			break;
		}
		case CGM_MARKER_TYPE:
		{
			if ( !ReadShortData((SHORT*)&m_eMarkerType, m_nIndexPrecision) )
			{
				return FALSE;
			}
#ifdef DUMP_CGM
			TRACE( "Marker Type [" );
			switch (m_eMarkerType)
			{
				case MARKER_DOT:
					TRACE( "Dot" );
					break;
				case MARKER_PLUS:
					TRACE( "Plus" );
					break;
				case MARKER_ASTERISK:
					TRACE( "Asterisk" );
					break;
				case MARKER_CIRCLE:
					TRACE( "Circle" );
					break;
				case MARKER_CROSS:
					TRACE( "Cross" );
					break;
				case MARKER_RESERVED:
				default:
					TRACE( "Reserved - Will Remap" );
					break;
			}
			TRACE( "]\n" );
#endif
			if ((SHORT)m_eMarkerType >= MARKER_RESERVED)
			{
				// Map it onto a good range.
				m_eMarkerType = (MARKER_TYPE)((SHORT)MARKER_DOT
						+ ((SHORT)m_eMarkerType - MARKER_RESERVED) % (MARKER_RESERVED-MARKER_DOT));
			}
			break;
		}
		case CGM_MARKER_SIZE:
		{
			if ( !ReadShortData(&m_nMarkerSize, m_nVdcIntegerPrecision) )
			{
				return FALSE;
			}
#ifdef DUMP_CGM
			TRACE( "Marker Size [%d]\n", m_nMarkerSize );
#endif
			break;
		}
		case CGM_MARKER_COLOR:
		{
			if (!ReadColorWithScale(&m_MarkerColor))
			{
				return FALSE;
			}
#ifdef DUMP_CGM
			TRACE( "Marker Color [%d,%d,%d]\n", m_MarkerColor.m_nRed, m_MarkerColor.m_nGreen, m_MarkerColor.m_nBlue );
#endif
			break;
		}
		case CGM_INTERIOR_STYLE:
		{
			if (!ReadShortData( &m_nInteriorStyle ))
			{
				return FALSE;
			}
#ifdef DUMP_CGM
			TRACE( "Interior Style [%d]\n", m_nInteriorStyle );
#endif
			break;
		}
		case CGM_FILL_COLOR:
		{
			if (!ReadColorWithScale(&m_FillColor))
			{
				return FALSE;
			}
#ifdef DUMP_CGM
			TRACE( "Fill Color [%d,%d,%d]\n", m_FillColor.m_nRed, m_FillColor.m_nGreen, m_FillColor.m_nBlue );
#endif
			break;
		}
		case CGM_HATCH_INDEX:
		{
			if ( !ReadShortData(&m_nHatchIndex, m_nIndexPrecision) )
			{
				return FALSE;
			}
#ifdef DUMP_CGM
			TRACE( "Hatch Index [%d]\n", m_nHatchIndex );
#endif
			break;
		}
#if 0
		// Due to being disabled, I didn't translate these calls from the PM style. They would be as above.  -RIP-
		case CGM_TEXT_COLOR:
		{
			CGM_COLOR color;

			read_cgm_color(&color);
			set_text_color(color);
			break;
		}
#endif
		case CGM_EDGE_TYPE:
		{
			if ( !ReadShortData(&m_nEdgeType, m_nIndexPrecision) )
			{
				return FALSE;
			}
#ifdef DUMP_CGM
			TRACE( "Edge Type [%d]\n", m_nEdgeType );
#endif
			break;
		}
		case CGM_EDGE_WIDTH:
		{
			if ( !ReadShortData(&m_nEdgeWidth, m_nVdcIntegerPrecision) )
			{
				return FALSE;
			}
#ifdef DUMP_CGM
			TRACE( "Edge Width [%d]\n", m_nEdgeWidth );
#endif
			break;
		}
		case CGM_EDGE_COLOR:
		{
			if (!ReadColorWithScale(&m_EdgeColor))
			{
				return FALSE;
			}
#ifdef DUMP_CGM
			TRACE( "Edge Color [%d,%d,%d]\n", m_EdgeColor.m_nRed, m_EdgeColor.m_nGreen, m_EdgeColor.m_nBlue );
#endif
			break;
		}
		case CGM_EDGE_VISIBILITY:
		{
			SHORT nVisibility;
			if (!ReadShortData( &nVisibility ))
			{
				return FALSE;
			}
			m_bEdgeVisibility = (nVisibility != FALSE);
#ifdef DUMP_CGM
			TRACE( "Edge Visibility [%d]\n", nVisibility );
#endif
			break;
		}
#if 0
// Not translated since these are all disabled.
		case CGM_FILL_REFERENCE_POINT:
		{
			read_vdc_point(&fill_reference);
			break;
		}
		case CGM_LINE_BUNDLE:
		case CGM_MARKER_BUNDLE:
		case CGM_TEXT_BUNDLE:
		case CGM_TEXT_FONT:
		case CGM_FILL_BUNDLE:
		case CGM_PATTERN_INDEX:
		case CGM_EDGE_BUNDLE:
#endif
		case CGM_COLOR_TABLE:
		{
			SHORT nIndex;
			SHORT nColors;
			SCgmColor *pColor;

			if (m_pColorPalette == NULL)
			{
				// Skip it.
				break;
			}

			// Read the starting color index
			ReadShortData(&nIndex, m_nColorIndexPrescision);

			// Compute how many colors are (technically) left.
			nColors = (nLength - m_nColorIndexPrescision)/(3*m_nColorPrecision);

#ifdef DUMP_CGM
			TRACE("{from %d, %d colors} : Read Precision %d\n", nIndex, nColors, m_nColorIndexPrescision);
#endif

			pColor = &m_pColorPalette[nIndex];
			while (nColors--)
			{
				if (nIndex <= m_nMaxColorIndex)
				{
					// Read a valid color.
					ReadColorTriple(pColor);
#ifdef SHOW_COLORS
//					if (debug_flags & 1024)
					{
						TRACE("     %d: (%d, %d, %d)\n", nIndex, pColor->m_nRed, pColor->m_nGreen, pColor->m_nBlue);
					}
#endif
					nIndex++;
					pColor++;
				}
				else
				{
					break;
				}
			}

			break;
		}
		default:
		{
			break;
		}
	}
	return TRUE;
}



// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::SetDefaults()
//
// Description:		Sets the default values used in processing the CGM data.
//							Since CGM data is processed as a flow of commands, it is
//							necessary to have an initial state from which to start.
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RCGMSingleGraphic::SetDefaults()
{
	m_nCurrentPenWidth = 1;

	// Color selection defaults to indexed.
	m_eColorSelectionMode = COLOR_SELECTION_INDEXED;

	// Set the default vdc type.
	m_nVdcType = 0;

	// Set the precision defaults to 16 bits.
	m_nVdcIntegerPrecision = 2;
	m_nIntegerPrecision = 2;
	m_nIndexPrecision = 2;

	// Set color precision to 8 bit.
	m_nColorPrecision = 1;
	m_nColorIndexPrescision = 1;

	// Set the color extent defaults.
	m_nMaxColorIndex = 63;

	m_bColorScale[COLOR_SCALE_RED] = FALSE;
	m_bColorScale[COLOR_SCALE_GREEN] = FALSE;
	m_bColorScale[COLOR_SCALE_BLUE] = FALSE;

	// Assume a 256 color palette.  We don't have to use it...
	m_pColorPalette = new SCgmColor[256];

	if (m_pColorPalette != NULL)
	{
		// Background color is white.
		m_pColorPalette[0].m_nRed =
		m_pColorPalette[0].m_nGreen =
		m_pColorPalette[0].m_nBlue = 255;
	}

	m_ptSourceOrigin.m_x = m_ptSourceOrigin.m_y = 0;
	m_ptSourceExtent.m_x = m_ptSourceExtent.m_y = 32767;
	m_sizeExtent.m_dx = m_sizeExtent.m_dy = 0;

	m_nFillMode = ALTERNATE;
}


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::ScaleColor
//
// Description:		Transforms the color from the color value specified into
//							a final color to be used for drawing.  This is based on
//							the defined color extents.
//
//							This routine can output to the same color variable if the
//							same color instance is used for the input and the output
//							variables.
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RCGMSingleGraphic::ScaleColor( const SCgmColor &Color, SCgmColor *pOutColor )
{
	// Scale if necessary.
	SCgmColor ColorLocal = Color;

	if (m_bColorScale[COLOR_SCALE_RED])
	{
		ColorLocal.m_nRed = ((Color.m_nRed - m_ColorOrigin.m_nRed) * 255) / m_ColorExtent.m_nRed;
	}

	if (m_bColorScale[COLOR_SCALE_GREEN])
	{
		ColorLocal.m_nGreen = ((Color.m_nGreen - m_ColorOrigin.m_nGreen) * 255) / m_ColorExtent.m_nGreen;
	}

	if (m_bColorScale[COLOR_SCALE_BLUE])
	{
		ColorLocal.m_nBlue = ((Color.m_nBlue - m_ColorOrigin.m_nBlue) * 255) / m_ColorExtent.m_nBlue;
	}

	// Set the color bits in the output color.
	pOutColor->m_nRed = ColorLocal.m_nRed;
	pOutColor->m_nGreen = ColorLocal.m_nGreen;
	pOutColor->m_nBlue = ColorLocal.m_nBlue;
}



// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::ReadCommand()
//
// Description:		Reads the command from the current stream buffer.
//
// Returns:				TRUE on success, FALSE if we ran out of data.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::ReadCommand( SHORT *pnClass, SHORT *pnID, SHORT *pnLength )
{
	// The command is stored in the stream as a short integer.  The high 4 bits
	// are the class of the command, the next 7 bits are the ID and the final 5
	// bits are the length of the command data.
	SHORT nCommand;
	if (!ReadShortData( &nCommand, 2 ))	// 2 byte precision.
	{
		return FALSE;	// We couldn't read the data, so fail.
	}

	*pnClass = (nCommand >> 12) & 0x0F;
	*pnID = (nCommand >> 5) & 0x7F;
	*pnLength = nCommand & 0x1F;

	return TRUE;
}

// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::ReadShortData()
//
// Description:		Reads a short integer worth of data into the variable
//							provided.  This is done respecting the precision value
//							specified.  The actual amount of buffer data read is the
//							size of the precision value, but only a short integer worth
//							of that data is kept and placed into the provied variable.
//
// Returns:				TRUE on success, FALSE if we ran out of data.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::ReadShortData( SHORT *pnShort, SHORT nPrecision )
{
	TpsAssert( nPrecision <= 4, "Invalid precision value for CGM read" );
	BYTE nData[4];

	// Read as many bytes out of the stream as we have precision.
	for (int nIndex = 0; nIndex < nPrecision; nIndex++)
	{
		if ((m_lBufferDataRead + 1) > m_uPsd3GraphicDataSize)
		{
			return FALSE;	// We are out of data, the read failed.
		}

		// Get our data value.
		nData[nIndex] = *m_pInputBuffer;
		// Advance the buffer.
		m_pInputBuffer++;
		m_lBufferDataRead++;
	}

	// Only keep the data that means something.  This could mean that we throw
	// data away!  We only want two bytes, but could read up to 4.
	if (nPrecision == 1)
	{
		*pnShort = (SHORT)nData[0];
	}
	else
	{
		// Put the first byte read in the high data position.
		*pnShort = (SHORT)nData[1] + (((SHORT)nData[0]) << 8);
	}

	return TRUE;
}


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::ReadData()
//
// Description:		Reads data from the input buffer and places it into the
//							data buffer given.  It is assumed that reading raw data
//							into the provied buffer will be understood correctly.
//
//							Even if the read runs out of data, the data that could be
//							read is placed into the buffer.
//
// Returns:				TRUE on success, FALSE if we ran out of data.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::ReadData( UCHAR *pData, LONG lBlockSize )
{
	// Read as many bytes out of the stream as we have precision.
	for (LONG lIndex = 0; lIndex < lBlockSize; lIndex++)
	{
		if ((m_lBufferDataRead + 1) > m_uPsd3GraphicDataSize)
		{
			return FALSE;	// We are out of data, the read failed.
		}

		// Get our data value.
		pData[lIndex] = *m_pInputBuffer;
		// Advance the buffer.
		m_pInputBuffer++;
		m_lBufferDataRead++;
	}

	return TRUE;
}


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::ReadPrecisionData()
//
// Description:		Reads a precision value from the data stream.
//
// Returns:				TRUE if the precision value was read successfully,
//							FALSE if we ran out of data.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::ReadPrecisionData( SHORT *pnPrecision )
{
	SHORT nNewPrecision;
	// Read the data from the buffer.
	if (!ReadShortData( &nNewPrecision, m_nIntegerPrecision ))
	{
		return FALSE;
	}

	// Translate from bits to bytes.
	nNewPrecision /= 8;
	if ((nNewPrecision > 0) && (nNewPrecision <= 4))
	{
		// Our precision is from 1 to 4, so accept the new value.
		*pnPrecision = nNewPrecision;
	}

	return TRUE;
}


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::ReadColorTriple()
//
// Description:		Reads an RGB color value out of the data stream.
//
// Returns:				TRUE if the color was read successfully, FALSE if we ran
//							out of data.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::ReadColorTriple( SCgmColor *pColor )
{
	SCgmColor Color;

	SHORT nColor;
	if (ReadShortData( &nColor, m_nColorPrecision ))
	{
		Color.m_nRed = (BYTE)(nColor & 0x00FF);
		if (ReadShortData( &nColor, m_nColorPrecision ))
		{
			Color.m_nGreen = (BYTE)(nColor & 0x00FF);
			if (ReadShortData( &nColor, m_nColorPrecision ))
			{
				Color.m_nBlue = (BYTE)(nColor & 0x00FF);

				// We have a legal color, so set the color given and say we succeeded.
				*pColor = Color;
				return TRUE;
			}
		}
	}

	return FALSE;
}


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::ReadColorWithScale()
//
// Description:		Reads an RGB color value out of the data stream and
//							translates it into the current color extent space so that
//							it is ready to be used for drawing.
//
// Returns:				TRUE if the color was read successfully, FALSE if we ran
//							out of data.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::ReadColorWithScale( SCgmColor *pColor )
{
	TpsAssert( pColor != NULL, "Invalid color variable in call to ReadColorWithScale" );
	SCgmColor Color;

	if (m_eColorSelectionMode == COLOR_SELECTION_INDEXED)
	{
		SHORT nIndex;

		// Read the index.

		if (!ReadShortData(&nIndex, m_nColorIndexPrescision))
		{
			return FALSE;
		}

		// Validate the index.
		if (nIndex < 0)
		{
			nIndex = 0;
		}
		if (nIndex > m_nMaxColorIndex)
		{
			nIndex = m_nMaxColorIndex;
		}

		// Plug the color in as desired.
		if (m_pColorPalette == NULL)
		{
			Color.m_nRed = Color.m_nGreen = Color.m_nBlue = 0;
		}
		else
		{
			Color = m_pColorPalette[nIndex];
		}
#ifdef SHOW_COLORS
//		if (debug_flags & 1024)
		{
			TRACE("     %d : (%d, %d, %d)\n", nIndex, Color.m_nRed, Color.m_nGreen, Color.m_nBlue);
		}
#endif
	}
	else
	{
		// Must be DIRECT
		if ( !ReadColorTriple(&Color) )
		{
			return FALSE;
		}
	}

	ScaleColor(Color, pColor);

	return TRUE;
}


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::ReadPoint()
//
// Description:		Reads a virtual display coordinate point from the input buffer.
//							In order to ensure that the graphic is placed in the
//							VDC area correctly, the source origin is subtracted from
//							each point as it is read in.
//
// Returns:				TRUE if the color was read successfully, FALSE if we ran
//							out of data.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::ReadPoint( RIntPoint *pPoint )
{
	SHORT nX;
	SHORT nY;

	if ( !ReadShortData( &nX, m_nVdcIntegerPrecision ) ||
		  !ReadShortData( &nY, m_nVdcIntegerPrecision ) )
	{
		return FALSE;
	}

	pPoint->m_x = (YIntCoordinate)nX - m_ptSourceOrigin.m_x;
	pPoint->m_y = (YIntCoordinate)nY - m_ptSourceOrigin.m_y;

	return TRUE;
}



// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::EnsureDataOnWordBoundary()
//
// Description:		Skips a data byte to ensure that the input buffer read
//							position is on a word boundary.
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RCGMSingleGraphic::EnsureDataOnWordBoundary()
{
	if (m_lBufferDataRead & 0x00000001)
	{
		// Advance the buffer.
		m_pInputBuffer++;
		m_lBufferDataRead++;
	}
}



// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::SeekRelative()
//
// Description:		Seeks to a new position in the buffer relative to the
//							current location in the buffer.
//
// Returns:				TRUE on a successful move, FALSE if we are out of data.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::SeekRelative( LONG lDistanceToMove )
{
	// Verify that we have enough data to move the specified distance.
	if ((m_lBufferDataRead + lDistanceToMove) > m_uPsd3GraphicDataSize)
	{
		// We don't so move to the end of the buffer, and return a failure.
		m_lBufferDataRead = m_uPsd3GraphicDataSize;
		return FALSE;
	}
	else
	{
		// Everything is OK so advance the buffer.
		m_pInputBuffer += lDistanceToMove;
		m_lBufferDataRead += lDistanceToMove;
		
		return TRUE;
	}
}


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::SeekAbsolute()
//
// Description:		Seeks to a specific point in the buffer data.
//
// Returns:				TRUE on a successful move, FALSE if we are out of data.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::SeekAbsolute( LONG lPosition )
{
	// Verify that we have enough data to move to the specified location.
	if (lPosition >= m_uPsd3GraphicDataSize)
	{
		// We don't so move to the end of the buffer, and return a failure.
		m_lBufferDataRead = m_uPsd3GraphicDataSize;
		return FALSE;
	}
	else
	{
		m_pInputBuffer = m_pPsd3GraphicData;
		m_pInputBuffer += lPosition;
		m_lBufferDataRead = lPosition;

		return TRUE;
	}
}


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::GetFillMethod()
//
// Description:		Returns the currently defined fill method and sets it in
//							the graphic for processing.
//
// Returns:				The value which defines the method of drawing for the fill.
//
// Exceptions:			None
//
// ****************************************************************************
//
YFillMethod RCGMSingleGraphic::SetFillMethod()
{
	YFillMethod nFillMethod = kFilled;	// Assume filled for the default.

	// Is there an edge?  i.e. are there outside lines/curves to be drawn?
	if (m_bEdgeVisibility != FALSE)
	{
		// Is the style not empty?
		if ( (m_nInteriorStyle != CGM_INTERIOR_STYLE_HOLLOW) &&
			  (m_nInteriorStyle != CGM_INTERIOR_STYLE_EMPTY) )
		{
			// OK, we have to fill and draw borders.
			nFillMethod = kFilledAndStroked;
		}
		else
		{
			// We only need to draw the outside, there is no fill.
			nFillMethod = kStroked;
		}

		// We are stroked, so put in place the line attributes.
		SetLineStyle(m_nEdgeType, m_nEdgeWidth, m_EdgeColor);
	}
	else
	{
		// OK, we don't have an outside, so we simply fill.
		TpsAssert( (m_nInteriorStyle != CGM_INTERIOR_STYLE_HOLLOW) && (m_nInteriorStyle != CGM_INTERIOR_STYLE_EMPTY), "No legal fill was specified.  Filled assumed." );
		nFillMethod = kFilled;
	}
	
	SetupFillMethod( nFillMethod, m_FillColor );

	return nFillMethod;
}



// ****************************************************************************
//
// Function Name:			RCGMSingleGraphic::SetupFillMethod()
//
// Description:			Sets up the graphic lists with the correct fill color
//								information.
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
void RCGMSingleGraphic::SetupFillMethod( YFillMethod& nFillMethod, const SCgmColor& FillColor )
{
	switch( nFillMethod )
	{
		case kStroked:
			//
			// we don't care about the fill color...
#ifdef DUMP_CGM
			TRACE("Object has outline only\n");
#endif
			break;
		case kFilled:
		case kFilledAndStroked:
		case kStrokedAndFilled:
			// set the current solid color...
			m_OperationsStream.InsertAtEnd( kSetFillColor );
			RSolidColor clrFill( FillColor.m_nRed, FillColor.m_nGreen, FillColor.m_nBlue );
			m_ColorsStream.InsertAtEnd( RColor(clrFill) );
#ifdef DUMP_CGM
			if (nFillMethod == kFilled)
			{
				TRACE( "Object is filled : Color [%d,%d,%d]\n",
					FillColor.m_nRed, FillColor.m_nGreen, FillColor.m_nBlue );
			}
			else if (nFillMethod == kFilledAndStroked)
			{
				TRACE( "Object is filled and stroked : Color [%d,%d,%d]\n",
					FillColor.m_nRed, FillColor.m_nGreen, FillColor.m_nBlue );
			}
			else
			{
				TRACE( "Object is stroked and filled : Color [%d,%d,%d]\n",
					FillColor.m_nRed, FillColor.m_nGreen, FillColor.m_nBlue );
			}
#endif
			break;
	}

	m_FillMethodsStream.InsertAtEnd( nFillMethod );
}


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::SetLineStyle()
//
// Description:		Returns the currently defined line style and sets it in
//							the graphic for processing.
//
// Returns:				The value which defines the style of line.
//
// Exceptions:			None
//
// ****************************************************************************
//
YPenStyle RCGMSingleGraphic::SetLineStyle(SHORT nStyle, SHORT nWidth, const SCgmColor& LineColor)
{
	YPenStyle penStyle;

	if (nStyle == CGM_LINE_TYPE_SOLID)
	{
		penStyle = kSolidPen;
	}
	else
	{
		penStyle = kDashPen;
	}

	// Insert the style of the line.
	m_OperationsStream.InsertAtEnd( kSetLineStyle );
	m_PenStylesStream.InsertAtEnd( penStyle );
#ifdef DUMP_CGM
	TRACE("Pen : Style [%s], ", (nStyle == CGM_LINE_TYPE_SOLID) ? "Solid" : "Dashed" );
#endif

	// Set our line width.
	m_OperationsStream.InsertAtEnd( kSetLineWidth );
	m_PenWidthsStream.InsertAtEnd( (YPenWidth)nWidth );
#ifdef DUMP_CGM
	TRACE("Width [%d], ", nWidth );
#endif

	// Set our line's color.
	RSolidColor rgbColor( LineColor.m_nRed, LineColor.m_nGreen, LineColor.m_nBlue );
	m_OperationsStream.InsertAtEnd( kSetLineColor );
	m_ColorsStream.InsertAtEnd( RColor(rgbColor) );
#ifdef DUMP_CGM
	TRACE("Color [%d,%d,%d]\n", LineColor.m_nRed, LineColor.m_nGreen, LineColor.m_nBlue );
#endif

	return penStyle;
}


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::ProcessRectangle()
//
// Description:		Reads the rectangle points out of the buffer stream and
//							creates a valid rectangle object in the base RSingleGraphic
//							class.
//
// Returns:				TRUE if the rectangle is successfully created, FALSE if we
//							run out of data.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::ProcessRectangle()
{
	RIntPoint Corner0, Corner1;

#ifdef DUMP_CGM
	TRACE("Processing a Rectangle :\n");
#endif

	// Read the two end points.
	if (!ReadPoint(&Corner0) ||
		 !ReadPoint(&Corner1))
	{
		return FALSE;
	}

	// Normalize the rectangle defined by these corners.
	// This also fixes up rectangles which are defined by the upper right
	// and lower left corners which would create opposite direction winding.
	if (Corner0.m_x > Corner1.m_x)	// Swap the X values if need be.
	{
		YIntCoordinate nTemp = Corner0.m_x;
		Corner0.m_x = Corner1.m_x;
		Corner1.m_x = nTemp;
	}
	if (Corner0.m_y > Corner1.m_y)	// Swap the Y values if need be.
	{
		YIntCoordinate nTemp = Corner0.m_y;
		Corner0.m_y = Corner1.m_y;
		Corner1.m_y = nTemp;
	}

	// Adjust our size to encompass the object.
	BumpMetafileExtent( Corner0, Corner1, m_nEdgeWidth );

	// Process it as a rectangle.

	// Set the fill items.  This includes an edge if visible.
	SetFillMethod();

	// Convert the box into a polygon of five points so that it can be
	// easily rotated.  We use 5 points instead of 4, because we must
	// explicitly close the polygon.
	m_OperationsStream.InsertAtEnd( kPolygon );
	m_PointCountsStream.InsertAtEnd( 5 );

	// Top left	- Assuming top left is (0,0) and bottom right is (n,n) based.
	m_OriginalPointsStream.InsertAtEnd( RRealPoint( Corner0 ) );
	// Bottom left
	m_OriginalPointsStream.InsertAtEnd( RRealPoint( Corner0.m_x, Corner1.m_y ) );
	// Bottom right
	m_OriginalPointsStream.InsertAtEnd( RRealPoint( Corner1 ) );
	// Top right
	m_OriginalPointsStream.InsertAtEnd( RRealPoint( Corner1.m_x, Corner0.m_y ) );
	// Top left again to close off the box.
	m_OriginalPointsStream.InsertAtEnd( RRealPoint( Corner0 ) );

	// Take care of the first box definition.
	if (!m_fFirstBox)
	{
		m_rFirstBox.m_Top = Corner0.m_y;
		m_rFirstBox.m_Left = Corner0.m_x;
		m_rFirstBox.m_Bottom = Corner1.m_y;
		m_rFirstBox.m_Right = Corner1.m_x;
		m_fFirstBox = TRUE;
	}
#ifdef DUMP_CGM
	TRACE("Top Left [%d,%d], Bottom Right [%d,%d]\n",
		Corner0.m_x, Corner0.m_y, Corner1.m_x, Corner1.m_y );
#endif

	return TRUE;
}



// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::ProcessPolyline()
//
// Description:		Reads the polyline points out of the buffer stream and
//							creates a valid polyline object in the base RSingleGraphic
//							class.
//
// Returns:				TRUE if the line is successfully created, FALSE if we run
//							out of data.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::ProcessPolyline( SHORT nNumPoints )
{
#ifdef DUMP_CGM
	TRACE("Processing a Polyline :\n");
#endif
	// Set the line drawing attributes.
	SetLineStyle( m_nLineType, m_nLineWidth, m_LineColor );

	// Polyline is an opcode followed by a point count
	m_OperationsStream.InsertAtEnd( kPolyline );
	// Read the points and place them into the list.
	return ProcessPointStream( nNumPoints, TRUE );	// Line based
}


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::ProcessPolygon()
//
// Description:		Reads the polygon points out of the buffer stream and
//							creates a valid polygon object in the base RSingleGraphic
//							class.
//
// Returns:				TRUE if the line is successfully created, FALSE if we run
//							out of data.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::ProcessPolygon( SHORT nNumPoints )
{
#ifdef DUMP_CGM
	TRACE("Processing a Polygon :\n");
#endif
	// Set our fill method for the polygon.  This will also take care of our edge.
	SetFillMethod();

	// Create our polygon data...
	m_OperationsStream.InsertAtEnd( kPolygon );
	// Read the points and place them into the list.
	return ProcessPointStream( nNumPoints, FALSE );	// Not line based. (Edge based)
}


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::ProcessPointStream()
//
// Description:		Reads a series of points out of the buffer stream and
//							places them into the point list in the RSingleGraphic.
//
// Returns:				TRUE if the points are read successfully, FALSE if we run
//							out of data.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::ProcessPointStream( SHORT nNumPoints, BOOLEAN bLineBased )
{
	// Put the count of points to be read onto the counts stream.
	m_PointCountsStream.InsertAtEnd( nNumPoints );

#ifdef DUMP_CGM
	int nPointCount = 0;
	TRACE("Number of Stream Points [%d]\n", nNumPoints );
	TRACE("Point Stream : ");
#endif
	// Read the points and place them into the list.
	RIntPoint ptOnLine;
	for (int nIndex = 0; nIndex < nNumPoints; nIndex++)
	{
		if (!ReadPoint( &ptOnLine ))
		{
#ifdef DUMP_CGM
			if ((nPointCount % 8) != 0)
			{
				TRACE( "\nOut of data reading Point Stream\n" );
			}
#endif
			return FALSE;
		}

		// Ensure the size and extents of the object have been adjusted for the point.
		int nLineEdgeWidth = (bLineBased) ? m_nLineWidth : m_nEdgeWidth;
		BumpMetafileExtent( ptOnLine, ptOnLine, nLineEdgeWidth );

		m_OriginalPointsStream.InsertAtEnd( RRealPoint( ptOnLine ) );

#ifdef DUMP_CGM
		if (((nPointCount % 8) == 0) && nPointCount)
		{
			// Space it out for readability.
			TRACE("               ");
		}
		TRACE("[%d,%d] ", ptOnLine.m_x, ptOnLine.m_y);
		nPointCount++;
		if ((nPointCount % 8) == 0)
		{
			TRACE( "\n" );
		}
#endif
	}

#ifdef DUMP_CGM
	if ((nPointCount % 8) != 0)
	{
		TRACE( "\nEnd of Point Stream\n" );
	}
#endif

	return TRUE;
}


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::ProcessElipse()
//
// Description:		Reads an elipse out of the buffer stream and
//							places it into the lists in the RSingleGraphic.
//
// Returns:				TRUE if the points are read successfully, FALSE if we run
//							out of data.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::ProcessEllipse()
{
	RIntPoint ptCenter;
	RIntPoint ptConjugateDiameter1;
	RIntPoint ptConjugateDiameter2;

	if (!ReadPoint(&ptCenter) ||					/* Read the center. */
		 !ReadPoint(&ptConjugateDiameter1) ||	/* Endpoint of conjugate diameter 1. */
		 !ReadPoint(&ptConjugateDiameter2) )	/* Endpoint of conjugate diameter 2. */
	{
		return FALSE;
	}

#ifdef DUMP_CGM
	TRACE("Draw Ellipse : Center [%d,%d], Diameter 1 [%d,%d], Diameter 2 [%d,%d]\n",
			ptCenter.m_x, ptCenter.m_y,
			ptConjugateDiameter1.m_x, ptConjugateDiameter1.m_y,
			ptConjugateDiameter2.m_x, ptConjugateDiameter2.m_y );
#endif

	// Create our bounding rectangle for the ellipse...
	RIntPoint ptUpperLeft;
	RIntPoint ptLowerRight;
	if (ptConjugateDiameter1.m_x == ptCenter.m_x && ptConjugateDiameter2.m_y == ptCenter.m_y)
	{
		// Normal orientation.
		ptUpperLeft.m_x = ptCenter.m_x - (ptConjugateDiameter2.m_x - ptCenter.m_x);
		ptUpperLeft.m_y = ptCenter.m_y - (ptConjugateDiameter1.m_y - ptCenter.m_y);
	}
	else if (ptConjugateDiameter2.m_x == ptCenter.m_x && ptConjugateDiameter1.m_y == ptCenter.m_y)
	{
		// Normal orientation.
		ptLowerRight.m_x = ptCenter.m_x + (ptConjugateDiameter1.m_x - ptCenter.m_x);
		ptLowerRight.m_y = ptCenter.m_y + (ptConjugateDiameter2.m_y - ptCenter.m_y);
	}
	else
	{
#ifdef DUMP_CGM
		TRACE("*** BAD ELLIPSE ***\n");
#endif
		return TRUE;	// Don't abort though, we just don't know how to deal with this.
	}

	// Normalize the points to ensure we aren't inverted.
	if (ptUpperLeft.m_x > ptLowerRight.m_x)
	{
		YIntCoordinate nTemp = ptLowerRight.m_x;
		ptLowerRight.m_x = ptUpperLeft.m_x;
		ptUpperLeft.m_x = nTemp;
	}
	if (ptUpperLeft.m_y > ptLowerRight.m_y)
	{
		YIntCoordinate nTemp = ptLowerRight.m_y;
		ptLowerRight.m_y = ptUpperLeft.m_y;
		ptUpperLeft.m_y = nTemp;
	}

	// Ensure the object sizing has been corrected.
	BumpMetafileExtent( ptUpperLeft, ptLowerRight, m_nEdgeWidth );

	// OK, we're here so we have a good ellipse and we can put it to the base class.

	// Output the fill data.
	SetFillMethod();

	// Specify that we are rendering an ellipse...
	m_OperationsStream.InsertAtEnd( kEllipse );

	// Put out the points...
	m_PointCountsStream.InsertAtEnd( 2 );
	m_OriginalPointsStream.InsertAtEnd( RRealPoint( ptUpperLeft ) );
	m_OriginalPointsStream.InsertAtEnd( RRealPoint( ptLowerRight ) );

	return TRUE;
}


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::ProcessCircle()
//
// Description:		Reads a circle out of the buffer stream and
//							places it into the lists in the RSingleGraphic.
//
// Returns:				TRUE if the points are read successfully, FALSE if we run
//							out of data.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::ProcessCircle()
{
	RIntPoint ptCenter;
	SHORT nRadius;

	if (!ReadPoint(&ptCenter) ||
		 !ReadShortData(&nRadius, m_nVdcIntegerPrecision) )
	{
		return FALSE;
	}

//	draw_ellipse(center, radius, radius);

#ifdef DUMP_CGM
	TRACE("Draw Circle : Center [%d,%d], Radius [%d]\n",
			ptCenter.m_x, ptCenter.m_y, nRadius );
#endif

	// A circle is just a special case of an elipse, so we'll simply perform
	// an eliptical render with a square for our bounding rect.

	// Create our bounding rectangle for the circle...
	RIntPoint ptUpperLeft;
	RIntPoint ptLowerRight;

	ptUpperLeft.m_x = ptCenter.m_x - nRadius;
	ptUpperLeft.m_y = ptCenter.m_y - nRadius;
	ptLowerRight.m_x = ptCenter.m_x + nRadius;
	ptLowerRight.m_y = ptCenter.m_y + nRadius;

	// Ensure the object sizing has been corrected.
	BumpMetafileExtent( ptUpperLeft, ptLowerRight, m_nEdgeWidth );

	// Output the fill data.
	SetFillMethod();

	// Specify that we are rendering an ellipse...
	m_OperationsStream.InsertAtEnd( kEllipse );

	// Put out the points...
	m_PointCountsStream.InsertAtEnd( 2 );
	m_OriginalPointsStream.InsertAtEnd( RRealPoint( ptUpperLeft ) );
	m_OriginalPointsStream.InsertAtEnd( RRealPoint( ptLowerRight ) );

	return TRUE;
}


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::Process3PtCircularArc()
//
// Description:		Reads a circular 3pt arc out of the buffer stream and
//							places it into the lists in the RSingleGraphic.
//
// Returns:				TRUE if the points are read successfully, FALSE if we run
//							out of data.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::Process3PtCircularArc(int nID)
{
	RIntPoint ptStart;
	RIntPoint ptIntermediate;
	RIntPoint ptEnd;
	SHORT nClosure = -1;

	// Read the parameters.
	if (!ReadPoint(&ptStart) ||
		 !ReadPoint(&ptIntermediate) ||
		 !ReadPoint(&ptEnd) )
	{
		return FALSE;
	}
	if (nID == CGM_CIRC_ARC_3PT_CLOSE)
	{
		if (!ReadShortData(&nClosure))
		{
			return FALSE;
		}
	}

	// *** NOTE ***
	// We don't currently support the rendering of this type of arc data, so
	// we don't actually post it to the base RSingleGraphic class.  To support
	// this data, we would need to determine how to interpret this data and
	// create correct list data for the base class.

	// We would also need to correctly update the sizing information based on
	// the BumpMetafileExtent call.
#ifdef DUMP_CGM
	TRACE("Draw 3 point Circular Arc : Start [%d,%d], Intermediate [%d,%d], End [%d,%d] - Ignored\n",
			ptStart.m_x, ptStart.m_y, ptIntermediate.m_x, ptIntermediate.m_y, ptEnd.m_x, ptEnd.m_y );
#endif

	return TRUE;
}


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::ProcessCenteredCircularArc()
//
// Description:		Reads a centered circular arc out of the buffer stream and
//							places it into the lists in the RSingleGraphic.
//
// Returns:				TRUE if the points are read successfully, FALSE if we run
//							out of data.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::ProcessCenteredCircularArc(SHORT nID)
{
	RIntPoint ptCenter;
	RIntPoint ptStartDelta;
	RIntPoint ptEndDelta;
	SHORT nRadius;
	SHORT nClosure = -1;
	double fStartAngle;
	double fEndAngle;

	// Read the parameters.
	if (!ReadPoint(&ptCenter) ||
		 !ReadPoint(&ptStartDelta) ||
		 !ReadPoint(&ptEndDelta) ||
		 !ReadShortData(&nRadius, m_nVdcIntegerPrecision) )
	{
		return FALSE;
	}

	if (nID == CGM_CIRC_ARC_CENTER_CLOSE)
	{
		if (!ReadShortData(&nClosure))
		{
			return FALSE;
		}
	}

	// Compute the angles.
	fStartAngle = AngleFromVector(ptStartDelta.m_x, ptStartDelta.m_y);
	fEndAngle = AngleFromVector(ptEndDelta.m_x, ptEndDelta.m_y);

#ifdef DUMP_CGM
	TRACE("Centered Circular Arc : Center [%d,%d], Start Delta [%d,%d], End Delta [%d,%d], Radius [%d], Closure [%d] (-1 = none, 0 = chord, 1 = pie)\n",
			ptCenter.m_x, ptCenter.m_y, ptStartDelta.m_x, ptStartDelta.m_y, ptEndDelta.m_x, ptEndDelta.m_y, nRadius, nClosure );
	TRACE("  Drawn via an elliptical arc using : Start Angle [%d], End Angle [%d]\n",
			(fStartAngle * 180.0 / 3.141592654), (fEndAngle * 180.0 / 3.141592654) );
#endif

	// Add the arc.
	ProcessEllipticalArc(ptCenter, (double)nRadius, (double)nRadius, fStartAngle, fEndAngle, nClosure);

	return TRUE;
}

// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::ProcessEllipticalArc()
//
// Description:		Reads an elliptical arc out of the buffer stream and
//							places it into the lists in the RSingleGraphic.
//
// Returns:				TRUE if the points are read successfully, FALSE if we run
//							out of data.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::ProcessEllipticalArc(SHORT nID)
{
	RIntPoint ptCenter;
	RIntPoint ptConjugateDiameter1;
	RIntPoint ptConjugateDiameter2;
	RIntPoint ptStartDelta;
	RIntPoint ptEndDelta;
	SHORT nClosure = -1;
	SHORT nRadiusX;
	SHORT nRadiusY;
	double fStartAngle;
	double fEndAngle;

	/* Read the parameters. */

	if (!ReadPoint(&ptCenter) ||
		 !ReadPoint(&ptConjugateDiameter1) ||
		 !ReadPoint(&ptConjugateDiameter2) ||
		 !ReadPoint(&ptStartDelta) ||		/* stored as dx, dy */
		 !ReadPoint(&ptEndDelta) )			/* stored as dx, dy */
	{
		return FALSE;
	}

	if (nID == CGM_ELLI_ARC_CLOSE)
	{
		if (!ReadShortData(&nClosure))
		{
			return FALSE;
		}
	}

#ifdef DUMP_CGM
	TRACE("Elliptical Arc : Center [%d,%d], Conjugate Diameter 1 [%d,%d], Conjugate Diameter 2 [%d,%d], Start Delta [%d,%d], End Delta [%d,%d], Closure [%d] (-1 = none, 0 = chord, 1 = pie)\n",
			ptCenter.m_x, ptCenter.m_y, ptConjugateDiameter1.m_x, ptConjugateDiameter1.m_y, ptConjugateDiameter2.m_x, ptConjugateDiameter2.m_y, ptStartDelta.m_x, ptStartDelta.m_y, ptEndDelta.m_x, ptEndDelta.m_y, nClosure );
#endif

	// Compute the start and end angles.
	fStartAngle = AngleFromVector(ptStartDelta.m_x, ptStartDelta.m_y);
	fEndAngle = AngleFromVector(ptEndDelta.m_x, ptEndDelta.m_y);

	if (ptConjugateDiameter1.m_x == ptCenter.m_x && ptConjugateDiameter2.m_y == ptCenter.m_y)
	{
		// Normal orientation.
		nRadiusX = ptConjugateDiameter2.m_x - ptCenter.m_x;
		nRadiusY = ptConjugateDiameter1.m_y - ptCenter.m_y;
	}
	else if (ptConjugateDiameter2.m_x == ptCenter.m_x && ptConjugateDiameter1.m_y == ptCenter.m_y)
	{
		// Normal orientation.
		nRadiusX = ptConjugateDiameter1.m_x - ptCenter.m_x;
		nRadiusY = ptConjugateDiameter2.m_y - ptCenter.m_y;
	}
	else
	{
#ifdef DUMP_CGM
		TRACE("*** BAD ELLIPSE ***\n");
#endif
		return TRUE;	// Don't abort the load, we will simply ignore this object.
	}

	// Add the arc.
	ProcessEllipticalArc(ptCenter, (double)nRadiusX, (double)nRadiusY, fStartAngle, fEndAngle, nClosure);

	return TRUE;
}


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::ProcessPolymarker()
//
// Description:		Reads a poly marker out of the buffer stream and
//							places it into the lists in the RSingleGraphic.
//
// Returns:				TRUE if the points are read successfully, FALSE if we run
//							out of data.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RCGMSingleGraphic::ProcessPolymarker(SHORT nNumPoints)
{
#ifdef DUMP_CGM
	TRACE("Reading off a stream of %d poly markers - Output not supported\n", nNumPoints);
#endif

	RIntPoint ptCenter;
	for (int nIndex = 0; nIndex < nNumPoints; nIndex++)
	{
		if ( !ReadPoint( &ptCenter ) )
		{
			return FALSE;
		}

		// TO DO: If we decide to support poly markers, write a function to do it
		// and put it here.
	}

	return TRUE;
}



// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::AngleFromVector()
//
// Description:		Converts a vector given in X and Y dimmensions into radian
//							angle measurements.
//
// Returns:				The angle of the vector in radians.
//
// Exceptions:			None
//
// ****************************************************************************
//
double RCGMSingleGraphic::AngleFromVector( SHORT nXLength, SHORT nYLength )
{
	return atan2((double)nYLength, (double)nXLength);
}


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::ProcessEllipticalArc()
//
// Description:		Takes the definitions for an eliptical arc and converts it
//							into a series of bezier points which produce the resulting
//							arc as desired.
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RCGMSingleGraphic::ProcessEllipticalArc(const RIntPoint &ptCenter, double fRadiusX, double fRadiusY, double fStartAngle, double fEndAngle, SHORT nClosure)
{
	// Closure values:
	// -1 = no closure (simple arc)
	//  0 = chord closure (end to end)
	//  1 = pie closure (end to center to end)

	// Compute points for bezier generation of an ellipse.
	RRealPoint Points[13];

	double fLeft = (double)ptCenter.m_x - fRadiusX;
	double fTop = (double)ptCenter.m_y - fRadiusY;
	double fRight = ptCenter.m_x + fRadiusX;
	double fBottom = ptCenter.m_y + fRadiusY;
	double fXOffset = fRadiusX*11/20;	// Where does this come from?
	double fYOffset = fRadiusY*11/20;

	// Ensure the object is large enough.
	RIntPoint ptUpperLeft((YIntCoordinate)fLeft, (YIntCoordinate)fTop);
	RIntPoint ptLowerRight((YIntCoordinate)fRight, (YIntCoordinate)fBottom);
	if (ptUpperLeft.m_x > ptLowerRight.m_x)
	{
		YIntCoordinate nX = ptUpperLeft.m_x;
		ptUpperLeft.m_x = ptLowerRight.m_x;
		ptLowerRight.m_x = nX;
	}
	if (ptUpperLeft.m_y > ptLowerRight.m_y)
	{
		YIntCoordinate nY = ptUpperLeft.m_y;
		ptUpperLeft.m_y = ptLowerRight.m_y;
		ptLowerRight.m_y = nY;
	}
	BumpMetafileExtent( ptUpperLeft, ptLowerRight, (nClosure == -1) ? m_nLineWidth : m_nEdgeWidth );

	// Construct the points for a full "bezier ellipse".  We'll break it into an
	// arc below.
	Points[0].m_x = fRight;
	Points[0].m_y = ptCenter.m_y;
	Points[1].m_x = fRight;
	Points[1].m_y = ptCenter.m_y - fYOffset;
	Points[2].m_x = ptCenter.m_x + fXOffset;
	Points[2].m_y = fTop;
	Points[3].m_x = ptCenter.m_x;
	Points[3].m_y = fTop;
	Points[4].m_x = ptCenter.m_x - fXOffset;
	Points[4].m_y = fTop;
	Points[5].m_x = fLeft;
	Points[5].m_y = ptCenter.m_y - fYOffset;
	Points[6].m_x = fLeft;
	Points[6].m_y = ptCenter.m_y;
	Points[7].m_x = fLeft;
	Points[7].m_y = ptCenter.m_y + fYOffset;
	Points[8].m_x = ptCenter.m_x - fXOffset;
	Points[8].m_y = fBottom;
	Points[9].m_x = ptCenter.m_x;
	Points[9].m_y = fBottom;
	Points[10].m_x = ptCenter.m_x + fXOffset;
	Points[10].m_y = fBottom;
	Points[11].m_x = fRight;
	Points[11].m_y = ptCenter.m_y + fYOffset;
	Points[12] = Points[0];

	// Keep angles between 0 and 2*PI.
	fStartAngle = NormalizeAngle(fStartAngle);
	fEndAngle = NormalizeAngle(fEndAngle);

	const double fPIOver2 = kPI/2.0;
	const double fPI3Over2 = kPI + fPIOver2;

	// Loop until done.
	double fAngle = fStartAngle;
	BOOL bFirst = TRUE;
//	COutlinePath Path;
	RRealPoint ptFirst;

	int nBezierCurves = 0;
	int nPointCount = 0;

	// Output the fill data.
	SetFillMethod();

	// Start a free form path, we'll then fill it with the correct bezier points.
	m_OperationsStream.InsertAtEnd( kPath );
	do
	{
		double fMin, fMax;
		int nPoint;

		// Figure out which quadrant we are in.
		if (fAngle < fPIOver2)
		{
			fMin = 0;
			fMax = fPIOver2;
			nPoint = 0;
		}
		else if (fAngle < kPI)
		{
			// Quadrant 1.
			fMin = fPIOver2;
			fMax = kPI;
			nPoint = 3;
		}
		else if (fAngle < fPI3Over2)
		{
			// Quadrant 2.
			fMin = kPI;
			fMax = fPI3Over2;
			nPoint = 6;
		}
		else
		{
			// Quadrant 3.
			fMin = fPI3Over2;
			fMax = k2PI;
			nPoint = 9;
		}

		// We have computed the quadrant. The 4 points starting at nPoint
		// define the bezier curve for this quadrant.

		// Compute where to stop within this quadrant.
		double fThisEnd = (fEndAngle >= fMin && fEndAngle <= fMax) ? fEndAngle : fMax;

		// Extract the four points into Curve.
		RRealPoint Curve[4];
		RRealPoint OtherCurve[4];		// The "other side" of a curve split.

		// See if we need to truncate the left side.
		if (fAngle == fMin)
		{
			memcpy(Curve, Points+nPoint, sizeof(Curve));
		}
		else
		{
			ASSERT(bFirst);
			double fTime = (fAngle - fMin)/(fMax-fMin);
			SplitCurveAt(Points+nPoint, fTime, OtherCurve, Curve);
		}

		// See if we need to truncate the right side.
		if (fThisEnd != fMax)
		{
			// We need to split the right side, too.
			double fTime = (fThisEnd - fAngle)/(fMax - fAngle);
			SplitCurveAt(Curve, fTime, Curve, OtherCurve);
		}

		// If this is the first time, we need a MoveTo.
		// Otherwise, the end of the last curve should be the first point of
		// this one (and is not placed).
		if (bFirst)
		{
			// We need a MoveTo for the first point.
			ptFirst.m_x = Curve[0].m_x;
			ptFirst.m_y = Curve[0].m_y;

			m_PathOperatorsStream.InsertAtEnd( kMoveTo );
			m_OriginalPointsStream.InsertAtEnd( ptFirst );
			nPointCount++;
		
			// No longer first time.
			bFirst = FALSE;
		}
		m_OriginalPointsStream.InsertAtEnd( Curve[1] );
		m_OriginalPointsStream.InsertAtEnd( Curve[2] );
		m_OriginalPointsStream.InsertAtEnd( Curve[3] );
		nPointCount += 3;	// 3 points to make up a bezier curve.
		nBezierCurves++;

		// Keep the angle between 0 and 2*PI.
		fAngle = NormalizeAngle(fThisEnd);
	} while (fAngle != fEndAngle);

	// We must have placed something!
	ASSERT(!bFirst);

	// We have output our stream of bezier points, so put in the bezier command
	// with our count.
	m_PathOperatorsStream.InsertAtEnd( kBezier );
	m_PathOperatorsStream.InsertAtEnd( (EPathOperator)nBezierCurves );

	// See if we want any closure.
	int nRepCount = 1;
	switch (nClosure)
	{
		case -1:
		{
			// Nothing to do. Leave open.
			break;
		}
		case 1:
		{
			// Pie closure.
			m_PathOperatorsStream.InsertAtEnd( kLine );
			m_PathOperatorsStream.InsertAtEnd( (EPathOperator)nRepCount );
			m_OriginalPointsStream.InsertAtEnd( (RRealPoint)ptCenter );
			nPointCount++;
			// Fall through to...
		}
		case 0:
		{
			// Chord closure.
			m_PathOperatorsStream.InsertAtEnd( kLine );
			m_PathOperatorsStream.InsertAtEnd( (EPathOperator)nRepCount );
			m_OriginalPointsStream.InsertAtEnd( ptFirst );
			nPointCount++;
			m_PathOperatorsStream.InsertAtEnd( kClose );
			break;
		}
		default:
		{
			// Illegal closure value.
			TRACE( "Illegal Closure value for an elliptical arc.\n" );
			ASSERT(FALSE);
			break;
		}
	}

	m_PathOperatorsStream.InsertAtEnd( kEnd );
	m_PointCountsStream.InsertAtEnd( nPointCount );
}


// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::SplitCurveAt()
//
// Description:		Splits an eliptical bezier into an arc piece based on a
//							particular "time" (0.0 - 1.0).
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RCGMSingleGraphic::SplitCurveAt(const RRealPoint* pSourceBezier, double fTime, RRealPoint* pNewLeft, RRealPoint* pNewRight)
{
	// Extract the points first. This allows us to put it back into the
	// same point memory (if required by the caller).
	RRealPoint ptA = pSourceBezier[0];
	RRealPoint ptB = pSourceBezier[1];
	RRealPoint ptC = pSourceBezier[2];
	RRealPoint ptD = pSourceBezier[3];

	pNewLeft[0] = ptA;
	pNewRight[3] = ptD;

	pNewLeft[1].m_x = ptA.m_x + (ptB.m_x - ptA.m_x)*fTime;
	pNewLeft[1].m_y = ptA.m_y + (ptB.m_y - ptA.m_y)*fTime;

	pNewRight[2].m_x = ptC.m_x + (ptD.m_x - ptC.m_x)*fTime;
	pNewRight[2].m_y = ptC.m_y + (ptD.m_y - ptC.m_y)*fTime;

	RRealPoint ptMBC;

	ptMBC.m_x = ptB.m_x + (ptC.m_x - ptB.m_x)*fTime;
	ptMBC.m_y = ptB.m_y + (ptC.m_y - ptB.m_y)*fTime;

	pNewLeft[2].m_x = pNewLeft[1].m_x + (ptMBC.m_x - pNewLeft[1].m_x)*fTime;
	pNewLeft[2].m_y = pNewLeft[1].m_y + (ptMBC.m_y - pNewLeft[1].m_y)*fTime;

	pNewRight[1].m_x = ptMBC.m_x + (pNewRight[2].m_x - ptMBC.m_x)*fTime;
	pNewRight[1].m_y = ptMBC.m_y + (pNewRight[2].m_y - ptMBC.m_y)*fTime;

	pNewLeft[3].m_x = pNewRight[0].m_x = pNewLeft[2].m_x + (pNewRight[1].m_x - pNewLeft[2].m_x)*fTime;
	pNewLeft[3].m_y = pNewRight[0].m_y = pNewLeft[2].m_y + (pNewRight[1].m_y - pNewLeft[2].m_y)*fTime;
}



/*
// Normalize an angle. Make it in the range [0, 2*PI).
*/

// ****************************************************************************
//
// Function Name:		RCGMSingleGraphic::NormalizeAngle()
//
// Description:		Normalize an angle. Make it in the range (0, 2*PI).
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
double RCGMSingleGraphic::NormalizeAngle(double fAngle)
{
	while (fAngle < 0)
	{
		fAngle += k2PI;
	}
	while (fAngle >= k2PI)
	{
		fAngle -= k2PI;
	}
	return fAngle;
}



/*
// Bump the metafile extent to include the points passed.
// It's assumed that x0 <= x1 and y0 <= y1.
*/

void RCGMSingleGraphic::BumpMetafileExtent(const RIntPoint &ptUpperLeft, const RIntPoint &ptLowerRight, SHORT nLineWidth )
{
#if 0
	if (debug_flags & 1024)
	{
		printf("bump_extent to include (%d, %d)(%d, %d)\n", ptUpperLeft.m_x, ptUpperLeft.m_y, ptLowerRight.m_x, ptLowerRight.m_y);
	}
#endif

/* Handle the metafile bounds. */

	long lOutsideEdgeWidth = (nLineWidth + 1)/2;
	RIntPoint ptAdjustedUpperLeft;
	RIntPoint ptAdjustedLowerRight;

	ptAdjustedUpperLeft.m_x = (long)ptUpperLeft.m_x - lOutsideEdgeWidth;
	ptAdjustedUpperLeft.m_y = (long)ptUpperLeft.m_y - lOutsideEdgeWidth;
	ptAdjustedLowerRight.m_x = (long)ptLowerRight.m_x + lOutsideEdgeWidth;
	ptAdjustedLowerRight.m_y = (long)ptLowerRight.m_y + lOutsideEdgeWidth;

	if (ptAdjustedUpperLeft.m_x < -32768
			|| ptAdjustedUpperLeft.m_y < -32768
			|| ptAdjustedLowerRight.m_x > 32767
			|| ptAdjustedLowerRight.m_y > 32767)
	{
		m_bTooBig = TRUE;
		TRACE( "CGM graphic too large, may render in unexpected fashion.\n" );
	}
	else
	{
		if (m_ptMetafileMin.m_x > (SHORT)ptAdjustedUpperLeft.m_x)
		{
			m_ptMetafileMin.m_x = (SHORT)ptAdjustedUpperLeft.m_x;
		}
		if (m_ptMetafileMin.m_y > (SHORT)ptAdjustedUpperLeft.m_y)
		{
			m_ptMetafileMin.m_y = (SHORT)ptAdjustedUpperLeft.m_y;
		}
		if (m_ptMetafileMax.m_x < (SHORT)ptAdjustedLowerRight.m_x)
		{
			m_ptMetafileMax.m_x = (SHORT)ptAdjustedLowerRight.m_x;
		}
		if (m_ptMetafileMax.m_y < (SHORT)ptAdjustedLowerRight.m_y)
		{
			m_ptMetafileMax.m_y = (SHORT)ptAdjustedLowerRight.m_y;
		}
	}

	// Now handle the extended bounds.
	SHORT nScaledWidth = nLineWidth * WIDTH_SCALE;

	ptAdjustedUpperLeft.m_x = (long)ptUpperLeft.m_x - nScaledWidth;
	ptAdjustedUpperLeft.m_y = (long)ptUpperLeft.m_y - nScaledWidth;
	ptAdjustedLowerRight.m_x = (long)ptLowerRight.m_x + nScaledWidth;
	ptAdjustedLowerRight.m_y = (long)ptLowerRight.m_y + nScaledWidth;

	if (ptAdjustedUpperLeft.m_x < -32768
			|| ptAdjustedUpperLeft.m_y < -32768
			|| ptAdjustedLowerRight.m_x > 32767
			|| ptAdjustedLowerRight.m_y > 32767)
	{
		m_bTooBig = TRUE;
		TRACE( "CGM graphic too large, may render in unexpected fashion.\n" );
	}
	else
	{
		if (m_sizeExtendedMin.m_dx > (SHORT)ptAdjustedUpperLeft.m_x)
		{
			m_sizeExtendedMin.m_dx = (SHORT)ptAdjustedUpperLeft.m_x;
		}
		if (m_sizeExtendedMin.m_dy > (SHORT)ptAdjustedUpperLeft.m_y)
		{
			m_sizeExtendedMin.m_dy = (SHORT)ptAdjustedUpperLeft.m_y;
		}
		if (m_sizeExtendedMax.m_dx < (SHORT)ptAdjustedLowerRight.m_x)
		{
			m_sizeExtendedMax.m_dx = (SHORT)ptAdjustedLowerRight.m_x;
		}
		if (m_sizeExtendedMax.m_dy < (SHORT)ptAdjustedLowerRight.m_y)
		{
			m_sizeExtendedMax.m_dy = (SHORT)ptAdjustedLowerRight.m_y;
		}
	}
}
