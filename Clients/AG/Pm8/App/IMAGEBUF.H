/*
// $Header: /PM8/App/IMAGEBUF.H 1     3/03/99 6:06p Gbeddow $
//
// Image buffer definitions.
//
// $Log: /PM8/App/IMAGEBUF.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 2     11/19/97 3:43p Jayn
// Rotated bitmap support.
// 
//    Rev 1.0   14 Aug 1997 15:21:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:12   Fred
// Initial revision.
// 
//    Rev 1.2   01 Nov 1996 17:11:52   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.1   15 May 1996 14:26:56   Jay
// Now supports interlaced GIF files.
// 
//    Rev 1.0   14 Mar 1996 13:47:04   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:04   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:42   JAY
// Initial revision.
// 
//    Rev 1.1   04 Aug 1994 07:32:42   JAY
// Large model and 32-bit changes
// 
//    Rev 1.0   16 Dec 1993 16:25:42   JAY
//  
*/

#ifndef __IMAGEBUF_H__
#define __IMAGEBUF_H__

typedef BYTE __HUGE *HPBYTE;

class ImageBuffer
{
private:
	ImageDriverPtr m_pDriver;
	IMAGE_INFO m_DriverInfo;
	CRect m_crSourceWindow;

	int m_nImageRowBytes;
	int m_nMaskRowBytes;
	LPBYTE m_pImageRow;
	LPBYTE m_pMaskRow;

	CPoint m_cpScaledDims;
	int m_nScaledImageRowBytes;
	int m_nScaledMaskRowBytes;

	BOOL m_fCacheScaledX;			// Do we cache scaled data or original data?
	BOOL m_fCacheScaledY;			// Do we cache scaled data or original data?

	LPBYTE m_pImageCache;
	LPBYTE m_pMaskCache;

	int m_nTopCacheLine;			// First line in the cache.
	int m_nBottomCacheLine;		// Last line in the cache.
	int m_nCacheLines;			// how many lines are physically allocated

	int m_nImageCacheByteWidth;		// How many bytes across is the data cache?
	int m_nMaskCacheByteWidth;		// How many bytes across is the mask cache?

protected:
	LPBYTE GetImageLine(int nLine)
		{ return m_pImageCache + (nLine-m_nTopCacheLine)*m_nImageCacheByteWidth; }
	LPBYTE GetMaskLine(int nLine)
		{ return m_pMaskCache + (nLine-m_nTopCacheLine)*m_nMaskCacheByteWidth; }

public:
	ImageBuffer(ImageDriverPtr d);
	~ImageBuffer();
	void get_source_window(LPRECT window);

/*
// Set the source window.
// This determines how large the source row will be.
*/

	void set_source_window(LPRECT window);

/*
// Initialize the image buffer.
// This tells the image buffer what scaling factor to use for the source
// as well as what part of this scaled source we will remain within.
*/

	ERRORCODE init(LPPOINT scaled_dims = NULL, LPRECT desired_window = NULL);

/*
// Read a strip into a buffer.
*/

	ERRORCODE read(RECT r, LPBYTE pImage, LPBYTE pMask, int nImageRowoffset = 0, int nMaskRowoffset = 0);

/*
// Special-case line access routine.
*/

	ERRORCODE GetSourceLine(int nOriginalLine, RECT scaled_r, LPBYTE pImage, LPBYTE pMask = NULL);
};

typedef ImageBuffer far *ImageBufferPtr;

#endif					// #ifndef __IMAGEBUF_H__
