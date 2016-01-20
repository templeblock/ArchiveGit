/*
// $Header: /PM8/App/MODETCRT.CPP 1     3/03/99 6:08p Gbeddow $
//
// Mode text frame creation routines.
//
// $Log: /PM8/App/MODETCRT.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:22:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:32:58   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:29:30   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:37:50   JAY
// Reverted. New series.
// 
//    Rev 1.1   05 Jan 1994 08:10:46   JAY
// Restructuring, cut/paste, images, etc.
# 
#    Rev 1.0   16 Dec 1993 16:28:06   JAY
#  
*/

#include "system.h"
#include "stdafx.h"
#include "pmw.h"

#include "pmwdoc.h"
#include "pmwview.h"

#include "frameobj.h"

/*
// Normal state.
*/

BOOL CPmwView::text_create_state_normal(EVENT_INFO *info)
{
	if (check_escape_event(info))
	{
		set_arrow_tool();
	}
	else if (info->flags & MK_LBUTTON)
	{
		rc.screen_to_ppnt(info->current_xy, &select_start);
		edit_state = TEXT_CREATE_STATE_DRAGGING;
	}
	return TRUE;
}

/*
// Initialization state.
*/

BOOL CPmwView::text_create_state_init(EVENT_INFO *info)
{
	deselect_all();

	set_cursor(AfxGetApp()->LoadCursor(IDC_TEXT_CREATE_CURSOR));

	edit_state = STATE_NORMAL;

	return text_create_state_normal(info);				/* Do it now, too. */
}

/*
// Release state.
// Wait for the user to release the mouse button.
*/

BOOL CPmwView::text_create_state_release(EVENT_INFO *info)
{
	if (!(info->flags & MK_LBUTTON))
	{
	/* Done waiting! */
		set_arrow_tool();
	}
	return TRUE;
}

BOOL CPmwView::text_create_state_dragging(EVENT_INFO *info)
{
	BOOL stop_now = check_escape_event(info);

/* Compute where we are. */

	PPNT p;

	rc.screen_to_ppnt(info->current_xy, &p);

	if (!stop_now && (info->flags & MK_LBUTTON))
	{
		if (info->new_position)
		{
			RECT r;

		/* Mouse moved. */

			if (marquee == NULL)
			{
				if ((marquee = new Marquee(GetSafeHwnd())) == NULL)
				{
					edit_state = STATE_RELEASE;
					return TRUE;
				}
				rc.register_toggle(marquee_toggle, marquee);
			}

		/* We have the marquee now. */

		/* Construct the rectangle for it. */

			r.left = info->current_xy.x;
			r.top = info->current_xy.y;
			r.right = rc.page_x_to_screen(select_start.x);
			r.bottom = rc.page_y_to_screen(select_start.y);

		/* Normalize. */

			if (r.left > r.right)
			{
				SHORT tmp = r.left;
				r.left = r.right;
				r.right = tmp;
			}

			if (r.top > r.bottom)
			{
				SHORT tmp = r.top;
				r.top = r.bottom;
				r.bottom = tmp;
			}

		/* Update the rectangle if it changed. */

			if (!EqualRect(&marquee->r, &r))
			{
				marquee->undraw();
				marquee->r = r;
				marquee->draw();
			}
		}
	}
	else
	{
		if (marquee != NULL)
		{                             
		/* Get rid of the marquee. */

			rc.remove_toggle(marquee_toggle);
			marquee->undraw();

			delete marquee;
			marquee = NULL;

			if (!stop_now)
			{
				PBOX bound;

			/* See what objects we need to select. */

				if ((bound.x0 = p.x) > (bound.x1 = select_start.x))
				{
					bound.x0 = bound.x1;
					bound.x1 = p.x;
				}

				if ((bound.y0 = p.y) > (bound.y1 = select_start.y))
				{
					bound.y0 = bound.y1;
					bound.y1 = p.y;
				}

			/*
 			// Create a frame where this box is.
 			*/

				FrameObjectPtr object;
				CPmwDoc *pDoc = GetDocument();
				FRAMEOBJ_CREATE_STRUCT fcs;

				memset(&fcs, 0, sizeof(fcs));
				fcs.bound = bound;

				if ((object = pDoc->create_frame_object(&fcs)) != NULL)
				{
					object->set_panel(pDoc->get_current_panel());

				/* Add us to the document... */

					pDoc->append_object(object);

				/* ...and make us show up. */

					pDoc->add_refresh(&bound);

				/* Make it the currently selected object. */

					pDoc->deselect_all();
					pDoc->select_object(object);
				}
			}
		}

	/* Go back to arrow tool state. */

		set_arrow_tool();
	}
	return TRUE;
}

/*
// The states for the text create mode.
*/

CPmwView::STATE_HANDLER CPmwView::text_create_state[] =
{
	CPmwView::text_create_state_init,
	CPmwView::text_create_state_normal,
	CPmwView::text_create_state_release,
	CPmwView::text_create_state_dragging
};

#define	TEXT_CREATE_STATE_COUNT			(sizeof(text_create_state)/sizeof(text_create_state[0]))

/*
// The text create mode handler.
*/

VOID CPmwView::mode_text_create(EVENT_INFO *info)
{
	SHORT state = edit_state;

	if (state > STATE_BASE_END)
	{
		if (state >= TEXT_CREATE_STATE_BASE)
		{
		/* One of our special states! */
			state -= (TEXT_CREATE_STATE_BASE-STATE_BASE_END);
		}
		else
		{
		/* Not a valid state for here. */
			state = TEXT_CREATE_STATE_COUNT;
		}
	}

	if (state < TEXT_CREATE_STATE_COUNT)
	{
	/* Valid state, process. */
		(this->*text_create_state[state])(info);
	}
}
