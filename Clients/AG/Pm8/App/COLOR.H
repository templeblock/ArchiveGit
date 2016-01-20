/*
// $Header: /PM8/App/COLOR.H 1     3/03/99 6:04p Gbeddow $
//
// Definitions for colors.
//
// $Log: /PM8/App/COLOR.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 2     9/14/98 12:06p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:19:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:26:24   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:24   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:39:12   JAY
// Reverted. New series.
// 
//    Rev 1.1   14 Feb 1994 11:28:56   JAY
// Made system_color_list near.
// 
//    Rev 1.0   16 Dec 1993 16:28:52   JAY
//  
*/

#ifndef __COLOR_H__
#define __COLOR_H__

#include "list.h"
#include "pagedefs.h"

typedef class ColorElement far *ColorElementPtr;

FARCLASS ColorElement : public ListElement
{
private:
	COLOR	m_color;
	CString m_csName;
public:
/* Constructor. */

	ColorElement(LPCSTR name, COLOR color);

/* Destructor. */

	virtual ~ColorElement();

/* Get the name of this element. */

	LPCSTR get_name(void) const;

/* Get the color of this element. */

	COLOR get_color(void) const;

/* Get the next color element in the list. */

	ColorElementPtr next_color();
};

/*
// A color list.
*/

FARCLASS ColorList : public List
{
public:
	ColorList();

/* Return the first color in the list. */

	ColorElementPtr first_color();

/* Read the color list. */

	ERRORCODE read(LPCSTR name);
};

/*
// Inline functions.
*/

/* ColorElement */

inline LPCSTR ColorElement::get_name(void) const
	{ return m_csName; }
inline COLOR ColorElement::get_color(void) const
	{ return m_color; }
inline ColorElementPtr ColorElement::next_color()
	{ return (ColorElementPtr)next_element(); }

/* ColorList */

inline ColorElementPtr ColorList::first_color()
	{ return (ColorElementPtr)first_element(); }

/*
// Externals.
*/

/* The global color list. */

extern ColorList near system_color_list;

#endif				// #ifndef __COLOR_H__

