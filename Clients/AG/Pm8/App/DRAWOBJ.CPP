/*
// $Header: /PM8/App/DRAWOBJ.CPP 1     3/03/99 6:05p Gbeddow $
//
// Drawing object routines.
//
// $Log: /PM8/App/DRAWOBJ.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 7     5/19/98 11:06a Johno
// Added rc->IsScreen() in DrawingObject::update(): "Normal" drawing is
// used for invisible hot spots if the output is the printer.
// 
// 6     5/14/98 6:19p Johno
// Added special drawing code to DrawingObject::update() for invisible hot
// spots
// 
// 5     5/14/98 10:51a Rlovejoy
// Apply x,y flips to gradient fills.
// 
// 4     5/08/98 12:02p Rlovejoy
// Send rotation to fill object.
// 
// 3     4/30/98 4:47p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
// 2     4/29/98 5:58p Rlovejoy
// Draw with fill objects.
// 
//    Rev 1.0   14 Aug 1997 15:20:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:14   Fred
// Initial revision.
// 
//    Rev 1.16   30 Jul 1997 11:11:20   Jay
// A missed check for convert_pbox().
// 
//    Rev 1.15   28 Jul 1997 14:04:54   Jay
// Fixes for draw objects overflows.
// 
//    Rev 1.14   20 Jul 1997 14:36:40   Fred
// Does not allow handle wrap when cropping; handles empty clip regions
// 
//    Rev 1.13   20 Jul 1997 13:58:20   Jay
// Default is "hairline", not "absolute, 0".
// 
//    Rev 1.12   20 Jun 1997 11:52:14   Fred
// Fixed bug with shadow assignment
// 
//    Rev 1.11   23 May 1997 17:05:18   Fred
// New effects
// 
//    Rev 1.10   04 Mar 1997 17:03:32   Jay
// Fixes for drawing objects, cropping, etc.
// 
//    Rev 1.9   26 Feb 1997 10:57:20   Jay
// Cropping; GetClipRgn() changes
// 
//    Rev 1.8   25 Feb 1997 17:01:14   Jay
// More crop goodies.
// 
//    Rev 1.7   25 Feb 1997 12:57:06   Jay
// Cropping and extensions
// 
//    Rev 1.6   19 Feb 1997 16:54:02   Jay
// Beginning of cropping
// 
//    Rev 1.5   11 Feb 1997 14:54:36   Jay
// Free draw tool
// 
//    Rev 1.4   29 Jan 1997 14:18:16   Jay
// Outline path and PtInPath
// 
//    Rev 1.3   24 Jan 1997 12:39:08   Jay
// Initial line palette; toggle tweaks, etc.
// 
//    Rev 1.2   23 Jan 1997 11:05:30   Jay
// New popup palettes
// 
//    Rev 1.1   17 Jan 1997 17:03:06   Jay
// New select for drawing objects.
// 
//    Rev 1.0   14 Mar 1996 13:44:10   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:50   FRED
// Initial revision.
// 
//    Rev 1.6   28 Aug 1995 10:57:42   FRED
// Fixes for flipped inflates.
// 
//    Rev 1.5   01 Aug 1995 12:59:38   JAY
// Added correct name.
// 
//    Rev 1.4   19 May 1995 09:24:34   FRED
// Added bounds to path record
// 
//    Rev 1.3   17 May 1995 16:58:14   FRED
// Draw object shadows
// 
//    Rev 1.2   17 May 1995 15:46:42   JAY
// 
//    Rev 1.1   13 May 1995 11:14:22   JAY
//  
// 
//    Rev 1.0   04 May 1995 17:15:50   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmgdb.h"
#include "utils.h"
#include "outpath.h"
#include "pathrec.h"

#include "drawobj.h"

/////////////////////////////////////////////////////////////////////////////
// DrawingObject

/*
// The creator.
// This is a static function which is registered with the database.
*/

ERRORCODE DrawingObject::create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, void* creation_data, PageObjectPtr far *record)
{
	DrawingObject* object = NULL;
	ERRORCODE error = ERRORCODE_None;

/* Create the object. */

	if ((object = new DrawingObject(type, owner)) == NULL)
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

DrawingObject::DrawingObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner)
		: RectPageObject(type, owner)
{
	memset(&m_DRecord, 0, sizeof(m_DRecord));

	record.select_flags = SELECT_FLAG_boundary
 									| SELECT_FLAG_size_handles
									| SELECT_FLAG_move_handle
									| SELECT_FLAG_rotate_handle
									;

	m_DRecord.m_nEffectsVersion = 2;

	m_Fill.SetType(FillFormatV2::FillSolid, COLOR_BLACK);
	m_Outline.SetType(OutlineFormat::OutlineHairline);
	m_Outline.m_ForegroundColor = COLOR_BLACK;
	m_Shadow.SetType(ShadowFormat::None);
	m_Shadow.m_ForegroundColor = COLOR_BLACK;
}

/*
// The destructor.
*/

DrawingObject::~DrawingObject(void)
{
}

/*
// Toggle the bounds for the object.
*/

void DrawingObject::ToggleBounds(RedisplayContext* rc, POINT* p)
{
	// Show the bound if panning or cropping.
	if (get_refresh_flags() & (REFRESH_FLAG_panning|REFRESH_FLAG_cropping))
	{
		RectPageObject::ToggleBounds(rc, p);
	}

	HDC hdc = rc->destination_hdc;
	CDC* pDC = CDC::FromHandle(hdc);

	COutlinePath Path;

	// Get the unrotated bounds of the object.

	PBOX ObjectBounds = GetUnrotatedDrawBound();
	rc->pbox_to_screen(&ObjectBounds, TRUE);

	CRect crObject;
	if (rc->convert_pbox(&ObjectBounds, crObject, NULL))
	{

		// Build a matrix for rotating the object.

		BOOL fRotated = get_rotation() != 0.0;
		BOOL fFlipped = (get_flags() & (OBJECT_FLAG_xflipped | OBJECT_FLAG_yflipped)) != 0;
		BOOL fNegateInflate = FALSE;
		COutlinePathMatrix RotateMatrix;

		RotateMatrix.Identity();

		if (fFlipped)
		{
			if (get_flags() & OBJECT_FLAG_xflipped)
			{
				RotateMatrix.XFlip(MakeFixed(crObject.left), MakeFixed(crObject.right));
				fNegateInflate = !fNegateInflate;
			}
			if (get_flags() & OBJECT_FLAG_yflipped)
			{
				RotateMatrix.YFlip(MakeFixed(crObject.top), MakeFixed(crObject.bottom));
				fNegateInflate = !fNegateInflate;
			}
		}

		if (fRotated)
		{
			POINT Origin;
	#if 1
			rc->ppnt_to_screen(get_origin(), &Origin);
	#else
			Origin.x = crObject.left+crObject.Width()/2;
			Origin.y = crObject.top+crObject.Height()/2;
	#endif

			RotateMatrix.TranslateBy(
				-MakeFixed(Origin.x),
				-MakeFixed(Origin.y));
			RotateMatrix.RotateBy(MakeFixed(get_rotation()*RAD2DEGC));
			RotateMatrix.TranslateBy(
				MakeFixed(Origin.x),
				MakeFixed(Origin.y));
		}

		if (GetPath(Path, crObject))
		{
			if (fRotated || fFlipped)
			{
				Path.Transform(&RotateMatrix);
			}

			int nOldPolyFillMode = SetPolyFillMode(hdc, WINDING);
			HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);

			HBRUSH hOldBrush = (HBRUSH)::SelectObject(hdc, hBrush);
			if (hOldBrush != NULL)
			{
				BOOL f = ApplyClipRgn(rc);
				if (f != -1)
				{
					Path.UsePolyline(TRUE);
					Path.Draw(CDC::FromHandle(hdc));
				}
				RestoreClipRgn(rc, f);

				SelectObject(hdc, hOldBrush);
			}

			SetPolyFillMode(hdc, nOldPolyFillMode);
		}
	}
}

/*
// Update.
*/

UpdateStatePtr DrawingObject::update(RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip, UpdateStatePtr ustate, REFRESH_TYPE refresh_type)
{
	HDC hdc = rc->destination_hdc;
	CDC* pDC = CDC::FromHandle(hdc);

	COutlinePath Path;

	// Get the unrotated bounds of the object.

	PBOX ObjectBounds = GetUnrotatedDrawBound();

   if ((IsInvisibleHotSpot() == TRUE) && (rc->IsScreen() == TRUE))
   {
      // Note: This code and comments was stolen from CFrameObject::DrawFrameOutline()
      // with almost no modification. It draws a dashed line around invisible hot spots
      // so they can be seen

      // First, draw the frame rectangle. This is drawn one pixel outside
      // of the content area.

      // Get the unrotated bound and move the upper left corner out one
      // pixel. We don't have to move the lower right corner out because
      // Lineto() is used to draw the frame and it automatically will draw
      // to the right and below the computed coordinates (unlike Rectangle).

      ObjectBounds.x0 -= rc->redisplay_x_pixel;
      ObjectBounds.y0 -= rc->redisplay_y_pixel;

      PBOX RotatedBound;
      PPNT v[4];
      compute_rotated_bound(ObjectBounds, get_rotation(), &RotatedBound, get_origin(), v);
   
      // Convert the PPNT coordinates to screen coordinates in p[]. Along the way
      // we accumulate the bounding rectangle which will be used for the toggle.
      POINT p[4];
      RECT r;
      int i;

      for (i = 0; i < 4; i++)
      {
         // Convert point to screen coordinates.
         p[i].x = rc->page_x_to_screen(v[i].x);
         p[i].y = rc->page_y_to_screen(v[i].y);

         // Accumulate point into bounding rectangle.
         if (i == 0)
         {
            r.left = r.right = p[i].x;
            r.top = r.bottom = p[i].y;
         }
         else
         {
            if (p[i].x < r.left) r.left = p[i].x;
            if (p[i].x > r.right) r.right = p[i].x;
            if (p[i].y < r.top) r.top = p[i].y;
            if (p[i].y > r.bottom) r.bottom = p[i].y;
         }
      }

      // Bump out the lower right corner of the toggle rectangle since the frame
      // will be drawn with LineTo which draws to the right and below the computed
      // coordinates.
      r.right++;
      r.bottom++;

      // Draw the frame using a gray "dotted" pen.
      HPEN hPen = ::CreatePen(PS_DOT, 1, RGB(192, 192, 192));
      if (hPen != 0)
      {
         HDC hDC = rc->destination_hdc;
         
         HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
         if (hOldPen != NULL)
         {
            // Erase any toggles.
            rc->toggle(FALSE, &r);

            // Draw opaque.
            int oldROP2 = SetROP2(hDC, R2_COPYPEN);
            int oldBkMode = SetBkMode(hDC, OPAQUE);

            // Draw the frame.
            ::MoveToEx(hDC, p[0].x, p[0].y, NULL);
            ::LineTo(hDC, p[1].x, p[1].y);
            ::LineTo(hDC, p[2].x, p[2].y);
            ::LineTo(hDC, p[3].x, p[3].y);
            ::LineTo(hDC, p[0].x, p[0].y);

            // Restore old DC settings.
            SetBkMode(hDC, oldBkMode);
            SetROP2(hDC, oldROP2);
            ::SelectObject(hDC, hOldPen);

            // Turn any toggles back on.
            rc->toggle(TRUE, &r);
         }

         ::DeleteObject(hPen);
      }
   }				
   else
   {
	   rc->pbox_to_screen(&ObjectBounds, TRUE);
	   if (ObjectBounds.x1 - ObjectBounds.x0 > 32766
            || ObjectBounds.y1 - ObjectBounds.y0 > 32766)
      {
         // Graphic is too large.
         return NULL;
      }

	   CRect crObject;
	   if (rc->convert_pbox(&ObjectBounds, crObject, NULL))
	   {
		   // Build a matrix for rotating the object.

		   BOOL fRotated = get_rotation() != 0.0;
		   BOOL fFlipped = (get_flags() & (OBJECT_FLAG_xflipped | OBJECT_FLAG_yflipped)) != 0;
		   BOOL fNegateInflate = FALSE;
		   COutlinePathMatrix RotateMatrix;

		   RotateMatrix.Identity();

		   if (fFlipped)
		   {
			   if (get_flags() & OBJECT_FLAG_xflipped)
			   {
				   RotateMatrix.XFlip(MakeFixed(crObject.left), MakeFixed(crObject.right));
				   fNegateInflate = !fNegateInflate;
			   }
			   if (get_flags() & OBJECT_FLAG_yflipped)
			   {
				   RotateMatrix.YFlip(MakeFixed(crObject.top), MakeFixed(crObject.bottom));
				   fNegateInflate = !fNegateInflate;
			   }
		   }

		   if (fRotated)
		   {
			   POINT Origin;

			   rc->ppnt_to_screen(get_origin(), &Origin);

			   RotateMatrix.TranslateBy(
				   -MakeFixed(Origin.x),
				   -MakeFixed(Origin.y));
			   RotateMatrix.RotateBy(MakeFixed(get_rotation()*RAD2DEGC));
			   RotateMatrix.TranslateBy(
				   MakeFixed(Origin.x),
				   MakeFixed(Origin.y));
		   }

		   CRect crOpenObject = crObject;

		   PCOORD lX, lY;
		   GetOutlineWidth(&lX, &lY);

		   int nX = rc->page_x_to_screen(lX)-rc->page_x_to_screen(0);
		   int nY = rc->page_y_to_screen(lY)-rc->page_y_to_screen(0);

		   int nWidth = nX;
		   if (nY > nWidth)
		   {
			   nWidth = nY;
		   }

		   if (nWidth < 0)
		   {
			   nWidth = COutlinePath::NoLine;
		   }
		   else
		   {
			   if (fNegateInflate)
			   {
				   nWidth = -nWidth;
			   }
		   }

		   // If this path is open (not closed):
		   // The draw code is going to use the Windows polygon function for
		   // this, which draws centered on the line. We want to keep the
		   // drawing within our bounds. So we shrink the theoretical bounding
		   // rectangle by half the line width. This seems to do what we want.

		   int nHalfWidth = nWidth;
		   if (nHalfWidth < 0) nHalfWidth = -nHalfWidth;
		   nHalfWidth = (nHalfWidth+1)/2;
		   if (nHalfWidth != 0)
		   {
			   crOpenObject.left += nHalfWidth;
			   crOpenObject.top += nHalfWidth;
			   crOpenObject.right -= nHalfWidth;
			   crOpenObject.bottom -= nHalfWidth;
			   if (crOpenObject.right <= crOpenObject.left)
			   {
				   crOpenObject.right = crOpenObject.left+1;
			   }
			   if (crOpenObject.bottom <= crOpenObject.top)
			   {
				   crOpenObject.bottom = crOpenObject.top+1;
			   }
		   }

		   if (GetPath(Path, crObject, &crOpenObject))
		   {
			   get_refresh_bound(&ObjectBounds, rc);

			   CRect crToggle;
			   rc->pbox_to_screen(&ObjectBounds, TRUE);
			   rc->convert_pbox(&ObjectBounds, crToggle, NULL);

			   rc->toggle(FALSE, crToggle);

			   // Build a matrix for making the shadow.

			   if (m_Shadow.IsVisible())
			   {
				   COutlinePathMatrix ShadowMatrix;
				   
				   // Get height and width in units of X resolution.
		   
				   PCOORD ShadowX, ShadowY;
				   GetShadowOffsets(&ShadowX, &ShadowY);
						   
				   // Build the matrix.
		   
				   ShadowMatrix.Identity();
				   ShadowMatrix.TranslateBy(
					   MakeFixed((int)(rc->page_x_to_screen(ShadowX)-rc->page_x_to_screen(0))),
					   MakeFixed((int)(rc->page_y_to_screen(ShadowY)-rc->page_y_to_screen(0))));
					   
				   // Build the shadow path.
		   
				   COutlinePath ShadowPath;
				   ShadowPath.Copy(&Path);
				   ShadowPath.Transform(&ShadowMatrix);
		   
				   if (fRotated || fFlipped)
				   {
					   ShadowPath.Transform(&RotateMatrix);
				   }

				   // Draw the shadow.
					   
				   BOOL f = ApplyClipRgn(rc, CLIP_TYPE_Shadow);
				   if (f != -1)
				   {
					   if (m_Fill.IsVisible())
					   {
						   ShadowPath.Draw2(
							   pDC,
							   colorref_from_color(m_Shadow.m_ForegroundColor),
							   m_Shadow.IsVisible(),
							   COutlinePath::NoLine,
							   RGB(0,0,0),
							   FALSE,
		   //					WINDING,
							   ALTERNATE,
							   rc->info.type == RASTER_TYPE_PALETTE);
					   }
					   else
					   {
						   ShadowPath.Draw2(
							   pDC,
							   RGB(0,0,0),
							   FALSE,
							   nWidth,
							   colorref_from_color(m_Shadow.m_ForegroundColor),
							   m_Shadow.IsVisible(),
		   //					WINDING,
							   ALTERNATE,
							   rc->info.type == RASTER_TYPE_PALETTE);
					   }
				   }
				   RestoreClipRgn(rc, f);
			   }
		   
			   if (fRotated || fFlipped)
			   {
				   Path.Transform(&RotateMatrix);
			   }

			   BOOL f = ApplyClipRgn(rc);
			   if (f != -1)
			   {
				   // Build fill object
				   CFill* pFillObject = DeriveFillType(m_Fill.m_nFillType);
				   pFillObject->SetFillColor(m_Fill.m_ForegroundColor, m_Fill.IsVisible());
				   if (m_Fill.m_nFillType >= FIRST_GRADIENT_TYPE)
				   {
					   ((CGradientFill*)pFillObject)->SetBlendColor(m_Fill.m_GradientBlendColor);
					   ((CGradientFill*)pFillObject)->SetRotations(
						   get_rotation(),
						   get_rotation(), 
						   (get_flags() & OBJECT_FLAG_xflipped),
						   (get_flags() & OBJECT_FLAG_yflipped));
				   }
               Path.Draw2(
					   pDC,
					   pFillObject,
					   nWidth,
					   colorref_from_color(m_Outline.m_ForegroundColor),
					   m_Outline.IsVisible(),
		   //			WINDING,
					   ALTERNATE,
					   rc->info.type == RASTER_TYPE_PALETTE);
				   delete pFillObject;
			   }
			   RestoreClipRgn(rc, f);

			   rc->toggle(TRUE, crToggle);
		   }
	   }
   }

	return NULL;
}

/*
// Get the path for this object.
*/

BOOL DrawingObject::GetPath(COutlinePath& Path, const CRect& crObject, const CRect* pcrOpenObject /*=NULL*/)
{
	if (m_DRecord.m_PathRecord > 0)
	{
		OutlinePathRecord* pPathRecord;

		if ((pPathRecord = (OutlinePathRecord*)get_database()->get_record(m_DRecord.m_PathRecord, NULL, RECORD_TYPE_OutlinePath)) != NULL)
		{
			Path.Copy(&pPathRecord->OutlinePath());
			pPathRecord->release();

		/*
		// Paths should be stored in 0..1 coordinates.
		// We map the path now into object coordinates.
		*/

			const CRect* pcrObject = &crObject;
			if (!Path.IsClosed() && pcrOpenObject != NULL)
			{
				// Use the open object thing instead.
				pcrObject = pcrOpenObject;
			}

			COutlinePathMatrix Matrix;

			Matrix.Identity();
			Matrix.ScaleBy(MakeFixed(pcrObject->Width()),
								MakeFixed(pcrObject->Height()));
			Matrix.TranslateBy(MakeFixed(pcrObject->left),
									 MakeFixed(pcrObject->top));

			Path.Transform(&Matrix);

			return TRUE;
		}
	}

	return FALSE;
}

/*
// Set a new outline path, freeing any old one.
*/

void DrawingObject::set_new_outline_path(DB_RECORD_NUMBER l)
{
	if (m_DRecord.m_PathRecord != l)
	{
		if (m_DRecord.m_PathRecord > 0)
		{
			get_database()->free_outline_path_record(m_DRecord.m_PathRecord);
		}

		m_DRecord.m_PathRecord = l;
	}
}

/*
// read_data()
//
// This is a method invoked by the read method to handle object-specific data.
*/

ERRORCODE DrawingObject::read_data(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = INHERITED::read_data(device)) == ERRORCODE_None)
	{
		error = device->read_record(&m_DRecord, sizeof(m_DRecord));
		if (error == ERRORCODE_None)
		{
			if (m_DRecord.m_nEffectsVersion < 2)
			{
				if (m_DRecord.m_nEffectsVersion == 0)
				{
					// Fill
					if (!m_DRecord.m_fOldFillColorExists)
					{
						m_DRecord.m_OldFill.SetType(FillFormatV1::FillNone);
					}
					else
					{
						m_DRecord.m_OldFill.SetType(FillFormatV1::FillSolid, m_DRecord.m_clOldFillColor);
					}

					// Outline
					if (!m_DRecord.m_fOldLineColorExists)
					{
						m_DRecord.m_OldOutline.SetType(OutlineFormat::OutlineNone);
						m_DRecord.m_OldOutline.m_ForegroundColor = COLOR_BLACK;
					}
					else
					{
						m_DRecord.m_OldOutline.SetType(OutlineFormat::OutlineAbsolute, MakeFixed(m_DRecord.m_nOldLineWidth));
						m_DRecord.m_OldOutline.m_ForegroundColor = m_DRecord.m_clOldLineColor;
					}

					// Shadow
					if (!m_DRecord.m_fOldShadowColorExists)
					{
						m_DRecord.m_OldShadow.SetType(ShadowFormat::ShadowNone);
						m_DRecord.m_OldShadow.m_ForegroundColor = COLOR_BLACK;
					}
					else
					{
						m_DRecord.m_OldShadow.SetType(ShadowFormat::ShadowDropDownRight, MakeFixed(OBJECT_SHADOW_FRACTION));
						m_DRecord.m_OldShadow.m_ForegroundColor = m_DRecord.m_clOldShadowColor;
					}

					// Set new effects version.
					m_DRecord.m_nEffectsVersion = 1;
				}
				if (m_DRecord.m_nEffectsVersion == 1)
				{
					m_Fill = m_DRecord.m_OldFill;
					m_Outline = m_DRecord.m_OldOutline;
					m_Shadow = m_DRecord.m_OldShadow;
					m_DRecord.m_nEffectsVersion = 2;
				}
			}
			else
			{
				// Read the structures.
				// Since these are read and written individually now, they
				// can be extended in the future.
				if ((error = device->read_record(&m_Fill, sizeof(m_Fill))) == ERRORCODE_None
					 && (error = device->read_record(&m_Outline, sizeof(m_Outline))) == ERRORCODE_None
					 && (error = device->read_record(&m_Shadow, sizeof(m_Shadow))) == ERRORCODE_None)
				{
					// Success!
				}
			}
		}
	}
	return error;
}

/*
// write_data()
//
// This is a method invoked by the write method to handle object-specific data.
*/

ERRORCODE DrawingObject::write_data(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = INHERITED::write_data(device)) == ERRORCODE_None
		 && ((error = device->write_record(&m_DRecord, sizeof(m_DRecord))) == ERRORCODE_None
		 && (error = device->write_record(&m_Fill, sizeof(m_Fill))) == ERRORCODE_None
		 && (error = device->write_record(&m_Outline, sizeof(m_Outline))) == ERRORCODE_None
		 && (error = device->write_record(&m_Shadow, sizeof(m_Shadow))) == ERRORCODE_None))
	{
		// Success!
	}
	return error;
}

/*
// size_data()
//
// This is a method invoked by the size method to handle object-specific data.
*/

ST_MAN_SIZE DrawingObject::size_data(StorageDevicePtr device)
{
	return INHERITED::size_data(device)
			+ device->size_record(sizeof(m_DRecord))
			+ device->size_record(sizeof(m_Fill))
			+ device->size_record(sizeof(m_Outline))
			+ device->size_record(sizeof(m_Shadow));
}

/*
// Get an object's name.
*/

BOOL DrawingObject::get_name(LPSTR buffer, SHORT size, SHORT max_width)
{
	if (m_DRecord.m_PathRecord > 0)
	{
		OutlinePathRecord* pPathRecord;

		if ((pPathRecord = (OutlinePathRecord*)get_database()->get_record(m_DRecord.m_PathRecord, NULL, RECORD_TYPE_OutlinePath)) != NULL)
		{
			strncpy(buffer, pPathRecord->Name(), size);
			pPathRecord->release();
			buffer[size-1] = '\0';

			return TRUE;
		}
	}

	return FALSE;
}

/*
// Assign an object to this.
*/

ERRORCODE DrawingObject::assign(PageObjectRef pobject)
{
	ERRORCODE error;
	DrawingObject& sobject = (DrawingObject&)pobject;

	if ((error = INHERITED::assign(pobject)) == ERRORCODE_None)
	{
		m_DRecord = sobject.m_DRecord;
		m_Fill = sobject.m_Fill;
		m_Outline = sobject.m_Outline;
		m_Shadow = sobject.m_Shadow;

	/* Increment the reference for our outline path. */

		DB_RECORD_NUMBER PathRecord = m_DRecord.m_PathRecord;

		if (PathRecord > 0)
		{
			OutlinePathRecord* pRecord;

			if ((pRecord = (OutlinePathRecord*)sobject.database->get_record(PathRecord,
										&error, RECORD_TYPE_OutlinePath)) != NULL)
			{
				DB_RECORD_NUMBER NewRecord;

			/* Duplicate the record into our database. */

				OutlinePathCreateStruct cs;

				cs.m_pszName = pRecord->Name();
				cs.m_pPath = &pRecord->OutlinePath();
				cs.m_pDesignBounds = &pRecord->DesignBounds();

				NewRecord = get_database()->new_outline_path_record(&cs);

			/* Release the old one. */

				pRecord->release();

			/* Set the new one. */

				m_DRecord.m_PathRecord = NewRecord;
			}
		}
	}
	return error;
}

PageObjectState* DrawingObject::CreateState(void)
{
	return new DrawingObjectState;
}

void DrawingObject::SaveState(PageObjectState* pState)
{
	DrawingObjectState* pMyState = (DrawingObjectState*)pState;

	INHERITED::SaveState(pState);

	pMyState->m_DRecord = m_DRecord;
	pMyState->m_Fill = m_Fill;
	pMyState->m_Outline = m_Outline;
	pMyState->m_Shadow = m_Shadow;

	if (m_DRecord.m_PathRecord > 0)
	{
		pMyState->m_pDatabase->inc_outline_path_record(m_DRecord.m_PathRecord);
	}
}

void DrawingObject::RestoreState(PageObjectState* pState)
{
	DrawingObjectState* pMyState = (DrawingObjectState*)pState;

	INHERITED::RestoreState(pState);

	DB_RECORD_NUMBER NewPath = pMyState->m_DRecord.m_PathRecord;

/* Maintain the reference counts on the outline path records. */

	if (NewPath != m_DRecord.m_PathRecord)
	{
		PMGDatabase* pDatabase = pMyState->m_pDatabase;

	/* In with the new. */
		if (NewPath > 0)
		{
			pDatabase->inc_outline_path_record(NewPath);
		}
	/* Out with the old. */
		if (m_DRecord.m_PathRecord > 0)
		{
			pDatabase->free_outline_path_record(m_DRecord.m_PathRecord);
		}
	}
	m_DRecord = pMyState->m_DRecord;
	m_Fill = pMyState->m_Fill;
	m_Outline = pMyState->m_Outline;
	m_Shadow = pMyState->m_Shadow;
}

/*
// Is this point in the object opaque?
*/

BOOL DrawingObject::PtIsOpaque(PPNT p, RedisplayContextPtr rc)
{
	if (rc == NULL)
	{
		return TRUE;
	}

	// Make sure we have some visible area.
	int f = ApplyClipRgn(rc);
	RestoreClipRgn(rc, f);
	if (f == -1)
	{
		return TRUE;
	}

	COutlinePath Path;

	// Get the unrotated bounds of the object.

	PBOX ObjectBounds = GetUnrotatedDrawBound();
	rc->pbox_to_screen(&ObjectBounds, TRUE);

	CRect crObject;
	if (rc->convert_pbox(&ObjectBounds, crObject, NULL))
   {
	   // Build a matrix for rotating the object.

	   BOOL fRotated = get_rotation() != 0.0;
	   BOOL fFlipped = (get_flags() & (OBJECT_FLAG_xflipped | OBJECT_FLAG_yflipped)) != 0;
	   BOOL fNegateInflate = FALSE;
	   COutlinePathMatrix RotateMatrix;

	   RotateMatrix.Identity();

	   if (fFlipped)
	   {
		   if (get_flags() & OBJECT_FLAG_xflipped)
		   {
			   RotateMatrix.XFlip(MakeFixed(crObject.left), MakeFixed(crObject.right));
			   fNegateInflate = !fNegateInflate;
		   }
		   if (get_flags() & OBJECT_FLAG_yflipped)
		   {
			   RotateMatrix.YFlip(MakeFixed(crObject.top), MakeFixed(crObject.bottom));
			   fNegateInflate = !fNegateInflate;
		   }
	   }

	   if (fRotated)
	   {
		   POINT Origin;
   #if 1
		   rc->ppnt_to_screen(get_origin(), &Origin);
   #else
		   Origin.x = crObject.left+crObject.Width()/2;
		   Origin.y = crObject.top+crObject.Height()/2;
   #endif

		   RotateMatrix.TranslateBy(
			   -MakeFixed(Origin.x),
			   -MakeFixed(Origin.y));
		   RotateMatrix.RotateBy(MakeFixed(get_rotation()*RAD2DEGC));
		   RotateMatrix.TranslateBy(
			   MakeFixed(Origin.x),
			   MakeFixed(Origin.y));
	   }

	   if (GetPath(Path, crObject))
	   {
		   if (fRotated || fFlipped)
		   {
			   Path.Transform(&RotateMatrix);
		   }

         POINT sp;
         rc->ppnt_to_screen(p, &sp);

         // Is this point in the outline path?
		   CFixedPoint fp;
		   fp.x = MakeFixed(sp.x);
		   fp.y = MakeFixed(sp.y);

		   return Path.PtInPath(fp);
	   }
   }

	// Error. Assume we are opaque.
	return TRUE;
}

/*
// Get the properties for this object.
*/

void DrawingObject::GetObjectProperties(CObjectProperties& Properties)
{
	// Fill
	Properties.SetFillPattern(m_Fill.m_nPattern);
	Properties.SetFillType(m_Fill.m_nFillType);
	Properties.SetFillForegroundColor(m_Fill.m_ForegroundColor);
	Properties.SetFillGradientBlendColor(m_Fill.m_GradientBlendColor);

	// Outline
	Properties.SetOutlineStyle(m_Outline.m_nStyle);
	Properties.SetOutlineWidthType(m_Outline.m_nWidthType);
	Properties.SetOutlineWidth(m_Outline.m_lWidth);
	Properties.SetOutlinePattern(m_Outline.m_nPattern);
	Properties.SetOutlineForegroundColor(m_Outline.m_ForegroundColor);

	// Shadow
	Properties.SetShadowStyle(m_Shadow.m_nStyle);
	Properties.SetShadowOffsetType(m_Shadow.m_nOffsetType);
	Properties.SetShadowXOffset(m_Shadow.m_lXOffset);
	Properties.SetShadowYOffset(m_Shadow.m_lYOffset);
	Properties.SetShadowPattern(m_Shadow.m_nPattern);
	Properties.SetShadowForegroundColor(m_Shadow.m_ForegroundColor);
}

/*
// Set the properties for this object.
*/

BOOL DrawingObject::SetObjectProperties(CObjectProperties& Properties)
{
	BOOL fSet = FALSE;
	SHORT n;
	CFixed l;
	COLOR c;

	// Fill

	n = m_Fill.m_nPattern;
	fSet |= Properties.ApplyFillPattern(n);
	m_Fill.m_nPattern = n;

	n = m_Fill.m_nFillType;
	fSet |= Properties.ApplyFillType(n);
	m_Fill.m_nFillType = n;

	c = m_Fill.m_ForegroundColor;
	fSet |= Properties.ApplyFillForegroundColor(c);
	m_Fill.m_ForegroundColor = c;

	c = m_Fill.m_GradientBlendColor;
	fSet |= Properties.ApplyFillGradientBlendColor(c);
	m_Fill.m_GradientBlendColor = c;

	// Outline

	n = m_Outline.m_nStyle;
	fSet |= Properties.ApplyOutlineStyle(n);
	m_Outline.m_nStyle = n;

	n = m_Outline.m_nWidthType;
	fSet |= Properties.ApplyOutlineWidthType(n);
	m_Outline.m_nWidthType = n;

	l = m_Outline.m_lWidth;
	fSet |= Properties.ApplyOutlineWidth(l);
	m_Outline.m_lWidth = l;

	n = m_Outline.m_nPattern;
	fSet |= Properties.ApplyOutlinePattern(n);
	m_Outline.m_nPattern = n;

	c = m_Outline.m_ForegroundColor;
	fSet |= Properties.ApplyOutlineForegroundColor(c);
	m_Outline.m_ForegroundColor = c;

	// Shadow

	n = m_Shadow.m_nStyle;
	fSet |= Properties.ApplyShadowStyle(n);
	m_Shadow.m_nStyle = n;

	n = m_Shadow.m_nOffsetType;
	fSet |= Properties.ApplyShadowOffsetType(n);
	m_Shadow.m_nOffsetType = n;

	l = m_Shadow.m_lXOffset;
	fSet |= Properties.ApplyShadowXOffset(l);
	m_Shadow.m_lXOffset = l;

	l = m_Shadow.m_lYOffset;
	fSet |= Properties.ApplyShadowYOffset(l);
	m_Shadow.m_lYOffset = l;

	n = m_Shadow.m_nPattern;
	fSet |= Properties.ApplyShadowPattern(n);
	m_Shadow.m_nPattern = n;

	c = m_Shadow.m_ForegroundColor;
	fSet |= Properties.ApplyShadowForegroundColor(c);
	m_Shadow.m_ForegroundColor = c;

	if (fSet)
	{
		changed_image();
	}

	return fSet;
}

void DrawingObject::get_refresh_bound(PBOX_PTR refresh_bound, RedisplayContextPtr rc)
{
	*refresh_bound = get_bound();
	
	PPNT Offset;
	
	GetShadowOffsets(&Offset.x, &Offset.y);
	
	if (get_flags() & OBJECT_FLAG_xflipped)
	{
		Offset.x = -Offset.x;
	}
	if (get_flags() & OBJECT_FLAG_yflipped)
	{
		Offset.y = -Offset.y;
	}

	// rotate the shadow offset vector

	ANGLE dRotation = get_rotation();

	if (dRotation != 0.0)
	{
#if 0
		CFixed fxRotation = MakeFixed(dRotation*RAD2DEGC);
		CFixed fxSine = Sine(fxRotation);
		CFixed fxCosine = Cosine(fxRotation);
		CFixed fxX = MakeFixed((int)Offset.x);
		CFixed fxY = MakeFixed((int)Offset.y);
		CFixed fxNewX = MulFixed(fxX, fxCosine)+MulFixed(fxY, fxSine);
		CFixed fxNewY = MulFixed(fxY, fxCosine)-MulFixed(fxX, fxSine);
		Offset.x = (int)HIWORD(fxNewX+0x8000L);
		Offset.y = (int)HIWORD(fxNewY+0x8000L);
#else
		PPNT Origin;
		Origin.x = Origin.y = 0;
		rotate_points(&Offset, 1, Origin, dRotation);
#endif
	}
	
	if (Offset.x > 0)
	{
		refresh_bound->x1 += Offset.x;
	}
	else
	{
		refresh_bound->x0 += Offset.x;
	}
	
	if (Offset.y  > 0)
	{
		refresh_bound->y1 += Offset.y;
	}
	else
	{
		refresh_bound->y0 += Offset.y;
	}

	// just in case fudging
		
	refresh_bound->x0 -= PAGE_RESOLUTION/10;
	refresh_bound->y0 -= PAGE_RESOLUTION/10;
	refresh_bound->x1 += PAGE_RESOLUTION/10;
	refresh_bound->y1 += PAGE_RESOLUTION/10;
}

/*
// Return the clipping region for this object.
*/

BOOL DrawingObject::GetClipRgn(CRgn& rgnClip, RedisplayContext* rc, int nType /*=CLIP_TYPE_Boundary*/)
{
	// No clip region needed.
	if (record.m_VisibleRect.left == 0.0
		 && record.m_VisibleRect.top == 0.0
		 && record.m_VisibleRect.right == 1.0
		 && record.m_VisibleRect.bottom == 1.0)
	{
		return FALSE;
	}

	// The cases we want both need the boundary, so get it first.
	if (!INHERITED::GetClipRgn(rgnClip, rc, CLIP_TYPE_Boundary))
	{
		return FALSE;
	}

	if (nType == CLIP_TYPE_Shadow)
	{
		// Offset the shadow.
		PPNT ShadowOffset;
		GetShadowOffsets(&ShadowOffset.x, &ShadowOffset.y);

		ANGLE dAngle = get_rotation();
		if (dAngle != 0.0)
		{
         PPNT Origin;
         Origin.x = Origin.y = 0;
			rotate_points(&ShadowOffset, 1, Origin, dAngle);
		}

		rgnClip.OffsetRgn((int)(rc->page_x_to_screen(ShadowOffset.x)-rc->page_x_to_screen(0)),
								(int)(rc->page_y_to_screen(ShadowOffset.y)-rc->page_y_to_screen(0)));
	}
	else
	{
		// The only other choice at this time.
		ASSERT(nType == CLIP_TYPE_Boundary);
	}
	return TRUE;
}

void DrawingObject::GetShadowOffsets(PCOORD* pX, PCOORD* pY)
{
	*pX = 0;
	*pY = 0;
	
	if (m_Shadow.IsVisible())
	{
		if (m_Shadow.m_nOffsetType == ShadowFormat::Absolute)
		{
			*pX = FixedInteger(MulDivFixed(m_Shadow.m_lXOffset, PAGE_RESOLUTION, 72));
			*pY = FixedInteger(MulDivFixed(m_Shadow.m_lYOffset, PAGE_RESOLUTION, 72));
		}
		else
		{
			// Compute extents to use for scaling.
			PBOX Bounds = GetUnrotatedDrawBound();
			PCOORD lX = Bounds.x1-Bounds.x0;
			PCOORD lY = Bounds.y1-Bounds.y0;
			if (lX < lY)
			{
				lY = lX;
			}
			if (lY < lX)
			{
				lX = lY;
			}

			*pX = FixedInteger(m_Shadow.m_lXOffset*lX);
			*pY = FixedInteger(m_Shadow.m_lYOffset*lY);

			// Enforce minimum size.
			PCOORD lMin = PAGE_RESOLUTION/12;

			lX = *pX;
			if (lX < 0)
			{
				lX = -lX;
			}

			lY = *pY;
			if (lY < 0)
			{
				lY = -lY;
			}

			PCOORD lMax = lX;
			if (lMax < lY)
			{
				lMax = lY;
			}

			if (lMax < lMin)
			{
				if (lMax == 0)
				{
					lX = lY = lMin;
				}
				else
				{
					lX = scale_pcoord(lX, lMin, lMax);
					lY = scale_pcoord(lY, lMin, lMax);
				}

				if (*pX >= 0)
				{
					*pX = lX;
				}
				else
				{
					*pX = -lX;
				}

				if (*pY >= 0)
				{
					*pY = lY;
				}
				else
				{
					*pY = -lY;
				}
			}
		}

		*pY = -*pY;
	}

#if 0
	if (m_Shadow.IsVisible())
	{
		PBOX Bounds = GetUnrotatedDrawBound();
		PCOORD x = Bounds.x1-Bounds.x0;
		PCOORD y = Bounds.y1-Bounds.y0;
		
		if (y < x)
		{
			x = y;
		}
		
		x /= 30;
		
		if (x < PAGE_RESOLUTION/12)
		{
			x = PAGE_RESOLUTION/12;
		}
		
		*pXOffset = x;
		*pYOffset = x;
	}
#endif
}

void DrawingObject::GetOutlineWidth(PCOORD* pX, PCOORD* pY)
{
	*pX = 0;
	*pY = 0;
	
	if (m_Outline.IsVisible())
	{
		if (m_Outline.m_nWidthType == OutlineFormat::Absolute)
		{
			*pX = *pY = FixedInteger(MulDivFixed(m_Outline.m_lWidth, PAGE_RESOLUTION, 72));
		}
		else
		{
			// Compute extents to use for scaling.
			PBOX Bounds = GetUnrotatedDrawBound();
			PCOORD lX = Bounds.x1-Bounds.x0;
			PCOORD lY = Bounds.y1-Bounds.y0;
			if (lX < lY)
			{
				lY = lX;
			}
			if (lY < lX)
			{
				lX = lY;
			}

			*pX = FixedInteger(m_Outline.m_lWidth*lX);
			*pY = FixedInteger(m_Outline.m_lWidth*lY);
		}
	}
}

/*
// Return the original dimensions of an object.
*/

BOOL DrawingObject::original_dims(PPNT_PTR p)
{
	if (m_DRecord.m_PathRecord > 0)
	{
		OutlinePathRecord* pPathRecord;

		if ((pPathRecord = (OutlinePathRecord*)get_database()->get_record(m_DRecord.m_PathRecord, NULL, RECORD_TYPE_OutlinePath)) != NULL)
		{
			CFixed fxWidth = pPathRecord->DesignBounds().Width();
			CFixed fxHeight = pPathRecord->DesignBounds().Height();
			
			pPathRecord->release();
			
			if ((fxWidth != 0) && (fxHeight != 0))
			{
				// Make maximum dimension 2 inches.
				
				if (fxWidth < fxHeight)
				{
					fxWidth = MulDivFixed(fxWidth, MakeFixed(PAGE_RESOLUTION*2), fxHeight);
					fxHeight = MakeFixed(PAGE_RESOLUTION*2);
				}
				else
				{
					fxHeight = MulDivFixed(fxHeight, MakeFixed(PAGE_RESOLUTION*2), fxWidth);
					fxWidth = MakeFixed(PAGE_RESOLUTION*2);
				}
				
				p->x = (int)HIWORD(fxWidth+0x8000L);
				p->y = (int)HIWORD(fxHeight+0x8000L);
				
				return TRUE;
			}
		}
	}
	
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
// DrawingObjectState

DrawingObjectState::DrawingObjectState()
{
	m_DRecord.m_PathRecord = 0;
}

DrawingObjectState::~DrawingObjectState()
{
	if (m_DRecord.m_PathRecord > 0)
	{
		m_pDatabase->free_outline_path_record(m_DRecord.m_PathRecord);
	}
}
