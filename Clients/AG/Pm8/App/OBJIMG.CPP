/*
// $Header: /PM8/App/OBJIMG.CPP 1     3/03/99 6:08p Gbeddow $
//
// PMG object image routines.
//
// $Log: /PM8/App/OBJIMG.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 5     4/07/98 8:43a Jayn
// Expanded size capabilities.
// 
// 4     2/23/98 5:44p Hforman
// 
// 3     2/23/98 10:29a Hforman
// more 1 pixel slop when checking bounds in draw_image()
// 
// 
// 2     2/10/98 5:21p Hforman
// modified draw_image() to allow 1 pixel slop when checking bounds
// 
//    Rev 1.0   14 Aug 1997 15:23:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:24   Fred
// Initial revision.
// 
//    Rev 1.13   20 Jul 1997 14:36:58   Fred
// Does not allow handle wrap when cropping; handles empty clip regions
// 
//    Rev 1.12   02 Jun 1997 09:40:26   Jay
// Fixed overflow in pt_is_opaque.
// 
//    Rev 1.11   04 Mar 1997 17:03:38   Jay
// Fixes for drawing objects, cropping, etc.
// 
//    Rev 1.10   19 Feb 1997 16:54:06   Jay
// Beginning of cropping
// 
//    Rev 1.9   13 Jan 1997 15:26:42   Jay
// Now compiles under 16-bit
// 
//    Rev 1.8   02 Jan 1997 09:52:58   Jay
// Includes malloc.h for 16-bit build.
// 
//    Rev 1.7   11 Dec 1996 15:56:22   Jay
// Support for masks > 64K
// 
//    Rev 1.6   09 Dec 1996 11:21:46   Jay
// Scaled PtIsOpaque now works.
// 
//    Rev 1.5   09 Dec 1996 10:19:32   Jay
// More image stuff.
// 
//    Rev 1.4   06 Dec 1996 09:15:46   Jay
// More transparent bitmap stuff.
// 
//    Rev 1.3   05 Dec 1996 12:30:16   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.2   04 Nov 1996 16:06:06   Jay
// New object image routines.
// 
//    Rev 1.1   01 Nov 1996 17:11:42   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.0   14 Mar 1996 13:43:54   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:34   FRED
// Initial revision.
// 
//    Rev 2.2   18 Oct 1995 15:58:08   FRED
// Fixes for 16 bit version after 32 bit changes
// 
//    Rev 2.1   15 Sep 1995 14:04:18   JAY
// More 32-bit changes.
// 
//    Rev 2.0   07 Feb 1995 15:47:56   JAY
// Initial revision.
// 
//    Rev 1.19   30 Jan 1995 09:17:00   JAY
// Now uses attribute DC of RC when necessary.
// 
//    Rev 1.18   30 Nov 1994 16:35:04   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.17   04 Aug 1994 07:32:32   JAY
// Large model and 32-bit changes
// 
//    Rev 1.16   26 May 1994 13:43:28   JAY
// Now uses remove_slash().
// 
//    Rev 1.15   31 Mar 1994 15:37:48   JAY
// Now selects the "good" palette into the source DC for a DDB draw. StretchBlt
// seems to need it to do color stretching correctly.
// 
//    Rev 1.14   01 Mar 1994 15:51:16   JAY
// Last strip support to fix incorrect assumption about last strips.
// 
//    Rev 1.13   28 Feb 1994 13:02:30   JAY
// Fixed stretching (actually, shrinking) problem with last strip.
// 
//    Rev 1.12   25 Feb 1994 09:43:48   JAY
// Some tweeking to prevent DDB images from going outside the object bounds.
// 
//    Rev 1.11   22 Feb 1994 15:02:18   JAY
// New rasterizing code for CGMs which uses GDI calls.
// 
//    Rev 1.10   16 Feb 1994 14:00:36   JAY
// Uses StorageFile::base_path instead of "." for tempnam() dir parameter.
// 
//    Rev 1.9   08 Feb 1994 09:18:36   JAY
// Added a fudge factor to the scale check code to prevent too much redraw at
// max scale of 1 due to round off errors.
// 
//    Rev 1.8   02 Feb 1994 16:32:46   JAY
// Sanity check in case of byte width mismatches or read error.
// 
//    Rev 1.7   02 Feb 1994 09:33:06   JAY
// Now sets the error_info member of the paint context in case error diffusion
// is used by an object.
// 
//    Rev 1.6   01 Feb 1994 17:43:40   JAY
// Now builds the image regardless of whether it's completely contained or not.
// 
//    Rev 1.5   01 Feb 1994 10:20:58   JAY
// Added dummy FillRect (with NULL_BRUSH) on solo StretchDIBits to fix a bug
// in Windows where dialogs don't know a StretchDIBits occurred behind them.
// 
//    Rev 1.4   31 Jan 1994 07:14:52   JAY
// Removed od('s for release version.
// 
//    Rev 1.3   19 Jan 1994 17:06:38   JAY
//  
// 
//    Rev 1.2   13 Jan 1994 17:00:18   JAY
//  
// 
//    Rev 1.1   05 Jan 1994 08:05:46   JAY
// Restructuring, etc.
// 
//    Rev 1.0   30 Dec 1993 16:09:42   JAY
// Initial revision.
*/

#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <dos.h>

#ifndef WIN32
#include <malloc.h>
#endif

#include "pmgdb.h"

#include "grafobj.h"

#include "utils.h"
#include "util.h"
#include "file.h"
#include "paint.h"
#include "objimg.h"
#include "bmp.h"        // For bitmap/DIB utilities

// We borrow this from PMWVIEW.CPP.
// If you need your own palette, feel free.
extern CPalette *pOurPal;

/*
// This is the amount an image can exceed its scaling parameters before it's
// considered too scaled. This is necessary for normal round-offs on screen
// redisplay, especially at max scaling of 1.
*/

#define SCALE_FUDGE  4

/////////////////////////////////////////////////////////////////////////////
// ObjectImageContext

/*
// The paint context we use for updating images.
*/

class ObjectImageContext : public PaintContext
{
protected:
   virtual void compute_source_extent(void);
   CBitmapObjectImage* object_image;
public:
   ObjectImageContext(CBitmapObjectImage* image);
};

/////////////////////////////////////////////////////////////////////////////
// CObjectImage

/*
// The template for image creation.
// It is assumed that this is NID'd by the application.
*/

char CObjectImage::image_template[5+1] = "~xxxI";

DWORD CObjectImage::minimum_image_space = 300*1024L;     /* 300K */

int CObjectImage::max_scale_up = 2;
int CObjectImage::max_scale_down = 2;

/*
// Constructor for the object image context.
*/

ObjectImageContext::ObjectImageContext(CBitmapObjectImage* image)
   : object_image(image)
{
}

/*
// Compute the source extent. This is the size of the complete object.
*/

void ObjectImageContext::compute_source_extent(void)
{
   source_width = object_image->m_ImageRect.right
                                 - object_image->m_ImageRect.left;
   source_height = object_image->m_ImageRect.bottom
                                 - object_image->m_ImageRect.top;
}

/////////////////////////////////////////////////////////////////////////////
// CBitmapObjectImage

/*
// The constructor for an object image.
*/

CBitmapObjectImage::CBitmapObjectImage()
{
   m_pFile = NULL;
   m_fRecording = FALSE;
   m_fMaskSource = FALSE;
   m_pMask = NULL;            // No mask yet.

   m_ulImageOffset = 0;

   m_fHasMask = NULL;
   m_ulMaskOffset = (ULONG)POSITION_UNKNOWN;          // For now!

#ifndef WIN32
   m_fHugeMask = FALSE;
#endif
}

/*
// The destructor for an object image.
*/

CBitmapObjectImage::~CBitmapObjectImage()
{
// od("Freeing object image...\r\n");
   FreeFile();
   if (m_pMask != NULL)
   {
#ifndef WIN32
      if (m_fHugeMask)
      {
         _hfree(m_pMask);
      }
      else
#endif
      {
         delete [] m_pMask;
      }
   }
}

void CBitmapObjectImage::FreeFile(void)
{
   // Close the file if we are recording. This will delete the temp file.
   delete m_pFile;
   m_pFile = NULL;
}

/*
// Begin image recording.
*/

ERRORCODE CBitmapObjectImage::begin_image(RedisplayContextPtr rc, PBOX bound, PBOX world)
{
   m_OriginalBound = bound;

/*
// Compute the area that we will be recording.
*/

/* First, compute the portion of this object which is visible. */

   if (!IntersectBox(&m_ImageArea, &bound, &world))
   {
      return ERRORCODE_BadParameter;
   }

/* Setup the raster info. */

   RasterInfo raster_info = rc->info;

/* Construct our image paint context. */

   if (raster_info.type == RASTER_TYPE_8)
   {
      raster_info.bpp = 8;
      raster_info.planes = 1;
   }

/* Create the paint context so we can compute the parameters for the bitmap. */

   PaintContext pc;

   pc.bound = m_ImageArea;
   pc.clip = NULL;
   pc.rc = rc;
   pc.raster_info = raster_info;

   if (!pc.pre_paint())
   {
      return ERRORCODE_BadParameter;
   }

   // Normalize the area on the object (in case it moves.)
   m_ImageArea.x0 -= bound.x0;
   m_ImageArea.y0 -= bound.y0;
   m_ImageArea.x1 -= bound.x0;
   m_ImageArea.y1 -= bound.y0;

   m_ImageRect = pc.dest_bound;

   // Compute the strip byte width.
   m_nImageByteWidth = pc.clipped_dest_width*raster_info.bpp;
   if (raster_info.bpp < 8)
   {
      m_nImageByteWidth += raster_info.bpp-1;
   }
   m_nImageByteWidth = (m_nImageByteWidth/8 + 3) &~3;

/* Compute the mask byte width. */

   m_nMaskByteWidth = ((pc.clipped_dest_width + 7)/8 + 3) & ~3;

/* Compute the offsets in the file. */

   // Make the temp file to use.

   ASSERT(m_pFile == NULL);
   TRY
   {
      m_pFile = new TmpFile(image_template);
   }
   END_TRY

   if (m_pFile == NULL)
   {
      return ERRORCODE_Memory;
   }

   LPCSTR pName = m_pFile->get_name();
   if (pName == NULL || *pName == '\0')
   {
      FreeFile();
      return ERRORCODE_Full;
   }

/* See how much disk space is available on the temp drive. */

   DWORD max_size = (DWORD)((m_ImageRect.right - m_ImageRect.left)*3
                           + m_nMaskByteWidth)
                        * (m_ImageRect.bottom-m_ImageRect.top);

// od("max size is %ld\r\n", max_size);

   if (Util::GetAvailableDiskSpace(pName) < minimum_image_space + max_size)
   {
      // Not enough room!
      FreeFile();
      return ERRORCODE_Full;
   }

   // Create the file.
   ERRORCODE error;
   if ((error = m_pFile->initialize()) != ERRORCODE_None)
   {
      FreeFile();
      return error;
   }

   m_fRecording = TRUE;
   return ERRORCODE_None;
}

/*
// End image recording.
*/

ERRORCODE CBitmapObjectImage::end_image()
{
   m_fRecording = FALSE;

   if (m_pFile == NULL)
   {
      return ERRORCODE_NotInitialized;
   }

   // Just close it.
   m_pFile->flush();
   return ERRORCODE_None;
}

/*
// Draw the image we have for this object.
*/

ERRORCODE CBitmapObjectImage::draw_image(PMGPageObjectPtr object, RedisplayContextPtr rc, PBOX extent)
{
   if (m_pFile == NULL)
   {
      return ERRORCODE_NotInitialized;
   }

   ERRORCODE error = ERRORCODE_None;
   PBOX bound = object->get_bound();

	// make sure our bounds haven't changed (within a pixel, anyway)
	PCOORD origWidth = m_OriginalBound.x1 - m_OriginalBound.x0;
	PCOORD origHeight = m_OriginalBound.y1 - m_OriginalBound.y0;
	PCOORD width = bound.x1 - bound.x0;
	PCOORD height = bound.y1 - bound.y0;

	if (origWidth  > (width + rc->redisplay_x_pixel)  ||
		 origWidth  < (width - rc->redisplay_x_pixel)  ||
		 origHeight > (height + rc->redisplay_y_pixel) ||
		 origHeight < (height - rc->redisplay_y_pixel))
	{
//		od("Object changed size.\r\n");
		return ERRORCODE_BadParameter;
	}

	// Make sure we can satisfy the request (again, within a pixel)
   extent.x0 -= bound.x0;
   extent.y0 -= bound.y0;
   extent.x1 -= (bound.x0 + rc->redisplay_x_pixel);
   extent.y1 -= (bound.y0 + rc->redisplay_y_pixel);

   if (!IsBoxInBox(&extent, &m_ImageArea))
   {
//    od("Extent not in image area.\r\n");
      return ERRORCODE_BadParameter;
   }

   // Begin with our paint context.
   ObjectImageContext oic(this);

   // Failed to prepare.
   if (!PreparePaintContext(oic, object, rc))
   {
      return ERRORCODE_None;
   }

/*
// Make sure the image has not been compressed or expanded too much.
*/

   int nImageWidth = m_ImageRect.right - m_ImageRect.left;
   int nThisWidth = oic.dest_bound.right - oic.dest_bound.left;

   if (nThisWidth > nImageWidth)
   {
      if (max_scale_up != 0)
      {
         if (nThisWidth > max_scale_up*nImageWidth + SCALE_FUDGE)
         {
         /* Made too large. */
//          od("X scaled up too much.\r\n");
            return ERRORCODE_BadParameter;
         }
      }
   }
   else if (nThisWidth < nImageWidth)
   {
      if (max_scale_down != 0)
      {
         if (nImageWidth > max_scale_down*nThisWidth + SCALE_FUDGE)
         {
         /* Made too small. */
//          od("X scaled down too much.\r\n");
            return ERRORCODE_BadParameter;
         }
      }
   }

   int nImageHeight = m_ImageRect.bottom - m_ImageRect.top;
   int nThisHeight = oic.dest_bound.bottom - oic.dest_bound.top;

   if (nThisHeight > nImageHeight)
   {
      if (max_scale_up != 0)
      {
         if (nThisHeight > max_scale_up*nImageHeight + SCALE_FUDGE)
         {
         /* Made too large. */
//          od("Y scaled up too much.\r\n");
            return ERRORCODE_BadParameter;
         }
      }
   }
   else if (nThisHeight < nImageHeight)
   {
      if (max_scale_down != 0)
      {
         if (nImageHeight > max_scale_down*nThisHeight + SCALE_FUDGE)
         {
         /* Made too small. */
//          od("Y scaled down too much.\r\n");
            return ERRORCODE_BadParameter;
         }
      }
   }

/*
// Next, initialize the strip for the full image width.
*/

   if (oic.initialize_strip(nImageWidth, FALSE, FALSE, m_fHasMask))
   {
      // Draw the DIB image.

      // The byte widths must match.
      if (oic.strip_byte_width != m_nImageByteWidth)
      {
         // Strip widths do not match!
         ASSERT(FALSE);
         return ERRORCODE_BadParameter;
      }

      // Initialize the graphic map.
      oic.build_graphic_map(nImageWidth);

      if (oic.m_fHasMask)
      {
         // Construct the palette for the mono DIB.
         oic.m_MaskInfo.bmiColors[0] = m_MaskColor[0];
         oic.m_MaskInfo.bmiColors[1] = m_MaskColor[1];

         // These must be the same for this all to work!
         ASSERT(m_nMaskByteWidth == oic.m_nMaskByteWidth);
      }

      // Run through all the strips, starting at the top.
      int current_source_line = oic.srce.top;

		BOOL f = object->ApplyClipRgn(rc);
		if (f != -1)
		{
			for (oic.current_strip = 0; oic.current_strip < oic.n_strips; oic.current_strip++)
			{
				// Compute the top and bottom of the strip.

				oic.compute_strip(oic.current_strip);

				// Compute the current strip height.

				int nThisStripHeight = (int)oic.bitmap_info.bmiHeader.biHeight;

				// We need to seek reversed since the data is reversed from what
				// we want (DIBs are upside-down).

				int nSeekLine = (oic.source_height - (current_source_line + nThisStripHeight));

				// Read the image data.
				ST_DEV_POSITION lOffset = m_ulImageOffset +
							(ST_DEV_POSITION)nSeekLine * (ST_DEV_POSITION)m_nImageByteWidth;
				UINT uImageSize = m_nImageByteWidth * nThisStripHeight;

				if ((error = m_pFile->seek(lOffset, ST_DEV_SEEK_SET)) != ERRORCODE_None
					|| (error = m_pFile->huge_read(oic.strip, uImageSize)) != ERRORCODE_None)
				{
					// Got an error reading data!
					ASSERT(error == ERRORCODE_DoesNotExist);
					break;
				}

				// Read the mask if necessary.

				if (oic.m_pMask != NULL)
				{
					ST_DEV_POSITION lMaskOffset = (ST_DEV_POSITION)nSeekLine * (ST_DEV_POSITION)m_nMaskByteWidth;
					UINT uMaskSize = m_nMaskByteWidth * nThisStripHeight;

					oic.m_MaskInfo.bmiHeader.biSizeImage = (DWORD)uMaskSize;

					// See where we need to read the mask from.
					if (m_pMask == NULL)
					{
						// Read from file.
						ASSERT(m_ulMaskOffset != POSITION_UNKNOWN);
						if ((error = m_pFile->seek(m_ulMaskOffset + lMaskOffset, ST_DEV_SEEK_SET)) != ERRORCODE_None
								|| (error = m_pFile->huge_read(oic.m_pMask, uMaskSize)) != ERRORCODE_None)
						{
							// Got an error reading mask!
							ASSERT(FALSE);
							break;
						}
					}
					else
					{
						// Copy from in-memory image.
#ifdef WIN32
	               memcpy(oic.m_pMask, m_pMask + lMaskOffset, uMaskSize);
#else
						hmemcpy(oic.m_pMask, (BYTE huge*)m_pMask + lMaskOffset, uMaskSize);
#endif
					}
				}

				// And show it on the screen!
				oic.show_graphic_map(oic.dest_rect, 0, nThisStripHeight);

				// Advance the current source line.
				current_source_line += nThisStripHeight;
			}
		}

		// Restore any changed clip region.
		object->RestoreClipRgn(rc, f);
   }

   // Make sure the file is closed.
   m_pFile->flush();

   return error;
}

/*
// Is this point in the object opaque.
*/

BOOL CBitmapObjectImage::PtIsOpaque(PMGPageObject* pObject, RedisplayContext* rc, PPNT p)
{
   BOOL fOpaque = TRUE;
   if (m_pMask != NULL && !m_fRecording)
   {
      static BYTE bMask[] =
      {
         0x80,
         0x40,
         0x20,
         0x10,
         0x08,
         0x04,
         0x02,
         0x01
      };
      // Compute the effective image area.
      int nImageWidth = m_ImageRect.right - m_ImageRect.left;
      int nImageHeight = m_ImageRect.bottom - m_ImageRect.top;

      PBOX Bound = pObject->get_bound();
      PCOORD OriginalWidth = m_OriginalBound.x1 - m_OriginalBound.x0;
      PCOORD OriginalHeight = m_OriginalBound.y1 - m_OriginalBound.y0;
      PCOORD Width = Bound.x1 - Bound.x0;
      PCOORD Height = Bound.y1 - Bound.y0;

      PBOX NewImageArea;

      // Compute NewImageArea. m_ImageArea is already normalized.
      NewImageArea.x0 = Bound.x0 + (PCOORD)((double)m_ImageArea.x0*(double)Width/(double)OriginalWidth);
      NewImageArea.y0 = Bound.y0 + (PCOORD)((double)m_ImageArea.y0*(double)Height/(double)OriginalHeight);
      NewImageArea.x1 = Bound.x0 + (PCOORD)((double)m_ImageArea.x1*(double)Width/(double)OriginalWidth);
      NewImageArea.y1 = Bound.y0 + (PCOORD)((double)m_ImageArea.y1*(double)Height/(double)OriginalHeight);

      // Scale the point down into the image.
      p.x = scale_pcoord(p.x - NewImageArea.x0,
                              nImageWidth,
                              NewImageArea.x1 - NewImageArea.x0);

      p.y = scale_pcoord(p.y - NewImageArea.y0,
                              nImageHeight,
                              NewImageArea.y1 - NewImageArea.y0);

      // Now see if it is within our image mask.
      if (p.x >= 0
          && p.y >= 0
          && p.x <= nImageWidth-1
          && p.y <= nImageHeight-1)
      {
         POINT sp;
         sp.x = (int)p.x;
         sp.y = (int)p.y;

         // Compute the byte.
         // Note that we read from the bottom.
#ifdef WIN32
         LPBYTE pByte = m_pMask
                           + ((nImageHeight-1) - sp.y)*m_nMaskByteWidth
                           + (sp.x >> 3);
#else
         BYTE huge* pByte = (BYTE huge*)m_pMask
                           + ((long)((nImageHeight-1) - sp.y))*(long)m_nMaskByteWidth
                           + (long)(sp.x >> 3);
#endif
         fOpaque = (*pByte & bMask[sp.x & 7]) == 0;
      }
		else
      {
         // Out-of-range is not opaque.
         TRACE("PtIsOpaque: Point out of range!\n");
         fOpaque = FALSE;
      }
   }
   return fOpaque;
}

/*
// Are we currently making an image?
*/

BOOL CBitmapObjectImage::making_image(void)
{  
   ASSERT(!m_fRecording || m_pFile != NULL);
   return m_fRecording;
}

ERRORCODE CBitmapObjectImage::record_image(int nTop, int nLines, LPBYTE s, LPBYTE mask)
{
   // Make sure we have a file to write to.
   if (m_pFile == NULL)
   {
      // Can not write to an unopened file.
      return ERRORCODE_NotInitialized;
   }

// od("Record image lines %d to %d (%d, %d); ", top, top+lines, m_nImageByteWidth, m_nMaskByteWidth);

   int nImageHeight = m_ImageRect.bottom - m_ImageRect.top;
   int nLineOffset = nImageHeight - (nTop + nLines);
   DWORD dwWriteSize = (DWORD)m_nImageByteWidth*(DWORD)nLines;

   ST_DEV_POSITION lOffset = m_ulImageOffset
                  + (ST_DEV_POSITION)nLineOffset * m_nImageByteWidth;
   TRACE("Offset: %ld, size: %lu\n", lOffset, dwWriteSize);

   // Seek to the "write" spot...
   // ...and do the "write" thing.

   ERRORCODE error;

   if ((error = m_pFile->seek(lOffset, ST_DEV_SEEK_SET)) != ERRORCODE_None
       || (error = m_pFile->huge_write(s, dwWriteSize)) != ERRORCODE_None)
   {
      // Not all "write"!
      return error;
   }

   // Handle the mask.

   if (mask != NULL)
   {
      m_MaskColor[0].rgbRed =
      m_MaskColor[0].rgbBlue =
      m_MaskColor[0].rgbGreen = 0;
      m_MaskColor[0].rgbReserved = 0;

      m_MaskColor[1].rgbRed =
      m_MaskColor[1].rgbBlue =
      m_MaskColor[1].rgbGreen = 255;
      m_MaskColor[1].rgbReserved = 0;

      if (!m_fHasMask)
      {
         // We have not seen a mask yet.
         // We want to go to memory (first).
         // If this is not possible, we go to the file.
         // In the latter case, we can not do a hit-test.

         long lSize  = (long)m_nMaskByteWidth
                           * (m_ImageRect.bottom - m_ImageRect.top);

         // Try the memory route.
         TRY
         {
#ifndef WIN32
            if (lSize > 65000L)
            {
               m_pMask = (LPBYTE)halloc(lSize, 1);
               m_fHugeMask = TRUE;
            }
            else
#endif
            {
               m_pMask = new BYTE[(size_t)lSize];
            }
         }
         END_TRY

         // If no memory, set up for the file write.
         if (m_pMask == NULL)
         {
            ASSERT(FALSE);       // We don't really want this to happen.
            m_ulMaskOffset = m_ulImageOffset
                  + (ST_DEV_POSITION)m_nImageByteWidth
                           * (m_ImageRect.bottom - m_ImageRect.top);
         }
         m_fHasMask = TRUE;
      }

      // Compute the mask offset.
      lOffset = (ST_DEV_POSITION)nLineOffset * m_nMaskByteWidth;
      dwWriteSize = (DWORD)m_nMaskByteWidth*(DWORD)nLines;

      if (m_pMask == NULL)
      {
         // Seek to the spot to write the mask and write it.

         if ((error = m_pFile->seek(m_ulMaskOffset + lOffset, ST_DEV_SEEK_SET)) != ERRORCODE_None
             || (error = m_pFile->huge_write(mask, dwWriteSize)) != ERRORCODE_None)
         {
            // Not all "write"!
            return error;
         }
      }
      else
      {
         // Copy the data.
#ifdef WIN32
         memcpy(m_pMask + lOffset, mask, (size_t)dwWriteSize);
#else
         hmemcpy((BYTE huge*)m_pMask + lOffset, mask, dwWriteSize);
#endif
      }
      TRACE("Mask offset: %ld, size: %lu\n", lOffset, dwWriteSize);

      m_fMaskSource = TRUE;
   }

   // All "write"!

   return ERRORCODE_None;
}

/*
// Record a Device Dependent Bitmap image.
*/

ERRORCODE CBitmapObjectImage::record_ddb(HBITMAP hbmBitmap, HBITMAP hbmMask, BOOL fRecordMask)
{
   // Make sure we are actually writing.
   if (m_pFile == NULL)
   {
      // Can not write to an unopened file.
      return ERRORCODE_NotInitialized;
   }

   ERRORCODE error;
   HGLOBAL hBitmap = NULL;
   HGLOBAL hMask = NULL;

   // Convert the bitmap to a DIB.
   if ((error = BitmapToDIB(hbmBitmap, (HPALETTE)pOurPal->GetSafeHandle(), &hBitmap)) != ERRORCODE_None)
   {
      return error;
   }

   if (fRecordMask)
   {
      // Convert the mask to a DIB.
      if ((error = BitmapToDIB(hbmMask, NULL, &hMask)) != ERRORCODE_None)
      {
         ::GlobalFree(hBitmap);
         return error;
      }
   }

   // We now have what we need.
   // Lock the handles to get the data.
   ASSERT(hBitmap != NULL && (!fRecordMask || hMask != NULL));

   LPBITMAPINFO pBitmap = NULL;
   LPBITMAPINFO pMask = NULL;

   // Use exception handling to make life easier.
   TRY
   {
      // Lock the bitmap.
      pBitmap = (LPBITMAPINFO)::GlobalLock(hBitmap);
      if (pBitmap == NULL)
      {
         AfxThrowMemoryException();
      }
      // Lock the mask if necessary.
      if (hMask != NULL)
      {
         pMask = (LPBITMAPINFO)::GlobalLock(hMask);
         if (pMask == NULL)
         {
            AfxThrowMemoryException();
         }
         // Remember the color palette.
         m_MaskColor[0] = pMask->bmiColors[0];
         m_MaskColor[1] = pMask->bmiColors[1];
      }

      // Record the data.
      error = record_image(0, (int)pBitmap->bmiHeader.biHeight,
                           (LPBYTE)FindDIBBits((LPSTR)pBitmap),
                           pMask == NULL ? NULL : (LPBYTE)FindDIBBits((LPSTR)pMask));
   }
   CATCH(CMemoryException, e)
   {
      // Memory error.
      error = ERRORCODE_Memory;
   }
   AND_CATCH_ALL(e)
   {
      ASSERT(FALSE);
      error = ERRORCODE_IntError;
   }
   END_CATCH_ALL

   if (pMask != NULL)
   {
      ASSERT(hMask != NULL);
      GlobalUnlock(hMask);
   }

   if (pBitmap != NULL)
   {
      ASSERT(hBitmap != NULL);
      GlobalUnlock(hBitmap);
   }

   if (hMask != NULL)
   {
      ::GlobalFree(hMask);
   }
   if (hBitmap != NULL)
   {
      ::GlobalFree(hBitmap);
   }

   return error;
}

BOOL CBitmapObjectImage::PreparePaintContext(PaintContext& oic, PMGPageObject* pObject, RedisplayContext* rc)
{
   PBOX bound = pObject->get_bound();

   // Plug in the important stuff.
   oic.bound = m_ImageArea;
   oic.bound.x0 += bound.x0;     /* Un-normalize. */
   oic.bound.y0 += bound.y0;
   oic.bound.x1 += bound.x0;
   oic.bound.y1 += bound.y0;

   oic.graphic_flags = 0;
   oic.do_xflip = FALSE;
   oic.do_yflip = FALSE;
   oic.clip = &rc->clip_rect;
   oic.rc = rc;
   oic.image_type = IMAGE_OUTLINE;        /* Just not bitmap... */

   switch (pObject->type())
   {
      case OBJECT_TYPE_Graphic:
      case OBJECT_TYPE_Background:
      {
         GraphicObjectPtr gobject = (GraphicObjectPtr)pObject;
         GraphicPtr graphic;
         PMGDatabasePtr database = gobject->get_database();

         if ((graphic = (GraphicPtr)database->get_record(gobject->get_graphic_record(), NULL, RECORD_TYPE_Graphic)) != NULL)
         {
            switch (graphic->record.type)
            {
               case GRAPHIC_TYPE_CGM:
               case GRAPHIC_TYPE_WMF:
               {
                  // Leave outline graphics alone.
                  break;
               }
               default:
               {
                  // Bitmap graphics need to match the render conditions when they were created.
                  oic.image_type = IMAGE_PALETTE;
                  oic.set_render_method(gobject->get_render_method());
                  oic.error_info = gobject->get_error_info();
                  break;
               }
            }
            graphic->release();
         }
         break;
      }
      default:
      {
      /* Something else. */
         break;
      }
   }

   // Setup the raster info.
   oic.raster_info = rc->info;

   if (oic.raster_info.type == RASTER_TYPE_8)
   {
      oic.raster_info.bpp = 8;
      oic.raster_info.planes = 1;
   }

/*
// First we do our pre-paint.
*/

   if (!oic.pre_paint())
   {
//    od("pre_paint failed.\r\n");
      return FALSE;
   }

   oic.set_render_method(ORDERED_DITHER);
   return TRUE;
}
