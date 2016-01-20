/*
//	IMAGESRC.CPP
//
//		Copyright (C) 1993 MicroLogic Software, Inc.
//
// Image source routines.
//
// $Header: /PM8/App/IMAGESRC.CPP 1     3/03/99 6:06p Gbeddow $
//
// $Log: /PM8/App/IMAGESRC.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 3     7/11/98 2:48p Jayn
// Normalizes the angle of rotation.
// 
// 2     11/19/97 3:43p Jayn
// Rotated bitmap support.
// 
//    Rev 1.1   17 Sep 1997 09:12:32   Jay
// Bitmap rotation
// 
//    Rev 1.0   14 Aug 1997 15:21:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:48   Fred
// Initial revision.
// 
//    Rev 1.4   23 Jun 1997 13:29:54   Fred
// Rearranged some things to make internal compiler error go away
// 
//    Rev 1.3   05 Dec 1996 12:30:14   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.2   05 Nov 1996 14:06:52   Jay
// Got rid of warnings, etc.
// 
//    Rev 1.1   01 Nov 1996 17:11:42   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.0   14 Mar 1996 13:48:00   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:10   FRED
// Initial revision.
// 
//    Rev 2.2   26 Jun 1995 10:08:32   JAY
// Got rid of doubles. Now uses CFixed.
// 
//    Rev 2.1   29 Mar 1995 08:59:12   JAY
// Lots of changes
// 
//    Rev 2.0   07 Feb 1995 15:47:54   JAY
// Initial revision.
// 
//    Rev 1.4   30 Nov 1994 16:33:16   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.3   04 Aug 1994 07:32:36   JAY
// Large model and 32-bit changes
// 
//    Rev 1.2   31 Jan 1994 07:15:20   JAY
// Removed od('s for release version.
// 
//    Rev 1.1   05 Jan 1994 08:07:12   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:28   JAY
// 
*/

#include "stdafx.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "utils.h"
#include	"pmgobj.h"
#include	"grafrec.h"
#include	"grafobj.h"
#include "image.h"
#include "rotutils.h"

#define DO_ROTATION

/*
// The default info method for an image source.
*/

ERRORCODE ImageSource::info(IMAGE_INFO_PTR info)
{
	ERRORCODE error;

	if ((error = driver->info(info)) == ERRORCODE_None)
	{
		our_info = *info;
	}
	return error;
}

/*
// The default open method for an image source.
*/

ERRORCODE ImageSource::open(PALETTE_INFO_PTR p)
{
	return driver->open(p);
}

/*
// The default close method for an image source.
*/

void ImageSource::close(VOID)
{
	driver->close();
}

/*
// The default close method for an image source.
*/

ERRORCODE ImageSource::read(int line, LPBYTE buffer, int count, LPBYTE pMask /*=NULL*/)
{
	return driver->read(line, buffer, count, pMask);
}

/*
// The default source extent method for an image source.
*/

void ImageSource::compute_source_extent(PPOINT extent)
{
	extent->x = our_info.width;
	extent->y = our_info.height;
}

/*
// Alter our render needs if necessary.
*/

ERRORCODE ImageSource::compute_render_needs(void)
{
	return ERRORCODE_None;
}

#ifdef DO_ROTATION

/////////////////////////////////////////////////////////////////////////////
// RotatedImageSource

/*
// The constructor for a rotated image source.
*/

RotatedImageSource::RotatedImageSource(ImageDriverPtr d)
			: m_Buffer(d), ImageSource(d)
{
	m_pImageStrip = NULL;
	m_pMaskStrip = NULL;
}

/*
// The info routine for a rotated image.
*/

ERRORCODE RotatedImageSource::info(IMAGE_INFO_PTR info)
{
	ERRORCODE error;

	if ((error = ImageSource::info(info)) == ERRORCODE_None)
	{
		// Not supposed to use these yet!
		info->width =
			info->height =
			info->row_bytes = -1;

		// We always have a mask.
		info->m_fHasMask = TRUE;
		info->m_fReverseMask = TRUE;
		info->m_nMaskBpp = 8;
	}
	return error;
}

/*
// The read routine for a rotated image.
*/

ERRORCODE RotatedImageSource::read(int nLine, LPBYTE pImage, int nCount, LPBYTE pMask /*=NULL*/)
{
//	TRACE("RIS: read(%d, %d)\n", nLine, nCount);

	// Handle all requested lines.
	while (nCount > 0)
	{
		// Do we have lines in the buffer to satisfy this request?
		if (nLine < m_nStripTop || nLine >= m_nStripBottom)
		{
			ERRORCODE error;

			// No, we need to refill the buffer.
			if ((error = FillTransferStrips(nLine)) != ERRORCODE_None)
			{
				return error;
			}
		}

		int nLineOffset = nLine - m_nStripTop;
		int nTake = m_nStripBottom - nLine;

		// Only take what we need.
		if (nTake > nCount)
		{
			nTake = nCount;
		}

		// Copy the data out.
		memcpy(pImage,
				 m_pImageStrip + nLineOffset*m_nImageRowBytes,
				 nTake * m_nImageRowBytes);

		memcpy(pMask,
				 m_pMaskStrip + nLineOffset*m_nMaskRowBytes,
				 nTake * m_nMaskRowBytes);

		nLine += nTake;
		nCount -= nTake;
	}
	return ERRORCODE_None;
}

/*
// The source extent routine for a rotated image.
*/

void RotatedImageSource::compute_source_extent(PPOINT extent)
{
	POINT p;

/* Get the object's rotation. */

	m_dRotation = ic->object->get_rotation();

	if (ic->do_xflip)
	{
		m_dRotation = -m_dRotation;
	}
	if (ic->do_yflip)
	{
		m_dRotation = -m_dRotation;
	}

	m_dRotation = normalize_angle(m_dRotation);

	// Handle rotations greater than PI/2 away from 0 (both directions).
	if (m_dRotation > PI/2 && m_dRotation < 3*PI/2)
	{
		ic->do_xflip = !ic->do_xflip;
		ic->do_yflip = !ic->do_yflip;
		m_dRotation -= PI;
	}

/*
// Compute the shear values.
*/

	m_dAlpha = -tan(m_dRotation/2);
	m_dBeta = sin(m_dRotation);

/*
// Compute the real (unscaled, unrotated) source bound.
*/

	ImageSource::compute_source_extent(&p);

	m_cpRealSource = p;

/*
// Compute our virtual size.
// We match the destination bound exactly.
*/

	// Compute the scaled source size (unrotated).
	PBOX unrotated_bound = ic->object->GetUnrotatedDrawBound();
	ic->rc->pbox_to_screen(&unrotated_bound, TRUE);

	PPNT DestExtent;
	DestExtent.x = unrotated_bound.x1 - unrotated_bound.x0;
	DestExtent.y = unrotated_bound.y1 - unrotated_bound.y0;
	if (DestExtent.x < p.x && DestExtent.y < p.y)
	{
		// We want to use the dest extent.
		m_cpScaledSource.x = DestExtent.x;
		m_cpScaledSource.y = DestExtent.y;

		m_cpRotatedExtent.x = ic->dest_bound.right - ic->dest_bound.left;
		m_cpRotatedExtent.y = ic->dest_bound.bottom - ic->dest_bound.top;
	}
	else
	{
		// Scaled source is the original source.
		m_cpScaledSource = m_cpRealSource;

		// Compute the rotated variant of the original source size.
		PBOX a;
		a.x0 = 0;
		a.y0 = 0;
		a.x1 = p.x;
		a.y1 = p.y;
		PBOX b;
		PPNT o;
		o.x = p.x/2;
		o.y = p.y/2;
		compute_rotated_bound(a, m_dRotation, &b, o);
		m_cpRotatedExtent.x = b.x1 - b.x0;
		m_cpRotatedExtent.y = b.y1 - b.y0;
	}

	extent->x = m_cpRotatedExtent.x;
	extent->y = m_cpRotatedExtent.y;

	m_cpRotatedCenter.x = m_cpRotatedExtent.x/2;
	m_cpRotatedCenter.y = m_cpRotatedExtent.y/2;

/*
// Compute the center offset. This determines how far off center the rotated destination
// bound will be when rotated BACK into "source coordinate space".
*/

	m_cpCenterOffset.x = (m_cpRotatedExtent.x - m_cpScaledSource.x)/2;
	m_cpCenterOffset.y = (m_cpRotatedExtent.y - m_cpScaledSource.y)/2;

//	od("Src: %d, %d; Scl src: %d, %d; Ext: %d, %d\r\n",
//					p, m_cpScaledSource, m_cpRotatedExtent);
}

/*
// Alter our render needs if necessary.
*/

ERRORCODE RotatedImageSource::compute_render_needs(void)
{
//	od("Desired source: %d, %d, %d, %d (%d, %d)\r\n", ic->srce, m_cpRotatedExtent);

	m_Srce = ic->srce;

/*
// Fixup the image info in the ImageContext so it can talk to us correctly.
// We keep ours the same so we can talk to the driver correctly.
*/

	ic->image_info.width = m_cpRotatedExtent.x;
	ic->image_info.height = m_cpRotatedExtent.y;

	m_nImageRowBytes = m_cpRotatedExtent.x*our_info.bytes_per_pixel;
	m_nMaskRowBytes = m_cpRotatedExtent.x;

	ic->image_info.row_bytes = m_nImageRowBytes;
	ic->image_info.m_nMaskRowBytes = m_nMaskRowBytes;

/*
// Initialize the image buffer.
*/

	return m_Buffer.init(&m_cpScaledSource);
}

/*
// The open routine for a rotated image.
*/

ERRORCODE RotatedImageSource::open(PALETTE_INFO_PTR palette)
{
/*
// Extract what our source range will be.
*/

	if (m_Srce.bottom < m_cpRotatedExtent.y-1)
	{
		// Handle possible round-up problems in the image DDA.
		m_Srce.bottom++;
	}

/*
// Compute the initial strip size.
*/

	m_nStripHeight = m_Srce.bottom - m_Srce.top;

//#define MAX_STRIP_BYTES	(4*1024*1024)
//#define MAX_STRIP_BYTES	(1*1024*1024)
#define MAX_STRIP_BYTES	(60000)

	int nMaxHeight = MAX_STRIP_BYTES / m_nImageRowBytes;
	if (m_nStripHeight > nMaxHeight)
	{
		m_nStripHeight = nMaxHeight;
	}

/*
// Allocate the strip.
*/

	ASSERT(m_nMaskRowBytes <= m_nImageRowBytes);
	TRY
	{
		m_pImageStrip = new BYTE[m_nStripHeight * m_nImageRowBytes];
		m_pMaskStrip = new BYTE[m_nStripHeight * m_nMaskRowBytes];
	}
	CATCH_ALL(e)
	{
		return ERRORCODE_Memory;
	}
	END_CATCH_ALL

/*
// We have the strip to use for transfer to the outside world.
*/

	m_nStripTop = 0xffff;					// Extremely large value.

	return ImageSource::open(palette);
}

/*
// The close routine for a rotated image.
*/

void RotatedImageSource::close(void)
{
	delete [] m_pImageStrip;
	m_pImageStrip = NULL;

	delete [] m_pMaskStrip;
	m_pMaskStrip = NULL;

	ImageSource::close();
}

/*
// Read a strip of scaled source data.
*/

ERRORCODE RotatedImageSource::read_source_strip(RECT r, LPBYTE pImage, LPBYTE pMask)
{
//	od("Read source strip: %d, %d, %d, %d\r\n", r);

	if (r.bottom > m_cpScaledSource.y)
	{
		r.bottom = m_cpScaledSource.y;
	}

	if (r.top < 0)
	{
		pImage += (-r.top)*m_nSourceImageRowBytes;
		pMask += (-r.top)*m_nSourceMaskRowBytes;
		r.top = 0;
	}

//	TRACE("m_Buffer.read(%d, %d, %d, %d)(%d)\n", r, r.bottom-r.top);
	return m_Buffer.read(r, pImage, pMask, m_nSourceImageRowBytes, m_nSourceMaskRowBytes);
}

/*
// Merge data into the transfer buffer.
*/

ERRORCODE RotatedImageSource::merge_rotated_data(LPBYTE pImage, LPBYTE pMask, int y_offset, int rows)
{
	LPBYTE pImageStrip = m_pImageStrip;
	LPBYTE pMaskStrip = m_pMaskStrip;
	pImage += y_offset*m_nSourceImageRowBytes;
	pMask += y_offset*m_nSourceMaskRowBytes;

	for (int i = 0; i < rows; i++)
	{
		// Copy the image data.
		LPBYTE s = pImage, d = pImageStrip;
		for (int b = 0; b < m_nImageRowBytes; b++)
		{
			*d++ |= *s++;
		}
		pImage += m_nSourceImageRowBytes;
		pImageStrip += m_nImageRowBytes;

		// Copy the mask data.
		s = pMask, d = pMaskStrip;
		for (b = 0; b < m_nMaskRowBytes; b++)
		{
			*d++ |= *s++;
		}
		pMask += m_nSourceMaskRowBytes;
		pMaskStrip += m_nMaskRowBytes;
	}

	return ERRORCODE_None;
}

/*
// Copy data into the transfer buffer.
*/

ERRORCODE RotatedImageSource::copy_rotated_data(LPBYTE pImage, LPBYTE pMask, int y_offset, int rows)
{
	LPBYTE pImageStrip = m_pImageStrip;
	LPBYTE pMaskStrip = m_pMaskStrip;
	pImage += y_offset*m_nSourceImageRowBytes;
	pMask += y_offset*m_nSourceMaskRowBytes;

	for (int i = 0; i < rows; i++)
	{
		memcpy(pImageStrip, pImage, m_nImageRowBytes);
		pImage += m_nSourceImageRowBytes;
		pImageStrip += m_nImageRowBytes;

		memcpy(pMaskStrip, pMask, m_nMaskRowBytes);
		pMask += m_nSourceMaskRowBytes;
		pMaskStrip += m_nMaskRowBytes;
	}

	return ERRORCODE_None;
}

/*
// Fill the transfer strips with however much data we can.
*/

ERRORCODE RotatedImageSource::FillTransferStrips(int nLine)
{
	ERRORCODE error;

	// Only read what we thought we were going to read.
	if (nLine >= m_Srce.bottom || nLine < m_Srce.top)
	{
		return ERRORCODE_BadParameter;
	}

	// Compute how much to read. Only read to the bottom.
	m_nStripTop = nLine;
	m_nStripBottom = nLine + m_nStripHeight;			/* Maximum possible. */
	if (m_nStripBottom > m_Srce.bottom)
	{
		m_nStripBottom = m_Srce.bottom;
	}

	int nDestRows = m_nStripBottom - m_nStripTop;

	TRACE("BEGIN FTS: %d, %d (%d)\n", m_nStripTop, m_nStripBottom, nDestRows);

	// Compute the source to read.
	PBOX Dest;
	Dest.x0 = 0;
	Dest.y0 = m_nStripTop;
	Dest.x1 = m_cpRotatedExtent.x;
	Dest.y1 = m_nStripBottom;
	PPNT Center;
	Center.x = m_cpRotatedCenter.x;
	Center.y = m_cpRotatedCenter.y;
	PBOX Source;
	compute_rotated_bound(Dest, -m_dRotation, &Source, Center);
	// Account for the change of coordinate space origins (dest to source).
	Source.x0 -= m_cpCenterOffset.x;
	Source.y0 -= m_cpCenterOffset.y;
	Source.x1 -= m_cpCenterOffset.x;
	Source.y1 -= m_cpCenterOffset.y;

	TRACE("Dest: %ld, %ld, %ld, %ld -> Source: %ld, %ld, %ld, %ld (total source: %d, %d)\n",
			Dest, Source, m_cpScaledSource.x, m_cpScaledSource.y);

/*
// Clear the strip.
*/

	int nThisStripHeight = m_nStripBottom - m_nStripTop;
	ASSERT(nThisStripHeight <= m_nStripHeight);
	memset(m_pImageStrip, 0, nThisStripHeight*m_nImageRowBytes);
	memset(m_pMaskStrip, 0, nThisStripHeight*m_nMaskRowBytes);

/*
// Calculate how many lines we need to read.
*/

	if (Source.y1 <= 0 || Source.y0 >= m_cpScaledSource.y)
	{
		return ERRORCODE_None;
	}

	if (Source.y1 > Source.y0)
	{
		// Reduce the size of the source.
		// We need to take into account the skews which can exceed
		// the size of the final rotated image.
		// We only want to worry about the source that contains actual data.

		int y0 = (Source.y0 < 0) ? 0 : Source.y0;
		int y1 = (Source.y1 > m_cpScaledSource.y) ? m_cpScaledSource.y : Source.y1;
		int cy = (Source.y0+Source.y1)/2;
		int nYDist = (y1-cy) > (cy-y0) ? y1-cy : cy-y0;

		int x0 = (Source.x0 < 0) ? 0 : Source.x0;
		int x1 = (Source.x1 > m_cpScaledSource.x) ? m_cpScaledSource.x : Source.x1;
		int cx = (Source.x0+Source.x1)/2;
		int nXDist = (x1-cx) > (cx-x0) ? x1-cx : cx-x0;

		Source.x0 = cx - nXDist;
		Source.y0 = cy - nYDist;
		Source.x1 = cx + nXDist;
		Source.y1 = cy + nYDist;

      int nXPad = skew_number(nYDist, m_dAlpha);
      if (nXPad < 0) nXPad = -nXPad;
		if (Source.x0 > -nXPad)
		{
			int nDiff = Source.x0 - (-nXPad);
			TRACE("Expand %d for left nXPad\n", nDiff);
			Source.x1 += nDiff;
			Source.x0 -= nDiff;
		}
		if (Source.x1 < m_cpScaledSource.x + nXPad)
		{
			int nDiff = m_cpScaledSource.x + nXPad - Source.x1;
			TRACE("Expand %d for right nXPad\n", nDiff);
			Source.x0 -= nDiff;
			Source.x1 += nDiff;
		}

		TRACE("FINAL Source: %ld, %ld, %ld, %ld\n", Source);

		// Compute dimensions of source rectangle.
		int nSourceWidth = Source.x1 - Source.x0;
		int nSourceHeight = Source.y1 - Source.y0;

		// Source rectangle.
		RECT r;
		r.left = Source.x0;
		r.right = Source.x1;

		// The strip width is the bigger of the two.
		int nStripWidth = (nSourceWidth > m_cpRotatedExtent.x) ? nSourceWidth : m_cpRotatedExtent.x;
		int nStripHeight = (nSourceHeight > nDestRows) ? nSourceHeight : nDestRows;

		// Compute how wide the source strip is in bytes.
		m_nSourceImageRowBytes = nStripWidth*our_info.bytes_per_pixel;
		m_nSourceMaskRowBytes = nStripWidth;

		DWORD dwSourceImageSize = (DWORD)nStripHeight*m_nSourceImageRowBytes;
		DWORD dwSourceMaskSize = (DWORD)nStripHeight*m_nSourceImageRowBytes;
		TRACE("dwSourceImageSize: %ld, dwSourceMaskSize: %ld\n", dwSourceImageSize, dwSourceMaskSize);

		HANDLE hImageMem;
		if ((hImageMem = GlobalAlloc(GMEM_MOVEABLE, dwSourceImageSize)) != NULL)
		{
			LPBYTE pImageMem = (LPBYTE)GlobalLock(hImageMem);
			HANDLE hMaskMem;
			if ((hMaskMem = GlobalAlloc(GMEM_MOVEABLE, dwSourceMaskSize)) != NULL)
			{
				LPBYTE pMaskMem = (LPBYTE)GlobalLock(hMaskMem);

				// Setup the image map.
				ROTATE_MAP ImageMap;
				ImageMap.data = pImageMem;
				ImageMap.width = nStripWidth;
				ImageMap.height = nStripHeight;
				ImageMap.bytes_per_row = m_nSourceImageRowBytes;
				ImageMap.bytes_per_pixel = our_info.bytes_per_pixel;

				ImageMap.bound.left = 0;
				ImageMap.bound.right = ImageMap.width;

				// Setup the mask map.
				ROTATE_MAP MaskMap;
				MaskMap.data = pMaskMem;
				MaskMap.width = nStripWidth;
				MaskMap.height = nStripHeight;
				MaskMap.bytes_per_row = m_nSourceMaskRowBytes;
				MaskMap.bytes_per_pixel = 1;

				MaskMap.bound.left = 0;
				MaskMap.bound.right = MaskMap.width;

				// Setup the rotate parms. This is the same for both (image and mask).
				ROTATE_PARMS parms;
				parms.center.x = nStripWidth/2;
				parms.center.y = nStripHeight/2;
				parms.angle = m_dRotation;

				// Read strips until done.
				for (r.top = Source.y0; r.top < Source.y1; r.top = r.bottom)
				{
					int strip_start = r.top;

					// Zero our source block.
					memset(pImageMem, 0, (UINT)dwSourceImageSize);
					memset(pMaskMem, 0, (UINT)dwSourceMaskSize);

					// Compute our source extent.
					// This can only kick in for the last strip (if multiple strips)
					if ((r.bottom = r.top + nStripHeight) > Source.y1)
					{
						r.bottom = Source.y1;
					}

					if (r.bottom > 0 && r.top < m_cpScaledSource.y)
					{
					/*
					// Compute rotatation parameters.
					*/

						ImageMap.bound.top = 0;
						ImageMap.bound.bottom = nStripHeight;

						MaskMap.bound.top = 0;
						MaskMap.bound.bottom = nStripHeight;

						TRACE("Map.bound: %d, %d, %d, %d; center: %d, %d\n",
								ImageMap.bound,
								parms.center);

					/*
					// Fill the buffer with the next bunch of lines.
					*/

						if (r.left < r.right)
						{
							TRACE(">>> rss: %d, %d, %d, %d; (%d, %d)", r, nSourceWidth, nSourceHeight);
							ASSERT(nStripWidth >= nSourceWidth);
							ASSERT(nStripHeight >= nSourceHeight);
							LPBYTE pImage = pImageMem
										+ (nStripHeight-nSourceHeight)/2*m_nSourceImageRowBytes
										+ (nStripWidth-nSourceWidth)/2*our_info.bytes_per_pixel;
							LPBYTE pMask = pMaskMem
										+ (nStripHeight-nSourceHeight)/2*m_nSourceMaskRowBytes
										+ (nStripWidth-nSourceWidth)/2;

							if ((error = read_source_strip(r, pImage, pMask)) != ERRORCODE_None)
							{
								return error;
							}

							// Rotate the data.
							TRACE("rotate_it; ");
							rotate_it(&ImageMap, &parms);
							rotate_it(&MaskMap, &parms);

							// Transfer the work area to the transfer strip.
							// Compute the source byte.
							ASSERT(nStripWidth >= m_cpRotatedExtent.x);
							ASSERT(nStripHeight >= nDestRows);
							LPBYTE pCopyImage = pImageMem
									+ (nStripHeight - nDestRows)/2*m_nSourceImageRowBytes
									+ (nStripWidth - m_cpRotatedExtent.x)/2*our_info.bytes_per_pixel;
							LPBYTE pCopyMask = pMaskMem
									+ (nStripHeight - nDestRows)/2*m_nSourceMaskRowBytes
									+ (nStripWidth - m_cpRotatedExtent.x)/2;
							int nCopyRows = nDestRows;
							if (r.top == Source.y0)
							{
								// First strip. Do a copy
#if 0
								if (m_nStripRowBytes == m_nSourceStripRowBytes)
								{
									UINT uThisSize = (UINT)nCopyRows*m_nStripRowBytes;
									TRACE("memcpy(%d); ", nCopyRows);
									memcpy(m_pImageStrip, source_mem, uThisSize);
								}
								else
#endif
								{
									// Do it a line at time.
									TRACE("crd(%d); ", nCopyRows);
									copy_rotated_data(pCopyImage, pCopyMask, 0, nCopyRows);
								}
							}
							else
							{
								// Later strip. Do a merge.
								TRACE("mrd(%d); ", nCopyRows);
								merge_rotated_data(pCopyImage, pCopyMask, 0, nCopyRows);
							}
							TRACE("<<<\n ");
						}
					}
				}
				GlobalUnlock(hMaskMem);
				GlobalFree(hMaskMem);
			}
			GlobalUnlock(hImageMem);
			GlobalFree(hImageMem);
		}
	}
//	TRACE(">>> FillTransferStrips (%ld, %ld): Done\n", Source.y0, Source.y1);

	return ERRORCODE_None;
}
#endif			// #ifdef DO_ROTATION
