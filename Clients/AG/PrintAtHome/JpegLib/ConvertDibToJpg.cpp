#include "windows.h"
#include "stdio.h"
#include "ConvertDibToJpg.h"
//#include "AGDib.h"

/////////////////////////////////////////////////////////////////////////////
bool ConvertDibToJpg(BITMAPINFOHEADER* pDIB, int nQuality/*(0-100)*/, LPCSTR strFileName)
{
	if (!pDIB || !strFileName || !(*strFileName))
		return false;

	if (nQuality < 0)
		nQuality = 0;
	if (nQuality > 100)
		nQuality = 100;

	// Use libjpeg functions to write scanlines to disk in JPEG format
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr); //Use default error handling (ugly!)

	jpeg_create_compress(&cinfo);

	FILE* pOutFile;
	errno_t err = fopen_s(&pOutFile, strFileName, "wb");
	if (!pOutFile || err !=0)
	{
		jpeg_destroy_compress(&cinfo);
		return false;
	}

	jpeg_stdio_dest(&cinfo, pOutFile);

	cinfo.image_width = pDIB->biWidth; //Image width and height, in pixels 
	cinfo.image_height = pDIB->biHeight;
	cinfo.input_components = 3; //Color components per pixel (RGB_PIXELSIZE -	see	jmorecfg.h)
	cinfo.in_color_space = JCS_RGB;	//Colorspace of input image

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, nQuality, true/*Limit to baseline JPEG values*/);
	jpeg_start_compress(&cinfo, true);

	// JSAMPLEs per row in output buffer
	int nSampsPerRow = cinfo.image_width * cinfo.input_components; 

	// Allocate array of pixel RGB values for JPEG file
	JSAMPARRAY jsmpArray;
	jsmpArray = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, nSampsPerRow, cinfo.image_height);

	bool bOK = DibToRGBArray(pDIB, nSampsPerRow, cinfo, jsmpArray);
	if (bOK)
		jpeg_write_scanlines(&cinfo, jsmpArray, cinfo.image_height);

	jpeg_finish_compress(&cinfo);

	fclose(pOutFile);

	jpeg_destroy_compress(&cinfo); // Free resources

	return bOK;
}

/////////////////////////////////////////////////////////////////////////////
//This function fills an RGB array values from the DIB - data in the jsamp array must be arranged top to bottom.
bool DibToRGBArray(BITMAPINFOHEADER* pDIB, int nSampsPerRow, struct jpeg_compress_struct cinfo, JSAMPARRAY jsmpPixels)
{
	if (!pDIB || nSampsPerRow <= 0)
		return false; 

	int nCTEntries = 0;
	switch (pDIB->biBitCount)
	{
		case 1:
			nCTEntries = 2; //Monochrome
			break;

		case 4:
			nCTEntries = 16; //16-color
			break;

		case 8:
			nCTEntries = 256; //256-color
			break;

		case 16:
		case 24:
		case 32:
			nCTEntries = 0; //No color table needed
			break;

		default:
			return false; //Unsupported format
	}

	// Point to the color table and pixels
	DWORD* dwCTab = (DWORD*)pDIB + pDIB->biSize;
	LPRGBQUAD pCTab = (LPRGBQUAD)(dwCTab);
	LPSTR lpBits = (LPSTR)pDIB + (WORD)pDIB->biSize + (WORD)(nCTEntries * sizeof(RGBQUAD));

	// Different formats for the image bits
	LPBYTE lpPixels = (LPBYTE)lpBits;
	RGBQUAD* pRgbQs = (RGBQUAD*)lpBits;
	WORD* wPixels = (WORD*)lpBits;

	// Set up the jsamps according to the bitmap's format.
	// Note that rows are processed bottom to top, because that's how bitmaps are created.
	switch (pDIB->biBitCount)
	{
		case 1:
		{
			int nUsed = (pDIB->biWidth + 7) / 8;
			int nUnused = (((nUsed + 3) / 4) * 4) - nUsed;
			int nBytesWide = nUsed + nUnused;
			int nLastBits = 8 - ((nUsed * 8) - pDIB->biWidth);

			for (int r=0; r < pDIB->biHeight; r++)
			{
				for (int p=0,q=0; p < nUsed; p++) 
				{ 
					int nRow = (pDIB->biHeight-r-1) * nBytesWide;
					int nByte = nRow + p;

					int nBUsed = (p <(nUsed - 1)) ? 8 : nLastBits;
					for (int b=0; b < nBUsed; b++) 
					{ 
						BYTE bytCTEnt = lpPixels[nByte] << b; 
						bytCTEnt = bytCTEnt >> 7;
						jsmpPixels[r][q+0] = pCTab[bytCTEnt].rgbRed;
						jsmpPixels[r][q+1] = pCTab[bytCTEnt].rgbGreen;
						jsmpPixels[r][q+2] = pCTab[bytCTEnt].rgbBlue;
						q += 3;
					}
				}
			}
			break;
		}

		case 4:
		{
			int nUsed = (pDIB->biWidth + 1) / 2;
			int nUnused = (((nUsed + 3) / 4) * 4) - nUsed;
			int nBytesWide = nUsed + nUnused;
			int nLastNibs = 2 - ((nUsed * 2) - pDIB->biWidth);

			for (int r=0; r < pDIB->biHeight; r++)
			{
				for (int p=0,q=0; p < nUsed; p++) 
				{ 
					int nRow = (pDIB->biHeight-r-1) * nBytesWide;
					int nByte = nRow + p;

					int nNibbles = (p < nUsed - 1) ? 2 : nLastNibs;
					for (int n=0; n < nNibbles; n++)
					{
						BYTE bytCTEnt = lpPixels[nByte] << (n*4);
						bytCTEnt = bytCTEnt >> (4-(n*4));
						jsmpPixels[r][q+0] = pCTab[bytCTEnt].rgbRed;
						jsmpPixels[r][q+1] = pCTab[bytCTEnt].rgbGreen;
						jsmpPixels[r][q+2] = pCTab[bytCTEnt].rgbBlue;
						q += 3;
					}
				}
			}
			break;
		}

		default:
		case 8:
		{
			int nUnused = (((pDIB->biWidth + 3) / 4) * 4) - pDIB->biWidth;

			for (int r=0; r < pDIB->biHeight; r++)
			{
				for (int p=0,q=0; p < pDIB->biWidth; p++,q+=3)
				{
					int nRow = (pDIB->biHeight-r-1) * (pDIB->biWidth + nUnused);
					int nPixel = nRow + p;
					jsmpPixels[r][q+0] = pCTab[lpPixels[nPixel]].rgbRed;
					jsmpPixels[r][q+1] = pCTab[lpPixels[nPixel]].rgbGreen;
					jsmpPixels[r][q+2] = pCTab[lpPixels[nPixel]].rgbBlue;
				}
			}
			break;
		}

		case 16:
		{
			int nBytesWide = (pDIB->biWidth);
			int nUnused = (((nBytesWide + 1) / 2) * 2) - nBytesWide;
			nBytesWide += nUnused;
			for (int r=0; r < pDIB->biHeight; r++)
			{
				for (int p=0,q=0; p < nBytesWide - nUnused; p++,q+=3)
				{
					int nRow = (pDIB->biHeight-r-1) * nBytesWide;
					int nPixel = nRow + p;

					WORD b16 = wPixels[nPixel];
					WORD wR = b16; wR <<=  1; wR >>= 11;
					WORD wG = b16; wG <<=  6; wG >>= 11;
					WORD wB = b16; wB <<= 11; wB >>= 11;

					BYTE bytVals[] = {	  0, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96,104,112,120,128,
										136,144,152,160,168,176,184,192,200,208,216,224,232,240,248,255};

					jsmpPixels[r][q+0] = bytVals[wR];
					jsmpPixels[r][q+1] = bytVals[wG];
					jsmpPixels[r][q+2] = bytVals[wB];
				}
			}
			break;
		}

		case 24:
		{
			int nBytesWide = (pDIB->biWidth*3);
			int nUnused = (((nBytesWide + 3) / 4) * 4) - nBytesWide;
			nBytesWide += nUnused;

			for (int r=0; r < pDIB->biHeight; r++)
			{
				for (int p=0,q=0; p < nBytesWide - nUnused; p+=3,q+=3)
				{ 
					int nRow = (pDIB->biHeight-r-1) * nBytesWide;
					int nPixel = nRow + p;
					jsmpPixels[r][q+0] = lpPixels[nPixel+2]; //Red
					jsmpPixels[r][q+1] = lpPixels[nPixel+1]; //Green
					jsmpPixels[r][q+2] = lpPixels[nPixel+0]; //Blue
				}
			}
			break;
		}

		case 32:
		{
			for (int r=0; r < pDIB->biHeight; r++)
			{
				for (int p=0,q=0; p < pDIB->biWidth; p++,q+=3)
				{
					int nRow = (pDIB->biHeight-r-1) * pDIB->biWidth;
					int nPixel = nRow + p;
					jsmpPixels[r][q+0] = pRgbQs[nPixel].rgbRed;
					jsmpPixels[r][q+1] = pRgbQs[nPixel].rgbGreen;
					jsmpPixels[r][q+2] = pRgbQs[nPixel].rgbBlue;
				}
			}
			break;
		}
	}

	return true;
}
