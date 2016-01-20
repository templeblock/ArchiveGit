/*
// $Header: /PM8/App/IMAGESRC.H 1     3/03/99 6:06p Gbeddow $
//
// Image source definitions.
//
// $Log: /PM8/App/IMAGESRC.H $
// 
// 1     3/03/99 6:06p Gbeddow
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
//    Rev 1.0   14 Aug 1997 09:34:14   Fred
// Initial revision.
// 
//    Rev 1.2   05 Dec 1996 12:30:36   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.1   01 Nov 1996 17:11:52   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.0   14 Mar 1996 13:42:52   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:16   FRED
// Initial revision.
// 
//    Rev 2.1   26 Jun 1995 10:08:32   JAY
// Got rid of doubles. Now uses CFixed.
// 
//    Rev 2.0   07 Feb 1995 15:48:42   JAY
// Initial revision.
// 
//    Rev 1.0   16 Dec 1993 16:26:12   JAY
//  
*/

#ifndef __IMAGESRC_H__
#define __IMAGESRC_H__

#include "cfixed.h"

/*
// An image source is where the image context gets the image.
// The source works with the image driver to get the data; it may perform
// some operations on it before passing it to the image context's processing
// routine.
*/

class ImageSource
{
protected:
	ImageDriverPtr driver;
	IMAGE_INFO our_info;
	ImageContextPtr ic;

public:
	ImageSource(ImageDriverPtr d) : driver(d) {}

	virtual ~ImageSource() {}
	virtual ERRORCODE info(IMAGE_INFO_PTR info);
	VOID attach_image_context(ImageContextPtr the_ic) { ic = the_ic; }
	virtual VOID compute_source_extent(PPOINT extent);
	virtual ERRORCODE compute_render_needs(VOID);
	virtual ERRORCODE open(PALETTE_INFO_PTR p);
	virtual ERRORCODE read(int line, LPBYTE buffer, int count, LPBYTE pMask = NULL);
	virtual VOID close(VOID);
};

typedef ImageSource far *ImageSourcePtr;

/*
// The source for a rotated image.
*/

class RotatedImageSource : public ImageSource
{
private:
	ImageBuffer m_Buffer;
	ANGLE m_dRotation;

	double m_dAlpha;								// x shear value
	double m_dBeta;								// y shear value

	CPoint m_cpRealSource;						// extent of source image
	CPoint m_cpScaledSource;					// extent of scaled source image
	CPoint m_cpRotatedExtent;					// extent of rotated scaled source image
	CPoint m_cpRotatedCenter;					// center of rotation (extent/2)
	CPoint m_cpCenterOffset;

	RECT m_Srce;
	int m_nImageRowBytes;
	int m_nMaskRowBytes;
	LPBYTE m_pImageStrip;
	LPBYTE m_pMaskStrip;

	int m_nStripTop;							// top row in transfer strip
	int m_nStripBottom;						// bottom row in transfer strip + 1
	int m_nStripHeight;						// (allocated) in rows
	UINT m_uStripSize;						// (allocated) in bytes

	int m_nSourceImageRowBytes;			// Bytes of source image row.
	int m_nSourceMaskRowBytes;				// Bytes of source mask row.

	ERRORCODE FillTransferStrips(int line);
	ERRORCODE read_source_strip(RECT r, LPBYTE pImage, LPBYTE pMask);
	ERRORCODE merge_rotated_data(LPBYTE pImage, LPBYTE pMask, int y_offset, int rows);
	ERRORCODE copy_rotated_data(LPBYTE pImage, LPBYTE pMask, int y_offset, int rows);
public:	
	RotatedImageSource(ImageDriverPtr d);
	virtual ERRORCODE info(IMAGE_INFO_PTR info);
	virtual VOID compute_source_extent(PPOINT extent);
	virtual ERRORCODE compute_render_needs(VOID);
	virtual ERRORCODE open(PALETTE_INFO_PTR p);
	virtual ERRORCODE read(int line, LPBYTE buffer, int count, LPBYTE pMask = NULL);
	virtual VOID close(VOID);
};

#endif					// #ifndef __IMAGESRC_H__
