/*
// $Header: /PM8/App/BACKOBJ.CPP 1     3/03/99 6:03p Gbeddow $
//
// Background object routines.
//
// $Log: /PM8/App/BACKOBJ.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:08   Fred
// Initial revision.
// 
//    Rev 1.2   10 Dec 1996 11:38:32   Jay
// Now preserves the file position while doing its housekeeping in read_data.
// 
//    Rev 1.1   23 Jul 1996 13:14:00   Jay
// Backgrounds phased out -> convert to graphics
// 
//    Rev 1.0   14 Mar 1996 13:48:34   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:40   FRED
// Initial revision.
// 
//    Rev 2.1   14 Sep 1995 09:32:22   JAY
// 32-bit changes
// 
//    Rev 2.0   07 Feb 1995 15:47:30   JAY
// Initial revision.
// 
//    Rev 1.7   30 Nov 1994 16:34:08   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.6   01 Jun 1994 16:35:34   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.5   10 Mar 1994 09:53:58   JAY
// Does not do (unnecessary) WHITENESS erase if printing.
// 
//    Rev 1.4   17 Feb 1994 09:25:14   JAY
// Added a RedisplayContext argument to get_refresh_bound() && is_opaque().
// 
//    Rev 1.3   11 Feb 1994 08:07:38   JAY
// Now does the clear only on the outside of the background image.
// 
//    Rev 1.2   13 Jan 1994 17:00:28   JAY
//  
// 
//    Rev 1.1   05 Jan 1994 08:05:54   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:14   JAY
// 
*/

#include "stdafx.h"

#include "pmgobj.h"
#include "utils.h"

#include "grafrec.h"
#include "grafobj.h"
#include "backobj.h"

#include "cgm.h"

/*****************************************************************************/
/*                              BackgroundObject                             */
/*****************************************************************************/

/*
// The creator for a background object.
*/

ERRORCODE BackgroundObject::create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *record)
{
	BackgroundObjectPtr object;
	ERRORCODE error;

/* Create the background object. */

	if ((object = new BackgroundObject(type, owner)) == NULL)
	{
		error = ERRORCODE_Memory;
	}
	else
	{
	/* We created the object. */
		if ((error = object->handle_creation_data(creation_data)) != ERRORCODE_None)
		{
			delete object;
			object = NULL;
		}
	}

	*record = (PageObjectPtr)object;
	return error;
}

/*
// The constructor for the background object.
*/

BackgroundObject::BackgroundObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner)
		: GraphicObject(type, owner)
{
}

BOOL BackgroundObject::shrink_to_fit()
{
	BOOL shrunk = FALSE;

/*
// Get the graphic record.
*/

	GraphicPtr graphic;
	ERRORCODE error;
	PBOX org_bound = get_bound();

	if ((graphic = (GraphicPtr)database->get_record(get_graphic_record(), &error, RECORD_TYPE_Graphic)) != NULL)
	{
		if (graphic->record.type == GRAPHIC_TYPE_CGM)
		{
			CGMHelperPtr helper = (CGMHelperPtr)graphic->my_little_helper();
			USHORT dx, dy;
			PCOORD bdx, bdy;

			CGM_POINT ext_min = helper->record.extended_min;
			CGM_POINT ext_max = helper->record.extended_max;
			CGM_POINT min = helper->record.metafile_min;
			CGM_POINT max = helper->record.metafile_max;

			dx = ext_max.x - ext_min.x;
			dy = ext_max.y - ext_min.y;

			bdx = org_bound.x1 - org_bound.x0;
			bdy = org_bound.y1 - org_bound.y0;

			if (dx != 0 && dy != 0)
			{
			/* Renormalize for extended space. */

				max.x -= (ext_min.x + min.x);
				min.x = -ext_min.x;

				max.y -= (ext_min.y + min.y);
				min.y = -ext_min.y;

			/* The extended min and max are normalized by the metafile min. */

				PBOX bound;

				bound.x1 = org_bound.x0 + scale_pcoord(max.x, bdx, dx);
				bound.x0 = org_bound.x0 + scale_pcoord(min.x, bdx, dx);
				bound.y1 = org_bound.y0 + scale_pcoord(max.y, bdy, dy);
				bound.y0 = org_bound.y0 + scale_pcoord(min.y, bdy, dy);

				set_bound(bound);
				shrunk = TRUE;
			}
		}

		graphic->release();
	}
	return shrunk;
}

/*
// read_data()
//
// This is a method invoked by the read method to handle object-specific data.
*/

ERRORCODE BackgroundObject::read_data(StorageDevicePtr device)
{
	ERRORCODE error = GraphicObject::read_data(device);
	if (error == ERRORCODE_None)
	{
		// Convert to a graphic.
		record.select_flags = SELECT_FLAG_boundary
 									| SELECT_FLAG_size_handles
									| SELECT_FLAG_move_handle
									| SELECT_FLAG_rotate_handle;
		remove_flags(OBJECT_FLAG_no_mask | OBJECT_FLAG_landscape);
		remove_refresh_flags(REFRESH_FLAG_opaque);

		ST_DEV_POSITION pos;
		device->tell(&pos);

		UpdateRotateHandle();
		shrink_to_fit();

		device->seek(pos, ST_DEV_SEEK_SET);

		my_type = OBJECT_TYPE_Graphic;
	}
	return error;
}

/*
// write_data()
//
// This is a method invoked by the write method to handle object-specific data.
*/

ERRORCODE BackgroundObject::write_data(StorageDevicePtr device)
{
	ASSERT(my_type == OBJECT_TYPE_Graphic);
	return GraphicObject::write_data(device);
}

/*
// size_data()
//
// This is a method invoked by the size method to handle object-specific data.
*/

ST_MAN_SIZE BackgroundObject::size_data(StorageDevicePtr device)
{
	ASSERT(my_type == OBJECT_TYPE_Graphic);
	return GraphicObject::size_data(device);
}
