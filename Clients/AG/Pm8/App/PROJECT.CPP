/*
// $Header: /PM8/App/PROJECT.CPP 1     3/03/99 6:11p Gbeddow $
//
// Implementation of project info object.
//
// $Log: /PM8/App/PROJECT.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:10   Fred
// Initial revision.
// 
//    Rev 1.1   01 Nov 1996 17:11:48   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.0   14 Mar 1996 13:46:08   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:18   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:04   JAY
// Initial revision.
// 
//    Rev 1.3   30 Nov 1994 16:34:28   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.2   06 Oct 1994 13:35:24   JAY
// get_panel_world() now also take a margin rect pointer.
// 
//    Rev 1.1   05 Jan 1994 08:07:02   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:32   JAY
// 
*/

#include "stdafx.h"
#include "pagedefs.h"
#include "project.h"
#include "utils.h"

/*
// Get the world of a panel. The dimensions of the document are passed.
*/

VOID ProjectInfo::get_panel_world(PBOX_PTR world,
											 PCOORD xsize,
											 PCOORD ysize,
											 SHORT panel,
											 LPCRECT margins /*=NULL*/)
											 
{
	RECT r;

	if (margins == NULL)
	{
		r.left = r.right =
			r.top = r.bottom = PAGE_RESOLUTION/4;
	}
	else
	{
		r = *margins;
	}

	PANEL_INFO *this_pip = pip + panel;

	world->x0 = scale_pcoord(this_pip->panel_pos.left, xsize, 1000)
						+ MulDiv(this_pip->panel_margin.left,
								r.left, 1000);
	world->y0 = scale_pcoord(this_pip->panel_pos.top, ysize, 1000)
						+ MulDiv(this_pip->panel_margin.top,
								r.top, 1000);
	world->x1 = scale_pcoord(this_pip->panel_pos.right, xsize, 1000)
						- MulDiv(this_pip->panel_margin.right,
								r.right, 1000);
	world->y1 = scale_pcoord(this_pip->panel_pos.bottom, ysize, 1000)
						- MulDiv(this_pip->panel_margin.bottom,
								r.bottom, 1000);
}

