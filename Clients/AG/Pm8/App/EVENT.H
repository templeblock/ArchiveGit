/*
// $Header: /PM8/App/EVENT.H 1     3/03/99 6:05p Gbeddow $
//
// Document record definitions.
//
// $Log: /PM8/App/EVENT.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:40   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:46   Fred
// Initial revision.
// 
//    Rev 1.2   14 Apr 1997 15:42:46   Jay
// Text box editing changes
// 
//    Rev 1.1   21 Feb 1997 15:11:34   Jay
// New mode handling; crop mode support
// 
//    Rev 1.0   14 Mar 1996 13:37:46   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:10   FRED
// Initial revision.
// 
//    Rev 2.2   05 Jun 1995 11:33:06   JAY
//  
// 
//    Rev 2.1   08 Feb 1995 13:34:04   JAY
// Reverted. New series.
// 
//    Rev 1.1   04 Aug 1994 08:30:50   JAY
// Large model and 32-bit changes
// 
//    Rev 1.0   16 Dec 1993 16:28:36   JAY
//  
*/

#ifndef __EVENT_H__
#define __EVENT_H__

/* FLAGS for the event_info */

#define SHIFT_KEY( key )  ( key & MK_SHIFT )
#define CTRL_KEY( key )	  ( key & MK_CONTROL )

typedef struct
{
	unsigned scan_code        :  8;
	unsigned extended_key     :  1;
	unsigned not_used		     :  2;
	unsigned used_internally  :  2;
	unsigned context_code     :  1;
	unsigned previous_state   :  1;
	unsigned transition_state :  1;
} KEYSTATE;

typedef enum
{	
	EVENT_MOUSE_MOVE,
	EVENT_BUTTON_RISING,
	EVENT_BUTTON_FALLING,
	EVENT_BUTTON_DBLCLK,
	EVENT_RBUTTON_RISING,
	EVENT_RBUTTON_FALLING,
	EVENT_RBUTTON_DBLCLK,
	EVENT_KEY_RISING,
	EVENT_KEY_FALLING,
	EVENT_KEY_DOWN,
	EVENT_KEY,
	EVENT_EDIT_CUT,
	EVENT_EDIT_COPY,
	EVENT_EDIT_PASTE,
	EVENT_EDIT_DELETE,

	// Special mode switching events.
	EVENT_ENTERING_MODE = 4000,
	EVENT_LEAVING_MODE,
	EVENT_REENTERING_MODE,
} EVENT;

typedef struct 
{
	EVENT    event;				// What event type this is.
	FLAGS 	flags;
	CPoint 	current_xy;
	CPoint 	old_xy;
	BOOL 		new_position;
	WORD		v_key;
	USHORT	repeat_count;
	KEYSTATE	key_state;
	BOOL		fInternal;			// for mode switches
	BOOL		fInit;				// for mode switches
} EVENT_INFO;

extern EVENT_INFO event_info;

#endif					// #ifndef __EVENT_H__
