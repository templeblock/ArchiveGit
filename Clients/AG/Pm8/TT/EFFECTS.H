/*
// $Workfile: EFFECTS.H $
// $Revision: 1 $
// $Date: 3/03/99 6:35p $
*/

/*
// Revision History:
//
// $Log: /PM8/TT/EFFECTS.H $
// 
// 1     3/03/99 6:35p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:14:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:45:14   Fred
// Initial revision.
// 
//    Rev 1.4   01 Jul 1997 09:08:22   Fred
// More changes for PMG 4.0
// 
//    Rev 1.3   02 May 1997 10:15:04   Fred
//  
// 
//    Rev 1.2   21 Apr 1997 08:48:00   Fred
// Fixes for rotated pens and shadows.
// 
//    Rev 1.1   18 Apr 1997 12:49:54   Fred
// Changes for PMG 4.0
// 
//    Rev 1.0   14 Mar 1996 13:21:06   Jay
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:14:44   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:41:36   FRED
// Initial revision.
// 
//    Rev 1.1   04 Oct 1995 10:59:22   FRED
// 32 bit TrueType engine
// 
//    Rev 1.0   29 Sep 1995 13:37:04   FRED
// Initial revision.
// 
//    Rev 1.49   09 Nov 1992 16:04:00   MACDUFF
// Added fx_contours_added
// 
//    Rev 1.48   05 Nov 1992 13:00:20   MACDUFF
// The fields of OUTLINE are now signed, and have comments indicating the
// significance of the sign.
// 
//    Rev 1.47   13 Oct 1992 09:19:10   MACDUFF
// Prototype for fx_pattern_processor depends on MLS_ASM_SC_wnNrowFill.
// 
//    Rev 1.46   04 Sep 1992 11:22:40   FRED
// Added fx_em_cell_height and fx_em_cell_width fields to the Fx
// structure.
// 
// 
//    Rev 1.45   29 Aug 1992 14:52:22   FRED
// Removed MLS_OUTLINE_contour_inline_pen. MLS_OUTLINE_light
// is used instead.
// 
// 
//    Rev 1.44   27 Aug 1992 17:40:32   FRED
// Added fx_is_fixed_pitch field to Fx structure.
// 
// 
//    Rev 1.43   25 Aug 1992 10:43:04   FRED
// Changed comment for fx_background_box. This is now the
// actual background box for the character. The Y coordinates
// are set by NewTrans(). The X coordinates are set by NewGlyph().
// 
// 
//    Rev 1.42   24 Aug 1992 12:18:32   FRED
// Added fx_static_pen field to Fx structure. Set_pen() places a
// copy of a static pen in this field so it will be accessible
// from the data segment.
// 
// fx_pen_descriptor is now a near pointer.
// 
// 
// 
//    Rev 1.41   24 Aug 1992 10:29:22   FRED
// Added definitions used to build "wing" endcaps.
// 
// 
//    Rev 1.40   23 Aug 1992 16:05:16   FRED
// Added fx_is_endcap field to Fx structure.
// 
// 
//    Rev 1.39   20 Aug 1992 11:23:02   FRED
// Actually added fx_integer_point_size to Fx structure definition.
// 
// 
//    Rev 1.38   20 Aug 1992 11:17:28   FRED
// Added fx_integer_point_size field to Fx structure.
// 
// 
//    Rev 1.37   15 Aug 1992 23:53:40   FRED
// Added fx_background_box field to Fx structure.
// 
// 
//    Rev 1.36   15 Aug 1992 14:21:36   FRED
// Added fx_em_resolution to Fx structure.
// 
// 
//    Rev 1.35   15 Aug 1992 13:57:16   FRED
// Added fx_eae_width and fx_em_eae_width fields to Fx structure.
// 
// 
//    Rev 1.34   14 Aug 1992 18:43:26   FRED
// Added fx_em_eae (BBOX) field to Fx structure.
// 
// 
//    Rev 1.33   14 Aug 1992 17:15:58   FRED
// Added definitions for minimum fixed pitch
// background padding.
// 
// 
//    Rev 1.32   14 Aug 1992 15:00:42   FRED
// Added fx_is_fixed_pitch field to Fx structure.
// 
// 
//    Rev 1.31   14 Aug 1992 12:58:22   FRED
// Undefined BACKGROUND_PAD_MAXIMUM to turn off maximum check.
// 
// Added fx_em_space_width field to Fx structure.
// 
// 
//    Rev 1.30   14 Aug 1992 10:57:30   FRED
// Added fx_character_code field to Fx structure.
// 
// 
//    Rev 1.29   14 Aug 1992 10:16:20   FRED
// Added MLS_PATTERN_background_plaid which references the
// special background version of the plaid pattern.
// 
// 
//    Rev 1.28   13 Aug 1992 22:30:22   FRED
// Added MLS_PATTERN definitions.
// 
// 
//    Rev 1.27   13 Aug 1992 17:16:10   FRED
// Added bricks and zigzag variables to Fx structure.
// 
// 
//    Rev 1.26   13 Aug 1992 10:46:06   FRED
// Added fx_background_y_offset to Fx structure.
// 
// 
//    Rev 1.25   12 Aug 1992 18:07:40   FRED
// Added fx_client_flags to Fx structure.
// 
// Defined FX_FLAG_no_pattern_alignment flag.
// 
// 
//    Rev 1.24   12 Aug 1992 17:36:30   FRED
// Added BACKGROUND_PAD definitions that define the
// extra padding placed around proportional characters
// with background effects.
// 
// 
//    Rev 1.23   12 Aug 1992 11:34:26   FRED
// Added fx_eae to Fx structure.
// 
// Defined FX_FLAG_clip_edges.
// 
// 
//    Rev 1.22   11 Aug 1992 12:32:12   FRED
// Defined MLS shadow numbers.
// 
// 
//    Rev 1.21   10 Aug 1992 17:04:54   FRED
// Added fx_shadow_pen to Fx structure.
// 
// 
//    Rev 1.20   10 Aug 1992 16:54:30   FRED
// Added pen definition numbers for contour and shadow pens that are
// built on the fly.
// 
// 
//    Rev 1.19   08 Aug 1992 16:10:58   FRED
// fx_pattern is now the MLS pattern number used by sc_ScanChar(). The
// client specified fill pattern is in fx_fill_pattern. fs_ContourScan()
// sets fx_pattern as needed.
// 
// 
//    Rev 1.18   08 Aug 1992 15:06:56   FRED
// Changed fx_cell_y0 to fx_cell_ascent.
// 
// 
//    Rev 1.17   08 Aug 1992 14:04:52   FRED
// Added definitions for MLS outlines (light, medimum, ...)
// 
// 
//    Rev 1.16   08 Aug 1992 13:56:42   FRED
// Added fx_contour_inline_pen, fx_contour_gap_pen, and
// fx_contour_outline_pen to Fx structure.
// 
// 
//    Rev 1.15   07 Aug 1992 18:48:40   FRED
// Added fx_x_pixels_per_em and fx_y_pixels_per_em fields to Fx
// structure.
// 
// 
//    Rev 1.14   07 Aug 1992 18:34:20   FRED
// Added special effects variables that were moved out of the key
// structure to the Fx structure.
// 
// 
//    Rev 1.13   07 Aug 1992 18:26:14   FRED
// Moved special effects variables from key into
// Fx structure.
// 
// 
//    Rev 1.12   07 Aug 1992 17:11:32   FRED
// Added definitions for different types of outlines
// (region, rectangle, diagonal).
// 
// 
//    Rev 1.11   07 Aug 1992 14:38:08   FRED
// Fixed spelling error.
// 
// 
//    Rev 1.10   07 Aug 1992 14:11:24   FRED
// Added fx_stroke_pen and fx_pen_descriptor fields to the Fx structure.
// 
// 
//    Rev 1.9   07 Aug 1992 11:07:14   FRED
// Changed some fields to signed.
// 
// 
//    Rev 1.8   07 Aug 1992 10:31:54   FRED
// Added fx_bitmap, fx_low_row, and fx_high_row fields
// to Fx structure.
// 
// Added get_pattern_data() macro.
// 
// 
//    Rev 1.7   06 Aug 1992 17:12:42   FRED
// Reworked Fx fields for one pattern descriptor.
// 
// 
//    Rev 1.6   06 Aug 1992 13:30:58   FRED
// New definitions for pattern processors that take
// 32 bit mask.
// 
// 
//    Rev 1.5   06 Aug 1992 11:12:26   FRED
// Fixed syntax error - missing ;'s
// 
//    Rev 1.4   06 Aug 1992 11:09:10   FRED
// Added the following fields to the Fx structure:
//  
//    fx_fill
//  
//       The MLS fill pattern number to use.
//  
//    fx_stroke
//  
//       The MLS stroke pattern number to use.
//  
//    fx_stroke_processor
//  
//       The near address of the function to perform the
//       stroke pattern fill.
//  
//    fx_stroke_pattern
//  
//       The stroke pattern descriptor.
//  
//    fx_stroke_pattern_data
//  
//       The offset of the bitmap data for a bitmap pattern.
//  
//    fx_flags
//  
//       Flags which effect the glyph imaging process.
//
// 
//    Rev 1.3   29 Jul 1992 16:08:26   FRED
// Fixed syntax error.
// 
// 
//    Rev 1.2   29 Jul 1992 16:03:56   FRED
// Added fx_fill_pattern_data.
// 
// 
//    Rev 1.1   29 Jul 1992 13:45:56   FRED
// Changed definition of fx_fill_processor to a function.
// 
// 
//    Rev 1.0   29 Jul 1992 13:21:08   FRED
// Initial revision.
*/

#ifndef __EFFECTS_H__
#define __EFFECTS_H__

#include "fscaler.h"

#pragma warning(disable : 4200)

/*
// Standard MicroLogic underline distance (at 1000/em).
*/

#define	MLS_UNDERLINE_DISTANCE		(-148)


/*
// Structure of a pattern descriptor. A pattern may be defined by a near
// function in which case its function field will be nonzero. Otherwise,
// the pattern is defined by the data and dimension fields in the structure.
// Normally, a pointer to one of these structures is passed to the routines
// which will be using them. Some special patterns have been defined which
// are identified by passing certain values as pointers:
//
//		NULL_PATTERN
//		NO_PATTERN
//
//			These values indicate no pattern has been specified.
//			Their value is 00000h.
//
//		BLACK_PATTERN
//
//			This value indicates an all black pattern which is
//			identified by a pointer equal to 0ffffh.
//
//		WHITE_PATTERN
//
//			This value indicates an all white pattern which is
//			identified by a pointer equal to 0fffeh.
*/

// Pattern processor prototype

typedef uint32 (*FxPatternProcessor)(uint32 m, int32 x, int32 y);

// Pattern function prototype

typedef uint32 (*FxPatternFunction)(void);

// Pattern function initialization prototype

typedef void (*FxPatternInitialize)(void);

// Pattern desciptor

struct PatternDescriptor
{
	FxPatternFunction pFunction;
	FxPatternInitialize pInitialize;
	int fRotate;
	int fExpand;
	int32 nXSize;
	int32 nYSize;
	int32 nXOffset;
	int32 nYOffset;
};

struct Pattern
{
	PatternDescriptor Descriptor;
	unsigned char bData[];
};

#define	NULL_PATTERN							(0)
#define	NO_PATTERN								(0)

#define	BLACK_PATTERN							(-1)
#define	WHITE_PATTERN							(-2)

#if 0
/*
// Define special pattern numbers that are recognized by set_pattern().
*/

#define	MLS_PATTERN_no_pattern				(0xffff)

/*
// Define the MLS pattern numbers.
*/

#define	MLS_PATTERN_black						 (0)
#define	MLS_PATTERN_white						 (1)
#define	MLS_PATTERN_gray_2_percent			 (2)
#define	MLS_PATTERN_gray_5_percent		  	 (3)
#define	MLS_PATTERN_gray_15_percent		 (4)
#define	MLS_PATTERN_gray_30_percent		 (5)
#define	MLS_PATTERN_gray_50_percent		 (6)
#define	MLS_PATTERN_gray_70_percent		 (7)
#define	MLS_PATTERN_gray_90_percent		 (8)
#define	MLS_PATTERN_horizontal				 (9)
#define	MLS_PATTERN_vertical					(10)
#define	MLS_PATTERN_graph_paper				(11)
#define	MLS_PATTERN_wood_grain				(12)
#define	MLS_PATTERN_starry_night			(13)
#define	MLS_PATTERN_sunset					(14)
#define	MLS_PATTERN_fountain					(15)
#define	MLS_PATTERN_rattan					(16)
#define	MLS_PATTERN_sunrise					(17)
#define	MLS_PATTERN_starburst				(18)
#define	MLS_PATTERN_radial					(19)
#define	MLS_PATTERN_swirl						(20)
#define	MLS_PATTERN_stripes					(21)
#define	MLS_PATTERN_zigzag					(22)
#define	MLS_PATTERN_plaid						(23)
#define	MLS_PATTERN_brick						(24)
#define	MLS_PATTERN_london_fog				(25)
#define	MLS_PATTERN_cloud						(26)
#define	MLS_PATTERN_tubular					(27)
#endif


/*
// Structure of an outline descriptor. An outline is defined by the data
// and dimension fields in the structure. Normally, a pointer to one of these
// structures is passed to the routines which will be using them. Some
// special outlines have been defined which are identified by passing certain
// values as pointers:
//
//		NULL_OUTLINE
//		NO_OUTLINE
//
//			These values indicate no outline has been specified.
//			Their value is 00000h.
//
// The data following the header defines the shape of the outline not as
// a bitmap but as a list of runs. This makes it faster to stroke the
// outline onto a bitmap. The structure of the data is as follows:
//
//		<outline data for row #0>
//		<outline data for row #1>
//		<outline data for row #2>
//			.
//			.
//			.
//		<outline data for row (OUTLINE_y_size-1)>
//
// There is a set of row data for each row in the outline whether it is empty
// or not. The OUTLINE_y_size field determines how many rows of data are
// in the outline. Each set of row data has the following structure:
//
//		<number of runs in row data>
//
//		<start of run #0>
//		<end of run #0>
//
//		<start of run #1>
//		<end of run #1>
//
//			.
//			.
//			.
//
// If the row is empty, the number of runs will be zero. Each of the run
// start and end coordinates is 32 bits wide. The number of bytes occupied
// by a each row in the outline is (8*number_of_runs)+4.
//
*/

struct OutlineDescriptor
{
	uint32 nType;
	int32 nXSize;				/* Positive = to the RIGHT */
	int32 nYSize;				/* Positive = DOWN */
	int32 nXOffset;			/* Positive = to the LEFT */
	int32 nYOffset;			/* Positive = UP */
};

struct Outline
{
	OutlineDescriptor Descriptor;
	unsigned char bData[];
};

/*
// The following definitions are for the various outline types:
//
//	OUTLINE_region
//
//		The outline shape is determined by a series of runs:
//
//			<# of runs in row 0> <run start/end> ...
//			<# of runs in row 1> <run start/end> ...
//				 ...
//
//	OUTLINE_rectangle
//
//		The outline shape is a rectangle with the dimensions
//		specified by the X size and Y size.
//
//	OUTLINE_diagonal
//
//		The outline shape is a diagonal line between two corners
//		of a rectangle with the dimensions specified by the X size
//		and Y size. If the X size is positive, then the line runs
//		from the upper left corner to the lower right. If the X
//		size is negative, then the line runs from the lower left
//		corner to the upper right.
*/

#define	OUTLINE_region			(0)
#define	OUTLINE_rectangle		(1)
#define	OUTLINE_diagonal		(2)

// Values used for sizing contour pens.

#define	CONTOUR_SCALE_ORIGIN			(25)
#define	CONTOUR_SCALE_SLOPE			(125)
#define	CONTOUR_GAP_NUMERATOR		(3)
#define	CONTOUR_GAP_DENOMINATOR		(4)

/*
// Define the MLS outline numbers.
*/

#define	NULL_OUTLINE				(0)
#define	NO_OUTLINE					(0)

#if 0
#define	MLS_OUTLINE_none			(0)
#define	MLS_OUTLINE_light			(1)
#define	MLS_OUTLINE_medium		(2)
#define	MLS_OUTLINE_heavy			(3)
#define	MLS_OUTLINE_calligraphy	(4)
#define	MLS_OUTLINE_contour_1	(5)
#define	MLS_OUTLINE_contour_2	(6)
#endif

#define	IS_OUTLINE(o)				((o) != OutlineFormat::None)
#define	IS_CONTOUR_OUTLINE(o)	(((o) == OutlineFormat::Contour1) || ((o) == OutlineFormat::Contour2))

/*
// Define special outline numbers that are recognized by set_pen() to
// use the various pens that are built on the fly in the context.
*/

#define	MLS_OUTLINE_no_pen					(-1)
#define	MLS_OUTLINE_contour_gap_pen		(-2)
#define	MLS_OUTLINE_contour_outline_pen	(-3)
#define	MLS_OUTLINE_shadow_pen				(-4)

/*
// Define the MLS shadow numbers.
*/

#if 0
#define	MLS_SHADOW_none						(0)
#define	MLS_SHADOW_drop_down_and_right	(1)
#define	MLS_SHADOW_drop_down_and_left		(2)
#define	MLS_SHADOW_drop_up_and_right		(3)
#define	MLS_SHADOW_drop_up_and_left		(4)
#define	MLS_SHADOW_3D_down_and_right		(5)
#define	MLS_SHADOW_3D_down_and_left		(6)
#define	MLS_SHADOW_3D_up_and_right			(7)
#define	MLS_SHADOW_3D_up_and_left			(8)
#endif

#define	IS_SHADOW(s)		((s) != ShadowFormat::None)
#define	IS_DROP_SHADOW(s)	((s) == ShadowFormat::Hairline)
#define	IS_3D_SHADOW(s)	((s) == ShadowFormat::Extruded)

/*
// This structure contains the variables used by the scan converter to
// build special effects. It is allocated in the context.
//
//		fx_character_code
//
//			The character code for the glyph being generated.
//
//		fx_is_fixed_pitch
//
//			Flag that is nonzero if the font is fixed pitch.
//
//		fx_is_symbol_font
//
//			Flag that is nonzero if the font is a symbol font.
//
//		fx_random_seed
//
//			The current seed for the random number generator.
//
//		fx_integer_point_size
//
//			The integer point size of the font.
//
//		fx_em_resolution
//
//			The Em resolution of the font.
//
//		fx_x_pixels_per_em
//
//			The number of pixels per EM along the X axis.
//
//		fx_y_pixels_per_em
//
//			The number of pixels per EM along the Y axis.
//
//		fx_em_space_width
//
//			The unscaled space width at the current Em resolution.
//
//		fx_cell_height
//
//			The font bbox height scaled to the point size.
//
//		fx_cell_width
//
//			The font bbox width scaled to the point size.
//
//		fx_cell_ascent
//
//			The font bbox ascent height scaled to the point size.
//			Note that this value is normally positive.
//
//		fx_em_cell_height
//
//			The font bbox height.
//
//		fx_em_cell_width
//
//			The font bbox width.
//
//		fx_char_x0
//
//			The minimum X of the scaled character bbox.
//
//		fx_char_x1
//
//			The maximum X of the scaled character bbox.
//
//		fx_char_y0
//
//			The minimum Y of the scaled character bbox.
//
//		fx_char_y1
//
//			The maximum Y of the scaled character bbox.
//
//		fx_fill_x
//
//			The X coordinate to fill.
//
//		fx_fill_y
//
//			The Y coordinate to fill.
//
//		fx_x_resolution
//
//			The X resolution of the output device.
//
//		fx_y_resolution
//
//			The Y resolution of the output device.
//
//		fx_orientation
//
//			The font orientation:
//
//				0 => portrait
//				1 => landscape
//
//		fx_fill_pattern
//
//			The MLS fill pattern number to use.
//
//		fx_stroke_pattern
//	
//			The MLS stroke pattern number to use.
//	
//		fx_pattern
//
//			The MLS fill pattern number used by sc_ScanChar().
//
//		fx_pattern_processor
//
//			The address of the function to retrieve the pattern data.
//
//		fx_pattern_descriptor
//
//			The pattern descriptor.
//
//		fx_pattern_data
//
//			The address of the bitmap data for a bitmap pattern.
//
//		fx_outline
//
//			The MLS outline number to use.
//
//		fx_static_pen
//
//			A data segment copy of a static pen.
//
//		fx_contour_gap_pen
//
//			The pen descriptor for the contour gap pen.
//
//		fx_contour_outline_pen
//
//			The pen descriptor for the contour outline pen.
//
//		fx_shadow_pen
//
//			The pen descriptor for the shadow pen.
//
//		fx_pen_descriptor
//
//			The near address of the pen descriptor.
//
//		fx_shadow
//	
//			The MLS shadow number to use.
//	
//		fx_bitmap
//
//			A near pointer to the sc_BitMapData structure that
//			describes the bitmap being used to hold the glyph.
//
//		fx_low_row
//
//			The number of the lowest row to fill.
//
//		fx_high_row
//
//			The number of the highest row to fill + 1.
//
//		fx_eae
//
//			A bounding box containing the extended attribute escapements
//			for the font (all values are positive).
//
//		fx_eae_width
//
//			The contribution that extended attribute escapements make
//			to a horizontal width metric such as an advance width or
//			space width. This value does not include any contributions
//			from Contour 1 and Contour 2 pens.
//
//		fx_flags
//	
//			Flags which effect the glyph imaging process.
//
//				FX_FLAG_initialize_buffer
//
//					Initialize the bitmap contents to zero.
//
//		fx_x_offset
//		fx_y_offset
//
//			Computed pen offsets.
//
//
//		fx_effects_rotation
//
//			The rotation of the outline. The effects will be rotated
//			by this amount.
//
//		fx_dRotation
//		fx_dCosRotation
//		fx_dSinRotation
//
//			Precomputed floating point values for rotation.
*/

typedef struct
{
	uint16 fx_character_code;
	uint16 fx_is_symbol_font;
	uint32 fx_random_seed;
	uint32 fx_integer_point_size;
	uint32 fx_em_resolution;
	uint32 fx_x_pixels_per_em;
	uint32 fx_y_pixels_per_em;
	uint32 fx_em_space_width;
	uint32 fx_cell_height;
	uint32 fx_cell_width;
	int32 fx_cell_ascent;
	uint32 fx_em_cell_height;
	uint32 fx_em_cell_width;
	int32 fx_char_x0;
	int32 fx_char_x1;
	int32 fx_char_y0;
	int32 fx_char_y1;
	int32 fx_fill_x;
	int32 fx_fill_y;
	uint32 fx_x_resolution;
	uint32 fx_y_resolution;
	FillFormat fx_Fill;
	uint16 fx_stroke_pattern;
	uint16 fx_pattern;
	FxPatternProcessor fx_pattern_processor;
	PatternDescriptor fx_pattern_descriptor;
	unsigned char* fx_pattern_data;
	OutlineFormat fx_Outline;
	OutlineDescriptor fx_contour_gap_pen;
	OutlineDescriptor fx_contour_outline_pen;
	OutlineDescriptor fx_shadow_pen;
	OutlineDescriptor fx_diagonal_pen;
	OutlineDescriptor* fx_pen_descriptor;
	ShadowFormat fx_Shadow;
	sc_BitMapData *fx_bitmap;
	int32 fx_low_row;
	int32 fx_high_row;
	BBOX fx_eae;
	int32 fx_eae_width;
	uint16 fx_flags;
	int32 fx_x_offset;
	int32 fx_y_offset;
	int32 fx_effects_rotation;
	double fx_dRotation;
	double fx_dCosRotation;
	double fx_dSinRotation;

	/* Brick variables. */

	int32	brick_x1;
	int32	brick_x2;
	int32	brick_x3;
	int32	brick_y1;
	int32	brick_y2;
	int32	brick_y3;

	/* Zigzag variables. */

	int32	zigzag_thickness;
	int32	zigzag_slope_numerator;
	int32	zigzag_slope_denominator;
	int32	zigzag_x;
	int32	zigzag_y;

} FX;

#define	FX_FLAG_initialize_buffer		(0x0001)
#define	FX_FLAG_fill_stroke				(0x0004)
#define	FX_FLAG_stroke_fill				(0x0008)
#define	FX_FLAG_clip_edges				(0x0010)

#define	get_pattern_data(m, x, y)	(*(Fx.fx_pattern_processor))((m),(x),(y))

// Pattern Coordinates Generator
//
// This object generates the logical coordinates for accessing (possibly)
// rotated pattern data.

class CPatternCoordinates
{
public:
	CPatternCoordinates(int32 nX, int32 nY);

public:
	void GetXY(int32& nX, int32& nY);
	int IsRotatedOrExpanded(void) const
		{ return m_fRotated || m_fExpanded; }

public:
	int m_fRotated;
	int m_fExpanded;
	double m_dX;
	double m_dY;
	double m_dXIncrement;
	double m_dYIncrement;
};

#pragma warning(default : 4200)

#endif
