/*
// $Header: /PM8/App/STYLEIT.H 1     3/03/99 6:12p Gbeddow $
//
// Style interator definitions.
//
// $Log: /PM8/App/STYLEIT.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:43:04   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:10:26   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:49:00   JAY
// Initial revision.
// 
//    Rev 1.1   06 Jan 1994 16:06:20   JAY
// Added get_s_index() and get_c_index().
// 
//    Rev 1.0   16 Dec 1993 16:26:12   JAY
//  
*/

#ifndef __STYLEIT_H__
#define __STYLEIT_H__

/*
// A paragraph style iterator.
// This is used to march through the style changes in a paragraph so the
// text stream can be parsed with style.
*/

FARCLASS StyleIterator
{
	friend ERRORCODE ParagraphRecord::adjust_style(CHARACTER_DELTA cdelta);
	friend SHORT ParagraphRecord::change_range_style(CHARACTER_RANGE crange, TextStylePtr new_style, BOOL temporary);
private:
	ParagraphPtr paragraph;		/* current paragraph. */
	C_INDEX	c_index;				/* offset in paragraph. */

	S_INDEX	s_index;				/* The index of this style */

	BOOL sip_advance(VOID);
	SHORT grab_style_change(BOOL fix_style = FALSE);

public:
	TextStyle style;				/* Current accumulated style. */
	STYLE_CHANGE_PTR scp;		/* Next style change to be parsed. */

	StyleIterator(ParagraphPtr paragraph = NULL);

	VOID init(ParagraphPtr paragraph);
	VOID compute(C_INDEX where);
	VOID compute_style_range(C_INDEX c_start, C_INDEX c_end, TextStylePtr style_result);
	S_INDEX get_s_index()
		{ return s_index; }
	C_INDEX get_c_index()
		{ return c_index; }
};

typedef StyleIterator far *StyleIteratorPtr;

#endif					// #ifndef __STYLEIT_H__
