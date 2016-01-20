/*
// $Header: /PM8/App/CGM.H 1     3/03/99 6:04p Gbeddow $
//
// Computer Graphics Metafile (CGM) file definitions.
//
// $Log: /PM8/App/CGM.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:58   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:02   Fred
// Initial revision.
// 
//    Rev 1.2   15 Jul 1997 09:33:48   Jay
// Support for larger images.
// 
//    Rev 1.1   23 May 1997 16:13:50   Jay
// Support for nFX winding
// 
//    Rev 1.0   14 Mar 1996 13:44:26   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:00   FRED
// Initial revision.
// 
//    Rev 2.1   14 Sep 1995 09:32:16   JAY
// 32-bit changes
// 
//    Rev 2.0   07 Feb 1995 15:48:24   JAY
// Initial revision.
// 
//    Rev 1.0   16 Dec 1993 16:26:00   JAY
//  
*/

#ifndef __CGM_H__
#define __CGM_H__

#include "outline.h"

#define CLOCAL near

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

typedef OUTLINE_COLOR CGM_COLOR;
typedef OUTLINE_POINT CGM_POINT;

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

FARCLASS CGMUpdateState : public OutlineUpdateState
{

private:

/*
// How are we parsing the CGM file?
*/

	CGM_PARSE_MODE parse_mode;

/*
// The discovered name for a CGM file while sizing.
*/

	LPSTR	discovered_name;

/*
// Are we doing a proof sizing (needing actual size)?
*/

	BOOL	proof;
	BOOL m_fTooBig;
	CGM_POINT metafile_min;
	CGM_POINT metafile_max;
	CGM_POINT extended_min;
	CGM_POINT extended_max;
	CGM_POINT	size_origin;
//	CGM_POINT	size_extent;
	USHORT size_extent_x;
	USHORT size_extent_y;

/*
// CGM specific variables.
*/

	SHORT vdc_type;

	SHORT	color_selection_mode;
	SHORT line_width_mode;
	SHORT marker_size_mode;
	SHORT edge_width_mode;

/* Precisions. */

	SHORT vdc_integer_precision;
	SHORT integer_precision;
	SHORT index_precision;
	SHORT color_precision;
	SHORT color_index_precision;

/* Index color variables. */

	SHORT	max_color_index;
	CGM_COLOR far *palette;

/* Color extent variables. */

	BOOL			color_scale[3];
	CGM_COLOR	color_origin;
	CGM_COLOR	color_extent;

/* File I/O routines. */

	BOOL CLOCAL read_cgm_short(LPSHORT s, SHORT precision);
	BOOL read_integer(LPSHORT s)	{ return read_cgm_short(s, integer_precision); }
	BOOL read_vdc(LPSHORT s)	{ return read_cgm_short(s, vdc_integer_precision); }
	BOOL read_enum(LPSHORT s)	{ return read_cgm_short(s, 2); }
	BOOL read_index(LPSHORT s)	{ return read_cgm_short(s, index_precision); }
	VOID CLOCAL read_precision(LPSHORT s);
	VOID CLOCAL read_cgm_triple_color(CGM_COLOR far *color);
	VOID CLOCAL read_cgm_color(OUTLINE_COLOR *ocolor);
	VOID CLOCAL read_vdc_point(CGM_POINT *point);

	VOID CLOCAL cgm_to_outline_color(CGM_COLOR color, OUTLINE_COLOR *ocolor);

/* Sizing routines. */

	VOID CLOCAL bump_metafile_extent(SHORT x0, SHORT y0, SHORT x1, SHORT y1, SHORT width);
	VOID CLOCAL size_ellipse_control(CGM_POINT center, CGM_POINT cd1, CGM_POINT cd2, SHORT width);
	VOID CLOCAL size_rectangle(CGM_POINT center, SHORT xr, SHORT yr, SHORT width);

	VOID			set_defaults(VOID);
	BOOL			read_next_point(OUTLINE_POINT *point);

public:

	typedef BOOL (CLOCAL CGMUpdateState::* CGM_PARSER)(SHORT, SHORT);

/* Parser routines. */

	BOOL			CLOCAL parser_ignore(SHORT id, SHORT length);
	BOOL			CLOCAL DRAW_delimiter(SHORT id, SHORT length);
	BOOL			CLOCAL DRAW_metafile(SHORT id, SHORT length);
	BOOL			CLOCAL SIZE_picture(SHORT id, SHORT length);
	BOOL			CLOCAL DRAW_picture(SHORT id, SHORT length);
	BOOL			CLOCAL DRAW_control(SHORT id, SHORT length);
	BOOL			CLOCAL SIZE_primitive(SHORT id, SHORT length);
	BOOL			CLOCAL DRAW_primitive(SHORT id, SHORT length);
	BOOL			CLOCAL SIZE_attribute(SHORT id, SHORT length);
	BOOL			CLOCAL DRAW_attribute(SHORT id, SHORT length);

/* Virtual functions of OutlineUpdateState which we override. */

	CGMUpdateState(GraphicPtr data, CGM_PARSE_MODE mode = CGM_PARSE_DRAW, BOOL proof = TRUE);
	~CGMUpdateState();
	ERRORCODE open_outline(VOID);
	BOOL parse_next_command(BOOL *primitive);

	ERRORCODE init(GraphicPtr graphic);
};

typedef CGMUpdateState far *CGMUpdateStatePtr;

FARCLASS CGMHelper : public GraphicHelper
{
private:
public:	

/*
// The pertinent data.
*/

	struct
	{
		CGM_POINT		metafile_min;
		CGM_POINT		metafile_max;
		SHORT				number_of_colors;
		CGM_POINT		extended_min;
		CGM_POINT		extended_max;
	} record;

/*
// The constructor.
*/

	CGMHelper(GraphicPtr graphic) : GraphicHelper(graphic) {}

/*
// Overridden methods (for storage purposes).
*/

	ERRORCODE read(StorageDevicePtr device);
	ERRORCODE write(StorageDevicePtr device);
	ST_DEV_IO_SIZE size(StorageDevicePtr device);

/*
// The init function.
*/

	ERRORCODE init(GRAPHIC_CREATE_STRUCT_PTR gcs);
};

typedef CGMHelper far *CGMHelperPtr;

#endif					// #ifndef __CGM_H__
