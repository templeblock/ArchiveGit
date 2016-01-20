#include "stdafx.h"
#include "PngToDib.h"
#include "AGDib.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

/////////////////////////////////////////////////////////////////////////////
bool CPng::IsValidPNGFile(LPCSTR strFileName)
{
	FILE* fp = fopen(strFileName, "rb");
	if (!fp)
		return false;
			
	// Read in some of the signature bytes
	char buf[PNG_BYTES_TO_CHECK];
	DWORD dwBytesRead = fread(buf, 1, PNG_BYTES_TO_CHECK, fp);
	fclose(fp);

	if (dwBytesRead != PNG_BYTES_TO_CHECK)
		return false;

	// Compare the first PNG_BYTES_TO_CHECK bytes of the signature.
	return !png_sig_cmp((UCHAR*)buf, (png_size_t)0, PNG_BYTES_TO_CHECK);
}

/////////////////////////////////////////////////////////////////////////////
HGLOBAL CPng::ReadPNG(LPCSTR strFileName)
{
	FILE* fp = fopen(strFileName, "rb");
	if (!fp)
		return NULL;

	// Read in some of the signature bytes
	char buf[PNG_BYTES_TO_CHECK];
	DWORD dwBytesRead = fread(buf, 1, PNG_BYTES_TO_CHECK, fp);

	if (dwBytesRead != PNG_BYTES_TO_CHECK)
		return NULL;

	// Compare the first PNG_BYTES_TO_CHECK bytes of the signature.
	if (png_sig_cmp((UCHAR*)buf, (png_size_t)0, PNG_BYTES_TO_CHECK))
		return NULL;

	//Create and initialize the png_struct with the desired error handler functions.
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		fclose(fp);
		return NULL;
	}

	//Allocate/initialize the memory for image information.
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		fclose(fp);
		return NULL;
	}

	//Set error handling
	if (setjmp(png_jmpbuf(png_ptr)))
	{ // This is the exception handler
		//Free all of the memory associated with the png_ptr and info_ptr 
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(fp);
		return NULL;
	}

	png_init_io(png_ptr, fp);

	//We have already read some of the signature 
	png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);

	HGLOBAL hMemory = PNGToDIB(png_ptr, info_ptr);

	//clean up after the read, and free any memory allocated
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

	fclose(fp);

	return hMemory;
}

/////////////////////////////////////////////////////////////////////////////
HGLOBAL CPng::PNGToDIB(png_structp png_ptr, png_infop info_ptr)
{
	//Read the image
	png_read_info(png_ptr, info_ptr);

	int nBitDepth = 0;
	int nColorType = 0;
	png_uint_32 width = 0;
	png_uint_32 height = 0;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &nBitDepth, &nColorType, NULL, NULL, NULL);

	IMAGE img;
	img.width = (LONG)width;
	img.height = (LONG)height;

	//Determine the type of format
	if (nColorType==PNG_COLOR_TYPE_RGB || nColorType==PNG_COLOR_TYPE_RGB_ALPHA)
	{
		img.pixdepth = 24;		// nBitDepth 8, 16
		img.palnum   = 0;
	}
	else
	{
		switch(nBitDepth) // bit depths 1, 2, 4, 8, 16
		{	
			case 2:  
				img.pixdepth = 4;		
				break;
			case 16: 
				img.pixdepth = 8;		
				break;
			default: 
				img.pixdepth = nBitDepth;
		}

		img.palnum = 1 << img.pixdepth;
	}

	//Allocate the image struct that will be used to transport file info
	if (!AllocImageStruct(&img))
	{
		longjmp(png_ptr->jmpbuf, 1);
	}

	//Not sure if we can take advantage of this info in a BMP, thus remove it.
	if (nColorType & PNG_COLOR_MASK_ALPHA)
		png_set_strip_alpha(png_ptr);

	//Set user transform function to deal with a Bitdepth of 2
	if (nBitDepth == 2)
	{
		png_set_user_transform_info(png_ptr, NULL, 4, 1);
		png_set_read_user_transform_fn(png_ptr, ConvertTo4BitsPerPix);
	}

	//Tell libpng to strip 16 bit/color files down to 8 bits/color
	if (nBitDepth == 16) 
		png_set_strip_16(png_ptr);

	if (nColorType==PNG_COLOR_TYPE_RGB || nColorType==PNG_COLOR_TYPE_RGB_ALPHA)
	{
		png_set_bgr(png_ptr);
	}

	png_read_update_info(png_ptr, info_ptr);

	//If palette, get it and fill our img struct
	if (img.palnum > 0)
	{
		if (nColorType == PNG_COLOR_TYPE_PALETTE)
		{
			png_colorp palette;
			int num_palette;
			png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
			if (num_palette > (int)img.palnum) 
				num_palette = img.palnum;

			memset(img.palette, 0, img.palnum * sizeof(png_color));
			memcpy(img.palette, palette, num_palette * sizeof(png_color));
		}
		else
		{
			int depth = (nBitDepth == 16)? 8 : nBitDepth ;
			png_build_grayscale_palette(depth, img.palette);
		}
	}

	png_read_image(png_ptr, img.rowptr);

	png_infop end_info = NULL;
	png_read_end(png_ptr, end_info);

	// Alloc the buffer to be big enough to hold all the bits
	DWORD dwLen = (DWORD)sizeof(BITMAPINFOHEADER) + (img.palnum * sizeof(RGBQUAD)) + img.imgbytes;

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
	pDib->biWidth			= (DWORD)img.width; 
	pDib->biHeight			= (DWORD)img.height; 
	pDib->biPlanes			= 1;
	pDib->biCompression		= BI_RGB;
	pDib->biXPelsPerMeter	= 3937; // 100 pixels/inch
	pDib->biYPelsPerMeter	= 3937;
	pDib->biSizeImage		= img.imgbytes;
	pDib->biBitCount		= img.pixdepth; 
	pDib->biClrUsed			= img.palnum;

	if (pDib->biBitCount >= 16)
		pDib->biBitCount = 24;	

	if (pDib->biClrUsed)
	{
		RGBQUAD* pRgb = DibColors(pDib);
		BYTE rgbq[RGBQUAD_SIZE];
		PALETTE* pal;
		UINT nIndex;
		memset(rgbq, 0, sizeof(rgbq));
		for (pal = img.palette, nIndex = img.palnum ; nIndex > 0 ; nIndex--, pal++)
		{
			rgbq[RGBQ_RED]			= pal->red;
			rgbq[RGBQ_GREEN]		= pal->green;
			rgbq[RGBQ_BLUE]			= pal->blue;
			rgbq[RGBQ_RESERVED]		= 0;
			memcpy((LPVOID)pRgb, rgbq, sizeof(RGBQUAD));
			pRgb++;
		}
	}

	memcpy(DibPtr(pDib), img.bmpbits, DibSizeImage(pDib));

	FreeImageStruct(&img);

	::GlobalUnlock(hMemory);
	return hMemory;
}

/////////////////////////////////////////////////////////////////////////////
void CPng::FreeImageStruct(IMAGE* img)
{
	if (img->palette)
	{
		::GlobalUnlock(img->palette);
		::GlobalFree(img->palette);
	}
	if (img->rowptr)
	{
		::GlobalUnlock(img->rowptr);
		::GlobalFree(img->rowptr);
	}
	if (img->bmpbits)
	{
		::GlobalUnlock(img->bmpbits);
		::GlobalFree(img->bmpbits);
	}
}

/////////////////////////////////////////////////////////////////////////////
bool CPng::AllocImageStruct(IMAGE* img)
{
	//Use this image struct instead off going direct to DIB incase
	//future need to from PNG to ???
	BYTE *bBitPointer,**bRowPointer;
	DWORD dwRowBytes;
	LONG lHeight;
	HGLOBAL hpalette;

	img->palette = NULL;
	img->rowptr = NULL;
	img->bmpbits = NULL;

	//Allocate palette 
	if (img->palnum > 0)
	{
		hpalette = ::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, (size_t)img->palnum * sizeof(PALETTE));
		if (!hpalette)
			return FALSE;

		img->palette = (png_color_struct *)::GlobalLock(hpalette);
		if (!img->palette)
		{
			FreeImageStruct(img);
			return FALSE;
		}
	}
	else
	{
		img->palette = NULL;
	}

	//Round off row bytes
	img->rowbytes = ((DWORD)img->width * img->pixdepth + 31) / 32 * 4;
	img->imgbytes = img->rowbytes * img->height;
	HGLOBAL hRowPtr = ::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,  (size_t)img->height * sizeof(BYTE *));
	if (!hRowPtr)
		return FALSE;

	img->rowptr = (UCHAR**)::GlobalLock(hRowPtr);
	if (!img->rowptr)
	{
		FreeImageStruct(img);
		return FALSE;
	}

	//Allocate space for the image data
	HGLOBAL hBMPBits = ::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, (size_t)img->imgbytes);
	if (!hBMPBits)
		return FALSE;

	img->bmpbits = (UCHAR*)::GlobalLock(hBMPBits);
	if (!img->bmpbits)
	{
		FreeImageStruct(img);
		return FALSE;
	}

	if (!img->rowptr || !img->bmpbits)
	{
		FreeImageStruct(img);
		img->palette = NULL;
		img->rowptr = NULL;
		img->bmpbits = NULL;
		return FALSE;
	}

	//Begin the assignment of the row pointers
	lHeight  = img->height;
	bRowPointer = img->rowptr;
	dwRowBytes = img->rowbytes - sizeof(DWORD);

	bBitPointer = img->bmpbits + img->imgbytes;
	while(--lHeight >= 0)
	{
		bBitPointer -= sizeof(DWORD);
		*((DWORD *)bBitPointer) = 0;
		bBitPointer -= dwRowBytes;
		*(bRowPointer++) = bBitPointer;
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
	png_uint_32 rowb;
	png_bytep p,q;
	png_byte c;

	rowb = (row_info->width+1)/2;
	q = data + rowb;
	p = data + rowb/2;

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
