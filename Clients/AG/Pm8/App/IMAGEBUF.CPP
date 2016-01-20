/*
//	IMAGEBUF.CPP
//
//		Copyright (C) 1993 MicroLogic Software, Inc.
//
// Image buffer routines.
//
// $Header: /PM8/App/IMAGEBUF.CPP 1     3/03/99 6:06p Gbeddow $
//
// $Log: /PM8/App/IMAGEBUF.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 3     1/13/98 11:15a Jayn
// Transparent GIF support.
// 
// 2     11/19/97 3:43p Jayn
// Rotated bitmap support.
// 
//    Rev 1.1   17 Sep 1997 09:12:32   Jay
// Bitmap rotation
// 
//    Rev 1.0   14 Aug 1997 15:21:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:46   Fred
// Initial revision.
// 
//    Rev 1.1   01 Nov 1996 17:11:40   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.0   14 Mar 1996 13:44:36   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:48   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:47:52   JAY
// Initial revision.
// 
//    Rev 1.4   30 Nov 1994 16:37:38   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.3   04 Aug 1994 07:33:58   JAY
// Large model and 32-bit changes
// 
//    Rev 1.2   31 Jan 1994 07:14:56   JAY
// Removed od('s for release version.
// 
//    Rev 1.1   05 Jan 1994 08:05:50   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:02   JAY
// 
*/

#include "stdafx.h"

#include <string.h>

#include	"pmgobj.h"
#include	"grafrec.h"
#include	"grafobj.h"
#include "image.h"
#include "utils.h"

/*
// The constructor for an image buffer.
*/

ImageBuffer::ImageBuffer(ImageDriverPtr d) : m_pDriver(d)
{
	m_pImageCache = NULL;
	m_pMaskCache = NULL;

	m_pImageRow = NULL;
	m_pMaskRow = NULL;
}

/*
// The destructor for an image buffer.
*/

ImageBuffer::~ImageBuffer()
{
	// Free the data cache.
	if (m_pImageCache != NULL)
	{
		HGLOBAL hImageCache = (HGLOBAL)GlobalHandle(m_pImageCache);
		GlobalUnlock(hImageCache);
		GlobalFree(hImageCache);
		m_pImageCache = NULL;
	}

	// Free the mask cache.
	if (m_pMaskCache != NULL)
	{
		HGLOBAL hMaskCache = (HGLOBAL)GlobalHandle(m_pMaskCache);
		GlobalUnlock(hMaskCache);
		GlobalFree(hMaskCache);
		m_pMaskCache = NULL;
	}

	// Free the image row.
	delete [] m_pImageRow;
	m_pImageRow = NULL;

	// Free the mask row.
	delete [] m_pMaskRow;
	m_pMaskRow = NULL;
}

/*
// Prepare for reading.
*/

ERRORCODE ImageBuffer::init(LPPOINT pDims /*=NULL*/, LPRECT pWindow /*=NULL*/)
{
	// Get the driver info.
	m_pDriver->info(&m_DriverInfo);

	// Compute the scaled dims.
	if (pDims == NULL)
	{
		m_cpScaledDims.x = m_DriverInfo.width;
		m_cpScaledDims.y = m_DriverInfo.height;
	}
	else
	{
		m_cpScaledDims = *pDims;
	}

	// Compute the scaled row size.
	m_nScaledImageRowBytes = m_cpScaledDims.x * m_DriverInfo.bytes_per_pixel;
	m_nScaledMaskRowBytes = m_cpScaledDims.x;

	// Set the source window.
	set_source_window(pWindow);

	// Allocate the source rows. The mask data is assumed to be a byte per pixel.
	m_nImageRowBytes = (m_DriverInfo.width*m_DriverInfo.bytes_per_pixel+3) & ~3;
	m_nMaskRowBytes = -1;
	TRY
	{
		m_pImageRow = new BYTE[m_nImageRowBytes];
		if (m_DriverInfo.m_fHasMask)
		{
			m_nMaskRowBytes = m_DriverInfo.m_nMaskRowBytes;
			m_pMaskRow = new BYTE[m_nMaskRowBytes];
		}
	}
	CATCH_ALL(e)
	{
		return ERRORCODE_Memory;
	}
	END_CATCH_ALL

	// Do we cache scaled data or original data?
	m_fCacheScaledX = (m_cpScaledDims.x <= (int)m_DriverInfo.width);
	m_fCacheScaledY = (m_cpScaledDims.y <= (int)m_DriverInfo.height);

	m_nImageCacheByteWidth = m_fCacheScaledX ? m_nScaledImageRowBytes : m_nImageRowBytes;
	m_nMaskCacheByteWidth = m_fCacheScaledX ? m_nScaledMaskRowBytes : m_nMaskRowBytes;

	return ERRORCODE_None;
}

/*
// Get the source window.
*/

void ImageBuffer::get_source_window(LPRECT window)
{
	*window = m_crSourceWindow;
}

/*
// Set the source window.
*/

void ImageBuffer::set_source_window(LPRECT pcrWindow)
{
	if (pcrWindow != NULL)
	{
		m_crSourceWindow = *pcrWindow;
	}
}

/*
// Copy source line to dest with possible (likely) scaling.
// Only the pixels from nStart to (nStart+nWidth) are copied.
*/

static void ScaleLine(LPBYTE pSource, int nSourcePixels, LPBYTE pDest, int nDestPixels, int nBytesPerPixel, int nStart, int nWidth)
{
	if (nWidth <= 0)
	{
		// Nothing to do.
		return;
	}

	int nFraction = 0;

	// Handle offset copy.
	if (nStart != 0)
	{
		pSource += nBytesPerPixel*scale_number(nStart+1, nSourcePixels, nDestPixels);
		if ((nFraction = nSourcePixels -
					mulrem(nStart+1, nSourcePixels, nDestPixels)) == nSourcePixels)
		{
			pSource -= nBytesPerPixel;
			nFraction = 0;
		}
		nFraction += nSourcePixels - nDestPixels;
	}

	//
	// Do the scale. We have separate code for 8- and 24- bit (for speed).
	//

	if (nBytesPerPixel == 3)
	{
		// 24-bit code.
		for (int nPixels = nWidth;;)
		{
			if ((nFraction += nDestPixels) >= nSourcePixels)
			{
				BYTE bPixel[3];

				bPixel[0] = pSource[0];
				bPixel[1] = pSource[1];
				bPixel[2] = pSource[2];

				while (nFraction >= nSourcePixels)
				{
					nFraction -= nSourcePixels;

					pDest[0] = bPixel[0];
					pDest[1] = bPixel[1];
					pDest[2] = bPixel[2];
					pDest += 3;

					// If we got all the pixels we wanted, it is time to leave.
					if (--nPixels == 0)
					{
						return;
					}
				}
			}
			pSource += 3;
		}
	}
	else
	{
		ASSERT(nBytesPerPixel == 1);
		// 8-bit code.
		for (int nPixels = nWidth;;)
		{
			if ((nFraction += nDestPixels) >= nSourcePixels)
			{
				BYTE bPixel = pSource[0];

				while (nFraction >= nSourcePixels)
				{
					nFraction -= nSourcePixels;

					*pDest++ = bPixel;

					// If we got all the pixels we wanted, it is time to leave.
					if (--nPixels == 0)
					{
						return;
					}
				}
			}
			pSource++;
		}
	}
}

/*
// Get a source line from somewhere.
*/

#define CACHE_BLOCK	16

ERRORCODE ImageBuffer::GetSourceLine(int nOriginalLine, RECT r, LPBYTE pImage, LPBYTE pMask)
{
/*
// "original_line" is the line number in the original image
// "r.top" is the line number of the scaled image
// "r.left" is the pixel to start copying from in scaled coordinates
// "r.right" is the number of pixels to copy in scaled coordinates
// "pImage" is the pointer for image data.
// "pMask" is the pointer to mask data. If can be non-NULL even if not receiving mask data.
*/

	BOOL fHasMask = m_DriverInfo.m_fHasMask;

	// Handle the case where r.left is negative.
	// This translates into a left of 0 with the appropriate shift into the
	// destination line.
	if (r.left < 0)
	{
		pImage += (-r.left) * m_DriverInfo.bytes_per_pixel;
		if (fHasMask)
		{
			ASSERT(pMask != NULL);
			pMask += (-r.left);
		}
		r.left = 0;
	}

	// Make sure the right is within the source extent.
	if (r.right > m_cpScaledDims.x)
	{
		r.right = m_cpScaledDims.x;
	}

	if (r.left >= r.right)
	{
		return ERRORCODE_None;
	}

	int nTestLine = m_fCacheScaledY ? r.top : nOriginalLine;

	// See if the line is in the cache.
	// This checks if we have a cache. It should not be the case that
	// we have a valid image cache without a valid mask cache (if we are
	// supposed to have one). So, we can get away with checking just the image
	// cache pointer.

	if (m_pImageCache != NULL
			&& nTestLine >= m_nTopCacheLine && nTestLine <= m_nBottomCacheLine)
	{
		// The line is in the cache.

		LPBYTE sp = GetImageLine(nTestLine);
		LPBYTE mp = NULL;
		if (fHasMask)
		{
			ASSERT(m_pMaskCache != NULL);
			mp = GetMaskLine(nTestLine);
		}

		if (m_fCacheScaledX)
		{
			// The cached line is scaled. Copy it directly to the buffer.
			// Offset for the left.
			memcpy(pImage,
					 sp + r.left*m_DriverInfo.bytes_per_pixel,
					 (r.right - r.left)*m_DriverInfo.bytes_per_pixel);
			if (mp != NULL)
			{
				memcpy(pMask, mp + r.left, (r.right - r.left));
			}
		}
		else
		{
			// We need to scale the line now.
			ScaleLine(sp, m_DriverInfo.width, pImage, m_cpScaledDims.x, 
						 m_DriverInfo.bytes_per_pixel, r.left, r.right-r.left);
			if (mp != NULL)
			{
				ScaleLine(mp, m_DriverInfo.width, pMask, m_cpScaledDims.x, 
							 1, r.left, r.right-r.left);
			}
		}
	}
	else
	{
		// The line is not in the cache.
		// We need to read it.

		ERRORCODE error;
		if ((error = m_pDriver->read(nOriginalLine, m_pImageRow, 1, m_pMaskRow)) != ERRORCODE_None)
		{
			return error;
		}

		BOOL fGotUserData = FALSE;

		// See if we can cache.
		if (m_pImageCache == NULL || nTestLine == m_nBottomCacheLine+1)
		{
			// We want to cache this line.
			for (;;)					// Either this or a goto - you decide.
			{
			/*
			// Get the place for the line to be cached.
			*/
				if (m_pImageCache == NULL)
				{
					// We need to initialize the cache.
					HGLOBAL hImageCache = GlobalAlloc(GMEM_MOVEABLE, (DWORD)CACHE_BLOCK*m_nImageCacheByteWidth);
					if (hImageCache == NULL)
					{
						// Could not allocate our cache. Ignore this.
						break;			// out of the for loop now
					}

					// Allocate the mask cache if we need one.
					if (fHasMask)
					{
						HGLOBAL hMaskCache = GlobalAlloc(GMEM_MOVEABLE, (DWORD)CACHE_BLOCK*m_nMaskCacheByteWidth);
						if (hMaskCache == NULL)
						{
							// Could not allocate our cache. Ignore this.
							// Free what we allocated so far.
							GlobalFree(hImageCache);
							break;			// out of the for loop now
						}
						m_pMaskCache = (HPBYTE)GlobalLock(hMaskCache);
					}

					m_pImageCache = (HPBYTE)GlobalLock(hImageCache);

					m_nTopCacheLine = nTestLine;
					m_nBottomCacheLine = nTestLine-1;
					m_nCacheLines = CACHE_BLOCK;
				}

			/*
			// We have a cache.
			*/

				if (nTestLine - m_nTopCacheLine >= m_nCacheLines)
				{
					ASSERT(nTestLine-m_nTopCacheLine < m_nCacheLines + CACHE_BLOCK);

					// We need to expand the block.
					HGLOBAL hImageCache = (HGLOBAL)GlobalHandle(m_pImageCache);
					GlobalUnlock(hImageCache);

					HGLOBAL hNewImage;
					if ((hNewImage = GlobalReAlloc(hImageCache,
								(DWORD)(m_nCacheLines + CACHE_BLOCK)*m_nImageCacheByteWidth,
								0)) == NULL)
					{
					/*
					// Unable to expand to include this line.
					// Try to get the memory back and leave.
					*/
						m_pImageCache = (LPBYTE)GlobalLock(hImageCache);
						break;			// out of the for loop now
					}

					// We want to keep the new image cache.
					m_pImageCache = (LPBYTE)GlobalLock(hNewImage);

					if (m_pMaskCache != NULL)
					{
						HGLOBAL hMaskCache = (HGLOBAL)GlobalHandle(m_pMaskCache);
						GlobalUnlock(hMaskCache);
						HGLOBAL hNewMask;
						if ((hNewMask = GlobalReAlloc(hMaskCache,
																 (DWORD)(m_nCacheLines + CACHE_BLOCK)*m_nMaskCacheByteWidth,
																 0)) == NULL)
						{
							// Unable to expand to include this line.
							// Try to get the memory back and leave.
							m_pMaskCache = (LPBYTE)GlobalLock(hMaskCache);
							break;			// out of the for loop now
						}
						// Grab the pointer for the new handle.
						m_pMaskCache = (LPBYTE)GlobalLock(hNewMask);
					}

					// We allocate the block. Update our line count.
					m_nCacheLines += CACHE_BLOCK;
				}

			//
			// Add the line to the cache.
			//

				LPBYTE pImageLine = GetImageLine(nTestLine);
				LPBYTE pMaskLine = NULL;
				if (m_pMaskCache != NULL)
				{
					pMaskLine = GetMaskLine(nTestLine);
				}

				if (m_fCacheScaledX)
				{
					// Scale the full line directly into the cache.
					ScaleLine(m_pImageRow, m_DriverInfo.width, pImageLine, m_cpScaledDims.x, m_DriverInfo.bytes_per_pixel, 0, m_cpScaledDims.x);
					// And copy the user data.
					memcpy(pImage,
							 pImageLine + r.left*m_DriverInfo.bytes_per_pixel,
							 (r.right - r.left)*m_DriverInfo.bytes_per_pixel);
					if (pMaskLine != NULL)
					{
						ASSERT(m_pMaskRow != NULL);
						// Scale the full line directly into the cache.
						ScaleLine(m_pMaskRow, m_DriverInfo.width, pMaskLine, m_cpScaledDims.x, 1, 0, m_cpScaledDims.x);
						// And copy the user data.
						memcpy(pMask, pMaskLine + r.left, (r.right - r.left));
					}
					// We got the user data.
					fGotUserData = TRUE;
				}
				else
				{
					// Copy the unscaled line to the cache.
					memcpy(pImageLine, m_pImageRow, m_nImageCacheByteWidth);
					if (pMaskLine != NULL)
					{
						memcpy(pMaskLine, m_pMaskRow, m_nMaskCacheByteWidth);
					}
				}

				// Update the bottom limit.
				m_nBottomCacheLine = nTestLine;

				// Success!
				break;
			}
		}

		// If we did not get the user data yet, scale it now from the source.
		if (!fGotUserData)
		{
			// No caching necessary or possible.
			// Scale the line, writing straight to the user memory.
			ScaleLine(m_pImageRow, m_DriverInfo.width, pImage, m_cpScaledDims.x, m_DriverInfo.bytes_per_pixel, r.left, r.right);
			if (fHasMask)
			{
				ASSERT(pMask != NULL);
				ASSERT(m_pMaskRow != NULL);
				ScaleLine(m_pMaskRow, m_DriverInfo.width, pMask, m_cpScaledDims.x, 1, r.left, r.right);
			}
		}
	}
	return ERRORCODE_None;
}

/*
// Read a strip into a buffer.
*/

ERRORCODE ImageBuffer::read(RECT r, LPBYTE pImage, LPBYTE pMask, int nImageRowOffset /*=0*/, int nMaskRowOffset /*=0*/)
{
	// Fix the row offset.
	if (nImageRowOffset == 0)
	{
		nImageRowOffset = m_nScaledImageRowBytes;
	}
	if (nMaskRowOffset == 0)
	{
		nMaskRowOffset = m_nScaledMaskRowBytes;
	}

	// Handle a negative r.left.
	if (r.left < 0)
	{
		pImage += (-r.left)*m_DriverInfo.bytes_per_pixel;
		pMask += (-r.left);
		r.left = 0;
	}

	// Make sure the right is within the source extent.
	if (r.right > m_cpScaledDims.x)
	{
		r.right = m_cpScaledDims.x;
	}

	if (r.right <= r.left)
	{
		return ERRORCODE_None;
	}

	r.right -= r.left;			/* Compute number of pixels. */
	int nImageCopyAmount = r.right*m_DriverInfo.bytes_per_pixel;
	int nMaskCopyAmount = r.right;

	// Compute DDA variables for source read.
	int nSrcFactor = m_DriverInfo.height;
	int nDstFactor = m_cpScaledDims.y;

	int nSourceLine;
	int nFraction;

	if (r.top == 0)
	{
		// Simple case.
		nSourceLine = 0;
		nFraction = 0;
	}
	else
	{
		nSourceLine = scale_number(r.top+1, nSrcFactor, nDstFactor);

		if ((nFraction = nDstFactor -
			mulrem(r.top+1, nSrcFactor, nDstFactor)) == nDstFactor)
		{
			nFraction = 0;
			nSourceLine--;
		}
		nFraction += nSrcFactor - nDstFactor;
	}

	while (r.top < r.bottom)
	{
		// Update the DDA.
		if ((nFraction += nDstFactor) >= nSrcFactor)
		{
			ERRORCODE error;
			BOOL fProcessed = FALSE;

			if ((error = GetSourceLine(nSourceLine, r, pImage, pMask)) != ERRORCODE_None)
			{
				return error;
			}

			// If the source does not provide a mask, provide one of our own.
			if (!m_DriverInfo.m_fHasMask)
			{
				memset(pMask, 0xff, nMaskCopyAmount);
			}
			else
			{
				if (!m_DriverInfo.m_fReverseMask)
				{
					// We come out reversed.
					for (int n = 0; n < nMaskCopyAmount; n++)
					{
						pMask[n] = ~pMask[n];
					}
				}
			}

			// If we need to copy any more lines, make copies of the previous line.
			while (pImage += nImageRowOffset,
					 pMask += nMaskRowOffset,
					 (++r.top < r.bottom && (nFraction -= nSrcFactor) >= nSrcFactor))
			{
				// Copy the scaled line.
				memcpy(pImage, pImage-nImageRowOffset, nImageCopyAmount);
				memcpy(pMask, pMask-nMaskRowOffset, nMaskCopyAmount);
			}
		}
		nSourceLine++;			// Next source line.
	}
	return ERRORCODE_None;
}
