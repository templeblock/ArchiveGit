// PixmapDrawContext - this module creates the Pixmap draw context
// in this version is it creates an offscreen pixmap draw context with a Win32 GDI dib section attached

#include "Box3DSupport.h"



//----------------------------------------------------------------------------------

TQ3DrawContextObject NewPixmapDrawContext( DocumentPtr theDocument, TQ3PixelType pixelFormat )
{
	TQ3DrawContextData			myDrawContextData;
	TQ3PixmapDrawContextData	myPixmapDrawContextData;
	TQ3DrawContextObject		myDrawContext ;
	TQ3Pixmap					aPixmap;
 	BITMAPINFO					*bitmapInfo;
	HDC							hdc;
	size_t						bmiSize = sizeof (BITMAPINFOHEADER) + (3 * sizeof (DWORD));

	bitmapInfo = malloc (bmiSize);
	if (NULL == bitmapInfo)
		return NULL;

		//	fill in draw context data.
	myDrawContextData.clearImageMethod = kQ3ClearMethodWithColor;
		//	Set the background color.
	myDrawContextData.clearImageColor.a = 1.0F;
	myDrawContextData.clearImageColor.r = (float)theDocument->clearColorR/(float)255;
	myDrawContextData.clearImageColor.g = (float)theDocument->clearColorG/(float)255;
	myDrawContextData.clearImageColor.b = (float)theDocument->clearColorB/(float)255;

	myDrawContextData.paneState = kQ3False;
	myDrawContextData.maskState = kQ3False;
	myDrawContextData.doubleBufferState = kQ3False;

		// set up device independent bitmap
	hdc = GetDC(theDocument->fWindow);
	theDocument->fMemoryDC = CreateCompatibleDC(hdc);

	bitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);	
	bitmapInfo->bmiHeader.biWidth = theDocument->fWidth;	
	bitmapInfo->bmiHeader.biHeight = -(LONG)theDocument->fHeight;	 // negative means top to bottom
	bitmapInfo->bmiHeader.biPlanes = 1;
	aPixmap.pixelType = pixelFormat;
	switch ( pixelFormat )
	{
	case kQ3PixelTypeRGB32:
	case kQ3PixelTypeARGB32:
		bitmapInfo->bmiHeader.biBitCount = 32;
		bitmapInfo->bmiHeader.biCompression = BI_RGB;	
		aPixmap.pixelSize = 32;
		break;
	default:
	case kQ3PixelTypeRGB16:
	case kQ3PixelTypeARGB16:
		bitmapInfo->bmiHeader.biBitCount = 16;
		bitmapInfo->bmiHeader.biCompression = BI_RGB;	
		aPixmap.pixelSize = 16;
		break;
	case kQ3PixelTypeRGB16_565:
		{
			/* create masks for 565 format 16-bit DIB */
			DWORD *maskTable = (DWORD *) &(bitmapInfo->bmiColors[0]);
			bitmapInfo->bmiHeader.biBitCount = 16;
			bitmapInfo->bmiHeader.biCompression	= BI_BITFIELDS;	
			*maskTable++ = 0xF800;	/* red */
			*maskTable++ = 0x07E0;	/* green */
			*maskTable = 0x001F;	/* blue */
			aPixmap.pixelSize = 16;
		}
		break;
	case kQ3PixelTypeRGB24:
		bitmapInfo->bmiHeader.biBitCount = 24;
		bitmapInfo->bmiHeader.biCompression = BI_RGB;	
		aPixmap.pixelSize = 24;
		break;
	}
		
	/* init other BITMAPINFOHEADER fields */
	bitmapInfo->bmiHeader.biSizeImage		= 0;	
	bitmapInfo->bmiHeader.biXPelsPerMeter	= 0;	
	bitmapInfo->bmiHeader.biYPelsPerMeter	= 0;	
	bitmapInfo->bmiHeader.biClrUsed			= 0;	
	bitmapInfo->bmiHeader.biClrImportant	= 0;	

	theDocument->fBitmap = CreateDIBSection(hdc, bitmapInfo, DIB_RGB_COLORS,	
											&theDocument->fBitStorage, NULL, 0);	
	SelectObject(theDocument->fMemoryDC, theDocument->fBitmap);

	/* init other QD3D pixmap fields */
	aPixmap.width = theDocument->fWidth;
	aPixmap.height = theDocument->fHeight;
	aPixmap.image = theDocument->fBitStorage;
	aPixmap.bitOrder = kQ3EndianLittle;   
	aPixmap.byteOrder = kQ3EndianLittle;   
	aPixmap.rowBytes = (aPixmap.width * (aPixmap.pixelSize/8));
	aPixmap.rowBytes = (aPixmap.rowBytes + 3) & ~3L;  /* make it long aligned */

	/* set up the pixmapDrawContext */
	myPixmapDrawContextData.pixmap = aPixmap;
	myPixmapDrawContextData.drawContextData = myDrawContextData;

	/*	Create draw context and return it, if it's nil the caller must handle */
	myDrawContext = Q3PixmapDrawContext_New(&myPixmapDrawContextData) ;
	
	free( bitmapInfo );
	return myDrawContext ;
}

