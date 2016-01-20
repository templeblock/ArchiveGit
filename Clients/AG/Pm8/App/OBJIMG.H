/*
// $Header: /PM8/App/OBJIMG.H 1     3/03/99 6:08p Gbeddow $
//
// Object image routines.
//
// $Log: /PM8/App/OBJIMG.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:48   Fred
// Initial revision.
// 
//    Rev 1.7   11 Dec 1996 15:56:24   Jay
// Support for masks > 64K
// 
//    Rev 1.6   09 Dec 1996 11:21:48   Jay
// Scaled PtIsOpaque now works.
// 
//    Rev 1.5   09 Dec 1996 10:19:46   Jay
// More image stuff.
// 
//    Rev 1.4   05 Dec 1996 12:30:36   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.3   05 Nov 1996 14:07:00   Jay
// Got rid of warnings, etc.
// 
//    Rev 1.2   04 Nov 1996 16:06:06   Jay
// New object image routines.
// 
//    Rev 1.1   01 Nov 1996 17:11:52   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.0   14 Mar 1996 13:44:44   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:56   FRED
// Initial revision.
// 
//    Rev 2.1   20 Dec 1995 09:55:18   JAY
// 
//    Rev 2.0   07 Feb 1995 15:48:46   JAY
// Initial revision.
// 
//    Rev 1.4   01 Mar 1994 15:50:56   JAY
// Last strip support to fix incorrect assumption about last strips.
// 
//    Rev 1.3   22 Feb 1994 15:01:56   JAY
// New rasterizing code for CGMs which uses GDI calls.
// 
//    Rev 1.2   13 Jan 1994 17:01:10   JAY
//  
// 
//    Rev 1.1   05 Jan 1994 08:08:24   JAY
// Restructuring, etc.
// 
//    Rev 1.0   30 Dec 1993 16:09:28   JAY
// Initial revision.
*/

#ifndef __OBJIMG_H__
#define __OBJIMG_H__

class TmpFile;
class PaintContext;

class CObjectImage
{
public:
	virtual ~CObjectImage() {}

/*
// Used during image recording.
*/

	virtual ERRORCODE begin_image(RedisplayContextPtr rc, PBOX bound, PBOX world) = 0;
	virtual ERRORCODE end_image(void) = 0;
	virtual BOOL making_image(void) = 0;			/* Are we making an image? */

/*
// Call this to draw a portion of the image to a redisplay context.
*/

	virtual ERRORCODE draw_image(PMGPageObjectPtr object, RedisplayContextPtr rc, PBOX extent) = 0;

/*
// Is this point in the object opaque.
*/

	virtual BOOL PtIsOpaque(PMGPageObject* pObject, RedisplayContext* rc, PPNT p) = 0;

/*
// The image file temporary name prefix.
*/

	static char image_template[5+1];

/*
// The minimum size to have on disk for images to work.
*/

	static DWORD minimum_image_space;

/*
// The maximum amount to scale an image up before regenerating it.
*/

	static int max_scale_up;

/*
// The maximum amount to scale an image down before regenerating it.
*/

	static int max_scale_down;
};

/*
// A object image saved as a bitmap (DIB or DDB).
*/

class CBitmapObjectImage : public CObjectImage
{
protected:
	friend class ObjectImageContext;
/*
// Variables.
*/

	PBOX m_OriginalBound;	/* What was the original bound? */
	PBOX m_ImageArea;			/* Bound of our portion on the screen */
	BOOL m_fRecording;		/* Are we currently recording? */
	RECT m_ImageRect;			/* Bound of the image */
	int m_nImageByteWidth;	/* Byte width of recorded image. */
	BOOL m_fMaskSource;		/* Source needs to be masked. */

	int m_nWidth;				/* Width of bitmap. */
	int m_nHeight;				/* Height of bitmap. */
	int m_nStripCount;		/* Number of strips in the image. */
	int m_nRowsPerStrip;		/* Number of rows per (non-last) strip. */
	int m_nLastStrip;			/* Number of rows in the last strip. */
	uint16 m_uRowSize;		/* Size in bytes of a row (counting planes). */

	TmpFile* m_pFile;

	int m_nMaskByteWidth;
	ULONG m_ulMaskOffset;		/* Offset to the mask (-1 if none). */
	ULONG m_ulImageOffset;		/* Offset to the image data. */
	RGBQUAD m_MaskColor[2];		// What is the zero color for the mask data?

	void FreeFile(void);
	BOOL PreparePaintContext(PaintContext& oic, PMGPageObject* pObject, RedisplayContext* rc);

	BOOL m_fHasMask;				// Does the object have a mask?
	LPBYTE m_pMask;				// The in-memory mask. Used for hit-testing, etc.
#ifndef WIN32
	BOOL m_fHugeMask;
#endif

public:

	CBitmapObjectImage();
	~CBitmapObjectImage();

/*
// Used during image recording.
*/

	virtual ERRORCODE begin_image(RedisplayContextPtr rc, PBOX bound, PBOX world);
	virtual ERRORCODE end_image(void);
	virtual BOOL making_image(void);				/* Are we making an image? */

/*
// Call this to draw a portion of the image to a redisplay context.
*/

	virtual ERRORCODE draw_image(PMGPageObjectPtr object, RedisplayContextPtr rc, PBOX extent);

/*
// Is this point in the object opaque.
*/

	virtual BOOL PtIsOpaque(PMGPageObject* pObject, RedisplayContext* rc, PPNT p);

/*
// Call this to write some image data to the file when recording.
*/

	ERRORCODE record_image(int nTop, int nLines, LPBYTE s, LPBYTE mask);

/*
// Record a Device Dependent Bitmap image.
*/

	ERRORCODE record_ddb(HBITMAP hBitmap, HBITMAP hMask, BOOL record_mask);
};

#endif					// #ifndef __OBJIMG_H__
