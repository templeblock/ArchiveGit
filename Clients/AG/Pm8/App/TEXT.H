/*
// $Header: /PM8/App/TEXT.H 1     3/03/99 6:12p Gbeddow $
//
// $Log: /PM8/App/TEXT.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 18    2/15/99 9:53a Cschende
// no changes
// 
// 17    2/11/99 11:55a Cschende
// Added code for Drop Caps implementation
// 
// 16    1/21/99 11:58a Cschendel
// added ETextWrapGutter for variable text wrap gutter amounts
// 
// 15    1/14/99 9:25a Cschende
// added  PreviousParagraphStart() function
// 
// 14    1/05/99 5:47p Cschende
// changed the character define used for the square bullet
// 
// 13    12/24/98 12:54p Cschende
// added defines for bullet symbols
// 
// 12    12/15/98 5:44p Cschende
// added bullet color support
// 
// 11    12/10/98 4:11p Cschende
// Added bullet, numbered list and multilevel list support in text
// 
// 10    6/03/98 1:43p Jayn
// Added MakeTextUndefined()
// 
// 9     4/30/98 4:48p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
// 8     4/29/98 6:32p Rlovejoy
// Updated for new fill format.
// 
// 7     3/27/98 5:48p Johno
// Changes in CTextStyle::operator =  for reference counts
// 
// 6     3/25/98 6:37p Johno
// HyperLink and Hyperlink now all Hyperlink for sanity
// 
// 5     3/25/98 4:03p Johno
// Added GetHyperlinkData
// 
// 4     3/24/98 5:27p Johno
// Added TOKEN_Hyperlink
// 
// 3     3/18/98 11:15a Jayn
// Page numbers
 * 
 * 2     10/30/97 5:20p Dennis
 * Added NextParagraphStart() and GetTextRecord() methods.
// 
//    Rev 1.0   14 Aug 1997 15:26:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:12   Fred
// Initial revision.
// 
//    Rev 1.34   26 Jun 1997 16:14:58   Jay
// Fixed old text style changes
// 
//    Rev 1.33   11 Jun 1997 17:28:24   Fred
// Bitmap drawing optimizations
// 
//    Rev 1.32   29 May 1997 08:53:02   Fred
// Improved support for old 3.0 effects
// 
//    Rev 1.31   27 May 1997 11:09:44   Fred
// Added new effects to CFrameAttributes
// 
//    Rev 1.30   23 May 1997 17:09:44   Fred
// New effects
// 
//    Rev 1.29   15 May 1997 16:53:42   Fred
// New text effects
// 
//    Rev 1.28   09 Apr 1997 14:51:40   Fred
// Start of new font caching (fonts and metrics)
// 
//    Rev 1.27   03 Apr 1997 09:32:42   Fred
// More text cleanup
// 
//    Rev 1.26   26 Mar 1997 15:57:46   Fred
// Start of new effects
// 
//    Rev 1.25   07 Feb 1997 09:01:20   Fred
// Added GetCharacterInformation()
// 
//    Rev 1.24   06 Jan 1997 17:07:12   Jay
// Character spacing.
// 
//    Rev 1.23   06 Jan 1997 10:17:42   Fred
// Start of new text object
// 
//    Rev 1.22   11 Jul 1996 11:35:28   Jay
//  
// 
//    Rev 1.21   10 Jul 1996 10:25:48   Jay
// Text states no longer allocated.
// 
//    Rev 1.20   05 Jul 1996 10:47:12   Jay
//  
// 
//    Rev 1.19   28 Jun 1996 16:18:18   Jay
//  
// 
//    Rev 1.18   27 Jun 1996 17:20:46   Jay
//  
// 
//    Rev 1.17   27 Jun 1996 13:27:44   Jay
//  
// 
//    Rev 1.16   26 Jun 1996 09:06:54   Jay
//  
// 
//    Rev 1.15   25 Jun 1996 13:13:16   Jay
//  
// 
//    Rev 1.14   24 Jun 1996 18:06:32   Jay
//  
// 
//    Rev 1.13   21 Jun 1996 14:30:28   Jay
// Stretch text, etc.
// 
//    Rev 1.12   20 Jun 1996 15:39:04   Jay
// Added Insert() to Tab Array
// 
//    Rev 1.11   18 Jun 1996 16:14:50   Jay
//  
// 
//    Rev 1.10   17 Jun 1996 12:01:26   Jay
//  
// 
//    Rev 1.9   14 Jun 1996 17:05:40   Jay
//  
// 
//    Rev 1.8   14 Jun 1996 08:57:24   Jay
//  
// 
//    Rev 1.7   13 Jun 1996 17:46:10   Jay
//  
// 
//    Rev 1.6   13 Jun 1996 15:53:06   Jay
//  
// 
//    Rev 1.5   04 Jun 1996 17:38:58   Jay
//  
// 
//    Rev 1.4   24 May 1996 09:27:00   Jay
// New text code.
// 
//    Rev 1.3   25 Apr 1996 12:16:30   Jay
//  
// 
//    Rev 1.2   23 Apr 1996 08:07:50   Jay
// More new stuff
// 
//    Rev 1.1   12 Apr 1996 13:08:48   Jay
// New text stuff
// 
//    Rev 1.0   14 Mar 1996 13:43:22   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:10:42   FRED
// Initial revision.
// 
//    Rev 2.4   22 Sep 1995 09:40:48   JAY
// Fixed a 32/16 size difference.
// 
//    Rev 2.3   28 Apr 1995 17:26:38   JAY
// No longer creates separate macro words.
// 
//    Rev 2.2   29 Mar 1995 08:59:02   JAY
// Lots of changes
// 
//    Rev 2.1   15 Mar 1995 10:40:50   JAY
// Support for new text editing
// 
//    Rev 2.0   07 Feb 1995 15:49:02   JAY
// Initial revision.
// 
//    Rev 1.1   16 Mar 1994 16:01:38   MACDUFF
// Divorced line spacing from font height
// 
//    Rev 1.0   16 Dec 1993 16:25:58   JAY
//  
// 
//    Rev 1.29   26 Jul 1993 08:48:12   JAY
// Added apply_change_to_style().
// 
//    Rev 1.28   23 Apr 1993 23:00:00   MACDUFF
// Added a comment
// 
//    Rev 1.27   23 Apr 1993 21:22:38   MACDUFF
// Added fields to TEXT_WORD, LINE, and STYLE to support more exact
// drawing boundaries.  Also defined OLD versions of these structures
// for support of documents made with the old ones.
// 
//    Rev 1.26   13 Apr 1993 10:32:34   JAY
// Added prototype for compute_range_style().
// Added #define for MINIMUM_SHRINK_SIZE.
// 
//    Rev 1.25   12 Apr 1993 16:33:40   JAY
// Added special character codes for BREAK and TAB characters.
// 
//    Rev 1.24   08 Apr 1993 16:22:12   JAY
// Added prototype for add_frame().
// Added FRAME_FLAG_fit_text.
// 
//    Rev 1.23   02 Apr 1993 13:42:12   JAY
// Added "make_changes_permanent()".
// 
//    Rev 1.22   31 Mar 1993 16:00:06   JAY
// Added FRAME_FLAG_use_white_space for banners
// 
//    Rev 1.21   30 Mar 1993 11:33:56   JAY
// Support for banner (stretch_frame)
// 
//    Rev 1.20   29 Mar 1993 15:06:28   JAY
// Added Temporary and Overridden flags to the token word.
// Added 'temporary' parameter to change_range_style().
// Added remove_temporary_tokens().
// 
//    Rev 1.19   27 Mar 1993 13:29:28   JAY
// Removed WORD_FLAG_needs_flow. Added WORD_FLAG_changed_size.
// 
//    Rev 1.18   26 Mar 1993 12:50:36   JAY
// New forced textflow determined by stretch (using new word flags).
// 
//    Rev 1.17   25 Mar 1993 16:11:50   JAY
// Removed expansion TOKEN.
// 
//    Rev 1.16   23 Mar 1993 16:48:22   JAY
// New field for stretched text original frame dimensions.
// 
//    Rev 1.15   22 Mar 1993 10:38:38   MACDUFF
// Changed order of horizontal alignments to Left-Center-Right.
// 
//    Rev 1.14   18 Mar 1993 09:46:40   MACDUFF
// Support for new Font/Style selection
// 
//    Rev 1.13   16 Mar 1993 15:07:18   MACDUFF
// New args to adjust_words() and rebuild_words() for new text redisplay.
// 
//    Rev 1.12   12 Mar 1993 13:55:14   MACDUFF
// Removed display op from STYLE; changed arg to rebuild_lines()
// 
//    Rev 1.11   04 Mar 1993 09:54:00   JAY
// Removed text_cursor_on() and text_cursor_off() defs since they don't exist.
// 
//    Rev 1.10   03 Mar 1993 09:38:18   JAY
// Added base_size file to STYLE structure.
// 
//    Rev 1.9   25 Feb 1993 09:35:46   JAY
// Added size and expansion fractional components.
// 
//    Rev 1.8   24 Feb 1993 13:32:14   JAY
// Stretched text support.
// 
//    Rev 1.7   24 Feb 1993 11:38:28   JAY
// Added STYLE_FRAC.
// 
//    Rev 1.6   17 Feb 1993 10:11:36   JAY
// Changed 'pitch' to 'expansion'. Add vertical alignment support.
// 
//    Rev 1.5   17 Feb 1993 09:08:18   JAY
// Fixed header files. New refresh code.
// 
//    Rev 1.4   05 Feb 1993 16:21:36   JAY
// Style changed for selected text.
// 
//    Rev 1.3   05 Feb 1993 13:04:26   JAY
// Text selection code and support
*/

#ifndef __TEXT_H__
#define __TEXT_H__

#include <stddef.h>

#include "pagedefs.h"
#include "cfixed.h"
#include "array.h"			// Used for the tab array.
#include "effects.h"

class StorageDevice;
class PMGDatabase;
class CTextRecord;
class TextStyle;
class CFrameObject;
class CWarpField;
class CFontDescription;
class CCharacterDescription;
struct HyperlinkData;
/////////////////////////////////////////////////////////////////////////////
//                        COMMON TEXT DEFINITIONS                          //
//                                                                         //
// These definitions are used by both the old text system and the new one. //
/////////////////////////////////////////////////////////////////////////////

/*
// The smallest unit of text (the CHARACTER).
*/

typedef USHORT	CHARACTER, *LPCHARACTER;

typedef CHARACTER FAR *CHARACTER_PTR;		// Obsolete. Do not use.

/*
// Word types.
*/

typedef enum
{
	WORD_TYPE_unknown = -1,				/* Special uses. Don't store in a word. */
	WORD_TYPE_solid,						/* Solid (non-white) text */
	WORD_TYPE_space,						/* White space */
	WORD_TYPE_tab,							/* Tab */
	WORD_TYPE_break,						/* Hard CR */
	WORD_TYPE_eop,							/* End of paragraph. */
	WORD_TYPE_eot = WORD_TYPE_eop,	/* End of text (3.00 name for eop). */
	WORD_TYPE_macro,						/* Macro (OBSOLETE). */
	WORD_TYPE_framebreak,					/* Move to next frame. (New with 3.00) */
	WORD_TYPE_dropcap						/* Drop cap character (new with 8.0) */
} WORD_TYPE;

/*
// Word flags.
*/

#define	WORD_FLAG_needs_sizing		1
#define	WORD_FLAG_changed_size		2
#define	WORD_FLAG_override_keep		4
#define	WORD_FLAG_override_flow		8
#define	WORD_FLAG_needs_building	16		/* New with 3.00 */
#define	WORD_FLAG_broken				32		/* New with 3.00 */
#define	WORD_FLAG_has_macro			64		/* New with 3.00 */
#define	WORD_FLAG_has_page_macro	128	/* New with 5.00 */

#define	WORD_FLAG_override			(WORD_FLAG_override_keep | WORD_FLAG_override_flow)
#define	WORD_FLAG_macro_mask			(WORD_FLAG_has_macro | WORD_FLAG_has_page_macro)

/*
// Alignment types.
*/

enum ALIGN_TYPE
{
	ALIGN_left,
	ALIGN_center,
	ALIGN_right,
	ALIGN_justify_left,
	ALIGN_justify_center,
	ALIGN_justify_right
};

enum VERT_ALIGN_TYPE
{
	ALIGN_top,
	ALIGN_middle,
	ALIGN_bottom
};

enum LEADING_TYPE
{
	LEADING_undefined = -1,		// Transient value for multi-values. DO NOT USE.
	LEADING_lines,					// relative - leading value specifies lines
	LEADING_points,				// absolute - leading value specifies points
	LEADING_atleast				// leading value is points (may not be implemented)
};

#define BULLET_LEVEL_MAX  5		// number of indent levels for an indented bulleted list

//  characters in the PressWriter Symbol font for type BULLET_symbol	
#define BULLET_circle			107	
#define BULLET_square			101	
#define BULLET_diamond			92	

// Bullet types and styles
enum BULLET_TYPE
{
	BULLET_undefined = -1,		// Transient value for multi-values.
	BULLET_none,				// no bullets
	BULLET_symbol,				// symbol bullets, a character in a font 
	BULLET_numbered,			// numbered bullets, style defined by BULLET_STYLE_NUMBERED
	BULLET_multilevel,			// multilevel bullets, style defined in BULLET_STYLE_MULTI
	BULLET_graphic				// currently not used, for graphical bullets
};

enum BULLET_STYLE				// The style of bullet numbered and multilevel lists. 
{
	BULLET_symbol_mix = 4,		//  different symbol for each level of a multilevel lists
	BULLET_arabic = 8,			// 1. default arabic 1.1.1 for multilevel lists
	BULLET_arabic_paren,		// 1) bullet number followed by a parenthesis
	BULLET_letter = 16,			// A. default uppercase followed by a period
	BULLET_letter_paren,		// A) uppercase followed by a parenthesis
	BULLET_roman = 32,			// I. roman numberal
	BULLET_roman_mix			// I.  A. 1. a. i. used for multilevel lists		
	 
};

/*
// Caps types.
*/

enum CAPS_TYPE
{
	CAPS_Normal,					// Normal Mixed
	CAPS_AllCaps,					// UPPERCASE EVERYTHING
	CAPS_NoCaps,					// lowercase everything
	CAPS_SmallCaps					// special smallcaps
};

enum POSITION_TYPE
{
	POSITON_Normal,				// normal position
	POSITON_Subscript,			// subscript position
	POSITON_Superscript			// superscript position
};

/*
// Text token codes.
*/

enum TEXT_TOKEN
{
	TOKEN_Free = -2,			// New with 3.00. Internal use only. DO NOT USE.
	TOKEN_Illegal = -1,

	FIRST_TEXT_TOKEN = 0,
   TOKEN_Face = FIRST_TEXT_TOKEN,
   TOKEN_Style,
   TOKEN_Size,
	TOKEN_Pattern,
	TOKEN_Outline,
	TOKEN_Background,
	TOKEN_Shadow,
   TOKEN_Color,

	// New with 2.00

   TOKEN_Bold,
   TOKEN_Italic,
   TOKEN_Underline,
   TOKEN_UnderlineColor,

	// New with 3.00

   TOKEN_Paragraph,

	// New with 4.00

   TOKEN_Spacing,
	TOKEN_FillPattern,
	TOKEN_FillForegroundColor,
	TOKEN_OutlineStyle,
	TOKEN_OutlineWidthType,
	TOKEN_OutlineWidth,
	TOKEN_OutlinePattern,
	TOKEN_OutlineForegroundColor,
	TOKEN_ShadowStyle,
	TOKEN_ShadowOffsetType,
	TOKEN_ShadowXOffset,
	TOKEN_ShadowYOffset,
	TOKEN_ShadowPattern,
	TOKEN_ShadowForegroundColor,
	TOKEN_WhiteOut,
   TOKEN_Hyperlink,

   LAST_TEXT_TOKEN,

	FIRST_PARAGRAPH_TOKEN = 0x1000,
	TOKEN_Alignment = FIRST_PARAGRAPH_TOKEN,
	TOKEN_LeadingType,
	TOKEN_Leading,
	TOKEN_FirstLineIndent,
	TOKEN_LeftMargin,
	TOKEN_RightMargin,
	TOKEN_SpaceBefore,
	TOKEN_SpaceAfter,
	TOKEN_TabStop,						// Contains a single tabstop.
	TOKEN_DefaultTabSize,			// Default tab size.

	// New with printShop 10

	TOKEN_BulletType,				// defined in BULLET_TYPE (character, numbered..)
	TOKEN_BulletStyle,			// style of bullet within a BULLET_TYPE
	TOKEN_BulletLevel,			// bullet indent level
	TOKEN_BulletFont,				// font (or graphic id?) for the bullet
	TOKEN_BulletColor,			// color for bullet (not fill per producer?)
	TOKEN_DropLines,			// paragraph drop caps 0 = dropcaps
////
	LAST_PARAGRAPH_TOKEN,

	TOKEN_Mask				= (SHORT)0x3FFF,
	TOKEN_Overridden		= (SHORT)0x4000,
	TOKEN_Temporary		= (SHORT)0x8000
};

enum PARAGRAPH_TOKEN
{
};

/*
// Definitions for a font style.
// (This is like bold, italic, etc.)
*/

enum FONT_STYLE
{
	FONT_STYLE_Undefined = -1,
	FONT_STYLE_Normal,
	FONT_STYLE_Bold,
	FONT_STYLE_Italic,
	FONT_STYLE_BoldItalic,
	NUMBER_OF_FONT_STYLES,
};

#define FONT_STYLE_Bold_Undefined	0x0100
#define FONT_STYLE_Italic_Undefined	0x0200

enum UNDERLINE_STYLE
{
	UNDERLINE_STYLE_Undefined = -1,
	UNDERLINE_STYLE_None = 0,
	UNDERLINE_STYLE_Single,				// Single underline
	UNDERLINE_STYLE_Double,				// Double underline
	UNDERLINE_STYLE_WordsOnly			// Only under words
};

// enum used to define the variable gutter to use when wrapping text to a shape
enum ETextWrapGutter
{
	GUTTER_none = 0,	// current state of PM7, PM8 will use GUTTER_medium
	GUTTER_tight,
	GUTTER_medium,
	GUTTER_loose
};

/////////////////////////////////////////////////////////////////////////////
//                        OLD TEXT DEFINITIONS                             //
//                                                                         //
// These are used for <3.0 documents                                       //
/////////////////////////////////////////////////////////////////////////////

/*
// Concepts and requirements:
// - There is always at least one word in a paragraph (the "eop" word).
// - Non-printing characters (eop, break) have width 0.
// - Text indexes (indicies?) must always remain consistent.
//   There are a number of routines to manage this.
*/

/*
// Type definitions.
*/

typedef SHORT	C_INDEX;				/* A character index. */
typedef SHORT	W_INDEX;				/* A word index. */
typedef SHORT	L_INDEX;				/* A line index. */
typedef SHORT	S_INDEX;				/* A style index. */

/*
// Text definitions.
*/

#define MAX_PARAGRAPH_SIZE		2000		/* Characters */

/*
// Special characters.
*/

#define	TAB_CHARACTER		0x8009
#define	BREAK_CHARACTER	0x800D
#define	MACRO_CHARACTER	0xe000		// Macros (placeholders) are >= this.

// Maximum special character possible.
#define	USER_CHARACTER_MAX	0xf800	// Unicode user range: 0xe000-0xf8ff

// Pre-defined special characters (starts at top and works down).
#define	MACRO_CHARACTER_Page	(USER_CHARACTER_MAX-1)

// Maximum macro character possible (below special characters).
#define	MACRO_CHARACTER_MAX	(MACRO_CHARACTER_Page-1) // Last possible user macro.

/*
// A delta when CHARACTERs are inserted or deleted.
*/

typedef struct
{
	C_INDEX	c_start;
	SHORT		count;
} CHARACTER_DELTA, FAR *CHARACTER_DELTA_PTR;

/*
// A character range structure.
*/

typedef struct
{
	C_INDEX	c_start;
	C_INDEX	c_end;
} CHARACTER_RANGE, FAR *CHARACTER_RANGE_PTR;

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

	// new fields added for PMW 1.00.07 and later
	LONG			line_spacing;			// baseline to base line distance for this word
} TEXT_WORD, FAR *TEXT_WORD_PTR;

inline unsigned int SizeOfOldTextWord() { return offsetof(TEXT_WORD, line_spacing); }

typedef struct
{
	UCHAR			type;						/* type of this word */
	UCHAR			flags;					/* flags for this word */
	C_INDEX		c_start;					/* First character in this word */
	PCOORD		x_offset;				/* offset of this word in text line */
	PCOORD		width;					/* width of this word */
	LONG			ascend;					/* ascend height of word */
	LONG			descend;					/* descend height of word */
} OLD_TEXT_WORD;

/*
// A delta when TEXT_WORDs are inserted or deleted.
*/

typedef struct
{
	W_INDEX	w_start;
	SHORT		count;
} WORD_DELTA, FAR *WORD_DELTA_PTR;

/*
// A word range structure.
*/

typedef struct
{
	W_INDEX	w_start;
	W_INDEX	w_end;
} WORD_RANGE, FAR *WORD_RANGE_PTR;

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
	PCOORD	line_spacing;	// baseline to baseline distance
} LINE, FAR *LINE_PTR;

inline unsigned int SizeOfOldLine() { return offsetof(LINE, line_spacing); }

/*
//	The 'old' line structure.  This structure is not just here for backward
// compatibility reasons: it happens to be useful for TEXTFLOW in comparing
//	a line's before and after states.
*/
typedef struct
{
	PCOORD	baseline;      /* baseline y offset from frame. */
	LONG		ascend;        /* ascent height for this line. */
	LONG		descend;       /* descent height for this line. */
	W_INDEX	w_start;			/* word index of first word in line */
	W_INDEX	w_end;			/* word index of last word in line */
} OLD_LINE;

/************************ the STYLE stuff ********************************/

/*
// A style change entry.
// The index in this structure tells at which point the style change
// starts; i.e. the character at this offset is the first character to
// have this new style attribute.
*/

typedef struct
{
	C_INDEX		c_index;					/* Index into text array. */
	SHORT			token;					/* (TEXT_TOKEN) Which attribute changed */
	LONG			new_value;				/* new value for attribute */
} STYLE_CHANGE, FAR *STYLE_CHANGE_PTR;

/*
// A style fraction.
// The "new_value" field must be able to contain this.
*/

typedef struct
{
	USHORT	num;
	USHORT	denom;
} STYLE_FRAC;

/////////////////////////////////////////////////////////////////////////////
//                        NEW TEXT DEFINITIONS                             //
// Begun: Apr. 9, 1996                                                     //
/////////////////////////////////////////////////////////////////////////////

/*
// Basic element indexing and counting types.
*/

typedef long int CHARACTER_INDEX;
typedef long int CHARACTER_COUNT;
typedef long int WORD_INDEX;
typedef long int WORD_COUNT;
typedef long int STYLE_INDEX;
typedef long int STYLE_COUNT;
typedef long int PARAGRAPH_INDEX;
typedef long int PARAGRAPH_COUNT;
typedef long int LINE_INDEX;
typedef long int LINE_COUNT;

typedef DWORD TEXT_STATE_ID;

/////////////////////////////////////////////////////////////////////////////
// Array elements used for text and frame records.

struct CTextStyleChange
{
// This structure is written with fixed size to disk.
// Do not add virtual members to this structure! Not even a destructor!
	CHARACTER_COUNT	m_lCharacters;	// Number of characters to next change.
												// May be zero.
	STYLE_INDEX			m_lNext;			// Next change index.
	WORD					m_wToken;		// Style change type (TEXT_TOKEN)
	WORD					m_wFlags;		// for whatever
	long					m_lValue;		// New value for attribute
	long					m_lOldValue;	// Old value for attribute
};

/*
// A text paragraph.
// These are stored in an array in the Text Record.
*/

struct CTextParagraph
{
// This structure is written with fixed size to disk.
// Do not add virtual members to this structure! Not even a destructor!
	CHARACTER_COUNT	m_lCharacters;					// # of characters in this paragraph
	DB_RECORD_NUMBER	m_lBaseParagraphStyle;		// paragraph formatting record #
	CTextStyleChange	m_StyleChange;					// Paragraph style change
	STYLE_INDEX			m_lFirstParagraphStyleChange;

	CTextParagraph(void);
};

/*
// A text word.
// These are stored in an array in the Frame Record.
// These are typically built by textflow.
*/

struct CTextWord
{
// This structure is written with fixed size to disk.
// Do not add virtual members to this structure! Not even a destructor!
	CHARACTER_COUNT	m_lCharacters;	// Number of characters in this word
	WORD					m_wType;			// Type of word (see WORD_TYPE)
	WORD					m_wFlags;		// General flags
	PCOORD				m_XOffset;		// X position relative to frame left
	PCOORD				m_Width;			// Word advance width
	PCOORD				m_Ascend;		// Ascend height of word
	PCOORD				m_Descend;		// Descend height of word
	PCOORD				m_DrawLeft;		// Left overhang of word
	PCOORD				m_DrawWidth;	// Draw width of word.
};

/*
// A text line.
// These are stored in an array in the Frame Record.
*/

struct CTextLine
{
// This structure is written with fixed size to disk.
// Do not add virtual members to this structure! Not even a destructor!
	CHARACTER_COUNT	m_lCharacters;	// Number of characters in this line
	WORD_COUNT			m_lWords;		// Number of words in this line
	PCOORD				m_Baseline;		// Baseline y offset from frame
	PCOORD				m_Ascend;		// Ascend height of line
	PCOORD				m_Descend;		// Descend height of line
	PCOORD				m_ExtentLeft;	// Left of drawing extent
	PCOORD				m_ExtentRight;	// Right of drawing extent
	short int			m_nColumn;			// Column number
	short int			m_nLineInColumn;	// Line synchronization number
	PCOORD				m_LeftOffset;	// Offset amount from m_ExtentLeft for a bullet.
	PCOORD				m_DropCapBaseline;	// base line for a drop cap 0 for no drop cap
	CFixed				m_DropCapSize;		// size if the drop cap character (for convenience)
};

/*
// A text tab.
*/

#define UNDERLINE_LEADER	((CHARACTER)0xe000)

struct CTextTab
{
// This structure is written with fixed size to disk.
// Do not add virtual members to this structure! Not even a destructor!

	enum TabType
	{
		TAB_remove = -1,	// Remove the tab (Internal)
		TAB_left,			// left adjusting horizontal tab stop
		TAB_center,			// center adjusting horizontal tab stop
		TAB_right,			// right adjusting horizontal tab stop
		TAB_decimal,		// decimal adjusting horizontal tab stop
		TAB_repeating,		// this tab repeats at a fixed amount
	};

	SHORT			m_nType;					// of type TabType
	CHARACTER	m_DecimalCharacter;	// decimal character
	CFixed		m_Position;				// positional information
	CHARACTER	m_Leader[2];			// Leader characters

	void Default(void);

	void ToStyleChange(CTextStyleChange* pChange) const;
	void FromStyleChange(const CTextStyleChange* pChange);
	PCOORD ComputePosition(const PBOX& Extent, PCOORD Position);
};

/////////////////////////////////////////////////////////////////////////////
// A helper macro.

#define DEFINE_STANDARD_STORAGE \
	ERRORCODE ReadData(StorageDevice* pDevice) \
		{ return pDevice->read_record(this, sizeof(*this)); } \
	ERRORCODE WriteData(StorageDevice* pDevice) \
		{ return pDevice->write_record(this, sizeof(*this)); } \
	ST_MAN_SIZE SizeofData(StorageDevice* pDevice) \
		{ return pDevice->size_record(sizeof(*this)); }

/*
// Character formatting.
// We don't implement ownership of the font at this level.
*/

struct CCharacterFormatting
{
// This structure is written with fixed size to disk.
// Do not add virtual members to this structure! Not even a destructor!
	// Character style currently in effect
	DB_RECORD_NUMBER		m_lStyle;			// Character style record
	// Font
	DB_RECORD_NUMBER		m_lFont;				// Record number of font.
	// Style
	BYTE						m_bBold;				// Are we Bold?
	BYTE						m_bItalic;			// Are we Italic?
	// Sizing
	CFixed					m_Size;				// Point size
	CFixed					m_BaseSize;			// Base point size
	CFixed					m_Expansion;		// Horizontal expansion
	// Character spacing (1.0 is normal, 0.5 is less, etc.)
	CFixed					m_Spacing;			// amount to adjust width
	// Lines
	BYTE						m_bUnderline;		// of type UNDERLINE_STYLE
	BYTE						m_bStrikeThrough;	// Strikethrough attribute (BOOL)
	// Specials
	BYTE						m_bCaps;				// of type CAPS_TYPE
	BYTE						m_bPosition;		// of type POSITION_TYPE
	// Effects
	COLOR						m_OldColor;			// 3.0 Character color
	SHORT						m_nOldFill;			// 3.0 Fill
	SHORT						m_nOldOutline;		// 3.0 Outline
	SHORT						m_nOldShadow;		// 3.0 Shadow

	// New Effects (Added 3/18/97 for PMG 4.0)
	SHORT m_nEffectsVersion;					// 0 = PMG 3.0
														// 1 = PMG 4.0
	SHORT m_fWhiteOut;
	FillFormatV1 m_Fill;
	OutlineFormat m_Outline;
	ShadowFormat m_Shadow;

   DB_RECORD_NUMBER  m_lHyperlinkStyle;

	BOOL DrawWhiteOut(void)
		{ return m_fWhiteOut && (!m_Fill.IsInvisibleOrWhite() || !m_Outline.IsInvisibleOrWhite() || !m_Shadow.IsInvisibleOrWhite()); }
	BOOL DrawFill(void)
		{ return m_Fill.IsVisible(); }
	BOOL DrawOutline(void)
		{ return m_Outline.IsVisible(); }
	BOOL DrawShadow(void)
		{ return m_Shadow.IsVisible(); }
	BOOL DrawFillMask(void)
		{ return !m_Fill.IsSameAsMask(); }
	BOOL DrawOutlineMask(void)
		{ return !m_Outline.IsSameAsMask(); }
	BOOL DrawShadowMask(void)
		{ return !m_Shadow.IsSameAsMask(); }

// Storage.
	DEFINE_STANDARD_STORAGE
};

/*
// Paragraph formatting.
*/

struct CParagraphFormatting
{
// This structure is written with fixed size to disk.
// Do not add virtual members to this structure! Not even a destructor!
	// Paragraph style currently in effect
	DB_RECORD_NUMBER		m_lStyle;					// Paragraph style record
	// Generally useful.
	DWORD						m_dwFlags;
	// Other useful settings.
	short int				m_nAlignment;				// of type ALIGN_TYPE
	short int				m_nLeadingType;			// of type LEADING_TYPE
	CFixed					m_Leading;					// In points.

	// Spacing.
	CFixed					m_FirstLineIndent;
	CFixed					m_LeftMargin;
	CFixed					m_RightMargin;
	CFixed					m_SpaceBefore;
	CFixed					m_SpaceAfter;
//	added for bullet/number list support new with PrintShop 10
	short int				m_nBulletType;			// of type BULLET_TYPE
	short int				m_nBulletStyle;		// style or bullet character
	BYTE						m_bBulletLevel;		// indent level of bulleted paragraph 0 based
	DB_RECORD_NUMBER		m_lBulletFont;			// font (or graphic id) for bullet
	COLOR						m_BulletColor;			// color of bullet
	short int				m_DropLines;			// # of lines for the drop cap (0=no dropcap)
// Storage.
	DEFINE_STANDARD_STORAGE
};

/*
// Frame formatting.
*/

struct CFrameFormatting
{
// This structure is written with fixed size to disk.
// Do not add virtual members to this structure! Not even a destructor!
	SHORT						m_nXFlipped;				// Are we x-flipped?
	SHORT						m_nYFlipped;				// Are we y-flipped?
	SHORT						m_nVerticalAlignment;	// VERT_ALIGN_TYPE

// Storage.
	DEFINE_STANDARD_STORAGE
};

/*
// Text metrics.
// These are the resultant metrics from a specific style.
*/

struct CTextMetrics
{
// This structure is written with fixed size to disk.
// Do not add virtual members to this structure! Not even a destructor!
	PCOORD					m_Ascend;
	PCOORD					m_Descend;
	PCOORD					m_FontXMin;
	PCOORD					m_FontXMax;
	PCOORD					m_UnderlinePosition;
	PCOORD					m_UnderlineThickness;

// Storage.
	DEFINE_STANDARD_STORAGE
};

/*
// A tab array.
*/

class CTabArray : public Array
{
public:
	// Constructor.
	CTabArray();

	// Assignment operator.
	CTabArray& operator = (const CTabArray &array);

	// Default tabstop size.
	CFixed DefaultTabSize(void) const
		{ return m_Record.m_lDefaultTabSize; }
	void DefaultTabSize(CFixed lDefaultTabSize)
		{ m_Record.m_lDefaultTabSize = lDefaultTabSize; }

	// How many tabs are in the array?
	int NumberOfTabs(void)
		{ return count(); }

	// Add a tab.
	void Add(CTextTab* pTab)
		{ add_element((LPVOID)pTab); }

	// Insert a tab.
	void Insert(CTextTab* pTab, int nWhere)
		{ insert_element((LPVOID)pTab, nWhere); }

	// Get a tab.
	CTextTab* GetAt(int nTab)
		{ return (CTextTab*)get_element(nTab); }

	// Get a tab given its position.
	int GetTabAt(CFixed lPosition);

	// Remove a tab.
	void RemoveAt(int nTab)
		{ delete_element(nTab); }

	// Remove a tab given its position.
	void RemoveTabAt(CFixed lPosition);

	// Remove all tabs.
	void RemoveAll(void)
		{ empty_array(); }

	// Find a tab given an x offset and an extent.
	BOOL FindTab(const PBOX& Extent, PCOORD Position, CTextTab* pTab);

	// Storage
	ERRORCODE ReadData(StorageDevice* pDevice);
	ERRORCODE WriteData(StorageDevice* pDevice);
	ST_MAN_SIZE SizeofData(StorageDevice* pDevice);

protected:

	struct
	{
		CFixed m_lDefaultTabSize;
	} m_Record;
};

/////////////////////////////////////////////////////////////////////////////
// Text style.

/*
// This class contains all the style/formatting components.
// Record management/ownership is handled at this level.
*/

class CTextStyle
{
public:

	// Constructor.
	CTextStyle(PMGDatabase* pDatabase = NULL, BOOL m_fTemporary = TRUE);

	// Copy constructor.
	CTextStyle(const CTextStyle far &s);

	// Destructor
	~CTextStyle();

	// Reset the style.
	void Reset(void);

	// Destroy
	void Destroy(void);

	// Convert from an old style.
	void FromOldStyle(const TextStyle& Style);

	// Set our defaults.
	void SetDefault(void);

	// Update font metrics after some kind of change.
	// This is analagous to the old fix_style() call.
	void UpdateFontMetrics(void);

	// Assignment operator.
	CTextStyle& operator = (const CTextStyle& s);

	// Equality operator.
	BOOL operator == (const CTextStyle& s);

	// "Unequality" operator.
	BOOL operator != (const CTextStyle& s) { return !(*this == s); }

	// Return the token for the passed value.
	// -1 means the incoming token value did not yield a good token.
	int TokenToIndex(int nToken) const;

	// Apply a style change.
	void ApplyChange(const CTextStyleChange* pStyleChange, BOOL fFixStyle = TRUE);

	// Build a style change.
	// 'this' style is the old style.
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle) const;

	// See if a style attribute is defined.
	BOOL TestStyle(int nToken) const;

	// See if another style matches us for this token.
	BOOL CompareStyles(int nToken, const CTextStyle* pStyle) const;

	// Make a style attribute undefined.
	void MakeUndefined(int nToken);

	// Make all text attributes undefined.
	void MakeTextUndefined(void);

	// Apply an old style change.
	void ApplyOldChange(const STYLE_CHANGE* pStyleChange, BOOL fFixStyle = TRUE);

	// Make the paragraph section undefined.
	void MakeParagraphUndefined(void);

	// Database access.
	PMGDatabase* Database(void) const
		{ return m_pDatabase; }
	void Database(PMGDatabase* pDatabase)
		{ m_pDatabase = pDatabase; }

	// Paragraph formatting access.
	DB_RECORD_NUMBER ParagraphStyle(void) const
		{ return m_Paragraph.m_lStyle; }
	int LeadingType(void) const
		{ return (int)m_Paragraph.m_nLeadingType; }
	void LeadingType(int nLeadingType)
		{ m_Paragraph.m_nLeadingType = (short int)nLeadingType; }
	int Alignment(void) const
		{ return (int)m_Paragraph.m_nAlignment; }
	void Alignment(int nAlignment)
		{ m_Paragraph.m_nAlignment = (short int)nAlignment; }

	short int BulletType(void) const
		{ return m_Paragraph.m_nBulletType; }
	void BulletType( short int type )
		{ m_Paragraph.m_nBulletType = type; }

	short int BulletStyle(void) const
		{ return m_Paragraph.m_nBulletStyle; }
	void BulletStyle( short int style )
		{ m_Paragraph.m_nBulletStyle = style; }

	CHARACTER BulletCharacter(void) const
		{ return (CHARACTER)m_Paragraph.m_nBulletStyle; }
	void BulletCharacter( CHARACTER character )
		{ m_Paragraph.m_nBulletStyle = (short int)character; }

	// not a stores value, calculated based on font size and bullet level
	CFixed BulletIndent(void) const;

	BYTE BulletLevel(void) const
		{ return m_Paragraph.m_bBulletLevel; }
	void BulletLevel( BYTE level )
		{ m_Paragraph.m_bBulletLevel = level; }

	DB_RECORD_NUMBER BulletFont(void) const
		{ return m_Paragraph.m_lBulletFont; }
	void BulletFont( DB_RECORD_NUMBER fontid )
		{ m_Paragraph.m_lBulletFont = fontid; }

	COLOR BulletColor(void) const
		{ return m_Paragraph.m_BulletColor; }
	void BulletColor( COLOR color )
		{ m_Paragraph.m_BulletColor = color; }

	short int DropLines(void) const
		{ return m_Paragraph.m_DropLines; }
	void DropLines( short int nLines )
		{ m_Paragraph.m_DropLines = nLines; }

#define Paragraph_Access(Var, Type) \
	void Var(Type l) { m_Paragraph.m_##Var = l; } \
	Type Var(void) const { return m_Paragraph.m_##Var; }

	Paragraph_Access(FirstLineIndent, CFixed)
	Paragraph_Access(LeftMargin, CFixed)
	Paragraph_Access(RightMargin, CFixed)
	Paragraph_Access(Leading, CFixed)
	Paragraph_Access(SpaceBefore, CFixed)
	Paragraph_Access(SpaceAfter, CFixed)
#undef Paragraph_Access

	// Character formatting access.
	const CCharacterFormatting* GetCharacterFormatting(void) const
		{ return &m_Character; }
	DB_RECORD_NUMBER CharacterStyle(void) const
		{ return m_Character.m_lStyle; }
	DB_RECORD_NUMBER Font(void) const
		{ return m_Character.m_lFont; }
	void Font(DB_RECORD_NUMBER lFont);
	int Bold(void) const
		{ return (int)(char)m_Character.m_bBold; }
	void Bold(int fBold)
		{ m_Character.m_bBold = (BYTE)fBold; }
	int Italic(void) const
		{ return (int)(char)m_Character.m_bItalic; }
	void Italic(int fItalic)
		{ m_Character.m_bItalic = (BYTE)fItalic; }
	CFixed Size(void) const
		{ return m_Character.m_Size; }
	void Size(CFixed Size)
		{ m_Character.m_Size = Size; }
	CFixed BaseSize(void) const
		{ return m_Character.m_BaseSize; }
	void BaseSize(CFixed Size)
		{ m_Character.m_BaseSize = Size; }
	CFixed Expansion(void) const
		{ return m_Character.m_Expansion; }
	void Expansion(CFixed Expansion)
		{ m_Character.m_Expansion = Expansion; }
	CFixed Spacing(void) const
		{ return m_Character.m_Spacing; }
	void Spacing(CFixed Spacing)
		{ m_Character.m_Spacing = Spacing; }
	int Underline(void) const
		{ return (int)(char)m_Character.m_bUnderline; }
	void Underline(int nUnderline)
		{ m_Character.m_bUnderline = (BYTE)nUnderline; }
	int StrikeThrough(void) const
		{ return (int)(char)m_Character.m_bStrikeThrough; }
	int Caps(void) const
		{ return (int)(char)m_Character.m_bCaps; }
	int Position(void) const
		{ return (int)(char)m_Character.m_bPosition; }
	int WhiteOut(void) const
		{ return m_Character.m_fWhiteOut; }
	void WhiteOut(int fWhiteOut)
		{ m_Character.m_fWhiteOut = fWhiteOut; }

//	COLOR OldColor(void) const
//		{ return m_Character.m_OldColor; }
	void OldColor(COLOR c)
	{
		m_Character.m_Fill.SetColor(m_Character.m_Fill.m_nPattern, c);
		m_Character.m_Outline.SetColor(m_Character.m_Outline.m_nPattern, c);
		m_Character.m_Shadow.SetColor(m_Character.m_Shadow.m_nPattern, c);
	}

	void OldFill(SHORT nFill)
		{ m_Character.m_Fill.FromOld(nFill, m_Character.m_Fill.m_ForegroundColor); }

	void OldOutline(SHORT nOutline)
		{ m_Character.m_Outline.FromOld(nOutline, m_Character.m_Outline.m_ForegroundColor); }

	void OldShadow(SHORT nShadow)
		{ m_Character.m_Shadow.FromOld(nShadow, m_Character.m_Shadow.m_ForegroundColor); }

#define Effect_Access(Type) \
	Type##Format& Type(void) { return m_Character.m_##Type; } \
	void Type(const Type##Format& Type) { m_Character.m_##Type = Type; } \
	void Type(int n##Type, COLOR Color) { m_Character.m_##Type.FromOld(n##Type, Color); }

	Effect_Access(Outline)
	Effect_Access(Shadow)

#undef Effect_Access

#define Effect_Field_Access(Effect, Field, Type, Var) \
	void Effect##Field(Type Var) { m_Character.m_##Effect.m_##Var = Var; } \
	const Type Effect##Field(void) const { return m_Character.m_##Effect.m_##Var; }

	Effect_Field_Access(Fill, Pattern, int, nPattern)
	Effect_Field_Access(Fill, ForegroundColor, COLOR, ForegroundColor)

	Effect_Field_Access(Outline, Style, SHORT, nStyle)
	Effect_Field_Access(Outline, WidthType, SHORT, nWidthType)
	Effect_Field_Access(Outline, Width, CFixed, lWidth)
	Effect_Field_Access(Outline, Pattern, SHORT, nPattern)
	Effect_Field_Access(Outline, ForegroundColor, COLOR, ForegroundColor)

	Effect_Field_Access(Shadow, Style, SHORT, nStyle)
	Effect_Field_Access(Shadow, OffsetType, SHORT, nOffsetType)
	Effect_Field_Access(Shadow, XOffset, CFixed, lXOffset)
	Effect_Field_Access(Shadow, YOffset, CFixed, lYOffset)
	Effect_Field_Access(Shadow, Pattern, SHORT, nPattern)
	Effect_Field_Access(Shadow, ForegroundColor, COLOR, ForegroundColor)
#undef Effect_Field_Access

	FillFormatV1& Fill(void) { return m_Character.m_Fill; } \
	void Fill(const FillFormatV1& Fill) { m_Character.m_Fill = Fill; } \
	void Fill(int nFill, COLOR Color) { m_Character.m_Fill.FromOld(nFill, Color); }

	void FillSetType(int nType, COLOR Color = TRANSPARENT_COLOR)
		{ m_Character.m_Fill.SetType(nType, Color); }
	void FillSetColor(int nPattern, COLOR Color)
		{ m_Character.m_Fill.SetColor(nPattern, Color); }

	void OutlineSetType(int nType, CFixed lWidth = 0)
		{ m_Character.m_Outline.SetType(nType, lWidth); }
	void OutlineSetColor(int nPattern, COLOR Color)
		{ m_Character.m_Outline.SetColor(nPattern, Color); }

	void ShadowSetType(int nType)
		{ m_Character.m_Shadow.SetType(nType, MakeFixed(TEXT_SHADOW_FRACTION)); }
	void ShadowSetColor(int nPattern, COLOR Color)
		{ m_Character.m_Shadow.SetColor(nPattern, Color); }

	// Text metrics access.

#define Metric_Access(Var, Type) \
	void Var(Type l) { m_Metrics.m_##Var = l; } \
	Type Var(void) const { return m_Metrics.m_##Var; }

	Metric_Access(Ascend, PCOORD)
	Metric_Access(Descend, PCOORD)
	Metric_Access(FontXMin, PCOORD)
	Metric_Access(FontXMax, PCOORD)
	Metric_Access(UnderlinePosition, PCOORD)
	Metric_Access(UnderlineThickness, PCOORD)

#undef Metric_Access

	// Frame formatting access.

	int XFlipped(void) const
		{ return (int)m_Frame.m_nXFlipped; }
	void XFlipped(int n)
		{ m_Frame.m_nXFlipped = (SHORT)n; }

	int YFlipped(void) const
		{ return (int)m_Frame.m_nYFlipped; }
	void YFlipped(int n)
		{ m_Frame.m_nYFlipped = (SHORT)n; }

	int VerticalAlignment(void) const
		{ return (int)m_Frame.m_nVerticalAlignment; }
	void VerticalAlignment(int n)
		{ m_Frame.m_nVerticalAlignment = (SHORT)n; }

	// The style components themselves.
	// These are public, but please use the access functions when possible.

	CParagraphFormatting		m_Paragraph;				// Paragraph formatting
	CTabArray					m_Tabs;						// Tabs
	CCharacterFormatting		m_Character;				// Character formatting
	CTextMetrics				m_Metrics;					// Text metrics
	CFrameFormatting			m_Frame;						// Frame formatting

	// Room for expansion.
	struct
	{
	} m_Record;

	// Storage.
	void ReadData(StorageDevice* pDevice);
	void WriteData(StorageDevice* pDevice);
	ST_MAN_SIZE SizeofData(StorageDevice* pDevice);

	CFrameObject* Frame(void) const
		{ return m_pFrame; }
	void Frame(CFrameObject* pFrame)
		{ m_pFrame = pFrame; }

	// Frame object access functions.
	CFixed GetEffectsRotation(void);

	// Initialization of unique font attribute structures.
	BOOL GetFontDescription(CFontDescription* pFontDescription);
	// Hyperlink stuff
   DB_RECORD_NUMBER HyperlinkStyle(void) const
	{ return m_Character.m_lHyperlinkStyle; }
	
   void HyperlinkStyle(DB_RECORD_NUMBER nHyperlinkStyle);
   
   BOOL
   GetHyperlinkData(HyperlinkData &Data);
protected:
	CFrameObject* m_pFrame;
	PMGDatabase* m_pDatabase;
	BOOL m_fTemporary;
};

/////////////////////////////////////////////////////////////////////////////
// Text state.
// This provides a "snapshot" of the text variables (excluding style) at a
// particular point in the text stream. These are allocated by the Text
// Record. They are used by Text Frames, TXPs, and anybody else pointing at a
// specific point in the text.

struct CTextState
{
//	TEXT_STATE_ID			m_ID;
	CHARACTER_INDEX		m_lCharacter;

	// Text record level

	PARAGRAPH_INDEX		m_lParagraph;
	CHARACTER_INDEX		m_lCharacterInParagraph;

	// Assignment operator.
//	CTextState& operator = (const CTextState& s);
	// Reset me!
	void Reset(void);
};

/////////////////////////////////////////////////////////////////////////////
// A text iterator.
//
// This includes a text state and a text style. It is used by things that need
// to maintain a complete set of variables while traversing the text
// (like TXPs).

class CTextIterator
{
public:
	CTextIterator(CTextRecord* pRecord = NULL);
	~CTextIterator(void);

	void SetRecord(CTextRecord* pRecord);

	// Style. Get a reference to the style.
	CTextStyle& Style(void)
		{ return m_Style; }

	// State. Get the text state.
	CTextState& State(void)
		{ return m_State; }

	// Character index. Where are we in the character stream?
	CHARACTER_INDEX CharacterIndex(void)
		{ return State().m_lCharacter; }
	void CharacterIndex(CHARACTER_INDEX lCharacter)
		{ State().m_lCharacter = lCharacter; }

	// Paragraph index. Where are we in the paragraph array?
	PARAGRAPH_INDEX ParagraphIndex(void)
		{ return State().m_lParagraph; }
	void ParagraphIndex(PARAGRAPH_INDEX lParagraph)
		{ State().m_lParagraph = lParagraph; }

	// Character index within paragraph. Where are we relative to the paragraph?
	CHARACTER_INDEX CharacterInParagraph(void)
		{ return State().m_lCharacterInParagraph; }
	void CharacterInParagraph(CHARACTER_INDEX lCharacterInParagraph)
		{ State().m_lCharacterInParagraph = lCharacterInParagraph; }

	// Database. What database are we owned by?
	PMGDatabase* Database(void)
		{ return m_Style.Database(); }

	//
	// Routine for setting the current position.
	//

	void SetPosition(CHARACTER_INDEX Index);

	// Update the style given a new position (state).
	void UpdateStyle(BOOL fReset = FALSE);

	// Reset this iterator.
	void Reset(void)
		{ SetRecord(NULL); }

	// Reset to the paragraph style.
	void ResetToParagraphStyle(void);

	// Reload the current change after a modification to the change array.
	CTextStyleChange* ReloadCurrentChange(void);

	// Get the current style change.
	CTextStyleChange* CurrentChange(void)
		{ return m_pCurrentChange; }

	// Get the index of the current style change.
	STYLE_INDEX CurrentChangeIndex(void) const
		{ return m_lCurrentChange; }

	// Get the start of the current style change.
	CHARACTER_INDEX CurrentChangeStart(void) const
		{ return m_lCurrentChangeStart; }

	// Get the index of the current style change.
	STYLE_INDEX LastChangeIndex(void) const
		{ return m_lLastChange; }

	// Get the index of the current style change.
	void LastChangeIndex(STYLE_INDEX lLastChange)
		{ m_lLastChange = lLastChange; }

	// Return the start of the next style change.
	CHARACTER_INDEX NextChangeStart(void);

	// Return the start of the next paragraph.
	CHARACTER_INDEX NextParagraphStart(void);

	// Return the start of the previous paragraph.
	CHARACTER_INDEX PreviousParagraphStart(void);

	// Advance to the next style change.
	void AdvanceCurrentChange(void);

	// Grab the current style change.
	void GrabCurrentChange(BOOL fFixStyle = FALSE);

	// Backup a change. This can only be done once. And it does not revert
	// m_Style. Used primarily for change traversal.
	void BackupAChange(void);

	// Insert a style change here.
	void InsertStyleChange(const CTextStyleChange& Change);

	// Delete the current style change.
	void DeleteCurrentChange(void);

	// Compute the style over a range.
	void ComputeStyleRange(CHARACTER_INDEX lStart, CHARACTER_INDEX lEnd, CTextStyle* pStyle);

	// Compute the style limited to a paragraph.
	void ComputeParagraphStyleRange(CHARACTER_INDEX lStart, CHARACTER_INDEX lEnd, CTextStyle* pStyle, BOOL fMergeStyles);

protected:
   // Helpers
   CTextRecord* GetTextRecord()
      { return m_pRecord; }

protected:

	// The text record we are bound to.

	CTextRecord*	m_pRecord;

	// The state we are following.

	CTextState		m_State;

	// The style at this point.

	CTextStyle		m_Style;

	// The state corresponding to current style.
	// Usually same as m_State, but different temporarily when we move
	// m_State to somewhere else.

	CTextState		m_LastStyleState;		// State corresponding to style.

	// The current style change to be processed.
	CTextStyleChange*	m_pCurrentChange;
	STYLE_INDEX m_lCurrentChange;
	CHARACTER_INDEX m_lCurrentChangeStart;

	// The last style change we processed.
	// This is useful when deleting style change (since they are stored in
	// linked-list fashion).
	STYLE_INDEX m_lLastChange;

};

/////////////////////////////////////////////////////////////////////////////
// CFrameAttributes structure.
// This is here for ease of inclusion (do not need framerec.h).

// A place to pass parameters in and out.

struct CFrameAttributes
{
	CFrameAttributes()
		{ m_fSet = FALSE; }
	BOOL m_fSet;

	short int m_nVerticalAlignment;
	CFixedRect m_Margins;
	short int m_nColumns;
	CFixed m_lColumnSpacing;
	CWarpField* m_pWarpField;

	FillFormatV1 m_Fill;
	OutlineFormat m_Outline;
	ShadowFormat m_Shadow;
};

#endif						/* #ifndef __TEXT_H__ */


