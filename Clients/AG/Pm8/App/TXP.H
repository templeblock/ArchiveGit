/*
// $Header: /PM8/App/TXP.H 1     3/03/99 6:12p Gbeddow $	
//
// Text pointer (insertion point) definitions.
//
// $Log: /PM8/App/TXP.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:42   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:22   Fred
// Initial revision.
// 
//    Rev 1.1   26 Jun 1996 14:22:22   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:46:12   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:24   FRED
// Initial revision.
// 
//    Rev 2.5   03 May 1995 18:08:36   FRED
// 
//    Rev 2.4   29 Mar 1995 08:59:14   JAY
// Lots of changes
// 
//    Rev 2.3   15 Mar 1995 10:40:52   JAY
// Support for new text editing
// 
//    Rev 2.2   09 Mar 1995 13:49:06   JAY
// Added access to the "transition" member.
// 
//    Rev 2.1   17 Feb 1995 16:45:06   JAY
// Changes necessary for text undo.
// 
//    Rev 2.0   07 Feb 1995 15:49:06   JAY
// Initial revision.
// 
//    Rev 1.3   28 Jan 1994 17:11:38   JAY
//  
// 
//    Rev 1.2   19 Jan 1994 17:08:18   JAY
//  
// 
//    Rev 1.1   13 Jan 1994 17:01:18   JAY
//  
// 
//    Rev 1.0   16 Dec 1993 16:26:02   JAY
//  
// 
//    Rev 1.9   14 May 1993 13:52:24   JAY
// Added first_c_in_line() and last_c_in_line().
// 
//    Rev 1.8   12 May 1993 14:36:50   JAY
// Now attempts to pull as many "like" keystrokes from the keyboard buffer
// before calling enhanced txp functions for block operations.
// 
//    Rev 1.7   24 Mar 1993 11:56:24   MACDUFF
// Support for style changes in selection ranges with multiple styles
// 
//    Rev 1.6   23 Mar 1993 16:45:18   JAY
// Changed 'stretched' flag to 'stretch_text'.
// 
//    Rev 1.5   24 Feb 1993 13:31:48   JAY
// Added "stretched" flag to TXP.
// 
//    Rev 1.4   09 Feb 1993 12:43:32   JAY
// Fixed text selection.
// Added additional positioning and selection keys.
// 
//    Rev 1.3   08 Feb 1993 10:45:34   JAY
// New text routines.
// 
//    Rev 1.2   05 Feb 1993 13:04:14   JAY
// Text selection code and support
*/

#ifndef __INC_TXP__
#define __INC_TXP__

#if 0
typedef class Txp far *TxpPtr;

/*
// Functions for toggling parts of a txp.
// These allow toggling of the single cursor and toggling of the text select.
// Bits in the "which" TOGGLE variable may be OR'd for multiple action!
*/

#define TXP_TOGGLE_CURSOR	1
#define TXP_TOGGLE_SELECT	2
#define TXP_TOGGLE_ALL		3
typedef FLAGS TXP_TOGGLE;

typedef VOID (*TXP_TOGGLE_FUNCTION)(TxpPtr, TXP_TOGGLE which, VOIDPTR data);

#define	TXP_FLAG_start				1					/* At start of text */
#define	TXP_FLAG_end				2					/* At end of text */
#define	TXP_FLAG_stretch_text	4					/* Frame has stretched text */
#define	TXP_FLAG_stretch_frame	8					/* Frame stretchs */

typedef enum
{
	TXP_NO_SELECTION,
	TXP_LEFT_END,
	TXP_RIGHT_END,
	TXP_UNKNOWN_END,
	TXP_CURRENT_END						/* Not universally supported. */
} TXP_END;

typedef enum
{
	TXP_COMPATABILITY_Safe,
	TXP_COMPATABILITY_PMG,
	TXP_COMPATABILITY_Windows
} TXP_COMPATABILITY;

/*
// A text pointer.
*/

FARCLASS Txp
{
private:
	PMGDatabasePtr			database;

	PCOORD					desired_x;
	DB_RECORD_NUMBER		f_record;
	DB_RECORD_NUMBER		p_record;
	TXP_TOGGLE_FUNCTION	toggle_function;
	VOIDPTR					toggle_data;

	FrameObjectPtr			object;
	C_INDEX					c_index;
	TextStyle				style;
	PBOX						cursor_box;
	PBOX						select_bound;
	C_INDEX					select_start;
	C_INDEX					select_end;
	TXP_TOGGLE				visible;
	FLAGS						flags;
	TextStyle				select_style;		/* composite style for selected region */
	BOOL						transition;

/*
// Change lines.
*/

	ERRORCODE change_line(SHORT l_delta, BOOL shifted);

public:

/*
// Constructor.
*/

	Txp(PMGDatabasePtr ref);

/*
// Destructor.
*/

	virtual ~Txp() {};

/*
// Return whether text is selected or not.
*/

	BOOL something_selected(VOID)
		{ return select_start != select_end; }

/*
// Get the object for this txp.
*/

	FrameObjectPtr get_object(VOID) 
		{ return object; }

/*
// Get the database for this txp.
*/

	PMGDatabasePtr get_database(VOID)
 		{ return database; }

/*
// Get the frame this txp is in.
*/

	DB_RECORD_NUMBER get_frame(VOID)
		{ return f_record; }

/*
// Get the paragraph this txp is in.
*/

	DB_RECORD_NUMBER get_paragraph(VOID)
		{ return p_record; }

/*
// Get the txp's flags.
*/

	FLAGS get_flags(VOID)
		{ return flags; }

/*
// Get the txp's visible flags.
*/

	TXP_TOGGLE get_visible(VOID)
		{ return visible; }

/*
// Get the cursor box.
*/

	PBOX get_cursor_box(VOID)
		{ return cursor_box; }

/*
// Get the transition flag.
*/

	BOOL get_transition(void)
		{ return transition; }

/*
// Set the transition flag.
*/

	void set_transition(BOOL fTransition = TRUE)
		{ transition = fTransition; }

/*
// Set the select bound.
*/

	VOID set_select_bound(PBOX pbox)
		{ select_bound = pbox; }

/*
// Get the select bound.
*/

	PBOX get_select_bound(VOID)
		{ return select_bound; }

/*
// Get the cursor index.
*/

	C_INDEX get_index(VOID)
		{ return c_index; }

/*
// Set the cursor index.
*/

	VOID set_index(C_INDEX index)
		{ c_index = index; }

/*
// Get the start of the select.
*/

	C_INDEX get_select_start()
		{ return select_start; }

/*
// Get the end of the select.
*/

	C_INDEX get_select_end()
		{ return select_end; }

/*
// Get the style for this txp.
*/

	TextStyleRef get_style(VOID)
		{ return style; }

/*
// Set the style for this txp.
*/

	VOID set_style(TextStyleRef new_style)
		{ style = new_style; }

/*
// Get the selection style for this txp.
*/

	TextStyleRef get_select_style(VOID)
		{ return select_style; }

/*
// Reset the txp.
*/

	VOID reset(BOOL fSaveFace = FALSE);

/*
// Return whether the txp is valid or not.
*/

	BOOL valid(VOID);

/*
// Attach a txp object to a frame.
*/

	VOID init(FrameObjectPtr frame_object, C_INDEX offset, TXP_TOGGLE_FUNCTION tfunc = NULL, VOIDPTR data = NULL);

/*
// Toggle the txp.
*/

	VOID toggle(TXP_TOGGLE which = TXP_TOGGLE_ALL);

/*
// Turn the all or part of a txp on.
*/

	VOID on(TXP_TOGGLE which = TXP_TOGGLE_ALL);

/*
// Turn the all or part of a txp off.
*/

	VOID off(TXP_TOGGLE which = TXP_TOGGLE_ALL);

/*
// Set the position for a txp.
*/

	VOID set_position(C_INDEX c_new, TXP_END which_end, BOOL set_desired_x);

/*
// Deselect all text selected by a txp.
*/

	VOID deselect(VOID);

/*
// Select all text.
*/

	VOID select_all(VOID);

/*
// Select a range of text.
*/

	void select_text(C_INDEX c_start, C_INDEX c_end);

/*
// Return the x offset corresponding to where the txp currently is.
*/

	PCOORD x_offset(TEXT_WORD_PTR wp = NULL);

/*
// Calculate a cursor box for the txp.
*/

	ERRORCODE calc_cursor(VOID);

/*
// Compute the style at the txp's current location.
*/

	ERRORCODE calc_style(VOID);

/*
// Calculate a cursor box for the txp and the style at that point.
*/

	ERRORCODE calc_all(VOID);

/*
// Set the desired x for a txp.
// This is the x position that is maintained when the cursor is moved up or
// down. The x position used is the txp's current x position.
*/

	VOID set_desired_x(VOID);

/*
// Move the txp to a point.
// This is useful for the mouse interface.
*/

	ERRORCODE move_to_point(PPNT p, TXP_END which_end, BOOL set_desired_x);

/*************************/
/* Character management. */
/*************************/

/*
// Insert a string into the text at the current txp location.
*/

	ERRORCODE insert_text(CHARACTER_PTR s, SHORT count, BOOL single = FALSE);

/*
// Delete characters in front of the current txp location.
*/

	ERRORCODE delete_text(SHORT count);

/*
// Delete characters in back of the current txp location.
*/

	ERRORCODE backspace(SHORT count);

/****************************/
/* Cursor movement methods. */
/****************************/

/*
// Move the insertion point left.
*/

	VOID left(BOOL shifted);

/*
// Move the insertion point right.
*/

	VOID right(BOOL shifted);

/*
// Move the insertion point left a word.
*/

	VOID left_word(BOOL shifted);

/*
// Move the insertion point right a word.
*/

	VOID right_word(BOOL shifted);

/*
// Move the insertion point up a line.
*/

	VOID up(BOOL shifted);

/*
// Move the insertion point down a line.
*/

	VOID down(BOOL shifted);

/*
// Move the insertion point to the beginning of current line.
*/

	VOID bol(BOOL shifted);

/*
// Move the insertion point to the end of current line.
*/

	VOID eol(BOOL shifted);

/*
// Move the insertion point to the beginning of current paragraph.
*/

	VOID bop(BOOL shifted);

/*
// Move the insertion point to the end of current paragraph.
*/

	VOID eop(BOOL shifted);

/*
// Return the first character index for the current line.
*/

	C_INDEX first_c_in_line(VOID);

/*
// Return the last character index for the current line.
*/

	C_INDEX last_c_in_line(VOID);

/*
// Get the character range corresponding to this txp.
*/

	VOID get_crange(CHARACTER_RANGE_PTR crange);

/*
// Update the range within the domain of this txp after a style change.
*/

	ERRORCODE update_range(CHARACTER_RANGE* pRange = NULL);

/***********/
/* Globals */
/***********/

/*
// Compatability mode.
*/

	static TXP_COMPATABILITY compatability;

};
#endif

#endif								/* #ifndef __INC_TXP__ */
