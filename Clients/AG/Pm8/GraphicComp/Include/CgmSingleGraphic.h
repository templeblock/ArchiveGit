// ****************************************************************************
//
// $Header: /PM8/GraphicComp/Include/CgmSingleGraphic.h 1     3/03/99 6:18p Gbeddow $
//
//  Project:			Graphic Component.
//
//  Author:				Richard Grenfell
//
//  Description:		Declaration of the RCgmSingleGraphic class.
//							The CGM graphics class is a vector graphic stored in CGM
//							raw data format.
//
//  Portability:		Platform independent
//
//  Copyright (C) 1999 The Learning Company. All Rights Reserved.
//
//  $Log: /PM8/GraphicComp/Include/CgmSingleGraphic.h $
// 
// 1     3/03/99 6:18p Gbeddow
// 
// 1     2/26/99 9:12p Rgrenfel
// Support for CGM graphic data.
//
// ****************************************************************************

#ifndef CGMSINGLEGRAPHIC_H_
#define CGMSINGLEGRAPHIC_H_

#ifndef _SINGLEGRAPHIC_H_
#include "SingleGraphic.h"
#endif

#ifndef _PSD3GRAPHIC_H_
#include "Psd3Graphic.h"
#endif

const YChunkTag kCGMSingleGraphicChunkID = 0x0216000a;

typedef enum
{
	CGM_DELIMITER,					/* Delimiter elements */
	CGM_METAFILE,					/* Metafile Descriptor elements */
	CGM_PICTURE,					/* Picture Descriptor elements */
	CGM_CONTROL,					/* Control elements */
	CGM_PRIMITIVE,					/* Graphical Primitive elements */
	CGM_ATTRIBUTE,					/* Attribute elements */
	CGM_ESCAPE,						/* Escape element */
	CGM_EXTERNAL,					/* External element */
	CGM_RESERVED					/* reserved for future standardization */
} CGM_CLASS;

/*
// Delimiter elements.
*/

typedef enum
{
	CGM_NOOP,
	CGM_BEGIN_METAFILE,
	CGM_END_METAFILE,
	CGM_BEGIN_PICTURE,
	CGM_BEGIN_PICTURE_BODY,
	CGM_END_PICTURE,
	CGM_DELIMITER_ELEMENTS
} CGM_DELIMITER_ELEMENT;

/*
// Metafile Descriptor elements.
*/

typedef enum
{
	CGM_METAFILE_VERSION = 1,
	CGM_METAFILE_DESCRIPTION,
	CGM_VDC_TYPE,
	CGM_INTEGER_PRECISION,
	CGM_REAL_PRECISION,
	CGM_INDEX_PRECISION,
	CGM_COLOR_PRECISION,
	CGM_COLOR_INDEX,
	CGM_MAXIMUM_COLOR_INDEX,
	CGM_COLOR_VALUE_EXTENT,
	CGM_METAFILE_ELEMENT_LIST,
	CGM_METAFILE_DEFAULTS,
	CGM_FONT_LIST,
	CGM_CHAR_SET_LIST,
	CGM_CHAR_CODING,
	CGM_METAFILE_ELEMENTS
} CGM_METAFILE_ELEMENT;

/*
// Picture Descriptor elements.
*/

typedef enum
{
	CGM_SCALING_MODE = 1,
	CGM_COLOR_SELECTION_MODE,
	CGM_LINE_WIDTH_MODE,
	CGM_MARKER_SIZE_MODE,
	CGM_EDGE_WIDTH_MODE,
	CGM_VDC_EXTENT,
	CGM_BACKGROUND_COLOR,
	CGM_PICTURE_ELEMENTS
} CGM_PICTURE_ELEMENT;

/*
// Control elements.
*/

typedef enum
{
	CGM_VDC_INTEGER_PRECISION = 1,
	CGM_VDC_REAL_PRECISION,
	CGM_AUXILIARY_COLOR,
	CGM_TRANSPARENCY,
	CGM_CLIP_RECTANGLE,
	CGM_CLIP_INDICATOR,
	CGM_CONTROL_ELEMENTS
} CGM_CONTROL_ELEMENT;

/*
// Graphical Primitive elements.
*/

typedef enum
{
	CGM_POLYLINE = 1,
	CGM_DISJOINT_POLYLINE,
	CGM_POLYMARKER,
	CGM_TEXT,
	CGM_RESTRICTED_TEXT,
	CGM_APPEND_TEXT,
	CGM_POLYGON,
	CGM_POLYGON_SET,
	CGM_CELL_ARRAY,
	CGM_GENERIC_PRIMITIVE,
	CGM_RECTANGLE,
	CGM_CIRCLE,
	CGM_CIRC_ARC_3PT,
	CGM_CIRC_ARC_3PT_CLOSE,
	CGM_CIRC_ARC_CENTER,
	CGM_CIRC_ARC_CENTER_CLOSE,
	CGM_ELLIPSE,
	CGM_ELLI_ARC,
	CGM_ELLI_ARC_CLOSE,
	CGM_PRIMITIVE_ELEMENTS
} CGM_PRIMITIVE_ELEMENT;

/*
// Attribute elements.
*/

typedef enum
{
	CGM_LINE_BUNDLE = 1,
	CGM_LINE_TYPE,
	CGM_LINE_WIDTH,
	CGM_LINE_COLOR,
	CGM_MARKER_BUNDLE,
	CGM_MARKER_TYPE,
	CGM_MARKER_SIZE,
	CGM_MARKER_COLOR,
	CGM_TEXT_BUNDLE,
	CGM_TEXT_FONT,
	CGM_TEXT_PRECISION,
	CGM_CHAR_EXP_FACTOR,
	CGM_CHAR_SPACING,
	CGM_TEXT_COLOR,
	CGM_CHAR_HEIGHT,
	CGM_CHAR_ORIENTATION,
	CGM_TEXT_PATH,
	CGM_TEXT_ALIGNMENT,
	CGM_CHAR_SET,
	CGM_ALTERNATE_CHAR_SET,
	CGM_FILL_BUNDLE,
	CGM_INTERIOR_STYLE,
	CGM_FILL_COLOR,
	CGM_HATCH_INDEX,
	CGM_PATTERN_INDEX,
	CGM_EDGE_BUNDLE,
	CGM_EDGE_TYPE,
	CGM_EDGE_WIDTH,
	CGM_EDGE_COLOR,
	CGM_EDGE_VISIBILITY,
	CGM_FILL_REFERENCE_POINT,
	CGM_PATTERN_TABLE,
	CGM_PATTERN_SIZE,
	CGM_COLOR_TABLE,
	CGM_ASPECT_SOURCE_FLAGS,
	CGM_ATTRIBUTE_ELEMENTS
} CGM_ATTRIBUTE_ELEMENT;

/*
// Escape elements.
*/

typedef enum
{
	CGM_ESCAPE_ESCAPE = 1,
	CGM_ESCAPE_ELEMENTS
} CGM_ESCAPE_ELEMENT;

/*
// External elements.
*/

typedef enum
{
	CGM_MESSAGE = 1,
	CGM_APPLICATION_DATA,
	CGM_EXTERNAL_ELEMENTS
} CGM_EXTERNAL_ELEMENT;

typedef enum
{
	CGM_PARSE_SIZE,
	CGM_PARSE_DRAW
} CGM_PARSE_MODE;

typedef enum
{
	COLOR_SELECTION_INDEXED,
	COLOR_SELECTION_DIRECT
} COLOR_SELECTION_MODE;

typedef enum
{
	CGM_LINE_TYPE_SOLID = 1,
	CGM_LINE_TYPE_DASHED
} ECGM_LINE_TYPE;

typedef enum
{
	CGM_INTERIOR_STYLE_HOLLOW = 0,
	CGM_INTERIOR_STYLE_FILLED,
	CGM_INTERIOR_STYLE_PATTERN,
	CGM_INTERIOR_STYLE_HATCH,
	CGM_INTERIOR_STYLE_EMPTY
} ECGM_INTERIOR_STYLE;

typedef enum
{
	COLOR_SCALE_RED = 0,
	COLOR_SCALE_GREEN,
	COLOR_SCALE_BLUE
} COLOR_SCALE;

typedef enum
{
	MARKER_DOT,
	MARKER_PLUS,
	MARKER_ASTERISK,
	MARKER_CIRCLE,
	MARKER_CROSS,
	MARKER_RESERVED
} MARKER_TYPE;

typedef struct
{
	BYTE	m_nRed;
	BYTE	m_nGreen;
	BYTE	m_nBlue;
} SCgmColor;

class RCGMSingleGraphic : public RSingleGraphic, public RPsd3Graphic
{
public:
	// Construction, Destruction & Initialization
	RCGMSingleGraphic();
	RCGMSingleGraphic(const RCGMSingleGraphic &rhs);
	virtual ~RCGMSingleGraphic();

	// Operations	
	virtual BOOLEAN					Initialize(const uBYTE* pGraphicData, BOOLEAN fAdjustLineWidth = FALSE, BOOLEAN fMonochrome = FALSE);
	virtual BOOLEAN					ValidateGraphic(const uBYTE* pGraphicData);
	virtual BOOLEAN					ContainsGradientData() {return FALSE;}
	virtual BOOLEAN					GetAdjustLineWidth() {return TRUE;}
	virtual YChunkTag					GetChunkStorageTag() {return kCGMSingleGraphicChunkID;}

private :
	// Member data
	RIntPoint							m_CurrentPosition;

	// CGM mode data
	SHORT									m_nFillMode;
//	CGM_PARSE_MODE						m_eParseMode;
	COLOR_SELECTION_MODE				m_eColorSelectionMode;
	SHORT									m_nLineWidthMode;
	SHORT									m_nMarkerSizeMode;
	SHORT									m_nEdgeWidthMode;

	SHORT									m_nVdcType;
	SHORT									m_nVdcIntegerPrecision;
	SHORT									m_nIntegerPrecision;
	SHORT									m_nIndexPrecision;
	SHORT									m_nColorPrecision;
	SHORT									m_nColorIndexPrescision;
	SHORT									m_nMaxColorIndex;

	BOOLEAN								m_bColorScale[3];
	SCgmColor							*m_pColorPalette;
	SCgmColor							m_ColorOrigin;
	SCgmColor							m_ColorExtent;

	RIntPoint							m_ptSourceOrigin;
	RIntPoint							m_ptSourceExtent;
	RIntSize								m_sizeOrigin;
	RIntSize								m_sizeExtent;

	RIntSize								m_sizeExtendedMin;
	RIntSize								m_sizeExtendedMax;

	RIntPoint							m_ptMetafileMin;
	RIntPoint							m_ptMetafileMax;

	SHORT									m_nCurrentPenWidth;

	BOOLEAN								m_bReverseXTransform;
	BOOLEAN								m_bReverseYTransform;

	SCgmColor							m_BackgroundColor;

	// Line attributes.

	SHORT									m_nLineType;
	SHORT									m_nLineWidth;
	SCgmColor							m_LineColor;

	// Edge attributes.

	BOOLEAN								m_bEdgeVisibility;
	SHORT									m_nEdgeType;
	SHORT									m_nEdgeWidth;
	SCgmColor							m_EdgeColor;

	// Fill attributes.
	SHORT									m_nInteriorStyle;
	SHORT									m_nHatchIndex;
	SCgmColor							m_FillColor;

	// Marker attributes.
	MARKER_TYPE							m_eMarkerType;
	SHORT									m_nMarkerSize;
	SCgmColor							m_MarkerColor;

	// This buffer moves as data is read, it is not a permanent buffer!
	const BYTE *						m_pInputBuffer;
	// Bytes Read from the buffer, should never exceed the full size.
	LONG									m_lBufferDataRead;

	BOOLEAN								m_bTooBig;

	// Member functions
	BOOLEAN								ConvertGraphicElements( const BYTE* pGraphicElements );

	void									SetDefaults();

	void									ScaleColor( const SCgmColor &Color, SCgmColor *pOutColor );

	double								AngleFromVector( SHORT nXLength, SHORT nYLength );

	void									BumpMetafileExtent( const RIntPoint &ptUpperLeft, const RIntPoint &ptLowerRight, SHORT nLineWidth );

	void									SplitCurveAt(const RRealPoint* pSourceBezier, double fTime, RRealPoint* pNewLeft, RRealPoint* pNewRight);

	// *** Buffer Read methods.
	// Reads a command from the data stream.
	BOOLEAN								ReadCommand( SHORT *pnClass, SHORT *pnID, SHORT *pnLength );
	// Reads a short data value from the stream with the given precision.
	BOOLEAN								ReadData( UCHAR *pData, LONG lBlockSize );
	BOOLEAN								ReadShortData( SHORT *pnShort, SHORT nPrecision = 2);

	BOOLEAN								ReadColorTriple( SCgmColor *pColor );
	BOOLEAN								ReadColorWithScale( SCgmColor *pColor );
	BOOLEAN								ReadPoint( RIntPoint *pPoint );
	BOOLEAN								ReadPrecisionData( SHORT *pnPrecision );

	void									EnsureDataOnWordBoundary();

	BOOLEAN								SeekRelative( LONG lDistanceToMove );
	BOOLEAN								SeekAbsolute( LONG lPosition );
	LONG									GetPosition() { return m_lBufferDataRead; }
	// *** End of buffer Read methods

	// ~~~ Command processing methods
	BOOLEAN								ProcessCommand( SHORT nClass, SHORT nID, SHORT nLength );

	BOOLEAN								DrawDelimiter(SHORT nID);
	BOOLEAN								DrawMetafile(SHORT nID);
	BOOLEAN								DrawPicture(SHORT nID);
	BOOLEAN								DrawControl(SHORT nID);
	BOOLEAN								DrawPrimitive(SHORT nID, SHORT nLength);
	BOOLEAN								DrawAttribute(SHORT nID, SHORT nLength);

	BOOLEAN								ProcessRectangle();
	BOOLEAN								ProcessPolyline( SHORT nNumPoints );
	BOOLEAN								ProcessPolygon( SHORT nNumPoints );
	BOOLEAN								ProcessEllipse();
	BOOLEAN								ProcessCircle();
	BOOLEAN								Process3PtCircularArc(int nID);
	BOOLEAN								ProcessCenteredCircularArc(SHORT nID);
	BOOLEAN								ProcessEllipticalArc(SHORT nID);
	BOOLEAN								ProcessPolymarker(SHORT nNumPoints);
	BOOLEAN								ProcessPointStream( SHORT nNumPoints, BOOLEAN bLineBased );

	void									ProcessEllipticalArc(const RIntPoint &ptCenter, double fRadiusX, double fRadiusY, double fStartAngle, double fEndAngle, SHORT nClosure);
	double								NormalizeAngle( double fAngle );

	YFillMethod							SetFillMethod();
	void									SetupFillMethod( YFillMethod& nFillMethod, const SCgmColor& FillColor );
	YPenStyle							SetLineStyle(SHORT nStyle, SHORT nWidth, const SCgmColor& LineColor);
	// ~~~ End of command processing methods.
};

#endif // CGMSINGLEGRAPHIC_H_
