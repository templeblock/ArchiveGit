/*
// Image Utility routines header file.
//
// $Header: /PM8/App/ImageUtils.h 1     3/03/99 6:06p Gbeddow $
//
// $Log: /PM8/App/ImageUtils.h $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 5     2/09/99 5:24p Mwilson
// Fixed 8 bit scale
// 
// 4     2/09/99 4:48p Mwilson
// updated to handle zooming and alpha channel
// 
// 3     2/08/99 5:52p Guidom
// Added function to scale to a specific CSize and name change of CDib to
// CPDib
// 
// 2     2/08/99 3:33p Mwilson
// added routine to spilt png file int 2 dibs.  One for Alpha channel and
// one for color data.
// 
// 1     2/08/99 2:29p Guidom
// Image utility class
// 
*/


#ifndef __IMAGE_UTILS_H
#define __IMAGE_UTILS_H

#include "CDib.h"

class CPImageUtils : public CObject {

	DECLARE_DYNAMIC(CPImageUtils)
public:
	// Construction
	CPImageUtils();
	~CPImageUtils();

	// Operations
	enum ColorDepth
	{
		colorDepth8Bit
	};

	enum ImageType
	{
		imageTypePNG
	};

	// Currently scale only supports scaling of 24-bit images..
	static int Scale(const RBitmapImage &refSourceDib, RBitmapImage *ptrScaledDib, int scale_factor, BOOL bMakeGrayScale = FALSE);
	static int Scale(const RBitmapImage &refSourceDib, RBitmapImage *ptrScaledDib, CSize new_size, BOOL bMakeGrayScale = FALSE);
	static int ChangeColorDepth(const RBitmapImage &refSourceDib, RBitmapImage *ptrOutputDib, BYTE *ColorMap, ColorDepth color_depth = colorDepth8Bit);

	//this function only works on png files that contain 24 bit color data and 8 bit Alpha channel
	static int PngToDib(StorageDevice* pPngImage, RBitmapImage* pForegroundImage, RBitmapImage** ppAlphaImage = NULL);

};

#endif // __IMAGE_UTILS_H