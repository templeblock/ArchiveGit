/*
// $Workfile: GERONIMO.H $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

#ifndef __GERONIMO_H__
#define __GERONIMO_H__

/****************************************************************************/
/*																								    */
/*			     		Geronimo Definitions for C Language Interface			    */
/*																								    */
/*			       Copyright (C) MicroLogic Software, Inc. 1988-1993			    */
/*									     All Rights Reserved							    */
/*																								    */
/*		These definitions are for the assembly language interface between	    */
/*		an application and Geronimo.  This definitions file is provided	    */
/*		to users along with the Geronimo font engine.							    */
/*																								    */
/*		To avoid name collisions, all names begin with "G_".					    */
/*																								    */
/*		This file is compatible with:														 */
/*																									 */
/*				Microsoft C 5.1, 6.0, 7.0													 */
/*				Turbo C 2.0																		 */
/*				Borland C++																		 */
/*																								    */
/****************************************************************************/

/*
// Revision History:
//
// $Log: /PM8/App/GERONIMO.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 2     4/30/98 4:47p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
//    Rev 1.0   14 Aug 1997 15:21:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:00   Fred
// Initial revision.
// 
//    Rev 1.5   15 May 1997 16:54:50   Fred
// New text effects
// 
//    Rev 1.4   03 Apr 1997 09:32:18   Fred
// More text cleanup
// 
//    Rev 1.3   26 Mar 1997 15:55:12   Fred
// Removed old geronimo stuff
// 
//    Rev 1.2   24 Jan 1997 15:00:30   Fred
// Removed some Geronimo conventions including changing G_FRACTION to a long
// 
//    Rev 1.1   06 Jan 1997 10:17:12   Fred
// Start of new text object
// 
//    Rev 1.0   14 Mar 1996 13:42:46   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:10   FRED
// Initial revision.
// 
//    Rev 2.1   15 Mar 1995 10:40:30   JAY
// Support for new text editing
// 
//    Rev 2.0   07 Feb 1995 15:48:38   JAY
// Initial revision.
// 
//    Rev 1.3   18 Mar 1994 13:34:30   MACDUFF
// Added old PMG line spacing to font description, so it will be
// cached.
// 
//    Rev 1.2   16 Mar 1994 16:02:18   MACDUFF
// Divorced line spacing from font height
// 
//    Rev 1.1   28 Jan 1994 15:02:30   FRED
// Brought into line with L:\GERONIMO.H.
// Context parameters is a different size.
// 
// 
//    Rev 1.0   16 Dec 1993 16:26:00   JAY
//  
// 
//    Rev 1.25   21 Apr 1993 16:15:24   FRED
// Fixed typo.
// 
//    Rev 1.24   23 Feb 1993 14:47:06   MACDUFF
// Added G_FONT_REQUEST type
// 
//    Rev 1.23   03 Feb 1993 09:56:20   MACDUFF
// Changes for C++ compatibility, with '#ifdef __cplusplus'.
// 
//    Rev 1.22   02 Feb 1993 11:47:14   MACDUFF
// Removed zero-byte array that offends Borland C
// 
//    Rev 1.21   02 Feb 1993 08:51:22   MACDUFF
// Added two fields that were supposed to be in G_CLIENT_BITMAP_PATTERN
// 
//    Rev 1.20   01 Feb 1993 15:29:56   MACDUFF
// Added G_R_not_enough_memory
// 
//    Rev 1.19   01 Feb 1993 14:35:20   MACDUFF
// New defines for geronimo_rasterize_path() (used by PMG).
// 
//    Rev 1.18   30 Dec 1992 15:12:10   FRED
// Fixed error in previous change. Changed TrueEffects context
// size from 25K to 50K.
// 
// 
//    Rev 1.17   30 Dec 1992 15:01:48   FRED
// Raised context size for TrueEffects from 22K to 50K.
// 
// 
//    Rev 1.16   26 Dec 1992 16:15:34   FRED
// G_FACE_INFORMATION.unique_font_name is now 60+1 bytes (was 40+1).
// 
// 
//    Rev 1.15   23 Dec 1992 09:42:44   FRED
// Added has_kerning_information field to
// Face Information structure.
// 
// 
//    Rev 1.14   18 Dec 1992 18:19:00   FRED
// Fixed FACE INFORMATION structure to match GERONIMO.TPI
// 
// 
//    Rev 1.13   18 Dec 1992 18:06:16   FRED
// Added the net FACEINFORMATION fields that are returned
// by TT Geronimo for gathering information about the font.
// 
// 
//    Rev 1.12   20 Nov 1992 18:53:56   FRED
// Change mlssym_ to symtt_
// 
// 
//    Rev 1.11   12 Nov 1992 15:08:02   ADAM
// defined mlssym_directory_address.
// 
//    Rev 1.10   18 Oct 1992 15:28:48   ADAM
// G_font_path is now the selected method.
// 
//    Rev 1.9   16 Oct 1992 09:53:06   ADAM
// TRUEFX build substitutes g_hDC instead of G_typeface in the
// FontDescription record.
// 
//    Rev 1.8   15 Oct 1992 15:41:08   FRED
// Added #ifdef TRUEFX's and TRUEFX specific definitions.
// 
// 
//    Rev 1.7   06 Jul 1992 12:27:40   FRED
// Removed _SIZE from G_MINIMUM_CONTEXT_SIZE_FOR_METRICS and
// G_MINIMUM_CONTEXT_SIZE_FOR_BITMAPS. Now MSC 6.0 won't give
// warning messages.
// 
// 
//    Rev 1.6   02 Jul 1992 12:00:16   FRED
// Added breathing support.
// 
// 
//    Rev 1.5   19 Jun 1992 13:54:08   FRED
// Changed G_METRIC_count to G_METRIC_maximum.
// 
// 
//    Rev 1.4   18 Jun 1992 18:02:26   FRED
// Added definitions for the geronimo_metric_list() function.
// 
// 
//    Rev 1.3   28 Apr 1992 13:02:14   ADAM
// 20000 too small, now 32768 (?)
// 
//    Rev 1.2   28 Apr 1992 12:15:24   ADAM
// G_MINIMUM_CONTEXT_SIZE_FOR_METRICS
// 
//    Rev 1.1   18 Dec 1991 17:08:22   FRED
// Merged in changes for horizontal clipping feature and
// extended attribute escapement (EAE) value reporting.
// 
// 
//    Rev 1.0.2.2   18 Dec 1991 15:14:30   FRED
// Added font and character extended attribute escapement (EAE) fields
// to the G_FONT_DESCRIPTION and G_BITMAP_PARAMETER structures which are
// filled in by Geronimo's metric and bitmap functions.
// 
// 
//    Rev 1.0.2.1   18 Dec 1991 09:42:14   FRED
// Added G_R_invalid_clip_data result code and renumbered the result
// codes that followed it.
// 
// Renamed G_BP_FLAG_horizontal_clip to G_BP_FLAG_clip_horizontal.
// 
// 
//    Rev 1.0.2.0   18 Dec 1991 09:38:36   FRED
// Added "buffer_clip_left" and "buffer_clip_right" fields to
// G_BITMAP_PARAMETERS structure. These fields are the dot
// coordinates of the leftmost and rightmost pixels to include
// in the generated bitmap. Internally, the sparse bitmap data
// for the entire row is generated, but only the run data which
// lies within the defined clipping region is actually filled.
// 
// Added the flag G_BP_FLAG_clip_horizontal which enables
// the horizontal clipping specified by the "buffer_clip_left"
// and "buffer_clip_right" fields of the G_BITMAP_PARAMETERS
// structure. If this flag is not set, the clip fields are
// ignored and the entire row is filled.
// 
// 
//    Rev 1.0   10 Sep 1991 14:30:26   FRED
// Initial revision.
*/

#include "text.h"

 
/*
// These definitions define the possible result codes returned by Geronimo.
//
// They are arranged so that result codes indicating errors are negative,
// and result codes indicating no errors are positive.
*/

typedef	enum
{
	G_R_partial									=	  2,
	G_R_complete								=	  1,
	G_R_ok										=	  0,
	G_R_error									=	 -1,
	G_R_internal_error						=	 -2,
	G_R_file_io									=	 -3,
	G_R_string_too_long						=	 -4,
	G_R_invalid_command						=	 -5,
	G_R_invalid_raster_command				=	 -6,
	G_R_no_character							=	 -7,
	G_R_invalid_character					=	 -8,
	G_R_no_typeface							=	 -9,
	G_R_invalid_typeface						=	-10,
	G_R_invalid_path_data					=	-11,
	G_R_invalid_hint_data					=	-12,
	G_R_invalid_effect_data					=	-13,
	G_R_invalid_transformation_data		=	-14,
	G_R_invalid_clip_data					=	-15,
	G_R_buffer_too_small						=	-16,
	G_R_context_too_small					=	-17,
	G_R_path_too_complex						=	-18,
	G_R_path_generation_error				=	-19,
	G_R_no_symbol_otl							=	-20,
	G_R_no_symset_dat							=	-21,
	G_R_breathing								=	-22,
	G_R_breathe_abort							=	-23,
	G_R_not_enough_memory					=	-24
} G_RESULT;


/*
// The possible commands to geronimo_make_bitmap().
//
// *** Note: Geronimo assumes G_C_size is 0.  Do not change its value. ***
*/

typedef	enum
{
	G_C_size			=	0,
	G_C_start		=	1,
	G_C_continue	=	2,
	G_C_done			=	3
} G_COMMAND;


/*
// Fixed point number.  The entire number is 32 bits.  The most significant
// 16 bits hold the integer.  The least significant 16 bits hold the fraction.
*/

typedef long G_FIXED;

/*
// Macros for accessing integer and fraction fields of a G_FIXED variable.
*/

#define	G_FIXED_integer(f)	(*(((short*)(&f))+1))
#define	G_FIXED_fraction(f)	(*(((short*)(&f))+0))

/*
// Structure of a bounding box with 16 bit integer values.  The units
// depend on the object being described.
*/

typedef	struct
{
	short ulx;				/* left   */
	short uly;				/* top    */
	short lrx;				/* right  */
	short lry;				/* bottom */
} G_BBOX_16;


/*
// Structure of a bounding box with 32 bit values.  The units
// depend on the object being described.
*/

typedef	struct
{
	long ulx;				/* left   */
	long uly;				/* top    */
	long lrx;				/* right  */
	long lry;				/* bottom */
} G_BBOX_32;


/*
// The defined typeface style values.  The numbering is the same as HP.
*/

typedef enum
{
	G_STYLE_upright		=	0,
	G_STYLE_italic			=	1
} G_STYLE;


/*
// The defined typeface stroke weight values.  The numbering is the same as HP.
*/

typedef enum
{
	G_WEIGHT_ultra_thin	=	-7,
	G_WEIGHT_extra_thin	=	-6,
	G_WEIGHT_thin			=	-5,
	G_WEIGHT_extra_light	=	-4,
	G_WEIGHT_light			=	-3,
	G_WEIGHT_demi_light	=	-2,
	G_WEIGHT_semi_light	=	-1,
	G_WEIGHT_medium		=	 0,
	G_WEIGHT_book			=	 0,
	G_WEIGHT_semi_bold	=	 1,
	G_WEIGHT_demi_bold	=	 2,
	G_WEIGHT_bold			=	 3,
	G_WEIGHT_extra_bold	=	 4,
	G_WEIGHT_black			=	 5,
	G_WEIGHT_extra_black	=	 6,
	G_WEIGHT_ultra_black	=	 7
} G_WEIGHT;


/*
// The defined typeface spacing type values.  The numbering is the same as HP.
*/

typedef enum
{
	G_SPACING_fixed			=	0,
	G_SPACING_proportional	=	1
} G_SPACING;


/*
// The typeface class values.
*/

typedef enum
{
	G_CLASS_serif			=	0,
	G_CLASS_square_serif	=	1,
	G_CLASS_sans_serif	=	2,
	G_CLASS_display		=	3,
	G_CLASS_script			=	4,
	G_CLASS_decorative	=	5,
	G_CLASS_symbol			=	6
} G_CLASS;


/*
// The structure used to describe a font.  A font is a combination of
// typeface and special effects.
*/

typedef	struct
{
	int				typeface;
//	unsigned short	pattern;
	FillFormatV2	Fill;
//	unsigned short	outline;
	OutlineFormat	Outline;
//	unsigned short	shadow;
	ShadowFormat	Shadow;
	G_FIXED			point_size;
	G_FIXED			vertical_expansion;
	G_FIXED			horizontal_expansion;
	G_FIXED			oblique;
	G_FIXED			effects_rotation;
	unsigned short	x_resolution;
	unsigned short	y_resolution;

	/* These are filled in by the geronimo_font_metrics() function. */
	/* They are all in units of 32 bit fractional dots except for   */
	/* the EAE values which are in integer device dots.             */

	/* NOTE: The order of these fields is assumed by Geronimo, do  */
	/*       not change them.                                      */

	long				font_underline_position;
	long				font_underline_thickness;
	long				font_space_width;
	G_BBOX_32		font_bbox;

	/* These are filled in by the geronimo_character_metrics() function.   */
	/* The BBOX and Delta X values are in units of 32 bit fractional dots. */
	/* The EAE values are in integer device dots.                          */

	/* NOTE: The order of these fields is assumed by Geronimo, do  */
	/*       not change them.                                      */

	G_BBOX_32		character_bbox;
	long				character_delta_x;

} G_FONT_DESCRIPTION;


/*
// The structure used to describe a request to geronimo_make_bitmap().
*/

typedef	struct
{
	/* These fields are set by the caller. */

	G_COMMAND						command;
	G_FONT_DESCRIPTION*			font_description;
	unsigned short					character;
	unsigned long					component;
	void*								outline;
	unsigned short					buffer_pad_left;
	unsigned short					buffer_pad_right;
	short								buffer_clip_left;
	short								buffer_clip_right;
	unsigned short					flags;
	short								start_row;
	unsigned short					row_count;
	void*								output_buffer_address;
	unsigned short					output_buffer_size;

	/* These fields are filled in by Geronimo.              */
	/* The BBOX fields are in units of device dots. */

	unsigned short					byte_width;
	unsigned short					bitmap_size;
	G_BBOX_16						bitmap_bbox;
	G_BBOX_16						character_bbox;
} G_BITMAP_PARAMETERS;

#endif					// #ifndef __GERONIMO_H__
