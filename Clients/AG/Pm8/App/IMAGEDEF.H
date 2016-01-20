/*
// $Header: /PM8/App/IMAGEDEF.H 1     3/03/99 6:06p Gbeddow $
//
// Image type definitions.
//
// $Log: /PM8/App/IMAGEDEF.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 2     8/18/98 7:01p Jayn
// Made CreateImage work with palette images.
// 
//    Rev 1.0   14 Aug 1997 15:21:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:12   Fred
// Initial revision.
// 
//    Rev 1.4   11 Apr 1997 11:23:46   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.3   05 Dec 1996 12:30:34   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.2   01 Nov 1996 17:11:52   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.1   10 May 1996 16:00:10   Jay
// New compression method.
// 
//    Rev 1.0   14 Mar 1996 13:45:52   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:08   FRED
// Initial revision.
// 
//    Rev 1.1   18 Aug 1995 15:06:02   JAY
// Photo CD; graphic images, etc.
// 
//    Rev 1.0   29 Mar 1995 09:00:02   JAY
// Initial revision.
// 
*/

#ifndef __IMAGEDEF_H__
#define __IMAGEDEF_H__

/*
// Image types.
*/

typedef enum
{
	IMAGE_BILEVEL,							/* 1 bit per pixel (1x1) */
	IMAGE_PALETTE,							/* n bits per pixel (nx1) */
	IMAGE_GRAYSCALE = IMAGE_PALETTE,
	IMAGE_PLANAR = IMAGE_PALETTE,		/* n bits per pixel (1xn) */
	IMAGE_FULL_COLOR,						/* 24 bits per pixel (8x3) */
	IMAGE_OUTLINE
} IMAGE_TYPE;

/*
// An image info structure.
*/

struct IMAGE_INFO
{
	IMAGE_INFO();
/*
// Standard header.
*/

	IMAGE_TYPE	image_type;			/* what type of image is this? */
	CHAR			bpp;					/* actual bits per pixel of file data */
	CHAR			planes;				/* number of planes */
	int			red_index;			/* BMP stores BGR instead of RGB. */
	BOOL			do_yflip;			/* Image is stored upside-down. */
	UINT			width;				/* Width in pixels. */
	UINT			height;				/* Height in lines. */
	BOOL			m_fHasMask;			/* Does this image have a mask? */
	int			m_nMaskBpp;			/* Bits per pixel for mask data */
	BOOL			m_fReverseMask;	/* We need to reverse the mask. */

/* Computed by info() method of ImageDriver. */

	int			bytes_per_pixel;	/* 1 or 3 */
	int			row_bytes;			/* size of a source row in bytes. */
	int			colors;				/* Computed from bpp and planes. */
	int			m_nMaskRowBytes;	/* size of a mask row in bytes. */
};

typedef IMAGE_INFO far *IMAGE_INFO_PTR;

/*
// Use by graphic image code.
*/

struct CGraphicImageHeader
{
	DWORD m_dwBandCount;				// Will be < 65536 for 16-bit version.
	DWORD m_dwBandHeight;			// Will be < 65536 for 16-bit version.
	DWORD m_dwBandSize;				// Will be < 65536 for 16-bit version.
	WORD m_wFlags;						// Not currently used.
	IMAGE_INFO m_Info;				// Image info used for this graphic./
	DWORD m_dwOffset[1];				// Offset in "file".

	enum
	{
		FLAG_DeflateCompression = 1
	};
};

#endif					// #ifndef __IMAGEDEF_H__
