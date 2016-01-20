/*
// $Header: /PM8/App/MODEZOOM.CPP 1     3/03/99 6:08p Gbeddow $
//
// Mode zoom routines.
//
*/

#include "system.h"
#include "stdafx.h"
#include "pmw.h"

#include "pmwdoc.h"
#include "pmwview.h"

#include "utils.h"

double CPmwView::ZoomFactor(void)
{
	return ((double)rc.GetScaleNumerator())/((double)rc.GetScaleDenominator());
}

BOOL CPmwView::CanZoomIn(void)
{
	return ZoomFactor() < 10.0;
}

void CPmwView::OnUpdateViewZoomIn(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(CanZoomIn());
}

BOOL CPmwView::CanZoomOut(void)
{
	return ZoomFactor() > 0.10;
}

void CPmwView::OnUpdateViewZoomOut(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(CanZoomOut());
}

void CPmwView::OnViewZoomIn()
{
	ZoomCommand(VIEW_ZOOM_IN);
}

void CPmwView::OnViewZoomOut()
{
	ZoomCommand(VIEW_ZOOM_OUT);
}

void CPmwView::SetZoomCursor(EVENT_INFO *info)
{
	CRect crWindow;
	GetWindowRect(&crWindow);
	
	CPoint cpCursor;
	GetCursorPos(&cpCursor);
	
	if ((info == NULL) || (crWindow.PtInRect(cpCursor)))
	{
		if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) == 0)
		{
			set_cursor(AfxGetApp()->LoadCursor(CanZoomIn() ? IDC_ZOOMIN_CURSOR : IDC_ZOOM_CURSOR));
		}
		else
		{
			set_cursor(AfxGetApp()->LoadCursor(CanZoomOut() ? IDC_ZOOMOUT_CURSOR : IDC_ZOOM_CURSOR));
		}
	}
}

/*
// Normal state.
*/

BOOL CPmwView::zoom_state_normal(EVENT_INFO *info)
{
	SetZoomCursor(info);
	if (check_escape_event(info))
	{
		if (!m_fZooming)
		{
			set_arrow_tool();
		}
	}
	else if (info->flags & MK_LBUTTON)
	{
		rc.screen_to_ppnt(info->current_xy, &select_start);
		edit_state = ZOOM_STATE_DRAGGING;
	}
	return TRUE;
}

/*
// Initialization state.
*/

BOOL CPmwView::zoom_state_init(EVENT_INFO *info)
{
	edit_state = STATE_NORMAL;

	return zoom_state_normal(info);			/* Do it now, too. */
}

/*
// Release state.
// Wait for the user to release the mouse button.
*/

BOOL CPmwView::zoom_state_release(EVENT_INFO *info)
{
	SetZoomCursor(info);
	
	if (!(info->flags & MK_LBUTTON))
	{
	/* Done waiting! */
		if (!m_fZooming)
		{
			set_arrow_tool();
		}
		else
		{
			edit_state = STATE_NORMAL;
		}
	}
	return TRUE;
}

BOOL CPmwView::zoom_state_dragging(EVENT_INFO *info)
{
	SetZoomCursor(info);
	
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
				int tmp = r.left;
				r.left = r.right;
				r.right = tmp;
			}                                            

			if (r.top > r.bottom)
			{
				int tmp = r.top;
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
		int nMarqueeXSize = 0;
		int nMarqueeYSize = 0;
		
		if (marquee != NULL)
		{                             
			nMarqueeXSize = marquee->r.right-marquee->r.left;
			nMarqueeYSize = marquee->r.bottom-marquee->r.top;
			
			/* Get rid of the marquee. */

			rc.remove_toggle(marquee_toggle);
			marquee->undraw();

			delete marquee;
			marquee = NULL;
		}

		if (!stop_now)
		{
			PBOX bound;
			CPmwDoc *pDoc = GetDocument();

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
			
			if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0)
			{
				bound.x0 = bound.x1 = (bound.x0+bound.x1)/2;
				bound.y0 = bound.y1 = (bound.y0+bound.y1)/2;
				change_view(VIEW_ZOOM_OUT, FALSE, &bound);
			}
			else
			{
				if ((nMarqueeXSize <= 6) && (nMarqueeYSize <= 6))
				{
					bound.x0 = bound.x1 = (bound.x0+bound.x1)/2;
					bound.y0 = bound.y1 = (bound.y0+bound.y1)/2;
					change_view(VIEW_ZOOM_IN, FALSE, &bound);
				}
				else
				{
				
					change_view(VIEW_AREA, TRUE, &bound);
				}
			}
		}

	/* Go back to arrow tool state. */

		if (!m_fZooming)
		{
			set_arrow_tool();
		}
		else
		{
			edit_state = STATE_RELEASE;
		}
	}
	return TRUE;
}

void CPmwView::zoom_cleanup(void)
{
	if (marquee != NULL)
	{                             
	/* Get rid of the marquee. */

		rc.remove_toggle(marquee_toggle);
		marquee->undraw();

		delete marquee;
		marquee = NULL;
	}
}

/*
// The states for the zoom mode.
*/

CPmwView::STATE_HANDLER CPmwView::zoom_state[] =
{
	CPmwView::zoom_state_init,
	CPmwView::zoom_state_normal,
	CPmwView::zoom_state_release,
	CPmwView::zoom_state_dragging
};

#define	ZOOM_STATE_COUNT			(sizeof(zoom_state)/sizeof(zoom_state[0]))

/*
// The zoom mode handler.
*/

VOID CPmwView::mode_zoom(EVENT_INFO *info)
{
	if (info->event >= EVENT_ENTERING_MODE)
	{
		// Special mode event.
		return;
	}

	SHORT state = edit_state;

	if (state > STATE_BASE_END)
	{
		if (state >= ZOOM_STATE_BASE)
		{
		/* One of our special states! */
			state -= (ZOOM_STATE_BASE-STATE_BASE_END);
		}
		else
		{
		/* Not a valid state for here. */
			state = ZOOM_STATE_COUNT;
		}
	}

	if (state < ZOOM_STATE_COUNT)
	{
	/* Valid state, process. */
		(this->*zoom_state[state])(info);
	}
}
