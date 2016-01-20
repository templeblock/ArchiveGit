/*
// $Header: /PM8/App/WPG.H 1     3/03/99 6:14p Gbeddow $
//
// Word Perfect Graphic (WPG) file definitions.
//
// $Log: /PM8/App/WPG.H $
// 
// 1     3/03/99 6:14p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:27:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:45:54   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:08   FRED
// Initial revision.
// 
//    Rev 2.1   05 Jun 1995 11:31:10   JAY
// Got rid of DGROUP objects.
// 
//    Rev 2.0   07 Feb 1995 15:49:08   JAY
// Initial revision.
// 
//    Rev 1.1   14 Jun 1994 16:47:30   JAY
// WPG 2.0 handling is now commented out. It didn't really work anyway.
// 
//    Rev 1.0   16 Dec 1993 16:26:06   JAY
//  
*/

#ifndef __WPG_H__
#define __WPG_H__

#include "outline.h"

#define	WLOCAL	near

/*
// A fixed number.
// WPG uses a standard 16:16 fixed point number.
*/

typedef LONG	WPG_FIXED;

/*
// Word Perfect file header.
*/

typedef struct
{
	BYTE		WP_file_id[4];
	DWORD		data_start;
	BYTE		product_type;
	BYTE		file_type;
	BYTE		version_major;
	BYTE		version_minor;
	WORD		encryption_key;
	WORD		reserved;
} WP_HEADER;

/*
// Word Perfect product types.
*/

typedef enum
{
	WPPT_WordPerfect		= 1,
	WPPT_Shell				= 2,
	WPPT_Notebook			= 3,
	WPPT_Calculator		= 4,
	WPPT_FileManager		= 5,
	WPPT_Calendar			= 6,
	WPPT_ProgramEditor	= 7,
	WPPT_MacroEditor		= 8,
	WPPT_PlanPerfect		= 9,
	WPPT_DataPerfect		= 10,
	WPPT_Mail				= 11,
	WPPT_Printer			= 12,
	WPPT_Scheduler			= 13,
	WPPT_Office				= 14
} WP_PRODUCT_TYPE;

/*
// Word Perfect file types.
*/

typedef enum
{
	WPFT_MacroFile			= 1,
	WPFT_HelpFile			= 2,
	WPFT_KeyboardDefFile	= 3,

	WPFT_WPDocument		= 10,
	WPFT_Dictionary		= 11,
	WPFT_Thesaurus			= 12,
/* Etc. */
	WPFT_WPG					= 22
} WP_FILE_TYPE;

/*
// WPG record types for version 1.
*/

typedef enum
{
	WPG1RT_FillAttributes	= 1,
	WPG1RT_LineAttributes	= 2,
	WPG1RT_MarkerAttributes	= 3,
	WPG1RT_PolyMarker			= 4,
	WPG1RT_Line					= 5,
	WPG1RT_PolyLine			= 6,
	WPG1RT_Rectangle			= 7,
	WPG1RT_Polygon				= 8,
	WPG1RT_Ellipse				= 9,
	WPG1RT_Curve				= 10,
	WPG1RT_Bitmap				= 11,
	WPG1RT_GText				= 12,
	WPG1RT_GTextAttributes	= 13,
	WPG1RT_ColorMap			= 14,
	WPG1RT_StartWPGData		= 15,
	WPG1RT_EndWPGData			= 16,
	WPG1RT_PostscriptData	= 17,
	WPG1RT_COUNT
} WPG_1_RECORD_TYPE;

/*
// WPG record types for version 2.
*/

#ifdef WPG2
typedef enum
{
	WPG2RT_Start_WPG					= 1,
	WPG2RT_End_WPG						= 2,
	WPG2RT_Form_Settings				= 3,
	WPG2RT_Ruler_Settings			= 4,
	WPG2RT_Grid_Settings				= 5,
	WPG2RT_Layer						= 6,
	WPG2RT_Reserved7					= 7,
	WPG2RT_Pen_Style_Definition	= 8,
	WPG2RT_Pattern_Definition		= 9,
	WPG2RT_Comment						= 10,
	WPG2RT_Color_Transfer			= 11,
	WPG2RT_Color_Palette				= 12,
	WPG2RT_DP_Color_Palette			= 13,
	WPG2RT_Bitmap_Data				= 14,
	WPG2RT_Text_Data					= 15,
	WPG2RT_Chart_Style				= 16,
	WPG2RT_Chart_Data					= 17,
	WPG2RT_Object_Image				= 18,
	WPG2RT_Reserved19					= 19,
	WPG2RT_Reserved20					= 20,
	WPG2RT_Polyline					= 21,
	WPG2RT_Polyspline					= 22,
	WPG2RT_Polycurve					= 23,
	WPG2RT_Rectangle					= 24,
	WPG2RT_Arc							= 25,
	WPG2RT_Compound_Polygon			= 26,
	WPG2RT_Bitmap						= 27,
	WPG2RT_Text_Line					= 28,
	WPG2RT_Text_Block					= 29,
	WPG2RT_Text_Path					= 30,
	WPG2RT_Chart						= 31,
	WPG2RT_Group						= 32,
	WPG2RT_Object_Capsule			= 33,
	WPG2RT_Font_Settings				= 34,
	WPG2RT_Line_Cap_Definition		= 35,
	WPG2RT_Line_Join_Definition	= 36,
	WPG2RT_Pen_Fore_Color			= 37,
	WPG2RT_DP_Pen_Fore_Color		= 38,
	WPG2RT_Pen_Back_Color			= 39,
	WPG2RT_DP_Pen_Back_Color		= 40,
	WPG2RT_Pen_Style					= 41,
	WPG2RT_Pen_Pattern				= 42,
	WPG2RT_Pen_Size					= 43,
	WPG2RT_DP_Pen_Size				= 44,
	WPG2RT_Line_Cap					= 45,
	WPG2RT_Line_Join					= 46,
	WPG2RT_Brush_Gradient			= 47,
	WPG2RT_DP_Brush_Gradient		= 48,
	WPG2RT_Brush_Fore_Color			= 49,
	WPG2RT_DP_Brush_Fore_Color		= 50,
	WPG2RT_Brush_Back_Color			= 51,
	WPG2RT_DP_Brush_Back_Color		= 52,
	WPG2RT_Brush_Pattern				= 53,
	WPG2RT_COUNT
} WPG_2_RECORD_TYPE;
#pragma pack(1)
typedef struct
{
	WORD		x_resolution;
	WORD		y_resolution;
	BYTE		coordinate_precision;

/*
// Stuff gets variable from here.
*/

} WPG_2_START;
/*
// WPG 2 format characterization flags.
*/

typedef enum
{
	CFLAG_Taper			= 0x0001,		/* Object is to be tapered (perspective) */
	CFLAG_Translate	= 0x0002,		/* Object is to be translated */
	CFLAG_Skew			= 0x0004,		/* Object is to be skewed */
	CFLAG_Scale			= 0x0008,		/* Object is to be scaled */
	CFLAG_Rotate		= 0x0010,		/* Object is to be rotated */
	CFLAG_ObjectID		= 0x0020,		/* Object has a unique identifier */
//	CFLAG_Reserved		= 0x0040,		/* Reserved */
	CFLAG_Edit_Lock	= 0x0080,		/* Edit locks are define for object */
//	CFLAG_Reserved		= 0x0100,		/* Reserved */
	CFLAG_Group_State	= 0x0200,		/* Group created by application */
	CFLAG_Local_State	= 0x0400,		/* Attribute changes local to object */
	CFLAG_Direction	= 0x0800,		/* Draw direction is clockwise */
	CFLAG_Winding		= 0x1000,		/* Path is drawn with winding rule */
	CFLAG_Filled		= 0x2000,		/* Path is filled */
	CFLAG_Closed		= 0x4000,		/* Path is Closed */
	CFLAG_Framed		= (SHORT)0x8000 /* Path is Framed */
} WPG2_CFLAGS;

/*
// Macros to help parse characterization flags.
// These are applied to the flags themselves.
*/

#define	HAS_EXTRA_PARAMS(flags)		((flags) & 0x00FF)
#define	HAS_TRANSFORM(flags)			((flags) & 0x001F)

#define	HAS_EDIT_LOCK_FLAGS(flags)	((flags) & CFLAG_Edit_Lock)
#define	HAS_OBJECT_ID(flags)			((flags) & CFLAG_ObjectID)
#define	HAS_ANGLE(flags)				((flags) & CFLAG_Rotate)
#define	HAS_SCALE_XFORM(flags)		((flags) & (CFLAG_Scale | CFLAG_Rotate))
#define	HAS_SKEW_XFORM(flags)		((flags) & (CFLAG_Skew | CFLAG_Rotate))
#define	HAS_TRANS_XFORM(flags)		((flags) & CFLAG_Translate)
#define	HAS_TAPER_XFORM(flags)		((flags) & CFLAG_Taper)

#define	IS_FILLED(flags)				((flags) & CFLAG_Filled)
#define	IS_CLOSED(flags)				((flags) & CFLAG_Closed)
#define	IS_FRAMED(flags)				((flags) & CFLAG_Framed)

#pragma pack()

#endif

#define	WPU					1200

typedef struct
{
	BYTE		version;
	BYTE		flags;
	SHORT		width;
	SHORT		height;
} WPG_1_START;

/*
// The WPG update state.
*/

class WPGUpdateState : public OutlineUpdateState
{
private:

/* Are we painting or initializing? */

	BOOL		painting;

/* Header extractions. */

	SHORT		version;
	SHORT		width;
	SHORT		height;
	SHORT		x_resolution;
	SHORT		y_resolution;

	BOOL		fixed_coordinates;

/* The color palette. */

	RGBTriple	far *palette;

#ifdef WPG2
/* Version 2 data. */

	FLAGS characterization_flags;
	FLAGS edit_lock_flags;
	SHORT object_id;
	WPG_FIXED angle;
	WPG_FIXED scale_x;
	WPG_FIXED scale_y;
	WPG_FIXED skew_x;
	WPG_FIXED skew_y;
	USHORT trans_x_fraction;
	LONG trans_x_integer;
	USHORT trans_y_fraction;
	LONG trans_y_integer;
	WPG_FIXED taper_x;
	WPG_FIXED taper_y;
#endif

/* File I/O routines. */

	BOOL read_record_length(LONG *length);
	VOID read_wpg_point(OUTLINE_POINT *point);
	ERRORCODE read_wpg_header(WP_HEADER *);
	ERRORCODE read_wpg_info(VOID);
	OUTLINE_COLOR rgb_from_color_index(BYTE index);

	VOID			set_defaults(VOID);
	BOOL			read_next_point(OUTLINE_POINT *point);

	typedef BOOL (WLOCAL WPGUpdateState::* WPG_PARSER)(BYTE *, LONG);

	typedef struct
	{
		WPG_PARSER	parser;
		BOOL			primitive;
	} WPG_1_PROCESSOR;

	static WPG_1_PROCESSOR BASED_CODE version_1_processor[];

#ifdef WPG2
	typedef struct
	{
		WPG_PARSER	parser;
		BOOL			primitive;
	} WPG_2_PROCESSOR;

	static WPG_2_PROCESSOR version_2_processor[];
#endif

	BOOL WLOCAL WPG_null_processor(BYTE *record_type, LONG length);
	BOOL WLOCAL WPG_process_end(BYTE *record_type, LONG length);

	BOOL WLOCAL WPG_1_fill_attributes(BYTE *record_type, LONG length);
	BOOL WLOCAL WPG_1_line_attributes(BYTE *record_type, LONG length);
	BOOL WLOCAL WPG_1_marker_attributes(BYTE *record_type, LONG length);
	BOOL WLOCAL WPG_1_polymarker(BYTE *record_type, LONG length);
	BOOL WLOCAL WPG_1_line(BYTE *record_type, LONG length);
	BOOL WLOCAL WPG_1_polyline(BYTE *record_type, LONG length);
	BOOL WLOCAL WPG_1_rectangle(BYTE *record_type, LONG length);
	BOOL WLOCAL WPG_1_polygon(BYTE *record_type, LONG length);
	BOOL WLOCAL WPG_1_ellipse(BYTE *record_type, LONG length);
	BOOL WLOCAL WPG_1_colormap(BYTE *record_type, LONG length);
	BOOL WLOCAL WPG_process_1_start(BYTE *record_type, LONG length);

	BOOL WLOCAL parse_1_command(BOOL *primitive);

#ifdef WPG2
	OUTLINE_COLOR WLOCAL WPG_2_read_color(BOOL dp);

	BOOL WLOCAL WPG_2_polyline(BYTE *record_type, LONG length);
	BOOL WLOCAL WPG_2_polycurve(BYTE *record_type, LONG length);
	BOOL WLOCAL WPG_2_rectangle(BYTE *record_type, LONG length);
	BOOL WLOCAL WPG_2_arc(BYTE *record_type, LONG length);
	BOOL WLOCAL WPG_2_pen_fore_color(BYTE *record_type, LONG length);
	BOOL WLOCAL WPG_2_pen_size(BYTE *record_type, LONG length);
	BOOL WLOCAL WPG_2_brush_fore_color(BYTE *record_type, LONG length);
	BOOL WLOCAL WPG_process_2_start(BYTE *record_type, LONG length);

	BOOL WLOCAL parse_2_command(BOOL *primitive);
	BOOL WLOCAL handle_cflags(VOID);			/* Handle characterization flags */
#endif

public:

/* Virtual functions of OutlineUpdateState which we override. */

	WPGUpdateState(GraphicPtr graphic, BOOL painting = TRUE);
	~WPGUpdateState();
	ERRORCODE open_outline(VOID);
	BOOL parse_next_command(BOOL *primitive);

	ERRORCODE init(GraphicPtr graphic);
};

typedef WPGUpdateState far *WPGUpdateStatePtr;

FARCLASS WPGHelper : public GraphicHelper
{
private:
public:	

/*
// The pertinent data.
*/

	struct
	{
		SHORT				version;
		ULONG				data_start;
	} record;

/*
// The constructor.
*/

	WPGHelper(GraphicPtr graphic) : GraphicHelper(graphic) {}

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

typedef WPGHelper far *WPGHelperPtr;

#endif					// #ifndef __WPG_H__
