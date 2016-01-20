/*
 * Name:
 *	IADMImage.hpp
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	IADMImage Class.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 11/20/96	DL	First version.
 *		Created by Dave Lazarony.
 */

#ifndef __IADMImage_hpp__
#define __IADMImage_hpp__

/*
 * Includes
 */
 
#ifndef __ADMImage__
#include "ADMImage.h"
#endif

#ifndef __IADMDrawer_hpp__
#include "IADMDrawer.hpp"
#endif

/*
 * Global Suite Pointer
 */

extern "C" ADMImageSuite *sADMImage;

/*
 * Wrapper Class
 */

class IADMImage
{
private:
	ADMImageRef fImage;

public:	
	IADMImage();
	IADMImage(ADMImageRef Image);
	IADMImage(int nilImage);

	operator ADMImageRef();

	int GetWidth();
	int GetHeight();
	int GetByteWidth();
	int GetBitsPerPixel();
	
	ASByte *BeginBaseAddressAccess();
	void EndBaseAddressAccess();
	
	IADMDrawer BeginADMDrawer();
	void EndADMDrawer();
	
	void BeginAGMImageAccess(struct _t_AGMImageRecord *imageRecord);
	void EndAGMImageAccess();
	
	ASErr GetPixel(const IASPoint &point, ASRGBColor &color);
	ASErr SetPixel(const IASPoint &point, const ASRGBColor &color);
};


inline IADMImage::IADMImage()	
{ 
	fImage = nil; 
}

inline IADMImage::IADMImage(ADMImageRef Image)		
{ 
	fImage = Image; 
}

inline IADMImage::IADMImage(int nilImage)
{
	if (nilImage == nil)
		fImage = (ADMImage *)nilImage;
}

inline IADMImage::operator ADMImageRef()				
{ 
	return fImage; 
}

inline int IADMImage::GetWidth()						
{ 
	return sADMImage->GetWidth(fImage); 
}

inline int IADMImage::GetHeight()						
{ 
	return sADMImage->GetHeight(fImage); 
}

inline int IADMImage::GetByteWidth()					
{ 
	return sADMImage->GetByteWidth(fImage); 
}

inline int IADMImage::GetBitsPerPixel()				
{ 
	return sADMImage->GetBitsPerPixel(fImage); 
}

inline ASByte *IADMImage::BeginBaseAddressAccess()				
{ 
	return sADMImage->BeginBaseAddressAccess(fImage); 
}

inline void IADMImage::EndBaseAddressAccess()				
{ 
	sADMImage->EndBaseAddressAccess(fImage); 
}

inline IADMDrawer IADMImage::BeginADMDrawer()		
{ 
	return sADMImage->BeginADMDrawer(fImage); 
}

inline void IADMImage::EndADMDrawer()		
{ 
	sADMImage->EndADMDrawer(fImage); 
}

inline void IADMImage::BeginAGMImageAccess(struct _t_AGMImageRecord *imageRecord)	
{ 
	sADMImage->BeginAGMImageAccess(fImage, imageRecord); 
}

inline void IADMImage::EndAGMImageAccess()	
{ 
	sADMImage->EndAGMImageAccess(fImage); 
}

inline ASErr IADMImage::GetPixel(const IASPoint &point, ASRGBColor &color)
{
	return sADMImage->GetPixel(fImage, (ASPoint *)&point, &color);
}

inline ASErr IADMImage::SetPixel(const IASPoint &point, const ASRGBColor &color)
{
	return sADMImage->SetPixel(fImage, (ASPoint *)&point, (ASRGBColor *)&color);
}

#endif








