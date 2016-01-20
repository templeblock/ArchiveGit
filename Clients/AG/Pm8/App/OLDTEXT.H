/*
// $Header: /PM8/App/OLDTEXT.H 1     3/03/99 6:08p Gbeddow $
//	Old Definitions for graphic objects and data.
//
*/

#ifndef __OLDTEXT_H__
#define __OLDTEXT_H__

/*
// The second smallest unit of text (the TEXT_WORD).
// TEXT_WORDs index into the CHARACTER array of a PARAGRAPH.
*/

typedef struct
{
	UCHAR			type;						/* type of this word */
	UCHAR			flags;					/* flags for this word */
	C_INDEX		c_start;					/* First character in this word */
	PCOORD		x_offset;				/* offset of this word in text line */
	PCOORD		width;					/* width of this word */
	LONG			ascend;					/* ascend height of word */
	LONG			descend;					/* descend height of word */

	/* These are maximum possible bitmap boundaries, used to compute where
		to redraw. */
	LONG			draw_left;				/* x_offset + draw_left = first pixel to draw */
	LONG			draw_width;				/* x_offset + draw_width = last pixel to draw */
} PMD_TEXT_WORD, far *PMD_TEXT_WORD_PTR;

/*
// A line structure. An array of these is stored in a frame to tell it
// where the text begins and where it's located.
// LINEs index into the WORD array of a PARAGRAPH.
*/

typedef struct
{
	PCOORD	baseline;      /* baseline y offset from frame. */
	LONG		ascend;        /* ascent height for this line. */
	LONG		descend;       /* descent height for this line. */
	W_INDEX	w_start;			/* word index of first word in line */
	W_INDEX	w_end;			/* word index of last word in line */
	PCOORD	refresh_xmin;	/* offset from frame left bounds where we must start drawing */
	PCOORD	refresh_xmax;	/* offset from frame right bounds where we must end drawing */
} PMD_LINE, far *PMD_LINE_PTR;

/*
// A style structure.
// The STYLE structure is used to contain the style of text at a certain
// point in a text stream.
*/

typedef struct
{
/*
// current font ascend and descent height.
*/

	LONG font_ascend;
	LONG font_descend;

/*
// paragraph level: information.
//
// WARNING: these have to be handled in textflow explicitly.
//    position relative data.
*/

	ALIGN_TYPE line_alignment;
	VERT_ALIGN_TYPE vertical_alignment;
	SHORT left_margin;
	SHORT right_margin;

/*
// character level: current font information.
*/

	INODE face;							/* font inode */
	FONT_STYLE fstyle;				/* normal, bold, italic, bold/italic */

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

	BOOL x_flipped;
	BOOL y_flipped;

	COLOR	color;						/* the color of the letter */
	COLOR	bcolor;						/* the color of the letter */

	/* Font xmin and xmax */
	LONG	font_xmin;
	LONG	font_xmax;

} PMD_STYLE, far* PMD_STYLE_PTR;

/*
// A paragraph.
// This structure is used as an INODE object, and contains all the text
// and style information for a block of text.
*/

typedef struct
{
	PMD_STYLE style;		/* paragraph style for this paragraph. */
	ARRAY text;				/* array of text data. (CHARACTER) */
	ARRAY	word;				/* array of words. (TEXT_WORD) */
	ARRAY change;			/* array of style changes. (STYLE_CHANGE) */
} PARAGRAPH_RECORD;

#endif						/* #ifndef __OLDTEXT_H__ */
