/*
// $Header: /PM8/App/LINEOBJ.CPP 1     3/03/99 6:07p Gbeddow $
//
// Line object routines.
//
// $Log: /PM8/App/LINEOBJ.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
// 7     8/25/98 2:46p Johno
// 
// 6     8/25/98 2:35p Jayn
// Fix for flipped lines when grouped.
// 
// 5     8/25/98 1:28p Johno
// Change to LineObject::group_calc()
// 
// 4     7/24/98 11:59a Rlovejoy
// Keep track of rotated line flags in a group.
// 
// 3     7/16/98 3:09p Rlovejoy
// Added virtual group_cal to rotate a line in a group.
// 
// 2     4/30/98 4:47p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
//    Rev 1.0   14 Aug 1997 15:22:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:04   Fred
// Initial revision.
// 
//    Rev 1.11   30 Jul 1997 10:43:44   Jay
// Increased refresh bounds for line. More flow frame cases.
// 
//    Rev 1.10   30 Jun 1997 12:56:52   Jay
// Fixed line hit test.
// 
//    Rev 1.9   13 Jun 1997 17:21:02   Jay
// pt_in_object check now makes sure point is within bound.
// 
//    Rev 1.8   23 May 1997 17:05:42   Fred
// New effects
// 
//    Rev 1.7   21 Feb 1997 15:08:58   Jay
// New mode handling; crop mode support
// 
//    Rev 1.6   24 Jan 1997 14:55:34   Jay
// Maps width of 1 to hairline case (0).
// 
//    Rev 1.5   24 Jan 1997 12:39:10   Jay
// Initial line palette; toggle tweaks, etc.
// 
//    Rev 1.4   07 Nov 1996 16:20:42   johno
// Moved strings to resource DLL
// 
//    Rev 1.3   15 Jul 1996 11:34:28   Jay
//  
// 
//    Rev 1.2   12 Jul 1996 16:47:54   Jay
// Text wrap
// 
//    Rev 1.1   08 Jul 1996 08:49:06   Jay
// Bigger handles. Better pt_in_object
// 
//    Rev 1.0   14 Mar 1996 13:43:46   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:28   FRED
// Initial revision.
// 
//    Rev 1.3   20 Aug 1995 15:38:18   FRED
// Rotate object control
// 
//    Rev 1.2   01 Aug 1995 12:59:40   JAY
// Added correct name.
// 
//    Rev 1.1   10 May 1995 13:13:54   JAY
// Now it's really a line.
// 
//    Rev 1.0   09 May 1995 08:07:04   JAY
// Initial revision.
*/

#include "stdafx.h"

#include "pmw.h"
#include "pmgobj.h"

#include "lineobj.h"

#include "outpath.h"
#include "utils.h"

#define CTRL_KEY(key)     ((key) & MK_CONTROL)
#define SHIFT_KEY(key)   ((key) & MK_SHIFT)
#define SPECIAL_KEY(key)	((key) & (MK_CONTROL|MK_SHIFT))

/////////////////////////////////////////////////////////////////////////////
// LineObject

/*
// The creator.
// This is a static function which is registered with the database.
*/

ERRORCODE LineObject::create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *record)
{
	LineObject* object = NULL;
	ERRORCODE error = ERRORCODE_None;

/* Create the frame object. */

	if ((object = new LineObject(type, owner)) == NULL)
	{
		error = ERRORCODE_Memory;
	}
	else
	{
	/* The object was created. Handle any creation data. */
	}

	*record = (PageObjectPtr)object;
	return error;
}

/*
// The constructor.
*/

LineObject::LineObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner)
		: DrawingObject(type, owner)
{
	memset(&m_LineRecord, 0, sizeof(m_LineRecord));
	record.select_flags &= ~SELECT_FLAG_rotate_handle;
}

/*
// The destructor for the Warp object.
*/

LineObject::~LineObject(void)
{
}

/*
// Toggle select marks for an object.
*/

BOOL LineObject::toggle_selects(RedisplayContextPtr rc, FLAGS which)
{
	HDC hdc = rc->destination_hdc;

	if (which == 0)
	{
		which = record.select_flags;
	}

	PBOX ObjectBounds = get_unrotated_bound();
	rc->pbox_to_screen(&ObjectBounds, FALSE);

	CRect crObject;
	rc->convert_pbox(&ObjectBounds, crObject, NULL);

	POINT p[2];

	p[0].x = crObject.left;
	p[0].y = crObject.top;
	p[1].x = crObject.right;
	p[1].y = crObject.bottom;

	if (record.flags & OBJECT_FLAG_xflipped)
	{
		int t = p[0].x;
		p[0].x = p[1].x;
		p[1].x = t;
	}

	if (record.flags & OBJECT_FLAG_yflipped)
	{
		int t = p[0].y;
		p[0].y = p[1].y;
		p[1].y = t;
	}

	if (which & SELECT_FLAG_size_handles)
	{
		ControlDot(hdc, p[0].x, p[0].y);
		ControlDot(hdc, p[1].x, p[1].y);
	}

	if (which & SELECT_FLAG_boundary)
	{
		// If we are moving/resizing the line and the position is not the
		// start position, then we want to toggle.
		if ((get_refresh_flags() & (REFRESH_FLAG_moving|REFRESH_FLAG_sizing))
			 && (memcmp(&bound_save, &rrecord.unrotated_bound, sizeof(PBOX)) != 0
				  || angle_save != get_rotation()))
		{
			xor_polyline(hdc, p, 2, FALSE);
		}
	}

	return TRUE;
}

VOID LineObject::group_calc(PBOX_PTR current_group_bound,
                              PBOX_PTR org_group_bound,
                              FLAGS group_flags,
                              ROTATION_COMPONENT_PTR group_rotc)
{
	// Call the base class
	INHERITED::group_calc(current_group_bound, org_group_bound, group_flags, group_rotc);

	// If a rotation has been set, rotate the bounds
//	PBOX Bound = rrecord.group_bound;
	// Works for sizing
	PBOX Bound = rrecord.unrotated_bound;
	PPNT Origin = get_origin();

	long lX0 = MakeFixed(Bound.x0);
	long lY0 = MakeFixed(Bound.y0);
	long lX1 = MakeFixed(Bound.x1);
	long lY1 = MakeFixed(Bound.y1);

#if 0
	TRACE("IN - rot: %f, xflip: %d, yflip: %d\n",
			get_rotation(),
			get_flags() & OBJECT_FLAG_xflipped,
			get_flags() & OBJECT_FLAG_yflipped);

	TRACE("     group rot: %f, xflip: %d, yflip: %d\n",
			group_rotc->rotation,
			group_flags & OBJECT_FLAG_xflipped,
			group_flags & OBJECT_FLAG_yflipped);
#endif

   CFixedPoint DeltaHalf;
	DeltaHalf.x = (lX1 - lX0)/2;
	DeltaHalf.y = (lY1 - lY0)/2;

	ANGLE rot = group_rotc->rotation * RAD2DEGC;

	COutlinePathMatrix RotateMatrix;
	RotateMatrix.Identity();
	RotateMatrix.TranslateBy(	// center of line at 0,0
		-(lX0 + DeltaHalf.x),
		-(lY0 + DeltaHalf.y));
	RotateMatrix.RotateBy(MakeFixed(rot));
	RotateMatrix.TranslateBy(
		MakeFixed(Origin.x),
		MakeFixed(Origin.y));

	// Rotate the line by the given rotation
	COutlinePath LinePath;

	if ((rrecord.flags ^ group_flags) & OBJECT_FLAG_xflipped)
	{
		long t = lX0;
		lX0 = lX1;
		lX1 = t;
	}
	if ((rrecord.flags ^ group_flags) & OBJECT_FLAG_yflipped)
	{
		long t = lY0;
		lY0 = lY1;
		lY1 = t;
	}
	LinePath.MoveTo(lX0, lY0);
	LinePath.LineTo(lX1, lY1);
	LinePath.Close();
	LinePath.End();

	LinePath.Transform(&RotateMatrix);

	long lXU0 = *(long*)LinePath.GetElement(1);
	long lYU0 = *(long*)LinePath.GetElement(2);
	long lXU1 = *(long*)LinePath.GetElement(4);
	long lYU1 = *(long*)LinePath.GetElement(5);

	remove_flags(OBJECT_FLAG_xflipped | OBJECT_FLAG_yflipped);
	if (lXU1 < lXU0)
	{
		long t = lXU1;
		lXU1 = lXU0;
		lXU0 = t;
		add_flags(OBJECT_FLAG_xflipped);
	}
	if (lYU1 < lYU0)
	{
		long t = lYU1;
		lYU1 = lYU0;
		lYU0 = t;
		add_flags(OBJECT_FLAG_yflipped);
	}

	LinePath.Free();

	// Set the bounds back
	Bound.x0 = FixedInteger(lXU0);
	Bound.x1 = FixedInteger(lXU1);
	Bound.y0 = FixedInteger(lYU0);
	Bound.y1 = FixedInteger(lYU1);
	set_rotation(0.0);
	rrecord.unrotated_bound = Bound;

#if 0
	TRACE("OUT - rot: %f, xflip: %d, yflip: %d\n",
			get_rotation(),
			get_flags() & OBJECT_FLAG_xflipped,
			get_flags() & OBJECT_FLAG_yflipped);
#endif

	calc();
}

/*
// Update.
*/

UpdateStatePtr LineObject::update(RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip, UpdateStatePtr ustate, REFRESH_TYPE refresh_type)
{
	HDC hdc = rc->destination_hdc;
	CDC* pDC = CDC::FromHandle(hdc);


	// Get the unrotated bounds of the object.

	PBOX ObjectBounds = get_unrotated_bound();
	rc->pbox_to_screen(&ObjectBounds, FALSE);

	CRect crObject;
	rc->convert_pbox(&ObjectBounds, crObject, NULL);

	long lX0 = MakeFixed(crObject.left);
	long lY0 = MakeFixed(crObject.top);
	long lX1 = MakeFixed(crObject.left+crObject.Width());
	long lY1 = MakeFixed(crObject.top+crObject.Height());

	if (record.flags & OBJECT_FLAG_xflipped)
	{
		long t = lX0;
		lX0 = lX1;
		lX1 = t;
	}

	if (record.flags & OBJECT_FLAG_yflipped)
	{
		long t = lY0;
		lY0 = lY1;
		lY1 = t;
	}

	CFixedPoint Delta;

	Delta.x = lX1 - lX0;
	Delta.y = lY1 - lY0;

	CFixed lTheta = ArcTangent2(Delta.y, Delta.x);

	COutlinePathMatrix RotateMatrix;
	RotateMatrix.Identity();
	RotateMatrix.RotateBy(lTheta);

	// Create an unrotated line so we can know where to start.

	COutlinePath LinePath;

	LinePath.MoveTo(0, 0);
	LinePath.LineTo(Delta.x, Delta.y);
	LinePath.Close();
	LinePath.End();

	LinePath.Transform(&RotateMatrix);

	long lXU0 = *(long*)LinePath.GetElement(1);
	long lYU0 = *(long*)LinePath.GetElement(2);
	long lXU1 = *(long*)LinePath.GetElement(4);
	long lYU1 = *(long*)LinePath.GetElement(5);

	LinePath.Free();

	TRACE("Unrotated line: (%08lx, %08lx), (%08lx, %08lx)\r\n",
				lXU0, lYU0, lXU1, lYU1);

	ASSERT(lXU0 == 0);
	ASSERT(lYU0 == 0);
//	ASSERT(lYU1 == 0);

	PCOORD lX, lY;
	GetOutlineWidth(&lX, &lY);

	int nX = rc->page_x_to_screen(lX)-rc->page_x_to_screen(0);
	int nY = rc->page_y_to_screen(lY)-rc->page_y_to_screen(0);

	int nWidth = nX;
	if (nY > nWidth)
	{
		nWidth = nY;
	}

	CFixed lWidth = MakeFixed(nWidth)/2;

	COutlinePath Path;

	if (lXU1 < lXU0)
	{
		long t = lXU1;
		lXU1 = lXU0;
		lXU0 = t;

		t = lYU1;
		lYU1 = lYU0;
		lYU0 = lYU1;
	}
	Path.MoveTo(lXU0, lYU0-lWidth);
	Path.LineTo(lXU1, lYU1-lWidth);
	Path.LineTo(lXU1, lYU1+lWidth);
	Path.LineTo(lXU0, lYU0+lWidth);
	Path.LineTo(lXU0, lYU0-lWidth);
	Path.Close();
	Path.End();

	// Build a matrix for rotating the object.

	RotateMatrix.Identity();
	RotateMatrix.RotateBy(-lTheta);
	RotateMatrix.TranslateBy(lX0, lY0);

	Path.Transform(&RotateMatrix);

	int nHalfWidth = (nWidth+1)/2;
	if (nHalfWidth == 0)
	{
		nHalfWidth++;
	}

	crObject.left -= nHalfWidth;
	crObject.top -= nHalfWidth;
	crObject.right += nHalfWidth;
	crObject.bottom += nHalfWidth;

	rc->toggle(FALSE, crObject);
	if (nWidth <= 1)
	{
	/* Draw a hairline (or very thin line). */
		Path.Draw2(
			pDC,
			0,
			FALSE,
			nWidth,
			colorref_from_color(m_Outline.m_ForegroundColor),
			m_Outline.IsVisible(),
			WINDING,
			rc->info.type == RASTER_TYPE_PALETTE);
	}
	else
	{
		Path.Draw2(
			pDC,
			colorref_from_color(m_Outline.m_ForegroundColor),
			m_Outline.IsVisible(),
			-1,
			0,
			FALSE,
			WINDING,
			rc->info.type == RASTER_TYPE_PALETTE);
	}
	rc->toggle(TRUE, crObject);

	return NULL;
}

/*
// Return whether a point is in an object or not.
*/

BOOL LineObject::pt_in_object(PPNT p, RedisplayContextPtr rc, PMGPageObjectPtr far *sub_object)
{
	if (sub_object != NULL)
	{
		*sub_object = this;
	}

/*
// Use the distance of point from line gig.
//
//     | Ax + By + C |
// d = ---------------
//     sqrt(A*A + B*B)
*/

	PBOX Bounds = get_unrotated_bound();

	if (Bounds.x0 == Bounds.x1
			&& Bounds.y0 == Bounds.y1)
	{
		/* Degenerate case. Try to be helpful - say we're on it. */
		return TRUE;
	}

	PCOORD lX, lY;
	GetOutlineWidth(&lX, &lY);

	PCOORD lWidth = lX;
	if (lY > lWidth)
	{
		lWidth = lY;
	}

	PCOORD HalfThick = (lWidth/2) + (PAGE_RESOLUTION/72);

	if (rc != NULL)
	{
		// Make sure this is big enough to grab.
		int x0 = rc->page_x_to_screen(0);
		int x1 = rc->page_x_to_screen(HalfThick);
		if (x1 - x0 < 4)
		{
			HalfThick = rc->screen_x_to_page(x0+4);
		}
	}

	if (p.x < Bounds.x0-HalfThick
		 || p.x > Bounds.x1+HalfThick
		 || p.y < Bounds.y0-HalfThick
		 || p.y > Bounds.y1+HalfThick)
	{
		// Not even close.
		return FALSE;
	}

	if (record.flags & OBJECT_FLAG_xflipped)
	{
		long t = Bounds.x0;
		Bounds.x0 = Bounds.x1;
		Bounds.x1 = t;
	}

	if (record.flags & OBJECT_FLAG_yflipped)
	{
		long t = Bounds.y0;
		Bounds.y0 = Bounds.y1;
		Bounds.y1 = t;
	}

/*
// In order to try and keep these numbers as small as possible, we will
// normalize vs. p0 before starting.
*/

	p.x -= Bounds.x0;
	p.y -= Bounds.y0;
	Bounds.x1 -= Bounds.x0;
	Bounds.y1 -= Bounds.y0;
//	Bounds.x0 = 0;
//	Bounds.y0 = 0;

	PCOORD A = -(Bounds.y1 /*-Bounds.y0*/);
	PCOORD B = Bounds.x1 /*-Bounds.x0*/;
	ASSERT(A != 0 || B != 0);

//	PCOORD C = Bounds.x0*Bounds.y1 - Bounds.x1*Bounds.x0;

// We switch to floating point here because the numbers can get very large.

	double dHalfThick = (double)HalfThick;
	double dHalfThickSquared = dHalfThick*dHalfThick;

	double dA = (double)A;
	double dB = (double)B;
	double dDistance = dA*(double)p.x + dB*(double)p.y;

	double dDistanceSquared = (dDistance*dDistance)/(dA*dA + dB*dB);

	return (dDistanceSquared <= dHalfThickSquared);
}

/*
// Return whether a box is over a handle or not.
*/

OBJECT_HANDLE LineObject::box_on_handle(PBOX_PTR box, RedisplayContextPtr rc)
{
	PBOX Bounds = get_unrotated_bound();

	if (record.flags & OBJECT_FLAG_xflipped)
	{
		long t = Bounds.x0;
		Bounds.x0 = Bounds.x1;
		Bounds.x1 = t;
	}

	if (record.flags & OBJECT_FLAG_yflipped)
	{
		long t = Bounds.y0;
		Bounds.y0 = Bounds.y1;
		Bounds.y1 = t;
	}

	BOOL fSwapped = FALSE;

	if (IsPtInBox(Bounds.x0, Bounds.y0, box))
	{
	/* On p0. Continue with state as is. */
	}

	else if (IsPtInBox(Bounds.x1, Bounds.y1, box))
	{
		fSwapped = TRUE;

	/* On p1. Switch points and continue. */
		PCOORD t;

		t = Bounds.x0;
		Bounds.x0 = Bounds.x1;
		Bounds.x1 = t;

		t = Bounds.y0;
		Bounds.y0 = Bounds.y1;
		Bounds.y1 = t;
	}
	else
	{
	/* No dice, bucko. */
		return OBJECT_HANDLE_NONE;
	}

/*
// Turn equal cases into non-equal cases.
*/

	int nWhich = (Bounds.x0 <= Bounds.x1) | ((Bounds.y0 <= Bounds.y1) << 1);

	if (Bounds.x0 == Bounds.x1)
	{
		if (record.flags & OBJECT_FLAG_xflipped)
		{
			nWhich ^= 1;
		}
		if (fSwapped)
		{
			nWhich ^= 1;
		}
	}

	if (Bounds.y0 == Bounds.y1)
	{
		if (record.flags & OBJECT_FLAG_yflipped)
		{
			nWhich ^= 2;
		}
		if (fSwapped)
		{
			nWhich ^= 2;
		}
	}

	static OBJECT_HANDLE ThemHandles[] =
	{
		OBJECT_HANDLE_LR,			// x0 > x1, y0 > y1
		OBJECT_HANDLE_LL,			// x0 < x1, y0 > y1
		OBJECT_HANDLE_UR,			// x0 > x1, y0 < y1
		OBJECT_HANDLE_UL			// x0 < x1, y0 < y1
	};

	return ThemHandles[nWhich];
}

void LineObject::calc_bounds(PCOORD_PTR the_corner_x, PCOORD_PTR the_corner_y, PCOORD_PTR opp_corner_x, PCOORD_PTR opp_corner_y, PPNT pos, FLAGS key)
{
	if (SPECIAL_KEY(key))
	{
		if (SHIFT_KEY(key))
		{
		/*
		// Compute absolute deltas and their signs.
		*/

			PCOORD lDX = pos.x - *opp_corner_x;
			PCOORD lDY = pos.y - *opp_corner_y;

			BOOL fNegX = (lDX < 0);
			if (fNegX)
			{
				lDX = -lDX;
			}
			BOOL fNegY = (lDY < 0);
			if (fNegY)
			{
				lDY = -lDY;
			}

		/*
		// We use angles where the tangent is easy:
		// if tan(theta) < .5, we go across.
		// if tan(theta) > 2 we go up.
		// else we go diagonal.
		//
		// This isn't strictly equal for the angles (it's off by ~3.5 degrees
		// on either side of the diagonal range), but it's close enough.
		//
		// Ideal diagonal range:
		// 30 degrees < theta < 60 degrees
		//
		// Our range:
		// 26.56 degrees < theta < 63.43
		*/

			if (lDY < lDX/2)
			{
			/* Straight across! */
				lDY = 0;
			}
			else if (lDX < lDY/2)
			{
			/* Straight up! */
				lDX = 0;
			}
			else
			{
			/* 45 degree line. */
				if (lDY < lDX)
				{
					lDX = lDY;
				}
				else
				{
					lDY = lDX;
				}
			}

			if (fNegX)
			{
				lDX = -lDX;
			}
			if (fNegY)
			{
				lDY = -lDY;
			}

			*the_corner_x = pos.x = *opp_corner_x + lDX;
			*the_corner_y = pos.y = *opp_corner_y + lDY;
			return;
		}
	}
	INHERITED::calc_bounds(the_corner_x, the_corner_y, opp_corner_x, opp_corner_y, pos, key);
}

/*
// Move an object's handle.
*/

PMG_OBJECT_ACTION LineObject::move_handle(DRAG_STATE state, PPNT pos, OBJECT_HANDLE handle, FLAGS shift_status, LPVOID far *data /*=NULL*/, BOOL fCropping /*=FALSE*/)
{
	switch (state)
	{
		case DRAG_STATE_continue:
		{
		/* Do our thing! */
		/* Nothing special to do at this time. */
//			return ACTION_DO_NOTHING;
		}
		default:
		{
			return INHERITED::move_handle(state, pos, handle, shift_status, data, fCropping);
		}
	}
}

/*
// Return the refresh bounds for this object.
*/

void LineObject::get_refresh_bound(PBOX_PTR refresh_bound, RedisplayContextPtr rc /*=NULL*/)
{
	// Get a rough idea of the refresh bounds.

	PCOORD lX, lY;
	GetOutlineWidth(&lX, &lY);

	PCOORD lWidth = lX;
	if (lY > lWidth)
	{
		lWidth = lY;
	}

	PCOORD HalfThick = (lWidth/2) + (PAGE_RESOLUTION/72);
   if (rc != NULL)
   {
      HalfThick += (rc->redisplay_x_pixel+rc->redisplay_y_pixel)/2;
   }
	PBOX ObjectBounds = get_unrotated_bound();

	refresh_bound->x0 = ObjectBounds.x0 - HalfThick;
	refresh_bound->y0 = ObjectBounds.y0 - HalfThick;
	refresh_bound->x1 = ObjectBounds.x1 + HalfThick;
	refresh_bound->y1 = ObjectBounds.y1 + HalfThick;
}

/*
// read_data()
//
// This is a method invoked by the read method to handle object-specific data.
*/

ERRORCODE LineObject::read_data(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = INHERITED::read_data(device)) == ERRORCODE_None)
	{
		error = device->read_record(&m_LineRecord, sizeof(m_LineRecord));
		record.select_flags &= ~SELECT_FLAG_rotate_handle;
	}
	return error;
}

/*
// write_data()
//
// This is a method invoked by the write method to handle object-specific data.
*/

ERRORCODE LineObject::write_data(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = INHERITED::write_data(device)) == ERRORCODE_None)
	{
		error = device->write_record(&m_LineRecord, sizeof(m_LineRecord));
	}
	return error;
}

/*
// size_data()
//
// This is a method invoked by the size method to handle object-specific data.
*/

ST_MAN_SIZE LineObject::size_data(StorageDevicePtr device)
{
	return INHERITED::size_data(device) + device->size_record(sizeof(m_LineRecord));
}

/*
// Get an object's name.
*/

BOOL LineObject::get_name(LPSTR buffer, SHORT size, SHORT max_width)
{
   strcpy(buffer, GET_PMWAPP()->GetResourceStringPointer(IDS_LINE));
	return TRUE;
}

/*
// Assign an object to this.
*/

ERRORCODE LineObject::assign(PageObjectRef pobject)
{
	ERRORCODE error;
	LineObject& sobject = (LineObject&)pobject;

	if ((error = INHERITED::assign(pobject)) == ERRORCODE_None)
	{
		m_LineRecord = sobject.m_LineRecord;
	}
	return error;
}

PageObjectState* LineObject::CreateState(void)
{
	return new LineObjectState;
}

void LineObject::SaveState(PageObjectState* pState)
{
	INHERITED::SaveState(pState);

	((LineObjectState*)pState)->m_LineRecord = m_LineRecord;
}

void LineObject::RestoreState(PageObjectState* pState)
{
	INHERITED::RestoreState(pState);

	m_LineRecord = ((LineObjectState*)pState)->m_LineRecord;
}

/*
// Add the outline for this object to the passed array.
//
// If pClipBox is not NULL, it is assumed that an intersect test has
// already been performed with this box against the object bound.
*/

void LineObject::AddOutline(COutlinePath& Path, PBOX* pClipBox /*=NULL*/)
{
	// Do nothing. Lines do not intersect.
}

/*
// How does text flow around us?
*/

long LineObject::GetFlowAround(void)
{
	return FLOW_AROUND_none;
}
