/*
// $Header: /PM8/App/STYLE.H 1     3/03/99 6:12p Gbeddow $
//
// Definitions for PMG database record objects.
//
// $Log: /PM8/App/STYLE.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:06   Fred
// Initial revision.
// 
//    Rev 1.1   12 Apr 1996 13:08:50   Jay
// New text stuff
// 
//    Rev 1.0   14 Mar 1996 13:46:36   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:44   FRED
// Initial revision.
// 
//    Rev 2.2   14 Sep 1995 09:32:20   JAY
// 32-bit changes
// 
//    Rev 2.1   15 Mar 1995 10:40:44   JAY
// Support for new text editing
// 
//    Rev 2.0   07 Feb 1995 15:49:00   JAY
// Initial revision.
// 
//    Rev 1.1   16 Mar 1994 16:02:20   MACDUFF
// Divorced line spacing from font height
// 
//    Rev 1.0   16 Dec 1993 16:26:14   JAY
//  
*/

#ifndef __STYLE_H__
#define __STYLE_H__

#include <stddef.h>
#include "text.h"

#define FONT_EXPANSION_UNIT 1000

//
// Default line spacing types
//

const LONG LINE_SPACING_OldPmg = -1;
const LONG LINE_SPACING_Win3 = -2;

/*
// A text style object.
// The text style object is used to contain the style of text at a certain
// point in a text stream.
//
// A text style has a pointer to a PMG database. This is necessary to allow
// access to the FontServer for style computations. Also, if we ever implement
// paragraph tags again, we'll need access to the database.
//
// A text style is assumed to own its face. That is, if the text style has
// a face number which is not 0, that face number is assumed to have a lock
// for the style structure. If the style is temporary, its destroy() member
// should be called before it goes out of scope or is delete'd. If the style is
// written to file and intended to be read back, it's destroy() method should
// not be called because it still exists (in another form).
*/

typedef class TextStyle far *TextStylePtr;
typedef class TextStyle far &TextStyleRef;

FARCLASS TextStyle
{
private:

	PMGDatabasePtr database;			// for font metrics, etc.
	BOOL temporary;						// are we temporary?
	int m_nUnderlinePosition;
	int m_nUnderlineThickness;

	struct TextStyle_record
	{
	/*
	// character level: current font information.
	*/

		DB_RECORD_NUMBER face;			/* font inode */
		SHORT font_style;					/* normal, bold, italic, bold/italic */

		SHORT	size_fraction;				/* fractional part of size */
		SHORT size;							/* in points */
		SHORT	base_size_fraction;		/* fractional part of base_size */
		SHORT base_size;					/* in points */
		SHORT	expansion_fraction;		/* fractional part of expansion */
		SHORT expansion;					/* Horizontal expansion factor (%age) */

		SHORT pattern;						/* MLS pattern number */
		SHORT outline;						/* MLS outline number */
		SHORT background;					/* MLS background number */
		SHORT shadow;						/* MLS shadow number */

		SHORT x_flipped;					// BOOL
		SHORT y_flipped;					// BOOL

		COLOR	color;						/* the color of the letter */
		COLOR	bcolor;						/* the color of the letter */

	/********************************/
	/* Paragraph level information. */
	/********************************/

		SHORT line_alignment;			// ALIGN_TYPE
		SHORT vertical_alignment;		// VERT_ALIGN_TYPE
		SHORT left_margin;
		SHORT right_margin;

	/**************************************************************/
	/* The following variables are filled in by the font server's */
	/* font_metrics() call.                                       */
	/**************************************************************/

		LONG font_ascend;				/* Current font ascend */
		LONG font_descend;			/* Current font descend */
		LONG font_xmin;				/* Current font xmin */
		LONG font_xmax;				/* Current font xmax */

	/*** Additional fields added for PMW 1.00.07 and above: ***/

		// Character info
		LONG m_lOldPMGLineSpacing;		// Line space for this font under old PMG
		LONG m_lWin3LineSpacing;		// Line space for this font under Win 3.x

		// Paragraph info
		LONG m_lLineSpacing;				// negative => use a default

	/*** Additional fields added for PMW 2.00.00 and above: ***/

		SHORT m_nUnderline;				// Underline (UNDERLINE_STYLE)
		COLOR m_clrUnderline;			// Underline color (0 = black, etc.)

	} record;

	// Size of "record" in versions of PMW before the new line spacing
	unsigned int RecordSize_OldLineSpacing() const
		{ return offsetof(TextStyle_record, m_lOldPMGLineSpacing); }

public:

/******************/
/* Implementation */
/******************/

/*
// Standard constructor.
*/

	TextStyle(PMGDatabasePtr owner, BOOL temp = TRUE);

/*
// Copy constructor.
*/

	TextStyle(const TextStyle far &s);

/*
// Destructor.
*/

	~TextStyle();

/*
// Assignment operator.
// This is necessary to handle face locking.
//
// Assignment of a style with a different database will operate correctly.
// In such a case, if the style is temporary, the style will be owned by the
// new database. If the style is not temporary, all references are transfered
// to the style's database.
*/

	TextStyleRef operator = (const TextStyle far &s);

/*
// Equality operator.
*/

	BOOL operator == (const TextStyle far &s);

/*
// "Unequality" operator.
*/

	BOOL operator != (const TextStyle far &s) { return !(*this == s); }

/*
// Set the default style.
// This will cause a font lock on the default face.
*/

	VOID set_default(VOID);

/*
// Destroying a style.
// This means (among other possible things) free the font face record.
*/

	VOID destroy(VOID);

/*
// Update font metrics after some kind of change.
// This is analagous to the old fix_style() call.
*/

	VOID update_font_metrics(VOID);

/*
// Return the token for the passed value.
// TOKEN_Illegal means the incoming token value did not yield a good token.
*/

	TEXT_TOKEN valid_token(TEXT_TOKEN token);

/*
// Apply a style change.
*/

	ERRORCODE apply_change(STYLE_CHANGE_PTR scp, BOOL fix_style = TRUE);

/*
// Build a style change.
// 'this' style is the old style.
*/

	ERRORCODE build_change(STYLE_CHANGE_PTR scp, TextStylePtr new_style);

/*
// See if a style attribute is defined.
*/

	SHORT test_style(TEXT_TOKEN token);

/*
// See if another style matches us for this token.
*/

	SHORT compare_styles(TEXT_TOKEN token, TextStylePtr style);

/*
// Make a style attribute undefined.
*/

	ERRORCODE make_undefined(TEXT_TOKEN token);

/*
// Set the database this style is using.
*/

	VOID set_database(PMGDatabasePtr db)
		{ database = db; }

/*
// Get the database this style is using.
*/

	PMGDatabasePtr get_database(VOID)
		{ return database; }

/*
// Style face access.
*/

	DB_RECORD_NUMBER get_face(VOID) const
 		{ return record.face; }

	VOID set_face(DB_RECORD_NUMBER face);

/*
// Style font style access.
//
// We need to do some tricky stuff here to make separate bold and italic
// attributes. The trick is how to deal with the undefined state.
// The old scheme would set the font style field to -1 if bold *or* italic
// styles were different. Now we need to maintain the undefined-ness for each.
//
// We use the lower two bits as before for bold and italic. In addition, we
// now also use the two upper bits as "undefined" bits for bold and italic.
// We no longer use the whole value to store undefined for either (though we
// support this transparently for someone accessing the old "style" method).
//
// We also add two more methods to access the individual bold and italic
// attributes.
//
// In a sense, we use one variable to hold the combined state of two.
*/

	FONT_STYLE get_font_style(VOID) const;

	VOID set_font_style(FONT_STYLE font_style);

/* These two return FALSE, TRUE or -1 for undefined. */

	int GetBoldStyle(void) const;
	int GetItalicStyle(void) const;

	void SetBoldStyle(int nBold);
	void SetItalicStyle(int nItalic);

/*
// Underline support.
*/

	int UnderlinePosition(void) const
		{ return m_nUnderlinePosition; }
	void UnderlinePosition(int nUnderlinePosition)
		{ m_nUnderlinePosition = nUnderlinePosition; }
	int UnderlineThickness(void) const
		{ return m_nUnderlineThickness; }
	void UnderlineThickness(int nUnderlineThickness)
		{ m_nUnderlineThickness = nUnderlineThickness; }
/*
// Style point size access.
*/

	SHORT get_size(VOID) const
 		{ return record.size; }

	VOID set_size(SHORT size)
 		{ record.size = size; }

	SHORT get_size_fraction(VOID) const
 		{ return record.size_fraction; }

	VOID set_size_fraction(SHORT fraction)
 		{ record.size_fraction = fraction; }

/*
// Base point size access.
*/

	SHORT get_base_size(VOID) const
 		{ return record.base_size; }

	VOID set_base_size(SHORT size)
 		{ record.base_size = size; }

	SHORT get_base_size_fraction(VOID) const
 		{ return record.base_size_fraction; }

	VOID set_base_size_fraction(SHORT fraction)
 		{ record.base_size_fraction = fraction; }

/*
// Expansion access.
*/

	SHORT get_expansion(VOID) const
 		{ return record.expansion; }

	VOID set_expansion(SHORT expansion)
 		{ record.expansion = expansion; }

	SHORT get_expansion_fraction(VOID) const
 		{ return record.expansion_fraction; }

	VOID set_expansion_fraction(SHORT fraction)
 		{ record.expansion_fraction = fraction; }

/*
// Style pattern access.
*/

	SHORT get_pattern(VOID) const
 		{ return record.pattern; }

	VOID set_pattern(SHORT pattern)
 		{ record.pattern = pattern; }

/*
// Style outline access.
*/

	SHORT get_outline(VOID) const
 		{ return record.outline; }

	VOID set_outline(SHORT outline)
 		{ record.outline = outline; }

/*
// Style background access.
*/

	SHORT get_background(VOID) const
 		{ return record.background; }

	VOID set_background(SHORT background)
 		{ record.background = background; }

/*
// Style shadow access.
*/

	SHORT get_shadow(VOID) const
 		{ return record.shadow; }

	VOID set_shadow(SHORT shadow)
 		{ record.shadow = shadow; }

/*
// Style color access.
*/

	COLOR get_color(VOID) const
 		{ return record.color; }

	VOID set_color(COLOR color)
 		{ record.color = color; }

	COLOR get_bcolor(VOID) const
 		{ return record.bcolor; }

	VOID set_bcolor(COLOR bcolor)
 		{ record.bcolor = bcolor; }

/*
// Alignment access.
*/

	ALIGN_TYPE get_line_alignment(VOID) const
 		{ return (ALIGN_TYPE)record.line_alignment; }

	VOID set_line_alignment(ALIGN_TYPE alignment)
 		{ record.line_alignment = alignment; }

	VERT_ALIGN_TYPE get_vertical_alignment(VOID) const
 		{ return (VERT_ALIGN_TYPE)record.vertical_alignment; }

	VOID set_vertical_alignment(VERT_ALIGN_TYPE alignment)
 		{ record.vertical_alignment = alignment; }

/*
// Margin access.
*/

	SHORT get_left_margin(VOID) const
 		{ return record.left_margin; }

	VOID set_left_margin(SHORT margin)
 		{ record.left_margin = margin; }

	SHORT get_right_margin(VOID) const
 		{ return record.right_margin; }

	VOID set_right_margin(SHORT margin)
 		{ record.right_margin = margin; }

/*
// Font metric access.
*/

	PCOORD get_font_ascend(VOID) const
 		{ return record.font_ascend; }

	VOID set_font_ascend(PCOORD ascend)
 		{ record.font_ascend = ascend; }

	PCOORD get_font_descend(VOID) const
 		{ return record.font_descend; }

	VOID set_font_descend(PCOORD descend)
 		{ record.font_descend = descend; }

	PCOORD get_font_xmin(VOID) const
 		{ return record.font_xmin; }

	VOID set_font_xmin(PCOORD xmin)
 		{ record.font_xmin = xmin; }

	PCOORD get_font_xmax(VOID) const
 		{ return record.font_xmax; }

	VOID set_font_xmax(PCOORD xmax)
 		{ record.font_xmax = xmax; }

	// Line spacing: if the paragraph line spacing value is one of the two 
	// "magic" negative values, then use the appropriate default value for
	// the font.  Otherwise, use the (positive) paragraph value.

	PCOORD get_font_line_spacing() const
		{ return (record.m_lLineSpacing == LINE_SPACING_OldPmg) ?
			record.m_lOldPMGLineSpacing :
			(record.m_lLineSpacing == LINE_SPACING_Win3) ?
			record.m_lWin3LineSpacing :
			record.m_lLineSpacing; }

	void set_line_spacing(LONG lLineSpacing)
		{ record.m_lLineSpacing = lLineSpacing; }

	void set_OldPMGLineSpacing(LONG lOldPMGLineSpacing)
		{ record.m_lOldPMGLineSpacing = lOldPMGLineSpacing; }

	void set_Win3LineSpacing(LONG lWin3LineSpacing)
		{ record.m_lWin3LineSpacing = lWin3LineSpacing; }

/*
// Flipped access.
*/

	BOOL get_xflipped(VOID) const
 		{ return record.x_flipped; }

	VOID set_xflipped(BOOL x_flipped)
 		{ record.x_flipped = x_flipped; }

	BOOL get_yflipped(VOID) const
 		{ return record.y_flipped; }

	VOID set_yflipped(BOOL y_flipped)
 		{ record.y_flipped = y_flipped; }

/*
// Underline access.
*/

	UNDERLINE_STYLE UnderlineStyle(void) const
		{ return (UNDERLINE_STYLE)record.m_nUnderline; }

	void UnderlineStyle(UNDERLINE_STYLE nUnderline)
		{ record.m_nUnderline = nUnderline; }

	COLOR UnderlineColor(void) const
		{ return record.m_clrUnderline; }

	void UnderlineColor(COLOR clrUnderline)
		{ record.m_clrUnderline = clrUnderline; }

/*
// read()
//
// Read a text style from a StorageDevice.
*/

	ERRORCODE read(StorageDevicePtr device);

/*
// write()
//
// Write a text style to a StorageDevice.
*/

	ERRORCODE write(StorageDevicePtr device);

/*
// size()
//
// Size a text style for a StorageDevice.
*/

	ST_MAN_SIZE size(StorageDevicePtr device);
};

#endif				// #ifndef __STYLE_H__
