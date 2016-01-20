/*
// Image Utility routines.
//
// $Header: /PM8/App/ImageUtils.cpp 1     3/03/99 6:06p Gbeddow $
//
// $Log: /PM8/App/ImageUtils.cpp $
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

#include "stdafx.h"
#include "ImageUtils.h"
#include "Pegasus.h"
#include "pngimpl.h"
#include "bitmapimage.h"

IMPLEMENT_DYNAMIC(CPImageUtils, CObject)

CPImageUtils::CPImageUtils()
{
}

CPImageUtils::~CPImageUtils()
{
}

int CPImageUtils::Scale(const RBitmapImage &refSourceDib, RBitmapImage *ptrScaledDib, int scale_factor, BOOL bMakeGrayScale /*= FALSE*/)
{
	int ret_val = 0;
	CPegasus PegasusObject;

	PegasusObject.ZoomDib(refSourceDib, *ptrScaledDib, scale_factor, bMakeGrayScale);

	return(ret_val);
}

int CPImageUtils::Scale(const RBitmapImage &refSourceDib, RBitmapImage *ptrScaledDib, CSize new_size, BOOL bMakeGrayScale /*= FALSE*/)
{
	int ret_val = 0;
	CPegasus PegasusObject;

	PegasusObject.ZoomDib(refSourceDib, *ptrScaledDib, new_size, bMakeGrayScale);

	return(ret_val);
}

int CPImageUtils::ChangeColorDepth(const RBitmapImage &refSourceDib, RBitmapImage *ptrOutputDib, BYTE *ColorMap, ColorDepth color_depth)
{
	int ret_val = 0 ;
	CPegasus PegasusObject;

	PegasusObject.ChangeColorDepth(refSourceDib, *ptrOutputDib, ColorMap);

	return(ret_val);
}

int CPImageUtils::PngToDib(StorageDevice* pPngImage, RBitmapImage* pForegroundImage, RBitmapImage** ppAlphaImage /*= NULL*/)
{
	ERRORCODE error;
	ASSERT(pForegroundImage);
	if(!pForegroundImage)
		return ERRORCODE_Fail;

	CPNGReader reader;
	if(reader.Initialize(pPngImage) != ERRORCODE_None)
		return ERRORCODE_Fail;

	png_infop pInfo = reader.GetInfo();
	DWORD m_wColorType = pInfo->color_type;
	BOOL bHasMask = (m_wColorType & PNG_COLOR_MASK_ALPHA);

	//the bit depth that is read in is the bit depth of each channel;
	DWORD dwBitDepth = pInfo->bit_depth;
	USHORT nWidth = (USHORT)pInfo->width;
	USHORT nHeight = (USHORT)pInfo->height;
	
	int nChannels = png_get_channels(reader.GetStruct(), pInfo);

	//don't count the mask channel
	if (bHasMask)
	{
		nChannels--;
	}
	//we only process 32 bit png files.
	ASSERT(nChannels >= 3);
	if(nChannels < 3)
		return ERRORCODE_Fail;

	if ((error = reader.AllocateRows()) != ERRORCODE_None)
	{
		return ERRORCODE_Fail;
	}

	// Now compute the full memory requirements for a row.
	int nSrcRowSize = (nWidth * dwBitDepth * nChannels / 8 + 3) & ~3;
	int nMaskRowSize = (nWidth*dwBitDepth / 8 + 3) & ~3;


	//ASSUME data is top down and we want it bottom up. (IS THIS A VALID ASSUMPTION)  
	//To do this we init bitmap data from the bottom and move up with each row.

	//Initialize RBitmap to proper size and colordepth
	pForegroundImage->Initialize(nWidth, nHeight, dwBitDepth * nChannels);
	BYTE* pColorData = (BYTE*)pForegroundImage->GetImageData(pForegroundImage->GetRawData());
	//move to last row
	pColorData += (nHeight - 1) * nSrcRowSize;

	BYTE* pMaskData = NULL;

	if(ppAlphaImage && bHasMask)
	{
		*ppAlphaImage = new RBitmapImage;
		RBitmapImage* pTemp = *ppAlphaImage;
		//Initialize RBitmap to proper size and colordepth and set pallete to grayscale
		pTemp->Initialize(nWidth, nHeight, 8, kDefaultXDpi, kDefaultYDpi, kUseGrayScalePalette);
		pMaskData = (BYTE*)pTemp->GetImageData(pTemp->GetRawData());
		//move to last row
		pMaskData += (nHeight - 1) * nMaskRowSize;

	}

	error = reader.ReadImage();
	if (error != ERRORCODE_None)
	{
		return error;
	}
	int count = nHeight;
	int line = 0;
	// Extract the rows.
	for (int n = 0; n < count; n++)
	{
		LPBYTE pRow = reader.GetRow(line++);
		LPBYTE pThisData = pColorData;
		LPBYTE pThisMask = pMaskData;
		for (int x = 0; x < nWidth; x++)
		{
			//we have to swap the red and blue channels.  Png stores these different then dibs.
			*pThisData++ = pRow[2];
			*pThisData++ = pRow[1];
			*pThisData++ = pRow[0];
			pRow += 3;

			if(pMaskData && bHasMask)
			{
				*pThisMask++ = *pRow;
			}
			if(bHasMask)
				pRow++;
		}
		if(pMaskData)
			pMaskData -= nMaskRowSize;

		pColorData -= nSrcRowSize;
	}

	return ERRORCODE_None;
}


