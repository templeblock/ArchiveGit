#include "proto.h"
#include "tgardr.h"
#include "memory.h"

static void tgaReadHeader( HFILE pFile, LPTGAHDR hdr );
static BOOL tgaCheckPic( LPTGAHDR head );
static void tgaGetRawData( LPTGASTATE tgaState, HPTR lpDst, int iCount );
static void tgaGetScanline( LPTGASTATE tgaState, int indepth, BOOL graymap, HPTR lpBuffer, HPTR lpFrameDst );

//************************************************************************
PDIB ReadTarga(LPCSTR lpFileName)
//************************************************************************
{
	OFSTRUCT of;
	HFILE	theFile;
	if ( (theFile = OpenFile(lpFileName, &of, OF_READ)) < 0 )
	{
		Print( "Cannot open file: '%s'", lpFileName );
		return(FALSE);
	}

	/* Open the file header and see if it is a valid TARGA image */
	TGAHDR PicHdr;
	tgaReadHeader( theFile, &PicHdr );

	/* Check for an acceptable image type */
	if ( !tgaCheckPic( &PicHdr ) )
	{
		Print( "Cannot open file: '%s'\nUnsupported format", lpFileName );
		_lclose( theFile );
		return( NULL );
	}

	/* skip past image description */
	if ( PicHdr.textSize )
		_llseek(theFile,(long)PicHdr.textSize,SEEK_CUR);

	/* byte/pixel on the disk */
	int indepth = ( PicHdr.dataBits + 7 ) / 8;

	/* Set up the color map */
	/* remember whether or not the image is color mapped. */
	BOOL colorMapped;
	if (( PicHdr.dataType == 1 ) || ( PicHdr.dataType == 9 ))
		colorMapped = TRUE;
	else
		colorMapped = FALSE;

	/* Read color map if provided */
	BOOL graymap = FALSE;
	RGBS RGBmap[256];

	if (( PicHdr.mapType == 1 ) && ( PicHdr.mapLength > 0 ))
	{
		int mapBytes = ( PicHdr.CMapBits + 7 ) / 8;

		/* We either want to read the color map data, or skip over it. */
		if ( colorMapped )
		{
			// NumEntries = PicHdr.mapLength;

			/* initialize color map entries */
			for( int i = 0; i < 256; i++ )
			{
				RGBmap[i].red   =
				RGBmap[i].green =
				RGBmap[i].blue  = 0;
			}

			int j = PicHdr.mapOrig;
			for( i = PicHdr.mapLength; i; i--, j++ )
			{
				switch( mapBytes )
				{
					case 2:
					{
						WORD wTemp;
						_hread( theFile, (HPTR)&wTemp, 2 );
						RGBmap[j].red   = (BYTE)( wTemp >> 7 ) & 0xF8;
						RGBmap[j].green = (BYTE)( wTemp >> 2 ) & 0xF8;
						RGBmap[j].blue  = (BYTE)( wTemp << 3 ) & 0xF8;
						break;
					}

					case 3:
					{
						_hread( theFile, (HPTR)&RGBmap[j].blue,  1 );
						_hread( theFile, (HPTR)&RGBmap[j].green, 1 );
						_hread( theFile, (HPTR)&RGBmap[j].red,   1 );
						break;
					}

					case 4:
					{
						WORD wTemp;
						_hread( theFile, (HPTR)&RGBmap[j].blue,  1 );
						_hread( theFile, (HPTR)&RGBmap[j].green,  1 );
						_hread( theFile, (HPTR)&RGBmap[j].red,    1 );
						_hread( theFile, (HPTR)&wTemp, 1 );
						break;
					}

					default:
					{
						_lclose( theFile );
						return( NULL );
					}
				}

				if ( graymap )
				{
					graymap =
						( RGBmap[j].red == RGBmap[j].green ) &&
						( RGBmap[j].red == RGBmap[j].blue );
				}
			}
		}
		else
		{
			int mapLength = mapBytes * PicHdr.mapLength;

			if ( mapLength )
				_llseek(theFile,(long)mapLength,SEEK_CUR);
		}
	}
	else
	{
		colorMapped = FALSE;

		if ( indepth == 1 )
			graymap = TRUE;
	}

    int outdepth = ( (indepth == 2) ? 3 : indepth );

	BITMAPINFOHEADER bmi;
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biWidth = PicHdr.x;
	bmi.biHeight = PicHdr.y;
	bmi.biPlanes = 1;
	bmi.biBitCount = outdepth * 8;
	bmi.biCompression = BI_RGB;
	bmi.biSizeImage = 0;
	bmi.biXPelsPerMeter = 1000;
	bmi.biYPelsPerMeter = 1000;
	bmi.biClrUsed = PicHdr.mapLength;
	bmi.biClrImportant = 0;

	RGBQUAD rgbQuad[256];
	_fmemset(rgbQuad, 0, sizeof(rgbQuad));
	for ( int i = 0; i < (int)bmi.biClrUsed; ++i )
	{
		rgbQuad[i].rgbRed = RGBmap[i].red;
		rgbQuad[i].rgbGreen = RGBmap[i].green;
		rgbQuad[i].rgbBlue = RGBmap[i].blue;
	}
		
	PDIB pDib;
	pDib = new CDib(&bmi, rgbQuad, NULL);
	if (!pDib)
	{
		Print( "Out of memory" );
		_lclose( theFile );
		return( NULL );
	}
				
	HPTR lp = (HPTR)Alloc(pDib->GetSizeImage()+4000/*slop*/);
	if (!lp)
	{
		Print( "Out of memory" );
		_lclose( theFile );
		return( pDib );
	}

	pDib->SetPtr(lp);

	/* See if compressed file or not */
	TGASTATE tgaState;
	tgaState.bCompressed       = ( PicHdr.dataType & 8 ) ? 1 : 0;
	tgaState.lpRGBmap          = RGBmap;
	tgaState.pFile             = theFile;
	tgaState.PixelsPerScanline = PicHdr.x;
	tgaState.BytesPerScanline  = PicHdr.x * indepth;
	tgaState.cnt               = 0;

	HPTR lpBuffer;
	if (tgaState.bCompressed)
	{
		DWORD dwPos = _llseek( theFile, 0L, SEEK_CUR );
		DWORD dwLength = _llseek( theFile, 0L, SEEK_END );
		dwPos = _llseek( theFile, dwPos, SEEK_SET );
		DWORD dwSize = dwLength - dwPos;
		lpBuffer = (HPTR)Alloc(dwSize+4000/*slop*/);
		if (lpBuffer)
			_hread( theFile, lpBuffer, dwSize);
	}
	else
		lpBuffer = NULL;
	tgaState.lpBuffer = lpBuffer;

	/* allocate space for one line of the image */
	HPTR lpIn;
	if (!(lpIn = (HPTR)Alloc( ((long)PicHdr.x*(long)indepth))+4000/*slop*/))
	{
		Print( "Out of memory" );
		_lclose( theFile );
		return( pDib );
	}

	for( int row = 0; row < PicHdr.y; row++ )
	{
		int sy = ( PicHdr.imType & 0x20 ) ? row : ( PicHdr.y - row - 1 );

		HPTR lpOut = pDib->GetXY(0, sy);
    	tgaGetScanline( &tgaState, indepth, graymap, lpIn, lpOut );
	}

	if (lpBuffer)
		FreeUp(lpBuffer);
		
	_lclose( theFile );
	if ( lpIn )
		FreeUp( lpIn );

	return( pDib );
}

//************************************************************************
static void tgaReadHeader( HFILE pFile, LPTGAHDR hdr )
//************************************************************************
{
	_hread( pFile, &hdr->textSize, 1);
	_hread( pFile, &hdr->mapType,  1);
	_hread( pFile, &hdr->dataType, 1);

	_hread( pFile, &hdr->mapOrig, 2);
	_hread( pFile, &hdr->mapLength, 2);

	_hread( pFile, &hdr->CMapBits, 1);

	_hread( pFile, &hdr->XOffset, 2);
	_hread( pFile, &hdr->YOffset, 2);
	_hread( pFile, &hdr->x, 2);
	_hread( pFile, &hdr->y, 2);

	_hread( pFile, &hdr->dataBits, 1);
	_hread( pFile, &hdr->imType,   1);
}

//************************************************************************
static BOOL tgaCheckPic( LPTGAHDR head )
//************************************************************************
{
	//	A proper image file should indicate the presence
	//	or absence of a color map
	if ((head->mapType != 0) && (head->mapType != 1))
		return (FALSE);

	//	Acceptable formats for ICB/TARGA (RGB) Images
	//
    //		1 -- Color Mapped
    //		2 -- RGB
    //		3 -- Black and White
    //		9 -- Compresssed Color Mapped
   	//	   10 -- Compresssed RGB
   	//	   11 -- Compresssed B&W Mapped

	if ((head->dataType != 1) && (head->dataType != 2) &&
		(head->dataType != 3) && (head->dataType != 9) &&
		(head->dataType !=10) && (head->dataType != 11))
		return(FALSE);

	//	If it is a color mapped image--- verify that color map is
	//	present and that data is at 256 levels, also varify CMapBits
	if ((head->dataType == 1) || (head->dataType == 9))
	{
		if (head->mapType != 1)
			return (FALSE);
		if (head->dataBits != 8)
			return (FALSE);
		if ((head->CMapBits != 16) && (head->CMapBits != 24) &&
			(head->CMapBits !=32))
			return (FALSE);
	}

	//	Verify that the number of bits per pixel is 8, 16, 24 or 32
	if ((head->dataBits != 8) && (head->dataBits != 16) &&
		(head->dataBits !=24) && (head->dataBits != 32))
	{
		return (FALSE);
	}

	//	Verify that the image type (descriptor byte) is one we support
	//	(non-interleaved and either screen origin)
	BYTE imType = head->imType & 0xE0;

	if ((imType != 0) && (imType != 0x20))
		return(FALSE);

	return (TRUE);
}


//************************************************************************
static void tgaGetScanline( LPTGASTATE tgaState, int indepth, BOOL graymap, HPTR lpIn, HPTR lpOut )
//************************************************************************
{
    HPTR lpSrc = lpIn;
	int iCount = tgaState->PixelsPerScanline;

	switch( indepth )
	{
		case 1 :
            if ( graymap && tgaState->lpRGBmap )
            {
				tgaGetRawData( tgaState, lpSrc, indepth );

				// Map MUST be gray 
				while( --iCount >= 0 )
					*lpOut++ = tgaState->lpRGBmap[ *lpSrc++ ].red;
            }
            else
				tgaGetRawData( tgaState, lpOut, indepth );
		break;

		case 2 :
			tgaGetRawData( tgaState, lpSrc, indepth );

			while( --iCount >= 0 )
			{
				UINT uiValue = ( lpSrc[1]<<8 ) | ( lpSrc[0] );

				*lpOut++ = ( uiValue >> 7 ) & 0xF8;
				*lpOut++ = ( uiValue >> 2 ) & 0xF8;
				*lpOut++ = ( uiValue << 3 ) & 0xF8;
				lpSrc += indepth;
			}
		break;

		case 3 :
			tgaGetRawData( tgaState, lpOut, indepth );
		break;

		case 4 :
			tgaGetRawData( tgaState, lpSrc, indepth );

		    while( --iCount >= 0 )
		    {
			    *lpOut++ = lpSrc[2];
			    *lpOut++ = lpSrc[1];
			    *lpOut++ = lpSrc[0];
			    lpSrc += indepth;
		    }
		break;
	}
}


//************************************************************************
static void tgaGetRawData(LPTGASTATE tgaState, HPTR lpDst, int iDepth)
//************************************************************************
{
	if (tgaState->bCompressed)
	{
		if (!tgaState->lpBuffer)
			return;

		int iCount = tgaState->PixelsPerScanline;
		while(iCount > 0)
		{	
			// if count is zero, we must start a new packet
			if (tgaState->cnt == 0)
			{
				unsigned char byte;

				// get packet header
				byte = *tgaState->lpBuffer++;

				// set count for packet
				tgaState->cnt = (0x7F & byte) + 1;

				// set type of packet
				tgaState->rawPacket = ((0x80 & byte) == 0);

				if (!tgaState->rawPacket)
				{
					hmemcpy(tgaState->DataBuf, tgaState->lpBuffer, iDepth);
					tgaState->lpBuffer += iDepth;
				}
			}

			int nPixels = tgaState->cnt;
			if (nPixels > iCount)
				nPixels = iCount;
			iCount -= nPixels;
			tgaState->cnt -= nPixels;
			if (tgaState->rawPacket)
			{
				int nBytes = nPixels * iDepth;
				hmemcpy(lpDst, tgaState->lpBuffer, nBytes);
				tgaState->lpBuffer += nBytes;
				lpDst += nBytes;
			}
			else
			{
				while (--nPixels >= 0)
				{
					hmemcpy(lpDst, tgaState->DataBuf, iDepth);
					lpDst += iDepth;
				}
			}
		}
	}
	else
		_hread( tgaState->pFile, lpDst, tgaState->BytesPerScanline );
}


