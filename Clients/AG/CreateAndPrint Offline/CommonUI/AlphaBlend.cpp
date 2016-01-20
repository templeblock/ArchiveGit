#include "stdafx.h"

/*********************************************************************\ 
*																	  *
*  Create24BPPDIBSection(HDC, int, int);							  *
*																	  *
*  Purpose: 														  *
*	   Creates a 24 bit-per-pixel DIB section with the specified	  *
*	   width and height.											  *
*																	  *
\*********************************************************************/ 
HBITMAP Create24BPPDIBSection(HDC hDC, int iWidth, int iHeight)
{
	// Initialize header to 0s.
	BITMAPINFO bmi;
	ZeroMemory(&bmi, sizeof(bmi));

	// Fill out the fields you care about.
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = iWidth;
	bmi.bmiHeader.biHeight = iHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;

	// Create the surface.
	BYTE* pBits = NULL;
	return ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (void**)&pBits, NULL, 0);
}

/*********************************************************************\ 
*																	  *
*  BitmapsCompatible(LPBITMAP, LPBITMAP)							  *
*																	  *
*  Purpose: 														  *
*	   Compares two bitmap structures to see if the surfaces are	  *
*	   the same width, height, and pixel format.					   *
*																	  *
\*********************************************************************/ 
bool BitmapsCompatible(LPBITMAP lpbm1, LPBITMAP lpbm2)
{
	if (lpbm1->bmBitsPixel != lpbm2->bmBitsPixel) return false;
	if (lpbm1->bmPlanes    != lpbm2->bmPlanes)	  return false;
	if (lpbm1->bmWidth	   != lpbm2->bmWidth)	  return false;
	if (lpbm1->bmHeight    != lpbm2->bmHeight)	  return false;

	return true;
}

/*********************************************************************\ 
*																	  *
*  BOOL BlendImages(HBITMAP, HBITMAP, HBITMAP, DWORD);				  *
*																	  *
*  Purpose: 														  *
*	   Blends two source images into a destination image			  *
*	   using a specified weighting value for the first source		  *
*	   image.														  *
*																	  *
*  Notes:															  *
*	   All HBITMAP parameters must be 24 bit-per-pixel DIB sections.   *
*	   All HBITMAP parameters must have the same widths and heights.   *
*	   Weighting values must be in the range of 0 .. 255.			   *
*																	  *
\*********************************************************************/ 
bool BlendImages(HBITMAP hbmSrc1, HBITMAP hbmSrc2, HBITMAP hbmDst, DWORD dwWeight1)
{
	// Only values between 0 and 255 are valid.
	if (dwWeight1 > 255)
		return false;
	
	// Get weighting value for second source image.
	DWORD dwWeight2 = 255 - dwWeight1;
	
	// Get information about the surfaces you were passed.
	BITMAP bmSrc1, bmSrc2, bmDst;
	if (!GetObject(hbmSrc1, sizeof(BITMAP), &bmSrc1)) return false;
	if (!GetObject(hbmSrc2, sizeof(BITMAP), &bmSrc2)) return false;
	if (!GetObject(hbmDst,	sizeof(BITMAP), &bmDst))  return false;

	// Make sure you have data that meets your requirements.
	if (!BitmapsCompatible(&bmSrc1, &bmSrc2)) return false;
	if (!BitmapsCompatible(&bmSrc1, &bmDst))  return false;
	if (bmSrc1.bmBitsPixel != 24) return false;
	if (bmSrc1.bmPlanes != 1) return false;
	if (!bmSrc1.bmBits || !bmSrc2.bmBits || !bmDst.bmBits) return false;

	DWORD dwWidthBytes = bmDst.bmWidthBytes;
	
	// Initialize the surface pointers.
	RGBTRIPLE* lprgbSrc1 = (RGBTRIPLE*)bmSrc1.bmBits;
	RGBTRIPLE* lprgbSrc2 = (RGBTRIPLE*)bmSrc2.bmBits;
	RGBTRIPLE* lprgbDst  = (RGBTRIPLE*)bmDst.bmBits;

	for (int y=0; y<bmDst.bmHeight; y++)
	{
		for (int x=0; x<bmDst.bmWidth; x++)
		{
			lprgbDst[x].rgbtRed   = (BYTE)((((DWORD)lprgbSrc1[x].rgbtRed * dwWeight1) + 
											((DWORD)lprgbSrc2[x].rgbtRed * dwWeight2)) >> 8);
			lprgbDst[x].rgbtGreen = (BYTE)((((DWORD)lprgbSrc1[x].rgbtGreen * dwWeight1) + 
											((DWORD)lprgbSrc2[x].rgbtGreen * dwWeight2)) >> 8);
			lprgbDst[x].rgbtBlue  = (BYTE)((((DWORD)lprgbSrc1[x].rgbtBlue * dwWeight1) + 
											((DWORD)lprgbSrc2[x].rgbtBlue * dwWeight2)) >> 8);
		}

		// Move to next scan line.
		lprgbSrc1 = (RGBTRIPLE *)((LPBYTE)lprgbSrc1 + dwWidthBytes);
		lprgbSrc2 = (RGBTRIPLE *)((LPBYTE)lprgbSrc2 + dwWidthBytes);
		lprgbDst  = (RGBTRIPLE *)((LPBYTE)lprgbDst	+ dwWidthBytes);
	}

	return true;
}

/*********************************************************************\ 
*																	  *
*  BOOL DoAlphaBlend()												  *
*																	  *
*  Purpose: 														  *
*	   Captures a copy of the source and destination areas and		  *
*	   alpha blends them into a memory surface that it displays 	  *
*	   into the destination area.									  * 	*																	  *    
*  Notes:															  *
*	   Takes the same parameters as the AlphaBlend function except	  *
*	   that the last parameter is a source weighting value rather	  *
*	   than a BLENDFUNCTION structure.								  *
*																	  *
\*********************************************************************/ 
BOOL DoAlphaBlend(
		HDC hdcDest,				  // Handle to destination DC.
		int nXOriginDest, 		   // X-coord of upper-left corner.
		int nYOriginDest, 		   // Y-coord of upper-left corner.
		int nWidthDest,			   // Destination width.
		int nHeightDest,			   // Destination height.
		HDC hdcSrc,				   // Handle to source DC.
		int nXOriginSrc,			   // X-coord of upper-left corner.
		int nYOriginSrc,			   // Y-coord of upper-left corner.
		int nWidthSrc,			   // Source width.
		int nHeightSrc,			   // Source height.
		DWORD dwSourceWeight) 	   // Source weighting (between 0 and 255).
{
	HBITMAP hbmSrc1 = NULL;
	HBITMAP hbmSrc2 = NULL;
	HBITMAP hbmDst = NULL;
	HDC hdcSrc1 = NULL;
	HDC hdcSrc2 = NULL;
	HDC hdcDst = NULL;

	do
	{ // a handy block to break from

		// Create surfaces for sources and destination images.
		hbmSrc1 = Create24BPPDIBSection(hdcDest, nWidthDest,nHeightDest);
		if (!hbmSrc1) break;

		hbmSrc2 = Create24BPPDIBSection(hdcDest, nWidthDest,nHeightDest);
		if (!hbmSrc2) break;

		hbmDst = Create24BPPDIBSection(hdcDest, nWidthDest,nHeightDest);
		if (!hbmDst) break;
		
		// Create HDCs to hold our surfaces.
		hdcSrc1 = CreateCompatibleDC(hdcDest);
		if (!hdcSrc1) break;
		
		hdcSrc2 = CreateCompatibleDC(hdcDest);
		if (!hdcSrc2) break;
		
		hdcDst = CreateCompatibleDC(hdcDest);
		if (!hdcDst) break;
		
		// Prepare the surfaces for drawing.
		SelectObject(hdcSrc1, hbmSrc1);
		SelectObject(hdcSrc2, hbmSrc2);
		SelectObject(hdcDst,  hbmDst);
		SetStretchBltMode(hdcSrc1, COLORONCOLOR);
		SetStretchBltMode(hdcSrc2, COLORONCOLOR);

		// Capture a copy of the source area.
		if (!StretchBlt(hdcSrc1, 0,0,nWidthDest,nHeightDest, hdcSrc,  nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, SRCCOPY))
			 break;
		
		// Capture a copy of the destination area.
		if (!StretchBlt(hdcSrc2, 0,0,nWidthDest,nHeightDest, hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, SRCCOPY))
			 break;

		// Blend the two source areas to create the destination image.
		bool bOK = BlendImages(hbmSrc1, hbmSrc2, hbmDst, dwSourceWeight);

		// Clean up objects you do not need any longer.
		// You cannot delete an object that's selected into an
		// HDC so delete the HDC first. 
		DeleteDC(hdcSrc1);
		DeleteDC(hdcSrc2);
		DeleteObject(hbmSrc1);
		DeleteObject(hbmSrc2);
		
		// Display the blended (destination) image to the target HDC.
		if (bOK)
			BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, hdcDst, 0,0, SRCCOPY);
		
		// Clean up the rest of the objects you created.
		DeleteDC(hdcDst);
		DeleteObject(hbmDst);

		return bOK;
	} while (0);

	if (hdcSrc1) DeleteDC(hdcSrc1);
	if (hdcSrc2) DeleteDC(hdcSrc2);
	if (hdcDst)  DeleteDC(hdcDst);
	if (hbmSrc1) DeleteObject(hbmSrc1);
	if (hbmSrc2) DeleteObject(hbmSrc2);
	if (hbmDst)  DeleteObject(hbmDst);

	return false;
}
