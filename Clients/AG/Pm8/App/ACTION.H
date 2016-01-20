/*
// $Header: /PM8/App/ACTION.H 1     3/03/99 6:01p Gbeddow $
//
// Actions for PrintMaster Gold page objects.
//
// $Log: /PM8/App/ACTION.H $
// 
// 1     3/03/99 6:01p Gbeddow
// 
// 2     4/14/98 4:41p Jayn
// Changes for the new table object.
// 
//    Rev 1.0   14 Aug 1997 15:17:58   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:48:30   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:36   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:18   JAY
// Initial revision.
// 
//    Rev 1.0   16 Dec 1993 16:25:36   JAY
//  
*/

#ifndef __ACTION_H__
#define __ACTION_H__

/* Generic object actions. */

#define	ACTION_DO_NOTHING		0				/* Nothing at all... */
#define	ACTION_SELECT			1				/* Select the object. */
#define	ACTION_DEFAULT_EDIT	2				/* Edit in a default manner. */

/* Calendar actions. */

#define	ACTION_SELECT_DATE		100			/* Select a date. */
#define	ACTION_EDIT_DATE			101			/* Edit a date. */
#define	ACTION_EDIT_TITLE			102			/* Edit the calendar title. */
#define	ACTION_CALENDAR_STYLE	103			/* Edit the calendar style. */

/* Card actions. */

#define	ACTION_EDIT_PANEL			200			/* High-level panel select. */

// Table actions
#define	ACTION_SELECT_CELLS		300			// Select cells in the table.

#endif						/* #ifndef __ACTION_H__ */
