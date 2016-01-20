#include "windows.h"
#include "PngToDib.h"
#include "Dib.h"

/////////////////////////////////////////////////////////////////////////////
bool CPng::IsValidPNGFile(LPCSTR strFileName)
{
	FILE* fp = fopen(strFileName, "rb");
	if (!fp)
		return false;
			
	// Read in some of the signature bytes
	BYTE buffer[PNG_BYTES_TO_CHECK];
	DWORD dwBytesRead = fread(buffer, 1, PNG_BYTES_TO_CHECK, fp);
	fclose(fp);

	if (dwBytesRead != PNG_BYTES_TO_CHECK)
		return false;

	// Compare the first PNG_BYTES_TO_CHECK bytes of the signature.
	return !png_sig_cmp(buffer, 0/*start*/, PNG_BYTES_TO_CHECK);
}

/////////////////////////////////////////////////////////////////////////////
COLORREF CPng::GetPngTransparentColor(LPCSTR strFileName, bool bDefaultToUpperLeft)
{
	HGLOBAL hMemory = CPng::ReadPNG(strFileName, bDefaultToUpperLeft);
	if (!hMemory)
		return CLR_NONE;

	// Do we have a bitmap file header preceding the DIB?
	char* pMemory = (char*)::GlobalLock(hMemory);
	BITMAPFILEHEADER* pBMPFile = (BITMAPFILEHEADER*)pMemory;
	#define BFT_BITMAP	0x4d42	// 'BM'
	if (pBMPFile->bfType == BFT_BITMAP)
		pMemory += sizeof(BITMAPFILEHEADER);

	COLORREF TransparentColor = CLR_NONE;
	BITMAPINFOHEADER* pDib = (BITMAPINFOHEADER*)pMemory;

	bool bHasTransparentColor = ((pDib->biClrImportant & 0xFF000000) == 0xFF000000);
	if (bHasTransparentColor)
		TransparentColor = pDib->biClrImportant & 0x00FFFFFF;
	pDib->biClrImportant = 0;

	::GlobalUnlock(hMemory);
	::GlobalFree(hMemory);

	return TransparentColor;
}

/////////////////////////////////////////////////////////////////////////////
HGLOBAL CPng::ReadPNG(LPCSTR strFileName, bool bDefaultToUpperLeft)
{
	FILE* fp = fopen(strFileName, "rb");
	if (!fp)
		return NULL;

	// Read in some of the signature bytes
	BYTE buffer[PNG_BYTES_TO_CHECK];
	DWORD dwBytesRead = fread(buffer, 1, PNG_BYTES_TO_CHECK, fp);

	if (dwBytesRead != PNG_BYTES_TO_CHECK)
		return NULL;

	// Compare the first PNG_BYTES_TO_CHECK bytes of the signature.
	if (png_sig_cmp(buffer, 0/*start*/, PNG_BYTES_TO_CHECK))
		return NULL;

	// Create and initialize the png_struct with the desired error handler functions.
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		fclose(fp);
		return NULL;
	}

	// Allocate/initialize the memory for image information.
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		fclose(fp);
		return NULL;
	}

	// Set error handling
	if (setjmp(png_jmpbuf(png_ptr)))
	{ // This is the exception handler
		//Free all of the memory associated with the png_ptr and info_ptr 
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(fp);
		return NULL;
	}

	png_init_io(png_ptr, fp);

	// We have already read some of the signature 
	png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);

	HGLOBAL hMemory = PNGToDIB(png_ptr, info_ptr, bDefaultToUpperLeft);

	// Clean up after the read, and free any memory allocated
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

	fclose(fp);

	return hMemory;
}

/////////////////////////////////////////////////////////////////////////////
HGLOBAL CPng::PNGToDIB(png_structp png_ptr, png_infop info_ptr, bool bDefaultToUpperLeft)
{
	// Read the image
	png_read_info(png_ptr, info_ptr);

	int iBitDepth = 0;
	int iColorType = 0;
	png_uint_32 dwWidth = 0;
	png_uint_32 dwHeight = 0;
	png_get_IHDR(png_ptr, info_ptr, &dwWidth, &dwHeight, &iBitDepth, &iColorType, NULL, NULL, NULL);

	IMAGE Image;
	Image.dwWidth = dwWidth;
	Image.dwHeight = dwHeight;
	Image.dwResolution = png_get_pixels_per_meter(png_ptr, info_ptr);
	if (!Image.dwResolution)
		Image.dwResolution = 3937; // 100 pixels/inch

	// Determine the type of format
	if (iColorType == PNG_COLOR_TYPE_RGB || iColorType == PNG_COLOR_TYPE_RGB_ALPHA)
	{
		Image.wBitDepth = 24; // iBitDepth 8, 16
		Image.dwPaletteSize = 0;
	}
	else
	{
		switch (iBitDepth) // bit depths 1, 2, 4, 8, 16
		{	
			case 2:  
				Image.wBitDepth = 4;		
				break;
			case 16: 
				Image.wBitDepth = 8;		
				break;
			default: 
				Image.wBitDepth = iBitDepth;
		}

		Image.dwPaletteSize = 1 << Image.wBitDepth;
	}

	// Allocate the image struct that will be used to transport file info
	if (!AllocImageStruct(Image))
		longjmp(png_ptr->jmpbuf, 1);

	// Not sure if we can take advantage of this info in a BMP, thus remove it.
	bool bHasAlpha = !!(iColorType & PNG_COLOR_MASK_ALPHA);
	if (bHasAlpha)
		png_set_strip_alpha(png_ptr);

	// Set user transform function to deal with a Bitdepth of 2
	if (iBitDepth == 2)
	{
		png_set_user_transform_info(png_ptr, NULL, 4, 1);
		png_set_read_user_transform_fn(png_ptr, ConvertTo4BitsPerPix);
	}

	// Tell libpng to strip 16 bit/color files down to 8 bits/color
	if (iBitDepth == 16) 
		png_set_strip_16(png_ptr);

	if (iColorType == PNG_COLOR_TYPE_RGB || iColorType == PNG_COLOR_TYPE_RGB_ALPHA)
		png_set_bgr(png_ptr);

	png_read_update_info(png_ptr, info_ptr);

	// If palette, get it and fill our image struct
	if (Image.dwPaletteSize > 0)
	{
		if (iColorType == PNG_COLOR_TYPE_PALETTE)
		{
			png_colorp palette;
			int iPaletteSize = 0;
			png_get_PLTE(png_ptr, info_ptr, &palette, &iPaletteSize);
			if (iPaletteSize > (int)Image.dwPaletteSize) 
				iPaletteSize = Image.dwPaletteSize;

			memset(Image.pPalette, 0, Image.dwPaletteSize * sizeof(png_color));
			memcpy(Image.pPalette, palette, iPaletteSize * sizeof(png_color));
		}
		else
		{
			int depth = (iBitDepth == 16)? 8 : iBitDepth ;
			png_build_grayscale_palette(depth, Image.pPalette);
		}
	}

	// Let's find the transparent color if there is one 
	COLORREF TransparentColor = CLR_NONE;
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
	{
		png_bytep trans = 0;
		int num_trans = 0;
		png_color_16p rgb;
		png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans, &rgb);

		if (iColorType == PNG_COLOR_TYPE_PALETTE)
		{
 			png_colorp palette;
			int iPaletteSize = 0;
			png_get_PLTE(png_ptr, info_ptr, &palette, &iPaletteSize);
			for (int i = 0; i < num_trans; ++i) 
			{
				if ((trans[i] == 0)) 
				{
					TransparentColor = RGB(palette[i].red, palette[i].green, palette[i].blue);
					break;
				}
			}
		}
		else
		{
			if (iBitDepth == 16) 
				TransparentColor = RGB(rgb->red >> 8, rgb->green >> 8, rgb->blue >> 8);
			else 
				TransparentColor = RGB(rgb->red, rgb->green, rgb->blue);
		}
	}

	png_read_image(png_ptr, Image.ppRow);

	png_infop end_info = NULL;
	png_read_end(png_ptr, end_info);

	// Allocate the buffer to be big enough to hold all the bits
	DWORD dwLen = (DWORD)sizeof(BITMAPINFOHEADER) + (Image.dwPaletteSize * sizeof(RGBQUAD)) + Image.dwImgBytes;

	// Allocate enough memory to hold the entire DIB
	HGLOBAL hMemory = ::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, dwLen);
	if (!hMemory)
		return NULL;

	//Begin filling out DIB info
	BITMAPINFOHEADER* pDib = (BITMAPINFOHEADER*)::GlobalLock(hMemory);
	if (!pDib)
	{
		::GlobalFree(hMemory);
		return NULL;
	}

	::ZeroMemory(pDib, dwLen);
	pDib->biSize			= sizeof(BITMAPINFOHEADER);
	pDib->biWidth			= Image.dwWidth; 
	pDib->biHeight			= Image.dwHeight; 
	pDib->biPlanes			= 1;
	pDib->biCompression		= BI_RGB;
	pDib->biXPelsPerMeter	= Image.dwResolution;
	pDib->biYPelsPerMeter	= Image.dwResolution;
	pDib->biSizeImage		= Image.dwImgBytes;
	pDib->biBitCount		= Image.wBitDepth; 
	pDib->biClrUsed			= Image.dwPaletteSize;
	pDib->biClrImportant	= (TransparentColor == CLR_NONE ? 0 : TransparentColor | 0xFF000000);

	if (pDib->biBitCount >= 16)
		pDib->biBitCount = 24;	

	if (pDib->biClrUsed)
	{
		RGBQUAD* pRgb = DibColors(pDib);
		BYTE rgbq[RGBQUAD_SIZE];
		PALETTE* pal;
		UINT nIndex;
		memset(rgbq, 0, sizeof(rgbq));
		for (pal = Image.pPalette, nIndex = Image.dwPaletteSize ; nIndex > 0 ; nIndex--, pal++)
		{
			rgbq[RGBQ_RED]			= pal->red;
			rgbq[RGBQ_GREEN]		= pal->green;
			rgbq[RGBQ_BLUE]			= pal->blue;
			rgbq[RGBQ_RESERVED]		= 0;
			memcpy(pRgb, rgbq, sizeof(RGBQUAD));
			pRgb++;
		}
	}

	memcpy(DibPtr(pDib), Image.pBits, DibSizeImage(pDib));

	if (TransparentColor == CLR_NONE && (bDefaultToUpperLeft || bHasAlpha))
	{
		// Default to the UpperLeft pixel
		BYTE* pPixel = DibPtrXYExact(pDib, 0, 0);
		if (DibBitCount(pDib) == 8)
		{
			BYTE i = *pPixel++;
			RGBQUAD* pColors = DibColors(pDib);
			if (pColors)
			{
				BYTE R = pColors[i].rgbRed;
				BYTE G = pColors[i].rgbGreen;
				BYTE B = pColors[i].rgbBlue;
				TransparentColor = RGB(R,G,B);
				pDib->biClrImportant = TransparentColor | 0xFF000000;
			}
		}
		else
		if (DibBitCount(pDib) == 24)
		{
			BYTE B = *pPixel++;
			BYTE G = *pPixel++;
			BYTE R = *pPixel++;
			TransparentColor = RGB(R,G,B);
			pDib->biClrImportant = TransparentColor | 0xFF000000;
		}
	}

	FreeImageStruct(Image);

	::GlobalUnlock(hMemory);
	return hMemory;
}

/////////////////////////////////////////////////////////////////////////////
void CPng::FreeImageStruct(const IMAGE& Image)
{
	if (Image.pPalette)
	{
		::GlobalUnlock(Image.pPalette);
		::GlobalFree(Image.pPalette);
	}

	if (Image.ppRow)
	{
		::GlobalUnlock(Image.ppRow);
		::GlobalFree(Image.ppRow);
	}

	if (Image.pBits)
	{
		::GlobalUnlock(Image.pBits);
		::GlobalFree(Image.pBits);
	}
}

/////////////////////////////////////////////////////////////////////////////
bool CPng::AllocImageStruct(IMAGE& Image)
{
	// Use this image struct instead off going direct to DIB in case future need to from PNG to ???
	Image.pPalette = NULL;
	Image.ppRow = NULL;
	Image.pBits = NULL;

	// Allocate palette 
	if (Image.dwPaletteSize <= 0)
		Image.pPalette = NULL;
	else
	{
		HGLOBAL hPalette = ::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, (size_t)Image.dwPaletteSize * sizeof(PALETTE));
		if (!hPalette)
			return FALSE;

		Image.pPalette = (png_color_struct*)::GlobalLock(hPalette);
		if (!Image.pPalette)
		{
			FreeImageStruct(Image);
			return FALSE;
		}
	}

	// Round off row bytes
	Image.dwRowBytes = (Image.dwWidth * Image.wBitDepth + 31) / 32 * 4;
	Image.dwImgBytes = Image.dwRowBytes * Image.dwHeight;
	HGLOBAL hRowPtr = ::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,  (size_t)Image.dwHeight * sizeof(BYTE*));
	if (!hRowPtr)
		return FALSE;

	Image.ppRow = (UCHAR**)::GlobalLock(hRowPtr);
	if (!Image.ppRow)
	{
		FreeImageStruct(Image);
		return FALSE;
	}

	//Allocate space for the image data
	HGLOBAL hpBits = ::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, (size_t)Image.dwImgBytes);
	if (!hpBits)
		return FALSE;

	Image.pBits = (UCHAR*)::GlobalLock(hpBits);
	if (!Image.pBits)
	{
		FreeImageStruct(Image);
		return FALSE;
	}

	if (!Image.ppRow || !Image.pBits)
	{
		FreeImageStruct(Image);
		Image.pPalette = NULL;
		Image.ppRow = NULL;
		Image.pBits = NULL;
		return FALSE;
	}

	// Begin the assignment of the row pointers
	long lHeight  = Image.dwHeight;
	BYTE** ppRow = Image.ppRow;
	DWORD dwRowBytes = Image.dwRowBytes - sizeof(DWORD);

	BYTE* bBitPointer = Image.pBits + Image.dwImgBytes;
	while (--lHeight >= 0)
	{
		bBitPointer -= sizeof(DWORD);
		*((DWORD*)bBitPointer) = 0;
		bBitPointer -= dwRowBytes;
		*(ppRow++) = bBitPointer;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CPng::ConvertTo4BitsPerPix(png_structp png_ptr, png_row_infop row_info, png_bytep data)
{
	static const png_byte pix[] = {
		0x00,0x01,0x02,0x03, 0x10,0x11,0x12,0x13,
		0x20,0x21,0x22,0x23, 0x30,0x31,0x32,0x33,
	};

	png_uint_32 rowb = (row_info->width+1)/2;
	png_bytep q = data + rowb;
	png_bytep p = data + rowb/2;

	png_byte c;
	if (rowb%2==1)
	{
		c = *p;
		*(--q) = pix[c>>4];
	}
	while (p>data)
	{
		c = *(--p);
		*(--q) = pix[c&0x0f];
		*(--q) = pix[c>>4];
	}
	row_info->bit_depth   = 4;
	row_info->pixel_depth = 4;
	row_info->rowbytes    = rowb;
}
